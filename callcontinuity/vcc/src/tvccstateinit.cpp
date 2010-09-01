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
* Description:   Implementation for the VCC Init state
*
*/

#include "tvccstateinit.h"
#include "cvccperformer.h"
#include "vccsettingsreader.h"
#include "rubydebug.h"

// -----------------------------------------------------------------------------
// c'tor
// -----------------------------------------------------------------------------
//

TVccStateInit::TVccStateInit()
	{
	RUBY_DEBUG_BLOCK( "TVccStateInit::TVccStateInit" );	
	} 

// -----------------------------------------------------------------------------
// Name
// -----------------------------------------------------------------------------
//	
TUint TVccStateInit::Name() const
	{
	RUBY_DEBUG_BLOCK( "TVccStateInit::Name" );
	return KVccStateInit;
	}

// -----------------------------------------------------------------------------
// LinkState
// -----------------------------------------------------------------------------
//
void TVccStateInit::LinkState(TVccState& aCalling)
	{
	RUBY_DEBUG_BLOCK( "TVccStateInit::LinkState" );
	iCalling = &aCalling;
	}
	
// -----------------------------------------------------------------------------
// Swap()
// -----------------------------------------------------------------------------
//
TInt TVccStateInit::Swap(MCCPCall& aCall)
	{
	RUBY_DEBUG_BLOCK( "TVccStateInit::Swap()" );
	return aCall.Swap();		
	}

// -----------------------------------------------------------------------------
// From MCCPCallObserver
// -----------------------------------------------------------------------------
//
void TVccStateInit::ErrorOccurred( CVccPerformer& aContext, 
                                   const TCCPError aError,
                                   MCCPCall* /*aCall*/ )
	{
	RUBY_DEBUG_BLOCK( "TVccStateInit::ErrorOccurred" );
	__ASSERT_DEBUG(aContext.CallObserver()!=NULL, User::Invariant());
	aContext.CallObserver()->ErrorOccurred( aError, 
	                                        aContext.PrimaryCall() );
	}

// -----------------------------------------------------------------------------
// From MCCPCallObserver 
// -----------------------------------------------------------------------------
//
void TVccStateInit::CallStateChanged( CVccPerformer& aContext, 
                                  const MCCPCallObserver::TCCPCallState aState,
                                  MCCPCall* /*aCall*/ )
	{
	RUBY_DEBUG_BLOCK( "TVccStateInit::CallStateChanged" );
	__ASSERT_DEBUG(aContext.CallObserver()!=NULL, User::Invariant());
	//start trigger if call is connected, stop if not.
	if( aState == MCCPCallObserver::ECCPStateConnected )
		{
		aContext.SetRemoteParties();
		aContext.ActivateTrigger( ETrue );
		
		}
	else
		{
		aContext.ActivateTrigger( EFalse );
		}
	aContext.CallObserver()->CallStateChanged( aState, 
	                                           aContext.PrimaryCall());
	}

// -----------------------------------------------------------------------------
// From MCCPCallObserver
// -----------------------------------------------------------------------------
//
void TVccStateInit::CallStateChangedWithInband( CVccPerformer& aContext, 
                                  const MCCPCallObserver::TCCPCallState aState,
                                  MCCPCall* /*aCall*/ )
	{
	RUBY_DEBUG_BLOCK( "TVccStateInit::CallStateChangedWithInband" );
	__ASSERT_DEBUG(aContext.CallObserver()!=NULL, User::Invariant());
	aContext.CallObserver()->CallStateChangedWithInband( aState, 
	                                                aContext.PrimaryCall());
		
	}

// -----------------------------------------------------------------------------
// From MCCPCallObserver
// -----------------------------------------------------------------------------
//
void TVccStateInit::CallEventOccurred( CVccPerformer& aContext, 
                                  const MCCPCallObserver::TCCPCallEvent aEvent,
                                  MCCPCall* aCall )
	{
	RUBY_DEBUG_BLOCK( "TVccStateInit::CallEventOccurred" );
	RUBY_DEBUG1( "TVccStateInit::CallEventOccurred, event: %d", aEvent );
	__ASSERT_DEBUG(aContext.CallObserver()!=NULL, User::Invariant());
	
	   if(  aContext.PrimaryCall() == aCall )
	        {
             //if remote end holds the call HO is not allowed at this end either
	        if( aEvent == MCCPCallObserver::ECCPRemoteHold )
	            {
	            //Trigger is informed that automatic HO is not allowed. 
	            aContext.ActivateTrigger( EFalse );
	            //for manual HO PS key needs to be updated
	            aContext.AllowHo( EFalse ); 
	            }
	        
	        else if( aEvent == MCCPCallObserver::ECCPRemoteResume )
	            {
	            //HO is allowed again
	            aContext.ActivateTrigger( ETrue );
	            //for manual HO PS key needs to be updated
	            aContext.AllowHo( ETrue );
	            }
	        else if( aEvent == MCCPCallObserver::ECCPNotifyRemotePartyInfoChange )
	            {
	            aContext.SetRemoteParties();
	            }
	
	    RUBY_DEBUG0("primary call event, forward it");
	    aContext.CallObserver()->CallEventOccurred( aEvent, 
	                                                 aContext.PrimaryCall());
	    }

	}
