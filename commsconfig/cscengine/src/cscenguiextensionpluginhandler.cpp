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
* Description:  For CSC needed Aiw handling for ui extension plug-ins
*
*/


#include <eikenv.h>
#include <bautils.h>
#include <pathinfo.h>
#include <SWInstApi.h>
#include <SWInstDefs.h>
#include <AiwCommon.hrh>
#include <cscengine.rsg>
#include <AiwServiceHandler.h>
#include <data_caging_path_literals.hrh>

#include "cscenglogger.h"
#include "cscengstartuphandler.h"
#include "mcscenguiextensionobserver.h"
#include "cscenguiextensionpluginhandler.h"

// Resource file location.
_LIT( KCSCEngineResourceFile, "cscengine.rsc" ); // file

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCEngUiExtensionPluginHandler::CCSCEngUiExtensionPluginHandler( 
    CEikonEnv& aEikEnv,
    MCSCEngUiExtensionObserver& aObserver,
    CCSCEngStartupHandler& aStartupHandler ):
    iEikEnv( aEikEnv ),
    iObserver( aObserver ),
    iStartupHandler( aStartupHandler )
    {
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CCSCEngUiExtensionPluginHandler::ConstructL()
    {
    CSCENGDEBUG( "CCSCEngUiExtensionPluginHandler::ConstructL - begin" );
    
    // Attach to AIW Framework.
    iServiceHandler = CAiwServiceHandler::NewL();
    
    // Load resource file to the memory.    
    TFileName resourceFile = TParsePtrC( PathInfo::RomRootPath() ).Drive();
    resourceFile.Append( KDC_RESOURCE_FILES_DIR );
    resourceFile.Append( KCSCEngineResourceFile );
    BaflUtils::NearestLanguageFile( iEikEnv.FsSession(), resourceFile );
    iResourceOffset = iEikEnv.AddResourceFileL( resourceFile );
    
    CSCENGDEBUG( "CCSCEngUiExtensionPluginHandler::ConstructL - end" );
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CCSCEngUiExtensionPluginHandler* 
    CCSCEngUiExtensionPluginHandler::NewL( 
        CEikonEnv& aEikEnv,
        MCSCEngUiExtensionObserver& aObserver,
        CCSCEngStartupHandler& aStartupHandler )
    {
    CCSCEngUiExtensionPluginHandler* self = 
        CCSCEngUiExtensionPluginHandler::NewLC( 
            aEikEnv, aObserver, aStartupHandler );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CCSCEngUiExtensionPluginHandler* 
    CCSCEngUiExtensionPluginHandler::NewLC( 
        CEikonEnv& aEikEnv,
        MCSCEngUiExtensionObserver& aObserver,
        CCSCEngStartupHandler& aStartupHandler )
    {
    CCSCEngUiExtensionPluginHandler* self = 
        new ( ELeave ) CCSCEngUiExtensionPluginHandler( 
            aEikEnv, aObserver, aStartupHandler );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCEngUiExtensionPluginHandler::~CCSCEngUiExtensionPluginHandler()
    {
    CSCENGDEBUG( 
     "CCSCEngUiExtensionPluginHandler::~CCSCEngUiExtensionPluginHandler - begin" );

    iEikEnv.DeleteResourceFile( iResourceOffset );
    iUiExtensionPluginInfoArray.Reset();
    iUiExtensionPluginInfoArray.Close();   
    delete iServiceHandler;
     
    CSCENGDEBUG( 
     "CCSCEngUiExtensionPluginHandler::~CCSCEngUiExtensionPluginHandler - end" );
    }
    
    
// ---------------------------------------------------------------------------
// Initializes CSC supported ui extension plugins.
// ---------------------------------------------------------------------------
//
EXPORT_C void CCSCEngUiExtensionPluginHandler::InitializePluginsL()
    {
    CSCENGDEBUG( 
        "CCSCEngUiExtensionPluginHandler::InitializePluginsL - begin" );
    
    // Attach to CSC supported ui extension plugins and execute initalization.   
    iServiceHandler->AttachL( R_CSCENG_INTEREST_UI_EXT_PLUGIN );
    
    iServiceHandler->ExecuteServiceCmdL( 
            KAiwCmdCSCUiExtensionPlugins,
            iServiceHandler->InParamListL(),
            iServiceHandler->OutParamListL(),
            EPluginInitialize,
            this );
           
    CSCENGDEBUG( 
        "CCSCEngUiExtensionPluginHandler::InitializePluginsL - end" );
    }


// ---------------------------------------------------------------------------
// Launches plugin provided setting view.
// ---------------------------------------------------------------------------
//
EXPORT_C void CCSCEngUiExtensionPluginHandler::LaunchUiExtensionL( 
    const TUid& aPluginUid, const TUid& aViewUid, TBool aLaunchedFromAI  )
    {
    CSCENGDEBUG( 
        "CCSCEngUiExtensionPluginHandler::LaunchUiExtensionL - begin" );
    
    // Set plugins Uid to generic param data.
    CAiwGenericParamList& paramList = iServiceHandler->OutParamListL();
    TAiwVariant variant( aPluginUid );
    TAiwGenericParam genericParamUid( EGenericParamError, variant );
    paramList.AppendL( genericParamUid );
            
    // Set view id to be returned to generic param data.
    variant.Reset();
    variant.Set( aViewUid );
    TAiwGenericParam genericParamViewId( EGenericParamError, variant );
    paramList.AppendL( genericParamViewId );
    
    // Set URL to generic param data if set in CSC startup.    
    TBuf<KMaxFileName> url;
    TInt err = iStartupHandler.GetParamUrl( url );
    
    if ( !err )
        {
        variant.Reset();
        variant.Set( url );
        TAiwGenericParam genericParamURL( EGenericParamError, variant );
        paramList.AppendL( genericParamURL );
        }
    
    // Execute service handler command.
    if ( aLaunchedFromAI )
        {
        iServiceHandler->ExecuteServiceCmdL( KAiwCmdCSCUiExtensionPlugins, 
                                             iServiceHandler->InParamListL(),
                                             paramList,
                                             EPluginOpenExtensionViewFromAI,
                                             this );     
        }
    else
        {
        iServiceHandler->ExecuteServiceCmdL( KAiwCmdCSCUiExtensionPlugins, 
                                             iServiceHandler->InParamListL(),
                                             paramList,
                                             EPluginOpenExtensionView,
                                             this );        
        }

                                  
    CSCENGDEBUG( 
        "CCSCEngUiExtensionPluginHandler::LaunchUiExtensionL - end" );
    }


// ---------------------------------------------------------------------------
// Returns item from UI Extension plug-in array
// ---------------------------------------------------------------------------
//
EXPORT_C TUiExtensionPluginInfo 
    CCSCEngUiExtensionPluginHandler::ItemFromPluginInfoArray( TInt aIndex )
    {
    CSCENGDEBUG( "CCSCEngUiExtensionPluginHandler::ItemFromPluginInfoArray" );
    
    return iUiExtensionPluginInfoArray[ aIndex ];
    }


// ---------------------------------------------------------------------------
// Returns plugin counts from UiExtensionPluginsInfoArray.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCSCEngUiExtensionPluginHandler::PluginCount() const
    {
    CSCENGDEBUG2( 
        "CCSCEngUiExtensionPluginHandler::PluginCount: %d", 
            iUiExtensionPluginInfoArray.Count() );
    
    return iUiExtensionPluginInfoArray.Count();
    }


// ---------------------------------------------------------------------------
// From class MAiwNotifyCallBack
// Handles received callbacks from AIW plugin.
// ---------------------------------------------------------------------------
//
TInt CCSCEngUiExtensionPluginHandler::HandleNotifyL( 
    TInt /*aCmdId*/,
    TInt aEventId,
    CAiwGenericParamList& /*aEventParamList*/,
    const CAiwGenericParamList& aInParamList )
    {
    CSCENGDEBUG( "CCSCEngUiExtensionPluginHandler::HandleNotifyL - begin");
    
    TInt err( KErrNone );
    
    switch ( aEventId )
        {
        case KAiwEventStarted:
            {
            CSCENGDEBUG( 
             "CCSCEngUiExtensionPluginHandler::HandleNotifyL KAiwEventStarted" );
            
            SetPluginInitInfo( aInParamList );
            
            TInt index( 0 );
            TUid pluginUid( KNullUid );
            
            GetPluginIndexAndUid( aInParamList, index, pluginUid );
            
            iObserver.NotifyUiExtensionPluginResponse( 
                EPluginInitialized, index, pluginUid );
            break;
            }
        case KAiwEventStopped:
            {
            TInt index( 0 );
            TUid pluginUid( KNullUid );
            
            GetPluginIndexAndUid( aInParamList, index, pluginUid );
            
            iObserver.NotifyUiExtensionPluginResponse( 
                EPluginExited, index, pluginUid );
            
            CSCENGDEBUG( 
             "CCSCEngUiExtensionPluginHandler::HandleNotifyL KAiwEventStopped" );
            break;
            }
        case KAiwEventError:
            {
            TInt index( 0 );
            TUid pluginUid( KNullUid );
            
            GetPluginIndexAndUid( aInParamList, index, pluginUid );
            
            iObserver.NotifyUiExtensionPluginResponse( 
                EPluginError, index, pluginUid );
            
            CSCENGDEBUG( 
               "CCSCEngUiExtensionPluginHandler::HandleNotifyL KAiwEventError" );
            break;
            }
        default:
            CSCENGDEBUG( 
                "CCSCEngUiExtensionPluginHandler::HandleNotifyL: !!DEFAULT!!" );
            break;
        }
    
    CSCENGDEBUG( "CCSCEngUiExtensionPluginHandler::HandleNotifyL - end" );
    
    return err;
    }
    

// ---------------------------------------------------------------------------
// For getting plug-ins index in plug-in info array and plug-ins uid.
// ---------------------------------------------------------------------------
//
void CCSCEngUiExtensionPluginHandler::GetPluginIndexAndUid( 
    const CAiwGenericParamList& aInParamList,
    TInt aIndex, 
    TUid aPluginUid )
    {
    const TAiwGenericParam* genericParam = NULL;

    // First and only generic value is plugins uid value.
    genericParam = aInParamList.FindFirst( aIndex,
                                           EGenericParamError,
                                           EVariantTypeTUid );

    aPluginUid = genericParam->Value().AsTUid();

    for ( TInt i = 0; i < iUiExtensionPluginInfoArray.Count(); i++ )
        {
        if ( aPluginUid == iUiExtensionPluginInfoArray[ i ].iPluginsUid )
            {
            aIndex = i;
            break;
            }
        }
    }
    

// ---------------------------------------------------------------------------
// For parsing and settings plug-in initialization info
// ---------------------------------------------------------------------------
//
void CCSCEngUiExtensionPluginHandler::SetPluginInitInfo(
    const CAiwGenericParamList& aInParamList )
    {
    CSCENGDEBUG( 
        "CCSCEngUiExtensionPluginHandler::SetPluginInitInfoL - begin" );

    TUiExtensionPluginInfo pluginInfo;
    
    TInt index = 0;
    
    if ( aInParamList.Count() )
        {
        const TAiwGenericParam* genericParam = NULL;

        // First generic value is plugin Uid.
        genericParam = aInParamList.FindFirst( index,
                                               EGenericParamError,
                                               EVariantTypeTUid );
        
        pluginInfo.iPluginsUid = genericParam->Value().AsTUid();
                               
        // Second generic value is UI extension´s name. 
        genericParam = aInParamList.FindNext( index,
                                              EGenericParamError,
                                              EVariantTypeDesC );
        
        pluginInfo.iUiExtensionName.Copy( genericParam->Value().AsDes() );

        // Third generic value is ui extension´s placing info
        // 0 = listbox, 1 = options menu
        genericParam = aInParamList.FindNext( index,
                                              EGenericParamError,
                                              EVariantTypeTInt32 );
        
        TInt32 place = genericParam->Value().AsTInt32();        
        pluginInfo.iPlace = place;

        CSCENGDEBUG2( 
            "CCSCEngUiExtensionPluginHandler::SetPluginInitInfoL  Place: %d",
                place );
        }
   
    iUiExtensionPluginInfoArray.Append( pluginInfo );
    
    CSCENGDEBUG( 
        "CCSCEngUiExtensionPluginHandler::SetPluginInitInfoL - end" );
    }
