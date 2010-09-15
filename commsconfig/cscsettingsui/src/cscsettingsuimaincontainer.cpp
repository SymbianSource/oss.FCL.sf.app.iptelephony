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
* Description:  Implements CSCSettingsUiMainContainer methods
*
*/


#include <aknlists.h>
#include <coecntrl.h>
#include <cchclient.h>
#include <cmmanagerext.h>
#include <StringLoader.h>
#include <spdefinitions.h>
#include <cscsettingsui.rsg>
#include <aknlistquerydialog.h>
#include <cvimpstsettingsstore.h>
#include <csxhelp/voip.hlp.hrh>
#include <csc.rsg>
#include <aknnotedialog.h>
#include <aknnotewrappers.h>

#include "cscconstants.h"
#include "cscsettingsui.hrh"
#include "cscsettingsuimodel.h"
#include "cscsettingsuilogger.h"
#include "cscengservicehandler.h"
#include "cscsettingsuiconstants.h"
#include "cscengdestinationshandler.h"
#include "cipapputilsaddressresolver.h"
#include "cscsettingsuimaincontainer.h"
#include "cscengservicepluginhandler.h"
#include "cscnoteutilities.h"
#include "cscengsettingscleanupplugininterface.h"

// Format of the setting item.
_LIT( KCSCSettingsUiListItemTextFormat, "\t%S\t\t%S" );

