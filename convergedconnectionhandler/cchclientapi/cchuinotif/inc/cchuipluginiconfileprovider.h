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
* Description:  Implements icon file provider for branding usage.
*
*/



#ifndef C_CCHUIPLUGINICONFILEPROVIDER_H
#define C_CCHUIPLUGINICONFILEPROVIDER_H

#include <e32base.h>
#include <AknsItemID.h>
#include <AknIconUtils.h>


/**
 *  Icon File Provider.
 *  Provides functionality to get icon file handle. Required to use
 *  when handling icon data fetched from branding server. Lifetime of
 *  the file handles must be maintained by using this provider. Otherwise
 *  usage of icon data (even in framework) might lead to a crash.
 *
 *  @code
 *   CCchUiPluginIconFileProvider* fp = 
 *      CCchUiPluginIconFileProvider::NewL( aFile );
 *   CleanupStack::PushL( fp );
 *
 *   AknIconUtils::CreateIconLC( aBitmap, aMask,
 *                              *ifp,
 *                               bsBitmap.BitmapId(), 
 *                               bsBitmap.BitmapMaskId() ); 
 *   CleanupStack::Pop( fp ); // framework frees via handle
 *
 *  @endcode
 *
 *  @lib cchuinotif.lib
 *  @since S60 5.0
 */
NONSHARABLE_CLASS( CCchUiPluginIconFileProvider ): 
    public CBase, public MAknIconFileProvider
    {
public:
    
    /**
     * Two-phased constructor.
     * 
     * @since S60 5.0
     * @param aFile File handle.
     */
    static CCchUiPluginIconFileProvider* NewL( RFile& aFile );
    
    /**
     * Destructor.
     */
    virtual ~CCchUiPluginIconFileProvider();
    

// from base class MAknIconFileProvider
    
    /**
     * From MAknIconFileProvider.
     * Returns an open file handle to the icon file.
     *
     * @param aFile Icon file should be opened in this file handle, which
     * is an empty file handle, when the AknIcon framework calls this 
     * method. The AknIcon framework takes care of closing the file handle
     * after having used it.
     * @param aType Icon file type.
     */
    void RetrieveIconFileHandleL( RFile& aFile, const TIconFileType aType );
    
    /**
     * From MAknIconFileProvider.
     * With this method, AknIcon framework informs that it does not use
     * this MAknIconFileProvider instance any more.  
     */
    void Finished();

private:

    CCchUiPluginIconFileProvider( RFile& aFile );

    
private: // data

    /**
     * Reference to file handle.
     * Not own.
     */
    RFile& iFile;
    };

#endif // C_CCHUIPLUGINICONFILEPROVIDER_H
