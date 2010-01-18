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
* Description:  Provides access to branded data.
*
*/


#ifndef C_CCHUIPLUGINBRANDINGHANDLER_H
#define C_CCHUIPLUGINBRANDINGHANDLER_H

#include <e32base.h>

#include "cchuicommon.hrh"

class MBSAccess;
class CFbsBitmap;
class CBSFactory;
class CSPSettings;


/**
 *  Branding Server handler.
 *  This class handles to use of branding server. It can be used
 *  to retrieve icons or complete files by brand id.
 *
 *  @code
 *   CCchUiPluginBrandingHandler* handler = 
 *      CCchUiPluginBrandingHandler::NewLC( aSpSettings );
 *   CFbsBitmap* bitmap = NULL;
 *   CFbsBitmap* mask = NULL;
 *   handler->RetrieveServiceIconL( aServiceId, *bitmap, *mask );
 *   // Handle bitmap, mask here. You still own those.
 *   CleanupStack::PopAndDestroy( handler ); 
 *  @endcode
 *
 *  @lib cchuinotif.lib
 *  @since S60 5.0
 */
NONSHARABLE_CLASS( CCchUiPluginBrandingHandler ): public CBase
    {
public:
    
    /**
     * Two-phased constructor.
     * 
     * @since S60 5.0
     * @param aServiceId Service id.
     */
    static CCchUiPluginBrandingHandler* NewL( 
        CSPSettings& aSpSettings );
    
    /**
     * Two-phased constructor.
     * 
     * @since S60 5.0
     * @param aServiceId Service id.
     */
    static CCchUiPluginBrandingHandler* NewLC( 
        CSPSettings& aSpSettings );

    /**
     * Destructor.
     */
    virtual ~CCchUiPluginBrandingHandler();
    
    /**
     * Retrieves specific branded service icon from branding server.
     *
     * @since S60 5.0
     * @param aServiceId service id
     * @param aBitmap stores retrieved icon
     * @param aMask stores retrieved icon mask
     */
    void RetrieveServiceIconL(
        TUint32 aServiceId,
        CFbsBitmap*& aBitmap, 
        CFbsBitmap*& aMask );

private:

    CCchUiPluginBrandingHandler( CSPSettings& aSpSettings );
    
    void ConstructL();
    
    
    /**
     * Prepares branding access.
     *
     * @since S60 5.0
     * @param aServiceId Service id.
     */
    void PrepareBrandingAccessL( TUint32 aServiceId );

    /**
     * Releases branding access.
     *
     * @since S60 5.0
     */
    void ReleaseBrandingAccess();
    

private: // data

    /**
     * Reference to service provider settings.
     * Not own.
     */
    CSPSettings& iSpSettings;
    
    /**
     * Branding server factory.
     * Own.
     */
    CBSFactory* iFactory;
    
    /**
     * Branding server access.
     * Own.
     */
    MBSAccess* iAccess;
    
    CCHUI_UNIT_TEST( T_CchUiPluginBrandingHandler )
    };

#endif // C_CCHUIPLUGINBRANDINGHANDLER_H
