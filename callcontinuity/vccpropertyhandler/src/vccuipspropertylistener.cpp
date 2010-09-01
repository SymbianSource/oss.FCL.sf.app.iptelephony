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
* Description:   Observes the state of a Vcc P&S property
*
*/


#include <e32cmn.h>
#include <e32debug.h>
#include <vccsubscribekeys.h>

#include "vccuipspropertylistener.h"
#include "rubydebug.h"

// ======== MEMBER FUNCTIONS ========
// -----------------------------------------------------------------------------
// C++ constructor
// -----------------------------------------------------------------------------
//
CVccUiPsPropertyListener::CVccUiPsPropertyListener( const TUint aKeyId )
    :CActive( EPriorityStandard ),
     iCategoryId( KPSVccPropertyCategory ),
     iKeyId( aKeyId )
    {
    }

// -----------------------------------------------------------------------------
// 2nd phase constructor
// -----------------------------------------------------------------------------
//
void CVccUiPsPropertyListener::ConstructL()
    {
    RUBY_DEBUG_BLOCKL( "CVccUiPsPropertyListener::ConstructL" );
    // Attach to categroy and key.
    User::LeaveIfError( iProperty.Attach( iCategoryId, iKeyId ) );
    // Add to active scheduler
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CVccUiPsPropertyListener* CVccUiPsPropertyListener::NewL(
    const TUint aKeyId )
    {
    RUBY_DEBUG_BLOCKL( "CVccUiPsPropertyListener::NewL" );

    CVccUiPsPropertyListener* self = new ( ELeave ) CVccUiPsPropertyListener(
                                                    aKeyId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CVccUiPsPropertyListener::~CVccUiPsPropertyListener()
    {
    RUBY_DEBUG0( "CVccUiPsPropertyListener::~CVccUiPsPropertyListener() - ENTER" );
    Cancel();
    iProperty.Close();
    iObservers.Close();
    RUBY_DEBUG0( "CVccUiPsPropertyListener::~CVccUiPsPropertyListener() - EXIT" );
    }

// -----------------------------------------------------------------------------
// Adds a Vcc property observer to the list of observers
// -----------------------------------------------------------------------------
//
EXPORT_C void CVccUiPsPropertyListener::AddObserverL(
    MVccPsPropertyListenerObserver& aObserver )
    {
    RUBY_DEBUG_BLOCKL( "CVccUiPsPropertyListener::AddObserverL" );

    if ( iObservers.Find( &aObserver ) == KErrNotFound )
        {
        User::LeaveIfError( iObservers.Append( &aObserver ) );
        }
    
    // And then start. Start() do not start if there are no observers
    // so we must also try to start here after adding an observer
    // (if somebody has called Start() before this method).
    Start();
    }

// -----------------------------------------------------------------------------
// Removes observer
// -----------------------------------------------------------------------------
//
EXPORT_C void CVccUiPsPropertyListener::RemoveObserver(
    MVccPsPropertyListenerObserver& aObserver )
    {
    RUBY_DEBUG_BLOCK( "CVccUiPsPropertyListener::RemoveObserver" );
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
EXPORT_C TInt CVccUiPsPropertyListener::CurrentValue()
    {
    RUBY_DEBUG_BLOCK( "CVccUiPsPropertyListener::CurrentValue" );
    TInt value( KErrNotFound );
    iProperty.Get( iCategoryId, iKeyId, value );
    return value;
    }

// -----------------------------------------------------------------------------
// Subscribes to property
// -----------------------------------------------------------------------------
//
EXPORT_C void CVccUiPsPropertyListener::Start()
    {
    RUBY_DEBUG_BLOCK( "CVccUiPsPropertyListener::Start" );

    // If no obsevers, do not start.
    if ( iObservers.Count() == 0 )
        {
        return;
        }
    
    // Subscribe to get updates.
    if ( !IsActive() )
        {
        iProperty.Subscribe( iStatus );       
        SetActive();
        }
    }

// -----------------------------------------------------------------------------
// Cancels the outstanding subsription request
// -----------------------------------------------------------------------------
//
void CVccUiPsPropertyListener::DoCancel()
    {
    RUBY_DEBUG0( "CVccUiPsPropertyListener::DoCancel - ENTER" );
    iProperty.Cancel();
    RUBY_DEBUG0( "CVccUiPsPropertyListener::DoCancel - EXIT" );
    }

// -----------------------------------------------------------------------------
// Handles the request completion event
// -----------------------------------------------------------------------------
//
void CVccUiPsPropertyListener::RunL()
    {
    RUBY_DEBUG_BLOCKL( "CVccUiPsPropertyListener::RunL" );
    TInt err = iStatus.Int();

    // Re-issue the subscription request if there are observers.
    // Do this before notifying our observser so that we get all
    // notifications (if the rest of the RunL takes some time).
    
    if ( iObservers.Count() > 0 )
        {
        Start();
        }
     
    if ( KErrNone == err )
        {
        // Notify property changes to observers.
        NotifyObserversL();
        }    
     }

// -----------------------------------------------------------------------------
// Notifies all observers about the changed value
// -----------------------------------------------------------------------------
//
void CVccUiPsPropertyListener::NotifyObserversL()
    {
    RUBY_DEBUG_BLOCKL( "CVccUiPsPropertyListener::NotifyObserversL" );

    TInt value = CurrentValue();

    RUBY_DEBUG1( " -value[%d]", value);

    for ( TInt i(0) ; i < iObservers.Count() ; i++ )
        {
        MVccPsPropertyListenerObserver* observer = iObservers[i];

        if ( observer )
            {
            observer->VccPropertyChangedL( iKeyId, value );
            }
        }
    }
