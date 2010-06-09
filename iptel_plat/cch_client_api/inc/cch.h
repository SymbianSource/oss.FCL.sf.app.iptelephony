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
* Description:  The new cch client api class
*
*/



#ifndef C_CCH_H
#define C_CCH_H

#include <e32base.h>
#include <ccherror.h>
#include <cchtypes.h>
#include <cchui.h>

class CCchImpl;


/**
 *  Class indicating the service status
 *
 *  @lib cch
 */
class TCchServiceStatus
    {

public:
    
    /**
     * Constructor.
     */
     IMPORT_C TCchServiceStatus( );
    
    /**
     * Returns the service state
     *
     * @return State of the service
     */
     IMPORT_C TCCHSubserviceState State() const;
    
    /**
     * Returns last known service error
     *
     * @return Error of the service
     */
     IMPORT_C TInt Error() const;
    
    /**
     * Sets the service state
     *
     * @param aState State for the service
     */
     IMPORT_C void SetState( TCCHSubserviceState aState );

    /**
     * Sets the service error
     *
     * @param aError Error for the service
     */
     IMPORT_C void SetError( TInt aError );
    	
private: // data

    /**
     * The state of the service
     */
	TCCHSubserviceState iState;
    /**
     * The error of the service
     */
    TInt iError;
    };
    
/**
 *  Observer class for getting service status change messages
 *
 *  @lib cch
 */
class MCchServiceStatusObserver 
    {

public:
    /**
     * Signaled when service status or error changes
     *
     * @param aServiceId Id of the service
     * @param aType Service type
     * @param aServiceStatus Service status
     */
    virtual void ServiceStatusChanged( TInt aServiceId, 
    								   TCCHSubserviceType aType, 
                                       const TCchServiceStatus& aServiceStatus ) = 0;

    };

/**
 *  Class for cch service management
 *
 *  CCchService is used for launching various kinds of service functions
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ?library
 */
class CCchService : public CBase
    {

public:

    /**
     * Launches service enable. See MCchServiceObserver for status changes.
     *
     * @param aType Type of service
     * @return Symbian error
     */
    virtual TInt Enable( TCCHSubserviceType aType ) = 0;
    
    /**
     * Launches service disable. See MCchServiceObserver for status changes.
     *
     * @param aType Type of service
     * @return Symbian error
     */
    virtual TInt Disable( TCCHSubserviceType aType ) = 0;
    
    /**
     * Returns the current service state
     *
     * @param aType Type of service
     * @param aStatus Status of the service, return value
     * @return Symbian error code
     */
    virtual TInt GetStatus( TCCHSubserviceType aType, 
    						TCchServiceStatus& aStatus ) const = 0;
    
    /**
     * Returns the connection parameters
     *
     * @param aType Type of service
     * @param aParameter Connection parameter of the service
     * @param aValue Value of the parameter
     * @return Symbian error code
     */
    virtual TInt GetConnectionParameter( TCCHSubserviceType aType,
    					TCchConnectionParameter aParameter, TInt& aValue ) const = 0;
    
    /**
     * Returns the connection parameters
     *
     * @param aType Type of service
     * @param aParameter Connection parameter of the service
     * @param aValue Value of the parameter
     * @return Symbian error code
     */
    virtual TInt GetConnectionParameter( TCCHSubserviceType aType, 
    					TCchConnectionParameter aParameter, RBuf& aValue ) const = 0;
    
    /**
     * Sets the connection parameters
     *
     * @param aType The type of service
     * @param aParameter Connection parameter of the service
     * @param aValue Value of the parameter
     * @return Symbian error code
     */
    virtual TInt SetConnectionParameter( TCCHSubserviceType aType, 
    					TCchConnectionParameter aParameter, TInt aValue ) = 0;
    
    /**
     * Sets the connection parameters
     *
     * @param aType The type of service
     * @param aParameter Connection parameter of the service
     * @param aValue Value of the parameter
     * @return Symbian error code
     */
    virtual TInt SetConnectionParameter( TCCHSubserviceType aType, 
    					TCchConnectionParameter aParameter, const TDesC& aValue ) = 0;
    
    /**
     * Reserves the service for exclusive usage
     *
     * @param aType Type of service
     * @return Symbian error code
     */
    virtual TInt Reserve( TCCHSubserviceType aType ) = 0;
    
    /**
     * Frees the service of exclusive usage
     *
     * @param aType Type of service
     * @return Symbian error code
     */
    virtual TInt Free( TCCHSubserviceType aType ) = 0;
    
    /**
     * Is the service reserved
     *
     * @param aType Type of service
     * @param aReserved True if the service is reserved
     * @return Symbian error code
     */
    virtual TInt IsReserved( TCCHSubserviceType aType, TBool& aReserved ) const = 0;
    
    /**
     * Returns the service id
     *
     * @return The id of the service
     */
    virtual TInt ServiceId() const = 0;
    
    /**
     * @deprecated Do not use this anymore, change to AddObserver!
     *
     * Adds observer for listening service events
     *
     * @param aObserver Event observing class
     */
    virtual void SetObserver( MCchServiceStatusObserver& aObserver ) = 0;
    
    /**
     * @deprecated Do not use this anymore, change to 
     * RemoveObserver method with paramater
     * 
     * Removes the observer of service events
     */
    virtual void RemoveObserver( ) = 0;

    /**
     * Adds observer for listening service events
     *
     * @param aObserver Event observing class
     * @return KErrAlreadyExist Observer already added
     *         KErrNone Observer was added
     */
    virtual TInt AddObserver( MCchServiceStatusObserver& aObserver ) = 0;
    
    /**
     * Removes the observer of service events
     *
     * @param aObserver Event observing class
     * @return KErrNotFound Observer was not found
     *         KErrNone Observer was removed
     */
    virtual TInt RemoveObserver( MCchServiceStatusObserver& aObserver ) = 0;
    
    /**
     * Checks if the type is supported
     *
     * @param aType Type of service
     * @param aSupported True if the service type is supported
     * @return Symbian error code
     */
    virtual TInt IsSupported( TCCHSubserviceType aType, TBool& aSupported ) const = 0;

protected:

    virtual ~CCchService() { };
    };
    
