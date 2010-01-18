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
* Description:   Implementation of the handover trigger
*
*/



#include <e32base.h>

#include <vccsubscribekeys.h>
#include "vccuipsproperty.h"
#include "vcchotrigger.h"
#include "vccsignallevelhandler.h"
#include "vccwlansignallevelhandler.h"
#include "vccgsmsignallevelhandler.h"
#include "rubydebug.h"
#include "vcchopolicyreader.h"
#include "vcccchmonitor.h"
#include "vccengpsproperty.h"
#include "cvccperformer.h"

// Min. signal strength.
static const TInt32 KStrengthMin = 110;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Symbian constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CVccHoTrigger* CVccHoTrigger::NewL()
    {
    RUBY_DEBUG_BLOCKL( "CVccHoTrigger::NewL" );

    CVccHoTrigger* self = new ( ELeave ) CVccHoTrigger();

    CleanupStack::PushL( self );

    self->ConstructL();

    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CVccHoTrigger::~CVccHoTrigger()
    {
    RUBY_DEBUG0( "CVccHoTrigger::~CVccHoTrigger() - ENTER" );

    delete iProperty;
    delete iWlanSignalLevelHandler;
    delete iGsmSignalLevelHandler;
    delete iHoPolicyReader;
    delete iCchMonitor;
    delete iEngPsProperty;
    
    RUBY_DEBUG0( "CVccHoTrigger::~CVccHoTrigger() - EXIT" );
    }

// -----------------------------------------------------------------------------
// CVccHoTrigger::CVccHoTrigger()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CVccHoTrigger::CVccHoTrigger() :         //: CActive( EPriorityStandard )
    iGsmClass( ESignalClassUndefined ),
    iWlanClass( ESignalClassUndefined ),
    iManualHoDone( EFalse ),
    iHoNotAllowed( EFalse ),
    iHoAllowedIfCsOriginated( ETrue )
    {
    RUBY_DEBUG_BLOCK( "CVccHoTrigger::CVccHoTrigger" );
    }
// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CVccHoTrigger::ConstructL()
    {
    RUBY_DEBUG_BLOCKL( "CVccHoTrigger::ConstructL" );

    iProperty = CVccUiPsProperty::NewL();
    iHoPolicyReader = CVccHoPolicyReader::NewL();
    iHoPolicyReader->ReadSettingsL();
    
    iEngPsProperty = CVccEngPsProperty::NewL();
    iPolicy = iHoPolicyReader->HoPolicy();
    TSignalLevelParams gsm = iHoPolicyReader->CsSignalLevelParams();
    TSignalLevelParams wlan = iHoPolicyReader->PsSignalLevelParams();
    

    iWlanSignalLevelHandler =
        CVccWlanSignalLevelHandler::NewL( *this, wlan, *iEngPsProperty );

         
    iGsmSignalLevelHandler =
        CVccGsmSignalLevelHandler::NewL( *this, gsm );
  
    iHoAllowedIfCsOriginated = iPolicy.DtAllowedWhenCsOriginated();
    // Previous signal level class must be undefined
    iPreviousGsmClass = ESignalClassUndefined;
    iPreviousWlanClass = ESignalClassUndefined;
    // In the beginning the VoIP service is not available.
    // This will be changed when the cch monitor calls us
    // when the service is available.
    iCchServiceStatus = EServiceUnavailable;
    
    // CS network has signal level zero if the service is not
    // available.
    iCsSignalLevel = KStrengthMin;

    // We are not stared yet.
    iStarted = EFalse;
    
    // Update P&S keys to tell if we can do ho or not.    
    UpdatePsKeysL();
    // Create the CCH monitor. We are the observer to be
    // notified when the status changes.
    iCchMonitor = CVccCchMonitor::NewL( *this );
    }

// -----------------------------------------------------------------------------
// Stop monitoring
// -----------------------------------------------------------------------------
//
EXPORT_C void CVccHoTrigger::Stop()
    {
    RUBY_DEBUG_BLOCK( "CVccHoTrigger::Stop" );
    Stop( *iWhoStartedMe );
    }

// -----------------------------------------------------------------------------
// Stop monitoring
// -----------------------------------------------------------------------------
//
EXPORT_C void CVccHoTrigger::Stop( CVccPerformer& aStopper )
    {
    RUBY_DEBUG_BLOCK( "CVccHoTrigger::Stop" );
    // If someone else that the one who started us tries
    // to stop us, we do not allow it.
    // There may be several performers that are in differnet state
    // and in those state they do not need trigger and thus try
    // to stop us.
    if ( iWhoStartedMe == &aStopper )
        {
        iWhoStartedMe = NULL;
        iStarted = EFalse;
        TRAP_IGNORE( UpdatePsKeysL() );
        TRAP_IGNORE( iProperty->NotifySubscriberL( EVccNoRequestOngoing ) );
        iWlanSignalLevelHandler->Stop();
        iGsmSignalLevelHandler->Stop();
        
        iGsmClass = ESignalClassUndefined;
        iWlanClass = ESignalClassUndefined;        
        
        // Reset CS signal strength to min
        iCsSignalLevel = KStrengthMin;
        
        // Disable (do not send notifications).
        // The service is not actually disabled.
        // It will be disabled (if needed) when the monitor
        // is deleted.
        iCchMonitor->DisableService();
        RUBY_DEBUG0( " -Trigger stopped");
        }
    }

// -----------------------------------------------------------------------------
// Start monitoring
// We must not leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CVccHoTrigger::Start( CVccPerformer& aWhoStartedMe, 
                                    TBool aCsOriginated )
    {
    RUBY_DEBUG_BLOCK( "CVccHoTrigger::Start" );
    if( iHoAllowedIfCsOriginated == EFalse && aCsOriginated )
        {
        RUBY_DEBUG0( "HO not allowed for CS originated calls" );
                
        Stop();
        return;
        }
    //anyone can start, but only the last one who started can stop the trigger
    iWhoStartedMe = &aWhoStartedMe;
    if( !iStarted )
        {
        
        // Previous signal level class must be undefined
        iPreviousGsmClass = ESignalClassUndefined;
        iPreviousWlanClass = ESignalClassUndefined;
    
        // read signal high/low levels, timers etc.
        TRAPD( err, iHoPolicyReader->ReadSettingsL() );
        RUBY_DEBUG1( "reading ho settings, err = %d", err );
        
        if ( !err )
        	{
		    TSignalLevelParams gsm = iHoPolicyReader->CsSignalLevelParams();
		    TSignalLevelParams wlan = iHoPolicyReader->PsSignalLevelParams();
		    iPolicy = iHoPolicyReader->HoPolicy();
		    
		    iWlanSignalLevelHandler->SetParams( wlan );
		    iGsmSignalLevelHandler->SetParams( gsm );
		    
		    TRAPD( err_wlan, iWlanSignalLevelHandler->StartL() );
		    RUBY_DEBUG1( "starting wlan signal level handler, err = %d", err_wlan );
		    TRAPD( err_gsm, iGsmSignalLevelHandler->StartL() );
		    RUBY_DEBUG1( "starting gsm signal level handler, err = %d", err_gsm );
		    
		    if ( !err_wlan && !err_gsm )
		    	{		    
			    // Enable the VoIP service to be able to do VoIP calls.
			    TRAPD( err_cch, iCchMonitor->EnableServiceL() );
			    RUBY_DEBUG1( "enabling service, err = %d", err_cch );
			    
			    if ( !err_cch )
			    	{
			    	// Everything was started ok
			    	iStarted = ETrue;
			    	}
		    	}
		    else
		    	{
		    	// Stop the handlers just in case either one was started
		    	iWlanSignalLevelHandler->Stop();
		    	iGsmSignalLevelHandler->Stop();
		    	}
        	}
        
        TRAP_IGNORE( UpdatePsKeysL() );
        }
    }