_LIT( KDoubleBackSlash, "\\" );
_LIT( KEmptyPassword, "*****" );

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCSettingsUiMainContainer::CCSCSettingsUiMainContainer(
    CCSCSettingsUiModel& aModel )
    : iModel( aModel )
    {
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CCSCSettingsUiMainContainer::ConstructL(
    const TRect& aRect)
    {
    CSCSETUIDEBUG( "CCSCSettingsUiMainContainer::ConstructL - begin" );

    CreateWindowL();
    ConstructListBoxL();
    SetRect( aRect );
    ActivateL();

    CSCSETUIDEBUG( "CCSCSettingsUiMainContainer::ConstructL - end" );
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCSettingsUiMainContainer::~CCSCSettingsUiMainContainer()
    {
    CSCSETUIDEBUG( 
       "CCSCSettingsUiMainContainer::~CCSCSettingsUiMainContainer - begin" );
    
    iListBoxItemArray.Reset();
    iListBoxItemArray.Close();
    
    if ( iCaption )
        {
        delete iCaption;
        }
    
    delete iListBox;
    
    CSCSETUIDEBUG( 
       "CCSCSettingsUiMainContainer::~CCSCSettingsUiMainContainer - end" );
    }


// ---------------------------------------------------------------------------
// CCSCSettingsUiMainContainer::UpdateContainerL
// Updates container and redraws listbox items.
// ---------------------------------------------------------------------------
//
void CCSCSettingsUiMainContainer::UpdateContainerL()
    {
    CSCSETUIDEBUG( "CCSCSettingsUiMainContainer::UpdateContainerL - begin" );

    // Get listbox items from model.
    CTextListBoxModel* model = iListBox->Model();
    MDesCArray* textArray = model->ItemTextArray();
    CDesCArray* listBoxItems = static_cast<CDesCArray*>( textArray );
    listBoxItems->Reset();
    iListBoxItemArray.Reset();

    // Initialize setting items.
    InitializeSettingItemsL();

    CSCSETUIDEBUG( "CCSCSettingsUiMainContainer::UpdateContainerL - end" );
    }

// ---------------------------------------------------------------------------
// CCSCSettingsUiMainContainer::HandoverNotificationToneQueryL
// Shows a query which changes handover notification tone setting for service.
// ---------------------------------------------------------------------------
//
void CCSCSettingsUiMainContainer::HandoverNotificationToneQueryL()
    {    
    CDesCArrayFlat* items = new ( ELeave ) CDesCArrayFlat( 2 );   
    CleanupStack::PushL( items );

    HBufC* onItem =  StringLoader::LoadLC( 
            R_CSCSETTINGSUI_SETTING_HANDOVER_NOTIF_TONE_ON );
    items->AppendL( *onItem );
    CleanupStack::PopAndDestroy( onItem );

    HBufC* offItem =  StringLoader::LoadLC( 
            R_CSCSETTINGSUI_SETTING_HANDOVER_NOTIF_TONE_OFF );
    items->AppendL( *offItem );
    CleanupStack::PopAndDestroy( offItem );

    TInt index( 0 );
    CAknListQueryDialog* dialog = 
        new ( ELeave ) CAknListQueryDialog( &index );

    dialog->PrepareLC( R_CSCSETTINGSUI_HANDOVER_NOTIFICATION_TONE_QUERY );
    dialog->SetItemTextArray( items );
    dialog->SetOwnershipType( ELbmDoesNotOwnItemArray );

    if ( dialog->RunLD() )
        {
        // Update setting according user selection
        if ( 0 == index )
            {   
            // Set handover notification tone setting value on
            iModel.SettingsHandler().SetHandoverNotifTonePrefL( 
                iModel.CurrentSPEntryId(), EOn );
            }
        else if ( 1 == index )
            {
            // Set handover notification tone setting value off
            iModel.SettingsHandler().SetHandoverNotifTonePrefL( 
                iModel.CurrentSPEntryId(), EOff );
            }
        else
            {
            // invalid index
            User::Leave( KErrGeneral );
            }
        }
    else
        {        
        // cancelled
        }

    CleanupStack::PopAndDestroy( items );
    UpdateContainerL();
    }

// ---------------------------------------------------------------------------
// CCSCSettingsUiMainContainer::PresenceReqPrefQueryL
// Shows a query which changes presence request preference for service.
// ---------------------------------------------------------------------------
//
void CCSCSettingsUiMainContainer::PresenceReqPrefQueryL()
    {    
    CDesCArrayFlat* items = new ( ELeave ) CDesCArrayFlat( 2 );   
    CleanupStack::PushL( items );

    HBufC* alwaysAskItem =  StringLoader::LoadLC( 
            R_CSCSETTINGSUI_PRES_PREF_ALWAYS_ASK );
    items->AppendL( *alwaysAskItem );
    CleanupStack::PopAndDestroy( alwaysAskItem );

    HBufC* autoAcceptItem =  StringLoader::LoadLC( 
            R_CSCSETTINGSUI_PRES_PREF_ACCEPT_AUTOMATICALLY );
    items->AppendL( *autoAcceptItem );
    CleanupStack::PopAndDestroy( autoAcceptItem );

    TInt index( 0 );
    CAknListQueryDialog* dialog = 
        new ( ELeave ) CAknListQueryDialog( &index );

    dialog->PrepareLC( R_CSCSETTINGSUI_PRESENCE_REQUEST_PREF_QUERY );
    dialog->SetItemTextArray( items );
    dialog->SetOwnershipType( ELbmDoesNotOwnItemArray );

    if ( dialog->RunLD() )
        {
        // Update setting according user selection
        if ( 0 == index )
            {   
            // Set presence request preference setting value
            iModel.SettingsHandler().SetPresenceReqPrefL( 
                iModel.CurrentSPEntryId(), EOff );
            }
        else if ( 1 == index )
            {
            // Set presence request preference setting value
            iModel.SettingsHandler().SetPresenceReqPrefL( 
                iModel.CurrentSPEntryId(), EOn );
            }
        else
            {
            // invalid index
            User::Leave( KErrGeneral );
            }
        }
    else
        {        
        // canceled
        }

    CleanupStack::PopAndDestroy( items );
    UpdateContainerL();
    }

// ---------------------------------------------------------------------------
// CCSCSettingsUiMainContainer::SaveImTonePathL
// Saves Im tone path to permanent storage.
// ---------------------------------------------------------------------------
//
void CCSCSettingsUiMainContainer::SaveImTonePathL( const TDesC& aTonePath )
    { 
    MVIMPSTSettingsStore* settings = CVIMPSTSettingsStore::NewLC();

    User::LeaveIfError( settings->SetL( 
        iModel.CurrentSPEntryId(), EServiceToneFileName, aTonePath ) );

    CleanupStack::PopAndDestroy();
    }

// ---------------------------------------------------------------------------
// CCSCSettingsUiMainContainer::ListBox
// Returns handle to the listbox.
// ---------------------------------------------------------------------------
//    
CAknSettingStyleListBox* CCSCSettingsUiMainContainer::ListBox()
    {
    return iListBox;
    }

// ---------------------------------------------------------------------------
// CCSCSettingsUiMainContainer::CurrentItemIndex
// Returns index of selected listbox item. 
// ---------------------------------------------------------------------------
//
TMainListBoxItem CCSCSettingsUiMainContainer::CurrentItem() const
    {
    TInt currentItemIndex( iListBox->CurrentItemIndex() );
    return iListBoxItemArray[ currentItemIndex ];
    }

// ---------------------------------------------------------------------------
// From class CoeControl
// CCSCSettingsUiMainContainer::ComponentControl
// ---------------------------------------------------------------------------
//
CCoeControl* CCSCSettingsUiMainContainer::ComponentControl( 
    TInt /*aIndex*/ ) const
    {
    return iListBox;
    }

// -----------------------------------------------------------------------------
// From class CoeControl
// For getting help context
// -----------------------------------------------------------------------------
//
void CCSCSettingsUiMainContainer::GetHelpContext( 
    TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = KCscHelpUid;
    aContext.iContext = KSERVTAB_HLP_SETTINGS;
    }

// ---------------------------------------------------------------------------
// From class CoeControl
// CCSCSettingsUiMainContainer::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CCSCSettingsUiMainContainer::OfferKeyEventL( 
    const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    TKeyResponse response = EKeyWasNotConsumed;
    response = iListBox->OfferKeyEventL( aKeyEvent, aType );

    if ( EKeyUpArrow == aKeyEvent.iCode ||  
        EKeyDownArrow == aKeyEvent.iCode  )
        {
        iModel.UpdateSoftkeys();
        }

    return response;
    }


// ---------------------------------------------------------------------------
// From class CoeControl
// CCSCSettingsUiMainContainer::HandleResourceChange
// ---------------------------------------------------------------------------
//
void CCSCSettingsUiMainContainer::HandleResourceChange( TInt aType )
    {
    if( aType == KAknsMessageSkinChange ||
        aType == KEikDynamicLayoutVariantSwitch )
        {
        TRect mainPaneRect;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, 
                                           mainPaneRect );
        SetRect( mainPaneRect );
        DrawNow();
        }

    CCoeControl::HandleResourceChange( aType );
    }


