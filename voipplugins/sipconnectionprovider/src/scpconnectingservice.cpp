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


#include "scpconnectingservice.h"
#include "scpservicehandlerbase.h"
#include "scpsubservice.h"
#include "scplogger.h"

// -----------------------------------------------------------------------------
// TScpConnectingService::TScpConnectingService
// -----------------------------------------------------------------------------
//
TScpConnectingService::TScpConnectingService()
    {
    SCPLOGSTRING( "TScpConnectingService::TScpConnectingService" );
    }

// -----------------------------------------------------------------------------
// TScpConnectingService::State
// -----------------------------------------------------------------------------
//
TCCHSubserviceState TScpConnectingService::State() const
    {
    SCPLOGSTRING( "TScpConnectingService::State" );

    return ECCHConnecting;
    }

// -----------------------------------------------------------------------------
// TScpConnectingService::EnableL
// -----------------------------------------------------------------------------
//
void TScpConnectingService::EnableL( CScpSubService& aSubService ) const
    {
    SCPLOGSTRING( "TScpConnectingService::EnableL" );

    // Launch refresh only if the sub service is in error condition
    // no need to launch refresh if connected iap is not supported -> 
    // just wait until valid iap is available
    if( aSubService.LastReportedError() != KErrNone && 
        aSubService.LastReportedError() != KErrNotSupported )
        {
        aSubService.SetEnableRequestedState( CScpSubService::EScpRefreshed );
        User::LeaveIfError( aSubService.Disable() );
        }
    }

// -----------------------------------------------------------------------------
// TScpConnectingService::Disable
// -----------------------------------------------------------------------------
//
TInt TScpConnectingService::Disable( CScpSubService& aSubService ) const
    {
    SCPLOGSTRING( "TScpConnectingService::Disable" );

    if( aSubService.EnableRequestedState() != CScpSubService::EScpRefreshed )
        { 
        aSubService.SetEnableRequestedState( CScpSubService::EScpDisabled );
        }
        
    CScpServiceHandlerBase& handler = aSubService.ServiceHandler();
    TInt result = handler.DisableSubService();

    ChangeState( aSubService, ECCHDisconnecting, KErrNone );

    return result;
    }

// -----------------------------------------------------------------------------
// TScpConnectingService::NetworkNotFound
// -----------------------------------------------------------------------------
//
void TScpConnectingService::NetworkNotFound( CScpSubService& aSubService ) const
    {
    SCPLOGSTRING( "TScpConnectingService::NetworkNotFound" );
    
    ChangeState( aSubService, ECCHConnecting, KCCHErrorNetworkLost );
    }

// -----------------------------------------------------------------------------
// TScpConnectingService::NetworkLost
// -----------------------------------------------------------------------------
//
void TScpConnectingService::NetworkLost( CScpSubService& aSubService ) const
    {
    SCPLOGSTRING( "TScpConnectingService::NetworkLost" );
    
    ChangeState( aSubService, ECCHConnecting, KCCHErrorNetworkLost );
    }

// -----------------------------------------------------------------------------
// TScpConnectingService::ServiceConnectionFailed
// -----------------------------------------------------------------------------
//
void TScpConnectingService::ServiceConnectionFailed( CScpSubService& aSubService ) const
    {
    SCPLOGSTRING( "TScpConnectingService::ServiceConnectionFailed" );

    ChangeState( aSubService, ECCHConnecting, KCCHErrorLoginFailed );
    }

// -----------------------------------------------------------------------------
// TScpConnectingService::ServiceInvalidSettings
// -----------------------------------------------------------------------------
//
void TScpConnectingService::ServiceInvalidSettings( CScpSubService& aSubService ) const
    {
    SCPLOGSTRING( "TScpConnectingService::ServiceInvalidSettings" );

    ChangeState( aSubService, ECCHConnecting, KCCHErrorInvalidSettings );
    }

// -----------------------------------------------------------------------------
// TScpConnectingService::ServiceConnected
// -----------------------------------------------------------------------------
//
void TScpConnectingService::ServiceConnected( CScpSubService& aSubService ) const
    {
    SCPLOGSTRING( "TScpConnectingService::ServiceConnected" );

    ChangeState( aSubService, ECCHEnabled, KErrNone );
    }

// -----------------------------------------------------------------------------
// TScpConnectingService::ServiceDisconnected
// -----------------------------------------------------------------------------
//
void TScpConnectingService::ServiceDisconnected( CScpSubService& aSubService ) const
    {
    SCPLOGSTRING( "TScpConnectingService::ServiceDisconnected" );

    ChangeState( aSubService, ECCHConnecting, KCCHErrorServiceNotResponding );
    }

// -----------------------------------------------------------------------------
// TScpConnectingService::BandwidthLimited
// -----------------------------------------------------------------------------
//
void TScpConnectingService::BandwidthLimited( CScpSubService& aSubService ) const
    {
    SCPLOGSTRING( "TScpConnectingService::BandwidthLimited" );
    
    ChangeState( aSubService, ECCHEnabled, KCCHErrorBandwidthInsufficient ); 
    }

// -----------------------------------------------------------------------------
// TScpConnectingService::AuthenticationFailed
// -----------------------------------------------------------------------------
//
void TScpConnectingService::AuthenticationFailed( CScpSubService& aSubService ) const
    {
    SCPLOGSTRING( "TScpConnectingService::AuthenticationFailed" );
    
    ChangeState( aSubService, ECCHConnecting, KCCHErrorAuthenticationFailed ); 
    }

// -----------------------------------------------------------------------------
// TScpConnectingService::ServiceConnectionCanceled
// -----------------------------------------------------------------------------
//
void TScpConnectingService::ServiceConnectionCanceled( CScpSubService& aSubService ) const
    {
    SCPLOGSTRING( "TScpConnectingService::ServiceConnectionCanceled" );
    
    ChangeState( aSubService, ECCHDisabled, KErrCancel ); 
    }

// -----------------------------------------------------------------------------
// TScpConnectingService::BearerNotSupported
// -----------------------------------------------------------------------------
//
void TScpConnectingService::BearerNotSupported( CScpSubService& aSubService ) const
    {
    SCPLOGSTRING( "TScpConnectingService::BearerNotSupported" );
    
    ChangeState( aSubService, ECCHConnecting, KErrNotSupported ); 
    }

//  End of File
