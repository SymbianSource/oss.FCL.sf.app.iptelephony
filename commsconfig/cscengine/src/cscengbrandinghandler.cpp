/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  For CSC needed branding server handling
*
*/


#include <coemain.h>
#include <AknsUtils.h>
#include <cbsbitmap.h>
#include <mbsaccess.h>
#include <mbsupdater.h>
#include <mbselement.h>
#include <cbsfactory.h>
#include <bselementfactory.h>
#include <AknsSkinInstance.h> 

#include "cscenglogger.h"
#include "cscengbrandinghandler.h"


_LIT8( KDefaultAppId, "xsp" );
_LIT8( KBrandedBitmapItemId, "default_brand_image" );

const TUint KVersion( 1 );

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCEngBrandingHandler::CCSCEngBrandingHandler()
    {    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CCSCEngBrandingHandler::ConstructL()
    {
    CSCENGDEBUG( "CCSCEngBrandingHandler::ConstructL - begin" ); 
    
    iFactory = CBSFactory::NewL( KNullDesC8, KDefaultAppId );
        
    CSCENGDEBUG( "CCSCEngBrandingHandler::ConstructL - end" );
    }   
    
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CCSCEngBrandingHandler* CCSCEngBrandingHandler::NewL()
    {
    CCSCEngBrandingHandler* self = new (ELeave) CCSCEngBrandingHandler;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCEngBrandingHandler::~CCSCEngBrandingHandler()
    {
    CSCENGDEBUG( 
        "CCSCEngBrandingHandler::~CCSCEngBrandingHandler - begin" ); 

    delete iFactory;
    
    CSCENGDEBUG( 
        "CCSCEngBrandingHandler::~CCSCEngBrandingHandler - end" ); 
    }
    

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
EXPORT_C void CCSCEngBrandingHandler::GetServiceBrandIconL( 
    const TDesC8& aBrandId, CFbsBitmap*& aBitmap, CFbsBitmap*& aMask,
    TInt aIconSize )
    {
    CSCENGDEBUG( 
        "CCSCEngBrandingHandler::GetServiceBrandIconL - begin" ); 
        
    // Create access to Branding server
    MBSAccess* access = iFactory->CreateAccessL( 
        aBrandId, ELangInternationalEnglish, KVersion );
    CleanupClosePushL( *access );
            
    // start fetching of branding image     
	// get structure containing the info needed to load brand image
	MBSElement* bitmapItem = NULL;
	bitmapItem = access->GetStructureL( KBrandedBitmapItemId );
	CleanupClosePushL( *bitmapItem );

	// get info from the bitmap element
	const CBSBitmap& bsBitmap = bitmapItem->BitmapDataL();
	
	RFile bitmapFile;
	CleanupClosePushL( bitmapFile ); 
	
	// get the bitmap file from where we can load the bitmap
	access->GetFileL( bsBitmap.BitmapFileId(), bitmapFile );	
	
	TInt err = KErrNotFound;
	CFbsBitmap* skinBitmap = NULL;
	
	// Try getting skinned images first
	
	if( CCoeEnv::Static() )
		{
		MAknsSkinInstance* skin = AknsUtils::SkinInstance();	

		TAknsItemID itemId;
		itemId.Set( bsBitmap.SkinIdMajor(), bsBitmap.SkinIdMinor() );
		
		CleanupStack::PushL( skinBitmap );
		
	    TRAP( err, skinBitmap = AknsUtils::CreateBitmapL( skin, itemId ) );
		
		CleanupStack::Pop( skinBitmap );
		}
    
    if( err == KErrNotFound ) 	// no skin bitmap found
    	{
    	iFile = bitmapFile;    	    
	    AknIconUtils::CreateIconLC( aBitmap, 
	                                aMask,
                					*this,
                					bsBitmap.BitmapId(), 
                					bsBitmap.BitmapMaskId() );

        CleanupStack::Pop( aMask );
        CleanupStack::Pop( aBitmap );
    	}
    else if( err )
    	{
    	User::Leave( err );
    	}
	else
		{
		aBitmap = skinBitmap;
		aMask = NULL;
		}
    
    TSize size( aIconSize, aIconSize );
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
    CleanupStack::PopAndDestroy( access );
    
    CSCENGDEBUG( 
        "CCSCEngBrandingHandler::GetServiceBrandIconL - end" ); 
    } 


// ---------------------------------------------------------------------------
// From class MAknIconFileProvider
// For getting an open file handle to the icon file.
// ---------------------------------------------------------------------------
//
void CCSCEngBrandingHandler::RetrieveIconFileHandleL(
            RFile& aFile, const TIconFileType /*aType*/ )
    {
    aFile.Duplicate( iFile );
    }


    

    
