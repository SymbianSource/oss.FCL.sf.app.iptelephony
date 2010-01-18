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


#include <e32std.h>
#include <implementationproxy.h>
#include "dhcppsy.hrh"
#include "dhcppsypositioner.h"

// LOCAL CONSTANTS AND MACROS
const TImplementationProxy KFactoryPtr =
    IMPLEMENTATION_PROXY_ENTRY( KDhcpPsyImplUid, CDhcpPsyPositioner::NewL );

// ========================= ==  == LOCAL FUNCTIONS ===============================

// Following function is required by Positioning Plug-in API at ordinal 1.

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
    TInt& aTableCount )
    {
    aTableCount = 1;
    return &KFactoryPtr;
    }

//
