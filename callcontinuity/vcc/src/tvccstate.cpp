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
* Description:   Default implementations for the VCC state machine functions
*
*/

#include "tvccstate.h"
#include "cvccperformer.h"
#include "rubydebug.h"


// -----------------------------------------------------------------------------
// c'tor
// -----------------------------------------------------------------------------
//

TVccState::TVccState()
	{
	RUBY_DEBUG_BLOCK( "TVccState::TVccState" );	
	}


// -----------------------------------------------------------------------------
// User has answered the incoming call
// -----------------------------------------------------------------------------
//
TInt TVccState::Answer( MCCPCall& aCall )
    {
    RUBY_DEBUG_BLOCK( "TVccState::Answer" );
 	return aCall.Answer();
    }
    
// -----------------------------------------------------------------------------
// User has rejected the incoming call
// -----------------------------------------------------------------------------
//
TInt TVccState::Reject( MCCPCall& aCall )
    {
    RUBY_DEBUG_BLOCK( "TVccState::Reject" );
  	return aCall.Reject();
    }
    
// -----------------------------------------------------------------------------
// Queue incoming call
// -----------------------------------------------------------------------------
//
TInt TVccState::Queue( MCCPCall& aCall )
    {
    RUBY_DEBUG_BLOCK( "TVccState::Queue" );
 	return aCall.Queue();
    }
    
// -----------------------------------------------------------------------------
// Dial a new call
// -----------------------------------------------------------------------------
//
TInt TVccState::Dial( MCCPCall& aCall )
    {
    RUBY_DEBUG_BLOCK( "TVccState::Dial" );
 	return aCall.Dial();
    }

// -----------------------------------------------------------------------------
// MT call user ringing now
// -----------------------------------------------------------------------------
//
TInt TVccState::Ringing( MCCPCall& aCall )
    {
    RUBY_DEBUG_BLOCK( "TVccState::Ringing" );
	return aCall.Ringing();
    }

// -----------------------------------------------------------------------------
// End current call
// -----------------------------------------------------------------------------
//
TInt TVccState::HangUp( CVccPerformer& /*aContext*/, MCCPCall& aCall )
    {
    RUBY_DEBUG_BLOCK( "TVccState::HangUp" );
    return aCall.HangUp();
 
    }

// -----------------------------------------------------------------------------
// Cancel ongoing request
// -----------------------------------------------------------------------------
//
TInt TVccState::Cancel( MCCPCall& aCall )
    {
    RUBY_DEBUG_BLOCK( "TVccState::Cancel" );
  	return aCall.Cancel();
    }


// -----------------------------------------------------------------------------
// Hold call
// -----------------------------------------------------------------------------
//
TInt TVccState::Hold( MCCPCall& aCall )
    {
    RUBY_DEBUG_BLOCK( "TVccState::Hold" );
    return aCall.Hold();
 
    }


// -----------------------------------------------------------------------------
// Resume held call
// -----------------------------------------------------------------------------
//
TInt TVccState::Resume( MCCPCall& aCall )
    {
    RUBY_DEBUG_BLOCK( "TVccState::Resume" );
 	return aCall.Resume();
    }


// -----------------------------------------------------------------------------
// Swap hold/resume states
// -----------------------------------------------------------------------------
//
TInt TVccState::Swap( MCCPCall& /*aCall*/ )
    {
    RUBY_DEBUG_BLOCK( "TVccState::Swap" );
    //return aCall.Swap();
    return KErrArgument; //not allowed

    }


// -----------------------------------------------------------------------------
// Get call recipient
// -----------------------------------------------------------------------------
//
const TDesC& TVccState::RemoteParty( const MCCPCall& aCall ) const
    {
    RUBY_DEBUG_BLOCK( "TVccState::RemoteParty" );
	return aCall.RemoteParty();
    }


// -----------------------------------------------------------------------------
// Get call recipient
// -----------------------------------------------------------------------------
//
const TDesC& TVccState::DialledParty( const MCCPCall& aCall ) const
    {
    RUBY_DEBUG_BLOCK( "TVccState::DialledParty" );
    return aCall.DialledParty();
 
    }



// -----------------------------------------------------------------------------
// Is call forwarded or not
// -----------------------------------------------------------------------------
//
TBool TVccState::IsCallForwarded( const MCCPCall& aCall ) const
    {
    RUBY_DEBUG_BLOCK( "TVccState::IsCallForwarded" );
 	return aCall.IsCallForwarded();
    }


// -----------------------------------------------------------------------------
// Is call mobile originated or not
// -----------------------------------------------------------------------------
//
TBool TVccState::IsMobileOriginated( const MCCPCall& aCall ) const
    {
    RUBY_DEBUG_BLOCK( "TVccState::IsMobileOriginated" );
 	return aCall.IsMobileOriginated();
    }


