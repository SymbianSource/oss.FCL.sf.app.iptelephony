/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Creates SMS message if Event in MailBox has occurred.
*
*/



#include <mceoutevent.h>
#include <mcemanager.h>

#include "ipvmbxbase.h"
#include "ipvmbxlogger.h"
#include "ipvmbxpanic.h"
#include "ipvmbxengine.h"


const TInt KMinReSubscribeDelta = 600; // 10 min
const TInt KMaxReSubscribeDelta = 86400; // 24 h
const TUint32 KServerRefreshInterval = KMaxReSubscribeDelta;
const TInt KSecondsToMicro = 1000000;

_LIT8( KEventHeader8, "message-summary" );


// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CIpVmbxBase::CIpVmbxBase(
    CIpVmbxEngine& aVmbxEngine,
    CMceManager& aMceManager ) :
        iSipProfile( NULL ),
        iMceOutEvent( NULL ),
        iMceManager( aMceManager ),
        iReSubscribe( NULL ),
        iVmbxEngine( aVmbxEngine )
    {
    }


// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CIpVmbxBase::ConstructL( TDesC8& aVoiceMailUri8 )
    {
    iReSubscribe = CDeltaTimer::NewL( CActive::EPriorityIdle );
    if ( iVmbxUri8.MaxLength() >= aVoiceMailUri8.Length() )
        {
        iVmbxUri8.Copy( aVoiceMailUri8 );
        }
    TCallBack cb( ReSubscribe, this );
    iUpdateEvent.Set( cb );
    }


// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CIpVmbxBase* CIpVmbxBase::NewL(
    CIpVmbxEngine& aVmbxEngine,
    TDesC8& aVoiceMailUri8,
    CMceManager& aMceManager )
    {
    IPVMEPRINT( "CIpVmbxBase::NewL" );

    CIpVmbxBase* self = new( ELeave ) CIpVmbxBase(
        aVmbxEngine,
        aMceManager );
    CleanupStack::PushL( self );
    self->ConstructL( aVoiceMailUri8 );
    CleanupStack::Pop( self );

    return self;
    }


// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
CIpVmbxBase::~CIpVmbxBase()
    {
    IPVMEPRINT( "CIpVmbxBase::~CIpVmbxBase - IN" );

    if ( iReSubscribe )
        {
        iReSubscribe->Remove( iUpdateEvent );
        }
    delete iReSubscribe;
    delete iMceOutEvent;

    IPVMEPRINT( "CIpVmbxBase::~CIpVmbxBase - OUT" );
    }


// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CIpVmbxBase::Initialize(
    TUint32 aServiceProviderId,
    CSIPProfile& aSipProfile )
    {
    iServiceProviderId = aServiceProviderId;
    iSipProfile = &aSipProfile;
    }


// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CIpVmbxBase::SubscribeL( TInt aReSubscribe  )
    {
    IPVMEPRINT( "CIpVmbxAppBase::SubscribeL - IN" );

    if ( aReSubscribe < KMinReSubscribeDelta )
        {
        // Resubscribe shouldn't occur too often
        aReSubscribe = KMinReSubscribeDelta;
        }
    else
        if ( aReSubscribe > KMaxReSubscribeDelta )
            {
            // Resubscribe shouldn't occur too infrequently
            aReSubscribe = KMaxReSubscribeDelta;
            }

    iReSubscribePeriod = ( TInt64 ) aReSubscribe * ( TInt64 ) KSecondsToMicro;

    __ASSERT_DEBUG( !iMceOutEvent, Panic( KErrAlreadyExists ) );

    if ( !iMceOutEvent )
        {
        iMceOutEvent = CMceOutEvent::NewL(
            iMceManager,
            *iSipProfile,
            iVmbxUri8,
            KEventHeader8,
            KServerRefreshInterval );
        }
    else
        {
        User::Leave( KErrAlreadyExists );
        }

    if ( CMceEvent::EIdle == iMceOutEvent->State() )
        {
        iMceOutEvent->SubscribeL();
        }
    else
        {
        User::Leave( KErrNotReady );
        }
    iState = ESubscribing;

    IPVMEPRINT( "CIpVmbxAppBase::SubscribeL - OUT" );
    }


// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CIpVmbxBase::TerminateEventL()
    {
    IPVMEPRINT( "CIpVmbxAppBase::TerminateEventL - IN" );

    iReSubscribe->Remove( iUpdateEvent );
    if ( iMceOutEvent && CMceEvent::EActive == iMceOutEvent->State() )
        {
        iMceOutEvent->TerminateL();
        iState = ETerminating;
        }
    else
        {
        User::Leave( KErrNotReady );
        }

    IPVMEPRINT( "CIpVmbxAppBase::TerminateEventL - OUT" );
    }


// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CIpVmbxBase::DeleteEvent()
    {
    delete iMceOutEvent;
    iMceOutEvent = NULL;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CIpVmbxBase::ReSubscribe( TAny* aThis )
    {
    CIpVmbxBase* base = reinterpret_cast<CIpVmbxBase*> ( aThis );
    __ASSERT_DEBUG( base, Panic( KErrArgument ) );
    IPVMEPRINT2(
        "CIpVmbxAppBase::ReSubscribeL - IN state: %d",
        base->iMceOutEvent->State() );

    if ( base && base->iMceOutEvent )
        {
        switch ( base->iMceOutEvent->State() )
            {
            case CMceEvent::EActive:
                {
                TRAPD( err, base->iMceOutEvent->UpdateL( KServerRefreshInterval ) );
                if ( err )
                    {
                    base->iVmbxEngine.HandleMessage(
                        base->iServiceProviderId, CIpVmbxEngine::EEngineNetworkError );
                    }
                base->iPendingResubscribeCount = 0;
                if ( !base->iReSubscribe->IsActive() )
                    {
                    base->iReSubscribe->QueueLong(
                        base->iReSubscribePeriod,
                        base->iUpdateEvent );
                    }
                break;
                }
            case CMceEvent::EPending:
                {
                if ( base->iPendingResubscribeCount++ )
                    {
                    // Allows to skip one resubscribe before error occurs
                    base->iVmbxEngine.HandleMessage(
                        base->iServiceProviderId,
                        CIpVmbxEngine::EEngineNetworkLost );
                    }
                break;
                }
            case CMceEvent::EIdle:
            case CMceEvent::ETerminated:
                {
                base->iVmbxEngine.HandleMessage(
                    base->iServiceProviderId,
                    CIpVmbxEngine::EEngineNetworkLost );
                break;
                }
            default:
                IPVMEPRINT( "Unhandled message!" );
            }
        }

    IPVMEPRINT( "CIpVmbxAppBase::ReSubscribeL - OUT" );
    return KErrNone;
    }


// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CIpVmbxBase::Cancel()
    {
    iState = EDisabled;
    iReSubscribe->Remove( iUpdateEvent );
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TUint32 CIpVmbxBase::ServiceProviderId() const
    {
    return iServiceProviderId;
    }


// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
const TDesC8& CIpVmbxBase::VmbxUrl() const
    {
    return iVmbxUri8;
    }


// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CIpVmbxBase::SetStateRegistered()
    {
    iState = ERegistered;
    if ( !iReSubscribe->IsActive() )
        {
        iReSubscribe->QueueLong( iReSubscribePeriod, iUpdateEvent );
        }
    }


// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
CIpVmbxBase::TIpVmbxBaseStates CIpVmbxBase::State() const
    {
    return iState;
    }


// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CIpVmbxBase::SetAccountMessageCount( TInt aTotalCount, TInt aNewCount )
    {
    iTotalMessageCount = aTotalCount;
    iNewMessageCount = aNewCount;
    }


// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CIpVmbxBase::AccountMessageCount( TInt& aTotalCount, TInt& aNewCount )
    {
    aTotalCount = iTotalMessageCount;
    aNewCount = iNewMessageCount;
    }

