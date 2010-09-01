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
* Description: 
*
*/


#ifndef DHCPPSYPANIC_H
#define DHCPPSYPANIC_H


//  INCLUDES
#include <e32std.h>

// CONSTANTS
_LIT( KDhcpPanicGategory, "DHCP PSY" );

// DATA TYPES
enum TDhcpPsyPanicReason
    {
    EPanicPositionRequestOngoing       //There is already position request ongoing
    };

// FUNCTION PROTOTYPES
inline void Panic( TInt aPanicCode )
    {
    User::Panic( KDhcpPanicGategory, aPanicCode );
    }


#endif // DHCPPSYPANIC_H

// End of File
