/*
* Copyright (c) 2003-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CCchFeatureManager implementation.
 *
*/


// INCLUDES

#include <e32svr.h> // RDebug
#include <featmgr.h>
#include "cchfeaturemanager.h"
#include <centralrepository.h>
#include "cchprivatecrkeys.h"
#include "cchlogger.h"
#include <settingsinternalcrkeys.h>
#include <CoreApplicationUIsSDKCRKeys.h>

// ============================ MEMBER FUNCTIONS ==============================

// ---------------------------------------------------------------------------
// CCchFeatureManager::CCchFeatureManager
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CCchFeatureManager::CCchFeatureManager()
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// CCchFeatureManager::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CCchFeatureManager::ConstructL()
    {
    CCHLOGSTRING( "CCchFeatureManager::ConstructL IN" );
    
        // Create Central Repository instance for Offline Mode
    iOfflineRepository = CRepository::NewL( KCRUidCoreApplicationUIs );
    
    TBool iDynamicVoIPSupported( EFalse );
    FeatureManager::InitializeLibL();
    
    iCoverDisplaySupported = FeatureManager::FeatureSupported( 
        KFeatureIdCoverDisplay );
    CCHLOGSTRING2( 
        "CCchFeatureManager::ConstructL: Cover display supported=%d", 
        iCoverDisplaySupported );
    
    iVoIPSupported = FeatureManager::FeatureSupported( KFeatureIdCommonVoip );
    if( iVoIPSupported )
        {
        CRepository* rep = CRepository::NewL( KCRUidTelephonySettings );
        if( rep->Get( KDynamicVoIP, iDynamicVoIPSupported ) == KErrNone )
            {
            iVoIPSupported &= iDynamicVoIPSupported;
            }
        
        delete rep;
        }
        
#ifdef _DEBUG
    iVoIPSupported = ETrue;
#endif
    CCHLOGSTRING2( "CCchFeatureManager::ConstructL: VoIP supported=%d", 
            iVoIPSupported );
    FeatureManager::UnInitializeLib(); 
    
    CCHLOGSTRING( "CCchFeatureManager::ConstructL OUT" );
    }
    
// ---------------------------------------------------------------------------
// CCchFeatureManager::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CCchFeatureManager* CCchFeatureManager::NewL()
    {
    CCchFeatureManager* self = new (ELeave) CCchFeatureManager;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// Destructor
CCchFeatureManager::~CCchFeatureManager()
    {
    delete iOfflineRepository;
    iOfflineRepository = NULL;
    }
    
// ----------------------------------------------------------------------------
// CCchFeatureManager::VoIPSupported()
// ----------------------------------------------------------------------------
//
TBool CCchFeatureManager::VoIPSupported() const
    {
    return iVoIPSupported;
    }

// ----------------------------------------------------------------------------
// CCchFeatureManager::OfflineMode()
// ----------------------------------------------------------------------------
//
TBool CCchFeatureManager::OfflineMode() const
    {
    CCHLOGSTRING( "CCchFeatureManager::OfflineMode IN" );

    TInt offline( 0 );
    iOfflineRepository->Get( KCoreAppUIsNetworkConnectionAllowed, offline );
    CCHLOGSTRING2( "CCchFeatureManager::OfflineMode Offline: %d", offline );
    
    return ECoreAppUIsNetworkConnectionNotAllowed == offline;
    }
    
// ----------------------------------------------------------------------------
// CCchFeatureManager::CoverDisplaySupported()
// ----------------------------------------------------------------------------
//
TBool CCchFeatureManager::CoverDisplaySupported() const
    {
    return iCoverDisplaySupported;
    }
    

// End of File



