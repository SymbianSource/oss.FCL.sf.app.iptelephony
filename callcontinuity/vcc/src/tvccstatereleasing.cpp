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
* Description:   Implementation for the VCC Releasing state
*
*/



#include "tvccstatereleasing.h"
#include "cvccperformer.h"
#include "rubydebug.h"


// -----------------------------------------------------------------------------
// c'tor
// -----------------------------------------------------------------------------
//

TVccStateReleasing::TVccStateReleasing()
	{
	RUBY_DEBUG_BLOCK( "TVccStateReleasing::TVccStateReleasing" );	
	} 

// -----------------------------------------------------------------------------
// Name
// -----------------------------------------------------------------------------
//	
TUint TVccStateReleasing::Name() const
	{
	RUBY_DEBUG_BLOCK( "TVccStateReleasing::Name" );
	return KVccStateReleasing;
	}

// -----------------------------------------------------------------------------
// LinkState
// -----------------------------------------------------------------------------
//	
void TVccStateReleasing::LinkState(TVccState& aInit)
	{
	RUBY_DEBUG_BLOCK( "TVccStateReleasing::LinkState" );
	iInit = &aInit;
	}
	
// -----------------------------------------------------------------------------
// CallStateChanged
// -----------------------------------------------------------------------------
//	
void TVccStateReleasing::CallStateChanged( CVccPerformer& aContext, 
                                const MCCPCallObserver::TCCPCallState aState,
                                MCCPCall* aCall )
	{
	RUBY_DEBUG_BLOCK( "TVccStateReleasing::CallStateChanged" );
	__ASSERT_DEBUG( aContext.CallObserver()!=NULL, User::Invariant() );
	__ASSERT_DEBUG(aContext.PrimaryCall()!=NULL, User::Invariant());
	__ASSERT_DEBUG(aCall != NULL, User::Invariant());
	if (aState == MCCPCallObserver::ECCPStateIdle )
		{
		//delete the actuall call object and move to the Init state
		if( aCall == aContext.PrimaryCall() )
		    {
		    ReleaseCall( aContext, *aContext.SecondaryCall(), *iInit, KVccHoNok );
		    aContext.CallObserver()->CallStateChanged( aState, aCall );
		    }
		else
		    {
		    ReleaseCall( aContext, *aContext.SecondaryCall(), *iInit, KVccHoOk );
		                
		    }
		    
		}
	//for some reason the old leg hasnt been released, so new leg's events must
	//be passed forward. 
	else if( aCall == aContext.PrimaryCall() ) 
	    {
	    aContext.CallObserver()->CallStateChanged( aState, aCall );
	    }
		
	}

// -----------------------------------------------------------------------------
// From MCCPCallObserver, new leg events need to go through even when 
// old leg has not yet been disconnected
// -----------------------------------------------------------------------------
//
void TVccStateReleasing::CallEventOccurred( CVccPerformer& aContext, 
                                  const MCCPCallObserver::TCCPCallEvent aEvent,
                                  MCCPCall* aCall)
    {
    if( aCall == aContext.PrimaryCall() ) 
        {
        aContext.CallObserver()->CallEventOccurred( aEvent, 
                                                    aContext.PrimaryCall());
        }
    }
