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



#ifndef C_CCHSERVICEIMPL_H
#define C_CCHSERVICEIMPL_H

#include <e32base.h>
#include <ccherror.h>
#include <cchtypes.h>
//from cchclient
#include "cchimpl.h"
#include "cch.h"


class CCchClientObserver;
class CCchImpl;

#ifdef CCHAPI_USE_CCHUI
class CCchUiPrivateApi;
#else 
#define CCchUiPrivateApi TInt 
#endif

/**
 *  Class for basic cch functionality, getting services
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib cch
 */
class CCchServiceImpl : public CCchService
    {

public:

    /**
     * Two-phased constructor.
     * 
     * @param aCch Cch service factory
     * @param aServiceId Id of the service
     * @param aCchUi CCH connectivity UI handling API
     */
    static CCchServiceImpl* NewL( CCchImpl& aCch, 
                                  TInt aServiceId,
                                  CCchUiPrivateApi& aCchUi );
    
    /**
     * Two-phased constructor.
     * 
     * @param aCch Cch service factory
     * @param aServiceId Id of the service
     * @param aCchUi CCH connectivity UI handling API
     */
    static CCchServiceImpl* NewLC( CCchImpl& aCch, 
                                   TInt aServiceId,
                                   CCchUiPrivateApi& aCchUi );
    
    /**
     * Destructor.
     */
    ~CCchServiceImpl();

public: // from CCchService

    /**
     * Launches service enable. See CCchServiceObserver for status changes.
     *
     * @param aType Type of service
     * @return Symbian error
     */
    TInt Enable( TCCHSubserviceType aType );
    
    /**
     * Launches service disable. See CCchServiceObserver for status changes.
     *
     * @param aType Type of service
     * @return Symbian error
     */
    TInt Disable( TCCHSubserviceType aType );
    
    /**
     * Returns the current service state
     *
     * @param aType Type of service
     * @param aStatus Status of the service, return value
     * @return Symbian error code
     */
    TInt GetStatus( TCCHSubserviceType aType, TCchServiceStatus& aStatus ) const;
    
    /**
     * Returns the connection parameters
     *
     * @param aType Type of service
     * @param aParameter Connection parameter of the service
     * @param aValue value of the parameter
     * @return Symbian error code
     */
    TInt GetConnectionParameter( TCCHSubserviceType aType, TCchConnectionParameter aParameter, TInt& aValue ) const;
    
    /**
     * Returns the connection parameters
     *
     * @param aType Type of service
     * @param aParameter Connection parameter of the service
     * @param aValue value of the parameter
     * @return Symbian error code
     */
    TInt GetConnectionParameter( TCCHSubserviceType aType, 
                                 TCchConnectionParameter aParameter, 
                                 RBuf& aValue ) const;
    
    /**
     * Sets the connection parameters
     *
     * @param aType The type of service
     * @param aParameter Connection parameter of the service
     * @param aValue value of the parameter
     * @return Symbian error code
     */
    TInt SetConnectionParameter( TCCHSubserviceType aType, 
                                 TCchConnectionParameter aParameter, 
                                 TInt aValue );
    
    /**
     * Sets the connection parameters
     *
     * @param aType The type of service
     * @param aParameter Connection parameter of the service
     * @param aValue value of the parameter
     * @return Symbian error code
     */
    TInt SetConnectionParameter( TCCHSubserviceType aType, 
                                 TCchConnectionParameter aParameter, 
                                 const TDesC& aValue );
    
    /**
     * Reserves the service for exclusive usage
     *
     * @param aType Type of service
     * @return Symbian error code
     */
    TInt Reserve( TCCHSubserviceType aType );
    
    /**
     * Frees the service of exclusive usage
     *
     * @param aType Type of service
     * @return Symbian error code
     */
    TInt Free( TCCHSubserviceType aType );
    
    /**
     * Is the service reserved
     *
     * @param aType Type of service
     * @param aReserved True if the service is reserved
     * @return Symbian error code
     */
    TInt IsReserved( TCCHSubserviceType aType, 
                     TBool& aReserved ) const;
    
    /**
     * Returns the service id
     *
     * @return Service id
     */
    TInt ServiceId() const;
    
    /**
     * @deprecated Do not use this anymore, change to AddObserver!
     *
     * Adds observer for listening service events
     *
     * @param aObserver Event observing class
     */
    void SetObserver( MCchServiceStatusObserver& aObserver );
    
    /**
     * @deprecated Do not use this anymore, change to 
     * RemoveObserver method with paramater
     * 
     * Removes the observer of service events
     */
    void RemoveObserver( );

    /**
     * Adds observer for listening service events
     *
     * @param aObserver Event observing class
     * @return KErrAlreadyExist Observer already added
     *         KErrNone Observer was added
     */
    TInt AddObserver( MCchServiceStatusObserver& aObserver );
    
    /**
     * Removes the observer of service events
     *
     * @param aObserver Event observing class
     * @return KErrNotFound Observer was not found
     *         KErrNone Observer was removed
     */
    TInt RemoveObserver( MCchServiceStatusObserver& aObserver );
    
    /**
     * Checks if the type is supported
     *
     * @param aType Type of service
     * @param aSupported True if the service type is supported
     * @return Symbian error code
     */
    TInt IsSupported( TCCHSubserviceType aType, TBool& aSupported ) const;
    
private:
    
    /**
     * The constructor
     * 
     * @param aCch Cch service factory
     * @param aServiceId The id of the service
     */
	CCchServiceImpl( CCchImpl& aCch, 
	                 TInt aServiceId, 
                     CCchUiPrivateApi& aCchUi );
    
    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

public:

	CCchImpl* CchImpl() const;
	
private: // data

	/**
	 * cch service factory, not own
	 */
	CCchImpl& iCch;
	
	/**
	 * Service Id
	 */
	TInt iServiceId; 
	
	/**
	 * Service observer
	 */
	CCchClientObserver* iObserver;
	/**
	 * CCH connectivity UI handling private API
	 */	
	CCchUiPrivateApi& iCchUi;

#ifdef CCHUNITTEST
    friend class UT_CCchService;
#endif
    };

#endif // C_CCHSERVICEIMPL_H
