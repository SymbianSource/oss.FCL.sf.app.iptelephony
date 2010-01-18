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
* Description:  Service storage class.
*
*/


#include "scpservicestorage.h"
#include "scplogger.h"
#include "scpservice.h"
#include "scpsubservice.h"
#include "scpsettinghandler.h"

const TInt KServiceIdCounterIncrement = 100;

// -----------------------------------------------------------------------------
// CScpServiceStorage::CScpServiceStorage
// -----------------------------------------------------------------------------
//
CScpServiceStorage::CScpServiceStorage( CScpSettingHandler& aSettingHandler ) : 
    iSettingHandler( aSettingHandler )
    {
    }

// -----------------------------------------------------------------------------
// CScpServiceStorage::NewL
// -----------------------------------------------------------------------------
//
CScpServiceStorage* CScpServiceStorage::NewL( CScpSettingHandler& aSettingHandler )
    {
    SCPLOGSTRING( "CScpServiceStorage::NewL" );

    CScpServiceStorage* self = new ( ELeave ) CScpServiceStorage( aSettingHandler );
    return self;
    }

// -----------------------------------------------------------------------------
// CScpServiceStorage::~CScpServiceStorage
// -----------------------------------------------------------------------------
//
CScpServiceStorage::~CScpServiceStorage()
    {
    SCPLOGSTRING( "CScpServiceStorage::~CScpServiceStorage" );

    iServiceItems.ResetAndDestroy();
    iServiceItems.Close();
    }

// -----------------------------------------------------------------------------
// CScpServiceStorage::CreateServiceL
// -----------------------------------------------------------------------------
//
CScpService& CScpServiceStorage::CreateServiceL( 
    TInt aServiceId, 
    CScpProfileHandler& aProfileHandler,
    MCchServiceObserver& aServiceObserver )
    {
    SCPLOGSTRING( "CScpServiceStorage::CreateServiceL" );

    CScpService* service( NULL );
    
    // Check that the service id does not already exist
    service = GetServiceByServiceId( aServiceId );
    if( service )
        {
        User::Leave( KErrAlreadyExists );
        }

    TInt newId = GenerateNewInternalIdForService();

    service = CScpService::NewL( newId, 
                                 aServiceId, 
                                 aProfileHandler,
                                 *this,
                                 aServiceObserver );

    CleanupStack::PushL( service );
    iServiceItems.AppendL( service );
    CleanupStack::Pop( service );

    CScpService* lastService = iServiceItems[ iServiceItems.Count() - 1 ];
    return *lastService;    
    }

// -----------------------------------------------------------------------------
// CScpServiceStorage::GenerateNewServiceId
// -----------------------------------------------------------------------------
//
TInt CScpServiceStorage::GenerateNewInternalIdForService()
    {
    SCPLOGSTRING( "CScpServiceStorage::GenerateNewInternalIdForService" );

    iServiceIdCounter += KServiceIdCounterIncrement;
    
    return iServiceIdCounter;
    }

// -----------------------------------------------------------------------------
// CScpServiceStorage::RemoveService
// -----------------------------------------------------------------------------
//
TInt CScpServiceStorage::RemoveService( TInt aId )
    {    
    SCPLOGSTRING2( "CScpServiceStorage::RemoveService id: %d", aId );

    for( TInt i=0; i<iServiceItems.Count(); i++)
        {
        CScpService* service = iServiceItems[ i ];
        if( service && aId == service->Id() )
            {
            delete service;
            iServiceItems.Remove( i );
            return KErrNone;
            }
        }

    return KErrNotFound;      
    }

// -----------------------------------------------------------------------------
// CScpServiceStorage::RemoveDisabledServices
// -----------------------------------------------------------------------------
//
void CScpServiceStorage::RemoveDisabledServices()
    {
    SCPLOGSTRING( "CScpServiceStorage::RemoveEmptyServices" );

    RArray< TInt > subServiceIds;
    RArray< TInt > removedServiceIds;

    for( TInt i=0; i<iServiceItems.Count(); i++)
        {
        CScpService* service = iServiceItems[ i ];
        if( service )
            {
            subServiceIds.Reset();
                
            service->GetSubServiceIds( subServiceIds );

            TBool remove( ETrue );

            // Remove if there are no sub services
            if( subServiceIds.Count() > 0 )
                {
                for ( TInt j=0; j<subServiceIds.Count(); j++ )
                    {
                    CScpSubService* subService = 
                        service->GetSubService( subServiceIds[ j ] );
                    
                    // Don't remove if there is at least one enabled or disconnectin
                    // sub service
                    if( subService->EnableRequestedState() == CScpSubService::EScpEnabled ||
                        subService->State() == ECCHDisconnecting )
                        {
                        remove = EFalse;
                        }
                    else
                        {
                        service->RemoveSubService( subService->Id() );
                        }
                    }
                }

            if( remove ) 
                {
                removedServiceIds.Append( service->Id() ); 
                }
            }
        }

    for( TInt i=0; i<removedServiceIds.Count(); i++ )
        {
        RemoveService( removedServiceIds[ i ] );
        }

    removedServiceIds.Close();
    subServiceIds.Close();
    }

