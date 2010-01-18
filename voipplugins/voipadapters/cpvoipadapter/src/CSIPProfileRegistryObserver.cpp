/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// CSIPProfileRegistryObserver::CSIPProfileRegistryObserver()
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
CSIPProfileRegistryObserver::CSIPProfileRegistryObserver()
    {
    }

// ---------------------------------------------------------------------------
// CSIPProfileRegistryObserver::ConstructL()
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//lint -e{1762} ConstructL is not const
void CSIPProfileRegistryObserver::ConstructL()
    {
    }

CSIPProfileRegistryObserver* CSIPProfileRegistryObserver::NewLC()
    {
    CSIPProfileRegistryObserver* self =
        new ( ELeave ) CSIPProfileRegistryObserver();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CSIPProfileRegistryObserver* CSIPProfileRegistryObserver::NewL()
    {
    CSIPProfileRegistryObserver* self =
        new ( ELeave ) CSIPProfileRegistryObserver();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
CSIPProfileRegistryObserver::~CSIPProfileRegistryObserver()
   {   
   }

// ---------------------------------------------------------------------------
// CSIPProfileRegistryObserver::ProfileRegistryEventOccurred.
// ---------------------------------------------------------------------------
void CSIPProfileRegistryObserver::ProfileRegistryEventOccurred( TUint32 
    /*aProfileId*/, TEvent /*aEvent*/ )
    {    
    }
// ---------------------------------------------------------------------------
// CSIPProfileRegistryObserver::ProfileRegistryEventOccurred.
// ---------------------------------------------------------------------------
void CSIPProfileRegistryObserver::ProfileRegistryErrorOccurred( TUint32 
    /*aProfileId*/, TInt /*aError*/ )
    {    
    }

//  End of File
