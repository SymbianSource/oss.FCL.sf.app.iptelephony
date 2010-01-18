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
* Description:  CCchPhoneStartupMonitor implementation.
 *
*/

#include "cchlogger.h"
#include "cchuihandler.h"
#include "cchphonestartupmonitor.h"

#include <startupdomainpskeys.h>

// ============================= LOCAL FUNCTIONS =============================

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// CCchPhoneStartupMonitor::CCchPhoneStartupMonitor
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CCchPhoneStartupMonitor::CCchPhoneStartupMonitor( 
    CCchUIHandler& aCchUIHandler ) :
    CActive( CActive::EPriorityStandard ),
    iCchUIHandler( aCchUIHandler )
    {
    CCHLOGSTRING( "CCchPhoneStartupMonitor::CCchPhoneStartupMonitor" );
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CCchPhoneStartupMonitor::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CCchPhoneStartupMonitor::ConstructL()
    {  
    CCHLOGSTRING( "CCchPhoneStartupMonitor::ConstructL - IN" );
    
    iStartupUiPhaseProperty.Attach( KPSUidStartup, KPSStartupUiPhase );
    MonitorPhoneStartupPhaseL();
    
    CCHLOGSTRING( "CCchPhoneStartupMonitor::ConstructL - OUT" );
    }

// ---------------------------------------------------------------------------
// CCchPhoneStartupMonitor::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CCchPhoneStartupMonitor* CCchPhoneStartupMonitor::NewL( 
    CCchUIHandler& aCchUIHandler )
    {
    CCHLOGSTRING( "CCchPhoneStartupMonitor::NewL" );
    CCchPhoneStartupMonitor* self = 
        CCchPhoneStartupMonitor::NewLC( aCchUIHandler );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CCchPhoneStartupMonitor::NewLC
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CCchPhoneStartupMonitor* CCchPhoneStartupMonitor::NewLC( 
    CCchUIHandler& aCchUIHandler )
    {
    CCHLOGSTRING( "CCchPhoneStartupMonitor::NewLC" );
    CCchPhoneStartupMonitor* self = 
        new (ELeave) CCchPhoneStartupMonitor( aCchUIHandler );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CCchPhoneStartupMonitor::~CCchPhoneStartupMonitor()
// Destructor.
// ---------------------------------------------------------------------------
//
CCchPhoneStartupMonitor::~CCchPhoneStartupMonitor()
    {   
    CCHLOGSTRING( "CCchPhoneStartupMonitor::~CCchPhoneStartupMonitor" );
    
    iStartupUiPhaseProperty.Cancel();
    Cancel();
    }

// ----------------------------------------------------------------------------
// CCchPhoneStartupMonitor::DoCancel()
// ----------------------------------------------------------------------------
void CCchPhoneStartupMonitor::DoCancel()
    {
    CCHLOGSTRING("CCchPhoneStartupMonitor::DoCancel()");   
    }

// ----------------------------------------------------------------------------
// CCchPhoneStartupMonitor::RunL()
// ----------------------------------------------------------------------------
void CCchPhoneStartupMonitor::RunL()
    {
    CCHLOGSTRING("CCchPhoneStartupMonitor::RunL() IN"); 
       
    MonitorPhoneStartupPhaseL();
    
    CCHLOGSTRING("CCchPhoneStartupMonitor::RunL() OUT"); 
    }

// ----------------------------------------------------------------------------
// CCchPhoneStartupMonitor::RunError()
// ----------------------------------------------------------------------------
TInt CCchPhoneStartupMonitor::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CCchPhoneStartupMonitor::MonitorPhoneStartupPhaseL()
// ----------------------------------------------------------------------------
void CCchPhoneStartupMonitor::MonitorPhoneStartupPhaseL()
    {
    CCHLOGSTRING("CCchPhoneStartupMonitor::MonitorPhoneStartupPhaseL() IN"); 
    
    TInt status( 0 );   
    TInt err = iStartupUiPhaseProperty.Get( 
        KPSUidStartup,
        KPSStartupUiPhase,
        status );
    
    CCHLOGSTRING2(
        "CCchPhoneStartupMonitor::MonitorPhoneStartupPhaseL() ERR=%d", err );
    
    if ( !err )
        {
        TBool networkConnAllowed = iCchUIHandler.NetworkConnectionsAllowed();

        CCHLOGSTRING2( " --> status=%d", status );
        CCHLOGSTRING2( " --> networkConnAllowed=%d", networkConnAllowed ); 
        
        if ( EStartupUiPhaseAllDone == status && !networkConnAllowed )
            {
            iCchUIHandler.ShowEmergencyWarningNoteL( EFalse );
            Cancel();
            }
        else
            {
            iStartupUiPhaseProperty.Subscribe( iStatus );
            SetActive();
            }
        }
    
    CCHLOGSTRING("CCchPhoneStartupMonitor::MonitorPhoneStartupPhaseL() OUT"); 
    }

// ========================== OTHER EXPORTED FUNCTIONS =======================

//  End of File
