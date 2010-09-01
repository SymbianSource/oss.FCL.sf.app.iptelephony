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
* Description:  
*
*/



#ifndef VOIPEVENTLOGGER_H
#define VOIPEVENTLOGGER_H

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

#define VELAPI_LOGGING_METHOD  0   // UREL BUILD

#else

#ifdef __WINS__
#define VELAPI_LOGGING_METHOD  2   // UDEB BUILD, WINS
#else
#define VELAPI_LOGGING_METHOD  2   // UDEB BUILD, HW
#endif // __WINS__

#endif // _DEBUG


//-----------------------------------------------------------------------------
//  LOGGING SETTINGS (DO NOT EDIT)
//-----------------------------------------------------------------------------

#if VELAPI_LOGGING_METHOD == 1      // Flogger

#include <flogger.h>
_LIT(KVELAPILogFolder,"VELAPI");
_LIT(KVELAPIAPILogFile,"VELAPI.TXT");

#elif VELAPI_LOGGING_METHOD == 2    // RDebug

#include <e32svr.h>

#endif


//-----------------------------------------------------------------------------
//  LOGGING MACROs (DO NOT EDIT)
//
//  USE THESE MACROS IN YOUR CODE
//-----------------------------------------------------------------------------

#if VELAPI_LOGGING_METHOD == 1      // Flogger

#define VELLOGSTRING(AA)           { RFileLogger::Write(KXSPSAPILogFolder(),KXSPSAPILogFile(),EFileLoggingModeAppend, _L(AA) ); }
#define VELLOGSTRING2(AA,BB)       { RFileLogger::WriteFormat(KXSPSAPILogFolder(),KXSPSAPILogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(_L(AA)),BB); }
#define VELLOGSTRING3(AA,BB,CC)    { RFileLogger::WriteFormat(KXSPSAPILogFolder(),KXSPSAPILogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(_L(AA)),BB,CC); }
#define VELLOGSTRING4(AA,BB,CC,DD) { RFileLogger::WriteFormat(KXSPSAPILogFolder(),KXSPSAPILogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(_L(AA)),BB,CC,DD); }

#elif VELAPI_LOGGING_METHOD == 2    // RDebug

#define VELLOGSTRING(AA)           { RDebug::Print(_L(AA)); }
#define VELLOGSTRING2(AA,BB)       { RDebug::Print(_L(AA),BB); }
#define VELLOGSTRING3(AA,BB,CC)    { RDebug::Print(_L(AA),BB,CC); }
#define VELLOGSTRING4(AA,BB,CC,DD) { RDebug::Print(_L(AA),BB,CC,DD); }

#else   // XSPSAPI_LOGGING_METHOD == 0 or invalid -> Disable loggings

#define VELLOGSTRING(AA)           // Example: XSPSLOGSTRING("Test");
#define VELLOGSTRING2(AA,BB)       // Example: XSPSLOGSTRING("Test %d", aValue);
#define VELLOGSTRING3(AA,BB,CC)    // Example: XSPSLOGSTRING("Test %d %d", aValue1, aValue2);
#define VELLOGSTRING4(AA,BB,CC,DD) // Example: XSPSLOGSTRING("Test %d %d %d", aValue1, aValue2, aValue3);

#endif  // VELAPI_LOGGING_METHOD

#endif  // VELAPILOGGER_H
