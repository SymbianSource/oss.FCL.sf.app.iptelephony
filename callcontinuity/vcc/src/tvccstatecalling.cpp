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
* Description:   Implementation for the VCC Calling state
*
*/

#include "tvccstatecalling.h"
#include "cvccperformer.h"
#include "rubydebug.h"


// -----------------------------------------------------------------------------
// c'tor
// -----------------------------------------------------------------------------
//
TVccStateCalling::TVccStateCalling()
	{
	RUBY_DEBUG_BLOCK( "TVccStateCalling::TVccStateCalling" );	
	} 

// -----------------------------------------------------------------------------
// Name
// -----------------------------------------------------------------------------
//	
TUint TVccStateCalling::Name() const
	{
	RUBY_DEBUG_BLOCK( "TVccStateCalling::Name" );
	return KVccStateCalling;
	}

// -----------------------------------------------------------------------------
// LinkState
// -----------------------------------------------------------------------------
//
void TVccStateCalling::LinkState(TVccState& aReleasing, 
                                TVccState& aInit, 
                                TVccState& aFailing )
	{
	RUBY_DEBUG_BLOCK( "TVccStateCalling::LinkState" );
	iReleasing = &aReleasing;
	iInit = &aInit;	
	iFailing = &aFailing;
	}
	
// -----------------------------------------------------------------------------
// HangUp
// -----------------------------------------------------------------------------
//
TInt TVccStateCalling::HangUp(CVccPerformer& aContext, MCCPCall& aCall)
	{
    RUBY_DEBUG_BLOCK( "TVccStateCalling::HangUp" );
    
	TInt err = KErrNotFound;
	
	__ASSERT_DEBUG(&aCall == aContext.PrimaryCall(), User::Invariant());
	
	//must release old leg and put old observer back aCall is the
	// same as returned in the iContext.PrimaryCall()
	aCall.RemoveObserver(aContext);
	
	//add new observer because the original call will be released and events 
	//must be suppressed and performer does not have to be on the path for this 
	//call anymore.
	TRAP_IGNORE(aCall.AddObserverL(*aContext.CallObserver()));
	
	//now hangup old leg and forget about it - CCE will take of it.
	//The performer will not be aware of the old leg release events.
	err = aCall.HangUp();
	
	//must release new call leg as well and suppress the events 	
	//new leg idle event should come in the releasing-state and
	//it will be suppressed.
	__ASSERT_DEBUG(aContext.SecondaryCall() != NULL, User::Invariant());
	aContext.SecondaryCall()->HangUp();
	
	//signal the performer for the "idle call", so it can stop the trigger
	//move to the releasing state
	aContext.SetState(*iReleasing);
	
	//stop trigger since call becomes not active
	aContext.ActivateTrigger( EFalse );
	
	//notify UI about hangup, hangup rejects HO but it is not failure
	//no need to show the "ho failed note"
	TRAP_IGNORE( aContext.Notifier().NotifySubscriberL(EVccCsToPsHoSuccessful, KErrNone) );
	
	return err;
	
	}
	
