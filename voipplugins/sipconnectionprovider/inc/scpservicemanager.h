/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef C_CSCPSERVICEMANAGER_H
#define C_CSCPSERVICEMANAGER_H

#include <cchservice.h>

#include "scpdefs.h"
#include "scpstatecontainer.h"

class CScpSettingHandler;
class CScpProfileHandler;
class CScpServiceStorage;
class CScpService;
class CScpSubService;
class CScpSipConnection;

/**
*  CScpServiceManager class
*  @lib sipconnectionprovider.dll
*/
class CScpServiceManager : public CBase
    {
public:    

    /**
    * Two-phased constructor
    */
    static CScpServiceManager* NewL( MCchServiceObserver& aServiceObserver );

    /**
    * Destructor.
    */
    virtual ~CScpServiceManager();

public: // New functions
   
    /**
     * Client enables service. CCHService implementation
     * should use MCCHServiceObserver for service state notifications.
     * @see MCCHServiceObserver
     * @param aServiceId
     */
    void EnableServiceL( TUint aServiceId,
                         TCCHSubserviceType aSubServiceType );
                                 
    /**
     * Client enables service. CCHService implementation
     * should use MCCHServiceObserver for service state notifications.
     * @see MCCHServiceObserver
     * @param aServiceId
     * @param aIapId
     */
    void EnableServiceL( TUint aServiceId,
                         TCCHSubserviceType aSubServiceType,
                         TUint aIapId );
    
    /**
     * Client disables service. 
     * @param aServiceId
     */
    void DisableServiceL( TUint aServiceId,
                          TCCHSubserviceType aSubServiceType );
    
    /**
     * Client checks is the service available by the IAP id array.
     * CCHService implementation should return ETrue if service is
     * registrable by the (some) IAP, array contains. 
     * @param aServiceId
     * @param aIapIdArray Id array of the currently available iaps.
     * @return ETrue if service is available.
     */
    TBool IsAvailableL( TUint aServiceId,
                        TCCHSubserviceType aSubServiceType,
                        const RArray<TUint32>& aIapIdArray ) const;
    
    /**
     * Get service state information.
     * @param aServiceId
     * @param aState Current service state.
     * @return General symbian error code.
     */
    TInt GetServiceState( TUint aServiceId,
                          TCCHSubserviceType aSubServiceType, 
                          TCCHSubserviceState& aState ) const;
    
    /**
     * Get service network information.
     * @param aServiceId
     * @param aSnapId Used Snap id (should be set as null if
     *               not in use)
     * @param aIapId Used IAP id (should be set as null if
     *               not in use)
     * @param aSnapLocked Is snap locked or not.
     * @param aPasswordSet Is password set or not.
     * @return General symbian error code.
     */
    TInt GetServiceNetworkInfo( TUint aServiceId,
                                TCCHSubserviceType aSubServiceType,
                                TUint32& aSnapId, 
                                TUint32& aIapId,
                                TBool& aSnapLocked,
                                TBool& aPasswordSet ) const;
                                        
    /**
     * Set Snap id for service.
     * @param aServiceId
     * @param aSnapId 
     * @return General symbian error code.
     */
    TInt SetSnapId( TUint aServiceId,
                    TCCHSubserviceType aSubServiceType,
                    TUint aSnapId );
                    
    /**
     * Set IAP id for service.
     * @param aServiceId
     * @param aSnapId 
     * @return General symbian error code.
     */
    TInt SetIapId( TUint aServiceId,
                   TCCHSubserviceType aSubServiceType,
                   TUint aIapId );                               
    
    /**
     * Get service specifics information about service and protocol
     * @param aServiceId
     * @param aSubservice
     * @param aBuffer  
     */
    void GetServiceInfoL( TUint aServiceId,
                          TCCHSubserviceType aSubServiceType,
                          RBuf& aBuffer ) const;

    /**
     * Sets service reserved or unreserved
     * @param aReserved Reserved
     * @param aServiceId
     * @param aSubServiceType Sub service type
     * @return Symbian error codes
     */
    TInt SetServiceReserved( TBool aReserved,
                             TUint aServiceId,
                             TCCHSubserviceType aSubServiceType );

    /**
     * Returns service reserved info
     * @param aServiceId
     * @param aSubServiceType Sub service type
     * @return ETrue if reserved
     */
    TBool IsReserved( TUint aServiceId,
                      TCCHSubserviceType aSubServiceType ) const;                              


    /**
     * Returns the connection parameters
     * @since S60 3.2.3
     * @param aServiceSelection Selected service, ID and type
     * @param aParameter Connection parameter of the service
     * @param aValue Value of the parameter
     * @leave Symbian error code
     */
    void GetConnectionParameterL( const TServiceSelection& aServiceSelection,
                                  TCchConnectionParameter aParameter,
                                  TInt& aValue ) const;
    
    /**
     * Returns the connection parameters
     * @since S60 3.2.3
     * @param aServiceSelection Selected service, ID and type
     * @param aParameter Connection parameter of the service
     * @param aValue Value of the parameter
     * @leave Symbian error code
     */
    void GetConnectionParameterL( const TServiceSelection& aServiceSelection, 
                                  TCchConnectionParameter aParameter,
                                  RBuf& aValue ) const;
    
    /**
     * Sets the connection parameters
     * @since S60 3.2.3
     * @param aServiceSelection Selected service, ID and type
     * @param aParameter Connection parameter of the service
     * @param aValue Value of the parameter
     * @leave Symbian error code
     */
    void SetConnectionParameterL( const TServiceSelection& aServiceSelection, 
                                 TCchConnectionParameter aParameter,
                                 TInt aValue );
    
    /**
     * Sets the connection parameters
     * @since S60 3.2.3
     * @param aServiceSelection Selected service, ID and type
     * @param aParameter Connection parameter of the service
     * @param aValue Value of the parameter
     * @leave Symbian error code
     */
    void SetConnectionParameterL( const TServiceSelection& aServiceSelection, 
                                  TCchConnectionParameter aParameter,
                                  const TDesC& aValue );

    
    
private:

    /**
     * Default constructor
     * @param aStateObserver State Observer
     */
    CScpServiceManager( MCchServiceObserver& aServiceObserver );
    
    /**
     * 2nd phase constructor
     */
    void ConstructL();
                           
    /**
     * Get service network information. Leaves on failure.
     * @param aServiceId
     * @param a Sub service type
     * @param aSnapId Used Snap id (should be set as null if
     *               not in use)
     * @param aIapId Used IAP id (should be set as null if
     *               not in use)
     * @param aSnapLocked Snap locked
     */
    void GetServiceNetworkInfoL( TUint aServiceId,
                                 TCCHSubserviceType aSubServiceType,
                                 TUint32& aSnapId, 
                                 TUint32& aIapId,
                                 TBool& aSnapLocked,
                                 TBool& aPasswordSet ) const;
                                                                
    /** 
     * Handles ALR events.
     * @param aServiceId Service Id
     * @param a Sub service type
     * @param aConnectionCreated ETrue if connection was created
     * @return SipConnection if exists
     */   
    CScpSipConnection* GetSipConnectionL( TUint aServiceId,
                                          TCCHSubserviceType aSubServiceType,
                                          TBool& aConnectionCreated ) const;

    /**
     * Gets and updates a service
     * @param aServiceId Service Id
     * @param a Sub service type
     * @return Service, NULL if failed
     */
    CScpService* GetServiceL( TUint aServiceId,
                              TCCHSubserviceType aSubServiceType );

    /**
     * Creates and updates a service. The service is saved to
     * service storage.
     * @param aServiceId Service Id
     * @param a Sub service type
     * @return Service
     */
    CScpService* CreateServiceL( TUint aServiceId,
                                 TCCHSubserviceType aSubServiceType );

    /**
     * Creates and updates a temporary service. The service is not
     * saved to service storage. These use cases are due to network
     * info queries.
     * @param aServiceId Service Id
     * @param a Sub service type
     * @return Service
     */
    CScpService* CreateTemporaryServiceL( TInt aServiceId,
                                          TCCHSubserviceType aSubServiceType ) const;

    /**
     * Set access point for a service
     * @param aServiceId Service Id
     * @param aSubServiceType Sub service type
     * @param aAccessPointType Access point type
     * @param aAccessPointId Id
     * @return Error code
     */ 
    TInt SetAccessPointId( TUint aServiceId,
                           TCCHSubserviceType aSubServiceType, 
                           TScpAccessPointType aAccessPointType, 
                           TInt aAccessPointId );
    
    /**
     * Updates the prfile handler's user agent header. Use before.
     * @param aServiceId Service Id
     * @param a SubServiceType sub service type
     */
    void UpdateProfileValuesL( TUint aServiceId, TCCHSubserviceType aSubServiceType ) const;
     
    /**
     * Updates username/password to XDM-setting
     * @param aServiceId Service Id
     * @param aUsername username
     * @param aSetUsername ETrue if username wish to be set
     * @param aPassword password
     * @param aSetPassword ETrue if username wish to be set
     */
    void SetUsernameAndPasswordToXdmL( 
        TUint aServiceId,
        const TDesC8& aUsername,
        TBool aSetUsername,
        const TDesC8& aPassword,
        TBool aSetPassword );
    
    /**
     * Checks restricted connections. In case where voip is not 
     * allowed via 3g and 3g ap is first ap, leave with error 
     * KCCHErrorAccessPointNotDefined so user can select another ap.
     * @param aServiceId Service Id
     */
    void CheckRestrictedConnectionsL( TUint aServiceId );
    
private:  

    /**
     * Observer for state changes
     */
    MCchServiceObserver& iServiceObserver; 
    
    /**
     * Setting handler. Own.
     */ 
    CScpSettingHandler* iSettingHandler;

    /**
     * Setting handler. Own.
     */     
    CScpProfileHandler* iProfileHandler;

    /**
     * Service settings storage. Own.
     */    
    CScpServiceStorage* iServiceStorage;
    
    /**
     * Service state container. Singleton.
     */
    TScpStateContainer iStateContainer;

private:

#ifdef _DEBUG
    friend class T_CScpServiceManager;
    friend class T_CScpSipHelper;
    friend class T_CScpService;
    friend class T_CScpSettingHandler;
#endif
    
    };    

#endif  // C_CSCPSERVICEMANAGER_H

// End of file