// ---------------------------------------------------------------------------
// CCSCSettingsUiMainContainer::ConstructListBoxL
// Creates listbox item for setting items.
// ---------------------------------------------------------------------------
//
void CCSCSettingsUiMainContainer::ConstructListBoxL()
    {
    CSCSETUIDEBUG( "CCSCSettingsUiMainContainer::ConstructListBoxL - begin" );

    // Create listbox and array for listbox items.
    iListBox = new( ELeave ) CAknSettingStyleListBox;
    iListBox->ConstructL( this, EAknListBoxSelectionList );    
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, 
        CEikScrollBarFrame::EAuto );
    iListBox->Model()->SetOwnershipType( ELbmOwnsItemArray );
    CDesCArrayFlat* itemsArray = 
        new ( ELeave ) CDesCArrayFlat( KCSCSettingsUiArrayGranularity );  
    iListBox->Model()->SetItemTextArray( itemsArray );

    // Initialize setting items.
    InitializeSettingItemsL();

    CSCSETUIDEBUG( "CCSCSettingsUiMainContainer::ConstructListBoxL - end" );
    }


// ---------------------------------------------------------------------------
// CCSCSettingsUiMainContainer::InitializeSettingItemsL
// Draws setting items at the first time after the initialization is completed
// ---------------------------------------------------------------------------
//
void CCSCSettingsUiMainContainer::InitializeSettingItemsL()
    {
    CSCSETUIDEBUG( 
        "CCSCSettingsUiMainContainer::InitializeSettingItemsL - begin" );

    // Get supported subservices
    TSupportedSubServices supSubServices;
    iModel.CCHHandler().SupportedSubServicesL( 
            iModel.CurrentSPEntryId(), supSubServices );

    // Make username setting
    MakeSettingItemL ( TMainListBoxItem::EUsername );

    // Make password setting
    MakeSettingItemL ( TMainListBoxItem::EPassword );

    // Make preferred service setting item if voip subservice is supported
    if ( supSubServices.iVoIP )
        {
        TBool isSipVoip( EFalse );     
        TRAPD( err, isSipVoip = iModel.SettingsHandler().IsSipVoIPL(
            iModel.CurrentSPEntryId() ) );
           
        if ( !err )
            {
            if ( isSipVoip && iModel.SettingsHandler().IsVccSupportedL( 
                iModel.CurrentSPEntryId() ) )
                {
                // If VCC is supported make vcc preferred setting item
                // and handover notification tone setting item
                MakeSettingItemL ( TMainListBoxItem::EVccPreferredService );
                MakeSettingItemL ( TMainListBoxItem::EHandoverNotifTone );
                }
            else
                {
                // If VCC is not supported make just preferred service setting
                MakeSettingItemL ( TMainListBoxItem::EPreferredService );
                }
            }
        }

    // Make setting item if IM is supported by service
    if ( supSubServices.iIm )
        {
        // IM tone setting.
        MakeSettingItemL( TMainListBoxItem::EImTone );
        }

    // Make setting item if presence is supported by service
    if ( supSubServices.iPresence )
        {
        // Presence request preference
        MakeSettingItemL( TMainListBoxItem::EAutoacceptInv );
        }

    // Connectivity setting (cannot be changed)
    MakeSettingItemL( TMainListBoxItem::EServiceConn );

    CSCSETUIDEBUG( 
        "CCSCSettingsUiMainContainer::InitializeSettingItemsL - end" );
    }

