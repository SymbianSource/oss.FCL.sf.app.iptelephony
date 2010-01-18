/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Sends and receives DTMF tones
*
*/



#ifndef CVCCDTMFPROVIDER_H
#define CVCCDTMFPROVIDER_H


#include <mccpdtmfprovider.h>
#include <mccpdtmfobserver.h>

class CConvergedCallProvider;
class CVccDirector;

/**
 *  DTMF provider.
 *  Contains DTMF providers from call providers and forward all requests between
 *  them and CCE
 *
 *  @lib vccperformer.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CVccDtmfProvider ) : public MCCPDTMFProvider,
						                public MCCPDTMFObserver
    {

public:

    /**
	 * Two-phased constructor.
     * @param aProviders Service providers
     * @param aObserver
     * @param aDirector Reference to Director object 
	 */
	static CVccDtmfProvider* NewL(
	        RPointerArray<CConvergedCallProvider>& aProviders, 
	        const MCCPDTMFObserver& aObserver,
	        CVccDirector& aDirector );
    
    /**
    * Destructor.
    */
    virtual ~CVccDtmfProvider(); 

  // From MCCPDTMFObserver

 	/**
	* HandleDTMFEvents.
	* @since S60 3.2
	* @param aEvent Event type
	* @param aError Error code
	* @param aTone Character
	*/
	void HandleDTMFEvent( const MCCPDTMFObserver::TCCPDtmfEvent aEvent,
	                               const TInt aError,
	                               const TChar aTone ) const;
	                               
	// From MCCPDTMFProvider

	/**
 	* Cancels asynchronous DTMF string sending.
 	* @return KErrNone request was started successfully
 	* @return KErrNotFound nothing found to cancel.
 	* @since Series 60 3.2
 	*/
 	TInt CancelDtmfStringSending();

 	/**
 	* Starts the transmission of a single DTMF tone across a connected and active call.
 	* @since S60 3.2
 	* @param aTone tone to be played.
 	* @return KErrNone request was started successfully
 	* @return KErrArgument if the specified tone contains illegal dtmf character
 	* @return in case of an error system wide error code
 	*/
 	TInt StartDtmfTone( const TChar aTone );

 	/**
 	* Stops playing current DTMF tone.
 	* @since S60 3.2
 	* @return KErrNone request was started successfully
    * @return KErrNotReady not ready to perform the requested action.
 	*/
 	TInt StopDtmfTone();

 	/**
 	* Plays DTMF string.
 	* @since S60 3.2
 	* @param aString String to be played.
 	* @return KErrNone request was started successfully
 	* @return KErrArgument if the specified string contains illegal dtmf characters
 	*/
 	TInt SendDtmfToneString( const TDesC& aString );

 	/**
 	* Continue or cancel sending DTMF string which was stopped with 'w'-character
 	* in string.
 	* @since S60 3.2
 	* @param aContinue ETrue if sending of the DTMF string should continue,
 	* EFalse if the rest of the DTMF string is to be discarded.
 	* @return KErrNone request was started successfully
 	* @return KErrNotFound no send existed which to continue
 	*/
 	TInt ContinueDtmfStringSending( const TBool aContinue );
 	
	/**
	* Add an observer for DTMF related events.
	* Currently CCE will set only one observer.
	* @since S60 v3.2
	* @param aObserver Observer to add.
	* @leave system error if observer adding fails
	*/
	void AddObserverL( const MCCPDTMFObserver& aObserver );
	
	/**
	* Remove an observer.
	* @since S60 v3.2
	* @param aObserver Observer to remove.
	* @return KErrNone if removed succesfully. 
	* @return KErrNotFound if observer was not found.
	*/
	TInt RemoveObserver( const MCCPDTMFObserver& aObserver );
	
private:

    /** 
     * C++ constructor
     * @param aObserver Reference to DTMF observer
     * @param aDirector Reference to Director
     */
    CVccDtmfProvider( const MCCPDTMFObserver& aObserver, CVccDirector& aDirector );

    /** 
     * Symbian second-phase constructor
     */
    void ConstructL();

private: // data

	/**
	* DTMF event observers.
	* Own.
	*/
	RPointerArray<MCCPDTMFObserver> iObservers;
	
	/**
	* DTMF providers.
	* Own
	*/
	RPointerArray<MCCPDTMFProvider> iProviders;
	
	/**
	 * VCC director
	 * Not own.
	 */
	CVccDirector* iDirector;
	
    friend class T_CVccDTMFProvider;
    };

#endif // CVCCDTMFPROVIDER_H
