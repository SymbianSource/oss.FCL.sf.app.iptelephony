/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implements access to branding server
*
*/


#include <coemain.h>
#include <spentry.h>
#include <mbsaccess.h>
#include <AknsUtils.h>
#include <cbsbitmap.h>
#include <cbsfactory.h>
#include <mbselement.h>
#include <spsettings.h>
#include <spproperty.h>
#include <spdefinitions.h>

#include "cchuipluginbrandinghandler.h"
#include "cchuipluginiconfileprovider.h"

_LIT8( KDefaultAppId, "xsp" );
_LIT8( KBrandedBitmapItemId, "service_passive_image_small" );

const TUint KIconWidth( 20 );
const TUint KIconHeight( 20 );
const TUint KBrandStringMaxLength( 255 );


// ======== MEMBER FUNCTIONS ========

CCchUiPluginBrandingHandler::CCchUiPluginBrandingHandler( 
    CSPSettings& aSpSettings ):
    iSpSettings( aSpSettings )
    {
    }

void CCchUiPluginBrandingHandler::ConstructL()
    {
    }

CCchUiPluginBrandingHandler* CCchUiPluginBrandingHandler::NewL(
    CSPSettings& aSpSettings )
    {
    CCchUiPluginBrandingHandler* self = NewLC( aSpSettings );
    CleanupStack::Pop( self );
    return self;
    }

CCchUiPluginBrandingHandler* CCchUiPluginBrandingHandler::NewLC(
    CSPSettings& aSpSettings )
    {
    CCchUiPluginBrandingHandler* self = 
        new (ELeave) CCchUiPluginBrandingHandler( aSpSettings );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CCchUiPluginBrandingHandler::~CCchUiPluginBrandingHandler()
    {
    delete iFactory;
    iFactory = NULL;
    if ( iAccess )
        {
        iAccess->Close();
        }
    }

// ---------------------------------------------------------------------------
// Retrieves service icon.
// ---------------------------------------------------------------------------
//
void CCchUiPluginBrandingHandler::RetrieveServiceIconL( 
    TUint32 aServiceId, CFbsBitmap*& aBitmap, CFbsBitmap*& aMask )
    {
    // start fetching of branding image     
    // get structure containing the info needed to load brand image
    MBSElement* bitmapItem = NULL;
    
    // Access must be created and destroyed for each call since this is
    // common implementation for all services, otherwise brand data
    // fetching might fail or return data from another service.
    PrepareBrandingAccessL( aServiceId );
    
    bitmapItem = iAccess->GetStructureL( KBrandedBitmapItemId() );
    CleanupClosePushL( *bitmapItem );

    // get info from the bitmap element
    const CBSBitmap& bsBitmap = bitmapItem->BitmapDataL();
    
    RFile bitmapFile;
    CleanupClosePushL( bitmapFile ); 
    
    // get the bitmap file from where we can load the bitmap
    iAccess->GetFileL( bsBitmap.BitmapFileId(), bitmapFile );    
    
    TInt err = KErrNotFound;
    CFbsBitmap* skinBitmap = NULL;
    
    // Try getting skinned images first
    if ( CCoeEnv::Static() )
        {
        MAknsSkinInstance* skin = AknsUtils::SkinInstance();    

        TAknsItemID itemId;
        itemId.Set( bsBitmap.SkinIdMajor(), bsBitmap.SkinIdMinor() );
        
        TRAP( err, skinBitmap = AknsUtils::CreateBitmapL( skin, itemId ) );
        }
    
    if ( err == KErrNotFound )
        {
        // no skin bitmap found
        
        // icon server take ownership of icon file provider
        // File provider duplicates the file handle, close the original
        // Duplication needed because lifetime of image is different than
        // this file handle's.        
        CCchUiPluginIconFileProvider* ifp = 
            CCchUiPluginIconFileProvider::NewL( bitmapFile );
        CleanupStack::PushL( ifp );
        
        AknIconUtils::CreateIconLC( aBitmap, aMask,
                                    *ifp,
                                    bsBitmap.BitmapId(), 
                                    bsBitmap.BitmapMaskId() );   
        CleanupStack::Pop( aMask );
        CleanupStack::Pop( aBitmap );
        CleanupStack::Pop( ifp );
        }
    else if ( err )
        {
        User::Leave( err );
        }
    else
        {
        aBitmap = skinBitmap;
        aMask = NULL;
        }
    // Icons must be initialized with setsize in order to use them
    TSize size( KIconWidth, KIconHeight );
    if ( aBitmap )
        {
        AknIconUtils::SetSize( aBitmap, size );
        }
    
    if ( aMask )
        {
        AknIconUtils::SetSize( aMask, size );
        }
        
    CleanupStack::PopAndDestroy( &bitmapFile );
    CleanupStack::PopAndDestroy( bitmapItem );
    ReleaseBrandingAccess();
    }

// ---------------------------------------------------------------------------
// Prepares branding access.
// ---------------------------------------------------------------------------
//
void CCchUiPluginBrandingHandler::PrepareBrandingAccessL( TUint32 aServiceId )
    {
    ReleaseBrandingAccess();
    
    HBufC8* brandiId8 = HBufC8::NewLC( KBrandStringMaxLength );
    TPtr8 brandiId8Ptr( brandiId8->Des() );
    
    HBufC* brandId = HBufC::NewLC( KBrandStringMaxLength );
    TPtr brandiIdPtr( brandId->Des() );
    
    CSPProperty* idProperty = CSPProperty::NewLC();
    iSpSettings.FindPropertyL( aServiceId,
                                EPropertyBrandId,
                                *idProperty );    
    if ( idProperty )
        {
        idProperty->GetValue( brandiIdPtr );
        brandiId8Ptr.Copy( brandiIdPtr );
        }
    CleanupStack::PopAndDestroy( idProperty );        
    CleanupStack::PopAndDestroy( brandId );        

    // default to english
    TInt brandLanguage = ELangInternationalEnglish; 
    CSPProperty* lanProperty = CSPProperty::NewLC();
    iSpSettings.FindPropertyL( aServiceId,
                                EPropertyBrandLanguage,
                                *lanProperty );    
    if ( lanProperty )
        {
        lanProperty->GetValue( brandLanguage );
        brandLanguage = ((TLanguage) (brandLanguage));
        }
    CleanupStack::PopAndDestroy( lanProperty );
    
    iFactory = CBSFactory::NewL( brandiId8Ptr, KDefaultAppId );
    iAccess = iFactory->CreateAccessL( brandiId8Ptr, 
            ((TLanguage) ( brandLanguage )) );
    CleanupStack::PopAndDestroy( brandiId8 );
    }

// ---------------------------------------------------------------------------
// Releases branding access.
// ---------------------------------------------------------------------------
//
void CCchUiPluginBrandingHandler::ReleaseBrandingAccess()
    {
    if ( iAccess )
        {
        iAccess->Close();
        iAccess = NULL;
        }
    delete iFactory;    
    iFactory = NULL;
    }


