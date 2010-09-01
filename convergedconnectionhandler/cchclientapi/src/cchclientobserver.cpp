/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Cch client api
*
*/


// INCLUDE FILES
#include "cchimpl.h"
#include "cchclientobserver.h"
#include "cchlogger.h"
#include "cchserviceimpl.h"
// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES
// None

// CONSTANTS
// None

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ============================= LOCAL FUNCTIONS =============================

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// CCchClientObserver::CCchClientObserver
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CCchClientObserver::CCchClientObserver( CCchServiceImpl& aCchService )
: CActive ( EPriorityStandard )
, iCchService( aCchService )
, iBackwardSupportObserver( NULL )
    {
    CActiveScheduler::Add( this );
    }
    
CCchClientObserver::~CCchClientObserver()
    {
    iObservers.Reset();

    Cancel();
    }

// ---------------------------------------------------------------------------
// CCchClientObserver::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CCchClientObserver::ConstructL()
    {
    CCHLOGSTRING( "CCchClientObserver::ConstructL: IN" );

    CCHLOGSTRING( "CCchClientObserver::ConstructL: OUT" );
    }
// ---------------------------------------------------------------------------
// CCchClientObserver::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CCchClientObserver* CCchClientObserver::NewL( CCchServiceImpl& aCchService )
    {
    CCHLOGSTRING( "CCchClientObserver::NewL: IN" );
    CCchClientObserver* self = 
        CCchClientObserver::NewLC( aCchService );
    CleanupStack::Pop( self );
    CCHLOGSTRING( "CCchClientObserver::NewL: OUT" );
    return self;
    }

// ---------------------------------------------------------------------------
// CCchClientObserver::NewLC
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CCchClientObserver* CCchClientObserver::NewLC( CCchServiceImpl& aCchService )
    {
    CCHLOGSTRING( "CCchClientObserver::NewLC: IN" );
    CCchClientObserver* self = 
        new (ELeave) CCchClientObserver( aCchService );
    CleanupStack::PushL( self );
    self->ConstructL();
    CCHLOGSTRING( "CCchClientObserver::NewLC: OUT" );
    return self;
    }

// ---------------------------------------------------------------------------
// CCchClientObserver::SetObserver
// ---------------------------------------------------------------------------
//
void CCchClientObserver::SetObserver( MCchServiceStatusObserver& aObserver )
    {
    CCHLOGSTRING( "CCchClientObserver::SetObserver: IN - deprecated use AddObserver" );
    
    // This is for old API user support remove this when all are using new API
    if( iBackwardSupportObserver )
        {
        // remove existing observer
        CCHLOGSTRING( "CCchClientObserver::SetObserver: removing old observer" );
        RemoveObserver();
        }
    
    CCHLOGSTRING( "CCchClientObserver::SetObserver: adding new observer" );
    // This is for old API user support remove this when all are using new API
    AddObserver( aObserver );
    iBackwardSupportObserver = &aObserver;

    CCHLOGSTRING( "CCchClientObserver::SetObserver: OUT" );
    }
    
// ---------------------------------------------------------------------------
// CCchClientObserver::RemoveObserver
// ---------------------------------------------------------------------------
//
void CCchClientObserver::RemoveObserver( )
    {    
    CCHLOGSTRING( "CCchClientObserver::RemoveObserver: IN - deprecated use the one with parameter" );
    
    if( iBackwardSupportObserver )
    	{
    	// remove using observer set in SetObserver. This is for old API user support
    	// remove this when all are using new API
    	RemoveObserver( *iBackwardSupportObserver );
    	iBackwardSupportObserver = NULL;
    	}
    
    CCHLOGSTRING( "CCchClientObserver::RemoveObserver: OUT" );
    }

