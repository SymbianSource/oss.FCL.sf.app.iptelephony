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
* Description:  For CSC needed AIW handling for service plug-ins
*
*/



#ifndef C_CSCENGSERVICPLUGINHANDLER_H
#define C_CSCENGSERVICPLUGINHANDLER_H

#include <AiwCommon.h>
#include "mcscengecomobserver.h"

const TUint KMaxServiceProviderNameLength = 128;

class CEikonEnv;
class CAiwServiceHandler;
class MAiwNotifyCallback;
class CCSCEngEcomMonitor;
class CAiwGenericParamList;
class CCSCEngServiceHandler;
class MCSCEngProvisioningObserver;

/**
 *  TServicePluginInfo class
 *
 *  For storing service plug-in information
 *
 *  @lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS ( TServicePluginInfo )
    {
    public:
        
        /**
         * Constructor.
         *
         * @since S60 3.2
         */
        TServicePluginInfo()
            : iProviderName( KNullDesC ),
              iPluginsUid( KNullUid ),
              iViewId( KNullUid ),
              iModifiedUi( 1 ),
              iRequiredIap( 0 ),
              iProvisioned( EFalse ),
              iOffered( EFalse )
            {}

    public: // data
        
        /*
         * Service provider name
         */
        TBuf<KMaxServiceProviderNameLength> iProviderName;
        
        /*
         * Service plug-in UID
         */
        TUid iPluginsUid;
        
        /*
         * Service plug-in view id
         */
        TUid iViewId;
        
        /*
         * 0 = plug-in has no UI
         * 1 = plug-in has UI
         * (only to provide backwards compatibility, 
         *  all new plug-ins must provide UI )
         */
        TInt iModifiedUi;
        
        /*
         * 0 = plug-in doesn´t required IAP
         * 1 = plug-in requires IAP
         * (only to provide backwards compatibility, 
         *  all new plug-ins DO NOT require IAP )
         */
        TInt iRequiredIap;
        
        /*
         * ETrue if plug-in is provisioned (configured)
         */
        TBool iProvisioned;
        
        /*
         * ETrue if plug-in is offered for configuration
         */
        TBool iOffered;
    };


