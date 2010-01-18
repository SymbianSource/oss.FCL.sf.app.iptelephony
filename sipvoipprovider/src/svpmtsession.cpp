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
* Description:  Representation of SVP MT session              
*
*/

#include <centralrepository.h>
#include <mcesecureoutsession.h>
#include <mceaudiocodec.h>
#include <settingsinternalcrkeys.h>
#include <mcertpsink.h>
#include <mceaudiostream.h>
#include <mceinsession.h>
#include <mcemediasource.h>
#include <mcertpsource.h>

#include "svpmtsession.h"
#include "svpaudioutility.h"
#include "svpsipconsts.h"
#include "svplogger.h"

// ---------------------------------------------------------------------------
// CSVPMtSession::CSVPMtSession
// ---------------------------------------------------------------------------
//
CSVPMtSession::CSVPMtSession( CMceInSession* aMtSession,
                              TMceTransactionDataContainer& aContainer,
                              TUint32 aVoIPProfileId,
                              MSVPSessionObserver& aObserver, 
                              CSVPUtility& aSVPUtility,
                              CSVPRtpObserver& aRtpObserver,
                              TInt aKeepAliveValue,
                              TBool aPreconditions )
    : CSVPSessionBase( aContainer, aObserver, aSVPUtility, aRtpObserver )
    {
    iSession = aMtSession;
    iVoIPProfileId = aVoIPProfileId;
    iKeepAliveValue = aKeepAliveValue;
    iPreconditions = aPreconditions;
    
    CMceSession::TControlPathSecurityLevel securityLevel =
         aMtSession->ControlPathSecurityLevel();

    // Not secure call event is sent according this, in mt case event isn't sent
    iCallEventToBeSent = MCCPCallObserver::ECCPSecureNotSpecified;

    if ( CMceSession::EControlPathSecure == securityLevel &&
         KMceSessionSecure == aMtSession->Type() )
        {
        SVPDEBUG1( "CSVPMtSession::CSVPMtSession Mt secure" )
        iSecured = ETrue;
        }
    else
        {
        SVPDEBUG1( "CSVPMtSession::CSVPMtSession Mt non secure" )
        iSecured = EFalse;
        }
    }

