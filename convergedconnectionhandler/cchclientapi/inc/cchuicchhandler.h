/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef C_CCCHUICCHHANDLER_H
#define C_CCCHUICCHHANDLER_H

#include <cch.h>
#include "cchuicommon.hrh"

/**
 * CCH Handler.
 * This class implements CCH handling.
 *
 * @code
 *  // Create intance and get snap used by service.
 *  iCCHHandler = CCchUiCchHandler::NewL();
 *  TInt serviceId( KServiceId );
 *  TInt snapId( KErrNone );
 *  iCCHHandler->GetConnectionSnapId( serviceId, snapId );
 * @endcode
 *
 * @lib cchui.lib
 * @since S60 5.0
 */
NONSHARABLE_CLASS( CCchUiCchHandler ) : public CBase
    {    
public:
            
    /**
     * Two-phased constructor.
     */
    static CCchUiCchHandler* NewL( CCch& aCch  );
    
    /**
     * Destructor.
     */
    virtual ~CCchUiCchHandler();

    
    /**
     * Get snap id.
     *
     * @since S60 5.0
     * @param aServiceId Service id.
     * @param aSnapId Snap id will be stored here.
     * @param aErr error code
     */
    void GetConnectionSnapIdL( TInt aServiceId, TInt& aSnapId, TInt& aErr );
    
    /**
     * Set snap id.
     *
     * @since S60 5.0
     * @param aServiceId Service id.
     * @param aSnapId Snap id to be set.
     * @param aErr error code.
     */
    void SetConnectionSnapIdL( TInt aServiceId, TInt aSnapId, TInt& aErr );
           
    /**
     * Get service username.
     *
     * @since S60 5.0
     * @param aServiceId Service id.
     * @param aUserName service username.
     * @param aErr error code.
     */
    void GetUsernameL( TInt aServiceId, RBuf& aUserName, TInt& aErr );
    
    /**
     * Set username for service.
     *
     * @since S60 5.0
     * @param aResultParams Contains needed info to set username.
     * @param aErr
     */
    void SetUsernameL( TCCHUiNotifierParams aResultParams, TInt& aErr );
    
    /**
     * Set password for service.
     *
     * @since S60 5.0
     * @param aResultParams Contains needed info to set password.
     * @param aErr error code.
     */
    void SetPasswordL( TCCHUiNotifierParams aResultParams, TInt& aErr );
    
    /**
     * Starts observing for service events for service specified
     * by aServiceId.
     *
     * @since S60 5.0
     * @param aServiceId service identifier
     * @param aObserver observer for service events
     */    
    void StartObservingL( 
            TUint aServiceId, 
            MCchServiceStatusObserver& aObserver );
        
    /**
     * Stops observing for specific service defined by aServiceId
     *
     * @since S60 5.0
     * @param aServiceId service identifier
     * @param aObserver observer for service events
     */            
    void StopObservingL( 
            TUint aServiceId, 
            MCchServiceStatusObserver& aObserver );
    
    /**
     * For checking service state.
     *
     * @since S60 5.0
     * @param aServiceId service identifier
     * @param aSubServiceType subservice type
     * @param aErr Symbian error code
     * @return subservice state
     */            
    TCCHSubserviceState GetServiceStateL( 
            TUint aServiceId, 
            TCCHSubserviceType aSubServiceType,
            TInt& aErr );
    
    /**
     * For getting current connections iap id.
     * Is left as zero if no current connection exists.
     *
     * @since S60 5.0
     * @param aServiceId service identifier
     * @param aSubServiceType subservice type
     * @param aIapId current connection iap id to be stored here
     * @param aErr error code to be stored here
     */            
    void GetCurrentConnectionIapIdL( 
            TUint aServiceId, 
            TCCHSubserviceType aSubServiceType,
            TInt& aIapId,
            TInt& aErr );
    
    /**
     * Enables service
     *
     * @since S60 5.0
     * @param aServiceId service identifier
     */                
    void EnableL( TUint aServiceId );

    /**
     * Disables service
     *
     * @since S60 5.0
     * @param aServiceId service identifier
     */                
    void DisableL( TUint aServiceId );    
    
    /**
     * For checking if all subservices are disabled
     *
     * @since S60 5.0
     * @param aServiceId service identifier
     * @param aState checked state
     * @return ETrue if all subservices are in checked state
     */                
    TBool AllSubservicesInStateL( TUint aServiceId, 
        TCCHSubserviceState aState );  
        
private:

    CCchUiCchHandler( CCch& aCch );    
 
private: // data
    
    /**
     * Handle to CCH Client API.
     * Not own.
     */
    CCch& iCchClientApi;
    
    CCHUI_UNIT_TEST( UT_CchUi )
    };

#endif // C_CCCHUICCHHANDLER_H
