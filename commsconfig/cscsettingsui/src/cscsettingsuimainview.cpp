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
* Description:  Implements CSCSettingsUiMainView methods
*
*/


#include <hlplch.h>
#include <aknview.h>
#include <featmgr.h>
#include <akntitle.h>
#include <aknlists.h>
#include <cchclient.h>
#include <e32property.h>
#include <aknViewAppUi.h>
#include <StringLoader.h>
#include <cmsettingsui.h>
#include <xSPViewServices.h>
#include <aknnotewrappers.h>
#include <cscsettingsui.rsg>
#include <ctsydomainpskeys.h>
#include <akntextsettingpage.h>
#include <cvimpstsettingsstore.h>
#include <DRMHelper.h>
#include <mediafilelist.h>
#include <aknpasswordsettingpage.h>
#include <aknradiobuttonsettingpage.h>
#include <aknnavi.h>
#include <aknnavide.h>
#include <vwsdef.h>
#include <AknDef.h>

#include "cscconstants.h"
#include "cscsettingsui.hrh"
#include "cscsettingsuimodel.h"
#include "cscsettingsuilogger.h"
#include "cscengservicehandler.h"
#include "cscengbrandinghandler.h"
#include "cscsettingsuimainview.h"
#include "cscsettingsuiconstants.h"
#include "cscengservicepluginhandler.h"
#include "cscappui.h"


#define AppUi() (static_cast<CAknViewAppUi*>(iAvkonAppUi) )

