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
* Description:  Provides macros for logging
*
*/


#ifndef __SCPLOGGER_H__
#define __SCPLOGGER_H__

#define SCP_TP_PRINT(s) L##s
#define SCP_STRA_PRINT(s) SCP_TP_PRINT(s)
#define SCP_STR_PRINT(t) SCP_STRA_PRINT("[SCP]") L##t
#define SCP_LIT_PRINT(s) TPtrC((const TText *) SCP_STR_PRINT(s))
#define SCP_LIT_CONVERT(s) TPtrC((const TText *)(s))

//-----------------------------------------------------------------------------
//  LOGGING MACROs 
//-----------------------------------------------------------------------------

// Logging method is defined in the mmp file

#ifdef SCP_LOGGING_0

    #define SCPLOGSTRING(a)           
    #define SCPLOGSTRING2(a,b)     
    #define SCPLOGSTRING3(a,b,c)    
    #define SCPLOGSTRING4(a,b,c,d)
    #define SCPLOGSTRING5(a,b,c,d,e) 

#endif

#ifdef SCP_LOGGING_1
    #ifdef _DEBUG
        #include <e32def.h> 
        #include <e32svr.h>   
        #define SCPLOGSTRING(a)          { RDebug::Print(SCP_LIT_PRINT(a)); } 
        #define SCPLOGSTRING2(a,b)       { RDebug::Print(SCP_LIT_PRINT(a),b); } 
        #define SCPLOGSTRING3(a,b,c)     { RDebug::Print(SCP_LIT_PRINT(a),b,c); } 
        #define SCPLOGSTRING4(a,b,c,d)   { RDebug::Print(SCP_LIT_PRINT(a),b,c,d); }  
        #define SCPLOGSTRING5(a,b,c,d,e) { RDebug::Print(SCP_LIT_PRINT(a),b,c,d,e); }     
    #else
        #define SCPLOGSTRING(a)           
        #define SCPLOGSTRING2(a,b)     
        #define SCPLOGSTRING3(a,b,c)    
        #define SCPLOGSTRING4(a,b,c,d) 
        #define SCPLOGSTRING5(a,b,c,d,e) 
    #endif
#endif

#ifdef SCP_LOGGING_2
    #ifdef _DEBUG
        #define SCPLOGSTRING(a) { ScpDebugWriteFormat(SCP_LIT_PRINT(a)); } 
        #define SCPLOGSTRING2(a,b) { ScpDebugWriteFormat(SCP_LIT_PRINT(a),b); } 
        #define SCPLOGSTRING3(a,b,c) { ScpDebugWriteFormat(SCP_LIT_PRINT(a),b,c); }    
        #define SCPLOGSTRING4(a,b,c,d) { ScpDebugWriteFormat(SCP_LIT_PRINT(a),b,c,d); };
        #define SCPLOGSTRING5(a,b,c,d,e) { ScpDebugWriteFormat(SCP_LIT_PRINT(a),b,c,d,e); };     
    #else
        #define SCPLOGSTRING(a)           
        #define SCPLOGSTRING2(a,b)     
        #define SCPLOGSTRING3(a,b,c)    
        #define SCPLOGSTRING4(a,b,c,d) 
        #define SCPLOGSTRING5(a,b,c,d,e) 
    #endif
#endif

#ifdef SCP_LOGGING_3 
    #ifdef _DEBUG   
        #include <e32def.h> 
        #include <e32svr.h>   
        #define SCPLOGSTRING(a)          { RDebug::Print(SCP_LIT_PRINT(a));\
                                           ScpDebugWriteFormat(SCP_LIT_PRINT(a)); } 
        #define SCPLOGSTRING2(a,b)       { RDebug::Print(SCP_LIT_PRINT(a),b);\
                                           ScpDebugWriteFormat(SCP_LIT_PRINT(a),b); } 
        #define SCPLOGSTRING3(a,b,c)     { RDebug::Print(SCP_LIT_PRINT(a),b,c);\
                                           ScpDebugWriteFormat(SCP_LIT_PRINT(a),b,c); } 
        #define SCPLOGSTRING4(a,b,c,d)   { RDebug::Print(SCP_LIT_PRINT(a),b,c,d);\
                                           ScpDebugWriteFormat(SCP_LIT_PRINT(a),b,c,d); }  
        #define SCPLOGSTRING5(a,b,c,d,e) { RDebug::Print(SCP_LIT_PRINT(a),b,c,d,e);\
                                           ScpDebugWriteFormat(SCP_LIT_PRINT(a),b,c,d,e); }     
    #else
        #define SCPLOGSTRING(a)           
        #define SCPLOGSTRING2(a,b)     
        #define SCPLOGSTRING3(a,b,c)    
        #define SCPLOGSTRING4(a,b,c,d)
        #define SCPLOGSTRING5(a,b,c,d,e) 
    #endif     
#endif

#include <flogger.h>

inline void ScpDebugWriteFormat(TRefByValue<const TDesC> aFmt,...) //lint !e960
    {
    _LIT(KDir, "scp"); //lint !e1534
    _LIT(KName, "scp.log"); //lint !e1534

    // take the ellipsis parameters
    VA_LIST args;
    VA_START(args,aFmt); //lint !e960
    RFileLogger::WriteFormat(KDir, KName, EFileLoggingModeAppend, aFmt, args);
    VA_END(args); //lint !e960
    }

#endif  // __SCPLOGGER_H__
