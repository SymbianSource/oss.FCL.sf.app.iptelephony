/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef SIPIMRESOLVERDEBUG_H
#define SIPIMRESOLVERDEBUG_H

#include <e32std.h>

//-----------------------------------------------------------------------------
// LOG/TEST MACROS
//-----------------------------------------------------------------------------

#ifdef _DEBUG
    #include <e32svr.h>
    #define SIPIMRLOG(AA)           { RDebug::Print(_L(AA)); }
    #define SIPIMRLOGP(AA,BB)       { RDebug::Print(_L(AA),BB); }
    #define SIPIMR_TEST(AA)         friend class AA; 
#else
    #define SIPIMRLOG(AA)           // Example: SIPIMRLOG("Test");
    #define SIPIMRLOGP(AA,BB)       // Example: SIPIMRLOGP("Test %d", aValue);
    #define SIPIMR_TEST(AA)
#endif

#endif  //SIPIMRESOLVERDEBUG_H

