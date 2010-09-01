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
* Description:   ECOM proxy file for nsmldmvccadapter
*
*/



#include    <e32base.h>
#include    <ecom/implementationproxy.h>
#include    "nsmldmvccadapter.h"

// ============================ MEMBER FUNCTIONS =============================

const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( 0x2000E5BE, CNSmlDmVCCAdapter::NewL )
    };

// Function used to return an instance of the proxy table.
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
    TInt& aTableCount )
    {
    //RDebug::Print(_L("SYNCML UMA ADAPTER - ImplementationGroupProxy"));
    aTableCount = sizeof( ImplementationTable )
                        / sizeof( TImplementationProxy );
    return ImplementationTable;
    }

//  End of File
