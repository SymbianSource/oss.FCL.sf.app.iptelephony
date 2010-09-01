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


#include "scpsubservicestate.h"
#include "scpsubservice.h"
#include "scplogger.h"

// -----------------------------------------------------------------------------
// TScpSubServiceState::TScpSubServiceState
// -----------------------------------------------------------------------------
//
TScpSubServiceState::TScpSubServiceState()
    {
    }

// -----------------------------------------------------------------------------
// TScpSubServiceState::ChangeState
// -----------------------------------------------------------------------------
//
void TScpSubServiceState::ChangeState( CScpSubService& aSubService, 
                                       TCCHSubserviceState aState,
                                       TInt aError ) const
    {
    SCPLOGSTRING( "TScpSubServiceState::ChangeState" );

    aSubService.ChangeState( aState, aError );
    }

// -----------------------------------------------------------------------------
// TScpSubServiceState::EnableL
// -----------------------------------------------------------------------------
//
void TScpSubServiceState::EnableL( CScpSubService& /*aSubService*/ ) const
    {
    SCPLOGSTRING( "TScpSubServiceState::EnableL" );
    }

// -----------------------------------------------------------------------------
// TScpSubServiceState::Disable
// -----------------------------------------------------------------------------
//
TInt TScpSubServiceState::Disable( CScpSubService& /*aSubService*/ ) const
    {
    SCPLOGSTRING( "TScpSubServiceState::Disable" );
 
    return KErrNotSupported; 
    }

// -----------------------------------------------------------------------------
// TScpSubServiceState::NetworkFound
// -----------------------------------------------------------------------------
//
void TScpSubServiceState::NetworkFound( CScpSubService& /*aSubService*/ ) const
    {
    SCPLOGSTRING( "TScpSubServiceState::NetworkFound" );
    }

// -----------------------------------------------------------------------------
// TScpSubServiceState::NetworkLost
// -----------------------------------------------------------------------------
//
void TScpSubServiceState::NetworkLost( CScpSubService& /*aSubService*/ ) const
    {
    SCPLOGSTRING( "TScpSubServiceState::NetworkLost" );
    }

// -----------------------------------------------------------------------------
// TScpSubServiceState::NetworkNotFound
// -----------------------------------------------------------------------------
//
void TScpSubServiceState::NetworkNotFound( CScpSubService& /*aSubService*/ ) const
    {
    SCPLOGSTRING( "TScpSubServiceState::NetworkNotFound" );
    }

// -----------------------------------------------------------------------------
// TScpSubServiceState::ServiceInvalidSettings
// -----------------------------------------------------------------------------
//
void TScpSubServiceState::ServiceInvalidSettings( CScpSubService& /*aSubService*/ ) const
    {
    SCPLOGSTRING( "TScpSubServiceState::ServiceInvalidSettings" );
    }

// -----------------------------------------------------------------------------
// TScpSubServiceState::ServiceConnectionFailed
// -----------------------------------------------------------------------------
//
void TScpSubServiceState::ServiceConnectionFailed( CScpSubService& /*aSubService*/ ) const
    {
    SCPLOGSTRING( "TScpSubServiceState::ServiceConnectionFailed" );
    }

// -----------------------------------------------------------------------------
// TScpSubServiceState::ServiceConnected
// -----------------------------------------------------------------------------
//
void TScpSubServiceState::ServiceConnected( CScpSubService& /*aSubService*/ ) const
    {
    SCPLOGSTRING( "TScpSubServiceState::ServiceConnected" );
    }

// -----------------------------------------------------------------------------
// TScpSubServiceState::ServiceDisconnected
// -----------------------------------------------------------------------------
//
void TScpSubServiceState::ServiceDisconnected( CScpSubService& /*aSubService*/ ) const
    {
    SCPLOGSTRING( "TScpSubServiceState::ServiceDisconnected" );
    }

// -----------------------------------------------------------------------------
// TScpSubServiceState::AuthenticationFailed
// -----------------------------------------------------------------------------
//
void TScpSubServiceState::AuthenticationFailed( CScpSubService& /*aSubService*/ ) const
    {
    SCPLOGSTRING( "TScpSubServiceState::AuthenticationFailed" );
    }

// -----------------------------------------------------------------------------
// TScpSubServiceState::Roaming
// -----------------------------------------------------------------------------
//
void TScpSubServiceState::Roaming( CScpSubService& /*aSubService*/ ) const 
    {
    SCPLOGSTRING( "TScpSubServiceState::Roaming" );
    }

// -----------------------------------------------------------------------------
// TScpSubServiceState::ServiceConnectionCanceled
// -----------------------------------------------------------------------------
//
void TScpSubServiceState::ServiceConnectionCanceled( CScpSubService& /*aSubService*/ ) const 
    {
    SCPLOGSTRING( "TScpSubServiceState::ServiceConnectionCanceled" );
    }

// -----------------------------------------------------------------------------
// TScpSubServiceState::RegistrationPending
// -----------------------------------------------------------------------------
//
void TScpSubServiceState::RegistrationPending( CScpSubService& /*aSubService*/ ) const 
    {
    SCPLOGSTRING( "TScpSubServiceState::RegistrationPending" );
    }

//  End of File