/**
 *  Class for basic cch functionality, getting services
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib cch
 */
class CCch : public CBase
    {

public:

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CCch* NewL();
    
    /**
     * Two-phased constructor.
     */
    IMPORT_C static CCch* NewLC();
    
    /**
     * Destructor.
     */
    ~CCch();

    /**
     * Returns one service
     *
     * @param aId Service id
     * @return Service or NULL if not found. Ownership is not transferred.
     */
    IMPORT_C CCchService* GetService( TInt aId );
    
    /**
     * Returns service ids of all configured services
     *
     * @param  aIds Array of TInt to be filled with ids
     * @return Symbian error code
     */
    IMPORT_C TInt GetServiceIds( RArray<TInt>& aIds ) const;
    
    /**
     * Returns services fulfilling the search criteria
     *
     * @param aType Service type
     * @param aServices Array of CCchService to be filled 
     * @return Symbian error code
     */
    IMPORT_C TInt GetServices( TCCHSubserviceType aType, 
                               RPointerArray<CCchService>& aServices );
        
    /**
     * Returns ETrue if connectivity dialogs are allowed in service enabling.
     *
     * @return ETrue if connectivity dialogs are allowed in service enabling
     */
    IMPORT_C TBool ConnectivityDialogsAllowed() const;
       
    /**
     * For allowing/disallowing connectivity dialogs showing in 
     * service enabling.
     * 
     * @param aConnectivityDialogsAllowed Boolean to set connectivity dialogs
     *        allowed/disallowed
     */ 
    IMPORT_C void SetConnectivityDialogsAllowed( 
        TBool aConnectivityDialogsAllowed );
    
    /**
     * For getting cchui api.
     * 
     * @return reference to cchui api.
     */
    IMPORT_C MCchUi& CchUiApi() const;
    
protected:

    /**
     * The constructor
     */
    CCch();
    
private:
    
    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private: // data

    /**
     * Cch client implementation
     * Own.  
     */
	CCchImpl* iImplementation;

#ifdef CCHUI_EUNIT
    friend class UT_CchUi;
#endif
    };

#endif // C_CCH_H
