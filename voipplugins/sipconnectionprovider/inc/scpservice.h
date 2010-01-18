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
* Description:  
*
*/


#ifndef C_SCPSERVICE_H
#define C_SCPSERVICE_H

#include <e32base.h>
#include <cchservice.h>

#include "scpdefs.h"

/**
 *  SCP service info.
 *  @lib sipconnectionprovider.dll
 */
class CScpSubService;
class CScpServiceStorage;
class CScpProfileHandler;
class CScpSipConnection;

class CScpService : public CBase
    {
public:

    /**
     * Constructor
     * @param aId Internal id
     * @param aServiceId Service Id
     */ 
    static CScpService* NewL( TInt aId, 
                              TInt aServiceId,
                              CScpProfileHandler& aProfileHandler,
                              CScpServiceStorage& aServiceStorage,
                              MCchServiceObserver& aServiceObserver );
    
    /**
     * Destructor
     */ 
    ~CScpService();

    /**
     * Returns internal id of this service
     * @return Id
     */ 
    TInt Id() const;

    /**
     * Returns profile handler
     */
    CScpProfileHandler& ProfileHandler() const;

    /**
     * Returns service storage
     */
    CScpServiceStorage& ServiceStorage() const;

    /**
     * Returns service observer
     */
    MCchServiceObserver& ServiceObserver() const;

    /**
     * Adds a new subservice to the service and returns it
     * @return New subservice
     */ 
    CScpSubService& AddSubServiceL( TCCHSubserviceType aSubServiceType );

    /**
     * Returns count of subservices for this service
     * @returns Count of subservices
     */ 
    TInt SubServiceCount() const;

    /**
     * Fills all internal ids of the subservices within this service 
     * to the given aray
     * @param aIds Id array to be filled
     */ 
    void GetSubServiceIds( RArray<TInt>& aIds ) const;

    /**
     * Returns a subservice with the given id
     * @param aId Internal id of the subservice 
     */ 
    CScpSubService* GetSubService( TInt aId ) const;

    /**
     * Removes a subservice from this service with given id
     * @param aId Internal id of the subservice
     * @return KErrNone if successfull
     */ 
    TInt RemoveSubService( TInt aId );

    /**
     * Returns service id of this service
     * @return service id
     */ 
    TInt ServiceId() const;

    /**
     * Sets service id for this service
     * @param aServiceId Service Id
     */ 
    void SetServiceId( TInt aServiceId );

    /**
     * Returns state of the service by sub service type
     * @param aSubServiceType Sub service type
     * @param aState Result state
     * @return Symbian error codes
     */ 
    TInt State( TCCHSubserviceType aSubServiceType, 
                TCCHSubserviceState& aState ) const;

    /**
     * Queries if this service contains a subservice with the given type
     * @return ETrue if sub service with given type exists
     */ 
    TBool ContainsSubServiceType( TCCHSubserviceType aSubServiceType ) const;

    /**
     * Returns a sub service from this service based on a type
     * @param a Type
     * @return Sub service
     */
    CScpSubService* GetSubServiceByType( TCCHSubserviceType aSubServiceType ) const;

    /**
     * Checks if sub services of this service contain same sip profile
     * @return ETrue if they contain the same profile
     */
    TBool SubServicesContainSameSipProfile() const;

    /**
     * Sets reservation status for the service based on type
     * @param aReserved Reserved
     * @param aSubServiceType Sub service type
     * @return Error code
     */
    TInt SetReserved( TBool aReserved, TCCHSubserviceType aSubServiceType );

    /**
     * Returns reservation status for the service based on type
     * @param aSubServiceType Sub service type
     * @return ETrue if reserved
     */
    TBool IsReserved( TCCHSubserviceType aSubServiceType ) const;

    /**
     * Set access point for the service
     * @param aSubServiceType Sub service type
     * @param aAccessPointType Access point type
     * @param aAccessPointId Id
     * @return Error code
     */ 
    TInt SetAccessPointId( TCCHSubserviceType aSubServiceType, 
                           TScpAccessPointType aAccessPointType, 
                           TInt aAccessPointId );
    
    /**
     * Checks is all subservices in disabled state
     * @return TBool ETrue if all service's subservices are in disabled state
     */
    TBool IsAllSubservicesDisabled() const;
    
    /**
     * Change all service's subservices last error to given error
     * @param aError error which to be set
     */
    void ChangeLastReportedErrors( const TInt aError );

private:

    /**
     * Default constructor hidden
     * @param aId Internal id
     * @param aServiceId Service Id
     */ 
    CScpService( TInt aId, 
                 TInt aServiceId,
                 CScpProfileHandler& aProfileHandler, 
                 CScpServiceStorage& aServiceStorage,
                 MCchServiceObserver& aServiceObserver );
    
    /**
     * Generates new internal sub service id
     * @return New id
     */ 
    TInt GenerateNewSubServiceId();

    /**
     * Checks if the service's access point can be modified
     * @param aSubServiceType Sub service type
     * @return ETrue if modifiable
     */ 
    TBool IsAccessPointModifiable( TCCHSubserviceType aSubServiceType ) const;

    /**
     * Gets sip connection that the service is using
     * @param aSubServiceType Sub service type
     * @param aSipConnectionCreated ETrue if the connection was created
     * @return Sip connection.
     */ 
    CScpSipConnection* GetSipConnection( TCCHSubserviceType aSubServiceType,
                                         TBool& aSipConnectionCreated );

private:

    /**
     * Internal sub service id
     */ 
    TInt iId;
    
    /**
     * Service id.
     */     
    TInt iServiceId;

    /**
     * Profile handler.
     */
    CScpProfileHandler& iProfileHandler;

    /**
     * Service storage
     */
    CScpServiceStorage& iServiceStorage; 

    /**
     * Service Observer
     */
    MCchServiceObserver& iServiceObserver;
    
    /**
     * Default sip profile.
     */     
    TInt iDefaultSipProfileId;
    
    /**
     * Sub services of this service
     */
    RPointerArray< CScpSubService > iSubServices; 

    /**
     * Counter for creating unique internal sub service ids
     */     
    TInt iSubServiceIdCounter;
    };



#endif // C_SCPSERVICE_H
