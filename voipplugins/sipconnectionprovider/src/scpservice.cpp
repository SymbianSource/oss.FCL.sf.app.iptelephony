/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#include <cchserviceobserver.h>

#include "scpservice.h"
#include "scpsubservice.h"
#include "scplogger.h"
#include "scputility.h"
#include "scpprofilehandler.h"

// -----------------------------------------------------------------------------
// CScpService::CScpService
// -----------------------------------------------------------------------------
//
CScpService::CScpService( TInt aId, 
                          TInt aServiceId,
                          CScpProfileHandler& aProfileHandler, 
                          CScpServiceStorage& aServiceStorage,
                          MCchServiceObserver& aServiceObserver ) :
    iId( aId ),
    iServiceId( aServiceId ),
    iProfileHandler( aProfileHandler ), 
    iServiceStorage( aServiceStorage ),
    iServiceObserver( aServiceObserver )
    {
    }

// -----------------------------------------------------------------------------
// CScpService::~CScpService
// -----------------------------------------------------------------------------
//
CScpService::~CScpService()
    {
    SCPLOGSTRING2( "CScpService[0x%x]::~CScpService", this );

    iSubServices.ResetAndDestroy();
    iSubServices.Close();
    }

// -----------------------------------------------------------------------------
// CScpService::NewL
// -----------------------------------------------------------------------------
//
CScpService* CScpService::NewL( TInt aId, 
                                TInt aServiceId,
                                CScpProfileHandler& aProfileHandler,
                                CScpServiceStorage& aServiceStorage, 
                                MCchServiceObserver& aServiceObserver )
    {
    SCPLOGSTRING3( "CScpService::NewL id: %d service id: %d", aId, aServiceId );

    CScpService* self = new (ELeave) CScpService( aId, 
                                                  aServiceId,
                                                  aProfileHandler,
                                                  aServiceStorage,
                                                  aServiceObserver );
    return self;
    }

// -----------------------------------------------------------------------------
// CScpService::Id
// -----------------------------------------------------------------------------
//
TInt CScpService::Id() const
    {
    SCPLOGSTRING2( "CScpService[0x%x]::Id", this );

    return iId;
    }

// -----------------------------------------------------------------------------
// CScpService::ProfileHandler
// -----------------------------------------------------------------------------
//
CScpProfileHandler& CScpService::ProfileHandler() const
    {
    return iProfileHandler;
    }

// -----------------------------------------------------------------------------
// CScpService::ServiceStorage
// -----------------------------------------------------------------------------
//
CScpServiceStorage& CScpService::ServiceStorage() const
    {
    return iServiceStorage;
    }

// -----------------------------------------------------------------------------
// CScpService::ServiceObserver
// -----------------------------------------------------------------------------
//
MCchServiceObserver& CScpService::ServiceObserver() const
    {
    return iServiceObserver;
    }

// -----------------------------------------------------------------------------
// CScpService::AddSubServiceL
// -----------------------------------------------------------------------------
//
CScpSubService& 
    CScpService::AddSubServiceL( TCCHSubserviceType aSubServiceType )
    {
    SCPLOGSTRING2( "CScpService[0x%x]::AddSubServiceL", this );

    TInt newId = GenerateNewSubServiceId();    
    CScpSubService* service = CScpSubService::NewL( newId, 
                                                    ServiceId(),
                                                    aSubServiceType,
                                                    *this );
    CleanupStack::PushL( service );
    iSubServices.AppendL( service );
    CleanupStack::Pop( service );

    return *iSubServices[ iSubServices.Count() - 1 ];
    }

// -----------------------------------------------------------------------------
// CScpService::SubServiceCount
// -----------------------------------------------------------------------------
//
TInt CScpService::SubServiceCount() const
    {
    SCPLOGSTRING2( "CScpService[0x%x]::SubServiceCount", this );

    return iSubServices.Count();
    }

// -----------------------------------------------------------------------------
// CScpService::GenerateNewSubServiceId
// -----------------------------------------------------------------------------
//
TInt CScpService::GenerateNewSubServiceId()
    {
    SCPLOGSTRING2( "CScpService[0x%x]::GenerateNewSubServiceId", this );

    iSubServiceIdCounter++;
    
    return iId + iSubServiceIdCounter;
    }

