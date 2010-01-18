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
* Description:  For timer handling.
*
*/


#include "cscengtimer.h"
#include "cscenglogger.h"
#include "mcscengtimerobserver.h"

const TInt KWaitConnectionToClose = 5000000; // 5 seconds

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCEngTimer::CCSCEngTimer( MCSCEngTimerObserver& aObserver ) 
    : CTimer( EPriorityStandard ), iObserver( aObserver )
    {    
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CCSCEngTimer::ConstructL()
    {
    CSCENGDEBUG( "CCSCEngTimer::ConstructL - begin" ); 
    
    CActiveScheduler::Add( this );
    CTimer::ConstructL();
    
    CSCENGDEBUG( "CCSCEngTimer::ConstructL - end" ); 
    }   
    
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CCSCEngTimer* CCSCEngTimer::NewL( MCSCEngTimerObserver& aObserver )
    {    
    CCSCEngTimer* self = new ( ELeave ) CCSCEngTimer( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCEngTimer::~CCSCEngTimer()
    {
    CSCENGDEBUG( "CCSCEngTimer::~CCSCEngTimer - begin" ); 
    
    CTimer::Cancel();
    
    CSCENGDEBUG( "CCSCEngTimer::~CCSCEngTimer - end" ); 
    }
   

// ---------------------------------------------------------------------------
// Start timer.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCSCEngTimer::StartTimer( TTimerType aTimerType )
    {
    CSCENGDEBUG( "CCSCEngTimer::StartTimer" ); 
    
    TInt error ( KErrNone );
    
    if ( CTimer::IsActive() )
        {
        CTimer::Cancel();
        }
    
    if ( EConnectionMonitoringTimer == aTimerType )
        {
        CTimer::After( KWaitConnectionToClose );
        }
    else
        {
        error = KErrArgument;
        }
    
    return error;
    }


// ---------------------------------------------------------------------------
// Stop timer.
// ---------------------------------------------------------------------------
//
EXPORT_C void CCSCEngTimer::StopTimer()
    {
    CSCENGDEBUG( "CCSCEngTimer::StopTimer - begin" ); 

    if ( CTimer::IsActive() )
        {
        CTimer::Cancel();
        }
    
    CSCENGDEBUG( "CCSCEngTimer::StopTimer - end" ); 
    }


// ---------------------------------------------------------------------------
// CCSCEngTimer::RunL
// ---------------------------------------------------------------------------
//
void CCSCEngTimer::RunL()
    {    
    iObserver.TimerExpired();
    }
