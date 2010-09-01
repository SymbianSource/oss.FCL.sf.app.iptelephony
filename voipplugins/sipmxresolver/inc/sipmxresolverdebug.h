/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides macros for logging and testing
*
*/



#ifndef SIPMXRESOLVERDEBUG_H
#define SIPMXRESOLVERDEBUG_H

//  INCLUDES
#include <e32def.h>

//-----------------------------------------------------------------------------
// LOG/TEST MACROS
//-----------------------------------------------------------------------------

#ifdef _DEBUG
    #include <e32svr.h>
    #define SIPMXRLOG(AA)           { RDebug::Print(_L(AA)); }
    #define SIPMXRLOGP(AA,BB)       { RDebug::Print(_L(AA),BB); }
    #define SIPMXR_TEST(AA)         friend class AA; 
#else
    #define SIPMXRLOG(AA)           // Example: SIPMXRLOG("Test");
    #define SIPMXRLOGP(AA,BB)       // Example: SIPMXRLOGP("Test %d", aValue);
    #define SIPMXR_TEST(AA)
#endif

#endif  //SIPMXRESOLVERDEBUG_H
