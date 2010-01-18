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


#include "scpenabled.h"
#include "scpservicehandlerbase.h"
#include "scpsubservice.h"
#include "scplogger.h"

// -----------------------------------------------------------------------------
// TScpEnabled::TScpEnabled
// -----------------------------------------------------------------------------
//
TScpEnabled::TScpEnabled()
    {
    SCPLOGSTRING( "TScpEnabled::TScpEnabled" );
    }

// -----------------------------------------------------------------------------
// TScpEnabled::State
// -----------------------------------------------------------------------------
//
TCCHSubserviceState TScpEnabled::State() const
    {
    SCPLOGSTRING( "TScpEnabled::State" );

    return ECCHEnabled;
    }

// -----------------------------------------------------------------------------
// TScpEnabled::Disable
// -----------------------------------------------------------------------------
//
TInt TScpEnabled::Disable( CScpSubService& aSubService ) const
    {
    SCPLOGSTRING( "TScpEnabled::Disable" );

    aSubService.SetEnableRequestedState( CScpSubService::EScpDisabled );

    CScpServiceHandlerBase& handler = aSubService.ServiceHandler();
    TInt result = handler.DisableSubService();

    ChangeState( aSubService, ECCHDisconnecting, KErrNone );

    return result;
    }

// -----------------------------------------------------------------------------
// TScpEnabled::NetworkLost
// -----------------------------------------------------------------------------
//
void TScpEnabled::NetworkLost( CScpSubService& aSubService ) const
    {
    SCPLOGSTRING( "TScpEnabled::NetworkLost" );

    ChangeState( aSubService, ECCHConnecting, KCCHErrorNetworkLost ); 
    }  

// -----------------------------------------------------------------------------
// TScpEnabled::ServiceConnected
// -----------------------------------------------------------------------------
//
void TScpEnabled::ServiceConnected( CScpSubService& aSubService ) const
    {
    SCPLOGSTRING( "TScpEnabled::ServiceConnected" );

    ChangeState( aSubService, ECCHEnabled, KErrNone );        
    }

// -----------------------------------------------------------------------------
// TScpEnabled::ServiceConnectionFailed
// -----------------------------------------------------------------------------
//
void TScpEnabled::ServiceConnectionFailed( CScpSubService& aSubService ) const
    {
    SCPLOGSTRING( "TScpEnabled::ServiceConnectionFailed" );

    ChangeState( aSubService, ECCHConnecting, KCCHErrorLoginFailed );        
    }

// -----------------------------------------------------------------------------
// TScpEnabled::ServiceDisconnected
// -----------------------------------------------------------------------------
//
void TScpEnabled::ServiceDisconnected( CScpSubService& aSubService ) const
    {
    SCPLOGSTRING( "TScpEnabled::ServiceDisconnected" );

    ChangeState( aSubService, ECCHConnecting, KCCHErrorServiceNotResponding );        
    }

// -----------------------------------------------------------------------------
// TScpEnabled::BandwidthLimited
// -----------------------------------------------------------------------------
//
void TScpEnabled::BandwidthLimited( CScpSubService& aSubService ) const 
    {
    SCPLOGSTRING( "TScpEnabled::BandwidthLimited" );
    
    ChangeState( aSubService, ECCHEnabled, KCCHErrorBandwidthInsufficient ); 
    }

// -----------------------------------------------------------------------------
// TScpEnabled::Roaming
// -----------------------------------------------------------------------------
//
void TScpEnabled::Roaming( CScpSubService& aSubService ) const 
    {
    SCPLOGSTRING( "TScpEnabled::Roaming" );
    
    ChangeState( aSubService, ECCHConnecting, KErrNone ); 
    }

//  End of File