// ---------------------------------------------------------------------------
// CCSCSettingsUiMainContainer::MakeSettingItemL
// Constructs setting list items.
// ---------------------------------------------------------------------------
//
void CCSCSettingsUiMainContainer::MakeSettingItemL(
    TMainListBoxItem::TSettingItems aItem )
    {
    CSCSETUIDEBUG( 
        "CCSCSettingsUiMainContainer::MakeSettingItemL - begin" );

    // Get listbox items from model.
    CTextListBoxModel* model = iListBox->Model();
    MDesCArray* textArray = model->ItemTextArray();
    CDesCArray* listBoxItems = static_cast<CDesCArray*>( textArray );

    TBuf<KCSCSettingsUiItemLength> listBoxItemText ( KNullDesC );

    TMainListBoxItem listBoxItem;
    listBoxItem.iItem = aItem;

    RBuf value;
    CleanupClosePushL( value );

    value.CreateL( 1 );
    value.Copy( KNullDesC );

    switch ( aItem  )
        {
        case TMainListBoxItem::EUsername:
            {
            GetUsernameL( value );
            break;
            }
        case TMainListBoxItem::EPassword:
            {
            value.ReAllocL( KEmptyPassword().Length() );
            value.Copy( KEmptyPassword );
            break;
            }
        case TMainListBoxItem::EPreferredService:
            {
            GetPreferredServiceSettingL( value );
            break;
            }
        case TMainListBoxItem::EVccPreferredService:
            {
            GetVccPreferredServiceSettingL( value );
            break;
            }
        case TMainListBoxItem::EHandoverNotifTone:
            {
            GetHandoverNotificationTonePrefL( value );
            break;
            }
        case TMainListBoxItem::EImTone:
            {
            GetImToneSettingL( value );
            break;
            }
        case TMainListBoxItem::EAutoacceptInv:
            {
            GetPresencePrefSettingL( value );
            break;
            }
        case TMainListBoxItem::EServiceConn:
            {
            TRAP_IGNORE( GetSnapSettingL( value ) );
            break;
            }
        default:
            User::Leave( KErrArgument );
            break;
        }

    listBoxItemText.Format(
            KCSCSettingsUiListItemTextFormat,
            GetCaptionL( aItem ),
            &value );

    CleanupStack::PopAndDestroy( &value );

    // Add to listbox
    iListBoxItemArray.Append( listBoxItem );
    listBoxItems->AppendL( listBoxItemText );
    iListBox->HandleItemAdditionL();

    CSCSETUIDEBUG( 
        "CCSCSettingsUiMainContainer::MakeSettingItemL - end" );
    }