// ---------------------------------------------------------------------------
// CSVPMtSession::NewL
// ---------------------------------------------------------------------------
//
CSVPMtSession* CSVPMtSession::NewL( CMceInSession* aMtSession,
                                    TMceTransactionDataContainer& aContainer,
                                    TUint32 aServiceId,
                                    TUint32 aVoIPProfileId,
                                    MSVPSessionObserver& aObserver,
                                    CSVPUtility& aSVPUtility,
                                    CSVPRtpObserver& aRtpObserver,
                                    TInt aKeepAliveValue,
                                    TBool aPreconditions )
    {
    CSVPMtSession* self = new ( ELeave ) CSVPMtSession( aMtSession,
                                                     aContainer, 
                                                     aVoIPProfileId,
                                                     aObserver,
                                                     aSVPUtility,
                                                     aRtpObserver,
                                                     aKeepAliveValue,
                                                     aPreconditions );
    CleanupStack::PushL( self );
    self->ConstructL( aServiceId );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CSVPMtSession::~CSVPMtSession
// ---------------------------------------------------------------------------
//
CSVPMtSession::~CSVPMtSession()
    {
    SVPDEBUG1( "CSVPMtSession::~CSVPMtSession" )
    }

// ---------------------------------------------------------------------------
// CSVPMtSession::ConstructL
// ---------------------------------------------------------------------------
//
void CSVPMtSession::ConstructL( TUint32 aServiceId )
    {
    SVPDEBUG1( "CSVPMtSession::ConstructL In" )
    
    iCallParameters = CCCPCallParameters::NewL();
    iCallParameters->SetServiceId( aServiceId );
    iCallParameters->SetCallType( CCPCall::ECallTypePS );
    
    // modify QoS preconditions
    if ( iPreconditions )
        {
        SVPDEBUG1( "CSVPMtSession::ConstructL preconditions supported" )
        iSession->SetModifierL( KMcePreconditions, KMcePreconditionsSupported );
        }
    else
        {
        SVPDEBUG1( "CSVPMtSession::ConstructL preconditions NOT supported" )
        iSession->SetModifierL( KMcePreconditions, KMcePreconditionsNotUsed );
        }
    
    // modify Old way hold on
    iSession->SetModifierL( KMceMediaDirection, KMceMediaDirectionWithAddress );
    
    InitializePropertyWatchingL();
    
    SVPDEBUG1( "CSVPMtSession::ConstructL Out" )
    }


// from MCCPCall
// ---------------------------------------------------------------------------
// CSVPMtSession::Answer
// ---------------------------------------------------------------------------
//      
TInt CSVPMtSession::Answer()
    {
    SVPDEBUG1( "CSVPMtSession::Answer In" )
    
    // stop expires timer
    StopTimers();
    
    // answer to session
    TRAPD( err, DoAnswerL() );
    
    if ( KErrNone != err )
        {
        SVPDEBUG2( "CSVPMtSession::Answer Out return(err)=%d", err )
        return err;
        }
    else
        {
        ExecCbCallStateChanged( MCCPCallObserver::ECCPStateAnswering );
        SVPDEBUG2( "CSVPMtSession::Answer Out return=%d", err )
        return err;
        }
    }

// ---------------------------------------------------------------------------
// CSVPMtSession::DoAnswerL
// ---------------------------------------------------------------------------
//      
void CSVPMtSession::DoAnswerL()
    {
    SVPDEBUG1( "CSVPMtSession::DoAnswerL In" )
    
    static_cast<CMceInSession*>( iSession )->AcceptL();
    HandleMtStreamsL( iSession->Streams() );
    
    SVPDEBUG1( "CSVPMtSession::DoAnswerL Out" )
    }

// ---------------------------------------------------------------------------
// CSVPMtSession::HandleMtStreamsL
// ---------------------------------------------------------------------------
// 
void CSVPMtSession::HandleMtStreamsL(
    const RPointerArray<CMceMediaStream>& aStreams )
    {
    SVPDEBUG1( "CSVPMtSession::HandleMtStreamsL In" )
    
    // go through streams and enable rtp source and speaker sink if found
    for ( TInt i = 0; i < aStreams.Count(); i++ )
        {
        SVPDEBUG1( "CSVPMtSession::HandleMtStreamsL for-loop" )
        
        CMceMediaStream* stream1 = aStreams[ i ];
        CMceMediaStream& boundStream = stream1->BoundStreamL();// fetch bound stream
        
        // enable RTP source
        if ( stream1->Source() && stream1->Source()->Type() == KMceRTPSource )
            {
            SVPDEBUG1( "CSVPMtSession::HandleMtStreamsL RTPSource found" )
        	stream1->Source()->EnableL();
        	SVPDEBUG1( "CSVPMtSession::HandleMtStreamsL RTPSource ENABLED" )
            }
        
        // enable speaker sink    
        SVPAudioUtility::EnableSpeakerSinkL( stream1->Sinks() );
        
        // enable mic source
        if ( stream1->Source() )
            {
            SVPAudioUtility::EnableMicSourceL( *stream1->Source() );
        	}
        
        // enable mic source from bound stream, if found
        if ( boundStream.Source() )
            {
            SVPAudioUtility::EnableMicSourceL( *boundStream.Source() );
            }
        }
    
    SVPDEBUG1( "CSVPMtSession::HandleMtStreamsL Out" )
    }

// ---------------------------------------------------------------------------
// CSVPMtSession::IsCallForwarded
// ---------------------------------------------------------------------------
//     
TBool CSVPMtSession::IsCallForwarded() const
    {
    return EFalse;  
    }
    
// ---------------------------------------------------------------------------
// CSVPMtSession::Release
// ---------------------------------------------------------------------------
//      
TInt CSVPMtSession::Release()
    {
    SVPDEBUG1( "CSVPMtSession::Release" )
    delete this;
    return KErrNone; 
    }
            
// ---------------------------------------------------------------------------
// CSVPMtSession::Reject
// ---------------------------------------------------------------------------
// 
TInt CSVPMtSession::Reject()
    {
    SVPDEBUG1( "CSVPMtSession::Reject In" )
    
    TRAPD( err, static_cast<CMceInSession*>(iSession )->RejectL(
            KSVPBusyHereReason, KSVPBusyHereVal ) );
    
    if ( !err )
        {
        ExecCbCallStateChanged( MCCPCallObserver::ECCPStateIdle );
        }
    
    SVPDEBUG2( "CSVPMtSession::Reject Out return=%d", err )
    return err;
    }

// ---------------------------------------------------------------------------
// CSVPMtSession::Queue
// ---------------------------------------------------------------------------
// 
TInt CSVPMtSession::Queue()
    {
    SVPDEBUG1( "CSVPMtSession::Queue In" )
    
    // Get PS waiting setting 
    TInt error( KErrNone );
    TInt psWaiting( EFalse );
    CRepository* repository = NULL;
    
    TRAP( error, repository = CRepository::NewL( KCRUidRichCallSettings ) );
    
    if ( KErrNone == error )
        {
        error = repository->Get( KRCSPSCallWaiting, psWaiting );
        delete repository;
        repository = NULL;
        
        if ( psWaiting )
            {
            // send 180 ringing
            Ringing();
            SVPDEBUG1( "CSVPMtSession::Queue Sending 180 Ringing" )
            
            // send 182 queued
            SVPDEBUG1( "CSVPMtSession::Queue() Sending 182 Queued" )
            TRAPD( err, static_cast<CMceInSession*> ( iSession )->RespondL(
                    KSVPQueuedReason, KSVPQueuedVal ) );
            
            if ( err )
                {
                SVPDEBUG2( "CSVPMtSession::Queue Out return=%d", err )
                return err;
                }
            }
        else
            {
            // ps waiting off, return error
            SVPDEBUG1( "CSVPMtSession::Queue Out return=KErrNotReady" )
            return KErrNotReady;
            }
        }
    
    SVPDEBUG2( "CSVPMtSession::Queue Out return=%d", error )
    return error;
    }

// ---------------------------------------------------------------------------
// CSVPMtSession::Ringing
// ---------------------------------------------------------------------------
// 
TInt CSVPMtSession::Ringing()
    {
    SVPDEBUG1( "CSVPMtSession::Ringing In" )
    
    TRAPD( err, static_cast<CMceInSession*>( iSession )->RingL() );
    
    SVPDEBUG2( "CSVPMtSession::Ringing Out return=%d", err )
    return err;
    }

// ---------------------------------------------------------------------------
// CSVPMtSession::Hold
// ---------------------------------------------------------------------------
//
TInt CSVPMtSession::Hold()
    {
    SVPDEBUG1( "CSVPMtSession::Hold In" )
    
    TBool sessionUpdateOngoing = ETrue;
    TRAP_IGNORE( SetRtpKeepAliveL( iSession, sessionUpdateOngoing ) );
    
    TInt status = CSVPSessionBase::Hold();
    
    SVPDEBUG2( "CSVPMtSession::Hold Out return=%d", status )
    return status;
    }

// ---------------------------------------------------------------------------
// CSVPMtSession::Resume
// ---------------------------------------------------------------------------
//
TInt CSVPMtSession::Resume()
    {
    SVPDEBUG1( "CSVPMtSession::Resume" )
    return CSVPSessionBase::Resume();
    }
 
// ---------------------------------------------------------------------------
// CSVPMtSession::IsMobileOriginated
// ---------------------------------------------------------------------------
//
TBool CSVPMtSession::IsMobileOriginated() const
    {
    SVPDEBUG1( "CSVPMtSession::IsMobileOriginated = EFalse" )
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CSVPMtSession::SetRtpKeepAliveL
// ---------------------------------------------------------------------------
//
void CSVPMtSession::SetRtpKeepAliveL( CMceSession* aSession,
        TBool aSessionUpdateOngoing )
    {
    SVPDEBUG1( "CSVPMtSession::SetRtpKeepAliveL In" )
    
    __ASSERT_ALWAYS( aSession, User::Leave( KErrArgument ) );
    UpdateKeepAliveL( *aSession, aSessionUpdateOngoing );
    
    SVPDEBUG1( "CSVPMtSession::SetRtpKeepAliveL Out" )
    }

