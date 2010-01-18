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
* Description:  CCCHConnMonHandler declaration
*
*/


#ifndef C_CCHCONNMONHANDLER_H
#define C_CCHCONNMONHANDLER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <rconnmon.h>

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CCCHServerBase;

// CLASS DECLARATION

/**
 *  CCCHConnMonHandler declaration
 *  Handles Connection Monitor for CCH server
 *  @lib cchserver.exe
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CCCHConnMonHandler ) : public CActive,
                                          private MConnectionMonitorObserver
    {
public: // Constructors and destructor

    /**
     * State's of object
     */
    enum TCCHConnMonHandlerState
        {
          EUninitialized,   /// Uninitialized
          EInitialized,     /// Initalized
          EGetIAPS,
          EGetSNAPs,
          EError            /// Error condition
        };

    /**
     * Two-phased constructor.
     */
    static CCCHConnMonHandler* NewL( CCCHServerBase& aServer );

    /**
     * Two-phased constructor.
     */
    static CCCHConnMonHandler* NewLC( CCCHServerBase& aServer );

    /**
     * Destructor.
     */
    virtual ~CCCHConnMonHandler();

public: // New functions

    /**
     * Scan available networks
     * @since S60 3.2
     * @param aWlanScan ETrue if WLAN network scan needed
     */
    void ScanNetworks( TBool aWlanScan = EFalse );

    /**
     * Cancel network scanning
     * @since S60 3.2
     */
    void ScanNetworksCancel();

    /**
     * Is Snap available
     * @since S60 3.2
     * @param aSNAPId
     * @return ETrue if SNAP is available
     */
    TBool IsSNAPAvailable( TUint aSNAPId ) const;

    /**
     * Is IAP available
     * @since S60 3.2
     * @param aIapId
     * @return ETrue if IAP is available
     */
    TBool IsIapAvailable( TUint aIapId ) const;

protected: // From base classes

    /**
     * From CActive
     * @since Series 60 3.0
     */
    void RunL();

    /**
     * From CActive
     * @since Series 60 3.0
     */
    void DoCancel();

private: // From MConnectionMonitorObserver

    /**
     * @see MConnectionMonitorObserver, called when ConnMon event occured.
     */
    void EventL( const CConnMonEventBase &aConnMonEvent );

private:

    /**
     * C++ default constructor.
     */
    CCCHConnMonHandler( CCCHServerBase& aServer );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * Get IAPs from Connection Monitor and performs network scan
     * @since S60 3.2
     * @param aBearerId Bearer specific connection id.
     */
    void GetIaps( TConnMonBearerId aBearerId );

    /**
     * Updates available IAP Array
     * @since S60 3.2
     * @param aIaps available IAPs
     */
    void UpdateIapArray( TConnMonIapInfo aIaps );

    /**
     * Get SNAPs from Connection Monitor
     * @since S60 3.2
     */
    void GetSNAPs();

    /**
     * Updates available SNAP array
     * @since S60 3.2
     * @param aSNAPs available SNAPs
     */
    void UpdateSnapArray( TConnMonSNAPInfo aSNAPs );

    /**
     * Request SNAP/IAP availability notifications from Connection Monitor.
     * @since S60 3.2
     */
    void NotifyL() ;

    /**
     * Stops notifications.
     * @since S60 3.2
     */
    void StopNotify();

private: // data

    /**
     * State of active object
     */
    TCCHConnMonHandlerState         iState;

    /**
     * Handle to server
     */
    CCCHServerBase&                 iServer;

    /**
     * Connection Monitor
     */
    RConnectionMonitor              iConnMon;

    /**
     * Package buffer for getting available IAPs
     */
    TConnMonIapInfoBuf              iIapsBuf;

    /**
     * Package buffer for getting available SNAPs
     */
    TConnMonSNAPInfoBuf             iSNAPbuf;

    /**
     * SNAP array
     */
    RArray<TUint>                   iAvailableSNAPs;

    /**
     * IAP Array
     */
    RArray<TUint>                   iAvailableIAPs;

    };

#endif // C_CCHCONNMONHANDLER_H

// End of file
