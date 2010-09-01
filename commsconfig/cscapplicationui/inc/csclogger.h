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


#ifndef CSCLOGGER_H
#define CSCLOGGER_H

#include <e32def.h>


//----------------------------------------------------------------------------
//  USER LOGGING SETTINGS
//----------------------------------------------------------------------------

#ifndef _DEBUG

/***************************
* Logging method variants:
*   0 = No logging
*   1 = RDebug
***************************/ 

#define CSC_DEBUG             0   // UREL BUILD

#else

#define CSC_DEBUG             1   // UDEB BUILD

#endif // _DEBUG



//----------------------------------------------------------------------------
//  LOGGING MACROs (DO NOT EDIT)
//
//  USE THESE MACROS IN YOUR CODE
//----------------------------------------------------------------------------

#if CSC_DEBUG == 1    // RDebug

#define CSCDEBUG(AA)           { RDebug::Print(_L(AA)); }
#define CSCDEBUG2(AA,BB)       { RDebug::Print(_L(AA),BB); }
#define CSCDEBUG3(AA,BB,CC)    { RDebug::Print(_L(AA),BB,CC); }
#define CSCDEBUG4(AA,BB,CC,DD) { RDebug::Print(_L(AA),BB,CC,DD); }

#else   // CSC_DEBUG == 0 or invalid -> Disable loggings

#define CSCDEBUG(AA)          
#define CSCDEBUG2(AA,BB)     
#define CSCDEBUG3(AA,BB,CC)    
#define CSCDEBUG4(AA,BB,CC,DD) 

#endif  // CSC_DEBUG

#endif  // CSCLOGGER_H

