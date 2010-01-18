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
* Description:  Declatirion of CAcpProviderListView
*
*/


#ifndef ACPPROVIDERLISTVIEW_H
#define ACPPROVIDERLISTVIEW_H

#include <eikclb.h>
#include <aknview.h>
#include <mspnotifychangeobserver.h>
#include <wlanmgmtcommon.h>
#include <xSPViewServices.h>

#include "macpdialogobserver.h"
#include "macpcontrollerobserver.h"
#include "macptimerobserver.h"
#include "accountcreationpluginconstants.h"
#include "accountcreationplugin.hrh" // enums

class CAcpTimer;
class CAcpDialog;
class CAcpProviderSpecificView;
class CAcpProviderListContainer;
class MAccountCreationPluginObserver;
class CAcpQueryView;
class CAcpController;
class CFbsBitmap;
class CSPNotifyChange;
class CEikonEnv;
class CSPSettings;

/**
 *  CAcpProviderListView class
 *  Declarition of CAcpProviderListView.
 *
 *  @lib accountcreationplugin.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CAcpProviderListView ) : public CAknView,
                                            public MEikListBoxObserver,
                                            public MAcpDialogObserver,
                                            public MAcpControllerObserver,
                                            public MAcpTimerObserver
    {
    public:

    /**
     * Two-phased constructor.
     *
     * @param aObserver Observer for notifying readiness.
     */
    static CAcpProviderListView* NewL(
        MAccountCreationPluginObserver& aObserver, TBool aLaunchedFromAI, CEikonEnv& aEikEnv );

    /**
     * Two-phased constructor.
     *
     * @param aObserver Observer for notifying readiness.
     */
    static CAcpProviderListView* NewLC(
        MAccountCreationPluginObserver& aObserver, TBool aLaunchedFromAI, CEikonEnv& aEikEnv );

    /**
     * Destructor.
     */
    virtual ~CAcpProviderListView();

    /**
     * Provisioning query.
     * 
     * @since S60 v3.2
     */
    void ProvisioningL();

    /**
     * Starts provider list download.
     * 
     * @since S60 v3.2
     */
    void DownloadProviderListL();
    
// from base class CAknView

    /**
     * From CAknView.
     * Returns view UID.
     * 
     * @since S60 v3.2
     * @return view UID.
     */
    TUid Id() const;

// from base class MAcpDialogObserver

    /**
     * From MAcpDialogObserver.
     * Informs that a dialog has been dismissed.
     * 
     * @since S60 v3.2
     * @param aError Error code.
     */ 
    void DialogDismissedL( TInt aError );

// from base class MAcpControllerObserver

    /**
     * From MAcpControllerObserver.
     * Informs that provider list is ready.
     * 
     * @since S60 v3.2
     * @param aError Error code.
     */ 
    void NotifyProviderListReady( TInt aError );
 
    /**
     * From MAcpControllerObserver.
     * Informs that downloading has been completed.
     * 
     * @since S60 v3.2
     * @param aError System wide error code.
     */ 
    void NotifyDownloadingCompleted( TInt aError );
    
    /**
      * From MAcpControllerObserver.
      * Informs that downloading sis file has been completed.
      * 
      * @since S60 v5.0
      * @param aFileName sis file name
      */ 
    void NotifyDownloadingSISCompleted( TDesC& aFileName );

    /**
     * From MAcpControllerObserver.
     * Informs that service settings have been saved.
     * 
     * @since S60 v3.2
     */ 
    void NotifyProvisioningCompleted();
    
    /**
     * From MAcpControllerObserver.
     * Notifies observer when settings have been completely saved.
     * 
     * @since S60 v3.2
     */ 
    void NotifySettingsSaved();
    
    /**
     * From MAcpTimerObserver
     * Notifies observer when timer expires.
     * 
     * @since S60 v5.0
     */ 
    void TimerExpired();


