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
* Description:   Function definitions for VCC Calling State
*
*/



#ifndef TVCCSTATECALLING_H
#define TVCCSTATECALLING_H

//  INCLUDES
#include <e32base.h>
#include <e32cmn.h> // HBufC

#include "tvccstate.h"

/**  Calling state name */
const TUint KVccStateCalling = 1;

//forward declarations
class CVccPerformer;

/**
* The base class for VCC state machine implementation.
* Contains default implementations for the functions.
* Current state machine is:
* 
* init -> calling -> releasing -> init
* 
*  @lib vccperformer.dll
*  @since S60 3.2
*/
class TVccStateCalling: public TVccState
    {
          
public:
	/**
	* c'tor
	*/
	TVccStateCalling();
	
    /**
    * Links the state i.e. provides transition points.
    * @param aReleasing reference to the realising state
    * @param aInit reference to the init state
    * @param afailing reference to the failing state
    */
    void LinkState( TVccState& aReleasing, 
                    TVccState& aInit, 
                    TVccState& aFailing);

// From base class TVccState
    
    /**
     * @see TVccState::Name()
     */
    TUint Name() const;
	
	/**
	* @See TVccState::HangUp()
	*/	
	TInt HangUp(CVccPerformer& aContext, MCCPCall& aCall);

    /**
     * @see TVccState::CallStateChanged()
     */
	void CallStateChanged( CVccPerformer& aContext, 
	                    const MCCPCallObserver::TCCPCallState aState,
	                    MCCPCall* aCall );
	
    /**
     * @see TVccState::CallStateChangedWithInband()
     */
	void CallStateChangedWithInband(CVccPerformer& aContext, 
	            const MCCPCallObserver::TCCPCallState aState,
	            MCCPCall* aCall );
	
    /**
     * @see TVccState::CallEventOccurred()
     */
	void CallEventOccurred( CVccPerformer& aContext, 
	            const MCCPCallObserver::TCCPCallEvent aEvent,
	            MCCPCall* aCall );
	    
    /**
     * @see TVccState::ErrorOccurred()
     */
    void ErrorOccurred( CVccPerformer& aContext, 
                        const TCCPError aError,
                        MCCPCall* aCall );	
	
private:
    
    //no copying
    TVccStateCalling(const TVccStateCalling&);
    TVccStateCalling operator = (const TVccStateCalling&);

    /**
    * Inspects the state changed for possible remote disconnect and other
    * failing hand-over cases.
    * @param aContext a state context
    * @param aState a current state value
    * @param aCall the call leg emetting the event
    * @param aCallee the object instance calling the callback
    * @param aCallback a function to call in case event must be emitted and not suppressed.
    */  
    void InspectChangedStates( CVccPerformer& aContext, 
                               const MCCPCallObserver::TCCPCallState aState,
                               MCCPCall* aCall,
                               MCCPCallObserver* aCallee,
                               void( MCCPCallObserver::*aCallback)(const MCCPCallObserver::TCCPCallState, MCCPCall*));

private: // data
    /**
     * Reference to Releasing state
     * Not own.
     */
	TVccState* iReleasing;

    /**
     * Reference to Init state
     * Not own.
     */
	TVccState* iInit;
	
    /**
     * Reference to Failing state
     * Not own.
     */
	TVccState* iFailing;
    };

#endif      // TVCCSTATECALLING_H

// End of File
