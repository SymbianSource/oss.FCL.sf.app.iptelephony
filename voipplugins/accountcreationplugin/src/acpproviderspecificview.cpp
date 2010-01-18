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
* Description:  Implements CAcpProviderSpecificView methods
*
*/


#include <aknlists.h>
#include <akntitle.h>
#include <akncontext.h> 
#include <aknViewAppUi.h>
#include <StringLoader.h>
#include <accountcreationplugin.rsg>
// Help launcher
#include <hlplch.h> 

#include "acpdialog.h"
#include "acpcontroller.h"
#include "acpproviderlistview.h"
#include "acpproviderspecificview.h"
#include "acpproviderspecificcontainer.h"
#include "accountcreationpluginlogger.h"
#include "accountcreationplugin.hrh"

// ---------------------------------------------------------------------------
// AcpProviderSpecificView::CAcpProviderSpecificView
// ---------------------------------------------------------------------------
//
CAcpProviderSpecificView::CAcpProviderSpecificView(
    CAcpController& aController, CAcpProviderListView& aProviderListView )
    : iController( aController ), iProviderListView( aProviderListView )
    {
    }

// ---------------------------------------------------------------------------
// CAcpProviderSpecificView::ConstructL
// ---------------------------------------------------------------------------
//
void CAcpProviderSpecificView::ConstructL()
    {
    ACPLOG( "CAcpProviderSpecificView::ConstructL begin" );

    iUi = static_cast<CAknViewAppUi*>( static_cast<CAknAppUi*> 
        ( iCoeEnv->AppUi() ) );

    BaseConstructL( R_ACP_PROVIDER_SPECIFIC_VIEW );

    ACPLOG( "CAcpProviderSpecificView::ConstructL end" );
    }

// ---------------------------------------------------------------------------
// CAcpProviderSpecificView::NewL
// ---------------------------------------------------------------------------
//      
CAcpProviderSpecificView* CAcpProviderSpecificView::NewL(
    CAcpController& aController, CAcpProviderListView& aProviderListView )
    {    
    CAcpProviderSpecificView* self = 
        CAcpProviderSpecificView::NewLC( aController, aProviderListView );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CAcpProviderSpecificView::NewLC
// ---------------------------------------------------------------------------
//
CAcpProviderSpecificView* CAcpProviderSpecificView::NewLC(
    CAcpController& aController, CAcpProviderListView& aProviderListView )
    {    
    CAcpProviderSpecificView* self = 
        new ( ELeave ) CAcpProviderSpecificView( 
        aController, aProviderListView );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CAcpProviderSpecificView::~CAcpProviderSpecificView
// ---------------------------------------------------------------------------
//
CAcpProviderSpecificView::~CAcpProviderSpecificView()
    {
    ACPLOG( "CAcpProviderSpecificView::~CAcpProviderSpecificView begin" );

    // Delete container when view is deactivated.
    if ( iContainer && iUi )
        {
        iUi->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        }

    ACPLOG( "CAcpProviderSpecificView::~CAcpProviderSpecificView end" );
    }         

// ---------------------------------------------------------------------------
// CAcpProviderSpecificView::SetTitlePaneTextL
// For changing text to the title pane.
// ---------------------------------------------------------------------------
// 
void CAcpProviderSpecificView::SetTitlePaneTextL( const TDesC& aText ) const
    {
    // Get handle to title pane.
    CEikStatusPane* statusPane = iEikonEnv->AppUiFactory()->StatusPane();

    if ( statusPane )
        {
        CAknTitlePane* titlePane =
            static_cast<CAknTitlePane*> ( statusPane->ControlL(
                TUid::Uid( EEikStatusPaneUidTitle ) ) );

            titlePane->SetTextL( aText ); // Set text. 
        }
    }

// ---------------------------------------------------------------------------
// CAcpProviderSpecificView::Id
// From CAknView.
// ---------------------------------------------------------------------------
//
TUid CAcpProviderSpecificView::Id() const
    {
    return KProviderSpecificViewId;
    }

// ---------------------------------------------------------------------------
// CAcpProviderSpecificView::HandleCommandL
// From CEikAppUi.
// ---------------------------------------------------------------------------
//
void CAcpProviderSpecificView::HandleCommandL( TInt aCommand )
    {
    ACPLOG2( "CAcpProviderSpecificView::HandleCommandL: cmd=%d", aCommand );

    switch ( aCommand )
        {
        case EAknSoftkeySelect:
            { 
            iProviderListView.ProvisioningL();         
            }
            break;
        case EAcpDownload:
            {
            iProviderListView.ProvisioningL();  
            }
            break;
        case EAknSoftkeyCancel:
            iUi->ActivateLocalViewL( KProviderListViewId );
            break;
        case EEikCmdExit:
        case EAknSoftkeyExit:
            iUi->HandleCommandL( aCommand );
            break;
        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// CAcpProviderSpecificView::DoActivateL
// From CAknView.
// ---------------------------------------------------------------------------
//
void CAcpProviderSpecificView::DoActivateL( 
    const TVwsViewId& /*aPrevViewId*/, 
    TUid /*aCustomMessageId*/,
    const TDesC8& /*aCustomMessage*/ )
    {
    ACPLOG( "CAcpProviderSpecificView::DoActivateL begin" );

    // Create container when view is activated.
    if ( !iContainer )
        {
        iContainer = CAcpProviderSpecificContainer::NewL(
            iController, ClientRect() );
        iContainer->SetMopParent( this );
        iUi->AddToStackL( *this, iContainer );
        }

    // Set provider specific name to title pane.
    TInt index = iController.ActiveIndex();
    SetTitlePaneTextL( iController.ProviderNameFromIndexL( index ) );

    ACPLOG( "CAcpProviderSpecificView::DoActivateL end" );
    }

// ---------------------------------------------------------------------------
// CAcpProviderSpecificView::DoDeactivate()
// From CAknView.
// ---------------------------------------------------------------------------
//
void CAcpProviderSpecificView::DoDeactivate()
    {
    ACPLOG( "CAcpProviderSpecificView::DoDeactivate begin" );

    // Delete container when view is deactivated.
    if ( iContainer )
        {
        iUi->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = NULL;
        }

    ACPLOG( "CAcpProviderSpecificView::DoDeactivate end" );
    }

// End of file.
