/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  provides uri parser methods for SVP 
*
*/

#include <uri8.h>
#include <sipstrings.h>    // For remote party data parsing
#include <sipfromheader.h> // For remote party data parsing
#include <sipaddress.h>    // For remote party data parsing

#include "svplogger.h"
#include "svpconsts.h"
#include "svpuriparser.h"
#include "svpcleanupresetanddestroy.h"

// ---------------------------------------------------------------------------
// CSVPUriParser::CSVPUriParser
// ---------------------------------------------------------------------------
//
CSVPUriParser::CSVPUriParser()
    {
    }

// ---------------------------------------------------------------------------
// CSVPUriParser::NewLC
// ---------------------------------------------------------------------------
//
CSVPUriParser* CSVPUriParser::NewLC()
    {
    CSVPUriParser* self = new ( ELeave ) CSVPUriParser;
    CleanupStack::PushL( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CSVPUriParser::NewL
// ---------------------------------------------------------------------------
//
CSVPUriParser* CSVPUriParser::NewL()
    {
    CSVPUriParser* self = CSVPUriParser::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CSVPUriParser::~CSVPUriParser
// ---------------------------------------------------------------------------
//
CSVPUriParser::~CSVPUriParser()
    {
    }

// ---------------------------------------------------------------------------
// CSVPUriParser::SetUserEqualsPhoneRequiredL
// ---------------------------------------------------------------------------
//
void CSVPUriParser::SetUserEqualsPhoneRequiredL( TBool aValue )
    {
    SVPDEBUG1( "CSVPUriParser::SetUserEqualsPhoneRequiredL" )
    iUserEqualsPhoneRequired = aValue;
    }

// ---------------------------------------------------------------------------
// CSVPUriParser::UserEqualsPhoneRequiredL
// ---------------------------------------------------------------------------
//
TBool CSVPUriParser::UserEqualsPhoneRequiredL() const
    {
    SVPDEBUG1( "CSVPUriParser::UserEqualsPhoneRequired In" )
    // check if user=phone is required
    SVPDEBUG2( "CSVPUriParser::UserEqualsPhoneRequired Out: %d",
           iUserEqualsPhoneRequired )
    return iUserEqualsPhoneRequired;    
    }

// ---------------------------------------------------------------------------
// CSVPUriParser::IsUriValidForUserEqualsPhoneL
// ---------------------------------------------------------------------------
//
TBool CSVPUriParser::IsUriValidForUserEqualsPhoneL( const TDesC8& aUri )
    {
    SVPDEBUG1( "CSVPUriParser::IsUriValidForUserEqualsPhoneL In" )
    
    /// flag to indicate if all checks are ok
    TBool userEqPhoneCheck = EFalse;    
    
    // Extract user info
    TUriParser8 uri8Parser;
    uri8Parser.Parse( aUri );
    CUri8* cUri8 = CUri8::NewLC( uri8Parser ); // CS:1
    
    const TUriC8& tempUri =  cUri8->Uri();
    const TDesC8& userInfo = tempUri.Extract( EUriUserinfo );
    
    TLex8 user( userInfo );
    TInt length = userInfo.Length();
    const TChar KSvpPlus( '+' );
    
    // Check if there is a preceeding plus
    if ( KSvpPlus == user.Peek() )
        {
        user.Inc();
        length--;
        }
    
    // Check the minimum length
    if ( KSVPMinUserInfoLength > length )
        {
        userEqPhoneCheck = EFalse;
        }
    else // Check that there is only digits
        {
        userEqPhoneCheck = ETrue;
        
        while ( !user.Eos() )
            {
            if ( !user.Peek().IsDigit() )
                {
                userEqPhoneCheck = EFalse;
                break;
                }
            
            user.Inc();
            }
        }
    
    CleanupStack::PopAndDestroy( cUri8 );
    
    SVPDEBUG2( "CSVPUriParser::IsUriValidForUserEqualsPhoneL Out: %d",
           userEqPhoneCheck )
    return userEqPhoneCheck;
    }

// ---------------------------------------------------------------------------
// CSVPUriParser::CompleteSipUriL
// ---------------------------------------------------------------------------
//
HBufC8* CSVPUriParser::CompleteSipUriL( 
    const TDesC8& aUri, const TDesC8& aAOR, TBool aIsEmergency ) const
    {
    SVPDEBUG1( "CSVPUriParser::CompleteSipUriL In" )
    
    // Copy the parameter to a new buffer.
    HBufC8* uri = aUri.AllocLC();   // CS: 1
    
#ifdef _DEBUG
    TBuf<KSvpMaxDebugBufferSize> tmpStr;
    tmpStr.Copy( aAOR );
    SVPDEBUG2( "CSVPUriParser::CompleteSipUriL IN aAOR: %S", &tmpStr )
    tmpStr.Copy( aUri );
    SVPDEBUG2( "CSVPUriParser::CompleteSipUriL IN aURI: %S", &tmpStr )
#endif // _DEBUG
    
    // Trim ALL white space from URI, even if used as an user name
    uri->Des().TrimAll();

    if ( uri )
        {
        //First check is this tel-uri. Domain part is not allowed in tel-uri.
        if ( uri->Length() >= KTelPrefixLength &&
             KErrNotFound != uri->Des().Left( KTelPrefixLength ).Find( KTelPrefix ) &&
             KErrNotFound == uri->Des().Find( KSVPAt ) )
            {
            CleanupStack::Pop( uri );  // CS:0
            return uri;
            }
        
        // parse display name if exists
        while ( KErrNotFound != uri->Find( KSVPSipPrefix ) )
            {
            TInt index = uri->Find( KSVPSipPrefix );
            
            if ( 0 == index )
                {
                uri->Des().Delete( index, 4 );
                }
            else
                {
                uri->Des().Delete( 0, index );
                }
            }
        
        // Check "<" and remove it if exists
        if ( uri->Length() && CheckLeftBracket( *uri ) )
            {
            RemoveLeftBracket( uri );
            CleanupStack::Pop( 1 ); // uri, ReAlloc possible
            CleanupStack::PushL( uri );
            }
        
        // Check ">" and remove it if exists
        if ( uri->Length() && CheckRightBracket( *uri ) )
            {
            RemoveRightBracket( uri );
            CleanupStack::Pop( 1 ); // uri, ReAlloc possible
            CleanupStack::PushL( uri );
            }
        
        // Check "sips:" and remove it if exists
        if ( uri->Length() && CheckSipsPrefix( *uri ) )
            {
            RemoveSipsPrefixL( uri );
            CleanupStack::Pop( 1 ); // uri, ReAlloc possible
            CleanupStack::PushL( uri );
            }
        
        // Add "sip:" prefix, if it's missing.
        if ( !CheckSipPrefix( *uri ) )
            {
            AddSipPrefixL( uri );
            CleanupStack::Pop( 1 ); // uri, ReAlloc possible
            CleanupStack::PushL( uri );
            }
        
        // Add "@" character if it's missing
        if ( !CheckAt( *uri ) )
            {
            AddAtL( uri );
            CleanupStack::Pop( 1 ); // uri, ReAlloc possible
            CleanupStack::PushL( uri );
            }
        
        // Add domain, if it's missing
        if ( !CheckDomain( *uri ) )
            {
            AddDomainL( uri, aAOR );
            CleanupStack::Pop( 1 ); // uri, ReAlloc possible
            CleanupStack::PushL( uri );
            }
        
        EscapeEncodeSipUriL( uri, EscapeUtils::EEscapeNormal );
        CleanupStack::Pop( 1 ); // uri, ReAlloc possible
        CleanupStack::PushL( uri );
        
        // Emergency call has no knowledge of the service ID, therefore
        // user=phone parameter is omitted
        if ( !aIsEmergency &&
             IsUriValidForUserEqualsPhoneL( *uri ) && 
             UserEqualsPhoneRequiredL() )
            {
            AddUserEqualsPhoneL( uri );
            }
        }
    else
        {
        SVPDEBUG1( "    CSVPUriParser::CompleteSipUriL uri null" )
        }
    
#ifdef _DEBUG
    tmpStr.Copy( *uri );
    SVPDEBUG2( "CSVPUriParser::CompleteSipUriL OUT: %S", &tmpStr )
#endif // _DEBUG
    
    CleanupStack::Pop( 1 ); // uri
    return uri;
    }

// ---------------------------------------------------------------------------
// CSVPUriParser::AddUserEqualsPhone
// ---------------------------------------------------------------------------
//
void CSVPUriParser::AddUserEqualsPhoneL( HBufC8*& aUri ) const
    {
    SVPDEBUG1( "CSVPUriParser::AddUserEqualsPhoneL In" )
   
    aUri = aUri->ReAllocL( aUri->Length() + 
                          KSVPUserEqualsPhoneLength + 
                          KSVPDoubleBracketLength ); 
    aUri->Des().Insert( 0, KSVPLeftBracketMark );
    aUri->Des().Append( KSVPUserEqualsPhone );
    aUri->Des().Append( KSVPRightBracketMark );
    
    SVPDEBUG1( "CSVPUriParser::AddUserEqualsPhoneL Out" )
    }

// ---------------------------------------------------------------------------
// CSVPUriParser::CompleteSecureSipUriL
// ---------------------------------------------------------------------------
//
HBufC8* CSVPUriParser::CompleteSecureSipUriL(
    const TDesC8& aUri, const TDesC8& aAOR ) const
    {
    SVPDEBUG1( "CSVPUriParser::CompleteSecureSipUriL In" )
    
    // Copy the parameter to a new buffer.
    HBufC8* uri = aUri.AllocLC();

#ifdef _DEBUG
    TBuf<KSvpMaxDebugBufferSize> tmpStr;
    tmpStr.Copy( aAOR );
    SVPDEBUG2( "CSVPUriParser::CompleteSecureSipUriL IN aAOR: %S", &tmpStr )
    tmpStr.Copy( aUri );
    SVPDEBUG2( "CSVPUriParser::CompleteSecureSipUriL IN aURI: %S", &tmpStr )
#endif // _DEBUG
    
    // Trim ALL white space from URI, even if used as an user name 
    uri->Des().TrimAll();

    if ( uri )
        {
        //First check is this tel-uri. Domain part is not allowed in tel-uri.
        if ( uri->Length() >= KTelPrefixLength &&
             KErrNotFound != uri->Des().Left( KTelPrefixLength ).Find( KTelPrefix ) &&
             KErrNotFound == uri->Des().Find( KSVPAt ) )
            {
            CleanupStack::Pop( uri );
            return uri;
            }
        
        // Check "<" and remove it if exists
        if ( uri->Length() && CheckLeftBracket( *uri ) )
            {
            RemoveLeftBracket( uri );
            CleanupStack::Pop( 1 ); // uri, ReAlloc possible
            CleanupStack::PushL( uri );
            }
        
        // Check ">" and remove it if exists
        if ( uri->Length() && CheckRightBracket( *uri ) )
            {
            RemoveRightBracket( uri );
            CleanupStack::Pop( 1 ); // uri, ReAlloc possible
            CleanupStack::PushL( uri );
            }
        
        // Check "sip:" and remove it if exists
        if ( uri->Length() && CheckSipPrefix( *uri ) )
            {
            RemoveSipPrefixL( uri );
            CleanupStack::Pop( 1 ); // uri, ReAlloc possible
            CleanupStack::PushL( uri );
            }
        
        // Add "sips:" prefix, if it's missing.
        if ( !CheckSipsPrefix( *uri ) )
            {
            AddSipsPrefixL( uri );
            CleanupStack::Pop( 1 ); // uri, ReAlloc possible
            CleanupStack::PushL( uri );
            }
        
        // Add "@" character if it's missing
        if ( !CheckAt( *uri ) )
            {
            AddAtL( uri );
            CleanupStack::Pop( 1 ); // uri, ReAlloc possible
            CleanupStack::PushL( uri );
            }
        
        // Add domain, if it's missing
        if ( !CheckDomain( *uri ) )
            {
            AddDomainL( uri, aAOR );
            CleanupStack::Pop( 1 ); // uri, ReAlloc possible
            CleanupStack::PushL( uri );
            }
        
        EscapeEncodeSipUriL( uri, EscapeUtils::EEscapeNormal );
        CleanupStack::Pop( 1 ); // uri, ReAlloc possible
        CleanupStack::PushL( uri );
        
        if ( UserEqualsPhoneRequiredL() &&
             IsUriValidForUserEqualsPhoneL( *uri ) )
            {
            AddUserEqualsPhoneL( uri );
            }
        }
    else
        {
        SVPDEBUG1( "    CSVPUriParser::CompleteSecureSipUriL uri null" )
        }
    
#ifdef _DEBUG
    tmpStr.Copy( *uri );
    SVPDEBUG2( "CSVPUriParser::CompleteSecureSipUriL OUT: %S", &tmpStr )
#endif // _DEBUG
    
    CleanupStack::Pop( 1 ); // uri
    return uri;
    }

// ---------------------------------------------------------------------------
// CSVPUriParser::CompleteEventSipUriL
// ---------------------------------------------------------------------------
//
HBufC8* CSVPUriParser::CompleteEventSipUriL( const TDesC8& aUri ) const
    {
    SVPDEBUG1( "CSVPUriParser::CompleteEventSipUriL In" )
    
    // Copy the parameter to a new buffer.
    HBufC8* uri = aUri.AllocLC();
    
    // Add "sip:" prefix, if it's missing.
    if ( !CheckSipPrefix( *uri ) )
        {
        AddSipPrefixL( uri );
        }
    
    CleanupStack::Pop( 1 ); // uri
    
    SVPDEBUG1( "CSVPUriParser::CompleteEventSipUriL Out" )
    return uri;
    }

// ---------------------------------------------------------------------------
// CSVPUriParser::CheckSipPrefix
// ---------------------------------------------------------------------------
//
TBool CSVPUriParser::CheckSipPrefix( const TDesC8& aUri ) const
    {
    SVPDEBUG1( "CSVPUriParser::CheckSipPrefix" )
    
    // The "sip:" prefix is expected to be at the beginning of the URI.
    if ( KErrNotFound == aUri.Find( KSVPSipPrefix ) )
        {
        return EFalse;
        }
    else
        {
        return ETrue;    
        }
    }

// ---------------------------------------------------------------------------
// CSVPUriParser::CheckSipsPrefix
// ---------------------------------------------------------------------------
//
TBool CSVPUriParser::CheckSipsPrefix( const TDesC8& aUri ) const
    {
    SVPDEBUG1( "CSVPUriParser::CheckSipsPrefix" )
    
    // The "sips:" prefix is expected to be at the beginning of the URI.
    return ( 0 == aUri.Find( KSVPSipsPrefix ) );
    }

// ---------------------------------------------------------------------------
// CSVPUriParser::CheckAt
// ---------------------------------------------------------------------------
//
TBool CSVPUriParser::CheckAt( const TDesC8& aUri ) const
    {
    SVPDEBUG1( "CSVPUriParser::CheckAt" )
    
    // It is expected that there is atleast one character between 
    // "sip:" prefix and "@" character.
    return ( aUri.Find( KSVPAt ) >= KSVPSipPrefixLength );
    }

// ---------------------------------------------------------------------------
// CSVPUriParser::CheckDomain
// ---------------------------------------------------------------------------
//
TBool CSVPUriParser::CheckDomain( const TDesC8& aUri ) const
    {
    SVPDEBUG1( "CSVPUriParser::CheckDomain" )
    
    // Check whether the "@" chearacter is the last one.
    return ( aUri.Find( KSVPAt ) < aUri.Length() - 1 );
    }

// ---------------------------------------------------------------------------
// CSVPUriParser::DomainL
// ---------------------------------------------------------------------------
//
HBufC8* CSVPUriParser::DomainL( const TDesC8& aUri )
    {
    SVPDEBUG1( "CSVPUriParser::DomainL In" )
    
    HBufC8* ptr = NULL;
    
    // Check that URI is complete; it has domain part:
    if ( aUri.Find( KSVPAt ) < aUri.Length() - 1 )
        {
        TInt uriLength = aUri.Length();
        TInt atPos = aUri.Find( KSVPAt );
        //__ASSERT_ALWAYS( atPos != KErrNotFound, User::Leave( KErrArgument ) );
        TPtrC8 domainPart = aUri.Right( uriLength - atPos - 1 );
        ptr = domainPart.AllocL();
        }
    
    SVPDEBUG1( "CSVPUriParser::DomainL Out" )
    return ptr;
    }

// ---------------------------------------------------------------------------
// CSVPUriParser::AddSipPrefixL
// ---------------------------------------------------------------------------
//
void CSVPUriParser::AddSipPrefixL( HBufC8*& aUri ) const 
    {
    SVPDEBUG1( "CSVPUriParser::AddSipPrefixL In" )
    aUri = aUri->ReAllocL( aUri->Length() + KSVPSipPrefixLength );
    aUri->Des().Insert( 0, KSVPSipPrefix );
    
    SVPDEBUG1( "CSVPUriParser::AddSipPrefixL Out" )
    }

// ---------------------------------------------------------------------------
// CSVPUriParser::RemoveSipPrefixL
// ---------------------------------------------------------------------------
//
void CSVPUriParser::RemoveSipPrefixL( HBufC8*& aUri ) const 
    {
    SVPDEBUG1( "CSVPUriParser::RemoveSipPrefixL" )
    
    aUri->Des().Delete( 0, KSVPSipPrefixLength );
    }

// ---------------------------------------------------------------------------
// CSVPUriParser::AddSipsPrefixL
// ---------------------------------------------------------------------------
//
void CSVPUriParser::AddSipsPrefixL( HBufC8*& aUri ) const 
    {
    SVPDEBUG1( "CSVPUriParser::AddSipsPrefixL In" )
    
    aUri = aUri->ReAllocL( aUri->Length() + KSVPSipsPrefixLength );
    aUri->Des().Insert( 0, KSVPSipsPrefix );
    
    SVPDEBUG1( "CSVPUriParser::AddSipsPrefixL Out" )
    }

// ---------------------------------------------------------------------------
// CSVPUriParser::RemoveSipsPrefixL
// ---------------------------------------------------------------------------
//
void CSVPUriParser::RemoveSipsPrefixL( HBufC8*& aUri ) const 
    {
    SVPDEBUG1( "CSVPUriParser::RemoveSipsPrefixL" )
    
    aUri->Des().Delete( 0, KSVPSipsPrefixLength );
    }

// ---------------------------------------------------------------------------
// CSVPUriParser::AddAtL
// ---------------------------------------------------------------------------
//
void CSVPUriParser::AddAtL( HBufC8*& aUri ) const
    {
    SVPDEBUG1( "CSVPUriParser::AddAtL In" )
    
    aUri = aUri->ReAllocL( aUri->Length() + KSVPAtLength );
    aUri->Des().Append( KSVPAt );
    
    SVPDEBUG1( "CSVPUriParser::AddAtL Out" )
    }

// ---------------------------------------------------------------------------
// CSVPUriParser::AddDomainL
// ---------------------------------------------------------------------------
//
void CSVPUriParser::AddDomainL( 
    HBufC8*& aUri, const TDesC8& aAOR ) const 
    {
    SVPDEBUG1( "CSVPUriParser::AddDomainL In" )
    
    // Parse the domain part from the AOR.
    HBufC8* domainBuf = HBufC8::NewLC( aAOR.Length() ); // CS : 1
    TInt atPos = aAOR.Find( KSVPAt );
    
    // Check, that the "@" character is in AOR.
    __ASSERT_ALWAYS( KErrNotFound != atPos, User::Leave( KErrArgument ) );
    // Check, that there is a domain part.
    __ASSERT_ALWAYS( atPos < aAOR.Length()-1, User::Leave( KErrArgument ) );
    
    // Copy the domain to the domain buffer.
    domainBuf->Des().Append( aAOR.Mid( atPos + 1 ) );
    
    // Re-allocate the URI
    aUri = aUri->ReAllocL( aUri->Length() + domainBuf->Length() );
    aUri->Des().Append( *domainBuf ); // Append the domain.
    
    CleanupStack::PopAndDestroy( domainBuf ); // CS : 0
    
    SVPDEBUG1( "CSVPUriParser::AddDomainL Out" )
    }

// ---------------------------------------------------------------------------
// CSVPUriParser::CheckLeftBracket
// ---------------------------------------------------------------------------
//
TBool CSVPUriParser::CheckLeftBracket( const TDesC8& aUri ) const
    {
    SVPDEBUG1( "CSVPUriParser::CheckLeftBracket" )
    
    // The "<" prefix is expected to be at the beginning of the URI.
    return ( 0 == aUri.Find( KSVPLeftBracketMark ) );
    }

// ---------------------------------------------------------------------------
// CSVPUriParser::RemoveLeftBracket
// ---------------------------------------------------------------------------
//
void CSVPUriParser::RemoveLeftBracket( HBufC8*& aUri ) const 
    {
    SVPDEBUG1( "CSVPUriParser::RemoveLeftBracket" )
    
    aUri->Des().Delete( 0, KSVPSingleBracketLength );
    }

// ---------------------------------------------------------------------------
// CSVPUriParser::CheckRightBracket
// ---------------------------------------------------------------------------
//
TBool CSVPUriParser::CheckRightBracket( const TDesC8& aUri ) const
    {
    SVPDEBUG1( "CSVPUriParser::CheckRightBracket" )
    
    // The ">" prefix is expected to be at the end of the URI.
    return ( aUri.Length() == ( aUri.Find( KSVPRightBracketMark )
                                + KSVPSingleBracketLength ) );
    }

// ---------------------------------------------------------------------------
// CSVPUriParser::RemoveRightBracket
// ---------------------------------------------------------------------------
//
void CSVPUriParser::RemoveRightBracket( HBufC8*& aUri ) const 
    {
    SVPDEBUG1( "CSVPUriParser::RemoveRightBracket" )
    
    aUri->Des().Delete( aUri->Length() - KSVPSingleBracketLength,
                        KSVPSingleBracketLength );
    }

// ---------------------------------------------------------------------------
// CSVPUriParser::EscapeEncodeSipUriL
// ---------------------------------------------------------------------------
//
void CSVPUriParser::EscapeEncodeSipUriL( HBufC8*& aSipUri,
        EscapeUtils::TEscapeMode aMode )
    {
    SVPDEBUG1( "CSVPUriParser::EscapeEncodeSipUriL In" )
    
    HBufC8* temp = EscapeUtils::EscapeEncodeL( *aSipUri, aMode );
    CleanupStack::PushL( temp );
    aSipUri = aSipUri->ReAllocL( temp->Length() );
    *aSipUri = *temp;
    CleanupStack::PopAndDestroy( temp );
    
    SVPDEBUG1( "CSVPUriParser::EscapeEncodeSipUriL Out" )
    }

// ---------------------------------------------------------------------------
// CSVPUriParser::EscapeDecodeSipUriL
// ---------------------------------------------------------------------------
//
void CSVPUriParser::EscapeDecodeSipUriL( HBufC8*& aSipUri )
    {
    SVPDEBUG1( "CSVPUriParser::EscapeDecodeSipUriL In" )
    
    HBufC8* temp = EscapeUtils::EscapeDecodeL( *aSipUri );
    CleanupStack::PushL( temp ); 
    
    if( temp->Length() > aSipUri->Length() )
        {
        aSipUri = aSipUri->ReAllocL( temp->Length() );
        }
    
    *aSipUri = *temp;
    
    CleanupStack::PopAndDestroy( temp );
    
    SVPDEBUG1( "CSVPUriParser::EscapeDecodeSipUriL Out" )
    }

// ---------------------------------------------------------------------------
// CSVPUriParser::ParseDisplayNameL
// ---------------------------------------------------------------------------
// 
HBufC* CSVPUriParser::ParseDisplayNameL( const TDesC8& aRemoteParty )
    {
    SVPDEBUG1( "CSVPUriParser::ParseDisplayNameL()" )
        
    // Open SIP string pool 
    SIPStrings::OpenL();

    // Make a local copy
    HBufC8* utf8 = HBufC8::NewLC( aRemoteParty.Length() ); // CS:1
    ( utf8->Des() ).Copy( aRemoteParty );
    
    // Escape decode
    CSVPUriParser::EscapeDecodeSipUriL( utf8 );

    // Create SIP From header
    CSIPFromHeader* originatorHdr = CSIPFromHeader::DecodeL( *utf8 );
    CleanupStack::PopAndDestroy( utf8 ); // CS:0
    CleanupStack::PushL( originatorHdr ); // CS:1
    
    // Extract and parse display name, format is "..."
    HBufC8* displayName8 = originatorHdr->SIPAddress().DisplayName().AllocL();
    CleanupStack::PopAndDestroy( originatorHdr ); // CS:0
    CleanupStack::PushL( displayName8 ); // CS:1

    // Convert to 16-bit
    HBufC* displayName = HBufC::NewL( displayName8->Length() );
    ( displayName->Des() ).Copy( *displayName8 );
    CleanupStack::PopAndDestroy( displayName8 ); // CS:0
    
    // Close SIP string pool
    SIPStrings::Close();

    // Parse: remove quotation marks from both ends
    displayName->Des().Trim(); 
    if ( KSVPQuotationMark() == displayName->Left( 1 ) && 
         KSVPQuotationMark() == displayName->Right( 1 ) )
        {
        displayName->Des().Delete( 0, 1 );
        displayName->Des().Delete( displayName->Length() - 1, 1 );
        }
    
    // Parse: remove backslashes   
    TLex lex( *displayName );
    const TChar backslash = '\\';
    TBool chDeleted = EFalse;
    while ( !lex.Eos() )
        {
        if ( backslash == lex.Peek() && !chDeleted )
            {
            // Backslash found, delete it if not consecutive
            displayName->Des().Delete( lex.Offset() , 1 );
            lex.Inc();
            chDeleted = ETrue;
            }
        else
            {
            // Not a backslash
            lex.Inc();
            chDeleted = EFalse;
            }
        }

    // Handle anonymous
    if ( KErrNotFound != displayName->Des().FindF( KSVPAnonymous ) )
        {
        SVPDEBUG1( "CSVPUriParser::ParseDisplayNameL, anonymous case" )
        displayName->Des().Copy( KNullDesC );
        }

    SVPDEBUG2( "CSVPUriParser::ParseDisplayNameL, display name: %S", 
        displayName )
    
    CleanupStack::PushL( displayName );
    HBufC8* utf8DisplayName8 = HBufC8::NewL( displayName->Length() );
    ( utf8DisplayName8->Des() ).Copy( *displayName );
    CleanupStack::PopAndDestroy( displayName );

    CleanupStack::PushL( utf8DisplayName8 ); 
    // Convert from UTF8 to unicode
    HBufC* unicodeDisplayName = EscapeUtils::ConvertToUnicodeFromUtf8L( *utf8DisplayName8 );
    CleanupStack::PopAndDestroy(utf8DisplayName8);

    return unicodeDisplayName;
    }

// ---------------------------------------------------------------------------
// CSVPUriParser::ParseRemotePartyUriL
// ---------------------------------------------------------------------------
// 
HBufC* CSVPUriParser::ParseRemotePartyUriL( const TDesC8& aRemoteParty )
    {
    SVPDEBUG1( "CSVPUriParser::ParseRemotePartyUriL()" )
        
    // Open SIP string pool 
    SIPStrings::OpenL();

    // Make a local copy
    HBufC8* utf8 = HBufC8::NewLC( aRemoteParty.Length() ); // CS:1
    ( utf8->Des() ).Copy( aRemoteParty );

    // Escape decode
    CSVPUriParser::EscapeDecodeSipUriL( utf8 );
    
    // Create SIP From header
    CSIPFromHeader* originatorHdr = CSIPFromHeader::DecodeL( *utf8 );
    CleanupStack::PopAndDestroy( utf8 ); // CS:0
    CleanupStack::PushL( originatorHdr ); // CS:1
        
    // Extract URI, returned without angle brackets
    HBufC8* uri8 = originatorHdr->SIPAddress().Uri8().Uri().UriDes().AllocL();
    CleanupStack::PopAndDestroy( originatorHdr ); // CS:0
    CleanupStack::PushL( uri8 ); // CS:1
    
    // Parse: remove URI parameters
    TInt posSemiColon = uri8->FindF( KSVPSemiCln );
    if ( KErrNotFound != posSemiColon )
        {
        uri8->Des().Delete( posSemiColon, uri8->Length() - posSemiColon );
        }
    
    // Convert from UTF8 to unicode
    HBufC* uri = EscapeUtils::ConvertToUnicodeFromUtf8L( *uri8 );
    CleanupStack::PopAndDestroy( uri8 ); // CS:0

    // Handle anonymous
    if ( KErrNotFound != uri->Des().FindF( KSVPAnonymous ) )
        {
        SVPDEBUG1( "CSVPUriParser::ParseRemotePartyUriL, anonymous case" )
        uri->Des().Copy( KNullDesC );
        }

    // Close SIP string pool
    SIPStrings::Close();
    
    SVPDEBUG2( "CSVPUriParser::ParseRemotePartyUriL, URI: %S", uri )
    return uri;
    }

//  End of File
