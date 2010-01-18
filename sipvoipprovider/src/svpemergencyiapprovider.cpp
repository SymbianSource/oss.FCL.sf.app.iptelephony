/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides IAP IDs for emergency
*
*/


#include "svpemergencyiapprovider.h"
#include "svplogger.h" // For logging


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CSVPEmergencyIapProvider::CSVPEmergencyIapProvider( TPriority aPriority )
    : CActive( aPriority )
    {  
    CActiveScheduler::Add( this );
    }
    
// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CSVPEmergencyIapProvider::ConstructL()
    {
    SVPDEBUG1("CSVPEmergencyIapProvider::ConstructL()")
    
    User::LeaveIfError( iConnectionMonitor.ConnectL() );
    iWait = new ( ELeave ) CActiveSchedulerWait();
    }

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CSVPEmergencyIapProvider* CSVPEmergencyIapProvider::NewL( 
    TPriority aPriority )
    {
    CSVPEmergencyIapProvider* self = CSVPEmergencyIapProvider::NewLC( 
        aPriority );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
CSVPEmergencyIapProvider* CSVPEmergencyIapProvider::NewLC( 
    TPriority aPriority )
    {
    CSVPEmergencyIapProvider* self = new( ELeave ) CSVPEmergencyIapProvider( 
        aPriority );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CSVPEmergencyIapProvider::~CSVPEmergencyIapProvider()
    {
    SVPDEBUG1("CSVPEmergencyIapProvider::~CSVPEmergencyIapProvider() In")
    
    Cancel();
    
    iConnectionMonitor.Close();
    
    delete iWait;
    
    SVPDEBUG1("CSVPEmergencyIapProvider::~CSVPEmergencyIapProvider() Out")
    }

// ---------------------------------------------------------------------------
// Requests all IAP IDs
// ---------------------------------------------------------------------------
//
TInt CSVPEmergencyIapProvider::RequestIapIds( RArray<TUint>& aIapIds )
    {
    SVPDEBUG1("CSVPEmergencyIapProvider::RequestIapIds()")
    
    if ( IsActive() )
        {
        return KErrInUse;
        }
    
    // Make request for IAP IDs
    iConnectionMonitor.GetPckgAttribute( 
        EBearerIdWLAN, 0, KIapAvailability, iIapInfoBuf, iStatus );    
    SetActive();
    
    // Wait until request completes
    iWait->Start();
    // Continues here after RunL completed

    // Copy IAP IDs to array
    if ( KErrNone == iError )
        {
        TUint count = iIapInfoBuf().iCount;
        for ( TInt i = 0; i < count; i++ )
            {
            aIapIds.Append( iIapInfoBuf().iIap[i].iIapId );
            }
        }
    
    return iError;
    }

// ---------------------------------------------------------------------------
// From class CActive.
// DoCancel
// ---------------------------------------------------------------------------
//
void CSVPEmergencyIapProvider::DoCancel()
    {
    SVPDEBUG1("CSVPEmergencyIapProvider::DoCancel()")

    iConnectionMonitor.CancelAsyncRequest( EConnMonGetPckgAttribute );    

    if ( iWait->IsStarted() )
        {
        iWait->AsyncStop();
        }
    }

// ---------------------------------------------------------------------------
// From class CActive.
// RunL
// ---------------------------------------------------------------------------
//
void CSVPEmergencyIapProvider::RunL()
    {
    iError = iStatus.Int();
    SVPDEBUG2("CSVPEmergencyIapProvider::RunL(), error: %d", iError)

    if ( iWait->IsStarted() )
        {
        iWait->AsyncStop();
        // Returns right after iWait.Start() call
        }
    }