// ---------------------------------------------------------------------------
// CCSCSettingsUiMainContainer::GetCaptionL
// Constructs setting list items.
// ---------------------------------------------------------------------------
//
HBufC* CCSCSettingsUiMainContainer::GetCaptionL( 
    TMainListBoxItem::TSettingItems aItem )
    {
    if ( iCaption != NULL )
        {
        delete iCaption;
        iCaption = NULL;
        }

    switch ( aItem  )
        {
        case TMainListBoxItem::EUsername:
            {
            iCaption = StringLoader::LoadL(
                R_CSCSETTINGSUI_SETTING_USERNAME_TITLE );
            break;
            }
        case TMainListBoxItem::EPassword:
            {
            iCaption = StringLoader::LoadL(
                R_CSCSETTINGSUI_SETTING_PASSWORD_TITLE );
            break;
            }
        case TMainListBoxItem::EPreferredService:
            {
            iCaption = StringLoader::LoadL(
                R_CSCSETTINGSUI_SETTING_PREFERRED_SERVICE_TITLE );
            break;
            }
        case TMainListBoxItem::EVccPreferredService:
            {
            iCaption = StringLoader::LoadL(
                R_CSCSETTINGSUI_SETTING_VCC_PREFERRED_SERVICE_TITLE );
            break;
            }
        case TMainListBoxItem::EHandoverNotifTone:
            {
            iCaption = StringLoader::LoadL(
                R_CSCSETTINGSUI_SETTING_HANDOVER_NOTIF_TONE_TITLE );
            break;
            }
        case TMainListBoxItem::EImTone:
            {
            iCaption = StringLoader::LoadL( 
                R_CSCSETTINGSUI_SETTING_IM_TONE_TITLE );
            break;
            }
        case TMainListBoxItem::EAutoacceptInv:
            {
            iCaption = StringLoader::LoadL( 
                R_CSCSETTINGSUI_PRESENCE_REQUEST_PREF_TEXT );
            break;
            }
        case TMainListBoxItem::EServiceConn:
            {
            iCaption = StringLoader::LoadL( 
                R_CSCSETTINGSUI_SERVICE_CONNECTIVITY_TEXT );
            break;
            }
        default:
            User::Leave( KErrArgument );
            break;
        }

    return iCaption;
    }
 
// ---------------------------------------------------------------------------
// CCSCSettingsUiMainContainer::GetUsername
// ---------------------------------------------------------------------------
//
void CCSCSettingsUiMainContainer::GetUsernameL( RBuf& aUsername )
    {
    aUsername.ReAllocL( KCCHMaxUsernameLength );
    User::LeaveIfError( iModel.CCHHandler().GetConnectionParameter( 
        iModel.CurrentSPEntryId(), ECchUsername, aUsername ) );
    }

// ---------------------------------------------------------------------------
// CCSCSettingsUiMainContainer::GetPresencePrefSettingL
// ---------------------------------------------------------------------------
//
void CCSCSettingsUiMainContainer::GetPreferredServiceSettingL( RBuf& aValue )
    {
    // Select text resource for the current pref telephony value
    TInt resource = ( iModel.SettingsHandler().IsPreferredTelephonyVoip() &&
        iModel.SettingsHandler().IsPreferredService(
        iModel.CurrentSPEntryId() ) ) ?
        R_CSCSETTINGSUI_SETTING_PREFERRED_SERVICE_ON :
        R_CSCSETTINGSUI_SETTING_PREFERRED_SERVICE_OFF;

    HBufC* value = StringLoader::LoadLC( resource );
    aValue.ReAllocL( value->Length() );
    aValue.Copy( value->Des() );
    CleanupStack::PopAndDestroy( value );
    }

