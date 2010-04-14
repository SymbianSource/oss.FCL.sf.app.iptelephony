/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CSC Application´s Service View
*
*/


#include <csc.rsg>
#include <eikclb.h>
#include <apgcli.h> 
#include <hlplch.h>
#include <featmgr.h>
#include <aknview.h>
#include <akntitle.h>
#include <aknlists.h>
#include <cchclient.h>
#include <AknsUtils.h>
#include <StringLoader.h>
#include <cscsettingsui.h>
#include <xSPViewServices.h>
#include <AiwServiceHandler.h>
#include <mspnotifychangeobserver.h>

#include "csc.hrh"
#include "cscappui.h"
#include "csclogger.h"
#include "cscdialog.h"
#include "cscconstants.h"
#include "cscserviceview.h"
#include "cscnoteutilities.h"
#include "cscengcchhandler.h"
#include "cscservicecontainer.h"
#include "cscengstartuphandler.h"
#include "cscengservicehandler.h"
#include "cscengbrandinghandler.h"
#include "cscengservicepluginhandler.h"
#include "cscenguiextensionpluginhandler.h"



// ======== MEMBER FUNCTIONS ========

const TUid KAIAppUid = { 0x102750F0 }; // active idle app uid

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCServiceView::CCSCServiceView( 
    CCSCEngServicePluginHandler& aServicePluginHandler,
    CCSCEngUiExtensionPluginHandler& aUiExtensionPluginHandler,
    CCSCEngStartupHandler& aStartupHandler,
    CCSCEngServiceHandler& aServiceHandler,
    CCSCEngBrandingHandler& aBrandingHandler,
    CCSCEngCCHHandler& aCCHHandler )
    :
    iServicePluginHandler( aServicePluginHandler ),
    iUiExtensionPluginHandler( aUiExtensionPluginHandler ),
    iStartupHandler( aStartupHandler ),
    iServiceHandler( aServiceHandler ),
    iBrandingHandler( aBrandingHandler ),
    iCCHHandler( aCCHHandler ),
    iStartup( ETrue )
    {
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CCSCServiceView::ConstructL()
    {
    CSCDEBUG( "CCSCServiceView::ConstructL - begin" );
    
    BaseConstructL( R_CSC_VIEW_SERVICE );
    iSettingsUi = CCSCSettingsUi::NewL( *iEikonEnv );
                     
    CSCDEBUG( "CCSCServiceView::ConstructL - end" );
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//      
CCSCServiceView* CCSCServiceView::NewL( 
    CCSCEngServicePluginHandler& aServicePluginHandler,
    CCSCEngUiExtensionPluginHandler& aUiExtensionPluginHandler,
    CCSCEngStartupHandler& aStartupHandler,
    CCSCEngServiceHandler& aServiceHandler,
    CCSCEngBrandingHandler& aBrandingHandler,
    CCSCEngCCHHandler& aCCHHandler )
    {
    CCSCServiceView* self = 
        CCSCServiceView::NewLC( 
            aServicePluginHandler, 
            aUiExtensionPluginHandler, 
            aStartupHandler,
            aServiceHandler,
            aBrandingHandler,
            aCCHHandler );
    
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCServiceView* CCSCServiceView::NewLC( 
    CCSCEngServicePluginHandler& aServicePluginHandler,
    CCSCEngUiExtensionPluginHandler& aUiExtensionPluginHandler,
    CCSCEngStartupHandler& aStartupHandler,
    CCSCEngServiceHandler& aServiceHandler,
    CCSCEngBrandingHandler& aBrandingHandler,
    CCSCEngCCHHandler& aCCHHandler )
    {
    CCSCServiceView* self = 
        new( ELeave ) CCSCServiceView( 
            aServicePluginHandler, 
            aUiExtensionPluginHandler, 
            aStartupHandler,
            aServiceHandler,
            aBrandingHandler,
            aCCHHandler );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCServiceView::~CCSCServiceView()
    {
    CSCDEBUG( "CCSCServiceView::~CCSCServiceView - begin" );
    
    iOfferedPluginUids.Reset();
    
    delete iSettingsUi;
    delete iContainer;
    delete iEngTimer;
    
    CSCDEBUG( "CCSCServiceView::~CCSCServiceView - end" );
    }


// ---------------------------------------------------------------------------
// CCSCServiceView::InitializeWithStartupParametersL
// ---------------------------------------------------------------------------
//
void CCSCServiceView::InitializeWithStartupParametersL()
    {
    CSCDEBUG( "CCSCServiceView::InitializeWithStartupParametersL" );
    
    CCSCEngStartupHandler::TAction action = iStartupHandler.GetParamAction();
    TBool isServiceViewDefault = 
        CCSCEngStartupHandler::EOpenSettingsUi != action;
    TUint serviceId = iStartupHandler.GetParamServiceId();
    TUid tabViewId( KNullUid );
    tabViewId.iUid = iServiceHandler.ServiceTabViewIdL( serviceId );
    iSettingsUi->InitializeL( 
        tabViewId, serviceId, isServiceViewDefault ? this : NULL );
    }


// ---------------------------------------------------------------------------
// From class CAknView.
// Handles the commands. If the command is command which is require to
// display outline-screen, the command display outline-screen
// corresponded to required.
// ---------------------------------------------------------------------------
// 
void CCSCServiceView::HandleCommandL( TInt aCommand )
    {
    CSCDEBUG2( "CCSCServiceView::HandleCommandL aCommand: %d", aCommand );
        
    switch ( aCommand )
        {
        case EAknSoftkeyOpen:
        case ECSCCmdOpen:
        case EAknSoftkeySelect:
            HandleListboxItemOpenL();
            break;
        case ECSCCommandDelete:
            iContainer->DeleteServiceL();
            break;
        case EAknCmdHelp:
            if ( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
                {
                CArrayFix<TCoeHelpContext>* buf = AppUi()->AppHelpContextL();
                HlpLauncher::LaunchHelpApplicationL(
                    iEikonEnv->WsSession(), buf );
                }
            break;         
        // Exit menu command.    
        case EAknSoftkeyExit:
            AppUi()->HandleCommandL( EEikCmdExit );
            break;           
        case EAknSoftkeyBack:
            AppUi()->HandleCommandL( EAknSoftkeyBack );
            break;
        // Let appui handle other commands.    
        default:
            CSCDEBUG( "CCSCServiceView::HandleCommandL: !!DEFAULT!!" );
            AppUi()->HandleCommandL( aCommand );
            break;
        }
    }    
        

// ---------------------------------------------------------------------------
// From class CAknView.
// The ID of view.
// ---------------------------------------------------------------------------
// 
TUid CCSCServiceView::Id() const
    {
    return KCSCServiceViewId;
    }

// ---------------------------------------------------------------------------
// From class CAknView.
// Handle foreground event.
// ---------------------------------------------------------------------------
// 
void CCSCServiceView::HandleForegroundEventL( TBool aForeground )
    {
    if ( aForeground )
        {
        ExecuteStartupActionsL( ETrue );
        }
    }


// ---------------------------------------------------------------------------
// For handling ui extension plug-in exits
// ---------------------------------------------------------------------------
// 
void CCSCServiceView::HandleUiExtensionExitL()
    {
    CSCDEBUG( "CCSCServiceView::HandleUiExtensionExit - begin" );
    
    AppUi()->ActivateLocalViewL( KCSCServiceViewId );
    
    TUid appUid = iStartupHandler.GetParamAppUid();
                          
    if ( appUid.iUid )
        {
        // Open application matching uid
        RApaLsSession session;
        CleanupClosePushL( session );
        
        User::LeaveIfError(session.Connect());
 
        TFileName fileName;
        TThreadId threadId;

        User::LeaveIfError(
            session.StartDocument( fileName, appUid, threadId ) );
                
        CleanupStack::PopAndDestroy( &session );
                       
        // Reset startup parameter app uid
        iStartupHandler.ResetUid( CCSCEngStartupHandler::EAppUid );
        }
         
    CSCDEBUG( "CCSCServiceView::HandleUiExtensionExit - end" );
    }


// ---------------------------------------------------------------------------
// For updating service view
// ---------------------------------------------------------------------------
// 
void CCSCServiceView::UpdateServiceViewL()
    {
    CSCDEBUG( "CCSCServiceView::UpdateServiceViewL - begin" );
    
    iContainer->UpdateServiceViewL();
    
    CSCDEBUG( "CCSCServiceView::UpdateServiceViewL - end" );
    }


// ---------------------------------------------------------------------------
// For deleting service.
// ---------------------------------------------------------------------------
// 
void CCSCServiceView::DeleteServiceL( TUint serviceId )
    {
    CSCDEBUG( "CCSCServiceView::DeleteServiceL - begin" );
    
    TUid pluginUid( KNullUid );
    TInt32 servicePluginId( 0 );
    
    TRAPD( err, servicePluginId = 
        iServiceHandler.ServiceSetupPluginIdL( serviceId ) );
    
    if ( !err && servicePluginId )
        {
        pluginUid.iUid = servicePluginId;
        iContainer->HandleServicePluginRemovationL( pluginUid );
        }
    else
        {
        iContainer->DeleteServiceL();
        }
    
    CSCDEBUG( "CCSCServiceView::DeleteServiceL - end" );
    }

// ---------------------------------------------------------------------------
// Shows information about failed configuration.
// ---------------------------------------------------------------------------
//
void CCSCServiceView::ServiceConfiguringFailedL( TInt aIndex )
    {
    CSCDEBUG( "CCSCServiceView::ServiceConfiguringFailedL - begin" );
    
    TServicePluginInfo pluginInfo;
    
    if ( iServicePluginHandler.PluginCount( 
        CCSCEngServicePluginHandler::EInitialized ) > aIndex )
        {
        pluginInfo = iServicePluginHandler.ItemFromPluginInfoArray( aIndex );
        
        if ( pluginInfo.iProviderName.Length() )
            {
            CCSCNoteUtilities::ShowInformationNoteL( 
                CCSCNoteUtilities::ECSCUnableToConfigureNote, 
                    pluginInfo.iProviderName );
            }
        
        // Try to remove uncomplete settings by calling service setup plugin.
        iServicePluginHandler.DoRemovationL( pluginInfo.iPluginsUid, EFalse );
        }   
          
    AppUi()->ActivateLocalViewL( KCSCServiceViewId );
        
    CSCDEBUG( "CCSCServiceView::ServiceConfiguringFailedL - end" );    
    }
 
 
// ---------------------------------------------------------------------------
// Hides dialog wait note.
// ---------------------------------------------------------------------------
//
 void CCSCServiceView::HideDialogWaitNote()
    {
    if ( iContainer )
        {
        iContainer->HideDialogWaitNote();
        }
    }
   
// ---------------------------------------------------------------------------
// From class MEikListBoxObserver.
// For handling list box events.
// ---------------------------------------------------------------------------
//
void CCSCServiceView::HandleListBoxEventL( CEikListBox* /*aListBox*/, 
                                           TListBoxEvent aEventType )
    {
    CSCDEBUG2( "CCSCServiceView::HandleListBoxEventL aEventType: %d",
        aEventType );
    
    switch( aEventType )
        {
        case EEventEnterKeyPressed:
        case EEventItemSingleClicked:
            HandleListboxItemOpenL();
            break;
        default:
            CSCDEBUG( "CCSCServiceView::HandleListBoxEventL !!DEFAULT!!" );
            break;
        }
    }


// ---------------------------------------------------------------------------
// From class MCSCServiceContainerObserver.
// Update softkey.
// ---------------------------------------------------------------------------
//
void CCSCServiceView::UpdateCbaL()
    {
    if( !Cba() )
       {
       return;
       }
    
    TInt commandId ( EAknSoftkeyOpen );
    HBufC* selectText = StringLoader::LoadLC( R_CSC_MSK_SELECT );
    HBufC* openText = StringLoader::LoadLC( R_CSC_MSK_OPEN );
    
    if( iContainer->CurrentItemIndex() >= 0 )
        {
        if( iContainer->ListBoxItem().iPluginUid != KAccountCreationPluginUID )
            {
            Cba()->SetCommandL( CEikButtonGroupContainer::EMiddleSoftkeyPosition,
                commandId, *openText );
            }
        else
            {
            commandId = EAknSoftkeySelect;
            Cba()->SetCommandL( CEikButtonGroupContainer::EMiddleSoftkeyPosition,
                commandId, *selectText );
            }
        }
    else
        {
        Cba()->MakeCommandVisibleByPosition( 
            CEikButtonGroupContainer::EMiddleSoftkeyPosition, EFalse );
        }
    
    Cba()->DrawDeferred();
    CleanupStack::PopAndDestroy( openText );
    CleanupStack::PopAndDestroy( selectText );
    }


// ---------------------------------------------------------------------------
// For handling listbox item opening
// ---------------------------------------------------------------------------
//
void CCSCServiceView::HandleListboxItemOpenL()
    {
    CSCDEBUG( "CCSCServiceView::HandleListboxItemOpen - begin" );

    TListBoxItem item = iContainer->ListBoxItem();
    
    switch ( item.iItemType )
        {
        case TListBoxItem::EUiExtension:
            {
            iUiExtensionPluginHandler.LaunchUiExtensionL( 
                item.iPluginUid, Id() );
            break;
            }
        case TListBoxItem::EGeneralService:
        case TListBoxItem::EServicePlugin:
            {
            iSettingsUi->InitializeL( Id(), item.iServiceId, this );
            iSettingsUi->LaunchSettingsUiL();
            break;
            }
        default:
            {
            User::Leave( KErrArgument );
            break;
            }       
        }
                       
    CSCDEBUG( "CCSCServiceView::HandleListboxItemOpen - end" );
    }

    
// ---------------------------------------------------------------------------
// For changing text in the title pane.
// ---------------------------------------------------------------------------
// 
void CCSCServiceView::SetTitlePaneTextL() const
    {
    CSCDEBUG( "CCSCServiceView::SetTitlePaneTextL - begin" );
    
    CEikStatusPane* statusPane = iEikonEnv->AppUiFactory()->StatusPane();
    
    CAknTitlePane* titlePane =
        static_cast<CAknTitlePane*> ( statusPane->ControlL(
            TUid::Uid( EEikStatusPaneUidTitle ) ) );
       
    HBufC* text = iEikonEnv->AllocReadResourceLC( R_CSC_SERVICE_VIEW_TITLE );
    titlePane->SetTextL( text->Des() );
    CleanupStack::PopAndDestroy( text );
        
    CSCDEBUG( "CCSCServiceView::SetTitlePaneTextL - end" );
    }


// ---------------------------------------------------------------------------
// Handle service configuration.
// ---------------------------------------------------------------------------
// 
TBool CCSCServiceView::HandleServiceConfigurationL( TUid aUid )
    {
    CSCDEBUG( "CCSCServiceView::HandleServiceConfigurationL" );
    
    TInt initializedCount( iServicePluginHandler.PluginCount( 
            CCSCEngServicePluginHandler::EInitialized ) );
    
    TBool canceled( EFalse );
    TServicePluginInfo pluginInfo;
    
    iUid = aUid;
    
    if ( KNullUid != aUid ) 
        {
        for ( TInt i( 0 ) ; i < initializedCount ; i++ )
            {            
            pluginInfo = iServicePluginHandler.ItemFromPluginInfoArray( i );
            
            if ( aUid == pluginInfo.iPluginsUid && !pluginInfo.iProvisioned )
                {                
                iStartupHandler.ResetUid( CCSCEngStartupHandler::EPluginUid );
                
                iOfferedPluginUids.Append( pluginInfo.iPluginsUid );
                
                iPluginInfo = pluginInfo;
                
                iNextPluginIndex = i;
                
                delete iEngTimer;
                iEngTimer = NULL;
                iEngTimer = CCSCEngTimer::NewL( *this );
                
                iEngTimer->StartTimer( CCSCEngTimer::ENoteDelayTimer );
                
                break;
                }
            }   
        
        }
    else
        {
        for ( TInt j( 0 ) ; j < initializedCount ; j++ )
            {            
            pluginInfo = iServicePluginHandler.ItemFromPluginInfoArray( j );                      
            
            if ( !pluginInfo.iProvisioned )
                {   
                iOfferedPluginUids.Append( pluginInfo.iPluginsUid );
                
                iPluginInfo = pluginInfo;
                
                iNextPluginIndex = j;
                
                delete iEngTimer;
                iEngTimer = NULL;
                iEngTimer = CCSCEngTimer::NewL( *this );

                iEngTimer->StartTimer( CCSCEngTimer::ENoteDelayTimer );
                
                break;
                }
            }
        }
    
    return canceled;
    }


// ---------------------------------------------------------------------------
// Executes startup actions based on startup parameters.
// ---------------------------------------------------------------------------
// 
void CCSCServiceView::ExecuteStartupActionsL( 
    TBool /*aForeGroundEvent*/, TBool aLaunchedFromAi )
    {
    CSCDEBUG( "CCSCServiceView::ExecuteStartupActionsL - begin" );
    
    CCSCEngStartupHandler::TAction action = iStartupHandler.GetParamAction();
    TUid pluginUid = iStartupHandler.GetParamPluginUid();
    TUint serviceId = iStartupHandler.GetParamServiceId();
    
    switch( action )
        {
        case  CCSCEngStartupHandler::EAddSetupPluginService:
            {
            if ( KNullUid != pluginUid )
                {
                HandleServiceConfigurationL( pluginUid );
                }           
            break;
            }
        case  CCSCEngStartupHandler::ERemoveSetupPluginService:
            {
            if ( KNullUid != pluginUid )
                {
                TRAPD( err, iContainer->HandleServicePluginRemovationL( 
                    pluginUid ) );
                
                // If error in service plugin removal, close csc.
                if ( err )
                    {
                    AppUi()->RunAppShutter();
                    }
                }
            break;
            }
        case  CCSCEngStartupHandler::EOpenUiExtension:
            {
            if ( KNullUid != pluginUid )
                {
                TRAP_IGNORE( 
                    iUiExtensionPluginHandler.LaunchUiExtensionL( 
                        pluginUid, Id(), aLaunchedFromAi ) );
                              
                iStartupHandler.ResetUid( 
                    CCSCEngStartupHandler::EPluginUid );
                }
            break;
            }
        case  CCSCEngStartupHandler::EOpenSettingsUi:
            {
            if ( serviceId )
                {
                TUid tabview( KNullUid );
                tabview.iUid = iServiceHandler.ServiceTabViewIdL( serviceId );
                iSettingsUi->InitializeL( tabview, serviceId, NULL );
                iSettingsUi->LaunchSettingsUiL();
                iStartupHandler.ResetServiceId();
                }
            break;
            }
        // Default action is to check unprovisioned service setup plugins.    
        default:
            {
            TInt initializedCount( iServicePluginHandler.PluginCount( 
                CCSCEngServicePluginHandler::EInitialized ) );
                          
             for ( TInt i( 0 ) ; i < initializedCount ; i++ )
                 {
                 TBool alreadyOffered( EFalse );
                        
                 TServicePluginInfo pluginInfo = 
                     iServicePluginHandler.ItemFromPluginInfoArray( i );
                        
                 for ( TInt j( 0 ); j < iOfferedPluginUids.Count() ; j++ )
                     {
                     if ( pluginInfo.iPluginsUid == iOfferedPluginUids[ j ] )
                         {
                         alreadyOffered = ETrue;
                         break;
                         }
                     }
                        
                 if ( !pluginInfo.iProvisioned && !alreadyOffered )
                     {                
                     TBool canceled = HandleServiceConfigurationL( 
                         pluginInfo.iPluginsUid );
                            
                     if ( !canceled )
                         {                    
                         break;
                         }
                     }
                 }
            break;
            }
        }
    
    CSCDEBUG( "CCSCServiceView::ExecuteStartupActionsL - end" );
    }


// ---------------------------------------------------------------------------
// From class CAknView.
// Dynamically initializes the contents of the menu list
// ---------------------------------------------------------------------------
// 
void CCSCServiceView::DynInitMenuPaneL( TInt aResourceId, 
                                        CEikMenuPane* aMenuPane )
    {
    CSCDEBUG( "CCSCServiceView::DynInitMenuPaneL - begin" );
    
    if ( R_CSC_SERVICEVIEW_MENU == aResourceId && aMenuPane )
        {    
        TBool itemSpecificCommandsEnabled = 
            MenuBar()->ItemSpecificCommandsEnabled();
        
        if( ( iContainer->CurrentItemIndex() >= 0 ) &&
            itemSpecificCommandsEnabled )
            {
            if( iContainer->ListBoxItem().iPluginUid != 
                KAccountCreationPluginUID )
                {
                aMenuPane->SetItemDimmed( EAknSoftkeySelect, ETrue );
                aMenuPane->SetItemDimmed( EAknSoftkeyOpen, EFalse );
                }
            else
                {
                aMenuPane->SetItemDimmed( EAknSoftkeySelect, EFalse );
                aMenuPane->SetItemDimmed( EAknSoftkeyOpen, ETrue );
                }
            }
        else
            {
            aMenuPane->SetItemDimmed( EAknSoftkeySelect, ETrue );
            aMenuPane->SetItemDimmed( EAknSoftkeyOpen, ETrue );
            }

        // ===================================================================
        // Delete service menu option.
        // ===================================================================
        //        
        if ( !itemSpecificCommandsEnabled )
            {
            TInt index = iContainer->CurrentItemIndex();
            TPoint penDownPoint = iContainer->PenDownPoint();
            iContainer->ListBox().View()->XYPosToItemIndex( 
                penDownPoint, index );
            iContainer->SetCurrentIndex( index );
            }
                     
        if ( TListBoxItem::EUiExtension == 
            iContainer->ListBoxItem().iItemType )
            {
            aMenuPane->SetItemDimmed( ECSCCommandDelete, ETrue );
            aMenuPane->SetItemSpecific( ECSCCommandDelete, EFalse ); 
            }
        else
            {
            aMenuPane->SetItemDimmed( ECSCCommandDelete, EFalse );
            aMenuPane->SetItemSpecific( ECSCCommandDelete, ETrue );    
            }           
        }
        
    CSCDEBUG( "CCSCServiceView::DynInitMenuPaneL - end" );
    }

         
// ---------------------------------------------------------------------------
// From class CAknView.
// Creates the Container class object.
// ---------------------------------------------------------------------------
//
void CCSCServiceView::DoActivateL( const TVwsViewId& aPrevViewId,
                                   TUid /*aCustomMessageId*/,
                                   const TDesC8& /*aCustomMessage*/)
    {
    CSCDEBUG( "CCSCServiceView::DoActivateL - begin" );
              
    // Create container if it's not exists.
    if( !iContainer )
        {        
        iContainer = CCSCServiceContainer::NewL( 
                ClientRect(),
                *this,
                iServicePluginHandler, 
                iUiExtensionPluginHandler,
                iServiceHandler,
                iBrandingHandler,
                iCCHHandler );
                
        SetTitlePaneTextL();
        iContainer->SetMopParent( this );
        iContainer->SetListBoxObserver( this );
        iContainer->MakeVisible( ETrue );
        AppUi()->AddToStackL( *this, iContainer );
        iContainer->SetCurrentIndex( iCurrentIndex );
        }
     
    // Check startup parameters to see is there something to do with plugins.
    if ( iStartup )
        {
        iStartup = EFalse;        
        if ( KAIAppUid == aPrevViewId.iAppUid )
            {
            // launched from active idle
            iStartupHandler.SetStartedFromHomescreen( ETrue );
            }
        else
            {
            iStartupHandler.SetStartedFromHomescreen( EFalse );
            }
        }

    UpdateCbaL();
    
    CSCDEBUG( "CCSCServiceView::DoActivateL - end" );
    }


// ---------------------------------------------------------------------------
// From class CAknView.
// Deletes the Container class object.
// ---------------------------------------------------------------------------
//
void CCSCServiceView::DoDeactivate()
    {
    CSCDEBUG( "CCSCServiceView::DoDeactivate - begin" );
                    
    if ( iContainer )
        {
        iCurrentIndex = iContainer->CurrentItemIndex();
        AppUi()->RemoveFromStack( iContainer );
        delete iContainer; 
        iContainer = NULL;
        }
    
    CSCDEBUG( "CCSCServiceView::DoDeactivate - end" );
    }
	
	
// ---------------------------------------------------------------------------
// From class MCSCEngTimerObserver.
// CCSCServiceView::TimerExpired
// ---------------------------------------------------------------------------
//
void CCSCServiceView::TimerExpired()
    {
    CSCDEBUG( "CCSCServiceView::TimerExpired - begin" );
    
    iStartupHandler.ResetUid( CCSCEngStartupHandler::EPluginUid );
    iOfferedPluginUids.Append( iPluginInfo.iPluginsUid );
    
    CCSCNoteUtilities::TCSCNoteType 
        type = CCSCNoteUtilities::ECSCConfigureServiceQuery;
     
    TRAP_IGNORE
        (
        if ( CCSCNoteUtilities::ShowCommonQueryL( 
           type, iPluginInfo.iProviderName ) )
           {        
           iServicePluginHandler.DoProvisioningL( 
               iPluginInfo.iPluginsUid, KCSCServiceViewId );
           }
        );

    iNextPluginIndex++;
    TInt pluginCount = iServicePluginHandler.PluginCount( 
            CCSCEngServicePluginHandler::EInitialized );
    
    for ( ; iNextPluginIndex < pluginCount; iNextPluginIndex++ )
        {
        CSCDEBUG2( "CCSCServiceView::TimerExpired -iNextPluginIndex = %d",
            iNextPluginIndex );
        
        iPluginInfo =
            iServicePluginHandler.ItemFromPluginInfoArray( iNextPluginIndex );
        
        if ( KNullUid != iUid )
            {
            if ( iUid == iPluginInfo.iPluginsUid && !iPluginInfo.iProvisioned )
                {
                iEngTimer->StartTimer( CCSCEngTimer::ENoteDelayTimer );
                break;
                }
            }
        else
            {
            if ( !iPluginInfo.iProvisioned )
                {
                iEngTimer->StartTimer( CCSCEngTimer::ENoteDelayTimer );
                break;
                }
            }
        }
    CSCDEBUG( "CCSCServiceView::TimerExpired - end" );
    }
	