// Preferred service not set
const TInt KPrefServiceNotSet = 0;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCSettingsUiMainView::CCSCSettingsUiMainView( 
    CCSCSettingsUiModel& aModel )
    : iModel( aModel )
    {
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CCSCSettingsUiMainView::ConstructL()
    {
    CSCSETUIDEBUG( "CCSCSettingsUiMainView::ConstructL - begin" );
        
    BaseConstructL( R_CSCSETTINGSUI_MAINVIEW );
    
    // Get handle to titlepane
    CEikStatusPane* statusPane = StatusPane();   
    iTitlePane = static_cast<CAknTitlePane*> 
        ( statusPane->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
        
    iImToneSelectionListOpen = EFalse;
    CSCSETUIDEBUG( "CCSCSettingsUiMainView::ConstructL - end" );
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//      
CCSCSettingsUiMainView* CCSCSettingsUiMainView::NewL( 
    CCSCSettingsUiModel& aModel )
    {    
    CCSCSettingsUiMainView* self = CCSCSettingsUiMainView::NewLC( aModel );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCSettingsUiMainView* CCSCSettingsUiMainView::NewLC( 
    CCSCSettingsUiModel& aModel )
    {    
    CCSCSettingsUiMainView* self = 
        new ( ELeave ) CCSCSettingsUiMainView( aModel );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCSettingsUiMainView::~CCSCSettingsUiMainView()
    {
    CSCSETUIDEBUG( 
        "CCSCSettingsUiMainView::~CCSCSettingsUiMainView - begin" );
    
    // Delete container when view is deactivated.
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        }
    
    delete iNaviDecorator;
    
    CSCSETUIDEBUG( "CCSCSettingsUiMainView::~CCSCSettingsUiMainView - end" );
    }         


// ---------------------------------------------------------------------------
// CCSCSettingsUiMainView::UpdateSoftkeysL
// Processes situation when it´s notified that softkeys need to be changed.
// ---------------------------------------------------------------------------
//
void CCSCSettingsUiMainView::UpdateSoftkeysL( )
    {
    // not used
    }


// ---------------------------------------------------------------------------
// From CAknView.
// CCSCSettingsUiMainView::Id
// ---------------------------------------------------------------------------
//
TUid CCSCSettingsUiMainView::Id() const
    {
    return KCSCSettingsUiMainViewId;
    }

// ---------------------------------------------------------------------------
// CCSCSettingsUiMainView::SetTitleTextL
// Changes given service name to title pane.
// ---------------------------------------------------------------------------
//
void CCSCSettingsUiMainView::SetTitleTextL()
    {
    // Create text and set it to titlepane.
    TBuf<KCSCSettingsUiItemLength> titleText ( KNullDesC );
    TUint entryId = iModel.CurrentSPEntryId();
    titleText = iModel.SettingsHandler().ServiceNameL( entryId );
    HBufC* text = StringLoader::LoadLC( 
        R_CSCSETTINGSUI_MAINVIEW_TITLE, 
        titleText );
    iTitlePane->SetTextL( *text );
    CleanupStack::PopAndDestroy( text );
    }

// ---------------------------------------------------------------------------
// CCSCSettingsUiMainView::IsEditPreferredServiceSettingAllowedL
//  For checking if editing of preferred service setting is allowed
// ---------------------------------------------------------------------------
//
TBool CCSCSettingsUiMainView::IsEditPreferredServiceSettingAllowedL()
    {
    TBool allowed( EFalse );
    TInt value( KErrNotFound );
    
    User::LeaveIfError( 
            RProperty::Get( 
                    KPSUidCtsyCallInformation,
                    KCTsyCallState,
                    value ) );

    // Editing is allowed if no ongoing call.
    if ( EPSCTsyCallStateNone == value )
        {
        allowed = ETrue;
        }
    else
        {
        // Show unable to change settings note
        HBufC* string = NULL;
        string = StringLoader::LoadL( 
                R_CSCSETTINGSUI_UNABLE_TO_EDIT_SETTING_NOTE );
                
        if ( string )
            {
            // Set text and show note.
            CleanupStack::PushL( string );   
            CAknInformationNote* note = 
            new ( ELeave ) CAknInformationNote( ETrue );
            note->ExecuteLD( *string );
            CleanupStack::PopAndDestroy( string );
            }
        }
    
    return allowed;
    }

// ---------------------------------------------------------------------------
// From CEikAppUi
// CCSCSettingsUiMainView::HandleCommandL
// ---------------------------------------------------------------------------
//
void CCSCSettingsUiMainView::HandleCommandL( TInt aCommand )
    {
    CSCSETUIDEBUG2( "CCSCSettingsUiMainView::HandleCommandL: command=%d",
         aCommand );
    
    TMainListBoxItem listBoxItem = iContainer->CurrentItem();
    
    switch ( aCommand )
        { 
        case ECSCMSKSettingsUiChange:
            {
            HandleMskChangeSelectionL( listBoxItem );
            break;
            }
        case ECSCSettingsUiOpen:
        case ECSCSettingsUiChange:
            {
            HandleListBoxSelectionL();
            break;
            }   
        case EAknCmdHelp:
            {
            if ( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
                {
                CArrayFix<TCoeHelpContext>* buf = AppUi()->AppHelpContextL();
                    HlpLauncher::LaunchHelpApplicationL(
                        iEikonEnv->WsSession(), buf );
                }
            break;
            }         
        case EAknSoftkeyBack:
            {
            HandleReturnToPreviousViewL();
            break;
            }
        case ECSCSettingsUiDelete:
            {
            CSCSETUIDEBUG( "    HandleCommandL - delete service" );
            iContainer->DeleteServiceL();
            iDeleted = ETrue;
            HandleReturnToPreviousViewL();
            break;
            }
        case EEikCmdExit:
        case EAknSoftkeyExit:
            {
            HandleSettingsUiExitL();
            break;
            }
        default:
            {
            AppUi()->HandleCommandL( aCommand );
            break;
            }
        }
    }


// ---------------------------------------------------------------------------
// From CAknView
// CCSCSettingsUiMainView::DoActivateL
// ---------------------------------------------------------------------------
//
void CCSCSettingsUiMainView::DoActivateL( 
    const TVwsViewId& /*aPrevViewId*/, 
    TUid /*aCustomMessageId*/,
    const TDesC8& /*aCustomMessage*/ )
    {
    CSCSETUIDEBUG( "CCSCSettingsUiMainView::DoActivateL - begin" );

    // Create container when view is activated.
    if ( !iContainer )
        {
        iContainer = new ( ELeave ) CCSCSettingsUiMainContainer( iModel );
        iContainer->SetMopParent( this );
        iContainer->ConstructL( ClientRect() );
        AppUi()->AddToStackL( *this, iContainer );
        iContainer->ListBox()->SetListBoxObserver( this );
        }

    SetTitleTextL();
    UpdateSoftkeysL();
    iContainer->UpdateContainerL();

    if ( !iNaviPane )
         {
         iNaviPane = static_cast<CAknNavigationControlContainer*>(
             iAvkonAppUi->StatusPane()->ControlL(
             TUid::Uid(EEikStatusPaneUidNavi)));
         } 

     if ( !iNaviDecorator )
         {
         iNaviDecorator = iNaviPane->CreateNavigationLabelL();
         iNaviPane->PushL( *iNaviDecorator );
         }

    CSCSETUIDEBUG( "CCSCSettingsUiMainView::DoActivateL - end" );
    }


// ---------------------------------------------------------------------------
// From CAknView
// CCSCSettingsUiMainView::DoDeactivate()
// ---------------------------------------------------------------------------
//
void CCSCSettingsUiMainView::DoDeactivate()
    {
    CSCSETUIDEBUG( "CCSCSettingsUiMainView::DoDeactivate - begin" );
    
    if ( iNaviPane && iNaviDecorator )
            {
            iNaviPane->Pop( iNaviDecorator );
            }
   delete iNaviDecorator;
   iNaviDecorator = NULL;
    
    // Delete container when view is deactivated.
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    
    CSCSETUIDEBUG( "CCSCSettingsUiMainView::DoDeactivate - end" );
    }
    
    
// ---------------------------------------------------------------------------
// From MEikListBoxObserver
// CSCSettingsUiMainView::HandleListBoxEventL
// ---------------------------------------------------------------------------
//
void CCSCSettingsUiMainView::HandleListBoxEventL( 
    CEikListBox* /*aListBox*/, TListBoxEvent aEventType )
    {
    TMainListBoxItem listBoxItem = iContainer->CurrentItem();
    
    switch ( aEventType )
        {
        case EEventEnterKeyPressed:
        case EEventItemSingleClicked:
            {
            if ( TMainListBoxItem::EPreferredService == listBoxItem.iItem || 
                 TMainListBoxItem::EVccPreferredService == listBoxItem.iItem ||
                 TMainListBoxItem::EHandoverNotifTone == listBoxItem.iItem )
                {
                HandleCommandL( ECSCMSKSettingsUiChange );
                }
            else
                {
                HandleListBoxSelectionL();
                }
            break;
            }
 
        default:
           break;
        }
    }


// ---------------------------------------------------------------------------
// From MEikListBoxObserver
// CCSCSettingsUiMainView::HandleListBoxSelectionL
// ---------------------------------------------------------------------------
//
void CCSCSettingsUiMainView::HandleListBoxSelectionL() 
    {        
    TMainListBoxItem listBoxItem = iContainer->CurrentItem();
    
    switch( listBoxItem.iItem )
        {
        case TMainListBoxItem::EUsername:
            ShowUsernameSettingPageL();
            break;           
        case TMainListBoxItem::EPassword:
            ShowPasswordSettingPageL();
            break;
        case TMainListBoxItem::EPreferredService:
            ShowPrefServiceSettingPageL();
            break;
        case TMainListBoxItem::EVccPreferredService:
            ShowVccPrefServiceSettingPageL();
            break;
        case TMainListBoxItem::EHandoverNotifTone:
            iContainer->HandoverNotificationToneQueryL();
            break;
        case TMainListBoxItem::EImTone:
            ShowImToneSelectionListL();
            break;
        case TMainListBoxItem::EAutoacceptInv:
            iContainer->PresenceReqPrefQueryL();
            break;
        case TMainListBoxItem::EServiceConn:
            LaunchCMSettingsUiL();
            break;
        default:
            User::Leave( KErrNotSupported );
            break;
        }
    }
    
    
// ---------------------------------------------------------------------------
// From MEikListBoxObserver
// CCSCSettingsUiMainView::DynInitMenuPaneL
// ---------------------------------------------------------------------------
//
void CCSCSettingsUiMainView::DynInitMenuPaneL( 
    TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    CSCSETUIDEBUG( "CCSCSettingsUiMainView::DynInitMenuPaneL - begin" );
    
    if ( aMenuPane && R_CSCSETTINGSUI_MAINVIEW_MENU == aResourceId )
        {
        TMainListBoxItem listBoxItem = iContainer->CurrentItem();
    
        switch ( listBoxItem.iItem )
            {
            // Hide "Change" and show "Open"
            case TMainListBoxItem::EServiceConn:
                aMenuPane->SetItemDimmed( ECSCSettingsUiChange, ETrue );
                aMenuPane->SetItemDimmed( ECSCSettingsUiOpen, EFalse );
                break;
            // Show "Change" and hide "Open"
            case TMainListBoxItem::EUsername:
            case TMainListBoxItem::EPassword:
            case TMainListBoxItem::EPreferredService:
            case TMainListBoxItem::EVccPreferredService:
            case TMainListBoxItem::EHandoverNotifTone:
            case TMainListBoxItem::EAutoacceptInv:     
            case TMainListBoxItem::EImTone:
                aMenuPane->SetItemDimmed( ECSCSettingsUiChange, EFalse );
                aMenuPane->SetItemDimmed( ECSCSettingsUiOpen, ETrue );
                break;   
            // Hide both of options. Case should not be possible.       
            default:
                 aMenuPane->SetItemDimmed( ECSCSettingsUiChange, ETrue );
                 aMenuPane->SetItemDimmed( ECSCSettingsUiOpen, ETrue );
            break;
            }
        if ( !(iModel.CCHHandler().IsServiceDisabled( 
            iModel.CurrentSPEntryId() ) ) )
            {
            aMenuPane->SetItemDimmed( ECSCSettingsUiDelete, ETrue );
            }
        }
     
    CSCSETUIDEBUG( "CCSCSettingsUiMainView::DynInitMenuPaneL - end" );
    }


// ---------------------------------------------------------------------------
// CCSCSettingsUiMainView::ShowUsernameSettingPageL()
// Shows username setting page - for username input.
// ---------------------------------------------------------------------------
void CCSCSettingsUiMainView::ShowUsernameSettingPageL()
    {
    // Editing is allowed if service is disabled.
    if ( iModel.CCHHandler().IsServiceDisabled( iModel.CurrentSPEntryId() ) )
        {
        RBuf username;
        CleanupClosePushL( username );
        username.CreateL( KCCHMaxUsernameLength );
        
        User::LeaveIfError( 
            iModel.CCHHandler().GetConnectionParameter( 
                    iModel.CurrentSPEntryId(), ECchUsername, username ) );
            
        CAknTextSettingPage* dlg = 
            new(ELeave) CAknTextSettingPage( 
                R_CSCSETTINGSUI_USERNAME_SETTING_PAGE, username );
        
        if ( dlg->ExecuteLD( CAknSettingPage::EUpdateWhenAccepted ) )
            { 
            // Set username       
            User::LeaveIfError( 
                iModel.CCHHandler().SetConnectionParameter( 
                    iModel.CurrentSPEntryId(), ECchUsername, username ) );
            }
        
        CleanupStack::PopAndDestroy( &username );
        
        iContainer->UpdateContainerL();
        }
    else
        {
        // Show unable to change settings note
        HBufC* string = NULL;
        string = StringLoader::LoadL( 
                R_CSCSETTINGSUI_UNABLE_TO_EDIT_WHILE_ONLINE );
        
        if ( string )
            {
            // Set text and show note.
            CleanupStack::PushL( string );   
            CAknInformationNote* note = 
                new ( ELeave ) CAknInformationNote( ETrue );
            note->ExecuteLD( *string );
            CleanupStack::PopAndDestroy( string );
            }
        }
    }



// ---------------------------------------------------------------------------
// CCSCSettingsUiMainView::ShowPasswordSettingPageL()
// Shows password setting page - for password input.
// ---------------------------------------------------------------------------
void CCSCSettingsUiMainView::ShowPasswordSettingPageL()
    {
    // Editing is allowed if service is disabled.
    if ( iModel.CCHHandler().IsServiceDisabled( iModel.CurrentSPEntryId() ) )
        {
        RBuf password;
        CleanupClosePushL( password );
        password.CreateL( KCCHMaxPasswordLength );
           
        CAknAlphaPasswordSettingPage* dlg = 
                new ( ELeave ) CAknAlphaPasswordSettingPage(
                        R_CSCSETTINGSUI_PASSWORD_SETTING_PAGE, 
                        password, 
                        KNullDesC );
        
        if ( dlg->ExecuteLD( CAknSettingPage::EUpdateWhenAccepted ) )
            {
            // Set password
            User::LeaveIfError( 
                iModel.CCHHandler().SetConnectionParameter( 
                    iModel.CurrentSPEntryId(), ECchPassword, password ) );
            }
           
        CleanupStack::PopAndDestroy( &password );
        
        iContainer->UpdateContainerL();
        }
    else
        {
        // Show unable to change settings note
        HBufC* string = NULL;
        string = StringLoader::LoadL( 
                R_CSCSETTINGSUI_UNABLE_TO_EDIT_WHILE_ONLINE );
        
        if ( string )
            {
            // Set text and show note.
            CleanupStack::PushL( string );   
            CAknInformationNote* note = 
                new ( ELeave ) CAknInformationNote( ETrue );
            note->ExecuteLD( *string );
            CleanupStack::PopAndDestroy( string );
            }
        }
    }


// ---------------------------------------------------------------------------
// CCSCSettingsUiMainView::ShowPrefServiceSettingPageL()
// Shows Internet call preferred setting page - for ON/OFF setting items
// ---------------------------------------------------------------------------
void CCSCSettingsUiMainView::ShowPrefServiceSettingPageL()
    {
    CSCSETUIDEBUG( "CCSCSettingsUiMainView::ShowPrefServiceSettingPageL" );
    
    if ( IsEditPreferredServiceSettingAllowedL() )
        {
        CDesCArrayFlat* items = 
             new ( ELeave ) CDesCArrayFlat( 2 );
        CleanupStack::PushL( items );
           
        AppendItemL( *items, R_CSCSETTINGSUI_SETTING_PREFERRED_SERVICE_ON );
        AppendItemL( *items, R_CSCSETTINGSUI_SETTING_PREFERRED_SERVICE_OFF );
                  
        // Get current value of the setting
        TInt value( iModel.SettingsHandler().IsPreferredTelephonyVoip() &&
                    iModel.SettingsHandler().IsPreferredService( 
                           iModel.CurrentSPEntryId() ) ? 
                           ECSCSettingsPrefServiceOn : 
                           ECSCSettingsPrefServiceOff);
              
        CAknRadioButtonSettingPage* dlg = new( ELeave ) 
            CAknRadioButtonSettingPage( 
                   R_CSCSETTINGSUI_PREF_SERVICE_SETTING_PAGE,
                   value, items );
            
        if ( dlg->ExecuteLD( CAknSettingPage::EUpdateWhenChanged ) )
            {
            // Change value of the preferred telephony setting and set
            // current preferred service id. If preferred telephony value
            // is changed as voip then we set this service as preferred
            // service. Otherwise we set preferred service as not set (=0)
            TUint serviceId( value == ECSCSettingsPrefServiceOn ? 
                             iModel.CurrentSPEntryId() : 
                             KPrefServiceNotSet );
               
            iModel.SettingsHandler().SetVoipAsPrefTelephony( 
                    value == ECSCSettingsPrefServiceOn,
                    serviceId );
            }
               
        CleanupStack::PopAndDestroy( items );   
        iContainer->UpdateContainerL();
        }
    }


// ---------------------------------------------------------------------------
// CCSCSettingsUiMainView::ShowVccPrefServiceSettingPageL()
// Shows Internet call preferred setting page - for ON/OFF setting items
// when VCC is supported
// ---------------------------------------------------------------------------
void CCSCSettingsUiMainView::ShowVccPrefServiceSettingPageL()
    {
    CSCSETUIDEBUG( "CCSCSettingsUiMainView::ShowVccPrefServiceSettingPageL" );
    
    if ( IsEditPreferredServiceSettingAllowedL() )
        {
        CDesCArrayFlat* items = 
             new ( ELeave ) CDesCArrayFlat( 2 );
        CleanupStack::PushL( items );
           
        AppendItemL( *items, R_CSCSETTINGSUI_SETTING_VCC_PREFERRED_SERVICE_ON );
        AppendItemL( *items, R_CSCSETTINGSUI_SETTING_VCC_PREFERRED_SERVICE_OFF );
                  
        // Get current value of the setting
        TInt value( iModel.SettingsHandler().IsPreferredTelephonyVoip() &&
                    iModel.SettingsHandler().IsPreferredService( 
                           iModel.CurrentSPEntryId() ) ? 
                           ECSCSettingsPrefServiceOn : 
                           ECSCSettingsPrefServiceOff);
              
        CAknRadioButtonSettingPage* dlg = new( ELeave ) 
            CAknRadioButtonSettingPage( 
                   R_CSCSETTINGSUI_VCC_PREF_SERVICE_SETTING_PAGE,
                   value, items );
            
        if ( dlg->ExecuteLD( CAknSettingPage::EUpdateWhenChanged ) )
            {
            // Change value of the preferred telephony setting and set
            // current preferred service id. If preferred telephony value
            // is changed as voip then we set this service as preferred
            // service. Otherwise we set preferred service as not set (=0)
            TUint serviceId( value == ECSCSettingsPrefServiceOn ? 
                             iModel.CurrentSPEntryId() : 
                             KPrefServiceNotSet );
               
            iModel.SettingsHandler().SetVoipAsPrefTelephony( 
                    value == ECSCSettingsPrefServiceOn,
                    serviceId );
            }
               
        CleanupStack::PopAndDestroy( items );   
        iContainer->UpdateContainerL();
        }
    }


// ---------------------------------------------------------------------------
// CCSCSettingsUiMainView::ShowImToneSelectionListL()
// ---------------------------------------------------------------------------
void CCSCSettingsUiMainView::ShowImToneSelectionListL()
    {    
    CSCSETUIDEBUG( "CCSCSettingsUiMainView::ShowImToneSelectionListL - begin" );
    
    if ( !iImToneSelectionListOpen )
        {
        iImToneSelectionListOpen = ETrue;
        CMediaFileList* list = CMediaFileList::NewL();
        CleanupStack::PushL( list );
      
        HBufC* popupTitle = StringLoader::LoadLC( 
            R_CSCSETTINGSUI_IM_TONE_POPUP_HEADING );
        
        HBufC* noTone = StringLoader::LoadLC( 
            R_CSCSETTINGSUI_NO_IM_TONE_TEXT );
        
        list->SetAttrL( 
            CMediaFileList::EAttrAutomatedType,
            CDRMHelper::EAutomatedTypeIMAlert );
        
        list->SetNullItemL( 
            *noTone,
            KNullDesC, 
            CMediaFileList::EMediaFileTypeAudio,
            CMediaFileList::ENullItemIconOff );
        
        list->SetAttrL( CMediaFileList::EAttrTitle, *popupTitle );
        
        TBuf<KCSCMaxImToneLength> toneName;
        TInt nullItem = KErrNotFound;    
        
        TBool result = list->ShowMediaFileListL(
            &toneName, &nullItem , NULL, NULL );
       
        CleanupStack::PopAndDestroy( noTone );
        CleanupStack::PopAndDestroy( popupTitle );
        CleanupStack::PopAndDestroy( list );
        
        if ( result )
            {
            iContainer->SaveImTonePathL( toneName );
            }
        else if( KErrNotFound != nullItem )
            {
            iContainer->SaveImTonePathL( KNullDesC );
            }
        else
            {
            // do nothing
            }
        
        iContainer->UpdateContainerL();
        iImToneSelectionListOpen = EFalse;
        }
    
    CSCSETUIDEBUG( "CCSCSettingsUiMainView::ShowImToneSelectionListL - end" );
    }


// ---------------------------------------------------------------------------
// CCSCSettingsUiMainView::HandleMskChangeSelectionL()
// Handles 'change' middle softkey selection.
// ---------------------------------------------------------------------------
void CCSCSettingsUiMainView::HandleMskChangeSelectionL(
    TMainListBoxItem aListBoxItem )
    {
    switch ( aListBoxItem.iItem )
        {
        case TMainListBoxItem::EPreferredService:
        case TMainListBoxItem::EVccPreferredService:
            {
            ChangePrefTelephonyValueL();
            }
            break;
        case TMainListBoxItem::EHandoverNotifTone:
            {
            ChangeHandoverNotifToneValueL();
            }
            break;
        default:
            {
            User::Leave( KErrArgument );
            }
            break;
        }
    }

// ---------------------------------------------------------------------------
// CCSCSettingsUiMainView::ChangePrefTelephonyValueL()
// Changes the value of the preferred telephony setting
// ---------------------------------------------------------------------------
void CCSCSettingsUiMainView::ChangePrefTelephonyValueL()
    {
    CSCSETUIDEBUG( "CCSCSettingsUiMainView::ChangePrefTelephonyValueL" );
    
    if ( IsEditPreferredServiceSettingAllowedL() )
        {
        // Check is this service preferred service
        TBool prefService( 
                iModel.SettingsHandler().IsPreferredTelephonyVoip() &&
                iModel.SettingsHandler().IsPreferredService( 
                iModel.CurrentSPEntryId() ));
          
        // Set preferred service id as not set (=0) if preferred telephony
        // setting is changed as CS.
        TUint serviceId( prefService ? KPrefServiceNotSet : 
                                         iModel.CurrentSPEntryId() );
          
        // Set preferred telephony setting value and current preferred
        // service id
        iModel.SettingsHandler().SetVoipAsPrefTelephony( 
              !prefService,
              serviceId );
          
        iContainer->UpdateContainerL();
        }
    }

// ---------------------------------------------------------------------------
// CCSCSettingsUiMainView::ChangeHandoverNotifToneValueL()
// Changes the value of the handover notify tone setting.
// ---------------------------------------------------------------------------
void CCSCSettingsUiMainView::ChangeHandoverNotifToneValueL()
    {
    TOnOff onOff = iModel.SettingsHandler().HandoverNotifTonePrefL( 
        iModel.CurrentSPEntryId() );
    
    if ( EOn == onOff )
        {
        iModel.SettingsHandler().SetHandoverNotifTonePrefL( 
            iModel.CurrentSPEntryId(), EOff );
        }
    else
        {
        iModel.SettingsHandler().SetHandoverNotifTonePrefL( 
            iModel.CurrentSPEntryId(), EOn );
        }
    
    iContainer->UpdateContainerL();
    }

// -----------------------------------------------------------------------------
// CCSCSettingsUiMainView::AppendItemL
// Appends resource texts to the des array.
// -----------------------------------------------------------------------------
//
void CCSCSettingsUiMainView::AppendItemL( 
    CDesCArrayFlat& aList, TInt aItem )
    {
    HBufC* string = StringLoader::LoadLC( aItem );
    aList.AppendL( *string );
    CleanupStack::PopAndDestroy( string );
    }    

// ---------------------------------------------------------------------------
// CCSCSettingsUiMainView::LaunchCMSettingsUiL
// Launches Connection Method Settings Ui for editing destinations.
// ---------------------------------------------------------------------------
//
void CCSCSettingsUiMainView::LaunchCMSettingsUiL()
    {
    CSCSETUIDEBUG( "CCSCSettingsUiMainView::LaunchCMSettingsUiL" );
    
    // Launch Connection Method Settings Ui.
    CCmSettingsUi* cmSettingsUi = CCmSettingsUi::NewL();
    CleanupStack::PushL( cmSettingsUi );
    if ( CCmSettingsUi::EExit == cmSettingsUi->RunSettingsL() )
        {
        HandleCommandL( EEikCmdExit );
        }
    CleanupStack::PopAndDestroy( cmSettingsUi );
    iContainer->UpdateContainerL();
    }

// ---------------------------------------------------------------------------
// CCSCSettingsUiMainView::HandleReturnToPreviousViewL
// Handles returning to previous view where settingsui was launced.
// ---------------------------------------------------------------------------
//
void CCSCSettingsUiMainView::HandleReturnToPreviousViewL()
    {
    CSCSETUIDEBUG( 
        "CCSCSettingsUiMainView::HandleReturnToPreviousViewL - IN" );

    if ( iModel.ReturnViewId() != KNullUid )
        {             
        TUid tabview( KNullUid );

        // Error code not needed.
        TRAP_IGNORE( 
            tabview.iUid = iModel.SettingsHandler().ServiceTabViewIdL( 
            iModel.CurrentSPEntryId() ) )

        if ( iDeleted && KCSCServiceViewId != iModel.ReturnViewId() )
            {
            TVwsViewId idleId;
            AknDef::GetPhoneIdleViewId( idleId );
            ActivateViewL( idleId );
            AppUi()->HandleCommandL( EEikCmdExit );
            }
        else if ( tabview.iUid == iModel.ReturnViewId().iUid )
            {
            RxSPViewServices viewServices;
            TInt err = viewServices.Activate( 
                KPhoneBookTabUid.iUid, 
                iModel.ReturnViewId().iUid );

            CSCSETUIDEBUG2( "   --> ACTIVATE ERR=%d", err );

            AppUi()->HandleCommandL( EEikCmdExit );
            }
        else
            {
            // Not deleted or launched from service tab,
            // activate previous view.
            AppUi()->ActivateLocalViewL( iModel.ReturnViewId() );
            }
        }
      
    CSCSETUIDEBUG( 
        "CCSCSettingsUiMainView::HandleReturnToPreviousViewL - OUT" );
    }

// ---------------------------------------------------------------------------
// CCSCSettingsUiMainView::HandleSettingsUiExitL
//  Handles exist from settingsui.
// ---------------------------------------------------------------------------
//
void CCSCSettingsUiMainView::HandleSettingsUiExitL()
    {
    CSCSETUIDEBUG( "CCSCSettingsUiMainView::HandleSettingsUiExitL - IN" );
    
    if ( iModel.ReturnViewId() != KNullUid )
        {
        TUid tabview( KNullUid );
        TRAPD( err, tabview.iUid = iModel.SettingsHandler().ServiceTabViewIdL( 
            iModel.CurrentSPEntryId() ) )
        
        if ( tabview.iUid == iModel.ReturnViewId().iUid && !err )
            {
            // Launched from service tab, go back there
            RxSPViewServices viewServices;
            viewServices.Activate( 
                KPhoneBookTabUid.iUid, 
                iModel.ReturnViewId().iUid );     
            
            AppUi()->HandleCommandL( EEikCmdExit );
            }
        else
            { 
            // Not launched from service tab, just exit
            AppUi()->HandleCommandL( EEikCmdExit );
            }        
        }
       
    CSCSETUIDEBUG( "CCSCSettingsUiMainView::HandleSettingsUiExitL - OUT" );
    }

// ---------------------------------------------------------------------------
// CCSCSettingsUiMainView::ResetViewL
// Resets service settings when switching services
// ---------------------------------------------------------------------------
//
void CCSCSettingsUiMainView::ResetViewL()
    {
    CSCSETUIDEBUG( "CCSCSettingsUiMainView::ResetViewL - IN" );

    // Create container when view is activated.
    if ( !iContainer )
        {
        iContainer = new (ELeave) CCSCSettingsUiMainContainer( iModel );
        iContainer->SetMopParent( this );
        iContainer->ConstructL( ClientRect() );
        AppUi()->AddToStackL( *this, iContainer );
        iContainer->ListBox()->SetListBoxObserver( this );
        }

    SetTitleTextL();
    UpdateSoftkeysL();
    iContainer->UpdateContainerL();

    CSCSETUIDEBUG( "CCSCSettingsUiMainView::ResetViewL - OUT" );
    }

// End of file.

