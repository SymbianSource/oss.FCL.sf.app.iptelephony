/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Panic method
*
*/



// INCLUDE FILES
#include    "rcsepanic.h"

// LOCAL CONSTANTS AND MACROS
_LIT( KRCSEPanic, "RCSE" );


// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RCSEPanic, Panics the current thread
// -----------------------------------------------------------------------------
//
void RCSEPanic( 
    TRCSEPanic aPanic )
    {
    User::Panic( KRCSEPanic, aPanic );
    }

//  End of File  
