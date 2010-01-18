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
* Description:  Declarition of CAcpProviderSpecificContainer
*
*/


#ifndef ACPPROVIDERSPECIFICCONTAINER_H
#define ACPPROVIDERSPECIFICCONTAINER_H

#include <coecntrl.h>
#include <coecntrl.h>
#include <barsread.h>
#include <eiklabel.h>
#include <eikedwin.h>

#include "accountcreationpluginconstants.h"

class CAcpController;
class CAknsBasicBackgroundControlContext;

/**
 *  CAcpProviderSpecificContainer class
 *  Declarition of CAcpProviderSpecificContainer.
 *
 *  @lib accountcreationplugin.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CAcpProviderSpecificContainer ) : public CCoeControl
    {
public:

    /**
     * Two-phased constructor.
     *
     * @param aController for plugins controller
     * @param aRect for rectangle
     */
    static CAcpProviderSpecificContainer* NewL( 
        CAcpController& aController, const TRect& aRect );

    /**
     * Two-phased constructor.
     *
     * @param aController for plugins controller
     * @param aRect for rectangle
     */
    static CAcpProviderSpecificContainer* NewLC( 
        CAcpController& aController, const TRect& aRect );

    virtual ~CAcpProviderSpecificContainer();
    
private:

    CAcpProviderSpecificContainer( CAcpController& aController );
    void ConstructL( const TRect& aRect );
    
public: // New methods.
    
    /**
     * Scrolls text up or down by the given amount of pixels.
     * 
     * @since S60 v3.2
     * @param aDelta Amount of pixels to move the text. Positive is down.
     */
    void ScrollText( TInt aDelta );
    
    /**
     * Repositions the labels.
     * 
     * @since S60 v3.2
     */
    void RepositionLabels();

// from base class CCoeControl
    
    /**
     * From CCoeControl.
     */ 
    CCoeControl* ComponentControl( TInt aIndex ) const;

    /**
     * From CCoeControl.
     * Supplies context (object ID) to child controls.
     *
     * @since S60 v3.2
     * @param aId UID.
     * @return UID.
     */
    TTypeUid::Ptr MopSupplyObject( TTypeUid aId );

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
     * Informs that draw must be committed.
     * 
     * @since S60 v3.2
     * @param aRect Rectangle.
     */
    void Draw( const TRect& aRect ) const;

    /**
     * From CCoeControl.
     * Informs that size has been changed.
     * 
     * @since S60 v3.2
     */
    virtual void SizeChanged();

	/**
     * From CoeControl.
     * Handles resource change.
     * Called by framework when the view layout is changed.
     * 
     * @since S60 v3.2
     * @param aType Type of resource.
     */
    virtual void HandleResourceChange( TInt aType );
    
    /**
     * From CoeControl.
     * Handles keypresses.
     * 
     * @since S60 v3.2
     * @param aKeyEvent Key event.
     * @param aType Type of event.
     */
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

private:  // data

    /**
     * Reference to plugins controller.
     */
    CAcpController& iController;

    /**
     * Localized "none" text from resources.
     * Own.
     */
    HBufC* iNone;

    /**
     * Setting item: description.
     * Own.
     */ 
    HBufC* iDescription;

    /**
     * Label for name.
     * Own.
     */
    CEikLabel* iLabel;

    /**
     * Label for type.
     * Own.
     */
    CEikLabel* iLabel2;

    /**
     * Label for description.
     * Own.
     */
    CEikLabel* iLabel3;

    /**
     * Edwin for provider specific name text.
     * Own.
     */
    CEikEdwin* iEdwin;

    /**
     * Edwin for provider specific type text.
     * Own.
     */
    CEikEdwin* iEdwin2;

    /**
     * Edwin for provider specific description text.
     * Own.
     */
    CEikEdwin* iEdwin3;

    /**
     * SKIN: the skin bitmap context for our control.
     * Own.
     */
    CAknsBasicBackgroundControlContext* iBgContext;
    
    /**
     * Y position for label scrolling.
     */
    TInt iScrollY;
    };

#endif // ACPPROVIDERSPECIFICCONTAINER_H

// End of file.
