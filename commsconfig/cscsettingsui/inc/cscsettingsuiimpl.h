/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declarition of CSCSettingsUiImpl
*
*/



#ifndef C_CSCSETTINGSUIIMPL_H
#define C_CSCSETTINGSUIIMPL_H

#include "mcscsettingsuimodelobserver.h"

class CEikonEnv;
class CCSCSettingsUiModel;
class CCSCSettingsUiMainView;
class MCoeView;


/**
*  CCSCSettingsUiImpl class
*  Declarition of CSCSettingsUiImpl.
*
*  @lib CSCSettingsUi.lib
*  @since S60 v3.2
*/
NONSHARABLE_CLASS( CCSCSettingsUiImpl ) : public CBase,
				                          public MCSCSettingsUiModelObserver
    {
    public:

        /**
         * Two-phased constructor.
         */
        static CCSCSettingsUiImpl* NewL( CEikonEnv& aEikEnv );
        
        
        /**
         * Destructor.
         */
        virtual ~CCSCSettingsUiImpl();
       

        /**
         * Initializes CSC settings UI.
         * @param   aReturnViewId   Identifier of the view where to return with 
         *                          "back" key.
         * @param   aServiceId      Service settings entry identifier.
         * @param   aDefaultAppView The view to set as the default for utilizing
         *                          application. If NULL, settings ui view
         *                          is set as the default view for the 
         *                          application.
         * @since   S60 v5.1
         */
        void InitializeL( 
            const TUid& aReturnViewId, 
            TUint aServiceId,
            const MCoeView* aDefaultAppView );
        
        
        /**
         * Launches Converged Service Configurator Settings UI with parameters
         * given at initialization. 
         * 
         * @since    S60 v3.2
         */
        void LaunchSettingsUiL();

        
        /**
         * From MCSCSettingsUiModelObserver.
         */
        void UpdateSoftkeys() const;
        
        
        /**
         * Returns reference counter.
         *
         * @since S60 5.0
         */
        TInt& ReferenceCounter();

        
    private:

        CCSCSettingsUiImpl(
            CEikonEnv& aEikEnv );

        void ConstructL();
            
    private: // data
                
        /**
         * Reference to Eikon environment.
         */
        CEikonEnv& iEikEnv;
        
        /**
         * Resource file offset.
         */
        TInt iResourceOffset;
        
        /**
         * Main view.
         * Not own.
         */
        CCSCSettingsUiMainView* iMainView;
        
        /**
         * Model
         * Own.
         */
        CCSCSettingsUiModel* iModel;
        
        /**
         * Reference counter ie. how many instances created
         */    
        TInt iReferenceCounter;
        
#ifdef _DEBUG
    friend class UT_cscsettingsui;
#endif        
        
    };

#endif // C_CSCSETTINGSUIIMPL_H
