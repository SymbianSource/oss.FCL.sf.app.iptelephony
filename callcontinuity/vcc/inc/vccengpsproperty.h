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
* Description:   P&S property writer/reader
*
*/



#ifndef C_VCCENGINEPSPROPERTY_H
#define C_VCCENGINEPSPROPERTY_H

#include <e32base.h>
#include <e32std.h>	
#include <e32property.h>

#include <vccsubscribekeys.h>

/**
 *  CVccEngPsProperty manages the VCC P&S keys on the VCC HO Engine side.  
 * 
 *  This class defines the property keys used by Vcc Ho Engine.
 *  It provides access to read/write values. Keys are deleted 
 *  when this class is deleted.
 * 
 *  Key KVccPropKeyHoStatus:
 *   EVccHoStateIdle 			Everything ok   
 *   EVccCsToPsHoStarted 		CS->PS HO started
 *   EVccCsToPsHoInprogress 	CS->PS HO in progress.
 *   EVccCsToPsHoSuccessful		CS->PS HO ended successfully.
 *   EVccCsToPsHoFailure 		HO server encountered failure.
 *   EVccHoStateUnknown 		nok, key value could not be fetched
 *   
 *  @code
 * 		CVccEngPsProperty* iVccPsp = CVccEngPsProperty::NewL();
 * 		...
 * 		iVccPsp.NotifySubscriberL( EVccCsToPsHoStarted );
 * 		...
 * 		iVccPsp.NotifySubscriberL( EVccCsToPsHoSuccessful );
 * 		...
 * 		delete iVccPsp;		
 *  @endcode
 * 
 *  @lib vcchoengine.lib
 *  @since S60 v3.2
 */
class CVccEngPsProperty : public CBase
{
public:
	/**
	 * Two-phased constructor.
     *
	 */
	static CVccEngPsProperty* NewL();
	
	/**
	 * Destructor.
	 */
	virtual ~CVccEngPsProperty();
	
	/**
     * Updates the Vcc property keys KVccPropKeyHoStatus and 
     * KVccPropKeyHoError. Subscribers of these properties will get the 
     * notification about the changed values.
     *
     * @since S60 5.0
     * @param aStatus The new value of the KVccPropKeyHoStatus key
     * @param aErrorCode The new value of the KVccPropKeyHoError key
     */
    void NotifySubscriberL( TVccHoStatus aStatus, 
    						TInt aErrorCode );

    /**
     * Gets the latest value of the Vcc property key KVccPropKeyHoStatus.
     *
     * @since S60 5.0
     * @param aStatus The value of the key
     * @return KErrNone / KErrPermissionDenied/ KErrNotFound/KErrArgument 
     */
    TInt GetCurrentHoStatus( TVccHoStatus& aStatus );

    /**
     * Gets the latest value of the Vcc property key KVccPropKeyHoError.
     *
     * @since S60 5.0
     * @param aErrorCode The value of the key
     * @return KErrNone / KErrPermissionDenied/ KErrNotFound/KErrArgument 
     */
    TInt GetCurrentErrorCode( TInt& aErrorCode );
    
private:
	/**
	 * Constructor
	 */
	CVccEngPsProperty();

	/**
	 * Constructor
	 */
	void ConstructL();

	/**
	 * Defines keys used by Vcc Ho engine
	 */
	void DefineKeysL();
	
	/**
	 * creates handles to properties
	 */
	void CreateHandlesL();
	
private: //data
	
	/**
	 * Handle to ho status property 
	 */
	RProperty iHoStatusKey;
	
	/**
	 * Handle to error code property 
	 */
	RProperty iErrorCodeKey;
};

#endif /*C_VCCENGPSPROPERTY_H*/
