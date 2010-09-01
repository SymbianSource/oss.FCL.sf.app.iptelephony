/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides RTP observation class
*
*/


#include <mcemediastream.h>
#include <mceaudiostream.h>
#include <mceaudiocodec.h>
#include "svprtpobserverrequest.h"
#include "svpsessionbase.h"
#include "svptimer.h"
#include "svpconsts.h"
#include "mccpcallobserver.h"
#include "svplogger.h"
#include "svpholdcontroller.h"

// LOCAL CONSTANTS
// RTCP timeout interval, 30 seconds.
const TInt KSVPRtcpTimeout = 30000;
// Drift 5 seconds, added to actual timeout allowing some randomness in
// actual timeout.
const TInt KSVPRtcpTimeoutDrift = 5000;

// ---------------------------------------------------------------------------
// CSVPRtpObserverRequest::CSVPRtpObserverRequest
// ---------------------------------------------------------------------------
//
CSVPRtpObserverRequest::CSVPRtpObserverRequest(
    CSVPSessionBase& aSession  ) : iSession( aSession )
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// CSVPRtpObserverRequest::~CSVPRtpObserverRequest
// ---------------------------------------------------------------------------
//
CSVPRtpObserverRequest::~CSVPRtpObserverRequest()
    {
    delete iTimer;
    }

