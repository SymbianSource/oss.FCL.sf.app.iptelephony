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
* Description:  Implements MSIPProfileRegistryObserver interface.
*
*/


// INCLUDE FILES
#include "CSIPProfileRegistryObserver.h"

// ---------------------------------------------------------------------------
// CSIPProfileRegistryObserver::CSIPProfileRegistryObserver();
// ---------------------------------------------------------------------------
//
CSIPProfileRegistryObserver::CSIPProfileRegistryObserver()
    {
    }

// ---------------------------------------------------------------------------
// CSIPProfileRegistryObserver::ConstructL()
// ---------------------------------------------------------------------------
//
void CSIPProfileRegistryObserver::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// CSIPProfileRegistryObserver::NewLC
// ---------------------------------------------------------------------------
//
CSIPProfileRegistryObserver* CSIPProfileRegistryObserver::NewLC()
    {
    CSIPProfileRegistryObserver* self =
        new ( ELeave ) CSIPProfileRegistryObserver();
    CleanupStack::PushL( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CSIPProfileRegistryObserver::NewL
// ---------------------------------------------------------------------------
//
CSIPProfileRegistryObserver* CSIPProfileRegistryObserver::NewL()
    {
    CSIPProfileRegistryObserver* self =
        new ( ELeave ) CSIPProfileRegistryObserver();
    CleanupStack::PushL( self );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CSIPProfileRegistryObserver::~CSIPProfileRegistryObserver
// ---------------------------------------------------------------------------
//
CSIPProfileRegistryObserver::~CSIPProfileRegistryObserver()
   {
   }

// ---------------------------------------------------------------------------
// CSIPProfileRegistryObserver::ProfileRegistryEventOccurred.
// ---------------------------------------------------------------------------
//
void CSIPProfileRegistryObserver::ProfileRegistryEventOccurred( TUint32 
    /*aProfileId*/, TEvent /*aEvent*/ )
    {
    }
// ---------------------------------------------------------------------------
// CSIPProfileRegistryObserver::ProfileRegistryEventOccurred.
// ---------------------------------------------------------------------------
//
void CSIPProfileRegistryObserver::ProfileRegistryErrorOccurred( TUint32 
    /*aProfileId*/, TInt /*aError*/ )
    {
    }

//  End of File
