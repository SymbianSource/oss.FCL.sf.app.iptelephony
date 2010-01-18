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
* Description:  Declarition of CAcpQueryContainer
*
*/


#ifndef ACPQUERYCONTAINER_H
#define ACPQUERYCONTAINER_H

#include <coecntrl.h>

#include "accountcreationpluginconstants.h"

class CAcpController;
class CAknSettingStyleListBox;

/**
 *  CAcpQueryContainer class
 *  Declarition of CAcpQueryContainer.
 *
 *  @lib accountcreationplugin.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CAcpQueryContainer ) : public CCoeControl
    {
public:

    /**
     * Two-phased constructor.
     *
     * @param aRect Rectangle.
     */
    static CAcpQueryContainer* NewL( const TRect& aRect );

    /**
     * Two-phased constructor.
     *
     * @param aRect Rectangle.
     */
    static CAcpQueryContainer* NewLC( const TRect& aRect );

    /**
     * Destructor.
     */
    virtual ~CAcpQueryContainer();

    /**
     * Returns handle to the listbox.
     *
     * @since S60 v3.2
     * @return Handle to the listbox.
     */        
    CAknSettingStyleListBox* ListBox();

    /**
     * Returns index of selected listbox item.
     *
     * @since S60 v3.2
     * @return Index of selected listbox item.
     */
    TInt CurrentItemIndex() const;

    /**
     * Shows username setting page.
     *
     * @since S60 v3.2
     */
    void ShowUsernameSettingPageL();  

    /**
     * Shows password setting page.
     *
     * @since S60 v3.2
     */
    void ShowPwordSettingPageL();

    /**
     * Returns container data.
     *
     * @since S60 v3.2
     * @param aUsername for username
     * @param aPassword for password
     */        
    void GetContainerDataL( TDes& aUsername, TDes& aPassword );

// from base class CCoeControl

    /**
     * From CCoeControl.
     * Offers a key event.
     * 
     * @since S60 v3.2
     * @param aKeyEvent Key event to be offered.
     * @param aType Type of the key event.
     * @return Key response.
     */
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, 
        TEventCode aType );

    /**
     * From CCoeControl.
     * Returns component control.
     * 
     * @since S60 v3.2
     * @param aIndex Index of the component control to be returned.
     * @return Component control.
     */ 
    CCoeControl* ComponentControl( TInt aIndex ) const;

private:

    CAcpQueryContainer();
    void ConstructL( const TRect& aRect );

    /**
     * Creates listbox item for provider list items.
     *
     * @since S60 v3.2
     */
    void ConstructListBoxL();

    /**
     * Fills listbox with items.
     *
     * @since S60 v3.2
     */
    void FillListboxWithItemsL();

    /**
     * Updates listbox list item.
     *
     * @since S60 v3.2
     * @param aIndex for index item to be updated
     */
    void UpdateListboxItemL( const TInt aIndex );

    /**
     * Fills selected listbox item with member data.
     *
     * @since S60 v3.2
     * @param aIndex Index of listbox item to be updated.
     * @param aFirstLine Item caption text.
     * @param aSecondLine Item member data text.
     */
    void FillListboxItemDataL( const TInt aIndex,
        TDes& aFirstLine, TDes& aSecondLine );

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
     * Informs that focus has been changed.
     * 
     * @param aDrawNow Tells if the window must be redrawn.
     * @since S60 v3.2
     */        
    virtual void FocusChanged( TDrawNow aDrawNow );        

	/**
     * From CoeControl.
     * Called by framework when the view layout is changed.
     * 
     * @since S60 v3.2
     * @param aType Type of resource change.
     */
    virtual void HandleResourceChange( TInt aType );

private: // data

    /**
     * Listbox for main view setting page items.
     * Own.
     */
    CAknSettingStyleListBox* iListBox;

    /**
     * Localized "none" text from resources.
     * Own.
     */
    HBufC* iNone;

    /**
     * Setting item: Server username.
     */ 
    TBuf<KAcpSipUsername> iUsername;

    /**
     * Setting item: Server password.
     */ 
    TBuf<KAcpSipPassword> iPassword;

    };

#endif // ACPQUERYCONTAINER_H

// End of file.
