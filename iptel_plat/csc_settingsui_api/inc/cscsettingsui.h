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
* Description:  Declarition of CSCSettingsUi API.
*
*/


#ifndef C_CSCSETTINGSUI_H
#define C_CSCSETTINGSUI_H

#include <e32def.h>
#include <e32base.h>

class CEikonEnv;
class CCSCSettingsUiImpl;
class MCoeView;

/**
 *  CSCSettingsUi class
 *  Implements CSCSettingsUi API methods.
 *
 *  @lib CSCSettingsUi.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CCSCSettingsUi ) : public CBase
    {
    public:
        
        /**
         * Two-phased constructor.
         */
        IMPORT_C static CCSCSettingsUi* NewL( CEikonEnv& aEikEnv );
 
        
        /**
         * Destructor.
         */
        virtual ~CCSCSettingsUi();
       
   
        /**
         * Initializes Converged Service Configurator Settings UI. 
         *  
         * @param   aReturnViewId   Identifier of the view where to return with 
         *                          "back" key.
         * @param   aServiceId      Service settings identifier.
         * @param   aDefaultAppView The view to set as the default for client
         *                          application. If NULL, settings ui view
         *                          is set as the default view for the 
         *                          application.
         * @since   S60 v5.1
         */
        IMPORT_C void InitializeL( 
            const TUid& aReturnViewId, 
            TUint aServiceId,
            const MCoeView* aDefaultAppView );
        
        /**
         * Launches Converged Service Configurator Settings UI with parameters
         * given at initialization. 
         * 
         * @since   S60 v3.2
         * @pre     Initialization is done at least once.
         */
        IMPORT_C void LaunchSettingsUiL() const;
        
    private:

        CCSCSettingsUi();

        void ConstructL( CEikonEnv& aEikEnv );
            
    private: // Data
    
        /**
         * Implementation of the CSC Settings UI.
         * Own.
         */
        CCSCSettingsUiImpl* iImpl;

#ifdef _DEBUG
    friend class UT_cscsettingsui;
#endif        
    };

#endif  // C_CSCSETTINGSUI_H

