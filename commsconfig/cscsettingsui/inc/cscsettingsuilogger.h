/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides macros for logging
*
*/


#ifndef CSCSETTINGSUILOGGER_H
#define CSCSETTINGSUILOGGER_H

#include <e32def.h>
#include <e32debug.h>


//----------------------------------------------------------------------------
//  USER LOGGING SETTINGS
//----------------------------------------------------------------------------

#ifndef _DEBUG

/***************************
* Logging method variants:
*   0 = No logging
*   1 = RDebug
***************************/ 

#define CSCSETUI_DEBUG             0   // UREL BUILD

#else

#define CSCSETUI_DEBUG             1   // UDEB BUILD

#endif // _DEBUG



//----------------------------------------------------------------------------
//  LOGGING MACROs (DO NOT EDIT)
//
//  USE THESE MACROS IN YOUR CODE
//----------------------------------------------------------------------------

#if CSCSETUI_DEBUG == 1    // RDebug

#define CSCSETUIDEBUG(AA)           { RDebug::Print(_L(AA)); }
#define CSCSETUIDEBUG2(AA,BB)       { RDebug::Print(_L(AA),BB); }
#define CSCSETUIDEBUG3(AA,BB,CC)    { RDebug::Print(_L(AA),BB,CC); }
#define CSCSETUIDEBUG4(AA,BB,CC,DD) { RDebug::Print(_L(AA),BB,CC,DD); }

#else   // CSCSETUI_DEBUG == 0 or invalid -> Disable loggings

#define CSCSETUIDEBUG(AA)          
#define CSCSETUIDEBUG2(AA,BB)     
#define CSCSETUIDEBUG3(AA,BB,CC)    
#define CSCSETUIDEBUG4(AA,BB,CC,DD) 

#endif  // CSCSETUI_DEBUG

#endif  // CSCSETTINGSUILOGGER_H
