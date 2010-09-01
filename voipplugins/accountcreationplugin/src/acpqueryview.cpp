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
* Description:  Implements CAcpQueryView methods
*
*/


#include <eikenv.h>
#include <aknlists.h>
#include <akntitle.h>
#include <aknviewappui.h>
#include <stringloader.h>
#include <accountcreationplugin.rsg>
#include <BrowserUiSDKCRKeys.h>
#include <apgcli.h>
#include <apgtask.h>
#include <centralrepository.h>

#include "acpcontroller.h"
#include "acpqueryview.h"
#include "acpdialog.h"
#include "acpquerycontainer.h"
#include "acpproviderlistview.h"
#include "accountcreationpluginlogger.h"


// ---------------------------------------------------------------------------
// CAcpQueryView::CAcpQueryView
// ---------------------------------------------------------------------------
//
CAcpQueryView::CAcpQueryView( CAcpController& aController ) 
    : iController( aController )
    {
    }

// ---------------------------------------------------------------------------
// CAcpQueryView::ConstructL
// ---------------------------------------------------------------------------
//
void CAcpQueryView::ConstructL()
    {
    ACPLOG( "CAcpQueryView::ConstructL begin" );

    iEikEnv = CEikonEnv::Static();
    iUi = static_cast<CAknViewAppUi*> ( static_cast<CAknAppUi*> 
        ( iEikEnv->EikAppUi() ) );
    
    BaseConstructL( R_ACP_QUERY_VIEW );

    iDialog = CAcpDialog::NewL( *this ); 

    ACPLOG( "CAcpQueryView::ConstructL end" );
    }

