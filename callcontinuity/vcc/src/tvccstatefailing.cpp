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
* Description:   Implementation for the VCC Failing state
*
*/



#include "tvccstatefailing.h"
#include "cvccperformer.h"
#include "rubydebug.h"



// -----------------------------------------------------------------------------
// c'tor
// -----------------------------------------------------------------------------
//
TVccStateFailing::TVccStateFailing()
	{
	RUBY_DEBUG_BLOCK( "TVccStateFailing::TVccStateFailing" );	
	} 

// -----------------------------------------------------------------------------
// Name
// -----------------------------------------------------------------------------
//	
TUint TVccStateFailing::Name() const
	{
	RUBY_DEBUG_BLOCK( "TVccStateFailing::Name" );
	return KVccStateFailing;
	}

// -----------------------------------------------------------------------------
// LinkState
// -----------------------------------------------------------------------------
//
void TVccStateFailing::LinkState( TVccState& aInit )
	{
	RUBY_DEBUG_BLOCK( "TVccStateFailing::LinkState" );
	iInit = &aInit;	
	}
	
// -----------------------------------------------------------------------------
// CallStateChanged
// -----------------------------------------------------------------------------
//	
void TVccStateFailing::CallStateChanged( CVccPerformer& aContext, 
                                const MCCPCallObserver::TCCPCallState aState,
                                MCCPCall* aCall )
	{
	RUBY_DEBUG_BLOCK( "TVccStateFailing::CallStateChanged" );
	InspectChangedStates(aContext, 
	                        aState, 
	                        aCall, 
	                        aContext.CallObserver(), 
	                        &MCCPCallObserver::CallStateChanged);
	}

// -----------------------------------------------------------------------------
// CallStateChangedWithInband
// -----------------------------------------------------------------------------
//  
void TVccStateFailing::CallStateChangedWithInband( CVccPerformer& aContext, 
                                const MCCPCallObserver::TCCPCallState aState,
                                MCCPCall* aCall )
    {
    RUBY_DEBUG_BLOCK( "TVccStateFailing::CallStateChangedWithInband" );
    InspectChangedStates(aContext, 
                            aState, 
                            aCall, 
                            aContext.CallObserver(), 
                            &MCCPCallObserver::CallStateChangedWithInband);  
    }

// -----------------------------------------------------------------------------
// From MCCPCallObserver
// -----------------------------------------------------------------------------
//
void TVccStateFailing::ErrorOccurred( CVccPerformer& aContext, 
                                const TCCPError aError,
                                MCCPCall* aCall )
    {
    RUBY_DEBUG_BLOCK( "TVccStateFailing::ErrorOccurred" );
    RUBY_DEBUG1( "TVccStateFailing::ErrorOccurred - error: %d", aError );
    __ASSERT_DEBUG( aContext.CallObserver()!=NULL, User::Invariant() );
    __ASSERT_DEBUG(aContext.PrimaryCall()!=NULL, User::Invariant());
    __ASSERT_DEBUG(aCall != NULL, User::Invariant());
    if( aCall == aContext.PrimaryCall() )
        {
        aContext.CallObserver()->ErrorOccurred( aError, aContext.PrimaryCall() );
        }
    }

// -----------------------------------------------------------------------------
// From MCCPCallObserver
// -----------------------------------------------------------------------------
//
void TVccStateFailing::CallEventOccurred(CVccPerformer& aContext, 
                                const MCCPCallObserver::TCCPCallEvent aEvent,
                                MCCPCall* aCall )
    {
    RUBY_DEBUG_BLOCK( "TVccStateFailing::CallEventOccurred" ); 
	__ASSERT_DEBUG(aContext.CallObserver()!=NULL, User::Invariant());
	__ASSERT_DEBUG(aCall != NULL, User::Invariant());
	__ASSERT_DEBUG(aContext.PrimaryCall()!=NULL, User::Invariant());
	    
	if( aCall == aContext.PrimaryCall() )
	    {
	    aContext.CallObserver()->CallEventOccurred( aEvent, aContext.PrimaryCall() );
	    }
	}

// -----------------------------------------------------------------------------
// CheckAndMoveToInit
// -----------------------------------------------------------------------------
//
void TVccStateFailing::CheckAndMoveToInit(CVccPerformer& aContext, MCCPCall* aCall,
                                            const MCCPCallObserver::TCCPCallState aState)
    {
       RUBY_DEBUG_BLOCK( "TVccStateFailing::CheckAndmoveToInit" ); 
        
       if ( !aContext.SecondaryCall() )
           {
           RUBY_DEBUG0( "Secondary call not exist - setting state to init");
           aContext.SetState(*iInit);
           }
     }

// -----------------------------------------------------------------------------
// InspectChangedStates
// -----------------------------------------------------------------------------
//
void TVccStateFailing::InspectChangedStates (CVccPerformer& aContext, 
                               const MCCPCallObserver::TCCPCallState aState,
                               MCCPCall* aCall,
                               MCCPCallObserver* aCallee,
                               void( MCCPCallObserver::*aCallback)
                               (const MCCPCallObserver::TCCPCallState, MCCPCall*))
    {
    __ASSERT_DEBUG( aContext.PrimaryCall() != NULL, User::Invariant() );
    __ASSERT_DEBUG(aCall != NULL, User::Invariant());

    RUBY_DEBUG1("call state is: %d", aState);
    // Remote party has disconnected the call during the handover
    if((aState == MCCPCallObserver::ECCPStateDisconnecting ||
        aState == MCCPCallObserver::ECCPStateIdle) && aCall == aContext.SecondaryCall() )
        {
        ReleaseCall(aContext, *aContext.SecondaryCall(), *iInit, KVccHoNok );
        }
    else if (aCall == aContext.PrimaryCall())
        {
        //events related to the old leg, so pass them to the telephony.
        (aCallee->*aCallback)( aState, aContext.PrimaryCall());
        }
            
    CheckAndMoveToInit( aContext, aCall, aState );
    }
