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
* Description:  Declatirion of CAcpQueryView
*
*/


#ifndef ACPQUERYVIEW_H
#define ACPQUERYVIEW_H

#include <eikclb.h>
#include <aknview.h>

#include "macpdialogobserver.h"

class CAcpController;
class CAcpQueryContainer;
class CAcpDialog;
class CEikonEnv;

/**
 *  CAcpQueryView class
 *  Declarition of CAcpQueryView class.
 *
 *  @lib accountcreationplugin.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CAcpQueryView ) : public CAknView,
                                     public MEikListBoxObserver,
                                     public MAcpDialogObserver
    {
public:

    /**
     * Items to be shown in this view/container.
     */
    enum TAcpGenericItems
        {
        EAcpGenericUsername = 0,
        EAcpGenericPassword
        };

    /**
     * Two-phased constructor.
     * @param aController ACP Controller
     */
    static CAcpQueryView* NewL( CAcpController& aController );

    /**
     * Two-phased constructor.
     * @param aController ACP Controller
     */
    static CAcpQueryView* NewLC( CAcpController& aController );

    /**
     * Destructor.
     */
    virtual ~CAcpQueryView();

// from base class CAknView

    /**
     * From CAknView.
     * Returns UID of the view class.
     * 
     * @since S60 v3.2
     * @return UID of the class.
     */
    TUid Id() const;

private:

    CAcpQueryView( CAcpController& aController );
    void ConstructL();

    /**
     * For changing text to the title pane.
     *
     * @since S60 v3.2
     * @param aText Text for title pane.
     */
    void SetTitlePaneTextL( const TDesC& aText ) const;

// from base class CEikAppUi

    /**
     * From CEikAppUi.
     * Handles command.
     * 
     * @since S60 v3.2
     * @param aCommand Command ID.
     */
    void HandleCommandL( TInt aCommand );

// from base class MEikListBoxObserver

    /**
     * From MEikListBoxObserver.
     * Handles list box event.
     * 
     * @since S60 v3.2
     * @param aListBox List box whose event is to be handled. Not used.
     * @param aEventType Type of the event.
     */
    void HandleListBoxEventL( 
        CEikListBox* /*aListBox*/, 
        TListBoxEvent aEventType );

    /**
     * From MEikListBoxObserver.
     * Handles list box selection.
     * 
     * @since S60 v3.2
     */
    void HandleListBoxSelectionL();

    /**
     * From MEikListBoxObserver.
     * Dynamically initiates menu pane.
     * Not used but here because of inheritance.
     * 
     * @since S60 v3.2
     * @param aResourceId ID of the resource. Not used.
     * @param aMenuPane Menu pane. Not used.
     */
    void DynInitMenuPaneL( TInt /*aResourceId*/, 
        CEikMenuPane* /*aMenuPane*/ );

// from base class MAcpDialogObserver
    
    /**
     * From MAcpDialogObserver.
     * Notifies that a dialog has been dismissed.
     * 
     * @since S60 v3.2
     * @param aError Error code. Not used.
     */ 
    void DialogDismissed( TInt /*aError*/ );

// from base class CAknView

    /**
     * From CAknView.
     * Activates this view.
     * 
     * @since S60 v3.2
     * @param aPrevViewId ID of previous view. Not used.
     * @param aCustomMessageId ID of a custom message. Not used.
     * @param aCustomMessage Custom message. Not used. 
     */
    void DoActivateL( 
        const TVwsViewId& /*aPrevViewId*/, 
        TUid /*aCustomMessageId*/, 
        const TDesC8& /*aCustomMessage*/ );

    /**
     * From CAknView.
     * Deactivates the view.
     * 
     * @since S60 v3.2
     */
    void DoDeactivate();

private: // data

    /**
     * Handle to Avkon view application Ui.
     * Not own.
     */
    CAknViewAppUi* iUi;

    /**
     * Container class for provider specific view.
     * Own.
     */
    CAcpQueryContainer* iContainer;

    /**
     * Handle to plugins controller.
     */
    CAcpController& iController;

    /**
     * Handle dialog utility to show global wait note.
     * Own.
     */
    CAcpDialog* iDialog;

    /**
     * For restoring browser's setting later on
     */
    TInt iWarning;

    /**
     * For restoring browser's setting later on
     */
    TInt iDataSaving;

   /**
    * An instance of CEikonEnv
    * Not own.
    */
    CEikonEnv* iEikEnv;
    };

#endif  // ACPQUERYVIEW_H

// End of file.

