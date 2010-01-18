/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class for timer handling
*
*/


#include "acptimer.h"
#include "macptimerobserver.h"

const TInt KPhonebookStartupDelay = 5000000; // 5 seconds

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CAcpTimer::CAcpTimer( MAcpTimerObserver& aObserver ) 
    : CTimer( EPriorityStandard ), iObserver( aObserver )
    {    
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CAcpTimer::ConstructL()
    {    
    CActiveScheduler::Add( this );
    CTimer::ConstructL();
    }   
    
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CAcpTimer* CAcpTimer::NewL( MAcpTimerObserver& aObserver )
    {    
    CAcpTimer* self = new ( ELeave ) CAcpTimer( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CAcpTimer::~CAcpTimer()
    {    
    CTimer::Cancel();
    }
   

// ---------------------------------------------------------------------------
// Start timer.
// ---------------------------------------------------------------------------
//
TInt CAcpTimer::StartTimer( TTimerType aTimerType )
    {    
    TInt error ( KErrNone );
    
    if ( CTimer::IsActive() )
        {
        CTimer::Cancel();
        }
    
    if ( EPhonebookStartupDelayTimer == aTimerType )
        {
        CTimer::After( KPhonebookStartupDelay );
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
void CAcpTimer::StopTimer()
    {
    if ( CTimer::IsActive() )
        {
        CTimer::Cancel();
        }
    }

// ---------------------------------------------------------------------------
// IsActive
// ---------------------------------------------------------------------------
//
TBool CAcpTimer::IsActive()
    {
    return CTimer::IsActive();
    }


// ---------------------------------------------------------------------------
// CAcpTimer::RunL
// ---------------------------------------------------------------------------
//
void CAcpTimer::RunL()
    {    
    iObserver.TimerExpired();
    }
