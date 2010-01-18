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



#ifndef RCSEPANIC_H
#define RCSEPANIC_H

//  INCLUDES
#include <e32std.h>

// DATA TYPES
enum TRCSEPanic
    {
    ERCSEUnknownCodec,
    ERCSEArraysDifferentSize,
    ERCSEZeroArray,
    ERCSEUnknownColumn,
    ERCSEUnknownRegistry,
    ERCSEKeyError,
    ERCSETransactionStart
    };

// FUNCTION PROTOTYPES
void RCSEPanic( TRCSEPanic aPanicCode );

#endif      // RCSEPANIC_H   
            
// End of File
