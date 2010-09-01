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
* Description:  For CSC application startup parameter handling
*
*/


#ifndef C_CSCENGSTARTUPHANDLER_H
#define C_CSCENGSTARTUPHANDLER_H


/**
 *  An instance of CSCEngStartupHandler
 *  For CSC application startup parameter handling
 *
 *  @lib CSCEngine.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS ( CCSCEngStartupHandler ) : public CBase
    {
    public:

        /**  Enumeration for different startup actions */
        enum TAction
            {
            EDefaultAction = 0,
            EAddSetupPluginService, // for service setup plugins
            ERemoveSetupPluginService,  
            EOpenUiExtension, // for ui extensions plugins
            EOpenSettingsUi
            };
        
        
        /**  Enumeration for different uid types */
        enum TUidType
            {
            EPluginUid = 0,
            EAppUid    
            };


        /**
         * Two-phased constructor.
         */
        IMPORT_C static CCSCEngStartupHandler* NewL();

 
        /**
         * Destructor.
         */
        virtual ~CCSCEngStartupHandler();
        
                
        /**
         * Sets CSC startup parameters.
         *
         * @since S60 v3.2
         * @param aParameters string containing startup parameters
         * @return KErrNone if successful
         */
        IMPORT_C TInt SetStartupParametersL( const TDesC& aParameters );
        
        
        /**
         * Gets startup parameter Action
         *
         * @since S60 v3.2
         * @return action enum
         */
        IMPORT_C TAction GetParamAction() const;
        
        /**
         * Gets startup parameter service id
         *
         * @since S60 v3.2
         * @return service id
         */
        IMPORT_C TUint GetParamServiceId() const;
        
        
        /**
         * Gets startup parameter UI Extension Plug-in UID
         *
         * @since S60 v3.2
         * @return plugin uid
         */
        IMPORT_C const TUid GetParamPluginUid() const;
        
        
        /**
         * Gets startup parameter application UID
         *
         * @since S60 v3.2
         * @return application uid
         */
        IMPORT_C const TUid GetParamAppUid() const;
        
        
        /**
         * Gets startup parameter URL. 
         *
         * @since S60 v3.2
         * @param aUrl url is stored here
         * @return error code
         */
        IMPORT_C TInt GetParamUrl( TDes& aUrl ) const;
        
        
        /**
         * Resets uid value by uid type
         *
         * @since S60 v3.2
         * @param aType plugin type
         */
        IMPORT_C void ResetUid( TUidType aType );
        
        
        /**
         * Resets service id
         *
         * @since S60 v3.2
         */
        IMPORT_C void ResetServiceId();
        
        /**
         * For settings flag if csc was started from homescreen.
         *
         * @since S60 v5.0
         */
        IMPORT_C void SetStartedFromHomescreen( TBool aStartedFromHomescreen );
                
        /**
         * For getting flag if csc was started from homescreen.
         *
         * @since S60 v5.0
         */
        IMPORT_C TBool StartedFromHomescreen() const;
        
                        
    private:

        CCSCEngStartupHandler();


        void ConstructL();
        
        
        /**
         * Parses and sets startup parameter action
         *
         * @since S60 v3.2
         * @param aParameters string containing startup parameters
         */
        void SetAction( const TDesC& aParameters );
        
        
        /**
         * Parses and sets startup parameter service id
         *
         * @since S60 v3.2
         * @param aParameters string containing startup parameters
         */
        void SetServiceId( const TDesC& aParameters );
        
        
        /**
         * Parses and sets startup parameter plugin uid
         *
         * @since S60 v3.2
         * @param aParameters string containing startup parameters
         * @param aUidType uid type
         */
        void SetUid( const TDesC& aParameters, TUidType aUidType );
        
                
        /**
         * Parses and sets startup parameter URL
         *
         * @since S60 v3.2
         * @param aParameters string containing startup parameters
         */
        void SetUrlL( const TDesC& aParameters );
        
        
    private: // data
    
        /**
         * Action type parameter
         */
        TAction iAction;
        
        /**
         * Service id parameter
         */
   		TUint iServiceId;
    
        /**
         * Plugin uid parameter
         */
   		TUid iPluginUid;
   		
   		/**
         * Application uid parameter
         */
   		TUid iAppUid;
   		
   		/**
         * URL parameter
         * Own.
         */
   		HBufC* iURL;       
   		
        /**
         * To deternmine if csc was started from homescreen.
         * Own.
         */
        TBool iStartedFromHomescreen;
    };

#endif  // C_CSCENGSTARTUPHANDLER_H