private:

    CAcpProviderListView( MAccountCreationPluginObserver& aObserver,
                          TBool aLaunchedFromAI, CEikonEnv& aEikEnv );
    
    void ConstructL();

    /**
     * For changing text to the title pane.
     * 
     * @since S60 v3.2
     */
    void SetTitlePaneTextL() const;
    
    /**
     * For saving account creation url to rcse
     * 
     * @since S60 v5.0
     * @param aServiceId service id
     */
    void SaveAccountCreationUrlL( TUint aServiceId );

// from base class CAknView

    /**
     * From CAknView.
     * Activates the view.
     * 
     * @since S60 v3.2
     * @param aPrevViewId ID of the previous view. Not used.
     * @param aCustomMessageId ID of custom message. Not used.
     * @param aCustomMessage Custom message. Not used.
     */
    void DoActivateL( const TVwsViewId& /*aPrevViewId*/, 
        TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/ );

    /**
     * From CAknView.
     * Deactivates the view.
     * 
     * @since S60 v3.2
     */
    void DoDeactivate();

    /**
     * From CAknView.
     * Handles commands.
     * 
     * @since S60 v3.2
     * @param aCommand Command ID.
     */
    void HandleCommandL( TInt aCommand );

// from base class MEikListBoxObserver

    /**
     * From MEikListBoxObserver.
     * Handles listbox events.
     * 
     * @since S60 v3.2
     * @param aListBox Listbox. Not used.
     * @param aEventType Event type.
     */
    void HandleListBoxEventL( CEikListBox* /*aListBox*/, 
        TListBoxEvent aEventType );

    /**
     * From MEikListBoxObserver.
     * Handles listbox selections.
     * 
     * @since S60 v3.2
     */
    void HandleListBoxSelectionL();

    /**
     * From MEikListBoxObserver.
     * Dynamically initiates menu pane.
     * 
     * @since S60 v3.2
     * @param aResourceId ID of resource.
     * @param aMenuPane Menu pane to be initiated.
     */
    void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

    /**
     * Informs that service settings have been saved.
     * 
     * @since S60 v3.2
     */
    void DoNotifyProvisioningCompletedL();

private: // data

    /**
     * Handle to plugins controller.
     * Own
     */
    CAcpController* iController;

    /**
     * Reference to observer.
     */
    MAccountCreationPluginObserver& iObserver;

    /**
     * Handle to Avkon view application UI.
     * Not own.
     */
    CAknViewAppUi* iUi;

    /**
     * Container class for provider list view.
     * Own.
     */
    CAcpProviderListContainer* iContainer;

    /**
     * Handle to provider specific view.
     * Not own.
     */
    CAcpProviderSpecificView* iProviderSpecificView;

    /**
     * Handle to query view.
     * Not own.
     */
    CAcpQueryView* iQueryView;

    /**
     * Handle dialog utility to show global wait note.
     * Own.
     */
    CAcpDialog* iDialog; 
    
    
    /**
     * Handle to acp timer.
     * Own.
     */
    CAcpTimer* iTimer;
    
    /**
     * Handle to serviec provider settings
     * Own.
     */
    CSPSettings* iSpSettings;

    /**
     * For restoring browser's setting later on.
     */
     TInt iWarning;

    /**
     * For restoring browser's setting later on.
     */
     TInt iDataSaving;
     
    /** 
     * Array of service ids.
     */
     RArray<TServiceId> iServiceIds;
          
    /**
     * Name of the service being installed.
     */
     HBufC* iServiceName;
     
     /**
      * Boolean indicating if CSC has been started from active idle
      */ 
     TBool iLaunchedFromAI;
     
     /**
      * Reference to CEikonEnv.
      */
     CEikonEnv& iEikEnv;

    /** 
     * ETrue if connection to NSA server is being established.
     */
     TBool iConnectingToServer;
     
     /*
      * Xsp view service
      * Own.
      */
     RxSPViewServices iXspViewServices;
     
     /*
      * Array of phonebook tab view ids
      */
     RArray<TInt32> iTabViewIds;
    };

#endif  // ACPPROVIDERLISTVIEW_H

// End of file.