// ---------------------------------------------------------------------------
// CCSCSettingsUiMainContainer::GetVccPreferredServiceSettingL
// ---------------------------------------------------------------------------
//
void CCSCSettingsUiMainContainer::GetVccPreferredServiceSettingL( RBuf& aValue )
    {
    // Select text resource for the current pref telephony value
    TInt resource = ( iModel.SettingsHandler().IsPreferredTelephonyVoip() &&
        iModel.SettingsHandler().IsPreferredService(
        iModel.CurrentSPEntryId() ) ) ?
        R_CSCSETTINGSUI_SETTING_VCC_PREFERRED_SERVICE_ON :
        R_CSCSETTINGSUI_SETTING_VCC_PREFERRED_SERVICE_OFF;

    HBufC* value = StringLoader::LoadLC( resource );
    aValue.ReAllocL( value->Length() );
    aValue.Copy( value->Des() );
    CleanupStack::PopAndDestroy( value );
    }

// ---------------------------------------------------------------------------
// CCSCSettingsUiMainContainer::GetHandoverNotificationToneL
// ---------------------------------------------------------------------------
//
void CCSCSettingsUiMainContainer::GetHandoverNotificationTonePrefL( 
    RBuf& aValue )
    {
    HBufC* value = NULL;
    TOnOff onOff = EOff;

    TRAPD( err, onOff = iModel.SettingsHandler().HandoverNotifTonePrefL( 
        iModel.CurrentSPEntryId() ) );

    if ( KErrNotFound == err )
        {
        iModel.SettingsHandler().SetHandoverNotifTonePrefL( 
            iModel.CurrentSPEntryId(), EOff );
        }
    else if ( err )
        {
        User::Leave( err );    
        }
    else
        {
        // nothing to do
        }

    if ( EOff == onOff )
        {
        value = StringLoader::LoadLC( 
            R_CSCSETTINGSUI_SETTING_HANDOVER_NOTIF_TONE_OFF );
        }
    else if ( EOn == onOff)
        {            
        value = StringLoader::LoadLC( 
            R_CSCSETTINGSUI_SETTING_HANDOVER_NOTIF_TONE_ON );
        }
    else if ( EOONotSet == onOff )
        {            
        iModel.SettingsHandler().SetHandoverNotifTonePrefL( 
            iModel.CurrentSPEntryId(), EOff );
        
        value = StringLoader::LoadLC( 
            R_CSCSETTINGSUI_SETTING_HANDOVER_NOTIF_TONE_OFF );
        }
    else
        {
        User::Leave( KErrGeneral );     
        }

    aValue.ReAllocL( value->Length() );
    aValue.Copy( value->Des() );

    CleanupStack::PopAndDestroy( value );
    }

// ---------------------------------------------------------------------------
// CCSCSettingsUiMainContainer::GetImToneSettingL
// ---------------------------------------------------------------------------
//
void CCSCSettingsUiMainContainer::GetImToneSettingL( RBuf& aValue )
    {
    aValue.ReAllocL( KCSCMaxImToneLength );

    MVIMPSTSettingsStore* settings = CVIMPSTSettingsStore::NewLC();

    TInt err = settings->GetL( 
        iModel.CurrentSPEntryId(), EServiceToneFileName, aValue );

    // If tone path is not found from settings, set Off text
    if ( KErrNotFound == err || aValue.Length() < 2 )
        {
        HBufC* noToneSelected = StringLoader::LoadLC( 
            R_CSCSETTINGSUI_IM_TONE_OFF );

        User::LeaveIfError( settings->SetL( 
            iModel.CurrentSPEntryId(), EServiceToneFileName, *noToneSelected ) );       

        // Get tone.
        User::LeaveIfError( settings->GetL( 
            iModel.CurrentSPEntryId(), EServiceToneFileName, aValue ) );

        CleanupStack::PopAndDestroy( noToneSelected );
        }
    else if ( err )
        {        
        User::Leave( err );
        }
    else
        {
        // KErrNone -> do nothing
        }

    TInt pos( 0 );    
    while ( KErrNotFound != pos )
        {
        pos = aValue.Find( KDoubleBackSlash );
        aValue.Delete( 0, pos+1 );
        }

    CleanupStack::PopAndDestroy();
    }

