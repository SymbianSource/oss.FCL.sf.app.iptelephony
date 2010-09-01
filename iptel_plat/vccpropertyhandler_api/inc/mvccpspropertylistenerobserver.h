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
* Description:   Observer for VCC HO property watcher 
*
*/



#ifndef M_VCCPSPROPERTYLISTENEROBSERVER_H
#define M_VCCPSPROPERTYLISTENEROBSERVER_H

/**
 *  Observer class for CVccUiPsPropertyListener
 *
 *  The class that implements this observer interfaces gets notified about 
 *  the VCC P&S property value changes.
 *
 *  @lib vccuipropertyhandler.lib
 *  @since S60 v3.2
 */
class MVccPsPropertyListenerObserver 
    {

public:

	/**
	 * Observer interface. Called when a P&S property value 
	 * has been updated.
	 * 
	 * @param akeyId VCC key uid
	 * @param aValue Current value of the VCC property.
	 */	
 	virtual void VccPropertyChangedL( 
            const TUint aKeyId,
            const TInt aValue ) = 0;
    
    };



#endif /*M_VCCPSPROPERTYLISTENEROBSERVER_H*/
