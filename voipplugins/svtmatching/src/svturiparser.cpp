/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Settings handler class for svtmatching.
*
*/


#include "svturiparser.h"

// Valid characters for GSM call
_LIT( KPhoneValidChars, "0123456789*#+pwPW" );
// For parsing protocol prefix and domain part out of a VoIP call URI
_LIT( KSvtColon, ":" );
_LIT( KSvtAt, "@" );
// Indicates the start of sip uri.
const TUint KStartBracket = '<'; 
// Indicates the end of sip uri.
const TUint KEndBracket = '>'; 
const TUint KSpaceMark = ' ';
const TUint KQuotationMark = '"';


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CSvtUriParser::CSvtUriParser()
    {
    }

  
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CSvtUriParser* CSvtUriParser::NewL()
    {
    CSvtUriParser* self = new ( ELeave ) CSvtUriParser;
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CSvtUriParser::~CSvtUriParser()
    {

    }

// ---------------------------------------------------------------------------
// Parses sip uri by ignore domain part setting
// ---------------------------------------------------------------------------
//
void CSvtUriParser::ParseAddressL( 
        TInt aIgnoreDomain, 
        const TDesC& aOriginal, 
        RBuf& aParsedAddress ) const
    {
    switch ( aIgnoreDomain )
        {
        case 1:
        case 2:
            {
            HandleUserNamePartL( aIgnoreDomain, aOriginal, aParsedAddress );
            break;
            }
        case 0:
        default:
            {
            aParsedAddress.Close();
            aParsedAddress.CreateL( aOriginal );
            break;
            }
        
        }
    }

// ---------------------------------------------------------------------------
// Resolves display name from sip uri
// ---------------------------------------------------------------------------
//
TInt CSvtUriParser::DisplayNameFromUri( 
        const TDesC& aData, 
        RBuf& aDisplayname ) const
    {
    TInt ret( KErrNotFound );
    
    TPtrC resultStr( aData );
    aDisplayname.Close();
    
    // resolves potential SIP Display info and removes it from uri
    // also possible "<" and ">" character are removed around the SIP uri
    TInt uriStartIndex = resultStr.LocateReverse( KStartBracket );      
    TInt uriEndIndex = resultStr.LocateReverse( KEndBracket );       
    
    if ( uriStartIndex > uriEndIndex )
        {
        // Start and end separators in wrong order: "xxxx>xxxx<xxx"
        return KErrArgument;
        }
        
    if ( KErrNotFound != uriStartIndex && KErrNotFound != uriEndIndex )
        {
        // brackets found so modify descriptor and save the display info
        
        // check if there is anything before "<" if there is use
        // it as displayname if match op fails.
        if ( uriStartIndex > 1 )
            {        
            TPtrC tempStr( resultStr.Left( uriStartIndex ) );          
            // remove possible quotation marks from displayname
            TInt index = tempStr.Locate( KQuotationMark );
            if ( KErrNotFound != index )
                {
                // marks have to be removed
                tempStr.Set( tempStr.Mid( ++index ) );    
                if ( tempStr[tempStr.Length() - 1] == KQuotationMark )
                    {
                    tempStr.Set( tempStr.Left( tempStr.Length() - 1 ) );
                    }
                }
            ret = aDisplayname.Create( tempStr );
            }                      
        }        
    else 
        {
        // it is also possible that displayname is included 
        // in, in case that there is no brackets around the uri. So if there is something
        // inside quotationMarks it should be used as displayname

        // check if displayname is found
        TInt displayNameStart = resultStr.Locate( KQuotationMark );
        TInt displayNameEnd = resultStr.LocateReverse( KQuotationMark );

        if ( displayNameStart != KErrNotFound 
            && displayNameEnd != KErrNotFound 
            && displayNameStart < displayNameEnd )
            {
            // displayname is included
            // ++, to remove quotationMark from the start
            ret = aDisplayname.Create( resultStr.Mid( ++displayNameStart, 
                // -1, to remove quotationMark from the end
                displayNameEnd - displayNameStart - 1 ) );  
            }                                
        else
            {
            // check if there is spaces in the uri, if there is
            // everything before it belongs to display name                
            TInt index = resultStr.LocateReverse( KSpaceMark );
            
            if ( KErrNotFound != index )            
                {
                // set displayname
                ret = aDisplayname.Create( resultStr.Left( index ) );
                }            

            }
        }
    
    return ret;
    }

// ---------------------------------------------------------------------------
// Check original address for spaces. 
// ---------------------------------------------------------------------------
//
void CSvtUriParser::CheckForSpacesL(
    const TDesC& aOriginal, 
    RBuf& aCheckedAddress ) const
    {
    aCheckedAddress.CreateL( aOriginal );
    
    TInt index( 0 );
    while ( index != KErrNotFound )
        {
        index = aCheckedAddress.Locate( KSpaceMark );
        
        // Remove space if it´s in begin or end of string
        if ( index == 0 || ( index == ( aCheckedAddress.Length() - 1 ) ) )
            {
            aCheckedAddress.Delete( index, 1 );
            }
        else
            {
            index = KErrNotFound;
            }
        }
    }

// ---------------------------------------------------------------------------
// Check is string valid for gsm call
// ---------------------------------------------------------------------------
//
TBool CSvtUriParser::IsValidGsmNumber( const TDesC& aOriginal ) const
    {
    TLex input( aOriginal );
    TPtrC valid( KPhoneValidChars );

    while ( valid.Locate( input.Peek() ) != KErrNotFound )
        {
        input.Inc();
        }
  
    return !input.Remainder().Length();
    }

// ---------------------------------------------------------------------------
// Gets username part from sip uri
// ---------------------------------------------------------------------------
//
TInt CSvtUriParser::GetUserNamePart( 
        const TDesC& aOriginal,
        TDes& aUserName ) const
    {
    if ( aOriginal.Length() > aUserName.MaxLength() )
        {
        return KErrArgument;
        }
    
    aUserName.Copy( aOriginal );
    
    // Parse protocol prefix and domain part out of a VoIP call URI
    TInt pos( aUserName.Find( KSvtColon ) );
    if ( pos > KErrNotFound )
        {
        aUserName.Delete( 0, pos+1 );
        }                
        
    pos = aUserName.Find( KSvtAt );
    if ( pos > KErrNotFound )
        {
        aUserName.Delete( pos, aUserName.Length() - pos );
        }
    
    if ( 0 == aUserName.Length() )
        {
        return KErrNotFound;
        }
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Handles sip uri's username part for ignore domain part values 1 and 2
// ---------------------------------------------------------------------------
//
void CSvtUriParser::HandleUserNamePartL( 
        TInt aIgnoreDomain, 
        const TDesC& aOriginal, 
        RBuf& aParsedAddress ) const
    {
    if ( 0 == aOriginal.Length() )
        {
        User::Leave( KErrArgument );
        }
    
    HBufC* buf = HBufC::NewLC( aOriginal.Length() );
    TPtr des( buf->Des() );
    
    User::LeaveIfError( GetUserNamePart( aOriginal, des ) );
    
    aParsedAddress.Close();
    
    if ( ( 1 == aIgnoreDomain && IsValidGsmNumber( des ) ) ||
           2 == aIgnoreDomain )
        {
        // Set parsed address
        User::LeaveIfError( aParsedAddress.Create( des ) );
        }
    else
        {
        User::LeaveIfError( aParsedAddress.Create( aOriginal ) );
        }
        
    CleanupStack::PopAndDestroy( buf );
    }


