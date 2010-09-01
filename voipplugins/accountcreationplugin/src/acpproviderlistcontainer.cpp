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
* Description:  Implements CAcpProviderListContainer methods
*
*/


#include <aknlists.h>
#include <pathinfo.h>
#include <AknsUtils.h>
#include <acpicons.mbg>
#include <AknIconArray.h>
#include <StringLoader.h>
#include <accountcreationplugin.rsg>
#include <data_caging_path_literals.hrh>
#include <imageconversion.h>
#include <fbs.h>
#include <gulicon.h>
#include <AknIconUtils.h>
#include <eikclbd.h>

#include "acpcontroller.h"
#include "acpproviderlistcontainer.h"
#include "accountcreationpluginlogger.h"
#include "accountcreationpluginconstants.h"


const TInt KComponentControlCount = 1;

// ---------------------------------------------------------------------------
// CAcpProviderListContainer::CAcpProviderListContainer
// ---------------------------------------------------------------------------
//
CAcpProviderListContainer::CAcpProviderListContainer( 
    CAcpController& aController )
    : iController( aController )
    {
    }

// ---------------------------------------------------------------------------
// CAcpProviderListContainer::ConstructL
// ---------------------------------------------------------------------------
//
void CAcpProviderListContainer::ConstructL( const TRect& aRect )
    {
    ACPLOG( "CAcpProviderListContainer::ConstructL begin" );
    
    CreateWindowL();
    ConstructListBoxL();
    SetRect( aRect );
    ActivateL();
    
    ACPLOG( "CAcpProviderListContainer::ConstructL end" );
    }

