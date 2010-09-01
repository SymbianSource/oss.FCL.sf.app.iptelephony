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



#ifndef CVCCTRANSFERPROVIDER_H
#define CVCCTRANSFERPROVIDER_H

class CVccPerformer;

#include <mccptransferprovider.h>           // MCCPTransferProvider
#include <mccptransferobserver.h>           // MCCPTransferObserver

/**
 *  DTMF provider.
 *  Contains DTMF providers from call providers and forward all requests between
 *  them and CCE
 *
 *  @lib vccperformer.lib
 *  @since S60 v3.2
 */
class CVccTransferProvider : public CBase,
                             public MCCPTransferProvider,
                             public MCCPTransferObserver
    {

public:

    /**
	 * Two-phased constructor.
     * @param aProviders Service providers
     * @param aObserver
     * @param aDirector Reference to Director object 
	 */
	static CVccTransferProvider* NewL ( CVccPerformer* aCall );
    
    /**
    * Destructor.
    */
    virtual ~CVccTransferProvider(); 

    /**
	* Attended transfer to given call recipient.
	* There needs to exist two calls. a call between A-B and A-C. A wants to transfer the call to B-C. 
	* Both calls A-B and A-C will be disconnected after transfer.
	* In above case C will be the aTransferTargetCall call recipient. B is the recipient of the 
	* current call between A-B - called on A's call object transfer provider. 
	* The A-B call is on hold.
    * Actual request result indication comes via observer classes
	* @param aTransferTargetCall Call containing the transfer target info.
    * @return KErrNone if request was started succesfully. 
    * @return KErrNotReady if call is not in connected or on-hold state.
    * @return KErrArgument transfer traget address was illegal.
    * @pre Call state is MCCPCallObserver::ECCPStateConnected or MCCPCallObserver::ECCPStateHold
	* @since S60 3.2
	*/
	TInt AttendedTransfer( MCCPCall& aTransferTargetCall );

    /**
	* Attended transfer to given address. There does not need to be 
	* a current call between the given transfer target.
    * Actual request result indication comes via observer class
	* @param aTransferTarget Transfer target address
    * @return KErrNone if request was started succesfully. 
    * @return KErrNotReady if call is not in connected or on-hold state.
    * @return KErrArgument transfer address was illegal.
    * @pre Call state is MCCPCallObserver::ECCPStateConnected or MCCPCallObserver::ECCPStateHold
	* @since S60 3.2
	*/
	TInt AttendedTransfer( const TDesC& aTransferTarget );

	/**
	* Unattended transfer. Call is requested to be transferred to given address.
	* After MCCPTransferObserver::ECCPRemoteTransferring event current call will be 
	* disconnected and no transfer status is checked from the operation. 
    * Actual request result indication comes via observer classes
	* @since S60 3.2
	* @param aTransferTarget Address of the target
    * @return KErrNone if request was started succesfully. 
    * @return KErrNotReady if call is not in connected or on-hold state.
    * @return KErrArgument transfer address was illegal.
    * @pre Call state is MCCPCallObserver::ECCPStateConnected or MCCPCallObserver::ECCPStateHold
	*/
	TInt UnattendedTransfer( const TDesC& aTransferTarget );

	/**
	* Accept incoming call transfer request from the call remote party.
    * Actual request result indication comes via observer class
	* @since Series 60 3.2
	* @param aAccept ETrue - accept transfer, EFalse do not accept transfer request.
    * @return KErrNone if request was started succesfully. 
    * @return KErrNotReady if call is not in connected or on-hold state.
    * @return KErrArgument transfer address was illegal.
    * @pre Call state is MCCPCallObserver::ECCPStateConnected or MCCPCallObserver::ECCPStateHold
	* @pre MCCPObserver::CallCreated is called with the newly created call
	*/
	TInt AcceptTransfer( const TBool aAccept );
	  
    /**
    * When the other end of the call has requested call transfer to third party it is notified to 
    * MCCPTransferObserver::TransferEventOccurred(MCCPTransferObserver::ECCPRemoteTransferring).
    * The new recipient of the call can be fetched via this method.
	* @since Series 60 3.2
	* @param none
	* @return new recipient for the call after transfer
	* @return KNullDesC if no transfer target is available
    * @pre Call state is MCCPCallObserver::ECCPStateConnected or MCCPCallObserver::ECCPStateHold
	* @pre MCCPObserver::CallCreated is called with the newly created call
	*/
	const TDesC& TransferTarget() const;
	
    /**
    * Add an observer for transfer related events.
    * Currently CCE will set only one observer.
    * @since S60 v3.2
    * @param aObserver Observer to add.
    * @return none
    * @leave system error if observer adding fails
    */
   void AddObserverL( const MCCPTransferObserver& aObserver );

    /**
    * Remove an observer.
    * @since S60 v3.2
    * @param aObserver Observer to remove.
    * @return KErrNone if removed succesfully. 
    * @return KErrNotFound if observer was not found.
    */
   TInt RemoveObserver( const MCCPTransferObserver& aObserver );
   
   void TransferEventOccurred( 
                   const MCCPTransferObserver::TCCPTransferEvent aEvent );
    
   void UpdateL();
private:

    /** 
     * C++ constructor
     * @param aObserver Reference to DTMF observer
     * @param aDirector Reference to Director
     */
    CVccTransferProvider( CVccPerformer* aCall );

    /** 
     * Symbian second-phase constructor
     * @param aProviders Reference to array of the Service Providers
     */
    void ConstructL( );

private: // data

	/**
	* DTMF event observers.
	* Own.
	*/
	RPointerArray<MCCPTransferObserver> iObservers;
    CVccPerformer* iPerfCall;
    MCCPTransferProvider* iCallTransferProvider;
	};

#endif // CVCCTRANSFERPROVIDER_H
