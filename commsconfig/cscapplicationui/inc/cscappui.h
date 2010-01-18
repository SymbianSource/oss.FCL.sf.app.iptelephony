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
* Description:  CSC Applications AppUi
*
*/


#ifndef C_CSCAPPUI_H
#define C_CSCAPPUI_H

#include <aknViewAppUi.h>

#include "mcscengcchobserver.h"
#include "mcscengserviceobserver.h"
#include "mcscengconnectionobserver.h"
#include "mcscenguiextensionobserver.h"
#include "mcscengprovisioningobserver.h"

class CEikAppUi;
class CAknViewAppUi;
class CCSCServiceView;
class CCSCEngCCHHandler;
class CCSCEngStartupHandler;
class CCSCEngServiceHandler;
class CCSCEngBrandingHandler;
class CCSCEngConnectionHandler;
class CCSCEngServicePluginHandler;
class CCSCEngUiExtensionPluginHandler;

// View ID's for CSC views.
const TUid KCSCServiceViewId = {947873};

 
/**
 *  An instance of class CCSCAppUi.
 *  User Interface part of AVKON application framework for CSC application
 *
 *  @lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CCSCAppUi ) : public CAknViewAppUi,  
                                 public MCSCEngProvisioningObserver,
                                 public MCSCEngUiExtensionObserver,
                                 public MCSCEngConnectionObserver,
                                 public MCSCEngServiceObserver,
                                 public MCSCEngCCHObserver
    {
    public:
    
        void ConstructL();

        CCSCAppUi();
    
        
        /**
         * Destructor.
         */
        virtual ~CCSCAppUi();
        
        
        // from base class CEikAppUi
                
        /**
         * From CEikAppUi 
         * 
         *
         * @since
         * @param aCommand Defines the command codes used to indicate the way
         * an application is to be launched. Not used in CSC.
         * @param aDocumentName A buffer that can contain the name of a file.
         * Used in CSC to pass startup parameters instead of file name.
         * @param aTail 
         */
        TBool ProcessCommandParametersL(
            TApaCommand aCommand,
            TFileName& aDocumentName,
            const TDesC8& aTail);
        
        
        /**
         * From CEikAppUi 
         * 
         *
         * @since
         * @param aUid
         * @param aParams startup parameters.
         */
        void ProcessMessageL( TUid aUid, const TDesC8 &aParams );

        // from base class CAknViewAppUi
    
        /**
         * From CAknViewAppUi 
         * Handles user menu selections.
         *
         * @since S60 v3.0
         * @param aCommand The enumerated code for the option selected
         */
        void HandleCommandL( TInt aCommand );
        
        
        /**
         * From CAknViewAppUi 
         * Handling changing of the skin and layout.
         *
         * @since S60 v3.0
         * @param aType for type of the change
         */
        void HandleResourceChangeL( TInt aType );
                
        
        // from base class MCSCProvisioningObserver
        
        /**
         * From MCSCProvisioningObserver
         * Notifies when configuring of service plug-in is done.
         *
         * @since S60 v3.2
         * @param aResponse plug-in response type
         * @param aIndex index
         * @param aPluginUid plugins uid
         */
        void NotifyServicePluginResponse( 
            const CCSCEngServicePluginHandler::TServicePluginResponse& aResponse, 
            const TInt aIndex, 
            const TUid& aPluginUid );
        
        
         // from base class MCSCUiExtensionObserver
        
        /**
         * From MCSCUiExtensionObserver
         * Notifies when ui extension plug-in is exited
         *
         * @since S60 v3.2
         * @param aResponse plug-in response type
         * @param aIndex index
         * @param aPluginUid plugins uid
         */
        void NotifyUiExtensionPluginResponse(
            const CCSCEngUiExtensionPluginHandler::TUiExtensionPluginResponse&
            aResponse, 
            const TInt aIndex, 
            const TUid& aPluginUid );
        
         
        // from base class MCSCEngServiceObserver
        
         /**
         * Called when change in service(s) occur
         *
         * @since S60 v3.2
         */
        void NotifyServiceChange();
        
        
        // from base class MCSCEngCCHObserver

        /**
         * From MCSCEngCCHObserver.
         */
        void ServiceStatusChanged(
            TUint aServiceId, 
            TCCHSubserviceType aType, 
            const TCchServiceStatus& aServiceStatus );
        
        
        // from base class MCSCEngConnectionObserver
    
        /**
         * From MCSCEngConnectionObserver.
         */
        void NotifyConnectionEvent( 
            CCSCEngConnectionHandler::TConnectionEvent aConnectionEvent );
        
        
    private:
            
        /*
         * For checking if SNAP is still in use.
         *
         * @since S60 3.2
         * @param aServiceId service id
         */
        void SnapCheckL( TUint aServiceId ) const;
            
    private: // data
                       
        /** 
         * For CSC Application startup parameter handling
         * Own.
         */
        CCSCEngStartupHandler* iStartupHandler;
    
        /** 
         * For CSC Application service handling
         * Own.
         */
        CCSCEngServiceHandler* iServiceHandler;
        
        /** 
         * For CSC Application branding handling
         * Own.
         */
        CCSCEngBrandingHandler* iBrandingHandler;
        
        /** 
         * For CSC Application CCH handling
         * Own.
         */
        CCSCEngCCHHandler* iCCHHandler;
        
        /** 
         * For CSC Application RConnection monitoring
         * Own.
         */
        CCSCEngConnectionHandler* iConnectionHandler;
                    
        /**
         * Pointer to CCSCServiceView
         * Not own.
         */
        CCSCServiceView* iServiceView;
        
        /**
         * Service which is monitored when disabling and removing service
         */
        TUint iMonitoredService;

#ifdef _DEBUG
    friend class UT_CSC;
#endif
             
    };

#endif // C_CSCAPPUI_H