// ---------------------------------------------------------------------------
// CAcpProviderListContainer::NewL
// ---------------------------------------------------------------------------
//      
CAcpProviderListContainer* CAcpProviderListContainer::NewL( 
    CAcpController& aController,
    const TRect& aRect )
    {    
    CAcpProviderListContainer* self = 
        CAcpProviderListContainer::NewLC( aController, aRect );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CAcpProviderListContainer::NewLC
// ---------------------------------------------------------------------------
//
CAcpProviderListContainer* CAcpProviderListContainer::NewLC(
    CAcpController& aController,
    const TRect& aRect )
    {    
    CAcpProviderListContainer* self = 
        new ( ELeave ) CAcpProviderListContainer( aController );
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    return self;
    }

// ---------------------------------------------------------------------------
// CAcpProviderListContainer::~CAcpProviderListContainer
// ---------------------------------------------------------------------------
//
CAcpProviderListContainer::~CAcpProviderListContainer()
    {
    ACPLOG( "CAcpProviderListContainer::~CAcpProviderListContainer begin" );
    delete iListBox;
    ACPLOG( "CAcpProviderListContainer::~CAcpProviderListContainer end" );
    }

// ---------------------------------------------------------------------------
// CAcpProviderListContainer::AddProvidersToListboxL
// Adds and updates listbox with providers.
// ---------------------------------------------------------------------------
//
void CAcpProviderListContainer::AddProvidersToListboxL( )
    {
    ACPLOG( "CAcpProviderListContainer::AddProvidersToListboxL begin" );

    CTextListBoxModel* model = iListBox->Model();
    MDesCArray* textArray = model->ItemTextArray();
    CDesCArray* listBoxItems = static_cast<CDesCArray*>( textArray );
    
    if ( !listBoxItems->Count() )
        {
        TInt count = iController.CountOfProviders(); // Amount of provider.
    
        LoadProviderIconsL( count ); // Load icons of providers.
    
        for ( TInt i = 0; i < count; i++ )
            {
            // Entry for listbox.
            TBuf<KMaxSettingLength> listBoxItemText( KNullDesC ); 
            // Get provider name for adding it to the list later on.
            TPtrC namePtr = iController.ProviderNameFromIndexL( i );
            // Formats the listbox entry with the index and the provider name.  
            listBoxItemText.Format( KListFormat, i,&namePtr );
            // Add the entry to the list.
            listBoxItems->AppendL( listBoxItemText );
            }
    
        iListBox->HandleItemAdditionL();
        iListBox->SetCurrentItemIndexAndDraw( 0 );
        }

    ACPLOG( "CAcpProviderListContainer::AddProvidersToListboxL end" );

    }

// ---------------------------------------------------------------------------
// CAcpProviderListContainer::ListBox
// Returns handle to the listbox.
// ---------------------------------------------------------------------------
//    
CAknSingleLargeStyleListBox* CAcpProviderListContainer::ListBox()
    {
    return iListBox;
    }

// ---------------------------------------------------------------------------
// CAcpProviderListContainer::CurrentItemIndex
// Returns index of selected listbox item. 
// ---------------------------------------------------------------------------
//
TInt CAcpProviderListContainer::CurrentItemIndex() const
    {
    return iListBox->CurrentItemIndex();
    }

// ---------------------------------------------------------------------------
// CAcpProviderListContainer::LoadDefaultIconsL
// Loads default icons.
// ---------------------------------------------------------------------------
//
void CAcpProviderListContainer::LoadProviderIconsL( TInt aCount )
    {
    ACPLOG( "CAcpProviderListContainer::LoadProviderIconsL begin" );

    if ( aCount > 0 )
        {
        // Delete previous icon array.
        CArrayPtr<CGulIcon>* oldArray = iListBox->ItemDrawer()->ColumnData()->
            IconArray();
        if ( oldArray )
            {
            iListBox->ItemDrawer()->ColumnData()->SetIconArray( NULL );
            delete oldArray;
            }
        
        // Create a new icon array.
        CAknIconArray* iconArray = new( ELeave ) CAknIconArray( aCount );

        for ( TInt i = 0; i < aCount; i++ )
            {
            // Try to get bitmap and mask for provider.
            CFbsBitmap* bitmap( NULL );
            CFbsBitmap* mask( NULL );
            
            iController.ProviderBitmapsFromIndexL( i, bitmap, mask );
 
            if ( !bitmap ) // Mask can be NULL.
                {
                // Default icon.
                LoadDefaultIconsL( iconArray );
                }
            else
                {
                // Provider icon.
                CGulIcon* providerIcon = CGulIcon::NewL( bitmap, mask );

                // Owned by provider.
                providerIcon->SetBitmapsOwnedExternally( ETrue );

                CleanupStack::PushL( providerIcon );
                // Insert into icon array.
                iconArray->AppendL( providerIcon );
                CleanupStack::Pop( providerIcon );
                }
            }

        // Set an icon array to the listbox.
        iListBox->ItemDrawer()->ColumnData()->SetIconArray( iconArray );
        }

    ACPLOG( "CAcpProviderListContainer::LoadProviderIconsL end" );
    }

// ---------------------------------------------------------------------------
// CAcpProviderListContainer::ResetListBox
// Resets the listbox.
// ---------------------------------------------------------------------------
//
void CAcpProviderListContainer::ResetListBox()
    {
    ACPLOG( "CAcpProviderListContainer::ResetListBox begin" );
    
    CDesCArrayFlat* array = static_cast<CDesCArrayFlat*>(
        iListBox->Model()->ItemTextArray() );
    
    if ( array )
        {
        array->Reset();
        }
    
    iListBox->SetCurrentItemIndexAndDraw( 0 );
    
    ACPLOG( "CAcpProviderListContainer::ResetListBox end" );
    }

// ---------------------------------------------------------------------------
// CAcpProviderListContainer::ConstructListBoxL
// Creates listbox item for provider list items.
// ---------------------------------------------------------------------------
//
void CAcpProviderListContainer::ConstructListBoxL()
    {
    ACPLOG( "CAcpProviderListContainer::ConstructListBoxL begin" );

    // Create listbox and array for listbox items.
    iListBox = new( ELeave ) CAknSingleLargeStyleListBox;
    iListBox->SetContainerWindowL( *this );
    iListBox->ConstructL( this, EAknGenericListBoxFlags );    
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, 
        CEikScrollBarFrame::EAuto );
    iListBox->Model()->SetOwnershipType( ELbmOwnsItemArray );

    CDesCArrayFlat* array = new ( ELeave ) CDesCArrayFlat( KArrayGranuality );
    iListBox->Model()->SetItemTextArray( array );

    // Set listbox background data when there are no providers available.
    HBufC* empty = StringLoader::LoadLC( R_ACP_PROVIDER_LIST_NO_PROVIDERS );
    iListBox->View()->SetListEmptyTextL( *empty );
    CleanupStack::PopAndDestroy( empty );

    ACPLOG( "CAcpProviderListContainer::ConstructListBoxL end" );
    }