// ---------------------------------------------------------------------------
// CCSCSettingsUiMainContainer::GetPresencePrefSettingL
// ---------------------------------------------------------------------------
//
void CCSCSettingsUiMainContainer::GetPresencePrefSettingL( RBuf& aValue )
    {
    HBufC* value = NULL;
    TOnOff onOff = EOff;

    TRAPD( err, onOff = iModel.SettingsHandler().PresenceReqPrefL( 
            iModel.CurrentSPEntryId() ) );

    if ( KErrNotFound == err )
        {
        iModel.SettingsHandler().SetPresenceReqPrefL( 
                iModel.CurrentSPEntryId(), EOff );
        }
    else if ( err )
        {
        User::Leave( err );    
        }
    else
        {
        // nothing to do
        }

    if ( EOff == onOff )
        {
        value = StringLoader::LoadLC( 
            R_CSCSETTINGSUI_PRES_PREF_ALWAYS_ASK );
        }
    else if ( EOn == onOff)
        {            
        value = StringLoader::LoadLC( 
            R_CSCSETTINGSUI_PRES_PREF_ACCEPT_AUTOMATICALLY );
        }
    else if ( EOONotSet == onOff )
        {            
        // Set presence request preference setting default value "off"
        iModel.SettingsHandler().SetPresenceReqPrefL( 
            iModel.CurrentSPEntryId(), EOff );
        
        value = StringLoader::LoadLC( 
            R_CSCSETTINGSUI_PRES_PREF_ALWAYS_ASK );
        }
    else
        {
        User::Leave( KErrGeneral );     
        }

    aValue.ReAllocL( value->Length() );
    aValue.Copy( value->Des() );

    CleanupStack::PopAndDestroy( value ); 
    }

// ---------------------------------------------------------------------------
// CCSCSettingsUiMainContainer::GetSnapSettingL
// ---------------------------------------------------------------------------
//
void CCSCSettingsUiMainContainer::GetSnapSettingL( RBuf& aValue )
    {
    // Get Snap name
    HBufC* snapName = NULL;
    TInt snapId( 0 );
    TInt err = iModel.CCHHandler().GetConnectionParameter( 
       iModel.CurrentSPEntryId(), ECchSnapId, snapId );       

    // If no error, try to get snap name.
    if ( KErrNone == err )
        {
        snapName = iModel.DestinationsHandler().SnapNameL( snapId );
        
        if ( snapName )
           {
           CleanupStack::PushL( snapName );
           aValue.ReAllocL( snapName->Length() );
           aValue.Copy( snapName->Des() );
           CleanupStack::PopAndDestroy( snapName );
           }
        }
    }

// ---------------------------------------------------------------------------
// From class CoeControl
// CCSCSettingsUiMainContainer::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CCSCSettingsUiMainContainer::CountComponentControls() const
    {
    return 1;
    }

// ---------------------------------------------------------------------------
// From class CoeControl
// CCSCSettingsUiMainContainer::SizeChanged
// ---------------------------------------------------------------------------
//
void CCSCSettingsUiMainContainer::SizeChanged()
    {
    iListBox->SetRect( Rect() );
    }

// ---------------------------------------------------------------------------
// From class CoeControl
// CCSCSettingsUiMainContainer::FocusChanged
// ---------------------------------------------------------------------------
//
void CCSCSettingsUiMainContainer::FocusChanged( 
    TDrawNow aDrawNow )
    {
    CCoeControl::FocusChanged( aDrawNow );

    if ( iListBox )
        {
        iListBox->SetFocus( IsFocused() );
        }
    }

