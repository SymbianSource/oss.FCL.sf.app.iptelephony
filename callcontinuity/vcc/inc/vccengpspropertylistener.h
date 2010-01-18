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



#ifndef C_VCCENGPSPROPERTYLISTENER_H
#define C_VCCENGPSPROPERTYLISTENER_H

#include <e32base.h>
#include <e32property.h>

#include "mvccengpspropertylistenerobserver.h"

/*
 *  CVccEngPsPropertyListener subsribes to the given property. It is used to 
 *  notify its observers about the changed the value of the property.
 *  
 *   
 *  @code
 *  void CExampleClass::ConstructL()
 * 	{
 *   CVccEngPsPropertyListener* iPropListener = 
 *				CVccEngPsPropertyListener::NewL( KPSVccPropertyCategory, 
                                                 KVccPropKeyHoRequest );
 * 	 iPropListener.AddObserver( this );
 * 	 iPropListener.Start();
 * 	}	
 * 
 * 	void CExampleClass::PropertyChangedL( 
            const TUid aCategoryId,
            const TUint aKeyId,
            const TInt aValue )
 * 	{
        
 *  	if( aKeyId )
 * 			{
 * 			switch( aValue )
 * 				{				
 * 				case EVccManualStartCsToPsHoRequest:
 * 					// start CS->PS handover 
 * 					break;
 * 				default:
 * 					break;
 * 				}			
 * 			}
 *  }
 *   
 *  @endcode
 
 * 
 *  @lib hoengine.lib
 *  @since S60 v3.2
 */
class CVccEngPsPropertyListener : public CActive
    {
public:

	/**
	 * Symbian two-phased constructor.
     *
     * @param aCategoryId	category uid 
     * @param aKeyId Id of a key of the property.
     */
    static CVccEngPsPropertyListener* NewL( const TUid aCategoryId, 
    										const TUint aKeyId );

    /**
     * Destructor.
     */
    virtual ~CVccEngPsPropertyListener();

    /**
     * Adds an observer
     * @param aObserver Reference to observer to be added
     */
    void AddObserverL( MVccEngPsPropertyListenerObserver& aObserver );

    /**
     * Removes an observer
     * @param aObserver  Reference to observer to be removed
     */
    void RemoveObserver( MVccEngPsPropertyListenerObserver& aObserver );

    /**
     * Get the current value of the property.
     * @return Current key value
     */
    TInt CurrentValue();

    /**
     * Starts the listener.
     */
    void Start();

private: 
	/**
	 * Constructor.
	 * 
	 * @param aPropertyUid	category uid 
	 * @param aKeyId Id of a key of the property.
	 */
	CVccEngPsPropertyListener( const TUid aCategoryId, const TUint aKeyId );
    
	/**
	 * 2nd phase constructor
	 */
 	void ConstructL();
 	
    /**
     * Notifies the observers about the property value change.
     * @leave Symbian error code in error case
     */
    void NotifyObserversL();

// from base class CActive.

    /**
     * @see CActive::RunL()
     */
    void RunL();
     
    /**
     * See CActive::DoCancel()
     */
    void DoCancel();
 
private: //data

	/**
	 * Uid of the property category to be watched.
	 */
 	TUid iCategoryId;
 
 	/**
 	 * The property key uid to be watched.
 	 */
 	TUint iKeyId;

 	/**
 	 * the property category.
 	 * Own
 	 */
 	RProperty iProperty;
   
 	/**
 	 * Obsevers for callback from this.
 	 * Own
 	 */
 	RPointerArray<MVccEngPsPropertyListenerObserver> iObservers;

    };

#endif // C_VCCENGPSPROPERTYLISTENER_H

