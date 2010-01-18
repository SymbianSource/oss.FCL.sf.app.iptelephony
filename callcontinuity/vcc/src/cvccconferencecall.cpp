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
* Description:   Base class of VCC conference call object
*
*/

#include <mccpcscall.h> //CCP API

#include "cvccconferencecall.h"
#include "cvccperformer.h"
#include "cconvergedcallprovider.h"
#include "rubydebug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C'tor
// ---------------------------------------------------------------------------
//
CVccConferenceCall::CVccConferenceCall( const TUint32 aServiceId,
                                   const MCCPConferenceCallObserver& aObserver,
                                   RVccCallArray& aCallArray )
: iServiceId( aServiceId ), 
  iObserver( const_cast<MCCPConferenceCallObserver*>( &aObserver ) ),
  iCallArray( aCallArray )
    {
    RUBY_DEBUG_BLOCK("CVccConferenceCall::CVccConferenceCall");
    }

// ---------------------------------------------------------------------------
// C'tor
// ---------------------------------------------------------------------------
//
CVccConferenceCall::CVccConferenceCall( MCCPConferenceCall& aConferenceCall,
                                   RVccCallArray& aCallArray )
: iServiceId( aConferenceCall.ServiceId() ), 
  iCallArray( aCallArray )
    {
    RUBY_DEBUG_BLOCK("CVccConferenceCall::CVccConferenceCall");
    RUBY_DEBUG0("CVccConferenceCall::CVccConferenceCall2");
    iConferenceCall = &aConferenceCall;
    // Get reference to CS plugin
    CConvergedCallProvider& csProv = iCallArray[0]->CsProvider();
    iCsProvider = &csProv;
    }
// ---------------------------------------------------------------------------
// 2nd phase C'tor
// ---------------------------------------------------------------------------
//
void CVccConferenceCall::ConstructL()
    {
    RUBY_DEBUG_BLOCKL( "CVccConferenceCall::ConstructL" );
    
    __ASSERT_ALWAYS( iCallArray.Count() > 0, User::Leave( KErrArgument ) );
    
    // Get reference to CS plugin
    CConvergedCallProvider& csProv = iCallArray[0]->CsProvider();
    iCsProvider = &csProv;
    
    // Order CS plugin to create new conference
    iConferenceCall = iCsProvider->NewConferenceL( iServiceId, *this );
    }

