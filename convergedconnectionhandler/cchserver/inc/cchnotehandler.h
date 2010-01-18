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
* Description:  CCH note handler.
*
*/

#ifndef CCHNOTEHANDLER_H
#define CCHNOTEHANDLER_H

// INCLUDES
#include <e32base.h>
#include <badesca.h>
#include <AknGlobalMsgQuery.h>
#include <centralrepository.h>

#include "cchclientserverinternal.h"

// CLASS DECLARATION
class CCCHServerBase;

/**
*  Global message query information
*
*  @since Series60 5.0
*/
class TGlobalMsgQueryInfo
    {
    public:
        // Message text resource Id
        TInt iResourceId;
        
        // Softkey Id
        TInt iSoftkeyId;
        
        // Secondary display index
        TInt iSecondaryDisplayIndex;
    };

/**
*  Handles cch global note showing.
*
*  @since Series60 5.0
*/
class CCchNoteHandler: public CActive
    {
    
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        *
        * @param aCchCenRep cch central repository.
        * @param aCoverDisplaySupported ETrue if cover display supported.
        * @return new instance.
        */
        static CCchNoteHandler* NewL( CCCHServerBase& aServer );
            
        /**
        * Destructor.
        */
        virtual ~CCchNoteHandler();
       
    public: // new function
    
       /**
        * Lauches Emergency note.
        * @since Series60 5.0
        * 
        * @param aCchGlobalNoteType cch global note type.
        * @param aSoftKeyConfig softkey configuration.
        * @param aType type of confirmation query.
        * @param aSecondaryDisplayIndex secondary display index.
        */
        void LaunchGlobalNoteL(
            const TInt aResourceID, 
            const TInt aSoftKeyConfig,
            const TInt aSecondaryDisplayIndex );

        /**
         * detects if query is being displayed
         */
        TBool CanBeDestroyed();
            
        /**
        * Return CCH Central Repository reference
        */
        CRepository* CchCenRep();
                    
    private: //from base class
    
        /**
        * @see CActive.
        */
        void RunL();

        /**
        * @see CActive.
        */
        void DoCancel();
        
        /**
        * @see CActive.
        */
        TInt RunError( TInt aError );
                    
    private:

        /**
        * C++ default constructor.
        */
        CCchNoteHandler( CCCHServerBase& aServer );

        /**
        * 2nd phase constructor
        */
        void ConstructL();
   
        /**
        * Deletes all temporary instances.
        */
        void DeleteAllL();

        /**
        * Load notification resource
        */
        HBufC* LoadResourceL( TInt aResourceID );
        
        /**
        * Scan the file location
        */
        HBufC* ScanFileL( const TDesC& aFileName, 
                        const TDesC& aFilePath );
                        
        /**
        * 
        */
        void DoLaunchGlobalNoteL( const TInt aResourceId, 
                const TInt aSoftKeyConfig,
                const TInt aSecondaryDisplayIndex );                        

    private:    // Data  
                        
        // Akn global message query 
        CAknGlobalMsgQuery*          iGlobalMsgQuery;
        
        // central repository, not own but can be deleted
        CRepository*      	         iCchCenRep; 
        
        // Handle to server
        CCCHServerBase&             iServer;

        // do we have secondary display?
        TBool                        iCoverDisplaySupported;
        
        // resource file path + name
        HBufC*                       iResourceFile;
        
        // Current resource Id
        TInt                         iResourceId;
        
        // should I commit suicide
        TBool                        iDie;       
        
        // File session
        RFs                          iFsSession;
        
        // Message query information array
        RArray<TGlobalMsgQueryInfo> iMsgQueryInfoArray;
    };

#endif      // CCHNOTEHANDLER_H  
            
// End of File
