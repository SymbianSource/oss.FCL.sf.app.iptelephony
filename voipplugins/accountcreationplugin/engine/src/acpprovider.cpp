/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implements CAcpProvider methods
*
*/


#include <fbs.h>
#include <gulicon.h>

#include "acpprovider.h"
#include "accountcreationpluginlogger.h"

// ---------------------------------------------------------------------------
// CAcpProvider::CAcpProvider
// ---------------------------------------------------------------------------
//
CAcpProvider::CAcpProvider() 
    {
    }

// ---------------------------------------------------------------------------
// CAcpProvider::NewL
// ---------------------------------------------------------------------------
//
CAcpProvider* CAcpProvider::NewL()
    {
    CAcpProvider* self = CAcpProvider::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CAcpProvider::NewLC
// ---------------------------------------------------------------------------
//
CAcpProvider* CAcpProvider::NewLC()
    {
    CAcpProvider* self = new ( ELeave ) CAcpProvider();
    CleanupStack::PushL( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CAcpProvider::~CAcpProvider
// ---------------------------------------------------------------------------
//
CAcpProvider::~CAcpProvider()
    {
    ACPLOG( "CAcpProvider::~CAcpProvider begin" );

    delete iProviderName;
    delete iIconUrl;
    delete iSisUrl;
    delete iCreationUrl;
    delete iActivationUrl;
    delete iBitmap;
    delete iMask;
    delete iProviderDescription;
    delete iProviderType;
    delete iMimeType;

    ACPLOG( "CAcpProvider::~CAcpProvider end" );
    }

// ---------------------------------------------------------------------------
// CAcpProvider::CopyL
// Copies provider data from given parameter to member data.
// ---------------------------------------------------------------------------
//
void CAcpProvider::CopyL( const CAcpProvider& aProvider )
    {
    ACPLOG( "CAcpProvider::CopyL begin" );

    SetProviderNameL( aProvider.ProviderName() );
    SetIconUrlL( aProvider.IconUrl() );
    SetPriority( aProvider.Priority() );
    SetSisUrlL( aProvider.SisUrl() );
    SetCreationUrlL( aProvider.CreationUrl() );
    SetActivationUrlL( aProvider.ActivationUrl() );
    SetProviderTypeL( aProvider.ProviderType() );
    SetProviderDescriptionL( aProvider.ProviderDescription() );

    ACPLOG( "CAcpProvider::CopyL end" );
    }

// ---------------------------------------------------------------------------
// CAcpProvider::ProviderName
// Returns name of the provider.
// ---------------------------------------------------------------------------
//
TPtrC CAcpProvider::ProviderName() const
    {
    if ( !iProviderName )
        {
        return KNullDesC();
        }
    return *iProviderName;
    }

// ---------------------------------------------------------------------------
// CAcpProvider::SetProviderNameL
// Sets name of the provider.
// ---------------------------------------------------------------------------
//
void CAcpProvider::SetProviderNameL( const TDesC& aProviderName )
    {
    ACPLOG2( "CAcpProvider::SetProviderNameL: %S", &aProviderName );

    // Check whether the provider name defined or not.
    if ( !aProviderName.Length() )
        {
        return;
        }

    if ( !iProviderName ) 
        {
        iProviderName = HBufC::NewL( aProviderName.Length() );
        }
    else
        {
        iProviderName = iProviderName->ReAllocL( 
            iProviderName->Length() + aProviderName.Length() );
        }
    iProviderName->Des().Append( aProviderName );
    }

// ---------------------------------------------------------------------------
// CAcpProvider::IconUrl
// Returns the location of icons.
// ---------------------------------------------------------------------------
//
TPtrC8 CAcpProvider::IconUrl() const
    {
    if ( !iIconUrl )
        {
        return KNullDesC8();
        }

    return *iIconUrl;
    }

// ---------------------------------------------------------------------------
// CAcpProvider::SetIconUrlL
// Sets the location of icons.
// ---------------------------------------------------------------------------
//
void CAcpProvider::SetIconUrlL( const TDesC8& aIconUrl )
    {
    ACPLOG2( "CAcpProvider::SetIconUrlL: %S", &aIconUrl );

    // Check whether the icon address is defined or not.
    if ( !aIconUrl.Length() )
        {
        return;
        }
 
    if ( !iIconUrl )
        {
        iIconUrl = HBufC8::NewL( aIconUrl.Length() );
        }
    else
        {
        iIconUrl = iIconUrl->ReAllocL( 
            iIconUrl->Length() + aIconUrl.Length() );
        }

    iIconUrl->Des().Append( aIconUrl );
    }

// ---------------------------------------------------------------------------
// CAcpProvider::Priority
// Returns priority of the provider.
// ---------------------------------------------------------------------------
//
TInt CAcpProvider::Priority() const
    {
    return iPriority;
    }

// ---------------------------------------------------------------------------
// CAcpProvider::SetPriority
// Sets priority of the provider.
// ---------------------------------------------------------------------------
//
void CAcpProvider::SetPriority( TInt aPriority )
    {
    ACPLOG2( "CAcpProvider::SetPriority: %d", aPriority );
    iPriority = aPriority;
    }

// ---------------------------------------------------------------------------
// CAcpProvider::SisUrl
// Returns the location of SIS file.
// ---------------------------------------------------------------------------
//
TPtrC8 CAcpProvider::SisUrl() const
    {
    if ( iSisUrl )
        {
        return *iSisUrl;
        }

    return KNullDesC8();
    }

// ---------------------------------------------------------------------------
// CAcpProvider::SetSisUrlL
// Sets the location of SIS file.
// ---------------------------------------------------------------------------
//
void CAcpProvider::SetSisUrlL( const TDesC8& aSisUrl )
    {
    ACPLOG2( "CAcpProvider::SetSisUrlL: %S", &aSisUrl );

    // Check whether the SIS address defined or not.
    if ( !aSisUrl.Length() )
        {
        return;
        }

    if ( !iSisUrl )
        {
        iSisUrl = HBufC8::NewL( aSisUrl.Length() );
        }
    else
        {
        iSisUrl = iSisUrl->ReAllocL( iSisUrl->Length() + aSisUrl.Length() );
        }
    iSisUrl->Des().Append( aSisUrl );

    ACPLOG( "CAcpProvider::SetSisUrlL: end" );
    }

// ---------------------------------------------------------------------------
// CAcpProvider::CreationUrl
// Returns the location of creation URL.
// ---------------------------------------------------------------------------
//
TPtrC8 CAcpProvider::CreationUrl() const
    {
    if ( iCreationUrl )
        {
        return *iCreationUrl;
        }
    return KNullDesC8();
    }

// ---------------------------------------------------------------------------
// CAcpProvider::SetCreationUrlL
// Sets the location of creation URL.
// ---------------------------------------------------------------------------
//
void CAcpProvider::SetCreationUrlL( const TDesC8& aCreationUrl )
    {
    ACPLOG( "CAcpProvider::SetCreationUrlL: begin ");

    // Check whether the creation address defined or not
    if ( !aCreationUrl.Length() )
        {
        return;
        }

    if ( !iCreationUrl )
        {
        iCreationUrl = HBufC8::NewL( aCreationUrl.Length() );
        }
    else
        {
        iCreationUrl = iCreationUrl->ReAllocL( 
            iCreationUrl->Length() + aCreationUrl.Length() );
        }
    iCreationUrl->Des().Append( aCreationUrl );

    ACPLOG( "CAcpProvider::SetCreationUrlL end")
    }    

// ---------------------------------------------------------------------------
// CAcpProvider::ActivationUrl
// Returns the location of activation URL.
// ---------------------------------------------------------------------------
//
TPtrC8 CAcpProvider::ActivationUrl() const
    {
    if ( iActivationUrl )
        {
        return *iActivationUrl;
        }

    return KNullDesC8();
    }

// ---------------------------------------------------------------------------
// CAcpProvider::SetActivationUrlL
// Sets the location of activation URL.
// ---------------------------------------------------------------------------
//
void CAcpProvider::SetActivationUrlL( const TDesC8& aActivationUrl )
    {
    ACPLOG2( "CAcpProvider::SetActivationUrlL: %S", &aActivationUrl );

    // Check whether the activation address defined or not.
    if ( !aActivationUrl.Length() )
        {
        return;
        }

    if ( !iActivationUrl )
        {
        iActivationUrl = HBufC8::NewL( aActivationUrl.Length() );
        }
    else
        {
        iActivationUrl = iActivationUrl->ReAllocL( 
            iActivationUrl->Length() + aActivationUrl.Length() );
        }

    iActivationUrl->Des().Append( aActivationUrl ); 
    }

// ---------------------------------------------------------------------------
// CAcpProvider::ContentData
// Returns the mime type of image file.
// ---------------------------------------------------------------------------
//
TPtrC8 CAcpProvider::ContentData() const
    {
    if ( iMimeType )
        {
        return *iMimeType;
        }
    return KNullDesC8();
    }

// ---------------------------------------------------------------------------
// CAcpProvider::SetContentDataL
// Sets the content type of image file.
// ---------------------------------------------------------------------------
//
void CAcpProvider::SetContentDataL( const TDesC8& aContentData )
    {
    ACPLOG2( "CAcpProvider::SetContentDataL: %S", &aContentData );
    
    // Check whether content data defined or not.
    if ( !aContentData.Length() )
        {
        return;
        }

    if ( !iMimeType )
        {
        iMimeType = HBufC8::NewL( aContentData.Length() );
        }
    else
        {
        iMimeType = iMimeType->ReAllocL( 
            iMimeType->Length() + aContentData.Length() );
        }
    iMimeType->Des().Append( aContentData ); 
    }

// ---------------------------------------------------------------------------
// CAcpProvider::ProviderType
// Returns the provider type
// ---------------------------------------------------------------------------
//
TPtrC CAcpProvider::ProviderType() const
    {
    if ( iProviderType )
        {
        return *iProviderType;
        }
    return KNullDesC();
    }

// ---------------------------------------------------------------------------
// CAcpProvider::SetProviderTypeL
// Sets the provider type
// ---------------------------------------------------------------------------
//
void CAcpProvider::SetProviderTypeL( const TDesC& aProviderType )
    {
    ACPLOG2( "CAcpProvider::SetProviderTypeL: %S", &aProviderType );

    // Check whether provider type defined or not.
    if ( !aProviderType.Length() )
        {
        return;
        }

    if ( !iProviderType )
        {
        iProviderType = HBufC::NewL( aProviderType.Length() );
        }
    else
        {
        iProviderType = iProviderType->ReAllocL( 
            iProviderType->Length() + aProviderType.Length() );
        }
    iProviderType->Des().Append( aProviderType );
    }

// ---------------------------------------------------------------------------
// CAcpProvider::ProviderDescription
// Returns the provider description
// ---------------------------------------------------------------------------
//
TPtrC CAcpProvider::ProviderDescription() const
    {
    if ( iProviderDescription )
        {
        return *iProviderDescription; 
        }
    return KNullDesC();
    }

// ---------------------------------------------------------------------------
// CAcpProvider::SetProviderDescriptionL
// Sets the provider description
// ---------------------------------------------------------------------------
//
void CAcpProvider::SetProviderDescriptionL( const TDesC& aProviderDescription )
    {
    ACPLOG2( "CAcpProvider::SetProviderDescriptionL: %S", &aProviderDescription );

    // Check whether the provider description defined or not.
    if ( !aProviderDescription.Length() )
        {
        return;
        }

    if ( !iProviderDescription )
        {
        iProviderDescription = HBufC::NewL( aProviderDescription.Length() );
        }
    else
        {
        iProviderDescription = iProviderDescription->ReAllocL( 
            iProviderDescription->Length() + aProviderDescription.Length() );
        }

    iProviderDescription->Des().Append( aProviderDescription );
    }

// ---------------------------------------------------------------------------
// CAcpProvider::SetBitmapL
// Copies a bitmap.
// ---------------------------------------------------------------------------
//
void CAcpProvider::SetBitmapL( CFbsBitmap* aBitmap, CFbsBitmap* aMask )
    {
    ACPLOG( "CAcpProvider::SetBitmapL begin" );
    
    if ( !aBitmap ) // Mask is not needed.
        {
        User::Leave( KErrArgument );
        }
    
    const TSize bitmapSize = aBitmap->SizeInPixels();
    if ( bitmapSize.iHeight > 0 && bitmapSize.iWidth > 0 )
        {
        ACPLOG3( " - bitmap size %d x %d pixels", bitmapSize.iWidth,
            bitmapSize.iHeight );
        
        delete iBitmap;
        iBitmap = NULL;
        delete iMask;
        iMask = NULL;
        
        ACPLOG( " - deleted bitmaps" );
    
        // Create new bitmap.
        iBitmap = new (ELeave) CFbsBitmap();
        iBitmap->Create( bitmapSize, aBitmap->DisplayMode() );
        ACPLOG( " - created new bitmap" );
        
        // Calculate data length.
        TInt stride = CFbsBitmap::ScanLineLength( bitmapSize.iWidth,
            aBitmap->DisplayMode() );
        TInt length = stride * bitmapSize.iHeight;
        ACPLOG2( " - actual data length: %d", length );
    
        aBitmap->LockHeap();
        Mem::Copy( iBitmap->DataAddress(), aBitmap->DataAddress(), length );
        aBitmap->UnlockHeap();

        // Create mask if needed.
        if ( aMask )
            {
            iMask = new (ELeave) CFbsBitmap();
            iMask->Create( aMask->SizeInPixels(), aMask->DisplayMode() );
            ACPLOG( " - created new mask" );

            // Calculate data length.
            TSize maskSize = aMask->SizeInPixels();
            TInt stride = CFbsBitmap::ScanLineLength( maskSize.iWidth,
                aMask->DisplayMode() );
            TInt length = stride * maskSize.iHeight;
            ACPLOG2( " - actual mask data length: %d", length );
            
            aMask->LockHeap();
            Mem::Copy( iMask->DataAddress(), aMask->DataAddress(), length );
            aMask->UnlockHeap();
            }
        }

    ACPLOG( "CAcpProvider::SetBitmapL end" );
    }

// ---------------------------------------------------------------------------
// CAcpProvider::BitMap
// Reference to bitmap and mask.
// ---------------------------------------------------------------------------
//
void CAcpProvider::GetBitmaps( CFbsBitmap*& aBitmap, CFbsBitmap*& aMask )
    {
    aBitmap = iBitmap;
    aMask = iMask;
    }

// End of file.
