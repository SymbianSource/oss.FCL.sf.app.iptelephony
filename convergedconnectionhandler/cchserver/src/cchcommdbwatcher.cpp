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
* Description:  CCCHCommDbWatcher implementation
*
*/


// INCLUDE FILES
#include <cmdestination.h>
#include <cmconnectionmethoddef.h>
#include <cmpluginwlandef.h>
#include <centralrepository.h>
#include <commsdat.h>

#include "cchcommdbwatcher.h"
#include "cchcommdbwatcherobserver.h"
#include "cchlogger.h"
#include "d32dbms.h"

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES
// None

// CONSTANTS
// None

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
const TInt KDefaultGranularity = 5;
const TInt KMaxCheckAttempts   = 3;
// Repository for CommsDat
const TUid KCDCommsRepositoryId = { 0xCCCCCC00 };

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ============================= LOCAL FUNCTIONS =============================

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// CCCHCommDbWatcher::CCCHCommDbWatcher
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CCCHCommDbWatcher::CCCHCommDbWatcher( MCCHCommDbWatcherObserver& aObserver ) :
    CActive( CActive::EPriorityStandard ), 
    iObserver( aObserver ), 
    iTableId( KCDTIdIAPRecord )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CCCHCommsDbWatcher::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CCCHCommDbWatcher::ConstructL()
    {
    CCHLOGSTRING( "CCCHCommDbWatcher::ConstructL IN" );
    
    iRepository = CRepository::NewL( KCDCommsRepositoryId );
    iDestinations = RArray<TDestinationData>( KDefaultGranularity );
    
    InitializeDestination();
    TInt err( RequestNotifications() );
    User::LeaveIfError( err );
    CCHLOGSTRING( "CCCHCommDbWatcher::ConstructL OUT" );
    }

