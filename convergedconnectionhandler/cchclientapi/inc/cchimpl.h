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
* Description:  cch client api implementation class
*
*/



#ifndef C_CCHIMPL_H
#define C_CCHIMPL_H

#include <e32base.h>
#include <ccherror.h>
#include <cchtypes.h>
#include "cch.h"
#include "cchclient.h"
#include "cchuiprivateapi.h"
#include "cchuicommon.hrh"

class CCchService;
class CCchServiceImpl;

// flag cchui support on. This is temporary fix until cchui is integrated into builds. 
#define CCHAPI_USE_CCHUI

#ifdef CCHAPI_USE_CCHUI
class CCchUiPrivateApi;
#else 
#define CCchUiPrivateApi TInt 
#endif

// DATA TYPES
typedef RPointerArray<CCchServiceImpl> RServiceImplArray;

/**
 *  Class for basic cch functionality, getting services
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib cch
 */
class CCchImpl : public CActive 
    {

public:

    /**
     * Two-phased constructor.
     */
    static CCchImpl* NewL( CCch& aCch );
    
    /**
     * Two-phased constructor.
     */
    static CCchImpl* NewLC( CCch& aCch );
    
    /**
     * Destructor.
     */
    ~CCchImpl();
    
    /**
     * Returns the RCCHClient
     * 
     * @return The used client
     */
    RCCHClient& CchClient();
    
    /**
     * Returns one service
     *
     * @param aId Service id
     * @return Cch service or NULL if service not found
     */
    CCchService* GetService( TInt aId );
    
    /**
     * Returns service ids of all configured services
     *
     * @param  aIds Array of TInt to be filled with service ids
     * @return Symbian error code
     */
    TInt GetServiceIds( RArray<TInt>& aIds );
    
    /**
     * Returns services fulfilling the search criteria
     *
     * @param aType Service type
     * @param aServices Array of CCchService to be filled 
     * @return Symbian error code
     */
    TInt GetServices( TCCHSubserviceType aType, 
                               RPointerArray<CCchService>& aServices );
    
    
    /**
     * Returns ETrue if connectivity dialogs are allowed in service enabling.
     *
     * @return ETrue if connectivity dialogs are allowed in service enabling
     */
    TBool ConnectivityDialogsAllowed() const;
        
    
    /**
     * For allowing/disallowing connectivity dialogs showing in 
     * service enabling.
     * 
     * @param aConnectivityDialogsAllowed Boolean to set connectivity dialogs
     *        allowed/disallowed
     */ 
    void SetConnectivityDialogsAllowed( TBool aConnectivityDialogsAllowed );
    
    
    /**
     * For getting cchui api.
     * 
     * @return reference to cchui api.
     */
    MCchUi& CchUiApi() const;
    
    
    /**
     * Finds out if corresponding Cch service exists already
     *
     * @param aServiceId Service Id
     * @return Index of the service or KErrNotFound if does not exist
     */
    TInt FindService( TInt aServiceId ) const;
    
private: //from CActive

	/**
	 * @see CActive.
	 */
	void RunL();
	
	/**
	 * @see CActive.
	 */
	void DoCancel();
	
	/**
	 * @see CActive
	 */
	TInt RunError(TInt aError);
	
private:
    
    /**
     * The constructor
     */
	CCchImpl();
    
    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL( CCch& aCch );
    
    /**
     * Queries the services from CCH client.
     * 
     * @param aType Type of the service
     * @param aServices Array of CCchServices to be filled
     */
    void GetServicesL( TCCHSubserviceType aType, 
            RPointerArray<CCchService>& aServices );
    
    /**
     * Queries the services from CCH client.
     * 
     * @param aServiceId If of the service
     * @param aType Type of the service
     * @param aServices Array of CCchServices to be filled
     * @return Symbian error
     */
    TInt GetCchServicesL( TInt aServiceId, TCCHSubserviceType aType, 
			CArrayFixFlat<TCCHService>& aServices );
    
    /**
     * Queries a service from CCH Client by id.
     * @param	aId The id of the queried service
     * @return	Pointer to the queried service
     */
    CCchServiceImpl* GetServiceL( TInt aId );
    
    
    /**
     * Returns service ids of all configured services, leave function
     *
     * @param  aIds Array of TInt to be filled with service ids
     */
    void GetServiceIdsL( RArray<TInt>& aIds );

private: // data

    /**
     * Own: CCH Server
     */
    RCCHServer iCchServer;
    
    /**
     * Own: CCH Client
     */
    RCCHClient iCchClient;
    
    /**
     * array the for new cch services
     */
    RServiceImplArray iCchServiceImpls;
    
    /**
     * Own: Cch UI handling interface
     */    
    CCchUiPrivateApi* iCchUi;
    
    /**
     * Flag which indicates if cchui connectivity dialogs are used when
     * enabling service.
     */  
    TBool iConnectivityDialogsAllowed;
    
    CCHUI_UNIT_TEST( UT_CchUi )
    };

#endif // C_CCHIMPL_H
