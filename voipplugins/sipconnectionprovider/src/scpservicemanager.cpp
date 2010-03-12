/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <e32std.h>
#include <escapeutils.h>

#include "scpservicemanager.h"
#include "scpsettinghandler.h"
#include "scpservicestorage.h"
#include "scputility.h"
#include "scplogger.h"
#include "scpprofilehandler.h"
#include "scpservice.h"
#include "scpsubservice.h"
#include "scpsipconnection.h"
#include "scpstatecontainer.h"

const TInt KUsernameMaxLength = 255;
const TInt KDomainMaxLength = 255;
const TInt KTempBufMaxLength = 255;

#ifdef _DEBUG
const TInt KDebugInfoMaxLength = 255;
#endif

// -----------------------------------------------------------------------------
// CScpServiceManager::CScpServiceManager()
// -----------------------------------------------------------------------------
//
CScpServiceManager::CScpServiceManager( MCchServiceObserver& aServiceObserver ) :
    iServiceObserver( aServiceObserver )
    {
    SCPLOGSTRING( "CScpServiceManager::CScpServiceManager");
    }

// -----------------------------------------------------------------------------
// CScpServiceManager::NewL
// -----------------------------------------------------------------------------
//
CScpServiceManager* CScpServiceManager::NewL( MCchServiceObserver& aServiceObserver )
    {
    SCPLOGSTRING( "CScpServiceManager::NewL");

    CScpServiceManager* self = new ( ELeave ) CScpServiceManager( aServiceObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CScpServiceManager::ConstructL
// -----------------------------------------------------------------------------
//
void CScpServiceManager::ConstructL()
    {
    SCPLOGSTRING( "CScpServiceManager::ConstructL");
    
    iProfileHandler = CScpProfileHandler::NewL();
    iSettingHandler = CScpSettingHandler::NewL( *iProfileHandler );
    iServiceStorage = CScpServiceStorage::NewL( *iSettingHandler );  
    

    iStateContainer.InitializeL();
    }

// -----------------------------------------------------------------------------
// CScpServiceManager::~CScpServiceManager
// -----------------------------------------------------------------------------
//
CScpServiceManager::~CScpServiceManager()
    {    
    SCPLOGSTRING( "CScpServiceManager::~CScpServiceManager"); 

    delete iServiceStorage;
    delete iSettingHandler;
    delete iProfileHandler;
    }

// -----------------------------------------------------------------------------
// CScpServiceManager::GetServiceL
// -----------------------------------------------------------------------------
//
CScpService* CScpServiceManager::GetServiceL( TUint aServiceId,
                                              TCCHSubserviceType aSubServiceType )
    {
    SCPLOGSTRING2( "CScpServiceManager::GetService: %i", aServiceId );

    CScpService* service( NULL );

    if( iSettingHandler->ServiceExistsL( aServiceId ) )
        {
        service = iServiceStorage->GetServiceByServiceId( aServiceId );

        if( service )
            {
            TRAPD( result, iSettingHandler->UpdateSettingsL( *service, aSubServiceType ) );
             
            if( result != KErrNone )
                {
#ifndef SCP_UNIT_TEST
                __ASSERT_DEBUG( EFalse, User::Panic( KNullDesC, KErrGeneral ) );
#endif
                service = NULL;                    
                }
            if ( KErrNoMemory == result )
                {
                User::Leave( KErrNoMemory );
                }
            }
        }

    return service;
    }

// -----------------------------------------------------------------------------
// CScpServiceManager::CreateServiceL
// -----------------------------------------------------------------------------
//
CScpService* CScpServiceManager::CreateServiceL( TUint aServiceId,
                                                 TCCHSubserviceType aSubServiceType )
    {
    SCPLOGSTRING2( "CScpServiceManager::CreateServiceL: %i", aServiceId );

    if( !iSettingHandler->ServiceExistsL( aServiceId ) )
        {
        User::Leave( KErrNotFound );
        }

    // Check if it is already available
    CScpService* service = iServiceStorage->GetServiceByServiceId( aServiceId );

    if( !service )
        {
        CScpService& createdService = 
            iServiceStorage->CreateServiceL( aServiceId, 
                                             *iProfileHandler, 
                                             iServiceObserver );
        
        service = &createdService;

        TRAPD( result, iSettingHandler->UpdateSettingsL( *service, aSubServiceType ) );
        
        if( result != KErrNone )
            {
            // Remove the service if something went wrong
            iServiceStorage->RemoveService( service->Id() );
            User::Leave( result );
            }
        }
    else
        {
#ifndef SCP_UNIT_TEST
        __ASSERT_DEBUG( EFalse, User::Panic( KNullDesC, KErrAlreadyExists ) );
#endif
        }
  
    return service;
    }
    
// -----------------------------------------------------------------------------
// CScpServiceManager::EnableServiceL
// -----------------------------------------------------------------------------
//
void CScpServiceManager::EnableServiceL( TUint aServiceId,
                                         TCCHSubserviceType aSubServiceType )
    {
    SCPLOGSTRING2( "CScpServiceManager::EnableServiceL service:%i", aServiceId );
    SCPLOGSTRING2( "CScpServiceManager::EnableServiceL type:%i", aSubServiceType );
    __ASSERT_DEBUG( aServiceId > 0, User::Panic( KNullDesC, KErrNotFound ) );
        
    CScpService* service = GetServiceL( aServiceId, aSubServiceType );
    
    if( !service )
        {
        service = CreateServiceL( aServiceId, aSubServiceType );
        }

    RArray< TInt > subServiceIds;
    CleanupClosePushL( subServiceIds );
    service->GetSubServiceIds( subServiceIds );
    
    for( TInt i=0 ;i<subServiceIds.Count(); i++ )
        {
        CScpSubService* subService = 
            service->GetSubService( subServiceIds[ i ] );
        
        if( subService )
            {
            SCPLOGSTRING2( "Subservice's type: %d", subService->SubServiceType() );

            // Check is VoIP supported
            if ( !iSettingHandler->IsVoIPSupported() && 
                 ECCHVoIPSub == subService->SubServiceType() )
                {
                SCPLOGSTRING( "No support for VoIP" );
                subService = NULL;
                }

            if ( subService && 
                 ( aSubServiceType == subService->SubServiceType() ||
                 ECCHUnknown == aSubServiceType ) )
                {
                UpdateProfileValuesL( aServiceId, aSubServiceType );                
                subService->EnableL();
                }
            }
        }

    CleanupStack::PopAndDestroy( &subServiceIds );                 

    SCPLOGSTRING( "CScpServiceManager::EnableServiceL out" );
    }

// -----------------------------------------------------------------------------
// CScpServiceManager::EnableServiceL
// -----------------------------------------------------------------------------
//
void CScpServiceManager::EnableServiceL( TUint aServiceId,
                                         TCCHSubserviceType aSubServiceType,
                                         TUint /*aIapId*/ )
    {
    // : Change sip profiles IAP settings, but
    // If SNAP != 0 add the IAP to SNAP (CommsDB)
    EnableServiceL( aServiceId, aSubServiceType );
    }

// -----------------------------------------------------------------------------
// CScpServiceManager::DisableServiceL
// -----------------------------------------------------------------------------
//
void CScpServiceManager::DisableServiceL( TUint aServiceId,
                                          TCCHSubserviceType aSubServiceType )
    {
    SCPLOGSTRING2( "CScpServiceManager::DisableService service:%i", aServiceId );
    __ASSERT_DEBUG( aServiceId > 0, User::Panic( KNullDesC, KErrNotFound ) );

    CScpService* service = GetServiceL( aServiceId, aSubServiceType );
    if( !service )
        {
        service = CreateServiceL( aServiceId, aSubServiceType );
        }

    RArray< TInt > subServiceIds;
    CleanupClosePushL( subServiceIds );
    service->GetSubServiceIds( subServiceIds );
    
    if( subServiceIds.Count() == 0 )
        {
        // Nothing to disable
        User::Leave( KErrNotFound );
        }

    TBool atLeastOneDisableSucceeded = EFalse; 

    for( TInt i=0 ;i<subServiceIds.Count(); i++ )
        {
        CScpSubService* subService = service->GetSubService( subServiceIds[ i ] );
        if( subService && ( aSubServiceType == subService->SubServiceType() ||
                            aSubServiceType == ECCHUnknown ) )
            {
            TInt result = subService->Disable(); 
            if( result == KErrNone )
                {
                atLeastOneDisableSucceeded = ETrue;
                }
            }
        }

    if( !atLeastOneDisableSucceeded )
        {
        User::Leave( KErrNotSupported );
        }

    CleanupStack::PopAndDestroy( &subServiceIds );

    // After the disabling there might be some cleaning to do
    iServiceStorage->RemoveDisabledServices();
    }

// -----------------------------------------------------------------------------
// CScpServiceManager::IsAvailableL
// -----------------------------------------------------------------------------
//
TBool CScpServiceManager::IsAvailableL( TUint /*aServiceId*/,
                                        TCCHSubserviceType /*a*/,
                                        const RArray<TUint32>& /*aIapIdArray*/ ) const
    {
    SCPLOGSTRING( "CScpServiceManager::IsAvailableL" );
    // :
    return ETrue;
    }
 
// -----------------------------------------------------------------------------
// CScpServiceManager::GetServiceState
// -----------------------------------------------------------------------------
//
TInt CScpServiceManager::GetServiceState( TUint aServiceId,
                                          TCCHSubserviceType aSubServiceType, 
                                          TCCHSubserviceState& aState ) const
    {
    SCPLOGSTRING2( "CScpServiceManager::GetServiceState service:%i", aServiceId );

    TInt result( KErrNone );
    aState = ECCHDisabled;
    
    CScpServiceManager* self = const_cast<CScpServiceManager*>(this);
    CScpService* service = NULL;
    TRAPD( err, service = self->GetServiceL( aServiceId, aSubServiceType ) );
    if ( KErrNoMemory == err )
        {
        return err;
        }
    
    if( service )
        {
        result = service->State( aSubServiceType, aState );
        }
    else
        {
        TRAP( result, service = CreateTemporaryServiceL( aServiceId, aSubServiceType ) );
        
        if( result == KErrNone && service )
            {
            result = service->State( aSubServiceType, aState );
            delete service;
            }
        }    

    return result;
    }
    
// -----------------------------------------------------------------------------
// CScpServiceManager::GetServiceNetworkInfo
// -----------------------------------------------------------------------------
//
TInt CScpServiceManager::GetServiceNetworkInfo( TUint aServiceId,
                                                TCCHSubserviceType aSubServiceType,
                                                TUint32& aSnapId, 
                                                TUint32& aIapId,
                                                TBool& aSnapLocked,
                                                TBool& aPasswordSet  ) const
    {
    SCPLOGSTRING( "CScpServiceManager::GetServiceNetworkInfo" );

    // Get service network info
    TRAPD( ret, GetServiceNetworkInfoL( aServiceId,
                                        aSubServiceType,
                                        aSnapId,
                                        aIapId,
                                        aSnapLocked,
                                        aPasswordSet) );
                    
    return ret;
    }
    
// -----------------------------------------------------------------------------
// CScpServiceManager::SetSnapId
// -----------------------------------------------------------------------------
//
TInt CScpServiceManager::SetSnapId( TUint aServiceId,
                                    TCCHSubserviceType aSubServiceType,
                                    const TUint aSnapId )
    {
    SCPLOGSTRING4( "CScpServiceManager::SetSnapId service id: %d type: %d snap: %d",
                   aServiceId, aSubServiceType, aSnapId );

    return SetAccessPointId( aServiceId, aSubServiceType, EScpSnap, aSnapId );
    }

// -----------------------------------------------------------------------------
// CScpServiceManager::SetIapId
// -----------------------------------------------------------------------------
//
TInt CScpServiceManager::SetIapId( TUint aServiceId,
                                   TCCHSubserviceType aSubServiceType,
                                   TUint aIapId )
    {
    SCPLOGSTRING4( "CScpServiceManager::SetIapId service id: %d type: %d iap: %d",
                   aServiceId, aSubServiceType, aIapId );

    return SetAccessPointId( aServiceId, aSubServiceType, EScpIap, aIapId );
    }

// -----------------------------------------------------------------------------
// CScpServiceManager::SetAccessPointId
// -----------------------------------------------------------------------------
//
TInt CScpServiceManager::SetAccessPointId( TUint aServiceId,
                                           TCCHSubserviceType aSubServiceType, 
                                           TScpAccessPointType aAccessPointType, 
                                           TInt aAccessPointId )
    {
    SCPLOGSTRING4( "CScpServiceManager[0x%x]::SetAccessPointId: type: %d id: %d", 
                    this, aSubServiceType, aAccessPointId );

    TBool serviceCreated = EFalse;
    TInt result = KErrNone;

    CScpService* service = iServiceStorage->GetServiceByServiceId( aServiceId );

    if( !service )
        {
        TRAP( result, service = CreateTemporaryServiceL( aServiceId, 
                                                         ECCHUnknown ) );

        if( result == KErrNone )
            {
            serviceCreated = ETrue;
            }
        }

    if( result == KErrNone )
        {
        result = service->SetAccessPointId( aSubServiceType, 
                                            aAccessPointType, 
                                            aAccessPointId );
        }
    
    if( serviceCreated )
        {
        delete service;
        }
    
    return result;
    }
 
// -----------------------------------------------------------------------------
// CScpServiceManager::CreateTemporaryServiceL
// -----------------------------------------------------------------------------
//   
CScpService* CScpServiceManager::CreateTemporaryServiceL( TInt aServiceId,
                                                          TCCHSubserviceType aSubServiceType ) const
    {
    SCPLOGSTRING3( "CScpServiceManager::CreateTemporaryServiceL service id: %d type: %d", 
                    aServiceId,
                    aSubServiceType );

    // Create a temporary service
    CScpService* service = CScpService::NewL( aServiceId, 
                                              aSubServiceType, 
                                              *iProfileHandler,
                                              *iServiceStorage,  
                                              iServiceObserver );

    CleanupStack::PushL( service );

    service->SetServiceId( aServiceId );

    iSettingHandler->UpdateSettingsL( *service, aSubServiceType );

    CleanupStack::Pop( service );

    return service;
    }

// -----------------------------------------------------------------------------
// CScpServiceManager::GetServiceInfoL
// -----------------------------------------------------------------------------
//
void CScpServiceManager::GetServiceInfoL( TUint aServiceId,
                                          TCCHSubserviceType aSubServiceType,
                                          RBuf& aBuffer ) const
    {
    SCPLOGSTRING3( "CScpServiceManager::GetServiceInfoL service id: %d type: %d", 
                    aServiceId, aSubServiceType );  

// Info request about the state of SCP for debugging purposes
#ifdef _DEBUG

    TInt infoCode = 0xAAA;
    if( aServiceId == infoCode )
        {
        aBuffer.Close();
        HBufC* buf = HBufC::NewL( KDebugInfoMaxLength );
        TPtr ptr = buf->Des();

        iServiceStorage->GetDebugInfo( ptr );
        iProfileHandler->GetDebugInfo( ptr );

        aBuffer.Assign( buf );    

        return;
        }
#endif

    TBool sipConnectionCreated( EFalse );
    
    CScpSipConnection* sipConnection = GetSipConnectionL( aServiceId,
                                                          aSubServiceType, 
                                                          sipConnectionCreated );

    if( sipConnectionCreated )
        {
        CleanupStack::PushL( sipConnection );
        }
      
    TBuf8<KUsernameMaxLength> username;
    TBuf8<KDomainMaxLength> domain;
    TBuf16<KTempBufMaxLength> tempBuf;
    
    User::LeaveIfError( sipConnection->GetUsername( username ) );
    User::LeaveIfError( sipConnection->GetDomain( domain ) );
    
    _LIT16( KUsername, "username=");
    _LIT16( KDomain, "domain=");
    _LIT16( KSpace, " ");
    
    HBufC* buffer = HBufC::NewL( KUsername().Length() +
                                 KDomain().Length() +
                                 ( KSpace().Length() * 2) +
                                 username.Length() +
                                 domain.Length() );
                                 
    buffer->Des().Copy( KUsername );
    tempBuf.Copy( username );
    buffer->Des().Append( tempBuf );
    buffer->Des().Append( KSpace );
    buffer->Des().Append( KDomain );
    tempBuf.Copy( domain );
    buffer->Des().Append( tempBuf );
    buffer->Des().Append( KSpace );
    
    aBuffer.Assign( buffer );    

    if( sipConnectionCreated )
        {
        CleanupStack::PopAndDestroy( sipConnection );
        }

    SCPLOGSTRING2( "Result: %S", &aBuffer );
    }

// -----------------------------------------------------------------------------
// CScpServiceManager::GetSipConnectionL
// -----------------------------------------------------------------------------
//
CScpSipConnection* CScpServiceManager::GetSipConnectionL( 
    TUint aServiceId,
    TCCHSubserviceType aSubServiceType,
    TBool& aConnectionCreated ) const
    {
    SCPLOGSTRING2( "CScpServiceManager::GetSipConnectionL service:%i", 
                    aServiceId );

    CScpSipConnection* sipConnection( NULL );
    aConnectionCreated = EFalse;
    TUint32 profileId( 0 );

    // Get SIP profile for service
    iSettingHandler->GetSipProfileIdByTypeL( aServiceId,
                                             aSubServiceType, 
                                             profileId );

    if( profileId != 0 )
        {
        if( iProfileHandler->SipConnectionExists( profileId ) )
            {
            sipConnection = iProfileHandler->GetSipConnection( profileId );
            }
        else
            {
            if( iProfileHandler->ProfileExists( profileId ) )
                {
                sipConnection = iProfileHandler->CreateSipConnectionL( profileId );
                aConnectionCreated = ETrue;
                }
            }
        }

    if( !sipConnection )
        {
        User::Leave( KErrNotFound );
        }

    return sipConnection;
    }

// -----------------------------------------------------------------------------
// CScpServiceManager::GetServiceNetworkInfoL
// -----------------------------------------------------------------------------
//
void CScpServiceManager::GetServiceNetworkInfoL( TUint aServiceId,
                                                 TCCHSubserviceType aSubServiceType,
                                                 TUint32& aSnapId, 
                                                 TUint32& aIapId,
                                                 TBool& aSnapLocked,
                                                 TBool& aPasswordSet ) const
    {
    SCPLOGSTRING2( "CScpServiceManager::GetServiceNetworkInfoL service:%i", 
                    aServiceId );
    __ASSERT_DEBUG( (TInt)aServiceId > KErrNotFound, User::Panic( KNullDesC, KErrGeneral ) );
    
    TBool serviceCreated = EFalse;

    CScpService* service = iServiceStorage->GetServiceByServiceId( aServiceId );

    if( !service )
        {
        service = CreateTemporaryServiceL( aServiceId, ECCHUnknown );

        serviceCreated = ETrue;
        CleanupStack::PushL( service );
        }

    if( aSubServiceType == ECCHUnknown &&
        service->SubServicesContainSameSipProfile() == EFalse )
        {
        // We can't return any valid snap/iap values if different
        // sip profiles are in use
        User::Leave( KErrNotSupported );
        }        

    TBool sipConnectionCreated( EFalse );
    CScpSipConnection* sipConnection = GetSipConnectionL( aServiceId,
                                                          aSubServiceType, 
                                                          sipConnectionCreated );
    
    TInt result = sipConnection->GetIap( aIapId );
    if( result != KErrNone )
        {
        aIapId = 0;
        }

    result = sipConnection->GetSnap( aSnapId );
    if( result != KErrNone )
        {
        aSnapId = 0;
        }
		
    aPasswordSet = sipConnection->IsPasswordSet(); 
    
    if( sipConnectionCreated )
        {
        delete sipConnection;
        }

    // Decide if snap is locked
    aSnapLocked = ETrue;

    if( aSubServiceType == ECCHUnknown || 
        aSubServiceType == ECCHVoIPSub )
        {
        aSnapLocked = EFalse;
        }
    else
        {
        CScpSubService* voipSubService = service->GetSubServiceByType( ECCHVoIPSub );
        if( voipSubService )
            {
            TInt sipProfileId = voipSubService->SipProfileId();

            CScpSubService* subService = service->GetSubServiceByType( aSubServiceType );

            if( subService )
                {
                // If profiles are same the snap can't be changed
                if( subService->SipProfileId() != sipProfileId )
                    {
                    aSnapLocked = EFalse;
                    }
                }
            else
                {
                User::Leave( KErrNotFound );
                }
            }
        else
            {
            aSnapLocked = EFalse;
            }
        }
        
    if( serviceCreated )
        {
        CleanupStack::PopAndDestroy( service );
        }
    
    SCPLOGSTRING2( "CScpServiceManager::ServiceNetworkInfo snap:%i", aSnapId );    
    SCPLOGSTRING2( "CScpServiceManager::ServiceNetworkInfo iap:%i", aIapId );
    }

// -----------------------------------------------------------------------------
// CScpServiceManager::ReserveService
// -----------------------------------------------------------------------------
//
TInt CScpServiceManager::SetServiceReserved( TBool aReserved,
                                             TUint aServiceId,
                                             TCCHSubserviceType aSubServiceType )
    {
    SCPLOGSTRING4( "CScpServiceManager::SetServiceReserved: %d service: %d type: %d",
                  aReserved, aServiceId, aSubServiceType );

    TInt result = KErrNone;

    CScpService* service = iServiceStorage->GetServiceByServiceId( aServiceId );

    if( service )
        {
        result = service->SetReserved( aReserved, aSubServiceType );
        }
    else
        {
        result = KErrNotFound;
        }

    return result;
    }

// -----------------------------------------------------------------------------
// CScpServiceManager::IsReserved
// -----------------------------------------------------------------------------
//
TInt CScpServiceManager::IsReserved( TUint aServiceId,
                                     TCCHSubserviceType aSubServiceType ) const
    {
    SCPLOGSTRING3( "CScpServiceManager::IsReserved service: %d type: %d",
                   aServiceId, aSubServiceType );

    TBool result = EFalse;

    CScpService* service = iServiceStorage->GetServiceByServiceId( aServiceId );

    if( service )
        {
        result = service->IsReserved( aSubServiceType );
        }

    return result;
    }

// -----------------------------------------------------------------------------
// CScpServiceManager::UpdateProfileValuesL
// -----------------------------------------------------------------------------
//
void CScpServiceManager::UpdateProfileValuesL( 
    TUint aServiceId,
    TCCHSubserviceType aSubServiceType ) const
    {
    TUint32 sipId( KErrNone );
    iSettingHandler->GetSipProfileIdByTypeL( aServiceId, aSubServiceType, sipId );
    iProfileHandler->UpdateSipProfileL( sipId, 
        iSettingHandler->IsUahTerminalTypeDefinedL( aServiceId ), 
        iSettingHandler->IsUahWLANMacDefinedL( aServiceId ), 
        iSettingHandler->UahStringLengthL( aServiceId ) );
    }

// -----------------------------------------------------------------------------
// CScpServiceManager::GetConnectionParameter
// -----------------------------------------------------------------------------
//
void CScpServiceManager::GetConnectionParameterL( 
                             const TServiceSelection& /*aServiceSelection*/,
                             TCchConnectionParameter /*aParameter*/,
                             TInt& /*aValue*/ ) const
    {
    SCPLOGSTRING( "CScpServiceManager::GetConnectionParameter (int return)\
    Not supported yet. GetServiceNetworkInfoL is used at the moment." );
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CScpServiceManager::GetConnectionParameter
// -----------------------------------------------------------------------------
//
void CScpServiceManager::GetConnectionParameterL( 
                             const TServiceSelection& aServiceSelection, 
                             TCchConnectionParameter aParameter,
                             RBuf& aValue ) const
    {
    SCPLOGSTRING3( "CScpServiceManager::GetConnectionParameter (TBuf return) service id: %d type: %d",
                   aServiceSelection.iServiceId, aServiceSelection.iType );

    // check supported get values here, to avoid unnecessary object creation
    if( aParameter!=ECchUsername )
        {
        SCPLOGSTRING2( "CScpServiceManager::GetConnectionParameter: not supported parameter:%d", aParameter );
        User::Leave(KErrArgument );
        }
    
    TBool sipConnectionCreated( EFalse );
    
    CScpSipConnection* sipConnection = GetSipConnectionL( 
                                            aServiceSelection.iServiceId,
                                            aServiceSelection.iType, 
                                            sipConnectionCreated );
    if( sipConnectionCreated )
        {
        CleanupStack::PushL( sipConnection );
        }
    
    
    if( aParameter==ECchUsername )
        {
        HBufC* buffer = NULL;
        TBuf16<KTempBufMaxLength> tempBuf;
        TBuf8<KUsernameMaxLength> username;
        
        if( sipConnection->GetUsername( username ) == KErrNone )
            {
            tempBuf.Copy( username );
            
            // Decode encoded username (spaces to %20).
            buffer = EscapeUtils::EscapeDecodeL( tempBuf );
            
            if ( buffer )
                {
                aValue.Copy( buffer->Des() );
                delete buffer;
                buffer = NULL;
                }
            else
                {
                aValue.Copy( KNullDesC );
                }
            }
        }
    
    if( sipConnectionCreated )
        {
        CleanupStack::PopAndDestroy( sipConnection );
        }
    
    SCPLOGSTRING2( "Username: %S", &aValue );
    }

// -----------------------------------------------------------------------------
// CScpServiceManager::SetConnectionParameter
// -----------------------------------------------------------------------------
//
void CScpServiceManager::SetConnectionParameterL( 
                             const TServiceSelection& /*aServiceSelection*/, 
                             TCchConnectionParameter /*aParameter*/,
                             TInt /*aValue*/ )
    {
    SCPLOGSTRING( "CScpServiceManager::SetConnectionParameter(TInt) \
    Not supported yet. Separate set methods used." );

    // this is not implemented yet, separate functions are used.
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CScpServiceManager::SetConnectionParameter
// -----------------------------------------------------------------------------
//
void CScpServiceManager::SetConnectionParameterL( 
                     const TServiceSelection& aServiceSelection, 
                     TCchConnectionParameter aParameter,
                     const TDesC& aValue )
    {
    SCPLOGSTRING3( "CScpServiceManager::SetConnectionParameter (TDesC) service id: %d type: %d",
                   aServiceSelection.iServiceId, aServiceSelection.iType );
    
    if( aParameter!=ECchUsername && aParameter!=ECchPassword )
        {
        User::Leave( KErrArgument );
        }

    HBufC8* buf8 = NULL;
    buf8 = EscapeUtils::ConvertFromUnicodeToUtf8L( aValue );
    CleanupStack::PushL( buf8 );
       
    TBool sipConnectionCreated( EFalse );
    
    CScpSipConnection* sipConnection  = 
        GetSipConnectionL( aServiceSelection.iServiceId,
                           aServiceSelection.iType, 
                           sipConnectionCreated );

    if( sipConnectionCreated )
        {
        CleanupStack::PushL( sipConnection );
        }

    // Get presence settings id.
    TInt presenceSettingsId( KErrNotFound );
    TRAP_IGNORE( iSettingHandler->GetSPSettingsIntPropertyL(
        aServiceSelection.iServiceId,
        ESubPropertyPresenceSettingsId,
        presenceSettingsId ) );
    
    if( aParameter == ECchUsername )
        {
        // Use EscapeUtils to convert spaces to %20.
        HBufC8* encodedUsername = EscapeUtils::EscapeEncodeL( 
            buf8->Des(), EscapeUtils::EEscapeNormal );
        
        if ( encodedUsername )
            {
            CleanupStack::PushL( encodedUsername );
            
            // Set username to SIP-profile
            iProfileHandler->SetUsernameAndPasswordL( 
                sipConnection->ProfileId(),
                encodedUsername->Des(), ETrue, KNullDesC8, EFalse );
            
            // Set username to XDM-setting if setting exists
            if ( KErrNotFound != presenceSettingsId )
                {
                SetUsernameAndPasswordToXdmL(
                    aServiceSelection.iServiceId,
                    *encodedUsername,
                    ETrue,
                    KNullDesC8,
                    EFalse );
                }           
            
            CleanupStack::PopAndDestroy( encodedUsername );
            
            SCPLOGSTRING2( "Username is set: %S", &aValue );
            }
        }
    else if( aParameter == ECchPassword )
        {
        // Set password to SIP-profile
        iProfileHandler->SetUsernameAndPasswordL( 
            sipConnection->ProfileId(),
            KNullDesC8, EFalse, buf8->Des(), ETrue );
        
        // Set password to XDM-setting if setting exists
        if ( KErrNotFound != presenceSettingsId )
            {
            SetUsernameAndPasswordToXdmL(
                aServiceSelection.iServiceId,
                KNullDesC8,
                EFalse,
                buf8->Des(),
                ETrue );
            }
        
        SCPLOGSTRING2( "Password is set: %S", &aValue  );
        }
    else
        {
        User::Leave( KErrArgument );
        }
    
    if( sipConnectionCreated )
        {
        CleanupStack::PopAndDestroy( sipConnection );
        }
    
    CleanupStack::PopAndDestroy( buf8 ); 
    }

// -----------------------------------------------------------------------------
// CScpServiceManager::SetUsernameAndPasswordToXdmL
// -----------------------------------------------------------------------------
//
void CScpServiceManager::SetUsernameAndPasswordToXdmL( 
    TUint aServiceId,
    const TDesC8& aUsername,
    TBool aSetUsername,
    const TDesC8& aPassword,
    TBool aSetPassword )
    {
    CScpService* service = GetServiceL( 
        aServiceId, ECCHPresenceSub );
    
    TBool deleteTemporaryService( EFalse );
    if ( !service )
        {
        TRAPD( err, service = CreateTemporaryServiceL( 
            aServiceId,
            ECCHPresenceSub ) );
                 
        if( err == KErrNone && service )
            {
            deleteTemporaryService = ETrue;
            }
        }
    
    if ( aSetUsername )
        {
        iSettingHandler->UpdateXdmUsernameL( 
            *service,
            ECCHPresenceSub,
            aUsername );
        }
    
    if ( aSetPassword )
        {
        iSettingHandler->UpdateXdmPasswordL(
            *service, 
            ECCHPresenceSub,
            aPassword );
        }
    
    if ( deleteTemporaryService )
        {
        delete service;
        service = NULL;
        }
    }

// End of file
