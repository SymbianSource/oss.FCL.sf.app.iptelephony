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
* Description:  Declarition of CSCSettingsUiMainContainer
*
*/


#ifndef C_CSCSETTINGSUIMAINCONTAINER_H
#define C_CSCSETTINGSUIMAINCONTAINER_H

#include "mcscengserviceobserver.h"
#include "mcscengprovisioningobserver.h"

class CCSCSettingsUiModel;
class CAknSettingStyleListBox;
class MCSCEngServiceObserver;
class MCSCEngProvisioningObserver;

/**
 *  TListBoxItem class
 *
 *  @lib CSCSettingsUi.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( TMainListBoxItem )
    {
    public:
    
        /** Enumeration of setting items **/
        enum TSettingItems
            {
            EUsername = 0,
            EPassword,
            EPreferredService,
            EVccPreferredService,
            EHandoverNotifTone,
            EImTone,
            EAutoacceptInv,
            EServiceConn
            };
        
        /**
         * Constructor.
         *
         * @since S60 3.2
         */
        TMainListBoxItem()
            : iItem( EUsername )
            {}

    public: // data

        /**
         * Listbox item
         */
        TSettingItems iItem;
    };

/**
 *  CCSCSettingsUiMainContainer class
 *  Declarition of CCSCSettingsUiMainContainer.
 *
 *  @lib CSCSettingsUi.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CCSCSettingsUiMainContainer ) : public CCoeControl
    {
public:

    CCSCSettingsUiMainContainer(
        CCSCSettingsUiModel& aModel );
    void ConstructL( const TRect& aRect );
    virtual ~CCSCSettingsUiMainContainer();

    /**
     * Updates container and redraws listbox items.
     *
     * @since S60 v3.2
     */
    void UpdateContainerL();

    /**
     * Shows a query which changes handover notification tone setting
     *
     * @since S60 v5.0
     */
    void HandoverNotificationToneQueryL();

    /**
     * Shows a query which changes presence request preference
     *
     * @since S60 v3.2
     */
    void PresenceReqPrefQueryL();

    /**
     * For saving IM tone path to service tab storage.
     *
     * @param aTonePath tone path to be saved.
     * @since S60 v5.0
     */
    void SaveImTonePathL( const TDesC& aTonePath );

    /**
     * Returns handle to the listbox.
     *
     * @since S60 v3.2
     * @return handle to the listbox
     */        
    CAknSettingStyleListBox* ListBox();

    /**
     * Returns selected list box item
     *
     * @since S60 v3.2
     * @return selected list box item
     */
    TMainListBoxItem CurrentItem() const;

    /**
     * Deletes the service in hand.
     *
     * @since S60 v5.2
     * @return ETrue if delete the service, EFalse if not delete the service.
     */
    TBool DeleteServiceL();

    /**
     * Launches cleanup plugin to remove settings.
     *
     * @since S60 v5.2
     * @param aServiceId ID of service to be removed.
     */ 
    void LaunchCleanupPluginL( TUint aServiceId ) const;

// from base class CCoeControl

    /**
     * From CCoeControl.
     */ 
    CCoeControl* ComponentControl( TInt aIndex ) const;

    /**
     * From CCoeControl.
     */
    TKeyResponse OfferKeyEventL( 
        const TKeyEvent& aKeyEvent, 
        TEventCode aType );

    /**
     * From CCoeControl.
     */ 
    void HandleResourceChange( TInt aType );

private:
    
    /**
     * Creates listbox item for setting items.
     *
     * @since S60 v3.2
     */
    void ConstructListBoxL();

    /**
     * Return caption for setting item.
     *
     * @since S60 v3.2
     * @param aItem for setting list item to be constructed
     * @return setting item caption
     */
    HBufC* GetCaptionL( TMainListBoxItem::TSettingItems aItem );

    /**
     * Get username.
     *
     * @since S60 v5.0
     * @param aUsername username is set to this. 
     */
    void GetUsernameL( RBuf& aUsername );

    /**
     * Get preferred service setting.
     *
     * @since S60 v5.0
     * @param aValue presence pref setting value is stored to this.
     */
    void GetPreferredServiceSettingL( RBuf& aValue );

    /**
     * Get preferred service setting when VCC is supported.
     *
     * @since S60 v5.0
     * @param aValue presence pref setting value is stored to this.
     */
    void GetVccPreferredServiceSettingL( RBuf& aValue );

    /**
     * Get handover notification tone setting.
     *
     * @since S60 v5.0
     * @param aValue handover notification tone setting value is stored 
     * to this.
     */
    void GetHandoverNotificationTonePrefL( RBuf& aValue );

    /**
     * Get precence preferred setting.
     *
     * @since S60 v5.0
     * @param aValue presence pref setting value is stored to this.
     */
    void GetPresencePrefSettingL( RBuf& aValue );

    /**
     * Get IM message tone setting.
     *
     * @since S60 v5.0
     * @param aValue im tone setting value is stored to this.
     */
    void GetImToneSettingL( RBuf& aValue );

    /**
     * Get SNAP settings.
     *
     * @since S60 v5.0
     * @param aValue snap setting value is stored to this.
     */
    void GetSnapSettingL( RBuf& aValue );

    /**
     * Draws setting items at the first time 
     * after the initialization is completed.
     *
     * @since S60 v3.2
     */
    void InitializeSettingItemsL();

    /**
     * Constructs setting list items.
     *
     * @since S60 v3.2
     * @param aItem for setting list item to be constructed
     */
    void MakeSettingItemL( TMainListBoxItem::TSettingItems aItem );

    /**
     * Cleanup RImplInfoPtrArray
     *
     * @since S60 v5.2
     * @param aArray Array to be destroyed.
     */
     static void ResetAndDestroy( TAny* aArray );

// from base class CCoeControl

    /**
     * From CCoeControl.
     */
    TInt CountComponentControls() const;

    /**
     * From CCoeControl
     */
    void GetHelpContext( TCoeHelpContext& aContext ) const;

    /**
     * From CCoeControl.
     */
    void SizeChanged();

    /**
     * From CoeControl.
     */        
    void FocusChanged( TDrawNow aDrawNow );

private:  // data

    /**
     * Reference to model class for settings handling.
     */
    CCSCSettingsUiModel& iModel;

    /**
     * Listbox for main view setting page items.
     * Own.
     */
    CAknSettingStyleListBox* iListBox;    

    /**
     * Listbox item array
     */
    RArray<TMainListBoxItem> iListBoxItemArray;

    /**
     * Setting item caption
     */
    HBufC* iCaption;

#ifdef _DEBUG
    friend class UT_cscsettingsui;
#endif        

    };

#endif // C_CSCSETTINGSUIMAINCONTAINER_H

