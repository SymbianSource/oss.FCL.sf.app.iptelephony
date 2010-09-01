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
* Description: 
*
*/



// INCLUDE FILES
#include <e32std.h>
#include <lbspositioninfo.h>      // TPositionInfoBase
#include "dhcppsypositioner.h"
#include "dhcppsylogging.h"
#include "dhcppsypanic.h"
#include "dhcppsysingletonhandler.h"
#include "dhcppsyrequester.h"
#include "dhcppsy.hrh"

// LOCAL CONSTANTS AND MACROS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDhcpPsyPositioner::CDhcpPsyPositioner
// C++default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDhcpPsyPositioner::CDhcpPsyPositioner()
    {
    // Nothing to do here
    }

// -----------------------------------------------------------------------------
// CDhcpPsyPositioner::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDhcpPsyPositioner::ConstructL( TAny* aConstructionParameters )
    {
    TRACESTRING( "CDhcpPsyPositioner::ConstructL start" )
    // Calling BaseConstructL is required by Positioning Plug-in API.
    BaseConstructL( aConstructionParameters );
    // Construct requester
    iDhcpRequestManager = CDhcpPsySingletonHandler::GetInstanceL();
    TRACESTRING( "CDhcpPsyPositioner::ConstructL end" )
    }

// -----------------------------------------------------------------------------
// CDhcpPsyPositioner::NewL
// Two - phased constructor.
// -----------------------------------------------------------------------------
//
CDhcpPsyPositioner* CDhcpPsyPositioner::NewL( TAny* aConstructionParameters )
    {
    CDhcpPsyPositioner* self = new( ELeave ) CDhcpPsyPositioner;
    CleanupStack::PushL( self );
    self->ConstructL( aConstructionParameters );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CDhcpPsyPositioner::~CDhcpPsyPositioner
//
// -----------------------------------------------------------------------------
//
CDhcpPsyPositioner::~CDhcpPsyPositioner()
    {
    TRACESTRING( "CDhcpPsyPositioner:: destructed, psy shutdown lauched")
    if (iDhcpRequestManager)
        {
        iDhcpRequestManager->ReleaseInstance();
        }
    TRACESTRING( "CDhcpPsyPositioner:: destructed" )
    }

// -----------------------------------------------------------------------------
// CDhcpPsyPositioner::StatusReportInterface
//
// -----------------------------------------------------------------------------
//
MPositionerStatus& CDhcpPsyPositioner::StatusReportInterface()
    {
    TRACESTRING( "CDhcpPsyPositioner::StatusReportInterface" )
    return *PositionerStatus();
    }

// -----------------------------------------------------------------------------
// CDhcpPsyPositioner::NotifyPositionUpdate
//
// -----------------------------------------------------------------------------
//
void CDhcpPsyPositioner::NotifyPositionUpdate (TPositionInfoBase& aPosInfo,
    TRequestStatus&    aStatus )
    {
    TRACESTRING( "CDhcpPsyPositioner::NotifyPositionUpdate" )
    iDhcpRequestManager->Requester()->NotifyPositionUpdate (aPosInfo,
            aStatus);
    TRACESTRING( "CDhcpPsyPositioner::NotifyPositionUpdate" )
    }

// -----------------------------------------------------------------------------
// CDhcpPsyPositioner::CancelNotifyPositionUpdate
//
// -----------------------------------------------------------------------------
//
void CDhcpPsyPositioner::CancelNotifyPositionUpdate ()
    {
    TRACESTRING( "CDhcpPsyPositioner::CancelNotifyPositionUpdate start" )
    //
    // This doens't actually cancel request, it will make sure that requester will not
    // complete actual request to client since it will not listen anymore
    iDhcpRequestManager->Requester()->CancelRequest();
    TRACESTRING( "CDhcpPsyPositioner::CancelNotifyPositionUpdate end" )
    }

//  End of File
