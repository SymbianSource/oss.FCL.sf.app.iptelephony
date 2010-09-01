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
* Description:   
*
*/



#include <mmtsy_names.h>

#include "vccgsmnotifications.h"
#include "rubydebug.h"

// ======== MEMBER FUNCTIONS ========
// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CVccGsmNotifications::CVccGsmNotifications( RMobilePhone& aPhone )
    : CActive( EPriorityStandard ),
      iPhone( aPhone )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// Symbian constructor.
// ---------------------------------------------------------------------------
//
void CVccGsmNotifications::ConstructL()
    {
    }
    
// ---------------------------------------------------------------------------
// Symbian constructor.
// ---------------------------------------------------------------------------
//    
CVccGsmNotifications* CVccGsmNotifications::NewL( RMobilePhone& aPhone )
    {
    CVccGsmNotifications* self = new(ELeave) CVccGsmNotifications( aPhone );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// C++ destructor.
// ---------------------------------------------------------------------------
//
CVccGsmNotifications::~CVccGsmNotifications()
    {
    iObserver = 0;
    Cancel();
    }
    
// ---------------------------------------------------------------------------
// Activates notifications
// ---------------------------------------------------------------------------
//
void CVccGsmNotifications::ActivateNotifications( MVccGsmNotifications* aObserver,
                                                  TInt32 aLow, TInt32 aHigh )
    {
    RUBY_DEBUG_BLOCK( "CVccGsmNotifications::ActivateNotifications" );
    RUBY_DEBUG2( " -aLow = %d aHigh = %d", aLow, aHigh );

    iLow = aLow;
    iHigh = aHigh;
    iObserver = aObserver;
    Cancel();
    iPhone.NotifySignalStrengthChange( iStatus, iStrength, iBar );
    RUBY_DEBUG0( " -call SetActive()" );
    SetActive();
    }
    
// ---------------------------------------------------------------------------
// Cancels notifications
// ---------------------------------------------------------------------------
//
void CVccGsmNotifications::CancelNotifications()
    {
    RUBY_DEBUG_BLOCK( "CVccGsmNotifications::CancelNotifications" );
    iObserver = NULL;
    Cancel();
    }

// ---------------------------------------------------------------------------
// RunL
// ---------------------------------------------------------------------------
//
void CVccGsmNotifications::RunL()
    {
    RUBY_DEBUG_BLOCK( "CVccGsmNotifications::RunL" );
    RUBY_DEBUG2( " -iObserver=%x iStrength=%d", iObserver, iStrength );
    RUBY_DEBUG2( " -iLow = %d iHigh = %d", iLow, iHigh );

    iObserver->GsmStrengthChanged( iStrength );
    iPhone.NotifySignalStrengthChange( iStatus, iStrength, iBar );
    SetActive();
    }
    
// ---------------------------------------------------------------------------
// Cancel
// ---------------------------------------------------------------------------
//
void CVccGsmNotifications::DoCancel()
    {
    RUBY_DEBUG_BLOCK( "CVccGsmNotifications::DoCancel" );
    iPhone.CancelAsyncRequest( EMobilePhoneNotifySignalStrengthChange );
    }