// -----------------------------------------------------------------------------
// CallStateChanged
// -----------------------------------------------------------------------------
//	
void TVccStateCalling::CallStateChanged( CVccPerformer& aContext, 
                                const MCCPCallObserver::TCCPCallState aState,
                                MCCPCall* aCall )
	{
	RUBY_DEBUG_BLOCK( "TVccStateCalling::CallStateChanged" );
	
	__ASSERT_DEBUG( aContext.SecondaryCall() != NULL, User::Invariant());
    __ASSERT_DEBUG( aContext.PrimaryCall() != NULL, User::Invariant() );
    
	if ( aState == MCCPCallObserver::ECCPStateConnected && 
         aContext.SecondaryCall() == aCall)
		{
        RUBY_DEBUG0( "TVccStateCalling::CallStateChanged - swap the calls" );
        
		aContext.SetCall(aContext.SecondaryCall());
		TRAP_IGNORE( aContext.Notifier().NotifySubscriberL( EVccCsToPsHoInprogress, 
		                                       KErrNone ) );
		
		//-> Set Next State - if the call is idle just destroy call object and 
		//go to init-state
		__ASSERT_DEBUG( aContext.SecondaryCall() != NULL, 
		                User::Invariant());
		
		if ( aContext.SecondaryCall()->State() == 
		     MCCPCallObserver::ECCPStateIdle)
			{
            RUBY_DEBUG0( "TVccStateCalling::CallStateChanged -\
                         release the old leg" );
            
			//delete the actuall call object and move to Init-state
            ReleaseCall( aContext, *aContext.SecondaryCall(), *iInit, 
                         KVccHoOk );
			}
		else
			{
            RUBY_DEBUG0( "TVccStateCalling::CallStateChanged -\
                          set state to iReleasing" );
                    
			//must wait for old leg to be released
			aContext.SetState( *iReleasing );	
			}				
        }
    else 
        {
        InspectChangedStates( aContext, 
                              aState, 
                              aCall, 
                              aContext.CallObserver(), 
                              &MCCPCallObserver::CallStateChanged );
        }
    }