// -----------------------------------------------------------------------------
// Set the domain type of the current call
// -----------------------------------------------------------------------------
//
EXPORT_C void CVccHoTrigger::SetCurrentDomainType( TCallDomainType aDomainType )
    {
    RUBY_DEBUG_BLOCK( "CVccHoTrigger::SetCurrentDomainType" );
    iDomainType = aDomainType;
    }

// -----------------------------------------------------------------------------
// Set the preferred domain type
// -----------------------------------------------------------------------------
//
EXPORT_C void CVccHoTrigger::SetPreferredDomainType( 
                                        TVccHoPolicyPreferredDomain aDomainType )
    {
    RUBY_DEBUG1( "CVccHoTrigger::SetPreferredDomainType(%d)", aDomainType );
    iPolicy.SetPreferredDomain( aDomainType );
    }

// -----------------------------------------------------------------------------
// Set Immediate Domain Transfer on/off
// -----------------------------------------------------------------------------
//
EXPORT_C void CVccHoTrigger::SetImmediateDomainTransfer( TBool aImmediateDT )
    {
    RUBY_DEBUG1( "CVccHoTrigger::SetImmediateDomainTransfer(%d)", aImmediateDT );
    iPolicy.SetDoImmediateHo(  aImmediateDT );
    }

// -----------------------------------------------------------------------------
// Handles wlan signal change notifications
// -----------------------------------------------------------------------------
//
void CVccHoTrigger::WlanSignalChanged(
        TInt32 /*aSignalStrength*/,
        TSignalStrengthClass aClass )
    {
    RUBY_DEBUG_BLOCK( "CVccHoTrigger::WlanSignalChanged" );

    //Store signal strength class to that we know later what is the strength
    iWlanClass = aClass;
    
    RUBY_DEBUG1( " -iWlanClass=%x ", iWlanClass );
    RUBY_DEBUG1( " -iDomainType=%x ", iDomainType );
    RUBY_DEBUG1( " -iGsmClass=%x ", iGsmClass );
    RUBY_DEBUG1( " -DoImmediateHO=%x ", iPolicy.DoImmediateHo() );
    RUBY_DEBUG1( " -PreferredDomain=%x ", iPolicy.PreferredDomain() );
    RUBY_DEBUG1( " -iGsmClass=%x ", iGsmClass );
    RUBY_DEBUG1( " -DoHoInHeldWaitingCalls=%x ", iPolicy.DoHoInHeldWaitingCalls() );
    RUBY_DEBUG0( "0 = GOOD, 1 = WEAK, 2 = UNDEFINED" );
    
    // If the previous class is the same as the new one
    // - do nothing.
    
    if ( iPreviousWlanClass == iWlanClass )
        {
        RUBY_DEBUG0( "No change in WLAN signal class -> return" );
    
        return;
        }
    else
        {
        iPreviousWlanClass = iWlanClass;
        }
    
    TriggerHo();
     }

