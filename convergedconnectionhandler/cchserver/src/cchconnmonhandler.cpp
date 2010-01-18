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
* Description:  CCCHConnMonHandler implementation
*
*/


// INCLUDE FILES
#include "cchconnmonhandler.h"
#include "cchserverbase.h"
#include "cchlogger.h"
#include "cchservicehandler.h"

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES
// None

// CONSTANTS
// None

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ============================= LOCAL FUNCTIONS =============================

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// CCCHConnMonHandler::CCCHConnMonHandler
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CCCHConnMonHandler::CCCHConnMonHandler( CCCHServerBase& aServer ) :
    CActive( CActive::EPriorityStandard ),
    iState( EUninitialized ),
    iServer( aServer )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CCCHConnMonHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CCCHConnMonHandler::ConstructL()
    {
    User::LeaveIfError( iConnMon.ConnectL() );
    NotifyL();
    }

// ---------------------------------------------------------------------------
// CCCHConnMonHandler::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CCCHConnMonHandler* CCCHConnMonHandler::NewL( CCCHServerBase& aServer )
    {
    CCCHConnMonHandler* self = CCCHConnMonHandler::NewLC( aServer );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CCCHConnMonHandler::NewLC
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CCCHConnMonHandler* CCCHConnMonHandler::NewLC( CCCHServerBase& aServer )
    {
    CCCHConnMonHandler* self = new (ELeave) CCCHConnMonHandler( aServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// Destructor
CCCHConnMonHandler::~CCCHConnMonHandler()
    {
    CCHLOGSTRING( "CCCHConnMonHandler::~CCCHConnMonHandler" );
    
    StopNotify();
    Cancel();
    iAvailableSNAPs.Close();
    iAvailableIAPs.Close();
    iConnMon.Close();
    }

// ---------------------------------------------------------------------------
// CCCHConnMonHandler::ScanNetworks
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHConnMonHandler::ScanNetworks( TBool aWlanScan )
    {
    CCHLOGSTRING( "CCCHConnMonHandler::ScanNetworks: IN" );
    
    if ( aWlanScan )
        {
        GetIaps( EBearerIdAll );
        }
            
    CCHLOGSTRING( "CCCHConnMonHandler::ScanNetworks: OUT" );
    }

// ---------------------------------------------------------------------------
// CCCHConnMonHandler::ScanNetworksCancel
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHConnMonHandler::ScanNetworksCancel()
    {
    Cancel();
    }

// ---------------------------------------------------------------------------
// CCCHConnMonHandler::IsSNAPAvailable
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CCCHConnMonHandler::IsSNAPAvailable( TUint aSNAPId ) const
    {    
    return ( KErrNotFound == iAvailableSNAPs.Find( aSNAPId ) ) 
        ? EFalse : ETrue; 
    }

// ---------------------------------------------------------------------------
// CCCHConnMonHandler::IsIapAvailable
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CCCHConnMonHandler::IsIapAvailable( TUint aIapId ) const
    {
    return ( KErrNotFound == iAvailableIAPs.Find( aIapId ) ) 
        ? EFalse : ETrue; 
    }

// ---------------------------------------------------------------------------
// CCCHConnMonHandler::GetIaps
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHConnMonHandler::GetIaps( TConnMonBearerId aBearerId )
    {
    if ( !IsActive() )
        {
        iConnMon.GetPckgAttribute( aBearerId,
                                   0,
                                   KIapAvailability,
                                   iIapsBuf,
                                   iStatus );
        iState = EGetIAPS;
        SetActive();
        }
    }

// ---------------------------------------------------------------------------
// CCCHConnMonHandler::UpdateIapArray
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHConnMonHandler::UpdateIapArray( TConnMonIapInfo aIaps )
    {
    CCHLOGSTRING2( "CCCHConnMonHandler::UpdateIapArray: IN count %d",
        iAvailableIAPs.Count() );
    
    iAvailableIAPs.Reset();
    TUint count( 0 );

    // Copy TConnMonIapInfo to RArray so we can use RArray::Find method
    while ( count < aIaps.iCount )
        {
        iAvailableIAPs.Append( aIaps.iIap[ count ].iIapId );
        count++;
        }
    CCHLOGSTRING2( "CCCHConnMonHandler::UpdateIapArray: OUT count %d",
        iAvailableIAPs.Count() );
    }

// ---------------------------------------------------------------------------
// CCCHConnMonHandler::GetSNAPs
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHConnMonHandler::GetSNAPs()
    {
    CCHLOGSTRING( "CCCHConnMonHandler::GetSNAPs: IN" );
    
    if ( !IsActive() )
        {
        iConnMon.GetPckgAttribute( EBearerIdAll, // some parameter
                                   0, // pass the size of buffer
                                   KSNAPsAvailability, // specify the request
                                   iSNAPbuf, // buffer for writing data
                                   iStatus );
        iState = EGetSNAPs;
        SetActive();
        }
    
    CCHLOGSTRING( "CCCHConnMonHandler::GetSNAPs: OUT" );
    }

// ---------------------------------------------------------------------------
// CCCHConnMonHandler::UpdateSnapArray
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHConnMonHandler::UpdateSnapArray( TConnMonSNAPInfo aSNAPs )
    {
    CCHLOGSTRING2( "CCCHConnMonHandler::UpdateSnapArray: IN count %d",
        iAvailableSNAPs.Count() );
   
    iAvailableSNAPs.Reset();
    TUint count( 0 );
    // Copy TConnMonSNAPInfo to RArray so we can use RArray::Find method
    while ( count < aSNAPs.iCount )
        {
        iAvailableSNAPs.Append( aSNAPs.iSNAP[ count ].iSNAPId );
        count++;
        }        
    CCHLOGSTRING2( "CCCHConnMonHandler::UpdateSnapArray: OUT count %d",
        iAvailableSNAPs.Count() );
    
    }
 
// ---------------------------------------------------------------------------
// CCCHConnMonHandler::NotifyL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHConnMonHandler::NotifyL()
    {
    CCHLOGSTRING( "CCCHConnMonHandler::NotifyL: IN" );
    
    iConnMon.NotifyEventL( *this );

    // Thresholds 
    TInt err = iConnMon.SetUintAttribute( EBearerIdAll,
                                          0,
                                          KBearerAvailabilityThreshold,
                                          1 );
                                          
    CCHLOGSTRING2( "CCCHConnMonHandler::NotifyL: OUT", err );
    }
    
// ---------------------------------------------------------------------------
// CCCHConnMonHandler::StopNotify
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHConnMonHandler::StopNotify()
    {
    iConnMon.CancelNotifications();
    }

// ---------------------------------------------------------------------------
// CCCHConnMonHandler::RunL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHConnMonHandler::RunL()
    {
    CCHLOGSTRING2( "CCCHConnMonHandler::RunL: IN error: %d", iStatus.Int() );
    
    if ( KErrNone == iStatus.Int() )
        {
        switch ( iState )
            {
            case EGetIAPS:
                {
                UpdateIapArray( iIapsBuf() );
                GetSNAPs();
                }
            break;
            case EGetSNAPs:
                {
                UpdateSnapArray( iSNAPbuf() );
                }
            break;
            default:
                break;
            }
        }
            
    CCHLOGSTRING( "CCCHConnMonHandler::RunL: OUT" );
    }

// ---------------------------------------------------------------------------
// CCCHConnMonHandler::DoCancel
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHConnMonHandler::DoCancel()
    {
    iConnMon.CancelAsyncRequest( EConnMonGetPckgAttribute );
    }

// ---------------------------------------------------------------------------
// CCCHConnMonHandler::EventL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHConnMonHandler::EventL( const CConnMonEventBase &aConnMonEvent )
    {
    CCHLOGSTRING3
        ( "CCCHConnMonHandler::EventL: IN EventType = %d, ConnectionId = %d", 
            aConnMonEvent.EventType(), aConnMonEvent.ConnectionId() );
    
    switch ( aConnMonEvent.EventType() )
        {
        case EConnMonIapAvailabilityChange:
            {
            const CConnMonIapAvailabilityChange* eventIap = NULL;
            eventIap = static_cast< const CConnMonIapAvailabilityChange* >(
                &aConnMonEvent );

            TConnMonIapInfo iaps = eventIap->IapAvailability();
            UpdateIapArray( iaps );
            }
            break;

        case EConnMonSNAPsAvailabilityChange:
            {
            const CConnMonSNAPsAvailabilityChange* eventSNAP = NULL;
            eventSNAP = static_cast< const CConnMonSNAPsAvailabilityChange* >(
                 &aConnMonEvent );

            TConnMonSNAPInfo snaps  = eventSNAP->SNAPAvailability();
            UpdateSnapArray( snaps );
            }
            break;
        default:
            break;
        }
    CCHLOGSTRING( "CCCHConnMonHandler::EventL: OUT" );
    }

// ========================== OTHER EXPORTED FUNCTIONS =======================

//  End of File
