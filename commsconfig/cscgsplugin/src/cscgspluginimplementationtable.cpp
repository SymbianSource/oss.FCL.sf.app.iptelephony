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
* Description:  cscgsplugin ecom implementation table.
*
*/


#include <e32std.h>
#include <implementationproxy.h>

#include "cscgsplugin.h"

const TImplementationProxy KCscGsPluginImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( 0x2001E63A, CCscGsPlugin::NewL )
    };

// ---------------------------------------------------------------------------
// ImplementationGroupProxy.
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
    TInt& aTableCount )
    {
    aTableCount = sizeof( KCscGsPluginImplementationTable )
        / sizeof( TImplementationProxy );
    return KCscGsPluginImplementationTable;
    }

