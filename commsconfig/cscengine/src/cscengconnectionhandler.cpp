/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  For handling interactions betweed UI and RConnection.
*
*/


#include <e32base.h>
#include <nifman.h>

#include "cscenglogger.h"
#include "cscengconnectionhandler.h"
#include "mcscengconnectionobserver.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCEngConnectionHandler::CCSCEngConnectionHandler( 
    MCSCEngConnectionObserver& aObserver ) : 
    CActive( EPriorityStandard ), 
    iObserver( aObserver )
    {    
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CCSCEngConnectionHandler::ConstructL()
    {
    CSCENGDEBUG( "CCSCEngConnectionHandler::ConstructL - begin" ); 
    
    CActiveScheduler::Add( this );

    iTimer = CCSCEngTimer::NewL( *this );
    
    // Open channel to Socket Server
    User::LeaveIfError( iSocketServ.Connect() );
    
    // Open connection
    User::LeaveIfError( iConnection.Open( iSocketServ ) );
    
    CSCENGDEBUG( "CCSCEngConnectionHandler::ConstructL - end" ); 
    }   
    
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CCSCEngConnectionHandler* CCSCEngConnectionHandler::NewL( 
    MCSCEngConnectionObserver& aObserver )
    {    
    CCSCEngConnectionHandler* self = 
        new ( ELeave ) CCSCEngConnectionHandler( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCEngConnectionHandler::~CCSCEngConnectionHandler()
    {
    CSCENGDEBUG( "CCSCEngConnectionHandler::~CCSCEngConnectionHandler - begin" ); 
    
    delete iTimer;
    
    Cancel();
    iConnection.Close();
    iSocketServ.Close();
    
    CSCENGDEBUG( "CCSCEngConnectionHandler::~CCSCEngConnectionHandler - end" ); 
    }

// ---------------------------------------------------------------------------
// CCSCEngConnectionHandler::StartListeningConnectionEvents
// ---------------------------------------------------------------------------
//
EXPORT_C void CCSCEngConnectionHandler::StartListeningConnectionEvents()
    {
    CSCENGDEBUG( "CCSCEngConnectionHandler::StartListeningConnectionEvents" ); 
    
    if ( IsActive() )
        {
        Cancel();
        }
    
    iTimer->StartTimer( CCSCEngTimer::EConnectionMonitoringTimer ); 
    iConnection.AllInterfaceNotification( iInfoBuf, iStatus );
    SetActive(); 
    }

// ---------------------------------------------------------------------------
// CCSCEngConnectionHandler::StopListeningConnectionEvents
// ---------------------------------------------------------------------------
//
EXPORT_C void CCSCEngConnectionHandler::StopListeningConnectionEvents()
     {
     CSCENGDEBUG( "CCSCEngConnectionHandler::StopListeningConnectionEvents" ); 
     
     iTimer->StopTimer();
     iConnection.CancelAllInterfaceNotification();
     }    

// ---------------------------------------------------------------------------
// From MCSCEngTimerObserver
// ---------------------------------------------------------------------------
//
void CCSCEngConnectionHandler::TimerExpired()
     {
     CSCENGDEBUG( "CCSCEngConnectionHandler::TimerExpired" ); 
     
     iConnection.CancelAllInterfaceNotification();
     iObserver.NotifyConnectionEvent( EEventTimedOut );
     } 
   
// ---------------------------------------------------------------------------
// CCSCEngConnectionHandler::RunL
// ---------------------------------------------------------------------------
//
void CCSCEngConnectionHandler::RunL()
    {
    if ( iStatus.Int() == KErrNone )
        {
        CSCENGDEBUG2( 
            "CCSCEngConnectionHandler::RunL STATE=%d", iInfoBuf().iState );
        
        if ( EInterfaceDown == iInfoBuf().iState )
            {
            iTimer->StopTimer();
            iObserver.NotifyConnectionEvent( EEventConnectionDown );
            }
        }
    }


// ---------------------------------------------------------------------------
// CCSCEngConnectionHandler::DoCancel
// ---------------------------------------------------------------------------
//
void CCSCEngConnectionHandler::DoCancel()
    {    
    iTimer->StopTimer();
    iConnection.CancelAllInterfaceNotification();
    }


