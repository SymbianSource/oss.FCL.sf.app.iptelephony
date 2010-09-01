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
* Description:   This module contains the implementation of RRubyDebug class 
*	             member functions.
*  %version: 3 %
*
*/



// INCLUDE FILES
#include <e32base.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS
#ifdef _DEBUG
_LIT( KRubyDebugStart,  "START" );
_LIT( KRubyDebugStop,   "EXIT" );
_LIT( KRubyDebugLeave,  "LEAVE!!!" );
_LIT( KRubyDebugStr,    "%S%S %S [F:%S][L:%d][TId:%d]" );
#endif

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

#ifdef _DEBUG
// -----------------------------------------------------------------------------
// RRubyDebug::RRubyDebug
// C++ constructor
// -----------------------------------------------------------------------------
//
    
inline RRubyDebug::RRubyDebug( const TText* aMsg, const TText* aFileName, const TInt aLine, TBool aCalledFromL,
                               const TText8* aFunctionName )
	: iMsg( aMsg ), iFileName( aFileName ), iLine ( aLine ), iCalledFromL ( aCalledFromL ), iLeave ( EFalse )
	{
	#ifndef RUBY_DISABLE_FUNCTION_NAMES
	iFunctionName = NULL;
	// If function name is defined, use it instead of aMsg
	if( aFunctionName )
	    {
	    TPtrC8 ptr8Name( aFunctionName );
	    iFunctionName = HBufC::New( ptr8Name.Length() );
	    if( iFunctionName )
	        {
	        // If memory for a 16-bit copy has been allocated successfully
	        iFunctionName->Des().Copy( ptr8Name );
	        iMsg.Set( *iFunctionName );
	        }
	    }
	#endif // RUBY_DISABLE_FUNCTION_NAMES
	TInt id = RThread().Id();
    iCalledFromL = aCalledFromL;  
    if( !iCalledFromL) 
    	{
    	RUBY_DEBUG_METHOD( KRubyDebugStr, &__K_RUBY_HEADER, &iMsg, &KRubyDebugStart, &iFileName, iLine, id );
    	}
    else {
    	#ifdef RUBY_DIDSPLAY_EVERYTHING_FOR_BLOCKL
    	// For BLOCKLs show this message only if RUBY_DIDSPLAY_EVERYTHING_FOR_BLOCKL
    	RUBY_DEBUG_METHOD( KRubyDebugStr, &__K_RUBY_HEADER, &iMsg, &KRubyDebugStart, &iFileName, iLine, id );
    	#endif
    	}
	}    

// -----------------------------------------------------------------------------
// RRubyDebug::~RRubyDebug
// Destructor
// -----------------------------------------------------------------------------
//
inline RRubyDebug::~RRubyDebug()
    {
    if ( !iLeave ) 
        {
        // This is done only if destructor is NOT called from Release().
        // Otherwise the cleanup stack pops this item automatically.
        TInt id = RThread().Id();
        if (!iCalledFromL) 
        	{
        	RUBY_DEBUG_METHOD( KRubyDebugStr, &__K_RUBY_HEADER, &iMsg, &KRubyDebugStop, &iFileName, iLine, id );
        	}
        else
        	{
        	#ifdef RUBY_DIDSPLAY_EVERYTHING_FOR_BLOCKL
    		// For BLOCKLs show this message only if RUBY_DIDSPLAY_EVERYTHING_FOR_BLOCKL
    		RUBY_DEBUG_METHOD( KRubyDebugStr, &KRubyDebugPrefix, &iMsg, &KRubyDebugStop, &iFileName, iLine, id );
    		#endif
        	}
        #ifndef RUBY_DISABLE_FUNCTION_NAMES
        delete iFunctionName;  // if any
        #endif  // RUBY_DISABLE_FUNCTION_NAMES
        CleanupStack::Pop( this ); // E32USER-CBase 90 panic is raised if
                                   // this is not the topmost item
        }
    }

// -----------------------------------------------------------------------------
// RRubyDebug::Release()
// Method that makes it possible to push this object into the cleanup stack.
// -----------------------------------------------------------------------------
//
inline void RRubyDebug::Release()
    {
    iLeave = ETrue;
    TInt id = RThread().Id();
    RUBY_DEBUG_METHOD( KRubyDebugStr, &__K_RUBY_HEADER, &iMsg, &KRubyDebugLeave, &iFileName, iLine, id );
    #ifndef RUBY_DISABLE_FUNCTION_NAMES
    delete iFunctionName;  // if any
    #endif  // RUBY_DISABLE_FUNCTION_NAMES
    this->~RRubyDebug();
    }
#endif

// -----------------------------------------------------------------------------
// RRubyDebug::TTruncateOverflow::Overflow()
// Method for ignoring the overflowing part of a debug message.
// -----------------------------------------------------------------------------
//
inline void RRubyDebug::TTruncateOverflow::Overflow( TDes& /*aDes*/ )
    {
    // We ignore the overflowing part of the descriptor...
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