// ---------------------------------------------------------------------------
// CAcpProviderListContainer::LoadDefaultIconsL
// Loads default icons.
// ---------------------------------------------------------------------------
//
void CAcpProviderListContainer::LoadDefaultIconsL( 
    CArrayPtr<CGulIcon>* aIconArray )
    {
    ACPLOG( "CAcpProviderListContainer::LoadDefaultIconsL begin" );

    // This when going to ROM (PhoneMemoryRootPath is used only for testing).
    TFileName fileName = 
        TParsePtrC( PathInfo::RomRootPath() ).Drive();
    fileName.Append( KDC_APP_BITMAP_DIR );
    fileName.Append( KIconFileName );

    // Just one icon for providers which does not have own icon.
    MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();

    CGulIcon* defaultIcon( NULL );
    TRAPD( err, defaultIcon = AknsUtils::CreateGulIconL( 
        skinInstance, 
        KAknsIIDDefault,
        fileName, 
        EMbmAcpiconsQgn_prop_set_conn_voip, 
        EMbmAcpiconsQgn_prop_set_conn_voip ) );

    if ( KErrNone != err )
        {
        ACPLOG2( "CreateGulIconL error: %d", err );
        }

    CleanupStack::PushL( defaultIcon );
    aIconArray->AppendL( defaultIcon );
    CleanupStack::Pop( defaultIcon );

    ACPLOG( "CAcpProviderListContainer::LoadDefaultIconsL end");
    }

// ---------------------------------------------------------------------------
// CAcpProviderListContainer::ComponentControl
// From class CoeControl
// ---------------------------------------------------------------------------
//
CCoeControl* CAcpProviderListContainer::ComponentControl( 
    TInt /*aIndex*/ ) const
    {
    return iListBox;
    }

// ---------------------------------------------------------------------------
// From class CoeControl
// CAcpProviderListContainer::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CAcpProviderListContainer::OfferKeyEventL( 
    const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    return iListBox->OfferKeyEventL( aKeyEvent, aType );
    }

// ---------------------------------------------------------------------------
// CAcpProviderListContainer::CountComponentControls
// From class CoeControl.
// ---------------------------------------------------------------------------
//
TInt CAcpProviderListContainer::CountComponentControls() const
    {
    return KComponentControlCount;
    }

// ---------------------------------------------------------------------------
// CAcpProviderListContainer::SizeChanged
// From class CoeControl.
// ---------------------------------------------------------------------------
//
void CAcpProviderListContainer::SizeChanged()
    {
    iListBox->SetRect( Rect() );
    }

// ---------------------------------------------------------------------------
// CAcpProviderListContainer::FocusChanged
// From class CoeControl.
// ---------------------------------------------------------------------------
//
void CAcpProviderListContainer::FocusChanged( TDrawNow aDrawNow )
    {
    CCoeControl::FocusChanged( aDrawNow );

    if ( iListBox )
        {
        iListBox->SetFocus( IsFocused() );
        }
    }

// ---------------------------------------------------------------------------
// CAcpProviderListContainer::HandleResourceChange
// From class CCoeControl.
// Called by framework when layout is changed.
// ---------------------------------------------------------------------------
//
void CAcpProviderListContainer::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange(aType);

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
