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
* Description:   Function definitions for VCC Failing State
*
*/



#ifndef TVCCSTATEFAILING_H
#define TVCCSTATEFAILING_H

//  INCLUDES
#include <e32base.h>
#include <e32cmn.h> // HBufC

#include "tvccstate.h"

/**  Failing state name */
const TUint KVccStateFailing = 3;

//forward declarations
class CVccPerformer;

/**
* Failing state. Can be accessed from calling state if some problem
* occurs.
*
* init -> calling -> releasing -> init
* 
*  @lib vccperformer.dll
*  @since S60 3.2
*/
class TVccStateFailing: public TVccState
    {
          
public:
	/**
	* c'tor
	*/
	TVccStateFailing();
	
    /**
    * Links the state i.e. provides transition points.
    * @param aReleasing a realising state
    */
    void LinkState( TVccState& aInit );
	
// From base class TVccState
    
    /**
     * @see TVccState::Name()
     */
	TUint Name() const;
	
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
    TVccStateFailing( const TVccStateFailing& );
    TVccStateFailing operator = ( const TVccStateFailing& );
    
    /**
    * If new leg is cleared, set the state again to init.
    * param aContext Reference to Performer object
    */
    void CheckAndMoveToInit( CVccPerformer& aContext,  MCCPCall* aCall,
                                        const MCCPCallObserver::TCCPCallState aState );
    
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
     * Reference to Init state
     * Not own.
     */
	TVccState* iInit;
	
	};

#endif      // TVCCSTATEFAILING_H

// End of File