// -----------------------------------------------------------------------------
// Get current call state
// -----------------------------------------------------------------------------
//
MCCPCallObserver::TCCPCallState TVccState::State( const MCCPCall& aCall ) const
    {
    RUBY_DEBUG_BLOCK( "TVccState::State" );
 	return aCall.State();
    }


// -----------------------------------------------------------------------------
// get call capabilities
// -----------------------------------------------------------------------------
//
MCCPCallObserver::TCCPCallControlCaps TVccState::Caps( const MCCPCall& aCall ) const
    {
    RUBY_DEBUG_BLOCK( "TVccState::Caps" );
 	return aCall.Caps();
    }

// -----------------------------------------------------------------------------
// SecureSpecified
// -----------------------------------------------------------------------------
//
TBool TVccState::SecureSpecified( const MCCPCall& aCall ) const
    {
    return aCall.SecureSpecified();
    }

// -----------------------------------------------------------------------------
// Is call secured
// -----------------------------------------------------------------------------
//
TBool TVccState::IsSecured( const MCCPCall& aCall ) const
    {
    RUBY_DEBUG_BLOCK( "TVccState::IsSecured" );
    return aCall.IsSecured();
    }


// -----------------------------------------------------------------------------
// Set paramaters
// -----------------------------------------------------------------------------
//
void TVccState::SetParameters( const CCCPCallParameters& /*aNewParams*/ )
    {
    RUBY_DEBUG_BLOCK( "TVccState::SetParameters" );
    //not implemented
    }

// -----------------------------------------------------------------------------
// Get paramaters
// -----------------------------------------------------------------------------
//
const CCCPCallParameters& TVccState::Parameters( const MCCPCall& aCall ) const
    {
    RUBY_DEBUG_BLOCK( "TVccState::Parameters" );
    return aCall.Parameters();
    
    }

// -----------------------------------------------------------------------------
// Return used tone.
// -----------------------------------------------------------------------------
//
TCCPTone TVccState::Tone( const MCCPCall& aCall ) const
	{
	RUBY_DEBUG_BLOCK( "TVccState::Tone" );
	return aCall.Tone();
	}
    

// -----------------------------------------------------------------------------
// Add observer.
// -----------------------------------------------------------------------------
//
void TVccState::AddObserverL( const MCCPCallObserver& aObserver, 
                              MCCPCall& aCall )
    {
    RUBY_DEBUG_BLOCKL( "TVccState::AddObserverL" );
 	aCall.AddObserverL(aObserver);
    }

// -----------------------------------------------------------------------------
// Add observer.
// -----------------------------------------------------------------------------
//
TInt TVccState::RemoveObserver( const MCCPCallObserver& aObserver, 
                                MCCPCall& aCall )
    {
    RUBY_DEBUG_BLOCK( "TVccState::RemoveObserver" );
	return aCall.RemoveObserver(aObserver);
    }


// -----------------------------------------------------------------------------
// Dial from MCCPCSCall
// -----------------------------------------------------------------------------
//
TInt TVccState::Dial( const TDesC8& aCallParams, MCCPCSCall& aCall  )
	{
	RUBY_DEBUG_BLOCK( "TVccState::Dial" );
	return aCall.Dial(aCallParams);
	//!!!!!!!!!!!!!!-> check if "if" is really as this cannot be really invoked
	// for non-cs call
	}

// -----------------------------------------------------------------------------
// NoFDNCheck from MCCPCSCall
// -----------------------------------------------------------------------------
//
void TVccState::NoFDNCheck( MCCPCSCall& aCall )
	{
	RUBY_DEBUG_BLOCK( "TVccState::NoFDNCheck" );
	aCall.NoFDNCheck();
	}

// -----------------------------------------------------------------------------
// GetMobileCallInfo from MCCPCSCall
// -----------------------------------------------------------------------------
//
TInt TVccState::GetMobileCallInfo( TDes8& aCallInfo, 
                                   const MCCPCSCall& aCall ) const
	{
	RUBY_DEBUG_BLOCK( "TVccState::GetMobileCallInfo" );
	return aCall.GetMobileCallInfo(aCallInfo);

	}
	
// -----------------------------------------------------------------------------
// SwitchAlternatingCall from MCCPCSCall
// -----------------------------------------------------------------------------
//
TInt TVccState::SwitchAlternatingCall( MCCPCSCall& aCall )
	{
	RUBY_DEBUG_BLOCK( "TVccState::SwitchAlternatingCall" );
	return aCall.SwitchAlternatingCall();
	}

// -----------------------------------------------------------------------------
// SwitchAlternatingCall from MCCPCSCall
// -----------------------------------------------------------------------------
//
TInt TVccState::GetMobileDataCallCaps( TDes8& aCaps, 
                                       const MCCPCSCall& aCall) const
	{
	RUBY_DEBUG_BLOCK( "TVccState::GetMobileDataCallCaps" );
	return aCall.GetMobileDataCallCaps(aCaps);
	}

