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
* Description:  RCCHClient implementation
*
*/


// INCLUDE FILES
#include "cchclient.h"
#include "cchlogger.h" 
#include "cchclientserver.h"
#include "cchclientserverinternal.h"

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES
// None

// CONSTANTS
// None

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ============================= LOCAL FUNCTIONS =============================

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// RCCHClient::RCCHClient
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
EXPORT_C RCCHClient::RCCHClient()
    {
    // No implementation required
    iService = NULL;
    }
    
EXPORT_C RCCHClient::~RCCHClient()
    {
    delete iService;
    }

// ---------------------------------------------------------------------------
// RCCHClient::Open
// Open subsession
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RCCHClient::Open( const RCCHServer& aRCCHServer )
    {
    CCHLOGSTRING( "RCCHClient::Open" );
    return RSubSessionBase::CreateSubSession( 
        aRCCHServer, ECCHOpenSubSession );
    }

// -----------------------------------------------------------------------------
// RCCHClient::Close
// Close subsession
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RCCHClient::Close()
    {
    CCHLOGSTRING( "RCCHClient::Close" );
    RSubSessionBase::CloseSubSession( ECCHCloseSubSession );
    }
    
// ---------------------------------------------------------------------------
// RCCHClient::RegisterToServer
// Send register message to server.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RCCHClient::RegisterToServer( TRequestStatus& aReqStatus ) const
    {
    const TIpcArgs args( TIpcArgs::ENothing );
    SendReceive( ECCHServerRegister, args, aReqStatus );
    }

// ---------------------------------------------------------------------------
// RCCHClient::RegisterToServerCancel
// Cancel register server request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RCCHClient::RegisterToServerCancel( ) const
    {
    const TIpcArgs args( TIpcArgs::ENothing );
    SendReceive( ECCHServerRegisterCancel, args );
    }


// ---------------------------------------------------------------------------
// RCCHClient::SubscribeToEvents
// Subscribe to specific service events.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RCCHClient::SubscribeToEvents( 
    const TServiceSelection& aServiceSelection,
    TPckgBuf<TServiceStatus>& aServiceStatus,
    TRequestStatus& aReqStatus ) const
    {
    const TIpcArgs args( 
        aServiceSelection.iServiceId, aServiceSelection.iType, 
            &aServiceStatus );
    SendReceive( ECCHSubscribeToEvents, args, aReqStatus );
    }
                                     
// ---------------------------------------------------------------------------
// RCCHClient::SubscribeToEvents
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RCCHClient::SubscribeToEventsCancel() const
    {
    const TIpcArgs args( TIpcArgs::ENothing );
    SendReceive( ECCHSubscribeToEventsCancel, args );
    }

// ---------------------------------------------------------------------------
// RCCHClient::GetServicesL
// Client queries for all services and sub-services.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RCCHClient::GetServicesL( 
    CArrayFixFlat<TCCHService>& aServices,
    TRequestStatus& aReqStatus )
    {
    TUint32 count( 0 );
    TPckgBuf<TUint32> countPckg( count );
    User::LeaveIfError( 
        SendReceive( ECCHServiceCount, TIpcArgs( 
            &countPckg, ECCHUnknown ) ) );
        
    for ( TInt i( 0 ); i < countPckg(); i++ )
        {
        TCCHService service;  
        aServices.AppendL( service );
        }
        
    if ( countPckg() )
        {
        TUint32 length = countPckg() * sizeof( TCCHService );
            
        if ( !iService )
            {
            iService = new (ELeave) TPtr8( reinterpret_cast<TText8*>( 
                &aServices[ 0 ] ), length, length );
            }
        else
            {
            iService->Set( reinterpret_cast<TText8*>( 
                &aServices[ 0 ] ), length, length );
            }
        
        const TIpcArgs args( KErrNotFound, ECCHUnknown, iService );
        SendReceive( ECCHGetServices, args, aReqStatus );
        }
    else
        {
        const TIpcArgs args( TIpcArgs::ENothing );
        SendReceive( ECCHGetServices, args, aReqStatus );
        }
    }

