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
* Description:  CSC Application´s Service View´s Container
*
*/


#include <fbs.h>
#include <csc.mbg>
#include <csc.rsg>
#include <e32std.h>
#include <aknapp.h>
#include <gulicon.h>
#include <eikclbd.h>
#include <aknlists.h>
#include <pathinfo.h>
#include <coecntrl.h>
#include <AknsUtils.h>
#include <cchclient.h>
#include <csc.hlp.hrh>
#include <akncontext.h>
#include <AknIconArray.h>
#include <StringLoader.h>
#include <aknViewAppUi.h>
#include <AiwServiceHandler.h>
#include <mspnotifychangeobserver.h>
#include <data_caging_path_literals.hrh>
#include <cscengsettingscleanupplugininterface.h>

#include "mcscservicecontainerobserver.h"
#include "csclogger.h"
#include "cscdialog.h"
#include "cscconstants.h"
#include "cscapplication.h"
#include "cscnoteutilities.h"
#include "cscengcchhandler.h"
#include "cscservicecontainer.h"
#include "cscengservicehandler.h"
#include "cscengbrandinghandler.h"
#include "cscengservicepluginhandler.h"
#include "cscenguiextensionpluginhandler.h"

const TUint KServiceListItemTextMaxLength = 64;
const TUint KMaxUiExetnsionNameLength = 64;

