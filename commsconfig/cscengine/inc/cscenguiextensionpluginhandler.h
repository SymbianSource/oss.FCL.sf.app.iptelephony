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
* Description:  For CSC needed AIW handling for ui extension plug-ins
*
*/



#ifndef C_CSCENGUIEXTENSIONPLUGINHANDLER_H
#define C_CSCENGUIEXTENSIONPLUGINHANDLER_H

#include <AiwCommon.h>

const TUint KMaxUiExtensionNameLength = 128;

class CEikonEnv;
class MAiwNotifyCallback;
class CAiwServiceHandler;
class CAiwGenericParamList;
class CCSCEngStartupHandler;
class MCSCEngUiExtensionObserver;


/**
*   iUiExtensionName  = UI Extension name
*   iPluginsUid       = Service plug-in UID
*   iPlace            = Place parameter
*/
NONSHARABLE_CLASS( TUiExtensionPluginInfo )
    {
    public:
    
        TUiExtensionPluginInfo()
            : iUiExtensionName( KNullDesC ),
              iPluginsUid( KNullUid ),
              iPlace( 0 )
            {}

    public:
        
        /*
        * UI Extension name
        */       
        TBuf<KMaxUiExtensionNameLength> iUiExtensionName;
        
        /*
        * UI Extension plug-in UID
        */
        TUid iPluginsUid;
        
        /*
        * UI Extension placing information
        * 0 = listbox
        * 1 = options menu
        */
        TInt iPlace;
    };



/**
 *  An instance of CSCUiExtensionPluginHandler
 *  Handles Application Interworking (AIW).
 *
 *  @lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CCSCEngUiExtensionPluginHandler ) : public CBase,
                                                       public MAiwNotifyCallback
    {
    public: 

        /**
         * State of the plugin.
         */
        enum TPluginState
            {
            EPluginInitialize = 1,
            EPluginOpenExtensionView,
            EPluginOpenExtensionViewFromAI
            };
        
        
        // UI extension plug-in callback events.
        enum TUiExtensionPluginResponse
            {
            EPluginInitialized = KAiwEventStarted,
            EPluginExited = KAiwEventStopped,
            EPluginError = KAiwEventError
            };
        
        /**
         * Two-phased constructor.
         */
        IMPORT_C static CCSCEngUiExtensionPluginHandler* NewL( 
            CEikonEnv& aEikEnv, 
            MCSCEngUiExtensionObserver& aObserver,
            CCSCEngStartupHandler& aStartupHandler );


        /**
         * Two-phased constructor.
         */
        IMPORT_C static CCSCEngUiExtensionPluginHandler* NewLC(
            CEikonEnv& aEikEnv, 
            MCSCEngUiExtensionObserver& aObserver,
            CCSCEngStartupHandler& aStartupHandler );


        /**
         * Destructor.
         */
        virtual ~CCSCEngUiExtensionPluginHandler();
 
        
        /**
         * Initializes CSC supported UI Extension plugins.
         *
         * @since S60 v3.2
         */
        IMPORT_C void InitializePluginsL();
    
        
        /**
         * Launches UI Extension plugin view
         *
         * @since S60 v3.2
         * @param aPluginUid contains plugins Uid
         * @param aViewUid contains view Uid to be returned
         * @param aLaunchedFromAI ETrue if launched from active idle
         */
        IMPORT_C void LaunchUiExtensionL( 
            const TUid& aPluginUid, const TUid& aViewUid,
            TBool aLaunchedFromAI = EFalse );
        
        
        /**
         * Returns item from UI Extension plug-in array
         *
         * @since S60 v3.2
         * @param aIndex array index
         */
        IMPORT_C TUiExtensionPluginInfo ItemFromPluginInfoArray( TInt aIndex );
        
        
        /**
         * Returns count of CSC supported ui extension plugins in array.
         * 
         * @since S60 v3.2
         * @param aType for items to be returned
         * @return count of profiles 
         */
        IMPORT_C TInt PluginCount() const;
   
   
        // from base class MAiwNotifyCallBack
 
        /**
         * From MAiwNotifyCallBack.
         * Handles received callbacks from AIW plugin.
         *
         * @since S60 v3.0
         * @param aCmdId for cmd id
         * @param aEventId for event id
         * @param aEventParamList for event param list
         * @param aInParamList for in param list
         */
        TInt HandleNotifyL( TInt aCmdId,
                            TInt aEventId,
                            CAiwGenericParamList& aEventParamList,
                            const CAiwGenericParamList& aInParamList );
     
            
    private:


        CCSCEngUiExtensionPluginHandler( 
            CEikonEnv& aEikEnv,
            MCSCEngUiExtensionObserver& aObserver, 
            CCSCEngStartupHandler& aStartupHandler );

   
        void ConstructL();
        
        
        /**
         * Gets plug-in index in plug-in info array and plug-ins uid
         *
         * @since S60 v3.2
         * @param aInParamList for params received from plugin
         * @param aIndex for plug-in info array index
         * @param aPluginUid for plug-in uid
         */ 
        void GetPluginIndexAndUid( 
            const CAiwGenericParamList& aInParamList,
            TInt aIndex, 
            TUid aPluginUid );
        
        /**
         * Set information of plug-ins to plug-in info array
         *
         * @since S60 v3.2
         * @param aInParamList for params received from plugin
         */        
        void SetPluginInitInfo( 
            const CAiwGenericParamList& aInParamList );
        
        
    private: // data
        
        /**
         * Reference to Eikon environment.
         */
        CEikonEnv& iEikEnv;
        
        /**
         * Observer for AIW commands
         */  
        MCSCEngUiExtensionObserver& iObserver;
        
        /**
         *  Reference to CCSCEngStartupHandler
         */         
        CCSCEngStartupHandler& iStartupHandler;
                 
        /**
         * For AIW Plugin services
         * Own.
         */
        CAiwServiceHandler* iServiceHandler;
        
        /**
         * CSC supported UI Extension plug-in information array
         */ 
        RArray<TUiExtensionPluginInfo> iUiExtensionPluginInfoArray;
        
        /**
         * Resource file offset.
         */
        TInt iResourceOffset;  
        
#ifdef _DEBUG
    friend class UT_CSCEngUiExtensionPluginHandler;
#endif    
    };

#endif // C_CSCENGUIEXTENSIONPLUGINHANDLER_H