// ---------------------------------------------------------------------------
// CAcpQueryView::NewL
// ---------------------------------------------------------------------------
//      
CAcpQueryView* CAcpQueryView::NewL( CAcpController& aController )
    {    
    CAcpQueryView* self = CAcpQueryView::NewLC( aController );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CAcpQueryView::NewLC
// ---------------------------------------------------------------------------
//
CAcpQueryView* CAcpQueryView::NewLC( CAcpController& aController )
    {    
    CAcpQueryView* self = new ( ELeave ) CAcpQueryView( aController );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CAcpQueryView::~CAcpQueryView
// ---------------------------------------------------------------------------
//
CAcpQueryView::~CAcpQueryView()
    {
    ACPLOG( "CAcpQueryView::~CAcpQueryView begin" );

    // Delete container when view is deactivated.
    if ( iContainer && iUi )
        {
        iUi->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        }

    delete iDialog;

    iEikEnv = NULL;

    ACPLOG( "CAcpQueryView::~CAcpQueryView end" );
    }         

// ---------------------------------------------------------------------------
// CAcpQueryView::SetTitlePaneTextL
// For changing text to the title pane.
// ---------------------------------------------------------------------------
// 
void CAcpQueryView::SetTitlePaneTextL( const TDesC& aText ) const
    {
    // Get handle to title pane.
    CEikStatusPane* statusPane = iEikonEnv->AppUiFactory()->StatusPane();
    if ( statusPane )
        {
        CAknTitlePane* titlePane =
            static_cast<CAknTitlePane*> ( statusPane->ControlL(
                TUid::Uid( EEikStatusPaneUidTitle ) ) );
        if ( titlePane )
            {
            titlePane->SetTextL( aText ); // Set text.
            }
        }
    }

// ---------------------------------------------------------------------------
// CAcpQueryView::Id
// From CAknView.
// ---------------------------------------------------------------------------
//
TUid CAcpQueryView::Id() const
    {
    return KQueryViewId;
    }

// ---------------------------------------------------------------------------
// CAcpQueryView::HandleCommandL
// From CEikAppUi.
// ---------------------------------------------------------------------------
//
void CAcpQueryView::HandleCommandL( TInt aCommand )
    {
    ACPLOG2( "CAcpQueryView::HandleCommandL: cmd=%d", aCommand );

    switch ( aCommand )
        {
        case EAknSoftkeyOk:
            {
            TInt index; // Indicates which provider to be activated

            TBuf<KSipUsername> username; // SIP username. 
            TBuf<KSipPassword> password; // SIP password.
            // URL for activating current provider.
            TBuf8<KMaxUrlLength> activationUrl;

            // Get current provider.
            index = iController.ActiveIndex();

            // Get username and password.
            iContainer->GetContainerDataL( username, password );

            // Get activation URL.
            activationUrl.Copy( 
                iController.ProviderActivationUrlFromIndexL( index ) );

            // Set username.
            activationUrl.Append( KAmperSand );
            activationUrl.Append( KUsername );
            activationUrl.Append( KPlacing );
            activationUrl.Append( username );

            // Set password.
            activationUrl.Append( KAmperSand );
            activationUrl.Append( KPassword );
            activationUrl.Append( KPlacing );
            activationUrl.Append( password );

            ACPLOG( "Launching browser");
            
            // Instantiate the repository for disabling 
            // all security warnings and data saving form of the browser.
            // Remember to restore the settings later on!!
            CRepository* brRepository = CRepository::NewLC( KCRUidBrowser );

            // Get original security warning setting.
            brRepository->Get( KBrowserNGShowSecurityWarnings, iWarning ); 

            // Get original data saving form setting.
            brRepository->Get( KBrowserFormDataSaving, iDataSaving );         

            // Set security warning off.
            brRepository->Set( KBrowserNGShowSecurityWarnings, 0 ); 

            // Set data saving form off.
            brRepository->Set( KBrowserFormDataSaving, 0 );         

            // Creates the application architecture server instance for 
            // getting new browser session.
            // Remember to close the session before going out of scope.
            RApaLsSession apaLsSession;
            CleanupClosePushL( apaLsSession );

            // Get current application list.
            TApaTaskList taskList( iEikEnv->WsSession() );
            
            // Get the browser session.
            TApaTask browserTask = taskList.FindApp( KCRUidBrowser );

            // Checks whether browser session exist or not.    
            if ( browserTask.Exists() )
                {
                // Brings the session to the foreground.
                browserTask.BringToForeground();
                // Get new buffer for sending the
                // activation request to server.
                HBufC8* param8 = HBufC8::NewLC( activationUrl.Length() );
                // Copy the activation URL.
                param8->Des().Append( activationUrl );
                // Try to activate provider earlier selected
                // by using the browser session.
                // UID not used.
                browserTask.SendMessage( TUid::Uid(0), *param8 );
                // Destroy the activation buffer.
                CleanupStack::PopAndDestroy( param8 );
                }
            else
                {
                // Browser instance does not exist but instance is needed
                // for activating provider via OTA.
                // Next connecting to the application architecture server
                // and try to start a new browser instance with the 
                // activation address.

                TBuf<KMaxUrlLength> activationUrl16; // The activation URL.
                activationUrl16.Copy( activationUrl );

                TThreadId threadId; // Browser instance's thread.

                // Check whether a handle number exist for the server.
                if ( !apaLsSession.Handle() )
                    {
                    // Connect to the server
                    User::LeaveIfError( apaLsSession.Connect() );
                    }

                // Launch browser and send the activation request to the server.
                User::LeaveIfError( 
                    apaLsSession.StartDocument( activationUrl16, 
                                                KCRUidBrowser, threadId ));
                }
            
            CleanupStack::PopAndDestroy( &apaLsSession );

            // Activate provider list view.
            iUi->ActivateLocalViewL( KProviderListViewId ); 
            }
            break;
        case EAknSoftkeySelect:
            {
            // Provisioning started.
            HandleListBoxSelectionL();
            }
            break;
        case EAknSoftkeyCancel:
            // Going back to the provider list view.
            iUi->ActivateLocalViewL( KProviderListViewId );
            break;
        case EEikCmdExit:
        case EAknSoftkeyExit:
            {
            // Creates a repository to restore the settings earlier
            // configured for browser.
            CRepository* repository = CRepository::NewLC( KCRUidBrowser );
            // Restore the security warnings.
            repository->Set( KBrowserNGShowSecurityWarnings, iWarning ); 
            // Restore the data saving form.
            repository->Set( KBrowserFormDataSaving, iDataSaving );         
            // Destroys the repository.
            CleanupStack::PopAndDestroy( repository );
            // Exit the application.
            iUi->HandleCommandL( aCommand );
            }
            break;
        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// CAcpQueryView::HandleListBoxEventL
// From MEikListBoxObserver.
// ---------------------------------------------------------------------------
//
void CAcpQueryView::HandleListBoxEventL( CEikListBox* /*aListBox*/, 
    TListBoxEvent aEventType )
    {
    switch ( aEventType )
        {
        case EEventEnterKeyPressed:
        case EEventItemDoubleClicked:
            HandleListBoxSelectionL();
            break;
        default:
           break;
        }
    }

// ---------------------------------------------------------------------------
// CAcpQueryView::HandleListBoxSelectionL
// From MEikListBoxObserver.
// ---------------------------------------------------------------------------
//
void CAcpQueryView::HandleListBoxSelectionL() 
    {
    ACPLOG2( "CAcpQueryView::HandleListBoxSelectionL: %d",
        iContainer->CurrentItemIndex() );

    switch ( iContainer->CurrentItemIndex() )
        {
        // Setting page item: Server username
        case EAcpGenericUsername:
            iContainer->ShowUsernameSettingPageL();
            break;
        // Setting page item: Server password
        case EAcpGenericPassword:
            iContainer->ShowPwordSettingPageL();
            break;
        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// CAcpProviderSpecificView::DynInitMenuPaneL
// From MEikListBoxObserver.
// ---------------------------------------------------------------------------
//
void CAcpQueryView::DynInitMenuPaneL( 
    TInt /*aResourceId*/, CEikMenuPane* /*aMenuPane*/ )
    {
    ACPLOG( "CAcpQueryView::DynInitMenuPaneL begin" );
    ACPLOG( "CAcpQueryView::DynInitMenuPaneL end" );
    }

// ---------------------------------------------------------------------------
// CAcpProviderListView::DialogDismissed
// From MAcpDialogObserver.
// ---------------------------------------------------------------------------
//
void CAcpQueryView::DialogDismissed( TInt /*aError*/ )
    {
    iDialog->DestroyWaitNote();
    }

// ---------------------------------------------------------------------------
// CAcpQueryView::DoActivateL
// From CAknView.
// ---------------------------------------------------------------------------
//
void CAcpQueryView::DoActivateL( 
    const TVwsViewId& /*aPrevViewId*/, 
    TUid /*aCustomMessageId*/,
    const TDesC8& /*aCustomMessage*/ )
    {
    ACPLOG( "CAcpQueryView::DoActivateL begin" );

    // Create container when view is activated.
    if ( !iContainer )
        {
        iContainer = CAcpQueryContainer::NewL( ClientRect() );
        iContainer->SetMopParent( this );
        iUi->AddToStackL( *this, iContainer );
        iContainer->ListBox()->SetListBoxObserver( this );
        }

    // Set provider specific name to title pane.
    TInt index = iController.ActiveIndex();
    TBuf<KAcpTitlePaneTextLength> name = 
        iController.ProviderNameFromIndexL( index );

    HBufC* caption = StringLoader::LoadLC( R_ACP_QUERY_TITLE_PANE_TEXT );
    name.Append( caption->Des() );

    SetTitlePaneTextL( name );

    ACPLOG( "set title pane" );

    CleanupStack::PopAndDestroy( caption );    

    ACPLOG( "CAcpQueryView::DoActivateL end" );
    }

// ---------------------------------------------------------------------------
// CAcpQueryView::DoDeactivate()
// From CAknView.
// ---------------------------------------------------------------------------
//
void CAcpQueryView::DoDeactivate()
    {
    ACPLOG( "CAcpQueryView::DoDeactivate begin" );

    // Delete container when view is deactivated.
    if ( iContainer )
        {
        iUi->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = NULL;
        }

    ACPLOG( "CAcpQueryView::DoDeactivate end" );
    }

// End of file.
