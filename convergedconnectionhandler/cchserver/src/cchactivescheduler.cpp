/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CCchActiveScheduler implementation
*
*/

// INCLUDE FILES
#include "cchlogger.h"
#include "cchactivescheduler.h"
#include "cchserverbase.h"

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES
// None

// CONSTANTS
// None

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// ============================= LOCAL FUNCTIONS =============================

// -----------------------------------------------------------------------------
// CCchActiveScheduler::CCchActiveScheduler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCchActiveScheduler::CCchActiveScheduler()
    {
    }

// Destructor
CCchActiveScheduler::~CCchActiveScheduler()
    {
    }

// -----------------------------------------------------------------------------
// CCchActiveScheduler::SetServer
//
// -----------------------------------------------------------------------------
//
void CCchActiveScheduler::SetServer( 
    CCCHServerBase* aServer )
    {
    iServer = aServer;
    }

// -----------------------------------------------------------------------------
// CCchActiveScheduler::Error
//
// -----------------------------------------------------------------------------
//
void CCchActiveScheduler::Error( 
    TInt aError ) const
    {
    CCHLOGSTRING2("CCchActiveScheduler::Error: %d", aError);

    if ( KErrServerTerminated == aError )
        {
        CCHLOGSTRING("CCchActiveScheduler - Server terminated");
        if ( iServer )
            {
            iServer->Restart();
            }
        }
    }

//  End of File