// -----------------------------------------------------------------------------
// CallStateChangedWithInband
// -----------------------------------------------------------------------------
//  
void TVccStateCalling::CallStateChangedWithInband( CVccPerformer& aContext, 
                                const MCCPCallObserver::TCCPCallState aState,
                                MCCPCall* aCall )
    {
    RUBY_DEBUG_BLOCK( "TVccStateCalling::CallStateChangedWithInband" );
    __ASSERT_DEBUG( aContext.SecondaryCall() != NULL, User::Invariant() );
    __ASSERT_DEBUG( aContext.PrimaryCall() != NULL, User::Invariant() );
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
void TVccStateCalling::ErrorOccurred( CVccPerformer& aContext, 
                                const TCCPError aError,
                                MCCPCall* aCall )
    {
    RUBY_DEBUG_BLOCK( "TVccStateCalling::ErrorOccurred" );
    RUBY_DEBUG1( "TVccStateCalling::ErrorOccurred - error: %d", aError );
    
    __ASSERT_DEBUG( aContext.CallObserver()!=NULL, User::Invariant() );
    __ASSERT_DEBUG(aContext.PrimaryCall()!=NULL, User::Invariant());
    __ASSERT_DEBUG(aCall != NULL, User::Invariant());
    
    if ( aCall == aContext.SecondaryCall() &&
         aContext.PrimaryCall()->State() == 
         MCCPCallObserver::ECCPStateConnected )
    	{
    	RUBY_DEBUG0( "TVccStateCalling::ErrorOccurred - set state to iFailing" );
    	        
    	aContext.SetState( *iFailing );
    	}
    else if( aCall == aContext.SecondaryCall() &&
            aContext.PrimaryCall()->State() != 
            MCCPCallObserver::ECCPStateConnected )
    	{
    //	RUBY_DEBUG0( "TVccStateCalling::ErrorOccurred - forward the error to CCE" );
    	//aContext.CallObserver()->ErrorOccurred( aError, 
    	  //                                      aContext.PrimaryCall() );
    	}
  		
    //Update PS key to notify handover failure
    TRAP_IGNORE( aContext.Notifier().NotifySubscriberL( EVccCsToPsHoFailure, 
                                                        aError ) );
    
    }

// -----------------------------------------------------------------------------
// From MCCPCallObserver
// -----------------------------------------------------------------------------
//
void TVccStateCalling::CallEventOccurred(CVccPerformer& aContext, 
                                const MCCPCallObserver::TCCPCallEvent aEvent,
                                MCCPCall* aCall )
    {
    RUBY_DEBUG_BLOCK( "TVccStateCalling::CallEventOccurred" ); 
    
	__ASSERT_DEBUG(aContext.CallObserver()!=NULL, User::Invariant());
	__ASSERT_DEBUG(aCall != NULL, User::Invariant());
	
	//in successful HO case ECCPRemoteTerminated is also received when server
	//disconnects the original leg
	if ( aEvent == MCCPCallObserver::ECCPRemoteTerminated &&
	     aCall == aContext.SecondaryCall()  &&
	     aContext.PrimaryCall()->State() != MCCPCallObserver::ECCPStateConnected )
		{
		RUBY_DEBUG1( "TVccStateCalling::CallEventOccurred -\
		             forward the call event to CCE: %d", aEvent );
		        
	 	aContext.CallObserver()->CallEventOccurred( aEvent, 
	                                                aContext.PrimaryCall());
	 	    
		}
	else if( aEvent == MCCPCallObserver::ECCPRemoteTerminated &&
         aCall == aContext.SecondaryCall()  &&
         aContext.PrimaryCall()->State() == MCCPCallObserver::ECCPStateConnected )
	    {
	    RUBY_DEBUG0( "TVccStateCalling::CallEventOccurred - set state to iFailing" );
	            
	    aContext.SetState( *iFailing );
	    }
	//if remote end holds the call HO is not allowed at this end either
	else if( aEvent == MCCPCallObserver::ECCPRemoteHold )
	    {
	    //Trigger is informed that automatic HO is not allowed. 
	    aContext.ActivateTrigger( EFalse );
	    //for manual HO PS key needs to be updated
	    aContext.AllowHo( EFalse );
	    aContext.CallObserver()->CallEventOccurred( aEvent, 
	                                                aContext.PrimaryCall());    
	    }
	else if( aEvent == MCCPCallObserver::ECCPRemoteResume )
	    {
	    //HO is allowed again
	    aContext.ActivateTrigger( ETrue );
	    //for manual HO PS key needs to be updated
	    aContext.AllowHo( ETrue );
	    aContext.CallObserver()->CallEventOccurred( aEvent, 
	                                                aContext.PrimaryCall());                    
	    }
	}


// -----------------------------------------------------------------------------
// InspectChangedStates
// -----------------------------------------------------------------------------
//
void TVccStateCalling::InspectChangedStates (CVccPerformer& aContext, 
                                const MCCPCallObserver::TCCPCallState aState,
                                MCCPCall* aCall,
                                MCCPCallObserver* aCallee,
                                void (MCCPCallObserver::*aCallback)
                                (const MCCPCallObserver::TCCPCallState, MCCPCall*))

                                
    {
    //Remote party has disconnected the call - the events might arrive
    //to the new leg first. The event must be issued on the old leg as
    //all previous events are suppressed.
    //
    if( (aState == MCCPCallObserver::ECCPStateDisconnecting ||
            aState == MCCPCallObserver::ECCPStateIdle) &&
            aCall == aContext.SecondaryCall() &&
            aContext.PrimaryCall()->State() != MCCPCallObserver::ECCPStateIdle &&
            aContext.PrimaryCall()->State() != MCCPCallObserver::ECCPStateConnected )
        {
        RUBY_DEBUG1( "TVccStateCalling::InspectChangedStates -\
                forward the events on the old leg, suppress from new one: %d", aState );
        //remove progress bar from UI
        TRAP_IGNORE( aContext.Notifier().NotifySubscriberL( EVccCsToPsHoFailure, KErrGeneral ) );
        (aCallee->*aCallback)( aState, aContext.PrimaryCall());
        }
    //old leg is released in remote disconnect or other abnormal case, this should never
    //happen in the successfull HO case.
    
    else if( aState == MCCPCallObserver::ECCPStateIdle &&
                    aCall == aContext.PrimaryCall() )
        {       
        RUBY_DEBUG1( "TVccStateCalling::InspectChangedStates - shouldnt happen in normal HO case,\
                      MT disconnected the call, or some other abnormal case: %d", aState );
                
        //remove progress bar from UI
        TRAP_IGNORE( aContext.Notifier().NotifySubscriberL( EVccCsToPsHoFailure, KErrGeneral ) );
        (aCallee->*aCallback)( aState, aContext.PrimaryCall() );
        aContext.SetState( *iInit );
        }
    }
