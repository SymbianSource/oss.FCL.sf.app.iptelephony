/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Source file for CAccountCreationPlugin
 *
*/

#include <eikenv.h>
#include <bautils.h>
#include <pathinfo.h>
#include <SWInstApi.h>
#include <SWInstDefs.h>
#include <aknViewAppUi.h>
#include <AknQueryDialog.h> 
#include <StringLoader.h>
#include <accountcreationplugin.rsg>
#include <data_caging_path_literals.hrh>

#include "accountcreationplugin.h"
#include "accountcreationpluginlogger.h"
#include "acpproviderlistview.h"
#include "accountcreationpluginconstants.h"
#include "accountcreationplugin.hrh"

const TUid KAIAppUid = { 0x102750F0 }; // active idle application uid        

// ---------------------------------------------------------------------------
// CAccountCreationPlugin::CAccountCreationPlugin
// ---------------------------------------------------------------------------
//
CAccountCreationPlugin::CAccountCreationPlugin()
    {
    }

// ---------------------------------------------------------------------------
// CAccountCreationPlugin::ConstructL
// ---------------------------------------------------------------------------
//
void CAccountCreationPlugin::ConstructL()
    {
    ACPLOG( "CAccountCreationPlugin::ConstructL begin" );

    // Load resource file to the memory.
    // This when going to ROM (PhoneMemoryRootPath is used only for testing).
    TFileName resourceFile = 
        TParsePtrC( PathInfo::RomRootPath() ).Drive();

    resourceFile.Append( KDC_RESOURCE_FILES_DIR );
    resourceFile.Append( KResourceFilename );
    BaflUtils::NearestLanguageFile( CCoeEnv::Static()->FsSession(), 
        resourceFile );
    iResourceOffset = CCoeEnv::Static()->AddResourceFileL( resourceFile );

    // Create handle to AIW Service Handler.
    iServiceHandler = CAiwServiceHandler::NewL();

    iEikEnv = CEikonEnv::Static();
    
    iCBEventParamList = CAiwGenericParamList::NewL();
    iCBEventParamList->Reset();

    ACPLOG( "CAccountCreationPlugin::ConstructL end" );
    }

