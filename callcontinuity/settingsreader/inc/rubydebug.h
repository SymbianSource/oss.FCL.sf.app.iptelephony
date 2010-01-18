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
* Description:   Interface of debugging utilities.
*      %version: 3 %
*
*/



#ifndef RUBYDEBUG_H
#define RUBYDEBUG_H

#include "rubydebugcfg.h"

//  INCLUDES
#include <e32svr.h> // For RDebug

#ifndef RUBY_DISABLE_FUNCTION_NAMES
    // No sense to store function name longer, than can be printed
    // 0x100 is the internal RDebug limit
    const TInt __K_RUBY_FUNCTION_NAME_LENGTH = 0x50;

    // Prepares function name for printing
    #define __RUBY_PREPARE_FUNCTION_NAME \
    TBuf<__K_RUBY_FUNCTION_NAME_LENGTH> __ruby_function_name__;\
    __ruby_function_name__.Copy( TPtrC8( (TText8*)&__PRETTY_FUNCTION__ ).Left( __K_RUBY_FUNCTION_NAME_LENGTH ) ); \

    // Token to paste the function name into
    #define __K_RUBY_FUNCTION_NAME_TOKEN "%S "

    // Comma and function name
    #define __RUBY_FUNCTION_NAME __ruby_function_name__

    // A wrapper that prepares and supplies the function name as __ruby_function_name__
    #define __RUBY_FUNCTION_NAMED( text ) \
        {\
        __RUBY_PREPARE_FUNCTION_NAME;\
        text;\
        }

    #define __RUBY_FUNCTION_NAME_POINTER8 (TText8*)&__PRETTY_FUNCTION__


#else  // RUBY_DISABLE_FUNCTION_NAMES

    #define __RUBY_PREPARE_FUNCTION_NAME

    // Don't print the function name
    #define __K_RUBY_FUNCTION_NAME_TOKEN ""

    // exclude the whole token, with the comma
    #define __RUBY_FUNCTION_NAME KNullDesC

    // no wrapping
    #define __RUBY_FUNCTION_NAMED( text ) text

    #define __RUBY_FUNCTION_NAME_POINTER8 NULL

#endif // RUBY_DISABLE_FUNCTION_NAMES

// Macro for printing filename both in unicode and non-unicode builds


#ifdef _UNICODE
    #define __RUBY_WIDEN2(x) L ## x
    #define __RUBY_WIDEN(x) __RUBY_WIDEN2(x)
    #define __RUBY_DBG_FILE__ __RUBY_WIDEN(__FILE__)
#else
    #define __RUBY_DBG_FILE__ __FILE__
#endif//_UNICODE



// Debugging is enabled only in _DEBUG builds
//#ifdef _DEBUG

// Select the debug output method
#ifndef __RUBY_DEBUG_TRACES_TO_FILE
#define RUBY_DEBUG_METHOD RDebug::Print
#else
#define RUBY_DEBUG_METHOD RRubyDebug::PrintToFile
#endif // __RUBY_DEBUG_TRACES_TO_FILE

//#endif // _DEBUG

#if defined(_DEBUG) && !defined(__RUBY_DEBUG_DISABLED)

//  #define RUBY_DEBUG_BLOCK(text) \
//      RRubyDebug trace_trace( _S(text), _S("" ## __RUBY_DBG_FILE__) , __LINE__, EFalse ); \
//      CleanupReleasePushL( trace_trace )