// ---------------------------------------------------------------------------
// Deletes service.
// ---------------------------------------------------------------------------
//
TBool CCSCSettingsUiMainContainer::DeleteServiceL()
    {
    CSCSETUIDEBUG( "CCSCSettingsUiMainContainer::DeleteServiceL - begin" );

    // Show confirmation query for service deletion.
    // Create confirmation query dialog.
    TBool isDelete( EFalse );
    HBufC* string = NULL;
    CAknQueryDialog* query = 
        new( ELeave ) CAknQueryDialog( CAknQueryDialog::ENoTone );

    CleanupStack::PushL( query );
    query->PrepareLC( R_CSC_DELETE_SERVICE_QUERY );
    string = StringLoader::LoadLC( 
        R_QTN_CSC_DELETE_SERVICE_QUERY, 
        iModel.SettingsHandler().ServiceNameL( iModel.CurrentSPEntryId() ) );
    query->SetPromptL( *string );
    CleanupStack::PopAndDestroy( string );
    CleanupStack::Pop( query );
    if ( query->RunLD() )
        {  
        // Checke whether current service is disabled.
        TBool  disabled = iModel.CCHHandler().IsServiceDisabled( iModel.CurrentSPEntryId() );
        // The current service is deleted directly if the service is disabled.
        if ( disabled )
            {            
            // First check if there is a service plugin UID.
            TInt count = iModel.ServicePluginHandler().PluginCount( 
                    CCSCEngServicePluginHandler::EInitialized );

            TRAPD( err, LaunchCleanupPluginL( iModel.CurrentSPEntryId() ) );
            if ( KErrNone != err )
                {
                iModel.SettingsHandler().DeleteServiceL( 
                        iModel.CurrentSPEntryId() );
                }
            isDelete = ETrue;                  
            }
        else
            {
            // A note that indicates unable to delete service is shown if the service is enabled.
            HBufC* string = NULL;
            string = StringLoader::LoadL( 
                            R_QTN_CSC_UNABLE_TO_DELETE_NOTE,
                            iModel.SettingsHandler().ServiceNameL( iModel.CurrentSPEntryId() ) );
            if ( string )
                {
                CleanupStack::PushL( string );   
                CAknInformationNote* note = new ( ELeave ) CAknInformationNote( ETrue );
                note->ExecuteLD( *string );
                CleanupStack::PopAndDestroy( string );
                }         
            }
        }

    CSCSETUIDEBUG( "CCSCSettingsUiMainContainer::DeleteServiceL - end" );
    return isDelete;
    }

// ---------------------------------------------------------------------------
// Launches cleanup plugin to remove settings.
// ---------------------------------------------------------------------------
//
void CCSCSettingsUiMainContainer::LaunchCleanupPluginL( 
    TUint aServiceId ) const
    {
    CSCSETUIDEBUG( 
        "CCSCSettingsUiMainContainer::LaunchCleanupPluginL - begin" );

    RImplInfoPtrArray implInfoArray;
    CleanupStack::PushL( TCleanupItem( 
        ResetAndDestroy, &implInfoArray ) );

    REComSession::ListImplementationsL(
        KCSCSettingsCleanupPluginInterfaceUid,
        implInfoArray );

    for ( TInt i( 0 ) ; i < implInfoArray.Count() ; i++ )
        {                    
        CCSCEngSettingsCleanupPluginInterface* plugin = 
            CCSCEngSettingsCleanupPluginInterface::NewL( 
                implInfoArray[i]->ImplementationUid() );

        CleanupStack::PushL( plugin );

        if ( CCSCEngSettingsCleanupPluginInterface::ESipVoIPCleanupPlugin 
            == plugin->PluginType() )
            {
            plugin->RemoveSettingsL( aServiceId );
            }

        CleanupStack::PopAndDestroy( plugin );
        }

    CleanupStack::PopAndDestroy( &implInfoArray );
    REComSession::FinalClose();

    CSCSETUIDEBUG(
        "CCSCSettingsUiMainContainer::LaunchCleanupPluginL - end" );
    }
 
// ---------------------------------------------------------------------------
// CCSCSettingsUiMainContainer::ResetAndDestroy
// ---------------------------------------------------------------------------
//
void CCSCSettingsUiMainContainer::ResetAndDestroy( TAny* aArray )
     {
     if ( aArray )
         {
         RImplInfoPtrArray* array = 
             reinterpret_cast<RImplInfoPtrArray*>( aArray );
         array->ResetAndDestroy();
         }
     }

// End of file.