// -----------------------------------------------------------------------------
// Handles GSM singnal change notifications
// -----------------------------------------------------------------------------
//
void CVccHoTrigger::GsmSignalChanged(
        TInt32 aSignalStrength,
        TSignalStrengthClass aClass )
    {
    RUBY_DEBUG_BLOCK( "VccHoTrigger::GsmSignalChanged" );
    RUBY_DEBUG1( " -iCsSignalLevel=%d ", aSignalStrength );

    //Store signal strength class to that we know later what is the strength
    iGsmClass = aClass;
    
    // Save the signal strength as "status".
    // Zero (0) means that the service is not available.
    iCsSignalLevel = aSignalStrength;

    // Update P&S keys to tell if we can do ho or not.
    // If the signal level has gone to zero, we are unable to
    // do hondover.
    
    TVccHoStatus hoStatus( EVccHoStateUnknown );
    iEngPsProperty->GetCurrentHoStatus( hoStatus );
    
    if( hoStatus != EVccCsToPsHoStarted || hoStatus != EVccCsToPsHoInprogress )
        {
        RUBY_DEBUG0( "CS to PS HO in progress, not updating keys" );
        TRAP_IGNORE( UpdatePsKeysL() );
        }
    
    RUBY_DEBUG1( " -iWlanClass=%x ", iWlanClass );
    RUBY_DEBUG1( " -iDomainType=%x ", iDomainType );
    RUBY_DEBUG1( " -iGsmClass=%x ", iGsmClass );
    RUBY_DEBUG1( " -DoImmediateHO=%x ", iPolicy.DoImmediateHo() );
    RUBY_DEBUG1( " -PreferredDomain=%x ", iPolicy.PreferredDomain() );
    RUBY_DEBUG1( " -DoHoInHeldWaitingCalls=%x ", iPolicy.DoHoInHeldWaitingCalls() );  
    RUBY_DEBUG0( "0 = GOOD, 1 = WEAK, 2 = UNDEFINED" );
    
    // If the previous class is the same as the new one
    // - do nothing.
    
    if ( iPreviousGsmClass == iGsmClass )
        {
        RUBY_DEBUG0( "No change in GSM signal class -> return" );
    
        return;
        }
    else
        {
        iPreviousGsmClass = iGsmClass;
        }
    
    TriggerHo();
    }

