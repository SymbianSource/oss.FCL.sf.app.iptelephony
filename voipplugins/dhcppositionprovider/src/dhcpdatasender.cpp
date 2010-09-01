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
* Description:  Send request data packets to the connected wlan network.
*
*/


#include <e32std.h>
#include <e32svr.h>

#include "cipapputilsaddressresolver.h"
#include "dhcpconstants.h"
#include "dhcpdatasender.h"
#include "dhcppsylogging.h"

_LIT8 (KDhcpWlanMacAddressFrmt, "" );
const TInt KDhcpWlanMacAddressLength = 0x20;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CDhcpDataRequestor* CDhcpDataRequestor::NewL(RSocket& aSocket)
    {
    CDhcpDataRequestor* self = new( ELeave ) CDhcpDataRequestor(aSocket);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CDhcpDataRequestor::~CDhcpDataRequestor()
    {
    TRACESTRING( "CDhcpDataRequestor::~CDhcpDataRequestor" );
    Cancel();
    delete iIpAppUtilsResolver;
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CDhcpDataRequestor::CDhcpDataRequestor(RSocket& aSocket):
    CActive ( EPriorityStandard ), iSocket ( aSocket )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// For sending a DHCP message. If a DHCP server address has been found then we
// use that, but if it has not been found then we try broadcasting.
// ---------------------------------------------------------------------------
//
void CDhcpDataRequestor::SendDHCPMessageL(TInetAddr aDhcpSrvAddr, TUint32 aIapProfileId,
        TRequestStatus& aStatus )
    {
    TRACESTRING2( "CDhcpDataRequestor::SendDHCPMessageL, dhcp server address: %u",
            aDhcpSrvAddr.Address() );

    // Make location request message.
    iIapProfileId = aIapProfileId;

    TBuf8<KDhcpWlanMacAddressLength> wlanMacAddress (KNullDesC8);

    User::LeaveIfError(
        iIpAppUtilsResolver->GetWlanMACAddress(wlanMacAddress,
            KDhcpWlanMacAddressFrmt)
        );

    User::LeaveIfError (
        iIpAppUtilsResolver->GetLocalIpAddressFromIap(iInetLclIpAddress, aIapProfileId)
        );

    TRACESTRING2( "CDhcpDataRequestor::SendDHCPMessage, local Ipaddres address: %u",
            iInetLclIpAddress.Address() );

    iLocationRequestMsg.MakeDhcpInformMsg (iInetLclIpAddress.Address(),
            wlanMacAddress );

    aStatus = KRequestPending;
    iClientStatus = &aStatus;
    TInetAddr inetAddr (aDhcpSrvAddr) ;
    inetAddr.SetPort( KDhcpDefaultSrvPort );
    iSocket.SendTo( iLocationRequestMsg, inetAddr, 0, iStatus, iLen );
    SetActive();
    }

// ---------------------------------------------------------------------------
// TransActionIdToFollow
// ---------------------------------------------------------------------------
//
TUint32 CDhcpDataRequestor::TransActionIdToFollow ()
    {
    return iLocationRequestMsg.TransactionId();
    }

// ---------------------------------------------------------------------------
// ContructL
// ---------------------------------------------------------------------------
//
void CDhcpDataRequestor::ConstructL ()
    {
    iIpAppUtilsResolver = CIPAppUtilsAddressResolver::NewL ();
    }

// ---------------------------------------------------------------------------
// RunL
// ---------------------------------------------------------------------------
//
void CDhcpDataRequestor::RunL ()
    {
    TRACESTRING2( "CDhcpDataRequestor::RunL %d", iStatus.Int());
    TRequestStatus *status = iClientStatus;
    User::RequestComplete (status, iStatus.Int());
    }

// ---------------------------------------------------------------------------
// CDhcpDataRequestor::DoCancel
// ---------------------------------------------------------------------------
//
void CDhcpDataRequestor::DoCancel ()
    {
    TRACESTRING( "CDhcpDataRequestor::DoCancel" );
    iSocket.CancelSend();
    }
