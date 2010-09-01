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


#include "scpstatecontainer.h"
#include "scplogger.h"
#include "scpdisabled.h"
#include "scpconnectingservice.h"
#include "scpenabled.h"
#include "scpdisconnecting.h"

// -----------------------------------------------------------------------------
// TScpStateContainer::TScpStateContainer
// -----------------------------------------------------------------------------
//
TScpStateContainer::TScpStateContainer()
    {
    SCPLOGSTRING( "TScpStateContainer::TScpStateContainer" );
    }

// -----------------------------------------------------------------------------
// TScpStateContainer::InitializeL
// -----------------------------------------------------------------------------
//
void TScpStateContainer::InitializeL()
    {
    SCPLOGSTRING( "TScpStateContainer::ConstructL" );

    User::LeaveIfError( Dll::SetTls( static_cast<TAny*>( this ) ) );
    }

// -----------------------------------------------------------------------------
// TScpStateContainer::Instance
// -----------------------------------------------------------------------------
//
TScpSubServiceState* TScpStateContainer::Instance( TCCHSubserviceState aState )
    {
    SCPLOGSTRING2( "TScpStateContainer::Instance state: %d", aState );

    TScpStateContainer* container = static_cast<TScpStateContainer*>( Dll::Tls() );
    __ASSERT_DEBUG( container, User::Panic( KNullDesC, KErrGeneral ) );
    
    switch( aState )
        {
        case ECCHDisabled:
            return &container->iDisabled;

        case ECCHConnecting:
            return &container->iConnectingService;

        case ECCHEnabled:
            return &container->iEnabled;

        case ECCHDisconnecting:
            return &container->iDisconnecting;

        default:
            break;           
        }
    
    __ASSERT_DEBUG( EFalse, User::Panic( KNullDesC, KErrGeneral ) );
    return NULL;
    }

//  End of File
