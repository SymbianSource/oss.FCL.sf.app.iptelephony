/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Transfer controller class, transfer is controlled through 
*                this class
*
*/

#include <mceinrefer.h>                     // CMceInRefer
#include <mceinevent.h>                     // CMceEvent, CMceInEvent
#include <mcetransactiondatacontainer.h>    // TMceTransactionDataContainer

#include "svptransfercontroller.h"
#include "svptransferstatecontext.h"
#include "svplogger.h"
#include "svpsessionbase.h"             // CSVPSessionBase
#include "svpsipconsts.h"
#include "svpconsts.h"

// ---------------------------------------------------------------------------
// CSVPTransferController::CSVPTransferController
// ---------------------------------------------------------------------------
//
CSVPTransferController::CSVPTransferController() :
    iTransferContext (NULL),
    iCCPTransferObserver (NULL),
    iAccepted( EFalse )
    {
    }

// ---------------------------------------------------------------------------
// CSVPTransferController::ConstructL
// ---------------------------------------------------------------------------
//
void CSVPTransferController::ConstructL( 
                                CMceSession* aMceSession,
                                CSVPSessionBase* aSVPSession,
                                TMceTransactionDataContainer& aContainer, 
                                MSVPTransferObserver& aObserver )
    {
    // Transfer state context
    iTransferContext = CSVPTransferStateContext::NewL( 
                                aMceSession, aSVPSession, 
                                aContainer, aObserver );
    }
    

