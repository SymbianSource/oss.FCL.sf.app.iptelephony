/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  cscgsplugin implementation.
*
*/


#include <featmgr.h>
#include <bautils.h>
#include <gscommon.hrh>
#include <bldvariant.hrh>
#include <StringLoader.h>
#include <AknNullService.h>
#include <gsparentplugin.h>
#include <cscgspluginrsc.rsg>
#include <gsprivatepluginproviderids.h>

#include    "cscgsplugin.h"

const TUid KCscGsPluginImplUid  = { 0x1020E568 };  // dll impl uid
const TUid KCscAppUid           = { 0x10275458 };  // from cscappui.mmp

_LIT( KCscApp, "z:\\sys\\bin\\csc.exe" );
_LIT( KCscGsPluginResourceFileName, "cscgspluginrsc.rsc" );

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCscGsPlugin::CCscGsPlugin()
    : iResources( *iCoeEnv ), iNullService(NULL)
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCscGsPlugin::~CCscGsPlugin()
    {
    FeatureManager::UnInitializeLib(); 
    iResources.Close();
    delete iNullService;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CCscGsPlugin::ConstructL()
    {
    FeatureManager::InitializeLibL(); 
    OpenLocalizedResourceFileL( KCscGsPluginResourceFileName, iResources );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCscGsPlugin* CCscGsPlugin::NewL( TAny* /*aInitParams*/ )
    {
    CCscGsPlugin* self = new ( ELeave ) CCscGsPlugin();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// From class CAknView.
// CCscGsPlugin::Id()
// ---------------------------------------------------------------------------
//
TUid CCscGsPlugin::Id() const
    {
    return KCscGsPluginImplUid;
    }

// ---------------------------------------------------------------------------
// From class CAknView.
// CCscGsPlugin::DoActivateL()
// ---------------------------------------------------------------------------
//
void CCscGsPlugin::DoActivateL( 
    const TVwsViewId& /*aPrevViewId*/,
    TUid /*aCustomMessageId*/,
    const TDesC8& /*aCustomMessage*/ )
    {
    }

// ---------------------------------------------------------------------------
// From class CAknView.
// CCscGsPlugin::DoDeactivate()
// ---------------------------------------------------------------------------
//
void CCscGsPlugin::DoDeactivate()
    {
    }

// ---------------------------------------------------------------------------
// From class CGSPluginInterface.
// CCscGsPlugin::GetCaptionL()
// ---------------------------------------------------------------------------
//
void CCscGsPlugin::GetCaptionL( TDes& aCaption ) const
    {
    HBufC* result = StringLoader::LoadL( R_CSCGSPLUGIN_CAPTION );
    
    if ( result->Des().Length() < aCaption.MaxLength() )
        {
        aCaption.Copy( *result );
        }
    else
        {
        aCaption = KNullDesC;
        }
   
    delete result;
    }

// ---------------------------------------------------------------------------
// From class CGSPluginInterface.
// CCscGsPlugin::PluginProviderCategory()
// ---------------------------------------------------------------------------
//
TInt CCscGsPlugin::PluginProviderCategory() const
    {
    return KGSPluginProviderInternal;
    }

// ---------------------------------------------------------------------------
// From class CGSPluginInterface.
// CCscGsPlugin::ItemType()
// ---------------------------------------------------------------------------
//
TGSListboxItemTypes CCscGsPlugin::ItemType()
    {
    return EGSItemTypeSettingDialog;
    }

// ---------------------------------------------------------------------------
// From class CGSPluginInterface.
// CCscGsPlugin::GetValue()
// ---------------------------------------------------------------------------
//
void CCscGsPlugin::GetValue( 
    const TGSPluginValueKeys /*aKey*/,
    TDes& /*aValue*/ )
    {
    }

// ---------------------------------------------------------------------------
// From class CGSPluginInterface.
// CCscGsPlugin::HandleSelection()
// ---------------------------------------------------------------------------
//
void CCscGsPlugin::HandleSelection(
    const TGSSelectionTypes /*aSelectionType*/ )
    {
    TRAP_IGNORE( LaunchCscAppL() );
    }

// ---------------------------------------------------------------------------
// From class CGSPluginInterface.
// CCscGsPlugin::CreateIconL()
// ---------------------------------------------------------------------------
//
CGulIcon* CCscGsPlugin::CreateIconL( const TUid /*aIconType*/ )
    {
    return NULL;
    }

// ---------------------------------------------------------------------------
// From class CGSPluginInterface.
// CCscGsPlugin::Visible()
// ---------------------------------------------------------------------------
//
TBool CCscGsPlugin::Visible() const
    {
    TBool visible( ETrue );
    TBool voipSupported( EFalse );
    
    // Im should be also checked from feature manager
    TBool imSupported( EFalse ); 
    
    voipSupported = FeatureManager::FeatureSupported( 
        KFeatureIdCommonVoip );
    
    // If both voip and im are not supported --> set plugin not visible
    if ( !voipSupported && !imSupported )
        {
        visible = EFalse;
        }
    
    return visible;
    }

// ---------------------------------------------------------------------------
// CCscGsPlugin::OpenLocalizedResourceFile()
// ---------------------------------------------------------------------------
//
void CCscGsPlugin::OpenLocalizedResourceFileL(
    const TDesC& aResourceFileName,
    RConeResourceLoader& aResourceLoader )
    {
    RFs fsSession;
    User::LeaveIfError( fsSession.Connect() );
    CleanupClosePushL(fsSession);

    // Find the resource file:
    TParse parse;
    parse.Set( aResourceFileName, &KDC_RESOURCE_FILES_DIR, NULL );
    TFileName fileName( parse.FullName() );

    // Get language of resource file:
    BaflUtils::NearestLanguageFile( fsSession, fileName );

    // Open resource file:
    aResourceLoader.OpenL( fileName );

    CleanupStack::PopAndDestroy(&fsSession);
    }


// ---------------------------------------------------------------------------
// CCscGsPlugin::LaunchCscAppL()
// ---------------------------------------------------------------------------
//
void CCscGsPlugin::LaunchCscAppL()
    {
    // Get the correct application data
    RWsSession ws;
    CleanupClosePushL( ws );
    User::LeaveIfError( ws.Connect() );

    // Find the task with uid
    TApaTaskList taskList( ws );
    TApaTask task = taskList.FindApp( KCscAppUid );

    if ( task.Exists() )
        {
        //Bring CSC to foreground.
        task.BringToForeground();
        }
    else
        {
        //Launch csc application
        TAppInfo app( KCscAppUid, KCscApp );
        iEmbedded = NULL;
        EmbedAppL( app );
        }
    CleanupStack::PopAndDestroy( &ws );
    }


// ---------------------------------------------------------------------------
// CCscGsPlugin::EmbedAppL()
// ---------------------------------------------------------------------------
//
void CCscGsPlugin::EmbedAppL( const TAppInfo& aApp )
    {
    // Launch settings app
    if ( iNullService )
        {
        delete iNullService;
        iNullService = NULL;
        }
    iNullService = CAknNullService::NewL( aApp.iUid, this );
    }