// -----------------------------------------------------------------------------
// Handles CCH monitor notifications
// -----------------------------------------------------------------------------
//
void CVccHoTrigger::CchServiceStatusChanged( TServiceStatus aStatus )
    {
    RUBY_DEBUG_BLOCK( "VccHoTrigger::CchServiceStateChanged" );
    RUBY_DEBUG1( " -Service status = %d", aStatus );
    
    // Save the status
    iCchServiceStatus = aStatus;
    
    TRAP_IGNORE( UpdatePsKeysL() );
    TriggerHo();
    }

// -----------------------------------------------------------------------------
// Check if the CS or PS services are available or not
// -----------------------------------------------------------------------------
//
TBool CVccHoTrigger::ServicesAvailable()
    {
    RUBY_DEBUG_BLOCK( "CVccHoTrigger::AreServicesAvailable" );
    
    TBool retVal( ETrue );
    
    // If PS service (VoIP service) is unavailable or
    // the CS service == 0 (i.e. signal level),
    // we cannot do handover.
    
    if ( iCchServiceStatus == EServiceUnavailable || 
                    iCsSignalLevel == KStrengthMin )
        {
        RUBY_DEBUG0( " -VoIP/CS service(s) not available" );
        retVal = EFalse;
        }
    
    return retVal;
    }

// -----------------------------------------------------------------------------
// Update (write) service status state to P&S.
// -----------------------------------------------------------------------------
//
void CVccHoTrigger::UpdatePsKeysL()
    {
    RUBY_DEBUG_BLOCK( "CVccHoTrigger::UpdatePsKeysL" );
    
    // If we are started and both services are available,
    // handover can be done.
    // Services available in CS means that we have
    // signal strength != KStrengthMin (which is 110, returned
    // from the CS signal level monitor).
    
    RUBY_DEBUG1( " -iStarted = %d", iStarted );
    RUBY_DEBUG1( " -iHoNotAllowed = %d", iHoNotAllowed );
    RUBY_DEBUG1( " -Service status = %d (0=unavailable, 1=available)", iCchServiceStatus );
    RUBY_DEBUG1( " -CS signal min level = 110, current level = %d", iCsSignalLevel );
        
    if ( iStarted &&
            ( iCchServiceStatus == EServiceAvailable ) && 
            ( iCsSignalLevel != KStrengthMin ) &&
            ( !iHoNotAllowed ) )
        {
        RUBY_DEBUG0( " -We are started and both services are available" );
        // Check HO direction restrictions
        if ( ( iPolicy.AllowedDirection() & ECsToPsAllowed ) && 
                ( iPolicy.AllowedDirection() & EPsToCsAllowed ) )
            {
            if( iPolicy.DoHoInHeldWaitingCalls() )
                {
                RUBY_DEBUG0( " -Services available and ho allowed to both \
                             directions, also in multicall" );
                iEngPsProperty->NotifySubscriberL( EVccHoStateIdle, KErrNone );
                }
            else
                {
                RUBY_DEBUG0( " -Services available and ho allowed to both \
                             directions but not in multicall" );
                iEngPsProperty->NotifySubscriberL( EVccHoStateIdleIfSingleCall, KErrNone );
                }
            }
        else if ( !( iPolicy.AllowedDirection() & ECsToPsAllowed  ) &&
                ( iPolicy.AllowedDirection() & EPsToCsAllowed ) )
            {
            if( iPolicy.DoHoInHeldWaitingCalls() )
                {
                RUBY_DEBUG0( " -HO allowed only to CS" );
                iEngPsProperty->NotifySubscriberL( EVccCsToPsNotAllowed, KErrNone );
                }
            else
                {
                RUBY_DEBUG0( " -HO allowed only to CS, in single call situation" );
                iEngPsProperty->NotifySubscriberL( EVccHoAllowedToCsIfSingleCall, KErrNone );
                }
            }
        else if ( !( iPolicy.AllowedDirection() & EPsToCsAllowed ) &&  
                    ( iPolicy.AllowedDirection() & ECsToPsAllowed ) )
            {
            if( iPolicy.DoHoInHeldWaitingCalls() )
                {
                RUBY_DEBUG0( " -HO allowed only to PS" );
                iEngPsProperty->NotifySubscriberL( EVccPsToCsNotAllowed, KErrNone );
                }
            else
                {
                RUBY_DEBUG0( " -HO allowed only to PS, in single call situation" );
                iEngPsProperty->NotifySubscriberL( EVccHoAllowedToPsIfSingleCall, KErrNone );
                }
            }
        }
    else
        {
        RUBY_DEBUG0( " -We are stopped or services are unavailable:" );
        iEngPsProperty->NotifySubscriberL( EVccHoUnavailable, KErrNotReady );        
        }
    }
    
