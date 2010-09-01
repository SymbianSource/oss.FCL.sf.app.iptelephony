/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   The base class for VCC state machine
*
*/



#ifndef TVCCSTATE_H
#define TVCCSTATE_H

//  INCLUDES
#include <e32base.h>
#include <e32cmn.h> // HBufC
#include <mccpcscall.h>
#include <cccpcallparameters.h>

#include "vccengpsproperty.h"
#include "cconvergedcallprovider.h"

//forward declarations
class CVccPerformer;

/**  Value for indicating unsuccesfull HO */
const TUint KVccHoNok = 0;

/**  Value for indicating succesfull HO */
const TUint KVccHoOk = 1;

/**
* The base class for VCC state machine implementation.
* Contains default implementations for the functions.
* 
*  @lib vccperformer.dll
*  @since S60 3.2
* 
*/
class TVccState 
    {
public:
    
    /**
    * Releases call
    * @since Series60 3.2
    * @param aCall Call to be removed
    * @param aProvs Reference to call provider array
    * @return Symbian error code
    */
    virtual TInt ReleaseCall( MCCPCall& aCall, 
                              RPointerArray<CConvergedCallProvider>& aProvs );
  
    /**
    * Gets the state name.
    * @since Series60 3.2 
    * @return Derived states name.
    */
    virtual TUint Name() const = 0;
    
    /**
    * Initiates a handover
    * @since Series60 3.2
    * @param aContext Reference to Performer object
    * @leave Symbian error code in error case. 
    */
    virtual void SwitchL( CVccPerformer& aContext );
     
// From  MCCPCall - look alikes.    
    /**
    * Answers to an incoming call.
    * @since S60 3.2
    * @param aCall Reference to a call object
    * @return KErrNone if request was started succesfully. In that case plug-in 
    * must return MCCPCallObserver::ECCPStateAnswering.
    * @return KErrNotReady if call is not in ringing or queued state.
    * @return KErrNotSupported if call does not support queueing. Plug-in will 
    * disconnect the call in this case.
    */
    virtual TInt Answer( MCCPCall& aCall );
      
    /**
    * Rejects an incoming call.
    * @since S60 3.2
    * @param aCall Reference to a call object
    * @return KErrNone if request was started succesfully. 
    * @return KErrNotReady if call is not in ringing or queued state.
    */
    virtual TInt Reject( MCCPCall& aCall );

    /**
    * Reguests plug-in to queue the call. 
    * @since S60 3.2
    * @param aCall Reference to a call object
    * @return KErrNone if request was started succesfully. 
    * @return KErrNotReady if call is not in ringing state or call waiting is not supported at the moment.
    */
    virtual TInt Queue( MCCPCall& aCall );

    /**
    * Starts dialing to recipient
    * @since S60 3.2
    * @param aCall Reference to a call object
    * @return KErrNone if request was started succesfully. In this case plug-in must return 
    * MCCPCallObserver::ECCPStateDialling.
    * @return KErrNotReady if call is not in idle state.
    * @return KErrNotSupported If call is not mobile originated.
    */
    virtual TInt Dial( MCCPCall& aCall );

    /**
    * Call is ringing
    * @since S60 3.2
    * @param aCall Reference to a call object
    * @return KErrNone if request was started succesfully.
    * @return KErrNotReady if call is not in idle state.
    */
    virtual TInt Ringing( MCCPCall& aCall );
      
    /**
    * Ends an ongoing call.
    * @since S60 3.2
    * @param aContext Reference to a Performer object
    * @param aCall Reference to a call object
    * @return KErrNone if request was started succesfully.
    * @return KErrAlreadyExists if call is already in idle state.
    */
    virtual TInt HangUp( CVccPerformer& aContext, MCCPCall& aCall );
      
    /**
    * Cancels the ongoing request
    * @since S60 3.2
    * @param aCall Reference to a call object
    * @return KErrNone if request was started succesfully.
    */
    virtual TInt Cancel( MCCPCall& aCall );
      
    /**
    * Puts the specified call on hold.
    * @since S60 3.2
    * @param aCall Reference to a call object
    * @return KErrNone if request was started succesfully. 
    * @return KErrNotReady if call is not in connected state.
    * @return KErrAlreadyExists if call is already held 
    * (MCCPCallObserver::ECCPStateHold).
    */
    virtual TInt Hold( MCCPCall& aCall );
      
    /**
    * Resumes the call.
    * @since S60 3.2
    * @param aCall Reference to a call object
    * @return KErrNone if request was started succesfully.
    * @return KErrNotReady if call is not in on-hold state.
    * @return KErrAlreadyExists if call is already in connected state 
    * MCCPCallObserver::ECCPStateConnected.
    */
    virtual TInt Resume( MCCPCall& aCall );
      
    /**
    * Swaps a connected call to its opposite state, either active or on hold
    * @since S60 3.2
    * @param aCall Reference to a call object
    * @return KErrNone if request was started succesfully.
    * @return KErrNotReady if call is not on in holded or connected state.
    */
    virtual TInt Swap( MCCPCall& aCall );
      
    /**
    * Returns the remote party of the call.
    * @since S60 3.2
    * @param aCall Reference to a call object
    * @return remote party address,
    */
    virtual const TDesC& RemoteParty( const MCCPCall& aCall ) const;

    /**
    * Returns the dialled party information.
    * @since S60 3.2
    * @param aCall Reference to a call object
    * @return dialled party address,
    */
    virtual const TDesC& DialledParty( const MCCPCall& aCall ) const;
      
    /**
    * Return boolean value if the incoming call is forwarded or not.
    * @since S60 3.2
    * @param aCall Reference to a call object
    * @return ETrue if call is forwarded, else EFalse
    */
    virtual TBool IsCallForwarded( const MCCPCall& aCall ) const;
     
    /**
    * Returns boolean value if the call is mobile originated or not.
    * @since S60 3.2
    * @param aCall Reference to a call object
    * @return ETrue if call is mobile originated, otherwise EFalse
    */
    virtual TBool IsMobileOriginated( const MCCPCall& aCall ) const;
      
    /**
    * Returns the state of the call.
    * @since S60 3.2
    * @param aCall Reference to a call object
    * @return TCCPCallState The current state of the call
    */
    virtual MCCPCallObserver::TCCPCallState State( const MCCPCall& aCall ) const;
      
    /**
    * Returns call control caps for this call.
    * This tells for CCE what call can currently do.
    * @since S60 3.2
    * @param aCall Reference to a call object
    * @return Call control caps defined in MCCPCallObserver::TCCPCallControlCaps.
    */
    virtual MCCPCallObserver::TCCPCallControlCaps Caps( const MCCPCall& aCall ) const;     
    
    /**
    * Return if call secure status is specified or not.
    * @since Series 60 5.0
    * @param None.
    * @return TBool ETrue if security is specified, else EFalse.
    */
    virtual TBool SecureSpecified( const MCCPCall& aCall ) const;
    
    /**
    * Get call's secure status.
    * @since Series 60 3.2
    * @param aCall Reference to a call object
    * @return TBool ETrue if security is enabled, else EFalse.
    */
    virtual TBool IsSecured( const MCCPCall& aCall ) const;
      
    /**
	* Set call parameters.
	* @since S60 3.2
	* @param aNewParams New call paramater information.
	* @return none
	*/
    virtual void SetParameters( const CCCPCallParameters& aNewParams );

    /**
	* Cet call parameters.
	* @since S60 3.2
	* @param aCall Reference to a call object
	* @return Current call paramater information.
	*/
    virtual const CCCPCallParameters& Parameters( const MCCPCall& aCall ) const;
      
    /**
    * Returns currently used tone for the call. Used in case tones are needed to be played.
	* See defenition for TCCPTone.
    * @since S60 3.2
    * @param aCall Reference to a call object
    * @return TCCPTone Used tone.
    */
    virtual TCCPTone Tone( const MCCPCall& aCall ) const;
      
    /**
    * Adds a new observer for MCCECall object. Called in MT call case. In MO 
    * case observer will be given at call construction.
    * Plug-in dependent feature if duplicates or more than one observers 
    * are allowed or not. Currently CCE will set only one observer.
    * @since S60 v3.2
    * @param aObserver Observer
    * @param aCall Reference to a call object
    * @return none
    * @leave system error if observer adding fails
    */
    virtual void AddObserverL( const MCCPCallObserver& aObserver, 
                               MCCPCall& aCall );

    /**
    * Remove an observer.
    * @since S60 v3.2
    * @param aObserver Observer
    * @param aCall Reference to a call object
    * @return KErrNone if removed succesfully. KErrNotFound if observer was not found.
    * Any other system error depending on the error.
    */
    virtual TInt RemoveObserver( const MCCPCallObserver& aObserver, 
                                 MCCPCall& aCall );
    
//from MCCPCSCall - look alikes.

  /**
	* Starts dialing to recipient
	* @since S60 3.2
	* @param aCallParams The call parameters used by the TSY (a TCallParamsPckg object)
	* @param aCall Reference to a call object
    * @return KErrNone if request was started succesfully. 
    * @return KErrNotReady if call is not in idle state.
    * @return KErrNotSupported If call is not mobile originated.
    * @pre Call state is MCCPCallObserver::ECCPStateIdle and call type is MO.
    * @since S60 3.2
    */	
	virtual TInt Dial( const TDesC8& aCallParams, MCCPCSCall& aCall );

	/**
	* Instruct to do no FDN checking when dialling.
	* Needs to be called before every dial for each call separately.
	* If this method is not called at all default is to use FDN. 
	* Value will be reset to default when call goes back to disconnected state.
	* @since S60 v3.2
	* @param aCall Reference to a call object
	* @return none
	*/
	virtual void NoFDNCheck( MCCPCSCall& aCall );

	/**
	* Returns call information
	* @since S60 3.2
	* @param aCallInfo TMobileCallInfoV3
	* @param aCall Reference to a call object
	* @return KErrNone if succesfull, otherwise another system wide error code
	*/
	virtual TInt GetMobileCallInfo( TDes8& aCallInfo, 
	                                const MCCPCSCall& aCall  ) const;

	/**
	* Switch in the alternating call mode of an alternating call
	* @since S60 3.2
	* @param aCall Reference to a call object
    * @return KErrNone or system error code.
	*/
	virtual TInt SwitchAlternatingCall( MCCPCSCall& aCall );

	/**
	* Gets the calls data call capabilities
	* @since S60 3.2
	* @param aCaps TMobileCallDataCapsV1
	* @param aCall Reference to a call object
	* @return KErrNone if the function member was successful, 
	* @return KErrNotSupported if call does not support circuit switched data, 
	* @return KErrNotFound if this call is not a data call
	*/
	virtual TInt GetMobileDataCallCaps( TDes8& aCaps, 
	                                    const MCCPCSCall& aCall ) const;
	
    /**
    * Log dialed  number. SAT related, check if dialed number is to be logged or not.
    * @since S60 3.2
    * @param aCall Reference to a call object
    * @return Log dialed number or not.
    */
    virtual TBool LogDialedNumber( const MCCPCSCall& aCall ) const;	

//from MCCPCallObserver - look alikes. 
    
 	/**
	* An error has occurred concerning a specific call.
	* @since S60 3.2
	* @param aContext Reference to Performer object
	* @param aError Error code.
	* @param aCall Reference to a call object
	* @return none
	*/
	virtual void ErrorOccurred( CVccPerformer& aContext, 
	                            const TCCPError aError,
	                            MCCPCall* aCall );

	/**
	* The state of the call has changed.
	* @since S60 3.2
	* @param aContext Reference to Performer object
	* @param aState Call state.
	* @param aCall Reference to a call object
	* @return none
	*/
	virtual void CallStateChanged( CVccPerformer& aContext, 
	        const MCCPCallObserver::TCCPCallState aState,
	        MCCPCall* aCall );

	/**
	* The state of the call has changed with inband tone, meaning network is playing 
	* the tone relating to the state.
	* @since S60 3.2
	* @param aContext Reference to Performer object
	* @param aState Call state.
	* @param aCall Reference to a call object
	* @return none
	*/
	virtual void CallStateChangedWithInband( CVccPerformer& aContext, 
            const MCCPCallObserver::TCCPCallState aState,
            MCCPCall* aCall );

	/**
	* An event has occurred concerning a specific call.
	* @since S60 3.2
	* @param aContext Reference to Performer object
	* @param aEvent Occurred event.
	* @param aCall Reference to a call object
	* @return none
	*/
	virtual void CallEventOccurred( CVccPerformer& aContext, 
	        const MCCPCallObserver::TCCPCallEvent aEvent,
	        MCCPCall* aCall );

	/**
	* Notifies changed call capabilities.
	* @since S60 3.2
	* @param aContext Reference to Performer object
	* @param aCapsFlags New capabilities for the call
	* @param aCall Reference to a call object
	* @return none
	*/
	virtual void CallCapsChanged( CVccPerformer& aContext, 
	                const TUint32 aCapsFlags,
	                MCCPCall* aCall);

protected:
 
    /**
    * Releases ongoing call.
    * @since Series60 3.2 
    * @param aContext Reference to Performer object
    * @param aCall Reference to a call object
    * @param aState Next call state
    * @param aHoValue Was the handover succesfull or not succesfull
    * @return KErrNone or Symbian error code
    */
    TInt ReleaseCall( CVccPerformer& aContext, MCCPCall& aCall, 
                      TVccState& aState, TUint aHoValue );
     
	 /**
     * C++ Constructor
     */
     TVccState();
    };

#endif      // TVCCSTATE_H

// End of File
