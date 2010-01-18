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


#include "csvtlogging.h"
#include "csvtsettingshandler.h"
#include "svtphonenumbervalidator.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSvtLogging::CSvtLogging
// ---------------------------------------------------------------------------
//
CSvtLogging::CSvtLogging()
    {

    }


// ---------------------------------------------------------------------------
// CSvtLogging::NewL
// ---------------------------------------------------------------------------
//
CSvtLogging* CSvtLogging::NewL()
    {
    CSvtLogging* self = CSvtLogging::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CSvtLogging::NewLC
// ---------------------------------------------------------------------------
//
CSvtLogging* CSvtLogging::NewLC()
    {
    CSvtLogging* self = new( ELeave ) CSvtLogging;
    CleanupStack::PushL( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CSvtLogging::~CSvtLogging
// ---------------------------------------------------------------------------
//
CSvtLogging::~CSvtLogging()
    {
    delete iParser;
    delete iSettingsHandler;
    iSipUserName.Close();
    }


// ---------------------------------------------------------------------------
// From class CLoggingPluginInterface.
// ---------------------------------------------------------------------------
//
void CSvtLogging::InitializeL( 
        TUint aServiceId, const TDesC& aOrigAddress )
    {
    __ASSERT_ALWAYS( aOrigAddress.Length() != 0, User::Leave( KErrArgument ) );
    
    iSipUserName.Close();
    
    delete iSettingsHandler;
    iSettingsHandler = NULL;
    iSettingsHandler = CreateSvtSettingsHandlerL( aServiceId );
    
    delete iParser;
    iParser = NULL;
    TInt domainClipSetting = iSettingsHandler->DomainPartClippingSetting();
    TRAPD( result, iParser = 
        CreateSipUriParserL( aOrigAddress, domainClipSetting ) );
    if ( KErrNone != result )
        {
        if ( KErrNoMemory == result )
            {
            User::Leave( KErrNoMemory );
            }
        else
            {
            // plugin is initialized with user name only 
            iSipUserName.Assign( aOrigAddress.AllocL() );
            }
        }
    }


// ---------------------------------------------------------------------------
// From class CLoggingPluginInterface.
// ---------------------------------------------------------------------------
//
TInt CSvtLogging::GetPhoneNumber( RBuf& aPhoneNumber )
    {
    TInt result( KErrNone );
    
    if ( iParser )
        {
        result = iParser->GetPhoneNumber( aPhoneNumber );        
        }
    else if ( iSipUserName.Length() != 0 )
        {
        if ( SvtPhoneNumberValidator::IsValidNumber( iSipUserName ) )
            {
            if ( aPhoneNumber.MaxLength() < iSipUserName.Length() )
                {
                result = aPhoneNumber.ReAlloc( iSipUserName.Length() );
                }
            
            if ( KErrNone == result )
                {
                aPhoneNumber.Copy( iSipUserName );
                }
            }
        }
    else
        {
        result = KErrNotReady;
        }
    
    return result;
    }


// ---------------------------------------------------------------------------
// From class CLoggingPluginInterface.
// ---------------------------------------------------------------------------
//
TInt CSvtLogging::GetVoipAddress( RBuf& aVoipAddress )
    {
    TInt result( KErrNone );
    
    if ( iParser )
        {
        result = iParser->GetVoipAddress( aVoipAddress );
        }
    else if ( iSipUserName.Length() != 0 )
        {
        if ( aVoipAddress.MaxLength() < iSipUserName.Length() )
            {
            result = aVoipAddress.ReAlloc( iSipUserName.Length() );
            }
        
        if ( KErrNone == result )
            {
            aVoipAddress.Copy( iSipUserName );
            }
        }
    else
        {
        result = KErrNotReady;
        }
    
    return result;
    }


// ---------------------------------------------------------------------------
// From class CLoggingPluginInterface.
// ---------------------------------------------------------------------------
//
TInt CSvtLogging::GetMyAddress( RBuf& aMyAddress )
    {
    TInt result( KErrNotReady );
    if ( iSettingsHandler )
        {
        TRAP( result, iSettingsHandler->GetUserAorL( aMyAddress ) );        
        }
    
    return result;
    }


// ---------------------------------------------------------------------------
// From class CLoggingPluginInterface.
// ---------------------------------------------------------------------------
//
TInt CSvtLogging::GetRemotePartyName( RBuf& aRemotePartyName )
    {
    TInt result( KErrNone );
    
    if ( iParser )
        {
        result = iParser->GetDisplayName( aRemotePartyName );
        }
    else if ( iSipUserName.Length() != 0 )
        {
        aRemotePartyName = KNullDesC();
        }
    else
        {
        return KErrNotReady;
        }
    
    return result;
    }


// ---------------------------------------------------------------------------
// CSvtLogging::CreateSvtSettingsHandlerL()
// ---------------------------------------------------------------------------
//
CSvtSettingsHandler* CSvtLogging::CreateSvtSettingsHandlerL( 
        TUint aServiceId ) const
    {
    return CSvtSettingsHandler::NewL( aServiceId );
    }


// ---------------------------------------------------------------------------
// CSvtLogging::CreateSipUriParserL()
// ---------------------------------------------------------------------------
//
CSvtSipUriParser* CSvtLogging::CreateSipUriParserL( 
        const TDesC& aOrigAddress, TInt aDomainClipSetting ) const
    {
    return CSvtSipUriParser::NewL( aOrigAddress, 
        ConvertToUriParserSetting( aDomainClipSetting ) );
    }


// ---------------------------------------------------------------------------
// CSvtLogging::ConvertToUriParserSetting()
// ---------------------------------------------------------------------------
//
CSvtSipUriParser::TDomainPartClippingSetting 
    CSvtLogging::ConvertToUriParserSetting( 
        TInt aDomainClipSetting ) const
    {
    CSvtSipUriParser::TDomainPartClippingSetting setting( 
        CSvtSipUriParser::ENoClipping );
    
    switch ( aDomainClipSetting )
        {
        case 0:
            setting = CSvtSipUriParser::ENoClipping;
            break;
        case 1:
            setting = CSvtSipUriParser::EClipDomainIfNumber;
            break;
        case 2:
            setting = CSvtSipUriParser::EClipDomain;
            break;
        default:
            ASSERT( EFalse );
        }
    
    return setting;
    }
