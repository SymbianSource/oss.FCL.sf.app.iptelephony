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
* Description: CCH UI Call State Listener
*
*/


#include <ctsydomainpskeys.h>

#include  "cchuicallstatelistener.h"
#include  "cchuicallstateobserver.h"
#include  "cchuilogger.h"

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CCchUiCallStateListener::CCchUiCallStateListener
// ---------------------------------------------------------------------------
//
CCchUiCallStateListener::CCchUiCallStateListener(
    MCchUiCallStateObserver& aObserver ):
    CActive( CActive::EPriorityStandard ),
    iObserver( aObserver )
    {
    CActiveScheduler::Add( this );
    
    iProperty.Attach( KPSUidCtsyCallInformation, KCTsyCallState );
    IssueRequest();
    }

// ---------------------------------------------------------------------------
// CCchUiCallStateListener::NewL
// ---------------------------------------------------------------------------
//
CCchUiCallStateListener* CCchUiCallStateListener::NewL(
    MCchUiCallStateObserver& aObserver )
    {
    CCchUiCallStateListener* self = new ( ELeave ) CCchUiCallStateListener(
        aObserver );
    CCHUIDEBUG2( "CCchUiCallStateListener::NewL this=%d", (TInt)self );
    return self;
    }

// ---------------------------------------------------------------------------
// CCchUiCallStateListener::~CCchUiCallStateListener
// ---------------------------------------------------------------------------
//
CCchUiCallStateListener::~CCchUiCallStateListener()
    {
    CCHUIDEBUG( "CCchUiCallStateListener::~CCchUiCallStateListener - Start" );
    
    Cancel();
    iProperty.Close();
    
    CCHUIDEBUG( "CCchUiCallStateListener::~CCchUiCallStateListener - End" );
    }

// ---------------------------------------------------------------------------
// From class CActive
// CCchUiCallStateListener::RunL
// ---------------------------------------------------------------------------
//
void CCchUiCallStateListener::RunL()
    { 
    if ( iStatus.Int() == KErrNone )
        {
        HandleChange();
        }
    IssueRequest();
    }

// ----------------------------------------------------------
// From class CActive
// CCchUiCallStateListener::DoCancel
// ----------------------------------------------------------
//
void CCchUiCallStateListener::DoCancel()
    {
    iProperty.Cancel();
    }

// ---------------------------------------------------------------------------
// CCchUiCallStateListener::HandleChange
// ---------------------------------------------------------------------------
//
void CCchUiCallStateListener::HandleChange()
    {
    CCHUIDEBUG( "CCchUiCallStateListener::HandleChange - Start" );
    
    TInt err = RProperty::Get( KPSUidCtsyCallInformation,
                               KCTsyCallState,
                               iCallState );
    if ( err == KErrNone )
        {
        iObserver.CallStateChanged( iCallState );
        }
    
    CCHUIDEBUG( "CCchUiCallStateListener::HandleChange - End" );
    }

// ---------------------------------------------------------------------------
// CCchUiCallStateListener::IssueRequest
// ---------------------------------------------------------------------------
//
void CCchUiCallStateListener::IssueRequest()
    {
    if ( !IsActive() )
        {
        iProperty.Subscribe( iStatus );
        SetActive();
        }
    }
