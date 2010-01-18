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
* Description:   Function definitions for VCC Init State
*
*/



#ifndef TVCCSTATEINIT_H
#define TVCCSTATEINIT_H

//  INCLUDES
#include <e32base.h>
#include <e32cmn.h> // HBufC

#include "tvccstate.h"

/**  Init state name */
const TUint KVccStateInit = 0;

//forward declarations
class CVccPerformer;

/**
* The base class for VCC state machine implementation.
* Contains default implementations for the functions.
* 
*  @lib vccperformer.dll
*  @since S60 3.2
*/
class TVccStateInit: public TVccState
    {
          
public:
	/**
	* c'tor
	*/
	TVccStateInit();
	
    /**
    * Links the state i.e. provides transition points.
    * @param aCalling reference to the calling state
    */	
	void LinkState( TVccState& aCalling );	
	
// From base class TVccState	

    /**
     * @see TVccState::Name()
     */
	TUint Name() const;
	
	/**
	* @see TVccState::Swap()
	*/
	TInt Swap(MCCPCall& aCall);

    /**
     * @see TVccState::ErrorOccurred()
     */
	void ErrorOccurred(CVccPerformer& aContext, 
	                    const TCCPError aError,
	                    MCCPCall* aCall );

    /**
     * @see TVccState::CallStateChanged()
     */
	void CallStateChanged(CVccPerformer& aContext, 
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
     * @see TVccState::CallCapsChanged()
     */
	void CallCapsChanged(CVccPerformer& aContext, const TUint32 aCapsFlags );
	
    /**
     * @see TVccState::SwitchL()
     */
	void SwitchL(CVccPerformer& aContext); 
	
private:
    //no copying
    TVccStateInit(const TVccStateInit&);
    TVccStateInit operator = (const TVccStateInit&);

private: // Data

    /**
     * Reference to Calling state
     * Not own.
     */
    TVccState* iCalling;
    };

#endif      // TVCCSTATEINIT_H

// End of File