// ---------------------------------------------------------------------------
// Two phased C'tor
// ---------------------------------------------------------------------------
//
CVccConferenceCall* CVccConferenceCall::NewL( const TUint32 aServiceId,
                                   const MCCPConferenceCallObserver& aObserver,
                                   RVccCallArray& aCallArray )
    {
    CVccConferenceCall* self = new( ELeave ) CVccConferenceCall( aServiceId,
                                                                 aObserver,
                                                                 aCallArray );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Two phased C'tor
// ---------------------------------------------------------------------------
//
CVccConferenceCall* CVccConferenceCall::NewL( MCCPConferenceCall& aConferenceCall, 
                                    RVccCallArray& aCallArray )
    {
    RUBY_DEBUG0("CVccConferenceCall::NewL2");
        
    CVccConferenceCall* self = new( ELeave ) CVccConferenceCall( 
                                                aConferenceCall,
                                                aCallArray );
    
    return self;
    }

// ---------------------------------------------------------------------------
// D'tor
// ---------------------------------------------------------------------------
//
CVccConferenceCall::~CVccConferenceCall()
    {
    RUBY_DEBUG0( "CVccConferenceCall::~CVccConferenceCall() - ENTER" );
    
    RUBY_DEBUG0( "CVccConferenceCall::~CVccConferenceCall() - EXIT" );
    }

// ---------------------------------------------------------------------------
// Signals CS plugin that conference is released
// ---------------------------------------------------------------------------
//
void CVccConferenceCall::ReleaseConference()
    {
    RUBY_DEBUG_BLOCK( "CVccConferenceCall::ReleaseConference" );
    iCsProvider->ReleaseConferenceCall( *iConferenceCall );
    }

// ---------------------------------------------------------------------------
//  @see MCCPConferenceCall::ServiceId 
// ---------------------------------------------------------------------------
//
TUint32 CVccConferenceCall::ServiceId() const
    {
    RUBY_DEBUG_BLOCK("CVccConferenceCall::ServiceId");
    
    return iServiceId;
    }

// ---------------------------------------------------------------------------
//  @see MCCPConferenceCall::HangUp 
// ---------------------------------------------------------------------------
//
TInt CVccConferenceCall::HangUp()
    {
    RUBY_DEBUG_BLOCK( "CVccConferenceCall::HangUp" );
    
    TInt retValue( iConferenceCall->HangUp() );
    
    RUBY_DEBUG1( "Error: %d", retValue );
    
    return retValue;
    }

// ---------------------------------------------------------------------------
//  @see MCCPConferenceCall::Hold 
// ---------------------------------------------------------------------------
//
TInt CVccConferenceCall::Hold()
    {
    RUBY_DEBUG_BLOCK( "CVccConferenceCall::Hold" );

    TInt retValue( iConferenceCall->Hold() );
    
    RUBY_DEBUG1( "Error: %d", retValue );
    
    return retValue;
    }

   
// ---------------------------------------------------------------------------
//  @see MCCPConferenceCall::Resume 
// ---------------------------------------------------------------------------
//
TInt CVccConferenceCall::Resume()
    {
    RUBY_DEBUG_BLOCK( "CVccConferenceCall::Resume" );
    
    TInt retValue(  iConferenceCall->Resume() );
    
    RUBY_DEBUG1( "Error: %d", retValue );
    
    return retValue;
    }


// ---------------------------------------------------------------------------
//  @see MCCPConferenceCall::Swap 
// ---------------------------------------------------------------------------
//
TInt CVccConferenceCall::Swap()
    {
    RUBY_DEBUG_BLOCK( "CVccConferenceCall::Swap" );
    
    TInt retValue( iConferenceCall->Swap() );
    
    RUBY_DEBUG1( "Error: %d", retValue );
    
    return retValue;
    }


// ---------------------------------------------------------------------------
//  @see MCCPConferenceCall::AddCallL 
// ---------------------------------------------------------------------------
//
void CVccConferenceCall::AddCallL( MCCPCall* aCall )
    {
    RUBY_DEBUG_BLOCK( "CVccConferenceCall::AddCallL" );

    // NULL pointer
    __ASSERT_ALWAYS( aCall, User::Leave( KErrArgument ) );
    RUBY_DEBUG0( "- Not a NULL pointer" );
    
    CVccPerformer* call = static_cast<CVccPerformer*>( aCall );

    // Tell CS plugin to add the call in conference and add it to the
    // local conference array
    iConferenceCall->AddCallL( call->PrimaryCall() );
        
    }


// ---------------------------------------------------------------------------
//  @see MCCPConferenceCall::RemoveCallL 
// ---------------------------------------------------------------------------
//
void CVccConferenceCall::RemoveCallL( MCCPCall* aCall )
    {
    RUBY_DEBUG_BLOCK( "CVccConferenceCall::RemoveCallL" );
    
    // NULL pointer
    __ASSERT_ALWAYS( aCall, User::Leave( KErrArgument ) );
    RUBY_DEBUG0( "- Not a NULL pointer" );
    
    CVccPerformer* call = static_cast<CVccPerformer*>( aCall );

    // Tell CS plugin to remeove call from conference
    iConferenceCall->RemoveCallL( call->PrimaryCall() );
    
    }

// ---------------------------------------------------------------------------
//  @see MCCPConferenceCall::CallCount 
// ---------------------------------------------------------------------------
//
TInt CVccConferenceCall::CallCount() const
    {
    RUBY_DEBUG_BLOCK( "CVccConferenceCall::CallCount" );
    
    return iConferenceCall->CallCount();
    }


// ---------------------------------------------------------------------------
//  @see MCCPConferenceCall::GoOneToOneL 
// ---------------------------------------------------------------------------
//
void CVccConferenceCall::GoOneToOneL( MCCPCall& aCall )
    {
    RUBY_DEBUG_BLOCK( "CVccConferenceCall::GoOneToOneL" );
    
    // Find call from conference array
    CVccPerformer* call = dynamic_cast<CVccPerformer*>( &aCall );
    
    // Check that the call was found in conference array
    __ASSERT_ALWAYS( call, User::Leave( KErrNotFound ) );   
    
    // Tell CS plugin to go one-on-one
    iConferenceCall->GoOneToOneL( *call->PrimaryCall() );
    }


// ---------------------------------------------------------------------------
//  @see MCCPConferenceCall::CurrentCallsToConferenceL 
// ---------------------------------------------------------------------------
//
void CVccConferenceCall::CurrentCallsToConferenceL()
    {
    RUBY_DEBUG_BLOCK( "CVccConferenceCall::CurrentCallsToConferenceL" );
    iConferenceCall->CurrentCallsToConferenceL();
    }
  
// ---------------------------------------------------------------------------
//  @see MCCPConferenceCall::AddObserverL 
// ---------------------------------------------------------------------------
//
void CVccConferenceCall::AddObserverL( 
                                   const MCCPConferenceCallObserver& aObserver )
    {
    RUBY_DEBUG_BLOCK( "CVccConferenceCall::AddObserverL" );
    
    iObserver = const_cast<MCCPConferenceCallObserver*>( &aObserver );
    }


// ---------------------------------------------------------------------------
//  @see MCCPConferenceCall::RemoveObserver 
// ---------------------------------------------------------------------------
//
TInt CVccConferenceCall::RemoveObserver( 
                                   const MCCPConferenceCallObserver& aObserver )
    {
    RUBY_DEBUG_BLOCK( "CVccConferenceCall::RemoveObserver" );
    
    TInt retValue( KErrNone );
    
    if ( iObserver ==  &aObserver )
        {
        iObserver = NULL;
        }
    else
        {
        retValue = KErrNotFound;
        }
    
    return retValue;
    }

// ---------------------------------------------------------------------------
//  @see MCCPConferenceCallObserver::ErrorOccurred 
// ---------------------------------------------------------------------------
//
void CVccConferenceCall::ErrorOccurred( TCCPConferenceCallError aError )
    {
    RUBY_DEBUG_BLOCK( "CVccConferenceCall::ErrorOccurred" );
    RUBY_DEBUG1( "- Error code [%d]", aError );
    
    // Tell CCE level that error occurred
    iObserver->ErrorOccurred( aError );
    }


// ---------------------------------------------------------------------------
//  @see MCCPConferenceCallObserver::ConferenceCallCapsChanged 
// ---------------------------------------------------------------------------
//
void CVccConferenceCall::ConferenceCallCapsChanged( 
    const TCCPConferenceCallCaps aCaps )
    {
    RUBY_DEBUG_BLOCK( "CVccConferenceCall::ConferenceCallStateChanged()" );
    RUBY_DEBUG1( "- Caps = %d", aCaps  );
    
    iObserver->ConferenceCallCapsChanged( aCaps );
    }


// ---------------------------------------------------------------------------
//  @see MCCPConferenceCallObserver::ConferenceCallStateChanged 
// ---------------------------------------------------------------------------
//
void CVccConferenceCall::ConferenceCallStateChanged( 
    const TCCPConferenceCallState aState )
    {
    RUBY_DEBUG_BLOCK( "CVccConferenceCall::ConferenceCallStateChanged" );
    RUBY_DEBUG1( "- State = %d", aState );
    iObserver->ConferenceCallStateChanged( aState );
    }

// ---------------------------------------------------------------------------
//  @see MCCPConferenceCallObserver::ConferenceCallEventOccurred 
// ---------------------------------------------------------------------------
//
void CVccConferenceCall::ConferenceCallEventOccurred( 
     const TCCPConferenceCallEvent aEvent, MCCPCall* aReferredCall )
    {
    RUBY_DEBUG_BLOCK( "CVccConferenceCall::ConferenceCallEventOccurred" );
    iObserver->ConferenceCallEventOccurred( aEvent, 
            MatchPluginToVccCall( aReferredCall ) );
    }


// ---------------------------------------------------------------------------
// Search a specific call from call array
// ---------------------------------------------------------------------------
//
CVccPerformer* CVccConferenceCall::MatchPluginToVccCall( MCCPCall* aPluginCall )
    {
    RUBY_DEBUG_BLOCK( "CVccConferenceCall::FindInCallArray()" );
    
    CVccPerformer* call = NULL;
    
    if ( aPluginCall )
        {
        for ( TInt i = 0; i < iCallArray.Count(); i++ )
            {
            // Comparison is made to CVccPerformer and MCCPCall objects
            // for simplicity sake
            if ( iCallArray[i]->PrimaryCall() == aPluginCall )
                {
                RUBY_DEBUG0( "- Call found" );
                call = iCallArray[i];
                break;
                }
            }
        }
    
    return call;
    }

// ---------------------------------------------------------------------------
// Collect all conference participants into array
// ---------------------------------------------------------------------------
//
TInt CVccConferenceCall::GetCallArray( RPointerArray<MCCPCall>& aCallArray )
    {
    RUBY_DEBUG_BLOCK( "CVccConferenceCall::GetCallArray" );
    RPointerArray<MCCPCall> pluginCalls;
    iConferenceCall->GetCallArray( pluginCalls );
    for( TInt i = 0; i < pluginCalls.Count(); i++ )
        {
        aCallArray.Append( MatchPluginToVccCall( pluginCalls[i] ) );
        }
        
    pluginCalls.Close();    
    return KErrNone;
    }
   
