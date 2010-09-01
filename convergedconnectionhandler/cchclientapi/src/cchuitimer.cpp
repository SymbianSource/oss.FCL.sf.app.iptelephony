/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "cchuitimer.h"
#include "mcchuitimerobserver.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCchUiTimer::CCchUiTimer( MCchUiTimerObserver& aObserver ) 
    : CTimer( EPriorityStandard ), iObserver( aObserver )
    {    
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CCchUiTimer::ConstructL()
    {    
    CActiveScheduler::Add( this );
    CTimer::ConstructL();
    }   
    
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCchUiTimer* CCchUiTimer::NewL( MCchUiTimerObserver& aObserver )
    {    
    CCchUiTimer* self = new ( ELeave ) CCchUiTimer( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCchUiTimer::~CCchUiTimer()
    {    
    CTimer::Cancel();
    }
   

// ---------------------------------------------------------------------------
// Start timer.
// ---------------------------------------------------------------------------
//
TInt CCchUiTimer::StartTimer( TTimerType /*aTimerType*/ )
    {    
    TInt error ( KErrNone );
    
    if ( CTimer::IsActive() )
        {
        CTimer::Cancel();
        }
        
    return error;
    }


// ---------------------------------------------------------------------------
// Stop timer.
// ---------------------------------------------------------------------------
//
void CCchUiTimer::StopTimer()
    {
    if ( CTimer::IsActive() )
        {
        CTimer::Cancel();
        }
    }


// ---------------------------------------------------------------------------
// RunL
// ---------------------------------------------------------------------------
//
void CCchUiTimer::RunL()
    {    
    iObserver.TimerExpired();
    }
