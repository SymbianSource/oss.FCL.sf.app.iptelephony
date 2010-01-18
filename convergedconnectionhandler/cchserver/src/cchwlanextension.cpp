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
* Description: 
*        class implementation for CCchWlanExtension
*
*/


// INCLUDE FILES
#include "cchwlanextension.h"
#include "cchprivatecrkeys.h"
#include "cchlogger.h"

#include <centralrepository.h>
#include <wlanmgmtclient.h>
#include <StringLoader.h>
#include <WlanCdbCols.h>

// CONSTANTS

const TUint32 KWlanScanRateDeactive = 0;    //scan not active
const TUint32 KWlanScanRateActive   = 300;  //value used when cch activates 1st scan

const TInt KRetryTimeout( 100000 ); // 0,1s delay for comms db begintransaction retry (microseconds)
const TInt KRetryCount( 6 );      // Maximum number of retries


// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// CCchWlanExtension::NewL()
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
CCchWlanExtension* CCchWlanExtension::NewL( )
    {
    CCchWlanExtension* self = new( ELeave ) CCchWlanExtension( );
    CleanupStack::PushL( self );
    self->ConstructL();

    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CCchWlanExtension::ConstructL()
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
void CCchWlanExtension::ConstructL()
    {
    CCHLOGSTRING2( "CCchWlanExtension::ConstructL[0x%x]: IN", this );
    // Create connection to the Access Points setting data.
    iAPCommsDatabase = CCommsDatabase::NewL( EDatabaseTypeIAP );
    
    // Create connection to the WLAN setting data.
    iWLANCommsDatabase = CCommsDatabase::NewL();
    
    iCchRepository = CRepository::NewL( KCRUidCch );
    CCHLOGSTRING( "CCchWlanExtension::ConstructL: OUT" );
    }

// ---------------------------------------------------------------------------
// CCchWlanExtension::CCchWlanExtension()
// Constructor.
// ---------------------------------------------------------------------------
CCchWlanExtension::CCchWlanExtension( ) :
	iLastSavedScanInterval( 0 )
    {
    }

// ---------------------------------------------------------------------------
// CGSUiVoIPExtension::~CCchWlanExtension()
// Destructor.
// ---------------------------------------------------------------------------
CCchWlanExtension::~CCchWlanExtension()
    {
    delete iWLANCommsDatabase;
    delete iAPCommsDatabase;
    delete iCchRepository;
    }

// ---------------------------------------------------------------------------
// CCchWlanExtension::ForceEnableWlanScanL
// ---------------------------------------------------------------------------
void CCchWlanExtension::ForceEnableWlanScanL()
    {
    CCHLOGSTRING( "CCchWlanExtension::ForceEnableWlanScanL: IN" );    
    TInt scanRate = CheckAvailabilityStatusL();
    if( KWlanScanRateDeactive != scanRate )
        {
        CCHLOGSTRING( "CCchWlanExtension::Forced to disable/enable wlan scan" ); 
        SetAvailabilitySwitchL( KWlanScanRateDeactive );    
        SetAvailabilitySwitchL( iLastSavedScanInterval );  
        }
    else
        {
        CCHLOGSTRING( "CCchWlanExtension::Normal wlan scan enable" ); 
        EnableWlanScanL();
        }
    
    CCHLOGSTRING( "CCchWlanExtension::ForceEnableWlanScanL: OUT" );    
    }

// ---------------------------------------------------------------------------
// CCchWlanExtension::EnableWlanScanL
// ---------------------------------------------------------------------------
void CCchWlanExtension::EnableWlanScanL()
    {
    CCHLOGSTRING( "CCchWlanExtension::EnableWlanScanL: IN" );
    TInt err( KErrNone );
    TInt wlanActivation( 0 );
    err = iCchRepository->Get( 
             KCCHWasWlanScanActivatedBeforeServiceEnabling, 
             wlanActivation );
    User::LeaveIfError( err );
    TInt scanRate = CheckAvailabilityStatusL();
    // Activate the WLAN scan setting if needed.
    if( KErrNone == err )
        {
        if( ECCHWlanScanNotDefined == wlanActivation )
            {
            if( KWlanScanRateDeactive < scanRate )    
                {
                iCchRepository->Set( KCCHWasWlanScanActivatedBeforeServiceEnabling, 
                                     ECCHWlanScanWasEnabled );    
                CCHLOGSTRING( "CCchWlanExtension::EnableWlanScanL; scan was enabled by user" );
                }
            else
                {
                iCchRepository->Set( KCCHWasWlanScanActivatedBeforeServiceEnabling, 
                                     ECCHWlanScanWasNotEnabled );    
                
                CCHLOGSTRING( "CCchWlanExtension::EnableWlanScanL; scan was enabled by CCH" );
                }
            }
        }
    if( KWlanScanRateDeactive == scanRate )    
        {
        SetAvailabilitySwitchL( iLastSavedScanInterval );            
        }

    CCHLOGSTRING( "CCchWlanExtension::EnableWlanScanL: OUT" );
    }
// ---------------------------------------------------------------------------
// CCchWlanExtension::DisableWlanScanL
// ---------------------------------------------------------------------------
void CCchWlanExtension::DisableWlanScanL()    
    {
    CCHLOGSTRING( "CCchWlanExtension::DisableWlanScanL: IN" );
    // Deactivate the WLAN scan setting if needed.    
    TInt err( KErrNone );
    TInt wlanActivation( 0 );
    err = iCchRepository->Get( 
             KCCHWasWlanScanActivatedBeforeServiceEnabling, 
             wlanActivation );
    User::LeaveIfError( err );
    TInt scanRate = CheckAvailabilityStatusL();             
    CCHLOGSTRING2( "CCchWlanExtension::DisableWlanScanL: ERROR:%d", err );
    if( KErrNone == err )
        {
        CCHLOGSTRING2( "CCchWlanExtension::DisableWlanScanL: activation:%d", wlanActivation );
        if ( KWlanScanRateDeactive < scanRate )
            {
            CCHLOGSTRING( "CCchWlanExtension::DisableWlanScanL: scan is active" );
            if( ECCHWlanScanNotDefined != wlanActivation )
                {
                if( ECCHWlanScanWasNotEnabled == wlanActivation )
                    {
                    CCHLOGSTRING( "CCchWlanExtension::DisableWlanScanL: disabling wlan scan" );
                    SetAvailabilitySwitchL( KWlanScanRateDeactive );                        
                    }
                err = iCchRepository->Set( KCCHWasWlanScanActivatedBeforeServiceEnabling, 
                                     ECCHWlanScanNotDefined );    
                User::LeaveIfError( err );
                }
            }            
        }
    CCHLOGSTRING( "CCchWlanExtension::DisableWlanScanL: OUT" );
    }

// ---------------------------------------------------------------------------
// CCchWlanExtension::CheckAvailabilityStatusL
// Checks WLAN availability setting status.
// ---------------------------------------------------------------------------
//
TUint32 CCchWlanExtension::CheckAvailabilityStatusL()
	{
	CCHLOGSTRING( "CCchWlanExtension::CheckAvailabilityStatusL: IN" );
    TUint32 scanRate( KErrNone );
    // Open WLAN setting table and get availability data.
    CCommsDbTableView* view = 
        iWLANCommsDatabase->OpenViewMatchingUintLC
            (
            TPtrC( WLAN_DEVICE_SETTINGS ),
            TPtrC( WLAN_DEVICE_SETTINGS_TYPE ),
            KWlanUserSettings
            );
    
    User::LeaveIfError( view->GotoFirstRecord() );
    view->ReadUintL( TPtrC( WLAN_BG_SCAN_INTERVAL ), scanRate );
    //ignore leave if there is no such column WLAN_SAVED_BG_SCAN_INTERVAL
    TRAP_IGNORE( view->ReadUintL( TPtrC( WLAN_SAVED_BG_SCAN_INTERVAL ), 
    							  iLastSavedScanInterval ) );
    
    if( 0 == iLastSavedScanInterval )
    	{
    	iLastSavedScanInterval = KWlanScanRateActive;
    	}
    CCHLOGSTRING3( 
    	"CCchWlanExtension::CheckAvailabilityStatusL: interval: %d, saved: %d", 
    	scanRate, iLastSavedScanInterval );
    CleanupStack::PopAndDestroy( view );
    CCHLOGSTRING( "CCchWlanExtension::CheckAvailabilityStatusL: OUT" );
    // Return scan rate value.
    return scanRate;
	}

// -----------------------------------------------------------------------------
// CCchWlanExtension::SetAvailabilitySwitchL
// Sets WLAN availability switch based on scan rate given.
// -----------------------------------------------------------------------------
void CCchWlanExtension::SetAvailabilitySwitchL( TUint32 aScanRate )
    {
    CCHLOGSTRING( "CCchWlanExtension::SetAvailabilitySwitchL: IN" );
    // Open WLAN setting table and record.
    CCommsDbTableView* view = iWLANCommsDatabase->OpenViewMatchingUintLC
        (
        TPtrC( WLAN_DEVICE_SETTINGS ),
        TPtrC( WLAN_DEVICE_SETTINGS_TYPE ),
        KWlanUserSettings
        );
        
    User::LeaveIfError( view->GotoFirstRecord() );
    User::LeaveIfError( view->UpdateRecord() );

    // Set new WLAN availability setting value to comms database.
    view->WriteUintL( TPtrC( WLAN_BG_SCAN_INTERVAL ), aScanRate );
    
    TInt round( 0 );
    TInt err( KErrNone );
       do
           {
           // Complete transaction procedure.
           err = view->PutRecordChanges();

           if ( KErrNone != err )
               {
               User::After( round * KRetryTimeout );
               round++;
               }
           }
       while ( KErrNone != err && round < KRetryCount);
          
     if ( KErrNone != err  )
        {
        CCHLOGSTRING( "CCchWlanExtension::SetAvailabilitySwitchL: PutRecordChanges was unsuccessful" );
        User::Leave( err );
        }
    
    CleanupStack::PopAndDestroy( view );
    
    // Notifying WLAN Engine about changes in settings.
    CWlanMgmtClient* wlanMgmtClient = CWlanMgmtClient::NewL();
    wlanMgmtClient->NotifyChangedSettings();
    delete wlanMgmtClient;
    CCHLOGSTRING( "CCchWlanExtension::SetAvailabilitySwitchL: OUT" );
    }
    
// End of file
