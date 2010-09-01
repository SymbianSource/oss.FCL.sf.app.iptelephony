/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Generates DTMF events in Inband DTMF    
*
*/


#include "svpdtmfeventgenerator.h"
#include "svplogger.h"
#include "svpdtmfeventobserver.h"

const TInt KSvpNormalEventInterval = 300000;
// Pause character ('p') interval is 2500 ms
const TInt KSvpPauseEventInterval = 2500000;

// ---------------------------------------------------------------------------
// CSVPDTMFEventGenerator::CSVPDTMFEventGenerator
// ---------------------------------------------------------------------------
//
CSVPDTMFEventGenerator::CSVPDTMFEventGenerator(  
                                           MSVPDTMFEventObserver& aObserver ): 
                                  
    CActive( EPriorityStandard ),      
    iObserver( aObserver )
    {
    // add object to active scheduler
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CSVPDTMFEventGenerator::CSVPDTMFEventGenerator
// ---------------------------------------------------------------------------
//
void CSVPDTMFEventGenerator::ConstructL()
    {
    // create timer
    User::LeaveIfError( iTimer.CreateLocal() );
    }

// ---------------------------------------------------------------------------
// CSVPDTMFEventGenerator::CSVPDTMFEventGenerator
// ---------------------------------------------------------------------------
//
CSVPDTMFEventGenerator* CSVPDTMFEventGenerator::NewL( 
                                            MSVPDTMFEventObserver& aObserver )
    {
    CSVPDTMFEventGenerator* self = 
        new ( ELeave ) CSVPDTMFEventGenerator( aObserver );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------------------------
// CSVPDTMFEventGenerator::~CSVPDTMFEventGenerator
// ---------------------------------------------------------------------------
//  
CSVPDTMFEventGenerator::~CSVPDTMFEventGenerator()
    {
    // cancel outgoing requests and close timer
    Cancel();
    iTimer.Close();
    }

// ---------------------------------------------------------------------------
// CSVPDTMFEventGenerator::RunL
// ---------------------------------------------------------------------------
//  
void CSVPDTMFEventGenerator::RunL()
    {
    SVPDEBUG2( "CSVPDTMFEventGenerator::RunL tick: %u", User::TickCount() )
    SVPDEBUG2( "CSVPDTMFEventGenerator::RunL iStatus: %d", iStatus.Int() )
    SVPDEBUG2( "CSVPDTMFEventGenerator::RunL iStringLength: %d", iStringLength )
    
    if ( KErrNone == iStatus.Int() )
        {
        if ( !iStartSent && iStringLength )
            {
            iObserver.InbandDtmfEventOccurred( ESvpDtmfSendStarted );
            iStartSent = ETrue;
            iStringLength--;
            TTimeIntervalMicroSeconds32 delay; 
            if ( iPause )
                {
                delay = KSvpPauseEventInterval;
                iPause = EFalse;
                }
            else
                {
                delay = KSvpNormalEventInterval;
                }
            if ( !IsActive() )
                {
                iTimer.After ( iStatus, delay );
                SetActive(); 
                }
            }
        else
            {  
            iObserver.InbandDtmfEventOccurred( ESvpDtmfSendStopped );
            iStartSent = EFalse;
            
            if ( iStringLength )
                {
                TTimeIntervalMicroSeconds32 delay = KSvpNormalEventInterval;
                if ( !IsActive() )
                    {
                    iTimer.After ( iStatus, delay );
                    SetActive(); 
                    }
                }
            else
                {
                iObserver.InbandDtmfEventOccurred( ESvpDtmfSendCompleted );
                }
            }     
        }
    else
        {
        User::Leave( iStatus.Int() );
        }
    }
    
// ---------------------------------------------------------------------------
// CSVPDTMFEventGenerator::StartDtmfEvents
// ---------------------------------------------------------------------------
//     
void CSVPDTMFEventGenerator::StartDtmfEvents( TInt aStringLength, TBool aPause )
    {
    SVPDEBUG1("CSVPDTMFEventGenerator::StartDtmfEvents In");
    Cancel();
    // Give the time to CTimer as microseconds 
    // event interval is default => 300ms  
    if ( aStringLength )
        {
        // save string length    
        iStringLength = aStringLength;
        iPause = aPause;
        iTimer.After( iStatus, KSvpNormalEventInterval );
        SetActive(); 
        } 
    }
    
// ---------------------------------------------------------------------------
// CSVPDTMFEventGenerator::DoCancel
// ---------------------------------------------------------------------------
// 
void CSVPDTMFEventGenerator::DoCancel()
    {
    SVPDEBUG1( "CSVPDTMFEventGenerator::DoCancel" )
    iStringLength = 0;
    iStartSent = EFalse;
    iPause = EFalse;
    iTimer.Cancel();
    }


// ---------------------------------------------------------------------------
// CSVPDTMFEventGenerator::StopEvents
// ---------------------------------------------------------------------------
//  
void CSVPDTMFEventGenerator::StopEvents()
    {
    SVPDEBUG1("CSVPDTMFEventGenerator::StopEvents In");
    // Always call through Cancel() as it will 
    // cancel the request in the scheduler and then it will call DoCancel()
    Cancel();
    }


// ---------------------------------------------------------------------------
// CSVPDTMFEventGenerator::RunError
// ---------------------------------------------------------------------------
// 
TInt CSVPDTMFEventGenerator::RunError( TInt aError )
    {
    SVPDEBUG2( "CSVPDTMFEventGenerator::RunError aError: %d", aError )
    
    return aError;
    }

//  End of File
