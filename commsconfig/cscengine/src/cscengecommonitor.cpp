/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  For monitoring ecom events.
*
*/

#include "cscenglogger.h"
#include "cscengecommonitor.h"
#include "mcscengecomobserver.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCEngEcomMonitor::CCSCEngEcomMonitor( MCSCEngEcomObserver& aObserver ) 
    : CActive( EPriorityStandard ),
    iObserver( aObserver )
    {    
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CCSCEngEcomMonitor::ConstructL()
    {
    CSCENGDEBUG( "CCSCEngEcomMonitor::ConstructL - begin" ); 
    
    // Open ECom session and start monitoring count of implementations.
    CActiveScheduler::Add( this );
    iEcomSession = REComSession::OpenL();
    StartMonitoring();
    
    CSCENGDEBUG( "CCSCEngEcomMonitor::ConstructL - end" ); 
    }   
    
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCEngEcomMonitor* CCSCEngEcomMonitor::NewL( MCSCEngEcomObserver& aObserver )
    {    
    CCSCEngEcomMonitor* self = new ( ELeave ) CCSCEngEcomMonitor( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCEngEcomMonitor::~CCSCEngEcomMonitor()
    {
    CSCENGDEBUG( "CCSCEngEcomMonitor::~CCSCEngEcomMonitor - begin" ); 

    Cancel();
    iEcomSession.Close();
    
    CSCENGDEBUG( "CCSCEngEcomMonitor::~CCSCEngEcomMonitor - end" ); 
    }


// ---------------------------------------------------------------------------
// From CActive
// CCSCEngEcomMonitor::RunL
// ---------------------------------------------------------------------------
//
void CCSCEngEcomMonitor::RunL()
    {
    CSCENGDEBUG( "CCSCEngEcomMonitor::RunL - begin" );
    
    iObserver.NotifyEcomEvent();
            
    // Start monitoring changed in ECom session again.    
    StartMonitoring();
    
    CSCENGDEBUG( "CCSCEngEcomMonitor::RunL - end" );
    }
    
    
// ---------------------------------------------------------------------------
// From CActive
// CCSCEngEcomMonitor::DoCancel
// ---------------------------------------------------------------------------
//
void CCSCEngEcomMonitor::DoCancel()
    {
    // Cancel ECom session notification service.
    iEcomSession.CancelNotifyOnChange( iStatus );
    }    


// ---------------------------------------------------------------------------
// CCSCEngEcomMonitor::StartMonitoring
// Starts monitoring about ECom notifications.
// ---------------------------------------------------------------------------
//
void CCSCEngEcomMonitor::StartMonitoring()
    {
    // Attach to ECom session notification service.
    if ( !IsActive() )
        {
        iEcomSession.NotifyOnChange( iStatus );
        SetActive();
        }
    }
    
// ---------------------------------------------------------------------------
// CCSCEngEcomMonitor::ResetAndDestroy
// Frees memory allocated by array in case of leave.
// ---------------------------------------------------------------------------
//
void CCSCEngEcomMonitor::ResetAndDestroy( TAny* aArray )
    {
    CSCENGDEBUG( "CCSCEngEcomMonitor::ResetAndDestroy" );	
    if ( aArray )
        {
        RImplInfoPtrArray* array = 
            reinterpret_cast<RImplInfoPtrArray*>( aArray );
        array->ResetAndDestroy();
        }
    }
