/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#ifndef C_CCCHUINOTIFCONNECTIONHANDLER_H
#define C_CCCHUINOTIFCONNECTIONHANDLER_H

#include <badesca.h>
#include <e32base.h>
#include <e32const.h>
#include <rconnmon.h>
#include <cmmanagerext.h>
#include <wlanmgmtcommon.h> // TWlanConnectionSecurityMode
#include <cmpluginwlandef.h>

#include "cchuicommon.hrh"

class RCmDestinationExt;
class CmConnectionMethodExt;

/**
 * Connection Handler for notifier plugin.
 * This class implements the access to SNAPs.
 *
 * @code
 * @endcode
 *
 * @lib cchui.lib
 * @since S60 5.0
 */
NONSHARABLE_CLASS( CCchUiNotifConnectionHandler ): public CBase
    {
public:
    
    /**
     * Two-phased constructor.
     */
    static CCchUiNotifConnectionHandler* NewL();
    
    /**
     * Two-phased constructor.
     */
    static CCchUiNotifConnectionHandler* NewLC();

    /**
     * Destructor.
     */
    virtual ~CCchUiNotifConnectionHandler();
    
    
    /**
     * Gets all gprs access points in internet snap to array.
     *
     * @since S60 5.0
     * @param aIaps will store iap names
     * @param aIapIds will store iap ids
     */
    TUint32 GetGprsAccessPointsL( 
        CDesCArray& aIaps, 
        RArray<TUint32>& aIapIds );
    
    /**
     * Gets connection name.
     *
     * @since S60 5.0
     * @param aIapid current connection iap id
     * @param aServiceName service name
     * @param aIapName accesspoint name to be stored here
     */
    void ConnectionNameL(  
        TUint aIapid, 
        TDesC& aServiceName,
        RBuf& aIapName );
    
    /**
     * Gets all but specified service´s destinations.
     *
     * @since S60 5.0
     * @param aServiceName services name which destination can be ignored.
     * @param aDestinationIds will store destination ids
     */    
    void GetDestinationsL(
        TDesC& aServiceName,
        RArray<TUint32>& aDestinationIds );

    /**
     * Gets service´s destination Id where the given iapid belongs.
     *
     * @since S60 9.2
     * @param aIapId internet accesspoint Id.
     * @param aError error code in return 
     * @param aDestinationId returns destination id
     */
    void GetDestinationL( 
        TUint aIapId, TInt& aError, TUint32& aDestinationId );
    
    /**
     * Gets all access point ids from snap.
     *
     * @since S60 5.0
     * @param aIapIds will store iap ids
     * @param aDestinationId id of destination to use
     */
    void GetAccessPointsFromSnapL(
        RArray<TUint32>& aIapIds, 
        TUint32 aDestinationId );
    
    /**
     * Gets all gprs accesspoints id's from the service's snap.
     *
     * @since S60 9.2
     * @param aIapIds will store iap ids
     * @param aDestinationId id of destination to use
     * @return error. KErrNotFound if no gprs accesspoints
     */
    TInt GetGprsAccessPointsSetToServiceSnapL(
        CDesCArray& aIaps, RArray<TUint32>& aIapIds, TUint aIapId );
    
    /**
     * Checks given connection methods equality   
     *
     * @since S60 9.2
     * @param aIapId Iap Id.
     * @param aIapIdToCompare Iap Id to compare.
     * @return ETrue if equals
     */
    TBool IsConnectionMethodSimilarL(
        TUint32 aIapId, TUint32 aIapIdToCompare );
    
private:

    CCchUiNotifConnectionHandler();
    
    void ConstructL();
        
    /**
     * Fills GPRS access point names and ids from provided destination.
     *
     * @since S60 5.0
     * @param aIaps will store iap names
     * @param aIapIds will store iap ids
     * @param aDestination destination to use
     */    
    void GetGprsAccessPointsFromSnapL( 
        CDesCArray& aIaps, RArray<TUint32>& aIapIds, 
        RCmDestinationExt& aDestination );

private: // data

    /**
     * Handle to connection method manager.
     * Own.
     */
    RCmManagerExt iCmManagerExt;
    
    CCHUI_UNIT_TEST( T_CCchUiNotifConnectionHandler )
    };

#endif // C_CCCHUINOTIFCONNECTIONHANDLER_H