// -----------------------------------------------------------------------------
// CScpServiceStorage::GetService
// -----------------------------------------------------------------------------
//
CScpService* CScpServiceStorage::GetService( TInt aId ) const
    {
    SCPLOGSTRING2( "CScpServiceStorage::GetService id: %d", aId );

    for( TInt i=0; i<iServiceItems.Count(); i++)
        {
        CScpService* service = iServiceItems[ i ];
        if( aId == service->Id() )
            {
            return service;
            }
        }
    
    return NULL;    
    }

// -----------------------------------------------------------------------------
// CScpServiceStorage::GetServiceByServiceId
// -----------------------------------------------------------------------------
//
CScpService* CScpServiceStorage::GetServiceByServiceId( TUint aServiceId ) const
    {
    SCPLOGSTRING2( "CScpServiceStorage::GetServiceByServiceId id: %d", aServiceId );

    for( TInt i=0; i<iServiceItems.Count(); i++)
        {
        CScpService* service = iServiceItems[ i ];
        if( aServiceId == service->ServiceId() )
            {
            return service;
            }
        }
    
    return NULL;
    }

// -----------------------------------------------------------------------------
// CScpServiceStorage::GetSubService
// -----------------------------------------------------------------------------
//
CScpSubService* CScpServiceStorage::GetSubService( TInt aId ) const
    {
    SCPLOGSTRING2( "CScpServiceStorage::GetSubService id: %d", aId );

    CScpSubService* returnSubService( NULL );

    RArray<TInt> subServiceIds;
    for( TInt i=0; i<iServiceItems.Count() && !returnSubService; i++)
        {
        CScpService* service = iServiceItems[ i ];
        
        subServiceIds.Reset();
        service->GetSubServiceIds( subServiceIds );

        for( TInt j=0; j<subServiceIds.Count(); j++)
            {
            returnSubService = service->GetSubService( subServiceIds[ j ] );

            if( returnSubService )
                {
                if( returnSubService->Id() == aId )
                    {
                    break;
                    }
                else
                    {
                    returnSubService = NULL;
                    }
                }
            }
        }

    subServiceIds.Close();
    
    return returnSubService;
    }

// -----------------------------------------------------------------------------
// CScpServiceStorage::GetServiceIds
// -----------------------------------------------------------------------------
//
void CScpServiceStorage::GetServiceIds( RArray<TInt>& aIds ) const
    {
    SCPLOGSTRING( "CScpServiceStorage::GetServiceIds" );
    __ASSERT_DEBUG( aIds.Count() == 0, User::Panic( KNullDesC, KErrGeneral ) );

    for( TInt i=0; i<iServiceItems.Count(); i++)
        {
        CScpService* service = iServiceItems[ i ];
        aIds.Append( service->Id() );
        }
    }

// -----------------------------------------------------------------------------
// CScpServiceStorage::GetSubServiceIds
// -----------------------------------------------------------------------------
//
void CScpServiceStorage::GetSubServiceIds( RArray<TInt>& aIds ) const
    {
    SCPLOGSTRING( "CScpServiceStorage::GetSubServiceIds" );
    __ASSERT_DEBUG( aIds.Count() == 0, User::Panic( KNullDesC, KErrGeneral ) );

    RArray<TInt> subServiceIds;

    for( TInt i=0; i<iServiceItems.Count(); i++ )
        {
        CScpService* service = iServiceItems[ i ];
        
        subServiceIds.Reset();
        service->GetSubServiceIds( subServiceIds );

        for( TInt j=0; j<subServiceIds.Count(); j++ )
            {
            aIds.Append( subServiceIds[ j ] );
            }
        }

    subServiceIds.Close();
    }

// -----------------------------------------------------------------------------
// CScpServiceStorage::GetSubServiceIds
// -----------------------------------------------------------------------------
//
void CScpServiceStorage::GetSubServiceIds( TCCHSubserviceType aSubServiceType,
                                           RArray<TInt>& aIds ) const
    {
    SCPLOGSTRING( "CScpServiceStorage::GetSubServiceIds" );
    __ASSERT_DEBUG( aIds.Count() == 0, User::Panic( KNullDesC, KErrGeneral ) );

    RArray< TInt > subServiceIds;
    GetSubServiceIds( subServiceIds );
    
    for( TInt i=0; i<subServiceIds.Count(); i++ )
        {
        CScpSubService* subService;
        subService = GetSubService( subServiceIds[ i ] );

        if( subService && subService->SubServiceType() == aSubServiceType )
            {
            aIds.Append( subService->Id() );
            }
        }
    
    subServiceIds.Close();
    }

