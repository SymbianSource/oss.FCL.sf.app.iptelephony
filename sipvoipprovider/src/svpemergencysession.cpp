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
* Description:  SVP Emergency Sesssion class. Handles emergency 
*                session methods.        
*
*/


#include <sip.h>
#include <sipprofile.h>
#include <sipprofileregistry.h>

#include <mceaudiostream.h>
#include <mcertpsource.h>
#include <mcertpsink.h>
#include <mcemicsource.h>
#include <mcespeakersink.h>
#include <mcetransactiondatacontainer.h>

#include "crcseprofileentry.h"
#include "crcseprofileregistry.h"

#include "svpemergencysession.h"
#include "svpuriparser.h"
#include "svputility.h"
#include "svpsipconsts.h"
#include "svpconsts.h"
#include "svppositioningprovider.h"
#include "svpemergencyconnection.h"
#include "svpholdcontroller.h"
#include "svpaudioutility.h"
#include "svpdtmfeventgenerator.h"

// ---------------------------------------------------------------------------
// CSVPEmergencySession::CSVPEmergencySession
// ---------------------------------------------------------------------------
//
CSVPEmergencySession::CSVPEmergencySession( 
    CSVPUtility& aSVPUtility, 
    CMceManager& aMceManager, 
    TBool aIsLastProfile, 
    TBool aIsDummy )
    : iSVPUtility( aSVPUtility ), 
      iMceManager( aMceManager ), 
      iSessionExpires( KSVPDefaultSessionExpires ), 
      iIsLastProfile( aIsLastProfile ), 
      iIsDummy( aIsDummy ), 
      iIsDialed( EFalse ), 
      iIsLIRequestReady( ETrue )
    {
    }

// ---------------------------------------------------------------------------
// CSVPEmergencySession::ConstructL
// ---------------------------------------------------------------------------
//
void CSVPEmergencySession::ConstructL( TUint32 aVoipProfileId, 
                                       const TDesC& aAddress, 
                                       const MCCPCallObserver& aObserver, 
                                       TBool aIsDummy )                                   
    {
    SVPDEBUG2("CSVPEmergencySession::ConstructL() IN, VoIP profile ID: %d", 
        aVoipProfileId)

    iEmergencyObserver = const_cast< MCCPCallObserver* >( &aObserver );
    iVoipProfileId = ( TInt )aVoipProfileId;

    // Start emergency timer, timeout 17 seconds
    StartTimerL( KSVPEmergencyTimeout, KSVPEmergencyExpired );

    if ( aIsDummy )
        {
        SVPDEBUG1("CSVPEmergencySession::ConstructL, dummy session")
        return;
        }

    // Initialize registries
    CRCSEProfileRegistry* reg = CRCSEProfileRegistry::NewLC(); // CS:1
    CSIP* sip = CSIP::NewLC( KSVPUid, *this ); // CS:2
    CSIPProfileRegistry* sipProfileRegistry = CSIPProfileRegistry::NewLC( 
        *sip, *this ); // CS:3

    // Find VoIP profile by its ID
    CRCSEProfileEntry* voipProfile = CRCSEProfileEntry::NewLC();  // CS:4
    reg->FindL( aVoipProfileId, *voipProfile );

    // Set Session Expires value
    iSessionExpires = ( TUint32 )voipProfile->iSIPSessionExpires;
    
    // There is only one (or zero) SIP profile per VoIP profile.
    // if profileId doesn't exist, report an error ( ECCPErrorGeneral ).
    if ( 0 == voipProfile->iIds.Count() )
        {
        SVPDEBUG1( "CSVPEmergencySession::ConstructL, no SIP profile, return" )
        CleanupStack::PopAndDestroy( 4, reg ); // CS:0
        ErrorOccurred( ECCPErrorGeneral );
        return;
        }
    CSIPProfile* sipProfile = sipProfileRegistry->ProfileL( 
        voipProfile->iIds[0].iProfileId );
    CleanupStack::PushL( sipProfile ); // CS:5

    // Get SIP profile ID
    User::LeaveIfError( 
        sipProfile->GetParameter( KSIPProfileId, iSipProfileId ) );
       
    // Get user AOR
    const TDesC8* userAor = NULL;
    User::LeaveIfError( sipProfile->GetParameter( KSIPUserAor, userAor ) );
    iUserAor = HBufC8::NewL( userAor->Length() );
    ( iUserAor->Des() ).Copy( *userAor );
    
    // Parse SIP URI
    HBufC8* recipient = HBufC8::NewLC( aAddress.Length() ); // CS:6
    ( recipient->Des() ).Copy( aAddress );
    CSVPUriParser* uriParser = CSVPUriParser::NewLC(); // CS:7
    iRecipientUri = uriParser->CompleteSipUriL( *recipient, *iUserAor, ETrue );
    CleanupStack::PopAndDestroy( 2, recipient ); // uriparser, recipient CS:5

    // Get registration status
    TBool registered( EFalse );
    sipProfile->GetParameter( KSIPProfileRegistered, registered );

    // Get SNAP ID
    TUint32 snapId( 0 );
    TInt errSnap = sipProfile->GetParameter( KSIPSnapId, snapId );

    // Define IAP ID
    if ( !registered && KErrNone == errSnap )
        {
        SVPDEBUG2("CSVPEmergencySession::ConstructL, snapId: %d", snapId)
        // Define IAP ID from SNAP ID, create WLAN connection
        iEmergencyConnection = CSVPEmergencyConnection::NewL( 
            CActive::EPriorityStandard, *this );
        iEmergencyConnection->ConnectWithSnapIdL( snapId );
        CleanupStack::PopAndDestroy( 5, reg );
        // reg, sip, sipProfileRegistry, voipProfile, sipProfile CS:0
        SVPDEBUG1("CSVPEmergencySession::ConstructL, return")
        return;
        }
    else
        {
        User::LeaveIfError( 
            sipProfile->GetParameter( KSIPAccessPointId, iIapId ) );
        }

    // Create MCE out session
    if ( registered )
        {
        SVPDEBUG1("CSVPEmergencySession::ConstructL, registered")
        iEmergencySession = CMceOutSession::NewL( 
            iMceManager, *sipProfile, *iRecipientUri );
        ConstructAudioStreamsL();
        }
    else
        {
        CreateUnregistedSessionL();
        }
    
    CleanupStack::PopAndDestroy( 5, reg ); 
    // CS:0 reg, sip, sipProfileRegistry, voipProfile, sipProfile

    // Request for position information
    RequestPosition( iIapId );

    SVPDEBUG1("CSVPEmergencySession::ConstructL() OUT")
    }

// ---------------------------------------------------------------------------
// CSVPEmergencySession::ConstructWithIapIdL
// ---------------------------------------------------------------------------
//
void CSVPEmergencySession::ConstructWithIapIdL( 
    TUint32 aIapId, 
    const TDesC& aAddress, 
    const MCCPCallObserver& aObserver, 
    TBool aIsDummy )                                   
    {
    SVPDEBUG2("CSVPEmergencySession::ConstructWithIapIdL() In, IAP ID: %d", 
        aIapId)

    iEmergencyObserver = const_cast< MCCPCallObserver* >( &aObserver );
    iVoipProfileId = KErrNotFound;
    iIapId = aIapId;
    
    // Copy recipient address
    iAddress = HBufC8::NewL( aAddress.Length() );
    ( iAddress->Des() ).Copy( aAddress );
    
    // Start emergency timer, timeout 17 seconds
    StartTimerL( KSVPEmergencyTimeout, KSVPEmergencyExpired );

    if ( aIsDummy )
        {
        SVPDEBUG1("CSVPEmergencySession::ConstructWithIapIdL, dummy session")
        return;
        }

    // Request for SIP proxy address
    SVPDEBUG1("CSVPEmergencySession::ConstructWithIapIdL, SIP proxy address")
    iEmergencyConnection = CSVPEmergencyConnection::NewL( 
        CActive::EPriorityStandard, *this );
    iEmergencyConnection->ConnectL( iIapId );
    
    SVPDEBUG1("CSVPEmergencySession::ConstructWithIapIdL() Out")
    }

