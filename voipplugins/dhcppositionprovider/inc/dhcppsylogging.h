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
* Description: 
*
*/


#ifndef DHCPPSYLOGGING_H
#define DHCPPSYLOGGING_H

//  INCLUDES

// CONSTANTS

// MACROS
#ifndef _DEBUG

/***************************
* Logging method variants:
*   0 = No logging
*   1 = Flogger
*   2 = RDebug
***************************/

#define DPP_LOGGING_METHOD  0   // UREL BUILD

#else

#ifdef __WINS__
#define DPP_LOGGING_METHOD  2   // UDEB BUILD, WINS
#else
#define DPP_LOGGING_METHOD  2   // UDEB BUILD, HW
#endif // __WINS__

#endif // _DEBUG

#if DPP_LOGGING_METHOD == 1      // Flogger

#include <flogger.h>

_LIT( KLoggingFolder, "dhcppsy" );
_LIT( KLoggingFile, "dhcppsylog.txt" );
_LIT( KLoggingFullName, "c:\\logs\\dhcppsy\\dhcppsylog.txt" );

#define CLEARTRACELOG \
    {RFs fs;fs.Connect();CFileMan* fileMan = CFileMan::NewL( fs ); \
    fileMan->Delete( KLoggingFullName ); delete fileMan; fs.Close();\
    }

#define TRACETEXT( S1 )         { RFileLogger::Write( KLoggingFolder(),\
KLoggingFile(), EFileLoggingModeAppend, S1 ); }

#define TRACESTRING( S1 )       { \
    _LIT( tempLogDes, S1 ); \
    RFileLogger::Write( KLoggingFolder(), KLoggingFile(), \
    EFileLoggingModeAppend, tempLogDes() ); \
    }

#define TRACESTRING2( S1, S2 )  { \
    _LIT( tempLogDes, S1 ); \
    RFileLogger::WriteFormat( KLoggingFolder(), KLoggingFile(), \
    EFileLoggingModeAppend, TRefByValue < const TDesC>( tempLogDes() ), S2 ); \
    }

#define TRACERAWPRINT (S1)
#define TRACERAWPRINT8(S1)

#define TRACEHEXDUMP(S1, S1LEN) { RFileLogger::HexDump(KLoggingFolder, \
    KLoggingFile, EFileLoggingModeAppend, 0, 0,S1, S1LEN);\
    }

#elif DPP_LOGGING_METHOD == 2    // RDebug

#include <e32svr.h>

#define CLEARTRACELOG
#define TRACETEXT (S1)             { RDebug::Print(S1); }
#define TRACESTRING(S1)            { RDebug::Print(_L(S1)); }
#define TRACESTRING2(S1,S2)        { RDebug::Print(_L(S1),S2); }
#define TRACERAWPRINT(S1)          { RDebug::RawPrint(S1); }
#define TRACERAWPRINT8(S1)         { RDebug::RawPrint(S1); }
#define TRACEHEXDUMP(S1, S1LEN)
#else

#define CLEARTRACELOG
#define TRACETEXT( S1 )
#define TRACESTRING( S1 )
#define TRACESTRING2( S1, S2 )
#define TRACERAWPRINT (S1)
#define TRACERAWPRINT8(S1)
#define TRACEHEXDUMP(S1, S1LEN)

#endif  // _DEBUG

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

#endif  // DHCPPSYLOGGING_H
// End of File
