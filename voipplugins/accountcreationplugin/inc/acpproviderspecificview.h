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
* Description:  Declatirion of CAcpProviderSpecificView
*
*/


#ifndef ACPPROVIDERSPECIFICVIEW_H
#define ACPPROVIDERSPECIFICVIEW_H

#include <eikclb.h>
#include <aknview.h>

#include "acpproviderlistview.h"

class CAcpController;
class CAcpProviderSpecificContainer;
class CAcpQueryView;
class CAcpProviderListView;

/**
 *  CAcpProviderSpecificView class
 *  Declarition of CAcpProviderSpecificView.
 *
 *  @lib accountcreationplugin.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CAcpProviderSpecificView ) : public CAknView
    {   
public: 

    /**
     * Two-phased constructor.
     *
     * @param aController for plugins controller
     */
    static CAcpProviderSpecificView* NewL( CAcpController& aController,
        CAcpProviderListView& aProviderListView );

    /**
     * Two-phased constructor.
     *
     * @param aController for plugins controller
     */
    static CAcpProviderSpecificView* NewLC( CAcpController& aController,
        CAcpProviderListView& aProviderListView );

    /**
     * Destructor.
     */
    virtual ~CAcpProviderSpecificView();

// from base class CAknView

    /**
     * From CAknView.
     * Returns ID of the view.
     * 
     * @since S60 v3.2
     * @return ID of the view.
     */
    TUid Id() const;

private:

    CAcpProviderSpecificView( CAcpController& aController,
        CAcpProviderListView& aProviderListView );
    void ConstructL();

    /**
     * For changing text to the title pane.
     *
     * @since S60 v3.2
     * @param aText for title pane text
     */
    void SetTitlePaneTextL( const TDesC& aText ) const;

// from base class CEikAppUi

    /**
     * From CEikAppUi.
     * Handles command.
     * 
     * @since S60 v3.2
     * @param aCommand Command index.
     */
    void HandleCommandL( TInt aCommand );

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

private: // data  

    /**
     * Handle to Avkon view application UI.
     * Not own.
     */
    CAknViewAppUi* iUi;

    /**
     * Handle to plugins controller.
     */
    CAcpController& iController;

    /**
     * Handle to query view.
     * Not own.
     */
    CAcpQueryView* iQueryView;

    /**
     * Handle to provider list view.
     */
    CAcpProviderListView& iProviderListView;

    /**
     * Container class for provider specific view.
     * Own.
     */
    CAcpProviderSpecificContainer* iContainer;

    /**
     * Handle dialog utility to show global wait note.
     * Own.
     */
    CAcpDialog* iDialog;
    };

#endif  // ACPPROVIDERSPECIFICVIEW_H

// End of file.
