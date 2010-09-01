/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Active object that tracks changes on a PS property.
*
*/



#ifndef C_VCCPSPROPERTYLISTENER_H
#define C_VCCPSPROPERTYLISTENER_H

#include <e32base.h>
#include <e32property.h>

#include "mvccpspropertylistenerobserver.h"

/*
 *  CVccUiPsPropertyListener notes its observers when the value of the VCC
 *  property changes.
 *
 *  This class can get notifications about the VCC properties.
 *
 *  @code
 *  void CExampleClass::ConstructL()
 *  {
 *   CVccUiPsPropertyListener* iHoStatusListener =
 *              CVccUiPsPropertyListener::NewL( KVccPropKeyHoStatus );
 *   iHoStatusListener.AddObserver( this );
 *   iHoStatusListener.Start();
 *  }
 *
 *  void CExampleClass::VccPropertyChangedL( const TUint aKeyId,
            const TInt aValue )
 *  {
 *      if( aKeyId )
 *          {
 *          switch( aValue )
 *              {
 *              case EVccCsToPsHoStarted:
 *                  // show UI notification
 *                  break;
 *              default:
 *                  break;
 *              }
 *          }
 *  }
 *
 *  @endcode
 *
 *  @lib vccuipropertyhandler.lib
 *  @since S60 v3.2
 */
class CVccUiPsPropertyListener : public CActive
    {
public:

    /**
     * Symbian two-phased constructor.
     *
     * @param aKeyId Id of a key of the property.
     */
    IMPORT_C static CVccUiPsPropertyListener* NewL( const TUint aKeyId );

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CVccUiPsPropertyListener();

    /**
     * Add an observer
     * @param aObserver Reference to observer object 
     */
    IMPORT_C void AddObserverL( MVccPsPropertyListenerObserver& aObserver );

    /**
     * Remove an observer.
     * @param aObserver Reference to observer object
     */
    IMPORT_C void RemoveObserver( MVccPsPropertyListenerObserver& aObserver );

    /**
     * Get the current value of the property.
     * @return Property value
     */
    IMPORT_C TInt CurrentValue();

    /**
     * Start the listener. The listener subscribes to the given property.
     */
    IMPORT_C void Start();

private:
    
    /**
     * Constructor.
     *
     * @param aKeyId Id of a key of the property.
     */
    CVccUiPsPropertyListener( const TUint aKeyId );

    /**
     * Symbian 2nd phase constructor
     */
    void ConstructL();
    
    /**
     * Notifies the observers about the Ho state change.
     */
    void NotifyObserversL();

// from base class CActive.

    /**
     * @see CActive::RunL()
     */
    void RunL();

    /**
     * @see CActive::DoCancel()
     */
    void DoCancel();

private: //data

    /**
     * The uid of the property category to be watched.
     */
    TUid iCategoryId;

    /**
     * The property key uid to be watched.
     */
    TUint iKeyId;

    /**
     * the property category.
     */
    RProperty iProperty;

    /**
     * Obsevers for callback from this.
     * Observers are not owned.
     */
    RPointerArray<MVccPsPropertyListenerObserver> iObservers;

    };

#endif // C_VCCPSPROPERTYLISTENER_H

