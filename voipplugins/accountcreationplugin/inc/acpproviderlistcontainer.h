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
* Description:  Declarition of CAcpProviderListContainer
*
*/


#ifndef ACPPROVIDERLISTCONTAINER_H
#define ACPPROVIDERLISTCONTAINER_H

#include <coecntrl.h>

class CFbsBitmap;
class CAcpController;
class CAknSettingStyleListBox;

/**
 *  CAcpProviderListContainer class
 *  Declarition of CAcpProviderListContainer.
 *
 *  @lib accountcreationplugin.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CAcpProviderListContainer ) : public CCoeControl
    {
    public:

    /**
     * Two-phased constructor.
     *
     * @param aController Plugins controller
     * @param aRect Rectangle
     */
    static CAcpProviderListContainer* NewL( CAcpController& aController,
        const TRect& aRect );

    /**
     * Two-phased constructor.
     *
     * @param aController Plugins controller
     * @param aRect Rectangle
     */
    static CAcpProviderListContainer* NewLC( CAcpController& aController,
        const TRect& aRect );

    /**
     * Destructor
     */
    virtual ~CAcpProviderListContainer();

    /**
     * Adds and updates listbox with providers.
     *
     * @since S60 v3.2
     */
    void AddProvidersToListboxL();

    /**
     * Returns handle to the listbox.
     *
     * @since S60 v3.2
     * @return handle to the listbox
     */        
    CAknSingleLargeStyleListBox* ListBox();

    /**
     * Returns index of selected listbox item.
     *
     * @since S60 v3.2
     * @return index of selected listbox item
     */
    TInt CurrentItemIndex() const;

    /**
     * Loads provider icons.
     *
     * @since S60 v3.2
     * @param aCount amount of providers
     */
    void LoadProviderIconsL( TInt aCount );
    
    /**
     * Resets listbox.
     * 
     * @since S60 v3.2
     */
    void ResetListBox();

// from base class CCoeControl

    /**
     * From CCoeControl.
     */ 
    CCoeControl* ComponentControl( TInt aIndex ) const;

    /**
     * From CCoeControl.
     */
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, 
        TEventCode aType );

private:

    CAcpProviderListContainer( CAcpController& aController );
    void ConstructL( const TRect& aRect );

    /**
     * Creates listbox item for provider list items.
     *
     * @since S60 v3.2
     */
    void ConstructListBoxL();

    /**
     * Loads default icons.
     *
     * @since S60 v3.2
     * @param aIconArray for icons
     */
    void LoadDefaultIconsL( CArrayPtr<CGulIcon>* aIconArray );

// from base class CCoeControl

    /**
    * From CCoeControl.
    * Returns number of component controls.
    * 
    * @since S60 v3.2
    * @return Number of component controls.
    */
    virtual TInt CountComponentControls() const;

    /**
    * From CCoeControl.
    * Informs that size has been changed.
    * 
    * @since S60 v3.2
    */
    virtual void SizeChanged();

    /**
    * From CoeControl.
    * Notifies that focus has been changed.
    * 
    * @since S60 v3.2
    * @param aDrawNow Tells if the window must be redrawn.
    */        
    virtual void FocusChanged( TDrawNow aDrawNow );        


    /**
     * From CoeControl.
     * Handles resource change.
     * Called by framework when the view layout is changed.
     * 
     * @since S60 v3.2
     * @param aType Type of resource.
     */
    virtual void HandleResourceChange( TInt aType );

private:  // data

    /**
     * Reference to plugins controller.
     */
    CAcpController& iController;

    /**
     * Listbox for main view setting page items.
     * Own.
     */
    CAknSingleLargeStyleListBox* iListBox;
    };

#endif // ACPPROVIDERLISTCONTAINER_H

// End of file.