// -----------------------------------------------------------------------------
// CScpService::GetSubServiceIds
// -----------------------------------------------------------------------------
//
void CScpService::GetSubServiceIds( RArray<TInt>& aIds ) const
    {
    SCPLOGSTRING2( "CScpService[0x%x]::GetSubServiceIds", this );
    __ASSERT_DEBUG( aIds.Count() == 0, User::Panic( KNullDesC, KErrGeneral ) );

    for( TInt i=0; i<iSubServices.Count(); i++)
        {
        const CScpSubService* subService = iSubServices[ i ];
        
        aIds.Append( subService->Id() );
        }    
    }

// -----------------------------------------------------------------------------
// CScpService::GetSubService
// -----------------------------------------------------------------------------
//
CScpSubService* CScpService::GetSubService( TInt aId ) const
    {
    SCPLOGSTRING3( "CScpService[0x%x]::GetSubService id: %d", this, aId );

    for( TInt i=0; i<iSubServices.Count(); i++)
        {
        CScpService* self = const_cast<CScpService*>(this);
        CScpSubService* subService = self->iSubServices[ i ];
        if( aId == subService->Id() )
            {
            return subService;
            }
        }
    
    return NULL;
    }

// -----------------------------------------------------------------------------
// CScpService::RemoveSubService
// -----------------------------------------------------------------------------
//
TInt CScpService::RemoveSubService( TInt aId )
    {
    SCPLOGSTRING3( "CScpService[0x%x]::RemoveSubService id: %d", this, aId );

    for( TInt i=0; i<iSubServices.Count(); i++)
        {
        CScpSubService* subService = iSubServices[ i ];
        if( aId == subService->Id() )
            {
            delete subService;
            iSubServices.Remove( i );
            return KErrNone;
            }
        }

    return KErrNotFound;        
    }

// -----------------------------------------------------------------------------
// CScpService::ServiceId
// -----------------------------------------------------------------------------
//
TInt CScpService::ServiceId() const
    {
    SCPLOGSTRING2( "CScpService[0x%x]::ServiceId", this );

    return iServiceId;
    }

// -----------------------------------------------------------------------------
// CScpService::SetServiceId
// -----------------------------------------------------------------------------
//
void CScpService::SetServiceId( TInt aServiceId )
    {
    SCPLOGSTRING3( "CScpService[0x%x]::SetServiceId service id: %d", 
                   this, aServiceId );

    iServiceId = aServiceId;
    }

// -----------------------------------------------------------------------------
// CScpService::State
// -----------------------------------------------------------------------------
//
TInt CScpService::State( TCCHSubserviceType aSubServiceType, 
                         TCCHSubserviceState& aState ) const
    {
    SCPLOGSTRING2( "CScpService[0x%x]::State", this );

    TInt result = KErrNone;

    if( ECCHUnknown == aSubServiceType )
        {
        TCCHSubserviceState state( ECCHUninitialized );

        // Return other than unknown state if all the subservices are in same state 
        for ( TInt i=0; i<iSubServices.Count(); i++ )
            {
            const CScpSubService* subService = iSubServices[i];
            
            if( state == ECCHUninitialized )
                {
                state = subService->State();
                result = subService->LastReportedError();
                } 
            else
                {
                if ( state != subService->State() ||
                     result != subService->LastReportedError() )
                    {   
                    // Sub services are in different states or have
                    // different error codes
                    SCPLOGSTRING( "Query failed: different states or error codes" );

                    state = ECCHUninitialized;
                    result = KErrUnknown;
                    break;
                    }
                }
            }

        aState = state;
        }
    else
        {
        CScpSubService* subService = GetSubServiceByType( aSubServiceType );

        if( subService )
            {
            aState = subService->State();
            result = subService->LastReportedError();    
            }
        else
            {
            result = KErrNotFound;
            }
        }

    SCPLOGSTRING3( "State: %d error: %d", aState, result );
    return result;
    }

// -----------------------------------------------------------------------------
// CScpService::GetSubServiceByType
// -----------------------------------------------------------------------------
//
CScpSubService* CScpService::GetSubServiceByType( TCCHSubserviceType aSubServiceType ) const
    {
    SCPLOGSTRING3( "CScpService[0x%x]::GetSubService type: %d",
                   this, aSubServiceType );

    for( TInt i=0; i<iSubServices.Count(); i++)
        {
        CScpSubService* subService = iSubServices[ i ];
        if( subService->SubServiceType() == aSubServiceType )
            {
            return subService;
            }
        }

    return NULL; 
    }