// -----------------------------------------------------------------------------
// From MCCPCallObserver
// -----------------------------------------------------------------------------
//
void TVccStateInit::CallCapsChanged( CVccPerformer& aContext, 
                                     const TUint32 aCapsFlags )
	{
	RUBY_DEBUG_BLOCK( "TVccStateInit::CallCapsChanged" );
	__ASSERT_DEBUG(aContext.CallObserver()!=NULL, User::Invariant());
	aContext.CallObserver()->CallCapsChanged( aCapsFlags,
	                                          aContext.PrimaryCall() );	
	}
	 
// -----------------------------------------------------------------------------
// 	SwitchL()
// -----------------------------------------------------------------------------
//
void TVccStateInit::SwitchL(CVccPerformer& aContext)
	{
	RUBY_DEBUG_BLOCKL( "TVccStateInit::SwitchL" );
	
	__ASSERT_DEBUG(aContext.PrimaryCall()!=NULL, User::Invariant());
	__ASSERT_ALWAYS(aContext.PrimaryCall()->State() == 
	          MCCPCallObserver::ECCPStateConnected,User::Leave(KErrAccessDenied));
	
	if ( aContext.PrimaryCall()->Parameters().CallType() == 
	    CCPCall::ECallTypeCSVoice )
		{
		RUBY_DEBUG0( "- make VoIP call" );
		aContext.Notifier().NotifySubscriberL(EVccCsToPsHoStarted, KErrNone);
		CCCECallParameters* params = CCCECallParameters::NewL();
		params->SetCallType( CCPCall::ECallTypePS );
		params->SetServiceId( KErrNone );
		TInt id = KErrNotFound;
		id =  VccSettingsReader::VoIPServiceIdL();
		__ASSERT_ALWAYS(id >= 0, User::Leave(KErrArgument));		
		params->SetServiceId( id );
		HBufC* uri = NULL;
		uri = VccSettingsReader::DomainTransferUriL();
		CleanupStack::PushL(uri);
		__ASSERT_ALWAYS (uri->Length() > 0, User::Leave(KErrArgument));				     
		aContext.CreateSecondaryCallL(*params, *uri, aContext);	
		CleanupStack::PopAndDestroy( uri ); //uri		
		__ASSERT_DEBUG(aContext.SecondaryCall()!=NULL, User::Invariant());
		//-> Set Next State (work around, VoIP should call CallStateChanged 
        //                    only after Dial has returned)
        aContext.SetState(*iCalling);
        User::LeaveIfError(aContext.SecondaryCall()->Dial());
        delete params;
		}
	else
		{
		RUBY_DEBUG0( "- make CS call" );
		aContext.Notifier().NotifySubscriberL(EVccPsToCsHoStarted, KErrNone);
		       
		CCCECallParameters* params = CCCECallParameters::NewL();
		params->SetCallType( CCPCall::ECallTypeCSVoice );
		params->SetServiceId( KErrNone );
		
		TInt id = KErrNotFound;
		id = VccSettingsReader::CSServiceIdL();
		__ASSERT_ALWAYS(id >= 0, User::Leave(KErrArgument));
		params->SetServiceId( id );
		HBufC* number = NULL;
		number = VccSettingsReader::DomainTransferNumberL();
		CleanupStack::PushL( number );
		__ASSERT_ALWAYS (number->Length() > 0, User::Leave(KErrArgument));
		// the events will come to the context
		aContext.CreateSecondaryCallL(*params, *number, aContext); 
		CleanupStack::PopAndDestroy( number ); //number
		__ASSERT_DEBUG(aContext.SecondaryCall()!=NULL, User::Invariant());
        //-> Set Next State (work around, VoIP should call CallStateChanged 
        //                    only after Dial has returned)
        aContext.SetState(*iCalling);
        
        //not used by cs-plugin
		User::LeaveIfError(static_cast<MCCPCSCall*>
		                        (aContext.SecondaryCall())->Dial(KNullDesC8));	
        delete params;
		}
	}
