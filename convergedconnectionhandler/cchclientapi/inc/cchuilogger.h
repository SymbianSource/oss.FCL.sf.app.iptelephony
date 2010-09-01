/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Logger definitions
*
*/


#ifndef CCHUILOGGER_H
#define CCHUILOGGER_H

#include <e32def.h>
#include <e32svr.h>

//----------------------------------------------------------------------------
//  USER LOGGING SETTINGS
//----------------------------------------------------------------------------

#ifndef _DEBUG

/***************************
* Logging method variants:
*   0 = No logging
*   1 = RDebug
***************************/ 

#define CCHUI_DEBUG             0   // UREL BUILD

#else

#define CCHUI_DEBUG             1   // UDEB BUILD

#endif // _DEBUG



//----------------------------------------------------------------------------
//  LOGGING MACROs (DO NOT EDIT)
//
//  USE THESE MACROS IN YOUR CODE
//----------------------------------------------------------------------------

#if CCHUI_DEBUG == 1    // RDebug

#define CCHUI_TP_PRINT(s) L##s
#define CCHUI_STRA_PRINT(s) CCHUI_TP_PRINT(s)
#define CCHUI_STR_PRINT(t) CCHUI_STRA_PRINT("[CCHUI]") L##t
#define CCHUI_LIT_PRINT(s) TPtrC((const TText *) CCHUI_STR_PRINT(s))

#define CCHUIDEBUG(AA)           { RDebug::Print(CCHUI_LIT_PRINT(AA)); }
#define CCHUIDEBUG2(AA,BB)       { RDebug::Print(CCHUI_LIT_PRINT(AA),BB); }
#define CCHUIDEBUG3(AA,BB,CC)    { RDebug::Print(CCHUI_LIT_PRINT(AA),BB,CC); }
#define CCHUIDEBUG4(AA,BB,CC,DD) { RDebug::Print(CCHUI_LIT_PRINT(AA),BB,CC,DD); }

#else   // CCHUI_DEBUG == 0 or invalid -> Disable loggings

#define CCHUIDEBUG(AA)          
#define CCHUIDEBUG2(AA,BB)     
#define CCHUIDEBUG3(AA,BB,CC)    
#define CCHUIDEBUG4(AA,BB,CC,DD) 

#endif  // CCHUI_DEBUG

#endif  // CCHUILOGGER_H