// -----------------------------------------------------------------------------
// CScpService::ContainsSubServiceType
// -----------------------------------------------------------------------------
//
TBool CScpService::ContainsSubServiceType( TCCHSubserviceType aSubServiceType ) const
    {
    SCPLOGSTRING3( "CScpService[0x%x]::ContainsSubServiceType type: %d",
                   this, aSubServiceType );

    for( TInt i=0; i<iSubServices.Count(); i++)
        {
        CScpSubService* subService = iSubServices[ i ];
        if( subService->SubServiceType() == aSubServiceType )
            {
            return ETrue;
            }
        }

    return EFalse; 
    }


// -----------------------------------------------------------------------------
// CScpService::SubServicesContainSameSipProfile
// -----------------------------------------------------------------------------
//
TBool CScpService::SubServicesContainSameSipProfile() const
    {
    SCPLOGSTRING2( "CScpService[0x%x]::SubServicesContainSameSipProfile", this );    

    TInt sipProfileId( 0 );

    for( TInt i=0; i<iSubServices.Count(); i++)
        {
        TInt oldSipProfileId = sipProfileId;
        
        const CScpSubService* subService = iSubServices[ i ];
        sipProfileId = subService->SipProfileId();

        if( oldSipProfileId != 0 &&
            sipProfileId != oldSipProfileId )
            {
            return EFalse;
            }
        }

    return ETrue; 
    }

// -----------------------------------------------------------------------------
// CScpService::SetReserved
// -----------------------------------------------------------------------------
//
TInt CScpService::SetReserved( TBool aReserved, 
                               TCCHSubserviceType aSubServiceType )
    {
    SCPLOGSTRING4( "CScpService[0x%x]::SetReserved: %d type: %d", 
                   this, aSubServiceType, aReserved );    

    TInt result = KErrNone;

    if( aSubServiceType != ECCHUnknown )
        {
        CScpSubService* subService = GetSubServiceByType( aSubServiceType );

        if( subService )
            {
            // If reserving the sub service must be enabled.
            // It is always possible to free the service
            if( subService->State() == ECCHEnabled ||
                aReserved == EFalse )
                {
                subService->SetReserved( aReserved );
                }
            else
                {
                result = KErrNotSupported;
                }
            }
        else
            {
            result = KErrNotFound;
            }
        }
    else
        {
        for( TInt i=0; i<iSubServices.Count(); i++)
            {
            CScpSubService* subService = iSubServices[ i ];

            // If reserving the sub service must be enabled.
            // It is always possible to free the service
            if( subService->State() == ECCHEnabled ||
                aReserved == EFalse )
                {
                subService->SetReserved( aReserved );
                }
            else
                {
                result = KErrNotSupported;
                }
            }
        }

    return result;
    }

// -----------------------------------------------------------------------------
// CScpService::IsReserved
// -----------------------------------------------------------------------------
//
TBool CScpService::IsReserved( TCCHSubserviceType aSubServiceType ) const
    {
    SCPLOGSTRING3( "CScpService[0x%x]::IsReserved: type: %d", 
                   this, aSubServiceType );

    TBool result = EFalse;

    if( aSubServiceType == ECCHUnknown )
        {
        for( TInt i=0; i<iSubServices.Count(); i++)
            {
            CScpSubService* subService = iSubServices[ i ];

            TBool reserved = subService->IsReserved();

            if( reserved )
                {
                result = ETrue;
                break;
                }
            }
        }
    else
        {
        CScpSubService* subService = GetSubServiceByType( aSubServiceType );

        if( subService )
            {
            result = subService->IsReserved();
            }
        }

    return result;
    }

