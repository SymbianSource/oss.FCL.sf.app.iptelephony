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
* Description:  Implements CAcpProviderListView methods
*
*/


#include <eikenv.h>
#include <aknlists.h>
#include <akntitle.h>
#include <aknViewAppUi.h>
#include <StringLoader.h>
#include <apgcli.h>
#include <apgtask.h>
#include <centralrepository.h>
#include <accountcreationplugin.rsg>
#include <spnotifychange.h>
#include <spsettings.h>
#include <spentry.h>
#include <spproperty.h>
#include <AknQueryDialog.h>
#include <crcseprofileregistry.h>
#include <crcseprofileentry.h>
#include <CxSPViewData.h>
#include <featmgr.h>
#include <hlplch.h>

#include "acptimer.h"
#include "acpdialog.h"
#include "acpproviderlistview.h"
#include "acpproviderspecificview.h"
#include "acpproviderlistcontainer.h"
#include "accountcreationpluginlogger.h"
#include "acpcontroller.h"
#include "maccountcreationpluginobserver.h"
#include "accountcreationpluginconstants.h"
#include "accountcreationplugin.hrh"

// Constants for launching Help.
_LIT( KVOIP_HLP_SERVCATALOG, "VOIP_HLP_SERVCATALOG" );
const TUid KHelpUid = { 0x1020E566 };

