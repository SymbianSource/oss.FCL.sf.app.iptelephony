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
* Description:   Implementation of Miscallaneous service functions
*
*/



#include <e32std.h>
#include <uri8.h>
#include <uriutils.h>
#include <authority8.h>
#include <delimitedpath8.h>
//#include <e32const.h>

#include "vccmiscutils.h"
#include "rubydebug.h"

_LIT( KVCCSIPScheme, "sip" );
_LIT( KVCCSIPSScheme, "sips" );
_LIT( KVCCHTTPScheme, "http" );
_LIT( KVCCHTTPSScheme, "https" );

const TUint KAt = '@';

const TInt KVCCMinVDNLength = 5; 

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Checks if 16 bit reference is valid URI (VDI).
// ---------------------------------------------------------------------------
//
EXPORT_C TInt TVCCMiscUtils::URIValidL( const TDesC16& aURI )
    {
    RUBY_DEBUG_BLOCK( "TVCCMiscUtils::URIValidL()" );
    
    TUriParser parser;
    // Check if uri contains multiple @ characters
    TUint atcount( 0 );
    for( TUint i(0); i<aURI.Length(); i++ )
        {
        if(  KAt == aURI[i] )
             {
             ++atcount;
             }
        }
    
    if( atcount>1 )
       {
       return KErrUnknown;
       }
      
    
    // Parse the URI
    TInt err = parser.Parse( aURI );
    
    if ( !err )
        {
        // Check for invalid characters
        // 0x00-0x1F and > 0x7F), space (0x20), delimiter characters ('<', '>', 
        // '#', '%', '"') and unwise characters ('{', '}', '|', '\', '^', '[', 
        // ']', '`') are illegal
        if ( !UriUtils::HasInvalidChars( aURI ) )
            {
            // Check if scheme is valid
            if ( parser.IsSchemeValid() )
                {
                // Extract the scheme for comparing
                const TDesC& scheme = parser.Extract( EUriScheme );
                
                // SIP or SIPS scheme
                if ( scheme.Compare( KVCCSIPScheme ) == 0 || 
                     scheme.Compare( KVCCSIPSScheme ) == 0 )
                    {
                    err = parser.Validate();
                    }
                // HTTP or HTTPS scheme
                else if ( scheme.Compare( KVCCHTTPScheme ) == 0 || 
                          scheme.Compare( KVCCHTTPSScheme ) == 0 )
                    {
                    // Do nothing at the moment.
                    }
                else
                    {
                    err = KErrUnknown;
                    }
                }
            // No scheme, maybe a host type
            else
                {
                UriUtils::TUriHostType type = UriUtils::HostType( aURI );
                
                switch ( type )
                    {
                    case UriUtils::EIPv6Host:
                    case UriUtils::EIPv4Host:
                    case UriUtils::ETextHost:
                        break;
                    default:
                        {
                        err = KErrUnknown;
                        break;
                        }
                    } // switch
                } // else
            } // if invalid characters
        else
            {
            err = KErrNotSupported;
            }
        }
    
    return err;
    }

// ---------------------------------------------------------------------------
// Checks if reference is valid VDN.
// ---------------------------------------------------------------------------
//
EXPORT_C TBool TVCCMiscUtils::VDNValidL(  const TDesC16& aVDN )
    {
    RUBY_DEBUG_BLOCK( "TVCCMiscUtils::VDNValidL()" );
    
    TBool retVal( ETrue );
    TLex input( aVDN );
    TInt numCount( 0 );
    
    // Is the string empty
    if ( aVDN.Length() > 0 )
        {
        input.Mark();
        
        // Deal with the '+' -sign at the beginning of the string
        if (input.Peek() == '+')
            {
            // Step forward
            input.Inc();
            }
      
        for ( ;; numCount++ )
            {
            if ( !( input.Peek().IsDigit() ) && !( input.Peek().Eos() ) )
                {
                // Next character is not a digit or end-of-string character
                retVal = EFalse;
                break;
                }
            else if ( input.Peek().Eos() && numCount < KVCCMinVDNLength )
                {
                // End of string but number count is too small to be a valid VDN
                retVal = EFalse;
                break;
                }
            else if ( input.Peek().Eos() && numCount >= KVCCMinVDNLength )
                {
                // At the end of string and number count is acceptable to be a
                // valid VDN
                break;
                }
            else
                {
                // step forward
                input.Inc();                
                }
            } // for
        }
    else
        {
        // Empty string
        retVal = EFalse;
        }
    
    return retVal;
    }

