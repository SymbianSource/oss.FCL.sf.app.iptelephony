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


#include "scpdisabled.h"
#include "scpsubservice.h"
#include "scpservicehandlerbase.h"
#include "scplogger.h"
#include "scputility.h"

// -----------------------------------------------------------------------------
// TScpDisabled::CTScpDisabled
// -----------------------------------------------------------------------------
//
TScpDisabled::TScpDisabled()
    {
    SCPLOGSTRING( "TScpDisabled::TScpDisabled" );
    }

// -----------------------------------------------------------------------------
// TScpDisabled::State
// -----------------------------------------------------------------------------
//
TCCHSubserviceState TScpDisabled::State() const
    {
    SCPLOGSTRING( "TScpDisabled::State" );

    return ECCHDisabled;
    }

// -----------------------------------------------------------------------------
// TScpDisabled::EnableL
// -----------------------------------------------------------------------------
//
void TScpDisabled::EnableL( CScpSubService& aSubService ) const
    {
    SCPLOGSTRING( "TScpDisabled::EnableL" );

    aSubService.SetSubServiceDisconnected( EFalse );
    aSubService.SetEnableRequestedState( CScpSubService::EScpEnabled );

    CScpServiceHandlerBase& handler = aSubService.ServiceHandler();
    TRAPD( error, handler.EnableSubServiceL() );

    if ( KErrNoMemory == error )
        {
        User::Leave( error );
        }
    
    if( error != KErrNone )
        {
        TInt cchError = TScpUtility::ConvertToCchError( error );
        if( cchError != KErrUnknown ) 
            {
            ChangeState( aSubService, ECCHConnecting, cchError );
            }
        else
            {
            // If another sub service is already in use, no need to
            // leave
            if( error != KErrInUse )
                {
                User::LeaveIfError( error );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// TScpDisabled::NetworkFound
// -----------------------------------------------------------------------------
//
void TScpDisabled::NetworkFound( CScpSubService& aSubService ) const
    {
    SCPLOGSTRING( "TScpDisabled::NetworkFound" );

    ChangeState( aSubService, ECCHConnecting, KErrNone );
    }

// -----------------------------------------------------------------------------
// TScpDisabled::NetworkNotFound
// -----------------------------------------------------------------------------
//
void TScpDisabled::NetworkNotFound( CScpSubService& aSubService ) const
    {
    SCPLOGSTRING( "TScpDisabled::NetworkNotFound" );

    // This is because some other app has enabled sip profile
    // and we move from disabled to connecting. But should the update
    // be done at all? See ServiceManager::UpdateService

    ChangeState( aSubService, ECCHConnecting, KCCHErrorNetworkLost );
    }

// -----------------------------------------------------------------------------
// TScpDisabled::NetworkLost
// -----------------------------------------------------------------------------
//
void TScpDisabled::NetworkLost( CScpSubService& aSubService ) const
    {
    SCPLOGSTRING( "TScpDisabled::NetworkLost" );

    ChangeState( aSubService, ECCHConnecting, KCCHErrorNetworkLost );
    }

// -----------------------------------------------------------------------------
// TScpDisabled::ServiceConnected
// -----------------------------------------------------------------------------
//
void TScpDisabled::ServiceConnected( CScpSubService& aSubService ) const
    {
    SCPLOGSTRING( "TScpDisabled::ServiceConnected" );

    ChangeState( aSubService, ECCHEnabled, KErrNone );
    }

//  End of File  
