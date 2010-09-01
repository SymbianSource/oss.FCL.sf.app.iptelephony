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
* Description:   CCP reference plug-in implementation.
*
*/

#include <mccpcall.h>
#include <mccpcallobserver.h>
#include "ccpdefs.h" 
#include <mccpforwardprovider.h>

#include "vccdefinitions.h"
#include "cvccperformer.h"
#include "vcchotrigger.h"
#include "vccsettingsreader.h"
#include "cvcctransferprovider.h"

//state machine
#include "tvccstateinit.h"
#include "tvccstatecalling.h"
#include "tvccstatefailing.h"
#include "tvccstatereleasing.h"
#include "rubydebug.h"

_LIT( KSVPSipPrefix, "sip:" );
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
CVccPerformer::CVccPerformer(RPointerArray<CConvergedCallProvider>& aProviders,
							 TVccState& aState, CVccHoTrigger& aVccHoTrigger,
							 TBool aCsOriginated ) :
    iProviders(aProviders),
    iCurrentState(&aState),
    iVccHoTrigger( aVccHoTrigger ),
    iCsOriginated( aCsOriginated),
    iTransferProvider( NULL ),
    iRemotePartiesSet( EFalse )
    {
    RUBY_DEBUG_BLOCK( "CVccPerformer::CVccPerformer" );
    }

// -----------------------------------------------------------------------------
// Default C++ destructor.
// -----------------------------------------------------------------------------
//
CVccPerformer::~CVccPerformer()
    {
    RUBY_DEBUG0( "CVccPerformer::~CVccPerformer() - ENTER" );
	delete iVccPsp;
	delete iTransferProvider;
    RUBY_DEBUG0( "CVccPerformer::~CVccPerformer() - EXIT" );
    }
