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
        iVmbxEngine( aVmbxEngine )
    {
    }


// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CIpVmbxBase::ConstructL( TDesC8& aVoiceMailUri8 )
    {
    if ( iVmbxUri8.MaxLength() >= aVoiceMailUri8.Length() )
        {
        iVmbxUri8.Copy( aVoiceMailUri8 );
        }
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

    TUint32 resubscribeInterval = ( TUint32 ) aReSubscribe;
        
    IPVMEPRINT2( "CIpVmbxAppBase::SubscribeL - resubscribe interval=%d",
        resubscribeInterval );
    
    __ASSERT_DEBUG( !iMceOutEvent, Panic( KErrAlreadyExists ) );

    if ( !iMceOutEvent )
        {
        iMceOutEvent = CMceOutEvent::NewL(
            iMceManager,
            *iSipProfile,
            iVmbxUri8,
            KEventHeader8,
            resubscribeInterval );
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


// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CIpVmbxBase::Cancel()
    {
    iState = EDisabled;
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