// ---------------------------------------------------------------------------
// CAccountCreationPlugin::NewL
// ---------------------------------------------------------------------------
//
CAccountCreationPlugin* CAccountCreationPlugin::NewL()
    {
    CAccountCreationPlugin* self = new ( ELeave ) CAccountCreationPlugin;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccountCreationPlugin::~CAccountCreationPlugin
// ---------------------------------------------------------------------------
//
CAccountCreationPlugin::~CAccountCreationPlugin()
    {
    ACPLOG( "CAccountCreationPlugin::~CAccountCreationPlugin begin" );

    delete iServiceHandler;

    if ( iResourceOffset )
        {
        CCoeEnv::Static()->DeleteResourceFile( iResourceOffset );
        }
 
	iEikEnv = NULL;
	delete iCBEventParamList;

    ACPLOG( "CAccountCreationPlugin::~CAccountCreationPlugin end" );
    }

// ---------------------------------------------------------------------------
// CAccountCreationPlugin::SendInitializedCallbackL
// Sends AIW service command when plugin initialization is completed. 
// ---------------------------------------------------------------------------
//
void CAccountCreationPlugin::SendInitializedCallbackL() const
    {
    ACPLOG( "CAccountCreationPlugin::SendInitializedCallbackL begin" );

    MAiwNotifyCallback* callback = 
        const_cast<MAiwNotifyCallback*>( iNotifyCallback );

    // Uid of ACP.
    TAiwVariant variant( KAccountCreationPluginUid );
    TAiwGenericParam genericParamUid( EGenericParamError, variant );
    CAiwGenericParamList& paramList = iServiceHandler->InParamListL();
    paramList.AppendL( genericParamUid );
    variant.Reset();

    // Localized name of ACP.
    HBufC* localizedName = StringLoader::LoadLC( R_ACP_NAME_IN_CUSTOMER_APP );     
    variant.Set( localizedName->Des() );
    TAiwGenericParam genericParamName( EGenericParamError, variant );
    paramList.AppendL( genericParamName );
    CleanupStack::PopAndDestroy( localizedName );
    variant.Reset();

    // Place of ACP in customer application.
    TInt32 placing( 0 );
    variant.Set( placing );
    TAiwGenericParam genericParamPlace( EGenericParamError, variant );
    paramList.AppendL( genericParamPlace );
	
    // Send callback to the customer application.
    callback->HandleNotifyL( 
        KAiwCmdCSCUiExtensionPlugins,
        KAiwEventStarted,
        *iCBEventParamList,
        paramList );

    ACPLOG( "CAccountCreationPlugin::SendInitializedCallbackL end" );
    }

// ---------------------------------------------------------------------------
// CAccountCreationPlugin::SendCompletedCallbackL
// Sends AIW service command when plugin is stopped its work.
// ---------------------------------------------------------------------------
//
void CAccountCreationPlugin::SendCompletedCallbackL() const
    {
    ACPLOG( "CAccountCreationPlugin::SendCompletedCallbackL begin" );

    MAiwNotifyCallback* callback = 
        const_cast<MAiwNotifyCallback*>( iNotifyCallback );

    // Uid of ACP.
    TAiwVariant variant( KAccountCreationPluginUid );
    TAiwGenericParam genericParamUid( EGenericParamError, variant );
    CAiwGenericParamList& paramList = iServiceHandler->InParamListL();
    paramList.AppendL( genericParamUid );

    // Send callback to the customer application.
    callback->HandleNotifyL( 
        KAiwCmdCSCUiExtensionPlugins,
        KAiwEventStopped,
        *iCBEventParamList,
        paramList );

    ACPLOG( "CAccountCreationPlugin::SendCompletedCallbackL end" );
    }

// ---------------------------------------------------------------------------
// CAccountCreationPlugin::SendErrorCallbackL
// Sends AIW service command when plugin error is reached.
// ---------------------------------------------------------------------------
//
void CAccountCreationPlugin::SendErrorCallbackL() const
    {
    ACPLOG( "CAccountCreationPlugin::SendErrorCallbackL begin" );

    MAiwNotifyCallback* callback = 
        const_cast<MAiwNotifyCallback*>( iNotifyCallback );

    // UID of ACP.
    TAiwVariant variant( KAccountCreationPluginUid );
    TAiwGenericParam genericParamUid( EGenericParamError, variant );
    CAiwGenericParamList& paramList = iServiceHandler->InParamListL();
    paramList.AppendL( genericParamUid );

    // Send callback to the customer application.
    callback->HandleNotifyL( 
        KAiwCmdCSCUiExtensionPlugins,
        KAiwEventError,
        *iCBEventParamList,
        paramList );

    ACPLOG( "CAccountCreationPlugin::SendErrorCallbackL end" );
    }    

// ---------------------------------------------------------------------------
// CAccountCreationPlugin::CompareUids
// Compares AIW service command connected uid to our plugin uid.
// ---------------------------------------------------------------------------
//
TBool CAccountCreationPlugin::CompareUids( 
    const CAiwGenericParamList& aOutParamList ) const
    {
    TInt index = 0;
    TBool commandForUs( EFalse );
    if ( 0 < aOutParamList.Count() )
        {
        // Take out the first parameter which is uid of the target plugin.                   
        const TAiwGenericParam* genericParam = NULL; 
        genericParam = aOutParamList.FindFirst( index,
                                                EGenericParamError,
                                                EVariantTypeTUid );
        if ( genericParam )
            {
            if ( KAccountCreationPluginUid == genericParam->Value().AsTUid() )
                {
                commandForUs = ETrue; // Request is for us.
                }
            }
        }

    ACPLOG2( "CAccountCreationPlugin::CompareUids: forUs=%d", commandForUs );
    return commandForUs;  
    }  

// ---------------------------------------------------------------------------
// CAccountCreationPlugin::LaunchPluginUiL
// Launches ACP UI.
// ---------------------------------------------------------------------------
//
void CAccountCreationPlugin::LaunchPluginUiL()
    {
    ACPLOG( "CAccountCreationPlugin::LaunchPluginUiL begin" );
 
        CAknViewAppUi* ui = 
            static_cast<CAknViewAppUi*> ( static_cast<CAknAppUi*>
                ( iEikEnv->EikAppUi() ) );


    // Create provider list view and activate it.
    if ( NULL == iProviderListView )
        {
            iProviderListView = CAcpProviderListView::NewL( *this,
                                                            iLaunchedFromAI,
															*iEikEnv );
        
        ui->AddViewL( iProviderListView ); // Ownership is transferred.
        }
    
    if ( iLaunchedFromAI )
        {
        TVwsViewId tvwsViewId( KAIAppUid, TUid::Uid( 1 ) );
        ui->ActivateViewL( tvwsViewId );
        }
    
    iProviderListView->DownloadProviderListL();

    ACPLOG( "CAccountCreationPlugin::LaunchPluginUiL end" );
    }

// ---------------------------------------------------------------------------
// CAccountCreationPlugin::HandleServiceCmdL
// From class CAiwServiceIfBase.
// Handles AIW service events from customer application.
// ---------------------------------------------------------------------------
//
void CAccountCreationPlugin::HandleServiceCmdL( 
    const TInt& /*aCmdId*/,
    const CAiwGenericParamList& /*aInParamList*/,
    CAiwGenericParamList& aOutParamList,
    TUint aCmdOptions,
    const MAiwNotifyCallback* aCallback )
    {
    ACPLOG2( 
        "CAccountCreationPlugin::HandleServiceCmdL: cmd=%d", aCmdOptions );
    
      if ( aCallback )
        {
        iNotifyCallback = aCallback;
        }
    
    if ( aCmdOptions == ELaunchedFromAI )
        {
        iLaunchedFromAI = ETrue;
        aCmdOptions = EExecute; 
        }
    
    switch ( aCmdOptions )
        {
        case EInitialize: // Initialization.
            {
            SendInitializedCallbackL();
            break;
            }
        case EExecute: // Execution.
            {
            // Check that service event is meant to us and launch plugin.
            if ( CompareUids( aOutParamList ) )
                {
                LaunchPluginUiL();
                }
            break;
            }
        default: // Not supported.
            {
            User::Leave( KErrNotSupported );
            break;    
            }
        }
    }

// ---------------------------------------------------------------------------
// CAccountCreationPlugin::InitialiseL
// From class MAccountCreationPluginObserver.
// ---------------------------------------------------------------------------
//
void CAccountCreationPlugin::InitialiseL( 
    MAiwNotifyCallback& /*aFrameworkCallback*/,
    const RCriteriaArray& /*aInterest*/ )
    {
    }

// ---------------------------------------------------------------------------
// CAccountCreationPlugin::NotifyAiwEventL
// From class MAccountCreationPluginObserver.
// ---------------------------------------------------------------------------
//
void CAccountCreationPlugin::NotifyAiwEventL( TInt aError )
    {
    ACPLOG2( "CAccountCreationPlugin::NotifyAiwEventL: error=%d", aError );

    // In case of error, send error callback to customer application.
    if ( aError )
        {
        SendErrorCallbackL();
        }
    // If successful event retrieved, send completed callback to customer.    
    else
        {
        SendCompletedCallbackL();
        }    
    }

// ---------------------------------------------------------------------------
// CAccountCreationPlugin::NotifySISDownloaded
// From class MAccountCreationPluginObserver.
// ---------------------------------------------------------------------------
//
void CAccountCreationPlugin::NotifySISDownloaded( TDesC& aFileName )
    {
    ACPLOG( "CAccountCreationPlugin::NotifySISDownloaded IN" );
    
    // Create the installer.
    SwiUI::RSWInstLauncher launcher; 
    SwiUI::TInstallOptions options;
    SwiUI::TInstallOptionsPckg optionsPckg;  

    // Set necessary options.
    options.iUpgrade = SwiUI::EPolicyNotAllowed;
    options.iOCSP = SwiUI::EPolicyNotAllowed;
    options.iUntrusted = SwiUI::EPolicyNotAllowed; 
    options.iCapabilities = SwiUI::EPolicyNotAllowed;               
    optionsPckg = options;  

    TInt err = launcher.Connect();

    // Check whether connected or not.
    if ( err == KErrNone )
        {           
        ACPLOG( "CAccountCreationPlugin::NotifySISDownloaded INSTALL 1" );
        
        // Launch the installer and install requested file on phone.
        launcher.Install( aFileName, optionsPckg );
        
        ACPLOG( "CAccountCreationPlugin::NotifySISDownloaded INSTALL 2" );
        }
    
    TRAP_IGNORE( SendCompletedCallbackL() );
    
    launcher.Close();
    
    ACPLOG( "CAccountCreationPlugin::NotifySISDownloaded OUT" );
    }

// End of file.
