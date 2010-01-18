/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Configuration file for RubyDebug macros
*
*/



#ifndef RUBYDEBUGCFG_H
#define RUBYDEBUGCFG_H


#include <e32cmn.h> // TLitC
#include <e32def.h> // _LIT

#include "rubydebugconfigselector.h"

// Common settings
// Is printed at the beginning of the line
_LIT( __K_RUBY_HEADER, "[<SRSF>] ");

// Is printed after __KRUBY_HEADER for the error messages
_LIT( __K_RUBY_ERROR, "[Error!] ");


#if defined(RUBY_EXTERNAL_VARIANT) && defined(__WINS__)
// set of options to use when the project is released to the customers

// Uncomment the following line to disable RUBY_DEBUGs. 
// This has no effect on RUBY_ERRORs
//#define __RUBY_DEBUG_DISABLED

// Uncomment the following line to get traces to C:\Logs\RubyTrace.log
#define __RUBY_DEBUG_TRACES_TO_FILE

// Uncomment the following line to stop disable RUBY_ERRORs
// Error print is always enabled in _DEBUG
//#define RUBY_DISABLE_ERRORS

// Uncomment the following line to disable extra output for RUBY_ASSERT_DEBUG
// RUBY_ASSERT_DEBUGs will become usual __ASSERT_DEBUGs
#define RUBY_DISABLE_ASSERT_DEBUG

// Uncomment the following line to disable automated function name printing
//#define RUBY_DISABLE_FUNCTION_NAMES

//#define RUBY_DIDSPLAY_EVERYTHING_FOR_BLOCKL

// Uncomment the following line to disable leave code printing in RUBY_TRAP_IGNORE
// macro. It would make it equivalent to TRAP_IGNORE
// #define RUBY_DISABLE_TRAP_IGNORE


#else //  RUBY_EXTERNAL_VARIANT
// set of options to use in the internal development

// Uncomment the following line to disable RUBY_DEBUGs. 
// This has no effect on RUBY_ERRORs
//#define __RUBY_DEBUG_DISABLED

// Uncomment the following line to get traces to C:\Logs\RubyTrace.log
//#define __RUBY_DEBUG_TRACES_TO_FILE

// Uncomment the following line to stop disable RUBY_ERRORs
// Error print is always enabled in _DEBUG
//#define RUBY_DISABLE_ERRORS

// Uncomment the following line to disable extra output for RUBY_ASSERT_DEBUG
// RUBY_ASSERT_DEBUGs will become usual __ASSERT_DEBUGs
//#define RUBY_DISABLE_ASSERT_DEBUG

// Uncomment the following line to disable automated function name printing
//#define RUBY_DISABLE_FUNCTION_NAMES

#define RUBY_DIDSPLAY_EVERYTHING_FOR_BLOCKL

// Uncomment the following line to disable leave code printing in RUBY_TRAP_IGNORE
// macro. It would make it equivalent to TRAP_IGNORE
// #define RUBY_DISABLE_TRAP_IGNORE

#endif  //  RUBY_EXTERNAL_VARIANT

#endif // RUBYDEBUGCFG_H
            
// End of File