// ---------------------------------------------------------------------------
// CCchClientObserver::AddObserver
// ---------------------------------------------------------------------------
//
TInt CCchClientObserver::AddObserver( 
    MCchServiceStatusObserver& aObserver )
    {
    CCHLOGSTRING2( "CCchClientObserver::AddObserver: IN count=%d", iObservers.Count() );
    
    TInt index = iObservers.Find( &aObserver );

    // Insert if not already observing
    if ( index == KErrNotFound )
        {
        CCHLOGSTRING( "CCchClientObserver::AddObserver: observer added" );
        iObservers.Append( &aObserver );
        }     
    else
        {
        CCHLOGSTRING( "CCchClientObserver::AddObserver: observer KErrAlreadyExists" );
        return KErrAlreadyExists;   
        }

    if ( iObservers.Count() )
        {
        if ( !IsActive() )
            {
            iCchService.CchImpl()->CchClient().SubscribeToEvents( 
                    TServiceSelection( iCchService.ServiceId(), ECCHUnknown ), 
                    iServiceStatus, 
                    iStatus );
            
            CCHLOGSTRING( "CCchClientObserver::AddObserver: setting active" );
            
            SetActive();
            }
        
        CCHLOGSTRING( "CCchClientObserver::AddObserver: subscrbing to events"  );
      
        }
    else
        {
        CCHLOGSTRING( "CCchClientObserver::AddObserver: already subscribed subscrbing to events - skipping"  );
        }
    
    CCHLOGSTRING2( "CCchClientObserver::AddObserver: OUT count=%d", iObservers.Count() );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CCchClientObserver::RemoveObserver
// ---------------------------------------------------------------------------
//
TInt CCchClientObserver::RemoveObserver( 
    MCchServiceStatusObserver& aObserver )
    {
    CCHLOGSTRING( "CCchClientObserver::RemoveObserver: IN" );
    CCHLOGSTRING2( "CCchClientObserver::RemoveObserver IN: COUNT=%d", iObservers.Count() );
    
    TInt index = iObservers.Find( &aObserver );
    
    if ( index == KErrNotFound )
        {
        CCHLOGSTRING("CCchClientObserver::RemoveObserver - observer NOT found KErnotFound returned" );
        return KErrNotFound;   
        }
    else
        {
        CCHLOGSTRING( "CCchClientObserver::RemoveObserver: removing observer"  );
        
        // Observer found.
        iObservers.Remove( index );
        iObservers.Compress();
        
        // Cancel subcribe to events and free up memory.
        if ( !iObservers.Count() )
            {
            CCHLOGSTRING( "CCchClientObserver::RemoveObserver: unsubscrbing from events"  );                  
            iCchService.CchImpl()->CchClient().SubscribeToEventsCancel( );      
            iObservers.Reset();
            }
        }

    CCHLOGSTRING2( "CCchClientObserver::RemoveObserver OUT: COUNT=%d", iObservers.Count() );
    
    CCHLOGSTRING( "CCchClientObserver::RemoveObserver: OUT" );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CCchClientObserver::DoCancel
// ---------------------------------------------------------------------------
//
void CCchClientObserver::DoCancel()
    {
    CCHLOGSTRING( "CCchClientObserver::DoCancel: IN" );
	
	CCHLOGSTRING( "CCchClientObserver::DoCancel: OUT" );
    }

// ---------------------------------------------------------------------------
// CCchClientObserver::RunL
// ---------------------------------------------------------------------------
//
void CCchClientObserver::RunL()
    {
    TServiceStatus serviceStatus = iServiceStatus();
    CCHLOGSTRING( "CCchClientObserver::RunL: IN" );

    //if someone is observing and correct service is changed
    if ( iObservers.Count() && ( serviceStatus.ServiceId() == iCchService.ServiceId() ) )
    	{
    	TCchServiceStatus status;
    	
    	status.SetError( serviceStatus.iError );
    	status.SetState( serviceStatus.iState );

    	CCHLOGSTRING2( "CCchClientObserver::RunL: count=%d", iObservers.Count() );
    	
    	// If observer array can change while iterating it add support for that.
    	// Then there is a need to iterate starting from beginning until done and save 
    	// already notified observers etc
    	TInt count( iObservers.Count() );
    	
 	    for( TInt i=0; i < count; i++ )
            {
            CCHLOGSTRING2( "CCchClientObserver::RunL: looping observers: %d", i );
            
            // If some client removes observer in middle of iteration start from beginning of array again.
            if ( count > iObservers.Count() )
                {
                CCHLOGSTRING( "CCchClientObserver::RunL: Start from beginning of array" );
                
                i = 0;
                count = iObservers.Count();
                }
            
            if( iObservers[ i ] )
                {
                CCHLOGSTRING2( "CCchClientObserver::RunL: send service status changed to client: %d", i );
                iObservers[ i ]->ServiceStatusChanged( iCchService.ServiceId(), 
                    serviceStatus.Type(), status );
                }
            }
        }
    
    if ( iObservers.Count() )
        {
        if ( !IsActive() )
            {
            CCHLOGSTRING( "CCchClientObserver::RunL: Subscribing " );
            iCchService.CchImpl()->CchClient().SubscribeToEvents( 
                    TServiceSelection( iCchService.ServiceId(), ECCHUnknown ),
                    iServiceStatus,
                    iStatus );
            
            CCHLOGSTRING( "CCchClientObserver::RunL: Setting active" );
            SetActive();    
           }
        } 


    CCHLOGSTRING( "CCchClientObserver::RunL: OUT" );
    }

// ---------------------------------------------------------------------------
// CCchClientObserver::RunError
// ---------------------------------------------------------------------------
//
TInt CCchClientObserver::RunError( TInt /*aError*/ )
    {
    CCHLOGSTRING( "CCchClientObserver::RunError: IN OUT" );
    return KErrNone;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