// -----------------------------------------------------------------------------
// Call creating comes here. In MT case observer is null and CCE will call
// SetObserver later on for us
// -----------------------------------------------------------------------------
//
CVccPerformer* CVccPerformer::NewL(
						RPointerArray<CConvergedCallProvider>& aProviders, 
						TVccState& aState,
						CVccHoTrigger& aVccHoTrigger,
						TBool aCsOriginated)
    {
    RUBY_DEBUG_BLOCKL( "CVccPerformer::NewL" );
    CVccPerformer* self = new( ELeave ) CVccPerformer(aProviders, 
    													aState, 
    													aVccHoTrigger,
    													aCsOriginated);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVccPerformer::ConstructL()
    {
    RUBY_DEBUG_BLOCKL( "CVccPerformer::ConstructL" );
        
    iVccPsp = CVccEngPsProperty::NewL();
    }

// -----------------------------------------------------------------------------
// User has answered the incoming call
// -----------------------------------------------------------------------------
//
TInt CVccPerformer::Answer()
    {
    RUBY_DEBUG_BLOCK( "CVccPerformer::Answer" );
 	return iCurrentState->Answer(*iPrimaryCall);
    }
    
// -----------------------------------------------------------------------------
// User has rejected the incoming call
// -----------------------------------------------------------------------------
//
TInt CVccPerformer::Reject()
    {
    RUBY_DEBUG_BLOCK( "CVccPerformer::Reject" );
  	return iCurrentState->Reject(*iPrimaryCall);
    }
    
// -----------------------------------------------------------------------------
// Queue incoming call
// -----------------------------------------------------------------------------
//
TInt CVccPerformer::Queue()
    {
    RUBY_DEBUG_BLOCK( "CVccPerformer::Queue" );
 	return iCurrentState->Queue(*iPrimaryCall);
    }
    
// -----------------------------------------------------------------------------
// Dial a new call
// -----------------------------------------------------------------------------
//
TInt CVccPerformer::Dial()
    {
    RUBY_DEBUG_BLOCK( "CVccPerformer::Dial" );
 	return iCurrentState->Dial(*iPrimaryCall);
    }

// -----------------------------------------------------------------------------
// MT call user ringing now
// -----------------------------------------------------------------------------
//
TInt CVccPerformer::Ringing()
    {
    RUBY_DEBUG_BLOCK( "CVccPerformer::Ringing" );
	return iCurrentState->Ringing(*iPrimaryCall);
    }

// -----------------------------------------------------------------------------
// End current call
// -----------------------------------------------------------------------------
//
TInt CVccPerformer::HangUp()
    {
    RUBY_DEBUG_BLOCK( "CVccPerformer::HangUp" );
    return iCurrentState->HangUp(*this, *iPrimaryCall);
    }

// -----------------------------------------------------------------------------
// Cancel ongoing request
// -----------------------------------------------------------------------------
//
TInt CVccPerformer::Cancel()
    {
    RUBY_DEBUG_BLOCK( "CVccPerformer::Cancel" );
  	return iCurrentState->Cancel(*iPrimaryCall);
    }

// -----------------------------------------------------------------------------
// Hold call
// -----------------------------------------------------------------------------
//
TInt CVccPerformer::Hold()
    {
    RUBY_DEBUG_BLOCK( "CVccPerformer::Hold" );
    return iCurrentState->Hold(*iPrimaryCall); 
    }

// -----------------------------------------------------------------------------
// Resume held call
// -----------------------------------------------------------------------------
//
TInt CVccPerformer::Resume()
    {
    RUBY_DEBUG_BLOCK( "CVccPerformer::Resume" );
 	return iCurrentState->Resume(*iPrimaryCall);
    }

// -----------------------------------------------------------------------------
// Swap hold/resume states
// -----------------------------------------------------------------------------
//
TInt CVccPerformer::Swap()
    {
    RUBY_DEBUG_BLOCK( "CVccPerformer::Swap" );
    return iCurrentState->Swap(*iPrimaryCall);
    }

// -----------------------------------------------------------------------------
// Get call recipient
// -----------------------------------------------------------------------------
//
const TDesC& CVccPerformer::RemoteParty() const
    {
    RUBY_DEBUG_BLOCK( "CVccPerformer::RemoteParty" );
    
    return iOrigRemoteParty;
    }

// -----------------------------------------------------------------------------
// RemotePartyName
// -----------------------------------------------------------------------------
//
const TDesC& CVccPerformer::RemotePartyName()
    {
    RUBY_DEBUG_BLOCK( "CVccPerformer::RemotePartyName" );
    return iOrigRemotePartyName;
    //return iCurrentState->RemoteParty(*iPrimaryCall);
    }

// -----------------------------------------------------------------------------
// Get call recipient
// -----------------------------------------------------------------------------
//
const TDesC& CVccPerformer::DialledParty() const
    {
    RUBY_DEBUG_BLOCK( "CVccPerformer::DialledParty" );
    return iCurrentState->DialledParty(*iPrimaryCall);
    }

// -----------------------------------------------------------------------------
// Is call forwarded or not
// -----------------------------------------------------------------------------
//
TBool CVccPerformer::IsCallForwarded() const
    {
    RUBY_DEBUG_BLOCK( "CVccPerformer::IsCallForwarded " );
 	return iCurrentState->IsCallForwarded(*iPrimaryCall);
    }

// -----------------------------------------------------------------------------
// Is call mobile originated or not
// -----------------------------------------------------------------------------
//
TBool CVccPerformer::IsMobileOriginated() const
    {
    RUBY_DEBUG_BLOCK( "CVccPerformer::IsMobileOriginated" );
 	return iCurrentState->IsMobileOriginated(*iPrimaryCall);
    }

// -----------------------------------------------------------------------------
// Get current call state
// -----------------------------------------------------------------------------
//
MCCPCallObserver::TCCPCallState CVccPerformer::State() const
    {
    RUBY_DEBUG_BLOCK( "CVccPerformer::State " );
 	return iCurrentState->State(*iPrimaryCall);
    }

// -----------------------------------------------------------------------------
// Get plug-in UID
// -----------------------------------------------------------------------------
//
TUid CVccPerformer::Uid() const
    {
    RUBY_DEBUG_BLOCK( "CVccPerformer::Uid " );
  
    return TUid::Uid( KVccCallProviderPlugId );
    }

// -----------------------------------------------------------------------------
// get call capabilities
// -----------------------------------------------------------------------------
//
MCCPCallObserver::TCCPCallControlCaps CVccPerformer::Caps() const
    {
    RUBY_DEBUG_BLOCK( "CVccPerformer::CVccPerformer" );
 	return iCurrentState->Caps(*iPrimaryCall);
    }


// -----------------------------------------------------------------------------
// SecureSpecified()
// -----------------------------------------------------------------------------
//
TBool CVccPerformer::SecureSpecified() const
    {
    return iCurrentState->SecureSpecified( *iPrimaryCall );
    }
// -----------------------------------------------------------------------------
// Is call secured
// -----------------------------------------------------------------------------
//
TBool CVccPerformer::IsSecured() const
    {
    RUBY_DEBUG_BLOCK( "CVccPerformer::IsSecured" );
    return iCurrentState->IsSecured(*iPrimaryCall);
    }

// -----------------------------------------------------------------------------
// Set parameters
// -----------------------------------------------------------------------------
//
void CVccPerformer::SetParameters( const CCCPCallParameters& aNewParams )
    {
    RUBY_DEBUG_BLOCK( "CVccPerformer::SetParameters " );
    iCurrentState->SetParameters(aNewParams);
    }

// -----------------------------------------------------------------------------
// Get parameters
// -----------------------------------------------------------------------------
//
const CCCPCallParameters& CVccPerformer::Parameters() const
    {
    RUBY_DEBUG_BLOCK( "CVccPerformer::Parameters" );
    return iCurrentState->Parameters(*iPrimaryCall);
    }

// -----------------------------------------------------------------------------
// Return used tone.
// -----------------------------------------------------------------------------
//
TCCPTone CVccPerformer::Tone() const
	{
	RUBY_DEBUG_BLOCK( "CVccPerformer::Tone " );
	return iCurrentState->Tone(*iPrimaryCall);
	}
  
// -----------------------------------------------------------------------------
// Get forward call enabling methods for the call
// -----------------------------------------------------------------------------
//
MCCPForwardProvider* CVccPerformer::ForwardProviderL
                                        ( const MCCPForwardObserver& aObserver )
    {
    RUBY_DEBUG_BLOCKL( "CVccPerformer::ForwardProviderL " );
 	return iPrimaryCall->ForwardProviderL(aObserver);
    }

// -----------------------------------------------------------------------------
// Get transfer call enabling methods for the call
// -----------------------------------------------------------------------------
//
MCCPTransferProvider* CVccPerformer::TransferProviderL
                                        ( const MCCPTransferObserver& aObserver )
    {
    RUBY_DEBUG_BLOCKL( "CVccPerformer::TransferProviderL " );
	if ( !iTransferProvider )
          {    
          iTransferProvider = CVccTransferProvider::NewL(this);
          iTransferProvider->AddObserverL( aObserver );
          }
      
    return iTransferProvider;
    }

// -----------------------------------------------------------------------------
// Add observer.
// the CCE will call that in the MT call case, to put the observer for a call in place
// -----------------------------------------------------------------------------
//
void CVccPerformer::AddObserverL( const MCCPCallObserver& aObserver )
    {
    RUBY_DEBUG_BLOCKL( "CVccPerformer::AddObserverL " );
 	iCallObs = const_cast<MCCPCallObserver*>( &aObserver );
 	iCurrentState->AddObserverL(*this, *iPrimaryCall);
    }

// -----------------------------------------------------------------------------
// remove observer.
// -----------------------------------------------------------------------------
//
TInt CVccPerformer::RemoveObserver(const MCCPCallObserver& aObserver )
    {
    RUBY_DEBUG_BLOCK( "CVccPerformer::RemoveObserver " );
	return iCurrentState->RemoveObserver(aObserver, *iPrimaryCall);
    }

// -----------------------------------------------------------------------------
// Dial from MCCPCSCall
// -----------------------------------------------------------------------------
//
TInt CVccPerformer::Dial( const TDesC8& aCallParams )
	{
	RUBY_DEBUG_BLOCK( "CVccPerformer::Dial" );
	return iCurrentState->Dial(aCallParams, 
	                        *static_cast<MCCPCSCall*>(iPrimaryCall));
	}

// -----------------------------------------------------------------------------
// NoFDNCheck from MCCPCSCall
// -----------------------------------------------------------------------------
//
void CVccPerformer::NoFDNCheck()
	{
	RUBY_DEBUG_BLOCK( "CVccPerformer::NoFDNCheck " );
	iCurrentState->NoFDNCheck(*static_cast<MCCPCSCall*>(iPrimaryCall));
	}

// -----------------------------------------------------------------------------
// GetMobileCallInfo from MCCPCSCall
// -----------------------------------------------------------------------------
//
TInt CVccPerformer::GetMobileCallInfo( TDes8& aCallInfo ) const
	{
	RUBY_DEBUG_BLOCK( "CVccPerformer::GetMobileCallInfo " );
	return iCurrentState->GetMobileCallInfo(aCallInfo,
	                          *static_cast<MCCPCSCall*>(iPrimaryCall));
	}
	
// -----------------------------------------------------------------------------
// SwitchAlternatingCall from MCCPCSCall
// -----------------------------------------------------------------------------
//
TInt CVccPerformer::SwitchAlternatingCall()
	{
	RUBY_DEBUG_BLOCK( "CVccPerformer::SwitchAlternatingCall " );
	return iCurrentState->SwitchAlternatingCall(
	                         *static_cast<MCCPCSCall*>(iPrimaryCall));
	}

// -----------------------------------------------------------------------------
// GetMobileDataCallCaps from MCCPCSCall
// -----------------------------------------------------------------------------
//
TInt CVccPerformer::GetMobileDataCallCaps( TDes8& aCaps ) const
	{
	RUBY_DEBUG_BLOCK( "CVccPerformer::GetMobileDataCallCaps " );
	return iCurrentState->GetMobileDataCallCaps(aCaps,
	                          *static_cast<MCCPCSCall*>(iPrimaryCall));
	}

// -----------------------------------------------------------------------------
// LogDialedNumber from MCCPCSCall
// -----------------------------------------------------------------------------
//
TInt CVccPerformer::LogDialedNumber() const
	{
	RUBY_DEBUG_BLOCK( "CVccPerformer::LogDialedNumber " );
	return iCurrentState->LogDialedNumber(
	                          *static_cast<MCCPCSCall*>(iPrimaryCall) );
	}
	
// -----------------------------------------------------------------------------
// MCCPCallObserver:: ErrorOccurred
// -----------------------------------------------------------------------------
//
void CVccPerformer::ErrorOccurred(const TCCPError aError, MCCPCall* aCall )
	{
	RUBY_DEBUG_BLOCK( "CVccPerformer::TCCPError ");	
	iCurrentState->ErrorOccurred(*this, aError, aCall );
	}
	
// -----------------------------------------------------------------------------
// MCCPCallObserver:: CallStateChanged
// -----------------------------------------------------------------------------
//	
void CVccPerformer::CallStateChanged( 
                                   const MCCPCallObserver::TCCPCallState aState, 
                                   MCCPCall* aCall  )
	{
	RUBY_DEBUG_BLOCK( "CVccPerformer::CallStateChanged ");
	iCurrentState->CallStateChanged( *this, aState, aCall );		
	}

// -----------------------------------------------------------------------------
// MCCPCallObserver:: CallStateChangedWithInband
// -----------------------------------------------------------------------------
//	
void CVccPerformer::CallStateChangedWithInband( 
                                   const MCCPCallObserver::TCCPCallState aState,
                                   MCCPCall* aCall )
	{
	RUBY_DEBUG_BLOCK( "CVccPerformer::CallStateChanged ");
	iCurrentState->CallStateChangedWithInband( *this, aState, aCall );
	}

// -----------------------------------------------------------------------------
// MCCPCallObserver::CallEventOccurred
// -----------------------------------------------------------------------------
//	
void CVccPerformer::CallEventOccurred( 
                                   const MCCPCallObserver::TCCPCallEvent aEvent, 
                                   MCCPCall* aCall  )
	{
	RUBY_DEBUG_BLOCK( "CVccPerformer::CallEventOccurred ");
	iCurrentState->CallEventOccurred(*this, aEvent, aCall);
	}
	
// -----------------------------------------------------------------------------
// MCCPCallObserver::CallCapsChanged
// -----------------------------------------------------------------------------
//	
void CVccPerformer::CallCapsChanged(const TUint32 aCapsFlags, MCCPCall* aCall )
	{
	RUBY_DEBUG_BLOCK( "CVccPerformer::CallCapsChanged");
	iCurrentState->CallCapsChanged(*this, aCapsFlags, aCall );
	}

// -----------------------------------------------------------------------------
//  Get CallObserver()
// -----------------------------------------------------------------------------
//
MCCPCallObserver* CVccPerformer::CallObserver()
    {
    RUBY_DEBUG_BLOCK( "CVccPerformer::CallObserver " );
  	return iCallObs;
    }

// -----------------------------------------------------------------------------
//  Creates actual implementation
// -----------------------------------------------------------------------------
//
MCCPCall* CVccPerformer::CreatePrimaryCallL( 
                                 const CCCPCallParameters& aParameters,
                                 const TDesC& aRecipient,
                                 const MCCPCallObserver& aObserver )
	{
    RUBY_DEBUG_BLOCKL( "CVccPerformer::CreatePrimaryCallL" );

    iPrimaryCall = NULL;
    TInt index = KErrNotFound;
    if (aParameters.CallType() != CCPCall::ECallTypePS )
	    {
	    index = VoipProviderIndex( EFalse );
	    }
    else
 	    {
 	    index = VoipProviderIndex( ETrue );
 	    }
    if( index != KErrNotFound )
        {
        iPrimaryCall = iProviders[ index ]->NewCallL(aParameters, aRecipient, *this);
        }
            
	if( !iPrimaryCall )
	    {
	    User::Leave( KErrNotFound );
	    }
    iCallObs = const_cast< MCCPCallObserver* >( &aObserver ); 
    
    SetRemoteParties();
    
	return this;
	}
	
// -----------------------------------------------------------------------------
//  saves incoming call and returns itself
// -----------------------------------------------------------------------------
//
MCCPCall* CVccPerformer::IncomingCall( MCCPCall* aCall )
    {
    RUBY_DEBUG_BLOCK( "CVccPerformer::IncomingCall" );
    iPrimaryCall = aCall;
    SetRemoteParties();
    
    return this;
     }

// -----------------------------------------------------------------------------
//  saves 3rd party mo call and returns itself
// -----------------------------------------------------------------------------
//
MCCPCall* CVccPerformer::MoCall( MCCPCall* aCall )
    {
    RUBY_DEBUG_BLOCK( "CVccPerformer::MoCall" );
    iPrimaryCall = aCall;
    return this;
     }

// -----------------------------------------------------------------------------
//  Returns actual call implementation
// -----------------------------------------------------------------------------
//	
MCCPCall* CVccPerformer::PrimaryCall()
	{
	RUBY_DEBUG_BLOCK( "CVccPerformer::PrimaryCall" );
	return iPrimaryCall;	
	}
	
// -----------------------------------------------------------------------------
//  Returns actual CS provider implementation
// -----------------------------------------------------------------------------
//
CConvergedCallProvider& CVccPerformer::CsProvider()
	{
	RUBY_DEBUG_BLOCK( "CVccPerformer::CsProvider" );
	return *iProviders[ VoipProviderIndex( EFalse ) ];
	            
	}
	
// -----------------------------------------------------------------------------
//  Returns actual PS provider implementation
// -----------------------------------------------------------------------------
//
CConvergedCallProvider& CVccPerformer::PsProvider()
	{
	RUBY_DEBUG_BLOCK( "CVccPerformer::PsProvider" );
	return *iProviders[ VoipProviderIndex( ETrue ) ];
	}

// -----------------------------------------------------------------------------
//  Updates the actual call
// -----------------------------------------------------------------------------
//
void CVccPerformer::SetCall(MCCPCall* aCall)
    {
    TRAP_IGNORE( SetCallL( aCall) );
    }


// -----------------------------------------------------------------------------
//  Updates the actual call
// -----------------------------------------------------------------------------
//
void CVccPerformer::SetCallL(MCCPCall* aCall)
	{
	__ASSERT_DEBUG(aCall == iSecondaryCall, User::Leave(KErrArgument));
    RUBY_DEBUG_BLOCK( "CVccPerformer::SetCall" );
    
    //inform through phone engine to phone about call bubble update needs
    MCCPCallObserver::TCCPCallEvent event = 
                              MCCPCallObserver::ECCPNotifyRemotePartyInfoChange;
    iCallObs->CallEventOccurred( event, this );
    
    //both secure call and not secure call events result in same 
    //securestatuschanged message in phone engine.
        
        
    MCCPCall* temp = iPrimaryCall;
	iPrimaryCall = aCall;
	iSecondaryCall = temp; //taking old call pointer
	if (iTransferProvider)
	    {
	    iTransferProvider->UpdateL();
	    }

	MCCPCallObserver::TCCPCallEvent event2;
	//if( iPrimaryCall->IsSecured() )
	//    {
	//    event2 = MCCPCallObserver::ECCPSecureCall;
	//    }
	//else
	//    {
	    event2 = MCCPCallObserver::ECCPNotSecureCall;
	//    }
    iCallObs->CallEventOccurred( event2, this );
        
	}
	
// -----------------------------------------------------------------------------
//  Releases ongoing call
// -----------------------------------------------------------------------------
//
TInt CVccPerformer::ReleaseCall(MCCPCall& /*aCall*/)
	{
	RUBY_DEBUG_BLOCK( "CVccPerformer::ReleaseCall" );
	TInt err = KErrNotFound;
	err = iCurrentState->ReleaseCall(*iPrimaryCall, iProviders);
	iPrimaryCall = NULL;
	return err;
	}
	
// -----------------------------------------------------------------------------
//  Suppressor calls this when HO is done
// -----------------------------------------------------------------------------
//    
void CVccPerformer::HandoverReady()
    {
    RUBY_DEBUG_BLOCK( "CVccPerformer::HandoverReady" );    
	iSecondaryCall = NULL;
    ActivateTrigger( ETrue );
    //get new service id for voip call and set it to parameters
    //this is used later for loading correct CTI plugins
    TInt VoIPServiceId(KErrNotFound);
    TRAP_IGNORE( VoIPServiceId = VccSettingsReader::VoIPServiceIdL() );
    RUBY_DEBUG1( "CVccPerformer::HandoverReady() -- new Service Id: %d", VoIPServiceId );
    CCCPCallParameters* params = NULL;
    TRAP_IGNORE( params = iPrimaryCall->Parameters().CloneL() );
    params->SetServiceId( VoIPServiceId );
    iPrimaryCall->SetParameters( *params );
    //inform through phone engine to phone about call bubble update needs
    MCCPCallObserver::TCCPCallEvent event = 
                              MCCPCallObserver::ECCPNotifyRemotePartyInfoChange;
    iCallObs->CallEventOccurred( event, this );
    
    RUBY_DEBUG0("Let HoTrigger know about changed domain");

    if( params->CallType() == CCPCall::ECallTypeCSVoice )
          {
          RUBY_DEBUG0("New domain is CS");
          iVccHoTrigger.SetCurrentDomainType( CVccHoTrigger::ECallDomainTypeCS );
          }
      else
          {
          RUBY_DEBUG0("New domain is PS");
          iVccHoTrigger.SetCurrentDomainType( CVccHoTrigger::ECallDomainTypePS );
          }
    
    delete params;
    }

// -----------------------------------------------------------------------------
//  Creates a new call to be handled
// -----------------------------------------------------------------------------
// 
void CVccPerformer::CreateSecondaryCallL(
                               const CCCPCallParameters& aParameters,
					      	   const TDesC& aRecipient,
					      	   const MCCPCallObserver& aObserver)
	{
	RUBY_DEBUG_BLOCKL( "CVccPerformer::CreateLocalSecondaryCallL" );
	iSecondaryCall = NULL;
	TInt index = KErrNotFound;
	if (aParameters.CallType() != CCPCall::ECallTypePS )
	    {
	    index = VoipProviderIndex( EFalse );
	    }
	else
	    {
	    index = VoipProviderIndex( ETrue );
	    }
	if( index != KErrNotFound )
	    {
	    iSecondaryCall = iProviders[ index ]->NewCallL(aParameters, 
	                                                   aRecipient, 
	                                                   aObserver);
	    }
	            
	if( !iSecondaryCall )
	    {
	    User::Leave( KErrNotFound );
	    }
	}

// -----------------------------------------------------------------------------
//  Creates a new call to be handled
// -----------------------------------------------------------------------------
// 
MCCPCall* CVccPerformer::SecondaryCall()
	{
	RUBY_DEBUG_BLOCK( "CVccPerformer::SecondaryCall" );
	return iSecondaryCall;
	}
	
// -----------------------------------------------------------------------------
//  Updates the state with the next one
// -----------------------------------------------------------------------------
// 
void CVccPerformer::SetState(TVccState& aState)
	{
	RUBY_DEBUG_BLOCK( "CVccPerformer::SetState" );
	iCurrentState = &aState;
	}

// -----------------------------------------------------------------------------
//  Returns the state name
// -----------------------------------------------------------------------------
// 
TUint CVccPerformer::StateName() const
	{
	RUBY_DEBUG_BLOCK( "CVccPerformer::StateName" );
	return iCurrentState->Name();
	}

// -----------------------------------------------------------------------------
//  Returns the Notifier
// -----------------------------------------------------------------------------
// 
CVccEngPsProperty& CVccPerformer::Notifier()
	{
	RUBY_DEBUG_BLOCK( "CVccPerformer::Notifier" );
	return *iVccPsp;
	}

// -----------------------------------------------------------------------------
//  Starts Handover procedure
// -----------------------------------------------------------------------------
// 
void CVccPerformer::SwitchL( const TInt aValue )
	{
	RUBY_DEBUG_BLOCK( "CVccPerformer::DoHandover" );
	//there must be an actuall call implementation created
	__ASSERT_DEBUG(iPrimaryCall != NULL, User::Leave(KErrArgument));
    
	//dont change remote party after/during handover
	iRemotePartiesSet = ETrue;
	// check which call is active and do handover for that call. other calls
	// are dropped.
	if( iPrimaryCall->State() == MCCPCallObserver::ECCPStateConnected )
		{
        switch ( iPrimaryCall->Parameters().CallType() )
            {
            case CCPCall::ECallTypeCSVoice:
                {
                // CS call active
                if ( aValue == EVccAutomaticStartCsToPsHoRequest || 
                     aValue == EVccManualStartCsToPsHoRequest ||
                     aValue == EVccAutomaticStartCsToPsHoRequestIfSingleCall )
                    {
                    // HO direction OK
                    RUBY_DEBUG0( "- CS to PS HO OK" );
            		iCurrentState->SwitchL(*this);
                    }
                break;
                }
            case CCPCall::ECallTypePS:
                {
                // PS call active
                if ( aValue == EVccAutomaticStartPsToCsHoRequest || 
                     aValue == EVccManualStartPsToCsHoRequest ||
                     aValue == EVccAutomaticStartPsToCsHoRequestIfSingleCall )
                    {
                    // HO direction OK
                    RUBY_DEBUG0( "- PS to CS HO OK" );
					iCurrentState->SwitchL(*this);
                    }
                break;
                }
            }        
		}
	}

// -----------------------------------------------------------------------------
//  Activates trigger when call becomes active
// -----------------------------------------------------------------------------
// 
void CVccPerformer::ActivateTrigger( TBool aActivation )
	{
	RUBY_DEBUG_BLOCK( "CVccPerformer::ActivateTrigger" );
	//Dont activate trigger if ho not allowed
	if( aActivation )
		{
		// Trigger is started only if call type is CS or PS
		CCPCall::TCallType type = iPrimaryCall->Parameters().CallType(); 
		if ( type == CCPCall::ECallTypeCSVoice || 
		     type == CCPCall::ECallTypePS )
		    {
		    //call became active, start trigger for automatic HOs
		    //inform trigger about the original domain so it can check
		    //possible HO restrictions
		    iVccHoTrigger.Start( *this, iCsOriginated );
		    }
		}
	else
		{
		//call is not active anymore, stop trigger
		iVccHoTrigger.Stop( *this );
		}
	}

// -----------------------------------------------------------------------------
//  Updates PS key using trigger
// -----------------------------------------------------------------------------
// 
void CVccPerformer::AllowHo( TBool aAllow )
    {
    TRAP_IGNORE( AllowHoL( aAllow) );
    }

// -----------------------------------------------------------------------------
//  Updates PS key using trigger
// -----------------------------------------------------------------------------
// 
void CVccPerformer::AllowHoL( TBool aAllow )
    {
    RUBY_DEBUG_BLOCK( "CVccPerformer::AllowHoL" );
    if( aAllow )
        {
        //HO is allowed, item can be in in call options menu
        iVccHoTrigger.HoAllowed();
        }
    else
        {
        //HO is not allowed (f.ex. remote hold) and UI item needs to be removed
        iVccHoTrigger.HoNotAllowedL();
        }
    }

// -----------------------------------------------------------------------------
//  Checks call state and starts trigger if connected
//  Conference call doesnt notify state Connected after conference ended
//  Only one call can be in Connected state
// -----------------------------------------------------------------------------
// 
void CVccPerformer::ConferenceEnded()
	{
	RUBY_DEBUG_BLOCK( "CVccPerformer::ConferenceEnded" );
	if( iPrimaryCall->State() == MCCPCallObserver::ECCPStateConnected )
		{
		ActivateTrigger( ETrue );
		}
	}

// -----------------------------------------------------------------------------
// Release secondary call leg, if needed, before calling destructor
// -----------------------------------------------------------------------------
// 
void CVccPerformer::ReleaseSecondaryCallLeg()
    {
    RUBY_DEBUG_BLOCK( "CVccPerformer::ReleaseSecondaryCallLeg" );
    if ( iSecondaryCall )
        {
        TInt err = iCurrentState->ReleaseCall(*iSecondaryCall, iProviders);
        if( err != KErrNone )
            {
            RUBY_DEBUG0("CVccPerformer::ReleaseSecondaryCallLeg:" );
            RUBY_DEBUG1("Error releasing secondary call leg: %d", err );
            }
        }
    }

// -----------------------------------------------------------------------------
// Save original RemoteParty and RemotePartyName
// -----------------------------------------------------------------------------
// 
void CVccPerformer::SetRemoteParties()
    {
    RUBY_DEBUG_BLOCK( "CVccPerformer::SetRemoteParties" );
    if( !iRemotePartiesSet )
        {
        iOrigRemoteParty = iPrimaryCall->RemoteParty();
        iOrigRemotePartyName = iPrimaryCall->RemotePartyName();
        ParseRemoteParty();
        }
    } 

// -----------------------------------------------------------------------------
// Return ps/cs provider index
// -----------------------------------------------------------------------------
// 
TInt CVccPerformer::VoipProviderIndex( TBool aVoipIndexWanted )
    {
    TInt index = KErrNotFound;
    CSInfo info;
    if( aVoipIndexWanted )
        {
        for( TInt i = 0; i < iProviders.Count(); i++ )
            {
            if( !iProviders[ i ]->GetCSInfo( info ) )
                {
                index = i;
                break;
                }
            }
        }
    else
        {
        for( TInt i = 0; i < iProviders.Count(); i++ )
            {
            if( iProviders[ i ]->GetCSInfo( info ) )
                {
                index = i;
                break;
                }
            }
        }
    return index;
    }

// -----------------------------------------------------------------------------
// Parse remoteparty number if call is VoIP call
// -----------------------------------------------------------------------------
// 
void CVccPerformer::ParseRemoteParty()
    {
    RUBY_DEBUG_BLOCK( "CVccPerformer::ParseRemoteParty" );
    TInt position = 0;
        
     // seek '@' sign (returns KErrNotFound if none)
    position = iOrigRemoteParty.Locate('@');
        
     // if found
     if(position != KErrNotFound)
           {
           TInt length = iOrigRemoteParty.Length();
            // remove the domain part from sip uri
           iOrigRemoteParty.Delete(position, length - position);
           //and delete 'sip:' prefix if found
           if ( iOrigRemoteParty.Find( KSVPSipPrefix ) != KErrNotFound )
               {
               RUBY_DEBUG0("SIP: prefix found, delete it");
               iOrigRemoteParty.Delete(0, 4);
               }
           }

    } 
