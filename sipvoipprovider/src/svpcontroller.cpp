/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handles all common logic for SVP and handles sessions.        
*
*/

#include <mcevideostream.h>
#include <mceaudiocodec.h>
#include <mcemediasink.h>
#include <mcemediasource.h>
#include <mceevent.h>
#include <mcesession.h>
#include <mceinrefer.h>
#include <mcesecuresession.h>
#include <mcertpsource.h>
#include <mccpdtmfobserver.h>
#include <sipstrings.h>
#include <sipstrconsts.h>

#include <crcseprofileentry.h>
#include <crcseprofileregistry.h>

#include <wlantrafficstreamparameters.h> // CWlanMgmtClient
#include <ccpdefs.h>
#include <featdiscovery.h>

#include "svpcontroller.h"
#include "svpmosession.h"
#include "svpmtsession.h"
#include "svplogger.h"
#include "svptimer.h"
#include "svputility.h"
#include "svpholdcontext.h"
#include "svpholdcontroller.h"
#include "svpcleanupresetanddestroy.h"
#include "svpaudioutility.h"
#include "svpsipconsts.h"
#include "svpemergencyiapprovider.h"
#include "svprtpobserver.h"
#include "svpsettings.h"
#include "svptransferstatecontext.h"

// ---------------------------------------------------------------------------
// CSVPController::CSVPController
// ---------------------------------------------------------------------------
//
CSVPController::CSVPController() : iSessionUpdateOngoing( EFalse )
    {
    
    }

// ---------------------------------------------------------------------------
// CSVPController::ConstructL
// ---------------------------------------------------------------------------
//
void CSVPController::ConstructL()
    {
    SVPDEBUG1( "CSVPController::ConstructL In" )
    
    iIncomingReferCallIndex = KErrNotFound;
    iHoldCallIndex = KErrNotFound;
    iSuppServices = CSVPSupplementaryServices::NewL();
    iSVPUtility = CSVPUtility::NewL();
    iRtpObserver = CSVPRtpObserver::NewL();
    TBool wlanSupported = CFeatureDiscovery::IsFeatureSupportedL( KFeatureIdProtocolWlan );
#ifndef __WINS__
    if ( wlanSupported )
        {
        iWlanMgmt = CWlanMgmtClient::NewL();
        }
#endif // __WINS__

    SVPDEBUG1( "CSVPController::ConstructL Out" )
    }