// ---------------------------------------------------------------------------
// RCCHClient::GetServicesL
// Get services which contains this type of sub-services
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RCCHClient::GetServicesL( 
    const TServiceSelection& aServiceSelection,
    CArrayFixFlat<TCCHService>& aServices,
    TRequestStatus& aReqStatus )
    {
    TUint32 count( 0 );
    TPckgBuf<TUint32> countPckg( count );
    
    // If some Service has selected Service count must be one,
    // otherwise we have to ask how many Services is enough
    if ( aServiceSelection.iServiceId !=  0 )
        {
        countPckg = 1;
        }
    else
        {
        User::LeaveIfError( 
            SendReceive( ECCHServiceCount, TIpcArgs( &countPckg, 
                aServiceSelection.iType ) ) );    
        }        
    
    for ( TInt i( 0 ); i < countPckg(); i++ )
        {
        TCCHService service;
        aServices.AppendL( service );
        }    
        
    if ( countPckg() )
        {
        TUint32 length = countPckg() * sizeof( TCCHService );
    
        if ( !iService )
            {
            iService = new (ELeave) TPtr8( reinterpret_cast<TText8*>( 
                &aServices[ 0 ] ), length, length );
            }
        else
            {
            iService->Set( reinterpret_cast<TText8*>( 
                &aServices[ 0 ] ), length, length );
            }
            
        const TIpcArgs args( aServiceSelection.iServiceId, 
            aServiceSelection.iType, 
            iService );
        SendReceive( ECCHGetServices, args, aReqStatus );
        }
    else
        {
        const TIpcArgs args( TIpcArgs::ENothing );
        SendReceive( ECCHGetServices, args, aReqStatus );
        } 
    }

// ---------------------------------------------------------------------------
// RCCHClient::GetServicesCancel
// Cancel get services request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RCCHClient::GetServicesCancel() const
    {
    const TIpcArgs args( TIpcArgs::ENothing );
    SendReceive( ECCHGetServicesCancel, args );
    }

// ---------------------------------------------------------------------------
// RCCHClient::GetServiceState
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RCCHClient::GetServiceState(
    const TServiceSelection& aServiceSelection,
    TCCHSubserviceState& aState ) const
    {
    TPckgBuf<TCCHSubserviceState> statePckg( aState );    
    const TIpcArgs args( 
        aServiceSelection.iServiceId, aServiceSelection.iType, &statePckg );    
    TInt error =  SendReceive( ECCHGetServiceState, args );
    aState = statePckg();
    return error;
    }

// ---------------------------------------------------------------------------
// RCCHClient::EnableService
// Enable services. All sub-services under the service are enabled.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RCCHClient::EnableService(
    const TServiceSelection& aServiceSelection,
    TRequestStatus& aReqStatus,
    TBool aConnectivityCheck ) const
    {
    const TIpcArgs args( 
        aServiceSelection.iServiceId, aServiceSelection.iType, 
            aConnectivityCheck );
    SendReceive( ECCHEnableService, args, aReqStatus );
    }

// ---------------------------------------------------------------------------
// RCCHClient::EnableServiceCancel
// Cancels enable services request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RCCHClient::EnableServiceCancel() const
    {
    const TIpcArgs args( TIpcArgs::ENothing );
    SendReceive( ECCHEnableServiceCancel, args );
    }
    
// ---------------------------------------------------------------------------
// RCCHClient::DisableService
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RCCHClient::DisableService(
    const TServiceSelection& aServiceSelection,
    TRequestStatus& aReqStatus ) const
    {
    const TIpcArgs args( 
        aServiceSelection.iServiceId, aServiceSelection.iType );
    SendReceive( ECCHDisableService, args, aReqStatus );
    }
    
// ---------------------------------------------------------------------------
// RCCHClient::DisableServiceCancel
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RCCHClient::DisableServiceCancel() const
    {
    const TIpcArgs args( TIpcArgs::ENothing );
    SendReceive( ECCHDisableServiceCancel, args );
    }    

