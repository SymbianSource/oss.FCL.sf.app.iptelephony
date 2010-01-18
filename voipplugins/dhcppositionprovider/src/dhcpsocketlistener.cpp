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
* Description:  Receives incoming DHCPInform messages
*
*/


#include <e32std.h>
#include <es_sock.h>
#include "dhcpconstants.h"
#include "dhcppsylogging.h"
#include "dhcpsocketlistener.h"

const TInt KDhcpSizeOfMinXIDPacket = 0x08;
const TInt KDhcpLocationOfXIDField = 0x04;
const TInt KDhcpLengthOfXIDField = 0x04;
const TInt KDhcpPeriodicTimerIntervalOneAndHalfSec(1500000);

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CDhcpPsySocketListener* CDhcpPsySocketListener::NewL(RSocket* aSocket)
    {
    TRACESTRING ( "CDhcpPsySocketListener::NewL" );
    CDhcpPsySocketListener* self = new( ELeave ) CDhcpPsySocketListener(aSocket);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CDhcpPsySocketListener::~CDhcpPsySocketListener()
    {
    TRACESTRING( "CDhcpPsySocketListener::~CDhcpPsySocketListener" );
    Cancel();
    delete iRcvdDataHeapBufferPtr;
    delete iPeriodicTimer;
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CDhcpPsySocketListener::CDhcpPsySocketListener( RSocket* aSocket) :
    CActive ( EPriorityStandard ), iSocket (aSocket),  iRcvdDataBuffer ( 0, 0, KDhcpPacketLength )
    {
    TRACESTRING( "CDhcpPsySocketListener::CDhcpPsySocketListener" );
    CActiveScheduler::Add( this );
    iAckPckg = TDhcpDataResponsePacket ();
    }

// ---------------------------------------------------------------------------
// CDhcpPsySocketListener::RcvDHCPMessage
//
// For receiving the message with a connectionless socket.
// This is done if no DHCP Server address was found with the Symbian way.
// Then we try it with Broadcast messages.
// ---------------------------------------------------------------------------
//
void CDhcpPsySocketListener::RcvDHCPMessageConnectionlessL(TRequestStatus& aStatus)
    {
    TRACESTRING( "CDhcpPsySocketListener::RcvDHCPMessageConnectionlessL" );
    aStatus = KRequestPending;
    iClientStatus = &aStatus;
    iIsConnectionless = ETrue;
    iRcvdDataHeapBufferPtr = HBufC8::NewL( KDhcpPacketLength );
    iRcvdDataBuffer.Set( iRcvdDataHeapBufferPtr->Des() );
    iRcvdDataBuffer.FillZ();
    iAddress.SetPort( KDhcpDefaultCliPort );
    iSocket->RecvFrom( iRcvdDataBuffer,
                       iAddress,
                       NULL,
                       iStatus,
                       iLen );
    StartTimer ();
    SetActive();
    }

// ---------------------------------------------------------------------------
// CDhcpPsySocketListener::RcvDHCPMessage
//
// For receiving DHCP message with a connected socket. If a DHCP server was
// found and a bind could be done.
// ---------------------------------------------------------------------------
//
void CDhcpPsySocketListener::RcvDHCPMessage(TRequestStatus& aStatus)
    {
    TRACESTRING( "CDhcpPsySocketListener::RcvDHCPMessage" );
    aStatus = KRequestPending;
    iClientStatus = &aStatus;
    iIsConnectionless = EFalse;
    iSocket->Recv( iAckPckg, 0 , iStatus, iLen );
    StartTimer ();
    SetActive();
    }

// ---------------------------------------------------------------------------
// CDhcpPsySocketListener::StartTimer
//
// Start timeout control for making sure that used client thread doesn't halt
// ---------------------------------------------------------------------------
//
void CDhcpPsySocketListener::StartTimer ()
    {
    TRACESTRING( "CDhcpPsySocketListener::StartTimer" );
    // Start the periodic timer, when ever the time elapsed
    // the PeriodicTimerCallBack() will get called.
    iPeriodicTimer->Start(KDhcpPeriodicTimerIntervalOneAndHalfSec,
            KDhcpPeriodicTimerIntervalOneAndHalfSec,
            TCallBack(PeriodicTimerCallBack, this));
    }

// ---------------------------------------------------------------------------
// RunL
// ---------------------------------------------------------------------------
//
void CDhcpPsySocketListener::RunL()
    {
    TRACESTRING2( "CDhcpPsySocketListener::RunL, %d", iStatus.Int() );
    TInt result = iStatus.Int();
    iPeriodicTimer->Cancel();
    if (result==KErrNone)
        {
        if ( iIsConnectionless )
            {
            iAckPckg.Copy( iRcvdDataHeapBufferPtr->Des() );
            }
        if (CheckAckXid ())
            {
            // Checks also that content is valid for location parsing.
            result = iAckPckg.ParseDHCPAck();
            }
        }
    TRequestStatus *status = iClientStatus;
    User::RequestComplete (status, result);
    }

// ---------------------------------------------------------------------------
// Return location civic information for caller
// ---------------------------------------------------------------------------
//
const TDesC8&   CDhcpPsySocketListener::LocationCivicInformation ()
    {
    TRACESTRING( "CDhcpPsySocketListener::LocationCivicInformation" );
    return iAckPckg.iCivicAddress;
    }

// ---------------------------------------------------------------------------
// Return geoConfig information for caller
// ---------------------------------------------------------------------------
//
const TDesC8&    CDhcpPsySocketListener::GeoConfigDhcpInformation ()
    {
    TRACESTRING( "CDhcpPsySocketListener::GeoConfigDhcpInformation" );
    return iAckPckg.iGeoConf;
    }

// ---------------------------------------------------------------------------
// Set transaction action id to follow
// ---------------------------------------------------------------------------
//
void CDhcpPsySocketListener::SetTransActionIdToFollow (TUint32 aXid)
    {
    TRACESTRING( "CDhcpPsySocketListener::SetTransActionIdToFollow" );
    iXid = aXid;
    }

// ---------------------------------------------------------------------------
// CDhcpPsySocketListener::CheckAckXid
//
// For checking if the XID (identifier of the DHCP message) of the received
// ACK - message matches the XID of the INFORM that was sent.
// ---------------------------------------------------------------------------
//
TBool CDhcpPsySocketListener::CheckAckXid()
    {
    TRACESTRING( "CDhcpPsySocketListener::CheckAckXID" );
    TBool retValue = EFalse;
    // Check if the message is the DHCP Ack that we wanted.
    // the length of the packet needs to be over 8 because otherwise
    // this leaves with error code.
    TRACESTRING2( "CDhcpPsySocketListener::CheckAckXID, ack length %d", iAckPckg.Length() );
    if ( iAckPckg.Length() > KDhcpSizeOfMinXIDPacket )
        {
        TUint32 ackXid = 0;
        // Copy the XID field from the message (4 octets from the 4:th octet)
        TPtrC8 xidPtr ( iAckPckg.Mid( KDhcpLocationOfXIDField, KDhcpLengthOfXIDField ) );
        ackXid = BigEndian::Get32(xidPtr.Ptr());
        // Compare the XID values
        if (ackXid == iXid)
            {
            TRACESTRING( "CDhcpPsySocketListener::CheckAckXID, true" );
            retValue = ETrue;
            }
        }
    return retValue;
    }

// ---------------------------------------------------------------------------
// CDhcpPsySocketListener::DoCancel
// ---------------------------------------------------------------------------
//
void CDhcpPsySocketListener::DoCancel()
    {
    TRACESTRING( "CDhcpPsySocketListener::DoCancel" );
    iPeriodicTimer->Cancel();
    iSocket->CancelRecv();
    }

// ---------------------------------------------------------------------------
// CDhcpPsySocketListener::ConstructL
// ---------------------------------------------------------------------------
//
void CDhcpPsySocketListener::ConstructL()
    {
    TRACESTRING ( "CDhcpPsySocketListener::ConstructL ");
    // Initialize the periodic timer.
    iPeriodicTimer = CPeriodic::NewL(CActive::EPriorityIdle);
    }

// ----------------------------------------------------------------------------
// CDhcpPsySocketListener::PeriodicTimerCallBack
// ----------------------------------------------------------------------------
//
TInt CDhcpPsySocketListener::PeriodicTimerCallBack(TAny* aAny)
    {
    TRACESTRING ( "CDhcpPsySocketListener::PeriodicTimerCallBack ");
    CDhcpPsySocketListener* self = STATIC_CAST(CDhcpPsySocketListener*, aAny);
    // Cancel outstanding requests
    self->CancelOutstandigRequest();
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CDhcpPsySocketListener::CancelOutstandigRequest
// ----------------------------------------------------------------------------
//
void CDhcpPsySocketListener::CancelOutstandigRequest ()
    {
    TRACESTRING ( "CDhcpPsySocketListener::CancelOutstandigRequest ");
    // Will cause outstanding request to complete prematurely
    iSocket->CancelRecv();
    }

