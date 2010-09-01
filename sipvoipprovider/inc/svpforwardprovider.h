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
* Description:  Forward functionality provider
*
*/




#ifndef C_SVPFORWARDPROVIDER_H
#define C_SVPFORWARDPROVIDER_H


#include <mccpforwardprovider.h>
#include "svputdefs.h"

class CSVPMoSession;

/**
 *  Forward functionality provider
 *
 *  ?more_complete_description
 *
 *  @lib svp.lib
 *  @since S60 v3.2
 */
class CSVPForwardProvider : public CBase,
                            public MCCPForwardProvider
    {

public:

    static CSVPForwardProvider* NewL( CSVPMoSession& aSession );

    virtual ~CSVPForwardProvider();


// from base class MCCPForwardProvider

    /**
    * From MCCPForwardProvider.
	* Returns pointer to array containing addresses to forward the call to.
	* Index zero contains the first address. If there are no addresses available
	* returns an empty array.
	* @since Series 60 3.2
	* @param none
	* @return Reference to array containing addresses
    * @leave KErrNotReady call is not in forwarding state
    * @leave KErrNotAllowed if call is not mobile originated
    * @leave system error code 
    * @pre Call state is MCCPCallObserver::ECCPStateForwarding and call type is MO
    * @pre Call MCCPForwardObserver::ECCPMultipleChoices event is received
	*/
	const CDesC8Array& GetForwardAddressChoicesL();

	/**
	* From MCCPForwardProvider.
	* Forward call to address at given index.
	* @since Series 60 3.2
	* @param aIndex Index of address where the call is to be forwarded. Address is 
	* found in the array received from GetForwardAddressChoicesL. Index starts from zero.
	* @return none
	* @leave KErrArgument Index is not in array
    * @leave KErrNotReady Call is not in forwarding state
    * @leave KErrNotAllowed If call is not mobile originated
    * @pre Call state is MCCPCallObserver::ECCPStateForwarding and call type is MO
    * @pre Call MCCPForwardObserver::ECCPMultipleChoices event is received
	*/
	void ForwardToAddressL( const TInt aIndex );
	
    /**
    * From MCCPForwardProvider.
    * Add an observer for forward related events.
    * Currently CCE will set only one observer.
    * @since S60 v3.2
    * @param aObserver Observer
    * @return none
    * @leave system error if observer adding fails
    */
    void AddObserverL( const MCCPForwardObserver& aObserver );

    /**
    * From MCCPForwardProvider.
    * Remove an observer.
    * @since S60 v3.2
    * @param none
    * @param aObserver Observer
    * @return KErrNone if removed succesfully. 
    * @return KErrNotFound if observer was not found.
    */
    TInt RemoveObserver( const MCCPForwardObserver& aObserver );



private:

    CSVPForwardProvider();

    void ConstructL( CSVPMoSession& aSession );

private: // data


    /**
     * MO Session
     * Not own.
     */
    CSVPMoSession* iMoSession;


private:

    // For testing
    SVP_UT_DEFS
    
    };


#endif // C_SVPFORWARDPROVIDER_H
