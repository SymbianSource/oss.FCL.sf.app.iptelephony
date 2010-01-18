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
* Description:  Implements CAcpProviderSpecificContainer methods
*
*/


#include <aknlists.h>
#include <stringloader.h>
#include <accountcreationplugin.rsg>
#include <AknTextSettingPage.h>
#include <AknPasswordSettingPage.h>

#include "acpqueryview.h"
#include "acpquerycontainer.h"
#include "accountcreationpluginlogger.h"
#include "accountcreationpluginconstants.h"

const TInt KComponentControlCount = 1;
_LIT( KAsterisks, "****" );

// ---------------------------------------------------------------------------
// CAcpQueryContainer::CAcpQueryContainer
// ---------------------------------------------------------------------------
//
CAcpQueryContainer::CAcpQueryContainer()
    {
    }

// ---------------------------------------------------------------------------
// CAcpQueryContainer::ConstructL
// ---------------------------------------------------------------------------
//
void CAcpQueryContainer::ConstructL( const TRect& aRect )
    {
    ACPLOG( "CAcpQueryContainer::ConstructL begin" );

    // Load "none" from resources to further use.
    iNone = StringLoader::LoadL( R_ACP_SETTING_PAGE_EMPTY_TEXT );

    // Default values for setting item texts.
    iUsername.Copy( *iNone );
    iPassword.Copy( *iNone );

    CreateWindowL();
    ConstructListBoxL();
    SetRect( aRect );
    ActivateL();

    ACPLOG( "CAcpQueryContainer::ConstructL end" );
    }

// ---------------------------------------------------------------------------
// CAcpQueryContainer::NewL
// ---------------------------------------------------------------------------
//      
CAcpQueryContainer* CAcpQueryContainer::NewL( const TRect& aRect )
    {    
    CAcpQueryContainer* self = CAcpQueryContainer::NewLC( aRect );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CAcpQueryContainer::NewLC
// ---------------------------------------------------------------------------
//
CAcpQueryContainer* CAcpQueryContainer::NewLC( const TRect& aRect )
    {    
    CAcpQueryContainer* self = new ( ELeave ) CAcpQueryContainer();
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    return self;
    }

// ---------------------------------------------------------------------------
// CAcpQueryContainer::~CAcpQueryContainer
// ---------------------------------------------------------------------------
//
CAcpQueryContainer::~CAcpQueryContainer()
    {
    ACPLOG( "CAcpQueryContainer::~CAcpQueryContainer begin" );
    delete iNone;
    delete iListBox;
    ACPLOG( "CAcpQueryContainer::~CAcpQueryContainer end" );
    }

// ---------------------------------------------------------------------------
// CAcpQueryContainer::ListBox
// Returns handle to the listbox.
// ---------------------------------------------------------------------------
//    
CAknSettingStyleListBox* CAcpQueryContainer::ListBox()
    {
    return iListBox;
    }

// ---------------------------------------------------------------------------
// CAcpQueryContainer::CurrentItemIndex
// Returns index of selected listbox item. 
// ---------------------------------------------------------------------------
//
TInt CAcpQueryContainer::CurrentItemIndex() const
    {
    return iListBox->CurrentItemIndex();
    }

// ---------------------------------------------------------------------------
// CAcpQueryContainer::ShowUsernameSettingPageL
// ---------------------------------------------------------------------------
//
void CAcpQueryContainer::ShowUsernameSettingPageL()
    {
    ACPLOG( "CAcpQueryContainer::ShowUsernameSettingPageL" );

    // Show text setting page.
    TBuf<KAcpTextPageLength> temporaryText ( iUsername );
    CAknTextSettingPage* dlg = new(ELeave) CAknTextSettingPage(
        R_ACP_USERNAME_SETTING_PAGE, temporaryText );
    if ( dlg->ExecuteLD(CAknSettingPage::EUpdateWhenAccepted ) )
        {
        iUsername.Copy( temporaryText );
        }
    // Update currently selected list item.
    UpdateListboxItemL( CurrentItemIndex() );     
    }

// ---------------------------------------------------------------------------
// CAcpQueryContainer::ShowPwordSettingPageL
// ---------------------------------------------------------------------------
//
void CAcpQueryContainer::ShowPwordSettingPageL()
    {
    ACPLOG( "CAcpQueryContainer::ShowPwordSettingPageL" );

    // Show password setting page.
    TBuf<KAcpPasswordPageLength> temporaryPassword ( KNullDesC );
    temporaryPassword.Copy( iPassword );
    TBuf<KAcpPasswordPageLength> oldPassword ( KNullDesC );
    temporaryPassword.Copy( iPassword );
    
    CAknSettingPage* dlg = new (ELeave) CAknAlphaPasswordSettingPage(
        R_ACP_PASSWORD_SETTING_PAGE, temporaryPassword, oldPassword );
    if ( dlg->ExecuteLD( CAknSettingPage::EUpdateWhenAccepted ) )
        {
        iPassword.Copy( temporaryPassword );
        } 
    // Update currently selected list item.
    UpdateListboxItemL( CurrentItemIndex() );     
    }

// ---------------------------------------------------------------------------
// CAcpQueryContainer::GetContainerDataL
// ---------------------------------------------------------------------------
//
void CAcpQueryContainer::GetContainerDataL( TDes& aUsername, TDes& aPassword )
    {
    // Setting item: Server username.
    if ( aUsername.Length() > iUsername.Length() )
        {
        User::Leave( KErrArgument );
        }
    aUsername.Copy( iUsername );

    // Setting item: Server password.
    if ( aPassword.Length() > iPassword.Length() )
        {
        User::Leave( KErrArgument );
        }
    if ( iPassword.Compare( *iNone ) )
        {
        aPassword.Copy( iPassword );
        }

    ACPLOG( "GetContainerDataL, Items appended" );
    }

// ---------------------------------------------------------------------------
// CAcpQueryContainer::ConstructListBoxL
// Creates listbox item for provider list items.
// ---------------------------------------------------------------------------
//
void CAcpQueryContainer::ConstructListBoxL()
    {
    ACPLOG( "CAcpQueryContainer::ConstructListBoxL begin" );

    // Create listbox and array for listbox items.
    iListBox = new( ELeave ) CAknSettingStyleListBox;
    iListBox->ConstructL( this, EAknListBoxSelectionList );    
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, 
        CEikScrollBarFrame::EAuto );
    iListBox->Model()->SetOwnershipType( ELbmOwnsItemArray );

    CDesCArrayFlat* array = new ( ELeave ) CDesCArrayFlat( KArrayGranuality );  
    iListBox->Model()->SetItemTextArray( array );

    // Fill container listbox with setting items.
    FillListboxWithItemsL();

    // Set listbox index to first item and draw.
    iListBox->HandleItemAdditionL();
    iListBox->SetCurrentItemIndexAndDraw( 0 );

    ACPLOG( "CAcpQueryContainer::ConstructListBoxL end" );
    }

