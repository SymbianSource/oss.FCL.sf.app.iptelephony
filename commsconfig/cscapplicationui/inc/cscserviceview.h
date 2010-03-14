/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CSC Applications Service View
*
*/


#ifndef C_CSCSERVICEVIEW_H
#define C_CSCSERVICEVIEW_H

#include <aknview.h>
#include "mcscservicecontainerobserver.h"
#include "mcscengtimerobserver.h"

class CCSCAppUi;
class CCSCDialog;
class CCSCEngCCHHandler;
class CCSCServiceContainer;
class CCSCEngStartupHandler;
class CCSCEngServiceHandler;
class CCSCEngBrandingHandler;
class CCSCEngServicePluginHandler;
class CCSCEngUiExtensionPluginHandler;
class CCSCSettingsUi;

/**
 *  CCSCServiceView class
 *  An instance of CCSCServiceView is the Application View object
 *  for the CSC application
 *
 *  @lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CCSCServiceView ) : public CAknView,
                                       public MEikListBoxObserver,
                                       public MCSCServiceContainerObserver,
                                       public MCSCEngTimerObserver
    {
    public:

        /**
         * Two-phased constructor.
         *
         * @param aServicePluginHandler reference to service plug-in handler
         * @param aUiExtensionPluginHandler reference to ui extension 
         *        plug-in handler
         * @param aStartupHandler reference to startup handler
         * @param aServiceHandler reference to service handler
         * @param aBrandingHandler reference to branding handler
         * @param aCCHHandler reference to cch handler
         */
        static CCSCServiceView* NewL( 
            CCSCEngServicePluginHandler& aServicePluginHandler,
            CCSCEngUiExtensionPluginHandler& aUiExtensionPluginHandler,
            CCSCEngStartupHandler& aStartupHandler,
            CCSCEngServiceHandler& aServiceHandler,
            CCSCEngBrandingHandler& aBrandingHandler,
            CCSCEngCCHHandler& aCCHHandler );


        /**
         * Two-phased constructor.
         *
         * @param aServicePluginHandler reference to service plug-in handler
         * @param aUiExtensionPluginHandler reference to ui extension 
         *        plug-in handler
         * @param aStartupHandler reference to startup handler
         * @param aServiceHandler reference to service handler
         * @param aBrandingHandler reference to branding handler
         * @param aCCHHandler reference to cch handler
         */
        static CCSCServiceView* NewLC( 
            CCSCEngServicePluginHandler& aServicePluginHandler,
            CCSCEngUiExtensionPluginHandler& aUiExtensionPluginHandler,
            CCSCEngStartupHandler& aStartupHandler,
            CCSCEngServiceHandler& aServiceHandler,
            CCSCEngBrandingHandler& aBrandingHandler,
            CCSCEngCCHHandler& aCCHHandler );

        
        /**
         * Destructor.
         */
        virtual ~CCSCServiceView();
        
        /**
         * Initializes view with startup parameters.
         * 
         * @since S60 v5.1
         */
        void InitializeWithStartupParametersL();
        
        
        // from base class CAknView
        
        /**
         * From CAknView.
         * Handles the commands. If the command is command which is require to
         * display outline-screen, the command display outline-screen
         * corresponded to required.
         *
         * @since S60 v3.0
         * @param aCommand Command to be handled.
         */
        void HandleCommandL( TInt aCommand );
        
        
        /**
         * From CAknView.
         *
         * @since S60 v3.0
         * @return The ID of view.
         */
        TUid Id() const;
        
        
        /**
         * From CAknView.
         *
         * @since S60 v3.0
         * @param aForeground
         */
        void HandleForegroundEventL( TBool aForeground );
        
         
        /**
         * For handling ui extension plug-in exits
         *
         * @since S60 v3.2
         */  
        void HandleUiExtensionExitL();
        
        
        /**
         * For updating service view
         *
         * @since S60 v3.2
         */  
        void UpdateServiceViewL();
        
        
        /**
         * For deleting service
         *
         * @since S60 v3.2
         * @param aServiceId
         */  
        void DeleteServiceL( TUint aServiceId );
        
        
        /**
         * Service configuration failed.
         *
         * @since S60 v3.2
         * @param aIndex index of plugin
         */ 
        void ServiceConfiguringFailedL( TInt aIndex );
        
        
        /**
         * Hides dialog wait note
         *
         * @since S60 v3.2
         * @param aIndex index of plugin
         */ 
        void HideDialogWaitNote();
        
                
        /**
         * For handling service configuration
         *
         * @since S60 v3.2
         * @param aUid for service setup plugin uid
         * @return ETrue if configuration canceled by user
         */  
        TBool HandleServiceConfigurationL( TUid aUid  );
        
        
        /**
         * Executes startup actions based on startup parameters.
         * 
         * @since   S60 v5.1
         * @param   aForeGroundEvent    ETrue if initiated by foreground event.
         * @param   aLaunchedFromAi     ETrue if launched from active idle.
         */
        void ExecuteStartupActionsL( 
            TBool aForeGroundEvent = EFalse,
            TBool aLaunchedFromAi = EFalse );
        
        
        // from base class MCSCServiceContainerObserver
        
        /**
         * Update softkey
         *
         * @since S60 v3.2
         */         
        void UpdateCbaL();
        
		
    protected:
              
        // from base class MEIkListBoxObserver
        
        /**
         * From MEikListBoxObserver.
         * For handling list box events.
         *
         * @since S60 v3.0
         * @param aListBox for listbox
         * @param aEventType for event type
         */
        void HandleListBoxEventL( CEikListBox* aListBox, 
                                  TListBoxEvent aEventType );    
                                  
		
    private:
        
        
        CCSCServiceView( 
            CCSCEngServicePluginHandler& aServicePluginHandler,
            CCSCEngUiExtensionPluginHandler& aUiExtensionPluginHandler,
            CCSCEngStartupHandler& aStartupHandler,
            CCSCEngServiceHandler& aServiceHandler,
            CCSCEngBrandingHandler& aBrandingHandler,
            CCSCEngCCHHandler& aCCHHandler );
        
  
        void ConstructL();
        
        
        /**
         * For handling listbox itemp opening.
         *
         * @since S60 v3.2
         */  
        void HandleListboxItemOpenL();
        
        
        /**
         * For changing text in the title pane.
         *
         * @since S60 v3.2
         */         
        void SetTitlePaneTextL() const;
                 
                
        // from base class CAknView
        
        /**
         * From CAknView.
         * Dynamically initializes the contents of the menu list.
         *
         * @since S60 v3.0
         * @param aResourceId Initiallized resource
         * @param aMenuPane Instance to MenuPanel
         */
        void DynInitMenuPaneL( TInt aResourceId,
                               CEikMenuPane* aMenuPane );
        
        
        /**
         * From CAknView.
         * Creates the Container class object.
         *
         * @since S60 v3.0
         * @param aPrevViewId is not used.
         * @param aCustomMessageId is not used.
         * @param aCustomMessage is not used.
         */
        void DoActivateL( const TVwsViewId& aPrevViewId,
                          TUid aCustomMessageId,
                          const TDesC8& aCustomMessage );


        /**
        * From CAknView.
        * Deletes the Container class object.
        *
        * @since S60 v3.0
        */
        void DoDeactivate();
        
        
        // from base class MCSCEngTimerObserver
        
        /**
         * From MCSCEngTimerObserver.
         *
         * @since S60 v5.0
         */
        void TimerExpired();
         
		
    private: // data
                           
        /**
         *  Reference to CCSCServicePluginHandler
         */          
        CCSCEngServicePluginHandler& iServicePluginHandler;
        
        /**
         *  Reference to CCSCUiExtensionPluginHandler
         */         
        CCSCEngUiExtensionPluginHandler& iUiExtensionPluginHandler;
                
        /**
         *  Reference to CCSCEngStartupHandler
         */         
        CCSCEngStartupHandler& iStartupHandler;
        
        /**
         *  Reference to CCSCEngServiceHandler
         */         
        CCSCEngServiceHandler& iServiceHandler;
        
        /**
         *  Reference to CCSCEngBrandingHandler
         */
        CCSCEngBrandingHandler& iBrandingHandler;
        
        /**
         *  Reference to CCSCEngCCHHandler
         */
        CCSCEngCCHHandler& iCCHHandler;
                
        /**
         * Pointer to service container
         * Own.
         */       
        CCSCServiceContainer* iContainer;
        
        /**
         * Pointer to CSC Settings UI
         * Own.
         */ 
        CCSCSettingsUi* iSettingsUi; 
        
        /**
         *  Flag indicating if DoActivateL is called first time (startup)
         */ 
        TBool iStartup;
                                                        
        /**
         * Current list item index in container
         */
        TInt iCurrentIndex;
        
        /**
         * Array for already offered to configure plugin uids
         */
        RArray<TUid> iOfferedPluginUids;
        
        /**
         * Pointer to timer
         * Own.
         */
        CCSCEngTimer* iEngTimer;
        
        /**
         * Plugin info.
         */
        TServicePluginInfo iPluginInfo;
        
        /**
         * Next plugin index.
         */
        TUint iNextPluginIndex;
		
        /**
         * Uid.
         */
        TUid iUid;
        
#ifdef _DEBUG
    friend class UT_CSC;
#endif
        
    };

#endif // C_CSCSERVICEVIEW_H
