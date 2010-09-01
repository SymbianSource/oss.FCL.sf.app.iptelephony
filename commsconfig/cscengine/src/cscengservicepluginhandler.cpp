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
* Description:  For CSC needed AIW handling for service plug-ins
*
*/


#include <eikenv.h>
#include <bautils.h>
#include <pathinfo.h>
#include <ecom/ecom.h>
#include <SWInstApi.h>
#include <SWInstDefs.h>
#include <AiwCommon.hrh>
#include <cscengine.rsg>
#include <AiwServiceHandler.h>
#include <mspnotifychangeobserver.h>
#include <data_caging_path_literals.hrh>

#include "cscenglogger.h"
#include "cscengecommonitor.h"
#include "cscengservicehandler.h"
#include "cscengservicepluginhandler.h"
#include "mcscengprovisioningobserver.h"

// Resource file location.
_LIT( KCSCEngineResourceFile, "cscengine.rsc" ); // file

const TInt KMandatoryParamCount = 3;


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCEngServicePluginHandler::CCSCEngServicePluginHandler(
    CEikonEnv& aEikEnv,
    MCSCEngProvisioningObserver& aObserver,
    CCSCEngServiceHandler& aServiceHandler ) :
    iEikEnv( aEikEnv ),
    iObserver( aObserver ),
    iServiceHandler( aServiceHandler )
    {
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CCSCEngServicePluginHandler::ConstructL()
    {
    CSCENGDEBUG( "CCSCEngServicePluginHandler::ConstructL - begin" );

    // Attach to AIW Framework.
    iAiwServiceHandler = CAiwServiceHandler::NewL();  
    
    // Load resource file to the memory.    
    TFileName resourceFile = TParsePtrC( PathInfo::RomRootPath() ).Drive();
    resourceFile.Append( KDC_RESOURCE_FILES_DIR );
    resourceFile.Append( KCSCEngineResourceFile );
    BaflUtils::NearestLanguageFile( iEikEnv.FsSession(), resourceFile );
    iResourceOffset = iEikEnv.AddResourceFileL( resourceFile );
        
    CSCENGDEBUG( "CCSCEngServicePluginHandler::ConstructL - end" );
    }
    
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CCSCEngServicePluginHandler* CCSCEngServicePluginHandler::NewL(
    CEikonEnv& aEikEnv,
    MCSCEngProvisioningObserver& aObserver,
    CCSCEngServiceHandler& aServiceHandler )
    {
    CCSCEngServicePluginHandler* self =
        CCSCEngServicePluginHandler::NewLC( 
            aEikEnv, aObserver, aServiceHandler );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CCSCEngServicePluginHandler* CCSCEngServicePluginHandler::NewLC(
    CEikonEnv& aEikEnv,
    MCSCEngProvisioningObserver& aObserver,
    CCSCEngServiceHandler& aServiceHandler )
    {
    CCSCEngServicePluginHandler* self =
        new ( ELeave ) CCSCEngServicePluginHandler( 
            aEikEnv, aObserver, aServiceHandler );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCEngServicePluginHandler::~CCSCEngServicePluginHandler()
    {
    CSCENGDEBUG( 
    	"CCSCEngServicePluginHandler::~CCSCEngServicePluginHandler - begin" );
    
    iEikEnv.DeleteResourceFile( iResourceOffset );
    iServicePluginInfoArray.Reset();
    iServicePluginInfoArray.Close();      
    delete iEcomMonitor;
    delete iAiwServiceHandler;
    
    CSCENGDEBUG( 
    	"CCSCEngServicePluginHandler::~CCSCEngServicePluginHandler - end" );
    }


// ---------------------------------------------------------------------------
// Initializes CSC supported plugins.
// ---------------------------------------------------------------------------
//
EXPORT_C void CCSCEngServicePluginHandler::InitializePluginsL()
    {
    CSCENGDEBUG( "CCSCEngServicePluginHandler::InitializePluginsL - begin" );

    // Reset service plugin info array.
    iServicePluginInfoArray.Reset();

    // Attach to CSC supported plugins and execute initalization.
    iAiwServiceHandler->AttachL( R_CSCENG_INTEREST_SERVICE_PLUGIN );
    iAiwServiceHandler->ExecuteServiceCmdL( 
            KAiwCmdCSCServicePlugins,
            iAiwServiceHandler->InParamListL(),
            iAiwServiceHandler->OutParamListL(),
            EPluginInitialize,
            this );
    
    // Start monitoring service setup plugins.
    if ( !iEcomMonitor )
        {
        iEcomMonitor = CCSCEngEcomMonitor::NewL( *this );
        }
        
    CSCENGDEBUG( "CCSCEngServicePluginHandler::InitializePluginsL - end" );
    }

// ---------------------------------------------------------------------------
// Informs plugin of provisioning
// ---------------------------------------------------------------------------
//
EXPORT_C void CCSCEngServicePluginHandler::DoProvisioningL(
    const TUid& aPluginUid, const TUid& aViewUid )
    {
    CSCENGDEBUG( "CCSCEngServicePluginHandler::DoProvisioningL - begin" );

    // Set plugins Uid to generic param data.
    CAiwGenericParamList& paramList = iAiwServiceHandler->OutParamListL();
    TAiwVariant variant( aPluginUid );
    TAiwGenericParam genericParamUid( EGenericParamError, variant );
    paramList.AppendL( genericParamUid );

    // Set view id to be returned to generic param data.
    variant.Reset();
    variant.Set( aViewUid );
    TAiwGenericParam genericParamViewId( EGenericParamError, variant );
    paramList.AppendL( genericParamViewId );

    // Set bogus iap id to maintain backwards compatibility with older service
    // plugins
    TUint32 bogusIap( KErrNone );

    variant.Reset();
    variant.Set( bogusIap );
    TAiwGenericParam genericParamIap( EGenericParamError, variant );
    paramList.AppendL( genericParamIap );

    // Execute service handler command.
    iAiwServiceHandler->ExecuteServiceCmdL( KAiwCmdCSCServicePlugins,
                                         iAiwServiceHandler->InParamListL(),
                                         paramList,
                                         EPluginProvisioning,
                                         this );

    CSCENGDEBUG( "CCSCEngServicePluginHandler::DoProvisioningL - end" );
    }


// ---------------------------------------------------------------------------
// Launches plugin provided setting view.
// ---------------------------------------------------------------------------
//
EXPORT_C void CCSCEngServicePluginHandler::LaunchPluginViewL(
    const TUid& aPluginUid, const TUid& aViewUid )
    {
    CSCENGDEBUG( "CCSCEngServicePluginHandler::LaunchPluginViewL - begin" );

    // Set plugins Uid to generic param data.
    CAiwGenericParamList& paramList = iAiwServiceHandler->OutParamListL();
    TAiwVariant variant( aPluginUid );
    TAiwGenericParam genericParamUid( EGenericParamError, variant );
    paramList.AppendL( genericParamUid );

    // Set view id to be returned to generic param data.
    variant.Reset();
    variant.Set( aViewUid );
    TAiwGenericParam genericParamViewId( EGenericParamError, variant );
    paramList.AppendL( genericParamViewId );

    // Execute service handler command.
    iAiwServiceHandler->ExecuteServiceCmdL( KAiwCmdCSCServicePlugins,
                                         iAiwServiceHandler->InParamListL(),
                                         paramList,
                                         EPluginModifySettings,
                                         this );

    CSCENGDEBUG( "CCSCEngServicePluginHandler::LaunchPluginViewL - end" );
    }


// ---------------------------------------------------------------------------
// Informs plugin from removation.
// ---------------------------------------------------------------------------
//
EXPORT_C void CCSCEngServicePluginHandler::DoRemovationL(
    const TUid& aPluginUid, TBool aDeleteSisPckg )
    {
    CSCENGDEBUG( "CCSCEngServicePluginHandler::DoRemovationL - begin" );

    // Set flag for application installer launching.
    iRunSwinst = aDeleteSisPckg;

    // Set plugins Uid to generic param data.
    CAiwGenericParamList& paramList = iAiwServiceHandler->OutParamListL();
    TAiwVariant variant( aPluginUid );
    TAiwGenericParam genericParamUid( EGenericParamError, variant );
    paramList.AppendL( genericParamUid );

    // Execute service handler command.
    iAiwServiceHandler->ExecuteServiceCmdL( KAiwCmdCSCServicePlugins,
                                         iAiwServiceHandler->InParamListL(),
                                         paramList,
                                         EPluginRemovation,
                                         this );

    CSCENGDEBUG( "CCSCEngServicePluginHandler::DoRemovationL - end" );
    }


// ---------------------------------------------------------------------------
// Returns plugin counts from PluginsInfoArray.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCSCEngServicePluginHandler::PluginCount( 
    const TPluginCount aType ) const
    {
    TInt count( 0 );

    for ( TInt i = 0; i < iServicePluginInfoArray.Count(); i++ )
        {
        switch ( aType )
            {
            // CSC supported and initialized plugin count.
            case EInitialized:
                count++;
                break;
            // CSC supported and unprovisioned plugin count.
            case EUnprovisioned:
                if ( !iServicePluginInfoArray[ i ].iProvisioned )
                    {
                    count++;
                    }
                break;
            default:
                break;
            }
        }

    CSCENGDEBUG2( "CCSCEngServicePluginHandler::PluginCount: %d", count );

    return count;
    }


// ---------------------------------------------------------------------------
// Return item from serviceplugininfoarray.
// ---------------------------------------------------------------------------
//
EXPORT_C TServicePluginInfo 
    CCSCEngServicePluginHandler::ItemFromPluginInfoArray( TInt aIndex )
    {
    return iServicePluginInfoArray[ aIndex ];
    }


// ---------------------------------------------------------------------------
// From class MAiwNotifyCallBack
// Handles received callbacks from AIW plugin.
// ---------------------------------------------------------------------------
//
TInt CCSCEngServicePluginHandler::HandleNotifyL(
    TInt /*aCmdId*/,
    TInt aEventId,
    CAiwGenericParamList& /*aEventParamList*/,
    const CAiwGenericParamList& aInParamList )
    {
    CSCENGDEBUG( "CCSCEngServicePluginHandler::HandleNotifyL - begin");

    TInt err( KErrNone );

    switch ( aEventId )
        {
        // ===================================================================
        // Every supported CSC plugin sends callback event when it has
        // been initialized. Generic parameter include 3 different values.
        // Initialization parameters are described in CSC Service Plugin
        // interface description document:
        // "Example Operator" <- Name shown to user
        // "00"               <- First value 0 or 1 provides information
        //                       whether Plugin has modifiable UI or not. This
        //                       information is needed when provisioning
        //                       control is given to Plugin.
        //                    <- Second value presents information whether
        //                       Plugin requires Active IAP in order to
        //                       process provisioning.
        // ===================================================================
        case KAiwEventStarted:
            {
            CSCENGDEBUG(
            "CCSCEngServicePluginHandler::HandleNotifyL - KAiwEventStarted" );

            // Parse initialization data and append data to the infoarray.
            SetPluginInitInfoL( aInParamList );
            
            TInt index( 0 );
            TUid pluginUid( KNullUid );
            
            GetPluginIndexAndUid( aInParamList, index, pluginUid );
            
            iObserver.NotifyServicePluginResponse( 
                EPluginInitialized, index, pluginUid );
            break;
            }

        // ===================================================================
        // Target plugins sends callback note when provisioning is finished.
        // Generic parameter include 1 value.
        // Initialization parameters are described in CSC Service Plugin
        // interface description document:
        // "TUid"             <- Plugins uid value [TUid]
        // ===================================================================
        case KAiwEventCompleted:
            {            
            TInt index( 0 );
            TUid pluginUid( KNullUid );
            
            GetPluginIndexAndUid( aInParamList, index, pluginUid );
            
            if ( index < iServicePluginInfoArray.Count() )
                {
                iServicePluginInfoArray[ index ].iProvisioned = ETrue;
                
                iObserver.NotifyServicePluginResponse( 
                    EPluginProvisioned, index, pluginUid );
                }
            
            CSCENGDEBUG(
            "CCSCEngServicePluginHandler::HandleNotifyL KAiwEventCompleted" );
            break;
            }

        // ===================================================================
        // Target plugins sends callback note when plugins modified ui is
        // ready. Generic parameter include 1 value.
        // Initialization parameters are described in CSC Service Plugin
        // interface description document:
        // "TUid"             <- Plugins uid value [TUid]
        // ===================================================================
        case KAiwEventStopped:
            {
            TInt index( 0 );
            TUid pluginUid( KNullUid );
            
            GetPluginIndexAndUid( aInParamList, index, pluginUid );
            
            iObserver.NotifyServicePluginResponse( 
               EPluginModified, index, pluginUid );
                 
            CSCENGDEBUG(
              "CCSCEngServicePluginHandler::HandleNotifyL KAiwEventStopped" );
            break;
            }

        // ===================================================================
        // Target plugins sends callback note when plugins setting removation
        // process is ready. Generic parameter include 1 value.
        // Initialization parameters are described in CSC Service Plugin
        // interface description document:
        // "TUid"             <- Plugins uid value [TUid]
        // ===================================================================
        case KAiwEventQueryExit:
            { 
            TInt index( 0 );
            TUid pluginUid( KNullUid );
            
            GetPluginIndexAndUid( aInParamList, index, pluginUid );
            GetPluginViewId( aInParamList );
            
            if ( iRunSwinst )
                {
                // Try to remove .sis file from the device.
                TRAP_IGNORE( RemovePluginSisL( pluginUid ) );
                }
                
            iObserver.NotifyServicePluginResponse( 
                EPluginRemoved, index, pluginUid );
            
            CSCENGDEBUG(
            "CCSCEngServicePluginHandler::HandleNotifyL KAiwEventQueryExit" );   
            break;
            }

        // ===================================================================
        // Target plugins sends callback note if plugins provisioning failed.
        // Generic parameter include 1 value.
        // Initialization parameters are described in CSC Service Plugin
        // interface description document:
        // "TUid"             <- Plugins uid value [TUid]
        // ===================================================================
        case KAiwEventError:
            {
            TInt index( 0 );
            TUid pluginUid( KNullUid );
            
            GetPluginIndexAndUid( aInParamList, index, pluginUid );
            
            iObserver.NotifyServicePluginResponse( 
               EPluginError, index, pluginUid );

            CSCENGDEBUG(
                "CCSCEngServicePluginHandler::HandleNotifyL KAiwEventError" );
            break;
            }
        default:
            CSCENGDEBUG(
                "CCSCEngServicePluginHandler::HandleNotifyL: !!DEFAULT!!" );
            break;
        }

    CSCENGDEBUG( "CCSCEngServicePluginHandler::HandleNotifyL - end" );

    return err;
    }


// ---------------------------------------------------------------------------
// From class MCSCEngEcomObserver
// Observer interface for notifying ecom events.
// ---------------------------------------------------------------------------
//
void CCSCEngServicePluginHandler::NotifyEcomEvent()
    {
    // Re-initialize service setup plugins.
    TRAP_IGNORE( InitializePluginsL() );
    }


// ---------------------------------------------------------------------------
// For Getting plug-ins index in plug-in info array and plug-ins uid.
// ---------------------------------------------------------------------------
//
void CCSCEngServicePluginHandler::GetPluginIndexAndUid( 
    const CAiwGenericParamList& aInParamList,
    TInt& aIndex, 
    TUid& aPluginUid )
    {
    const TAiwGenericParam* genericParam = NULL;

    genericParam = aInParamList.FindFirst( aIndex,
                                           EGenericParamError,
                                           EVariantTypeTUid );

    aPluginUid = genericParam->Value().AsTUid();

    for ( TInt i = 0; i < iServicePluginInfoArray.Count(); i++ )
        {
        if ( aPluginUid == iServicePluginInfoArray[ i ].iPluginsUid )
            {
            aIndex = i;
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// For Getting and setting plug-ins view id.
// ---------------------------------------------------------------------------
//
void CCSCEngServicePluginHandler::GetPluginViewId( 
    const CAiwGenericParamList& aInParamList )
    {
    const TAiwGenericParam* genericParam = NULL;
    TUid pluginUid( KNullUid );
    TUid viewId( KNullUid );
    TInt index( 0 );
    
    // First is plug-in uid, not needed now
    genericParam = aInParamList.FindFirst( index,
                                           EGenericParamError,
                                           EVariantTypeTUid );
    
    pluginUid = genericParam->Value().AsTUid();                              
                                           
    // Find next which is view id
    genericParam = aInParamList.FindNext( index,
                                          EGenericParamError,
                                          EVariantTypeTUid );
    
    viewId = genericParam->Value().AsTUid();

    for ( TInt i = 0; i < iServicePluginInfoArray.Count(); i++ )
        {
        if ( pluginUid == iServicePluginInfoArray[ i ].iPluginsUid )
            {
            iServicePluginInfoArray[ i ].iViewId = viewId;
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// For parsing and settings plug-in initialization info
// ---------------------------------------------------------------------------
//
void CCSCEngServicePluginHandler::SetPluginInitInfoL(
    const CAiwGenericParamList& aInParamList )
    {
    CSCENGDEBUG( "CCSCEngServicePluginHandler::SetPluginInitInfoL - begin" );

    if ( KMandatoryParamCount <= aInParamList.Count() )
        {
        TServicePluginInfo pluginInfo;
        TInt index = 0;
        
        const TAiwGenericParam* genericParam = NULL;

        // First generic value is plugin Uid.
        genericParam = aInParamList.FindFirst( index,
                                               EGenericParamError,
                                               EVariantTypeTUid );

        pluginInfo.iPluginsUid = genericParam->Value().AsTUid();

        // Second generic value is plugin name.
        genericParam = aInParamList.FindNext( index,
                                              EGenericParamError,
                                              EVariantTypeDesC );

        pluginInfo.iProviderName.Copy( genericParam->Value().AsDes() );

        CSCENGDEBUG2(
         "CCSCEngServicePluginHandler::SetPluginInitInfoL: PROVIDER NAME: %S",
             &pluginInfo.iProviderName );

        // Third generic value is modified Ui and needed Iap.
        genericParam = aInParamList.FindNext( index,
                                              EGenericParamError,
                                              EVariantTypeDesC8 );

        TPtrC8 data = genericParam->Value().AsData();

        TInt hasUi = 0;
        TLex8 lexerUi( data.Left( 1 ) );
        lexerUi.Val( hasUi );

        pluginInfo.iModifiedUi = hasUi;

        CSCENGDEBUG2(
           "CCSCEngServicePluginHandler::SetPluginInitInfoL  Ui: %d", hasUi );

        TInt reqIap = 0;
        TLex8 lexerIap( data.Right( 1 ) );
        lexerIap.Val( reqIap );

        pluginInfo.iRequiredIap = reqIap;

        CSCENGDEBUG2(
         "CCSCEngServicePluginHandler::SetPluginInitInfoL  Iap: %d", reqIap );
        
        
        // Check if plug-in is already provisioned
        pluginInfo.iProvisioned = EFalse;
        RArray<TUint> serviceIds;
        CleanupClosePushL( serviceIds );
        
        TRAPD( err, iServiceHandler.GetAllServiceIdsL( serviceIds ) );
         
        for ( TInt i( 0 ) ; i < serviceIds.Count() && !err ; i++ )
            {
            TInt32 pluginUid( 0 );
            TInt err2( KErrNone );
            
            TRAP( err2, pluginUid =
                iServiceHandler.ServiceSetupPluginIdL( serviceIds[ i ] ) );
                
            
            CSCENGDEBUG2(
            "CCSCEngServicePluginHandler::SetPluginInitInfoL ERR=%d", err2 );
            
                        
            if ( !err2 && ( pluginInfo.iPluginsUid.iUid == pluginUid ) )
                {
                pluginInfo.iProvisioned = ETrue;
                }
            }
        
        CleanupStack::PopAndDestroy( &serviceIds );

        // Check if the plugin is already in array before appending.
        TBool found( EFalse );
        for ( TInt counter( 0 ); 
            counter < iServicePluginInfoArray.Count();
            counter++ )
            {
            if ( iServicePluginInfoArray[counter].iPluginsUid == 
                pluginInfo.iPluginsUid )
                {
                found = ETrue;
                }
            }
        if ( !found )
            {
            iServicePluginInfoArray.Append( pluginInfo );
            }
        }
        
    CSCENGDEBUG( "CCSCEngServicePluginHandler::SetPluginInitInfoL - end" );
    }


// ---------------------------------------------------------------------------
// Invokes application installer to remove .sis from device.
// ---------------------------------------------------------------------------
//
void CCSCEngServicePluginHandler::RemovePluginSisL( 
    const TUid& aPluginUid ) const
    {
    CSCENGDEBUG( "CCSCEngServicePluginHandler::RemovePluginSisL - begin" );
    
    // Create & connect to the software installation server.
    SwiUI::RSWInstSilentLauncher swInstaller;
    CleanupClosePushL( swInstaller );
    User::LeaveIfError( swInstaller.Connect() );
    
    // Make silent uninstall via sowtware installation server.
    SwiUI::TInstallOptionsPckg uninstallOptions;
    User::LeaveIfError( 
        swInstaller.SilentUninstall( 
            aPluginUid , 
            uninstallOptions, 
            SwiUI::KSisxMimeType ) );
    
    // Close & destroy server after uninstallation.
    CleanupStack::PopAndDestroy( &swInstaller );
        
    CSCENGDEBUG( "CCSCEngServicePluginHandler::RemovePluginSisL - end" );
    }