// ---------------------------------------------------------------------------
// CSVPTransferController::NewL
// ---------------------------------------------------------------------------
//
CSVPTransferController* CSVPTransferController::NewL( 
                            CMceSession* aMceSession,
                            CSVPSessionBase* aSVPSession,
                            TMceTransactionDataContainer& aContainer, 
                            MSVPTransferObserver& aObserver )
    {
    CSVPTransferController* self = CSVPTransferController::NewLC( 
                                        aMceSession, 
                                        aSVPSession,
                                        aContainer,
                                        aObserver );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CSVPTransferController::NewLC
// ---------------------------------------------------------------------------
//
CSVPTransferController* CSVPTransferController::NewLC( 
                            CMceSession* aMceSession,
                            CSVPSessionBase* aSVPSession,
                            TMceTransactionDataContainer& aContainer, 
                            MSVPTransferObserver& aObserver )
    {
    CSVPTransferController* self = new( ELeave ) CSVPTransferController;
    CleanupStack::PushL( self );
    self->ConstructL( aMceSession, aSVPSession, aContainer, aObserver );
    return self;
    }
    

// ---------------------------------------------------------------------------
// CSVPTransferController::~CSVPTransferController
// ---------------------------------------------------------------------------
//
CSVPTransferController::~CSVPTransferController()
    {
    delete iTransferContext;
    }
    

// ---------------------------------------------------------------------------
// Handle mce event observer notify events. 
// The state of the event has changed.
// ---------------------------------------------------------------------------
//
void CSVPTransferController::HandleEventStateChangedL( 
                            CMceEvent& /* aEvent */,
                            TInt aStatusCode )
    {
    if ( KSVPOKVal == aStatusCode )
        {
        if ( iTransferContext->IsAttended() )
            {
            SVPDEBUG1( "CSVPTransferController::HandleEventStateChangedL: KSVPOKVal & attended" );
            if ( KSVPTransferAcceptedStateIndex == iTransferContext->CurrentState())
                {
                // F25 OK received - already acccepted 
                // leave for not to create new session later
                SVPDEBUG1( "CSVPTransferController::HandleEventStateChangedL: transfer in progress");
                User::Leave( KSVPErrTransferInProgress );
                }

            // F18 OK received as response for "trying"
            // Set and apply the next state - accepted
            iTransferContext->SetCurrentStateL( KSVPTransferAcceptedStateIndex );
            iTransferContext->ApplyCurrentStateL();
            SVPDEBUG1( "CSVPTransferController::HandleEventStateChangedL:ApplyCurrentStateL done" );
            }
        else    // unattended transfer
            {
            SVPDEBUG1( "CSVPTransferController::HandleEventStateChangedL: KSVPOKVal unattended" );
            if ( iTransferContext->CheckIsSessionRemoteHold() &&
                 KSVPTransferAcceptedStateIndex == iTransferContext->CurrentState() )
                {
                // Transferer has put transferee on hold before sending refer
                SVPDEBUG1( "CSVPTransferController::HandleEventStateChangedL: Snom or similar as transferer" );
                SendNotifyL( aStatusCode ); // 200 OK
                }
            }
        }
    else
        {
        SVPDEBUG2("CSVPTransferController::HandleEventStateChangedL: aStatusCode = %d",
              aStatusCode);
        }
    SVPDEBUG1("CSVPTransferController::HandleEventStateChangedL: Out");    
    }

// ---------------------------------------------------------------------------
// CSVPTransferController::NotifyReceivedL
// ---------------------------------------------------------------------------
//
void CSVPTransferController::NotifyReceivedL( 
                            CMceEvent& aEvent,
                            TMceTransactionDataContainer* aContainer )
    {
    SVPDEBUG1("CSVPTransferController::NotifyReceivedL: In");    
    
    // check data container and match the event to the active refer event
    if ( aContainer && iTransferContext->MceEvent() == &aEvent )
        {                  
        TInt statusCode = aContainer->GetStatusCode();        
        HBufC8* content = aContainer->GetContent();
        
        SVPDEBUG2( "CSVPTransferController::NotifyReceivedL:\
            statusCode: %d", statusCode );
        SVPDEBUG2( "CSVPTransferController::NotifyReceivedL:\
            aContainer->GetContent()->Length(): %d", content->Length() );

        if ( !content->Find( TPtrC8( KSVPNotifyTrying ) ) )
            {
            if ( iTransferContext->IsAttended() )
                {                
                SVPDEBUG1( "CSVPTransferController::NotifyReceivedL, Attended case:\
                    SIP/2.0 100 Trying" );

                // Attended transfer (F17) NOTIFY , transferee is trying establish new session.
                // Check if 202 Accepted already received
                if ( iAccepted )
                    {
                    // Stop the refer timer
                    iTransferContext->StopReferTimer( );
                    // Set and apply the next state - accepted
                    iTransferContext->SetCurrentStateL( 
                                    KSVPTransferAcceptedStateIndex );
                    iTransferContext->ApplyCurrentStateL();
                    }
                }
            else
                {
                SVPDEBUG1( "CSVPTransferController::NotifyReceivedL, UnAttended case:\
                    SIP/2.0 100 Trying" );

                // Check if 202 Accepted already received
                if ( iAccepted )
                    { 
                    // Stop the refer timer
                    iTransferContext->StopReferTimer( );

                    SVPDEBUG1( "CSVPTransferController::NotifyReceivedL, Unattended: is accepted" );
                        
                    // Unattended transfer: F7 NOTIFY, display "transferred" note
                    if( iCCPTransferObserver )
                        {
                        SVPDEBUG1( "CSVPTransferController::NotifyReceivedL, send local transfer" );
                        iCCPTransferObserver->TransferEventOccurred( 
                                MCCPTransferObserver::ECCPLocalTransfer );
                        }                

                    // Set and apply the next state - accepted
                    iTransferContext->SetCurrentStateL( 
                            KSVPTransferAcceptedStateIndex );
                    iTransferContext->ApplyCurrentStateL();                
                
                    iTransferContext->SetCurrentStateL( 
                                            KSVPTransferTerminatingStateIndex );
                    iTransferContext->ApplyCurrentStateL();
                    // Unattended transfer is ok, hangup the session
                    iTransferContext->TransferObserver().TransferNotification( 
                                                          ESVPTransferOKHangUp );
                    }
                }
            }
        else if ( !content->Find( TPtrC8( KSVPNotifyOK ) ) || 
                  !content->Find( TPtrC8( KSVPNotifyOk2 ) ) )
            {
            if ( iTransferContext->IsAttended() )
                {
                SVPDEBUG1( "CSVPTransferController::NotifyReceivedL, Attended case:\
                    SIP/2.0 200 OK" );

                // Attended transfer (F24) 200 OK
                if( iCCPTransferObserver )
                    {
                    SVPDEBUG1( "CSVPTransferController::NotifyReceivedL, send local transfer" );
                    iCCPTransferObserver->TransferEventOccurred( 
                                MCCPTransferObserver::ECCPLocalTransfer );  
                    }

                // Set and apply the next state - terminating
                iTransferContext->SetCurrentStateL( 
                                        KSVPTransferTerminatingStateIndex );
                iTransferContext->ApplyCurrentStateL();

                // Attended transfer is ok, hangup the session
                iTransferContext->TransferObserver().TransferNotification(
                                                      ESVPTransferOKHangUp );
                }
            else
                {
                SVPDEBUG1( "CSVPTransferController::NotifyReceivedL, UnAttended: SIP/2.0 200 OK" );                    
                }
            }
        else if ( ( !content->Find( TPtrC8( KSVPNotifyRinging ) ) || 
                    !content->Find( TPtrC8( KSVPNotifyRinging183 ) ) ) &&
                    !iTransferContext->IsAttended())
            {
            // Polycom send Ringing instead of Trying in unattended case.
            SVPDEBUG1( "CSVPTransferController::NotifyReceivedL, UnAttended and Ringing" );

            // Check if 202 Accepted already received
            if ( iAccepted )
                { 
                // Stop the refer timer
                iTransferContext->StopReferTimer( );

                SVPDEBUG1( "CSVPTransferController::NotifyReceivedL, Unattended: Polycom case" );
                    
                // Unattended transfer: display "transferred" note
                if( iCCPTransferObserver )
                    {
                    SVPDEBUG1( "CSVPTransferController::NotifyReceivedL, send local transfer" );
                    iCCPTransferObserver->TransferEventOccurred( 
                            MCCPTransferObserver::ECCPLocalTransfer );
                    }                

                // Set and apply the next state - accepted
                iTransferContext->SetCurrentStateL( 
                        KSVPTransferAcceptedStateIndex );
                iTransferContext->ApplyCurrentStateL();                
            
                iTransferContext->SetCurrentStateL( 
                                        KSVPTransferTerminatingStateIndex );
                iTransferContext->ApplyCurrentStateL();
                // Unattended transfer is ok, hangup the session
                iTransferContext->TransferObserver().TransferNotification( 
                                                      ESVPTransferOKHangUp );
                }
            }
        else if ( !content->Find( TPtrC8( KSVPNotifyServiceUnavailable ) ) )
            {
            // Notify that comes after the accepted refer, if
            // the (wrong) address of the refer cannot be reached.
            if ( KSVPTransferAcceptedStateIndex == iTransferContext->CurrentState() )
                {
                SVPDEBUG1( "CSVPTransferController::NotifyReceivedL, 503 -> ECCPTransferFailed");

                // Set and apply the next state - terminating
                iTransferContext->SetCurrentStateL( 
                                        KSVPTransferTerminatingStateIndex );
                iTransferContext->ApplyCurrentStateL();

                // Transfer fails, notify client.
                iTransferContext->TransferObserver().TransferNotification(
                                                      ESVPTransferDecline );
                }
            else
                {
                SVPDEBUG1( "CSVPTransferController::NotifyReceivedL, 503.");
                }
            }
        else
            {
            SVPDEBUG1("CSVPTransferController::NotifyReceivedL:\
                Unhandled container content");
            }
        
        // Ownership transferred here   
        delete content;
        }
        
    SVPDEBUG1("CSVPTransferController::NotifyReceivedL: Out");    
    }
    
    
// ---------------------------------------------------------------------------
// CSVPTransferController::HandleReferStateChangeL
// ---------------------------------------------------------------------------
//
void CSVPTransferController::HandleReferStateChangeL( CMceRefer& aRefer,
				                                TInt aStatusCode )
    {
    SVPDEBUG1("CSVPTransferController::HandleReferStateChangeL() In");    

    if ( iTransferContext->MceRefer() == &aRefer )
        {                          
        if ( KSVPAcceptedVal == aStatusCode )
            {
            // Accepted unattended F6, Attended F16
            SVPDEBUG2( "CSVPTransferController::HandleReferStateChangeL: Accept: %i",
			             KSVPAcceptedVal );

            // Display "transferring" note
            if( iCCPTransferObserver )
                {
                SVPDEBUG1( "CSVPTransferController::HandleReferStateChangeL, send remote transferring" );
                iCCPTransferObserver->TransferEventOccurred( 
                        MCCPTransferObserver::ECCPRemoteTransferring ); 
                }
            // Continue acceptance when also notify F7 / F17 "trying" received
            iAccepted = ETrue;
            }
        else if ( ( KSVPBadRequestVal <= aStatusCode && 
                    KSVPRequestPendingVal >= aStatusCode ) ||
                    KSVPDeclineVal == aStatusCode ||
                    KSVPServerInternalErrorVal == aStatusCode ||
                    KSVPPreconditionFailureVal == aStatusCode )
            {
            // Decline, Request Failure 4xx or Server Failure 5xx
            SVPDEBUG2( "CSVPTransferController::HandleReferStateChangeL: Code: %i", aStatusCode );
            // Stop the refer timer
            iTransferContext->StopReferTimer( );

            // Set and apply the next state - terminating
            iTransferContext->SetCurrentStateL( 
                                      KSVPTransferTerminatingStateIndex );
            iTransferContext->ApplyCurrentStateL();            

            // Notify the observer about the decline.
            iTransferContext->TransferObserver().TransferNotification( 
                                                    ESVPTransferDecline );
            }
            
        else
            {
            // Not handled
            SVPDEBUG2( "CSVPTransferController::HandleReferStateChangeL: \
                Unknown StatusCode: %i", aStatusCode );
            }
        }    
    else
        {
        // Unknown refer - not handled
        SVPDEBUG1( "CSVPTransferController::HandleReferStateChangeL: \
            Unknown refer");
        }    

    SVPDEBUG1("CSVPTransferController::HandleReferStateChangeL() Out");            
    }
        
            
// ---------------------------------------------------------------------------
// CSVPTransferController::IncomingReferL
// ---------------------------------------------------------------------------
//
void CSVPTransferController::IncomingReferL( CMceInRefer* aRefer,
        const TDesC8& aReferTo, TMceTransactionDataContainer* aContainer )
    {
    SVPDEBUG1( "CSVPTransferController::IncomingReferL In" )
    
    // Is new incoming refer handling possible
    if ( KSVPTransferIdleStateIndex == iTransferContext->CurrentState() )
        {
        SVPDEBUG1( "CSVPTransferController::IncomingReferL: allowed" )
        
        iTransferContext->SetMceRefer( static_cast<CMceRefer*>( aRefer ) );
        iTransferContext->SetIncomingReferToL( aReferTo );
        CDesC8Array* headers = aContainer->GetHeaders();// get headers
        
        if ( headers )
            {
            TBool found = EFalse;
            
            for( TInt i = 0; i < headers->MdcaCount() && !found; i++ )
                {
                TPtrC8 tmpHeader = headers->MdcaPoint( i );
                
                if ( KErrNotFound != tmpHeader.FindF( KSVPReferredBy ) )
                    {
                    SVPDEBUG1( "KSVPReferredBy found" )
                    found = ETrue;
                    iTransferContext->SetIncomingReferredByL( tmpHeader );
                    }
                }
            }
        
        delete headers;
        headers = NULL;
        }
    else
        {
        SVPDEBUG1( "CSVPTransferController::IncomingReferL: not allowed" )
        User::Leave( KSVPErrTransferStateError );
        }
    
    if ( iTransferContext->IsAttended() )
        {
        SVPDEBUG1( "CSVPTransferController::IncomingReferL: Attended case, send accept" )
        
        // send trying notification and wait respond for it
        CMceInEvent* inEvent = NULL;
        
        TRAPD( acceptError, inEvent = static_cast<CMceInRefer*>(
                iTransferContext->MceRefer() )->AcceptL() );
        
        if ( KErrNone == acceptError )
            {
            // Apply state, changes to next state (pending)
            SVPDEBUG1( "CSVPTransferController::IncomingReferL: pending state" )
            iTransferContext->SetMceEvent( static_cast<CMceEvent*>( inEvent ) );
            iTransferContext->ApplyCurrentStateL();
            }
        else
            {
            // Set and apply the next state - terminating
            SVPDEBUG2("CSVPTransferController::IncomingReferL: acc fails = %d", acceptError )
            iTransferContext->SetCurrentStateL( KSVPTransferTerminatingStateIndex );
            iTransferContext->ApplyCurrentStateL();
            }
        }
    else
        {
        // Apply state, changes to next state (pending)
        SVPDEBUG1( "CSVPTransferController::IncomingReferL: UnAttended case pending" )
        iTransferContext->ApplyCurrentStateL();
        }
    
    SVPDEBUG1( "CSVPTransferController::IncomingReferL Out" )
    }

// ---------------------------------------------------------------------------
// CSVPTransferController::IsMceRefer
// ---------------------------------------------------------------------------
//
TBool CSVPTransferController::IsMceRefer( CMceRefer& aRefer )
    {
    return ( iTransferContext->MceRefer() == &aRefer );
    }
    
// ---------------------------------------------------------------------------
// CSVPTransferController::IsAttended
// ---------------------------------------------------------------------------
//
TBool CSVPTransferController::IsAttended( )
    {
    return ( iTransferContext->IsAttended() );
    }

// ---------------------------------------------------------------------------
// CSVPTransferController::SetTransferDataL
// ---------------------------------------------------------------------------
//
void CSVPTransferController::SetTransferDataL( CDesC8Array* aUserAgentHeaders,
                                               TInt aSecureStatus )
    {
    SVPDEBUG1("  CSVPTransferController::SetTransferDataL" );
    iTransferContext->SetTransferDataL( aUserAgentHeaders, aSecureStatus );
    }

// ---------------------------------------------------------------------------
// CSVPTransferController::SetMceSessionObject
// ---------------------------------------------------------------------------
//
void CSVPTransferController::SetMceSessionObject( CMceSession* aSession )
    {
    iTransferContext->SetMceSessionObject( aSession );
    }

// ---------------------------------------------------------------------------
// CSVPTransferController::SendNotifyL
// ---------------------------------------------------------------------------
//
void CSVPTransferController::SendNotifyL( TInt aStatusCode )
    {
    SVPDEBUG2("CSVPTransferController::SendNotifyL() code = %d", aStatusCode);

    CMceInEvent* inEvent = static_cast< CMceInEvent* > (
                                            iTransferContext->MceEvent());   
    if (inEvent)
        {
        HBufC8* contentType = KSVPMessageSipfrag().AllocLC(); //message/sipfrag
        HBufC8* content = NULL; 
    
        if (KSVPOKVal == aStatusCode )
            {
            content = KSVPNotifyOK().AllocLC(); // "SIP/2.0 200 OK"
            }
        else if ( KSVPNotFoundVal == aStatusCode ||
                  KSVPBusyHereVal == aStatusCode ||
                  KSVPDeclineVal == aStatusCode )
            {
            content = KSVPNotifyServiceUnavailable().AllocLC(); // "503"
            }
        else
            {
            SVPDEBUG2("CSVPTransferController::SendNotifyL unknown aStatusCode = %d", aStatusCode);
            content = KSVPNotifyServiceUnavailable().AllocLC(); // "503"
            }

        CDesC8Array* headers = NULL;
        headers = new( ELeave ) CDesC8ArrayFlat( KSVPContactArrayGranularity );
        CleanupStack::PushL( headers );
        headers->AppendL( KSVPSubsStateTerminated );

        // Notify is sent to transferer (unattended  msg F15, attended msg F24)
        TRAPD( errNotify, inEvent->TerminateL( headers, contentType, content ) );

        if ( KErrNone == errNotify )
            {
            SVPDEBUG1("CSVPTransferController::SendNotifyL, notify sending OK");
            CleanupStack::Pop( 3, contentType );    // headers, content, contentType
            }
        else
            {
            // error handling
            SVPDEBUG2("CSVPTransferController::SendNotifyL: errNotify = %d", errNotify );
            CleanupStack::PopAndDestroy( 3, contentType );    // headers, content, contentType
            }
        }
        
    if ( iTransferContext->IsAttended() )
        {
        SVPDEBUG1("CSVPTransferController::SendNotifyL() Attended done");
        }
    else
        {
        SVPDEBUG1("CSVPTransferController::SendNotifyL() UnAttended to terminating state");
        // Finish unattended incoming transfer sequence
        // Set and apply the next state - terminating
        iTransferContext->SetCurrentStateL( KSVPTransferTerminatingStateIndex );
        iTransferContext->ApplyCurrentStateL();
        }

    SVPDEBUG1("CSVPTransferController::SendNotifyL() Out");
    }

// ---------------------------------------------------------------------------
// CSVPTransferController::IsIncomingTransfer
// ---------------------------------------------------------------------------
//
TBool CSVPTransferController::IsIncomingTransfer()
    {
    SVPDEBUG2("CSVPTransferController::IsIncomingTransfer = %d",
                iTransferContext->IsIncoming() );
    return iTransferContext->IsIncoming();
    }

// ---------------------------------------------------------------------------
// CSVPTransferController::TerminateTransfer
// ---------------------------------------------------------------------------
//
void CSVPTransferController::TerminateTransfer()
    {
    SVPDEBUG1("CSVPTransferController::TerminateTransfer" )
    TRAP_IGNORE( TerminateTransferL() );
    }

// ---------------------------------------------------------------------------
// CSVPTransferController::TerminateTransferL
// ---------------------------------------------------------------------------
//
void CSVPTransferController::TerminateTransferL()
    {
    iTransferContext->SetCurrentStateL( KSVPTransferTerminatingStateIndex );
    iTransferContext->ApplyCurrentStateL();
    }

// ---------------------------------------------------------------------------
// CSVPTransferController::AttendedTransfer
// ---------------------------------------------------------------------------
//
TInt CSVPTransferController::AttendedTransfer( MCCPCall& aTransferTargetCall )
    {  
    SVPDEBUG1( "CSVPTransferController::AttendedTransfer call IN" );
    TRAPD( transError, TransferL( &aTransferTargetCall, KNullDesC, ETrue ));
    SVPDEBUG2( "CSVPTransferController::AttendedTransfer A return: %d", transError );
    return transError;
    }

// ---------------------------------------------------------------------------
// CSVPTransferController::AttendedTransfer
// ---------------------------------------------------------------------------
//
TInt CSVPTransferController::AttendedTransfer( const TDesC& aTransferTarget )
    {  
    SVPDEBUG1( "CSVPTransferController::AttendedTransfer target IN" );
    TRAPD( transError, TransferL( NULL, aTransferTarget, ETrue ));
    SVPDEBUG2( "CSVPTransferController::AttendedTransfer B return: %d", transError );
    return transError;
    }

// ---------------------------------------------------------------------------
// CSVPTransferController::UnattendedTransfer
// ---------------------------------------------------------------------------
//
TInt CSVPTransferController::UnattendedTransfer( const TDesC& aTransferTarget )
    {  
    SVPDEBUG1( "CSVPTransferController::UnattendedTransfer IN" );
    TRAPD( transError, TransferL( NULL, aTransferTarget, EFalse ));
    SVPDEBUG2( "CSVPTransferController::UnattendedTransfer return: %d", transError );
    return transError;
    }

// ---------------------------------------------------------------------------
// CSVPTransferController::AcceptTransfer
// ---------------------------------------------------------------------------
//
TInt CSVPTransferController::AcceptTransfer( const TBool aAccept )
    {
    SVPDEBUG2("CSVPTransferController::AcceptTransfer() IN aAccept = \
        %d", aAccept);
    TInt acceptError = KErrNone;
    TInt stateError = KErrNone;
    TInt currentState = iTransferContext->CurrentState();
    CMceInEvent* inEvent(NULL);
    
    // Is state "pending"
    if ( KSVPTransferPendingStateIndex == currentState )
        {
        if ( aAccept )
            {
            TRAP( acceptError, ( inEvent = static_cast< CMceInRefer* > (iTransferContext->MceRefer())->AcceptL()));
            SVPDEBUG2("CSVPTransferController::AcceptTransfer()\
                 AcceptL = %d", acceptError);
            }
        else
            {
            SVPDEBUG1("CSVPTransferController::AcceptTransfer() reject");
            TRAP( acceptError, ( static_cast< CMceInRefer* > (iTransferContext->MceRefer())->RejectL()) );
            }
            
        if ( KErrNone == acceptError )
            {
            if ( aAccept )
                {
                // Set the received event
                iTransferContext->SetMceEvent(static_cast< CMceEvent* >(inEvent));
                
                // Set and apply the next state - accepted
                TRAP( acceptError,
                    iTransferContext->SetCurrentStateL( 
                                            KSVPTransferAcceptedStateIndex );
                    iTransferContext->ApplyCurrentStateL();
                    );
                }
            else
                {
                // Set and apply the next state - terminating
                TRAP( acceptError,
                    iTransferContext->SetCurrentStateL( 
                                            KSVPTransferTerminatingStateIndex );
                    iTransferContext->ApplyCurrentStateL();        
                    );
                }
            }
        else
            {
            SVPDEBUG2("CSVPTransferController::AcceptTransfer()\
                 fails = %d", acceptError);
            // Set and apply the next state - terminating
            TRAP( stateError,
                iTransferContext->SetCurrentStateL( 
                                        KSVPTransferTerminatingStateIndex );
                iTransferContext->ApplyCurrentStateL();
                );
            SVPDEBUG2("CSVPTransferController::AcceptTransfer()\
                 stateError = %d", stateError);
            }
        }
    else
        {
        SVPDEBUG2("CSVPTransferController::AcceptTransfer()\
             current state is not pending: %d", currentState);       
        acceptError = KSVPErrTransferStateError;
        }

    SVPDEBUG2("CSVPTransferController::AcceptTransfer()\
         OUT acceptError = %d", acceptError);    
    return acceptError;
    }

// ---------------------------------------------------------------------------
// CSVPTransferController::TransferTarget
// ---------------------------------------------------------------------------
//
const TDesC& CSVPTransferController::TransferTarget() const
    {   
    return iTransferContext->IncomingReferTo();
    }

// ---------------------------------------------------------------------------
// CSVPTransferController::AddObserverL
// ---------------------------------------------------------------------------
//    
void CSVPTransferController::AddObserverL( 
                                    const MCCPTransferObserver& aObserver )
    {
    SVPDEBUG1("CSVPTransferController::AddObserverL() In");
    // set transfer observer
    // only one observer used at a time, replaces current one
    iCCPTransferObserver = const_cast<MCCPTransferObserver*>(&aObserver);

    SVPDEBUG1("CSVPTransferController::AddObserverL() Out");
    }

// ---------------------------------------------------------------------------
// CSVPTransferController::RemoveObserver
// ---------------------------------------------------------------------------
//
TInt CSVPTransferController::RemoveObserver( 
                                    const MCCPTransferObserver& aObserver )
    {
    SVPDEBUG1("CSVPTransferController::RemoveObserver");
    TInt err = KErrNone;
    if ( iCCPTransferObserver == const_cast<MCCPTransferObserver*>
                (&aObserver) )
        {
        iCCPTransferObserver = NULL;
        }
    else
        {
        err = KErrNotFound;
        }
    return err;
    }

// ---------------------------------------------------------------------------
// CSVPTransferController::TransferL
// ---------------------------------------------------------------------------
//
void CSVPTransferController::TransferL( MCCPCall* aCall, 
                                        const TDesC& aTarget,
                                        const TBool aAttendedTransfer )
    {
    SVPDEBUG1("CSVPTransferController::TransferL() In");

    // Transfer possible.
    if ( KSVPTransferIdleStateIndex == iTransferContext->CurrentState() )
        {
        iAccepted = EFalse;
        
        // Set transfer parameters
        iTransferContext->SetTransferParmsL( 
                            static_cast< CSVPSessionBase* >(aCall),
                            aTarget,
                            aAttendedTransfer );

        // Apply state, execute the refer and change to next state (pending).
        iTransferContext->ApplyCurrentStateL();
        }
    else
        {
        SVPDEBUG1( "CSVPTransferController::TransferL: Error - transfer in progress" );
        iTransferContext->TransferObserver().TransferFailed( KErrInUse );
        TerminateTransferL();
        }

    SVPDEBUG1("CSVPTransferController::TransferL() OUT");
    }


