/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "cchconnmonhandlernotifier.h"

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES
// None

// CONSTANTS
// None

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
const TInt KPeriodicTimerInterval( 5000000 ); // 5sec.

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
    iConnChangeListenerTimer = CPeriodic::NewL( CActive::EPriorityIdle );
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
    iConnChangeListenerTimer->Cancel();
    delete iConnChangeListenerTimer;
    StopNotify();
    Cancel();
    iPendingRequests.Close();
	iUnsolvedConnIds.Close();
    iConnIapIds.Close();
    iAvailableSNAPs.Close();
    iAvailableIAPs.Close();
    iConnMon.Close();
    }

// ---------------------------------------------------------------------------
// CCCHConnMonHandler::ScanNetworks
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHConnMonHandler::ScanNetworks(
    TBool aWlanScan, MCCHConnMonHandlerNotifier* aObserver )
    {
    CCHLOGSTRING( "CCCHConnMonHandler::ScanNetworks: IN" );
    
    if ( aWlanScan )
        {
        iNetworkScanningObserver = aObserver;
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
// CCCHConnMonHandler::SetSNAPsAvailabilityChangeListener
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHConnMonHandler::SetSNAPsAvailabilityChangeListener(
    MCCHConnMonHandlerNotifier* aObserver )
    {
    CCHLOGSTRING( "CCCHConnMonHandler::SetSNAPsAvailabilityChangeListener" );
    iSNAPsAvailabilityObserver = aObserver;
    if ( iSNAPsAvailabilityObserver )
        {
        iConnChangeListenerTimer->Start(
            KPeriodicTimerInterval,
            KPeriodicTimerInterval,
            TCallBack( PeriodicTimerCallBack, this ) );
        }
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

// ----------------------------------------------------------------------------
// CCCHConnMonHandler::PeriodicTimerCallBack
// The call back function.
// ----------------------------------------------------------------------------
//
TInt CCCHConnMonHandler::PeriodicTimerCallBack( TAny* aAny )
    {
    CCHLOGSTRING( "CCCHConnMonHandler::PeriodicTimerCallBack" );
    
    CCCHConnMonHandler* self = static_cast<CCCHConnMonHandler*>( aAny );
    self->iConnChangeListenerTimer->Cancel();
    
    if ( self->iSNAPsAvailabilityObserver )
        {
        self->iSNAPsAvailabilityObserver->
            SNAPsAvailabilityChanged( KErrTimedOut );
        }
    
    return KErrNone;
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
                if ( iNetworkScanningObserver )
                    {
                    iNetworkScanningObserver->NetworkScanningCompletedL(
                        iSNAPbuf(), KErrNone );
                    iNetworkScanningObserver = NULL;
                    }
                }
                break;
            
            case EGetIAP:
                {
                CCHLOGSTRING2( "CCCHConnMonHandler::RunL: iap: %d", iConnIapId );
                TCCHConnectionInfo info;
                info.iIapId  = iConnIapId;
                info.iConnId = iConnId;
                                    
                if ( KErrNotFound == iConnIapIds.Find( info ) )
                    {
                    iConnIapIds.Append( info );
                    }
                iConnId    = 0;
                iConnIapId = 0;
                
                CCHLOGSTRING2( "CCCHConnMonHandler::RunL: unsolved conn count: %d", iUnsolvedConnIds.Count() );
                if ( iUnsolvedConnIds.Count() )
                    {
                    GetIapId();
                    }
                }
                break;
                
            case EGetConnectionCount:
                {
                CCHLOGSTRING2( "CCCHConnMonHandler::RunL: conn count: %d", iConnCount );
                TBool familiar( EFalse );
                TUint connId( KErrNone );
                TUint subConnCount( KErrNone );
                for ( TInt i( 1 ); i <= iConnCount; i++ )
                    {
                    if ( !iConnMon.GetConnectionInfo( i, connId, subConnCount ) )
                        {
                        familiar = EFalse;
                        for ( TInt j( 0 ); j < iConnIapIds.Count(); j++ )
                            {
                            if ( connId == iConnIapIds[ j ].iConnId )
                                {
                                CCHLOGSTRING2( "CCCHConnMonHandler::RunL: iap %d is familiar connection", iConnIapIds[ j ].iIapId );
                                familiar = ETrue;
                                break;
                                }
                            }
                        
                        if ( !familiar && KErrNotFound == iUnsolvedConnIds.Find( connId ) )
                            {
                            iUnsolvedConnIds.Append( connId );
                            }
                        }
                    }
                iConnCount = 0;
                
                CCHLOGSTRING2( "CCCHConnMonHandler::RunL: unsolved conn count: %d", iUnsolvedConnIds.Count() );
                if ( iUnsolvedConnIds.Count() )
                    {
                    GetIapId();
                    }
                }
                break;
                
            default:
                break;
            }
        
        
        if ( iPendingRequests.Count() && !IsActive() )
            {
            CCHLOGSTRING2( "CCCHConnMonHandler::RunL: request pending : %d", 
                iPendingRequests[ 0 ] );
            switch ( iPendingRequests[ 0 ] )
                {
                case EGetIAP:
                    {
                    GetIapId();
                    }
                    break;
                        
                case EGetConnectionCount:
                    {
                    GetConnectionCount();
                    }
                    break;
                        
                default:
                    break;
                }
            
            iPendingRequests.Remove( 0 );
            iPendingRequests.Compress();
            }
        }
    else
        {
        if ( iNetworkScanningObserver )
            {
            iNetworkScanningObserver->NetworkScanningCompletedL(
                iSNAPbuf(), iStatus.Int() );
            iNetworkScanningObserver = NULL;
            }
        }
            
    CCHLOGSTRING( "CCCHConnMonHandler::RunL: OUT" );
    }

