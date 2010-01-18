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
* Description:  Implements CSCSettingsUiModel methods
*
*/


#include <eikenv.h>
#include <cchclient.h>
#include <cmmanagerext.h>
#include <AknIconUtils.h>
#include <mspnotifychangeobserver.h>

#include "cscsettingsui.hrh"
#include "cscengcchhandler.h"
#include "cscsettingsuimodel.h"
#include "cscsettingsuilogger.h"
#include "cscengservicehandler.h"
#include "cscengbrandinghandler.h"
#include "cscsettingsuiconstants.h"
#include "cscengdestinationshandler.h"
#include "mcscsettingsuimodelobserver.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCSettingsUiModel::CCSCSettingsUiModel( 
    MCSCSettingsUiModelObserver& aObserver,
    CEikonEnv& aEikEnv )
    : iObserver( aObserver ),
    iEikEnv( aEikEnv )
    {
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CCSCSettingsUiModel::ConstructL()
    {
    CSCSETUIDEBUG( "CCSCSettingsUiModel::ConstructL - begin" );
    
    // Create handle to CSC's Service Provider Settings Handler.
    iSPSHandler = CCSCEngServiceHandler::NewL( this );
    
    // Create handle to CSC's Converged Connection Handler.
    iCCHHandler = CCSCEngCCHHandler::NewL( *this );
    
    // Create handle to CSC's Destinations Handler.
    iDestinationsHandler = CCSCEngDestinationsHandler::NewL();
    
    // Create handle to CSC's Branding Server Handler.
    iBSHandler = CCSCEngBrandingHandler::NewL();
    
    CSCSETUIDEBUG( "CCSCSettingsUiModel::ConstructL - end" );
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCSettingsUiModel* CCSCSettingsUiModel::NewL( 
    MCSCSettingsUiModelObserver& aObserver,
    CEikonEnv& aEikEnv )
    {
    CCSCSettingsUiModel* self = 
        CCSCSettingsUiModel::NewLC( aObserver, aEikEnv );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCSettingsUiModel* CCSCSettingsUiModel::NewLC(
    MCSCSettingsUiModelObserver& aObserver,
    CEikonEnv& aEikEnv )
    {
    CCSCSettingsUiModel* self = 
        new ( ELeave ) CCSCSettingsUiModel( aObserver, aEikEnv );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCSettingsUiModel::~CCSCSettingsUiModel()
    {
    CSCSETUIDEBUG( "CCSCSettingsUiModel::~CCSCSettingsUiModel - begin" );
    
    delete iBSHandler;
    delete iSPSHandler;
    delete iCCHHandler;
    delete iDestinationsHandler;
    
    CSCSETUIDEBUG( "CCSCSettingsUiModel::~CCSCSettingsUiModel - end" );
    }


// ---------------------------------------------------------------------------
// CCSCSettingsUiModel::SettingsHandler
// Returns reference to CSC's Service Provider Settings Handler.
// ---------------------------------------------------------------------------
//
CCSCEngServiceHandler& CCSCSettingsUiModel::SettingsHandler() const
    {
    return *iSPSHandler;
    }


// ---------------------------------------------------------------------------
// CCSCSettingsUiModel::CCHHandler
// Returns reference to CSC's Converged Connection Handler.
// ---------------------------------------------------------------------------
//
CCSCEngCCHHandler& CCSCSettingsUiModel::CCHHandler() const
    {
    return *iCCHHandler;
    }


// ---------------------------------------------------------------------------
// CCSCSettingsUiModel::DestinationsHandler
// Returns reference to CSC's Destinations Handler.
// ---------------------------------------------------------------------------
//
CCSCEngDestinationsHandler& CCSCSettingsUiModel::DestinationsHandler() const
    {
    return *iDestinationsHandler;
    }

// ---------------------------------------------------------------------------
// CCSCSettingsUiModel::BSHandler
// Returns reference to CSC's Branding Server Handler.
// ---------------------------------------------------------------------------
//
CCSCEngBrandingHandler& CCSCSettingsUiModel::BSHandler() const
    {
    return *iBSHandler;
    }

// ---------------------------------------------------------------------------
// CCSCSettingsUiModel::StoreInitializationDataL
// Stores initialization information.
// ---------------------------------------------------------------------------
//
void CCSCSettingsUiModel::StoreInitializationDataL(
    const TUid& aViewId,
    TUint aServiceId )
    {
    CSCSETUIDEBUG( "CCSCSettingsUiModel::StoreInitializationDataL - begin" );
    
    // View id for customer application return view id.
    iViewId = aViewId;
    
    // Check that service exists in service table.
    RArray<TUint> spEntryIds;
    CleanupClosePushL( spEntryIds );
    iSPSHandler->GetAllServiceIdsL( spEntryIds );
    User::LeaveIfError( spEntryIds.Find( aServiceId ) );
    CleanupStack::PopAndDestroy( &spEntryIds );
    iServiceId = aServiceId;
    
    CSCSETUIDEBUG( "CCSCSettingsUiModel::StoreInitializationDataL - end" );
    }

// ---------------------------------------------------------------------------
// CCSCSettingsUiModel::UpdateSoftkeys
// To notify when softkeys need to be changed.
// ---------------------------------------------------------------------------
//
void CCSCSettingsUiModel::UpdateSoftkeys()
    {
    CSCSETUIDEBUG( "CCSCSettingsUiModel::UpdateSoftkeys - begin" );
   
    iObserver.UpdateSoftkeys();
    
    CSCSETUIDEBUG( "CCSCSettingsUiModel::UpdateSoftkeys - end" );
    }


// ---------------------------------------------------------------------------
// CCSCSettingsUiModel::ReturnViewId
// Returns uid of the view where to be returned.
// ---------------------------------------------------------------------------
//
TUid CCSCSettingsUiModel::ReturnViewId() const
    {
    return iViewId;
    }


// ---------------------------------------------------------------------------
// CCSCSettingsUiModel::CurrentSPEntryId
// Returns currently selected service provider entry id.
// ---------------------------------------------------------------------------
//
TUint CCSCSettingsUiModel::CurrentSPEntryId() const
    {
    return iServiceId;
    }

// ---------------------------------------------------------------------------
// From MCSCEngCCHObserver.
// CCSCSettingsUiModel::ServiceStatusChanged
// ---------------------------------------------------------------------------
//
void CCSCSettingsUiModel::ServiceStatusChanged(
    TUint /*aServiceId*/, 
    TCCHSubserviceType /*aType*/, 
    const TCchServiceStatus& /*aServiceStatus*/ )
    {
    // not used
    }


// ---------------------------------------------------------------------------
// From MCSCEngServiceObserver.
// CCSCSettingsUiModel::NotifyServiceChange
// ---------------------------------------------------------------------------
//
void CCSCSettingsUiModel::NotifyServiceChange()
    {
    // not used
    }
    