// Format of the list item string.
_LIT( KServiceListItemTextFormat, "%d\t%S\t" );

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCServiceContainer::CCSCServiceContainer( 
    MCSCServiceContainerObserver& aContainerObserver,
    CCSCEngServicePluginHandler& aServicePluginHandler,
    CCSCEngUiExtensionPluginHandler& aUiExtensionPluginHandler,
    CCSCEngServiceHandler& aServiceHandler,
    CCSCEngBrandingHandler& aBrandingHandler,
    CCSCEngCCHHandler& aCCHHandler ):
    iContainerObserver( aContainerObserver ),
    iServicePluginHandler( aServicePluginHandler ),
    iUiExtensionPluginHandler( aUiExtensionPluginHandler ),
    iServiceHandler( aServiceHandler ),
    iBrandingHandler( aBrandingHandler ),
    iCCHHandler( aCCHHandler )
    {
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CCSCServiceContainer::ConstructL( const TRect& aRect )
    {
    CSCDEBUG( "CCSCServiceContainer::ConstructL - begin" );
    
    CreateWindowL(); 
    iDialog = CCSCDialog::NewL();
    
    iListBox = new (ELeave) CAknDouble2LargeStyleListBox;
    iListBox->SetContainerWindowL( *this );
    iListBox->ConstructL( this, EAknGenericListBoxFlags );
    iListBox->HideSecondRow( ETrue );
    iListBox->Model()->SetOwnershipType( ELbmOwnsItemArray );
    
    HBufC* emptyTxt = StringLoader::LoadLC( R_QTN_CSC_NO_SERVICES_INSTALLED );
    iListBox->View()->SetListEmptyTextL( *emptyTxt );
    CleanupStack::PopAndDestroy( emptyTxt );
    
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
                                                 CEikScrollBarFrame::EOff,
                                                 CEikScrollBarFrame::EAuto );
    
    CTextListBoxModel* model = iListBox->Model();
    MDesCArray* textArray = model->ItemTextArray();
    iListBoxItems = static_cast<CDesCArray*>( textArray );
    
    iEikMenuBar = new ( ELeave ) CEikMenuBar();
    iEikMenuBar->ConstructL( this, NULL, R_CSC_SERVICEVIEW_MENUBAR );
    
    SetRect( aRect );
    
    UpdateServiceViewL();
    
    ActivateL();

    CSCDEBUG( "CCSCServiceContainer::ConstructL - end" );
    }
    

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCServiceContainer* CCSCServiceContainer::NewL( 
    const TRect& aRect,
    MCSCServiceContainerObserver& aContainerObserver,
    CCSCEngServicePluginHandler& aServicePluginHandler,
    CCSCEngUiExtensionPluginHandler& aUiExtensionPluginHandler,
    CCSCEngServiceHandler& aServiceHandler,
    CCSCEngBrandingHandler& aBrandingHandler,
    CCSCEngCCHHandler& aCCHHandler )
    {
    CCSCServiceContainer* self = 
        new ( ELeave ) CCSCServiceContainer( 
            aContainerObserver,
            aServicePluginHandler, 
            aUiExtensionPluginHandler,
            aServiceHandler,
            aBrandingHandler,
            aCCHHandler );
    
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCServiceContainer::~CCSCServiceContainer()
    {
    CSCDEBUG( "CCSCServiceContainer::~CCSCServiceContainer - begin" );
    
    delete iDialog;        
    delete iArray;   
    delete iListBox;    
    delete iEikMenuBar;
    
    iListBoxItemArray.Reset();
    iListBoxItemArray.Close();
    
    CSCDEBUG( "CCSCServiceContainer::~CCSCServiceContainer - end" );
    }


// ---------------------------------------------------------------------------
// Updates services to listbox.
// ---------------------------------------------------------------------------
//
void CCSCServiceContainer::UpdateServiceViewL()
    {
    CSCDEBUG( "CCSCServiceContainer::UpdateServiceViewL - begin" );
    
    TInt currentItemIndex = CurrentItemIndex();
    
    // Reset listbox items and item array
    iListBoxItems->Reset();
    iListBoxItemArray.Reset();
        
    // Find and add Ui Extension Plug-ins
    iUiExtensionCount = iUiExtensionPluginHandler.PluginCount();
    
    // Create icon array or cleanup iconarray if it already exists
    CArrayPtr<CGulIcon>* iconArray =  
        iListBox->ItemDrawer()->ColumnData()->IconArray();
    
    if ( !iconArray )
        {
        iconArray = new( ELeave ) CAknIconArray( 1 );
         // Set icon array for ListBox.
        iListBox->ItemDrawer()->ColumnData()->SetIconArray( iconArray );
        }
    else
        {
        iconArray->ResetAndDestroy();
        }

    // move add new service plugin to top
    RArray<TUiExtensionPluginInfo> uiPlugins;
    CleanupClosePushL( uiPlugins );
    
    for( TInt i = 0; i < iUiExtensionCount; i++ )
        {
        TUiExtensionPluginInfo uiExtension = 
            iUiExtensionPluginHandler.ItemFromPluginInfoArray( i );
                        
        if ( KAccountCreationPluginUID == uiExtension.iPluginsUid )
            {
            uiPlugins.InsertL( uiExtension, 0 );
            }
        else
            {
            uiPlugins.AppendL( uiExtension );
            }
        }

    const TInt itemHeight( iListBox->ItemHeight() );
    TInt iconSize( KIconSizeQvgaPortrait ); // Default icon size (QVGA portrait).
    if ( KItemHeightQvgaLandscape == itemHeight ) // QVGA landscape
        {
        iconSize = KIconSizeQvgaLandscape;
        }
               
    for ( TUint i( 0 ) ; i < iUiExtensionCount ; i++ )
        {
        TUiExtensionPluginInfo uiExtension = uiPlugins[i];
                        
        if ( 0 == uiExtension.iPlace )
            {                        
            TBuf<KMaxUiExetnsionNameLength> extensionName;
            extensionName.Copy( uiExtension.iUiExtensionName );
                
            // To keep track which kind of listbox items we have
            TListBoxItem listBoxItem;
            listBoxItem.iItemType = TListBoxItem::EUiExtension;
            listBoxItem.iPluginUid = uiExtension.iPluginsUid;
                        
            iListBoxItemArray.Append( listBoxItem );
            
            // Set icon for ui extension
            SetIconL( listBoxItem, iconArray, iconSize );
                                                
            TBuf<KServiceListItemTextMaxLength> listBoxItemText;          
            
            listBoxItemText.Format(
                KServiceListItemTextFormat,
                iconArray->Count() - 1,
                &extensionName );
            
            iListBoxItems->AppendL( listBoxItemText );
            }
        }
    
    CleanupStack::PopAndDestroy( &uiPlugins );
    
    // Get services  
    RArray<TUint> serviceIds;
    CleanupClosePushL( serviceIds );
    
    TRAPD( err, iServiceHandler.GetAllServiceIdsL( serviceIds ) );
            
    for ( TInt i(0) ; i < serviceIds.Count() && !err ; i++ )
        {                
        // Check service validity from cch before adding to listbox
        if ( iCCHHandler.IsServiceValidL( serviceIds[ i ] ) )
            {
            TBuf<KServiceListItemTextMaxLength> listBoxItemText;          
            HBufC* serviceString = NULL;
                            
            serviceString = StringLoader::LoadLC( 
                R_QTN_CSC_SERVICE_LISTBOX_ITEM, 
                iServiceHandler.ServiceNameL( serviceIds[ i ] ) );
                            
            // To keep track which kind of listbox items we have
            TListBoxItem listBoxItem;
            listBoxItem.iServiceId = serviceIds[ i ];
                        
            // Set icon for service
            SetIconL( listBoxItem, iconArray, iconSize );
                        
            listBoxItemText.Format(
                KServiceListItemTextFormat,
                iconArray->Count() - 1,
                serviceString );
                       
            iListBoxItems->AppendL( listBoxItemText );    
                                           
            TInt32 servicePluginId( 0 );
            TInt err2( KErrNone );
                        
            // Find service plug-in id if service has one
            TRAP( 
                err2, servicePluginId = 
                iServiceHandler.ServiceSetupPluginIdL( serviceIds[ i ] ) );
                                                              
            if ( !err2 && servicePluginId )
                {            
                listBoxItem.iItemType = TListBoxItem::EServicePlugin;
                listBoxItem.iPluginUid.iUid = servicePluginId;
                }
            else if ( KErrNotFound == err2 )
                {
                listBoxItem.iItemType = TListBoxItem::EGeneralService;
                }
                        
            iListBoxItemArray.Append( listBoxItem );    

            CleanupStack::PopAndDestroy( serviceString );   
            }
        }
    
    CleanupStack::PopAndDestroy( &serviceIds );  
    
    UpdateListboxL();
    
    if( iListBoxItemArray.Count() > 0 )
    	{
        if ( 0 <= currentItemIndex && 
           ( iListBoxItemArray.Count() > currentItemIndex  ) )
            {
            iListBox->SetCurrentItemIndexAndDraw( currentItemIndex );
            }
        else
            {
            iListBox->SetCurrentItemIndexAndDraw( 0 );
            }
    	}
    	
    CSCDEBUG( "CCSCServiceContainer::UpdateSerUpdateServiceViewL - end" );
    }


// ---------------------------------------------------------------------------
// Update listbox. Handle item addition and drawing.
// ---------------------------------------------------------------------------
//
void CCSCServiceContainer::UpdateListboxL()
    {
    CSCDEBUG( "CCSCServiceContainer::UpdateListboxL - begin" );
    
    iListBox->HandleItemAdditionL(); 
    
    CSCDEBUG( "CCSCServiceContainer::UpdateListboxL - end" );
    }

// ---------------------------------------------------------------------------
// Returns current item index of listbox.
// ---------------------------------------------------------------------------
//
TInt CCSCServiceContainer::CurrentItemIndex() const
    {
    CSCDEBUG2( 
    "CCSCServiceContainer::CurrentItemIndex:%d",iListBox->CurrentItemIndex());
    
    return iListBox->CurrentItemIndex();
    }


// ---------------------------------------------------------------------------
// Returns currently selected list box item.
// ---------------------------------------------------------------------------
//
TListBoxItem CCSCServiceContainer::ListBoxItem()
    {    
    return iListBoxItemArray[ CurrentItemIndex() ];
    }

// ---------------------------------------------------------------------------
// Returns reference to listbox.
// ---------------------------------------------------------------------------
//
CAknDouble2LargeStyleListBox& CCSCServiceContainer::ListBox()
    {    
    return *iListBox;
    }

// ---------------------------------------------------------------------------
// Returns pen down point.
// ---------------------------------------------------------------------------
//
TPoint CCSCServiceContainer::PenDownPoint()
    {
    return iPenDownPoint;
    }

// ---------------------------------------------------------------------------
// Returns service count.
// ---------------------------------------------------------------------------
//
TInt CCSCServiceContainer::ServiceCount()
    {
    TUint serviceCount( 0 );
    
    for ( TInt i ( 0 ) ; i < iListBoxItemArray.Count() ; i++ )
        {
        //Add count if list box item type is general service or service plugin
        if ( 
            TListBoxItem::EGeneralService == iListBoxItemArray[i].iItemType ||
            TListBoxItem::EServicePlugin == iListBoxItemArray[i].iItemType )
            {
            serviceCount++;
            }
        }
    
    return serviceCount;
    }

// ---------------------------------------------------------------------------
// Set current item index.
// ---------------------------------------------------------------------------
//
void CCSCServiceContainer::SetCurrentIndex( TInt aIndex )
    {
    CSCDEBUG2( "CCSCServiceContainer::SetCurrentIndex: aIndex=%d", aIndex );
    
    if( ( aIndex < iListBoxItemArray.Count() ) && ( aIndex >= 0 ) )
        {
        iListBox->SetCurrentItemIndex( aIndex );
        }
    else
        {
        iListBox->SetCurrentItemIndex( 0 );
        }
    }


// ---------------------------------------------------------------------------
// Deletes service.
// ---------------------------------------------------------------------------
//
void CCSCServiceContainer::DeleteServiceL()
    {
    CSCDEBUG( "CCSCServiceContainer::DeleteServiceL - begin" );
    
    if (  iListBoxItemArray.Count() )
        {
        TListBoxItem item = ListBoxItem();
        TBool  disabled = iCCHHandler.IsServiceDisabled( item.iServiceId );
        
        // if enabled -> show unable to delete note
        if ( !disabled )
            {
            CCSCNoteUtilities::ShowInformationNoteL( 
                CCSCNoteUtilities::ECSCUnableToDeleteNote,
                    iServiceHandler.ServiceNameL( item.iServiceId ) );
            return;
            }

        // Show confirmation query for service deletion                        
        if ( CCSCNoteUtilities::ShowCommonQueryL( 
            CCSCNoteUtilities::ECSCDeleteServiceQuery, 
            iServiceHandler.ServiceNameL( item.iServiceId ) ) )
            {
            // First check if there is a service plugin UID.
            TInt count = iServicePluginHandler.PluginCount( 
                CCSCEngServicePluginHandler::EInitialized );
            TBool setupPlugin( EFalse );
            for ( TInt i = 0; i < count && !setupPlugin; i++ )
                 {
                 TServicePluginInfo pluginInfo = 
                     iServicePluginHandler.ItemFromPluginInfoArray( i );
                 if ( pluginInfo.iPluginsUid == item.iPluginUid )
                     {
                     CSCDEBUG2( "   DeleteServiceL - plugin UID match found: %d",
                         item.iPluginUid );
                     setupPlugin = ETrue;
                     }
                 }

            if ( setupPlugin )
                {
                HandleServicePluginRemovationL( item.iPluginUid );
                }
            else
                {
                TRAPD( err, LaunchCleanupPluginL( item.iServiceId ) );
                if ( KErrNone != err )
                    {
                    iServiceHandler.DeleteServiceL( item.iServiceId );
                    }
                }
            }
        }
    
    CSCDEBUG( "CCSCServiceContainer::DeleteServiceL - end" );
    }

// ---------------------------------------------------------------------------
// Handles service plugin removation.
// ---------------------------------------------------------------------------
//
void CCSCServiceContainer::HandleServicePluginRemovationL( const TUid& aUid )
    {
    CSCDEBUG( 
         "CCSCServiceContainer::HandleServicePluginRemovationL - begin" );  
    
    TUint serviceId( 0 );   
    ResolveServiceIdL( aUid, serviceId );
    if ( !serviceId )
        {
        User::Leave( KErrNotFound );
        }
        
    TBool disabled = iCCHHandler.IsServiceDisabled( serviceId );
        
    // If service is not disabled, try to disable it
    if ( !disabled )
        {
        iDialog->LaunchWaitNoteL( R_QTN_CSC_REMOVING_SERVICE );
        User::LeaveIfError( iCCHHandler.DisableService( serviceId ) );
        return;
        } 
        
    // Remove service plugin
    TInt error( KErrNone );
    TRAP( error, iServicePluginHandler.DoRemovationL( aUid, EFalse ) );
        
    // If error, remove remainging setting by cleanupplugin
    if ( error )
        {
        error = KErrNone;
        TRAP( error, LaunchCleanupPluginL( serviceId ) );
               
        if ( error )
            {
            iServiceHandler.DeleteServiceL( serviceId );
            }  
        }
    }


// ---------------------------------------------------------------------------
// Updates container data because of layout change.
// ---------------------------------------------------------------------------
//
void CCSCServiceContainer::HandleResourceChange( TInt aType )
    {
    CSCDEBUG( "CCSCServiceContainer::HandleResourceChange - begin" );
    
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
    
    CSCDEBUG( "CCSCServiceContainer::HandleResourceChange - end" );
    }


// ---------------------------------------------------------------------------
// Sets listbox observer.
// ---------------------------------------------------------------------------
//
void CCSCServiceContainer::SetListBoxObserver( 
    MEikListBoxObserver* aObserver )
    {
    iListBox->SetListBoxObserver( aObserver );
    }


// ---------------------------------------------------------------------------
// Hides dialog wait note.
// ---------------------------------------------------------------------------
//
 void CCSCServiceContainer::HideDialogWaitNote()
    {
    iDialog->DestroyWaitNote();
    }


// ---------------------------------------------------------------------------
// Launches cleanup plugin to remove settings.
// ---------------------------------------------------------------------------
//
 void CCSCServiceContainer::LaunchCleanupPluginL( TUint aServiceId ) const
    {
    CSCDEBUG( "CCSCServiceContainer::LaunchCleanupPluginL - begin" );
    
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
                implInfoArray[ i ]->ImplementationUid() );
        
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
    
    CSCDEBUG( "CCSCServiceContainer::LaunchCleanupPluginL - end" );
    }
 
 
 // ---------------------------------------------------------------------------
 // Resolve service id from service plugi uid.
 // ---------------------------------------------------------------------------
 //
 void CCSCServiceContainer::ResolveServiceIdL( 
     const TUid& aUid, TUint& aServiceId )
     {     
     RArray<TUint> serviceIds;
     CleanupClosePushL( serviceIds );
     iServiceHandler.GetAllServiceIdsL( serviceIds );
     
     TInt serviceCount(  serviceIds.Count() );
     
     if ( serviceCount )
         {
         for ( TInt i = 0 ; i < serviceCount ; i++ )
              {
              TInt32 serviceSetupPluginId( KErrNotFound );
              TRAPD( err, serviceSetupPluginId = 
                  iServiceHandler.ServiceSetupPluginIdL( serviceIds[ i ] ) );
                  
              if ( KErrNone == err && 
                  serviceSetupPluginId == aUid.iUid )
                  {
                  aServiceId = serviceIds[ i ];
                  break;
                  }
              }
         }
     else
         {
         User::Leave( KErrNotFound );
         }
         
     CleanupStack::PopAndDestroy( &serviceIds );
     }

