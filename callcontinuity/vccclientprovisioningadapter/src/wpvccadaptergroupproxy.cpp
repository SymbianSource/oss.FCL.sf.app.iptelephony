/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Table of plugins for ECom.
*
*/



#include <e32std.h>
#include <ecom/implementationproxy.h>

#include "wpvccadapteruids.h"
#include "wpvccadapter.h"

// ---------------------------------------------------------------------------
// Return implementation proxy entry
// ---------------------------------------------------------------------------
//
const TImplementationProxy KImplementationTable[] =
    {
   	IMPLEMENTATION_PROXY_ENTRY(
    	KProvisioningVccAdapterImplementationUid,
	    CWPVccAdapter::NewL )
    };

// ---------------------------------------------------------------------------
// This function returns a TImplementationProxy* pointer pointing to table
// of implementations allowed to instantiate with this plugin collection.
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
    TInt& aTableCount )
    {
    aTableCount =
        sizeof( KImplementationTable ) / sizeof( TImplementationProxy );
    return KImplementationTable;
    }