// -----------------------------------------------------------------------------
// If manual HO is done, no automatic should be made.
// -----------------------------------------------------------------------------
//
EXPORT_C void CVccHoTrigger::ManualHoCallStarted()
    {
    RUBY_DEBUG_BLOCK( "CVccHoTrigger::ManualHoCallStarted" );
    iManualHoDone = ETrue;
    }

// -----------------------------------------------------------------------------
// After manual HO call has been released automatic HOs can be made again.
// -----------------------------------------------------------------------------
//
EXPORT_C void CVccHoTrigger::ManualHoCallReleased()
    {
    RUBY_DEBUG_BLOCK( "CVccHoTrigger::ManualHoCallReleased" );
    iManualHoDone = EFalse;
    }

// -----------------------------------------------------------------------------
// During conference HO is not allowed
// -----------------------------------------------------------------------------
//
EXPORT_C void CVccHoTrigger::HoNotAllowedL()
	{
	RUBY_DEBUG_BLOCK( "CVccHoTrigger::HoNotAllowedL" );
	iHoNotAllowed = ETrue;
	UpdatePsKeysL();
	}

// -----------------------------------------------------------------------------
// After conference HO is allowed again
// -----------------------------------------------------------------------------
//
EXPORT_C void CVccHoTrigger::HoAllowed()
	{
	RUBY_DEBUG_BLOCK( "CVccHoTrigger::HoAllowed" );
	iHoNotAllowed = EFalse;
	TRAP_IGNORE( UpdatePsKeysL() );
	}

// -----------------------------------------------------------------------------
// Read settings
// -----------------------------------------------------------------------------
//
EXPORT_C void CVccHoTrigger::ReadHoAllowedWhenCsOriginatedSettingL()
    {
    iHoPolicyReader->ReadSettingsL();
    iPolicy = iHoPolicyReader->HoPolicy();
    iHoAllowedIfCsOriginated = iPolicy.DtAllowedWhenCsOriginated();
    }
    