// ---------------------------------------------------------------------------
// CSVPController::NewL
// ---------------------------------------------------------------------------
//
CSVPController* CSVPController::NewL()
    {
    CSVPController* self = new ( ELeave ) CSVPController();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CSVPController::~CSVPController
// ---------------------------------------------------------------------------
//
CSVPController::~CSVPController()
    {
    SVPDEBUG1( "CSVPController::~CSVPController In" )
    
#ifndef __WINS__
    delete iWlanMgmt;
#endif // __WINS__

    delete iSVPUtility;
    
    // clean array that contains SVP sessions.
    iSessionArray.ResetAndDestroy();
    iSessionArray.Close();
    
    // SVP emergency session
    delete iEmergencySession;
 
    iEmergencyProfileIds.Close();
    iEmergencyIapIds.Close();
    
    delete iSuppServices;
    delete iMceManager;
    delete iRtpObserver;
    
    delete iDtmfString;
    
    SVPDEBUG1( "CSVPController::~CSVPController Out" )
    }

// ---------------------------------------------------------------------------
// CSVPController::InitializeL 
// ---------------------------------------------------------------------------
//
void CSVPController::InitializeL( const TUint32 aServiceId,
		  					      const MCCPObserver& aMonitor, 
		                          const MCCPSsObserver& aSsObserver )
    {
    
    SVPDEBUG2( "CSVPController::InitializeL serviceId = %d", aServiceId )
    
    SVPDEBUG2( "CSVPController::InitializeL iCCPMonitor: 0x%x", iCCPMonitor )
    
    //Find callprovider plugin uid that should be used when registering to MCE
    TInt ccpUid = SvpSettings::IntPropertyL( aServiceId, 
										EPropertyCallProviderPluginId );
    SVPDEBUG2( "CSVPController::InitializeL callproviderpluginuid=%d", ccpUid )

    // See CConvergedCallProvider API documentation. If already called,
    // leave with KErrAlreadyExists.
    __ASSERT_ALWAYS( !iCCPMonitor, User::Leave( KErrAlreadyExists ) );
    __ASSERT_ALWAYS( !iCCPSsObserver, User::Leave( KErrAlreadyExists ) );
    __ASSERT_ALWAYS( iSVPUtility, User::Leave( KErrNotReady ) );
    __ASSERT_ALWAYS( iRtpObserver, User::Leave( KErrNotReady ) );
    
    // save CCP monitor 
    iCCPMonitor = const_cast< MCCPObserver* >( &aMonitor );
     
    // save CCP Supplementary Services observer
    iCCPSsObserver = const_cast< MCCPSsObserver* >( &aSsObserver );
    
    // get terminal type and wlan mac address, needed for user agent header
    iSVPUtility->GetTerminalTypeL( iTerminalType );
    iSVPUtility->GetWlanMACAddressL( iWlanMacAddress );
    
    TUid uid = { ccpUid };
    // create Mce Manager, establishes connection to Mce server
    iMceManager = CMceManager::NewL( uid, &iContainer );
    
    // set observers for asynchronous events
    iMceManager->SetSessionObserver( this );
    iMceManager->SetInSessionObserver( this );
    iMceManager->SetInReferObserver( this );
    iMceManager->SetReferObserver( this );
    iMceManager->SetMediaObserver( this );
    iMceManager->SetEventObserver( this );
    iMceManager->SetDtmfObserver( this );
    iMceManager->SetRtpObserver( iRtpObserver );
    
    iTrafficStreamCreated = EFalse;
    
    SVPDEBUG1( "CSVPController::InitializeL Out" )
    } 

// ---------------------------------------------------------------------------
// CSVPController::InitializeL 
// ---------------------------------------------------------------------------
//
void CSVPController::InitializeL( const MCCPObserver& aMonitor,
                                  const MCCPSsObserver& aSsObserver )
    {
    SVPDEBUG2( "CSVPController::InitializeL iCCPMonitor: 0x%x", iCCPMonitor )
    SVPDEBUG2( "CSVPController::InitializeL iCCPSsObserver: 0x%x", iCCPSsObserver )
    
    // See CConvergedCallProvider API documentation. If already called,
    // leave with KErrAlreadyExists.
    __ASSERT_ALWAYS( !iCCPMonitor, User::Leave( KErrAlreadyExists ) );
    __ASSERT_ALWAYS( !iCCPSsObserver, User::Leave( KErrAlreadyExists ) );
    __ASSERT_ALWAYS( iSVPUtility, User::Leave( KErrNotReady ) );
    __ASSERT_ALWAYS( iRtpObserver, User::Leave( KErrNotReady ) );
    
    // save CCP monitor 
    iCCPMonitor = const_cast< MCCPObserver* >( &aMonitor );
    
    // save CCP Supplementary Services observer
    iCCPSsObserver = const_cast< MCCPSsObserver* >( &aSsObserver );

    // get terminal type and wlan mac address, needed for user agent header
    iSVPUtility->GetTerminalTypeL( iTerminalType );
    iSVPUtility->GetWlanMACAddressL( iWlanMacAddress );
    
    // create Mce Manager, establishes connection to Mce server
    iMceManager = CMceManager::NewL( KSVPImplementationUid, &iContainer );
    
    // set observers for asynchronous events
    iMceManager->SetSessionObserver( this );
    iMceManager->SetInSessionObserver( this );
    iMceManager->SetInReferObserver( this );
    iMceManager->SetReferObserver( this );
    iMceManager->SetMediaObserver( this );
    iMceManager->SetEventObserver( this );
    iMceManager->SetDtmfObserver( this );
    iMceManager->SetRtpObserver( iRtpObserver );
    
    iTrafficStreamCreated = EFalse;

    SVPDEBUG1( "CSVPController::InitializeL Out" )
    }
    
// ---------------------------------------------------------------------------
// CSVPController::NewCallL
// ---------------------------------------------------------------------------
// 
MCCPCall* CSVPController::NewCallL( 
                            const CCCPCallParameters& aParameters,
                            const TDesC& aRecipient,
                            const MCCPCallObserver& aObserver )
    {
    SVPDEBUG1( "CSVPController::NewCallL In" )
   
    __ASSERT_ALWAYS( &aParameters, User::Leave( KErrArgument ) );
    __ASSERT_ALWAYS( &aRecipient, User::Leave( KErrArgument ) );
    __ASSERT_ALWAYS( &aObserver, User::Leave( KErrArgument ) );
 
#ifndef __WINS__
    // Preliminary Call Admission Control implementation, run only if no stream yet open
    if ( !iTrafficStreamCreated )
        {
        SVPDEBUG1( "CSVPController::NewCallL - Creating WLAN traffic stream.." )

        TRequestStatus requestStatus;
        TUint streamId = 0;
        TWlanTrafficStreamParameters streamParams( KSVPWlanTrafficStreamParamUserPriority );
        TWlanTrafficStreamStatus streamStatus = EWlanTrafficStreamStatusActive;

        // Disabling WLAN traffic stream automation and get stream status
        iWlanMgmt->CreateTrafficStream( requestStatus, streamParams, streamId, streamStatus );
        User::WaitForRequest( requestStatus );
        
        // Save traffic stream ID, needed for stream deletion
        iTrafficStreamId = streamId;

        if ( EWlanTrafficStreamStatusInactiveNoBandwidth == streamStatus )
            {
            SVPDEBUG1( "CSVPController::NewCallL - No bandwidth in traffic stream, leaving with ECCPErrorNetworkBusy" )        

            // Enabling WLAN traffic stream automation back by deleting used stream
            iWlanMgmt->DeleteTrafficStream( requestStatus, iTrafficStreamId );
            User::WaitForRequest( requestStatus );
            
            TCCPError err = ECCPErrorNetworkBusy;
            User::Leave( err );
            }
        SVPDEBUG1( "CSVPController::NewCallL - WLAN traffic stream created, continuing with NewCallL" )

        iTrafficStreamCreated = ETrue;
        }
    else
        {
        SVPDEBUG1( "CSVPController::NewCallL - WLAN traffic stream already created, continuing with NewCallL" )
        ;
        }
#endif // __WINS__
    
    SVPDEBUG2( "CSVPController::NewCallL aRecipient length = %d", aRecipient.Length() )
    SVPDEBUG2( "CSVPController::NewCallL aRecipient = %S", &aRecipient )
    SVPDEBUG2( "CSVPController::NewCallL Serviceid = %d", aParameters.ServiceId() )

    // Prepare and remove possible DTMF suffix
    HBufC* parsedRecipient = NULL;
    parsedRecipient = ParseRecipientDtmfSuffixL( aRecipient );
  
    // "convert" recpient to 8-bit format
    HBufC8* recipient = HBufC8::NewLC( parsedRecipient->Length() ); // CS:1
    TPtr8 temp = recipient->Des();
    temp.Copy( *parsedRecipient );

    // Pointer to parsedRecipient no longer needed
    delete parsedRecipient;
    parsedRecipient = NULL;
    
    // fetch SIP profile ID from VoIP profiles
    RPointerArray< CRCSEProfileEntry > entryArray;
    CleanupResetAndDestroy<
        RPointerArray<CRCSEProfileEntry> >::PushL( entryArray ); //CS:2
    
    CRCSEProfileRegistry* reg = CRCSEProfileRegistry::NewLC(); // CS:3
    
    // take service id to local variable
    TUint32 serviceId = aParameters.ServiceId();
    
    // Get VoIP profile by service id
    reg->FindByServiceIdL( serviceId, entryArray );
    __ASSERT_ALWAYS( entryArray.Count(), User::Leave( KErrArgument ) );
    
    // Take first entry from array
    CRCSEProfileEntry* entry = entryArray[0];
    
    // create new session
    MCCPCall* moSessionTemp = CreateNewSessionL( *recipient, 
                                                 *entry, 
                                                 aParameters,
                                                 aObserver );
    
    CleanupStack::PopAndDestroy( 2, &entryArray ); // CS:1
    CleanupStack::PopAndDestroy( recipient ); // CS:0
    
    SVPDEBUG1( "CSVPController::NewCallL Out" )
    return moSessionTemp;
    }

// ---------------------------------------------------------------------------
// CSVPController::CreateNewSessionL
// ---------------------------------------------------------------------------
//
MCCPCall* CSVPController::CreateNewSessionL( 
    TDesC8& aRecipient, 
    CRCSEProfileEntry& aVoipProfile, 
    const CCCPCallParameters& aParameters,
    const MCCPCallObserver& aObserver )
    {
    SVPDEBUG1( "CSVPController::CreateNewSessionL In" )
    
    // array for provisioned data
    CDesC8ArrayFlat* userAgentHeaders = new( ELeave ) CDesC8ArrayFlat( 4 );
    CleanupStack::PushL( userAgentHeaders ); // CS:1
    
    // variable for storing security status
    TUint32 securityStatus( 0 );
    
    // set provisioning data
    SVPDEBUG1( "CSVPController::CreateNewSessionL Set provisioning data..." )
    iSVPUtility->SetProvisioningDataL( aVoipProfile, 
                                       *userAgentHeaders,
                                       securityStatus,
                                       iTerminalType,
                                       iWlanMacAddress  );
    
    SVPDEBUG2( "CSVPController::CreateNewSessionL Security status: %d", securityStatus )
    SVPDEBUG2( "CSVPController::CreateNewSessionL UAHeaders count: %d", userAgentHeaders->Count() )
    
    // Creates a SVP Mo session 
    CSVPMoSession* moSessionTemp = CSVPMoSession::NewL( *iMceManager,
                                                        aRecipient,
                                                        aVoipProfile, 
                                                        aParameters,
                                                        iContainer,
                                                        *this,
                                                        *iSVPUtility,
                                                        *iRtpObserver,
                                                        securityStatus,
                                                        userAgentHeaders );
    CleanupStack::Pop( userAgentHeaders ); // CS:0, to member data
    CleanupStack::PushL( moSessionTemp ); // CS:1
    
    FinalizeSessionCreationL( moSessionTemp );
    
    // set CCP session observer to SVP session
    moSessionTemp->AddObserverL( aObserver );
    // set CCP supplementary services events observer to SVP session
    moSessionTemp->SetSsObserver( *iCCPSsObserver );
    
    CleanupStack::Pop( moSessionTemp ); // CS:0
    
    // session pointer is passed to CCP
    SVPDEBUG1( "CSVPController::CreateNewSessionL Out" )
    return moSessionTemp;
    }

// ---------------------------------------------------------------------------
// Releases call
// ---------------------------------------------------------------------------
// 
TInt CSVPController::ReleaseCall( MCCPCall& aCall )
    {
    SVPDEBUG1( "CSVPController::ReleaseCall In" )

    TInt response = KErrNotFound;

    // To ensure correct value
    iIncomingReferCallIndex = KErrNotFound;
    iSessionUpdateOngoing = EFalse;

    for ( TInt i = 0; i < iSessionArray.Count(); i++ )
        {
        if ( iSessionArray[i] == &aCall )
            {
            if ( iDtmfStringSending )
                {
                SVPDEBUG1( "CSVPController::ReleaseCall - Abort DTMF Sending" )
                
                // send abort event
                iSessionArray[i]->DtmfObserver().HandleDTMFEvent(
                       MCCPDTMFObserver::ECCPDtmfSequenceAbort,
                       KErrNone,
                       iDtmfStringLex.Peek() );
                
                SVPDEBUG1( "CSVPController::ReleaseCall - Complete DTMF" )
                
                // send sequence completed event
                iSessionArray[i]->DtmfObserver().HandleDTMFEvent(
                    MCCPDTMFObserver::ECCPDtmfStringSendingCompleted,
                    KErrNone,
                    iDtmfStringLex.Peek() );
                
                // sequence complete, clear flags
                iDtmfStringSending = EFalse;
                iFirstDtmfSent = EFalse;
                
                delete iDtmfString;
                iDtmfString = NULL;
                }
            iSessionArray[i]->Release();
            response = KErrNone;
            }
        }

#ifndef __WINS__
    // Preliminary Call Admission Control implementation, delete only if created in NewCallL 
    if ( iTrafficStreamCreated )
        {
        SVPDEBUG1( "CSVPController::ReleaseCall - Deleting WLAN traffic stream" )

        TRequestStatus requestStatus;

        // Enabling WLAN traffic stream automation back by deleting used stream
        iWlanMgmt->DeleteTrafficStream( requestStatus, iTrafficStreamId );
        User::WaitForRequest( requestStatus );
        
        iTrafficStreamCreated = EFalse;

        SVPDEBUG1( "CSVPController::ReleaseCall - WLAN traffic stream deleted" )
        }
    else
        {
        SVPDEBUG1( "CSVPController::ReleaseCall - No WLAN traffic stream to delete" )
        ;
        }
#endif // __WINS__

    SVPDEBUG1( "CSVPController::ReleaseCall Out" )
    return response;
    }

// ---------------------------------------------------------------------------
// Releases emergency call
// ---------------------------------------------------------------------------
// 
TInt CSVPController::ReleaseEmergencyCall( MCCPEmergencyCall& /*aCall*/ )
    {
    SVPDEBUG1( "CSVPController::ReleaseEmergencyCall In" )

    // To ensure correct value next time
    iIncomingReferCallIndex = KErrNotFound;
    iSessionUpdateOngoing = EFalse;

    delete iEmergencySession;
    iEmergencySession = NULL;
    
    SVPDEBUG1( "CSVPController::ReleaseEmergencyCall Out" )
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Releases conference call
// ---------------------------------------------------------------------------
// 
TInt CSVPController::ReleaseConferenceCall( MCCPConferenceCall& /*aCall*/ )
    {
    SVPDEBUG1( "CSVPController::ReleaseConferenceCall In" )
    SVPDEBUG1( "CSVPController::ReleaseConferenceCall Out" )
    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// CSVPController::RemoveFromArray
// ---------------------------------------------------------------------------
// 
void CSVPController::RemoveFromArray( CSVPSessionBase& aSession )
    {
    SVPDEBUG1( "CSVPController::RemoveFromArray In" )
    
    // When Sessions destructor is called object be removed from array
    // manually after when data has been deleted.
    SVPDEBUG2( "CSVPController::RemoveFromArray aSession: 0x%x", &aSession )
    
    TInt index = FindSVPSession( aSession.Session() );  
    if ( KErrNotFound != index )
        {
        SVPDEBUG1( "CSVPController::RemoveFromArray Session found - removing" )
        iSessionArray.Remove( index );
        iSessionArray.Compress();
        }
    
    // Try also to remove it from RTCP observer. Ingore the return code, yet
    // again this is "just in case" thing.
    iRtpObserver->RemoveSessionFromObserving( &aSession );
    SVPDEBUG1( "CSVPController::RemoveFromArray Out" )
    }

// ---------------------------------------------------------------------------
// CSVPController::FindSVPSession
// ---------------------------------------------------------------------------
//
TInt CSVPController::FindSVPSession( const CMceSession& aSession ) const
    {
    SVPDEBUG1( "CSVPController::FindSVPSession In" )
    
    TInt count = iSessionArray.Count();
    SVPDEBUG2( "CSVPController::FindSVPSession count = %d", count )
    
    while ( count )
        {
        count--;
        
        if (  &iSessionArray[count]->Session() == &aSession )
            {
            SVPDEBUG2( "CSVPController::FindSVPSession Out return: %d", count )
            return count;
            }
        }
    
    SVPDEBUG1( "CSVPController::FindSVPSession Out return: KErrNotFound" )
    return KErrNotFound;
    }

// ---------------------------------------------------------------------------
// CSVPController::Uid
// ---------------------------------------------------------------------------
//
const TUid& CSVPController::Uid() const
    {
    SVPDEBUG1( "CSVPController::Uid" )
    return KSVPImplementationUid; 
    }

// ---------------------------------------------------------------------------
// CSVPController::CreateNonSecureSessionL
// ---------------------------------------------------------------------------
// 
void CSVPController::CreateNonSecureSessionL( TInt aSessionIndex )
    {
    SVPDEBUG1( "CSVPController::CreateNonSecureSessionL In" )
    
    // secure session failed -> create non secure Mo session
    static_cast< CSVPMoSession* >( iSessionArray[ aSessionIndex ] )->
            CreateNonSecureSessionL( *iMceManager );
    
    SVPDEBUG1( "CSVPController::CreateNonSecureSessionL Out" )
    }

// ---------------------------------------------------------------------------
// CSVPController::CheckIfSecureFailed
// ---------------------------------------------------------------------------
//    
TBool CSVPController::CheckIfSecureFailed( TInt aStatusCode ) const
    {
    SVPDEBUG2( "CSVPController::CheckIfSecureFailed In, aStatusCode: %d", aStatusCode )
    
    // if secure fails with the following response codes
    // normal session is tried next
    if ( KSVPNotAcceptableHereVal == aStatusCode ||
         KSVPNotAcceptableVal == aStatusCode ||
         KSVPPreconditionFailureVal == aStatusCode ||
         KSVPMethodNotAllowedVal == aStatusCode )
        {
        SVPDEBUG1( "CSVPController::CheckIfSecureFailed Out ETrue" )
        return ETrue;
        }
    else
        {
        SVPDEBUG1( "CSVPController::CheckIfSecureFailed Out EFalse" )
        return EFalse;
        }
    }

// ---------------------------------------------------------------------------
// CSVPController::SessionStateChanged
// ---------------------------------------------------------------------------
// 
void CSVPController::SessionStateChanged( CMceSession& aSession,
    TMceTransactionDataContainer* aContainer )
    {
    SVPDEBUG1( "CSVPController::SessionStateChanged In" )
    
    // Note, getting the status code may reset it in the container. So do it
    // only once and be sure that it stays as it should be.
    const TInt statusCode = aContainer->GetStatusCode();
    SVPDEBUG2( "CSVPController::SessionStateChanged statusCode: %d", statusCode )
    
    // check if this is emergency session
    if ( iEmergencySession )
        {
        SVPDEBUG1( "CSVPController::SessionStateChanged, emergency session" )
        
        if ( CMceSession::EEstablished == aSession.State() )
            {
            // Rest of the profiles not needed since session is established
            iEmergencyProfileIds.Reset();
            iEmergencyIapIds.Reset();
            }
        
        if ( CMceSession::EEstablished == aSession.State() &&
             iEmergencySession->HasHoldController() &&
             ( ( iEmergencySession->HoldController() ).HoldInProgress() ||
               ( iEmergencySession->HoldController() ).HoldRolledBack() ) )
            {
            // Handle hold
            TRAPD( error, iEmergencySession->HoldSessionStateChangedL( aSession ) )
            
            if ( error )
                {
                SVPDEBUG2( "CSVPController::SessionStateChanged, emergency session error %d", error )
                }
            }
        else
            {
            iEmergencySession->SessionStateChanged( statusCode );
            }
        }
    else if ( iSessionUpdateOngoing )
        {
        // Non-hold session update, e.g. codec renegotiation ongoing; no actions
        // needed until MCE session state is established.
        SVPDEBUG1( "CSVPController::SessionStateChanged - MT Session Update" )
        
        if ( CMceSession::EEstablished == aSession.State() )
            {
            // Session update done (Ack received)
            iSessionUpdateOngoing = EFalse;
            SVPDEBUG1( "CSVPController::SessionStateChanged - MT Session Updated" )
            }
        }
    else
        {
        // fetch correct session
        const TInt ind = FindSVPSession( aSession );
        
        // KErrNone = status, status had to be error code
        if ( KErrNotFound != ind &&
             iSessionArray[ ind ]->SecurePreferred() &&
             CheckIfSecureFailed( statusCode ) ||
             CMceSession::ECancelling == aSession.State() )
            {
            SVPDEBUG1( "CSVPController::SessionStateChanged - Secure failed, trying normal session" )
            
            // trying normal session after secure session trial failed
            TRAPD( err, CreateNonSecureSessionL( ind ) );
            
            if ( err )
                {
                iSessionArray[ ind ]->GetCCPSessionObserver().
                    ErrorOccurred( ECCPRedirection, iSessionArray[ ind ] );
                iSessionArray[ ind ]->GetCCPSessionObserver().
                    CallStateChanged( MCCPCallObserver::ECCPStateIdle,
                                      iSessionArray[ ind ]);
                }
            }
        else if ( KErrNotFound != ind )
            {
            SVPDEBUG1( "CSVPController::SessionStateChanged - Session found" )
            
            CSVPSessionBase* session = iSessionArray[ ind ];
            
            if ( !session->CallId() &&
                 KSVPMultipleChoicesVal != statusCode &&
                 KSVPMovedPermanentlyVal != statusCode &&
                 KSVPMovedTemporarilyVal != statusCode )
                {
                // CallId not stored yet and not a 3XX case,
                // check and store headers data.
                CheckHeadersData( session, aContainer );
                }
            
            if ( !session->SecurePreferred() )
                {
                session->ReleaseTempSecure();
                }
            
            // Release attended transfer target session index 
            // if e.g. early stage session terminate
            if ( CMceSession::ETerminated == aSession.State() )
                {
                iHoldCallIndex = KErrNotFound;
                }
            
            if ( CMceSession::EProceeding == aSession.State() )
                {
                SVPDEBUG1( "CSVPController::SessionStateChanged EProceeding" )
                
                if ( KErrNotFound != iHoldCallIndex && iSessionArray.Count() > 1 )
                    {
                    SVPDEBUG3( "CSVPController::SessionStateChanged, atte trans target, ind %d, iHCInd %d",
                                                ind, iHoldCallIndex )
                    CheckCallEventToBeSent( session, iSessionArray[iHoldCallIndex] );
                    ExecCbIncomingCall( session, *iSessionArray[iHoldCallIndex] );
                    iHoldCallIndex = KErrNotFound;
                    }
                else if ( session->HasHoldController() &&
                          ( session->HoldController() ).HoldInProgress() )
                    {
                    // Do nothing here; this only prevents wrong new call
                    // callback to CCP.
                    SVPDEBUG1( "CSVPController::SessionStateChanged - HoldInProgress" )
                    }
                else if ( session->IsEmptyReInvite() )
                    {
                    SVPDEBUG1( "CSVPController::SessionStateChanged - Empty re-Invite handling ongoing" )
                    }
                else if ( !session->IsIdle() )
                    {
                    SVPDEBUG1( "CSVPController::SessionStateChanged - NOP" )
                    }
                else
                    {
                    SVPDEBUG1( "CSVPController::SessionStateChanged Incoming" )
                    ExecCbIncomingCall( session );
                    session->SessionStateChanged( statusCode );
                    }
                }
            else if ( CMceSession::EEstablished == aSession.State() &&
                      session->HasHoldController() &&
                    ( ( session->HoldController() ).HoldInProgress() ||
                      ( session->HoldController() ).HoldRolledBack() ) )
                {
                SVPDEBUG1( "CSVPController::SessionStateChanged EEstablished & HoldController" )
                
                session->HandleSessionStateChanged( aSession );
                }
            else if ( ( CMceSession::EEstablished == aSession.State() || 
                        CMceSession::ETerminated == aSession.State() ) &&
                        KErrNone != statusCode &&
                        KErrNotFound != iIncomingReferCallIndex &&
                        iIncomingReferCallIndex < iSessionArray.Count() &&
                        iSessionArray[ iIncomingReferCallIndex ]->IsIncomingTransfer() )
                {
                SVPDEBUG1( "CSVPController::SessionStateChanged transferor" )
                
                // Transferor sends notify to original when new session connected/failed.
                // If secure is mandatory but path unsecure, attended transfer req must be rejected
                if ( session->SecureMandatory() &&
                     CMceSession::EControlPathUnsecure == aSession.ControlPathSecurityLevel() &&
                     iSessionArray[ iIncomingReferCallIndex ]->IsAttended() )
                    {
                    iSessionArray[ iIncomingReferCallIndex ]->SendNotify( KSVPDeclineVal );
                    }
                else
                    {
                    iSessionArray[ iIncomingReferCallIndex ]->SendNotify( statusCode );
                    }

                if ( iSessionArray[ iIncomingReferCallIndex ]->IsAttended() )
                    {
                    // remotepartyinfo and/or secure status might have been changed,
                    // proper event will be sent later on.
                    CheckCallEventToBeSent( session, iSessionArray[iIncomingReferCallIndex] );
                    }
                iIncomingReferCallIndex = KErrNotFound;
                session->SessionStateChanged( statusCode );
                }
            // 3xx Call forward handling
            else if ( KSVPMultipleChoicesVal == statusCode  ||
                      KSVPMovedPermanentlyVal == statusCode ||
                      KSVPMovedTemporarilyVal == statusCode )
                {
                HandleCallForward( statusCode, ind, aContainer );
                }
            else
                {
                if ( ( aSession.State() == CMceSession::EOffering || 
                       aSession.State() == CMceSession::ETerminating ||
                       aSession.State() == CMceSession::ETerminated ) &&
                       iSessionArray[ ind ]->HasHoldController() &&
                       ( ( iSessionArray[ ind ]->HoldController() ).HoldInProgress() ||
                       ( iSessionArray[ ind ]->HoldController() ).HoldRolledBack() ) )
                    {
                    // Failed hold e.g. 408 response
                    SVPDEBUG1("CSVPController::SessionStateChanged()\
                    State change: Failed && HoldController");            
                    if ( 491 == statusCode )
                        {
                        SVPDEBUG1( 
                        "CSVPController::SessionStateChanged - UpdateFailed()" )
                        UpdateFailed( aSession, aContainer );
                        }
                    else if ( 100 <= statusCode && 200 > statusCode ) // Provisional response
                        {
                        // NOP
                        SVPDEBUG1( 
                        "CSVPController::SessionStateChanged - Provisional - NOP" )
                        }
                    else
                        {
                        iSessionArray[ ind ]->HandleSessionStateChanged( aSession );
                        session->SessionStateChanged( statusCode );                
                        }
                    }                
                
                else if ( session->HasHoldController() )
                    {
                    // MCE Session Refresh; no actions by the SVP
                    SVPDEBUG1( 
                    "CSVPController::SessionStateChanged - Session refresh" )
                    if ( CMceSession::ETerminating == aSession.State() ||
                         CMceSession::ETerminated == aSession.State() )
                        {
                        session->SessionStateChanged( statusCode );
                        }
                    }
                else if ( !iFailed )
                    {
                    SVPDEBUG1( 
                    "CSVPController::SessionStateChanged - Not handled -> Notify session" )
                    session->SessionStateChanged( statusCode );
                    }                
                else
                    {
                    // Failed() callback occurred before EEstablished state
                    // -> No action needed, session termination timer is set
                    }
                }
            }
        else
            {
            SVPDEBUG1( "CSVPController::SessionStateChanged else" )
            }
        }
    
    SVPDEBUG1( "CSVPController::SessionStateChanged Out" )
    }
    
// ---------------------------------------------------------------------------
// CSVPController::SessionConnectionStateChanged
// ---------------------------------------------------------------------------
//  
void CSVPController::SessionConnectionStateChanged(
                CMceSession& aSession,
                TBool aActive )
    {
    SVPDEBUG2("CSVPController::SessionConnectionStateChanged Active ETrue, Inactive EFalse: %d", aActive )
    
    if ( iEmergencySession )
        {
        iEmergencySession->SessionConnectionStateChanged( aSession, aActive );
        }
    }
    
// ---------------------------------------------------------------------------
// CSVPController::Failed
// ---------------------------------------------------------------------------
//
#ifdef _DEBUG
void CSVPController::Failed( CMceSession& aSession, TInt aError )
#else
void CSVPController::Failed( CMceSession& aSession, TInt /*aError*/ )
#endif // _DEBUG
    {
    SVPDEBUG1("CSVPController::Failed In" )
    SVPDEBUG2("CSVPController::Failed With error code: %d", aError )
    
    TInt ind = FindSVPSession( aSession );
    
    if ( KErrNotFound != ind )
        {
        SVPDEBUG1("CSVPController::Failed() Session found")
        
        if ( &iSessionArray[ ind ]->GetCCPSessionObserver() )
            {
        	iSessionArray[ ind ]->GetCCPSessionObserver().
            	ErrorOccurred( ECCPErrorConnectionError, 
            	               iSessionArray[ ind ] );
        
        	// Actually MCE session state should always 
        	// be ETerminated when this method is 
        	// called; but check anyway:        
        	if ( CMceSession::ETerminated == aSession.State() )
        	    {
                iFailed = ETrue;
        	    // start termination timer, needed for UI
        	    // to handle Disconnected-bubble   
        	    TRAP_IGNORE( iSessionArray[ ind ]->StartTimerL( 
        	            KSVPTerminatingTime, KSVPTerminationTimerExpired ) );
            
        	    iSessionArray[ ind ]->GetCCPSessionObserver().
        		    CallStateChanged( MCCPCallObserver::ECCPStateDisconnecting, 
        		                      iSessionArray[ ind ] );
            
        	    SVPDEBUG1("CSVPController::Failed() Session terminating");            
        	    }
            }
        else
            {
            iCCPMonitor->ErrorOccurred( MCCPObserver::ECCPIncomingCallFailed ); 
            }
        }
    else
        {
        SVPDEBUG1("CSVPController::Failed() Session not Found!!");
        }
    
    SVPDEBUG1("CSVPController::Failed Out" )
    }

// ---------------------------------------------------------------------------
// CSVPController::UpdateFailed
// ---------------------------------------------------------------------------
//
void CSVPController::UpdateFailed(
    			CMceSession& aSession,
    			TMceTransactionDataContainer* aContainer )
    {
    SVPDEBUG1( "CSVPController::UpdateFailed In" )
    
    TInt ind = FindSVPSession( aSession );
    TInt statusCode = aContainer->GetStatusCode();
    
    SVPDEBUG2( "CSVPController::UpdateFailed statusCode: %d", statusCode )
    
    if ( KErrNotFound != ind )
        {
        iSessionArray[ ind ]->UpdateFailed( aSession, statusCode );
        }
    iSessionUpdateOngoing = EFalse;
    SVPDEBUG1( "CSVPController::UpdateFailed Out" )
    }
    
// ---------------------------------------------------------------------------
// CSVPController::EventStateChanged
// ---------------------------------------------------------------------------
//			
void CSVPController::EventStateChanged(
    			CMceEvent& aEvent,
				TMceTransactionDataContainer* aContainer )
    {
    SVPDEBUG1( "CSVPController:EventStateChanged In" )
    
    iContainer = *aContainer;
    TInt ind = FindSVPSession( *aEvent.AssociatedSession() );
    TInt statusCode = iContainer.GetStatusCode();
    
    SVPDEBUG2( "CSVPController:EventStateChanged statusCode: %d", statusCode )

    if ( KErrNotFound != ind )
        {
        SVPDEBUG2( "CSVPController::EventStateChanged ind = %d", ind )
        TInt err = iSessionArray[ ind ]->EventStateChanged( aEvent, statusCode );
        
        if ( KSVPOKVal == statusCode &&
             iSessionArray[ ind ]->IsAttended() &&
             KErrNone == err )
            {
            SVPDEBUG1( "CSVPController::EventStateChanged Atte transfer cont." )
            
            TRAP( err, CreateNewTransferSessionL( ind, ETrue ) );
            
            if ( err )
                {
                SVPDEBUG2( "CSVPController::EventStateChanged: err: %d", err )
                }
            }
        }
    else
        {
        SVPDEBUG1( "CSVPController::EventStateChanged Session not Found!!" )
        }

    SVPDEBUG1("CSVPController:EventStateChanged OUT");
    }
    
// ---------------------------------------------------------------------------
// CSVPController::NotifyReceived
// ---------------------------------------------------------------------------
//
void CSVPController::NotifyReceived(
				CMceEvent& aEvent,
				TMceTransactionDataContainer* aContainer )
    {
    SVPDEBUG1( "CSVPController::NotifyReceived In" )
    
    TInt ind = FindSVPSession( *aEvent.AssociatedSession() );
    
    if ( KErrNotFound != ind )
        {         
        iSessionArray[ ind ]->NotifyReceived( aEvent, aContainer );
        }
    else
        {
        SVPDEBUG1( "CSVPController::NotifyReceived, session not found !" )
        }

    SVPDEBUG1( "CSVPController::NotifyReceived Out" )
    }

// ---------------------------------------------------------------------------
// CSVPController::EventConnectionStateChanged
// ---------------------------------------------------------------------------
//			
void CSVPController:: EventConnectionStateChanged(
                CMceEvent& /*aEvent*/,
                TBool /*aActive*/ )
    {
    SVPDEBUG1( "CSVPController:: EventConnectionStateChanged In" )
    SVPDEBUG1( "CSVPController:: EventConnectionStateChanged Out" )
    }

// ---------------------------------------------------------------------------
// CSVPController::Failed
// ---------------------------------------------------------------------------
//
#ifdef _DEBUG
void CSVPController::Failed( CMceEvent& /*aEvent*/, TInt aError )
#else
void CSVPController::Failed( CMceEvent& /*aEvent*/, TInt /*aError*/ )
#endif // _DEBUG
    {
    SVPDEBUG2("CSVPController::Failed Event failure, error: %d", aError )
    }

// ---------------------------------------------------------------------------
// CSVPController::IncomingSession
// ---------------------------------------------------------------------------
//
void CSVPController::IncomingSession(
                CMceInSession* aSession,
				TMceTransactionDataContainer* aContainer )
    {
    SVPDEBUG1( "CSVPController::IncomingSession In" )
    SVPDEBUG2( "CSVPController::IncomingSession - status code: %d", aContainer->GetStatusCode() )
    
    if ( iEmergencySession )
        {
        SVPDEBUG1( "CSVPController::IncomingSession - ongoing emergency call - reject incoming call" )
        TRAP_IGNORE( aSession->RejectL( KSVPBusyHereReason, KSVPBusyHereVal ) )
        // Ownership of aSession is changed, delete it
        delete aSession;
        aSession = NULL;
        return;
        }
    
    // IncomingSessionHandlerL is used to handle possible leave 
    SVPDEBUG1( "CSVPController::IncomingSession IncomingSessionHandlerL" )
    TRAPD( err, IncomingSessionHandlerL( aSession, aContainer ) );
    
    if ( err )
        {
        SVPDEBUG2( "CSVPController::IncomingSession: err: %d", err )
        
        // RejectL must be called in case SVP needs to delete session.
        // TRAP is used because of RejectL might contain a leaver.
        switch ( err )
            {
            case KErrNotSupported:
                {
                TRAP( err, aSession->RejectL( KSVPCallTransactDoesNotExistReason,
                        KSVPCallDoesNotExistVal ) );
                break;
                }
            
            case KSVPErrWrongMinSE:
                // NOP. Reject has been done as a result of call to
                // IncomingSessionHandlerL.
                break;
            
            case KSVPErrDnDRejection:
                {
                // Reject call because of Do not Disturb service active.
                TRAP( err, aSession->RejectL( KSVPBusyHereReason,
                        KSVPBusyHereVal ) );
                break;
                }
             
             case KSVPErrAnonymousCallRejection:
                {
                // Reject call because of Anonymous Barring service active.
                TRAP( err, aSession->RejectL( KSVPNotAcceptableHereReason,
                        KSVPNotAcceptableHereVal ) );
                break;
                }
             
             case KSVPErrCodecMismatch:
                {
                TRAP( err, aSession->RejectL( KSVPIncompatibleMediaFormatReason,
                        KSVPNotAcceptableHereVal ) );
                break;
                }
             
             default:
                {
                // NOP.
                break;
                }
            }
        
        if ( KErrNotFound == FindSVPSession( *aSession ) )
            {
            // Leave has occured before CSVPMtSession was created.
            delete aSession;
            }
        else
            {
            SVPDEBUG1( "CSVPController::IncomingSession IncomingSessionHandlerL leave occurred" )
            // clean up session 
            TerminateSession( *aSession );
            }
        }
    }

// ---------------------------------------------------------------------------
// CSVPController::TerminateSession
// ---------------------------------------------------------------------------
//
void CSVPController::TerminateSession( CMceInSession& aSession ) 
    {
    SVPDEBUG1( "CSVPController::TerminateSession In" )
    
    // find correct session
    TInt index =  FindSVPSession( aSession );
    
    if ( KErrNotFound != index )
        {
        CSVPSessionBase* tempBase = iSessionArray[ index ];
        // remove session pointer from array and compress array
        iSessionArray.Remove( index );
        iSessionArray.Compress();
        
        // delete session  
        delete tempBase;
        tempBase = NULL;            
        }
            
    SVPDEBUG1( "CSVPController::TerminateSession Out" )  
    }

// ---------------------------------------------------------------------------
// CSVPController::CheckMinSessionExpiresL
// ---------------------------------------------------------------------------
//
void CSVPController::CheckMinSessionExpiresL( CMceInSession& aSession,
                                              CDesC8Array& aHeaders, 
                                              TInt aMinSE  )
    {
    SVPDEBUG1( "CSVPController::CheckMinSessionExpiresL In" )
    
    TInt count = aHeaders.MdcaCount();
    TUint32 sessionExpires = 0;
    TInt error = KErrNone;
    while ( count-- )  
    	{
        TPtrC8 tmpHeader = aHeaders.MdcaPoint( count );
        if ( KErrNotFound != tmpHeader.Find( KSVPSessionExpires ) )
            {
            // "Session-Expires:" found
            SVPDEBUG1( "CSVPController::CheckMinSEL: 'Session-Expires:' found" )
            
            TInt offset = tmpHeader.FindF( KSVPRefresher );
            if ( KErrNotFound != offset )
                {
                tmpHeader.Set( tmpHeader.Left( offset ) );
                }
            
            TPtrC8 header;
            header.Set( tmpHeader );
            if ( header.FindF( KSVPCln ) )
                {
                // 5 digits should be enought for Min-SE
                // For example: "Min-SE: 1200" 
                TLex8 expires( header.Right( 5 ) ); 
                TInt digits = 0;
                while ( !expires.Eos() )
                    {
                    if ( expires.Peek().IsDigit() )
                        {
                        digits++; 
                        }
                    expires.Inc();
                    }  
                TLex8 expiresVal( header.Right( digits ) );
                error = expiresVal.Val( sessionExpires, EDecimal );
                SVPDEBUG2(" CSVPController::CheckMinSEL sesExp: %d", sessionExpires )

                if ( error || aMinSE > sessionExpires  )  
                    {
                    SVPDEBUG2(" CSVPController::CheckMinSEL reject, err: %d", error )
                    CDesC8ArrayFlat* minSEHeader = 
                        new ( ELeave ) CDesC8ArrayFlat( 1 );
                    CleanupStack::PushL( minSEHeader ); // CS:0
                        
                    HBufC8* header = HBufC8::NewL( 
                                 KSVPMinExpiresLenght + digits  );
                    CleanupStack::PushL( header );  // CS:1
                    
                    header->Des().Copy( KSVPMinSessionExpires );
                    header->Des().AppendNum( aMinSE );
                    minSEHeader[ 0 ].AppendL( *header );
                    CleanupStack::PopAndDestroy( header ); // CS:1
                    
                    aSession.RejectL( KSVPIntervalTooSmallReason,
              		 			      KSVPSessionIntervalTooSmallVal,
                                      minSEHeader );
                    
                    // ownership to Mce
                    CleanupStack::Pop( minSEHeader ); // CS:0
                    User::Leave( KSVPErrWrongMinSE );
                    }
                }
            }
        }
    
    SVPDEBUG1( "CSVPController::CheckMinSessionExpiresL Out" )
    }

// ---------------------------------------------------------------------------
// CSVPController::FetchExpiresTime
// ---------------------------------------------------------------------------
//
void CSVPController::FetchExpiresTime( TUint32& aExpiresTime, 
                                       CDesC8Array& aHeaders ) const
    {
    SVPDEBUG1( "CSVPController::FetchExpiresTime In" )
    
    TBool expiresTaken = EFalse;
    for ( TInt i = 0; i < aHeaders.MdcaCount() && !expiresTaken; i++ )
        {
        TPtrC8 tmpHeader = aHeaders.MdcaPoint( i );
        if ( KErrNotFound != tmpHeader.Find( KSVPExpiresHeader ) &&
             KErrNotFound == tmpHeader.Find( KSVPSessionExpires ) )
            {
            // "Expires:" found
            SVPDEBUG1(" CSVPController::FetchExpiresTimer: 'Expires:' found")

            TInt colonMark = tmpHeader.FindF( KSVPCln );
           
            if ( colonMark )
                {
                
                TLex8 expires( tmpHeader.Right( 5 ) ); // three digits max
                TInt digits = 0;
                while ( !expires.Eos() )
                    {
                    if ( expires.Peek().IsDigit() )
                        {
                        digits++; 
                        }
                    expires.Inc();
                    }
                TLex8 expiresVal( tmpHeader.Right( digits ) );
                TInt error = expiresVal.Val( aExpiresTime, EDecimal );

                if ( error )
                    {
                    SVPDEBUG2(" CSVPController::FetchExpiresTimer, set default value\
                     Val error: %d", error );
                    // if error occurred, set default time
                    aExpiresTime = KSVPDefaultExpiresTime;
                    expiresTaken = ETrue;
                    } 
                else if ( KSVPDefaultExpiresTime < aExpiresTime )
                    {
                    aExpiresTime = KSVPDefaultExpiresTime;
                    expiresTaken = ETrue;
                    }
                else
                    {
                    SVPDEBUG2(" CSVPController::FetchExpiresTime: %d", aExpiresTime )
                    expiresTaken = ETrue;
                    }
                }                 
            }
        }
    
    SVPDEBUG1( "CSVPController::FetchExpiresTime Out" )
    }

// ---------------------------------------------------------------------------
// CSVPController::IncomingSessionHandlerL
// ---------------------------------------------------------------------------
//
void CSVPController::IncomingSessionHandlerL( CMceInSession* aSession,
			                    	TMceTransactionDataContainer* aContainer )
    {
    SVPDEBUG1( "CSVPController::IncomingSessionHandlerL In" )
    
    __ASSERT_ALWAYS( aSession, User::Leave( KErrArgument ) );
    __ASSERT_ALWAYS( aContainer, User::Leave( KErrArgument ) );

    CDesC8Array* headers = aContainer->GetHeaders();
    __ASSERT_ALWAYS( headers, User::Leave( KErrArgument ) );
    CleanupStack::PushL( headers ); // CS:1

    IsTransferTargetCaseL( headers );
    
    // check if Require header contains precondition and that Supported header
    // includes option tag 100rel, providing support for PRACK's
    iPreconditions = IsPreconditionRequired( *headers );

    // Default expiration time (120 s) is set due IOP issue,
	// otherwise incoming call would continue eternity in some cases.
    TUint32 expireTime = KSVPDefaultExpiresTime;
    // check if expires header is present and update expiration time
    FetchExpiresTime( expireTime, *headers );
    
    // mt session temp
    CSVPMtSession* mtSessionTemp = NULL;
    
    CRCSEProfileRegistry* reg = CRCSEProfileRegistry::NewLC();      // CS:2
    
    RPointerArray< CRCSEProfileEntry > voipProfiles;
    CleanupResetAndDestroy< RPointerArray< CRCSEProfileEntry > >::PushL(
        voipProfiles );                                             // CS:3
    
    reg->FindBySIPProfileIdL( aSession->Profile(), voipProfiles );
    
    // Take first profile from the array
    const TUint32 voipProfileId = voipProfiles[ 0 ]->iId;
    const TUint32 minSE = voipProfiles[ 0 ]->iSIPMinSE;
    
    CheckMinSessionExpiresL( *aSession, *headers, minSE );
     
    SVPDEBUG2("CSVPController::IncomingSessionHandlerL VoIP profile id: %d",
        voipProfileId );
             
    // Take first profile from the array
    const TUint32 serviceId = voipProfiles[ 0 ]->iServiceProviderId;
    
    SVPDEBUG2("CSVPController::IncomingSessionHandlerL Service id: %d",
        serviceId );
    
    // create SIP and ProfileRegistry for profile handling
    CSIP* sip = CSIP::NewLC( KSVPUid, *this );      // CS: 4
    SVPDEBUG1( "CSVPController::IncomingSessionHandlerL sip CREATED" );
    
    CSIPProfileRegistry* sipProfileRegistry = 
        CSIPProfileRegistry::NewLC( *sip, *this );  // CS: 5
    
    SVPDEBUG1( "CSVPController::IncomingSessionHandlerL\
    profile registry CREATED" );
    
    // retrieve SIP profile by using sip profile id, note ownership transfer
    CSIPProfile* profile = sipProfileRegistry->ProfileL( aSession->Profile() );
    
    // Get keep-alive timer value
    TUint32 iapId = 0;
    TBool found = EFalse;
    TInt keepAliveValue;
    
    profile->GetParameter( KSIPAccessPointId, iapId );

    TRAPD( errKeepAlive, 
        found = iSVPUtility->GetKeepAliveByIapIdL( iapId, keepAliveValue ) );
    
    SVPDEBUG3( "CSVPController::IncomingSessionHandlerL\
                GetKeepAliveByIapIdL: errKeepAlive = %d found = %d",
                errKeepAlive, found );
     
    if ( !found )
        {
        const TDesC8* aor;
        profile->GetParameter( KSIPUserAor, aor );
        TRAP( errKeepAlive, found = 
            iSVPUtility->GetKeepAliveByAORL( *aor, keepAliveValue ) );
        
        SVPDEBUG3( "CSVPController::IncomingSessionHandlerL\
                    GetKeepAliveByAORL: errKeepAlive = %d found = %d",
                    errKeepAlive, found );
        }
    
    delete profile;
    CleanupStack::PopAndDestroy( 2, sip );          // CS:3
    
    CMceRtpSource* rtpSource = NULL;
    
    // modify codecs and codec settings if streams found
    if ( aSession->Streams().Count() )
        {
      	const RPointerArray<CMceMediaStream>& streams = aSession->Streams();
      	
        SVPDEBUG2("CSVPController::IncomingSessionHandlerL Streamcount: %d",
            streams.Count() );
        
        // disable rtp source and speaker sink so that audio is not on
        // before session is up signalling-wise
        for ( TInt i = 0; i < streams.Count(); i++ )
            {
            SVPDEBUG1( "CSVPController::IncomingSessionHandlerL disabling" );
            
            CMceMediaStream* stream1 = streams[i];
            
            // if stream is not audio stream -> remove 
            if ( KMceAudio != stream1->Type() )
                {
                aSession->RemoveStreamL( *streams[ i ] );
                }
            else
                {
                // search for RTP source
                if ( stream1->Source() && 
                     KMceRTPSource == stream1->Source()->Type() )
                    {
                    SVPDEBUG1(
                        "CSVPController::IncomingSessionHandlerL\
                         RTPSource found" );
                    
                	rtpSource = static_cast<CMceRtpSource*>(
                	    stream1->Source() );
                    }
            
                SVPAudioUtility::DisableSpeakerSinkL( stream1->Sinks() );
                SVPAudioUtility::DisableMicSourceL( *stream1 );
                }
            }
            
        // sets MMF priorities and sets codec specific settings
        CheckStreamsL( *voipProfiles[ 0 ], *aSession, keepAliveValue );
        if ( rtpSource )
            {
            iSVPUtility->UpdateJitterBufferSizeL( *rtpSource );
            }
        
        // create SVP Mt session    
        mtSessionTemp = CSVPMtSession::NewL( aSession,
                                             iContainer,
                                             serviceId,
                                             voipProfileId, 
                                             *this, 
                                             *iSVPUtility,
                                             *iRtpObserver,
                                             keepAliveValue,
                                             iPreconditions );
        
        CleanupStack::PushL( mtSessionTemp );
        FinalizeSessionCreationL( mtSessionTemp );
        CleanupStack::Pop( mtSessionTemp );
        }
    else
        {
        SVPDEBUG1( "CSVPController::IncomingSessionHandlerL NO streams!" );
        
        // no streams found, handle streams like in Mo-case  
        mtSessionTemp = CSVPMtSession::NewL( aSession,
                                             iContainer,
                                             serviceId,
                                             voipProfileId, 
                                             *this, 
                                             *iSVPUtility,
                                             *iRtpObserver,
                                             keepAliveValue,
                                             iPreconditions );
        
        CleanupStack::PushL( mtSessionTemp );
        // construct audio streams 
        mtSessionTemp->ConstructAudioStreamsL();
        FinalizeSessionCreationL( mtSessionTemp );
        CleanupStack::Pop( mtSessionTemp );
        }
    
    CleanupStack::PopAndDestroy( 3, headers );                          // CS:0
    
    // set expires timer
    if ( expireTime )
        {
        SVPDEBUG2( "CSVPController::IncomingSessionHandlerL expireTime: %i s.", expireTime );
        mtSessionTemp->StartTimerL( KSVPMilliSecondCoefficient * expireTime,
                                    KSVPExpiresTimeExpired );
        }
    
    SVPDEBUG1( "CSVPController::IncomingSessionHandlerL Updating call" );
    
    aSession->UpdateL();
    
    SVPDEBUG2( "CSVPController::IncomingSessionHandlerL, Call state after update: %i",
        aSession->State() );
    
    if ( CMceSession::EProceeding == aSession->State() )
        {
        const TInt ind = FindSVPSession( *aSession );
        iCCPMonitor->IncomingCall( iSessionArray[ ind ] );
        
        SVPDEBUG1( "CSVPController::IncomingSessionHandlerL - EProceeding" );
        
        iSessionArray[ ind ]->SessionStateChanged( KErrNone );
        }
    
    // reset flag
    iPreconditions = EFalse;
    
    SVPDEBUG1( "CSVPController::IncomingSessionHandlerL Out" )
    }
   
// ---------------------------------------------------------------------------
// CSVPController::CheckStreamsL
// ---------------------------------------------------------------------------
//    
void CSVPController::CheckStreamsL( CRCSEProfileEntry& aVoipProfile, 
                                    CMceSession& aInSession,
                                    TInt aKeepAliveValue,
                                    TBool aSessionUpdateOngoing)
    {
    SVPDEBUG1( "CSVPController::CheckStreamsL In" )
    
    __ASSERT_ALWAYS( &aInSession, User::Leave( KErrArgument ) );

    const RPointerArray< CMceMediaStream >& streamArray = 
        aInSession.Streams();
    
    const TInt streamCount( streamArray.Count() );
    
    SVPDEBUG3("CSVPController::CheckStreamsL aVoIPProfileId: %u streamCount: %d ",
            aVoipProfile.iId, streamCount );
    
    // Set codec settings, this also removes unneeded codecs.
    for ( TInt i = 0; i < streamCount; i++ )
        {
        iSVPUtility->SetAudioCodecsMTL( aVoipProfile, 
                                        *streamArray[ i ], 
                                        aKeepAliveValue,
                                        aSessionUpdateOngoing );

        // Set the priorities for the remaining codecs. This will handle also the
        // DTMF priorites once correct codecs are set in the streams. Also note
        // the downlink/uplink (in/out) discrimination.

        if ( KMceAudio == streamArray[i]->Type() && 
            streamArray[i]->BoundStream() )
            {
            SVPDEBUG2( "CSVPController::CheckStreamsL round: %d start", i )
            
            CMceAudioStream* stream = static_cast<CMceAudioStream*>( streamArray[i] );
            
            TBool dtmfMode = EFalse;
            if ( SVPAudioUtility::IsDownlinkStream( *stream ) )
                {
                dtmfMode = SVPAudioUtility::SetPriorityCodecValuesL( *stream,
                    static_cast<CMceAudioStream&>( stream->BoundStreamL() ) );
                }
            else
                {
                dtmfMode = SVPAudioUtility::SetPriorityCodecValuesL(
                    static_cast<CMceAudioStream&>( stream->BoundStreamL() ),
                        *stream );
                }
            
            iSVPUtility->SetDtmfMode( dtmfMode );
            
            SVPDEBUG2( "CSVPController::CheckStreamsL round: %d done", i )
            }
        }
    
    SVPDEBUG1( "CSVPController::CheckStreamsL Out" )
    }

// -----------------------------------------------------------------------------
// CSVPController::GetCallIdFromUserHeadersL
// Extract call id from user headers
// -----------------------------------------------------------------------------
//
TBool CSVPController::GetCallIdFromUserHeadersL(
    const CDesC8Array& aUserHeaders,
    TDes8& aCallId )
    {
    // All variables is used for parser.
    TInt tmpMark1 = 0;
    TInt tmpMark2 = 0;
    
    TBool found = EFalse;

    // Find replaces header descriptor from userheaders array.
    for ( TInt m = 0; &aUserHeaders && m < aUserHeaders.Count() && !found; m++ )
        {
        TPtrC8 tmpHeader = aUserHeaders[m];
        if ( KErrNotFound != tmpHeader.FindF( KSVPReplacesColonTxt ) )
            {
            SVPDEBUG1("  CSVPController::\
            GetCallIdFromUserHeadersL: Replaces: found:");

            // variables used for parse tags from replaces header.
            tmpMark1 = tmpHeader.Locate( KSVPColonMark );
            tmpMark2 = tmpHeader.Locate( KSVPSemiColonMark );
            if ( KErrNotFound == tmpMark1 ||
                 KErrNotFound == tmpMark2 )
                {
                SVPDEBUG3("CSVPController::GetCallIdFromUserHeadersL leave \
                      tmpMark1: %d, tmpMark2: %d", tmpMark1, tmpMark2 )
                User::Leave( KErrArgument );
                }

            // Call id is between tmpMark1 and tmpMark2 and there is a space
            // between Replaces: -text and callid
            if ( ( tmpHeader.Mid( tmpMark1+2, tmpMark2 - tmpMark1 - 2 ).Length() < KSVPTempStringlength ) )
                {
                aCallId.Copy( 
                    tmpHeader.Mid( tmpMark1+2, tmpMark2 - tmpMark1 - 2 ) );

                found = ETrue;                
                }
            else
                {
                SVPDEBUG1("  CSVPController::\
                GetCallIdFromUserHeadersL: Replaces: Too long, Leave");
                User::Leave( KErrArgument );
                }
            }
        }
    
    SVPDEBUG2( "CSVPController::GetCallIdFromUserHeadersL return %d", found )
    return found;
    }

// -----------------------------------------------------------------------------
// CSVPController::IsTransferTargetCaseL
// -----------------------------------------------------------------------------
//
void CSVPController::IsTransferTargetCaseL( CDesC8Array* aHeaders )
    {
    SVPDEBUG1( "CSVPController::IsTransferTargetCaseL In" )
    
    iHoldCallIndex = KErrNotFound;
    TBool found = EFalse;
    
    if ( aHeaders && aHeaders->Count() )
        {
        // Variable to contain callID from replaces header.
        TBuf8< KSVPTempStringlength > repCallId( KNullDesC8 );

        // This is transfer session if Replaces: text is found from userheader
        if ( GetCallIdFromUserHeadersL( *aHeaders, repCallId ) )
            {
            SVPDEBUG1( "  CSVPController::IsTransferTargetCaseL: \
                (attended) Transfer target case" )
            
            if ( repCallId.Length() != 0 )
                {
                // Find from SVP session array a session including same CallId
                // that parsed from replace header.
                for ( TInt v = 0; v < iSessionArray.Count() && !found; v++ )
                    {
                    SVPDEBUG2( "  CSVPController::IsTransferTargetCaseL v=%d", v )
                    
                    TBuf8< KSVPTempStringlength > holdCallId( KNullDesC8 );     
                    TDesC8* callid2 = iSessionArray[v]->CallId();
                    
                    if ( callid2 )
                        {
                        TInt index = callid2->Find( KSVPCallId_replaces );
                        holdCallId.Append( 
                        callid2->Mid( index + KSVPCallId_replaces().Length() ) );

                        SVPDEBUG2( "CSVPController::IsTransferTargetCaseL: SesState: %d",
                                     iSessionArray[v]->State() )
                        
                        if ( repCallId == holdCallId && 
                             MCCPCallObserver::ECCPStateDisconnecting != 
                                iSessionArray[v]->State() )
                            {
                            iHoldCallIndex = v;
                            }
                        }
                    }
                
                if ( KErrNotFound == iHoldCallIndex )
                    {
                    SVPDEBUG1("CSVPController::IsTransferTargetCaseL: \
                        sessions or CallIds does not match, leave !!!" ) 
                    User::Leave( KErrNotSupported );
                    }
                }
            else
                {
                SVPDEBUG1("CSVPController::IsTransferTargetCaseL: \
                    Transfer session but CallId wasn't found, leave !!!" )
                User::Leave( KErrNotSupported );
                }
            }
        else
            {
		    TSupplementaryServicesEvent restrictEvent = 
		    	iSuppServices->CheckRestrictionsL( *aHeaders );

			if ( ESVPSSDoNotDisturb == restrictEvent )
				{
				User::Leave( KSVPErrDnDRejection );
				}
			if ( ESVPSSAnonymousBarring == restrictEvent )
				{
				User::Leave( KSVPErrAnonymousCallRejection );
				}
            SVPDEBUG1("  CSVPController::IsTransferTargetCaseL: \
                Normal incoming case");
            }
        }

    SVPDEBUG2( "CSVPController::IsTransferTargetCaseL Out iHoldCallIndex: %d",
                iHoldCallIndex )
    }

// --------------------------------------------------------------------------
// CSVPController::IsPreconditionRequired
// Check if Require header contains precondition and that Supported header
// includes option tag 100rel, providing support for PRACK's
// --------------------------------------------------------------------------
TBool CSVPController::IsPreconditionRequired( CDesC8Array& aHeaders )
    {
    SVPDEBUG1( "CSVPController::IsPreconditionRequired In" )
        
    TInt count = aHeaders.MdcaCount();
    TBool require = EFalse;
    TBool supported = EFalse;
    while ( count-- )  
        {
        TPtrC8 tmpHeader = aHeaders.MdcaPoint( count );
        if ( KErrNotFound != tmpHeader.Find( KSVPRequire ) && !require )
            {
            // "Require:" found
            SVPDEBUG1( "CSVPController::IsPreconditionRequired 'Require-header' found" )
            
            if ( tmpHeader.FindF( KSVPPrecondition ) )
                {
                // "precondition" found in Require header field
                SVPDEBUG1( "CSVPController::IsPreconditionRequired 'precondition' found" )
                require = ETrue;
                }
            }
        else if ( KErrNotFound != tmpHeader.Find( KSVPSupported ) && !supported )
            {
            // "Supported:" found
            SVPDEBUG1( "CSVPController::IsPreconditionRequired 'Supported-header' found" )
            
            if ( tmpHeader.FindF( KSVP100rel ) )
                {
                // "100rel" found in Supported header field
                SVPDEBUG1( "CSVPController::IsPreconditionRequired '100rel' found" )
                supported = ETrue;
                }
            }
        }
    if ( require && supported )
        {
        SVPDEBUG1( "CSVPController::IsPreconditionRequired return ETrue" )
        return ETrue;
        }        
    SVPDEBUG1( "CSVPController::IsPreconditionRequired return EFalse" )
    return EFalse;
    }
	
// ---------------------------------------------------------------------------
// CSVPController::CheckHeadersData
// Check and store data (FromHeader, ToHeader, CallId) from the headers
// to the SessionBase.
// ---------------------------------------------------------------------------
//    
void CSVPController::CheckHeadersData( CSVPSessionBase* aSVPSession, 
                            TMceTransactionDataContainer* aContainer )
    {
    SVPDEBUG1( "CSVPController::CheckHeadersData In" )
    
    TRAPD( stringErr, SIPStrings::OpenL() );

    if ( KErrNone == stringErr )
        {
        // get headers
        CDesC8Array* headers = aContainer->GetHeaders();
	    RStringF fromHdr = SIPStrings::StringF(SipStrConsts::EFromHeader);
	    RStringF fromCompHdr = SIPStrings::StringF(SipStrConsts::EFromHeaderCompact);

	    RStringF toHdr = SIPStrings::StringF(SipStrConsts::EToHeader);
	    RStringF toCompHdr = SIPStrings::StringF(SipStrConsts::EToHeaderCompact);

	    RStringF callidHdr = SIPStrings::StringF(SipStrConsts::ECallIDHeader);
	    RStringF callidCompHdr = SIPStrings::StringF(SipStrConsts::ECallIDHeaderCompact);
	    
	    RStringF cSeqHdr = SIPStrings::StringF(SipStrConsts::ECSeqHeader);

        if ( headers )
            {
            TBool fromFound( EFalse );
            TBool toFound( EFalse );
            TBool callIdFound( EFalse );
            TBool cSeqFound( EFalse );

            for( TInt i = 0;i < headers->MdcaCount();i++ )
                {
                TPtrC8 tmpHeader = headers->MdcaPoint( i );
                TInt colonMark = tmpHeader.FindF( KSVPCln );
                
                SVPDEBUG2( "CSVPController::CheckHeadersData colonMark=%d", colonMark )
                
                TInt fromHdrPos = tmpHeader.FindF( fromHdr.DesC() );
                TInt fromCompHdrPos = tmpHeader.FindF( fromCompHdr.DesC() );

                TInt toHdrPos = tmpHeader.FindF( toHdr.DesC() );
                TInt toCompHdrPos = tmpHeader.FindF( toCompHdr.DesC() );

                TInt callidHdrPos = tmpHeader.FindF( callidHdr.DesC() );
                TInt callidCompHdrPos = tmpHeader.FindF( callidCompHdr.DesC() );
                
                TInt cSeqHdrPos = tmpHeader.FindF( cSeqHdr.DesC() );

                // Header name is separeted by colonmark from header body.
                // There might be space between name and colonmark.

                if ( !fromFound &&
                     ( (KErrNotFound < fromHdrPos && fromHdrPos < colonMark) ||
                       (KErrNotFound < fromCompHdrPos && fromCompHdrPos < colonMark ) ) )
                    {
                    fromFound = ETrue;
                    aSVPSession->SetFromHeader( headers->MdcaPoint( i ) );
                    }
                else if ( !toFound &&
                        ( (KErrNotFound < toHdrPos && toHdrPos < colonMark) ||
                          (KErrNotFound < toCompHdrPos && toCompHdrPos < colonMark ) ) )
                    {
                    toFound = ETrue;
                    aSVPSession->SetToHeader( headers->MdcaPoint( i ) );
                    }
                else if ( !callIdFound &&
                        ( (KErrNotFound < callidHdrPos && callidHdrPos < colonMark) ||
                          (KErrNotFound < callidCompHdrPos && callidCompHdrPos < colonMark ) ) )
                    {
                    callIdFound = ETrue;
                    aSVPSession->SetCallId( headers->MdcaPoint( i ) );
                    }
                else if ( !cSeqFound && 
                          KErrNotFound < cSeqHdrPos && cSeqHdrPos < colonMark )
                    {
                    cSeqFound = ETrue;
                    aSVPSession->SetCSeqHeader( headers->MdcaPoint( i ) );
                    }
                }
            }
        
        SIPStrings::Close();
        delete headers;
        headers = NULL;
        }
    else
        {
        SVPDEBUG2( "CSVPController::CheckHeadersData stringErr=%d", stringErr )
        }
    
    SVPDEBUG1( "CSVPController::CheckHeadersData Out" )
    }

// ---------------------------------------------------------------------------
// CSVPController::CheckContactData
// Check and store contact data from the headers to the MoSession
// ---------------------------------------------------------------------------
//    
TInt CSVPController::CheckContactData( CSVPSessionBase* aSVPSession, 
                            TMceTransactionDataContainer* aContainer )
    {
    SVPDEBUG1( "CSVPController::CheckContactData In" )
    
    TInt count( 0 );
    TRAPD( stringErr, SIPStrings::OpenL() );

    if ( KErrNone == stringErr && aSVPSession->IsMobileOriginated() )
        {
        CDesC8Array* headers = aContainer->GetHeaders();

        if ( headers )
            {
    	    RStringF contactHdr = SIPStrings::StringF(SipStrConsts::EContactHeader);
    	    RStringF contactHdrComp = SIPStrings::StringF(SipStrConsts::EContactHeaderCompact);

            CSVPMoSession* moSession = static_cast< CSVPMoSession* >( aSVPSession );
            moSession->ResetForwardAddressChoices();

            TInt ret( 0 );
            for( TInt i( 0 ); i < headers->MdcaCount(); i++ )
                {
                TPtrC8 tmpHeader = headers->MdcaPoint( i );
                TInt colonMark = tmpHeader.FindF( KSVPCln );
                TPtrC8 left = tmpHeader.Left( colonMark ); // string until to first colon mark

                if ( left.CompareF( contactHdr.DesC() ) == KErrNone ||
                     left.CompareF( contactHdrComp.DesC() ) == KErrNone ) 
                    {
                    SVPDEBUG1( "CSVPController::CheckContactData - Contact header found" )

                    // Get the remaining part of the contact header string and
                    // send it to mo session for parsing 
                    TPtrC8 right = tmpHeader.Right( 
                        tmpHeader.Length() - left.Length() - 1 );

                    TRAPD( addErr, ret = moSession->AddForwardAddressL( right ) );
                    if (KErrNone == addErr)
                        {
                        count = count + ret;
                        }
                    else
                        {
                        SVPDEBUG2( "CSVPController::CheckContactData: addErr = %d", addErr )
                        }
                    }
                }
            }
        else
            {
            SVPDEBUG1( "CSVPController::CheckContactData No headers" )
            }

        SIPStrings::Close();
        delete headers;
        headers = NULL;
        }
    else
        {
        SVPDEBUG2( "CSVPController::CheckContactData stringErr=%d", stringErr )
        }
    
    SVPDEBUG2( "CSVPController::CheckContactData Out return=%d", count )
    return count;
    }

// ---------------------------------------------------------------------------
// CSVPController::IncomingUpdate
// ---------------------------------------------------------------------------
//			
void CSVPController::IncomingUpdate(
				CMceSession& aOrigSession, 
				CMceInSession* aUpdatedSession,
				TMceTransactionDataContainer* aContainer )
    {
    SVPDEBUG1( "CSVPController::IncomingUpdate In" )
    
    iContainer = *aContainer;
    const TInt sessionIndex = FindSVPSession( aOrigSession );
    
    if ( KErrNotFound != sessionIndex )
        {
        SVPDEBUG1( "CSVPController::IncomingUpdate - Session found" )
        
        // handle RE-INVITE without SDP
        if ( !aUpdatedSession->Streams().Count() )
            {
            SVPDEBUG1( "CSVPController::IncomingUpdate No streams present" )
            
            // this call sets iEmptyReInvite flag to ETrue for session, 
            // flag prevents "ghots session" to be seen on UI 
            // when empty Re-Invite is received
            iSessionArray[ sessionIndex ]->SetEmptyReInvite();
                               
            
            #ifdef _DEBUG
            
            TRAPD( noSdpErr, IncomingUpdateNoSdpHandlerL( sessionIndex,
                                                          aUpdatedSession ) );
            
            SVPDEBUG2( "CSVPController::IncomingUpdate trapped: %d", noSdpErr )
            
            #else   // _UREL
            
            TRAP_IGNORE( IncomingUpdateNoSdpHandlerL(
                sessionIndex, aUpdatedSession ) )
            
            #endif  // _DEBUG
            }
        
        else
            {
            SVPDEBUG1( "CSVPController::IncomingUpdate - Normal case" )
            IncomingNormalUpdate( sessionIndex,
                    aOrigSession, aUpdatedSession );
            }
        
        SVPDEBUG1( "CSVPController::IncomingUpdate - Handled" )
        }
    else if ( iEmergencySession && 
              aUpdatedSession && 
              aUpdatedSession->Streams().Count() )
        {
        // Handle emergency hold
        TRAPD( error, iEmergencySession->IncomingRequestL(
                aUpdatedSession, iContainer ) );
        
        if ( error )
            {
            SVPDEBUG2( "CSVPController::IncomingUpdate, emergency error=%d",
                    error )
            }
        }
    
    SVPDEBUG1( "CSVPController::IncomingUpdate Out" )
    }

// ---------------------------------------------------------------------------
// CSVPController::IncomingNormalUpdate
// ---------------------------------------------------------------------------
//
void CSVPController::IncomingNormalUpdate( TInt aSessionIndex,
                                           CMceSession& aOrigSession,
                                           CMceInSession* aUpdatedSession )
    {
    SVPDEBUG1( "CSVPController::IncomingNormalUpdate In" )
    
    TInt err = iSessionArray[ aSessionIndex ]->IncomingRequest(
        *aUpdatedSession );
    
    if ( KErrSVPHoldNotHoldRequest == err )
        {
        SVPDEBUG1( "CSVPController::IncomingNormalUpdate - Not Hold/Resume" )
        
        if ( iSessionArray[ aSessionIndex ]->HasHoldController() &&
             KSVPHoldConnectedStateIndex != iSessionArray[ aSessionIndex ]->
                 HoldController().HoldState() )
            {
            // Hold is active; must update MCE streams state 
            // correspondingly
            TRAP( err, iSessionArray[ aSessionIndex ]->HoldController().
                            RefreshHoldStateL() );
            SVPDEBUG2( "CSVPController::IncomingNormalUpdate - Err: %d", err )
            }
        
        TRAP( err, UpdateSessionL( aOrigSession, *aUpdatedSession ) );
        
        SVPDEBUG2( "CSVPController::IncomingNormalUpdate - Updated err: %d",
            err )
        }
    else if ( iDtmfStringSending )
        {
        SVPDEBUG1( "CSVPController::IncomingNormalUpdate - Dtmf sending will be stopped" )
        // send stop event to the previous character in string 
        // Default tone char
        TChar dtmfToneChar( '0' );

        iSessionArray[ aSessionIndex ]->DtmfObserver().HandleDTMFEvent( 
                MCCPDTMFObserver::ECCPDtmfSequenceStop, 
                KErrNone, 
                dtmfToneChar );

        // send sequence stop event
        iSessionArray[ aSessionIndex ]->DtmfObserver().HandleDTMFEvent( 
             MCCPDTMFObserver::ECCPDtmfStringSendingCompleted, 
             KErrNone, 
             dtmfToneChar );

        // sequence complete, clear flags
        iDtmfStringSending = EFalse;
        iFirstDtmfSent = EFalse;
        
        SVPDEBUG1( "CSVPController::IncomingNormalUpdate - Dtmf sending stopped" )
        }
    else
        {
        SVPDEBUG2( "CSVPController::IncomingNormalUpdate - IncomingRequest err: %d", err )
        }
    
    iSessionArray[ aSessionIndex ]->SetUpdatedSession( aUpdatedSession );
    
    SVPDEBUG1( "CSVPController::IncomingNormalUpdate Out" )
    }

// ---------------------------------------------------------------------------
// CSVPController::IncomingUpdateNoSdpHandlerL
// ---------------------------------------------------------------------------
//
void CSVPController::IncomingUpdateNoSdpHandlerL( TInt aSessionIndex,
        CMceInSession* aUpdatedSession )
    {
    SVPDEBUG1( "CSVPController::IncomingUpdateNoSdpHandlerL In" )
    if ( iRtpObserver )
        {
        iRtpObserver->ResetSessionInObserving( iSessionArray[ aSessionIndex ] );
		}
    // set updated session to SVP, ownership is transferred
    // old session is obsolete
    iSessionArray[ aSessionIndex ]->SetUpdatedSession( aUpdatedSession );
    
    // construct audio streams again, adding all the supported codecs
    iSessionArray[ aSessionIndex ]->ConstructAudioStreamsL();
    
    // finally update the session
    aUpdatedSession->UpdateL();
    
    SVPDEBUG1( "CSVPController::IncomingUpdateNoSdpHandlerL Out" )
    }

// ---------------------------------------------------------------------------
// CSVPController::UpdateSessionL
// ---------------------------------------------------------------------------
//			
void CSVPController::UpdateSessionL( CMceSession& aOrigSession,
                                     CMceInSession& aUpdatedSession )
    {
    SVPDEBUG1( "CSVPController::UpdateSessionL In" )
    
    __ASSERT_ALWAYS( &aOrigSession, User::Leave( KErrArgument ) );
    __ASSERT_ALWAYS( &aUpdatedSession, User::Leave( KErrArgument ) );
    
    CRCSEProfileRegistry* reg = CRCSEProfileRegistry::NewLC();
    RPointerArray< CRCSEProfileEntry > voipProfiles;
    CleanupResetAndDestroy< RPointerArray< CRCSEProfileEntry > >::PushL(
        voipProfiles );
    
    reg->FindBySIPProfileIdL( aOrigSession.Profile(), voipProfiles );
    
    TInt keepAliveTime = 0;
    TInt index = FindSVPSession( aOrigSession );
    if ( KErrNotFound != index )
        {
        keepAliveTime = iSessionArray[ index ]->GetKeepAliveTime();
        __ASSERT_ALWAYS( iRtpObserver, User::Leave( KErrTotalLossOfPrecision ) );
        iRtpObserver->ResetSessionInObserving( iSessionArray[ index ] );
        }

    // flag prevents wrong handling after SessionStateChanged -callback
    iSessionUpdateOngoing = ETrue;

    // sets MMF priorities and sets codec specific settings
    CheckStreamsL( *voipProfiles[ 0 ], aUpdatedSession, keepAliveTime,
                   iSessionUpdateOngoing );
    CleanupStack::PopAndDestroy( 2, reg );
    
    SVPDEBUG1( "CSVPController::UpdateSessionL - Checked" )
    
    aUpdatedSession.UpdateL();
    SVPDEBUG1( "CSVPController::UpdateSessionL Out" )
    }

// ---------------------------------------------------------------------------
// CSVPController::IncomingRefer
// ---------------------------------------------------------------------------
//			
void CSVPController::IncomingRefer( CMceInRefer* aRefer,
        const TDesC8& aReferTo, TMceTransactionDataContainer* aContainer )
    {
    SVPDEBUG1( "CSVPController::IncomingRefer In" )
    
    // IncomingReferHandlerL is used to handle possible leave 
    SVPDEBUG1( "CSVPController::IncomingRefer IncomingReferHandlerL" )
    TRAPD( err, IncomingReferHandlerL( aRefer, aReferTo, aContainer ) );
    
    if ( err )
        {
        SVPDEBUG2("CSVPController::IncomingRefer: err: %d", err )
        if ( err == KSVPErrTransferInProgress )
            {
            SVPDEBUG1( "CSVPController::IncomingRefer - transfer in progress \
                -> ignore" )
            }
        else
            {
            // TRAP is used because of RejectL might leave.
            SVPDEBUG1( "CSVPController::IncomingRefer -> reject" )
            TRAP( err, aRefer->RejectL() );
        
            if ( err )
                {
                SVPDEBUG2("CSspController::IncomingRefer: RejectL err: \
                    %d", err )
                }
            }
        }
    
    SVPDEBUG1( "CSVPController::IncomingRefer Out" )
    }

// ---------------------------------------------------------------------------
// CSVPController::IncomingReferHandlerL
// ---------------------------------------------------------------------------
//			
void CSVPController::IncomingReferHandlerL( CMceInRefer* aRefer,
        const TDesC8& aReferTo, TMceTransactionDataContainer* aContainer )
    {
    SVPDEBUG1( "CSVPController::IncomingReferHandlerL In" )
    
    __ASSERT_ALWAYS( aRefer, User::Leave( KErrArgument ) );
    
    iContainer = *aContainer;
    TInt sessionIndex = FindSVPSession( *aRefer->AssociatedSession() );
    
    if ( KErrNotFound != sessionIndex )    
        {
        SVPDEBUG2( "CSVPController::InRefHL: AssoSes OK,ind=%d",
                sessionIndex )
        
        iSessionArray[ sessionIndex ]->IncomingReferL( 
                aRefer, aReferTo, aContainer );
        
        if ( iSessionArray[ sessionIndex ]->IsAttended() )
            {
            SVPDEBUG1( "CSVPController::InRefHL: - Attended case" )
            }
        else
            {
            SVPDEBUG1( "CSVPController::InRefHL: - Unattended case" )
            // Create a new session
            CreateNewTransferSessionL( sessionIndex, EFalse );
            }
        }
    else
        {
        SVPDEBUG1( "CSVPController::InRefHL: Session not Found!!" )
        User::Leave( KErrNotFound );
        }
    
    SVPDEBUG1( "CSVPController::IncomingReferHandlerL Out" ) 
    }

// ---------------------------------------------------------------------------
// CSVPController::CreateNewTransferSessionL
// ---------------------------------------------------------------------------
//
void CSVPController::CreateNewTransferSessionL( TInt aSessionIndex,
        TBool aAttended )
    {
    SVPDEBUG1( "CSVPController::CreateNewTransferSessionL In" )

    iIncomingReferCallIndex = aSessionIndex;
    // Create a new session
    // fetch SIP profile ID from VoIP profiles
    RPointerArray< CRCSEProfileEntry > entryArray;
    CleanupResetAndDestroy<
        RPointerArray<CRCSEProfileEntry> >::PushL( entryArray ); //CS: 1
    
    CRCSEProfileRegistry* reg = CRCSEProfileRegistry::NewLC(); //CS:2

    const CCCPCallParameters& callParams = iSessionArray[ aSessionIndex ]->Parameters();
    SVPDEBUG2(" CSVPController::CNTSL: iServiceId = %d", callParams.ServiceId() )

    // Get VoIP profile by service id
    reg->FindByServiceIdL( callParams.ServiceId(), entryArray );
    // Take first entry from array
    CRCSEProfileEntry* entry = NULL;
    if (entryArray.Count() > 0)
        {
        entry = entryArray[0];
        }
    else
        {
        User::Leave(KErrNotFound);
        }
    // array for provisioned data
    CDesC8ArrayFlat* userAgentHeaders = new( ELeave )CDesC8ArrayFlat( 4 );
    CleanupStack::PushL( userAgentHeaders );
    
    // variable for storing security status
    TUint32 securityStatus = 0;
    
    // set provisioning data
    SVPDEBUG1( "CSVPController::CNTSL: Set provisioning data..." )
    TRAP_IGNORE( iSVPUtility->SetProvisioningDataL( 
                                *entry, 
                                *userAgentHeaders,
                                securityStatus,
                                iTerminalType,
                                iWlanMacAddress ) );
    
    // only one sip profile per voip profile
    TInt sipProfileId = entry->iIds[ 0 ].iProfileId; 
    SVPDEBUG2( "CSVPController::CNTSL: sipProfileId=%d", sipProfileId )
    
    if ( KSVPStatusNonSecure != securityStatus )
        {
        // If preferred sec is 1 or 2, we check also secure mechanism of sip profile.
        // create SIP and ProfileRegistry for URI handling
        CSIP* sip = CSIP::NewL( KSVPUid, *this );
        CleanupStack::PushL( sip );
        SVPDEBUG1( "CSVPController::CNTSL: sip CREATED" )
        
        CSIPProfileRegistry* sipProfileRegistry = 
            CSIPProfileRegistry::NewL( *sip, *this );
        CleanupStack::PushL( sipProfileRegistry );

        SVPDEBUG1( "CSVPController::CNTSL: sipProfileRegistry CREATED" )
        
        // retrieve SIP profile by using sip profile id
        CSIPProfile* profile = sipProfileRegistry->ProfileL( sipProfileId );
        CleanupStack::PushL( profile );

        // set secure status to 0 if no security mechanism found from SIP profile
        iSVPUtility->ResolveSecurityMechanismL( *profile, securityStatus );

        CleanupStack::PopAndDestroy( 3, sip );  // profile, sipProfileRegistry, sip
        }
    
    // set transfer data
    SVPDEBUG1( "CSVPController::CNTSL: Set transfer data..." )
    iSessionArray[ aSessionIndex ]->SetTransferDataL(
            userAgentHeaders, securityStatus );
    
    SVPDEBUG3( "CSVPController::CNTSL: Header count:%d, sec status:%d",
            userAgentHeaders->Count(), securityStatus )
    
    CSVPMoSession* moSessionTemp = NULL;
    
    // In attended and unattended transfer case recipient is solved here
    SVPDEBUG1( "CSVPController::CNTSL: (Un)Attended, create new mo session" )
    const TDesC& referTo = iSessionArray[ aSessionIndex ]->TransferTarget();
    // "convert" recpient to 8-bit format
    HBufC8* recipient = HBufC8::NewLC( referTo.Length() );
    recipient->Des().Copy( referTo );
    
    moSessionTemp = CSVPMoSession::NewL( *iMceManager,
                                         *recipient,
                                         *entry, 
                                         callParams,
                                         iContainer,
                                         *this,
                                         *iSVPUtility,
                                         *iRtpObserver,
                                         securityStatus,
                                         userAgentHeaders );
    
    CleanupStack::PopAndDestroy( recipient );
    CleanupStack::PushL( moSessionTemp );
    
    // dtmf and rtp observervers are set
    moSessionTemp->SetDtmfObserver( iSessionArray[ aSessionIndex ]->DtmfObserver() );
    iRtpObserver->AddSessionForObservingL( moSessionTemp );
    
    // created SVP session is appended to session array
    iSessionArray.AppendL( moSessionTemp );
    CleanupStack::Pop( moSessionTemp );
    CleanupStack::Pop( userAgentHeaders );
    CleanupStack::PopAndDestroy( 2, &entryArray );
    
    // set CCP session observer to SVP session
    SVPDEBUG1( "CSVPController::CNTSL: AddObserverL" )
    moSessionTemp->AddObserverL( iSessionArray[ aSessionIndex ]->GetCCPSessionObserver() );
    
    //set CCP supplementary services events observer to SVP session
    SVPDEBUG1( "CSVPController::CNTSL: AddSsObserverL" )
    moSessionTemp->SetSsObserver( iSessionArray[ aSessionIndex ]->GetSsObserver() );
        
    SVPDEBUG1( "CSVPController::CNTSL: callcreated, send to CCP" )
    ExecCbCallCreated( moSessionTemp, iSessionArray[ aSessionIndex ], aAttended );
    
    SVPDEBUG1( "CSVPController::CreateNewTransferSessionL Out" )
    }

// ---------------------------------------------------------------------------
// CSVPController::HandleForwardEvent
// ---------------------------------------------------------------------------
// 
void CSVPController::HandleCallForward( TInt aStatusCode,
        TInt aSessionIndex, TMceTransactionDataContainer* aContainer )
    {
    SVPDEBUG1( "CSVPController::HandleForwardEvent In" )
    SVPDEBUG2( "CSVPController::HandleForwardEvent aStatusCode=%d", aStatusCode )
    
    if ( !iSessionArray[ aSessionIndex ]->IsMobileOriginated() )
        {
        iSessionArray[ aSessionIndex ]->
            GetCCPSessionObserver().ErrorOccurred( ECCPErrorNotReached, 
                                              iSessionArray[ aSessionIndex ] );
        }
    else
        {
        CSVPMoSession* session = static_cast< CSVPMoSession* >(
                iSessionArray[ aSessionIndex ] );
        
        switch ( aStatusCode )
            {
            case KSVPMultipleChoicesVal:    // 300
            case KSVPMovedPermanentlyVal:   // 301
                {
                // Get contact headers and notify
                TInt count = CheckContactData( session, aContainer );

                if ( 0 < count )
                    {
                    session->NotifyForwardEvent( aStatusCode );
                    }
                else
                    {
                    session->GetCCPSessionObserver().ErrorOccurred( 
                                                        ECCPErrorNotReached,
                                                        session );
                    }
                break;
                }
            case KSVPMovedTemporarilyVal:   // 302
                {
                // Just notify, this call forward is handled automatically by mce
                session->NotifyForwardEvent( aStatusCode );
                session->GetCCPSessionObserver().
                    CallStateChanged( MCCPCallObserver::ECCPStateForwarding,
                                      session );
                break;
                }
            default:
                {
                SVPDEBUG1( "CSVPController::HandleForwardEvent: unknown code" )
                session->GetCCPSessionObserver().ErrorOccurred( 
                                                    ECCPErrorNotReached,
                                                    session );
                }
            }
        }
    
    SVPDEBUG1( "CSVPController::HandleForwardEvent Out" )
    }

// ---------------------------------------------------------------------------
// CSVPController::StreamStateChanged
// ---------------------------------------------------------------------------
//			
void CSVPController::StreamStateChanged( CMceMediaStream& aStream )
    {
    SVPDEBUG1("CSVPController::StreamStateChanged In" )
    
    if ( &aStream )
        {
        if ( !iEmergencySession )
            {
            const TInt index = FindSVPSession( *aStream.Session() );
            if ( KErrNotFound != index )
                {
                iSessionArray[ index ]->HandleStreamStateChange( aStream );
                }
                
            SVPDEBUG2( "CSVPController::StreamStateChanged index: %d", index )
            }
        else // Emergency session
            {
            iEmergencySession->StreamStateChanged( aStream );
            }
        }
    else
        {
        SVPDEBUG1( "CSVPController::StreamStateChanged, faulty arguments" )
        }
    
    SVPDEBUG1("CSVPController::StreamStateChanged Out" )
    }

// ---------------------------------------------------------------------------
// CSVPController::StreamStateChanged
// ---------------------------------------------------------------------------
//
void CSVPController::StreamStateChanged( CMceMediaStream& aStream,
    CMceMediaSink& aSink )
    {
    SVPDEBUG1( "CSVPController::StreamStateChanged SINK In" )
    
    if ( &aStream && &aSink )
        {
        SVPDEBUG2( "CSVPController::StreamStateChanged SINK Stream State: %d", aStream.State() )
        SVPDEBUG2( "CSVPController::StreamStateChanged SINK Sink IsEnabled: %d", aSink.IsEnabled() )
        
        if ( !iEmergencySession )
            {
            const TInt index = FindSVPSession( *aStream.Session() );
            
            if ( KErrNotFound != index )
                {
                iSessionArray[ index ]->HandleStreamStateChange( aStream, aSink );
                }
            
            SVPDEBUG2( "CSVPController::StreamStateChanged SINK index: %d", index )
            }
        else // Emergency session
            {
            iEmergencySession->StreamStateChanged( aStream );
            }
        }
    else
        {
        SVPDEBUG1( "CSVPController::StreamStateChanged SINK, faulty arguments" )
        }
    
    SVPDEBUG1( "CSVPController::StreamStateChanged SINK Out" )
    }

// ---------------------------------------------------------------------------
// CSVPController::StreamStateChanged
// ---------------------------------------------------------------------------
//
void CSVPController::StreamStateChanged( CMceMediaStream& aStream,
    CMceMediaSource& aSource )
    {
    SVPDEBUG1( "CSVPController::StreamStateChanged SOURCE In" )
    
    if ( &aStream && &aSource )
        {
        SVPDEBUG2( "CSVPController::StreamStateChanged SOURCE Stream State: %d", aStream.State() )
        SVPDEBUG2( "CSVPController::StreamStateChanged SOURCE Source IsEnabled: %d", aSource.IsEnabled() )

        if ( !iEmergencySession )
            {
            const TInt index = FindSVPSession( *aStream.Session() );           
            if ( KErrNotFound != index )
                {
                iSessionArray[ index ]->HandleStreamStateChange( aStream, aSource );
                }
            
            SVPDEBUG2( "CSVPController::StreamStateChanged SOURCE index: %d", index )
            }
        else // Emergency session
            {
            iEmergencySession->StreamStateChanged( aStream );
            }        
        
        }
    
    SVPDEBUG1( "CSVPController::StreamStateChanged SOURCE Out" )
    }


// from ConvergedCallProvider
// ---------------------------------------------------------------------------
// CSVPController::NewEmergencyCallL
// ---------------------------------------------------------------------------
//
MCCPEmergencyCall* CSVPController::NewEmergencyCallL( 
    const TUint32 /*aServiceId*/,
    const TDesC&  aAddress, 
    const MCCPCallObserver& aObserver )                                            
    {
    SVPDEBUG1( "CSVPController::NewEmergencyCallL In" )
    
    // Retrieve available VoIP and IAP IDs
    if ( 0 == iEmergencyProfileIds.Count() && 0 == iEmergencyIapIds.Count() )
        {
        CRCSEProfileRegistry* reg = CRCSEProfileRegistry::NewLC(); // CS:1
        CSIP* sip = CSIP::NewLC( KSVPUid, *this ); // CS:2
        CSIPProfileRegistry* sipProfileRegistry = CSIPProfileRegistry::NewLC( 
            *sip, *this ); // CS:3
        
        // Get all VoIP profile IDs into an array
        RArray< TUint32 > voipProfileIds;
        CleanupClosePushL( voipProfileIds );    //CS: 4
        reg->GetAllIdsL( voipProfileIds );
    
        // Sort the VoIP profile array so that registered profiles are first
        for ( TInt i = 0; i < voipProfileIds.Count(); i++ )
            {
            TBool registered( EFalse );
            CRCSEProfileEntry* entry = CRCSEProfileEntry::NewLC(); // CS:5
            reg->FindL( voipProfileIds[i], *entry );
            // There is only one (or zero) SIP profile per VoIP profile.
            // If profileId is not found,
            // CSVPEmergencySession::ConstructL will handle the error
            if ( 0 < entry->iIds.Count() )
                {
                SVPDEBUG2( "CSVPController::NewEmergencyCallL, SIP Id count:%d",
                        entry->iIds.Count() )
                CSIPProfile* sipProfile = sipProfileRegistry->ProfileL( 
                    entry->iIds[0].iProfileId );
                sipProfile->GetParameter( KSIPProfileRegistered, registered );
                delete sipProfile;
                }
            CleanupStack::PopAndDestroy( entry ); // CS:4
            if ( registered )
                {
                // Move registered VoIP profile IDs to the front
                iEmergencyProfileIds.Insert( voipProfileIds[i], 0 );
                }
            else
                {
                iEmergencyProfileIds.Append( voipProfileIds[i] );
                }
            }
        
        CleanupStack::PopAndDestroy( 4, reg ); 
        // CS:0 voipProfileIds, sipProfileRegistry, sip, reg
        
        // Request and wait for IAP IDs
        CSVPEmergencyIapProvider* iapProvider = 
            CSVPEmergencyIapProvider::NewLC( 
            CActive::EPriorityStandard ); // CS:1
        iapProvider->RequestIapIds( iEmergencyIapIds );
        CleanupStack::PopAndDestroy( iapProvider ); // CS:0
        }
    
    SVPDEBUG2("CSVPController::NewEmergencyCallL, VoIP count:%d", 
            iEmergencyProfileIds.Count() )
    SVPDEBUG2("CSVPController::NewEmergencyCallL, IAP count:%d", 
            iEmergencyIapIds.Count() )
    
    // Define last try
    TBool isLastId( EFalse );
    
    if ( ( 0 == iEmergencyProfileIds.Count() && 
           1 == iEmergencyIapIds.Count() ) ||
         ( 1 == iEmergencyProfileIds.Count() && 
           0 == iEmergencyIapIds.Count() )
       )
        {
        isLastId = ETrue;
        SVPDEBUG1("CSVPController::NewEmergencyCallL, last ID");
        }
    
    // Create session
    CSVPEmergencySession* emergencySession = NULL;
    
    if ( iEmergencyProfileIds.Count() )
        {
        // Create emergency session with VoIP ID
        TRAPD( err, emergencySession = CSVPEmergencySession::NewL( 
            *iMceManager,
            iEmergencyProfileIds[0], 
            aAddress,
            aObserver,
            *iSVPUtility, 
            isLastId ) )
    
        if ( err )
            {
            // Create dummy session for session release
            emergencySession = CSVPEmergencySession::NewL( 
                *iMceManager,
                iEmergencyProfileIds[0], 
                aAddress,
                aObserver,
                *iSVPUtility, 
                isLastId, 
                ETrue );   
            }
        
        // Update profile array
        iEmergencyProfileIds.Remove( 0 );
        }
    else if ( iEmergencyIapIds.Count() )
        {
        // Create emergency session with IAP ID
        TRAPD( err, emergencySession = CSVPEmergencySession::NewL( 
            *iMceManager,
            aAddress,
            aObserver,
            *iSVPUtility,
            iEmergencyIapIds[0],  
            isLastId ) )
    
        if ( err )
            {
            // Create dummy session for session release
            emergencySession = CSVPEmergencySession::NewL( 
                *iMceManager,
                aAddress,
                aObserver,
                *iSVPUtility, 
                iEmergencyIapIds[0], 
                isLastId, 
                ETrue );   
            }        
        
        // Update IAP array
        iEmergencyIapIds.Remove( 0 );
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    
    // save emergency session to controller
    iEmergencySession = emergencySession;
    
    if ( iCCPDtmfObserver )
            {
            SVPDEBUG1( "CSVPController::NewEmergencyCallL setting DTMFObserver" )
            iEmergencySession->SetDtmfObserver( *iCCPDtmfObserver );
            }
    
    SVPDEBUG1( "CSVPController::NewEmergencyCallL Out" )
    // return pointer to CCP emergency call object
    return emergencySession;
    }
                        
// ---------------------------------------------------------------------------
// CSVPController::NewConferenceL
// ---------------------------------------------------------------------------
//
MCCPConferenceCall* CSVPController::NewConferenceL( 
        const TUint32 /* aServiceId */, 
        const MCCPConferenceCallObserver& /*aObserver*/ )
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// CSVPController::AcceptTransfer
// ---------------------------------------------------------------------------
//                 
void CSVPController::AcceptTransfer( TBool /*aAccept*/ ) 
    {
    SVPDEBUG1( "CSVPController::AcceptTransfer In" )
    SVPDEBUG1( "CSVPController::AcceptTransfer Out" )
    }

// ---------------------------------------------------------------------------
// CSVPController::ForwardCallToAddressL
// ---------------------------------------------------------------------------
//
TInt CSVPController::ForwardCallToAddressL( const TInt /*aIndex*/ ) 
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// CSVPController::Caps
// ---------------------------------------------------------------------------
//
TUint32 CSVPController::Caps() const
    {
    return 0;
    }

// ---------------------------------------------------------------------------
// CSVPController::DTMFProvider
// ---------------------------------------------------------------------------
//
MCCPDTMFProvider* CSVPController::DTMFProviderL(
    const MCCPDTMFObserver& aObserver )
    {
    SVPDEBUG1( "CSVPController::DTMFProviderL In" )
    
    iCCPDtmfObserver = &aObserver;
    
    TInt sessions = iSessionArray.Count();
    while ( sessions )
        {
        sessions--;
        iSessionArray[ sessions ]->SetDtmfObserver( aObserver );
        
        SVPDEBUG2( "CSVPController::DTMFProviderL sessions: %d", sessions )
        }
    
    if ( iEmergencySession )
        {
        iEmergencySession->SetDtmfObserver( aObserver );
        }
    
    SVPDEBUG1( "CSVPController::DTMFProviderL Out" )
    return this;
    }


// ---------------------------------------------------------------------------
// CSVPController::ExtensionProvider
// ---------------------------------------------------------------------------
//
MCCPExtensionProvider* CSVPController::ExtensionProviderL( 
    const MCCPExtensionObserver& /*aObserver*/ )
    {
    return NULL;
    }
  
// dtmf provider

// ---------------------------------------------------------------------------
// CSVPController::CancelDtmfStringSending
// ---------------------------------------------------------------------------
//
TInt CSVPController::CancelDtmfStringSending()
    {
    SVPDEBUG1( "CSVPController::CancelDtmfStringSending In" )
    
    TInt err( KErrNotSupported ); 
    // find active session
    
    TInt sesCount = iSessionArray.Count();
    CSVPSessionBase* session = NULL;
    while ( sesCount )
        {
        sesCount--;
        session = iSessionArray[ sesCount ];
        
        if ( SVPAudioUtility::DtmfActionCapableSession( *session ) )
            {
            err = session->CancelDtmfStringSending();
            }
        SVPDEBUG3( "CSVPController::CancelDtmfStringSending sesCount: %d, err: %d",
                     sesCount, err )
        session = NULL;
        }
    
    if ( iEmergencySession && 
         SVPAudioUtility::DtmfActionCapableSession( *iEmergencySession ) )
        {
        err = iEmergencySession->CancelDtmfStringSending();
        }
        
    SVPDEBUG2("CSVPController::CancelDtmfStringSending Out return=%d", err )
    iDtmfStringSending = EFalse;
    iFirstDtmfSent = EFalse;
    return err;
    }

// ---------------------------------------------------------------------------
// CSVPController::StartDtmfTone
// ---------------------------------------------------------------------------
//
TInt CSVPController::StartDtmfTone( const TChar aTone )
    {
    SVPDEBUG1( "CSVPController::StartDtmfTone In" )
    
    TInt err( KErrNotSupported );
    
    // Save DTMF tone for later use in outband dtmf start/stop events
    iDtmfTone = aTone;
    // Send the tone to all sessions, but check mute and hold cases where session must
    // be in 'connected' state in order to send DTMF's. Session will then
    // discriminate between inband and outband DTMF's.
    TInt sesCount = iSessionArray.Count();
    CSVPSessionBase* session = NULL;
    while ( sesCount )
        {
        sesCount--;
        session = iSessionArray[ sesCount ];
        // Checking hold and mute status
        if ( SVPAudioUtility::DtmfActionCapableSession( *session ) &&
                !session->IsSessionMuted()  )
            {
            err = session->StartDtmfTone( aTone );
            }
        SVPDEBUG3( "CSVPController::StartDtmfTone sesCount: %d, err: %d",
                      sesCount, err )
        session = NULL;
        }
    
    if ( iEmergencySession && 
         SVPAudioUtility::DtmfActionCapableSession( *iEmergencySession ) )
        {
        err = iEmergencySession->StartDtmfTone( aTone );
        }
    
    SVPDEBUG2("CSVPController::StartDtmfTone Out return=%d", err )
    return err;
    }

// ---------------------------------------------------------------------------
// CSVPController::StopDtmfTone
// ---------------------------------------------------------------------------
//
TInt CSVPController::StopDtmfTone()
    {
    SVPDEBUG1( "CSVPController::StopDtmfTone In" )
    
    TInt err( KErrNotSupported );
    
    TInt sesCount = iSessionArray.Count();
    CSVPSessionBase* session = NULL;
    while ( sesCount )
        {
        sesCount--;
        session = iSessionArray[ sesCount ];
        
        if ( SVPAudioUtility::DtmfActionCapableSession( *session ) )
            {
            err = session->StopDtmfTone();
            }
        SVPDEBUG3( "CSVPController::StopDtmfTone sesCount: %d, err: %d",
                     sesCount, err )
        session = NULL;
        }

    if ( iEmergencySession && 
         SVPAudioUtility::DtmfActionCapableSession( *iEmergencySession ) )
        {
        err = iEmergencySession->StopDtmfTone();
        }

    SVPDEBUG2("CSVPController::StopDtmfTone Out return=%d", err )
    return err;
    }

// ---------------------------------------------------------------------------
// CSVPController::SendDtmfToneString
// ---------------------------------------------------------------------------
//
TInt CSVPController::SendDtmfToneString( const TDesC& aString )
    {
    SVPDEBUG1( "CSVPController::SendDtmfToneString In" )
    
    TInt err( KErrNotSupported );
    
    delete iDtmfString;
    iDtmfString = NULL;
    TRAPD( errBuf, iDtmfString = HBufC::NewL( aString.Length() ) );
    if ( KErrNone != errBuf )
        {
        return errBuf;
        }
    // Take local copy of the dtmf string to be sent
    // This is needed for outband dtmf sequence start/stop event
    *iDtmfString = aString;
    iDtmfStringLex.Assign( *iDtmfString );
    
    TInt sesCount = iSessionArray.Count();
    CSVPSessionBase* session = NULL;

    while ( sesCount )
        {
        sesCount--;
        session = iSessionArray[ sesCount ];
        // Checking hold and mute status
        if ( SVPAudioUtility::DtmfActionCapableSession( *session ) &&
             !session->IsSessionMuted() )
            {
            err = session->SendDtmfToneString( aString );
            if ( KErrNone == err )
                {
                iDtmfStringSending = ETrue;
                iFirstDtmfSent = EFalse;
                }
            }
        SVPDEBUG3( "CSVPController::SendDtmfToneString sesCount: %d, err: %d",
                     sesCount, err )
        session = NULL;
        }
    
    if ( iEmergencySession )
        {
        if ( SVPAudioUtility::DtmfActionCapableSession( *iEmergencySession ) )
            {
            err = iEmergencySession->SendDtmfToneString( aString );
            }
        iDtmfStringSending = ETrue;
        iFirstDtmfSent = EFalse;        
        }

    SVPDEBUG2("CSVPController::SendDtmfToneString Out return=%d", err )
    return err;
    }

// ---------------------------------------------------------------------------
// CSVPController::ContinueDtmfStringSending
// ---------------------------------------------------------------------------
//
TInt CSVPController::ContinueDtmfStringSending( const TBool /*aContinue*/ )
    {
    // SVP sessionbase used to implement this as only returning
    // KErrNotSupported, so why not accept the situation here and just
    // return KErrNotSupported.
    
    SVPDEBUG1( "CSVPController::ContinueDtmfStringSending KErrNotSupported" )
    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// CSVPController::AddObserverL
// ---------------------------------------------------------------------------
//   
void CSVPController::AddObserverL( const MCCPDTMFObserver& /*aObserver*/ )
    {
    SVPDEBUG1( "CSVPController::AddObserverL MCCPDTMFObserver" )
    }

// ---------------------------------------------------------------------------
// CSVPController::RemoveObserver
// ---------------------------------------------------------------------------
//
TInt CSVPController::RemoveObserver( const MCCPDTMFObserver& /*aObserver*/ )
    {
    SVPDEBUG1( "CSVPController::RemoveObserver MCCPDTMFObserver" )
    return KErrNotSupported;
    }

  
// refer observer
  
// ---------------------------------------------------------------------------
// CSVPController::ReferStateChanged
// ---------------------------------------------------------------------------
//
void CSVPController::ReferStateChanged( CMceRefer& aRefer,
        TMceTransactionDataContainer* aContainer )
    {
    SVPDEBUG1( "CSVPController::ReferStateChanged In" )
    
    iContainer = *aContainer;
    TInt ind = KErrNotFound;

    // loop session array and check refer
    for ( TInt s = 0; s < iSessionArray.Count() && 
          KErrNotFound == ind; s++ )
        { 
        if ( iSessionArray[ s ]->IsMceRefer( aRefer ) )
            {
            ind = s;
            } 
        }

    if ( KErrNotFound != ind )
        {
        SVPDEBUG2(" CSVPController::ReferStateChanged ind: %d", ind );
        TInt statusCode = iContainer.GetStatusCode();

        iSessionArray[ ind ]->ReferStateChanged( aRefer, statusCode );
        }
    
    SVPDEBUG1( "CSVPController::ReferStateChanged Out" )
    }

// ---------------------------------------------------------------------------
// CSVPController::ReferConnectionStateChanged
// ---------------------------------------------------------------------------
//
void CSVPController::ReferConnectionStateChanged( CMceRefer& /*aRefer*/,
        TBool /*aActive*/ )
    {
    SVPDEBUG1( "CSVPController::ReferConnectionStateChanged" )
    }

// ---------------------------------------------------------------------------
// CSVPController::Failed
// ---------------------------------------------------------------------------
//
#ifdef _DEBUG
void CSVPController::Failed( CMceRefer& /*aRefer*/, TInt aError )
#else
void CSVPController::Failed( CMceRefer& /*aRefer*/, TInt /*aError*/ )
#endif // _DEBUG
    {
    SVPDEBUG2( "CSVPController::Failed Refer failed with error: %d", aError )
    }


// Mce DMTF observer
// ---------------------------------------------------------------------------
// CSVPController::DtmfToneReceived
// ---------------------------------------------------------------------------
//
void CSVPController::DtmfToneReceived( CMceSession& /*aSession*/,
        CMceAudioStream& /*aStream*/, const TChar& /*aTone*/ )
    {
    // Not supported
    SVPDEBUG1( "CSVPController:: DtmfToneReceived - Not supported" )
    }
    
// ---------------------------------------------------------------------------
// CSVPController::DtmfEventReceived
// ---------------------------------------------------------------------------
//
void CSVPController::DtmfEventReceived( CMceSession& aSession,
                                        CMceAudioStream& /*aStream*/,
                                        CMceMediaSource& /*aSource*/,
                                        TMceDtmfEvent aEvent )
    {
    SVPDEBUG3( "CSVPController::DtmfEventReceived In, aEvent: %d, iDtmfStringSending: %d",
                   aEvent, iDtmfStringSending )
    
    // find what session received the event
    const TInt index = FindSVPSession( aSession );
    
    if ( KErrNotFound != index )
        {
         // match dtmf event
        MCCPDTMFObserver::TCCPDtmfEvent dtmfEvent = 
            iSVPUtility->GetDtmfEvent( aEvent, iDtmfStringSending );
        
        // dtmf string
        if ( iDtmfStringSending ) 
            { 
            // only start event received from mce
            // logic below needed so that stop events can be sent
            if ( MCCPDTMFObserver::ECCPDtmfSequenceStart == dtmfEvent )
                {
                if ( !iFirstDtmfSent )
                    {
                    SVPDEBUG1( "CSVPController::DtmfEventReceived FIRST SEND" )
                    
                    // send start
                    iFirstDtmfSent = ETrue;
                     // call back event to application
                    iSessionArray[ index ]->
                        DtmfObserver().HandleDTMFEvent( dtmfEvent, 
                                                        KErrNone, 
                                                        iDtmfStringLex.Peek() );
                    }  
                else
                    {
                    SVPDEBUG1( "CSVPController::DtmfEventReceived STOP TO PREVIOUS" )
                    
                    // send stop event to the previous character in string 
                    iSessionArray[ index ]->DtmfObserver().HandleDTMFEvent( 
                        MCCPDTMFObserver::ECCPDtmfSequenceStop, 
                        KErrNone, 
                        iDtmfStringLex.Get() );
                     SVPDEBUG1("CSVPController::DtmfEventReceived START TO CURRENT");
                    // send start event to the current character in string
                    iSessionArray[ index ]->DtmfObserver().HandleDTMFEvent( 
                        dtmfEvent, 
                        KErrNone, 
                        iDtmfStringLex.Peek() );
                    }
                }
             else
                {
                SVPDEBUG1( "CSVPController::DtmfEventReceived STOP TO PREVIOUS AND LAST" )
                
                // send stop event to the previous character in string 
                iSessionArray[ index ]->DtmfObserver().HandleDTMFEvent( 
                        MCCPDTMFObserver::ECCPDtmfSequenceStop, 
                        KErrNone, 
                        iDtmfStringLex.Peek() );
                
                SVPDEBUG1( "CSVPController::DtmfEventReceived COMPLETE" )
                
                // send sequence stop event
                iSessionArray[ index ]->DtmfObserver().HandleDTMFEvent( 
                     MCCPDTMFObserver::ECCPDtmfStringSendingCompleted, 
                     KErrNone, 
                     iDtmfStringLex.Peek() );
                // sequence complete, clear flags
                iDtmfStringSending = EFalse;
                iFirstDtmfSent = EFalse;
               
                delete iDtmfString;
                iDtmfString = NULL;
                }
            }
        // manual dtmf
        else
            {
            // call back event to application
            iSessionArray[ index ]->DtmfObserver().HandleDTMFEvent( 
                                            dtmfEvent, 
                                            KErrNone, 
                                            iDtmfTone );
            }
        }
    
    else if ( iEmergencySession )
        {
        const MCCPDTMFObserver& dtmfObs = iEmergencySession->DtmfObserver();
        SVPDEBUG2("CSVPController::DtmfEventReceived, %d = DtmfObserver()", &dtmfObs )
         // match dtmf event
        MCCPDTMFObserver::TCCPDtmfEvent dtmfEvent = 
            iSVPUtility->GetDtmfEvent( aEvent, iDtmfStringSending );
        
        // dtmf string
        if ( iDtmfStringSending && NULL != &dtmfObs ) 
            { 
            // only start event received from mce
            // logic below needed so that stop events can be sent
            if ( MCCPDTMFObserver::ECCPDtmfSequenceStart == dtmfEvent )
                {
                if ( !iFirstDtmfSent )
                    {
                    SVPDEBUG1(
                    "CSVPController::DtmfEventReceived, emergency FIRST SEND")
                    
                    // send start
                    iFirstDtmfSent = ETrue;
                     // call back event to application
                    dtmfObs.HandleDTMFEvent( dtmfEvent, KErrNone, iDtmfStringLex.Peek() );
                    }  
                else
                    {
                    SVPDEBUG1("CSVPController::DtmfEventReceived,\
                     emergency STOP TO PREVIOUS")
                    
                    // send stop event to the previous character in string 
                    dtmfObs.HandleDTMFEvent( 
                        MCCPDTMFObserver::ECCPDtmfSequenceStop, 
                        KErrNone, 
                        iDtmfStringLex.Get() );
                     SVPDEBUG1("CSVPController::DtmfEventReceived,\
                      emergency START TO CURRENT")
                    // send start event to the current character in string
                    dtmfObs.HandleDTMFEvent( 
                        dtmfEvent, 
                        KErrNone, 
                        iDtmfStringLex.Peek() );
                    }
                }
             else
                {
                SVPDEBUG1("CSVPController::DtmfEventReceived,\
                 emergency STOP TO PREVIOUS AND LAST")
                
                // send stop event to the previous character in string 
                dtmfObs.HandleDTMFEvent( 
                    MCCPDTMFObserver::ECCPDtmfSequenceStop, 
                    KErrNone, 
                    iDtmfStringLex.Peek() );
                
                SVPDEBUG1("CSVPController::DtmfEventReceived,\
                 emergency COMPLETE")
                
                // send sequence stop event
                dtmfObs.HandleDTMFEvent( 
                     MCCPDTMFObserver::ECCPDtmfStringSendingCompleted, 
                     KErrNone, 
                     iDtmfStringLex.Peek() );
                // sequence complete, clear flags
                iDtmfStringSending = EFalse;
                iFirstDtmfSent = EFalse;
                
                delete iDtmfString;
                iDtmfString = NULL;
                }
            }
        // manual dtmf
        else if( NULL != &dtmfObs )
            {
            SVPDEBUG1("CSVPController::DtmfEventReceived,\
                    manual dtmf ,  call back event to application")
            dtmfObs.HandleDTMFEvent( dtmfEvent, KErrNone, iDtmfTone );
            }
        else
            {
            SVPDEBUG1("CSVPController::DtmfEventReceived, DtmfObs not set")
            }
        }
    
    SVPDEBUG1( "CSVPController::DtmfEventReceived Out" )
    }
                                  
// ---------------------------------------------------------------------------
// CSVPController::DtmfErrorOccured
// ---------------------------------------------------------------------------
//
 void CSVPController::DtmfErrorOccured( CMceSession& aSession,
         CMceAudioStream& /*aStream*/,CMceMediaSource& /*aSource*/,
         TInt aError )
    {
    SVPDEBUG2( "CSVPController::DtmfErrorOccured In, aError: %d", aError )
    
    // find what session received the event
    const TInt index = FindSVPSession( aSession );
    if ( KErrNotFound != index )
        {
        // match dtmf event, unknown set as default in error case, 'tis ok?
        const MCCPDTMFObserver::TCCPDtmfEvent dtmfEvent = 
            MCCPDTMFObserver::ECCPDtmfUnknown;
        
        // default tone char
        TChar dtmfToneChar('0');
        
        // call back error
        iSessionArray[ index ]->
            DtmfObserver().HandleDTMFEvent( dtmfEvent, 
                                            aError, 
                                            dtmfToneChar );  
        }
    
    SVPDEBUG1( "CSVPController::DtmfErrorOccured Out" )
    }

// ---------------------------------------------------------------------------
// CSVPController::FinalizeSessionCreationL
// ---------------------------------------------------------------------------
//
void CSVPController::FinalizeSessionCreationL( CSVPSessionBase* aSession )
    {
    SVPDEBUG2( "CSVPController::FinalizeSessionCreationL In, aSession: 0x%x",
        aSession )
    
    __ASSERT_ALWAYS( aSession, User::Leave( KErrArgument ) );
    
    if ( iCCPDtmfObserver )
      	{
       	aSession->SetDtmfObserver( *iCCPDtmfObserver );
       	}
    
    iRtpObserver->AddSessionForObservingL( aSession );
    iSessionArray.AppendL( aSession );
    
    SVPDEBUG1( "CSVPController::FinalizeSessionCreationL Out" )
    }

// ---------------------------------------------------------------------------
// CSVPController::ExecCbErrorOccurred
// ---------------------------------------------------------------------------
// 
TInt CSVPController::ExecCbErrorOccurred( MCCPObserver::TCCPError aError )
    {
    SVPDEBUG2( "CSVPController::ExecCbErrorOccurred In, aError=%d", aError )
    
    TInt status = KErrNotFound;
    
    if ( iCCPMonitor )
        {
        status = KErrNone;
        iCCPMonitor->ErrorOccurred( aError );
        }
    
    SVPDEBUG2( "CSVPController::ExecCbErrorOccurred Out return=%d", status )
    return status;
    }

// ---------------------------------------------------------------------------
// CSVPController::IncomingCall
// ---------------------------------------------------------------------------
// 
TInt CSVPController::ExecCbIncomingCall( MCCPCall* aCall )
    {
    SVPDEBUG2( "CSVPController::ExecCbIncomingCall In, aCall= 0x%x", aCall )

    TInt status = KErrNotFound;
    
    if ( iCCPMonitor )
        {
        status = KErrNone;
        iCCPMonitor->IncomingCall( aCall );
        }
    
    SVPDEBUG2( "CSVPController::ExecCbIncomingCall Out return=%d", status )
    return status;
    }

// ---------------------------------------------------------------------------
// CSVPController::ExecCbIncomingCall
// ---------------------------------------------------------------------------
// 
TInt CSVPController::ExecCbIncomingCall( MCCPCall* aCall, MCCPCall& aTempCall )
    {
    SVPDEBUG2( "CSVPController::ExecCbIncomingCall In, aCall= 0x%x", aCall )
    SVPDEBUG2( "CSVPController::ExecCbIncomingCall aTempCall= 0x%x", &aTempCall )
    
    TInt status = KErrNotFound;
    
    if ( iCCPMonitor )
        {
        status = KErrNone;
        iCCPMonitor->IncomingCall( aCall, aTempCall );
        }
    
    SVPDEBUG2( "CSVPController::ExecCbIncomingCall Out return=%d", status )
    return status;
    }

// ---------------------------------------------------------------------------
// CSVPController::ExecCbCallCreated
// ---------------------------------------------------------------------------
// 
TInt CSVPController::ExecCbCallCreated( MCCPCall* aNewTransferCall,
        MCCPCall* aOriginator, TBool aAttended )
    {
    SVPDEBUG2( "CSVPController::ExecCbCallCreated In, aNewTransferCall= 0x%x", aNewTransferCall )
    SVPDEBUG2( "CSVPController::ExecCbCallCreated          aOriginator= 0x%x", aOriginator )
    SVPDEBUG2( "CSVPController::ExecCbCallCreated            aAttended= %d", aAttended )
    
    TInt status = KErrNotFound;
    
    if ( iCCPMonitor )
        {
        status = KErrNone;
        iCCPMonitor->CallCreated( aNewTransferCall, aOriginator, aAttended );
        }
    
    SVPDEBUG2( "CSVPController::ExecCbCallCreated Out return=%d", status )
    return status;
    }

// ---------------------------------------------------------------------------
// CSVPController::ParseRecipientDtmfSuffixL
// ---------------------------------------------------------------------------
// 
HBufC* CSVPController::ParseRecipientDtmfSuffixL( const TDesC& aRecipient ) const
    {
    __ASSERT_ALWAYS( &aRecipient, User::Leave( KErrArgument ) );

    SVPDEBUG2( "CSVPController::ParseRecipientDtmfSuffixL In, aRecipient=%S", &aRecipient )
    
    HBufC* result = aRecipient.AllocLC(); // CS:1
    
    TInt recipientLength = result->Length();
    if ( recipientLength )
        {
        if ( IsValidDtmfRecipientL( *result ) )
            {
            TInt loopCount = 0;
            if (  KErrNotFound != KSVPDtmfTelNumRange().Locate( result->Des()[loopCount] ) )
                {
                loopCount++;
                TBool doLoop = ETrue;
                do
                    {
                    if ( loopCount < recipientLength )
                        {
                        if ( KErrNotFound != 
                            KSVPDtmfDelimiterRange().Locate( result->Des()[loopCount] ) )
                            {
                            TInt suffixLength = recipientLength - loopCount;
                            result->Des().Delete( loopCount, suffixLength );
                            doLoop = EFalse;
                            SVPDEBUG1(
                                 "CSVPController::ParseRecipientDtmfSuffixL, DTMF suffix removed" )
                            }
                        else 
                            {
                            loopCount++;
                            }
                        }
                    else
                        {
                        doLoop = EFalse;
                        }
                    } while ( doLoop );
                }
            }
        }
    SVPDEBUG2( "CSVPController::ParseRecipientDtmfSuffixL Out, result=%S", result )
    CleanupStack::Pop( result ); // CS:0
    return result;
    }

// ---------------------------------------------------------------------------
// CSVPController::IsValidDtmfRecipientL
// ---------------------------------------------------------------------------
// 
TBool CSVPController::IsValidDtmfRecipientL( const TDesC& aRecipient ) const
    {
    __ASSERT_ALWAYS( &aRecipient, User::Leave( KErrArgument ) );

    SVPDEBUG1( "CSVPController::IsValidDtmfRecipientL In" )

    TBool result = ETrue;
    if ( aRecipient.Length() )
        {
        TBool loopDo = ETrue;
        TInt loopCount = 0;
        do
            {
            if ( loopCount < aRecipient.Length() )
                {
                if ( KErrNotFound == KSVPDtmfAllValidChars().Locate( aRecipient[loopCount] ) )
                    {
                    result = EFalse;
                    loopDo = EFalse;
                    }
                else
                    {
                    loopCount++;
                    }
                }
            else
                {
                loopDo = EFalse;
                }
            } while ( loopDo );
        }
    else
        {
        SVPDEBUG1( "CSVPController::IsValidDtmfRecipientL, Invalid recipient length" )
        result = EFalse;
        }
    SVPDEBUG2( "CSVPController::IsValidDtmfRecipientL Out, result=%d" , result )
    return result;
    }

// ---------------------------------------------------------------------------
// CSVPController::CheckCallEventToBeSent
// ---------------------------------------------------------------------------
//
void CSVPController::CheckCallEventToBeSent( CSVPSessionBase* aNewSVPSession, 
                     CSVPSessionBase* aOldSVPSession ) const
    {
    SVPDEBUG1( "CSVPController::CheckCallEventToBeSent In" )

    if ( aOldSVPSession->IsSecured() != aNewSVPSession->IsSecured() )
        {
        // Session secure status changed, need to send proper event
        if ( aNewSVPSession->IsSecured() )
            {
            SVPDEBUG1( "CSVPController::CheckCallEventToBeSent, unsecure -> secure case" )
            aNewSVPSession->SetCallEventToBeSent( MCCPCallObserver::ECCPSecureCall );
            }
        else
            {
            SVPDEBUG1( "CSVPController::CheckCallEventToBeSent, secure -> unsecure case" )
            aNewSVPSession->SetCallEventToBeSent( MCCPCallObserver::ECCPNotSecureCall );
            }
        }
    else
        {
        // remoteparty will be updated anyway after attended transfer
        SVPDEBUG1( "CSVPController::CheckCallEventToBeSent, remoteparty will be updated after attended transfer" )
        aNewSVPSession->SetCallEventToBeSent( MCCPCallObserver::ECCPNotifyRemotePartyInfoChange );
        }
    SVPDEBUG1( "CSVPController::CheckCallEventToBeSent Out" )
    }

// ---------------------------------------------------------------------------
// From class MSIPObserver
// CSVPController::IncomingRequest
// ---------------------------------------------------------------------------
// 
void CSVPController::IncomingRequest( 
    TUint32 /*aIapId*/, CSIPServerTransaction* /*aTransaction*/ )
    {
    SVPDEBUG1( "CSVPController::IncomingRequest" )
    }

// ---------------------------------------------------------------------------
// From class MSIPObserver
// CSVPController::TimedOut
// ---------------------------------------------------------------------------
//     
void CSVPController::TimedOut( 
    CSIPServerTransaction& /*aSIPServerTransaction*/ )
    {
    SVPDEBUG1( "CSVPController::TimedOut" )
    }

// ---------------------------------------------------------------------------
// From class MSIPProfileRegistryObserver
// CSVPController::ProfileRegistryErrorOccurred
// ---------------------------------------------------------------------------
//  
void CSVPController::ProfileRegistryErrorOccurred( 
    TUint32 /*aSIPProfileId*/, TInt /*aError*/ )
    {
    SVPDEBUG1( "CSVPController::ProfileRegistryErrorOccurred" )
    }

// ---------------------------------------------------------------------------
// From class MSIPProfileRegistryObserver
// CSVPController::ProfileRegistryEventOccurred
// ---------------------------------------------------------------------------
//  
void CSVPController::ProfileRegistryEventOccurred( 
    TUint32 /*aProfileId*/, TEvent /*aEvent*/ )
    {
    SVPDEBUG1( "CSVPController::ProfileRegistryEventOccurred" )
    }