// Cleanup function for RCSE arrays.
void CleanupResetAndDestroy( TAny* aPtr )
    {
    RPointerArray<CRCSEProfileEntry>* entries =
        static_cast<RPointerArray<CRCSEProfileEntry>*>( aPtr );
        
    entries->ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// CAcpProviderListView::CAcpProviderListView
// ---------------------------------------------------------------------------
//
CAcpProviderListView::CAcpProviderListView(
    MAccountCreationPluginObserver& aObserver, TBool aLaunchedFromAI,
            CEikonEnv& aEikEnv )
    : iObserver( aObserver ), iLaunchedFromAI( aLaunchedFromAI ), 
    iEikEnv( aEikEnv )
    {
    }

// ---------------------------------------------------------------------------
// CAcpProviderListView::ConstructL
// ---------------------------------------------------------------------------
//
void CAcpProviderListView::ConstructL()
    {
    ACPLOG( "CAcpProviderListView::ConstructL begin" );
 
    iUi = static_cast<CAknViewAppUi*> ( static_cast<CAknAppUi*> 
        ( iEikEnv.EikAppUi() ) );

    BaseConstructL( R_ACP_PROVIDER_LIST_VIEW );

    iController = CAcpController::NewL( *this );
    iDialog = CAcpDialog::NewL( *this );
    iTimer = CAcpTimer::NewL( *this );
    iServiceName = HBufC::NewL( 0 );
    
    // Get list of services.
    iSpSettings = CSPSettings::NewL();
    iSpSettings->FindServiceIdsL( iServiceIds );
    
    // Get tab view ids
    User::LeaveIfError( iXspViewServices.Open() );
    
    TInt err( KErrNone );
    TInt tabViewCount( 0 );
    
    err = iXspViewServices.GetViewCount( tabViewCount );
    
    for( TInt i( 0 ) ; i < tabViewCount && !err ; i++ )
        {
        TInt bufferLength( 0 );
        err = iXspViewServices.GetPackedViewDataBufferLength(
                i, bufferLength );
        
        ACPLOG( "CAcpProviderListView::ConstructL 6" );
        
        if( !err && bufferLength > 0 )
            {
            HBufC8* packed = HBufC8::NewLC( bufferLength );
            TPtr8 packedPtr = packed->Des();
            
            err = iXspViewServices.GetPackedViewData( i, packedPtr );
            
            if( !err )
                {     
                CxSPViewData* viewData = CxSPViewData::NewL( *packed );
                CleanupStack::PushL( viewData );
                
                ACPLOG2( "  --> VIEW ID: %d", viewData->OriginalViewId() );
                
                iTabViewIds.AppendL( viewData->OriginalViewId() );
                CleanupStack::PopAndDestroy( viewData );    
                }                                               
            CleanupStack::PopAndDestroy( packed );
            }
        }
    
    iXspViewServices.Close();
    
    ACPLOG( "CAcpProviderListView::ConstructL end" );
    }

// ---------------------------------------------------------------------------
// CAcpProviderListView::NewL
// ---------------------------------------------------------------------------
//      
CAcpProviderListView* CAcpProviderListView::NewL( 
    MAccountCreationPluginObserver& aObserver, TBool aLaunchedFromAI,
	        CEikonEnv& aEikEnv )
    {    
    CAcpProviderListView* self = CAcpProviderListView::NewLC( aObserver,
            aLaunchedFromAI, aEikEnv );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CAcpProviderListView::NewLC
// ---------------------------------------------------------------------------
//
CAcpProviderListView* CAcpProviderListView::NewLC(
    MAccountCreationPluginObserver& aObserver, TBool aLaunchedFromAI,
	        CEikonEnv& aEikEnv )
    {    
    CAcpProviderListView* self = 
        new ( ELeave ) CAcpProviderListView( aObserver, aLaunchedFromAI,
		                                     aEikEnv );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CAcpProviderListView::~CAcpProviderListView
// ---------------------------------------------------------------------------
//
CAcpProviderListView::~CAcpProviderListView()
    {
    ACPLOG( "CAcpProviderListView::~CAcpProviderListView begin" );

    // Delete container.
    if ( iContainer && iUi )
        {
        iUi->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        }

    delete iDialog;
    delete iController;
    delete iTimer;
    delete iSpSettings;
    iServiceIds.Close();    
    delete iServiceName;
    
    iTabViewIds.Close();
    iXspViewServices.Close();
    
    ACPLOG( "CAcpProviderListView::~CAcpProviderListView end" );
    }         

// ---------------------------------------------------------------------------
// CAcpProviderListView::ProvisioningL
// ---------------------------------------------------------------------------
//
void CAcpProviderListView::ProvisioningL()
    {
    ACPLOG( "CAcpProviderListView::ProvisioningL begin" );
    
    TInt index = iController->ActiveIndex();
    TPtrC8 sisUrl = iController->ProviderSisUrlFromIndexL( index );
    
    // Save service name.
    delete iServiceName;
    iServiceName = NULL;
    iServiceName = iController->ProviderNameFromIndexL( index ).AllocL();

	if ( sisUrl.Length() )
        {
        // Install sis package.
        ACPLOG( " - Fetching sis file" );
        
        iDialog->LaunchWaitNoteL( R_ACP_DOWNLOADING_PROVIDER_TEXT,
            *iServiceName );
        
        iController->FetchSisFileFromNetworkL( sisUrl );
        }
    // No sis url, get wbxml content instead.
    else
        {
        ACPLOG( " - Fetching wbxml content" );

        TBuf8<KMaxUrlLength> activationUrl;

        // Get activation URL. Insert session id if needed.
        if ( iController->SessionId().Length() )
            {
            // Change http://address?param=value to
            //        http://adress;jsessionid=[sessionid]?param=value

            TPtrC8 provActUrl = iController->
                ProviderActivationUrlFromIndexL( index );
            
            if ( KErrNotFound == provActUrl.Find( KSessionId ) )
                {
                // Find ? and insert the session id part before it.
                TInt qmIndex = provActUrl.Find( KQuestionMark8 );
                if ( KErrNotFound != qmIndex )
                    {
                    activationUrl.Append( provActUrl.Mid( 0, qmIndex ) );
                    activationUrl.Append( KSessionId );
                    activationUrl.Append( iController->SessionId() );
                    activationUrl.Append( provActUrl.Mid( qmIndex ) );
                    }
                }
            else
                {
                activationUrl.Append( provActUrl );
                }
            }
        else
            {
            // Error. Can't download settings without a session id.
            TRAP_IGNORE( iDialog->ShowGlobalNoteL(
                R_ACP_DOWNLOAD_FAILED, *iServiceName ) );
            
            iUi->ActivateLocalViewL( KProviderListViewId ); 
            return;
            }

        iDialog->LaunchWaitNoteL( R_ACP_DOWNLOADING_PROVIDER_TEXT,
            *iServiceName );
        
        iController->FetchActionFileL( activationUrl );
        }

    // Activate provider list view.
    iUi->ActivateLocalViewL( KProviderListViewId ); 
    }

// ---------------------------------------------------------------------------
// CAcpProviderListView::DownloadProviderListL
// Starts provider list download.
// ---------------------------------------------------------------------------
// 
void CAcpProviderListView::DownloadProviderListL()
    {
    iDialog->LaunchWaitNoteL( R_ACP_WAIT_NOTE_TEXT );
    iConnectingToServer = ETrue;
    iController->FetchProviderListFromNetworkL();
    }

// ---------------------------------------------------------------------------
// CAcpProviderListView::SetTitlePaneTextL
// For changing text to the title pane.
// ---------------------------------------------------------------------------
// 
void CAcpProviderListView::SetTitlePaneTextL() const
    {
    // Get handle to title pane.
    CEikStatusPane* statusPane = iEikonEnv->AppUiFactory()->StatusPane();
    if ( statusPane )
        {
        CAknTitlePane* titlePane =
            static_cast<CAknTitlePane*>( statusPane->ControlL(
                TUid::Uid( EEikStatusPaneUidTitle ) ) );

        // Set title pane text from resouces.   
        HBufC* titleText = StringLoader::LoadLC( R_ACP_PROVIDER_LIST_TITLE );
        titlePane->SetTextL( titleText->Des() );

        CleanupStack::PopAndDestroy( titleText );
        }
    }

// ---------------------------------------------------------------------------
// CAcpProviderListView::Id
// From class CAknView.
// ---------------------------------------------------------------------------
//
TUid CAcpProviderListView::Id() const
    {
    return KProviderListViewId;
    }

// ---------------------------------------------------------------------------
// CAcpProviderListView::DoActivateL
// From class CAknView.
// ---------------------------------------------------------------------------
//
void CAcpProviderListView::DoActivateL( 
    const TVwsViewId& /*aPrevViewId*/, 
    TUid /*aCustomMessageId*/,
    const TDesC8& /*aCustomMessage*/ )
    {
    ACPLOG( "CAcpProviderListView::DoActivateL begin" );

    // Create container when view is activated.
    if ( !iContainer )
        {
        iContainer = CAcpProviderListContainer::NewL(
            *iController, ClientRect() );
        iContainer->SetMopParent( this );
        iUi->AddToStackL( *this, iContainer );
        iContainer->ListBox()->SetListBoxObserver( this );
        }

    SetTitlePaneTextL();
    
    iContainer->AddProvidersToListboxL();

    ACPLOG( "CAcpProviderListView::DoActivateL end" );
    }

// ---------------------------------------------------------------------------
// CAcpProviderListView::DoDeactivate()
// From class CAknView.
// ---------------------------------------------------------------------------
//
void CAcpProviderListView::DoDeactivate()
    {
    ACPLOG( "CAcpProviderListView::DoDeactivate begin" );
    
    // Delete container when view is deactivated.
    if ( iContainer )
        {
        iUi->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = NULL;
        }

    ACPLOG( "CAcpProviderListView::DoDeactivate" );
    }

// ---------------------------------------------------------------------------
// CAcpProviderListView::HandleCommandL
// From class CAknView.
// ---------------------------------------------------------------------------
//
void CAcpProviderListView::HandleCommandL( TInt aCommand )
    {
    ACPLOG2( "CAcpProviderListView::HandleCommandL: cmd=%d", aCommand );

    TInt index = iContainer->CurrentItemIndex();

    ACPLOG2( " - index=%d", index );

    iController->SaveActiveIndex( index );

    switch ( aCommand )
        {
        case EAknSoftkeySelect:
            {
            ACPLOG( " - View details" );
            if ( KErrNotFound != index )
                {
                HandleListBoxSelectionL();
                }
            }
            break;

        case EAcpDownload:
            {
            ACPLOG( " - Download" );          
            ProvisioningL();
            }
            break;

        case EAcpRefresh:
            {
            ACPLOG( " - Refresh" );
            iContainer->ResetListBox();
            iDialog->LaunchWaitNoteL( R_ACP_WAIT_NOTE_TEXT );
            iController->FetchProviderListFromNetworkL();
            }
            break;
            
        case EAcpHelp:
            {
            ACPLOG( " - Help" );
            FeatureManager::InitializeLibL();
            if ( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
                {
                // Launch help.
                CArrayFixFlat<TCoeHelpContext>* array = 
                    new (ELeave) CArrayFixFlat<TCoeHelpContext>( 1 );  
                CleanupStack::PushL( array ); 

                array->AppendL( TCoeHelpContext( KHelpUid,
                    KVOIP_HLP_SERVCATALOG ) );
                
                HlpLauncher::LaunchHelpApplicationL(
                    iEikonEnv->WsSession(), array );
                
                CleanupStack::Pop( array ); // Ownership passed.
                }
            FeatureManager::UnInitializeLib();
            }
            break;

        case EAknSoftkeyBack:
        case EEikCmdExit:
        case EAknSoftkeyExit:
            {
            ACPLOG( " - Exit" );
            if ( iLaunchedFromAI )
                {
                AppUi()->HandleCommandL( EEikCmdExit );
                }
            else
                {
                iObserver.NotifyAiwEventL( KErrNone );
                }
            }
            break;

        default:
            break;
        }

    ACPLOG( "CAcpProviderListView::HandleCommandL end" );
    }

// ---------------------------------------------------------------------------
// CAcpProviderListView::HandleListBoxEventL
// From MEikListBoxObserver
// ---------------------------------------------------------------------------
//
void CAcpProviderListView::HandleListBoxEventL( 
    CEikListBox* /*aListBox*/, TListBoxEvent aEventType )
    {
    switch ( aEventType )
        {
        case EEventEnterKeyPressed:
        case EEventItemSingleClicked:
            HandleCommandL( EAknSoftkeySelect );
            break;
        default:
           break;
        }
    }

// ---------------------------------------------------------------------------
// CAcpProviderListView::HandleListBoxSelectionL
// From MEikListBoxObserver
// ---------------------------------------------------------------------------
//
void CAcpProviderListView::HandleListBoxSelectionL() 
    {
    // Set active list index to controller before activating next view.
    ACPLOG( "CAcpProviderListView::HandleListBoxSelectionL" );

    // Create provider specific view and activate it.
    if ( NULL == iProviderSpecificView )
        {
        iProviderSpecificView = 
            CAcpProviderSpecificView::NewL( *iController, *this );
        iUi->AddViewL( iProviderSpecificView ); // Ownership is transferred.
        }
    iUi->ActivateLocalViewL( iProviderSpecificView->Id() );
    }

// ---------------------------------------------------------------------------
// CAcpProviderListView::DynInitMenuPaneL
// From MEikListBoxObserver
// ---------------------------------------------------------------------------
//
void CAcpProviderListView::DynInitMenuPaneL( 
    TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    ACPLOG( "CAcpProviderListView::DynInitMenuPaneL begin" );

    if ( R_ACP_PROVIDER_LIST_MENU == aResourceId &&
        iController->CountOfProviders() == 0 )  
        {
        aMenuPane->SetItemDimmed( EAknSoftkeySelect, ETrue );
        aMenuPane->SetItemDimmed( EAcpDownload, ETrue );
        }

    ACPLOG( "CAcpProviderListView::DynInitMenuPaneL end" );
    }

// ---------------------------------------------------------------------------
// CAcpProviderListContainer::DialogDismissed
// From MAcpDialogObserver.
// ---------------------------------------------------------------------------
//
void CAcpProviderListView::DialogDismissedL( TInt aError )
    {
    ACPLOG2( "CAcpProviderListView::DialogDismissed (%d)", aError );

    iDialog->DestroyWaitNote();

    if ( KErrNotFound == aError ) // Canceled.
        {
        iController->CancelHttpRequest();
        }
    
    // If launched from active idle.
    if ( iLaunchedFromAI )
        {
        ACPLOG( "CAcpProviderListView::DialogDismissed - EXIT" );
        
        AppUi()->HandleCommandL( EEikCmdExit );
        }
    }

// ---------------------------------------------------------------------------
// CAcpProviderListContainer::NotifyProviderListReady
// From MAcpControllerObserver.
// ---------------------------------------------------------------------------
//
void CAcpProviderListView::NotifyProviderListReady( TInt aError )
    {
    ACPLOG2( 
        "CAcpProviderListView::NotifyProviderListReady: err=%d", aError );

    // Always remove wait dialog from screen.
    iDialog->DestroyWaitNote();

    if ( KErrNone == aError )
        {
        // List loaded, it's ok to activate the view now.
        TRAP_IGNORE( iUi->ActivateLocalViewL( Id() ) );
        
        if ( iContainer )
            {
            TRAP_IGNORE( iContainer->AddProvidersToListboxL() );
            }
        }
    else if ( KErrCancel == aError )
        {
        // No action needed.
        }
    else
        {
        if ( iConnectingToServer )
            {
            // Failed connecting to NSA server.
            TRAP_IGNORE( iDialog->ShowGlobalNoteL( R_ACP_CONNECTION_FAILED ) );
            }
        else
            {
            // Failed downloading SIS/wbxml file.
            TRAP_IGNORE( iDialog->ShowGlobalNoteL( R_ACP_DOWNLOAD_FAILED,
                *iServiceName ) );
            }
        }
    
    // Launched from active idle.
    if ( KErrNone != aError && iLaunchedFromAI )
        {
        ACPLOG( " - Launched from AI, exit." );
        TRAP_IGNORE( AppUi()->ProcessCommandL( EAknCmdExit ) );
        }
    
    iConnectingToServer = EFalse;
    }

// ---------------------------------------------------------------------------
// CAcpQueryView::NotifyDownloadingCompleted
// From MAcpControllerObserver.
// ---------------------------------------------------------------------------
//
void CAcpProviderListView::NotifyDownloadingCompleted( TInt aError )
    {
    ACPLOG( "CAcpProviderListView::NotifyDownloadingCompleted - IN" );
    
    // Destroy wait note if timer is not active
    if ( !iTimer->IsActive() )
        {
        iDialog->DestroyWaitNote();
        }
    
    // Show error note if needed.
    if ( KErrNone != aError )
        {
        TRAP_IGNORE( iDialog->ShowGlobalNoteL( R_ACP_DOWNLOAD_FAILED,
            *iServiceName ) );
        } 
    
    ACPLOG( "CAcpProviderListView::NotifyDownloadingCompleted - OUT" );
    }

// ---------------------------------------------------------------------------
// CAcpQueryView::NotifyDownloadingSISCompleted
// From MAcpControllerObserver.
// ---------------------------------------------------------------------------
//
void CAcpProviderListView::NotifyDownloadingSISCompleted( TDesC& aFileName )
    {
    ACPLOG( "CAcpProviderListView::NotifyDownloadingSISCompleted - IN" );
    
    iTimer->StopTimer();
    iDialog->DestroyWaitNote();
    iObserver.NotifySISDownloaded( aFileName );
    
    ACPLOG( "CAcpProviderListView::NotifyDownloadingSISCompleted - OUT" );
    }

// ---------------------------------------------------------------------------
// CAcpQueryView::NotifyProvisioningCompleted
// From MAcpControllerObserver.
// ---------------------------------------------------------------------------
//
void CAcpProviderListView::NotifyProvisioningCompleted()
    {
    ACPLOG( "CAcpProviderListView::NotifyProvisioningCompleted" );

    // Keep dialog in screen for additional time to give tab time
    // to shut down phonebook before attempting to open it again.
    iTimer->StartTimer(  CAcpTimer::EPhonebookStartupDelayTimer );
    }

// ---------------------------------------------------------------------------
// CAcpQueryView::DoNotifyProvisioningCompletedL
// Informs that service settings have been saved.
// ---------------------------------------------------------------------------
//
void CAcpProviderListView::DoNotifyProvisioningCompletedL()
    {
    ACPLOG( "CAcpProviderListView::DoNotifyProvisioningCompletedL begin" );
            
    ACPLOG( "   --> get new service id" );
    
    // Get service id of the newly provisioned service.
    RArray<TServiceId> newServiceIds;
    CleanupClosePushL( newServiceIds );
    iSpSettings->FindServiceIdsL( newServiceIds );
    
    TServiceId serviceId( 0 );
    for ( TInt i = 0; i < newServiceIds.Count(); i++ )
        {
        if ( KErrNotFound == iServiceIds.Find( newServiceIds[i] ) )
            {
            ACPLOG( "   --> new service id found" );
            
            serviceId = newServiceIds[i];
            break;
            }
        }
    
    CleanupStack::PopAndDestroy( &newServiceIds );
    
    if ( !serviceId )
        {
        ACPLOG( " - no new service id found" );
        // Remove wait note and leave
        iDialog->DestroyWaitNote();
        User::Leave( KErrNotFound );
        }
    
    // Get new view id
    TInt err( KErrNone );
    TInt tabViewCount( 0 );
    TInt newTabViewId( 0 );
    RArray<TInt32> tabViewIds;
    CleanupClosePushL( tabViewIds );
    
    ACPLOG( " - open view services" );
    
    err = iXspViewServices.Open();
    
    ACPLOG2( " - open view services ERR=%d", err );
         
    err = iXspViewServices.GetViewCount( tabViewCount );
    
    ACPLOG2( " - get view count: %d", tabViewCount );
        
    for( TInt i( 0 ) ; i < tabViewCount && !err ; i++ )
        {
        TInt bufferLength( 0 );
        err = iXspViewServices.GetPackedViewDataBufferLength(
                i, bufferLength );
            
        if( !err && bufferLength > 0 )
            {
            HBufC8* packed = HBufC8::NewLC( bufferLength );
            TPtr8 packedPtr = packed->Des();
                
            err = iXspViewServices.GetPackedViewData( i, packedPtr );
                
            if( !err )
                {     
                CxSPViewData* viewData = CxSPViewData::NewL( *packed );
                CleanupStack::PushL( viewData );
                    
                ACPLOG2( "  --> VIEW ID: %d", viewData->OriginalViewId() );
                    
                tabViewIds.AppendL( viewData->OriginalViewId() );
                CleanupStack::PopAndDestroy( viewData );    
                }                                               
            CleanupStack::PopAndDestroy( packed );
            }
        }
    
    for ( TInt i = 0; i < tabViewIds.Count(); i++ )
        {
        if ( KErrNotFound == iTabViewIds.Find( tabViewIds[ i ] ) )
            {
            ACPLOG( "   --> new tab view id found" );
                
            newTabViewId = tabViewIds[ i ];
            break;
            }
        }
    
    CleanupStack::PopAndDestroy( &tabViewIds );
    
    if ( !newTabViewId )
        {
        ACPLOG( " - no new tab view id found" );
        // Remove wait note and leave
        iDialog->DestroyWaitNote();
        User::Leave( KErrNotFound );
        }
     
    // Save account creation URL for the service.
    SaveAccountCreationUrlL( serviceId );
               
    // Remove wait note
    iDialog->DestroyWaitNote();
    
    // Show message query that informs service is installed
    if ( iLaunchedFromAI )
        {
        // If service installation was started from service widget
        // in homescreen.       
        iDialog->ShowMessageQueryL( 
            R_ACP_SERVICE_INSTALLED_FROM_WIDGET_NOTE,
            *iServiceName );
        }
    else
        {
        iDialog->ShowGlobalNoteL( 
            R_ACP_SERVICE_INSTALLED_NOTE,
            *iServiceName );
        }
         
    err = iXspViewServices.Activate( KPhoneBookTabUid.iUid, newTabViewId );
    
    ACPLOG2( " - activate phonebook tab ERR=%d", err );
    User::LeaveIfError( err );
    
    iXspViewServices.Close();
    
    ACPLOG( "CAcpProviderListView::DoNotifyProvisioningCompletedL end" );
    }

// ---------------------------------------------------------------------------
// CAcpProviderListView::SaveAccountCreationUrlL
// For saving account creation url to rcse.
// ---------------------------------------------------------------------------
// 
void CAcpProviderListView::SaveAccountCreationUrlL( TUint aServiceId )
    {
    TBuf8<KMaxUrlLength> creationUrl;
       
    // Get activation URL. Insert session id if needed.
    if ( iController->SessionId().Length() )
        {
        // Change http://address?param=value to
        //        http://adress;jsessionid=[sessionid]?param=value

        TPtrC8 provCreUrl = iController->
            ProviderCreationUrlFromIndexL( iController->ActiveIndex() );
           
        if ( KErrNotFound == provCreUrl.Find( KSessionId ) )
            {
            // Find ? and insert the session id part before it.
            TInt qmIndex = provCreUrl.Find( KQuestionMark8 );
            if ( KErrNotFound != qmIndex )
                {
                creationUrl.Append( provCreUrl.Mid( 0, qmIndex ) );
                creationUrl.Append( KSessionId );
                creationUrl.Append( iController->SessionId() );
                creationUrl.Append( provCreUrl.Mid( qmIndex ) );
                }
            }
        else
            {
            creationUrl.Copy( provCreUrl );
            }
        }
       
    CRCSEProfileRegistry* rcseRegistry = CRCSEProfileRegistry::NewLC();
    RPointerArray<CRCSEProfileEntry> entries;
    CleanupStack::PushL( TCleanupItem( CleanupResetAndDestroy, &entries ) );
       
    rcseRegistry->FindByServiceIdL( aServiceId, entries );
    if ( entries.Count() )
        {
        CRCSEProfileEntry* entry = entries[0];
        entry->iAccountCreationUrl.Copy( creationUrl );
           
        rcseRegistry->UpdateL( entry->iId, *entry );
        }
    else
        {
        User::Leave( KErrNotFound );
        }
       
    CleanupStack::PopAndDestroy( 2, rcseRegistry );
    }
    
// ---------------------------------------------------------------------------
// From MAcpControllerObserver.
// NotifySettingsSaved
// ---------------------------------------------------------------------------
//
void CAcpProviderListView::NotifySettingsSaved()
    {
    ACPLOG( "CAcpProviderListView::NotifySettingsSaved" );

    if ( !iTimer->IsActive() || !iDialog->IsActive() )
        {
        // Close only if timer is not active, if active
        // message query is being shown. Closing will be done when user
        // dismissed query.
        
        ACPLOG( "CAcpProviderListView::NotifySettingsSaved - EXIT" );
        
        TRAP_IGNORE( AppUi()->ProcessCommandL( EAknCmdExit ) )
        }
    }

// ---------------------------------------------------------------------------
// From MAcpTimerObserver.
// TimerExpired
// ---------------------------------------------------------------------------
//
void CAcpProviderListView::TimerExpired()
    {
    ACPLOG( "CAcpProviderListView::TimerExpired" );
        
    // Handle provisioning completed
    TRAPD( err, DoNotifyProvisioningCompletedL() )
    
    if ( err )
        {
        ACPLOG2( "CAcpProviderListView::TimerExpired - ERR=%d", err );
        
        TRAP_IGNORE( AppUi()->ProcessCommandL( EAknCmdExit ) )
        }
    }

// End of file.