// -----------------------------------------------------------------------------
// LogDialedNumber from MCCPCSCall
// -----------------------------------------------------------------------------
//
TBool TVccState::LogDialedNumber( const MCCPCSCall& aCall ) const
    {
    RUBY_DEBUG_BLOCK( "TVccState::GetMobileDataCallCaps" );
    return aCall.LogDialedNumber();
    }
//  Releases ongoing call
// -----------------------------------------------------------------------------
//
TInt TVccState::ReleaseCall( MCCPCall& aCall, 
                             RPointerArray<CConvergedCallProvider>& aProvs )
	{
	RUBY_DEBUG_BLOCK( "TVccState::ReleaseCall" );
	TInt err = KErrNotFound;
	for (TUint i=0; i < aProvs.Count(); i++)
		{
		 err = aProvs[i]->ReleaseCall(aCall);
		 if (err == KErrNone)
		 	{
		 	break;
		 	}
		}
	return err;
	}
	
// -----------------------------------------------------------------------------
//  Releases ongoing call
// -----------------------------------------------------------------------------
//
TInt TVccState::ReleaseCall(CVccPerformer& aContext, 
                                MCCPCall& aCall, 
                                TVccState& aState,
                                TUint aHoValue )
	{
	RUBY_DEBUG_BLOCK( "TVccState::ReleaseCall" );
	TInt err = KErrNotFound;
	err = aContext.CsProvider().ReleaseCall(aCall);
	if (err!=KErrNone)
		{
		aContext.PsProvider().ReleaseCall(aCall);
		}
		
	if( aHoValue == KVccHoNok )
	    {
	    //MT party of the call disconnected the call during HO
	    TRAP_IGNORE(aContext.Notifier().NotifySubscriberL( EVccCsToPsHoFailure, 
	            KErrGeneral ) );
	    }
	else
	    {
	    aContext.HandoverReady();
	    TRAP_IGNORE(aContext.Notifier().NotifySubscriberL(EVccCsToPsHoSuccessful,
                KErrNone) );
	    }
	//-> Set Next State
	aContext.SetState(aState);
	return err;
	}
    
// -----------------------------------------------------------------------------
// From MCCPCallObserver
// -----------------------------------------------------------------------------
//
void TVccState::ErrorOccurred(CVccPerformer& /*aContext*/, 
                            const TCCPError /*aError*/,
                            MCCPCall* /*aCall*/ )
	{
	RUBY_DEBUG_BLOCK( "TVccState::ErrorOccurred" );	
	}

// -----------------------------------------------------------------------------
// From MCCPCallObserver
// -----------------------------------------------------------------------------
//
void TVccState::CallStateChanged(CVccPerformer& /*aContext*/, 
                            const MCCPCallObserver::TCCPCallState /*aState*/,
                            MCCPCall* /*aCall*/ )
	{
	RUBY_DEBUG_BLOCK( "TVccState::CallStateChanged" );
	}

// -----------------------------------------------------------------------------
// From MCCPCallObserver
// -----------------------------------------------------------------------------
//
void TVccState::CallStateChangedWithInband(CVccPerformer& /*aContext*/, 
                            const MCCPCallObserver::TCCPCallState /*aState*/,
                            MCCPCall* /*aCall*/  )
	{
	RUBY_DEBUG_BLOCK( "TVccState::CallStateChangedWithInband" );
		
	}

// -----------------------------------------------------------------------------
// From MCCPCallObserver
// -----------------------------------------------------------------------------
//
void TVccState::CallEventOccurred(CVccPerformer& /*aContext*/, 
                            const MCCPCallObserver::TCCPCallEvent /*aEvent*/,
                            MCCPCall* /*aCall*/ )
	{
	RUBY_DEBUG_BLOCK( "TVccState::CallEventOccurred" );	
	}
// -----------------------------------------------------------------------------
// From MCCPCallObserver
// -----------------------------------------------------------------------------
//
void TVccState::CallCapsChanged(CVccPerformer& aContext, 
                            const TUint32 aCapsFlags,
                            MCCPCall* aCall )
	{
	RUBY_DEBUG_BLOCK( "TVccState::CallCapsChanged" );	
	aContext.CallObserver()->CallCapsChanged( aCapsFlags, aCall );
	}
    
// -----------------------------------------------------------------------------
// SwitchL()
// -----------------------------------------------------------------------------
//
void TVccState::SwitchL(CVccPerformer& /*aContext*/)
	{
	RUBY_DEBUG_BLOCK( "TVccState::SwitchL" );
	User::Leave(KErrCompletion);
	}
