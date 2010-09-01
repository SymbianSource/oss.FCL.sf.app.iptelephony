/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


#include "csvtsipuriparser.h"
#include "svtphonenumbervalidator.h"

_LIT( KColon, ":");
_LIT( KAtSign, "@");
_LIT( KLeftElbow, "<" );
_LIT( KRightElbow, ">" );

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSvtSipUriParser::CSvtSipUriParser
// ---------------------------------------------------------------------------
//
CSvtSipUriParser::CSvtSipUriParser( TDomainPartClippingSetting aOptions )
    :
    iOptions( aOptions )
    {

    }


// ---------------------------------------------------------------------------
// CSvtSipUriParser::ConstructL
// ---------------------------------------------------------------------------
//
void CSvtSipUriParser::ConstructL( const TDesC& aSipUri )
    {
    ParseSipUriL( aSipUri );
    }


// ---------------------------------------------------------------------------
// CSvtSipUriParser::NewL
// ---------------------------------------------------------------------------
//
CSvtSipUriParser* CSvtSipUriParser::NewL( const TDesC& aSipUri,
        TDomainPartClippingSetting aOptions )
    {
    CSvtSipUriParser* self = CSvtSipUriParser::NewLC( aSipUri, aOptions );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CSvtSipUriParser::NewLC
// ---------------------------------------------------------------------------
//
CSvtSipUriParser* CSvtSipUriParser::NewLC( const TDesC& aSipUri,
        TDomainPartClippingSetting aOptions )
    {
    CSvtSipUriParser* self = new( ELeave ) CSvtSipUriParser( aOptions );
    CleanupStack::PushL( self );
    self->ConstructL( aSipUri );
    return self;
    }


// ---------------------------------------------------------------------------
// CSvtSipUriParser::~CSvtSipUriParser
// ---------------------------------------------------------------------------
//
CSvtSipUriParser::~CSvtSipUriParser()
    {
    iUserName.Close();
    iDomain.Close();
    iDisplayName.Close();
    }


// ---------------------------------------------------------------------------
// CSvtSipUriParser::GetVoipAddress
// ---------------------------------------------------------------------------
//
TInt CSvtSipUriParser::GetVoipAddress( RBuf& aVoipAddress ) const
    {
    aVoipAddress = KNullDesC();
    
    if ( IsAnonymousUri( iUserName ) )
        {
        return KErrNone;
        }
    
    TInt result( KErrNone );
    TInt addressLength = AddressLength( iUserName, iDomain, iOptions );
    if ( aVoipAddress.MaxLength() < addressLength )
        {
        result = aVoipAddress.ReAlloc( addressLength );
        }
    
    if ( KErrNone == result )
        {
        aVoipAddress.Append( iUserName );
        
        if ( iUserName.Length() < addressLength )
            {
            // domain part clipping isn't used
            aVoipAddress.Append( KAtSign() );
            aVoipAddress.Append( iDomain );
            }
        }
    
    return result;
    }


// ---------------------------------------------------------------------------
// CSvtSipUriParser::GetPhoneNumber
// ---------------------------------------------------------------------------
//
TInt CSvtSipUriParser::GetPhoneNumber( RBuf& aPhoneNumber ) const
    {
    aPhoneNumber = KNullDesC();
    
    TInt result( KErrNone );
    if ( SvtPhoneNumberValidator::IsValidNumber( iUserName ) 
            && ( ENoClipping != iOptions ) )
        {
        if ( aPhoneNumber.MaxLength() < iUserName.Length() )
            {
            result = aPhoneNumber.ReAlloc( iUserName.Length() );
            }
        
        if ( KErrNone == result )
            {
            aPhoneNumber.Append( iUserName );
            }        
        }
    
    return result;
    }


// ---------------------------------------------------------------------------
// CSvtSipUriParser::GetDisplayName
// ---------------------------------------------------------------------------
//
TInt CSvtSipUriParser::GetDisplayName( RBuf& aDisplayName ) const
    {
    aDisplayName = KNullDesC();
    
    TInt result( KErrNone );
    if ( aDisplayName.MaxLength() < iDisplayName.Length() )
        {
        result = aDisplayName.ReAlloc( iDisplayName.Length() );
        }
    
    if ( KErrNone == result )
        {
        aDisplayName = iDisplayName;
        }
    
    return result;
    }


// ---------------------------------------------------------------------------
// CSvtSipUriParser::ParseSipUriL
// Accepted URI forms are: [schema:]user@host[:port] and 
// displayname <[schema:]user@host[:port]>. URI parameters and headers are 
// ripped off in MCE & SVP.
// ---------------------------------------------------------------------------
//
void CSvtSipUriParser::ParseSipUriL( const TDesC& aSipUri )
    {
    if ( IsAnonymousUri( aSipUri ) )
        {
        ParsePrivateAddress();
        }
    else
        {
        RBuf sipUri( aSipUri.AllocL() );
        CleanupClosePushL( sipUri );
        
        // remove trailing and leading white spaces from input uri
        sipUri.TrimAll();
        ParseDisplayNameL( sipUri );
        RemoveElbows( sipUri );
        ParseUserNameL( sipUri );
        ParseDomainL( sipUri );
        
        CleanupStack::PopAndDestroy( &sipUri );
        }
    }


// ---------------------------------------------------------------------------
// CSvtSipUriParser::ParseDisplayNameL
// ---------------------------------------------------------------------------
//
void CSvtSipUriParser::ParseDisplayNameL( const TDesC& aSipUri )
    {
    iDisplayName.Close();
    
    TInt leftElbowIndex = aSipUri.Find( KLeftElbow );
    TInt rightElbowIndex = aSipUri.Find( KRightElbow );
    if ( ( 0 < leftElbowIndex ) && ( KErrNotFound != rightElbowIndex ) )
        {
        // display name is available
        iDisplayName.Assign( aSipUri.Left( leftElbowIndex ).AllocL() );
        
        // remove whitespace around display name
        iDisplayName.TrimAll();
        
        // remove quatation marks around display name
        _LIT( KQuatationMark, "\"");
        if ( iDisplayName.Length() && iDisplayName[0] == '"' )
            {
            iDisplayName.Delete( 0, KQuatationMark().Length() );
            }
        
        if ( iDisplayName.Length() 
                && iDisplayName[iDisplayName.Length() - 1] == '"')
            {
            iDisplayName.Delete( 
                iDisplayName.Length() - 1, KQuatationMark().Length() );
            }
        }
    }


// ---------------------------------------------------------------------------
// CSvtSipUriParser::RemoveElbows
// ---------------------------------------------------------------------------
//
void CSvtSipUriParser::RemoveElbows( TDes& sipUri ) const
    {
    TInt leftElbowIndex = sipUri.Find( KLeftElbow );
    if ( KErrNotFound != leftElbowIndex )
        {
        sipUri.Delete( leftElbowIndex, KLeftElbow().Length() );
        }
    
    TInt rightElbowIndex = sipUri.Find( KRightElbow );
    if ( KErrNotFound != rightElbowIndex )
        {
        sipUri.Delete( rightElbowIndex, KRightElbow().Length() );
        }
    }


// ---------------------------------------------------------------------------
// CSvtSipUriParser::ParseUserNameL
// ---------------------------------------------------------------------------
//
void CSvtSipUriParser::ParseUserNameL( const TDesC& aSipUri )
    {
    // If uri does not specify schema, user name starts from the beginning of
    // the uri. Otherwise additional calculations are done.
    TInt userNameStartInd = 0;
    TInt indexOfColon = aSipUri.Find( KColon() );
    if ( KErrNotFound != indexOfColon )
        {
        userNameStartInd = aSipUri.Find( KColon() ) + KColon().Length();
        __ASSERT_ALWAYS( KColon().Length() <= userNameStartInd, 
            User::Leave( KErrArgument ) );
        }
    
    TInt userNameEndInd = aSipUri.Find( KAtSign() ) - KAtSign().Length();
    __ASSERT_ALWAYS( 0 <= userNameEndInd, User::Leave( KErrArgument ) );
    
    TInt userNameLength = ( userNameEndInd - userNameStartInd ) + 1;
    __ASSERT_ALWAYS( 0 < userNameLength, User::Leave( KErrArgument ) );
    
    iUserName.Close();
    iUserName.Assign( aSipUri.Mid( userNameStartInd, userNameLength ).AllocL() );
    }


// ---------------------------------------------------------------------------
// CSvtSipUriParser::ParseDomainL
// ---------------------------------------------------------------------------
//
void CSvtSipUriParser::ParseDomainL( const TDesC& aSipUri )
    {
    TInt hostNameStartInd = aSipUri.Find( KAtSign() ) + KAtSign().Length();
    __ASSERT_ALWAYS( KAtSign().Length() <= hostNameStartInd, 
        User::Leave( KErrArgument ) );
    
    TInt hostNameEndInd = aSipUri.Length() - 1;
    TInt hostNameLength = ( hostNameEndInd - hostNameStartInd ) + 1;
    __ASSERT_ALWAYS( 0 < hostNameLength, User::Leave( KErrArgument ) );
    
    iDomain.Close();
    iDomain.Assign( aSipUri.Mid( hostNameStartInd, hostNameLength ).AllocL() );
    }


// ---------------------------------------------------------------------------
// CSvtSipUriParser::ParsePrivateAddress
// ---------------------------------------------------------------------------
//
void CSvtSipUriParser::ParsePrivateAddress()
    {
    iUserName.Close();
    iUserName = KNullDesC();
    
    iDomain.Close();
    iDomain = KNullDesC();
    }


// ---------------------------------------------------------------------------
// CSvtSipUriParser::AddressLength
// ---------------------------------------------------------------------------
//
TInt CSvtSipUriParser::AddressLength( const TDesC& aUserName, 
        const TDesC& aDomain, TDomainPartClippingSetting aClipSetting ) const
    {
    TInt addressLength( 0 );
    TBool isPhoneNumber( SvtPhoneNumberValidator::IsValidNumber( aUserName ) );
    switch ( aClipSetting )
        {
        case ENoClipping:
            addressLength = 
            aUserName.Length() + KAtSign().Length() + aDomain.Length();
            break;
        
        case EClipDomainIfNumber:
            addressLength = isPhoneNumber 
                ? aUserName.Length() 
                : aUserName.Length() + KAtSign().Length() + aDomain.Length();
            break;
        
        case EClipDomain:
            addressLength = aUserName.Length();
            break;
        
        default:
            ASSERT( EFalse );
        }
    
    return addressLength;
    }


// ---------------------------------------------------------------------------
// CSvtSipUriParser::IsAnonymousUri
// ---------------------------------------------------------------------------
//
TBool CSvtSipUriParser::IsAnonymousUri( const TDesC& aCandidate ) const
    {
    if ( aCandidate.Length() == 0 )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }
