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
* Description:  Declatirion of CSCSettingsUiMainView
*
*/


#ifndef C_CSCSETTINGSUIMAINVIEW_H
#define C_CSCSETTINGSUIMAINVIEW_H

class CCSCSettingsUiModel;
class CAknRadioButtonSettingPage;

class CAknNavigationControlContainer;
class CAknNavigationDecorator;
#include "cscsettingsuimaincontainer.h"

/**
 *  CCSCSettingsUiMainView class
 *  Declarition of CCSCSettingsUiMainView.
 *
 *  @lib CSCSettingsUi.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CCSCSettingsUiMainView ) : public CAknView,
                                              public MEikListBoxObserver
    {
public: 

    /**
     * Two-phased constructor.
     *
     * @param aModel for reference to the model
     */
    static CCSCSettingsUiMainView* NewL( 
        CCSCSettingsUiModel& aModel );

    /**
     * Two-phased constructor.
     *
     * @param aModel for reference to the model
     */
    static CCSCSettingsUiMainView* NewLC( 
        CCSCSettingsUiModel& aModel );

    /**
     * Destructor.
     */
    virtual ~CCSCSettingsUiMainView();

    /**
     * Processes situation when softkeys need to be changed.
     *
     * @since S60 v3.2
     */
    void UpdateSoftkeysL();

    /**
     * Resets service settings when switching services.
     * 
     * @since S60 v5.2
     */
    void ResetViewL();

// from base class CAknView

    /**
     * From CAknView.
     */
    TUid Id() const;

private:

    CCSCSettingsUiMainView(
        CCSCSettingsUiModel& aModel );
    void ConstructL();

    /**
     * Changes given service name to title pane.
     *
     * @since S60 v3.2
     */   
    void SetTitleTextL();

    /**
     * For checking if editing of preferred service setting is allowed
     *
     * @since S60 v3.2
     * @return ETrue if editing allowed
     */
    TBool IsEditPreferredServiceSettingAllowedL();

    /**
     * Shows preferred service setting page.
     *
     * @since S60 v5.0
     */ 
    void ShowUsernameSettingPageL();

    /**
     * Shows preferred service setting page.
     *
     * @since S60 v3.2
     */ 
    void ShowPasswordSettingPageL();        

    /**
     * Shows preferred service setting page.
     *
     * @since S60 v3.2
     */ 
    void ShowPrefServiceSettingPageL();

    /**
     * Shows preferred service setting page when VCC is supported.
     *
     * @since S60 v5.0
     */ 
    void ShowVccPrefServiceSettingPageL();

    /**
     * Shows IM tone selection list.
     *
     * @since S60 v5.0
     */ 
    void ShowImToneSelectionListL();

    /**
     * Handles 'change' middle softkey selection.
     * 
     * @param aListBoxItem listbox item
     * @since S60 v5.0
     */ 
    void HandleMskChangeSelectionL( TMainListBoxItem aListBoxItem );

    /**
     * Changes preferred telephony setting value.
     * From VoIP(PS) to CS or vice versa.
     * @since S60 v3.2
     */ 
    void ChangePrefTelephonyValueL();

    /**
     * Changes handover notify tone value (on/off).
     * @since S60 v5.0
     */ 
    void ChangeHandoverNotifToneValueL();

    /**
     * Appends resource texts to the des array.
     *
     * @since S60 v3.2
     * @param aList Text array.
     * @param aItem Text resource id.
     */ 
    void AppendItemL(CDesCArrayFlat& aList, TInt aItem);

    /**
     * Launches Connection Method Settings Ui for editing destinations.
     *
     * @since S60 v5.0
     */   
    void LaunchCMSettingsUiL();

    /**
     * Handles returning to previous view where settingsui was launced.
     *
     * @since S60 v5.0
     * @param aViewBack for checking if back the view,
     * ETrue if back the view, EFalse if not.
     */
    void HandleReturnToPreviousViewL( TBool aViewBack = ETrue );

    /**
     * Handles exist from settingsui.
     *
     * @since S60 v5.0
     */
    void HandleSettingsUiExitL();

// from base class CEikAppUi

    /**
     * From CEikAppUi.
     */
    void HandleCommandL( TInt aCommand );

// from base class CAknView

    /**
     * From CAknView.
     */
    void DoActivateL( 
        const TVwsViewId& aPrevViewId, 
        TUid aCustomMessageId, 
        const TDesC8& aCustomMessage );

    /**
     * From CAknView.
     */
    void DoDeactivate();

// from base class MEikListBoxObserver

    /**
     * From MEikListBoxObserver.
     */
    void HandleListBoxEventL( 
        CEikListBox* aListBox, 
        TListBoxEvent aEventType );

    /**
     * From MEikListBoxObserver.
     */
    void HandleListBoxSelectionL();

    /**
     * From MEikListBoxObserver.
     */
    void DynInitMenuPaneL(
        TInt aResourceId, 
        CEikMenuPane* aMenuPane );

private: // data  

    /**
     * Handle to model class for settings handling.
     */
    CCSCSettingsUiModel& iModel;

    /**
     * Handle to title pane.
     * Not own.
     */
    CAknTitlePane* iTitlePane;

    /**
     * Container class for main view.
     * Own.
     */
    CCSCSettingsUiMainContainer* iContainer;

    /**
     * Navigation pane.
     * Not own.
     */
    CAknNavigationControlContainer* iNaviPane;

    /**
     * Navigation decorator.
     * Own.
     */
    CAknNavigationDecorator* iNaviDecorator;

    /**
     * Flag for telling if IM tone selection list is open.
     */
    TBool iImToneSelectionListOpen;

    /**
     * Flag for telling if service is deleted.
     */
    TBool iDeleted;

    /**
     * Flag for telling if SNAP list is open.
     */
    TBool iSnapListOpen;

#ifdef _DEBUG
    friend class UT_cscsettingsui;
#endif

    };

#endif  // C_CSCSETTINGSUIMAINVIEW_H

