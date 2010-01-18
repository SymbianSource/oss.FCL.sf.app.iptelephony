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
* Description:  CCCHCommDbWatcher declaration
*
*/


#ifndef C_CCHCOMMDBWATCHER_H
#define C_CCHCOMMDBWATCHER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <commdb.h>
#include <cmmanager.h>
#include <commsdattypesv1_1.h>

// CONSTANTS
// Stop requesting new notifications after this many consecutive errors
const TInt KCchConnCenRepErrorThreshold = 80;

// MACROS
// None

// DATA TYPES
class TDestinationData
	{
	public:
	inline TDestinationData(): iDestId(0), iIapCount(0), iWlanIaps(0){}
	inline TDestinationData(TInt aDestId, TInt aIapCount):
		iDestId(aDestId), iIapCount(aIapCount), iWlanIaps(0){}
	TInt iDestId;	//CMM Destination ID
	TInt iIapCount; //Number of IAPs under this destination
	TInt iWlanIaps; //Number of WLAN IAPs under this destination
	};

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class MCCHCommDbWatcherObserver;
class CRepository;

// CLASS DECLARATION

/**
 *  CCCHCommDbWatcher declaration
 *  Handles CommsDb changes and notifies CCHServiceHandler at IAP count changes
 *  @lib cchserver.exe
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CCCHCommDbWatcher ) : public CActive
    {

public: // Constructors and destructor

    static CCCHCommDbWatcher* NewL( MCCHCommDbWatcherObserver& aObserver );

    static CCCHCommDbWatcher* NewLC( MCCHCommDbWatcherObserver& aObserver );

    ~CCCHCommDbWatcher();

public: // new functions
	
    /**
     * Returns number of IAPs that have WLAN bearer type
     * @param aSNAPId ID of SNAP to check. Use value KErrNotFound 
     *        to get number of WLAN IAPS without SNAP binding
     * @param aWLANIapsOnly if ETrue, returns WLAN iap count in given SNAP
     * @param aUpdateDestinations if ETrue, update destination before checking  
     * @return number of IAPs in given SNAP. KErrNotFound if no IAPS
     * @since Series 60 3.2
     */
    TInt GetIapCountFromSnap( TInt aSNAPId, TBool aWLANIapsOnly, TBool aUpdateDestinations = EFalse );
     
    /**
     * Checks is IAP's bearer type WLAN AP.
     * @param aIapId of IAP to check.
     * @return ETrue if given IAP id is WLAN AP, otherwise EFalse.
     * @since Series 60 3.2
     */
    TBool IsWlanApL( TInt aIapId );

protected: // From base classes

    /**
     * From CActive
     * @since Series 60 3.2
     */
    void RunL();

    /**
     * From CActive
     * @since Series 60 3.2
     */
    TInt RunError( TInt aError ); 

    /**
     * From CActive
     * @since Series 60 3.2
     */
    void DoCancel();

private:

    /**
     * C++ default constructor.
     */
    CCCHCommDbWatcher( MCCHCommDbWatcherObserver& aObserver );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();
    
    /*
     * Request notifications of cenrep changes
	 * @return KErrNone if succesful
     */
    TInt RequestNotifications();
    
    /**
     * Check changes in IAPs / SNAP configurations
     * @param aChanged will be ETrue if there has been changes to 
     *  either total WLAN AP count or SNAP/IAP configuration
     * @param aSNAPId will contain ID of the changed SNAP that 
     *  contains now more WLAN Iaps
     */
    void CheckIapsL( TBool& aChanged, TInt& aSNAPId );
    
	/**
	 * Count number of Iaps in an array of TDestinationData.
	 * Helper function to detect new iap additions
	 * @param aArray Array to count from
	 * @return total number of IAPs from the given destinations array
     * @since S60 3.2
	 */
    TInt CountIapsFromArray( RArray<TDestinationData>& aArray ) const;

	/**
	 * Updates internal iDestinations store after a SNAP configuration change.
     * Determines also the SNAP where WLAN iap(s) were added.
	 * @param aDestinations current Snap configuration attained from cmmanager
     * @param aChanged will be ETrue if there is a new WLAN iap
     * @param aSNAPId holds the SNAP id of the added WLAN iap
     * @since S60 3.2
	 */
    void UpdateDestinationStore( RArray<TDestinationData>& aDestinations,
        TBool& aChanged,
        TInt& aSNAPId);
    
    /**
     * Updates IAPs / SNAPs configurations and notifies our client if 
     * there is some changes.
     * @return TInt general symbian error code.
     */
    TInt UpdateIapTable();
    
    /**
     * Open connection to cmmanager 
     * @return RCmManager reference to cmmanager
     */
    RCmManager& CmManagerL();
    
    /**
     * Initializes destinations. 
     */
    void InitializeDestination();

private: // data

	///not own: observer to notify when there are changes in WLAN IAP/SNAPs
	MCCHCommDbWatcherObserver& iObserver;

	///own: Commsdb access to get notifications about changes
    CCommsDatabase*				iCommDb;
    
    // Central repository to get notifications of commsdat changes
    CRepository* iRepository;
    
    // Table id for iap table
    TUint32 iTableId;
    
	///own: array holding information about iap count per destination
	//      index 0 will contain count of iaps not bound to any
	//      destination
	RArray<TDestinationData>   iDestinations;

	//own, connectionmethodmanager
	RCmManager				   iCmm;
	
	// indicates is cmmanager open
	TBool                      iCmmOpen;
	
	// Holds the last cmmanager error
	TInt                       iLastError;
	
	// Error counter for cenrep notification request errors
	TInt                       iErrorCounter;
	
	// Indicates do we have to use force when asking snap's iap
	TBool                      iUseForce;
    };

#endif // C_CCHCOMMDBWATCHER_H

// End of file
