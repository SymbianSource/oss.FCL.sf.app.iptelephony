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
* Description:  Implements CSCSettingsUiImpl methods.
*
*/


#include <eikclb.h>
#include <aknview.h>
#include <bautils.h>
#include <aknnavi.h>
#include <pathinfo.h>
#include <akntitle.h>
#include <cchclient.h>
#include <akncontext.h>
#include <aknViewAppUi.h>
#include <xSPViewServices.h>
#include <data_caging_path_literals.hrh>

#include "cscsettingsui.hrh"
#include "cscsettingsuiimpl.h"
#include "cscsettingsuimodel.h"
#include "cscsettingsuilogger.h"
#include "cscsettingsuimainview.h"
#include "cscsettingsuiconstants.h"


#define AppUi() (static_cast<CAknViewAppUi*>(iAvkonAppUi) )

// ======== MEMBER FUNCTIONS ========   
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCSettingsUiImpl::CCSCSettingsUiImpl( CEikonEnv& aEikEnv )
    : iEikEnv( aEikEnv )
    {  
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CCSCSettingsUiImpl::ConstructL()
    {
    CSCSETUIDEBUG( "CCSCSettingsUiImpl::ConstructL - begin" );

    iModel = CCSCSettingsUiModel::NewL( *this, iEikEnv );
    
    // Load resource file to the memory.
    TFileName resourceFile = TParsePtrC( PathInfo::RomRootPath() ).Drive();
    resourceFile.Append( KDC_RESOURCE_FILES_DIR );
    resourceFile.Append( KCSCSettingsUiResourceFile );
    BaflUtils::NearestLanguageFile( iEikEnv.FsSession(), resourceFile );
    iResourceOffset = iEikEnv.AddResourceFileL( resourceFile );
     
    CSCSETUIDEBUG( "CCSCSettingsUiImpl::ConstructL - end" ); 
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCSettingsUiImpl* CCSCSettingsUiImpl::NewL( CEikonEnv& aEikEnv )
    {
    CCSCSettingsUiImpl* self = new (ELeave) CCSCSettingsUiImpl( aEikEnv );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCSettingsUiImpl::~CCSCSettingsUiImpl()
    {
    CSCSETUIDEBUG( "CCSCSettingsUi::~CCSCSettingsUiImpl - begin" );
    
    delete iModel;
    
    // Remove resource file from the memory.
    if ( iResourceOffset )
        {
        iEikEnv.DeleteResourceFile( iResourceOffset );
        }
    
    CSCSETUIDEBUG( "CCSCSettingsUi::~CCSCSettingsUiImpl - end" );
    }


// ---------------------------------------------------------------------------
// CCSCSettingsUiImpl::InitializeL
// ---------------------------------------------------------------------------
//
void CCSCSettingsUiImpl::InitializeL( 
        const TUid& aReturnViewId, 
        TUint aServiceId,
        const MCoeView* aDefaultAppView )
    {
    CSCSETUIDEBUG4( 
      "CCSCSettingsUiImpl::InitializeL, VIEW ID=%d, SERVICE ID=%d, ISDEFAULTVIEW=%d",
      aReturnViewId.iUid, aServiceId, NULL == aDefaultAppView );
    
    if ( NULL == iMainView )
        {
        iMainView = CCSCSettingsUiMainView::NewL( *iModel );
        AppUi()->AddViewL( iMainView ); // ownership is transferred
        }
    
    // Set initialization information data. The function leaves if given 
    // service does not exist.
    iModel->StoreInitializationDataL( aReturnViewId, aServiceId );
    
    ( NULL == aDefaultAppView ) 
        ? AppUi()->SetDefaultViewL( *iMainView )
        : AppUi()->SetDefaultViewL( *aDefaultAppView );
    }


// ---------------------------------------------------------------------------
// CCSCSettingsUiImpl::LaunchSettingsUiL
// Launches Converged Service Configurator Settings UI.
// ---------------------------------------------------------------------------
//
void CCSCSettingsUiImpl::LaunchSettingsUiL()
    {
    CSCSETUIDEBUG( "CCSCSettingsUiImpl::LaunchSettingsUiL - begin" );
    __ASSERT_ALWAYS( NULL != iMainView, User::Leave( KErrNotReady ) );
    
    // Activate main view with given service properties.
    AppUi()->ActivateLocalViewL( KCSCSettingsUiMainViewId );
    
    CSCSETUIDEBUG( "CCSCSettingsUiImpl::LaunchSettingsUiL - end" );
    }


// ---------------------------------------------------------------------------
// From MCSCSettingsUiModelObserver.
// CCSCSettingsUiImpl::UpdateSoftkeys
// ---------------------------------------------------------------------------
//
void CCSCSettingsUiImpl::UpdateSoftkeys() const
    {
    // Pass notification to main view.
    CCSCSettingsUiMainView* mainView = 
        static_cast<CCSCSettingsUiMainView*> 
            ( AppUi()->View( KCSCSettingsUiMainViewId ) );
    
    TVwsViewId activeViewId;
    AppUi()->GetActiveViewId( activeViewId );                
    
    if ( mainView && 
         KCSCSettingsUiMainViewId == activeViewId.iViewUid )
        {
        TRAP_IGNORE( mainView->UpdateSoftkeysL() );
        }
    }
    

// ---------------------------------------------------------------------------
// CCSCSettingsUiImpl::ReferenceCounter
// ---------------------------------------------------------------------------
//
TInt& CCSCSettingsUiImpl::ReferenceCounter()
    {
    CSCSETUIDEBUG( "CCSCSettingsUiImpl::ReferenceCounter - IN"); 
    return iReferenceCounter;
    }
