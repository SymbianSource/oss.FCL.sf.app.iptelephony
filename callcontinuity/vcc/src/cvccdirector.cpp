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
* Description:   Handles all common logic for VCC proxy and handles sessions.
*
*/

//cleanup stack operations
#include <mccpssobserver.h>
#include <spnotifychange.h>
#include <spsettings.h>
#include <spentry.h>
#include <spproperty.h>

#include "vcchotrigger.h"
#include "cvccdirector.h"
#include "cvccconferencecall.h"
#include "cvccperformer.h"
#include "vccengpspropertylistener.h"
#include "vccsubscribekeys.h"
#include "vccsettingsreader.h"
#include "vccdefinitions.h"
#include "rubydebug.h"
#include "cvccdtmfprovider.h"

// ---------------------------------------------------------------------------
// CVccDirector::CVccDirector
// ---------------------------------------------------------------------------
//
CVccDirector::CVccDirector(): 
	iImplementationUid( KVCCImplementationUid )
	
    {
    RUBY_DEBUG_BLOCK( "CVccDirector::CVccDirector" );
    }

// ---------------------------------------------------------------------------
// CVccDirector::PointerArrayCleanup
// ---------------------------------------------------------------------------
//
void CVccDirector::PointerArrayCleanup( TAny* aArray )
    {
    static_cast<RPointerArray<CConvergedCallProvider>*>( aArray )->ResetAndDestroy();
    static_cast<RPointerArray<CConvergedCallProvider>*>( aArray )->Close();
    }

// ---------------------------------------------------------------------------
// CVccDirector::ConstructL
// ---------------------------------------------------------------------------
//
void CVccDirector::ConstructL()
    {
    RUBY_DEBUG_BLOCKL( "CVccDirector::ConstructL" );

    FillCallProviderArrayL();
    
    // Create the HO-trigger and connect to the wlan network
    iHoTrigger = CVccHoTrigger::NewL();
    
	iPropListener = CVccEngPsPropertyListener::NewL( KPSVccPropertyCategory, 
            KVccPropKeyHoRequest );
	RUBY_DEBUG0( "- Called CVccEngPsPropertyListener::NewL" );
	iPropListener->AddObserverL( *this );
	RUBY_DEBUG0( "- Called iPropListener->AddObserverL" );
	iPropListener->Start();
	RUBY_DEBUG0( "- Called iPropListener->Start()" );
	
	//notifies about the changes to SP table
	iSpNotifier = CSPNotifyChange::NewL( *this );
	//start service provider table notifier
    StartSpNotifierL();
    
	//link states
  	iStateInit.LinkState(iStateCalling);
  	iStateCalling.LinkState(iStateReleasing, iStateInit, iStateFailing );
  	iStateReleasing.LinkState(iStateInit);
  	iStateFailing.LinkState( iStateInit );
  	
  	iHoKeyValue = KErrNotFound;
    }

