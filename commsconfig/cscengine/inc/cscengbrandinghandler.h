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



#ifndef C_CSCBRANDINGHANDLER_H
#define C_CSCBRANDINGHANDLER_H

#include <AknIconUtils.h>

class MBSAccess;
class CBSFactory;

/**
 * An instance of CCSCEngBrandingHandler
 * For CSC needed Branding Server handling
 *
 * @lib cscengine.lib
 * @since Series 60 3.2
 */
NONSHARABLE_CLASS ( CCSCEngBrandingHandler ) : public CBase, 
                                               public MAknIconFileProvider
    {    
    public:
        
        /**
         * Two-phased constructor.
         */ 
        IMPORT_C static CCSCEngBrandingHandler* NewL();


        /**
         * Destructor.
         */
        IMPORT_C virtual ~CCSCEngBrandingHandler();
        
        
        /**
         * Get branded bitmap for service.
         *
         * @since Series 60 3.2
         * @param aBrandId service brand id
         * @param aBitmap bitmap is stored to this
         * @param aMask mask is stored to this
         * @param aIconSize Size of the icon in pixels
         */
        IMPORT_C void GetServiceBrandIconL( 
           const TDesC8& aBrandId, CFbsBitmap*& aBitmap, CFbsBitmap*& aMask,
           TInt aIconSize );

        // from base class MAknIconFileProvider
            
        /**
         * From MAknIconFileProvider
         * Returns an open file handle to the icon file.
         *
         * @param aFile Icon file should be opened in this file handle, which
         * is an empty file handle, when the AknIcon framework calls this 
         * method. The AknIcon framework takes care of closing the file handle
         * after having used it.
         * @param aType Icon file type.
         * @since 
         */
        void RetrieveIconFileHandleL(
                RFile& aFile, const TIconFileType aType );


        /**
         * From MAknIconFileProvider
         * With this method, AknIcon framework informs that it does not use
         * this MAknIconFileProvider instance any more.  
         *
         * @since
         */
        void Finished(){};

                       
    private:

        CCSCEngBrandingHandler();

        void ConstructL();
        
              
    private:  // data
        
        /**
         * Pointer to CBSFactory
         * Own.
         */
        CBSFactory* iFactory;
        
        /**
         * File.
         */
        RFile iFile;       
        
#ifdef _DEBUG
    friend class UT_CSCEngBrandingHandler;
#endif
    };

#endif // C_CSCBRANDINGHANDLER_H
