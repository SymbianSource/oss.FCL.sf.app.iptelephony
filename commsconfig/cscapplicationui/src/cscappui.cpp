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
* Description:  CSC Application´s AppUi
*
*/


#include <apgcli.h>
#include <AknDoc.h>
#include <eikclb.h>
#include <aknview.h>
#include <eikappui.h>
#include <cchclient.h>
#include <cmmanagerext.h>
#include <AknIconUtils.h>
#include <AknsConstants.h>
#include <cscsettingsui.h>
#include <xSPViewServices.h>
#include <AiwServiceHandler.h>
#include <mspnotifychangeobserver.h>

#include "cscappui.h"
#include "csclogger.h"
#include "cscdocument.h"
#include "cscconstants.h"
#include "cscserviceview.h"
#include "cscengcchhandler.h"
#include "cscengstartuphandler.h"
#include "cscengservicehandler.h"
#include "cscengbrandinghandler.h"
#include "cscengconnectionhandler.h"
#include "cscengservicepluginhandler.h"
#include "cscengdestinationshandler.h"
#include "cscenguiextensionpluginhandler.h"

const TInt KMaxParamLength = 255;

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCAppUi::CCSCAppUi()                         
    {
    }
    
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CCSCAppUi::ConstructL()
    {
    CSCDEBUG( "CCSCAppUi::ConstructL - begin" );
    
    BaseConstructL( 
        EAknEnableSkin | EAknEnableMSK | EAknSingleClickCompatible );
    
    iStartupHandler = CCSCEngStartupHandler::NewL();
    iServiceHandler = CCSCEngServiceHandler::NewL( this );
    iBrandingHandler = CCSCEngBrandingHandler::NewL();
    iCCHHandler = CCSCEngCCHHandler::NewL( *this );
    iConnectionHandler = CCSCEngConnectionHandler::NewL( *this );
    
    CCSCEngServicePluginHandler* servicePluginHandler = 
        CCSCEngServicePluginHandler::NewL( 
            *iEikonEnv, *this, *iServiceHandler );
    
    CleanupStack::PushL( servicePluginHandler );
    
    CCSCEngUiExtensionPluginHandler* uiExtensionPluginHandler = 
        CCSCEngUiExtensionPluginHandler::NewL( 
            *iEikonEnv, *this, *iStartupHandler );

    CleanupStack::Pop( servicePluginHandler );
    
    // Responsibility or Plugin Handlers are transferred to document,
    // because UI framework might make calls to ECom plugin after the
    // application UI is deleted. Case exists when Exit() is 
    // called from plugin.
    CCSCDocument* doc = static_cast< CCSCDocument* >( Document() );
    doc->SetOwnershipOfPluginHandlers( 
        servicePluginHandler, uiExtensionPluginHandler );
    
    TRAP_IGNORE(   
        servicePluginHandler->InitializePluginsL();
        uiExtensionPluginHandler->InitializePluginsL();
        );
        
    iServiceView = CCSCServiceView::NewL( 
        *servicePluginHandler, 
        *uiExtensionPluginHandler,
        *iStartupHandler,
        *iServiceHandler,
        *iBrandingHandler,
        *iCCHHandler );
    
    AddViewL( iServiceView );
     
    CSCDEBUG( "CCSCAppUi::ConstructL - end" );
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCAppUi::~CCSCAppUi()
    {
    CSCDEBUG( "CCSCAppUi::~CCSCAppUi - begin" );
        
    delete iStartupHandler;
    delete iServiceHandler;
    delete iBrandingHandler;
    delete iCCHHandler;
    delete iConnectionHandler;
                  
    CSCDEBUG( "CCSCAppUi::~CCSCAppUi - end" );
    }


// ---------------------------------------------------------------------------
// From class CEikAppUi
// For giving startup parameters to CSC application
// ---------------------------------------------------------------------------
//
TBool CCSCAppUi::ProcessCommandParametersL(
    TApaCommand /*aCommand*/,
    TFileName& aDocumentName,
    const TDesC8& /*aTail*/)
    {
    CSCDEBUG( "CCSCAppUi::ProcessCommandParametersL - begin" );
    
    if ( aDocumentName.Length() )
        {
        iStartupHandler->SetStartupParametersL( aDocumentName );
        if ( CCSCEngStartupHandler::EOpenSettingsUi == 
            iStartupHandler->GetParamAction() )
            {
            iServiceView->InitializeWithStartupParametersL();
            }
        }
        
    CSCDEBUG( "CCSCAppUi::ProcessCommandParametersL - end" );
    
    return EFalse;
    }

// ---------------------------------------------------------------------------
// From class CEikAppUi
// For giving startup parameters to CSC application
// ---------------------------------------------------------------------------
//
void CCSCAppUi::ProcessMessageL( TUid /* aUid */, const TDesC8 &aParams )
	{
	CSCDEBUG( "CCSCAppUi::ProcessMessageL - begin" );
	    
	if ( aParams.Length() && ( aParams.Length() <= KMaxParamLength ) )
	    {
        TBuf<KMaxParamLength> params;
	    params.Copy( aParams );  
	    iStartupHandler->SetStartupParametersL( params );
	        
	    iServiceView->InitializeWithStartupParametersL();
	    iServiceView->ExecuteStartupActionsL( EFalse, EFalse );
	    }
	        
	CSCDEBUG( "CCSCAppUi::ProcessMessageL - end" );
	}


// ---------------------------------------------------------------------------
// From class CAknViewAppUi
// For command handling.
// ---------------------------------------------------------------------------
//
void CCSCAppUi::HandleCommandL( TInt aCommand ) 
    {
    CSCDEBUG( "CCSCAppUi::HandleCommandL - begin" );
    
    switch ( aCommand )
        {
        case EEikCmdExit:
        case EAknSoftkeyExit:
        case EAknSoftkeyBack:
        case EAknCmdExit:
            Exit();
            break;
        default:
            break;
        }
    
    CSCDEBUG( "CCSCAppUi::HandleCommandL - end" );
    }


// ---------------------------------------------------------------------------
// From class CAknViewAppUi
// Handling changing of the skin and layout.
// ---------------------------------------------------------------------------
//
void CCSCAppUi::HandleResourceChangeL( TInt aType )
    {
    CSCDEBUG( "CCSCAppUi::HandleResourceChangeL - begin" );
    
    if ( aType == KAknsMessageSkinChange || 
         aType == KEikDynamicLayoutVariantSwitch )
        {
        iServiceView->UpdateLayout( aType );
        }

    CAknViewAppUi::HandleResourceChangeL( aType );
    
    CSCDEBUG( "CCSCAppUi::HandleResourceChangeL - end" );
    }


// ---------------------------------------------------------------------------
// From class MCSCProvisioningObserver
// Notifies service plug-in event
// ---------------------------------------------------------------------------
//
void CCSCAppUi::NotifyServicePluginResponse( 
    const CCSCEngServicePluginHandler::TServicePluginResponse& aResponse, 
    const TInt aIndex, 
    const TUid& aPluginUid )
    {
    CSCDEBUG( "CCSCAppUi::NotifyServicePluginResponse - begin" );
    
    CCSCEngStartupHandler::TAction action = iStartupHandler->GetParamAction();
    TUid pluginUid = iStartupHandler->GetParamPluginUid();
  
    switch ( aResponse )
        {
        case CCSCEngServicePluginHandler::EPluginInitialized:
            {           
            // Run service configuration procedure, if new plugin is added
            // after service setup plugins has been initialized and plugin 
            // is set to be configured in startup parameters.
            if ( CCSCEngStartupHandler::EAddSetupPluginService == action &&
                aPluginUid == pluginUid )
                {       
                TRAP_IGNORE( 
                    TBool canceled ( EFalse );
                    canceled = iServiceView->HandleServiceConfigurationL( 
                                aPluginUid );
                                
                    if ( canceled )
                        {
                        iServiceView->ExecuteStartupActionsL();
                        }
                    );
                } 
            else if ( iStartupHandler->StartedFromHomescreen() )
                {
                TRAP_IGNORE( iServiceView->HandleServiceConfigurationL( KNullUid ) );
                }
            break;
            }
        case CCSCEngServicePluginHandler::EPluginProvisioned:
            {
            CSCDEBUG( "CCSCAppUi::NotifyServicePluginResponse - plugin provisioned" );
                  
            // shutdown csc (provisioned service plugin should open phonebook tab).
            RunAppShutter();
            break;
            }
        case CCSCEngServicePluginHandler::EPluginRemoved:
            {
            // Service removed. If application is started with statup
            // parameters, close application and retuns to application
            // installer. Otherwise dismiss wait note from the screen.
            if ( CCSCEngStartupHandler::ERemoveSetupPluginService == action &&
                KNullUid != pluginUid )
                {
                iServiceView->HideDialogWaitNote();
                RunAppShutter();
                }
            else
                {
                CCSCDocument* doc = 
                    static_cast< CCSCDocument* >( Document() );
                    
                CCSCEngServicePluginHandler& handler = 
                    doc->ServicePluginHandler();

                TServicePluginInfo pluginInfo = 
                    handler.ItemFromPluginInfoArray( aIndex );
                          
                CAknView* view = View( pluginInfo.iViewId );
                if( !view )
                    {
                    RemoveView( pluginInfo.iViewId );
                    }
                else
                    {
                    // nothing to do
                    }
                       
                iServiceView->HideDialogWaitNote();
                }
            break;
            }
        case CCSCEngServicePluginHandler::EPluginError:
            {
            TRAP_IGNORE( iServiceView->ServiceConfiguringFailedL( aIndex ) );
            break;
            }
        case CCSCEngServicePluginHandler::EPluginModified:
        default:
             break;
        }
    
    CSCDEBUG( "CCSCAppUi::NotifyServicePluginResponse - end" );
    }


// ---------------------------------------------------------------------------
// From class MCSCUiExtensionObserver
// Notifies when configuring of service plug-in is done.
// ---------------------------------------------------------------------------
//
void CCSCAppUi::NotifyUiExtensionPluginResponse( 
    const CCSCEngUiExtensionPluginHandler::TUiExtensionPluginResponse& 
    aResponse, 
    const TInt /*aIndex*/, 
    const TUid& /*aPluginUid*/ )
    {
    CSCDEBUG( "CCSCAppUi::NotifyUiExtensionPluginResponse - begin" );
    
    switch ( aResponse )
        {
        case CCSCEngUiExtensionPluginHandler::EPluginExited:
            {
            TRAP_IGNORE( iServiceView->HandleUiExtensionExitL() );
            break;
            }
        case CCSCEngUiExtensionPluginHandler::EPluginError:
            {
            TRAP_IGNORE( ActivateLocalViewL( KCSCServiceViewId ) );
            break;
            }
        case CCSCEngUiExtensionPluginHandler::EPluginInitialized:
            // nothing to do
        default:
            break;
        }
   
    CSCDEBUG( "CCSCAppUi::NotifyUiExtensionPluginResponse - end" );
    }


// ---------------------------------------------------------------------------
// From class MCSCEngServiceObserver
// Notifies when service(s) have changed.
// ---------------------------------------------------------------------------
//
void CCSCAppUi::NotifyServiceChange()
    {
    CSCDEBUG( "CCSCAppUi::NotifyServiceChange - begin" );
    
    TVwsViewId activeViewId;
    GetActiveViewId( activeViewId );
    
    if( iServiceView && KCSCServiceViewId == activeViewId.iViewUid )
        {
        TRAP_IGNORE( iServiceView->UpdateServiceViewL() );
        }
    
    CSCDEBUG( "CCSCAppUi::NotifyServiceChange - end" );  
    }


// ---------------------------------------------------------------------------
// From class MCSCEngCCHObserver
// ---------------------------------------------------------------------------
//
void CCSCAppUi::ServiceStatusChanged(
    TUint aServiceId, 
    TCCHSubserviceType /*aType*/, 
    const TCchServiceStatus& aServiceStatus )
    {
    CSCDEBUG2( 
       "CCSCAppUi::ServiceStatusChanged - STATE=%d", aServiceStatus.State() );
    
    if ( ECCHDisabled == aServiceStatus.State() )
        {        
        TVwsViewId activeViewId;
        GetActiveViewId( activeViewId );
        
        if( iServiceView && KCSCServiceViewId == activeViewId.iViewUid )
            {            
            TRAPD( err, SnapCheckL( aServiceId ) );  
            
            if ( KErrInUse == err )
                {
                iMonitoredService = aServiceId;
                }
            else
                {
                TRAP_IGNORE( iServiceView->DeleteServiceL( aServiceId ) );
                }
            }
        }
        
    CSCDEBUG( "CCSCAppUi::ServiceStatusChanged - end" );
    }


// ---------------------------------------------------------------------------
// From class MCSCEngConnectionObserver
// ---------------------------------------------------------------------------
//
void CCSCAppUi::NotifyConnectionEvent( 
    CCSCEngConnectionHandler::TConnectionEvent aConnectionEvent )
    {
    CSCDEBUG( "CCSCAppUi::NotifyConnectionEvent - begin" );
        
    TVwsViewId activeViewId;
    GetActiveViewId( activeViewId );
    
    if( ( iServiceView && KCSCServiceViewId == activeViewId.iViewUid ) &&
        ( CCSCEngConnectionHandler::EEventConnectionDown == aConnectionEvent || 
          CCSCEngConnectionHandler::EEventTimedOut == aConnectionEvent ) )
        {        
        iConnectionHandler->StopListeningConnectionEvents();
        TRAP_IGNORE( iServiceView->DeleteServiceL( iMonitoredService ) );
        }
    
    CSCDEBUG( "CCSCAppUi::NotifyConnectionEvent - end" );
    }

// ---------------------------------------------------------------------------
// For checking if SNAP is still in use.
// ---------------------------------------------------------------------------
//
void CCSCAppUi::SnapCheckL( TUint aServiceId ) const
    {
    CSCDEBUG( "CCSCAppUi::SnapCheckL - begin" );  
    
    CCSCEngDestinationsHandler* destHandler =
        CCSCEngDestinationsHandler::NewL();
    CleanupStack::PushL( destHandler );
    
    TInt snapId( 0 );
    snapId = iServiceHandler->SnapIdL( aServiceId );
    
    if ( destHandler->IsSnapInUseL( snapId ) )
        {
        iConnectionHandler->StartListeningConnectionEvents();
        User::Leave( KErrInUse );
        }
    
    CleanupStack::PopAndDestroy( destHandler );
    
    CSCDEBUG( "CCSCAppUi::SnapCheckL - end" );  
    }
