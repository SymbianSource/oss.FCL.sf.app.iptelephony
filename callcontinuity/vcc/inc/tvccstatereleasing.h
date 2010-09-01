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
* Description:   Function definitions for VCC Releasing State
*
*/



#ifndef TVCCSTATERELEASING_H
#define TVCCSTATERELEASING_H

//  INCLUDES
#include <e32base.h>
#include <e32cmn.h> // HBufC

#include "tvccstate.h"

/**  Releasing state name */
const TUint KVccStateReleasing = 2;

//forward declarations
class CVccPerformer;

/**
* The base class for VCC state machine implementation.
* Contains default implementations for the functions.
* 
*  @lib vccperformer.dll
*  @since S60 3.2
*/
class TVccStateReleasing: public TVccState
    {
          
public:
	/**
	* c'tor
	*/
	
	TVccStateReleasing();

    /**
    * Links the state i.e. provides transition points.
    * @param aInit an init state
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
     * @see TVccState::CallEventOccurred()
     */
	void CallEventOccurred( CVccPerformer& aContext, 
	            const MCCPCallObserver::TCCPCallEvent aEvent,
	            MCCPCall* aCall );
	    
private:
    //no copying
    TVccStateReleasing(const TVccStateReleasing&);
    TVccStateReleasing operator = (const TVccStateReleasing&);

private:
    
    /**
     * Reference to Init state
     * Not own.
     */
	TVccState* iInit;
};

#endif      // TVCCSTATERELEASING_H

// End of File
