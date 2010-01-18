/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Hold controller, interface class to SVP sessions
*
*/
 

#include    <mcesession.h>
#include    <mcemediastream.h>
#include    <mceaudiostream.h>

#include    "svpholdcontroller.h"
#include    "svpholdmediahandler.h"
#include    "svpholdobserver.h"
#include    "svpsessionbase.h"
#include    "svpsipconsts.h"
#include    "svpconsts.h"
#include    "svplogger.h"


// ---------------------------------------------------------------------------
// CSVPHoldController::CSVPHoldController
// ---------------------------------------------------------------------------
//
CSVPHoldController::CSVPHoldController() :
        iContext( NULL ),
        iPreviousHoldState( ESVPHoldConnected ),
        iHoldRequestCompleted( EFalse ),
        iHoldRequest( ESVPHoldNoRequest ),
        iReinviteCrossover( EFalse )
    {
    }

// ---------------------------------------------------------------------------
// CSVPHoldController::ConstructL
// ---------------------------------------------------------------------------
//
void CSVPHoldController::ConstructL( 
                            CMceSession& aSession,
                            TMceTransactionDataContainer& aContainer,
                            MSVPHoldObserver* aObserver,
                            TBool aIsMobileOriginated )
    {
    iContext = CSVPHoldContext::NewL( aSession, aContainer, aObserver,
                                      aIsMobileOriginated );
    }