// ---------------------------------------------------------------------------
// From class CoeControl
// Offer key events for controls.
// ---------------------------------------------------------------------------
// 
TKeyResponse CCSCServiceContainer::OfferKeyEventL( 
    const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    CSCDEBUG2( 
        "CCSCServiceContainer::OfferKeyEventL: aKeyEvent: %d", 
            aKeyEvent.iScanCode );
    
    TKeyResponse result( EKeyWasNotConsumed );
    
    if ( iEikMenuBar->ItemSpecificCommandsEnabled() && 
        EKeyBackspace  == aKeyEvent.iCode &&
        ( TListBoxItem::EUiExtension != ListBoxItem().iItemType ) )
        {
        DeleteServiceL();
        result = EKeyWasConsumed;
        }
    else
        {
        result = iListBox->OfferKeyEventL( aKeyEvent, aType );
        }

    iContainerObserver.UpdateCbaL();        
    
    return result;
    }


// ---------------------------------------------------------------------------
// From class CoeControl
// Returns component controls.
// ---------------------------------------------------------------------------
//
CCoeControl* CCSCServiceContainer::ComponentControl( TInt aIndex ) const
    {
    switch ( aIndex )
        {
        case 0:
            return iListBox;
        default:
            return NULL;
        }
    }


// ---------------------------------------------------------------------------
// From class CoeControl
// Handle pointer event.
// ---------------------------------------------------------------------------
//
void CCSCServiceContainer::HandlePointerEventL(
    const TPointerEvent& aPointerEvent )
    {
    iPenDownPoint = aPointerEvent.iPosition;
    CCoeControl::HandlePointerEventL( aPointerEvent );
    }