// ---------------------------------------------------------------------------
// CVccDirector::NewL
// ---------------------------------------------------------------------------
//
CVccDirector* CVccDirector::NewL()
    {
    RUBY_DEBUG_BLOCKL( "CVccDirector::NewL" );
    CVccDirector* self = new ( ELeave ) CVccDirector();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CVccDirector::~CVccDirector
// ---------------------------------------------------------------------------
//
CVccDirector::~CVccDirector()
    {
    RUBY_DEBUG0( "CVccDirector::~CVccDirector() - ENTER" );

    delete iConference;
    iProviders.ResetAndDestroy();
    iProviders.Close();
    iPerfArray.ResetAndDestroy();
    iPerfArray.Close();
    iInitialisedPlugins.Close();
    delete iPropListener;
    
    if( iSpNotifier )
    	{
    	iSpNotifier->Cancel();
    	}
   	delete iSpNotifier;
    
    delete iHoTrigger;
    delete iDtmfProvider;
    REComSession::FinalClose();
    
    RUBY_DEBUG0( "CVccDirector::~CVccDirector() - EXIT" );
    }

// ---------------------------------------------------------------------------
// CVccDirector::FillCallProviderArrrayL
// ---------------------------------------------------------------------------
//
void CVccDirector::FillCallProviderArrayL()
	{
    //no supplementary services
    //creating cs and sipvoipproviders
    
    //List implementations

    RImplInfoPtrArray implementations;
    TCleanupItem arrayCleanup( PointerArrayCleanup, &implementations );
    CleanupStack::PushL( arrayCleanup );

    CConvergedCallProvider::ListImplementationsL( implementations );
 
    for( TInt i = 0; i<implementations.Count(); i++ )
        {
        RUBY_DEBUG1( "- for loop counter value: %d", i );
        CImplementationInformation *info = implementations[i];
        if ( IsVccOwnedPlugin ( info->ImplementationUid().iUid ) )
            {
            CConvergedCallProvider* provider =
                CConvergedCallProvider::NewL( info->ImplementationUid() );
            CleanupStack::PushL( provider );
           
            User::LeaveIfError( iProviders.Append( provider ) );
            
            CleanupStack::Pop( provider ); //aProvider
            }
        }
    
    CleanupStack::PopAndDestroy();//implementations
	}

// ---------------------------------------------------------------------------
// CVccDirector::IsVccOwnedPlugin
// Checks is the given call provider plugin used by VCC 
// ---------------------------------------------------------------------------
//
TBool CVccDirector::IsVccOwnedPlugin( TInt aPluginId )
	{
	if( aPluginId == KSipVoipCallProviderPlugId || 
		aPluginId == KCSCallProviderPlugId	)
			return ETrue;
	
	return EFalse;
	}

// ---------------------------------------------------------------------------
// CVccDirector::InitializeL
// Tries to initialize all call provider plugins used by VCC.
// ---------------------------------------------------------------------------
//
void CVccDirector::InitializeL( const MCCPObserver& aMonitor,
                                  const MCCPSsObserver& aSsObserver )
    {
    RUBY_DEBUG_BLOCKL( "CVccDirector::InitializeL" );
    
    if ( iProviders.Count() == 0 )
    	{
    	User::Leave( KErrNotFound );
    	}
	
    iMandatoryPluginFailedError = KErrNone;

    //MT: setting CVccDirector as observer to plugin
    Initialize( *this, aSsObserver );

    // Leave if mandatory plugins failed
    User::LeaveIfError( iMandatoryPluginFailedError );

    // save CCP monitor ; CCCEPluginMCCPObserver
    const MCCPCSObserver& obs = static_cast<const MCCPCSObserver&>( aMonitor );
    iCCPObserver = const_cast<MCCPCSObserver*>(&obs);

    const MCCPSsObserver& ssObs = static_cast<const MCCPSsObserver&>( aSsObserver );
    iCCPSsObserver = const_cast<MCCPSsObserver*>(&ssObs);
    
    }

// ---------------------------------------------------------------------------
// CVccDirector::InitializeL
// Tries to initialize call provider plugins VCC uses. 
// Informs if  failure.
// ---------------------------------------------------------------------------
//
TBool CVccDirector::Initialize( const MCCPObserver& aMonitor,
        const MCCPSsObserver& aSsObserver )
	{
    RUBY_DEBUG_BLOCK( "CVccDirector::Initialize" );
   
    TBool initSucceeded = ETrue;
	
	for ( TInt i = 0; i < iProviders.Count() ; i++)
        {
        TUid pluginUid = iProviders[ i ]->Uid();
        RUBY_DEBUG1( "starting to initialize plugin index= %d", i );
    	
        TRAPD( err, InitializeL( aMonitor, aSsObserver, *(iProviders[ i ]) ) );
        RUBY_DEBUG1( "initialization err = %d", err );
        if ( KErrNone != err && KCSCallProviderPlugId == pluginUid.iUid ) // CS plugin mandatory
            {
            RUBY_DEBUG0( "-- Mandatory VCC plugin initialization FAILED" );    
            initSucceeded = EFalse; 
            iMandatoryPluginFailedError = err;
            }
        else
            {
            switch( err )
                {
                case KErrNone:
                    iInitialisedPlugins.Append(  pluginUid.iUid );
                    //fall-through
                case KErrAlreadyExists:
                    RUBY_DEBUG0( "-- VCC plugin initialization OK" );
                    break;
                default:
                    RUBY_DEBUG0( "-- VCC plugin initialization FAILED" );
                    initSucceeded = EFalse;
                    break;
                }
            }
        }
	
	RUBY_DEBUG1( "-- VCC plugin nbr of initialized plugins:%d",
			iInitialisedPlugins.Count() );
	return initSucceeded; 
    }

// ---------------------------------------------------------------------------
// CVccDirector::InitializeL
// Finds from the VCC settings the service id that should be used with the 
// given plugin. Uses the service id to initialize plugin.
// ---------------------------------------------------------------------------
//
void CVccDirector::InitializeL( const MCCPObserver& aMonitor,
        const MCCPSsObserver& aSsObserver, CConvergedCallProvider& aPlugin )
	{
    RUBY_DEBUG_BLOCKL( "CVccDirector::InitializeL" );
	TInt serviceId = VccSettingsReader::ServiceIdL( aPlugin.Uid().iUid );
	RUBY_DEBUG1( "Initialize plugin with serviceId = %d", serviceId );	   
    
	User::LeaveIfError( serviceId );
	
	aPlugin.InitializeL( serviceId, aMonitor, 
			aSsObserver );
	}

// ---------------------------------------------------------------------------
// CVccDirector::NewCallL
// ---------------------------------------------------------------------------
//
MCCPCall* CVccDirector::NewCallL(
                            const CCCPCallParameters& aParameters,
                            const TDesC& aRecipient,
                            const MCCPCallObserver& aObserver )
    {
    RUBY_DEBUG_BLOCKL( "CVccDirector::NewCallL" );

    // Check call type and translate it into domain type
    CVccPerformer* perf;
    TBool csOriginated = EFalse;
    
    if( aParameters.CallType() == CCPCall::ECallTypeCSVoice )
        {
        RUBY_DEBUG0("CS Call");
        iHoTrigger->SetCurrentDomainType( CVccHoTrigger::ECallDomainTypeCS );
        csOriginated = ETrue;
        iHoTrigger->ReadHoAllowedWhenCsOriginatedSettingL();
        }
    else
        {
        RUBY_DEBUG0("PS Call");
        iHoTrigger->SetCurrentDomainType( CVccHoTrigger::ECallDomainTypePS );
        }
    
    perf = CVccPerformer::NewL( iProviders, 
                                iStateInit, 
                                *iHoTrigger, 
                                csOriginated );
    CleanupStack::PushL( perf );
    User::LeaveIfError( iPerfArray.Append( perf ) ); 
    CleanupStack::Pop( perf ); // perf
    return perf->CreatePrimaryCallL( aParameters, aRecipient, aObserver );
    }

// ---------------------------------------------------------------------------
// Releases call
// ---------------------------------------------------------------------------
//
TInt CVccDirector::ReleaseCall( MCCPCall& aCall )
    {
    RUBY_DEBUG_BLOCK( "CVccDirector::ReleaseCall" );
    TInt err = KErrNotFound;
    //Go through the performer array to see which performer should be deleted.
    for( TInt i = 0; i < iPerfArray.Count(); i++ )
    	{
    	if( iPerfArray[i] == &aCall )
    		{
            err = iPerfArray[ i ]->ReleaseCall( aCall ); 
            iPerfArray[ i ]->ReleaseSecondaryCallLeg();
            delete iPerfArray[ i ];
            iPerfArray.Remove( i );
            iPerfArray.Compress();
            break;
    		}
    	}
    if( iPerfArray.Count() == 0 )
        {
        //make it possible to initiate HOs in new call
        iHoTrigger->ManualHoCallReleased();
        iHoTrigger->HoAllowed();
        iHoTrigger->Stop();
        }
    //If iHoKeyValue it means that PropertyChanged function has been visited
    //i.e. handover should be made. Non-active calls need to be released
    //before handover can be made.
    if ( iHoKeyValue >= KErrNone )
        {
        TRAP( err, PropertyChangedL( KPSVccPropertyCategory, 
                            KVccPropKeyHoRequest, 
                            iHoKeyValue ) );
        }
    
    return err;
    }

// ---------------------------------------------------------------------------
// Releases emergency call
// ---------------------------------------------------------------------------
//
TInt CVccDirector::ReleaseEmergencyCall( MCCPEmergencyCall& aCall )
    {
    RUBY_DEBUG_BLOCK( "CVccDirector::ReleaseEmergencyCall" );
    for( TInt i = 0; i < iProviders.Count() ; i++)
           {
           TUid pluginUid = iProviders[ i ]->Uid();
           if (pluginUid.iUid == KCSCallProviderPlugId )
               {
               RUBY_DEBUG0( "CS Plugin found" ); 
               return iProviders[i]->ReleaseEmergencyCall( aCall );
               }
           }
    
    return KErrNotFound;
    }

// ---------------------------------------------------------------------------
// Releases conference call
// ---------------------------------------------------------------------------
//
TInt CVccDirector::ReleaseConferenceCall( MCCPConferenceCall& /*aCall*/ )
    {
    RUBY_DEBUG_BLOCK( "CVccDirector::ReleaseConferenceCall" );
    
    iConference->ReleaseConference();
    delete iConference;
    iConference = NULL;
    iHoTrigger->HoAllowed();
    if( iPerfArray.Count() > 0 )
    	{
    	for( TInt i = 0; i < iPerfArray.Count(); i++ )
    		{
    		iPerfArray[i]->ConferenceEnded();
    		}
    	}
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CVccDirector::Uid
// ---------------------------------------------------------------------------
//
const TUid& CVccDirector::Uid() const
    {
    RUBY_DEBUG_BLOCK( "CVccDirector::Uid" );
    return iImplementationUid;
    }

// from ConvergedCallProvider

// ---------------------------------------------------------------------------
// CVccDirector::NewEmergencySessionL
// ---------------------------------------------------------------------------
//
MCCPEmergencyCall* CVccDirector::NewEmergencyCallL(
    const TUint32 aServiceId,
    const TDesC&  aAddress,
    const MCCPCallObserver& aObserver )
    {
    RUBY_DEBUG_BLOCKL( "CVccDirector::NewEmergencyCallL" );
    for( TInt i = 0; i < iProviders.Count() ; i++)
        {
        TUid pluginUid = iProviders[ i ]->Uid();
        if (pluginUid.iUid == KCSCallProviderPlugId )
            {
            RUBY_DEBUG0( "CS Plugin found" ); 
            return iProviders[i]->NewEmergencyCallL( aServiceId, aAddress, aObserver );
            }
        }
    return NULL;
    }

// ---------------------------------------------------------------------------
// CVccDirector::NewConferenceL
// ---------------------------------------------------------------------------
//
MCCPConferenceCall* CVccDirector::NewConferenceL(
        const TUint32 aServiceId,
        const MCCPConferenceCallObserver& aObserver )
    {
    RUBY_DEBUG_BLOCKL( "CVccDirector::NewConferenceL" );
    
    if ( !iConference )
        {
        iConference = CVccConferenceCall::NewL( aServiceId, aObserver, iPerfArray );
        }
    else
        {
        iConference->AddObserverL( aObserver );
        }

    iHoTrigger->HoNotAllowedL();
    return iConference;
    }

// ---------------------------------------------------------------------------
// CVccDirector::Caps
// ---------------------------------------------------------------------------
//
TUint32 CVccDirector::Caps() const
    {
    RUBY_DEBUG_BLOCK( "CVccDirector::Caps()" );
    return 0;
    }

// ---------------------------------------------------------------------------
// CVccDirector::DTMFProvider
// ---------------------------------------------------------------------------
//
MCCPDTMFProvider* CVccDirector::DTMFProviderL(
                                           const MCCPDTMFObserver& aObserver )
    {
    RUBY_DEBUG_BLOCKL( "CVccDirector::DTMFProviderL" );
    if( !iDtmfProvider )
        {
        iDtmfProvider = CVccDtmfProvider::NewL( iProviders, aObserver, *this );
        }
    return iDtmfProvider; 
    }


// ---------------------------------------------------------------------------
// CVccDirector::ExtensionProvider
// ---------------------------------------------------------------------------
//
MCCPExtensionProvider* CVccDirector::ExtensionProviderL(
                                const MCCPExtensionObserver& /*aObserver*/ )
    {
    RUBY_DEBUG_BLOCKL( "CVccDirector::ExtensionProviderL" );
    return NULL;
    }

// ---------------------------------------------------------------------------
// CVccDirector::AddObserverL
// ---------------------------------------------------------------------------
//
void CVccDirector::AddObserverL( const MCCPDTMFObserver& aObserver )
    {
    RUBY_DEBUG_BLOCKL( "CVccDirector::AddObserverL" );
    if(iDtmfProvider)
        {
        iDtmfProvider->AddObserverL(aObserver);
        }
    }

// ---------------------------------------------------------------------------
// CVccDirector::RemoveObserver
// ---------------------------------------------------------------------------
//
TInt CVccDirector::RemoveObserver( const MCCPDTMFObserver& aObserver )
    {
    RUBY_DEBUG_BLOCK( "CVccDirector::RemoveObserver" );
    if(iDtmfProvider)
        {
        return iDtmfProvider->RemoveObserver(aObserver);
        }
    return KErrNotFound;
    }

// ---------------------------------------------------------------------------
// CVccDirector::GetLifeTime
// ---------------------------------------------------------------------------
//
TBool CVccDirector::GetLifeTime( TDes8& aLifeTimeInfo )
    {
    for( TInt i = 0; i < iProviders.Count(); i++ )
        {
        if( iProviders[ i ]->GetLifeTime( aLifeTimeInfo ) )
            {
            return ETrue;
            }
        }
    return EFalse;
    }
    
// ---------------------------------------------------------------------------
// CVccDirector::GetCSInfo
// ---------------------------------------------------------------------------
//
TBool CVccDirector::GetCSInfo( CSInfo& aCSInfo )
    {
    RUBY_DEBUG_BLOCK( "CVccDirector::GetCSInfo" );
    for( TInt i = 0; i < iProviders.Count(); i++ )
        {
        if( iProviders[ i ]->GetCSInfo( aCSInfo ) )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CVccDirector::CPPObserver()
// ---------------------------------------------------------------------------
//
const MCCPCSObserver* CVccDirector::CPPObserver() const
    {
    RUBY_DEBUG_BLOCK( "CVccDirector::CPPObserver" );
    return iCCPObserver;
    }

// ---------------------------------------------------------------------------
// CVccDirector::ErrorOccurred
// ---------------------------------------------------------------------------
//
void CVccDirector::ErrorOccurred( const TCCPError /*aError*/ )
    {
    RUBY_DEBUG_BLOCK( "CVccDirector::ErrorOccurred" );
    }

// ---------------------------------------------------------------------------
// CVccDirector::IncomingCall
// Transfer call
// ---------------------------------------------------------------------------
//
void CVccDirector::IncomingCall( MCCPCall* /*aCall*/, MCCPCall& /*aTempCall*/ )
    {
    RUBY_DEBUG_BLOCK( "CVccDirector::IncomingCall" );
    }

// ---------------------------------------------------------------------------
// CVccDirector::IncomingCall
// 
// ---------------------------------------------------------------------------
//
void CVccDirector::IncomingCall( MCCPCall* aCall )
    {
    TRAP_IGNORE( IncomingCallL( aCall ) );
    }

// ---------------------------------------------------------------------------
// CVccDirector::IncomingCallL
// 
// ---------------------------------------------------------------------------
//
void CVccDirector::IncomingCallL( MCCPCall* aCall )
    {
    RUBY_DEBUG_BLOCK( "CVccDirector::IncomingCallL" );

    RUBY_DEBUG1( "CVccDirector::IncomingCallL - array count: %d", iPerfArray.Count() );
    TBool csOriginated = EFalse;
    if( aCall->Parameters().CallType() == CCPCall::ECallTypeCSVoice )
        {
        RUBY_DEBUG0("incoming CS Call");
        iHoTrigger->SetCurrentDomainType( CVccHoTrigger::ECallDomainTypeCS );
        csOriginated = ETrue;
        iHoTrigger->ReadHoAllowedWhenCsOriginatedSettingL();
        
        }
    
    else
        {
        RUBY_DEBUG0("incoming PS Call");
        iHoTrigger->SetCurrentDomainType( CVccHoTrigger::ECallDomainTypePS );
        }
    
    //Create 1 performer for each call
    
    CVccPerformer* perf = CVccPerformer::NewL( iProviders, 
                                               iStateInit, 
                                               *iHoTrigger,
                                               csOriginated );
    CleanupStack::PushL( perf );
    User::LeaveIfError( iPerfArray.Append( perf ) );
	CleanupStack::Pop( perf ); //perf
    perf->IncomingCall( aCall );
 	iCCPObserver->IncomingCall( perf );
    }

// ---------------------------------------------------------------------------
// CVccDirector::CallCreated
// ---------------------------------------------------------------------------
//
void CVccDirector::CallCreated( MCCPCall* /*aNewTransferCall*/,
                              MCCPCall* /*aOriginator*/,
                              TBool /*aAttented*/ )
    {
    RUBY_DEBUG_BLOCK( "CVccDirector::CallCreated" );
    }

// ---------------------------------------------------------------------------
// CVccDirector::DataPortName
// ---------------------------------------------------------------------------
//
void CVccDirector::DataPortName( TName& aPortName )
    {
    RUBY_DEBUG_BLOCK( "CVccDirector::DataPortName" );
    iCCPObserver->DataPortName( aPortName );
    }

// -----------------------------------------------------------------------------
//  Gets called when PS key is changed
// -----------------------------------------------------------------------------
//
void CVccDirector::PropertyChangedL( const TUid /*aCategoryId*/, 
                                    const TUint aKeyId,
                                    const TInt aValue )	
    {
    RUBY_DEBUG_BLOCKL( "CVccDirector::PropertyChangedL" );
    // Go through the performer array to see which call is in active state.
    // HO is done for the active call. Other calls are dropped.
    if (aKeyId == KVccPropKeyHoRequest && 
    	( aValue == EVccManualStartCsToPsHoRequest ||
    	  aValue == EVccAutomaticStartCsToPsHoRequest ||
          aValue == EVccManualStartPsToCsHoRequest ||
          aValue == EVccAutomaticStartPsToCsHoRequest ||
          aValue == EVccAutomaticStartPsToCsHoRequestIfSingleCall ||
          aValue == EVccAutomaticStartCsToPsHoRequestIfSingleCall ) )
		{
			// If manual handover is initiated during active call, automatic 
			// handovers are no longer done during the ongoing call
			// ==> stopping HoTrigger
			if ( aValue == EVccManualStartCsToPsHoRequest || 
             	aValue == EVccManualStartPsToCsHoRequest )
				{
				iHoTrigger->ManualHoCallStarted();
				}
		//Check state of call and handover when possible
        SwitchL( aValue );
		
	    }//if
		
	else if (aKeyId == KVccPropKeyHoStatus)
		{
		RUBY_DEBUG0( "CVccDirector::PropertyChangedL -- no handover" );
		iHoKeyValue = KErrNotFound;	
		}//else-if

    }

// -----------------------------------------------------------------------------
// Callback function for service settings table observer.
// Service provider settings table was changed.  
// Only VCC service changes are notified
// -----------------------------------------------------------------------------
//
void CVccDirector::HandleNotifyChange( TServiceId /*aServiceId*/ )
    {
    RUBY_DEBUG_BLOCK( "CVccDirector::HandleNotifyChange" );
    
    if( !IsPluginInitialized())
    	RetryInitialization();
    }

// -----------------------------------------------------------------------------
// HandleError event recieved from service provider settings table observer
// -----------------------------------------------------------------------------
//
void CVccDirector::HandleError( TInt /*aError*/ )
   {
   RUBY_DEBUG_BLOCK( "CVccDirector::HandleError" );
   }

// -----------------------------------------------------------------------------
// Get call type of the call and return it.
// -----------------------------------------------------------------------------
//
TInt CVccDirector::CurrentCallTypeForDTMF()
    {
    RUBY_DEBUG_BLOCK( "CVccDirector::CurrentCallTypeForDTMF" );
    TInt ret = KErrNotFound;
    
    for( TInt i = 0; i < iPerfArray.Count(); i++ )
        {
        if( iPerfArray[i]->PrimaryCall()->State() == 
            MCCPCallObserver::ECCPStateDialling ||
            iPerfArray[i]->PrimaryCall()->State() == 
            MCCPCallObserver::ECCPStateConnecting ||
            iPerfArray[i]->PrimaryCall()->State() == 
            MCCPCallObserver::ECCPStateConnected )
            {
            ret = (TInt) iPerfArray[i]->PrimaryCall()->Parameters().CallType();
            break;
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// Get call type of the call and and fetch that call provider and
// return it.
// -----------------------------------------------------------------------------
//
CConvergedCallProvider* CVccDirector::GetProvider()
    {
    CConvergedCallProvider* ret = NULL;
    
    //if no performers in array, all calls have been disconnected
    //and current call must be emergency call -> return CS provider
    if( iPerfArray.Count() > 0 )
        {
        for( TInt i = 0; i < iPerfArray.Count(); i++ )
            {
            if( iPerfArray[i]->PrimaryCall()->State() == 
                MCCPCallObserver::ECCPStateDialling ||
                iPerfArray[i]->PrimaryCall()->State() == 
                MCCPCallObserver::ECCPStateConnecting ||
                iPerfArray[i]->PrimaryCall()->State() == 
                MCCPCallObserver::ECCPStateConnected )
                {
                if( iPerfArray[ i ]->PrimaryCall()->Parameters().CallType() == 
                                               CCPCall::ECallTypePS )
                    {
                    ret = &iPerfArray[i]->PsProvider();
                    break;
                    }
                else
                    {
                    ret = &iPerfArray[i]->CsProvider();
                    break;
                    }
                }
            }
        }
    else
        {
        for( TInt i = 0; i < iProviders.Count(); i++ )
            {
            TUid pluginUid = iProviders[ i ]->Uid();
            if( pluginUid.iUid == KCSCallProviderPlugId )
                {
                ret = iProviders[ i ];
                }
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
//  Checks state of calls and acts accordingly
//  Anything else except Waiting or connected - hangup
//  Waiting - reject
//  Connected - handover
// -----------------------------------------------------------------------------
//
void CVccDirector::SwitchL( const TInt aValue )
    {
    RUBY_DEBUG_BLOCKL( "CVccDirector::SwitchL" );
    //if there is only 1 performer in the array it means that handover can be
    //made with that performer. If there are more performers it meas that it
    //is multicall situation and non-active calls need to be released before
    //handover.
    //This function is called as many times as there are performers. See 
    //ReleaseCall!
    if (iPerfArray.Count() > 1 )
        {
        if( aValue == EVccAutomaticStartCsToPsHoRequestIfSingleCall || 
            aValue == EVccAutomaticStartPsToCsHoRequestIfSingleCall )
            {
            return;
            }
        //Check that the connected call is capable to make handover (ie. it is
        //PS call and the direction of HO is PS to CS and vica verse).
	    for( TInt i = 0; i < iPerfArray.Count(); i++ )
	    	{
	    	//call is connected
	    	if( iPerfArray[i]->PrimaryCall()->State() == MCCPCallObserver::ECCPStateConnected )
	    		{
	    		//cs call and the direction is ps to cs. no ho, return
	    		if( iPerfArray[i]->PrimaryCall()->Parameters().CallType() ==
	    		        CCPCall::ECallTypeCSVoice &&
	    		        ( aValue == EVccManualStartPsToCsHoRequest 
	    				|| aValue == EVccAutomaticStartPsToCsHoRequest ) )
	    			{
	    			iHoKeyValue = KErrNotFound;
	    			return;
	    			}
	    		//ps call and the direction is cs to ps. no ho, return
	    		else if( iPerfArray[i]->PrimaryCall()->Parameters().CallType() ==
	    		        CCPCall::ECallTypePS && 
						( aValue == EVccManualStartCsToPsHoRequest 
	    				|| aValue == EVccAutomaticStartCsToPsHoRequest ) )
	    			{
	    			iHoKeyValue = KErrNotFound;
	    			return;
	    			}
	    		}
	    	}
	    //It is now ok to hangup/reject non-active calls and make handover.
        for( TInt i = 0; i < iPerfArray.Count(); i++)
            {
            if ( (iPerfArray[i]->PrimaryCall()->State() == MCCPCallObserver::ECCPStateRinging) ||
                  iPerfArray[i]->PrimaryCall()->State() == MCCPCallObserver::ECCPStateConnecting   )
                {
                RUBY_DEBUG0( "CVccDirector::SwitchL - Reject");
                User::LeaveIfError( iPerfArray[i]->Reject() );
                iHoKeyValue = aValue;
                break;
                }
            
            else if (iPerfArray[i]->PrimaryCall()->State() != MCCPCallObserver::ECCPStateConnected)
                {
                RUBY_DEBUG0( "CVccDirector::SwitchL - Hangup");
                User::LeaveIfError( iPerfArray[i]->HangUp() );
                iHoKeyValue = aValue;
                break;
                }
            } //for             
        } //if
    else
        {
        iPerfArray[0]->SwitchL(aValue);
        iHoKeyValue = KErrNotFound;

        }//else
    }

// -----------------------------------------------------------------------------
// Checks are the plugins vcc uses initialised
// -----------------------------------------------------------------------------
//
TBool CVccDirector::IsPluginInitialized()
	{
	RUBY_DEBUG_BLOCK( "CVccDirector::IsPluginInitialized" );
	
	if( iProviders.Count() == 0 )
		return EFalse;
		
	TBool initialised = ETrue;
	for( TInt i = 0; i < iProviders.Count() ; i++)
		{
		TUid pluginUid = iProviders[ i ]->Uid();
		
		TInt found = iInitialisedPlugins.Find( pluginUid.iUid );
		if( found == KErrNotFound )	
			{
			initialised = EFalse;
			break;
			}
		}
	
	RUBY_DEBUG1( "-- IsPluginInitialised:%d", initialised );
	return initialised;
	}

// -----------------------------------------------------------------------------
// Starts settings provider table observer.
// Director is notified about the change in VCC settings. 
// -----------------------------------------------------------------------------
//
void CVccDirector::StartSpNotifierL()
	{
	RUBY_DEBUG_BLOCKL( "CVccDirector::StartSpNotifierL" );
	__ASSERT_DEBUG(iSpNotifier, User::Leave( KErrNotFound ));
	

	RIdArray array;
	CleanupClosePushL( array );

	
	iSpNotifier->NotifyChangeL( array );
	
	CleanupStack::PopAndDestroy( &array );
	}

// -----------------------------------------------------------------------------
// Stops settings provider table observer.
// -----------------------------------------------------------------------------
//
void CVccDirector::StopSpNotifierL()
	{
	RUBY_DEBUG_BLOCK( "CVccDirector::StopSpNotifierL" );
	__ASSERT_DEBUG( iSpNotifier, User::Leave( KErrNotFound ) );
	iSpNotifier->NotifyChangeCancel();
	}

// -----------------------------------------------------------------------------
// Tries to initialize the call provider plugins that were not 
// initialized the first time because of missing settings.
// -----------------------------------------------------------------------------
//
void CVccDirector::RetryInitialization()
	{
	RUBY_DEBUG_BLOCK( "CVccDirector::ReTryInitializationL" );
	Initialize( *this, *iCCPSsObserver );
	}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVccDirector::MoCallCreated( MCCPCall& aCall )
    {
    RUBY_DEBUG_BLOCK( "CVccDirector::MoCallCreated" );
    
    RUBY_DEBUG1( "CVccDirector::MoCallCreated - array count: %d", iPerfArray.Count() );
    TBool csOriginated = EFalse;
    if( aCall.Parameters().CallType() == CCPCall::ECallTypeCSVoice )
        {
        csOriginated = ETrue;
        iHoTrigger->ReadHoAllowedWhenCsOriginatedSettingL();
            
        }
    //Create 1 performer for each call
        
    CVccPerformer* perf = CVccPerformer::NewL( iProviders, 
                                               iStateInit, 
                                               *iHoTrigger,
                                               csOriginated );
    CleanupStack::PushL( perf );
    User::LeaveIfError( iPerfArray.Append( perf ) );
    CleanupStack::Pop( perf ); //perf
    perf->MoCall( &aCall );
    iCCPObserver->MoCallCreated( *perf );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVccDirector::ConferenceCallCreated( MCCPConferenceCall& aConferenceCall )
    {
    RUBY_DEBUG_BLOCK( "CVccDirector::ConferenceCallCreated" );
    if ( !iConference )
        {
        TInt err( KErrNone );
        TRAP( err, CreateConferenceL( aConferenceCall ) );
        if( KErrNone == err )
           {
           TRAP_IGNORE( iHoTrigger->HoNotAllowedL() );
           iCCPObserver->ConferenceCallCreated( *iConference );  
           }
        else
           {
           RUBY_DEBUG1( "CVccDirector::Conference call creation failed - error: %d", err );
           }
        }
    }

// -----------------------------------------------------------------------------
// CVccDirector::CreateConferenceL
//
// -----------------------------------------------------------------------------
//
void CVccDirector::CreateConferenceL( MCCPConferenceCall& aConferenceCall )
    {
    RUBY_DEBUG_BLOCK( "CVccDirector::CreateConferenceL" );
    iConference = CVccConferenceCall::NewL( aConferenceCall, iPerfArray );
    aConferenceCall.AddObserverL( *iConference );
    }
