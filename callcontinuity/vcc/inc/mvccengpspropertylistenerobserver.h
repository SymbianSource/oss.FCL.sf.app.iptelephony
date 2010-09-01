/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Observer for property listener 
*
*/



#ifndef M_VCCENGPSLISTENEROBSERVER_H
#define M_VCCENGPSLISTENEROBSERVER_H

/**
 *  Observer class for CVccEngPsPropertyListener
 *
 *  The class that implements this observer interfaces gets notified about 
 *  the P&S property value changes.
 *
 *  @lib hoengine.lib
 *  @since S60 v3.2
 */
class MVccEngPsPropertyListenerObserver 
    {

public:

	/**
	 * Observer interface. Called when a P&S property value 
	 * has been updated.
	 * 
	 * @param aCategoryId	the category uid of the property
	 * @param akeyId the key uid of the property
	 * @param aValue Current value of the property.
	 */	
 	virtual void PropertyChangedL( 
 			const TUid aCategoryId,
            const TUint aKeyId,
            const TInt aValue ) = 0;
    
    };



#endif /*M_VCCENGPSLISTENEROBSERVER_H*/
