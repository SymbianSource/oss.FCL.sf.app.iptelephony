/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef RCSELOGGER_H
#define RCSELOGGER_H

//  INCLUDES
#include <e32def.h>


//-----------------------------------------------------------------------------
//  USER LOGGING SETTINGS
//-----------------------------------------------------------------------------

#ifndef _DEBUG

/***************************
* Logging method variants:
*   0 = No logging
*   1 = Flogger
*   2 = RDebug
***************************/ 

#define RCSE_LOGGING_METHOD  0   // UREL BUILD

#else

#ifdef __WINS__
#define RCSE_LOGGING_METHOD  2   // UDEB BUILD, WINS
#else
#define RCSE_LOGGING_METHOD  2   // UDEB BUILD, HW
#endif // __WINS__

#endif // _DEBUG


//-----------------------------------------------------------------------------
//  LOGGING SETTINGS (DO NOT EDIT)
//-----------------------------------------------------------------------------

#if RCSE_LOGGING_METHOD == 1      // Flogger

#include <flogger.h>
_LIT(KRCSELogFolder,"RCSE");
_LIT(KRCSELogFile,"RCSELOG.TXT");

#elif RCSE_LOGGING_METHOD == 2    // RDebug

#include <e32svr.h>

#endif


//-----------------------------------------------------------------------------
//  LOGGING MACROs (DO NOT EDIT)
//
//  USE THESE MACROS IN YOUR CODE
//-----------------------------------------------------------------------------

#if RCSE_LOGGING_METHOD == 1      // Flogger

#define RCSELOGSTRING(AA)           { RFileLogger::Write(KRCSELogFolder(),KRCSELogFile(),EFileLoggingModeAppend, _L(AA) ); }
#define RCSELOGSTRING2(AA,BB)       { RFileLogger::WriteFormat(KRCSELogFolder(),KRCSELogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(_L(AA)),BB); }
#define RCSELOGSTRING3(AA,BB,CC)    { RFileLogger::WriteFormat(KRCSELogFolder(),KRCSELogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(_L(AA)),BB,CC); }
#define RCSELOGSTRING4(AA,BB,CC,DD) { RFileLogger::WriteFormat(KRCSELogFolder(),KRCSELogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(_L(AA)),BB,CC,DD); }
#define RCSELOGTHREAD()             { RFileLogger::Write(KRCSELogFolder(),KRCSELogFile(),EFileLoggingModeAppend, RThread().FullName() ); }

#elif RCSE_LOGGING_METHOD == 2    // RDebug

#define RCSELOGSTRING(AA)           { RDebug::Print(_L(AA)); }
#define RCSELOGSTRING2(AA,BB)       { RDebug::Print(_L(AA),BB); }
#define RCSELOGSTRING3(AA,BB,CC)    { RDebug::Print(_L(AA),BB,CC); }
#define RCSELOGSTRING4(AA,BB,CC,DD) { RDebug::Print(_L(AA),BB,CC,DD); }
#define RCSELOGTHREAD()             { RDebug::Print( RThread().FullName() ); }

#else   // RCSE_LOGGING_METHOD == 0 or invalid -> Disable loggings

#define RCSELOGSTRING(AA)           // Example: RCSELOGSTRING("Test");
#define RCSELOGSTRING2(AA,BB)       // Example: RCSELOGSTRING("Test %d", aValue);
#define RCSELOGSTRING3(AA,BB,CC)    // Example: RCSELOGSTRING("Test %d %d", aValue1, aValue2);
#define RCSELOGSTRING4(AA,BB,CC,DD) // Example: RCSELOGSTRING("Test %d %d %d", aValue1, aValue2, aValue3);
#define RCSELOGTHREAD()

#endif  // RCSE_LOGGING_METHOD

#endif  // RCSELOGGER_H