// ---------------------------------------------------------------------------
// CSVPEmergencySession::ContinueConstructWithIapIdL
// ---------------------------------------------------------------------------
//
void CSVPEmergencySession::RequestSipProxyAddressL()
    {
    if ( NULL == iEmergencyConnection )
        {
        User::Leave( KErrGeneral );
        }

    iEmergencyConnection->RequestSipProxyAddressL( iIapId );
    }

// ---------------------------------------------------------------------------
// CSVPEmergencySession::ContinueConstructWithIapIdL
// ---------------------------------------------------------------------------
//
void CSVPEmergencySession::ContinueConstructWithIapIdL( 
    const TDesC16& aSipProxyAddress )
    {
    SVPDEBUG2("CSVPEmergencySession::ContinueConstructWithIapIdL(),\
        SIP proxy address: %S", &aSipProxyAddress)

    if ( 0 == aSipProxyAddress.Length() )
        {
        User::Leave( KErrNotFound );
        }

    // Parse user AOR
    iUserAor = HBufC8::NewL(
        KSVPSipPrefix().Length() + 
        KSVPAnonymous().Length() + 
        KSVPAt().Length() + 
        aSipProxyAddress.Length() );
    TPtr8 userAorPtr = iUserAor->Des();
    userAorPtr.Copy( KSVPSipPrefix );
    userAorPtr.Append( KSVPAnonymous );
    userAorPtr.Append( KSVPAt );
    userAorPtr.Append( aSipProxyAddress );

    // Parse SIP URI
    SVPDEBUG1("CSVPEmergencySession::ContinueConstructWithIapIdL,\
        parse SIP URI")
    CSVPUriParser* uriParser = CSVPUriParser::NewLC(); // CS:1
    iRecipientUri = uriParser->CompleteSipUriL( *iAddress, *iUserAor, ETrue );
    CleanupStack::PopAndDestroy( uriParser ); // CS:0

    // Create MCE session
    CreateUnregistedSessionL();

    // Request for position information
    RequestPosition( iIapId );
    
    SVPDEBUG1("CSVPEmergencySession::ContinueConstructWithIapIdL() Out")
    }

