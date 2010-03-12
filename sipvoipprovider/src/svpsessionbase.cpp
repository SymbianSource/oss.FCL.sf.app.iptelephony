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
* Description:  Session base class for SVP MO and MT sessions. Contains methods 
*                common for SVP MO and MT sessions.
*
*/

#include <e32math.h>
#include <escapeutils.h>

#include <mcetransactiondatacontainer.h>
#include <mceaudiostream.h>
#include <mcertpsink.h>
#include <mcertpsource.h>
#include <mceaudiocodec.h>
#include <mcedefs.h>
#include <mcesecuresession.h> 
#include <mcesecureoutsession.h>
#include <mcesecureinsession.h>
#include <mcemicsource.h>
#include <mcespeakersink.h>
#include <siperr.h>
#include <telephonydomainpskeys.h> // for mute 

#include "svpsessionbase.h"
#include "svpholdcontroller.h"
#include "svptransfercontroller.h"
#include "svptransferstatecontext.h"
#include "svptimer.h"
#include "svpsipconsts.h"
#include "svpuriparser.h"
#include "svpaudioutility.h"
#include "svpvolumeobserver.h"
#include "svplogger.h"
#include "svputility.h"
#include "svprtpobserver.h"
#include "svppropertywatch.h"
#include "svpdtmfeventgenerator.h"


// Local Consts
const TInt KMaxTimerValueMO = 4000;
const TInt KMinTimerValueMO = 2100;
const TInt KMaxTimerValueMT = 2000;
const TInt KMinTimerValueMT = 10;

const TInt KRandomDividerOne = 1000000;
const TInt KRandomDividerTwo = 1000;

const TInt KMaxPhoneVolume = 10;

