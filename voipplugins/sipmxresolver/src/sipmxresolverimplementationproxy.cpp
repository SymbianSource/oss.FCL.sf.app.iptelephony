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
* Description:  ECOM implementation proxy for sipmxresolver
*
*/



// INCLUDE FILES
#include <e32base.h>
#include "sipmxresolver.h"
#include <e32svr.h>
#include "sipmxresolveruids.h"

// Exported proxy for instantiation method resolution
// Use the interface implementation UID and Implementation factory method
// as a pair for ECom instantiation.
const TImplementationProxy Implementations[] = 
    {
    // Remove PCLint warning "Suspicious cast"
    //lint -e611
    IMPLEMENTATION_PROXY_ENTRY( KSIPMXRESOLVERIMPLUID, CSipMXResolver::NewL )
    //lint +e611
    };

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// ImplementationGroupProxy
// Function to return the implementation proxy table 
// -----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof( Implementations ) / sizeof( TImplementationProxy );
    return Implementations;
    }

