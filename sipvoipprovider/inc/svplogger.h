/*
* Copyright (c) 2006 - 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides macros for logging and debugging.
*
*/

#ifndef SVPLOGGER_H
#define SVPLOGGER_H

#ifdef _DEBUG  // enable debug macros

#ifndef COMPONENT
#define COMPONENT
_LIT( KComponent, "SVP" );
#endif // COMPONENT

#include <e32svr.h>
#include <e32cmn.h>

/*****************************************************
* Format string for adding component to prints.
* 
* Parameters passed within macros from code are
* formatted with string that came as a parameter
*****************************************************/
_LIT( KSVPPrintFormatString, "%S: %S" );

/*
 *  Prints for text without params
 */
static inline void PrintL( const TDesC& aStr )
    {
    HBufC* line = HBufC::NewLC( 256 );
    TPtr ptr( line->Des() );
    ptr.Format( KSVPPrintFormatString(), &KComponent(), &aStr );   
    RDebug::Print( line->Des() );
    CleanupStack::PopAndDestroy( line );
    }
static inline void Print( const TDesC& aStr )
    {
    TRAP_IGNORE( PrintL( aStr ) )
    }

/* 
 * Prints for text with TInt as a param
 */
static inline void PrintL( const TDesC& aStr, const TInt& aTInt )
    {
    HBufC* line1 = HBufC::NewLC( 256 );
    TPtr ptr1( line1->Des() );
    ptr1.Format( aStr, aTInt );
    
    HBufC* line2 = HBufC::NewLC( 256 );
    TPtr ptr2( line2->Des() );
    ptr2.Format( KSVPPrintFormatString(), &KComponent(), line1 );   
    RDebug::Print( line2->Des() );
    CleanupStack::PopAndDestroy( 2 );
    }
static inline void Print( const TDesC& aStr, const TInt& aTInt )
    {
    TRAP_IGNORE( PrintL( aStr, aTInt ) )
    }

/*
 * Prints for text with any pointer as a param
 */
static inline void PrintL( const TDesC& aStr, const TAny* aPtr )
    {
    HBufC* line1 = HBufC::NewLC( 256 );
    TPtr ptr1( line1->Des() );
    ptr1.Format( aStr, aPtr );
    
    HBufC* line2 = HBufC::NewLC( 256 );
    TPtr ptr2( line2->Des() );
    ptr2.Format( KSVPPrintFormatString(), &KComponent(), line1 ); 
    RDebug::Print( line2->Des() );
    CleanupStack::PopAndDestroy( 2 );
    }
static inline void Print( const TDesC& aStr, const TAny* aPtr )
    {
    TRAP_IGNORE( PrintL( aStr, aPtr ) )
    }

/* 
 * Prints for text with two TInt as a param
 */
static inline void PrintL( const TDesC& aStr,
                           const TInt& aTInt1,
                           const TInt& aTInt2 )
    {
    HBufC* line1 = HBufC::NewLC( 256 );
    TPtr ptr1( line1->Des() );
    ptr1.Format( aStr, aTInt1, aTInt2 );
        
    HBufC* line2 = HBufC::NewLC( 256 );
    TPtr ptr2( line2->Des() );
    ptr2.Format( KSVPPrintFormatString(), &KComponent(), line1 );   
    RDebug::Print( line2->Des() );
    CleanupStack::PopAndDestroy( 2 );
    }
static inline void Print( const TDesC& aStr,
                          const TInt& aTInt1,
                          const TInt& aTInt2 )
    {
    TRAP_IGNORE( PrintL( aStr, aTInt1, aTInt2 ) )
    }

/*
 * Prints for text with TInt and a string as a parameter
 */
static inline void PrintL( const TDesC& aStr1,
                           const TInt& aTInt,
                           const TDesC* aStr2 )
    {
    HBufC* line1 = HBufC::NewLC( 256 );
    TPtr ptr1( line1->Des() );
    ptr1.Format( aStr1, aTInt, aStr2 );
        
    HBufC* line2 = HBufC::NewLC( 256 );
    TPtr ptr2( line2->Des() );
    ptr2.Format( KSVPPrintFormatString(), &KComponent(), line1 );   
    RDebug::Print( line2->Des() );
    CleanupStack::PopAndDestroy( 2 );
    }
static inline void Print( const TDesC& aStr1,
                          const TInt& aTInt,
                          const TDesC* aStr2 )
    {
    TRAP_IGNORE( PrintL( aStr1, aTInt, aStr2 ) )
    }


#define SVPDEBUG1( a ) { _LIT( KStr, a ); Print( KStr() ); }
#define SVPDEBUG2( a, b ){ _LIT( KStr, a ); Print( KStr(), b ); }
#define SVPDEBUG3( a, b, c ){ _LIT( KStr, a ); Print( KStr(), b, c ); }

#else // _DEBUG

#define SVPDEBUG1( a )
#define SVPDEBUG2( a, b )
#define SVPDEBUG3( a, b, c )

#endif // _DEBUG

#endif // SVPLOGGER_H