// ---------------------------------------------------------------------------
// CSVPSessionBase::CSVPSessionBase
// ---------------------------------------------------------------------------
//
CSVPSessionBase::CSVPSessionBase( TMceTransactionDataContainer& aContainer,
    MSVPSessionObserver& aObserver, CSVPUtility& aSVPUtility,
    CSVPRtpObserver& aRtpObserver ) :
    iSVPUtility( aSVPUtility ),
    iRtpObserver( aRtpObserver ),
    iContainer( aContainer ),
    iObserver( aObserver ),
    iErrorInULandDLFirstTime ( ETrue ),
    iEarlyMediaOngoing ( EFalse )
    {
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::~CSVPSessionBase
// ---------------------------------------------------------------------------
//
CSVPSessionBase::~CSVPSessionBase()
    {
    SVPDEBUG1( "SVPSessionBase::~CSVPSessionBase In" )
    
    delete iCallParameters;
    
    delete iHoldController;
    
    iObserver.RemoveFromArray( *this );
    
    delete iSession;
    
    StopTimers();
    
    iTimers.Close();
    
    delete iRecipient;
    
    delete iDisplayName;
    
    delete iTransferController;
    
    delete iMutePropertyWatch;
    
    delete iVolObserver;
    
    delete iFromHeader;
    
    delete iToHeader;
    
    delete iCallId;
    
    delete iCSeqHeader;
    
    delete iEventGenerator;
    
    delete (iTempSecSession != iSession ? iTempSecSession : NULL );
    
    delete iDtmfString;
    
    SVPDEBUG1( "CSVPSessionBase::~CSVPSessionBase Out" )
    }
   
// ---------------------------------------------------------------------------
// CSVPSessionBase::ReleaseTempSecure
// ---------------------------------------------------------------------------
//
void CSVPSessionBase::ReleaseTempSecure()
    {
    delete iTempSecSession;
    iTempSecSession = NULL;
    }
    
// ---------------------------------------------------------------------------
// CSVPSessionBase::Session
// ---------------------------------------------------------------------------
//
const CMceSession& CSVPSessionBase::Session() const   
    {
    return *iSession;
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::StartTimerL
// ---------------------------------------------------------------------------
//
void CSVPSessionBase::StartTimerL( TInt aMilliSeconds, TInt aTimerId )
    {
    SVPDEBUG1( "CSVPSessionBase::StartTimerL In" )
    
    CSVPTimer* timer = CSVPTimer::NewL( *this, aTimerId );
    CleanupStack::PushL( timer );
    timer->SetTime( aMilliSeconds );
    iTimers.AppendL( timer );
    CleanupStack::Pop( timer );
    
    SVPDEBUG1( "CSVPSessionBase::StartTimerL Out" )
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::StopTimers
// ---------------------------------------------------------------------------
//
void CSVPSessionBase::StopTimers()
    {
    SVPDEBUG1( "CSVPSessionBase::StopTimers In" )
    
    while ( iTimers.Count() )
        {
        iTimers[0]->Stop();
        delete iTimers[0];
        iTimers.Remove( 0 );
        }
    
    SVPDEBUG1( "CSVPSessionBase::StopTimers Out" )
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::ReinviteCrossoverTime
// ---------------------------------------------------------------------------
//
TInt CSVPSessionBase::ReinviteCrossoverTime()
    {
    SVPDEBUG1( "CSVPSessionBase::ReinviteCrossoverTime In" )
    
    TInt maxValue( 0 );
    TInt minValue( 0 );
    
    if ( IsMobileOriginated() )
        {
        minValue = KMinTimerValueMO;
        maxValue = KMaxTimerValueMO;
        }
    else
        {
        minValue = KMinTimerValueMT;
        maxValue = KMaxTimerValueMT; 
        }
    
    TInt timerValue( 0 );
    TTime time;
    time.HomeTime();
    TInt64 seed( time.Int64() );
    
    for ( TInt i = 0; i < ( maxValue - minValue ); i++ )
        {
        TInt random = Math::Rand( seed );
        TReal random2 =  ( TReal )random / KRandomDividerOne;
        TInt random3 = ( TInt )( minValue * random2 ) / KRandomDividerTwo;
        
        if ( minValue <= random3 && minValue >= random3 )
            {
            timerValue = random3;
            break;
            }
        }
    
    SVPDEBUG2("CSVPSessionBase::ReinviteCrossoverTime Out return: %d", timerValue )
    return timerValue;    
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::StopTimer
// ---------------------------------------------------------------------------
//
void CSVPSessionBase::StopTimer( TInt aTimerId )
    {
    SVPDEBUG2( "CSVPSessionBase::StopTimer In, Timer -- ID: %d", aTimerId )

    // Find the timer and delete it.
    for ( TInt t = 0; t < iTimers.Count(); )
        {
        if ( iTimers[ t ] ->Id() == aTimerId )
            {
            iTimers[t]->Stop();
            delete iTimers[ t ];
            iTimers.Remove( t );
            }
        else
            {
            t++;
            }
        }
    
    SVPDEBUG1( "CSVPSessionBase::StopTimer Out" )
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::TimedOut
// ---------------------------------------------------------------------------
//    
void CSVPSessionBase::TimedOut( TInt aTimerId )
    {
    SVPDEBUG1( "CSVPSessionBase::TimedOut In" )
     
    // Find the timer and delete it.
    for ( TInt t = 0; t < iTimers.Count(); )
        {
        if ( iTimers[ t ] ->Id() == aTimerId )
            {
            delete iTimers[ t ];
            iTimers.Remove( t );
            }
        else
            {
            t++;
            }
        }
    
    if ( KSVPInviteTimerExpired == aTimerId )
        {
        // INVITE timer expired, notify client
        SVPDEBUG1( "CSVPSessionBase::TimedOut Invite timer expired, callback" )
        ExecCbErrorOccurred( ECCPErrorNotResponding );
        
        // send cancel
        TRAPD( errCancel, static_cast< CMceOutSession* >( iSession )->CancelL() );
        
        if ( errCancel )
            {
            SVPDEBUG2( "CSVPSessionBase::TimedOut Cancel: %d", errCancel )
            }
        
        ExecCbCallStateChanged( MCCPCallObserver::ECCPStateDisconnecting );
        ExecCbCallStateChanged( MCCPCallObserver::ECCPStateIdle );
        }
    
    if ( KSVPTerminationTimerExpired == aTimerId )
        {
        // Renew terminating timer to keep session alive when incoming 
        // transfer ongoing, not longer than max default expire time (120s)
        if ( IsIncomingTransfer() &&
             !IsAttended() &&
             iTerminatingRepeat < KSVPDefaultExpiresTime )
            {
            SVPDEBUG2( "CSVPSessionBase::TimedOut UnAtte case iTerminatingRepeat = %d",
                iTerminatingRepeat )
            
            // Continue timeout time if the new session of the 
            // incoming transfer not yet established
            iTerminatingRepeat = iTerminatingRepeat +
                KSVPTerminatingTime / KSVPMilliSecondCoefficient;
            
            TRAPD( errTimer, StartTimerL( KSVPTerminatingTime,
                    KSVPTerminationTimerExpired ) );
            
            if ( errTimer )
                {
                SVPDEBUG2( "CSVPSessionBase::TimedOut Termination leave with code %d",
                        errTimer )
                }
            }
        else
            {
            SVPDEBUG1( "CSVPSessionBase::TimedOut Termination timer, callback" )
            ExecCbCallStateChanged( MCCPCallObserver::ECCPStateIdle );
            }
        }
    
    if ( KSVPRemoteEndDiedExpired == aTimerId )    
        {
        SVPDEBUG1( "CSVPSessionBase::TimedOut remote end died, callback" )
        // disconnecting state callback sent before timer started
        ExecCbCallStateChanged( MCCPCallObserver::ECCPStateIdle );
        }
    
    if ( KSVPHangUpTimerExpired == aTimerId )    
        {
        SVPDEBUG1( "CSVPSessionBase::TimedOut HangUp timer expired, callback" )
        // disconnecting state callback sent before timer started
        ExecCbCallStateChanged( MCCPCallObserver::ECCPStateIdle );
        }
    
    if ( KSVPExpiresTimeExpired == aTimerId )
        {
        SVPDEBUG1( "CSVPSessionBase::TimedOut expires timer expired" )
        // send 487
        TRAPD( err, static_cast<CMceInSession*>( iSession )->RespondL(
                KSVPRequestTerminatedReason, KSVPRequestTerminatedVal ) );
        
        if ( err )
            {
            // sending response failed
            // not serious because session is put to idle state anyway
            }
        // event is needed in upper level for correct bubble and log handling
        ExecCbCallEventOccurred( MCCPCallObserver::ECCPRemoteTerminated );
        // idle can be sent directly without disconnecting state
        ExecCbCallStateChanged( MCCPCallObserver::ECCPStateIdle );
        }
    
    if ( KSVPHoldTimerExpired == aTimerId ||
         KSVPResumeTimerExpired == aTimerId )
        {
        if ( iHoldController )
            {
            SVPDEBUG1( "CSVPSessionBase::TimedOut - Hold/Resume request Expired" )
            
            iHoldController->TimedOut();
            TInt contErr = iHoldController->ContinueHoldProcessing( *iSession );
            
            if ( KSVPResumeTimerExpired == aTimerId &&
                 CMceSession::EEstablished == iSession->State() )
                {
                // MCE state error prevents cancelling session; go to terminating
                SVPDEBUG1( "CSVPSessionBase::TimedOut - MCE established" )
                HandleSessionStateChanged( *iSession );
                SVPDEBUG1( "CSVPSessionBase::TimedOut - hold/resume done" )
                }
            else
                {
                TRAPD( cancelErr, static_cast<CMceOutSession*>( iSession )->CancelL() );
                
                if ( cancelErr )
                    {
                    // This removes compile warnings; no need for other error 
                    // handling
                    SVPDEBUG2( "CSVPSessionBase::TimedOut - Hold/Resume, Err %i", 
                            cancelErr )
                    }
                }
            
            SVPDEBUG2( "CSVPSessionBase::TimedOut - Hold/Resume Handled, Err %i",
                    contErr )
            }
        }
    
    if ( KSVPReferTimerExpired == aTimerId )
        {
        // Refer timeout
        SVPDEBUG1( "CSVPSessionBase::TimedOut Transfer/Refer timer expired" )
        
        if ( iTransferController )
            {
            // Inform application about the error
            ExecCbErrorOccurred( ECCPTransferFailed );
            // Terminate transfer
            iTransferController->TerminateTransfer();
			}
        }
    
    if ( KSVPReInviteTimerExpired == aTimerId )
        {
        if ( iHoldController )
            {
            SVPDEBUG1( "CSVPSessionBase::TimedOut - ReInvite timer expired" )
            iHoldController->RetryHoldRequest( iSession );
            }
        }

    if ( KSVPICMPErrorTimerExpired == aTimerId )
        {
        TBool bothStreamsDisabled ( ETrue );
        TRAP_IGNORE( bothStreamsDisabled = IsBothStreamsDisabledL() );
        if ( bothStreamsDisabled )
            {
            SVPDEBUG1( "CSVPSessionBase::TimedOut Error In UL and DL \
            NOT First Time! -> Start Terminating Timer and BYE" )
            // notify CCE about the error & bring session down.
            ExecCbErrorOccurred( ECCPErrorConnectionError );
            ExecCbCallStateChanged( MCCPCallObserver::ECCPStateDisconnecting );
        
            TRAP_IGNORE( StartTimerL( KSVPTerminatingTime, KSVPRemoteEndDiedExpired ) );
            }
        else
            {
            SVPDEBUG1( "CSVPSessionBase::TimedOut UpLink or DownLink \
            is not Disabled! -> No Terminating Timer needed, all is OK!" )
            }
        SetErrorInULandDLFirstTime( ETrue );        
        }

    if ( KSVPSinkResumeICMPErrorTimerExpired == aTimerId ||
         KSVPSourceResumeICMPErrorTimerExpired == aTimerId )
        {
        if ( iHoldController )
            {
            // ICMP errors in resume disables RTP sink, try to enable it: 
            SVPDEBUG1( "CSVPSessionBase::TimedOut - Resume ICMP error timer expired" )
            TRAPD( refreshErr, HoldController().RefreshHoldStateL() );
            if ( refreshErr )
                {
                // Removes compile warnings; no other error handling
                SVPDEBUG2( "CSVPSessionBase::TimedOut - refreshHold err %d", refreshErr )
                }
            }
        }

    SVPDEBUG1( "CSVPSessionBase::TimedOut Out" )
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::SessionStateChanged
// ---------------------------------------------------------------------------
//
void CSVPSessionBase::SessionStateChanged( TInt aStatusCode )
    {
    SVPDEBUG2( "CSVPSessionBase::SessionStateChanged  In,  aStatusCode= %d", aStatusCode )
    SVPDEBUG2( "CSVPSessionBase::SessionStateChanged iSession->State()= %d", iSession->State() )
    
    TInt status = aStatusCode;
    TCCPError err = iSVPUtility.GetCCPError( status, iTone );
    
    // On error call voip event logger with original status, 3xx are not logged.
    if ( ECCPErrorNone != err && ECCPErrorNotReached != err )
        {
        TRAP_IGNORE( iSVPUtility.LogVoipEventL( aStatusCode,
                iSipProfileId, RemoteParty(), CSeqHeader() ) );
        }
    
    #ifdef _DEBUG   // UDEB build
    
    TRAPD( trapErr, SessionStateChangedL( aStatusCode, err, status ) );
    SVPDEBUG2( "CSVPSessionBase::SessionStateChanged trapErr: %d", trapErr )
    
    #else // UREL build
    
    TRAP_IGNORE( SessionStateChangedL( aStatusCode, err, status ) );
    
    #endif // _DEBUG
    
    SVPDEBUG1( "CSVPSessionBase::SessionStateChanged Out" )
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::InitializePropertyWatchingL
// ---------------------------------------------------------------------------
//
void CSVPSessionBase::InitializePropertyWatchingL()
    {
    SVPDEBUG1( "CSVPSessionBase::InitializePropertyWatchingL In" )
    
    // create property watcher for watching mute P&S property changes
    iMutePropertyWatch = CSVPPropertyWatch::NewL(
            *this, KPSUidTelMicrophoneMuteStatus, KTelMicrophoneMuteState );
    
    // check initial mute value                                  
    TInt value( EPSTelMicMuteOff );
    RProperty::Get( KPSUidTelMicrophoneMuteStatus, 
            KTelMicrophoneMuteState, value );
    
    // check initial mute status 
    if( EPSTelMicMuteOff == value )
        {
        SVPDEBUG1( "CSVPSessionBase::InitializePropertyWatchingL Initally mute: OFF" )
        }
    else if ( EPSTelMicMuteOn == value )
        {
        SVPDEBUG1( "CSVPSessionBase::InitializePropertyWatchingL Initially Mute: ON" )
        // if mute is initially on, we must mute the audio
        ValueChangedL( KTelMicrophoneMuteState, EPSTelMicMuteOn );
        }
    
    // create instance of volume obsever
    iVolObserver = CSVPVolumeObserver::NewL( *this );
    
    SVPDEBUG1( "CSVPSessionBase::InitializePropertyWatchingL Out" )
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::ExecCbErrorOccurred
// ---------------------------------------------------------------------------
// 
TInt CSVPSessionBase::ExecCbErrorOccurred( TCCPError aError )
    {
    SVPDEBUG2( "CSVPSessionBase::ExecCbErrorOccurred In, aError= %d", aError )
    
    TInt status = KErrNotFound;
    
    if ( iCCPSessionObserver )
        {
        status = KErrNone;
        iCCPSessionObserver->ErrorOccurred( aError, this );
        }
    
    SVPDEBUG2( "CSVPSessionBase::ExecCbErrorOccurred Out return=%d", status )
    return status;
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::ExecCbCallStateChanged
// ---------------------------------------------------------------------------
// 
TInt CSVPSessionBase::ExecCbCallStateChanged(
        MCCPCallObserver::TCCPCallState aNewState )
    {
    SVPDEBUG2( "CSVPSessionBase::ExecCbCallStateChanged In, aNewState= %d", aNewState )
    SVPDEBUG2( "CSVPSessionBase::ExecCbCallStateChanged iSessionState= %d", iSessionState )
    
    TInt status = (TInt) aNewState;
    
    if ( !( (TInt) MCCPCallObserver::ECCPStateIdle > status ) )
        {
        // state transition OK
        iSessionState = (MCCPCallObserver::TCCPCallState) status;
        
        if ( iCCPSessionObserver )
            {
            status = KErrNone;
            iCCPSessionObserver->CallStateChanged( iSessionState, this );
            }
        }
    
    SVPDEBUG2( "CSVPSessionBase::ExecCbCallStateChanged Out return=%d", status )
    return status;
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::ExecCbCallEventOccurred
// ---------------------------------------------------------------------------
//
TInt CSVPSessionBase::ExecCbCallEventOccurred(
        MCCPCallObserver::TCCPCallEvent aEvent )
    {
    SVPDEBUG2( "CSVPSessionBase::ExecCbCallEventOccurred In, aEvent= %d", aEvent )
    
    TInt status = KErrNotFound;
    
    if ( iCCPSessionObserver )
        {
        status = KErrNone;
        iCCPSessionObserver->CallEventOccurred( aEvent, this );
        }
    
    SVPDEBUG2( "CSVPSessionBase::ExecCbCallEventOccurred Out, return= %d", status )
    return status;
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::ExecCbSsEventOccurred
// ---------------------------------------------------------------------------
//
TInt CSVPSessionBase::ExecCbSsEventOccurred(
        MCCPSsObserver::TCCPSsCallForwardEvent aEvent )
    {
    SVPDEBUG2( "CSVPSessionBase::ExecCbSsEventOccurred In, aEvent= %d", aEvent )
    
    TInt status = KErrNotFound;
    
    if ( iCCPSsObserver )
        {
        status = KErrNone;
        iCCPSsObserver->CallForwardEventOccurred( aEvent, *iRecipient );
        }
    
    SVPDEBUG2( "CSVPSessionBase::ExecCbSsEventOccurred Out return=%d", status )
    return status;
    }


//----------------------------------------------------------------------------
//CSVPSessionBase::SetSsObserver
//----------------------------------------------------------------------------
//
void CSVPSessionBase::SetSsObserver( const MCCPSsObserver& aObserver )
    {
    SVPDEBUG1( "CSVPSessionBase::SetSsObserver In" )
    
    iCCPSsObserver = const_cast< MCCPSsObserver* >( &aObserver );
    
    SVPDEBUG1( "CSVPSessionBase::SetSsObserver In" )
    }

//----------------------------------------------------------------------------
//CSVPSessionBase::GetSsObserver
//----------------------------------------------------------------------------
//
const MCCPSsObserver& CSVPSessionBase::GetSsObserver( )
    {
    SVPDEBUG1( "CSVPSessionBase::GetSsObserver" )
    
    return *iCCPSsObserver;    
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::SetDtmfObserver
// ---------------------------------------------------------------------------
//
void CSVPSessionBase::SetDtmfObserver( const MCCPDTMFObserver& aObserver )
    {
    SVPDEBUG1( "CSVPSessionBase::SetDtmfObserver In" )
    
    iCCPDtmfObserver = const_cast< MCCPDTMFObserver* >( &aObserver );
    
    SVPDEBUG1( "CSVPSessionBase::SetDtmfObserver Out" )
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::DtmfObserver
// ---------------------------------------------------------------------------
//
const MCCPDTMFObserver& CSVPSessionBase::DtmfObserver()
    {
    SVPDEBUG1( "CSVPSessionBase::DtmfObserver" )
    
    return *iCCPDtmfObserver;
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::EventStateChanged
// ---------------------------------------------------------------------------
//
TInt CSVPSessionBase::EventStateChanged( CMceEvent& aEvent, TInt aStatusCode )
    {
    SVPDEBUG1( "CSVPSessionBase::EventStateChanged In" )
    
    TInt err( KErrNone );
    
    if ( iTransferController )
        {
        TRAP( err, iTransferController->HandleEventStateChangedL(
                aEvent, aStatusCode ) );
        
        if ( KSVPErrTransferInProgress == err )
            {
            // Transfer allready in progress
            SVPDEBUG1( "CSVPSessionBase::EventStateChanged, KSVPErrTransferInProgress" )
            }
        else if ( KErrNone != err )
            {
            SVPDEBUG2( "CSVPSessionBase::EventStateChanged, err = %d", err )
            // Inform application about the error
            ExecCbErrorOccurred( ECCPTransferFailed );
            }
        }
    else
        {
        // Inform application about the error
        ExecCbErrorOccurred( ECCPTransferFailed );
        err = KSVPErrTransferErrorBase;
        }
    
    SVPDEBUG2( "CSVPSessionBase::EventStateChanged Out return=%d", err )
    return err;
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::NotifyReceived
// ---------------------------------------------------------------------------
//
void CSVPSessionBase::NotifyReceived( CMceEvent& aEvent,
        TMceTransactionDataContainer* aContainer )
    {
    SVPDEBUG1( "CSVPSessionBase::NotifyReceived In" )
    
    if ( iTransferController )
        {
        TRAPD( err, iTransferController->NotifyReceivedL(
                aEvent, aContainer ) );
        
        if ( KErrNone != err )
            {
            // Inform application about the error
            ExecCbErrorOccurred( ECCPTransferFailed );
            }
        }
    else
        {
        // Inform application about the error
        ExecCbErrorOccurred( ECCPTransferFailed );
        }
    
    SVPDEBUG1( "CSVPSessionBase::NotifyReceived Out" )
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::ReferStateChanged
// ---------------------------------------------------------------------------
// 
void CSVPSessionBase::ReferStateChanged( CMceRefer& aRefer, TInt aStatusCode )
    {
    SVPDEBUG1( "CSVPSessionBase::ReferStateChangedL In" )
    
    if ( iTransferController )
        {
        TRAPD( err, iTransferController->HandleReferStateChangeL(
                aRefer, aStatusCode ) );
        
        if ( KErrNone != err )
            {
            // Inform application about the error
            ExecCbErrorOccurred( ECCPTransferFailed );
            }
        }
    else
        {
        // Inform application about the error
        ExecCbErrorOccurred( ECCPTransferFailed );
        }
    
    SVPDEBUG1( "CSVPSessionBase::ReferStateChangedL Out" )
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::MceTransactionDataContainer
// ---------------------------------------------------------------------------
//
TMceTransactionDataContainer& CSVPSessionBase::MceTransactionDataContainer()
    {
    return iContainer;
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::IncomingReferL
// ---------------------------------------------------------------------------
//
void CSVPSessionBase::IncomingReferL( CMceInRefer* aRefer,
        const TDesC8& aReferTo, TMceTransactionDataContainer* aContainer )
    {
    SVPDEBUG1( "CSVPSessionBase::IncomingReferL In" )
    
    if ( !iTransferController )        
        {
        // Create new transfercontroller
        iTransferController = CSVPTransferController::NewL(
                iSession, this, iContainer, *this );
        }
    
    iTransferController->IncomingReferL( aRefer, aReferTo, aContainer );
    
    SVPDEBUG1( "CSVPSessionBase::IncomingReferL Out" )    
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::IsMceRefer
// ---------------------------------------------------------------------------
//
TBool CSVPSessionBase::IsMceRefer( CMceRefer& aRefer )
    {
    if ( iTransferController )
        {
        return iTransferController->IsMceRefer( aRefer );        
        }
    else
        {
        return EFalse;    
        }   
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::IsAttended
// ---------------------------------------------------------------------------
//
TBool CSVPSessionBase::IsAttended()
    {
    if ( iTransferController )
        {
        return iTransferController->IsAttended();
        }
    else
        {
        return EFalse;
        }
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::SetTransferDataL
// ---------------------------------------------------------------------------
//
void CSVPSessionBase::SetTransferDataL( CDesC8Array* aUserAgentHeaders,
        TInt aSecureStatus )
    {
    SVPDEBUG1( "CSVPSessionBase::SetTransferDataL In" )
    
    if ( iTransferController )
        {
        iTransferController->SetTransferDataL(
            aUserAgentHeaders, aSecureStatus );
        }
    
    SVPDEBUG1( "CSVPSessionBase::SetTransferDataL Out" )
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::SendNotify
// ---------------------------------------------------------------------------
//
void CSVPSessionBase::SendNotify( TInt aStatusCode )
    {
    SVPDEBUG1( "CSVPSessionBase::SendNotify In" )
    
    if ( iTransferController )
        {
        #ifdef _DEBUG
        
        TRAPD( err, iTransferController->SendNotifyL( aStatusCode ) );
        SVPDEBUG2("CSVPSessionBase::SendNotify SendNotifyL err: %d", err )
        
        #else
        
        TRAP_IGNORE( iTransferController->SendNotifyL( aStatusCode ) );
        
        #endif // _DEBUG
        }
    
    SVPDEBUG1( "CSVPSessionBase::SendNotify Out" )
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::IsIncomingTransfer
// ---------------------------------------------------------------------------
//
TBool CSVPSessionBase::IsIncomingTransfer()
    {
    if ( iTransferController )
        {
        return iTransferController->IsIncomingTransfer();
        }
    else
        {
        return EFalse;
        }
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::TransferTarget
// ---------------------------------------------------------------------------
//
const TDesC& CSVPSessionBase::TransferTarget() const
    {
    SVPDEBUG1( "CSVPSessionBase::TransferTarget" )
    
    if ( iTransferController )
        {
        return iTransferController->TransferTarget();
        }
    else
        {
        return KNullDesC;
        }
    }

// -----------------------------------------------------------------------------
// CSVPSessionBase::SetFromHeader
// -----------------------------------------------------------------------------
//
TInt CSVPSessionBase::SetFromHeader( const TDesC8& aFromHeader )
    {
    SVPDEBUG1( "CSVPSessionBase::SetFromHeader" )
    
    delete iFromHeader;
    iFromHeader = NULL;
    
    TRAPD( err, iFromHeader = aFromHeader.AllocL() );
    return err;
    }

// -----------------------------------------------------------------------------
// CSVPSessionBase::FromHeader
// -----------------------------------------------------------------------------
//
TDesC8* CSVPSessionBase::FromHeader()
    {
    return iFromHeader;
    }  

// -----------------------------------------------------------------------------
// CSVPSessionBase::SetToHeader
// -----------------------------------------------------------------------------
//
TInt CSVPSessionBase::SetToHeader( const TDesC8& aToHeader )
    {
    SVPDEBUG1( "CSVPSessionBase::SetToHeader" )
    
    delete iToHeader;
    iToHeader = NULL;
    
    TRAPD( err, iToHeader = aToHeader.AllocL() );
    return err;
    }

// -----------------------------------------------------------------------------
// CSVPSessionBase::ToHeader
// -----------------------------------------------------------------------------
//
TDesC8* CSVPSessionBase::ToHeader()
    {
    return iToHeader;
    }

// -----------------------------------------------------------------------------
// CSVPSessionBase::SetCallId
// -----------------------------------------------------------------------------
//
TInt CSVPSessionBase::SetCallId( const TDesC8& aCallId )
    {
    SVPDEBUG1( "CSVPSessionBase::SetCallId" )
    
    delete iCallId;
    iCallId = NULL;
    
    TRAPD( err, iCallId = aCallId.AllocL() );
    return err;
    }

// -----------------------------------------------------------------------------
// CSVPSessionBase::CallId
// -----------------------------------------------------------------------------
//
TDesC8* CSVPSessionBase::CallId()
    {
    return iCallId;
    }

// -----------------------------------------------------------------------------
// CSVPSessionBase::SetCSeqHeader
// -----------------------------------------------------------------------------
//
TInt CSVPSessionBase::SetCSeqHeader( const TDesC8& aCSeq )
    {
    SVPDEBUG1( "CSVPSessionBase::SetCSeqHeader In" )
    
    delete iCSeqHeader;
    iCSeqHeader = NULL;
    
    TRAPD( err, iCSeqHeader = HBufC::NewL( aCSeq.Length() ) );
    iCSeqHeader->Des().Copy( aCSeq );
    
    // Check "CSeq:" and remove it from the beginning if exists
    if ( 0 == iCSeqHeader->Des().FindF( KSVPCSeqPrefix ) )
        {
        // CSeq: is in the beginning of the string, remove it and 1 blank
        iCSeqHeader->Des().Delete( 0, KSVPCSeqPrefixLength + 1 );
        }
    
    // Check/remove the sequence number and the blank
    const TInt spacePosition = iCSeqHeader->Des().Find( KSVPSpace2 );
    
    if ( spacePosition != KErrNotFound )
        {
        iCSeqHeader->Des().Delete( 0, spacePosition + 1 );
        }
    
    SVPDEBUG2( "CSVPSessionBase::SetCSeqHeader Out return=%d", err )
    return err;
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::CSeqHeader
// ---------------------------------------------------------------------------
// 
const TDesC& CSVPSessionBase::CSeqHeader() const
    {
    SVPDEBUG1( "CSVPSessionBase::CSeqHeader In" )  
    
    if ( iCSeqHeader )
        {
        SVPDEBUG1( "CSVPSessionBase::CSeqHeader Out" )
        return *iCSeqHeader;
        }
    else
        {
        SVPDEBUG1( "CSVPSessionBase::CSeqHeader Out KNullDesC" )
        return KNullDesC;
        }
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::AddObserverL
// ---------------------------------------------------------------------------
//
void CSVPSessionBase::AddObserverL( const MCCPCallObserver& aObserver )
    {
    SVPDEBUG1( "CSVPSessionBase::AddObserverL In" )
    
    // set session observer
    // only one observer used at a time, replaces current one
    iCCPSessionObserver = const_cast< MCCPCallObserver* >( &aObserver );
    
    SVPDEBUG1( "CSVPSessionBase::AddObserverL Out" )
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::GetCCPSessionObserver
// ---------------------------------------------------------------------------
//
MCCPCallObserver& CSVPSessionBase::GetCCPSessionObserver()
    {
    SVPDEBUG1( "CSVPSessionBase::GetCCPSessionObserver" )
    // get session observer
    return *iCCPSessionObserver;
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::RemoveObserver
// ---------------------------------------------------------------------------
//
TInt CSVPSessionBase::RemoveObserver( const MCCPCallObserver& /*aObserver*/ )
    {
    return KErrNotSupported; 
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::InbandDtmfEventOccurred
// ---------------------------------------------------------------------------
//
void CSVPSessionBase::InbandDtmfEventOccurred( TSVPDtmfEvent aEvent )
    {
    SVPDEBUG1( "CSVPSessionBase::InbandDtmfEventOccurred In" )
    SVPDEBUG2( "CSVPSessionBase::InbandDtmfEventOccurred aEvent:%d", aEvent )
      
    switch ( aEvent )
        {
        case ESvpDtmfSendStarted:
            {
            DtmfObserver().
                HandleDTMFEvent( MCCPDTMFObserver::ECCPDtmfSequenceStart, 
                                 KErrNone, 
                                 iDtmfLex.Peek() );
            break;
            }
            
        case ESvpDtmfSendStopped:
            {
            DtmfObserver().HandleDTMFEvent( 
                                 MCCPDTMFObserver::ECCPDtmfSequenceStop, 
                                 KErrNone, 
                                 iDtmfLex.Get() );
            break;
            }
            
        case ESvpDtmfSendCompleted:
            {
            DtmfObserver().HandleDTMFEvent( 
                     MCCPDTMFObserver::ECCPDtmfStringSendingCompleted, 
                     KErrNone, 
                     iDtmfLex.Peek() );
            break;
            }
        
        default:
            {
            SVPDEBUG1( "CSVPSessionBase::InbandDtmfEventOccurred Default" )
            break;
            }
        }
    
    SVPDEBUG1( "CSVPSessionBase::InbandDtmfEventOccurred Out" )
    }

  
// ---------------------------------------------------------------------------
// CSVPSessionBase::ConstructAudioStreamsL
// ---------------------------------------------------------------------------
//
void CSVPSessionBase::ConstructAudioStreamsL()
    {
    SVPDEBUG1( "CSVPSessionBase::ConstructAudioStreamsL In" )
    
    // OUT STREAM
    // create "audio out" stream first, puts audioOutStream to cleanupstack
    CMceAudioStream* audioOutStream = CMceAudioStream::NewLC();     // CS: 1
    // create mic source
    CMceMicSource* mic = CMceMicSource::NewLC();                    // CS: 2
    // set source for "audio out" stream, in this case it's microphone
    // there can be only one source at a time.
    audioOutStream->SetSourceL( mic );
    // pop mic from cleanupstack
    CleanupStack::Pop( mic );                                       // CS: 1
    // create rtp sink for mic
    CMceRtpSink* rtpSink = CMceRtpSink::NewLC();                    // CS: 2
    audioOutStream->AddSinkL( rtpSink );
    CleanupStack::Pop( rtpSink );                                   // CS: 1
    
    // IN STREAM
    // create "audio in" stream
    CMceAudioStream* audioInStream = CMceAudioStream::NewLC();      // CS: 2
    // create rtp source for "audio in" stream
    CMceRtpSource* rtpSource = CMceRtpSource::NewLC(
            KSvpJitterBufferLength, KSvpJitterBufferThreshold, 
            KSvpStandbyTimerInMillisecs );                          // CS: 3
    audioInStream->SetSourceL( rtpSource );
    // pop rtp source from cleanupstack
    CleanupStack::Pop( rtpSource );                                 // CS: 2
    // create speaker sink for "audio in" stream
    CMceSpeakerSink* speakerSink = CMceSpeakerSink::NewLC();        // CS: 3
    audioInStream->AddSinkL( speakerSink );
    // pop speaker from cleanupstack
    CleanupStack::Pop( speakerSink );                               // CS: 2
    
    // Bind "audio out" stream to "audio in" stream. Ownership of the stream
    // is transferred thus we must pop the instream from CS.
    audioOutStream->BindL( audioInStream );
    CleanupStack::Pop( audioInStream );                             // CS: 1
    
    // Add stream to session. Note that the ownership is transferred thus
    // we pop the audioOutStream in this phase.
    iSession->AddStreamL( audioOutStream );
    CleanupStack::Pop( audioOutStream );                            // CS: 0
    
    // establish Old way hold support 
    iSession->SetModifierL( KMceMediaDirection,
            KMceMediaDirectionWithAddress );
    
    // preconditions are set so that long negotiation is taken in to use
    // when establishing secure call
    if ( SecureMandatory() || SecurePreferred() )
        {
        iSession->SetModifierL( KMceSecPreconditions, 
                                KMcePreconditionsE2ESupported );
        }
    else
        {
        // else modify preconditions off
        iSession->SetModifierL( KMcePreconditions, 
                                KMcePreconditionsNotUsed );
        }
    
    SVPDEBUG2( "CSVPSessionBase::ConstructAudioStreamsL audioInStream codec count: %d",
            audioInStream->Codecs().Count() )
    SVPDEBUG2( "CSVPSessionBase::ConstructAudioStreamsL audioOutStream codec count: %d",
            audioOutStream->Codecs().Count() )
    
    // add codecs to audiostream
    iSVPUtility.SetAudioCodecsMOL(
            iVoIPProfileId, *audioInStream, iKeepAliveValue );

    // set same local media port from InStream to OutStream 
    audioOutStream->SetLocalMediaPortL( audioInStream->LocalMediaPort() );
    iSVPUtility.UpdateJitterBufferSizeL( *rtpSource );
    
    // set MMF priorities and preferences to codecs
    SVPDEBUG1( "CSVPSessionBase::ConstructAudioStreamsL Set MMF priorities" )
    iSVPUtility.SetDtmfMode( SVPAudioUtility::SetPriorityCodecValuesL(
            *audioInStream, *audioOutStream ) );
    
    SVPDEBUG1( "CSVPSessionBase::ConstructAudioStreamsL Out" )
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::HangUp
// ---------------------------------------------------------------------------
//
TInt CSVPSessionBase::HangUp()
    {
    SVPDEBUG1( "CSVPSessionBase::HangUp In" )
    
    TInt err( KErrNone );
    
    if ( CMceSession::EEstablished == iSession->State() )
        {
        SVPDEBUG1( "CSVPSessionBase::HangUp() TerminateL" )
        
        TRAP( err, iSession->TerminateL() );
        
        if ( !err )
            {
            ExecCbCallStateChanged( MCCPCallObserver::ECCPStateDisconnecting );
            
	        // IsAttended() and different timer values needed for 
            // attended transfer when there is also one waiting call queued
            // and ringing tone for queued session needs to be played
            TInt timerID = 0;
            TInt delay = 0;
            
            if ( IsAttended() )
            	{
	            delay = KSVPTerminatingTime;
	            timerID = KSVPTerminationTimerExpired;
            	}
            else{
	            delay = KSVPMoHangupTerminatingTime;
	            timerID = KSVPHangUpTimerExpired;
            	}
            
            TRAPD( errTimer, StartTimerL( delay, timerID ) );
            
            if ( errTimer )
                {
                SVPDEBUG2("CSVPSessionBase::HangUp timer leave with error code %d",
                        errTimer )
                }
            else
            	{
            	iAlreadyTerminating = ETrue;
            	}
            }   
        }
    else
        {
        SVPDEBUG2( "CSVPSessionBase::HangUp() Wrong state: %d", iSession->State() )
        
        StopTimers();
        ExecCbCallStateChanged( MCCPCallObserver::ECCPStateIdle );
        }
    
    SVPDEBUG2( "CSVPSessionBase::HangUp Out return=%d", err )
    return err;
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::Hold
// ---------------------------------------------------------------------------
//
TInt CSVPSessionBase::Hold()
    {
    SVPDEBUG1( "CSVPSessionBase::Hold In" )
    
    TInt err( KErrNone );
    
    if ( CMceSession::EEstablished == iSession->State() )
        {
        if ( !iHoldController )
            {
            TRAP( err, iHoldController = 
                    CSVPHoldController::NewL( *iSession, 
                                              iContainer, 
                                              this,
                                              IsMobileOriginated() ) );
            if ( KErrNone != err )
                {
                return err;
                }
            }
        
        iHoldController->Muted( iMuted );
        err = iHoldController->HoldSession( iSession );
        
        if ( KErrNone != err )
            {
            ExecCbErrorOccurred( ECCPLocalHoldFail );
            }
        else
            {
            TRAP( err, StartTimerL( KSVPHoldExpirationTime,
                    KSVPHoldTimerExpired ) );
            
            SVPDEBUG2( "CSVPSessionBase::Hold - Started Expire timer, err %i",
                       err )
            }
        }
    
    else
        {
        ExecCbErrorOccurred( ECCPLocalHoldFail );
        }
    
    SVPDEBUG2( "CSVPSessionBase::Hold Out return=%d", err )
    return err;
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::Resume
// ---------------------------------------------------------------------------
//
TInt CSVPSessionBase::Resume()
    {
    SVPDEBUG1( "CSVPSessionBase::Resume In" )
    
    TInt err( KErrNone );
    
    if ( !iHoldController )
        {
        TRAP( err, iHoldController = CSVPHoldController::NewL(
                *iSession, iContainer, this, IsMobileOriginated() ) );
        
        if ( KErrNone != err )
            {
            SVPDEBUG2( "CSVPSessionBase::Resume Out return(err)=%d", err )
            return err;
            }
        }
    
    iHoldController->Muted( iMuted );  
    err = iHoldController->ResumeSession( iSession );
    
    if ( KErrNone != err )
        {
        ExecCbErrorOccurred( ECCPLocalResumeFail );
        }
    else
        {
        TRAP( err, StartTimerL( KSVPResumeExpirationTime,
                KSVPResumeTimerExpired ) );
        SVPDEBUG2("CSVPSessionBase::Resume - Started Expire timer, err %i",
                   err );
        }
        
    SVPDEBUG2( "CSVPSessionBase::Resume Out return=%d", err )
    return err;  
    }
        
// ---------------------------------------------------------------------------
// CSVPSessionBase::IncomingRequest
// ---------------------------------------------------------------------------
//
TInt CSVPSessionBase::IncomingRequest( CMceInSession& aUpdatedSession )
    {
    SVPDEBUG1( "CSVPSessionBase::IncomingRequest In" )

    // Check if own request is ongoing
    if ( iHoldController )
        {
        iHoldController->CheckCrossOver( *this );
        }
    
    if ( &aUpdatedSession )
        {
        //Because there is an update we have to discard everything about ICMP -3
        StopTimer ( KSVPICMPErrorTimerExpired );
        if ( !IsErrorInULandDLFirstTime() )
            {
            SetErrorInULandDLFirstTime( ETrue );
            }
        
        TInt err( KErrNone );
        
        if ( !iHoldController )
            {
            TRAP( err, iHoldController = CSVPHoldController::NewL(
                    *iSession, iContainer, this, IsMobileOriginated() ) );
            
            if ( KErrNone != err )
                {
                return err;
                }
            
            TBool sessionUpdateOngoing = ETrue;
            
            // Most likely a hold case, MT needs to set the keepalive
            TRAP_IGNORE( SetRtpKeepAliveL( &aUpdatedSession,
                    sessionUpdateOngoing ) );
            }
        
        SVPDEBUG2( "CSVPSessionBase::IncomingRequest iSession: 0x%x", iSession )
        
        // Update changed session to the transfercontroller
        if ( iTransferController )
            {
            iTransferController->SetMceSessionObject( &aUpdatedSession );
            }
        
        iHoldController->Muted( iMuted );
        
        TInt status = iHoldController->IncomingRequest( &aUpdatedSession );
        SVPDEBUG2( "CSVPSessionBase::IncomingRequest Out return=%d", status )
        return status;
        }
    else
        {
        SVPDEBUG1( "CSVPSessionBase::IncomingRequest Out illegal aUpdatedSession" )
        return KErrArgument;
        }
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::HoldController
// ---------------------------------------------------------------------------
//    
CSVPHoldController& CSVPSessionBase::HoldController() const
    {
    return *iHoldController;
    }
    
// ---------------------------------------------------------------------------
// CSVPSessionBase::HasHoldController
// ---------------------------------------------------------------------------
//
TBool CSVPSessionBase::HasHoldController() const
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
// CSVPSessionBase::HandleSessionStateChanged
// ---------------------------------------------------------------------------
//
void CSVPSessionBase::HandleSessionStateChanged( CMceSession& aSession )
    {
    SVPDEBUG1( "CSVPSessionBase::HandleSessionStateChanged In" )
    

    if ( iHoldController )
        {
        if ( iHoldController->HoldInProgress() )
            {
            SVPDEBUG1( "CSVPSessionBase::HandleSessionStateChanged HoldInProgress" )
            
            StopTimers();
            iHoldController->Muted( iMuted );
            TInt err = iHoldController->ContinueHoldProcessing( aSession );
            
            if ( KErrNone != err )
                {
                SVPDEBUG2( "CSVPSessionBase::HandleSessionStateChanged, err: %d", err )
                ExecCbErrorOccurred( ECCPLocalHoldFail );
                }
            }
        else if ( iHoldController->ResumeFailed() )
            {
            // Must terminate session; timer expired
            SVPDEBUG1( "CSVPSessionBase::HandleSessionStateChanged Resume failed - Terminating" )
                       
            TRAPD( err, iSession->TerminateL() );
            
            if ( !err )
                {
                SVPDEBUG1( "CSVPSessionBase::HandleSessionStateChanged Disconnecting" )
                ExecCbCallStateChanged( MCCPCallObserver::ECCPStateDisconnecting );
                
                TRAP( err, StartTimerL( KSVPMoHangupTerminatingTime,
                        KSVPHangUpTimerExpired ) );
                
                if ( KErrNone != err )
                    {
                    SVPDEBUG2("CSVPSessionBase::HandleSessionStateChanged Hangup-timer error %d",
                            err )
                    }
                }
            else
                {
                SVPDEBUG1( "CSVPSessionBase::HandleSessionStateChanged - Term ERROR!" )
                }
            }
        else if ( iHoldController->HoldFailed() )
            {
            // Hold timer expired; no actions needed here
            SVPDEBUG1( "CSVPSessionBase::HandleSessionStateChanged: Hold request failed - nothing to do" )
            }
        else
            {
            SVPDEBUG1( "CSVPSessionBase::HandleSessionStateChanged - else" )
            }
        }
    
    SVPDEBUG1( "CSVPSessionBase::HandleSessionStateChanged Out" )
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::HandleStreamStateChange
// ---------------------------------------------------------------------------
//
void CSVPSessionBase::HandleStreamStateChange( CMceMediaStream& aStream )
    {
    SVPDEBUG2( "CSVPSessionBase::HandleStreamStateChange(aStream)  stream state= %d",
            aStream.State() )
    SVPDEBUG2( "CSVPSessionBase::HandleStreamStateChange(aStream)  stream  type= %d",
        aStream.Type() )
    SVPDEBUG2( "CSVPSessionBase::HandleStreamStateChange(aStream) session state= %d",
        aStream.Session()->State() )

    if ( CMceMediaStream::EStreaming == aStream.State() &&
         CMceSession::EOffering == aStream.Session()->State() )
        {
        SVPDEBUG1( "CSVPSessionBase::HandleStreamStateChange(aStream) early media started" )
        ExecCbCallEventOccurred( MCCPCallObserver::ECCCSPEarlyMediaStarted );
        iEarlyMediaOngoing = ETrue;
        }
    TBool bothStreamsDisabled ( ETrue );
    TRAP_IGNORE( bothStreamsDisabled = IsBothStreamsDisabledL() );
    if ( bothStreamsDisabled &&
         CMceSession::EEstablished == aStream.Session()->State() )
        {
        SetErrorInULandDLFirstTime( EFalse ); 
        TRAP_IGNORE( StartTimerL( KSVPICMPErrorTime, KSVPICMPErrorTimerExpired ) )
        }
      else if ( !bothStreamsDisabled && !IsErrorInULandDLFirstTime() )
          {
          SetErrorInULandDLFirstTime( ETrue );
          StopTimer ( KSVPICMPErrorTimerExpired );
          }
    SVPDEBUG1( "CSVPSessionBase::HandleStreamStateChange(aStream) Out" )
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::HandleStreamStateChange
// ---------------------------------------------------------------------------
// 
void CSVPSessionBase::HandleStreamStateChange( CMceMediaStream& aStream,
        CMceMediaSink& aSink )
    {
    SVPDEBUG1( "CSVPSessionBase::HandleStreamStateChange(aStream,aSink) In" )
    
    CMceSession::TState sessionState = CMceSession::ETerminated;
    
    if ( &aStream )
        {
        SVPDEBUG2( "CSVPSessionBase::HandleStreamStateChange(aStream,aSink) stream state=%d",
                aStream.State() )
        SVPDEBUG2( "CSVPSessionBase::HandleStreamStateChange(aStream,aSink) stream type=%d",
                aStream.Type() )
        }
    
    if ( &aStream && aStream.Session() )
        {
        sessionState = aStream.Session()->State();
        SVPDEBUG2( "CSVPSessionBase::HandleStreamStateChange(aStream,aSink) session state=%d", sessionState )
        }
    
    if ( &aSink )
        {
        SVPDEBUG2( "CSVPSessionBase::HandleStreamStateChange(aStream,aSink) is sink enabled=%d",
                aSink.IsEnabled() )

        if ( HasHoldController() && ESVPConnected == HoldController().HoldState() && 
             !aSink.IsEnabled() && CMceSession::EEstablished == sessionState )
            {
            // Hold state is connected but sink is disabled -> try enable after a while
            SVPDEBUG1( "CSVPSessionBase::HandleStreamStateChange - Resume ICMP, Sink" )
            TRAP_IGNORE( StartTimerL( KSVPSinkResumeICMPErrorTime, KSVPSinkResumeICMPErrorTimerExpired ) )
            }
        }
    
    SVPDEBUG1( "CSVPSessionBase::HandleStreamStateChange(aStream,aSink) Out" )
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::HandleStreamStateChange
// ---------------------------------------------------------------------------
//
void CSVPSessionBase::HandleStreamStateChange( CMceMediaStream& aStream,
        CMceMediaSource& aSource )
    {
    SVPDEBUG1( "CSVPSessionBase::HandleStreamStateChange(aStream,aSource) In" )
    
    CMceSession::TState sessionState = CMceSession::ETerminated;
    
    if ( &aStream )
        {
        SVPDEBUG2( "CSVPSessionBase::HandleStreamStateChange(aStream,aSource) stream state=%d",
                aStream.State() )
        SVPDEBUG2( "CSVPSessionBase::HandleStreamStateChange(aStream,aSource) stream type=%d",
                aStream.Type() )
        }
    
    if ( &aStream && aStream.Session() )
        {
        sessionState = aStream.Session()->State();
        SVPDEBUG2( "CSVPSessionBase::HandleStreamStateChange(aStream,aSource) session state=%d", sessionState )
        }
    
    if ( &aSource )
        {
        SVPDEBUG2( "CSVPSessionBase::HandleStreamStateChange(aStream,aSource) is source enabled=%d",
                aSource.IsEnabled() )

        if ( HasHoldController() && ESVPConnected == HoldController().HoldState() && 
             !aSource.IsEnabled() && CMceSession::EEstablished == sessionState )
            {
            // Hold state is connected but source is disabled -> try enable after a while
            SVPDEBUG1( "CSVPSessionBase::HandleStreamStateChange - Resume ICMP, Source" )
            TRAP_IGNORE( StartTimerL( KSVPSourceResumeICMPErrorTime, KSVPSourceResumeICMPErrorTimerExpired ) )
            }
        }
    
    SVPDEBUG1( "CSVPSessionBase::HandleStreamStateChange(aStream,aSource) Out" )
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::SessionLocallyHeld, from MSVPHoldObserver
// ---------------------------------------------------------------------------
// 
void CSVPSessionBase::SessionLocallyHeld()
    {
    SVPDEBUG1( "CSVPSessionBase::SessionLocallyHeld In" )
    
    ExecCbCallStateChanged( MCCPCallObserver::ECCPStateHold );
    iRtpObserver.ResetSessionInObserving( this );
    
    SVPDEBUG1( "CSVPSessionBase::SessionLocallyHeld Out" )
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::SessionLocallyResumed, from MSVPHoldObserver
// ---------------------------------------------------------------------------
// 
void CSVPSessionBase::SessionLocallyResumed()
    {
    SVPDEBUG1( "CSVPSessionBase::SessionLocallyResumed In" )
    
    ExecCbCallStateChanged( MCCPCallObserver::ECCPStateConnected );
    iRtpObserver.ResetSessionInObserving( this );
    
    SVPDEBUG1( "CSVPSessionBase::SessionLocallyResumed Out" )
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::SessionRemoteHeld, from MSVPHoldObserver
// ---------------------------------------------------------------------------
// 
void CSVPSessionBase::SessionRemoteHeld()
    {
    SVPDEBUG1( "CSVPSessionBase::SessionRemoteHeld In" )
    
    ExecCbCallEventOccurred( MCCPCallObserver::ECCPRemoteHold );
    iRtpObserver.ResetSessionInObserving( this );
    
    SVPDEBUG1( "CSVPSessionBase::SessionRemoteHeld Out" )
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::SessionRemoteResumed, from MSVPHoldObserver
// ---------------------------------------------------------------------------
// 
void CSVPSessionBase::SessionRemoteResumed()
    {
    SVPDEBUG1( "CSVPSessionBase::SessionRemoteResumed In" )
    
    ExecCbCallEventOccurred( MCCPCallObserver::ECCPRemoteResume );
    iRtpObserver.ResetSessionInObserving( this );
    
    SVPDEBUG1( "CSVPSessionBase::SessionRemoteResumed Out" )
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::HoldRequestFailed, from MSVPHoldObserver
// ---------------------------------------------------------------------------
// 
void CSVPSessionBase::HoldRequestFailed()
    {
    SVPDEBUG1( "CSVPSessionBase::HoldRequestFailed In" )
    
    ExecCbErrorOccurred( ECCPLocalHoldFail );
    
    SVPDEBUG1( "CSVPSessionBase::HoldRequestFailed Out" )
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::ResumeRequestFailed, from MSVPHoldObserver
// ---------------------------------------------------------------------------
// 
void CSVPSessionBase::ResumeRequestFailed()
    {
    SVPDEBUG1( "CSVPSessionBase::ResumeRequestFailed In" )
    
    ExecCbErrorOccurred( ECCPLocalResumeFail );
    
    SVPDEBUG1( "CSVPSessionBase::ResumeRequestFailed Out" )
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::RemoteParty
// ---------------------------------------------------------------------------
// 
const TDesC& CSVPSessionBase::RemoteParty() const
    {
    delete iRecipient;
    iRecipient = NULL;
    
    // RemotePartyL is used to handle leave situations
    TRAPD( remoteErr, RemotePartyL() );
    
    if ( KErrNone != remoteErr )
        {
        // something went wrong, return null
        SVPDEBUG2( "CSVPSessionBase::RemoteParty remoteErr=%d", remoteErr )
        return KNullDesC;
        }
    else
        {
        return *iRecipient;
        }
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::RemotePartyL
// ---------------------------------------------------------------------------
// 
void CSVPSessionBase::RemotePartyL() const
    {
    SVPDEBUG1( "CSVPSessionBase::RemotePartyL In" )
    
    // fetch recipient from Mce, convert recipient to 16-bit descriptor
    if ( IsMobileOriginated() )
        {
        SVPDEBUG1( "CSVPSessionBase::RemotePartyL, mo case, Fetch: RECIPIENT" )
        iRecipient = HBufC::NewL( iSession->Recipient().Length() );
        iRecipient->Des().Copy( iSession->Recipient() );
 
        // remove all extra parameters from recipient address
        TInt index = iRecipient->Des().FindF( KSVPSemiColon );
        
        if ( KErrNotFound != index )
            {
            iRecipient->Des().Delete( index, iRecipient->Length() );
            // remove left bracket if exists
            TInt bracketLocation = iRecipient->Locate( KSVPLeftBracket );
            if ( KErrNotFound != bracketLocation )
                {
                iRecipient->Des().Delete( bracketLocation, 1 );
                }
            }
        
        // check if anonymous address
        index = iRecipient->Des().FindF( KSVPAnonymous );
        if ( KErrNotFound != index )
            {
            // Anonymous address case
            SVPDEBUG2( "CSVPSessionBase::RemotePartyL: Anonymous = %d", index )
            iRecipient->Des().Copy( KNullDesC );
            }
        }
    else 
        {
        SVPDEBUG1( "CSVPSessionBase::RemotePartyL, mt case, Fetch: ORIGINATOR" )
        HBufC* uri = CSVPUriParser::ParseRemotePartyUriL( 
            iSession->Originator() );
        delete iRecipient;
        iRecipient = uri;
        uri = NULL;
        }
    
    SVPDEBUG1( "CSVPSessionBase::RemotePartyL Out" )
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::RemotePartyName
// ---------------------------------------------------------------------------
// 
const TDesC& CSVPSessionBase::RemotePartyName()
    {
    SVPDEBUG1( "CSVPSessionBase::RemotePartyName()" )
    
    HBufC* displayName = NULL;
    TRAPD( err, displayName = CSVPUriParser::ParseDisplayNameL( 
        iSession->Originator() ) )

    if ( err || NULL == displayName ) 
        {
        SVPDEBUG1( "CSVPSessionBase::RemotePartyName, return KNullDesC" )
        delete displayName;
        displayName = NULL;
        return KNullDesC;
        }
    else
        {
        delete iDisplayName;
        iDisplayName = displayName;
        displayName = NULL;
        return *iDisplayName;
        }    
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::DialledParty
// ---------------------------------------------------------------------------
// 
const TDesC& CSVPSessionBase::DialledParty() const
    {
    SVPDEBUG1( "CSVPSessionBase::DialledParty not implemented" )  
    return KNullDesC;
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::State
// ---------------------------------------------------------------------------
//
MCCPCallObserver::TCCPCallState CSVPSessionBase::State() const
    {
    SVPDEBUG1( "CSVPSessionBase::State In" )
    SVPDEBUG2( "CSVPSessionBase::State iSession->State()=%d", iSession->State() )
    
    // ccpState can be safely initialized with StatusIdle
    MCCPCallObserver::TCCPCallState ccpState =
        MCCPCallObserver::ECCPStateIdle;
    
    switch ( iSession->State() )
        {
        case CMceSession::EIdle:
            {
            SVPDEBUG1( "CSVPSessionBase::State EIdle received" )
            ccpState = MCCPCallObserver::ECCPStateIdle;
            break;
            }
        case CMceSession::EOffering:
            {
            SVPDEBUG1( "CSVPSessionBase::State EOffering received" )
            ccpState = MCCPCallObserver::ECCPStateConnecting;
            break;
            }
        case CMceSession::EIncoming:
            {
            SVPDEBUG1( "CSVPSessionBase::State EIncoming received" )
            ccpState = MCCPCallObserver::ECCPStateAnswering;
            break;
            }
        case CMceSession::EReserving:
            {
            SVPDEBUG1( "CSVPSessionBase::State EReserving received" )
            ccpState = MCCPCallObserver::ECCPStateConnecting;
            break;
            }
        case CMceSession::EAnswering: 
            {
            SVPDEBUG1( "CSVPSessionBase::State EAnswering received" )
            ccpState = MCCPCallObserver::ECCPStateAnswering;
            break;
            }
        case CMceSession::EProceeding:
            {
            SVPDEBUG1( "CSVPSessionBase::State EProceeding received" )
            ccpState = MCCPCallObserver::ECCPStateConnecting;
            break;
            }
        case CMceSession::EEstablished:
            {
            SVPDEBUG1( "CSVPSessionBase::State EEstablished received" )
            ccpState = MCCPCallObserver::ECCPStateConnected;
            if( iHoldController && ESVPOnHold == iHoldController->HoldState() )
                {
                SVPDEBUG1( "    CSVPSessionBase::State Established and holded" )
                ccpState = MCCPCallObserver::ECCPStateHold;
                }
            break;
            }
        case CMceSession::ECancelling:
            {
            SVPDEBUG1( "CSVPSessionBase::State ECancelling received" )
            ccpState = MCCPCallObserver::ECCPStateIdle;  
            break;
            }
        case CMceSession::ETerminating:
            {
            SVPDEBUG1( "CSVPSessionBase::State ETerminating received" )
            ccpState = MCCPCallObserver::ECCPStateDisconnecting;
            break;
            }
        case CMceSession::ETerminated:
            {
            SVPDEBUG1( "CSVPSessionBase::State ETerminated received" )
            
            if ( MCCPCallObserver::ECCPStateDisconnecting == iSessionState )
                {
                SVPDEBUG1( "CSVPSessionBase::State Saving state for termination timer" )
                ccpState = MCCPCallObserver::ECCPStateDisconnecting;
                }
            else
                {
                ccpState = MCCPCallObserver::ECCPStateIdle;     
                }
            
            break;
            }
        default:
            {
            SVPDEBUG1( "CSVPSessionBase::State DEFAULT" )
            // This block should never be reached.
            __ASSERT_DEBUG( EFalse, User::Panic(
                    KSVPName, KSVPPanicBadArgument ) );
            break;
            }
        }
    
    SVPDEBUG2( "CSVPSessionBase::State Out return=%d", ccpState )
    return ccpState;
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::GetKeepAliveTime
// ---------------------------------------------------------------------------
//    
TInt CSVPSessionBase::GetKeepAliveTime()
    {
    return iKeepAliveValue;
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::Type
// ---------------------------------------------------------------------------
//
TUid CSVPSessionBase::Uid() const
    {
    return KSVPImplementationUid;
    }


// ---------------------------------------------------------------------------
// CSVPSessionBase::Conference
// ---------------------------------------------------------------------------
//
MCCPConferenceCall* CSVPSessionBase::ConferenceProviderL( 
    const MCCPConferenceCallObserver& /*aObserver*/ )
    {
	return NULL;    
	}
	
// ---------------------------------------------------------------------------
// CSVPSessionBase::IsSecured
// ---------------------------------------------------------------------------
//
TBool CSVPSessionBase::IsSecured() const
    {
	SVPDEBUG2( "CSVPSessionBase::IsSecured: %d", iSecured )
	return iSecured;    
	}

// ---------------------------------------------------------------------------
// CSVPSessionBase::IsMobileOriginated
// ---------------------------------------------------------------------------
//
TBool CSVPSessionBase::IsMobileOriginated() const
    {
	SVPDEBUG1( "CSVPSessionBase::IsMobileOriginated" )
	return EFalse;    
	}

// ---------------------------------------------------------------------------
// CSVPSessionBase::IsCallForwarded
// ---------------------------------------------------------------------------
//
TBool CSVPSessionBase::IsCallForwarded() const
    {
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::Swap
// ---------------------------------------------------------------------------
//
TInt CSVPSessionBase::Swap()
    {
    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::Caps
// ---------------------------------------------------------------------------
// 
MCCPCallObserver::TCCPCallControlCaps CSVPSessionBase::Caps() const
    {
    SVPDEBUG1( "CSVPSessionBase::Caps return ECCPCapsPSCall" )
    return MCCPCallObserver::ECCPCapsPSCall;
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::Release
// ---------------------------------------------------------------------------
//    
TInt CSVPSessionBase::Release()
    {
    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::Dial
// ---------------------------------------------------------------------------
//
TInt CSVPSessionBase::Dial()
    {
    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::Answer
// ---------------------------------------------------------------------------
//
TInt CSVPSessionBase::Answer()
    {
    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::Cancel
// ---------------------------------------------------------------------------
//
TInt CSVPSessionBase::Cancel()
    {
    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::Reject
// ---------------------------------------------------------------------------
// 
TInt CSVPSessionBase::Reject()
    {
    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::Queue
// ---------------------------------------------------------------------------
// 
TInt CSVPSessionBase::Queue()
    {
    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::Ringing
// ---------------------------------------------------------------------------
// 
TInt CSVPSessionBase::Ringing()
    {
    return KErrNotSupported;  
    }
   
// ---------------------------------------------------------------------------
// CSVPSessionBase::TransferProvider
// ---------------------------------------------------------------------------
//
MCCPTransferProvider* CSVPSessionBase::TransferProviderL(
     const MCCPTransferObserver& aObserver )
    {
    SVPDEBUG1( "CSVPSessionBase::TransferProviderL In" )
    
    if ( !iTransferController )
        {
        iTransferController = CSVPTransferController::NewL( 
                                                        iSession,
                                                        this,
                                                        iContainer, 
                                                        *this );
        iTransferController->AddObserverL( aObserver );
        }
    
    SVPDEBUG1( "CSVPSessionBase::TransferProviderL Out" )
    return iTransferController;
	}

// ---------------------------------------------------------------------------
// CSVPSessionBase::ForwardProviderL
// ---------------------------------------------------------------------------
//
MCCPForwardProvider* CSVPSessionBase::ForwardProviderL(
     const MCCPForwardObserver& /*aObserver*/ )
    {
    return NULL;
    }


// transfer observer

// ---------------------------------------------------------------------------
// From MSVPTransferObserver
// Notifier for succesful transfer.
// ---------------------------------------------------------------------------
// 
void CSVPSessionBase::TransferNotification( TInt aNotifyCode )
    {
    SVPDEBUG2( "CSVPSessionBase::TransferNotification In, aNotifyCode: %d",
        aNotifyCode );
    
    if ( ESVPTransferOKHangUp == aNotifyCode )
        {
        SVPDEBUG1( "CSVPSessionBase::TransferNotification: ESVPTransferOKHangUp" )
        // Transfer complete - hang up current session.
        HangUp();
        }
    else if ( ESVPTransferDecline == aNotifyCode )
        {
        SVPDEBUG1( "CSVPSessionBase::TransferNotification: ESVPTransferDecline" )
        // Inform application - transfer was rejected by the other party,
        // not a failure, to be precise
        ExecCbErrorOccurred( ECCPTransferFailed );
        }
    else if ( ESVPIncomingRefer == aNotifyCode )
        {
        SVPDEBUG1( "CSVPSessionBase::TransferNotification: ESVPIncomingRefer" )
        // Currently no CallEventOccurred notify to the observer
        }
    else
        {
        // Internal transfer error if comes here
        SVPDEBUG1( "CSVPSessionBase::TransferNotification: Unknown notify" )
        }
    
    SVPDEBUG1( "CSVPSessionBase::TransferNotification Out" )
    }

// ---------------------------------------------------------------------------
// From MSVPTransferObserver
// Notifier for transfer failure.
// ---------------------------------------------------------------------------
//
#ifdef _DEBUG
void CSVPSessionBase::TransferFailed( TInt aError )
#else
void CSVPSessionBase::TransferFailed( TInt /*aError*/ )
#endif  // _DEBUG
    {
    SVPDEBUG1( "CSVPSessionBase::TransferFailed In" )
    SVPDEBUG2(" CSVPSessionBase::TransferFailed aError=%d", aError )
    
    // Inform application about the error
    ExecCbErrorOccurred( ECCPTransferFailed );
    
    SVPDEBUG1( "CSVPSessionBase::TransferFailed Out" )
    }

//  from CCP DTMF provider
// ---------------------------------------------------------------------------
// CSVPSessionBase::CancelDtmfStringSending
// ---------------------------------------------------------------------------
// 
TInt CSVPSessionBase::CancelDtmfStringSending()
    {
    SVPDEBUG1( "CSVPSessionBase::CancelDtmfStringSending In" )
    
    TInt dtmfErr( KErrNone );
    
    if ( !iSVPUtility.GetDTMFMode() ) 
        {
        dtmfErr = KErrNotFound;
        if ( iEventGenerator )
            {
            iEventGenerator->StopEvents();
            dtmfErr = KErrNone;
            }
        }
    else 
        {
        const RPointerArray<CMceMediaStream>& streams = iSession->Streams();
        TInt count = streams.Count();
        while( count )
            {
            count--;
            CMceMediaStream& mediaStream = *streams[ count ];
            if ( SVPAudioUtility::DtmfActionCapableStream( mediaStream ) )
                {
                TRAP( dtmfErr, mediaStream.Source()->CancelDtmfToneSequenceL() );
                }
            else
                {
                dtmfErr = KErrNotSupported;
                }
            
            if ( KErrNone != dtmfErr )
                {
                return dtmfErr;
                }
            }
        }
    
    SVPDEBUG2( "CSVPSessionBase::CancelDtmfStringSending Out return=%d", dtmfErr )
    return dtmfErr;
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::StartDtmfTone
// ---------------------------------------------------------------------------
//
TInt CSVPSessionBase::StartDtmfTone( const TChar aTone )
    {
    SVPDEBUG1( "CSVPSessionBase::StartDtmfTone In" )
    
    TInt dtmfErr( KErrNone );
    
    if ( iSVPUtility.GetDTMFMode() )
        {
        SVPDEBUG1( "CSVPSessionBase::StartDtmfTone DTMF Outband" )
        // fetch streams
        const RPointerArray<CMceMediaStream>& streams = iSession->Streams();
        TInt count = streams.Count();
        while ( count )
            {
            count--;
            CMceMediaStream& mediaStream = *streams[ count ];
            if ( SVPAudioUtility::DtmfActionCapableStream( mediaStream ) )
                {
                TRAP( dtmfErr, mediaStream.Source()->StartDtmfToneL( aTone ) );
                }
            else
                {
                dtmfErr = KErrNotSupported;
                }
            
            if ( KErrNone != dtmfErr )
                {
                SVPDEBUG2( "CSVPSessionBase::StartDtmfToneL dtmfErr: %d",
                    dtmfErr )
                
                return dtmfErr;
                }
            }
        }
    else
        {
        SVPDEBUG1( "CSVPSessionBase::StartDtmfTone DTMF Inband" )
        iDtmfTone = aTone;
        DtmfObserver().HandleDTMFEvent( MCCPDTMFObserver::ECCPDtmfManualStart, 
                                        KErrNone, 
                                        aTone );
        }
    
    SVPDEBUG2( "CSVPSessionBase::StartDtmfTone Out return=%d", dtmfErr )
    return dtmfErr;
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::StopDtmfTone
// ---------------------------------------------------------------------------
//
TInt CSVPSessionBase::StopDtmfTone()
    {
    SVPDEBUG1( "CSVPSessionBase::StopDtmfTone In" )
    
    TInt dtmfErr( KErrNone );
    
    if ( iSVPUtility.GetDTMFMode() )
        {
        const RPointerArray<CMceMediaStream>& streams = iSession->Streams();
        TInt count = streams.Count();
        while( count )
            {
            count--;
            CMceMediaStream& mediaStream = *streams[ count ];
            if ( SVPAudioUtility::DtmfActionCapableStream( mediaStream ) )
                {
                TRAP( dtmfErr, mediaStream.Source()->StopDtmfToneL() );
                }
            // NOP with inband.
            
            if ( KErrNone != dtmfErr )
                {
                SVPDEBUG2( "CSVPSessionBase::StopDtmfTone dtmfErr: %d",
                    dtmfErr )
                
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

    SVPDEBUG2( "CSVPSessionBase::StopDtmfTone Out return=%d", dtmfErr )
    return dtmfErr;
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::SendDtmfToneString
// ---------------------------------------------------------------------------
//
TInt CSVPSessionBase::SendDtmfToneString( const TDesC& aString )
    {
    SVPDEBUG1( "CSVPSessionBase::SendDtmfToneString In" )
    
    TInt dtmfErr( KErrNone );
    TChar dtmfPause('p');
    // MCE calls if outband DTMF.
    // Exception is pause character 'p' which is handled always locally
    if ( !iSVPUtility.GetDTMFMode() ||
         ( aString.Length() == 1 &&
           dtmfPause == aString[0] ) ) 
        {
        SVPDEBUG1( "CSVPSessionBase::SendDtmfToneString Inband" )
        
        delete iDtmfString;
        iDtmfString = NULL;
        TRAP( dtmfErr, iDtmfString = HBufC::NewL( aString.Length() ) );
        if ( KErrNone != dtmfErr )
            {
            return dtmfErr;
            }
                
        *iDtmfString = aString;
        iDtmfLex.Assign( *iDtmfString );
        
        if ( !iEventGenerator )
            {
            TRAP( dtmfErr,
            iEventGenerator = CSVPDTMFEventGenerator::NewL( *this ) );
            }
        
        if ( KErrNone != dtmfErr )
            {
            SVPDEBUG2( "CSVPSessionBase::SendDtmfToneString IB dtmfErr: %d",
                dtmfErr )
            
            return dtmfErr;
            }
        
        // Dtmf pause length is 2500ms
        TBool pauseChar = ( aString.Length() == 1 && 
                            dtmfPause == aString[0] ); 
        // start events
        iEventGenerator->StartDtmfEvents( aString.Length(), pauseChar );
        }
    else
        {
        SVPDEBUG1( "CSVPSessionBase::SendDtmfToneString Outband" )
        
        const RPointerArray<CMceMediaStream>& streams = iSession->Streams();
        TInt count = streams.Count();
        while( count )
            {
            count--;
            CMceMediaStream& mediaStream = *streams[ count ];
            
            if ( SVPAudioUtility::DtmfActionCapableStream( mediaStream ) )
                {
                TRAP( dtmfErr,
                    mediaStream.Source()->SendDtmfToneSequenceL( aString ) );
                }
            else
                {
                dtmfErr = KErrNotSupported;
                }
            
            if ( KErrNone != dtmfErr )
                {
                SVPDEBUG2( "CSVPSessionBase::SendDtmfToneString Out OB dtmfErr: %d", dtmfErr )
                return dtmfErr;
                }
            }
        }
    
    SVPDEBUG2( "CSVPSessionBase::SendDtmfToneString Out return=%d", dtmfErr )
    return dtmfErr;
    }


// from MSVPPropertyWatchObserver
// ---------------------------------------------------------------------------
// CSVPSessionBase::ValueChangedL
// ---------------------------------------------------------------------------
//
void CSVPSessionBase::ValueChangedL( TInt aKey, TInt aValue )
    {
    SVPDEBUG3("CSVPSessionBase::ValueChangedL In aKey: %d, aValue: %d ", aKey, aValue )

    // fetch streams from session
    const RPointerArray<CMceMediaStream>& streams = iSession->Streams();

    switch ( aKey )
        {
        case KTelMicrophoneMuteState:
            {
            PerformMuteChangeL( streams, aValue );
            break;
            }
        default:
            {
            SVPDEBUG1( "CSVPSessionBase::ValueChangedL DEFAULT - NOP" )
            break;
            }
        }
    
    SVPDEBUG1( "CSVPSessionBase::ValueChangedL Out" )
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::PerformMuteChangeL
// ---------------------------------------------------------------------------
//
void CSVPSessionBase::PerformMuteChangeL(
        const RPointerArray<CMceMediaStream>& aStreams, TInt aValue )
    {
    SVPDEBUG1( "CSVPSessionBase::PerformMuteChangeL In" )
    
    if ( CMceSession::EOffering == iSession->State() ||
         iHoldController && ESVPOnHold == iHoldController->HoldState() )
        {
		// call is ringing or on hold, we don't perform mute or unmute
        SVPDEBUG1( "CSVPSessionBase::PerformMuteChangeL, no need for mute nor unmute" )
        }
    else
        {
        for ( TInt i = 0; i < aStreams.Count(); i++ )
            {
            if ( IsMobileOriginated() )
                {
                if ( aStreams[ i ]->Source() )
                    {
                    if ( EPSTelMicMuteOn == aValue )
                        {
                        // mute mic source
                        SVPAudioUtility::DisableMicSourceL(
                                *( aStreams[ i ]->Source() ) );
                        }
                    else
                        {
                        // un-mute mic source            
                        SVPAudioUtility::EnableMicSourceL(
                                *( aStreams[ i ]->Source() ) );
                        }
                    }
                }
            else
                {
                if( CMceSession::EEstablished == iSession->State() )
                    {
                    if ( EPSTelMicMuteOn == aValue )
                        {
                        // mute mic source
                        SVPAudioUtility::DisableMicSourceL(
                                *( aStreams[ i ]->BoundStreamL().Source() ) );
                        }
                    else
                        {
                        // un-mute mic source            
                        SVPAudioUtility::EnableMicSourceL(
                                *( aStreams[ i ]->BoundStreamL().Source() ) );
                        }
                    }
                }
            }
        
        if ( EPSTelMicMuteOff == aValue )
            {
            SVPDEBUG1( "CSVPSessionBase::PerformMuteChangeL - Enable Speaker" )
            SVPAudioUtility::EnableSpeakerSinksL( aStreams );
            }
        }

    if ( EPSTelMicMuteOn == aValue ) { iMuted = ETrue; }
    if ( EPSTelMicMuteOff == aValue ) { iMuted = EFalse; }

    if ( iHoldController )
        {
        SVPDEBUG1( "CSVPSessionBase::PerformMuteChangeL - update mute status" )
        iHoldController->Muted( iMuted );
        }

    SVPDEBUG1( "CSVPSessionBase::PerformMuteChangeL Out" )    
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::PropertyDeleted
// ---------------------------------------------------------------------------
//
#ifdef _DEBUG
void CSVPSessionBase::PropertyDeleted( TInt aKey )
#else
void CSVPSessionBase::PropertyDeleted( TInt /*aKey*/ )
#endif // __DEBUG
    {
    SVPDEBUG2("CSVPSessionBase::PropertyDeleted aKey=%d", aKey )
    }


// from MSVPVolumeUpdateObserver    
// ---------------------------------------------------------------------------
// CSVPSessionBase::VolumeChanged
// ---------------------------------------------------------------------------
//  
void CSVPSessionBase::VolumeChanged( TInt aVolume )
    {
#ifdef _DEBUG
    TRAPD( volErr, VolumeChangedL( aVolume ) );
    SVPDEBUG2("CSVPSessionBase::VolumeChanged volErr=%d", volErr )
#else
    TRAP_IGNORE( VolumeChangedL( aVolume ) )
#endif // _DEBUG
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::VolumeChangedL
// ---------------------------------------------------------------------------
// 
void CSVPSessionBase::VolumeChangedL( TInt aVolume )
    {
    SVPDEBUG1( "CSVPSessionBase::VolumeChangedL In" )
    
    // fetch streams from session
    const RPointerArray<CMceMediaStream>& stream = iSession->Streams();
    
    for ( TInt i = 0; i < stream.Count(); i++ )
        {
        // fetch mediastreams for handling
        CMceMediaStream* mediaStream = stream[ i ];
        CMceMediaStream& boundStream = mediaStream->BoundStreamL();
        
        // fetch sinks from current stream
        const RPointerArray<CMceMediaSink>& sinks = mediaStream->Sinks();
        
        for ( TInt y = 0; y < sinks.Count(); y++ )
            {
            // find speaker sink
            if ( KMceSpeakerSink == sinks[ y ]->Type() )
                {
                SVPDEBUG1( "CSVPSessionBase::VolumeChangedL Speaker found(A)" )
                CMceSpeakerSink* speakerSink =
                        static_cast<CMceSpeakerSink*>( sinks[ y ] );
                // get sink max volume and adjust setVol accordingly
                TInt maxVol = speakerSink->MaxVolumeL();
                TInt setVol = maxVol * aVolume / KMaxPhoneVolume;

                SVPDEBUG2("CSVPSessionBase::VolumeChangedL maxVol=%d", maxVol )
                SVPDEBUG2("CSVPSessionBase::VolumeChangedL aVolume=%d", aVolume )
                SVPDEBUG2("CSVPSessionBase::VolumeChangedL setVol=%d", setVol )

                speakerSink->SetVolumeL( setVol );
                }
            } 
        
        const RPointerArray<CMceMediaSink>& boundSinks = boundStream.Sinks();
        
        for ( TInt z = 0; z < boundSinks.Count(); z++ )
            {
            // find speaker sink from bound stream
            if ( KMceSpeakerSink == boundSinks[ z ]->Type() )
                {
                SVPDEBUG1( "CSVPSessionBase::VolumeChangedL Speaker found(B)" )
                
                CMceSpeakerSink* speakerSink =
                        static_cast<CMceSpeakerSink*>( boundSinks[ z ] );
                // get sink max volume and adjust setVol accordingly
                TInt maxVol = speakerSink->MaxVolumeL();
                TInt setVol = maxVol * aVolume / KMaxPhoneVolume;
                
                SVPDEBUG2("CSVPSessionBase::VolumeChangedL maxVol=%d", maxVol )
                SVPDEBUG2("CSVPSessionBase::VolumeChangedL aVolume=%d", aVolume )
                SVPDEBUG2("CSVPSessionBase::VolumeChangedL setVol=%d", setVol )

                speakerSink->SetVolumeL( setVol );
                }
            }
        }
    
    SVPDEBUG1( "CSVPSessionBase::VolumeChangedL Out" )
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::SecureMandatory
// ---------------------------------------------------------------------------
// 
TBool CSVPSessionBase::SecureMandatory() const
    {
    SVPDEBUG2( "CSVPSessionBase::SecureMandatory=%d", iSecureMandatory )
    return iSecureMandatory;
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::SecurePreferred
// ---------------------------------------------------------------------------
// 
TBool CSVPSessionBase::SecurePreferred() const
    {
    SVPDEBUG2( "CSVPSessionBase::SecurePreferred=%d", iSecurePreferred )
    return iSecurePreferred;
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::SetSecureMandatory
// ---------------------------------------------------------------------------
// 
void CSVPSessionBase::SetSecureMandatory( TBool aSecMandatory )
    {
    iSecureMandatory = aSecMandatory;
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::SetSecurePreferred
// ---------------------------------------------------------------------------
// 
void CSVPSessionBase::SetSecurePreferred( TBool aSecPreferred )
    {
    iSecurePreferred = aSecPreferred;
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::SetCallEventToBeSent
// ---------------------------------------------------------------------------
// 
void CSVPSessionBase::SetCallEventToBeSent( MCCPCallObserver::TCCPCallEvent aCallEventToBeSent )
    {
    SVPDEBUG1( "CSVPSessionBase::SetCallEventToBeSent" )
    iCallEventToBeSent = aCallEventToBeSent;
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::SetParameters
// ---------------------------------------------------------------------------
// 
void CSVPSessionBase::SetParameters( 
    const CCCPCallParameters& /*aNewParams*/ )
    {
    SVPDEBUG1( "CSVPSessionBase::SetParameters - NOP" )
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::Parameters
// ---------------------------------------------------------------------------
// 
const CCCPCallParameters& CSVPSessionBase::Parameters() const
    {
    SVPDEBUG1( "CSVPSessionBase::Parameters In" )
    return *iCallParameters;
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::Tone
// ---------------------------------------------------------------------------
//    
TCCPTone CSVPSessionBase::Tone() const
    {
    SVPDEBUG2( "CSVPSessionBase::Tone iTone: %d", iTone )
    return iTone;
    }
    
// ---------------------------------------------------------------------------
// CSVPSessionBase::SetCryptoContextL
// ---------------------------------------------------------------------------
//
void CSVPSessionBase::SetCryptoContextL()
    {
    SVPDEBUG1( "CSVPSessionBase::SetCryptoContextL In" )
    
    if ( KMceSessionSecure == iSession->Type() )
        {
        // get supported crypto contexts
        if ( IsMobileOriginated() )
            {
            SVPDEBUG1( "CSVPSessionBase::SetCryptoContextL Secure Mo" )
            
            const RArray<TMceCryptoContext>& cryptoContext = 
                static_cast<CMceSecureOutSession*> ( iSession )->
                SupportedCryptoContextsL();
            SVPDEBUG2( "CSVPSessionBase::SetCryptoContextL Supported crypto context count=%d",
                    cryptoContext.Count() )
            
            // set crypto contexts to session
            static_cast<CMceSecureOutSession*> ( iSession )->
                SetCryptoContextsL( cryptoContext );
            }
        else
            {
            SVPDEBUG1( "CSVPSessionBase::SetCryptoContextL Secure Mt" )
            
            const RArray<TMceCryptoContext>& cryptoContext = 
            static_cast<CMceSecureInSession*> ( iSession )->
                SupportedCryptoContextsL();
            SVPDEBUG2( "CSVPSessionBase::SetCryptoContextL Supported crypto context count=%d",
                    cryptoContext.Count() )
            
            // set crypto contexts to session
            static_cast<CMceSecureInSession*> ( iSession )->
                SetCryptoContextsL( cryptoContext );
            }
        }
    
    SVPDEBUG1( "CSVPSessionBase::SetCryptoContextL Out" )
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::CheckMmfPrioritiesForDtmfL
// ---------------------------------------------------------------------------
//
void CSVPSessionBase::CheckMmfPrioritiesForDtmfL(
    const RPointerArray<CMceMediaStream>& aAudioStreams ) const
    {
    SVPDEBUG1( "CSVPSessionBase::CheckMmfPrioritiesForDtmfL In" )
    
    RPointerArray<CMceMediaSource> micsToEnable;
    CleanupClosePushL( micsToEnable );
    const TInt streamCount( aAudioStreams.Count() );
    
    SVPDEBUG2("CSVPSessionBase::CheckMmfPrioritiesForDtmfL streamCount: %d",
        streamCount )
    
    // First disable the mic before doing any priority updating.
    for ( TInt s = 0; s < streamCount; s++ )
        {
        CMceMediaSource* mic = aAudioStreams[s]->Source();
        
        if ( mic && KMceMicSource == mic->Type() && mic->IsEnabled() )
            {
            SVPDEBUG1( "CSVPSessionBase::CheckMmfPrioritiesForDtmfL disable mic 1" )
            mic->DisableL();
            micsToEnable.AppendL( mic );
            }
        
        // At this point we must have a bound stream, otherwise the session
        // would be a bit incomplete. Thus can and should leave here if bound
        // stream is not found.
        mic = NULL;
        mic = aAudioStreams[s]->BoundStreamL().Source();
        
        if ( mic && KMceMicSource == mic->Type() && mic->IsEnabled() )
            {
            SVPDEBUG1( "CSVPSessionBase::CheckMmfPrioritiesForDtmfL disable mic 2" )
            mic->DisableL();
            micsToEnable.AppendL( mic );
            }
        
        mic = NULL;
        }
    
    // Set the correct priority and preference values.
    for ( TInt k = 0; k < streamCount; k++ )
        {
        // No need to check against BoundStream(), see comments above.
        if ( KMceAudio == aAudioStreams[k]->Type() )
            {
            SVPDEBUG2( "CSVPSessionBase::CheckMmfPrioritiesForDtmfL round: %d start", k )
            CMceAudioStream* stream = static_cast<CMceAudioStream*>( aAudioStreams[k] );
            
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
            
            iSVPUtility.SetDtmfMode( dtmfMode );
            
            SVPDEBUG2( "CSVPSessionBase::CheckMmfPrioritiesForDtmfL round: %d done", k )
            }
        }
    
    // Priorities are now correct, so update the session.
    iSession->UpdateL();
    
    // Now enable mics after we have correct priorities.
    const TInt mics = micsToEnable.Count();
    
    for ( TInt t = 0; t < mics; t++ )
        {
        SVPDEBUG2( "CSVPSessionBase::CheckMmfPrioritiesForDtmfL enabling mics, round: %d", t )
        micsToEnable[t]->EnableL();
        }
    
    // Mics not owned
    CleanupStack::PopAndDestroy( &micsToEnable );
    
    SVPDEBUG1( "CSVPSessionBase::CheckMmfPrioritiesForDtmfL Out" )
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::SessionStateChangedL
// ---------------------------------------------------------------------------
//
void CSVPSessionBase::SessionStateChangedL( TInt aOrigStatus,
    TCCPError aError, TInt aModStatus )
    {
    SVPDEBUG1( "CSVPSessionBase::SessionStateChangedL In" )
    SVPDEBUG2( "CSVPSessionBase::SessionStateChangedL aOrigStatus=%d",
        aOrigStatus )
    SVPDEBUG2( "CSVPSessionBase::SessionStateChangedL aError=%d",
        aError )
    SVPDEBUG2( "CSVPSessionBase::SessionStateChangedL aModStatus=%d",
        aModStatus )
    
    // secure status was "secure call mandatory", only secure allowed
    if ( KErrNone != aModStatus && SecureMandatory() )
        {
        if ( KSVPNotAcceptableHereVal == aModStatus ||
             KSVPNotAcceptableVal == aModStatus ||
             KSVPPreconditionFailureVal == aModStatus ||
             KSVPMethodNotAllowedVal == aModStatus )
            {
            SVPDEBUG1( "CSVPSessionBase::SessionStateChangedL - Secure session failed!" )
            // secure call failed due to recipients restrictions
            User::LeaveIfError( ExecCbErrorOccurred( ECCPSecureCallFailed ) );
            }
        else
            {
            // secure call failed with SIP error response
            // inform application about the error response received
            User::LeaveIfError( ExecCbErrorOccurred( aError ) );
            
            // start termination timer, needed for UI
            // to handle Disconnected-bubble   
            StartTimerL( KSVPTerminatingTime, KSVPTerminationTimerExpired );
            User::LeaveIfError( ExecCbCallStateChanged( MCCPCallObserver::ECCPStateDisconnecting ) );
            }
        }
   
    if ( KSVPRingingVal == aOrigStatus )
        {
        SVPDEBUG1( "CSVPSessionBase::SessionStateChangedL - 180 ringing received" )
        User::LeaveIfError( ExecCbCallStateChanged( MCCPCallObserver::ECCPStateConnecting ) );
        if ( iEarlyMediaOngoing )
            {
            // early media event is sent again to avoid simultaneous early media and ringback tone
            SVPDEBUG1( "CSVPSessionBase::SessionStateChangedL ringing ja earlymedia flag on" )
            ExecCbCallEventOccurred( MCCPCallObserver::ECCCSPEarlyMediaStarted );
            iEarlyMediaOngoing = EFalse;
            }
        }
    
    if ( KSVPQueuedVal == aOrigStatus )
        {
        SVPDEBUG1( "CSVPSessionBase::SessionStateChangedL - 182 queued received" )
        User::LeaveIfError( ExecCbSsEventOccurred( MCCPSsObserver::ESsCallWaiting ) );
        }
    
    switch( iSession->State() )
        {
        case CMceSession::EEstablished:
            {
            // stop mt drop out timer or mo invite timer
            StopTimers();
            
            // mute status must be checked again in Mt -> 
            // there might be one call already in mute state        
            TInt value( EPSTelMicMuteOff );
            RProperty::Get( KPSUidTelMicrophoneMuteStatus, 
                    KTelMicrophoneMuteState, value );
            
            // check initial mute status 
            if ( EPSTelMicMuteOn == value )
                {
                SVPDEBUG1( "CSVPSessionBase::SessionStateChangedL Mute: ON" )
                ValueChangedL( KTelMicrophoneMuteState, EPSTelMicMuteOn );
                }
            // secure session created successfully, set iSecured flag to ETrue
            if ( KMceSessionSecure == iSession->Type() && 
                 CMceSession::EControlPathSecure == iSession->ControlPathSecurityLevel() )
                {
                SVPDEBUG1( "CSVPSessionBase::SessionStateChangedL - Secure session created" )
                iSecured = ETrue;
                }
            else
                {
                SVPDEBUG1( "CSVPSessionBase::SessionStateChangedL - Non-secure session created" )
                iSecured = EFalse;
                if ( SecurePreferred() )
                    {
                    // Secure preferred but unsecure session created -> send info to ccp api
                    SVPDEBUG1( "CSVPSessionBase::SessionStateChangedL - secure was preferred" )
                    iCallEventToBeSent = MCCPCallObserver::ECCPNotSecureCall;
                    }
                }
            
            // We need get the call state now because session state will
            // change when doing CheckMmfPrioritiesForDtmfL because it updates
            // the session. This leads into 'Offering' state which will
            // panic our State() function call.
            // Note: session's state will fall back to 'Established' because
            // the priority update does not lead into a real update.
            MCCPCallObserver::TCCPCallState callState = State();
            
            // We need to check MMF priorities only in MO case if really
            // neccessary
            if ( IsMobileOriginated() &&
                 SVPAudioUtility::MmfPriorityUpdateNeededL( iSession->Streams() ) )
                {
                CheckMmfPrioritiesForDtmfL( iSession->Streams() );
                }
            if ( IsMobileOriginated() )
                {
                TBool sessionUpdateOngoing = EFalse;
                UpdateKeepAliveL( *iSession, sessionUpdateOngoing );
                }
            
            // enable speaker sink
            SVPAudioUtility::EnableSpeakerSinksL( iSession->Streams() );
            // call state callback
            User::LeaveIfError( ExecCbCallStateChanged( callState ) );
            
            // 'NotSecure' call event must be sent after the
            // 'callstatechanged' - event, not before because phone needs an
            // active call for playing the unsecure tone.
            if ( MCCPCallObserver::ECCPSecureNotSpecified != iCallEventToBeSent )
                {
                 if ( MCCPCallObserver::ECCPSecureCall == iCallEventToBeSent ||
                     MCCPCallObserver::ECCPNotSecureCall == iCallEventToBeSent )
                    {
                    SVPDEBUG1( "CSVPSessionBase::SessionStateChangedL - sending secure specific call event" )
                    User::LeaveIfError( ExecCbCallEventOccurred( iCallEventToBeSent ) );
                    }
                SVPDEBUG1( "CSVPSessionBase::SessionStateChangedL - sending remotePartyInfoChanged event" )
                User::LeaveIfError( ExecCbCallEventOccurred( MCCPCallObserver::ECCPNotifyRemotePartyInfoChange ) );
                iCallEventToBeSent = MCCPCallObserver::ECCPSecureNotSpecified;
                }
            break;
            }
        case CMceSession::EAnswering:
        case CMceSession::EReserving:
        case CMceSession::EIncoming:
        case CMceSession::EOffering:
            {
            SVPDEBUG1( "CSVPSessionBase:SessionStateChangedL: No action" )
            break;
            }
        case CMceSession::ECancelling:
        case CMceSession::EIdle:
        case CMceSession::ETerminating:
        case CMceSession::EProceeding:
            {
            SVPDEBUG1( "CSVPSessionBase:SessionStateChangedL: Callback state change" )
            User::LeaveIfError( ExecCbCallStateChanged( State() ) );
            break;
            }
        case CMceSession::ETerminated:
            {
            SVPDEBUG1( "CSVPSessionBase::SessionStateChangedL Terminated" )
            
            if ( MCCPCallObserver::ECCPStateDisconnecting != iSessionState &&
                 MCCPCallObserver::ECCPStateIdle != iSessionState &&
                 iCCPSessionObserver )
                {
                // probably some SIP error occurred, inform client
                if ( aOrigStatus < 0 )
                	{
                    SVPDEBUG2( "CSVPSessionBase::SessionStateChangedL, aOrigStatus=%d", aOrigStatus )
                    
                    switch ( aOrigStatus )
                        {
                        case KErrSIPForbidden:
                            {
                            User::LeaveIfError( ExecCbErrorOccurred( ECCPErrorNoService ) );
                            }
                            break;
                        default:
                            {
                            User::LeaveIfError( ExecCbErrorOccurred( ECCPGlobalFailure ) );
                            }
                            break;
                        }
                    }
                
                if ( KErrNone != aModStatus )
                    {
                    User::LeaveIfError( ExecCbErrorOccurred( aError ) );
                    }
                
                iSessionState = MCCPCallObserver::ECCPStateDisconnecting;
                iTerminatingRepeat = 0;
                
                StartTimerL( KSVPTerminatingTime, KSVPTerminationTimerExpired );
                User::LeaveIfError( ExecCbCallStateChanged( State() ) );
                
                // Termination engaged by the remote end ->
                // Client is informed with ECCPRemoteTerminated event
                // This prevents automatic unhold when two calls, 
                // one on hold and one active, exist.
                if ( !iAlreadyTerminating )
                    {
                    User::LeaveIfError( ExecCbCallEventOccurred(
                            MCCPCallObserver::ECCPRemoteTerminated ) );
                    }
                }
            else if ( iCCPSessionObserver )
                {
                User::LeaveIfError( ExecCbCallStateChanged(
                        MCCPCallObserver::ECCPStateIdle ) );
                }
            else
                {
                SVPDEBUG1( "CSVPSessionBase::SessionStateChangedL Session creation failed in early stage - terminate" )
                iObserver.TerminateSession(  (CMceInSession&) Session() );
                }
            break;
            }
        default:
            {
            SVPDEBUG1( "CSVPSessionBase:SessionStateChangedL DEFAULT" )
            break;
            }
        }
    
    SVPDEBUG1( "CSVPSessionBase::SessionStateChangedL Out" )
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::UpdateKeepAliveL
// ---------------------------------------------------------------------------
//
void CSVPSessionBase::UpdateKeepAliveL( CMceSession& aSession, 
        TBool aSessionUpdateOngoing )
    {
    SVPDEBUG1( "CSVPSessionBase:UpdateKeepAliveL In" )
    
    // update keepalive -> mo and mt
    SVPDEBUG1( "CSVPSessionBase::UpdateKeepAliveL Starting CN handling" )
    
    const RPointerArray<CMceMediaStream>& streamArray = aSession.Streams();
    TInt streamCount( streamArray.Count() );
    TBool cnInAnswer = EFalse;
    
    for ( TInt i = 0; i < streamCount; i++ )
        {
        CMceAudioStream* stream = static_cast<CMceAudioStream*>( streamArray[ i ] );
        
        if ( iSVPUtility.IsComfortNoise( *stream ) )
            {
            cnInAnswer = ETrue;
            SVPDEBUG1( "CSVPSessionBase::UpdateKeepAliveL CN Found" )
            iSVPUtility.SetCNKeepAliveL( *stream, iKeepAliveValue );
            }
        }
    
    if ( !cnInAnswer )
        {
        SVPDEBUG1( "CSVPSessionBase::UpdateKeepAliveL CN NOT Found" )
        SVPDEBUG2( "CSVPSessionBase::SessionStateChangedL Streamcount: %d", streamCount )
        
        while( streamCount-- )
            {
            CMceAudioStream* stream = static_cast< CMceAudioStream* >( streamArray[ streamCount ] );
            iSVPUtility.SetKeepAliveL( *stream, iKeepAliveValue );
            }
        }
    
    if ( !aSessionUpdateOngoing )
        {
        SVPDEBUG1( "CSVPSessionBase::UpdateKeepAliveL  -> UPDATE" )
        aSession.UpdateL();
        SVPDEBUG1( "CSVPSessionBase::UpdateKeepAliveL  -> UPDATE DONE" )
        }
    SVPDEBUG1("CSVPSessionBase:UpdateKeepAliveL Out")
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::UpdateFailed
// ---------------------------------------------------------------------------
//    
void CSVPSessionBase::UpdateFailed( CMceSession& aSession, TInt aStatusCode )
    {
    SVPDEBUG1( "CSVPSessionBase::UpdateFailed In" )
    SVPDEBUG2( "CSVPSessionBase::UpdateFailed aStatusCode=%d", aStatusCode )
    
    if ( iHoldController )
        {
        iHoldController->RequestFailed( aSession, aStatusCode, *this );
        }
    
    SVPDEBUG1( "CSVPSessionBase::UpdateFailed Out" )
    }


// ---------------------------------------------------------------------------
// CSVPSessionBase::SetRtpKeepAliveL
// ---------------------------------------------------------------------------
//
void CSVPSessionBase::SetRtpKeepAliveL( CMceSession* /*aSession*/, 
                                        TBool /*aSessionUpdateOngoing*/ )
    {
    SVPDEBUG1( "CSVPSessionBase::SetRtpKeepAliveL" )
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::SetUpdatedSession
// ---------------------------------------------------------------------------
//
void CSVPSessionBase::SetUpdatedSession( CMceInSession* aUpdatedSession )
    {
    SVPDEBUG1( "CSVPSessionBase::SetUpdatedSession In" )
    
    delete iSession;
    iSession = aUpdatedSession;

	// lets check should mic be muted 
    const RPointerArray<CMceMediaStream>& streamsArray = iSession->Streams(); 
    const TInt streamCount( streamsArray.Count() );
    if ( streamCount && iMuted )
        {
        SVPDEBUG1( "CSVPSessionBase::SetUpdatedSession Mic should be muted" )
        
        if ( IsMobileOriginated() ) 
            {
            for ( TInt i = 0; i < streamCount; i++ )
                {
                if ( streamsArray[i]->Source()->IsEnabled() )
                    {
                    SVPDEBUG1( "CSVPSessionBase::SetUpdatedSession Mic is not muted" )
                    SVPDEBUG1( " -> disable mic" )
                    streamsArray[i]->Source()->DisableL();
                    }
                else 
                    {
                    SVPDEBUG1( "CSVPSessionBase::SetUpdatedSession Mic is already" )
                    SVPDEBUG1( " muted -> no need to disable mic" )
                    }
                }
            }
        else
            {
            // mute mic source
            for ( TInt i = 0; i < streamCount; i++ )
                {
                if ( streamsArray[i]->BoundStreamL().Source()->IsEnabled() )
                    {
                    streamsArray[i]->BoundStreamL().Source()->DisableL();
                    }
                }
            }
        }

    // Update changed session also to the transfercontroller
    if ( iTransferController )
        {
        SVPDEBUG1( "CSVPSessionBase::SetUpdatedSession also to the transfercontroller" )
        iTransferController->SetMceSessionObject( aUpdatedSession );
        }

    SVPDEBUG1( "CSVPSessionBase::SetUpdatedSession Out" )
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::SetEmptyReInvite
// ---------------------------------------------------------------------------
//
void CSVPSessionBase::SetEmptyReInvite()
    {
    SVPDEBUG1( "CSVPSessionBase::SetEmptyReInvite In" )
    iEmptyReInvite = ETrue;
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::IsEmptyReInvite
// ---------------------------------------------------------------------------
//
TBool CSVPSessionBase::IsEmptyReInvite()
    {
    SVPDEBUG1( "CSVPSessionBase::IsEmptyReInvite In" )
    return iEmptyReInvite;
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::IsIdle
// ---------------------------------------------------------------------------
//
TBool CSVPSessionBase::IsIdle()
    {
    SVPDEBUG1( "CSVPSessionBase::IsIdle In" )
    return ( MCCPCallObserver::ECCPStateIdle == iSessionState );
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::SecureSpecified
// ---------------------------------------------------------------------------
//
TBool CSVPSessionBase::SecureSpecified( ) const 
   {
   return ETrue;
   }

// ---------------------------------------------------------------------------
// CSVPSessionBase::SetErrorInULandDLFirstTime
// ---------------------------------------------------------------------------
//
void CSVPSessionBase::SetErrorInULandDLFirstTime( TBool aFirstTime )
    {
    iErrorInULandDLFirstTime = aFirstTime;
    SVPDEBUG2("CSVPSessionBase::SetErrorInULandDLFirstTime=%d",iErrorInULandDLFirstTime)
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::IsErrorInULandDLFirstTime
// ---------------------------------------------------------------------------
//
TBool CSVPSessionBase::IsErrorInULandDLFirstTime()
    {
    SVPDEBUG2("CSVPSessionBase::IsErrorInULandDLFirstTime=%d",iErrorInULandDLFirstTime)
    return iErrorInULandDLFirstTime;
    }


// ---------------------------------------------------------------------------
// CSVPSessionBase::IsBothStreamsDisabledL
// ---------------------------------------------------------------------------
//
TBool CSVPSessionBase::IsBothStreamsDisabledL() const
    {
    SVPDEBUG1( "CSVPSessionBase::IsBothStreamsDisabledL() In" )
    TBool bothStreamsDisabled = ETrue;
    const RPointerArray<CMceMediaStream>& streams = Session().Streams();
    TInt streamsCount = streams.Count();

    for ( TInt i = 0; i < streamsCount; i++ )
        {
        if( streams[ i ]->State() != CMceMediaStream::EDisabled || 
            streams[ i ]->BoundStreamL().State() != CMceMediaStream::EDisabled )
            {
            bothStreamsDisabled = EFalse;
            }
        }

    SVPDEBUG2("CSVPSessionBase::IsBothStreamsDisabledL Out,ret=%d",bothStreamsDisabled)
    return bothStreamsDisabled;
    }

// ---------------------------------------------------------------------------
// CSVPSessionBase::IsSessionMuted
// ---------------------------------------------------------------------------
//
TBool CSVPSessionBase::IsSessionMuted() const
    {
    SVPDEBUG2("CSVPSessionBase::IsSessionMuted: %d", iMuted )
    return iMuted;
    }

