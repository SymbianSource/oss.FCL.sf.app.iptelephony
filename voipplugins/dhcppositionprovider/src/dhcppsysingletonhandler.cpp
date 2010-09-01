/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implement the singleton handler for dhcppositionprovider.
*
*/

#include "dhcppsylogging.h"
#include "dhcppsyrequester.h"
#include "dhcppsysingletonhandler.h"

// ---------------------------------------------------------------------------
// CDhcpPsySingletonHandler::GetInstanceL
// ---------------------------------------------------------------------------
//
CDhcpPsySingletonHandler* CDhcpPsySingletonHandler::GetInstanceL ()
    {
    TRACESTRING( "CDhcpPsySingletonHandler:: GetInstanceL" )
    CDhcpPsySingletonHandler* self =
        reinterpret_cast < CDhcpPsySingletonHandler*>( Dll::Tls() );

    if ( !self )
        {
        self = new( ELeave ) CDhcpPsySingletonHandler;
        CleanupStack::PushL( self );
        self->ConstructL ();
        CleanupStack::Pop( self );
        Dll::SetTls( self );
        }

    self->iRefCount++;
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CDhcpPsySingletonHandler::~CDhcpPsySingletonHandler ()
    {
    TRACESTRING( "CDhcpPsySingletonHandler:: ~CDhcpPsySingletonHandler" )
    delete iPsyRequester;
    iPsyRequester = NULL;
    }

// -----------------------------------------------------------------------------
// CDhcpPsySingletonHandler::ConstructL
// -----------------------------------------------------------------------------
//
void CDhcpPsySingletonHandler::ConstructL ()
    {
    TRACESTRING( "CDhcpPsySingletonHandler::ConstructL" )
    iPsyRequester = CDhcpPsyRequester::NewL();
    }

// -----------------------------------------------------------------------------
// CDhcpPsySingletonHandler::ReleaseInstance
// -----------------------------------------------------------------------------
//
void CDhcpPsySingletonHandler::ReleaseInstance()
    {
    TRACESTRING( "CDhcpPsySingletonHandler::ReleaseInstance" )
    iRefCount--;
    if ( iRefCount == 0 )
        {
        delete this;
        Dll::SetTls( NULL );
        }
    }

// -----------------------------------------------------------------------------
// CDhcpPsySingletonHandler::Requester
// -----------------------------------------------------------------------------
//
CDhcpPsyRequester* CDhcpPsySingletonHandler::Requester ()
    {
    TRACESTRING( "CDhcpPsySingletonHandler::Requester" )
    return iPsyRequester;
    }