// -----------------------------------------------------------------------------
// CScpServiceStorage::GetSubServiceIds
// -----------------------------------------------------------------------------
//
void CScpServiceStorage::GetSubServiceIds( TUint32 aProfileId,
                                           TCCHSubserviceType aSubServiceType,  
                                           RArray<TInt>& aIds ) const
    {
    SCPLOGSTRING3( "CScpServiceStorage::GetSubServiceIds profileId: %d type: %d",
                   aProfileId, aSubServiceType );
    __ASSERT_DEBUG( aIds.Count() == 0, User::Panic( KNullDesC, KErrGeneral ) );

    RArray< TInt > subServiceIds;
    GetSubServiceIds( subServiceIds );
    
    for( TInt i=0; i<subServiceIds.Count(); i++ )
        {
        CScpSubService* subService;
        subService = GetSubService( subServiceIds[ i ] );

        if( subService &&
            subService->SipProfileId() == aProfileId &&
            ( subService->SubServiceType() == aSubServiceType ||
            aSubServiceType == ECCHUnknown ) )
            {
            aIds.Append( subService->Id() );
            }
        }
    
    subServiceIds.Close();
    }

// -----------------------------------------------------------------------------
// CScpServiceStorage::IsSubServiceEnabled
// -----------------------------------------------------------------------------
//
TBool CScpServiceStorage::IsSubServiceEnabled( TUint32 aProfileId ) const
    {
    SCPLOGSTRING2( "CScpServiceStorage::IsSubServiceEnabled profile id: %d",
                   aProfileId );

    TBool result( EFalse );
    RArray< TInt > subServiceIds;

    GetSubServiceIds( aProfileId, ECCHUnknown, subServiceIds );

    for( TInt i=0; i<subServiceIds.Count(); i++ )
        {
        CScpSubService* subService = GetSubService( subServiceIds[ i ] );
        if( subService )
            {
            // Find at least one sub service that is still enabled
            if( subService->EnableRequestedState() == CScpSubService::EScpEnabled )
                {
                result = ETrue;
                break;
                }
            }
        }

    subServiceIds.Close();
    return result;
    }

// -----------------------------------------------------------------------------
// CScpServiceStorage::AreAllSubServicesDisconnected
// -----------------------------------------------------------------------------
//
TBool CScpServiceStorage::AreAllSubServicesDisconnected( TUint32 aProfileId ) const
    {
    SCPLOGSTRING2( "CScpServiceStorage::AreAllSubServicesDisconnected profile id: %d",
                   aProfileId );

    TBool result( ETrue );
    RArray< TInt > subServiceIds;

    GetSubServiceIds( aProfileId, ECCHUnknown, subServiceIds );

    for( TInt i=0; i<subServiceIds.Count(); i++ )
        {
        CScpSubService* subService = GetSubService( subServiceIds[ i ] );
        if( subService )
            {
            // Find at least one sub service that has been disabled but
            // it has not yet disconnected the service
            if( subService->EnableRequestedState() == CScpSubService::EScpDisabled &&
                subService->SubServiceDisconnected() == EFalse )
                {
                result = EFalse;
                break;
                }
            }
        }

    subServiceIds.Close();
    return result;
    }

// -----------------------------------------------------------------------------
// CScpServiceStorage::SettingsHandler
// -----------------------------------------------------------------------------
//
CScpSettingHandler& CScpServiceStorage::SettingsHandler()
    {
    SCPLOGSTRING( "CScpServiceStorage::SettingsHandler" );

    return iSettingHandler;
    }


#ifdef _DEBUG
// -----------------------------------------------------------------------------
// CScpServiceStorage::GetDebugInfo
// -----------------------------------------------------------------------------
//
void CScpServiceStorage::GetDebugInfo( TDes& aInfo ) const
    {
    TInt serviceCount = iServiceItems.Count();
    TInt subServiceCount( 0 );
    TInt enabledSubServices( 0 );

    for( TInt i=0; i<serviceCount; i++ )
        {
        CScpService* service = iServiceItems[ i ];
        subServiceCount += service->SubServiceCount();
    
        RArray<TInt> subServiceIds;
        service->GetSubServiceIds( subServiceIds );

        for( TInt j=0; j<subServiceIds.Count(); j++ )
            {
            CScpSubService* subService = service->GetSubService( subServiceIds[ j ] );
            
            //if( subService->IsEnabled() )
            if( subService->EnableRequestedState() == CScpSubService::EScpEnabled )
                {
                enabledSubServices++;
                }
            }

        subServiceIds.Close();
        }

    TBuf< 255 > buffer;
    buffer.Format( _L( "\nServices: %d\n SubServices: %d\n Enabled: %d\n" ),
                  serviceCount, subServiceCount, enabledSubServices ); 

    aInfo.Append( buffer );
    }
#endif

//  End of File  

