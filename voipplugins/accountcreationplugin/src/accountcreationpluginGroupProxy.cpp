/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Group proxy file for AccountCreationPlugin
 *
*/


#include <e32std.h>
#include <ecom/implementationproxy.h>

#include "accountcreationplugin.h"
#include "accountcreationpluginconstants.h"
#include "accountcreationplugin.hrh"


// Map interface UIDs.
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( 
        KAccountCreationPluginImplUid, CAccountCreationPlugin::NewL )
    };

// ---------------------------------------------------------------------------
// ImplementationGroupProxy
// Exported proxy for instantiation method resolution.
// Returns: ImplementationTable
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( 
    TInt& aTableCount )
    {
    aTableCount = 
        sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
    return ImplementationTable;
    }

// End of file.