// ---------------------------------------------------------------------------
// CAcpQueryContainer::FillListboxWithItemsL
// Fills listbox with items.
// ---------------------------------------------------------------------------
//
void CAcpQueryContainer::FillListboxWithItemsL()
    {
    ACPLOG("CAcpQueryContainer::FillListboxWithItemsL");

    // Get listbox items from model.
    CTextListBoxModel* model = iListBox->Model();
    MDesCArray* textArray = model->ItemTextArray();
    CDesCArray* listBoxItems = static_cast<CDesCArray*>( textArray );
    TBuf<KAcpSettingItemLength> listBoxItemText ( KNullDesC );          

    // Setting item: Server username.
    HBufC* caption = StringLoader::LoadLC( R_ACP_USERNAME_SETTING_PAGE_TEXT );
    listBoxItemText.Format(
        KAcpSettingItemTextFormat,
        caption,
        &iUsername );          
    listBoxItems->AppendL( listBoxItemText );
    CleanupStack::PopAndDestroy( caption );    
    ACPLOG( "FillListboxWithItemsL: Server username appended." );

    // Setting item: Server password.
    caption = StringLoader::LoadLC( R_ACP_PASSWORD_SETTING_PAGE_TEXT );
    listBoxItemText.Format(
        KAcpSettingItemTextFormat,
        caption,
        &iPassword );          
    listBoxItems->AppendL( listBoxItemText );
    CleanupStack::PopAndDestroy( caption );    
    ACPLOG( 
        "CAcpQueryContainer::FillListboxWithItemsL: Server password appended." 
        );
    }