// -----------------------------------------------------------------------------
// Initiate the actual ho when its ok
// -----------------------------------------------------------------------------
//
void CVccHoTrigger::TriggerHo()
	{
	RUBY_DEBUG_BLOCK( "CVccHoTrigger::TriggerHo" );
	//Check if manual ho is already made during this call and the 
	//service availability.
	if( iManualHoDone || !ServicesAvailable() || iHoNotAllowed )
		{
		return;
		}
	
	//If immediate ho is "ON" do it first and then return.
	if( iPolicy.DoImmediateHo() )
		{
		if ( DoImmediateHo() )
		    {
		    RUBY_DEBUG0( "VccHoTrigger::TriggerHo - immediate HO was initiated" );
		    return;
		    }
		RUBY_DEBUG0( "VccHoTrigger::TriggerHo - no immediate HO" );
		}
	
	if ( iWlanClass == ESignalClassWeak &&
	     iGsmClass == ESignalClassNormal && 
	     ( iPolicy.AllowedDirection() & EPsToCsAllowed  ))
	    {
	    RUBY_DEBUG0( "VccHoTrigger::WlanSignalChanged - NotifySubscriberL" );
	        
	    // First empty the key, so that director 
	    // gets notified about all changes
	        
	    TRAP_IGNORE( iProperty->NotifySubscriberL( EVccNoRequestOngoing ) );
	    // Check whether HO is allowed in multicall situation
	    if( iPolicy.DoHoInHeldWaitingCalls() )
	        {
	        TRAP_IGNORE( iProperty->NotifySubscriberL(
	                    EVccAutomaticStartPsToCsHoRequest ) );
	        }
	    else
	        {
	        TRAP_IGNORE( iProperty->NotifySubscriberL(
	                    EVccAutomaticStartPsToCsHoRequestIfSingleCall ) );
	        }
	        
	    }        
	
	else if ( iGsmClass == ESignalClassWeak &&
            iWlanClass == ESignalClassNormal &&
            ( iPolicy.AllowedDirection() & ECsToPsAllowed ))
        {
        RUBY_DEBUG0( "VccHoTrigger::GsmSignalChanged - NotifySubscriberL" );
        
        // First empty the key, so that director 
        // gets notified about all changes
        
        TRAP_IGNORE( iProperty->NotifySubscriberL( EVccNoRequestOngoing ) );
        // Check whether HO is allowed in multicall situation
        if( iPolicy.DoHoInHeldWaitingCalls() )
            {
            TRAP_IGNORE( iProperty->NotifySubscriberL(
                        EVccAutomaticStartCsToPsHoRequest ) );
            }
        else
            {
            TRAP_IGNORE( iProperty->NotifySubscriberL(
                        EVccAutomaticStartCsToPsHoRequestIfSingleCall ) );
            }
        }
    }

// -----------------------------------------------------------------------------
// Initiate immediate ho
// -----------------------------------------------------------------------------
//
TBool CVccHoTrigger::DoImmediateHo()
	{
	RUBY_DEBUG_BLOCK( "CVccHoTrigger::DoImmediateHo" );
	TBool ret( EFalse );
	if( ( iPolicy.PreferredDomain() == ECsPreferred ) &&
	    iGsmClass ==  ESignalClassNormal &&
        ( iPolicy.AllowedDirection() & EPsToCsAllowed ) )  
		{
		// Current call is PS, CS signal is ok, preferred domain is CS
        // and immediate HO is requested -> HANDOVER to CS
        
        RUBY_DEBUG0( "VccHoTrigger::GsmSignalChanged - NotifySubscriberL ->\
                      IMMEDIATE HO to CS" );
        
        // First empty the key, so that director gets
        // notified about all changes
        
        TRAP_IGNORE( iProperty->NotifySubscriberL( EVccNoRequestOngoing ) );
        
        // Check whether HO is allowed in multicall situation
        if( iPolicy.DoHoInHeldWaitingCalls() )
            {
            TRAP_IGNORE( iProperty->NotifySubscriberL(
                        EVccAutomaticStartPsToCsHoRequest ) );
            ret = ETrue;
            }
        else
            {
            TRAP_IGNORE( iProperty->NotifySubscriberL(
                        EVccAutomaticStartPsToCsHoRequestIfSingleCall ) );
            ret = ETrue;
            }
		}
	
	else if ( (iPolicy.PreferredDomain() == EPsPreferred)  &&
	      iWlanClass == ESignalClassNormal &&
	      ( iPolicy.AllowedDirection() & ECsToPsAllowed  ) )
        {
        // Current call is CS, PS signal is ok, preferred domain is PS and 
	    // immediate HO is requested -> HANDOVER to PS

        RUBY_DEBUG0( "VccHoTrigger::WlanSignalChanged - NotifySubscriberL ->\
                      IMMEDIATE HO to PS" );
	       
	    // First empty the key, so that director gets 
	    // notified about all changes
	        
	    TRAP_IGNORE( iProperty->NotifySubscriberL( EVccNoRequestOngoing ) );
	        
        // Check whether HO is allowed in multicall situation
	    if( iPolicy.DoHoInHeldWaitingCalls() )
            {
	        TRAP_IGNORE( iProperty->NotifySubscriberL(
	                    EVccAutomaticStartCsToPsHoRequest ) );
	        ret = ETrue;
	        }
	    else
	        {
	        TRAP_IGNORE( iProperty->NotifySubscriberL(
	                       EVccAutomaticStartCsToPsHoRequestIfSingleCall ) );
	        ret = ETrue;
	        }
        }
	return ret;
	}
