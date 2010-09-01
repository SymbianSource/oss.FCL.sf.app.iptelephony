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
* Description:  account creation plugin params source file
*
*/


#include <bitmaptransforms.h>
#include <fbs.h>
#include <gulicon.h>

#include "acpimagehandler.h" 
#include "accountcreationpluginlogger.h"
#include "macpimagehandlerobserver.h"
#include "accountcreationengineconstants.h"


// ---------------------------------------------------------------------------
// CAcpImageHandler::NewL
// ---------------------------------------------------------------------------
//
CAcpImageHandler* CAcpImageHandler::NewL( MImageHandlerObserver& aController )
    {
    ACPLOG( "CAcpImageHandler::NewL begin" );
    CAcpImageHandler* self = new(ELeave) CAcpImageHandler( aController );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);
    ACPLOG( "CAcpImageHandler::NewL end" );
    return self; 
    }

// ---------------------------------------------------------------------------
// CAcpImageHandler::CAcpImageHandler
// ---------------------------------------------------------------------------
//    
CAcpImageHandler::CAcpImageHandler( MImageHandlerObserver& aController )
    : CActive( EPriorityStandard ),
    iController( aController )
    {
    }

// ---------------------------------------------------------------------------
// CAcpImageHandler::ConstructL()
// ---------------------------------------------------------------------------
//
void CAcpImageHandler::ConstructL()
    {
    ACPLOG( "CAcpImageHandler::ConstructL begin" );

    // Create file server connection.
    User::LeaveIfError( iFs.Connect() );

    // Create a new active object scheduler for this objects.
    CActiveScheduler::Add( this );

    ACPLOG( "CAcpImageHandler::ConstructL end" );
    }

// ---------------------------------------------------------------------------
// CAcpImageHandler::~CAcpImageHandler()
// ---------------------------------------------------------------------------
//
CAcpImageHandler::~CAcpImageHandler()
    {
    ACPLOG( "CAcpImageHandler::~CAcpImageHandler begin" );

    Cancel();
    iFs.Close();

    delete iImageDecoder;
    delete iBitmap;
    delete iMask;

    ACPLOG( "CAcpImageHandler::~CAcpImageHandler end" );
    }

// ---------------------------------------------------------------------------
// CAcpImageHandler::DoCancel
// ---------------------------------------------------------------------------
//
void CAcpImageHandler::DoCancel()
    {
    ACPLOG( "CAcpImageHandler::DoCancel begin" );

    if ( iImageDecoder )
        {
        iImageDecoder->Cancel();
        }

    ACPLOG( "CAcpImageHandler::DoCancel end" );
    }

// ---------------------------------------------------------------------------
// CAcpImageHandler::RunL
// ---------------------------------------------------------------------------
//	
void CAcpImageHandler::RunL()
    {
    ACPLOG( "CAcpImageHandler::RunL begin" );

    if ( iStatus == KErrUnderflow )
        {
        // Converting continues..
        iImageDecoder->ContinueConvert( &iStatus );

        // Requests of the active object outstanding.
        SetActive();
        }
    else
        {
        iState = EIdle; // Bitmap done and decoder state is idle.
        // Send a client notify about that.
        iController.NotifyImageCompletion( iStatus.Int() );

        // Now it's safe to delete image decoder and close file system handle.
        Cancel();
        delete iImageDecoder;
        iImageDecoder = NULL;
        
        iFs.Close();
        }

    ACPLOG( "CAcpImageHandler::RunL end" );  
    }

// ---------------------------------------------------------------------------
// CAcpImageHandler::StartToDecodeL
// Decodes a received image to the bitmap
// ---------------------------------------------------------------------------
//
void CAcpImageHandler::StartToDecodeL( const TDesC8& aSourceData, 
    const TDesC8& aContentType )
    {
    ACPLOG( "CAcpImageHandler::StartToDecodeL begin");

    ACPLOG2( " - Data length: %d", aSourceData.Length() );

    if ( aSourceData.Length() > KErrNone )
        {
        // Encoding the image
        iState = EDecoding;

        // Delete decoder
        delete iImageDecoder;
        iImageDecoder = NULL;

        // Create new image decoder.
        iImageDecoder = CImageDecoder::DataNewL( iFs, aSourceData,
            aContentType, CImageDecoder::EAllowGeneratedMask );

        // Recreate bitmap.
    	delete iBitmap;
    	iBitmap = NULL;
        iBitmap = new (ELeave) CFbsBitmap();

        // Delete old mask.
        delete iMask;
        iMask = NULL;
        
        // Check if a mask is present.
        if ( iImageDecoder->FrameInfo().iFlags &
            TFrameInfo::ETransparencyPossible )
            {
            iMask = new (ELeave) CFbsBitmap();
            }
        
        TSize frameinfo = iImageDecoder->FrameInfo().iOverallSizeInPixels;
        TDisplayMode frameDisplayMode = 
            iImageDecoder->FrameInfo().iFrameDisplayMode;
        
        ACPLOG2( " - Converted data length: %d bytes",
            iImageDecoder->FrameInfo().iBitsPerPixel * 
            frameinfo.iWidth * frameinfo.iHeight / KBitsPerByte );

        // Creates a bitmap and mask with the image sizes and display mode.
        iBitmap->Create( frameinfo, frameDisplayMode );
        
        if ( iMask )
            {
            iMask->Create( frameinfo, EGray256 );
            ACPLOG( " - created mask with 8-bit transparency" );

            iImageDecoder->Convert( &iStatus, *iBitmap, *iMask );
            }
        else
            {
            iImageDecoder->Convert( &iStatus, *iBitmap );
            }

        // The active object has requested.
        SetActive();
        }

    ACPLOG( "CAcpImageHandler::StartToDecodeL end" );
    }

// ---------------------------------------------------------------------------
// CAcpImageHandler::GetBitmap
// Reference to already converted bitmap
// ---------------------------------------------------------------------------
//	
CFbsBitmap* CAcpImageHandler::GetBitmap()
    {
    return iBitmap; // Bitmap already converted from given image file.
    }

// ---------------------------------------------------------------------------
// CAcpImageHandler::GetMask
// Reference to already converted mask
// ---------------------------------------------------------------------------
//  
CFbsBitmap* CAcpImageHandler::GetMask()
    {
    return iMask;
    }

// End of file.
