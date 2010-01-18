/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of class CCchUiClientObserver.
*
*/


#include "cchuiclientobserver.h"

// ======== MEMBER FUNCTIONS ========

CCchUiClientObserver::CCchUiClientObserver( 
    MCchUiObserver& aObserver ):
    iObserver( aObserver )
    {
    }

CCchUiClientObserver* CCchUiClientObserver::NewL( 
    MCchUiObserver& aObserver )
    {
    CCchUiClientObserver* self = NewLC( aObserver );
    CleanupStack::Pop(self);
    return self;
    }

CCchUiClientObserver* CCchUiClientObserver::NewLC( 
    MCchUiObserver& aObserver )
    {
    CCchUiClientObserver* self = new (ELeave) CCchUiClientObserver( 
        aObserver );
    CleanupStack::PushL(self);
    return self;
    }

CCchUiClientObserver::~CCchUiClientObserver()
    {
    }

// ---------------------------------------------------------------------------
// Returns observer reference
// ---------------------------------------------------------------------------
//
MCchUiObserver& CCchUiClientObserver::Observer()
    {
    return iObserver;
    }         
