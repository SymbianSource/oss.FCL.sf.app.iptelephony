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
* Description:  Resolve ip address of the DHCP server at the WLAN network
*
*/


#include <e32std.h>
#include <es_sock.h>
#include "dhcppsylogging.h"
#include "dhcpconstants.h"
#include "dhcpserveripaddressresolver.h"

const TInt KDhcpPeriodicTimerInterval05Sec(500000);

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CDhcpPsyDhcpServerIPResolver* CDhcpPsyDhcpServerIPResolver::NewL( RConnection& aConnection,
        TRequestStatus& aStatus )
    {
    TRACESTRING ( "CDhcpPsyDhcpServerIPResolver::NewL" );
    CDhcpPsyDhcpServerIPResolver* self = new( ELeave ) CDhcpPsyDhcpServerIPResolver(
        aConnection, aStatus );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CDhcpPsyDhcpServerIPResolver::~CDhcpPsyDhcpServerIPResolver()
    {
    TRACESTRING( "CDhcpPsyDhcpServerIPResolver::~CDhcpPsyDhcpServerIPResolver" );
    Cancel();
    delete iPeriodicTimer;
    }

// ---------------------------------------------------------------------------
// C++ onstructor
// ---------------------------------------------------------------------------
//
CDhcpPsyDhcpServerIPResolver::CDhcpPsyDhcpServerIPResolver( RConnection& aConnection,
        TRequestStatus& aStatus) : CActive ( EPriorityStandard ), iConnection(aConnection),
        iClientStatus (aStatus)
    {
    TRACESTRING( "CDhcpPsyDhcpServerIPResolver::CDhcpPsyDhcpServerIPResolver" );
    CActiveScheduler::Add( this );
    iInetAddress.Init (KAfInet6);
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CDhcpPsyDhcpServerIPResolver::ConstructL()
    {
    TRACESTRING( "CDhcpPsyDhcpServerIPResolver::ConstructL" );
    iClientStatus = KRequestPending;
    iPckg().iAddr.SetFamily( KAfInet6 );    // Ipv 6, contains support both
                                            // ipv4 and ipv6 versions
                                            //
    iConnection.Ioctl( KCOLConfiguration, KConnGetServerAddr,
        iStatus, &iPckg );

    SetActive ();

    // Initialize the periodic timer.
    iPeriodicTimer = CPeriodic::NewL(CActive::EPriorityIdle);
    // Start the periodic timer, when ever the time elapsed
    // the PeriodicTimerCallBack() will get called.
    iPeriodicTimer->Start(KDhcpPeriodicTimerInterval05Sec,
            KDhcpPeriodicTimerInterval05Sec, TCallBack(PeriodicTimerCallBack,
            this));

    }

// ---------------------------------------------------------------------------
// RunL
// ---------------------------------------------------------------------------
//
void CDhcpPsyDhcpServerIPResolver::RunL()
    {
    TRACESTRING( "CDhcpPsyDhcpServerIPResolver::RunL");
    iDhcpResponseReceived = ETrue;
    TInt result = iStatus.Int();
    TRACESTRING2( "CDhcpPsyDhcpServerIPResolver::RunL, result %d",result);
    if (result==KErrNone)
        {
        iInetAddress = TInetAddr::Cast(iPckg().iAddr);
        }
    iPeriodicTimer->Cancel();
    CompleteRequest (result);
    }

// ---------------------------------------------------------------------------
// DhcpServerAddress
// ---------------------------------------------------------------------------
//
TInetAddr& CDhcpPsyDhcpServerIPResolver::DhcpServerInetAddress ()
    {
    TRACESTRING2 ( "CDhcpPsyDhcpServerIPResolver::DhcpServerInetAddress, %u ",
            iInetAddress.Address());
    return iInetAddress;
    }

// ---------------------------------------------------------------------------
// DoCancel
// ---------------------------------------------------------------------------
//
void CDhcpPsyDhcpServerIPResolver::DoCancel()
    {
    TRACESTRING( "CDhcpPsyDhcpServerIPResolver::DoCancel" );
    iConnection.CancelIoctl();
    if (iPeriodicTimer)
        {
        iPeriodicTimer->Cancel();
        }
    }

// ----------------------------------------------------------------------------
// CDhcpPsyDhcpServerIPResolver::CancelOutstandigRequest
// ----------------------------------------------------------------------------
//
void CDhcpPsyDhcpServerIPResolver::CancelOutstandigRequest ()
    {
    TRACESTRING ( "CDhcpPsyDhcpServerIPResolver::CancelOutstandigRequest ");
    iConnection.CancelIoctl();
    if (!iDhcpResponseReceived)
        {
        // If real dhcp server can't found, we have to use the broadcast address
        // Most probaply, device is behind of wlan subnet which has not own dhcp server.
        iInetAddress.SetAddress(KInetAddrBroadcast);
        }
    }

// ----------------------------------------------------------------------------
// CDhcpPsyDhcpServerIPResolver::CompleteRequest
// ----------------------------------------------------------------------------
//
void CDhcpPsyDhcpServerIPResolver::CompleteRequest (TInt aStatusCode)
    {
    TRACESTRING2 ( "CDhcpPsyDhcpServerIPResolver::CompleteRequest, status %d ",
            aStatusCode);
    TRequestStatus* status = &iClientStatus;
    User::RequestComplete(status, aStatusCode);
    }

// ----------------------------------------------------------------------------
// CDhcpPsyDhcpServerIPResolver::PeriodicTimerCallBack
// ----------------------------------------------------------------------------
//
TInt CDhcpPsyDhcpServerIPResolver::PeriodicTimerCallBack(TAny* aAny)
    {
    TRACESTRING ( "CDhcpPsyDhcpServerIPResolver::PeriodicTimerCallBack ");
    CDhcpPsyDhcpServerIPResolver* self = STATIC_CAST(CDhcpPsyDhcpServerIPResolver*, aAny);
    // Cancel outstanding requests
    self->CancelOutstandigRequest();
    return KErrNone;
    }

// End of file
