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
* Description:  Declarition of CAcpImageHandler
*
*/


#ifndef ACPIMAGEHANDLER_H
#define ACPIMAGEHANDLER_H

#include <e32std.h>
#include <e32base.h>
#include <f32file.h>
#include <imageconversion.h>

class CFbsBitmap;
class CBitmapRotator;
class MImageHandlerObserver;

/**
 *  CAcpImageHandler class
 *  Declarition of CAcpImageHandler.
 *
 *  @lib accountcreationplugin.lib
 *  @since S60 v3.2
 */
class CAcpImageHandler : public CActive
    {
private:
    /**
     * States of execution.
     */
    enum TState 
        {
        EIdle = 0,
        EDecoding,
        EScaling
        };
public:

    /**
     * Two-phased constructor.
     *
     * @param aController for observer.
     */
    static CAcpImageHandler* NewL( MImageHandlerObserver& aController );

    /**
     * Destructor.
     */
    virtual ~CAcpImageHandler();

    /**
     * Decodes an image to bitmap.
     *
     * @since S60 v3.2
     * @param aFileName A file to be decoded to the bitmap.
     */
    void StartToDecodeL( const TDesC8& aSourceData,
        const TDesC8& aContentType );

    /**
     * Get bitmap.
     *
     * @since S60 v3.2
     * @return Reference to bitmap already converted.
     */
    CFbsBitmap* GetBitmap();

    /**
     * Get mask.
     *
     * @since S60 v3.2
     * @return Reference to the mask from bitmap conversion.
     */
    CFbsBitmap* GetMask();

protected: 

// from base class CActive

    /**
     * From CActive.
     * Cancels an outstanding request.
     * 
     * @since S60 v3.2
     */
    void DoCancel();

    /**
     * From CActive.
     * Handles request completion event.
     * 
     * @since S60 v3.2
     */
    void RunL();

private: 
    CAcpImageHandler( MImageHandlerObserver& aController ); 
    void ConstructL();

private:  

    /**
     * Observer reference.
     */
	MImageHandlerObserver& iController;

    /**
     * Handle for file server.
     */
    RFs iFs;

    /**
     * Handle for image decoder.
     * Own. 
     */
    CImageDecoder* iImageDecoder;

    /**
     * State of execution (decoding or scaling).
     */
    TState iState;

    /**
     * Converted bitmap.
     * Own. 
     */
    CFbsBitmap* iBitmap;
    
    /**
     * Converted mask.
     * Own.
     */
    CFbsBitmap* iMask;
    
    // For unit testing.
#ifdef _DEBUG
    friend class T_CAcpImageHandler;
#endif
    };

#endif // ACPIMAGEHANDLER_H

// End of file.
