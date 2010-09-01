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


#include "svprtpobserver.h"
#include "svprtpobserverrequest.h"
#include "svpsessionbase.h"
#include "svplogger.h"

// ---------------------------------------------------------------------------
// CSVPRtpObserver::CSVPRtpObserver
// ---------------------------------------------------------------------------
//
CSVPRtpObserver::CSVPRtpObserver()
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// CSVPRtpObserver::~CSVPRtpObserver
// ---------------------------------------------------------------------------
//
CSVPRtpObserver::~CSVPRtpObserver()
    {
    iRequestArray.ResetAndDestroy();
    iRequestArray.Close();
    }

// ---------------------------------------------------------------------------
// CSVPRtpObserver::NewLC
// ---------------------------------------------------------------------------
//
CSVPRtpObserver* CSVPRtpObserver::NewLC()
    {
    CSVPRtpObserver* self = new ( ELeave ) CSVPRtpObserver();
    CleanupStack::PushL( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CSVPRtpObserver::NewL
// ---------------------------------------------------------------------------
//
CSVPRtpObserver* CSVPRtpObserver::NewL()
    {
    CSVPRtpObserver* self = CSVPRtpObserver::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CSVPRtpObserver::SRReceived
// ---------------------------------------------------------------------------
//
void CSVPRtpObserver::SRReceived( CMceSession& aSession,
    CMceMediaStream& aStream )
    {
    SVPDEBUG1( "CSVPRtpObserver::SRReceived" )
    
    CSVPRtpObserverRequest* assocReq = FindByMceSession( &aSession );
    if ( assocReq )
        {
        assocReq->RtcpPacketReceived( 
            CSVPRtpObserverRequest::ERtcpSrPacket, aSession, aStream );
        }
    }

// ---------------------------------------------------------------------------
// CSVPRtpObserver::RRReceived
// ---------------------------------------------------------------------------
//
void CSVPRtpObserver::RRReceived( CMceSession& aSession,
    CMceMediaStream& aStream )
    {
    SVPDEBUG1( "CSVPRtpObserver::RRReceived" )
    
    CSVPRtpObserverRequest* assocReq = FindByMceSession( &aSession );
    if ( assocReq )
        {
        assocReq->RtcpPacketReceived(
            CSVPRtpObserverRequest::ERtcpRrPacket, aSession, aStream );
        }
    }

// ---------------------------------------------------------------------------
// CSVPRtpObserver::InactivityTimeout
// ---------------------------------------------------------------------------
//
void CSVPRtpObserver::InactivityTimeout( CMceMediaStream& /*aStream*/,
    CMceRtpSource& /*aSource*/ )
    {
    SVPDEBUG1( "CSVPRtpObserver::InactivityTimeout" )
    }

// ---------------------------------------------------------------------------
// CSVPRtpObserver::SsrcAdded
// ---------------------------------------------------------------------------
//
void CSVPRtpObserver::SsrcAdded( CMceMediaStream& /*aStream*/,
    CMceRtpSource& /*aSource*/, TUint /*aSsrc*/ )
    {
    SVPDEBUG1( "CSVPRtpObserver::SsrcAdded" )
    }

// ---------------------------------------------------------------------------
// CSVPRtpObserver::SsrcRemoved
// ---------------------------------------------------------------------------
//
void CSVPRtpObserver::SsrcRemoved( CMceMediaStream& /*aStream*/,
    CMceRtpSource& /*aSource*/, TUint /*aSsrc*/ )
    {
    SVPDEBUG1( "CSVPRtpObserver::SsrcRemoved" )
    }

// ---------------------------------------------------------------------------
// CSVPRtpObserver::AddSessionForObservingL
// ---------------------------------------------------------------------------
//
void CSVPRtpObserver::AddSessionForObservingL( CSVPSessionBase* aSession )
    {
    SVPDEBUG2( "CSVPRtpObserver::AddSessionForObservingL aSession: 0x%x",
        aSession )
        
    __ASSERT_ALWAYS( aSession, User::Leave( KErrArgument ) );
    __ASSERT_ALWAYS( !FindBySvpSession( aSession ),
        User::Leave( KErrAlreadyExists ) );
    
    CSVPRtpObserverRequest* req = CSVPRtpObserverRequest::NewLC( *aSession );
    iRequestArray.AppendL( req );
    CleanupStack::Pop( req );
    }

// ---------------------------------------------------------------------------
// CSVPRtpObserver::RemoveSessionFromObserving
// ---------------------------------------------------------------------------
//
TInt CSVPRtpObserver::RemoveSessionFromObserving( CSVPSessionBase* aSession )
    {
    SVPDEBUG2( "CSVPRtpObserver::RemoveSessionFromObserving aSession: 0x%x",
        aSession )
    
    TInt ret = KErrNone;
    if ( !aSession )
        {
        ret = KErrArgument;
        }
    else
        {
        CSVPRtpObserverRequest* todel = FindBySvpSession( aSession );
        if ( todel )
            {
            TInt index = iRequestArray.Find( todel );
            iRequestArray.Remove( index );
            delete todel;
            }
        else
            {
            ret = KErrNotFound;
            }
        }
        
    SVPDEBUG2( "CSVPRtpObserver::RemoveSessionFromObserving ret: %d", ret )
    
    return ret;
    }

// ---------------------------------------------------------------------------
// CSVPRtpObserver::ResetSessionInObserving
// ---------------------------------------------------------------------------
//
TInt CSVPRtpObserver::ResetSessionInObserving( CSVPSessionBase* aSession )
    {
    SVPDEBUG2( "CSVPRtpObserver::ResetSessionInObserving aSession: 0x%x",
        aSession )
    
    TInt ret = KErrNone;
    if ( !aSession )
        {
        ret = KErrArgument;
        }
    else
        {
        CSVPRtpObserverRequest* toreset = FindBySvpSession( aSession );
        if ( toreset )
            {
            toreset->Reset();
            }
        else
            {
            ret = KErrNotFound;
            }
        }
    
    SVPDEBUG2( "CSVPRtpObserver::ResetSessionInObserving ret: %d", ret )
    
    return ret;
    }

// ---------------------------------------------------------------------------
// CSVPRtpObserver::FindBySvpSession
// ---------------------------------------------------------------------------
//
CSVPRtpObserverRequest* CSVPRtpObserver::FindBySvpSession(
    const CSVPSessionBase* aSession )
    {
    TInt count = iRequestArray.Count();
    CSVPRtpObserverRequest* ret( NULL );
    while( count-- && !ret && aSession )
        {
        if ( aSession == iRequestArray[count]->SvpSession() )
            {
            ret = iRequestArray[count];
            }
        }
        
    SVPDEBUG2( "CSVPRtpObserver::FindBySvpSession ret: 0x%x", ret )
    
    return ret;
    }
    
// ---------------------------------------------------------------------------
// CSVPRtpObserver::FindByMceSession
// ---------------------------------------------------------------------------
//
CSVPRtpObserverRequest* CSVPRtpObserver::FindByMceSession(
    const CMceSession* aSession )
    {
    TInt count = iRequestArray.Count();
    CSVPRtpObserverRequest* ret( NULL );
    while( count-- && !ret && aSession )
        {
        if ( aSession == iRequestArray[count]->MceSession() )
            {
            ret = iRequestArray[count];
            }
        }
    
    SVPDEBUG2( "CSVPRtpObserver::FindByMceSession ret: 0x%x", ret )
    
    return ret;
    }
