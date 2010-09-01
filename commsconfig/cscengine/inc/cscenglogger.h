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


#ifndef CSCENGLOGGER_H
#define CSCENGLOGGER_H

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

#define CSCENG_DEBUG             0   // UREL BUILD

#else

#define CSCENG_DEBUG             1   // UDEB BUILD

#endif // _DEBUG



//----------------------------------------------------------------------------
//  LOGGING MACROs (DO NOT EDIT)
//
//  USE THESE MACROS IN YOUR CODE
//----------------------------------------------------------------------------

#if CSCENG_DEBUG == 1    // RDebug

#define CSCENGDEBUG(AA)           { RDebug::Print(_L(AA)); }
#define CSCENGDEBUG2(AA,BB)       { RDebug::Print(_L(AA),BB); }
#define CSCENGDEBUG3(AA,BB,CC)    { RDebug::Print(_L(AA),BB,CC); }
#define CSCENGDEBUG4(AA,BB,CC,DD) { RDebug::Print(_L(AA),BB,CC,DD); }

#else   // CSCENG_DEBUG == 0 or invalid -> Disable loggings

#define CSCENGDEBUG(AA)          
#define CSCENGDEBUG2(AA,BB)     
#define CSCENGDEBUG3(AA,BB,CC)    
#define CSCENGDEBUG4(AA,BB,CC,DD) 

#endif  // CSCENG_DEBUG

#endif  // CSCENGLOGGER_H