// ---------------------------------------------------------------------------
// CAcpQueryContainer::UpdateListboxItemL
// Updates setting page items.
// ---------------------------------------------------------------------------
//
void CAcpQueryContainer::UpdateListboxItemL( const TInt aIndex )
    {
    ACPLOG("CAcpQueryContainer::UpdateListboxItemL begin");

    // Get listbox items from model.
    CTextListBoxModel* model = iListBox->Model();
    MDesCArray* textArray = model->ItemTextArray();
    CDesCArray* listBoxItems = static_cast<CDesCArray*>( textArray );

    // Remove edited item from the list.
    listBoxItems->Delete( aIndex );

    // Fill line variables with correct data.
    TBuf<KAcpSettingItemLength> firstLine( KNullDesC );
    TBuf<KAcpSettingItemLength> secondLine( KNullDesC );
    TBuf<KAcpSettingItemLength> listBoxItemText( KNullDesC );          
    FillListboxItemDataL( aIndex, firstLine, secondLine );

    // Create new lisbox item data and insert it to correct place.
    listBoxItemText.Format(
        KAcpSettingItemTextFormat,
        &firstLine,
        &secondLine );
    listBoxItems->InsertL( aIndex, listBoxItemText );

    // Update listbox.
    iListBox->HandleItemAdditionL(); 

    ACPLOG( "CAcpQueryContainer::UpdateListboxItemL end" );
    }

// ---------------------------------------------------------------------------
// CAcpQueryContainer::FillListboxItemDataL
// Sets selected listbox item with member data.
// ---------------------------------------------------------------------------
//
void CAcpQueryContainer::FillListboxItemDataL( const TInt aIndex,
    TDes& aFirstLine, TDes& aSecondLine )
    {
    HBufC* caption = NULL;
    switch ( aIndex )
        {
        // Setting item: Server username.
        case CAcpQueryView::EAcpGenericUsername:
            caption = StringLoader::LoadL( R_ACP_USERNAME_SETTING_PAGE_TEXT );
            aFirstLine.Copy( caption->Des() );
            delete caption;
            aSecondLine.Copy( iUsername );
            break;
        // Setting item: Server password.
        case CAcpQueryView::EAcpGenericPassword:
            caption = StringLoader::LoadL( R_ACP_PASSWORD_SETTING_PAGE_TEXT );
            aFirstLine.Copy( caption->Des() );
            delete caption;
            // Show asterisks instead of the password.
            if ( iPassword.Length() )
                {
                aSecondLine.Copy( KAsterisks );
                }
            else
                {
                aSecondLine.Copy( *iNone );
                }
            break;
        default:
            User::Leave( KErrNotSupported );
            break;
        }

    ACPLOG( "CAcpQueryContainer::FillListboxItemDataL" );
    ACPLOG2( "FillListboxItemDataL: List item filled: %d", aIndex );
    }

// ---------------------------------------------------------------------------
// CAcpQueryContainer::OfferKeyEventL
// From class CoeControl.
// ---------------------------------------------------------------------------
//
TKeyResponse CAcpQueryContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    TKeyResponse response = EKeyWasNotConsumed;
    if ( aType == EEventKey )
        {
        response = iListBox->OfferKeyEventL( aKeyEvent, aType );
        }
    return response;
    }

// ---------------------------------------------------------------------------
// CAcpQueryContainer::ComponentControl
// From class CoeControl.
// ---------------------------------------------------------------------------
//
CCoeControl* CAcpQueryContainer::ComponentControl( TInt aIndex ) const
    {
    switch ( aIndex )
        {
        case 0 :
            return iListBox;
        default:
            return 0;
        }
    }

// ---------------------------------------------------------------------------
// CAcpQueryContainer::CountComponentControls
// From class CoeControl.
// ---------------------------------------------------------------------------
//
TInt CAcpQueryContainer::CountComponentControls() const
    {
    return KComponentControlCount;
    }

// ---------------------------------------------------------------------------
// CAcpQueryContainer::SizeChanged
// From class CoeControl.
// ---------------------------------------------------------------------------
//
void CAcpQueryContainer::SizeChanged()
    {
    iListBox->SetRect( Rect() );
    }

// ---------------------------------------------------------------------------
// CAcpQueryContainer::FocusChanged
// From class CoeControl.
// ---------------------------------------------------------------------------
//
void CAcpQueryContainer::FocusChanged( TDrawNow aDrawNow )
    {
    CCoeControl::FocusChanged( aDrawNow );

    if ( iListBox )
        {
        iListBox->SetFocus( IsFocused() );
        }
    }

// ---------------------------------------------------------------------------
// CAcpQueryContainer::HandleResourceChange
// From class CoeControl.
// Called by framework when layout is changed.
// ---------------------------------------------------------------------------
//
void CAcpQueryContainer::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange( aType );

    // *****************************
    // ADDED FOR SCALABLE UI SUPPORT
    // *****************************
    if ( KEikDynamicLayoutVariantSwitch == aType )
        {
        TRect rect;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, rect );
        SetRect( rect );
        }
    }

// End of file.
