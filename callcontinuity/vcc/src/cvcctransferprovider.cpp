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
* Description:   Takes of call transfer 
*                (forwards call transfer requests to SVP or CS transfer 
*                provider 
*
*/



#include "cvcctransferprovider.h"
#include "cvccperformer.h"
#include "cvccdirector.h" // KVCCImplementationUid
#include "rubydebug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Symbian constructor
// ---------------------------------------------------------------------------
//
CVccTransferProvider* CVccTransferProvider::NewL( CVccPerformer* aCall )
	{
  RUBY_DEBUG_BLOCK( "CVccTransferProvider::NewL" );
	
	CVccTransferProvider* self;
	self = new( ELeave ) CVccTransferProvider( aCall );
	CleanupStack::PushL( self );
	self->ConstructL( );
	CleanupStack::Pop( self );
	return self;
	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CVccTransferProvider::~CVccTransferProvider()
	{
	iObservers.Close();
	}

// ---------------------------------------------------------------------------
// CVccTransferProvider::CVccTransferProvider
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CVccTransferProvider::CVccTransferProvider(CVccPerformer* aCall )
    : iPerfCall( aCall )
    {
    RUBY_DEBUG_BLOCK( "CVccDtmfProvider::CVccDtmfProvider" );		
    
    
    }
		
// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CVccTransferProvider::ConstructL( )
	 {
     RUBY_DEBUG_BLOCK( "CVccTransferProvider::ConstructL" );
     iCallTransferProvider = iPerfCall->PrimaryCall()->TransferProviderL(*this);
     iCallTransferProvider->AddObserverL(*this);
     }

// ---------------------------------------------------------------------------
// CVccTransferProvider::AttendedTransfer
// ---------------------------------------------------------------------------
//
TInt CVccTransferProvider::AttendedTransfer( MCCPCall& aTransferTargetCall )
    {
    RUBY_DEBUG_BLOCK( "CVccTransferProvider::AttendedTransfer" );
    MCCPCall* target = &aTransferTargetCall;
    if ( target )
        {
        if ( KVCCImplementationUid == target->Uid() )
            {
            RUBY_DEBUG0( "CVccTransferProvider::AttendedTransfer - Target call Plugin Uid matches VCC Plugin Uid" );
            CVccPerformer* targetCall = static_cast< CVccPerformer* >(&aTransferTargetCall);
            target = targetCall->PrimaryCall();
            }
        }
    return iCallTransferProvider->AttendedTransfer( *target );
    }

// ---------------------------------------------------------------------------
// CVccTransferProvider::AttendedTransfer
// ---------------------------------------------------------------------------
//
TInt CVccTransferProvider::AttendedTransfer( const TDesC& aTransferTarget )
	 {
	 RUBY_DEBUG_BLOCK( "CVccTransferProvider::AttendedTransfer" );    
	 return iCallTransferProvider->AttendedTransfer( aTransferTarget );
	 }

// ---------------------------------------------------------------------------
// CVccTransferProvider::UnattendedTransfer
// ---------------------------------------------------------------------------
//
TInt CVccTransferProvider::UnattendedTransfer( const TDesC& aTransferTarget )
	 {
	 RUBY_DEBUG_BLOCK( "CVccTransferProvider::UnattendedTransfer" );
     return iCallTransferProvider->UnattendedTransfer( aTransferTarget );
     }

// ---------------------------------------------------------------------------
// CVccTransferProvider::AcceptTransfer
// ---------------------------------------------------------------------------
//
TInt CVccTransferProvider::AcceptTransfer( const TBool aAccept )
	 {
	 RUBY_DEBUG_BLOCK( "CVccTransferProvider::AcceptTransfer" );
     return iCallTransferProvider->AcceptTransfer( aAccept );
     
     }

// ---------------------------------------------------------------------------
// CVccTransferProvider::TransferTarget()
// ---------------------------------------------------------------------------
//
const TDesC& CVccTransferProvider::TransferTarget() const
	 {
	 RUBY_DEBUG_BLOCK( "CVccTransferProvider::TransferTarget" );
	 return iCallTransferProvider->TransferTarget( );
	 }

// ---------------------------------------------------------------------------
// CVccTransferProvider::AddObserverL
// ---------------------------------------------------------------------------
//
void CVccTransferProvider::AddObserverL( const MCCPTransferObserver& aObserver )
	 {
	 RUBY_DEBUG_BLOCK( "CVccTransferProvider::AddObserverL" );
	 iObservers.InsertInAddressOrderL( &aObserver );
	 }

// ---------------------------------------------------------------------------
// CVccTransferProvider::RemoveObserver
// ---------------------------------------------------------------------------
//
TInt CVccTransferProvider::RemoveObserver( const MCCPTransferObserver& aObserver )
	 {
	 RUBY_DEBUG_BLOCK( "CVccTransferProvider::RemoveObserver" );
	 TInt res = iObservers.Find( &aObserver );
	    if ( res != KErrNotFound )
	        {
	        iObservers.Remove( res );
	        res = KErrNone;
	        }
	 return res;	
     }

// ---------------------------------------------------------------------------
// CVccTransferProvider::TransferEventOccurred
// ---------------------------------------------------------------------------
//
void CVccTransferProvider::TransferEventOccurred( 
                const MCCPTransferObserver::TCCPTransferEvent aEvent )
    {
    RUBY_DEBUG_BLOCK( "CVccTransferProvider::TransferEventOccurred" );
    for ( TInt i = 0; i < iObservers.Count(); i++ )
       {
       iObservers[i]->TransferEventOccurred( aEvent );
       }   

    }

// ---------------------------------------------------------------------------
// CVccTransferProvider::UpdateL
// ---------------------------------------------------------------------------
//
void CVccTransferProvider::UpdateL()
    {
    RUBY_DEBUG_BLOCK( "CVccTransferProvider::UpdateL" );
    iCallTransferProvider->RemoveObserver(*this);
    iCallTransferProvider = iPerfCall->PrimaryCall()->TransferProviderL(*this);
    iCallTransferProvider->AddObserverL(*this);
    }

