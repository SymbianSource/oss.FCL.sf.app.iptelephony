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


#include "scpdisconnecting.h"
#include "scplogger.h"

// -----------------------------------------------------------------------------
// TScpDisconnecting::TScpDisconnecting
// -----------------------------------------------------------------------------
//
TScpDisconnecting::TScpDisconnecting()
    {
    SCPLOGSTRING( "TScpDisconnecting::TScpDisconnecting" );
    }

// -----------------------------------------------------------------------------
// TScpDisconnecting::State
// -----------------------------------------------------------------------------
//
TCCHSubserviceState TScpDisconnecting::State() const
    {
    SCPLOGSTRING( "TScpDisconnecting::State" );

    return ECCHDisconnecting;
    }

// -----------------------------------------------------------------------------
// TScpDisconnecting::Disable
// -----------------------------------------------------------------------------
//
TInt TScpDisconnecting::Disable( CScpSubService& /*aSubService*/ ) const
    {
    SCPLOGSTRING( "TScpDisconnecting::Disable" );

    // Already disconnecting
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// TScpDisconnecting::ServiceDisconnected
// -----------------------------------------------------------------------------
//
void TScpDisconnecting::ServiceDisconnected( CScpSubService& aSubService ) const
    {
    SCPLOGSTRING( "TScpDisconnecting::ServiceDisconnected" );

    ChangeState( aSubService, ECCHDisabled, KErrNone );
    }

// -----------------------------------------------------------------------------
// TScpDisconnecting::NetworkLost
// -----------------------------------------------------------------------------
//
void TScpDisconnecting::NetworkLost( CScpSubService& aSubService ) const
    {
    SCPLOGSTRING( "TScpDisconnecting::NetworkLost" );

    ChangeState( aSubService, ECCHDisabled, KErrNone );
    }


//  End of File
