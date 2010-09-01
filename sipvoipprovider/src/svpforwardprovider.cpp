/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Forward functionality provider
*
*/



#include "svpforwardprovider.h"
#include "svpmosession.h"
#include "svplogger.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSVPForwardProvider::CSVPForwardProvider
// ---------------------------------------------------------------------------
//
CSVPForwardProvider::CSVPForwardProvider()
    {

    }

// ---------------------------------------------------------------------------
// CSVPForwardProvider::ConstructL
// ---------------------------------------------------------------------------
//
void CSVPForwardProvider::ConstructL( CSVPMoSession& aSession )
    {
    
    SVPDEBUG1( "CSVPForwardProvider::ConstructL" );
    iMoSession = const_cast< CSVPMoSession* >( &aSession );
    
    }


// ---------------------------------------------------------------------------
// CSVPForwardProvider::NewL
// ---------------------------------------------------------------------------
//
CSVPForwardProvider* CSVPForwardProvider::NewL( CSVPMoSession& aSession )
    {
    CSVPForwardProvider* self = new( ELeave ) CSVPForwardProvider;
    CleanupStack::PushL( self );
    self->ConstructL( aSession );
    CleanupStack::Pop( self );
    return self;
    }



// ---------------------------------------------------------------------------
// CSVPForwardProvider::~CSVPForwardProvider
// ---------------------------------------------------------------------------
//
CSVPForwardProvider::~CSVPForwardProvider()
    {
    
    }




// ---------------------------------------------------------------------------
// From class MCCPForwardProvider.
// CSVPForwardProvider::GetForwardAddressChoicesL
// ---------------------------------------------------------------------------
//
const CDesC8Array& CSVPForwardProvider::GetForwardAddressChoicesL()
    {
    
    SVPDEBUG1( "CSVPForwardProvider::GetForwardAddressChoicesL" );
    return iMoSession->GetForwardAddressChoicesL();
    
    }
    
// ---------------------------------------------------------------------------
// From class MCCPForwardProvider.
// CSVPForwardProvider::ForwardToAddressL
// ---------------------------------------------------------------------------
//
void CSVPForwardProvider::ForwardToAddressL( const TInt aIndex )
    {
    
    SVPDEBUG1( "CSVPForwardProvider::ForwardToAddressL" );
    iMoSession->ForwardToAddressL( aIndex );
    
    }

// ---------------------------------------------------------------------------
// From class MCCPForwardProvider.
// CSVPForwardProvider::AddObserverL
// ---------------------------------------------------------------------------
//
void CSVPForwardProvider::AddObserverL( const MCCPForwardObserver& aObserver )
    {
    
    SVPDEBUG1( "CSVPForwardProvider::AddObserverL" );
    iMoSession->AddForwardObserverL( aObserver );
    
    }
    
// ---------------------------------------------------------------------------
// From class MCCPForwardProvider.
// CSVPForwardProvider::RemoveObserver
// ---------------------------------------------------------------------------
//
TInt CSVPForwardProvider::RemoveObserver( const MCCPForwardObserver& aObserver )
    {
    
    SVPDEBUG1( "CSVPForwardProvider::RemoveObserver" );
    return iMoSession->RemoveForwardObserver( aObserver );
    
    }