// ---------------------------------------------------------------------------
// From class MCSCEngCCHObserver
// ---------------------------------------------------------------------------
//
void CCSCServiceContainer::ServiceStatusChanged(
    TUint /*aServiceId*/, 
    TCCHSubserviceType /*aType*/, 
    const TCchServiceStatus& /*aServiceStatus*/ )
    {
    // not used
    }


// ---------------------------------------------------------------------------
// From class CoeControl
// Handles controls size changes.
// ---------------------------------------------------------------------------
//
void CCSCServiceContainer::SizeChanged()
    {
    if ( iListBox )
        {
        iListBox->SetRect( Rect() );
        }
    }
    

// ---------------------------------------------------------------------------
// From class CoeControl
// Handles focus changes.
// ---------------------------------------------------------------------------
//
void CCSCServiceContainer::FocusChanged( TDrawNow aDrawNow )
    {
    CCoeControl::FocusChanged( aDrawNow );
    
    if( iListBox )
        {
        iListBox->SetFocus( IsFocused() );
        }            
    }
    
    
// ---------------------------------------------------------------------------
// From class CoeControl
// Counts components controls.
// ---------------------------------------------------------------------------
//
TInt CCSCServiceContainer::CountComponentControls() const
    {
    return 1; // only iListBox
    }