/**
 *  CSCEngServicePluginHandler class
 *  Handles Application Interworking (AIW) and service plug-in handling
 *
 *  @lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CCSCEngServicePluginHandler ) : public CBase,
                                                   public MAiwNotifyCallback,
                                                   public MCSCEngEcomObserver
    {
    public: 

        /** State of the plugin */
        enum TPluginState
            {
            EPluginInitialize = 1,
            EPluginProvisioning,
            EPluginModifySettings,
            EPluginRemovation
            };
        
        /** Type of plugin count to be returned */
        enum TPluginCount
            {
            EInitialized = 0,
            EUnprovisioned
            };
        
       // Provisioning plug-in callback service events.
        enum TServicePluginResponse
            {
            EPluginInitialized = KAiwEventStarted,
            EPluginProvisioned = KAiwEventCompleted,
            EPluginModified = KAiwEventStopped,
            EPluginRemoved = KAiwEventQueryExit,
            EPluginError = KAiwEventError
            };
            
        /**
         * Two-phased constructor.
         *
         * @param aObserver reference to service plug-in provisioning observer
         * @param aServiceHandler reference to service handler
         */
        IMPORT_C static CCSCEngServicePluginHandler* NewL(
            CEikonEnv& aEikEnv, 
            MCSCEngProvisioningObserver& aObserver,
            CCSCEngServiceHandler& aServiceHandler );

        
        /**
         * Two-phased constructor.
         *
         * @param aObserver reference to service plug-in provisioning observer
         * @param aServiceHandler reference to service handler
         */
        IMPORT_C static CCSCEngServicePluginHandler* NewLC( 
            CEikonEnv& aEikEnv,
            MCSCEngProvisioningObserver& aObserver,
            CCSCEngServiceHandler& aServiceHandler );

        
        /**
         * Destructor.
         */
        virtual ~CCSCEngServicePluginHandler();
 
 
        /**
         * Initializes CSC supported plugins.
         *
         * @since S60 v3.2
         */
        IMPORT_C void InitializePluginsL();
    
    
        /**
         * Informs plugin of provisioning.
         *
         * @since S60 v3.2
         * @param aPluginUid contains plugins Uid
         * @param aViewUid contains view Uid to be returned
         */
        IMPORT_C void DoProvisioningL( const TUid& aPluginUid,
                                       const TUid& aViewUid );
    
    
        /**
         * Launches plugin provided setting view.
         *
         * @since S60 v3.2
         * @param aPluginUid contains plugins Uid
         * @param aViewUid contains view Uid to be returned
         */
        IMPORT_C void LaunchPluginViewL( const TUid& aPluginUid, 
                                         const TUid& aViewUid );
    
        
        /**
         * Informs plugin from removation.
         *
         * @since S60 v3.2
         * @param aPluginUid contains plugins Uid
         * @param aRemoveSisPckg for invoking application installer
         */
        IMPORT_C void DoRemovationL( const TUid& aPluginUid,
                                     TBool aRemoveSisPckg );
    
                
        /**
         * Returns count of CSC supported plugins in array.
         * 
         * @since S60 v3.2
         * @param aType for items to be returned
         * @return count of profiles 
         */
        IMPORT_C TInt PluginCount( const TPluginCount aType ) const;
        
                
        /**
         * Returns item from service plug-in array
         * 
         * @param aIndex index
         * @since S60 v3.2
         */
        IMPORT_C TServicePluginInfo ItemFromPluginInfoArray( TInt aIndex );
        
        
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


        // from base class MCSCEngEcomObserver
    
        /**
         * Observer interface for notifying ecom events.
         *
         * @since S60 v3.2
         */
        void NotifyEcomEvent();
    
    private:


        CCSCEngServicePluginHandler( 
            CEikonEnv& aEikEnv,
            MCSCEngProvisioningObserver& aObserver,
            CCSCEngServiceHandler& aServiceHandler );

   
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
            TInt& aIndex, 
            TUid& aPluginUid );
        
        
         /**
         * Gets and sets plug-in view id
         *
         * @since S60 v3.2
         * @param aInParamList for params received from plugin
         */ 
        void GetPluginViewId( 
            const CAiwGenericParamList& aInParamList );
        
        /**
         * Set information of plug-ins to plug-in info array
         *
         * @since S60 v3.2
         * @param aInParamList for params received from plugin
         */        
        void SetPluginInitInfoL( const CAiwGenericParamList& aInParamList );
        
        
        /**
         * Invokes application installer to remove .sis package from device.
         *
         * @since S60 v3.2
         * @param aPluginUid contains plugin uid
         */        
        void RemovePluginSisL( const TUid& aPluginUid ) const;
        
        
    private: // data
        
        /**
         * Reference to Eikon environment.
         */
        CEikonEnv& iEikEnv;
        
        /**
         * Observer for AIW commands
         */  
        MCSCEngProvisioningObserver& iObserver;
        
        /*
         * Reference to CCSCEngServiceHandler
         */
        CCSCEngServiceHandler& iServiceHandler;
                        
        /**
         * For AIW Plugin services
         * Own.
         */
        CAiwServiceHandler* iAiwServiceHandler;
        
        /**
         * For monitoring Ecom changes.
         * Own.
         */
        CCSCEngEcomMonitor* iEcomMonitor;
                      
        /**
         * Array for storing information of service plug-ins
         */
        RArray<TServicePluginInfo> iServicePluginInfoArray;
        
        /**
         * Resource file offset.
         */
        TInt iResourceOffset;
        
        /**
         * Flag indicating that should application installer to be run.
         */
        TBool iRunSwinst;
            
#ifdef _DEBUG
    friend class UT_CSCEngServicePluginHandler;
#endif
    };

#endif // C_CSCSERVICPLUGINHANDLER_H
