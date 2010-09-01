/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __CCHLOGGER_H__
#define __CCHLOGGER_H__

// INCLUDES
#include <e32def.h>
#include <e32svr.h>
#include <e32base.h>
#include <e32std.h>

// CONSTS
const TInt KMaxLogLineLength = 512;
_LIT8( KModuleName8, "CCH" );

//----------------------------------------------------------------------------
//  USER LOGGING SETTINGS
//----------------------------------------------------------------------------

#ifndef _DEBUG

/***************************
 * Logging method variants:
 *   0 = No logging
 *   1 = Flogger
 *   2 = RDebug
 ***************************/ 

#define CCH_LOGGING_METHOD      0   // UREL BUILD

#else

#ifdef __WINS__
#define CCH_LOGGING_METHOD      2   // UDEB BUILD, WINS
#else
#define CCH_LOGGING_METHOD      2   // UDEB BUILD, HW
#endif // __WINS__

#ifdef TRACE_ENTRY_EXIT_POINT // MACRO IN MMP FILE
    #define TRACE_ENTRY_POINT TRACE_FUNC_ENTRY_DEBUG_PRINT
    #define TRACE_EXIT_POINT TRACE_FUNC_EXIT_DEBUG_PRINT    
#endif

#endif // _DEBUG

// [module_name]_ENTRY_POINT;[thread_id];[function_name];[filename]:[line_number];
_LIT8( KFuncEntryFormat8, "%S_ENTRY;%d;%S;%S:%d;");
// [module_name]_EXIT_POINT;[thread_id];[function_name];[filename]:[line_number];
_LIT8( KFuncExitFormat8,  "%S_EXIT_;%d;%S;%S:%d;");
//-----------------------------------------------------------------------------
//  LOGGING SETTINGS (DO NOT EDIT)
//-----------------------------------------------------------------------------

#if CCH_LOGGING_METHOD == 1      // Flogger

#include <flogger.h>
_LIT(KCCHLogFolder,"CCH");
_LIT(KCCHLogFile,"CCHLOG.TXT");

#elif CCH_LOGGING_METHOD == 2    // RDebug
//Check if entry exit trace should be activated 




#endif


NONSHARABLE_CLASS( TOverflowTruncate8 ) : public TDes8Overflow
    {

public:
    void Overflow( TDes8& /*aDes*/ ) {}
    };

//Trace macro 8bit
inline void TraceDebugPrint( TRefByValue<const TDesC8> aFmt, ...)
    {
    VA_LIST list;
    VA_START( list, aFmt );
        
    TOverflowTruncate8 overflow;
    TBuf8<KMaxLogLineLength> buf8;
    buf8.AppendFormatList( aFmt, list, &overflow );
    TBuf16<KMaxLogLineLength> buf16( buf8.Length() );
    buf16.Copy( buf8 );
    TRefByValue<const TDesC> tmpFmt( _L("%S") );
    RDebug::Print( tmpFmt, &buf16 );
    }

//-----------------------------------------------------------------------------
//  LOGGING MACROs (DO NOT EDIT)
//
//  USE THESE MACROS IN YOUR CODE !
//-----------------------------------------------------------------------------

#if CCH_LOGGING_METHOD == 1      // Flogger

#define CCHLOGSTRING(AA)           { RFileLogger::Write(KCCHLogFolder(),KCCHLogFile(),EFileLoggingModeAppend, _L(AA) ); }
#define CCHLOGSTRING2(AA,BB)       { RFileLogger::WriteFormat(KCCHLogFolder(),KCCHLogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(_L(AA)),BB); }
#define CCHLOGSTRING3(AA,BB,CC)    { RFileLogger::WriteFormat(KCCHLogFolder(),KCCHLogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(_L(AA)),BB,CC); }
#define CCHLOGSTRING4(AA,BB,CC,DD) { RFileLogger::WriteFormat(KCCHLogFolder(),KCCHLogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(_L(AA)),BB,CC,DD); }

#elif CCH_LOGGING_METHOD == 2    // RDebug

#define CCHLOGSTRING(AA)           { RDebug::Print(_L(AA)); }
#define CCHLOGSTRING2(AA,BB)       { RDebug::Print(_L(AA),BB); }
#define CCHLOGSTRING3(AA,BB,CC)    { RDebug::Print(_L(AA),BB,CC); }
#define CCHLOGSTRING4(AA,BB,CC,DD) { RDebug::Print(_L(AA),BB,CC,DD); }



#else   // CCH_LOGGING_METHOD == 0 or invalid -> Disable loggings

#define CCHLOGSTRING(AA)           // Example: CCHLOGSTRING("Test");
#define CCHLOGSTRING2(AA,BB)       // Example: CCHLOGSTRING("Test %d", aValue);
#define CCHLOGSTRING3(AA,BB,CC)    // Example: CCHLOGSTRING("Test %d %d", aValue1, aValue2);
#define CCHLOGSTRING4(AA,BB,CC,DD) // Example: CCHLOGSTRING("Test %d %d %d", aValue1, aValue2, aValue3);

#define TRACE_ENTRY_POINT
#define TRACE_EXIT_POINT
    
#endif  // CCH_LOGGING_METHOD

#ifdef TRACE_ENTRY_EXIT_POINT // MACRO IN MMP FILE
    #ifdef __ARMCC__
        #define TRACE_FUNC_ENTRY_DEBUG_PRINT                                                                \
        {                                                                                                   \
            const TUint threadId( RThread().Id() );                                                         \
            TPtrC8 ptr8File( (TUint8*)__MODULE__ );                                                         \
            TPtrC8 ptr8PF( (TUint8*)__PRETTY_FUNCTION__ );                                                  \
            TraceDebugPrint( KFuncEntryFormat8, &KModuleName8, threadId, &ptr8PF, &ptr8File, __LINE__ );    \
        }
        #define TRACE_FUNC_EXIT_DEBUG_PRINT                                                                 \
        {                                                                                                   \
            const TUint threadId( RThread().Id() );                                                         \
            TPtrC8 ptr8File( (TUint8*)__MODULE__ );                                                         \
            TPtrC8 ptr8PF( (TUint8*)__PRETTY_FUNCTION__ );                                                  \
            TraceDebugPrint( KFuncExitFormat8, &KModuleName8, threadId, &ptr8PF, &ptr8File, __LINE__ );     \
        }
    #else // __WINS__
        #define TRACE_FUNC_ENTRY_DEBUG_PRINT                                                                \
        {                                                                                                   \
            const TUint threadId( RThread().Id() );                                                         \
            TPtrC8 ptr8File( (TUint8*)__FILE__ );                                                           \
            TPtrC8 ptr8PF( (TUint8*)__PRETTY_FUNCTION__ );                                                  \
            TraceDebugPrint( KFuncEntryFormat8, &KModuleName8, threadId, &ptr8PF, &ptr8File, __LINE__ );    \
        }
        #define TRACE_FUNC_EXIT_DEBUG_PRINT                                                                 \
        {                                                                                                   \
            const TUint threadId( RThread().Id() );                                                         \
            TPtrC8 ptr8File( (TUint8*)__FILE__ );                                                           \
            TPtrC8 ptr8PF( (TUint8*)__PRETTY_FUNCTION__ );                                                  \
            TraceDebugPrint( KFuncExitFormat8, &KModuleName8, threadId, &ptr8PF, &ptr8File, __LINE__ );     \
        }           
    #endif
#endif

#endif  // CCHLOGGER_H
