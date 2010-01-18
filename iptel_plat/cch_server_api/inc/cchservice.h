/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef C_CCHSERVICE_H
#define C_CCHSERVICE_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <ecom/ecom.h>
#include <cchclientserver.h>

#include "CCHServiceObserver.h"


// CONSTANTS
/** Interface UID of this ECOM interface*/
const TUid KCCHConnectivityPluginIFUid = { 0x10275457 };

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// CLASS DECLARATION
    
/**
 *  CCchService declaration.
 *  CCH service connectivity plug-in API.
 *  @since S60 3.2
 */
class CCchService : public CBase
    {
    
public:

    static CCchService* NewL( TUid aImplementationUid,
                              MCchServiceObserver& aCCHObserver );

    
    virtual inline ~CCchService();
    
    /**
     * Client enables service. CCHService implementation
     * should use MCCHServiceObserver for service state notifications.
     * @since S60 3.2
     * @param aServiceSelection Selected service, ID and type
     */
    virtual void EnableServiceL( const TServiceSelection& aServiceSelection ) = 0;
                                 
    /**
     * Client enables service. CCHService implementation
     * should use MCCHServiceObserver for service state notifications.
     * @since S60 3.2
     * @param aServiceSelection Selected service, ID and type
     * @param aIapId
     */
    virtual void EnableServiceL( const TServiceSelection& aServiceSelection, 
                                 TUint aIapId ) = 0;
   
    /**
     * Client disables service. 
     * @since S60 3.2
     * @param aServiceSelection Selected service, ID and type
     */
    virtual void DisableServiceL( const TServiceSelection& aServiceSelection  ) = 0;
    
    /**
     * Client checks is the service available by the IAP id array.
     * CCHService implementation should return ETrue if service is
     * registrable by the (some) IAP, array contains. 
     * @since S60 3.2
     * @param aServiceSelection Selected service, ID and type
     * @param aIapIdArray Id array of the currently available iaps.
     * @return ETrue if service is available.
     */
    virtual TBool IsAvailableL( const TServiceSelection& aServiceSelection,
                                const RArray<TUint32>& aIapIdArray ) const = 0;
    
    /**
     * Get service state information.
     * @since S60 3.2
     * @param aServiceSelection Selected service, ID and type
     * @param aState Current service state.
     * @return General symbian error code.
     */
    virtual TInt GetServiceState( const TServiceSelection& aServiceSelection, 
                                  TCCHSubserviceState& aState ) const = 0;
    
    /**
     * Get service network information.
     * @since S60 3.2
     * @param aConnectionInfo Selected service, ID and type, used 
     *               SNAP id (should be set as null if not in use),
     *               used IAP id (should be set as null if not in use),
     *               SNAP locked indicator
     * @return General symbian error code.
     */
    virtual TInt GetServiceNetworkInfo( TServiceConnectionInfo& aConnectionInfo ) const = 0;
                                        
    /**
     * Set SNAP id for service.
     * @since S60 3.2
     * @param aServiceSelection Selected service, ID and type
     * @param aSnapId 
     * @return General symbian error code.
     */
    virtual TInt SetSnapId( const TServiceSelection& aServiceSelection,
                            TUint aSnapId ) = 0;
                            
    /**
     * Set IAP id for service.
     * @since S60 3.2
     * @param aServiceSelection Selected service, ID and type
     * @param aIapId 
     * @return General symbian error code.
     */
    virtual TInt SetIapId( const TServiceSelection& aServiceSelection,
                           TUint aIapId ) = 0;                               
    
    /**
     * Get service specifics information about service and protocol
     * @since S60 3.2
     * @param aServiceSelection Selected service, ID and type
     */
    virtual void GetServiceInfoL( const TServiceSelection& aServiceSelection,
                                  RBuf& aBuffer ) const = 0;
                                  
    /**
     * Reserves the service for exclusive use
     * @since S60 3.2
     * @param aServiceSelection Selected service, ID and type
     * @return General symbian error code.
     */
    virtual TInt ReserveService( const TServiceSelection& aServiceSelection ) = 0;

    /**
     * Frees the exclusive service reservation
     * @since S60 3.2
     * @param aServiceSelection Selected service, ID and type
     * @return General symbian error code.
     */
    virtual TInt FreeService( const TServiceSelection& aServiceSelection ) = 0;
    
    /**
     * Is the service reserved
     * @since S60 3.2
     * @param aServiceSelection Selected service, ID and type
     * @return TBool 
     */
    virtual TBool IsReserved( const TServiceSelection& aServiceSelection ) const = 0;

    /**
     * Returns the connection parameters
     * @since S60 3.2.3
     * @param aServiceSelection Selected service, ID and type
     * @param aParameter Connection parameter of the service
     * @param aValue Value of the parameter
     * @return Symbian error code
     */
    virtual TInt GetConnectionParameter( 
                         const TServiceSelection& aServiceSelection,
                         TCchConnectionParameter aParameter,
                         TInt& aValue ) const = 0;
    
    /**
     * Returns the connection parameters
     * @since S60 3.2.3
     * @param aServiceSelection Selected service, ID and type
     * @param aParameter Connection parameter of the service
     * @param aValue Value of the parameter
     * @return Symbian error code
     */
    virtual TInt GetConnectionParameter(  
                         const TServiceSelection& aServiceSelection, 
                         TCchConnectionParameter aParameter,
                         RBuf& aValue ) const = 0;
    
    /**
     * Sets the connection parameters
     * @since S60 3.2.3
     * @param aServiceSelection Selected service, ID and type
     * @param aParameter Connection parameter of the service
     * @param aValue Value of the parameter
     * @return Symbian error code
     */
    virtual TInt SetConnectionParameter(  
                         const TServiceSelection& aServiceSelection, 
                         TCchConnectionParameter aParameter,
                         TInt aValue ) = 0;
    
    /**
     * Sets the connection parameters
     * @since S60 3.2.3
     * @param aServiceSelection Selected service, ID and type
     * @param aParameter Connection parameter of the service
     * @param aValue Value of the parameter
     * @return Symbian error code
     */
    virtual TInt SetConnectionParameter(  
                         const TServiceSelection& aServiceSelection, 
                         TCchConnectionParameter aParameter,
                         const TDesC& aValue ) = 0;

public: // Data

    /// Unique key for implementations of this interface.
    TUid iInstanceKey;

protected: // Constructors 

    inline CCchService();
    
    };

#include "CCHService.inl"

#endif // C_CCHSERVICE_H

// End of file