// -----------------------------------------------------------------------------
// CScpService::SetAccessPointId
// -----------------------------------------------------------------------------
//
TInt CScpService::SetAccessPointId( TCCHSubserviceType aSubServiceType, 
                                    TScpAccessPointType aAccessPointType, 
                                    TInt aAccessPointId )
    {
    SCPLOGSTRING4( "CScpService[0x%x]::SetAccessPointId: type: %d id: %d", 
                    this, aSubServiceType, aAccessPointId );
    __ASSERT_DEBUG( iSubServices.Count() > 0, User::Panic( KNullDesC, KErrGeneral ) );

    TInt result = KErrNotFound;

    if( IsAccessPointModifiable( aSubServiceType ) )
        {
        TBool sipConnectionCreated = EFalse;
        CScpSipConnection* sipConnection = GetSipConnection( aSubServiceType,
                                                             sipConnectionCreated );
        if( sipConnection )
            {      
            switch ( aAccessPointType )
                {
                case EScpIap:
                    {
                    result = sipConnection->SetIap( aAccessPointId );
                    }
                    break;
            
                case EScpSnap:
                    {
                    result = sipConnection->SetSnap( aAccessPointId );
                    }
                    break;

                default:
                    break;

                }

            if( sipConnectionCreated )
                {
                delete sipConnection;
                }
            }
        }
    else
        {
        result = KErrNotSupported;
        }

    return result;
    }

// -----------------------------------------------------------------------------
// CScpService::IsAccessPointModifiable
// -----------------------------------------------------------------------------
//
TBool CScpService::IsAccessPointModifiable( TCCHSubserviceType aSubServiceType ) const
    {
    SCPLOGSTRING3( "CScpService[0x%x]::IsAccessPointModifiable: type: %d", 
                    this, aSubServiceType );

    TBool modifiable = EFalse;

    TCCHSubserviceState state = ECCHUninitialized;

    TInt error = State( aSubServiceType, state );

    if( state == ECCHDisabled || ( state == ECCHConnecting && error != KErrNone ) )

        {
        if( !IsReserved( aSubServiceType ) )
            {
            // We can't set any valid access point values if different
            // sip profiles are in use
            if( aSubServiceType != ECCHUnknown || ( aSubServiceType == ECCHUnknown &&
                                                    SubServicesContainSameSipProfile() ) )
                {
                modifiable = ETrue;
                } 
            }
        }

    return modifiable;
    }

// -----------------------------------------------------------------------------
// CScpService::GetSipConnection
// -----------------------------------------------------------------------------
//
CScpSipConnection* CScpService::GetSipConnection( TCCHSubserviceType aSubServiceType,
                                                  TBool& aSipConnectionCreated )
    {
    SCPLOGSTRING3( "CScpService[0x%x]::GetSipConnection: type: %d", 
                    this, aSubServiceType );

    CScpSipConnection* sipConnection = NULL;
    CScpSubService* subService = NULL;

    if( aSubServiceType == ECCHUnknown && iSubServices.Count() > 0 )
        {
        subService = iSubServices[0];
        }
    else
        {
        subService = GetSubServiceByType( aSubServiceType );
        }

    if( subService )
        {
        TInt sipProfileId = subService->SipProfileId();

        if( iProfileHandler.ProfileExists( sipProfileId ) )
            {
            if( iProfileHandler.SipConnectionExists( sipProfileId ) )
                {
                sipConnection = iProfileHandler.GetSipConnection( sipProfileId );
                }
            else
                {
                aSipConnectionCreated = ETrue;
                TRAP_IGNORE( sipConnection = 
                             iProfileHandler.CreateSipConnectionL( sipProfileId ) );
                }
            }
        }

    return sipConnection;  
    }

// -----------------------------------------------------------------------------
// CScpService::IsAllSubservicesDisabled
// -----------------------------------------------------------------------------
//
TBool CScpService::IsAllSubservicesDisabled() const
    {
    TBool response( ETrue );
       
    for ( TInt i( 0 ); i < iSubServices.Count() && response; i++ )
        {
        SCPLOGSTRING3( "CScpService[0x%x]::IsAllSubservicesDisabled: state: %d", 
                            this, iSubServices[ i ]->State() );
        if ( ECCHDisabled != iSubServices[ i ]->State() )
            {
            response = EFalse;
            }
        }
    
    return response;
    }
        
// -----------------------------------------------------------------------------
// CScpService::ChangeLastReportedErrors
// -----------------------------------------------------------------------------
//
void CScpService::ChangeLastReportedErrors(
    const TInt aError )
    {
    for ( TInt i( 0 ); i < iSubServices.Count(); i++ )
        {
        iSubServices[ i ]->SetLastReportedError( aError );
        }   
    }

// End of file
