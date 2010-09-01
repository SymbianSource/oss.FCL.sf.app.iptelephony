/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Observes the state of P&S property
*
*/



#include <e32cmn.h>
#include <vccsubscribekeys.h>

#include "vccengpspropertylistener.h"
#include "rubydebug.h"

//#include "vccuipropertyhandler.pan"

// ======== MEMBER FUNCTIONS ========
// -----------------------------------------------------------------------------
// C++ constructor
// -----------------------------------------------------------------------------
//
CVccEngPsPropertyListener::CVccEngPsPropertyListener( 
const TUid aCategoryUid,
		const TUint aKeyId ):
CActive( EPriorityStandard ),
iCategoryId( aCategoryUid ),
iKeyId( aKeyId )
	{
	RUBY_DEBUG_BLOCK( "VccEngPsPropertyListener::CVccEngPsPropertyListener" );
	}

// -----------------------------------------------------------------------------
// 2nd phase constructor
// -----------------------------------------------------------------------------
//
void CVccEngPsPropertyListener::ConstructL()
    {
    RUBY_DEBUG_BLOCKL( "CVccEngPsPropertyListener::ConstructL" );
    // Attach to categroy and key.
    User::LeaveIfError( iProperty.Attach( iCategoryId, iKeyId ));
    // Add to active scheduler
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CVccEngPsPropertyListener* CVccEngPsPropertyListener::NewL(
		                                                 const TUid aCategoryId, 
		                                                 const TUint aKeyId)   
    {
	RUBY_DEBUG_BLOCKL("CVccEngPsPropertyListener::NewL");
   
	CVccEngPsPropertyListener* self = new (ELeave) CVccEngPsPropertyListener( 
			                                              aCategoryId, aKeyId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
   
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CVccEngPsPropertyListener::~CVccEngPsPropertyListener()
    {
    RUBY_DEBUG0( "CVccEngPsPropertyListener::~CVccEngPsPropertyListener() - ENTER" );
    Cancel();
    iProperty.Close();
    iObservers.Close();
    RUBY_DEBUG0( "CVccEngPsPropertyListener::~CVccEngPsPropertyListener() - EXIT" );
    }

// -----------------------------------------------------------------------------
// Add Vcc property observer
// -----------------------------------------------------------------------------
//
void CVccEngPsPropertyListener::AddObserverL( 
        MVccEngPsPropertyListenerObserver& aObserver )
    {
    RUBY_DEBUG_BLOCKL( "CVccEngPsPropertyListener::AddObserverL" );
    
    if ( iObservers.Find( &aObserver ) == KErrNotFound )
        {
        User::LeaveIfError( iObservers.Append( &aObserver ) );
        }
    }

// -----------------------------------------------------------------------------
// Removes observer
// -----------------------------------------------------------------------------
//
void CVccEngPsPropertyListener::RemoveObserver( 
        MVccEngPsPropertyListenerObserver& aObserver )
    {
    RUBY_DEBUG_BLOCK( "CVccEngPsPropertyListener::RemoveObserver" );
   
    TInt index = iObservers.Find( &aObserver );
    
    if ( index != KErrNotFound )
        {
        iObservers.Remove( index );
        }
    }

// -----------------------------------------------------------------------------
// Fetches the current value of the key
// -----------------------------------------------------------------------------
//
TInt CVccEngPsPropertyListener::CurrentValue()
    {
    RUBY_DEBUG_BLOCK( "CVccEngPsPropertyListener::CurrentValue" );
   
    TInt value( KErrNotFound );
    iProperty.Get( iCategoryId, iKeyId, value );
    return value;
    }


// -----------------------------------------------------------------------------
// Cancels the outstanding subsription request
// -----------------------------------------------------------------------------
//
void CVccEngPsPropertyListener::DoCancel()
    {
    RUBY_DEBUG0( "CVccEngPsPropertyListener::DoCancel - ENTER" );
    iProperty.Cancel();
    RUBY_DEBUG0( "CVccEngPsPropertyListener::DoCancel - EXIT" );
    }

// -----------------------------------------------------------------------------
// Subsribes to property
// -----------------------------------------------------------------------------
//
void CVccEngPsPropertyListener::Start()
    {
    RUBY_DEBUG_BLOCK( "CVccEngPsPropertyListener::Start" );
   
    // Subscribe to get updates.
    iProperty.Subscribe( iStatus );
    SetActive();
    }

// -----------------------------------------------------------------------------
// RunL
// -----------------------------------------------------------------------------
//
void CVccEngPsPropertyListener::RunL()
    {
    TInt err = iStatus.Int();
Start();
    RUBY_DEBUG_BLOCK( "CVccEngPsPropertyListener::RunL" );
    RUBY_DEBUG1( " - Status: %d", iStatus.Int() );
    if ( KErrNone == err )
        {
        RUBY_DEBUG0( " - CVccEngPsPropertyListener::RunL - \
                     if ( KErrNone == err )" );
        // Notify property changes to observers.
        NotifyObserversL();
        }
    }

// -----------------------------------------------------------------------------
// Notify all observers about the changed value
// -----------------------------------------------------------------------------
//
void CVccEngPsPropertyListener::NotifyObserversL()
    {
    RUBY_DEBUG_BLOCKL( "CVccEngPsPropertyListener::NotifyObserversL" );
   
    TInt value = CurrentValue();
    RUBY_DEBUG1( " - value [%d]", value );
   
    for ( TInt i(0) ; i < iObservers.Count() ; i++ )
        {
        MVccEngPsPropertyListenerObserver* observer = iObservers[i];

        if ( observer )
            {
            observer->PropertyChangedL( iCategoryId, iKeyId, value );
            }
        }
    }
