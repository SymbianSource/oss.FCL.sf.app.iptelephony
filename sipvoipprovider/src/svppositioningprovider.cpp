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
* Description:  Provides position information
*
*/


#include <dhcppsy.h>

#include "svppositioningprovider.h"
#include "svppositioningproviderobserver.h"
#include "svplogger.h" // For logging


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CSVPPositioningProvider::CSVPPositioningProvider( 
    TPriority aPriority, MSVPPositioningProviderObserver& aObserver )
    : CActive( aPriority ), 
      iObserver( aObserver )
    {  
    iPositionServer = RPositionServer();
    iPositioner = RPositioner();
    
    CActiveScheduler::Add( this );
    }
    

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CSVPPositioningProvider::ConstructL()
    {
    User::LeaveIfError( iPositionServer.Connect() );
    }


// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CSVPPositioningProvider* CSVPPositioningProvider::NewL( 
    TPriority aPriority, MSVPPositioningProviderObserver& aObserver )
    {
    CSVPPositioningProvider* self = 
        CSVPPositioningProvider::NewLC( aPriority, aObserver );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
CSVPPositioningProvider* CSVPPositioningProvider::NewLC( 
    TPriority aPriority, MSVPPositioningProviderObserver& aObserver )
    {
    SVPDEBUG1("CSVPPositioningProvider::NewLC");
    
    CSVPPositioningProvider* self = 
        new( ELeave ) CSVPPositioningProvider( aPriority, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CSVPPositioningProvider::~CSVPPositioningProvider()
    {
    SVPDEBUG1("CSVPPositioningProvider::~CSVPPositioningProvider");
    
    Cancel();
    
    iPositioner.Close();
    iPositionServer.Close();
    
    delete iPositionGenericInfo;
    }


// ---------------------------------------------------------------------------
// Opens positioning module
// ---------------------------------------------------------------------------
//
void CSVPPositioningProvider::OpenModuleL( TInt aModuleId )
    {
    SVPDEBUG1("CSVPPositioningProvider::OpenModuleL");
    
    TPositionModuleId moduleId;
    moduleId.iUid = aModuleId;
    User::LeaveIfError( iPositioner.Open( iPositionServer, moduleId ) );
    }


// ---------------------------------------------------------------------------
// Closes positioning module
// ---------------------------------------------------------------------------
//
void CSVPPositioningProvider::CloseModule()
    {
    SVPDEBUG1("CSVPPositioningProvider::CloseModule");
    
    Cancel();
    
    iPositioner.Close();
    }


// ---------------------------------------------------------------------------
// Requests position information asynchronically for the given SIP profile
// ---------------------------------------------------------------------------
//
void CSVPPositioningProvider::MakePositioningRequestL( 
    TUint32 aIapId, const TDesC& aApplicationName, TUint32 /*aTimeout*/ )
    {
    SVPDEBUG1("CSVPPositioningProvider::MakePositioningRequestL");
    
    delete iPositionGenericInfo;
    iPositionGenericInfo = NULL;
    iPositionGenericInfo = HPositionGenericInfo::NewL();
    
    // Set requested field
    iPositionGenericInfo->SetRequestedField( EDhcpPsyIAPProfileIdField );
    iPositionGenericInfo->SetValue( EDhcpPsyIAPProfileIdField, aIapId );
    
    // Set requestor 
    // - ERequestorService: request originates from an application 
    // - EFormatApplication: request contains the textual name of an 
    //   application
    User::LeaveIfError( iPositioner.SetRequestor( 
        CRequestor::ERequestorService,
        CRequestor::EFormatApplication,
        aApplicationName ) );    
    
    /* TEMPORARY FIX
    // Set timeout
    TPositionUpdateOptions positionUpdateOptions;
    positionUpdateOptions.SetUpdateTimeOut( 
        TTimeIntervalMicroSeconds( aTimeout ) );
    User::LeaveIfError( 
        iPositioner.SetUpdateOptions( positionUpdateOptions ) );
    */

    // Make position request
    iPositioner.NotifyPositionUpdate( *iPositionGenericInfo, iStatus);
    SetActive();
    }


// ---------------------------------------------------------------------------
// From class CActive.
// DoCancel
// ---------------------------------------------------------------------------
//
void CSVPPositioningProvider::DoCancel()
    {
    iPositioner.CancelRequest( EPositionerNotifyPositionUpdate );
    }


// ---------------------------------------------------------------------------
// From class CActive.
// RunL
// ---------------------------------------------------------------------------
//
void CSVPPositioningProvider::RunL()
    {
    SVPDEBUG1("CSVPPositioningProvider::RunL");
    
    if ( KErrNone != iStatus.Int() )
        {
        iObserver.PositioningErrorOccurred( iStatus.Int() );
        }
    else if ( iPositionGenericInfo && 
              iPositionGenericInfo->IsRequestedField( 
                  EDhcpPsyLocationResultDataField ) 
            )
        {
        TPtrC8 locInfo;
        iPositionGenericInfo->GetValue( 
            EDhcpPsyLocationResultDataField, locInfo );
        iObserver.PositioningRequestComplete( locInfo );
        }
    else
        {
        iObserver.PositioningErrorOccurred( KErrNotFound );
        }
    }
