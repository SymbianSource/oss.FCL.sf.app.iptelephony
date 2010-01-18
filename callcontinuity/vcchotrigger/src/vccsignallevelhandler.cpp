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
* Description:   Implementation of the base class of signal level handler
*
*/



#include "vccsignallevelhandler.h"
#include "vccsignallevelobserver.h"
#include "rubydebug.h"


// Min. signal strength in absolut dBm, i.e. the value
// in real world is -110 dBm (so quite weak).
static const TInt32 KStrengthMin = 110;

// ---------------------------------------------------------------------------
// C++ destructor.
// ---------------------------------------------------------------------------
//
CVccSignalLevelHandler::~CVccSignalLevelHandler()
    {
    // Cancel any outstanding requests.
    Cancel();

    iStrength = 0;
    }

// ---------------------------------------------------------------------------
// Symbian second-phase constructor.
// ---------------------------------------------------------------------------
//
void CVccSignalLevelHandler::ConstructL()
    {
    CTimer::ConstructL();
    }

// ---------------------------------------------------------------------------
// C++ constructor.
// ---------------------------------------------------------------------------
//
CVccSignalLevelHandler::CVccSignalLevelHandler(
    MVccSignalLevelObserver& aObserver,
    const TSignalLevelParams& aParams )
    : CTimer( EPriorityStandard ),
    iObserver( aObserver ),
    iParams( aParams )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// Star signal level observing.
// ---------------------------------------------------------------------------
//
void CVccSignalLevelHandler::StartL()
    {
    RUBY_DEBUG_BLOCK( "CVccSignalLevelHandler::StartL" );

    if ( !iNotificationsOn )
        {
        EnableNotificationsL();
        iNotificationsOn = ETrue;
        }

    RUBY_DEBUG0( " -call Cancel()" );
    Cancel();

    RUBY_DEBUG0( " -call GetStrength()" );
    GetStrength();

    RUBY_DEBUG1( " -iStrength = %d, set op = EOperationGet \
                 state = EStrengthUnknown", iStrength );
    
    iOperation = EOperationGet;

    iState = EStrengthUnknown;

    RUBY_DEBUG0( " -call SetActive()" );
    SetActive();
    }

// ---------------------------------------------------------------------------
// Stop signal level observing.
// ---------------------------------------------------------------------------
//
void CVccSignalLevelHandler::Stop()
    {
    RUBY_DEBUG_BLOCK( "CVccSignalLevelHandler::Stop ");

    DisableNotifications();

    iNotificationsOn = EFalse;

    iStrength = 0;

    Cancel();
    }

// ---------------------------------------------------------------------------
// Set new parameters.
// ---------------------------------------------------------------------------
//
void CVccSignalLevelHandler::SetParams( const TSignalLevelParams& aParams )
    {
    RUBY_DEBUG_BLOCK( "CVccSignalLevelHandler::SetParams" );
    iParams = aParams;
    }