// ---------------------------------------------------------------------------
// CSVPEmergencySession::NewL
// ---------------------------------------------------------------------------
//
CSVPEmergencySession* CSVPEmergencySession::NewL(  
                                        CMceManager& aMceManager, 
                                        TUint32 aVoipProfileId, 
                                        const TDesC& aAddress, 
                                        const MCCPCallObserver& aObserver,
                                        CSVPUtility& aSVPUtility, 
                                        TBool aIsLastProfile, 
                                        TBool aIsDummy )
    {
    CSVPEmergencySession* self = new( ELeave ) CSVPEmergencySession( 
        aSVPUtility, aMceManager, aIsLastProfile, aIsDummy );
    CleanupStack::PushL( self );
    self->ConstructL( aVoipProfileId, aAddress, aObserver, aIsDummy );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CSVPEmergencySession::NewL
// ---------------------------------------------------------------------------
//
CSVPEmergencySession* CSVPEmergencySession::NewL(  
                                        CMceManager& aMceManager, 
                                        const TDesC& aAddress, 
                                        const MCCPCallObserver& aObserver,
                                        CSVPUtility& aSVPUtility, 
                                        TUint32 aIapId, 
                                        TBool aIsLastProfile, 
                                        TBool aIsDummy )
    {
    CSVPEmergencySession* self = new( ELeave ) CSVPEmergencySession( 
        aSVPUtility, aMceManager, aIsLastProfile, aIsDummy );
    CleanupStack::PushL( self );
    self->ConstructWithIapIdL( aIapId, aAddress, aObserver, aIsDummy );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CSVPEmergencySession::~CSVPEmergencySession
// ---------------------------------------------------------------------------
//
CSVPEmergencySession::~CSVPEmergencySession()
    {
    SVPDEBUG1("CSVPEmergencySession::~CSVPEmergencySession() In")

    StopTimers();  
    iTimers.Close();

    delete iEmergencySession;
    delete iFailedEmergencySession;
    
    delete iUserAor;
    delete iAddress;
    delete iRecipientUri;
    
    delete iPositionInformation;
    delete iPositioningProvider;
    
    delete iEmergencyConnection;
    
    delete iHoldController;
    
    delete iDTMFEventGenerator;
    
    delete iDtmfString;

    SVPDEBUG1("CSVPEmergencySession::~CSVPEmergencySession() Out")
    }

// ---------------------------------------------------------------------------
// CSVPEmergencySession::State
// ---------------------------------------------------------------------------
//
MCCPCallObserver::TCCPCallState CSVPEmergencySession::State() const
    {
    SVPDEBUG1("CSVPEmergencySession::State()")

    // ccpState can be safely initialized with StatusIdle
    MCCPCallObserver::TCCPCallState ccpState = 
        MCCPCallObserver::ECCPStateIdle;
    
    switch ( iEmergencySession->State() )
        {
        case CMceSession::EIdle:
            SVPDEBUG1("CSVPEmergencySession::State, MCE EIdle")
            ccpState = MCCPCallObserver::ECCPStateIdle;
            break;
       
        case CMceSession::EProceeding:
            SVPDEBUG1("CSVPEmergencySession::State, MCE EProceeding")
            ccpState = MCCPCallObserver::ECCPStateConnecting;
            break;
            
        case CMceSession::EEstablished:
            SVPDEBUG1("CSVPEmergencySession::State, MCE EEstablished")
            ccpState = MCCPCallObserver::ECCPStateConnected;
            break;
            
        case CMceSession::ECancelling:
            SVPDEBUG1("CSVPEmergencySession::State, MCE ECancelling")
            ccpState = MCCPCallObserver::ECCPStateIdle;  
            break;
            
        case CMceSession::ETerminating:
            SVPDEBUG1("CSVPEmergencySession::State, MCE ETerminating")
            ccpState = MCCPCallObserver::ECCPStateDisconnecting;
            break;
            
        case CMceSession::ETerminated:
            SVPDEBUG1("CSVPEmergencySession::State, MCE ETerminated")
            if ( MCCPCallObserver::ECCPStateDisconnecting == iSessionState )
                {
                ccpState = MCCPCallObserver::ECCPStateDisconnecting;
                }
            else
                {
                ccpState = MCCPCallObserver::ECCPStateIdle;     
                }
            break;
            
        default:
            SVPDEBUG1("CSVPEmergencySession::State, DEFAULT")
            // This block should never be reached.
            break;
        }

    return ccpState;
    }

// ---------------------------------------------------------------------------
// Traps SessionStateChangedL
// ---------------------------------------------------------------------------
//
void CSVPEmergencySession::SessionStateChanged( TInt aStatusCode )
    {
    TRAPD( err, SessionStateChangedL( aStatusCode ) )
    if ( err )
        {
        SVPDEBUG2("CSVPEmergencySession::SessionStateChanged, err: %d", err)
        }
    }

// ---------------------------------------------------------------------------
// Session connection state changed
// ---------------------------------------------------------------------------
//
void CSVPEmergencySession::SessionConnectionStateChanged(
    CMceSession& aSession, TBool aActive )
    {
    SVPDEBUG2("CSVPEmergencySession::SessionConnectionStateChanged - %d", 
        aActive)
    
    if ( iEmergencySession == &aSession && aActive )
        {
        ProceedDial();
        }
    }

// ---------------------------------------------------------------------------
// Stream state changed
// ---------------------------------------------------------------------------
//
void CSVPEmergencySession::StreamStateChanged( CMceMediaStream& aStream )
    {
    SVPDEBUG1("CSVPEmergencySession::StreamStateChanged()")
    
    if ( CMceMediaStream::EDisabled == aStream.State() &&
         CMceSession::EEstablished == aStream.Session()->State() )
        {
        // Remote end died
        iEmergencyObserver->CallStateChanged( 
            MCCPCallObserver::ECCPStateDisconnecting, NULL );
        TRAP_IGNORE( StartTimerL( 
            KSVPTerminatingTime, KSVPRemoteEndDiedExpired ) )
        }
    else if ( CMceMediaStream::EStreaming == aStream.State() )
        {
        SVPDEBUG1("CSVPEmergencySession::StreamStateChanged() - EStreaming")
        StopTimer( KSVPRemoteEndDiedExpired );
        }
    }

// ---------------------------------------------------------------------------
// Handle incoming request, i.e. hold
// ---------------------------------------------------------------------------
//
void CSVPEmergencySession::IncomingRequestL( 
    CMceInSession* aUpdatedSession, TMceTransactionDataContainer aContainer )
    {
    SVPDEBUG1("CSVPEmergencySession::IncomingRequest() In")

    if ( !aUpdatedSession )
        {
        User::Leave( KErrArgument );
        }

    // Create hold controller
    if ( NULL == iHoldController )
        {
        iHoldController = CSVPHoldController::NewL( 
            *iEmergencySession,
            aContainer,
            this,
            ETrue );
        }

    iEmergencySession = aUpdatedSession;

    User::LeaveIfError( iHoldController->IncomingRequest( aUpdatedSession ) );
    }

// ---------------------------------------------------------------------------
// Returns ETrue, if hold controller exists
// ---------------------------------------------------------------------------
//
TBool CSVPEmergencySession::HasHoldController() const
    {
    if ( iHoldController )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// ---------------------------------------------------------------------------
// Returns reference to hold controller
// ---------------------------------------------------------------------------
//    
CSVPHoldController& CSVPEmergencySession::HoldController() const
    {
    return *iHoldController;
    }

// ---------------------------------------------------------------------------
// Handles hold session state changes
// ---------------------------------------------------------------------------
//
void CSVPEmergencySession::HoldSessionStateChangedL( CMceSession& aSession )
    {
    SVPDEBUG1("CSVPEmergencySession::HoldSessionStateChangedL() In")
    
    if ( NULL == iHoldController )
        {
        User::Leave( KErrNotFound );
        }

    if ( iHoldController->HoldInProgress() )
        {
        SVPDEBUG1(
            "CSVPEmergencySession::HoldSessionStateChangedL, in progress")
        StopTimers();
        if ( iHoldController->ContinueHoldProcessing( aSession ) )
            {
            iEmergencyObserver->ErrorOccurred( ECCPLocalHoldFail, NULL );
            }
        }

    SVPDEBUG1("CSVPEmergencySession::HoldSessionStateChangedL() Out")
    }

// ---------------------------------------------------------------------------
// Returns DTMF observer
// ---------------------------------------------------------------------------
//
const MCCPDTMFObserver& CSVPEmergencySession::DtmfObserver()
    {
    return *iDtmfObserver;
    }

// ---------------------------------------------------------------------------
// Sets DTMF observer
// ---------------------------------------------------------------------------
//
void CSVPEmergencySession::SetDtmfObserver( 
    const MCCPDTMFObserver& aObserver )
    {
    SVPDEBUG1("CSVPEmergencySession::SetDtmfObserver()")
    
    iDtmfObserver = const_cast<MCCPDTMFObserver*>( &aObserver );
    }

// ---------------------------------------------------------------------------
// Starts DTMF tone
// ---------------------------------------------------------------------------
//
TInt CSVPEmergencySession::StartDtmfTone( const TChar aTone )
    {
    SVPDEBUG1("CSVPEmergencySession::StartDtmfTone() In")
    
    TInt dtmfErr( KErrNone );
    if ( iSVPUtility.GetDTMFMode() )
        {
        const RPointerArray<CMceMediaStream>& streams = 
            iEmergencySession->Streams();
        TInt count = streams.Count();
        while ( count )
            {
            count--;
            CMceMediaStream& mediaStream = *streams[ count ];
            if ( SVPAudioUtility::DtmfActionCapableStream( mediaStream ) )
                {
                TRAP( dtmfErr, mediaStream.Source()->StartDtmfToneL( aTone ) )
                }
            else
                {
                dtmfErr = KErrNotSupported;
                }
            
            if ( KErrNone != dtmfErr )
                {
                SVPDEBUG2("CSVPEmergencySession::StartDtmfToneL dtmfErr: %d",
                    dtmfErr)
                return dtmfErr;
                }
            }
        }
    else
        {
        iDtmfTone = aTone;
        DtmfObserver().HandleDTMFEvent( MCCPDTMFObserver::ECCPDtmfManualStart, 
                                        KErrNone, 
                                        aTone );
        }
    SVPDEBUG1("CSVPEmergencySession::StartDtmfTone() Out")
    return dtmfErr;
    }

// ---------------------------------------------------------------------------
// Stops DTMF tone
// ---------------------------------------------------------------------------
//
TInt CSVPEmergencySession::StopDtmfTone()
    {
    SVPDEBUG1("CSVPEmergencySession::StopDtmfTone() In")
    
    TInt dtmfErr( KErrNone );
    if ( iSVPUtility.GetDTMFMode() )
        {
        const RPointerArray<CMceMediaStream>& streams = 
            iEmergencySession->Streams();
        TInt count = streams.Count();
        while( count )
            {
            count--;
            CMceMediaStream& mediaStream = *streams[ count ];
            if ( SVPAudioUtility::DtmfActionCapableStream( mediaStream ) )
                {
                TRAP( dtmfErr, mediaStream.Source()->StopDtmfToneL() )
                }
            // NOP with inband.
            
            if ( KErrNone != dtmfErr )
                {
                SVPDEBUG2("CSVPEmergencySession::StopDtmfTone dtmfErr: %d",
                    dtmfErr)
                
                return dtmfErr;
                }
            }
        }
    else
        {
        DtmfObserver().HandleDTMFEvent( MCCPDTMFObserver::ECCPDtmfManualStop, 
                                        KErrNone, 
                                        iDtmfTone );
        }
    SVPDEBUG1("CSVPEmergencySession::StopDtmfTone() Out")
    return dtmfErr;
    }

// ---------------------------------------------------------------------------
// Sends DTMF tone string
// ---------------------------------------------------------------------------
//
TInt CSVPEmergencySession::SendDtmfToneString( const TDesC& aString )
    {
    SVPDEBUG1("CSVPEmergencySession::SendDtmfToneString() In")
    
    TInt error( KErrNone );

    TChar dtmfPause('p');
    // MCE calls if outband DTMF.
    // Exception is character 'p' which is handled always locally
    if ( !iSVPUtility.GetDTMFMode() ||
        ( aString.Length() == 1 &&
          dtmfPause == aString[0] ) )   
        {
        delete iDtmfString;
        iDtmfString = NULL;
        TRAP( error, iDtmfString = HBufC::NewL( aString.Length() ) );
        if ( KErrNone != error )
            {
            return error;
            }
                       
        *iDtmfString = aString;
        iDtmfLex.Assign( *iDtmfString );
        
        SVPDEBUG1("CSVPEmergencySession::SendDtmfToneString, inband")
        if ( !iDTMFEventGenerator )
            {
            TRAP( error,
            iDTMFEventGenerator = CSVPDTMFEventGenerator::NewL( *this ) );
            }
        
        if ( KErrNone != error )
            {
            return error;    
            }
        
        // Dtmf pause interval is 2.5s
        TBool pauseChar = ( aString.Length() == 1 && 
                            dtmfPause == aString[0] ); 
        // start events
        iDTMFEventGenerator->StartDtmfEvents( aString.Length(), pauseChar );
        }
    else
        {
        SVPDEBUG1("CSVPEmergencySession::SendDtmfToneString, outband")
        
        const RPointerArray<CMceMediaStream>& streams = 
            iEmergencySession->Streams();
        TInt count = streams.Count();
        while( count && KErrNone == error )
            {
            count--;
            CMceMediaStream& mediaStream = *streams[ count ];
            if ( SVPAudioUtility::DtmfActionCapableStream( mediaStream ) )
                {
                TRAP( error, mediaStream.Source()->SendDtmfToneSequenceL( 
                    aString ) )
                }
            else
                {
                error = KErrNotSupported;
                }
            }
        }

    SVPDEBUG2("CSVPEmergencySession::SendDtmfToneString, error: %d", error)
    return error;
    }

// ---------------------------------------------------------------------------
// Cancels DTMF tone string sending
// ---------------------------------------------------------------------------
//
TInt CSVPEmergencySession::CancelDtmfStringSending()
    {
    SVPDEBUG1("CSVPEmergencySession::CancelDtmfStringSending() In")
    
    TInt error( KErrNone );
    if ( !iSVPUtility.GetDTMFMode() ) 
        {
        error = KErrNotFound;
        if ( iDTMFEventGenerator )
            {
            iDTMFEventGenerator->StopEvents();
            error = KErrNone;
            }
        }
    else 
        {
        SVPDEBUG1("CSVPEmergencySession::CancelDtmfStringSending, outband")
        error = KErrNotSupported;
        }

    SVPDEBUG2("CSVPEmergencySession::CancelDtmfStringSending, error: %d", 
        error)
    return error;
    }

// ---------------------------------------------------------------------------
// CSVPEmergencySession::Dial
// ---------------------------------------------------------------------------
//
TInt CSVPEmergencySession::Dial( const TDesC& /*aRecipient*/ )
    {
    SVPDEBUG1("CSVPEmergencySession::Dial()")

    if ( iIsDummy )
        {
        SVPDEBUG1("CSVPEmergencySession::Dial, dummy session")
        ErrorOccurred( ECCPErrorGeneral );
        return KErrNone;
        }

    iIsDialed = ETrue;
    ProceedDial();

    iEmergencyObserver->CallStateChanged( 
        MCCPCallObserver::ECCPStateDialling, NULL );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CSVPEmergencySession::Cancel
// ---------------------------------------------------------------------------
//
TInt CSVPEmergencySession::Cancel()
    {
    SVPDEBUG1("CSVPEmergencySession::Cancel()")
    
    TInt error( KErrNone );    
    if ( iEmergencySession )
        {
        TRAP( error, ( ( CMceOutSession* )iEmergencySession )->CancelL() )
        if ( error )
            {
            SVPDEBUG2("CSVPEmergencySession::Cancel, error: %d", error)
            iEmergencyObserver->CallStateChanged( 
                MCCPCallObserver::ECCPStateIdle, NULL );
            }
        }
    else
        {
        iEmergencyObserver->CallStateChanged( 
            MCCPCallObserver::ECCPStateIdle, NULL );
        }
    
    return error;
    }

// ---------------------------------------------------------------------------
// CSVPEmergencySession::HangUp
// ---------------------------------------------------------------------------
//
TInt CSVPEmergencySession::HangUp()
    {
    SVPDEBUG1("CSVPEmergencySession::HangUp()")

    if ( NULL == iEmergencySession )
        {
        iEmergencyObserver->CallStateChanged( 
            MCCPCallObserver::ECCPStateIdle, NULL );
        return KErrNone;
        }
    
    CMceSession::TState mceState = iEmergencySession->State();
    SVPDEBUG2("CSVPEmergencySession::HangUp, MCE state: %d", mceState)
    
    TInt err( KErrNone );

    if ( CMceSession::EEstablished == mceState || 
         CMceSession::EOffering == mceState )
        {
        if ( CMceSession::EEstablished == mceState )
            {
            TRAP( err, iEmergencySession->TerminateL() )
            }
        else // CMceSession::EOffering
            {
            err = Cancel();
            }
        
        if ( !err )
            {
            iEmergencyObserver->CallStateChanged( 
                MCCPCallObserver::ECCPStateDisconnecting, NULL );
            TRAP_IGNORE( StartTimerL( 
                KSVPMoHangupTerminatingTime, KSVPHangUpTimerExpired ) )
            }   
        }    
    else // Wrong MCE state
        {
        SVPDEBUG2("CSVPEmergencySession::HangUp, wrong state: %d ", mceState )
        StopTimers();
        iEmergencyObserver->CallStateChanged( 
            MCCPCallObserver::ECCPStateIdle, NULL );
        }
    
    return err;
    }

// ---------------------------------------------------------------------------
// CSVPEmergencySession::Answer
// ---------------------------------------------------------------------------
//
TInt CSVPEmergencySession::Answer()
    {
    SVPDEBUG1("CSVPEmergencySession::Answer()")
    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// CSVPEmergencySession::TimedOut
// ---------------------------------------------------------------------------
// 
void CSVPEmergencySession::TimedOut( TInt aTimerId )
    {
    SVPDEBUG1("CSVPEmergencySession::TimedOut()")
    
     // Find the timer and delete it.
    for ( TInt t = 0; t < iTimers.Count(); )
        {
        if ( iTimers[t] ->Id() == aTimerId )
            {
            delete iTimers[t];
            iTimers.Remove( t );
            }
        else
            {
            t++;
            }
        }

    switch ( aTimerId )
        {
        case KSVPHangUpTimerExpired:
            SVPDEBUG1("CSVPEmergencySession::TimedOut, hangup")
            iEmergencyObserver->CallStateChanged( 
                MCCPCallObserver::ECCPStateIdle, NULL );
            break;

        case KSVPTerminationTimerExpired:
            SVPDEBUG1("CSVPEmergencySession::TimedOut, termination")
            iSessionState = MCCPCallObserver::ECCPStateIdle;
            iEmergencyObserver->CallStateChanged( State(), NULL );
            break;

        case KSVPInviteTimerExpired:
            SVPDEBUG1("CSVPEmergencySession::TimedOut, invite")
            // Connection made but no answer, no point trying other profiles
            ErrorOccurred( ECCPEmergencyFailed );
            Cancel();
            iEmergencyObserver->CallStateChanged( 
                MCCPCallObserver::ECCPStateDisconnecting, NULL );
            iEmergencyObserver->CallStateChanged( 
                MCCPCallObserver::ECCPStateIdle, NULL );
            break;
        
        case KSVPEmergencyExpired:
            SVPDEBUG1("CSVPEmergencySession::TimedOut, emergency")
            ErrorOccurred( ECCPErrorTimedOut );
            break;
        
        case KSVPRemoteEndDiedExpired:
            SVPDEBUG1("CSVPEmergencySession::TimedOut, remote end died")
            iEmergencyObserver->CallStateChanged( 
                MCCPCallObserver::ECCPStateIdle, NULL );
            break;
        
        default:
            SVPDEBUG1("CSVPEmergencySession::TimedOut, no timer found")
            break;
        }    
    }

// ---------------------------------------------------------------------------
// From class MSIPObserver
// CSVPEmergencySession::IncomingRequest
// ---------------------------------------------------------------------------
// 
void CSVPEmergencySession::IncomingRequest( 
    TUint32 /*aIapId*/, CSIPServerTransaction* /*aTransaction*/ )
    {
    }

// ---------------------------------------------------------------------------
// From class MSIPObserver
// CSVPEmergencySession::TimedOut
// ---------------------------------------------------------------------------
//     
void CSVPEmergencySession::TimedOut( 
    CSIPServerTransaction& /*aSIPServerTransaction*/ )
    {
    }

// ---------------------------------------------------------------------------
// From class MSIPProfileRegistryObserver
// CSVPEmergencySession::ProfileRegistryErrorOccurred
// ---------------------------------------------------------------------------
//  
void CSVPEmergencySession::ProfileRegistryErrorOccurred( 
    TUint32 /*aSIPProfileId*/, TInt /*aError*/ )
    {
    }

// ---------------------------------------------------------------------------
// From class MSIPProfileRegistryObserver
// CSVPEmergencySession::ProfileRegistryEventOccurred
// ---------------------------------------------------------------------------
//  
void CSVPEmergencySession::ProfileRegistryEventOccurred( 
    TUint32 /*aProfileId*/, TEvent /*aEvent*/ )
    {
    }

// ---------------------------------------------------------------------------
// From class MSVPPositioningProviderObserver.
// Handles successful positioning requests
// ---------------------------------------------------------------------------
//
void CSVPEmergencySession::PositioningRequestComplete( 
    const TDesC8& aPosition )
    {
    SVPDEBUG2("CSVPEmergencySession::PositioningRequestComplete() - \
     length: %d", aPosition.Length())
    
    iIsLIRequestReady = ETrue;
    
    delete iPositionInformation;
    iPositionInformation = NULL;
    TRAPD( err, iPositionInformation = HBufC8::NewL( aPosition.Length() ) )
    if ( !err )
        {
        ( iPositionInformation->Des() ).Copy( aPosition );
        }

    iPositioningProvider->CloseModule();
    ProceedDial();
    }

// ---------------------------------------------------------------------------
// From class MSVPPositioningProviderObserver.
// Handles failed positioning requests
// ---------------------------------------------------------------------------
//
#ifdef _DEBUG
void CSVPEmergencySession::PositioningErrorOccurred( TInt aError )
#else
void CSVPEmergencySession::PositioningErrorOccurred( TInt /*aError*/ )
#endif // _DEBUG
    {
    SVPDEBUG2("CSVPEmergencySession::PositioningErrorOccurred( %d )", aError)
    
    iIsLIRequestReady = ETrue;
    iPositioningProvider->CloseModule();
    ProceedDial();
    }

// ---------------------------------------------------------------------------
// From class MSVPEmergencyConnectionObserver.
// SNAP connection is established
// ---------------------------------------------------------------------------
//
void CSVPEmergencySession::SnapConnected()
    {
    SVPDEBUG1("CSVPEmergencySession::SnapConnected() In")
    
    if ( NULL == iEmergencyConnection )
        {
        SVPDEBUG1("CSVPEmergencySession::SnapConnected, no connection object")
        ErrorOccurred( ECCPErrorGeneral );
        return;
        }
    
    TInt err = iEmergencyConnection->IapId( iIapId );
    SVPDEBUG2("CSVPEmergencySession::ConnectionReady, IAP ID: %d", iIapId)
    if ( err )
        {
        SVPDEBUG2("CSVPEmergencySession::SnapConnected, error: %d", err)
        ErrorOccurred( ECCPErrorGeneral );
        return;
        }

    TRAP( err, CreateUnregistedSessionL() )
    if ( err )
        {
        SVPDEBUG2("CSVPEmergencySession::SnapConnected, error: %d", err)
        ErrorOccurred( ECCPErrorGeneral );
        return;
        }

    // Request for position information
    RequestPosition( iIapId );    
    
    ProceedDial();
        
    SVPDEBUG1("CSVPEmergencySession::SnapConnected() Out")
    }

// ---------------------------------------------------------------------------
// From class MSVPEmergencyConnectionObserver.
// Connection is established
// ---------------------------------------------------------------------------
//    
void CSVPEmergencySession::Connected()
    {
    SVPDEBUG1("CSVPEmergencySession::Connected()")
    
    TRAPD( error, RequestSipProxyAddressL() )
    if ( error )
        {
        SVPDEBUG2("CSVPEmergencySession::Connected, error: %d", error)
        ErrorOccurred( ECCPErrorGeneral );
        }
    }

// ---------------------------------------------------------------------------
// From class MSVPEmergencyConnectionObserver.
// SIP proxy address ready
// ---------------------------------------------------------------------------
//    
void CSVPEmergencySession::SipProxyAddressReady( const TDesC16& aAddress )
    {
    SVPDEBUG1("CSVPEmergencySession::SipProxyAddressReady()")
    
    TRAPD( error, ContinueConstructWithIapIdL( aAddress ) )
    if ( error )
        {
        SVPDEBUG2("CSVPEmergencySession::SipProxyAddressReady, error: %d", 
            error)
        ErrorOccurred( ECCPErrorGeneral );
        }
    }

// ---------------------------------------------------------------------------
// From class MSVPEmergencyConnectionObserver.
// Connection error has occurred
// ---------------------------------------------------------------------------
//
#ifdef _DEBUG
void CSVPEmergencySession::ConnectionError( TInt aError )
#else
void CSVPEmergencySession::ConnectionError( TInt /*aError*/ )
#endif // _DEBUG
    {
    SVPDEBUG2("CSVPEmergencySession::ConnectionError, error: %d", aError)
    
    ErrorOccurred( ECCPErrorGeneral );
    }

// ---------------------------------------------------------------------------
// From class MSVPHoldObserver.
// Local hold
// ---------------------------------------------------------------------------
//
void CSVPEmergencySession::SessionLocallyHeld()
    {
    SVPDEBUG1("CSVPEmergencySession::SessionLocallyHeld()")
    iEmergencyObserver->CallStateChanged( MCCPCallObserver::ECCPStateHold, 
                                          NULL );
    }

// ---------------------------------------------------------------------------
// From class MSVPHoldObserver.
// Local resume
// ---------------------------------------------------------------------------
//
void CSVPEmergencySession::SessionLocallyResumed()
    {
    SVPDEBUG1("CSVPEmergencySession::SessionLocallyResumed()")
    iEmergencyObserver->CallStateChanged( 
        MCCPCallObserver::ECCPStateConnected, NULL );
    }

// ---------------------------------------------------------------------------
// From class MSVPHoldObserver.
// Remote hold
// ---------------------------------------------------------------------------
//
void CSVPEmergencySession::SessionRemoteHeld()
    {
    SVPDEBUG1("CSVPEmergencySession::SessionRemoteHeld()")
    iEmergencyObserver->CallEventOccurred( MCCPCallObserver::ECCPRemoteHold, 
                                           NULL );
    }

// ---------------------------------------------------------------------------
// From class MSVPHoldObserver.
// Remote resume
// ---------------------------------------------------------------------------
//
void CSVPEmergencySession::SessionRemoteResumed()
    {
    SVPDEBUG1("CSVPEmergencySession::SessionRemoteResumed()")
    iEmergencyObserver->CallEventOccurred( 
        MCCPCallObserver::ECCPRemoteResume, NULL );
    }

// ---------------------------------------------------------------------------
// From class MSVPHoldObserver.
// Hold request failed
// ---------------------------------------------------------------------------
//
void CSVPEmergencySession::HoldRequestFailed()
    {
    SVPDEBUG1("CSVPEmergencySession::HoldRequestFailed()")
    iEmergencyObserver->ErrorOccurred( ECCPLocalHoldFail, NULL );
    }

// ---------------------------------------------------------------------------
// From class MSVPHoldObserver.
// Resume request failed
// ---------------------------------------------------------------------------
//
void CSVPEmergencySession::ResumeRequestFailed()
    {
    SVPDEBUG1("CSVPEmergencySession::ResumeRequestFailed()")
    iEmergencyObserver->ErrorOccurred( ECCPLocalResumeFail, NULL );
    }

// ---------------------------------------------------------------------------
// Inband DTMF event occurred
// ---------------------------------------------------------------------------
//   
void CSVPEmergencySession::InbandDtmfEventOccurred( TSVPDtmfEvent aEvent )
    {
    SVPDEBUG2("CSVPEmergencySession::InbandDtmfEventOccurred(), event:%d", 
        aEvent)
    
    switch ( aEvent )
        {
        case ESvpDtmfSendStarted:
            iDtmfObserver->HandleDTMFEvent( 
                MCCPDTMFObserver::ECCPDtmfSequenceStart, 
                KErrNone, 
                iDtmfLex.Peek() );
            break;
            
        case ESvpDtmfSendStopped:
            iDtmfObserver->HandleDTMFEvent( 
                MCCPDTMFObserver::ECCPDtmfSequenceStop, 
                KErrNone, 
                iDtmfLex.Get() );
            break;
            
        case ESvpDtmfSendCompleted:
            {
            iDtmfObserver->HandleDTMFEvent( 
                MCCPDTMFObserver::ECCPDtmfStringSendingCompleted, 
                KErrNone, 
                iDtmfLex.Peek() );
            
            // String can be deleted as whole string has been sent 
            delete iDtmfString;
            iDtmfString = NULL;
            
            break;
            }
            
        default:
            break;
        }
    
    SVPDEBUG1("CSVPEmergencySession::InbandDtmfEventOccurred() Out");
    }

// ---------------------------------------------------------------------------
// Deletes failed MCE session and creates a new one without LI. Used when 
// 415 Unsupported Media Type received.
// ---------------------------------------------------------------------------
//
void CSVPEmergencySession::CreateNonLISessionL()
    {
    SVPDEBUG1("CSVPEmergencySession::CreateNonLISessionL() IN")

    // Save failed emergency session
    iFailedEmergencySession = iEmergencySession;
    iEmergencySession = NULL;

    // Delete LI
    delete iPositionInformation;
    iPositionInformation = NULL;
    iIsLIRequestReady = ETrue;
    
    if ( KErrNotFound == iVoipProfileId )
        {
        // No VoIP profile, trying IAP emergency call
        SVPDEBUG1("CSVPEmergencySession::CreateNonLISessionL, IAP EC")
        CreateUnregistedSessionL();
        return;
        }
    
    // Initialize SIP registry
    CSIP* sip = CSIP::NewLC( KSVPUid, *this ); // CS:1
    CSIPProfileRegistry* sipProfileRegistry = CSIPProfileRegistry::NewLC( 
        *sip, *this ); // CS:2

    // Get SIP profile, only one SIP profile per VoIP profile
    CSIPProfile* sipProfile = sipProfileRegistry->ProfileL( iSipProfileId );
    CleanupStack::PushL( sipProfile ); // CS:3

    // Create MCE out session
    TBool registered( EFalse );
    sipProfile->GetParameter( KSIPProfileRegistered, registered );
    if ( registered )
        {
        SVPDEBUG1("CSVPEmergencySession::CreateNonLISessionL, registered")
        iEmergencySession = CMceOutSession::NewL( 
            iMceManager, *sipProfile, *iRecipientUri );
        ConstructAudioStreamsL();
        }
    else
        {
        CreateUnregistedSessionL();        
        }
    
    CleanupStack::PopAndDestroy( 3, sip ); 
    // CS:0 sip, sipProfileRegistry, sipProfile

    SVPDEBUG1("CSVPEmergencySession::CreateNonLISessionL() OUT")
    }

// ---------------------------------------------------------------------------
// Creates unregistered MCE session
// ---------------------------------------------------------------------------
//
void CSVPEmergencySession::CreateUnregistedSessionL()
    {
    SVPDEBUG1("CSVPEmergencySession::CreateUnregistedSessionL()")
    
    iEmergencySession = CMceOutSession::NewL( 
        iMceManager, iIapId, *iUserAor, *iRecipientUri );

    ConstructAudioStreamsL();
    }

// ---------------------------------------------------------------------------
// Handles emergency session state change
// ---------------------------------------------------------------------------
//
void CSVPEmergencySession::SessionStateChangedL( TInt aMceStatusCode )
    {
    SVPDEBUG3("CSVPEmergencySession::SessionStateChangedL,\
     status: %d,state: %d ", aMceStatusCode, iEmergencySession->State() )
         
    // Match TCCPSessionError
    TInt ccpStatusCode = aMceStatusCode;     
    TCCPError ccpError = iSVPUtility.GetCCPError( ccpStatusCode, iTone );
    
    // Call voip event logger with original status
    if ( ECCPErrorNone != ccpError && KErrNotFound != iVoipProfileId )
        {
        iSVPUtility.LogVoipEventL( 
            aMceStatusCode, iSipProfileId, KNullDesC, KNullDesC );
        }        
   
    // Status code returns KErrNone if TCCPSessionError is not found
    if ( KErrNone != ccpStatusCode )
        {
        SVPDEBUG2("CSVPEmergencySession::SessionStateChanged:\
         error: %d", aMceStatusCode )

        if ( ECCPErrorRejected == ccpError || 
             ECCPErrorBusy == ccpError || 
             ECCPErrorTimedOut == ccpError )
            {     
            SVPDEBUG1("CSVPEmergencySession::SessionStateChanged\
             iSessionState -> Disconnecting")
            iSessionState = MCCPCallObserver::ECCPStateDisconnecting;
            }
        else if ( KSVPUnsupportedMediaTypeVal == aMceStatusCode && 
                  CMceSession::ETerminated == iEmergencySession->State() && 
                  iPositionInformation )
            {
            // MT cannot handle LI: create a new session without LI
            CreateNonLISessionL();
            ProceedDial();
            return;
            }

        ErrorOccurred( ccpError );
        }
    
    // Handle provisional responses 1XX
    if ( KSVPRingingVal == aMceStatusCode || 
         KSVPForwardedVal == aMceStatusCode || 
         KSVPQueuedVal == aMceStatusCode || 
         KSVPSessionProgressVal == aMceStatusCode )
        {        
        // Response received, stop emergency timer
        StopTimer( KSVPEmergencyExpired );

        if ( KSVPRingingVal == aMceStatusCode )
            {
            SVPDEBUG1(
            "CSVPEmergencySession::SessionStateChanged, 180 Ringing received")
            iEmergencyObserver->CallStateChanged( 
                MCCPCallObserver::ECCPStateConnecting, NULL );
            }
        }

    switch( iEmergencySession->State() )
        {
        case CMceSession::EEstablished:
            {
            StopTimers();
            
            // Save state because it may change while checking priorities
            MCCPCallObserver::TCCPCallState ccpState = State();
            
            if ( SVPAudioUtility::MmfPriorityUpdateNeededL( 
                     iEmergencySession->Streams() ) )
                {
                CheckMmfPrioritiesForDtmfL( iEmergencySession->Streams() );
                }
            
            UpdateKeepAliveL();
            
            SVPAudioUtility::EnableSpeakerSinksL( 
                iEmergencySession->Streams() );
            
            iEmergencyObserver->CallStateChanged( ccpState, NULL );
            break;
            }
        case CMceSession::EAnswering:
        case CMceSession::EReserving:
        case CMceSession::EIncoming:
        case CMceSession::EOffering:
            {
            SVPDEBUG1("CSVPEmergencySession:SessionStateChanged, no action")
            break;
            }
        case CMceSession::ECancelling:
        case CMceSession::EIdle:
        case CMceSession::ETerminating:
        case CMceSession::EProceeding:
            {
            SVPDEBUG1("CSVPEmergencySession:SessionStateChanged:\
             Callback state change")
            iEmergencyObserver->CallStateChanged( State(), NULL );
            break;
            }
        case CMceSession::ETerminated:
            {
            SVPDEBUG1("CSVPEmergencySession::SessionStateChanged, terminated")
            iSessionState = MCCPCallObserver::ECCPStateDisconnecting;
            StartTimerL( KSVPTerminatingTime, KSVPTerminationTimerExpired );
            iEmergencyObserver->CallStateChanged( State(), NULL );
            break;
            }
        default:
            {
            SVPDEBUG1("CSVPEmergencySession:SessionStateChanged: default")
            break;
            }
        }
        
    SVPDEBUG1("CSVPEmergencySession::SessionStateChanged Out")
    }

// ---------------------------------------------------------------------------
// Determines if dialing can be proceeded
// ---------------------------------------------------------------------------
//
void CSVPEmergencySession::ProceedDial()
    {
    SVPDEBUG1("CSVPEmergencySession::ProceedDial()")
    
    if ( iIsDialed && 
         iIsLIRequestReady && 
         iEmergencySession &&
         iEmergencySession->ConnectionActive() )
        {
        TRAPD( err, DialL() );
        if ( err )
            {
            SVPDEBUG2("CSVPEmergencySession::ProceedDial(), error: %d", err)
            ErrorOccurred( ECCPErrorGeneral );
            }
        }
    }

// ---------------------------------------------------------------------------
// CSVPEmergencySession::DialL
// ---------------------------------------------------------------------------
//
void CSVPEmergencySession::DialL()
    {
    SVPDEBUG1("CSVPEmergencySession::DialL()")
    
    // Add priority header
    CDesC8ArrayFlat* headers = new ( ELeave ) CDesC8ArrayFlat( 1 );
    CleanupStack::PushL( headers ); // CS:1
    headers->AppendL( KSVPEmergencyPriorityEmergency() );
    
    // Add location related headers
    if ( iPositionInformation && iUserAor )
        {
        // Create content headers
        CDesC8ArrayFlat* contentHeaders = new ( ELeave ) CDesC8ArrayFlat( 1 );
        CleanupStack::PushL( contentHeaders ); // CS:2

        // Content-ID: alice at atlanta.example.com
        HBufC8* contentIdValue = ParseContentIdL( *iUserAor );
        CleanupStack::PushL( contentIdValue ); // CS:3

        HBufC8* contentId = HBufC8::NewLC( 
            KSVPEmergencyContentIdName().Length() + 
            contentIdValue->Length() ); // CS:4
        ( contentId->Des() ).Copy( KSVPEmergencyContentIdName() );
        ( contentId->Des() ).Append( *contentIdValue );

        contentHeaders->AppendL( *contentId );
        CleanupStack::PopAndDestroy( contentId ); // CS:3

        // Geolocation: <cid:alice at atlanta.example.com>;
        // inserted-by=alice at atlanta.example.com;recipient=endpoint
        HBufC8* cidValue = ParseCidL( *contentIdValue );
        CleanupStack::PushL( cidValue ); // CS:4
        
        HBufC8* geolocation = HBufC8::NewL( 
            KSVPEmergencyGeolocation().Length() + 
            cidValue->Length() + 
            KSVPEmergencyTagInsertedBy().Length() + 
            contentIdValue->Length() + 
            KSVPEmergencyTagRecipientEndpoint().Length() );
        ( geolocation->Des() ).Copy( KSVPEmergencyGeolocation() );
        ( geolocation->Des() ).Append( *cidValue );
        ( geolocation->Des() ).Append( KSVPEmergencyTagInsertedBy() );
        ( geolocation->Des() ).Append( *contentIdValue );
        ( geolocation->Des() ).Append( KSVPEmergencyTagRecipientEndpoint() );
        
        // cidValue, contentIdValue CS:2
        CleanupStack::PopAndDestroy( 2, contentIdValue );
        CleanupStack::PushL( geolocation ); // CS:3
        headers->AppendL( *geolocation );
        CleanupStack::PopAndDestroy( geolocation ); // CS:2

        // Accept: application/pidf+xml
        headers->AppendL( KSVPEmergencyAcceptApplicationSdp() );

        // Content-Type: application/pidf+xml
        HBufC8* contentType = HBufC8::NewLC( 
            KSVPEmergencyApplicationPidfXml().Length() ); // CS:3
        ( contentType->Des() ).Copy( KSVPEmergencyApplicationPidfXml() );

        // Establish call
        ( ( CMceOutSession* )iEmergencySession )->EstablishL( 
            iSessionExpires, 
            headers, 
            contentType, 
            iPositionInformation->AllocL(), 
            contentHeaders );

        // contentType, contentHeaders, Ownership transferred CS:1
        CleanupStack::Pop( 2, contentHeaders );
        }
    else
        {
        ( ( CMceOutSession* )iEmergencySession )->EstablishL( 
            iSessionExpires, headers );
        }
    CleanupStack::Pop( headers ); // Ownership transferred, CS:0
    
    // Start INVITE timer in case remote end does not respond
    StartTimerL( KSVPInviteTimer, KSVPInviteTimerExpired );
    }

// ---------------------------------------------------------------------------
// CSVPEmergencySession::ConstructAudioStreamsL
// ---------------------------------------------------------------------------
//  
void CSVPEmergencySession::ConstructAudioStreamsL()
    {
    SVPDEBUG1( "CSVPEmergencySession::ConstructAudioStreamsL In" )

    // OUT STREAM
    CMceAudioStream* audioOutStream = CMceAudioStream::NewLC(); // CS:1
    // Set microphone as audio out source, only one source at a time    
    CMceMicSource* mic = CMceMicSource::NewLC(); // CS:2
    audioOutStream->SetSourceL( mic );
    CleanupStack::Pop( mic ); // CS:1
    CMceRtpSink* rtpSink = CMceRtpSink::NewLC(); // CS:2
	audioOutStream->AddSinkL( rtpSink );
	CleanupStack::Pop( rtpSink ); // CS:1
  
    // IN STREAM
    CMceAudioStream* audioInStream = CMceAudioStream::NewLC(); // CS:2
	CMceRtpSource* rtpSource = CMceRtpSource::NewLC( 
	    KSvpJitterBufferLength, 
	    KSvpJitterBufferThreshold, 
	    KSvpStandbyTimerInMillisecs ); // CS:3
	audioInStream->SetSourceL( rtpSource );
	CleanupStack::Pop( rtpSource ); // CS:2
	// Set speaker as audio in sink	
	CMceSpeakerSink* speakerSink = CMceSpeakerSink::NewLC(); // CS:3
	audioInStream->AddSinkL( speakerSink );
	CleanupStack::Pop( speakerSink ); // CS:2

	// Bind "audio out" stream to "audio in" stream
	audioOutStream->BindL( audioInStream );
	CleanupStack::Pop( audioInStream ); // CS: 1
	
    iEmergencySession->AddStreamL( audioOutStream );
    CleanupStack::Pop( audioOutStream ); // CS: 0
    
    // Modify QoS preconditions off
    iEmergencySession->SetModifierL( 
        KMcePreconditions, KMcePreconditionsNotUsed );
        
    // Modify Old way hold on
    iEmergencySession->SetModifierL( 
        KMceMediaDirection, KMceMediaDirectionWithAddress );

    SVPDEBUG2("CSVPEmergencySession::ConstructAudioStreamsL, in codecs: %d", 
        audioInStream->Codecs().Count())
    SVPDEBUG2("CSVPEmergencySession::ConstructAudioStreamsL, out codecs: %d", 
        audioOutStream->Codecs().Count())

    // Set keep-alive value
    SetKeepAlive();
    
    // Add codecs to audiostream
    iSVPUtility.SetAudioCodecsForEmergencyL( 
        *audioInStream, iKeepAlive, iVoipProfileId );
    
    audioOutStream->SetLocalMediaPortL( audioInStream->LocalMediaPort() );
    
    iSVPUtility.UpdateJitterBufferSizeL( *rtpSource );
    
    // Set MMF priorities and preferences to codecs
    iSVPUtility.SetDtmfMode( SVPAudioUtility::SetPriorityCodecValuesL( 
        *audioInStream, *audioOutStream ) );
                                         
    SVPDEBUG1( "CSVPEmergencySession::ConstructAudioStreamsL Out" )
    }

// ---------------------------------------------------------------------------
// Sets keep-alive value
// ---------------------------------------------------------------------------
//  
void CSVPEmergencySession::SetKeepAlive()
    {
    SVPDEBUG1("CSVPEmergencySession::SetKeepAlive()")
    
    // Get keep-alive value, do not leave: return default value instead
    TBool found( EFalse );
    TRAPD( errKeepAlive, 
        found = iSVPUtility.GetKeepAliveByIapIdL( iIapId, iKeepAlive ) )
    if ( errKeepAlive || !found )
        {
        TRAP( errKeepAlive, found = iSVPUtility.GetKeepAliveByAORL( 
            *iUserAor, iKeepAlive ) )
        if ( errKeepAlive || !found )
            {
            iKeepAlive = KSVPDefaultUDPRefreshInterval;
            }
        }
    
    SVPDEBUG2("CSVPEmergencySession::KeepAlive, value: %d", iKeepAlive)
    }

// ---------------------------------------------------------------------------
// Updates keepalive parameters for MCE session
// ---------------------------------------------------------------------------
//
void CSVPEmergencySession::UpdateKeepAliveL()
    {
    SVPDEBUG1("CSVPEmergencySession::UpdateKeepAliveL()")
    
    const RPointerArray<CMceMediaStream>& streamArray = 
        iEmergencySession->Streams();
    TInt streamCount( streamArray.Count() );
    TBool cnInAnswer( EFalse );
    
    for ( TInt i = 0; i < streamCount; i++ )
        {
        CMceAudioStream* stream = 
            static_cast<CMceAudioStream*>( streamArray[i] );
        if ( iSVPUtility.IsComfortNoise( *stream ) )
            {
            cnInAnswer = ETrue;
            iSVPUtility.SetCNKeepAliveL( *stream, iKeepAlive );
            }
        }
    
    if ( !cnInAnswer )
        {
        while( streamCount-- )
            {
            CMceAudioStream* stream = 
                static_cast<CMceAudioStream*>( streamArray[streamCount] );
            iSVPUtility.SetKeepAliveL( *stream, iKeepAlive );
            }
        }
    
    iEmergencySession->UpdateL();
    }

// ---------------------------------------------------------------------------
// Check audio priorities in audio streams
// ---------------------------------------------------------------------------
//
void CSVPEmergencySession::CheckMmfPrioritiesForDtmfL(
    const RPointerArray<CMceMediaStream>& aAudioStreams ) const
    {
    SVPDEBUG1( "CSVPEmergencySession::CheckMmfPrioritiesForDtmfL In" )
    
    RPointerArray<CMceMediaSource> micsToEnable;
    CleanupClosePushL( micsToEnable );
    const TInt streamCount( aAudioStreams.Count() );
    
    // First disable the mic before doing any priority updating.
    for ( TInt s = 0; s < streamCount; s++ )
        {
        CMceMediaSource* mic = aAudioStreams[s]->Source();
        
        if ( mic && KMceMicSource == mic->Type() && mic->IsEnabled() )
            {
            mic->DisableL();
            micsToEnable.AppendL( mic );
            }
        mic = NULL;
        
        if ( aAudioStreams[s]->BoundStream() )
            {
            mic = aAudioStreams[s]->BoundStreamL().Source();
            if ( mic && KMceMicSource == mic->Type() && mic->IsEnabled() )
                {
                mic->DisableL();
                micsToEnable.AppendL( mic );
                }
            }
        mic = NULL;
        }
    
    // Set the correct priority and preference values.
    for ( TInt k = 0; k < streamCount; k++ )
        {
        if ( KMceAudio == aAudioStreams[k]->Type() && 
            aAudioStreams[k]->BoundStream() )
            {
            CMceAudioStream* stream = 
                static_cast<CMceAudioStream*>( aAudioStreams[k] );
            
            TBool dtmfMode = EFalse;
            if ( SVPAudioUtility::IsDownlinkStream( *stream ) )
                {
                dtmfMode = SVPAudioUtility::SetPriorityCodecValuesL( 
                    *stream,
                    static_cast<CMceAudioStream&>( stream->BoundStreamL() ) );
                }
            else
                {
                dtmfMode = SVPAudioUtility::SetPriorityCodecValuesL(
                    static_cast<CMceAudioStream&>( stream->BoundStreamL() ),
                    *stream );
                }
            
            iSVPUtility.SetDtmfMode( dtmfMode );
            }
        }
        
    // Priorities are now correct, so update the session.
    iEmergencySession->UpdateL();
    
    // Now enable mics after we have correct priorities.
    const TInt mics = micsToEnable.Count();
    for ( TInt t = 0; t < mics; t++ )
        {
        micsToEnable[t]->EnableL();
        }
    
    // Mics not owned
    CleanupStack::PopAndDestroy( &micsToEnable );
    
    SVPDEBUG1( "CSVPEmergencySession::CheckMmfPrioritiesForDtmfL Out" )
    }

// ---------------------------------------------------------------------------
// CSVPEmergencySession::StartTimerL
// ---------------------------------------------------------------------------
//
void CSVPEmergencySession::StartTimerL( TInt aMilliSeconds, TInt aTimerId )
    {
    CSVPTimer* timer = CSVPTimer::NewL( *this, aTimerId );
    timer->SetTime( aMilliSeconds );
    iTimers.Append( timer );
    }

// ---------------------------------------------------------------------------
// Stop timer
// ---------------------------------------------------------------------------
//
void CSVPEmergencySession::StopTimer( TInt aTimerId )
    {
    SVPDEBUG2("CSVPEmergencySession::StopTimer, ID: %d", aTimerId)

    for ( TInt i = 0; i < iTimers.Count(); )
        {
        if ( aTimerId == iTimers[i]->Id() )
            {
            iTimers[i]->Stop();
            delete iTimers[i];
            iTimers.Remove( i );
            }
        else
            {
            i++;
            }
        }
    }

// ---------------------------------------------------------------------------
// CSVPEmergencySession::StopTimers
// ---------------------------------------------------------------------------
//
void CSVPEmergencySession::StopTimers()
    {
    SVPDEBUG1("CSVPEmergencySession::StopTimers")
    
    while ( iTimers.Count() )
        {
        iTimers[0]->Stop();
        delete iTimers[0];
        iTimers.Remove( 0 );   
        }
    }

// ---------------------------------------------------------------------------
// Notify client about an error
// ---------------------------------------------------------------------------
//
void CSVPEmergencySession::ErrorOccurred( TCCPError aError )
    {
    SVPDEBUG2("CSVPEmergencySession::ErrorOccurred: %d", (TInt)aError)
    
    if ( iIsLastProfile )
        {
        iEmergencyObserver->ErrorOccurred( ECCPEmergencyFailed, NULL );
        }
    else
        {
        iEmergencyObserver->ErrorOccurred( aError, NULL );
        }
    }

// ---------------------------------------------------------------------------
// Traps leaves of RequestPositionL
// ---------------------------------------------------------------------------
//
void CSVPEmergencySession::RequestPosition( TUint32 aIapId )
    {
    SVPDEBUG1( "CSVPEmergencySession::RequestPosition" )
    iIsLIRequestReady = EFalse;
    TRAP_IGNORE( RequestPositionL( aIapId ) );
    }

// ---------------------------------------------------------------------------
// Initializes position provider and requests for position information
// ---------------------------------------------------------------------------
//
void CSVPEmergencySession::RequestPositionL( TUint32 aIapId )
    {
    iPositioningProvider = CSVPPositioningProvider::NewL( 
        KSVPEmergencyPositioningPriority, *this );
    if ( iPositioningProvider )
        {
        iPositioningProvider->OpenModuleL( 
            KSVPEmergencyDhcpDefaultPsyModuleId );
        iPositioningProvider->MakePositioningRequestL( 
            aIapId, 
            KSVPEmergencyApplicationName, 
            KSVPEmergencyPositioningTimeout );
        }
    }

// ---------------------------------------------------------------------------
// Parses content ID from URI: removes "sip(s):" prefix, if one exists
// ---------------------------------------------------------------------------
//
HBufC8* CSVPEmergencySession::ParseContentIdL( const TDesC8& aUri )
    {
    HBufC8* contentId = HBufC8::NewLC( aUri.Length() ); // CS:1
    ( contentId->Des() ).Copy( aUri );
    if ( 0 == contentId->Find( KSVPSipPrefix ) )
        {
        contentId->Des().Delete( 0, KSVPSipPrefixLength );
        }
    else if ( 0 == contentId->Find( KSVPSipsPrefix ) )
        {
        contentId->Des().Delete( 0, KSVPSipsPrefixLength );
        }
    CleanupStack::Pop( contentId ); // CS:0
    contentId = contentId->ReAllocL( contentId->Length() );
    return contentId;
    }

// ---------------------------------------------------------------------------
// Parses cid: adds "cid:" prefix and brackets
// ---------------------------------------------------------------------------
//
HBufC8* CSVPEmergencySession::ParseCidL( const TDesC8& aContentId )
    {
    HBufC8* cid = HBufC8::NewL( 
        KSVPLeftBracketMark().Length() + 
        KSVPCidPrefix().Length() + 
        aContentId.Length() + 
        KSVPRightBracketMark().Length() );

    TPtr8 cidPtr = cid->Des();
    cidPtr.Copy( KSVPLeftBracketMark() );
    cidPtr.Append( KSVPCidPrefix() );
    cidPtr.Append( aContentId );
    cidPtr.Append( KSVPRightBracketMark() );
    
    return cid;
    }