//      #define RUBY_DEBUG_BLOCKL(text) \
//      RRubyDebug trace_trace( _S(text), _S("" ## __RUBY_DBG_FILE__), __LINE__, ETrue ); \
//      CleanupReleasePushL( trace_trace )
// A temporary fix to cope with builds on RVCT 530. It didn't compile with _RUBY_DBG_FILE
    #define RUBY_DEBUG_BLOCK(text) \
    RRubyDebug trace_trace( _S(text), _S("") , __LINE__, EFalse, __RUBY_FUNCTION_NAME_POINTER8 ); \
    CleanupReleasePushL( trace_trace )

    #define RUBY_DEBUG_BLOCKL(text) \
    RRubyDebug trace_trace( _S(text), _S(""), __LINE__, ETrue, __RUBY_FUNCTION_NAME_POINTER8 ); \
    CleanupReleasePushL( trace_trace )

    #define RUBY_DEBUG0(text) \
        __RUBY_FUNCTION_NAMED( RUBY_DEBUG_METHOD( _L("%S %S "L##text L## " [F:%s][L:%d][TId:%d]"), &__K_RUBY_HEADER, &__RUBY_FUNCTION_NAME, __RUBY_DBG_FILE__, __LINE__, TUint(RThread().Id()) ) )

    #define RUBY_DEBUG1(text, p1) \
        __RUBY_FUNCTION_NAMED( RUBY_DEBUG_METHOD( _L("%S %S "L##text L## " [F:%s][L:%d][TId:%d]"), &__K_RUBY_HEADER, &__RUBY_FUNCTION_NAME, p1, __RUBY_DBG_FILE__, __LINE__, TUint(RThread().Id()) ) )

    #define RUBY_DEBUG2(text, p1, p2) \
        __RUBY_FUNCTION_NAMED( RUBY_DEBUG_METHOD( _L("%S %S "L##text L## " [F:%s][L:%d][TId:%d]"), &__K_RUBY_HEADER, &__RUBY_FUNCTION_NAME, p1, p2, __RUBY_DBG_FILE__, __LINE__, TUint(RThread().Id()) ) )

#else // Debugging disabled

    // Macros expand to nothing:
    #define RUBY_DEBUG_BLOCK(text)
    #define RUBY_DEBUG0(text)
    #define RUBY_DEBUG1(text, p1)
    #define RUBY_DEBUG2(text, p1, p2)
    #define RUBY_DEBUG_BLOCKL(text)

#endif // defined(_DEBUG) && !defined(__RUBY_DEBUG_DISABLED)

#if !defined(RUBY_DISABLE_ERRORS) || defined(_DEBUG)
    // if error messages are allowed (even for UREL)
    #define RUBY_ERROR0(text) \
    __RUBY_FUNCTION_NAMED( \
        RUBY_DEBUG_METHOD( _L("%S[Error!] %S "L##text L## " [F:%s][L:%d][TId:%d]"), &__K_RUBY_HEADER, &__RUBY_FUNCTION_NAME, __RUBY_DBG_FILE__, __LINE__, TUint(RThread().Id()) )\
    )

    #define RUBY_ERROR1(text, p1) \
    __RUBY_FUNCTION_NAMED( \
    RUBY_DEBUG_METHOD( _L("%S[Error!] %S "L##text L## " [F:%s][L:%d][TId:%d]"), &__K_RUBY_HEADER, &__RUBY_FUNCTION_NAME, p1, __RUBY_DBG_FILE__, __LINE__, TUint(RThread().Id()) )\
    )

    #define RUBY_ERROR2(text, p1, p2) \
    __RUBY_FUNCTION_NAMED( \
    RUBY_DEBUG_METHOD( _L("%S[Error!] %S "L##text L## " [F:%s][L:%d][TId:%d]"), &__K_RUBY_HEADER, &__RUBY_FUNCTION_NAME, p1, p2, __RUBY_DBG_FILE__, __LINE__, TUint(RThread().Id()) )\
    )
#else
    // error reporting disabled
    // Macros expand to nothing:
    #define RUBY_ERROR_BLOCK(text)
    #define RUBY_ERROR0(text)
    #define RUBY_ERROR1(text, p1)
    #define RUBY_ERROR2(text, p1, p2)
#endif  // !defined(RUBY_DISABLE_ERRORS)

#if !defined(RUBY_DISABLE_ASSERT_DEBUG) || defined(_DEBUG)
    #define RUBY_ASSERT_DEBUG(condition, action) \
      __RUBY_FUNCTION_NAMED(\
        __ASSERT_ALWAYS( condition, RUBY_DEBUG_METHOD(  _L("%S[Error!] %S [Assert failed!] "L###condition L## " [F:%s][L:%d][TId:%d]"), &__K_RUBY_HEADER, &__RUBY_FUNCTION_NAME, __RUBY_DBG_FILE__, __LINE__, TUint(RThread().Id())) ); \
        __ASSERT_DEBUG( (condition), (action) )\
      );
#else
    // Macro expands to default:
    #define RUBY_ASSERT_DEBUG(condition, action) __ASSERT_DEBUG(condition,action)
#endif

// Same as TRAP_IGNORE, but in case of leave, prints the leave code via RUBY_ERROR
// Can be disabled by RUBY_DISABLE_TRAP_IGNORE or RUBY_DISABLE_ERRORS
// If disabled, is equivalent to TRAP_IGNORE
// @see rubydebugcfg.h
#ifndef RUBY_DISABLE_TRAP_IGNORE
    #define RUBY_TRAP_IGNORE( s ) \
        {\
        TRAPD( err, s );\
        if( err != KErrNone )\
            {\
            RUBY_ERROR1( "RUBY_TRAP_IGNORE leaves with [%d]", err );\
            }\
        }
#else  // RUBY_DISABLE_TRAP_IGNORE
    #define RUBY_TRAP_IGNORE( s ) TRAP_IGNORE( s )
#endif  // RUBY_DISABLE_TRAP_IGNORE

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Debug class for printing START, EXIT and LEAVE messages to RDebug in every
*  code block. Must be placed in the very beginning of a code block and the
*  code block cannot leave items in the cleanup stack (like NewLC).
*
*  @since Series 60 2.8
*
*  @note If this object is not the topmost item when a code block is exited,
*        E32USER-CBase 90 panic is raised (@see CleanupStack::Pop( TAny* )).
*/
class RRubyDebug
    {
    public:
        /**
        * C++ constructor.
        * @since Series 60 2.8
        * @param aMsg Debug message.
        * @param aCalledFromL ETrue if called from RUBY_DEBUG_BLOCK_L
        */
        inline RRubyDebug( const TText* aMsg, const TText* aFileName, const TInt aLine, TBool aCalledFromL,
                           const TText8* aFunctionName = NULL );

        /**
        * Destructor.
        */
        inline ~RRubyDebug();

        /**
        * Destructor for Cleanup support. Called when a method leaves.
        */
        inline void Release();

        /**
        * Support for writing traces to file.
        */
         static void PrintToFile( TRefByValue<const TDesC> aFmt, ... );

    private:
        /**
        *  Class for truncating debug messages if they are too long.
        *  @since Series 60 2.8
        */
        class TTruncateOverflow : public TDesOverflow
            {
            public: // Methods from TDesOverflow

                inline void Overflow( TDes& aDes );
            };

    private:
        // A pointer to the debug message
        TPtrC iMsg;

        // A pointer to the filename where RUBY_DEBUG_BLOCK is written
        TPtrC iFileName;

        // Number of the line where RUBY_DEBUG_BLOCK is written
        TInt iLine;

        // ETrue if we are called from RUBY_DEBUG_BLOCKL
        TBool iCalledFromL;

        // Flag that is set when a leave occurs
        TBool iLeave;

    #ifndef RUBY_DISABLE_FUNCTION_NAMES
        // Used only if aFunctionName is defined in constructor
        HBufC* iFunctionName;
    #endif  // RUBY_DISABLE_FUNCTION_NAMES

    };

#include "RubyDebug.inl"

#endif      // RUBYDEBUG_H

// End of File