// -----------------------------------------------------------------------------
// CSVPHoldController::NewL
// -----------------------------------------------------------------------------
//
CSVPHoldController* CSVPHoldController::NewL( 
                            CMceSession& aSession,
                            TMceTransactionDataContainer& aContainer,
                            MSVPHoldObserver* aObserver,
                            TBool aIsMobileOriginated )
    {    
    CSVPHoldController* self = new ( ELeave ) CSVPHoldController();
    
    CleanupStack::PushL( self );
    self->ConstructL( aSession, aContainer, aObserver,
                      aIsMobileOriginated );
                      
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------------------------
// CSVPHoldController::NewLC
// ---------------------------------------------------------------------------
//
CSVPHoldController* CSVPHoldController::NewLC( 
                            CMceSession& aSession,
                            TMceTransactionDataContainer& aContainer,
                            MSVPHoldObserver* aObserver,
                            TBool aIsMobileOriginated )
    {
    CSVPHoldController* self = new ( ELeave ) CSVPHoldController();
    CleanupStack::PushL( self );
    self->ConstructL( aSession, aContainer, aObserver,
                      aIsMobileOriginated );

    return self;
    }


// ---------------------------------------------------------------------------
// CSVPHoldController::~CSVPHoldController
// ---------------------------------------------------------------------------
//
CSVPHoldController::~CSVPHoldController()
    {
    delete iContext;
    SVPDEBUG1( "CSVPHoldController::~CSVPHoldController Done" );
    }

// ---------------------------------------------------------------------------
// CSVPHoldController::RetryHoldRequest
// ---------------------------------------------------------------------------
//
TInt CSVPHoldController::RetryHoldRequest( CMceSession* aSession )
    {
    SVPDEBUG1( "CSVPHoldController::RetryHoldRequest -- IN" );
    TInt err = KErrNone;
    switch ( iHoldRequest )
        {
        case ESVPHoldToHold:
            {
            SVPDEBUG1( "CSVPHoldController::RetryHoldRequest -- Hold" );
            err = HoldSession( aSession );
            break;
            }
            
        case ESVPHoldToResume:
            {
            SVPDEBUG1( "CSVPHoldController::RetryHoldRequest -- Resume" );
            err = ResumeSession( aSession );
            break;
            }
        
        default:
            {
            SVPDEBUG1( "CSVPHoldController::RetryHoldRequest -- No request!!" );
            err = KErrNotFound;
            break;
            }
        }
    
    iHoldRequest = ESVPHoldNoRequest;
    SVPDEBUG1( "CSVPHoldController::RetryHoldRequest -- OUT" );
    return err;
    }

// ---------------------------------------------------------------------------
// CSVPHoldController::HoldSession
// ---------------------------------------------------------------------------
//
TInt CSVPHoldController::HoldSession( CMceSession* aSession )
    {
    SVPDEBUG1( "CSVPHoldController::HoldSession -- IN" );

    if ( HoldAllowed() )
        {
        iHoldRequest = ESVPHoldToHold;
        iHoldRequestCompleted = EFalse;
        SVPDEBUG1( "CSVPHoldController::HoldSession -- Allowed" );
        TInt err = KErrNone;
        iContext->SetSessionObject( aSession );

        TRAP( err, iContext->ApplyCurrentStateL( aSession,
                                                 ESVPHoldToHold ) );
        
        SVPDEBUG2(
        "CSVPHoldController::HoldSession -- ApplyCurrentState done, err = %i",
         err );
         
        switch ( err )
            {                
            case KErrSVPHoldLocalOldwayholdNeeded:
                {
                SVPDEBUG1(
                "CSVPHoldController::HoldSession: Oldway hold needed" );
                
                iContext->SetFirstAttempt( EFalse ); // Clearing the flag                
                TRAP( err, iContext->ApplyCurrentStateL( aSession,
                                                         ESVPHoldToHold ) );
                return err;
                }
                
            default:
                {
                SVPDEBUG2(
                "CSVPHoldController::HoldSession - default, err: %i", err );
                return err;
                }
            }        
        }
        
    else
        {
        return KErrSVPHoldInProgress;
        }
    }

// ---------------------------------------------------------------------------
// CSVPHoldController::ResumeSession
// ---------------------------------------------------------------------------
//
TInt CSVPHoldController::ResumeSession( CMceSession* aSession )
    {
    if ( ResumeAllowed() )
        {
        iHoldRequest = ESVPHoldToResume;
        iHoldRequestCompleted = EFalse;
        TInt err = KErrNone;
        iContext->SetSessionObject( aSession );
        TRAP( err, iContext->ApplyCurrentStateL( aSession,
                                                 ESVPHoldToResume ) );
        return err;
        }
    else
        {
        return KErrSVPHoldResumeInProgress;
        }
    }

// ---------------------------------------------------------------------------
// CSVPHoldController::TimedOut
// ---------------------------------------------------------------------------
//
void CSVPHoldController::TimedOut()
    {
    iContext->TimedOut();
    }

// ---------------------------------------------------------------------------
// CSVPHoldController::IncomingRequest
// ---------------------------------------------------------------------------
//
TInt CSVPHoldController::IncomingRequest( CMceSession* aSession )
    {
    SVPDEBUG1( "CSVPHoldController::IncomingRequest IN" )
    if ( IncomingRequestAllowed() )
        {
        SVPDEBUG1( "CSVPHoldController::IncomingRequest allowed" )
        
        iHoldRequestCompleted = EFalse;
        iContext->SetSessionObject( aSession );
        TInt err = KErrNone;
        TRAP( err, iContext->ApplyCurrentStateL( aSession,
                                                 ESVPHoldIncoming ) );
        
        if ( KErrSVPHoldNotHoldRequest == err )
            {
            iHoldRequestCompleted = ETrue;
            }
        
        return err;            
        }
        
    else
        {
        SVPDEBUG1( "CSVPHoldController::IncomingRequest not allowed!" )
        
        return KErrSVPHoldRequestPending;
        }    
    }

// ---------------------------------------------------------------------------
// CSVPHoldController::IncomingResponse
// ---------------------------------------------------------------------------
//
TInt CSVPHoldController::IncomingResponse( 
                            CMceSession* aSession, 
                            TInt aStatusCode )
    {
    if ( IncomingResponseAllowed() )
        {
        SVPDEBUG1( "CSVPHoldController::IncomingResponse - Allowed" );
        iContext->SetSessionObject( aSession );
        iContext->SetResponseStatusCode( aStatusCode );
        TInt err = KErrNone;
        TRAP( err, iContext->ApplyCurrentStateL() );
        iContext->SetResponseStatusCode( KErrNotFound );
                                                 
        SVPDEBUG2( "CSVPHoldController::IncomingResponse - Error = %i", err );                                         
        return err;            
        }
        
    else
        {
        SVPDEBUG1( "CSVPHoldController::IncomingResponse - not Allowed!" );
        return KErrSVPHoldRequestPending;
        }        
    }

// ---------------------------------------------------------------------------
// CSVPHoldController::HoldInProgress
// ---------------------------------------------------------------------------
//
TBool CSVPHoldController::HoldInProgress() const
    {
    return !iHoldRequestCompleted;
    }

// ---------------------------------------------------------------------------
// CSVPHoldController::HoldRolledBack
// ---------------------------------------------------------------------------
//
TBool CSVPHoldController::HoldRolledBack() const
    {
    return iContext->HoldRolledBack();
    }

// ---------------------------------------------------------------------------
// CSVPHoldController::HoldFailed
// ---------------------------------------------------------------------------
//
TBool CSVPHoldController::HoldFailed()
    {
    return iContext->HoldFailed();    
    }

// ---------------------------------------------------------------------------
// CSVPHoldController::ResumeFailed
// ---------------------------------------------------------------------------
//
TBool CSVPHoldController::ResumeFailed()
    {
    return iContext->ResumeFailed();    
    }

// ---------------------------------------------------------------------------
// CSVPHoldController::ContinueHoldProcessing
// ---------------------------------------------------------------------------
//
TInt CSVPHoldController::ContinueHoldProcessing( CMceSession& aSession )
    {
    SVPDEBUG1( "CSVPHoldController::ContinueHoldProcessing - IN" );
    iReinviteCrossover = EFalse;
    TInt err = KErrNone;
    iContext->SetSessionObject( &aSession );

    TRAP( err, iContext->ApplyCurrentStateL() );
    
    iHoldRequestCompleted = ETrue;
    SVPDEBUG2( "CSVPHoldController::ContinueHoldProcessing - Ready, err = %i",
                err );
    return err;
    }

// ---------------------------------------------------------------------------
// CSVPHoldController::RequestFailed
// ---------------------------------------------------------------------------
//
TInt CSVPHoldController::RequestFailed( CMceSession& aSession, 
                                        TInt aStatusCode, 
                                        CSVPSessionBase& aBase )
    {
    SVPDEBUG1( "CSVPHoldController::RequestFailed - IN" );


    aBase.StopTimers();
    
    // If 491 received first time, try it once more. No need to enable /
    // disable audios
    if ( KSVPRequestPendingVal == aStatusCode )
        {
        SVPDEBUG1( "CSVPHoldController::RequestFailed - 491 received" )
        if ( !iReinviteCrossover )
            {
            // First 491 received              
            iReinviteCrossover = ETrue;
            iContext->CrossOver( ETrue );                
            }
        else
            {
            // Second 491; hold/resume request failed
            iReinviteCrossover = EFalse;
            iContext->CrossOver( EFalse );
            }
        }

    TInt err = KErrNone;
    if ( iReinviteCrossover )
        {
        TRAP( err, aBase.StartTimerL( aBase.ReinviteCrossoverTime(),
                                       KSVPReInviteTimerExpired ) );
        if ( err )
            {
            SVPDEBUG2("CSVPHoldController::RequestFailed - Timer, err %i",
                       err )
            }
        }

    iContext->SetCallRequestFailed( ETrue );
    err = IncomingResponse( &aSession, aStatusCode );
    iContext->SetCallRequestFailed( EFalse );
    SVPDEBUG1( "CSVPHoldController::RequestFailed - OUT" );
    return err;
    }


// ---------------------------------------------------------------------------
// CSVPHoldController::CheckCrossOver
// ---------------------------------------------------------------------------
//
void CSVPHoldController::CheckCrossOver( CSVPSessionBase& aBase )
    {
    SVPDEBUG1( "CSVPHoldController::CheckCrossOver - IN" );
    if ( iReinviteCrossover )
        {
        SVPDEBUG1("CSVPHoldController::CheckCrossOver - True");

        aBase.StopTimers();
        iReinviteCrossover = EFalse;
        iContext->CrossOver( EFalse );
 
        TSVPHoldRequestType previousRequest = HoldRequest();
        
        if ( ESVPLocalHold == previousRequest ||
             ESVPLocalDoubleHold == previousRequest )
            {
            aBase.HoldRequestFailed();
            }
        
        else if ( ESVPLocalResume == previousRequest ||
                  ESVPLocalDoubleHoldResume == previousRequest )
            {
            aBase.ResumeRequestFailed();
            }
        
        else
            {
            // This case should never happen
            SVPDEBUG2( "CSVPHoldController::CheckCrossOver - Error,  = %i",
                        previousRequest );
            }
        }
    
    SVPDEBUG1( "CSVPHoldController::CheckCrossOver - OUT" );
    }

// ---------------------------------------------------------------------------
// CSVPHoldController::HoldState
// Returns the hold state.
// ---------------------------------------------------------------------------
//
TSVPSessionHoldState CSVPHoldController::HoldState() const
    {
    SVPDEBUG1( "CSVPHoldController::HoldState" );
    
    TSVPHoldStateIndex currentState = iContext->CurrentState();
    
    
    if ( KSVPHoldConnectedStateIndex == currentState )
        {
        SVPDEBUG1( "CSVPHoldController::HoldState - CONNECTED" );
        return ESVPConnected;
        }
        
    else if ( KSVPHoldEstablishingStateIndex == currentState )
        {
        SVPDEBUG1( "CSVPHoldController::HoldState - ESTABLISHING" );
        return ESVPEstablishing;
        }
        
    else
        {
        SVPDEBUG1( "CSVPHoldController::HoldState - ONHOLD" );
        return ESVPOnHold;
        }
    }

// ---------------------------------------------------------------------------
// CSVPHoldController::HoldRequest
// Returns the hold request type.
// ---------------------------------------------------------------------------
//
TSVPHoldRequestType CSVPHoldController::HoldRequest() const
    {
    SVPDEBUG1( "CSVPHoldController::HoldRequest" );
    return iContext->HoldRequest();
    }

// ---------------------------------------------------------------------------
// CSVPHoldController::HoldAllowed
// ---------------------------------------------------------------------------
//
TBool CSVPHoldController::HoldAllowed()
    {
    TSVPHoldStateIndex currentState = iContext->CurrentState();
    
    if ( KSVPHoldOutStateIndex == currentState ||
         KSVPHoldEstablishingStateIndex == currentState ||
         KSVPHoldDHStateIndex == currentState )
        {
        SVPDEBUG1( "CSVPHoldController::HoldAllowed - EFalse" );
        return EFalse;
        }
    else
        {
        SVPDEBUG1( "CSVPHoldController::HoldAllowed - ETrue" );
        return ETrue;
        }
    }

// ---------------------------------------------------------------------------
// CSVPHoldController::ResumeAllowed
// ---------------------------------------------------------------------------
//   
TBool CSVPHoldController::ResumeAllowed()
    {
    TSVPHoldStateIndex currentState = iContext->CurrentState();
    if ( KSVPHoldInStateIndex == currentState ||
         KSVPHoldEstablishingStateIndex == currentState ||
         KSVPHoldConnectedStateIndex == currentState )
        {
        return EFalse;
        }
    else
        {
        SVPDEBUG1( "CSVPHoldController::ResumeAllowed" );
        return ETrue;
        }    
    }

// ---------------------------------------------------------------------------
// CSVPHoldController::Muted
// ---------------------------------------------------------------------------
//
void CSVPHoldController::Muted( TBool aMuted )
    {
    iContext->Muted( aMuted );
    }

// ---------------------------------------------------------------------------
// CSVPHoldController::IncomingRequestAllowed
// ---------------------------------------------------------------------------
//   
TBool CSVPHoldController::IncomingRequestAllowed()
    {
    if ( KSVPHoldEstablishingStateIndex == iContext->CurrentState() )
        {
        return EFalse;
        }
    else
        {
        SVPDEBUG1( "CSVPHoldController::IncomingRequestAllowed" );
        
        return ETrue;
        }        
    }
    
// ---------------------------------------------------------------------------
// CSVPHoldController::IncomingResponseAllowed
// ---------------------------------------------------------------------------
//   
TBool CSVPHoldController::IncomingResponseAllowed()
    {
    if ( KSVPHoldEstablishingStateIndex == iContext->CurrentState() )
        {
        SVPDEBUG1( "CSVPHoldController::IncomingResponseAllowed" );
        return ETrue;
        }
    else
        {
        return EFalse;
        }        
    }

// ---------------------------------------------------------------------------
// CSVPHoldController::HoldEvent
// ---------------------------------------------------------------------------
//
MCCPCallObserver::TCCPCallEvent CSVPHoldController::HoldEvent()
    {
    return iContext->HoldEvent();
    }

// ---------------------------------------------------------------------------
// CSVPHoldController::RefreshHoldStateL
// ---------------------------------------------------------------------------
//
void CSVPHoldController::RefreshHoldStateL()
    {
    SVPDEBUG1( "CSVPHoldController::RefreshHoldStateL - In" );

    CMceSession* session = iContext->SessionObject();
    const RPointerArray< CMceMediaStream >& streams = session->Streams();
    CMceMediaStream* mediaStream = NULL;

    TInt streamCount = streams.Count();
    for ( TInt i = 0; i < streamCount; i++ )
        {
        mediaStream = streams[ i ];
        TMceMediaType mediaType = mediaStream->Type();
        if ( KMceAudio == mediaType )
            {
            SVPDEBUG1( "CSVPHoldController::RefreshHoldStateL - Stream found" );
            mediaStream = streams[ i ];
            RefreshL( *mediaStream );
            break;
            }
        }
       
    SVPDEBUG1( "CSVPHoldController::RefreshHoldStateL - Out" );
    }

// ---------------------------------------------------------------------------
// CSVPHoldController::RefreshL
// ---------------------------------------------------------------------------
//
void CSVPHoldController::RefreshL( CMceMediaStream& aMediaStream )
    {
    if ( &aMediaStream )
        {
        switch ( iContext->CurrentState() )
            {
            case KSVPHoldOutStateIndex:
                {
                iContext->MediaHandler().PerformMediaActionL(
                        aMediaStream, ESVPLocalHold );
                break;
                }
                
            case KSVPHoldInStateIndex:
                {
                iContext->MediaHandler().PerformMediaActionL(
                        aMediaStream, ESVPRemoteHold );
                break;
                }
                
            case KSVPHoldDHStateIndex:
                {
                iContext->MediaHandler().PerformMediaActionL(
                        aMediaStream, ESVPLocalDoubleHold );
                break;
                }
            
            case KSVPHoldConnectedStateIndex:
                {
                // Special case here; for enablingIOP: audio is lost on Snom M3 when it unholds the call
                SVPDEBUG1( "CSVPHoldController::RefreshL -> Refresh audio" );
                iContext->MediaHandler().EnableAudioL( *iContext );
                SVPDEBUG1( "CSVPHoldController::RefreshL <- Refresh audio done" );
                break;
                }
                
            case KSVPHoldEstablishingStateIndex:
                {
                SVPDEBUG1( "CSVPHoldController::RefreshL - Not needed" );
                break;
                }
            }
        }

    SVPDEBUG1( "CSVPHoldController::RefreshL - Out" );
    }


