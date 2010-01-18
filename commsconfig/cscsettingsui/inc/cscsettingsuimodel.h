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
* Description:  Declarition of CSCSettingsUiModel
*
*/

 
#ifndef C_CSCSETTINGSUIMODEL_H
#define C_CSCSETTINGSUIMODEL_H

#include "mcscengcchobserver.h"
#include "mcscengserviceobserver.h"

class CCSCEngCCHHandler;
class CCSCEngServiceHandler;
class CCSCEngBrandingHandler;
class CCSCEngDestinationsHandler;
class MCSCSettingsUiModelObserver;

/**
 *  CCSCSettingsUiModel class
 *  Declarition of CCSCSettingsUiModel.
 *
 *  @lib CSCSettingsUi.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CCSCSettingsUiModel ) : public CBase,
                                           public MCSCEngCCHObserver,
                                           public MCSCEngServiceObserver
    {
    public: 
            
        /**
         * Two-phased constructor.
         * @param aObserver for settings ui observer
         * @param aEikEnv for Eikon environment
         */
        static CCSCSettingsUiModel* NewL(
            MCSCSettingsUiModelObserver& aObserver,
            CEikonEnv& aEikEnv );


        /**
         * Two-phased constructor.
         * @param aObserver for settings ui observer
         * @param aEikEnv for Eikon environment
         */
        static CCSCSettingsUiModel* NewLC(
            MCSCSettingsUiModelObserver& aObserver,
            CEikonEnv& aEikEnv );
        
        
        /**
         * Destructor.
         */
        virtual ~CCSCSettingsUiModel();
    
    
        /**
         * Returns reference to CSC's Service Provider Settings Handler.
         *
         * @since S60 v3.2
         * @return reference to SPS handle
         */
        CCSCEngServiceHandler& SettingsHandler() const;
    
    
        /**
         * Returns reference to CSC's Converged Connection Handler.
         *
         * @since S60 v3.2
         * @return reference to CCH handle
         */
        CCSCEngCCHHandler& CCHHandler() const;
    
    
        /**
         * Returns reference to CSC's Destinations Handler.
         *
         * @since S60 v3.2
         * @return reference to Destinations handle
         */
        CCSCEngDestinationsHandler& DestinationsHandler() const;
            
        
        /**
         * Returns reference to CSC's Branding Server Handler.
         *
         * @since S60 v3.2
         * @return reference to Branding Server handle
         */
        CCSCEngBrandingHandler& BSHandler() const;
    
        
        /**
         * Stores initialization information.
         * Function leaves if given service doesn't exists.
         *
         * @since S60 v3.2
         * @param aViewId for view id to be activated when exiting
         * @param aServiceId for service settings entry id
         */
        void StoreInitializationDataL(
            const TUid& aViewId, 
            TUint aServiceId );
            
        
        /**
         * To notify when softkeys need to be changed.
         *
         * @since S60 v3.2
         */
        void UpdateSoftkeys();
    
    
        /**
         * Returns uid of the view where to be returned.
         *
         * @since S60 v3.2
         * @return uid of the return view
         */
        TUid ReturnViewId() const;
    
    
        /**
         * Returns currently selected service provider entry id.
         *
         * @since S60 v3.2
         * @return id of the currently selected service entry id
         */
        TUint CurrentSPEntryId() const;
        
        
        // from base class MCSCEngCCHObserver

        /**
         * From MCSCEngCCHObserver.
         */
        void ServiceStatusChanged(
            TUint aServiceId, 
            TCCHSubserviceType aType, 
            const TCchServiceStatus& aServiceStatus );       
        
        
        // from base class MCSCEngServiceObserver

        /**
         * From MCSCEngServiceObserver.
         */
        void NotifyServiceChange();
        
        
    private:

        CCSCSettingsUiModel( 
            MCSCSettingsUiModelObserver& aObserver,
            CEikonEnv& aEikEnv );

        void ConstructL();

    private: // data
        
        /**
         * Reference to settings ui model observer.
         */
        MCSCSettingsUiModelObserver& iObserver;
        
        /**
         * Reference to Eikon environment.
         */
        CEikonEnv& iEikEnv;
        
        /**
         * Handle to CSC's Service Provider Settings Handler.
         * Own.
         */
        CCSCEngServiceHandler* iSPSHandler;
        
        /**
         * Handle to CSC's Converged Connection Handler.
         * Own.
         */
        CCSCEngCCHHandler* iCCHHandler;
        
        /**
         * Handle to CSC's Destinations Handler.
         * Own.
         */
        CCSCEngDestinationsHandler* iDestinationsHandler;
        
        /**
         * Handle to CSC's Branding Server Handler.
         * Own.
         */
        CCSCEngBrandingHandler* iBSHandler;
        
        /**
         * Initialization data: Customer application return view id.
         */
        TUid iViewId;

        /**
         * Initialization data: Service Provider Settings Entry id.
         */
        TUint iServiceId;      
    };

#endif // C_CSCSETTINGSUIMODEL_H
