/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef C_CCCHUICONNECTIONHANDLER_H
#define C_CCCHUICONNECTIONHANDLER_H

#include <e32base.h>
#include <e32const.h>
#include <rconnmon.h>
#include <cmmanagerext.h>
#include <wlanmgmtcommon.h> // TWlanConnectionSecurityMode
#include <cmpluginwlandef.h>

#include "cchuicommon.hrh"
#include "mcchuitimerobserver.h"

class CCchUiCchHandler;
class CCchUiSpsHandler;
class RCmDestinationExt;
class CmConnectionMethodExt;
class CConnectionUiUtilities;


/**
 * Connection Handler.
 * This class implements the creation and altering of connections and SNAPs.
 *
 * @code
 *  // Create CCH handler and SPS handler and give them as parameters when
 *  // you create Connection  handler.
 *  iCCHHandler = CCchUiCchHandler::NewL();    
 *  iSpsHandler = CCchUiSpsHandler::NewL();
 *  iConnectionHandler = CCchUiConnectionHandler::NewL( 
 *       *iCCHHandler, *iSpsHandler );
 * @endcode
 *
 * @lib cchui.lib
 * @since S60 5.0
 */
NONSHARABLE_CLASS( CCchUiConnectionHandler ): public CBase
    {
public:
    
    /**
     * Two-phased constructor.
     *
     * @param aCchUiCchHandler CCH handler reference.
     * @param aSpsHandler Service settings handler reference.
     */
    static CCchUiConnectionHandler* NewL(
        CCchUiCchHandler& aCchUiCchHandler,
        CCchUiSpsHandler& aSpsHandler );
    
    /**
     * Two-phased constructor.
     *
     * @param aCchUiCchHandler CCH handler reference.
     * @param aSpsHandler Service settings handler reference.
     */
    static CCchUiConnectionHandler* NewLC(
        CCchUiCchHandler& aCchUiCchHandler,
        CCchUiSpsHandler& aSpsHandler );

    /**
     * Destructor.
     */
    virtual ~CCchUiConnectionHandler();
    
    
    /**
     * Displays a dialog to select one wlan access point from all
     * available and adds access point to snap (creates snap if
     * it doesn't exist).
     *
     * @since S60 5.0
     * @param aServiceId Service id.
     * @param aSnapId Snap id of the service. Snap is verified before use.
     * @param aServiceName Service name (used as snap name if needed)
     */
    void SearchAccessPointsL(
        TUint aServiceId, 
        TUint32 aSnapId, 
        const TDesC& aServiceName );
            
    /**
     * Copies iap from specific SNAP to another.
     *
     * @since S60 5.0
     * @param aServiceId service id
     * @param aServiceName service name
     * @param aSourceIap source IAP
     * @param aTargetSnap target SNAP
     */         
    void CopyIapToServiceSnapL( 
        TUint aServiceId,
        const TDesC& aServiceName,
        TUint32 aSourceIap, 
        TUint32 aTargetSnap );
    
    /**
     * Removes connection from specific service´s SNAP.
     *
     * @since S60 5.0
     * @param aServiceName service´s name
     * @param aIapId access point id to be removed
     */            
    void RemoveConnectionL( const TDesC& aServiceName, TInt aIapId );
    
    /**
     * Returns ETrue if wlan search (query) is ongoing.
     *
     * @since S60 5.0
     * @return ETrue if wlan search ongoing.
     */ 
    TBool SearchWlanOngoing();
       
private:

    CCchUiConnectionHandler(
        CCchUiCchHandler& aCchUiCchHandler,
        CCchUiSpsHandler& aSpsHandler );
    
    void ConstructL();
    
    /**
     * Checks if connection method already exists.
     *
     * @since S60 5.0
     * @param aDestination Target destination.
     * @param aSsid SSID of the connection method.
     * @param aSecurityMode Security mode of the candidate.
     * @param aAlreadyExists If access point already exists, this is ETrue.
     * @param aHidden Is candidate hidden or not.
     * @return TInt Iap id of the new connection method or KErrNotFound.
     */
    TInt ConnectionMethodAlreadyExistsL( 
        RCmDestinationExt& aDestination, 
        const TDesC& aSsid, 
        CMManager::TWlanSecMode aSecurityMode,
        TBool& aAlreadyExists,
        TBool aHidden );
    
    /**
     * Adds new connection method to snap.
     *
     * @since S60 5.0
     * @param aDestination Target destination.
     * @param aSsid SSID of the connection method.
     * @param aSecurityMode Security mode of the connection method.
     * @param aAlreadyExists If access point already exists, this is ETrue.
     * @param aSetHidden Set iap hidden.
     * @return TInt Iap id of the new connection method or old
     *         if already existed.
     */
    TInt AddNewConnectionMethodL( 
        RCmDestinationExt& aDestination,
        const TDesC& aSsid,
        TWlanConnectionSecurityMode aSecurityMode,
        TBool& aAlreadyExists,
        TBool aSetHidden );
    
    /**
     * Saves security settings to created connection method.
     *
     * @since S60 5.0
     * @param aSecMode Security mode.
     * @param aKey Key.
     * @param aHex Use hex.
     * @param aIapId Iap id.
     */
    void SaveSecuritySettingsL(
        const TWlanConnectionSecurityMode aSecMode, 
        const HBufC* aKey, 
        const TBool aHex, 
        const TUint32 aIapId );
            
    /**
     * Sets specific SNAP in use for specific service.
     *
     * @since S60 5.0
     * @param aServiceId Service id of the service to be handled.
     * @param aSNAPId Snap id to set.
     */
    void SetSnapToUseL( 
        TUint aServiceId, 
        TUint32 aSNAPId );    
       
    /**
     * Creates SNAP for service. Ownership is passed.
     *
     * @since S60 5.0
     * @param aServiceName service name
     * @return Created destination, ownership is passed.
     */            
    RCmDestinationExt CreateServiceSnapL( 
        const TDesC& aServiceName );
                
private: // data

    /**
     * Reference to CCH handler.
     * Not own.
     */
    CCchUiCchHandler& iCCHHandler;
    
    /**
     * Reference to service settings handler.
     * Not own.
     */
    CCchUiSpsHandler& iSpsHandler;
    
    /**
     * Pointer to connection ui utilities.
     * Own.
     */
    CConnectionUiUtilities* iConnUiUtils;
        
    /**
     * Handle to connection method manager.
     * Own.
     */
    RCmManagerExt iCmManagerExt;
    
    /**
     * Handle to connection monitor. Required to resolve hidden/public status.
     * Own.
     */
    RConnectionMonitor iConnMon;
    
    /**
     * Flag for determining if search wlan search (query) is ongoing
     */
    TBool iSearchWlanOngoing;
    
    CCHUI_UNIT_TEST( UT_CchUi )
    };

#endif // C_CCCHUICONNECTIONHANDLER_H
