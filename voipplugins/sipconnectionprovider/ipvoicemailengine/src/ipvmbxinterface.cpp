/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Interface class to control voice mail engine
*
*/



#include "ipvmbxinterface.h"
#include "ipvmbxengine.h"
#include "ipvmbxlogger.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CIpVmbxInterface::CIpVmbxInterface( MIpVmbxObserver& aEngineObserver ) : 
    iEngineObserver( aEngineObserver )
    {
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CIpVmbxInterface::ConstructL()
    {
    iIpVmbxEngine = CIpVmbxEngine::NewL( *this );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CIpVmbxInterface* CIpVmbxInterface::NewL( 
    MIpVmbxObserver& aEngineObserver )
    {
    IPVMEPRINT( "CIpVmbxInterface::NewL - IN" );
    
    CIpVmbxInterface* self = new( ELeave ) CIpVmbxInterface( aEngineObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    IPVMEPRINT( "CIpVmbxInterface::NewL - OUT" );
    return self;
    }
    

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CIpVmbxInterface::~CIpVmbxInterface()
    {
    IPVMEPRINT( "CIpVmbxInterface::~CIpVmbxInterface - IN" );
    
    delete iIpVmbxEngine;
    
    IPVMEPRINT( "CIpVmbxInterface::~CIpVmbxInterface - OUT" );
    }


// ---------------------------------------------------------------------------
// Subscribes profile to ip voice mail server
// ---------------------------------------------------------------------------
//
EXPORT_C void CIpVmbxInterface::SubscribeL( 
    TUint32 aServiceProviderId, 
    CSIPProfile& aSipProfile )
    {
    IPVMEPRINT( "CIpVmbxInterface::SubscribeL - IN" );
    
    iIpVmbxEngine->SubscribeProfileL( aServiceProviderId, aSipProfile );
    
    IPVMEPRINT( "CIpVmbxInterface::SubscribeL - OUT" );
    }


// ---------------------------------------------------------------------------
// Un-subscribes profile from ip voice mail server
// ---------------------------------------------------------------------------
//
EXPORT_C void CIpVmbxInterface::UnsubscribeL( TUint32 aServiceProviderId )
    {
    IPVMEPRINT( "CIpVmbxInterface::UnsubscribeL - IN" );
    
    iIpVmbxEngine->UnsubscribeProfileL( aServiceProviderId );
    
    IPVMEPRINT( "CIpVmbxInterface::UnsubscribeL - OUT" );
    }


// ---------------------------------------------------------------------------
// Check if profile is registered to ip voice mail server
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CIpVmbxInterface::IsSubscribed( 
    TUint32 aServiceProviderId, 
    TBool& aProfileSubscribed ) const
    {
    
    return iIpVmbxEngine->ProfileSubscribed( aServiceProviderId, aProfileSubscribed );
    }


// ---------------------------------------------------------------------------
// Send messages through observer
// ---------------------------------------------------------------------------
//
void CIpVmbxInterface::SendMessage( 
    TUint32 aServiceProviderId, 
    MIpVmbxObserver::TVmbxMessage aMessage )
    {
    IPVMEPRINT( "CIpVmbxInterface::SendMessage - IN" );
    
    iEngineObserver.HandleMessage( aServiceProviderId, aMessage );
    
    IPVMEPRINT( "CIpVmbxInterface::SendMessage - OUT" );
    }