// ---------------------------------------------------------------------------
// CSVPRtpObserverRequest::NewLC
// ---------------------------------------------------------------------------
//
CSVPRtpObserverRequest* CSVPRtpObserverRequest::NewLC(
    CSVPSessionBase& aSession )
    {
    CSVPRtpObserverRequest* self =
        new ( ELeave ) CSVPRtpObserverRequest( aSession );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CSVPRtpObserverRequest::NewL
// ---------------------------------------------------------------------------
//
CSVPRtpObserverRequest* CSVPRtpObserverRequest::NewL(
    CSVPSessionBase& aSession )
    {
    CSVPRtpObserverRequest* self = CSVPRtpObserverRequest::NewLC( aSession );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CSVPRtpObserverRequest::ConstructL
// ---------------------------------------------------------------------------
//
void CSVPRtpObserverRequest::ConstructL()
    {
    // NB, no need for a special timer ID.
    iTimer = CSVPTimer::NewL( *this, KErrNone );
    }

// ---------------------------------------------------------------------------
// CSVPRtpObserverRequest::SvpSession
// ---------------------------------------------------------------------------
//
const CSVPSessionBase* CSVPRtpObserverRequest::SvpSession() const
    {
    return &iSession;
    }

// ---------------------------------------------------------------------------
// CSVPRtpObserverRequest::MceSession
// ---------------------------------------------------------------------------
//
const CMceSession* CSVPRtpObserverRequest::MceSession() const
    {
    return &( iSession.Session() );
    }

// ---------------------------------------------------------------------------
// CSVPRtpObserverRequest::RtcpPacketReceived
// ---------------------------------------------------------------------------
//
void CSVPRtpObserverRequest::RtcpPacketReceived(
    TReceivedRtcpPacket /*aPacket*/,
    CMceSession& aSession, 
    CMceMediaStream& aStream )
    {
    SVPDEBUG2( "CSVPRtpObserverRequest::RtcpPacketReceived aStream.IsEnabled: %d",
        aStream.IsEnabled() )
    SVPDEBUG2( "CSVPRtpObserverRequest::RtcpPacketReceived aSession: 0x%x",
        &aSession )
    SVPDEBUG2( "CSVPRtpObserverRequest::RtcpPacketReceived iSession: 0x%x",
        &( iSession.Session() ) )
    
    // Check that the given session matches ours
    if ( &aSession == &( iSession.Session() ) )
        {
        SVPDEBUG1( "CSVPRtpObserverRequest::RtcpPacketReceived, (re)start timer" )
        
        // For the sake of "just to be sure", stop the timer first
        const TInt timeout( DetermineTimeoutValue( aStream ) );
        iTimer->Stop();
        iTimer->SetTime( timeout );
        iDisconnectSent = EFalse;
        }
    }

// ---------------------------------------------------------------------------
// CSVPRtpObserverRequest::Reset
// ---------------------------------------------------------------------------
//
void CSVPRtpObserverRequest::Reset()
    {
    SVPDEBUG1( "CSVPRtpObserverRequest::Reset" )
    
    iTimer->Stop();
    }

// ---------------------------------------------------------------------------
// CSVPRtpObserverRequest::TimedOut
// ---------------------------------------------------------------------------
//
void CSVPRtpObserverRequest::TimedOut( TInt /*aTimerId*/ )
    {
    // For the sake of "just to be sure", stop the timer first
    iTimer->Stop();
    
    if ( IsSessionInHoldState() )
        {
        SVPDEBUG1( "CSVPRtpObserverRequest::TimedOut, (re)start timer because Session is on HOLD" )
        iTimer->SetTime( KSVPRtcpTimeout );
        iDisconnectSent = EFalse;
        }
    else
        {
        SVPDEBUG2( "CSVPRtpObserverRequest::TimedOut iDisconnectSent: %d",
            iDisconnectSent )
        
        // First CCE must be notified about 'disconnecting', state. Then timer is
        // re-started so that CCE & UI can handle the events with time (i.e user
        // has actually time to see the UI notifications).
        // After timer has fired second time, then send 'idle' event to the
        // CCE.
        
        
        if ( !iDisconnectSent )
            {
            iSession.GetCCPSessionObserver().CallStateChanged(
                MCCPCallObserver::ECCPStateDisconnecting, &iSession ); 
            
            iTimer->SetTime( KSVPTerminatingTime );
            iDisconnectSent = ETrue;
            }
        else
            {
            iSession.GetCCPSessionObserver().CallStateChanged(
                MCCPCallObserver::ECCPStateIdle, &iSession ); 
            }
        }
    }

// ---------------------------------------------------------------------------
// CSVPRtpObserverRequest::DetermineTimeoutValue
// ---------------------------------------------------------------------------
//
TInt CSVPRtpObserverRequest::DetermineTimeoutValue(
    CMceMediaStream& aStream ) const
    {
    // NB: aStream is not const because call to Codecs() is not const => this
    // saves a const_cast and keeps some sanity in casting. Though, we do not
    // modify anything in given aStream...
    
    TInt timeout( KSVPRtcpTimeout );
    if ( KMceAudio == aStream.Type() )
        {
        CMceAudioStream& audio( static_cast<CMceAudioStream&>( aStream ) );
        
        const RPointerArray<CMceAudioCodec>& codecs( audio.Codecs() );
        const TInt count = codecs.Count();
        for( TInt k = 0; k < count; k++ )
            {
            // MCE API talks about ms' but returns somethings that look awfully
            // close to seconds... Oh, well...
            const TInt kpval(
                codecs[k]->KeepAliveTimer() * KSVPMilliSecondCoefficient );
            if ( kpval > timeout )
                {
                timeout = kpval;
                }
                
            SVPDEBUG2( "CSVPRtpObserverRequest::DetermineTimeoutValue k: %d", k )
            SVPDEBUG2( "CSVPRtpObserverRequest::DetermineTimeoutValue kpval: %d", kpval )
            }
            
        SVPDEBUG2( "CSVPRtpObserverRequest::DetermineTimeoutValue count: %d", count )
        }
    
    // Now add the drifting to the timeout.
    timeout += KSVPRtcpTimeoutDrift;
    
    SVPDEBUG2( "CSVPRtpObserverRequest::DetermineTimeoutValue timeout: %d", timeout )
    
    return timeout;
    }

// ---------------------------------------------------------------------------
// CSVPRtpObserverRequest::IsSessionInHoldState
// ---------------------------------------------------------------------------
//
TBool CSVPRtpObserverRequest::IsSessionInHoldState()
    {
    if ( SvpSession()->HasHoldController() &&
            ESVPOnHold == (SvpSession()->HoldController()).HoldState() )
        {
        SVPDEBUG1( "CSVPRtpObserverRequest::IsSessionInHoldState ETrue" )
        return ETrue;
        }
    else
        {
        SVPDEBUG1( "CSVPRtpObserverRequest::IsSessionInHoldState EFalse" )
        return EFalse;
        }
    }
