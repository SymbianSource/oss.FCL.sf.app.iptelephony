/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef C_CSCSERVICESTORAGE_H
#define C_CSCSERVICESTORAGE_H

#include <e32base.h>
#include <cchclientserver.h>

#include "scpdefs.h"

class CScpService;
class CScpSubService;
class CScpProfileHandler;
class CScpSettingHandler;
class MCchServiceObserver;

/**
*  Service storage class.
*
*  @lib sipconnectionprovider.dll
*/
class CScpServiceStorage : public CBase
    {
public:  // Constructors and destructor

    /**
     * Two-phased constructor
     */
    static CScpServiceStorage* NewL(CScpSettingHandler& iScpSettingHandler);  
    
    /**
     * Destructor.
     */
    virtual ~CScpServiceStorage();

    /**
     * Creates a service to the storage.
     * @param aService Service settings.
     */
    CScpService& CreateServiceL( TInt aServiceId, 
                                 CScpProfileHandler& aProfileHandler,
                                 MCchServiceObserver& aServiceObserver );

    /**
     * Removes a service from the storage by internal id.
     * @param aId Id of the service.
     */
    TInt RemoveService( TInt aId );

    /**
     * Removes services with disabled or no subservices from the storage.
     * @since Series 60 3.0
     */
    void RemoveDisabledServices();

    /**
     * Returns a service from the storage by id.
     * @param aId Id of the service.
     * @return Service if found (ownership not transferred) - otherwise NULL
     */
    CScpService* GetService( TInt aId ) const;
            
    /**
     * Fills all internal ids of the services to the given array.
     * @param aIds Internal ids of the services.
     */
    void GetServiceIds( RArray<TInt>& aIds ) const;

    /**
     * Returns a service from the storage by service id.
     * @param aId Service id of the service.
     * @return Service if found (ownership not transferred) - otherwise NULL
     */
    CScpService* GetServiceByServiceId( TUint aServiceId ) const;               

    /**
     * Returns a sub service from the storage by sub service internal id.
     * @param aId Internal id of the sub service.
     * @return Sub service if found (ownership not transferred) - otherwise NULL
     */
    CScpSubService* GetSubService( TInt aId ) const;

    /**
     * Fills all internal sub service ids to the given array.
     * @param aIds Array of sub service internal ids.
     */
    void GetSubServiceIds( RArray<TInt>& aIds ) const;

    /**
     * Fills all internal sub service ids by type to the given array.
     * @param aIds Array of sub service internal ids.
     */
    void GetSubServiceIds( TCCHSubserviceType aSubServiceType,
                           RArray<TInt>& aIds ) const;

    /**
     * Fills sub service ids to the given array that match profile id and type
     * @param aProfileId Sip profile id
     * @param a Sub service type
     * @param aIds Array of sub service internal ids.
     */
    void GetSubServiceIds( TUint32 aProfileId,
                           TCCHSubserviceType aSubServiceType, 
                           RArray<TInt>& aIds ) const;

    /**
     * Queries if a sub service with given profile id is enabled
     * @param aProfileId Sip profile id
     * @return ETrue if enabled
     */
    TBool IsSubServiceEnabled( TUint32 aProfileId ) const;

    /**
     * Queries if all sub service with given profile ids are
     * disconnected from service. 
     * @param aProfileId Sip profile id
     * @return ETrue if all are disconnected
     */
    TBool AreAllSubServicesDisconnected( TUint32 aProfileId ) const;
   
   /**
     * Returns CScpSettingHandler 
     * @return CScpSettingHandler reference
     */
    CScpSettingHandler& SettingsHandler();

#ifdef _DEBUG
    void GetDebugInfo( TDes& aInfo ) const;
#endif

private:  // Constructors

    /**
     * C++ default constructor.
     */
    CScpServiceStorage(CScpSettingHandler& aScpSettingHandler);
    
    /**
     * Generates a new internal id for a service
     * @return Ner internal id
     */
    TInt GenerateNewInternalIdForService();

private:

    // Settings handler
    CScpSettingHandler& iSettingHandler;

    // Services
    RPointerArray<CScpService> iServiceItems;

    // Counter for generating unique internal service ids
    TInt iServiceIdCounter;

    /**
     *
     */
    CPeriodic* iRemoveTimer;
    };

#endif      // C_CSCSERVICESTORAGE_H   
            
// End of File