// ---------------------------------------------------------------------------
// RCCHClient::GetServiceInfo
// Get service specifics information about service and protocol
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RCCHClient::GetServiceInfo(
    const TServiceSelection& aServiceSelection,
    TDes& aBuffer ) const
    {
    TPckgBuf<TServiceConnectionInfo> serviceConnInfoPckg;
    serviceConnInfoPckg().SetServiceId( aServiceSelection.iServiceId );
    serviceConnInfoPckg().SetType( aServiceSelection.iType );
    serviceConnInfoPckg().SetParameter( aServiceSelection.iParameter );
    
    // only service id, type and parameter values are used from 
    // TServiceConnectionInfo 
    const TIpcArgs args( &serviceConnInfoPckg, &aBuffer ); 
    return SendReceive( ECCHGetServiceInfo, args ); 
    }

// ---------------------------------------------------------------------------
// RCCHClient::SetConnectionInfo
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RCCHClient::SetConnectionInfo(
    const TPckgBuf<TServiceConnectionInfo>& aServiceConnInfo,
    TRequestStatus& aReqStatus )
    {
    const TIpcArgs args( &aServiceConnInfo );
    SendReceive( ECCHSetConnectionInfo, args, aReqStatus );
    }

// -----------------------------------------------------------------------------
// RCCHClient::SetConnectionInfoCancel
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RCCHClient::SetConnectionInfoCancel() const
    {
    const TIpcArgs args( TIpcArgs::ENothing );
    SendReceive( ECCHSetConnectionInfoCancel, args );
    }
                                         
// -----------------------------------------------------------------------------
// RCCHClient::GetConnectionInfo
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

EXPORT_C void RCCHClient::GetConnectionInfo(
    const TServiceSelection& aServiceSelection,
    TPckgBuf<TServiceConnectionInfo>& aServiceConnInfo,
    TRequestStatus& aReqStatus ) const
    {
    aServiceConnInfo().SetServiceId( aServiceSelection.iServiceId );
    aServiceConnInfo().SetType( aServiceSelection.iType );
    aServiceConnInfo().SetParameter( aServiceSelection.iParameter );
    // only service id, type and parameter values are used from 
    // TServiceConnectionInfo 
    const TIpcArgs args( &aServiceConnInfo ); 
    SendReceive( ECCHGetConnectionInfo, args, aReqStatus );
    }

// -----------------------------------------------------------------------------
// RCCHClient::GetConnectionInfoCancel
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

EXPORT_C void RCCHClient::GetConnectionInfoCancel() const
    {
    const TIpcArgs args( TIpcArgs::ENothing );
    SendReceive( ECCHGetConnectionInfoCancel, args );
    }

// ---------------------------------------------------------------------------
// RCCHClient::ReserveService
// Reserves the service for exclusive use
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RCCHClient::ReserveService(
    const TServiceSelection& aServiceSelection ) 
    {
    const TIpcArgs args( 
        aServiceSelection.iServiceId, aServiceSelection.iType );
    return SendReceive( ECCHReserveService, args ); 
    }

// ---------------------------------------------------------------------------
// RCCHClient::FreeService
// Frees the exclusive service reservation
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RCCHClient::FreeService(
    const TServiceSelection& aServiceSelection )
    {
    const TIpcArgs args( 
        aServiceSelection.iServiceId, aServiceSelection.iType );
    return SendReceive( ECCHFreeService, args ); 
    }

// ---------------------------------------------------------------------------
// RCCHClient::IsReserved
// Returns true if the service is reserved for exclusive use
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C TBool RCCHClient::IsReserved(
    const TServiceSelection& aServiceSelection ) const
    { 
    TPckgBuf<TBool> reserved;
    const TIpcArgs args( 
        aServiceSelection.iServiceId, aServiceSelection.iType,&reserved );
    SendReceive( ECCHIsReserved, args ); 
    return reserved();
    }
    
// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