// -----------------------------------------------------------------------------
// From class CoeControl
// For getting help context
// -----------------------------------------------------------------------------
//
void CCSCServiceContainer::GetHelpContext( 
    TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = KUidCscHelp;
    aContext.iContext = KVOIP_HLP_CSC;
    }


// ---------------------------------------------------------------------------
// Sets icons for lisbox items.
// ---------------------------------------------------------------------------
//
void CCSCServiceContainer::SetIconL( const TListBoxItem& aListBoxItem,
    CArrayPtr<CGulIcon>* aIconArray, TInt aIconSize )
    {
    CSCDEBUG( "CCSCServiceContainer::SetIconL - begin" );
         
    if ( NULL == aIconArray )
        {
        return;
        }
                 
    TFileName iconsFileName = TParsePtrC( PathInfo::RomRootPath() ).Drive();
    iconsFileName.Append( KDC_APP_BITMAP_DIR );
    iconsFileName.Append( KCSCAifFile );
    
    MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
    
    if ( TListBoxItem::EUiExtension == aListBoxItem.iItemType )
        {   
        // specific icon for accountcreationplugin
        if ( KAccountCreationPluginUID == aListBoxItem.iPluginUid )
            {
            aIconArray->AppendL( AknsUtils::CreateGulIconL( 
                    skinInstance, 
                    KAknsIIDDefault,
                    iconsFileName, 
                    EMbmCscQgn_prop_voip_new_service, 
                    EMbmCscQgn_prop_voip_new_service ) );
            }
        // specific icon for advanced settings plugin (sip voip settings)
        else if ( KAdvancedSettingsPluginUID == aListBoxItem.iPluginUid )
            {
            aIconArray->AppendL( AknsUtils::CreateGulIconL( 
                    skinInstance, 
                    KAknsIIDDefault,
                    iconsFileName, 
                    EMbmCscQgn_prop_set_voip_advanced, 
                    EMbmCscQgn_prop_set_voip_advanced ) );
            }
        else // generic icon
            {
            aIconArray->AppendL( AknsUtils::CreateGulIconL( 
                    skinInstance, 
                    KAknsIIDDefault,
                    iconsFileName,
                    EMbmCscQgn_prop_set_service, 
                    EMbmCscQgn_prop_set_service ) );
            }
        } 
    // Service in question, try to get branded icon. If fails use default icon
    else 
        {
        CSCDEBUG( "CCSCServiceContainer::SetIconL - branded icon" );
        CFbsBitmap* bitmap = NULL;
        CFbsBitmap* mask = NULL;
        
        // Get service brand id and branded icon(s).       
        TRAPD( err, 
            TBuf16<KBrandIdMaxLength> brandId16 ( KNullDesC );
            iServiceHandler.BrandIdL( 
                aListBoxItem.iServiceId, 
                brandId16 );
            
            TBuf8<KBrandIdMaxLength> brandId8 ( KNullDesC8 );
            brandId8.Copy( brandId16 );
            iBrandingHandler.GetServiceBrandIconL( brandId8, bitmap, 
                mask, aIconSize ); );
        // Use default icons if error.
        if ( KErrNone != err )
            {
            aIconArray->AppendL( AknsUtils::CreateGulIconL( 
                skinInstance, 
                KAknsIIDDefault,
                iconsFileName,
                EMbmCscQgn_prop_set_service, 
                EMbmCscQgn_prop_set_service ) );
            }
        // Use branded icons if KErrNone.    
        else
            {
            if ( bitmap )
                {
                CSCDEBUG( "CCSCServiceContainer::SetIconL - branded icon" );
                CleanupStack::PushL( bitmap );
                CleanupStack::PushL( mask );
                
                CFbsBitmap* newBitmap = new (ELeave) CFbsBitmap;     
                if ( KErrNone == newBitmap->Duplicate( bitmap->Handle() ) )
                    {
                    CleanupStack::PushL( newBitmap );
                    //ownership transferred (newBitmap)
                    
                    //Create new mask
                    CFbsBitmap* newMask = new (ELeave) CFbsBitmap; 
                    if( KErrNone == newMask->Duplicate( mask->Handle() ) )
                        {
                        CSCDEBUG( "CCSCServiceContainer::SetIconsL - branded newMASK" );
                        
                        CleanupStack::PushL ( newMask );
                        CGulIcon* icon = CGulIcon::NewL( 
                            AknIconUtils::CreateIconL( newBitmap ),
                            AknIconUtils::CreateIconL( newMask ) );
                        
                        CleanupStack::Pop( newMask );
                        CleanupStack::Pop( newBitmap );
                    
                        CleanupStack::PushL( icon );     
                        aIconArray->AppendL( icon );
                        CleanupStack::Pop( icon );
                        CSCDEBUG( "CCSCServiceContainer::SetIconsL - branded newMASK done" );
                        }
                    else
                        {
                        CSCDEBUG( "CCSCServiceContainer::SetIconsL - Error creating mask duplicate" );
                        CleanupStack::PopAndDestroy( newBitmap );
                        delete newMask;
                        newMask = NULL;
                        }
                    }
                else
                    {
                    delete newBitmap;
                    newBitmap = NULL;
                    
                    aIconArray->AppendL( AknsUtils::CreateGulIconL( 
                        skinInstance, 
                        KAknsIIDDefault,
                        iconsFileName,
                        EMbmCscQgn_prop_set_service, 
                        EMbmCscQgn_prop_set_service ) );  
                    }
                
                CleanupStack::PopAndDestroy( mask );
                CleanupStack::PopAndDestroy( bitmap );
                }
            }
        }
             
    CSCDEBUG( "CCSCServiceContainer::SetIconsL - end" );
    }    


// ---------------------------------------------------------------------------
// CCSCServiceContainer::ResetAndDestroy
// ---------------------------------------------------------------------------
//
void CCSCServiceContainer::ResetAndDestroy( TAny* aArray )
    {
    CSCDEBUG( "CCSCServiceContainer::ResetAndDestroy - end" );	
    
    if ( aArray )
        {
        RImplInfoPtrArray* array = 
            reinterpret_cast<RImplInfoPtrArray*>( aArray );
        array->ResetAndDestroy();
        }
    
    CSCDEBUG( "CCSCServiceContainer::ResetAndDestroy - end" );
    }
    
