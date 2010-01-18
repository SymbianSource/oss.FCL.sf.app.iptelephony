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
* Description:  
*
*/


#include <e32std.h>
#include <implementationproxy.h>
#include "nsmldmvoipadapter.h"

const TUint  KNSmlDMVoIPAdapterImplUid = 0x10201F9D;

// -----------------------------------------------------------------------------
// TImplementationProxy ImplementationTable[]
// Needed because of ECOM architecture
// -----------------------------------------------------------------------------
const TImplementationProxy ImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY( KNSmlDMVoIPAdapterImplUid,
        CNSmlDmVoIPAdapter::NewL )
    };

// -----------------------------------------------------------------------------
// ImplementationGroupProxy()
// Needed because of ECOM architecture
// -----------------------------------------------------------------------------
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
    TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) / sizeof(
        TImplementationProxy );
    return ImplementationTable;
    }

//  End of File