// ---------------------------------------------------------------------------
// CCCHCommsDbWatcher::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CCCHCommDbWatcher* CCCHCommDbWatcher::NewL( 
    MCCHCommDbWatcherObserver& aObserver )
    {
    CCCHCommDbWatcher* self = CCCHCommDbWatcher::NewLC( aObserver );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CCCHCommsDbWatcher::NewLC
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CCCHCommDbWatcher* CCCHCommDbWatcher::NewLC( 
    MCCHCommDbWatcherObserver& aObserver )
    {
    CCCHCommDbWatcher* self = new (ELeave) CCCHCommDbWatcher( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// Destructor
CCCHCommDbWatcher::~CCCHCommDbWatcher()
    {
    CCHLOGSTRING( "CCCHCommDbWatcher::~CCCHCommDbWatcher" );
    iDestinations.Close();
    // Cancel outstanding request, if exists
    Cancel();
    delete iRepository;
    if ( iCmmOpen )
        {
        iCmm.Close();
        }
    }

// ---------------------------------------------------------------------------
// CCCHCommsDbWatcher::RequestNotifications()
// 
// ---------------------------------------------------------------------------
//
TInt CCCHCommDbWatcher::RequestNotifications()
    {
    CCHLOGSTRING( "CCCHCommDbWatcher::RequestNotifications()" );   
    CCHLOGSTRING2( "Calling iRepository->NotifyRequest() for table 0x%08X", iTableId );
    TInt err = iRepository->NotifyRequest( iTableId, KCDMaskShowRecordType, iStatus );

    if ( KErrNone == err )
        {
        SetActive();
        }
    else
        {
        CCHLOGSTRING2( "ERROR, iRepository->NotifyRequest() <%d>", err );
        }
    return err;
    }

// ---------------------------------------------------------------------------
// CCCHCommsDbWatcher::CmManagerL
// 
// ---------------------------------------------------------------------------
//
RCmManager& CCCHCommDbWatcher::CmManagerL()
    {
    // Remove this method and open cmmanager connection in ConstructL
    // after cmmanager is refactored -> to become a server, schedule is unknown
    if ( !iCmmOpen )
        {
        CCHLOGSTRING( "CCCHCommDbWatcher::CmManagerL open connection" );
        iCmm.OpenL();
        iCmmOpen = ETrue;
        }
    
    return iCmm; 
    }

// ---------------------------------------------------------------------------
// CCCHCommsDbWatcher::GetWLANIapCountFromSnap
//
// ---------------------------------------------------------------------------
//
TInt CCCHCommDbWatcher::GetIapCountFromSnap( 
    TInt aSNAPId, 
    TBool aWLANIapsOnly,
    TBool aUpdateDestinations )
    {
    CCHLOGSTRING( "CCCHCommDbWatcher::GetIapCountFromSnap - Forced CheckIapsL" );

    // Remove this error check after cmmanager is refactored -> to become 
    // a server, schedule is unknown
    // Force destination update if we are pending unlock event
    if ( iLastError || iUseForce || aUpdateDestinations )
        {
        CCHLOGSTRING( "CCCHCommDbWatcher::GetIapCountFromSnap - Forced DESTINATION UPDATE" );
        InitializeDestination();
        iUseForce = EFalse;
        }
    
    TInt iaps( KErrNotFound );
    for ( TInt i = 0; iaps == KErrNotFound && i < iDestinations.Count(); i++ )
        {
        if ( iDestinations[ i ].iDestId == aSNAPId )
            {
            iaps = 0; //if destination is not found, KErrNotFound is returned
            if ( aWLANIapsOnly )
                {
                iaps = iDestinations[ i ].iWlanIaps;
                }
            else
                {
                iaps = iDestinations[ i ].iIapCount;
                }
            }
        }
        
    return iaps;
    }

// ---------------------------------------------------------------------------
// CCCHCommsDbWatcher::IsWLANAPL
//
// ---------------------------------------------------------------------------
//
TBool CCCHCommDbWatcher::IsWlanApL( 
    TInt aIapId )
    {
    CCHLOGSTRING( "CCCHCommDbWatcher::IsWLANAPL: IN" );
    TBool ret( EFalse );
    TUint32 bearer( KErrNone );

    // Bearer Type
    bearer = CmManagerL().ConnectionMethodL( aIapId ).GetIntAttributeL( 
            CMManager::ECmBearerType );
    ret = KUidWlanBearerType == bearer;

    CCHLOGSTRING3( "CCCHCommDbWatcher::IsWLANAPL: iap id: %d is wlan ap: %d", 
        aIapId, ret );
    return ret;
    }
   
// ---------------------------------------------------------------------------
// CCCHCommsDbWatcher::RunL
//
// ---------------------------------------------------------------------------
//
void CCCHCommDbWatcher::RunL()
    {
    if ( iStatus.Int() < KErrNone )
        {
        CCHLOGSTRING2( "CCCHCommDbWatcher::RunL: error <%d>", iStatus.Int() );
        iErrorCounter++;
        if ( iErrorCounter > KCchConnCenRepErrorThreshold )
            {
            CCHLOGSTRING2( "Over %d consecutive errors, stopping notifications permanently",
                    KCchConnCenRepErrorThreshold );
            return;
            }
        }
    else
        {
        iErrorCounter = 0;
        UpdateIapTable();
        }

    CCHLOGSTRING2( "CCCHCommDbWatcher::RunL: CenRep event 0x%08X", iStatus.Int() );

    TInt err( RequestNotifications() );
    User::LeaveIfError( err );
    }

// ---------------------------------------------------------------------------
// CCCHCommsDbWatcher::DoCancel
//
// ---------------------------------------------------------------------------
//
void CCCHCommDbWatcher::DoCancel()
    {
    iRepository->NotifyCancel( iTableId, KCDMaskShowRecordType );
    }

// ---------------------------------------------------------------------------
// CCCHCommsDbWatcher::RunError
//
// ---------------------------------------------------------------------------
//

TInt CCCHCommDbWatcher::RunError( TInt /*aError*/ )
    {
    TInt err( RequestNotifications() );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CCCHCommsDbWatcher::CheckIapsL
//
// ---------------------------------------------------------------------------
//
void CCCHCommDbWatcher::CheckIapsL( TBool& aChanged, TInt& aSNAPId )
	{
    CCHLOGSTRING2( "CCCHCommDbWatcher::CheckIapsL: IN Current iap count %d",
            CountIapsFromArray( iDestinations ) );


  	RArray<TUint32> destIdArray = RArray<TUint32>( KDefaultGranularity );
    CleanupClosePushL( destIdArray ); // CS:1
  	RArray<TDestinationData> currentDestinations = RArray<TDestinationData>( 
  		KDefaultGranularity ); 
    CleanupClosePushL( currentDestinations );	 // CS:2

    //Get count of iaps that do not belong to any destination
    RArray<TUint32> cmMethods = RArray<TUint32>( KDefaultGranularity );
    CleanupClosePushL( cmMethods ); // CS:3
    CmManagerL().ConnectionMethodL( cmMethods );
        
    TDestinationData destinationlessIaps( KErrNotFound, cmMethods.Count() );
    CCHLOGSTRING2( "CCCHCommDbWatcher::CheckIapsL: cmMethods count %d",
            cmMethods.Count() );
    for ( TInt i = 0; i < cmMethods.Count(); i++ )
        {
   	    RCmConnectionMethod cm = CmManagerL().ConnectionMethodL( cmMethods[ i ] );
        CleanupClosePushL( cm ); // CS:4
        
        if( KUidWlanBearerType == 
            cm.GetIntAttributeL( CMManager::ECmBearerType ) )
            {
            destinationlessIaps.iWlanIaps++;
            }
        CleanupStack::PopAndDestroy( &cm ); // CS:3
        }
        
    CleanupStack::PopAndDestroy( &cmMethods ); // CS:2
    currentDestinations.Append( destinationlessIaps ); 

    //Get count of iaps per destination
    CCHLOGSTRING2( "CCCHCommDbWatcher::CheckIapsL: cmMethods count %d",
            destIdArray.Count() )
    CmManagerL().AllDestinationsL( destIdArray );
    for ( TInt i = 0; i < destIdArray.Count(); i++ )
        {
        RCmDestination destination = CmManagerL().DestinationL( destIdArray[ i ] );
        CleanupClosePushL( destination ); // CS:3
        TDestinationData destinationInfo( 
            destination.Id(), destination.ConnectionMethodCount() );
		 
         for ( TInt k = 0; k < destinationInfo.iIapCount; k++ )
			 {
			 RCmConnectionMethod cm = destination.ConnectionMethodL( k );
			 CleanupClosePushL( cm ); // CS:4
             
			 if( KUidWlanBearerType == 
			 	 cm.GetIntAttributeL( CMManager::ECmBearerType ) )
				 {
				 destinationInfo.iWlanIaps++;				 	
				 }
		     CleanupStack::PopAndDestroy( &cm ); // CS:3
			 }
            
		 currentDestinations.Append( destinationInfo );
		 CleanupStack::PopAndDestroy( &destination ); // CS:2
	     }
    
	destIdArray.Close();
    
    UpdateDestinationStore( currentDestinations, aChanged, aSNAPId );
    
    CleanupStack::PopAndDestroy( &currentDestinations ); // CS:1
    CleanupStack::PopAndDestroy( &destIdArray ); // CS:0
	
    CCHLOGSTRING2( "CCCHCommDbWatcher::CheckIapsL: OUT changed: %d", aChanged );
	}

// ---------------------------------------------------------------------------
// CCCHCommsDbWatcher::CountIapsFromArray
//
// ---------------------------------------------------------------------------
//
TInt CCCHCommDbWatcher::CountIapsFromArray( 
    RArray<TDestinationData>& aArray ) const
	{
	TInt iaps = 0;
	for ( TInt i=0; i<aArray.Count(); i++ )
		{
		iaps+= aArray[i].iIapCount;
		}
	return iaps;	
	}

// ---------------------------------------------------------------------------
// CCCHCommsDbWatcher::UpdateDestinationStore
//
// ---------------------------------------------------------------------------
//
void CCCHCommDbWatcher::UpdateDestinationStore( 
    RArray<TDestinationData>& aDestinations,
    TBool& aChanged,
    TInt& aSNAPId )
    {
	//Now compare fetched data with store
	TInt iapsBefore = CountIapsFromArray ( iDestinations );
	TInt iapsAfter  = CountIapsFromArray ( aDestinations );

    aSNAPId = KErrNotFound;
    aChanged = EFalse;

    //no further checking is needed if iap count decreased
    if ( iapsAfter >= iapsBefore )
		{
        //compare store with current setup for changes
        //only change in WLAN iap count is reported
		for ( TInt i = 0; !aChanged && i < aDestinations.Count(); i++ )
			{
            TBool alreadyexists = EFalse;
			for ( TInt j = 0; !aChanged && j < iDestinations.Count(); j++ )
				{
                if ( aDestinations[i].iDestId == iDestinations[j].iDestId )
                    {
                    alreadyexists = ETrue;
                    // Do not update snap id if destination is -1, this means 
                    // that cmmanager hasn't moved iap to the right destination yet
                    if ( aDestinations[i].iWlanIaps > 
                         iDestinations[j].iWlanIaps )
					    {
                        if ( KErrNotFound == aDestinations[ i ].iDestId )
                            {
                            // If destination -1 is the only updated destination
                            // we have to use force next time when asking GetIapCountFromSnap
                            iUseForce = ETrue;
                            CCHLOGSTRING( "CCCHCommDbWatcher - USE THE FORCE NEXT TIME" );
                            }
                        else
                            {
                            //wlan iap count increased in comp[i]
                            aSNAPId = aDestinations[i].iDestId;
                            aChanged = ETrue;
                            iUseForce = EFalse;
                            CCHLOGSTRING2( "CCCHCommDbWatcher - snap id: %d", aSNAPId );
                            }
                        }
                    }
				}
                
            //if there is a new SNAP, check wlan iap count in the new IAP
            if ( !alreadyexists && aDestinations[i].iWlanIaps > 0 )
                {
                aSNAPId = aDestinations[i].iDestId;
                aChanged = ETrue;
                CCHLOGSTRING2( "CCCHCommDbWatcher new snap id: %d", aSNAPId );
                }
			}
		}
		
	//update store
	iDestinations.Reset();
	for ( TInt i=0; i < aDestinations.Count(); i++ )
		{
		iDestinations.Append( aDestinations[i] );					
        // Destinations are refreshed, set last error to kerrnone
        iLastError = KErrNone;
		}
    }

// ---------------------------------------------------------------------------
// CCCHCommsDbWatcher::UpdateIapTable
//
// ---------------------------------------------------------------------------
//
TInt CCCHCommDbWatcher::UpdateIapTable()
    {
    TBool configurationChanged( EFalse );
    TInt changedSnapId( KErrNotFound );
    TInt error( KErrNone );
    TRAP( error, CheckIapsL( configurationChanged, changedSnapId ) );
            
    if ( error == KErrNone )
        {
        if( configurationChanged )
            {
            // new IAP(s) added, notify observer
            iObserver.HandleWLANIapAdded( changedSnapId );
            }
        }
    
    CCHLOGSTRING2( "CCCHCommDbWatcher::UpdateIapTable: CActive iStatus : %d", iStatus.Int() );
    return error;
    }

// ---------------------------------------------------------------------------
// CCCHCommsDbWatcher::InitializeDestination
//
// ---------------------------------------------------------------------------
//
void CCCHCommDbWatcher::InitializeDestination()
    {
    //call CheckWLANIapsL to get initial iap/snap list to array 
    //we're not interested in the return value at this point
    TBool configurationChanged( EFalse );
    TInt changedSnapId( 0 );
    // Loop, because transaction might pend and error(KErrLocked) may occur
    TInt error( KErrNone );
    for ( TInt i( 0 ); i < KMaxCheckAttempts; i++ )
        {
        TRAP( error, CheckIapsL( configurationChanged, changedSnapId ) ); 
        CCHLOGSTRING2( 
            "CCCHCommDbWatcher::InitializeDestination; error: %d", error );
        if( KErrNone == error || KErrNotFound == error )
            {
            break;
            }
        }
        
    if( error )
        {
        iLastError = error;
        }
    }

// ========================== OTHER EXPORTED FUNCTIONS =======================

//  End of File
