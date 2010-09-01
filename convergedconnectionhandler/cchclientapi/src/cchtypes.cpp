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
* Description:  Cch basic types
*
*/


// INCLUDE FILES
#include "cch.h"
#include "cchtypes.h"

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

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ============================= LOCAL FUNCTIONS =============================

// ============================ MEMBER FUNCTIONS =============================

// --------------------------------------------------------------------------
// TCchServiceStatus::TCchServiceStatus
// --------------------------------------------------------------------------
//
EXPORT_C TCchServiceStatus::TCchServiceStatus()
: iState( ECCHUninitialized )
, iError( KErrNone )
    {
    }

// --------------------------------------------------------------------------
// TCchServiceStatus::State
// --------------------------------------------------------------------------
//
EXPORT_C TCCHSubserviceState TCchServiceStatus::State() const
	{ 
	return iState; 
	}

// --------------------------------------------------------------------------
// TCchServiceStatus::Error
// --------------------------------------------------------------------------
//	
EXPORT_C TInt TCchServiceStatus::Error() const
	{ 
	return iError; 
	}

// --------------------------------------------------------------------------
// TCchServiceStatus::SetState
// --------------------------------------------------------------------------
//
EXPORT_C void TCchServiceStatus::SetState( TCCHSubserviceState aState )
	{ 
	iState = aState; 
	}

// --------------------------------------------------------------------------
// TCchServiceStatus::SetError
// --------------------------------------------------------------------------
//
EXPORT_C void TCchServiceStatus::SetError( TInt aError )
	{ 
	iError = aError; 
	}