// ---------------------------------------------------------------------------
// CCCHConnMonHandler::StartMonitoringConnectionChanges
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHConnMonHandler::StartMonitoringConnectionChanges()
    {
    iConnIapIds.Reset();
    
    if ( !IsActive() )
        {
        GetConnectionCount();
        }
    else
        {
        iPendingRequests.Append( EGetConnectionCount );
        }
    }

// ---------------------------------------------------------------------------
// CCCHConnMonHandler::StopMonitoringConnectionChanges
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHConnMonHandler::StopMonitoringConnectionChanges( 
    RArray<TUint>& aIapIds )
    {
    aIapIds.Reset();
    for ( TInt i( 0 ); i < iConnIapIds.Count(); i++ )
        {
        if ( KErrNotFound == aIapIds.Find( iConnIapIds[ i ].iIapId ) )
            {
            aIapIds.Append( iConnIapIds[ i ].iIapId );
            }
        }
    }
	
// ---------------------------------------------------------------------------
// CCCHConnMonHandler::GetConnectionCount
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHConnMonHandler::GetConnectionCount()
    {
    iState = EGetConnectionCount;
    iConnMon.GetConnectionCount( iConnCount, iStatus ); 
    SetActive(); 
    }
     
// ---------------------------------------------------------------------------
// CCCHConnMonHandler::GetIapId
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHConnMonHandler::GetIapId()
    {
    if ( iUnsolvedConnIds.Count() )
        {
        iConnId = iUnsolvedConnIds[ 0 ];
        iUnsolvedConnIds.Remove( 0 );
        iUnsolvedConnIds.Compress();
        
        iState = EGetIAP;
        iConnMon.GetUintAttribute( iConnId, 0, KIAPId, iConnIapId, iStatus );
        SetActive();
        }
    }
            
// ---------------------------------------------------------------------------
// CCCHConnMonHandler::RemoveIapId
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHConnMonHandler::RemoveIapId(
    TUint aConnId )
    {
    for ( TInt i( 0 ); i < iConnIapIds.Count(); i++ )
        {
        if ( aConnId == iConnIapIds[ i ].iConnId )
            {
            iConnIapIds.Remove( i );
            iConnIapIds.Compress();
            break;
            }
        }
    }
	
// ---------------------------------------------------------------------------
// CCCHConnMonHandler::DoCancel
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHConnMonHandler::DoCancel()
    {
    iConnMon.CancelAsyncRequest( EConnMonGetPckgAttribute );
    if ( iNetworkScanningObserver )
        {
        iNetworkScanningObserver->NetworkScanningCompletedL(
            iSNAPbuf(), KErrCancel );
        iNetworkScanningObserver = NULL;
        }
    if ( iSNAPsAvailabilityObserver )
        {
        iSNAPsAvailabilityObserver->SNAPsAvailabilityChanged( KErrCancel );
        iSNAPsAvailabilityObserver = NULL;
        }
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
            
            if ( iSNAPsAvailabilityObserver )
                {
                iConnChangeListenerTimer->Cancel();
                iSNAPsAvailabilityObserver->SNAPsAvailabilityChanged( KErrNone );
                }
            }
            break;
			
        case EConnMonCreateConnection:
            {
            const CConnMonCreateConnection* eventCreate = NULL; 
            eventCreate = static_cast< const CConnMonCreateConnection* >(
                &aConnMonEvent );
            iUnsolvedConnIds.Append( eventCreate->ConnectionId() );
                        
            if ( !IsActive() )
                {
                GetIapId();
                }
            else
                {
                iPendingRequests.Append( EGetIAP );
                }
            }
            break;
            
        case EConnMonDeleteConnection:
            {
            const CConnMonDeleteConnection* eventDelete = NULL; 
            eventDelete = static_cast< const CConnMonDeleteConnection* >(
                &aConnMonEvent );
            TUint connId = eventDelete->ConnectionId();
             
            RemoveIapId( connId );
            }
            break;
            
        default:
            break;
        }
    CCHLOGSTRING( "CCCHConnMonHandler::EventL: OUT" );
    }

// ========================== OTHER EXPORTED FUNCTIONS =======================

//  End of File