// ---------------------------------------------------------------------------
// Handles getting the signal strength and notifying the observer about
// strength changes.
// ---------------------------------------------------------------------------
//
void CVccSignalLevelHandler::RunL()
    {
    RUBY_DEBUG_BLOCK( "CVccSignalLevelHandler::RunL" );

    // Zero (0) is not acceptable.
    if ( !iStrength )
        {
        RUBY_DEBUG0( " -0 strength not acceptable, setting to KStrengthMin");
        iStrength = KStrengthMin;
        }

    RUBY_DEBUG3( " -iStrength = %d iState = %d iOp = %d", iStrength, iState, iOperation );

    switch ( iOperation )
        {
        case EOperationGet:
            {

            // We are in the Get-mode to get the signal strength.
            // If the strength is < than the high level (== the strength
            // is good), start timer to check if we are still in good level
            // after the timer completes.
            // The same is done if we have a low level (== bad).

            RUBY_DEBUG0( " -EOperationGet");

            if ( iStrength <= iParams.iHighLevel && iStrength > 0 )
                {
                RUBY_DEBUG0( "  set state = EStrengthHigh, op = EOperationWait" );

                After( iParams.iHighTimeout );
                iState = EStrengthHigh;
                iOperation = EOperationWait;
                }
            else if ( iStrength >= iParams.iLowLevel )
                {
                RUBY_DEBUG0( "  set state = EStrengtLow, op = EOperationWait" );
                After( iParams.iLowTimeout );
                iState = EStrengthLow;
                iOperation = EOperationWait;
                }
            else
                {
                RUBY_DEBUG0( "  strength between low and high, set op = EOperationNone" );

                iOperation = EOperationNone;
                // PCLint
                }
            break;
            }

        case EOperationWait:
            {

            // Timer has completed. Check the signal level again.

            RUBY_DEBUG0( " -EOperationWait" );
            RUBY_DEBUG0( "  set op = EOperationComplete" );
            GetStrength();

            SetActive();

            iOperation = EOperationComplete;

            break;
            }

        case EOperationComplete:
            {
            // Checking signal strength is now done.
            // Notify our observer (if needed).

            RUBY_DEBUG1( " -EOperationComplete, iStrength = %d", iStrength );

            // Do we have a good signal level?
            if ( iStrength <= iParams.iHighLevel && iStrength > 0 && iState == EStrengthHigh )
                {
                RUBY_DEBUG0( " -if ( iStrength <= iParams.iHighLevel" );
                NotifyChanges( iStrength, MVccSignalLevelObserver::ESignalClassNormal );
                }
            // Or do we have a bad signal level?
            else if ( iStrength >= iParams.iHighLevel && iState == EStrengthLow )
                {
                RUBY_DEBUG0( " -else if ( iStrength >= iParams.iHighLevel" );
                NotifyChanges( iStrength, MVccSignalLevelObserver::ESignalClassWeak );
                }
            else
                {
                // PCLint
                }

            iOperation = EOperationNone;

            break;
            }

        default:
            {
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// Handles signal strength changes notified by the subsystem
// (either the WLAN or the GSM).
// ---------------------------------------------------------------------------
//
void CVccSignalLevelHandler::StrengthChanged()
    {

    RUBY_DEBUG_BLOCK( "CVccSignalLevelHandler::StrengthChanged" );

    // The zero (0) is not good.
    if ( !iStrength )
        {
        iStrength = KStrengthMin;
        }

    RUBY_DEBUG3( " -iStrength = %d, iState = %d, iOp = %d",
                             iStrength, iState, iOperation);

    // Check in which direction the signal strength changed
    // and start the timeout timer to see if the signal level
    // stays at that (new) level.

    RUBY_DEBUG1( " -iHighLevel = %d", iParams.iHighLevel );
    RUBY_DEBUG1( " -iLowLevel = %d", iParams.iLowLevel );
    if ( iStrength <= iParams.iHighLevel && iStrength > 0 )  // && 
        //( iState == EStrengthLow || iState == EStrengthUnknown ) )
        {
        // Cancel outstanding timer in all the cases
        Cancel();
        // We have a good level after a bad one.

        RUBY_DEBUG0( " -call After( hightimeout ), set state = High, op = Wait");
        After( iParams.iHighTimeout );
        iState = EStrengthHigh;
        iOperation = EOperationWait;
        }
    else if ( iStrength >= iParams.iLowLevel )  //&& 
            //( iState == EStrengthHigh || iState == EStrengthUnknown ) )
        {
        // Cancel outstanding timer in all the cases
        Cancel();
        // We have a bad level after a good one.

        RUBY_DEBUG0( " -call After( lowtimeout ), set state = Low, op = Wait");

        After ( iParams.iLowTimeout );
        iState = EStrengthLow;
        iOperation = EOperationWait;
        }
    else
        {
        RUBY_DEBUG0( "-EOperationNone ");
        iOperation = EOperationNone;
        }
    }

// ---------------------------------------------------------------------------
// Cancel outstanding requests.
// ---------------------------------------------------------------------------
//
void CVccSignalLevelHandler::DoCancel()
    {
    RUBY_DEBUG_BLOCK( "CVccSignalLevelHandler::DoCancel" );
    switch ( iOperation )
        {
        case EOperationWait:
            {
            RUBY_DEBUG0( "EOperationWait" );
            CTimer::DoCancel();

            break;
            }

        case EOperationGet:
            // fall-through intended here
        case EOperationComplete:
            {
            RUBY_DEBUG0( "EOperationGet / EOperationComplete" );
            CancelGetStrength();

            break;
            }

        default:
            {
            break;
            }
        }
    }
