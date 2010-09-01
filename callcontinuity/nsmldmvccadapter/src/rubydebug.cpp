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
* Description:   Implementation of nsmldmvccadapter
*
*/



#include "rubydebug.h"

#ifdef __RUBY_DEBUG_TRACES_TO_FILE
#include <f32file.h>
#endif

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS
#ifdef __RUBY_DEBUG_TRACES_TO_FILE
#ifdef _DEBUG // UDEB version:
_LIT( KSpace, " " );
_LIT( KColon, ":" );
_LIT( KFormat, "%02d" );
_LIT( KFormat2, "%06d" );
#endif
#endif


// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RRubyDebug::PrintToFile
// Method for writing traces to a file.
// -----------------------------------------------------------------------------
//
#ifdef __RUBY_DEBUG_TRACES_TO_FILE

#ifdef _DEBUG // UDEB version:
void RRubyDebug::PrintToFile( TRefByValue<const TDesC> aFmt, ... )
    {
    _LIT( KRubyLogFileName, "c:\\Data\\Logs\\RubyTrace_vccdmplugin.txt" );

    const TInt KRubyDebugMaxLineLength = 0x84; // rest will be truncated

    const TInt KRubyDebugOpenFileRetries = 100;
    const TInt KRubyDebugOpenFileInterval = 1000;

    const TUint16 KRubyDebugLineSep1 = 0x0d;
    const TUint16 KRubyDebugLineSep2 = 0x0a;

    // Handle variable argument list
    VA_LIST list;
  VA_START( list, aFmt );
  TBuf<KRubyDebugMaxLineLength+2> aBuf;
    TTruncateOverflow overflow;
    
    // Append the time...
    TTime currentTime;
    currentTime.HomeTime();
    TDateTime dateTime = currentTime.DateTime();

    aBuf.AppendFormat( KFormat, dateTime.Hour() );
    aBuf.Append( KColon );

    aBuf.AppendFormat( KFormat, dateTime.Minute() );
    aBuf.Append( KColon );

    aBuf.AppendFormat( KFormat, dateTime.Second() );
    aBuf.Append( KColon );

    aBuf.AppendFormat( KFormat2, dateTime.MicroSecond() );
    aBuf.Append( KSpace );
    // time done
    
  aBuf.AppendFormatList( aFmt, list, &overflow );
  if( aBuf.Length() > ( KRubyDebugMaxLineLength - 2 ) )
    {
    aBuf.Delete(aBuf.Length() - 2, 2);
    }
  
    // Add linefeed characters
    aBuf.Append( KRubyDebugLineSep1 );
    aBuf.Append( KRubyDebugLineSep2 );

    RFs fs;
    if ( fs.Connect() == KErrNone )
        {
        RFile file;

        // Open file in an exclusive mode so that only one thread 
        // can acess it simultaneously
        TUint fileMode = EFileWrite | EFileShareExclusive;

        TInt err = file.Open( fs, KRubyLogFileName, fileMode );

        // Create a file if it doesn't exist
        if ( err == KErrNotFound )
            {
            err = file.Create( fs, KRubyLogFileName, fileMode );
            }
        else
            {
            // Error in opening the file
            TInt retryCount = KRubyDebugOpenFileRetries;
            while ( err == KErrInUse && retryCount-- )
                {
                // Some other tread is accessing the file, wait a while...
                User::After( KRubyDebugOpenFileInterval );
                err = file.Open( fs, KRubyLogFileName, fileMode );
                }
            }

        // Check if we have access to a file
        if ( err == KErrNone )
            {
            TInt offset = 0;
            if ( file.Seek( ESeekEnd, offset ) == KErrNone )
                {
                // Append text to the end of file
                TPtr8 ptr8( (TUint8*)aBuf.Ptr(), aBuf.Size(), aBuf.Size() );
                file.Write( ptr8 );
                }
            file.Close();
            }

        fs.Close();
        }
    }

#else // UREL version:
void RRubyDebug::PrintToFile( TRefByValue<const TDesC> /*aFmt*/, ... )
    {
    }
#endif

#endif // __RUBY_DEBUG_TRACES_TO_FILE

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
