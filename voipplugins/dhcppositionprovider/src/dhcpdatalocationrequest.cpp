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
* Description:  Builds the dhcp location information request buffer
*
*/


#include <e32math.h>
#include "dhcppsylogging.h"             // For logging
#include "dhcpconstants.h"
#include "dhcpdatalocationrequest.h"

const TInt KDhcpFileBufferLength = 0x80;        // 128dec
const TInt KDhcpSnameLength = 0x40;

const TInt KDhcpChAddressLength = 0x10;   // 16dec

// Data positions constants.
const TInt KDhcpMessageOpCodePos = 0x00;
const TInt KDhcpHardwareAddressTypePos = 0x01;
const TInt KDhcpHardwareAddressLengthPos = 0x02;
const TInt KDhcpHopsRelayPos = 0x03;
const TInt KDhcpTransActionIdPos = 0x04;     // 4 byte

const TInt KDhcpSecsPos = 0x08;
const TInt KDhcpFlagsPos = 0x0A;
const TInt KDhcpCiAddrPos = 0x0C;
const TInt KDhcpYiAddrPos = 0x10;
const TInt KDhcpSiAddrPos = 0x14;
const TInt KDhcpGiAddrPos = 0x18;

// Operation code constans.
const TUint8    KDhcpMessageOperationCode = 0x01;       // 1 = BOOTREQUEST
const TUint8    KDhcpHardwareAddressType = 0x01;        // HType 1 = 10 mb ethernet
const TUint8    KDhcpHardwareAddressLength = 0x06;      // Hardware address length,
                                                        // 6 bytes 10 mb ethernet
const TUint8    KDhcpHopsRelayInform = 0x00;            // Hops 0 in DHCPINFORM


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// TDhcpLocationRequestPacket
// ---------------------------------------------------------------------------
//
TDhcpLocationRequestPacket::TDhcpLocationRequestPacket()
    {
    iXid = 0;
    }

// ---------------------------------------------------------------------------
// Return used transaction id.
// ---------------------------------------------------------------------------
//
TUint32 TDhcpLocationRequestPacket::TransactionId ()
    {
    return iXid;
    }

// ---------------------------------------------------------------------------
// Constructs a DHCP Inform message
// ---------------------------------------------------------------------------
//
void TDhcpLocationRequestPacket::MakeDhcpInformMsg ( TUint32 aLocalIp,
    const TDesC8& aMacAddr )
    {
    TRACESTRING( "TDhcpLocationRequestPacket::MakeDhcpInform" );
    RandomiseXid ();

    // Inital part, we do byte work at here.
    SetLength (0x1C);
    FillZ();

    iBuffer[KDhcpMessageOpCodePos] = KDhcpMessageOperationCode;
    iBuffer[KDhcpHardwareAddressTypePos] = KDhcpHardwareAddressType;
    // We could also get value in here from actual length of
    // mac address buffer.
    iBuffer[KDhcpHardwareAddressLengthPos] = KDhcpHardwareAddressLength;
    iBuffer[KDhcpHopsRelayPos] = KDhcpHopsRelayInform;

    BigEndian::Put32(&(iBuffer[KDhcpTransActionIdPos]), TUint32 (iXid));

    // Set secs and flags fields to the buffer.
    TUint16 secsAndFlagsValue = 0;      // 0 in flags = no broadcast
    BigEndian::Put16(&(iBuffer[KDhcpSecsPos]), TUint16 (secsAndFlagsValue));
    BigEndian::Put16(&(iBuffer[KDhcpFlagsPos]), TUint16 (secsAndFlagsValue));

    // Local ip address
    BigEndian::Put32(&(iBuffer[KDhcpCiAddrPos]), TUint32 (aLocalIp));
    TUint32 xiAddrValue = 0x00;
    BigEndian::Put32(&(iBuffer[KDhcpYiAddrPos]), TUint32 (xiAddrValue));
    BigEndian::Put32(&(iBuffer[KDhcpSiAddrPos]), TUint32 (xiAddrValue));
    BigEndian::Put32(&(iBuffer[KDhcpGiAddrPos]), TUint32 (xiAddrValue));

    // wlan mac address
    TBuf8<KDhcpChAddressLength>  chAddress (KNullDesC8);
    chAddress.Copy (aMacAddr.Left(KDhcpChAddressLength));
    if (chAddress.Length()<KDhcpChAddressLength)
        {
        // If given mac address is less max of field,
        // we have make sure that rest of bytes
        // are filled as zero-byte.
        TInt oldLength = chAddress.Length();
        chAddress.SetLength(KDhcpChAddressLength);
        for (TInt ii = oldLength; ii<KDhcpChAddressLength;ii++)
            {
            chAddress[ii] = 0x00;
            }
        }

    Append(chAddress);

    TBuf8<KDhcpFileBufferLength> fileBuffer(KNullDesC8);
    // sname
    fileBuffer.SetLength(KDhcpSnameLength);
    fileBuffer.FillZ();
    Append(fileBuffer);

    // file
    fileBuffer.SetLength(KDhcpFileBufferLength);
    fileBuffer.FillZ();
    Append(fileBuffer);

    // magic cookie
    TPtrC8 ptr (KDhcpMagicCookie, sizeof(KDhcpMagicCookie));
    Append(ptr);

    // And add all rest parts to the request message
    TPtrC8 ptrInform (KDhcpDhcpMsgInform, sizeof(KDhcpDhcpMsgInform));
    Append(ptrInform);

    TPtrC8 ptrRequestList (KDhcpDhcpParameterRequestList, sizeof(KDhcpDhcpParameterRequestList));
    Append(ptrRequestList);

    TPtrC8 ptrGeoOptionParams (KDhcpGeoOptionParams, sizeof(KDhcpGeoOptionParams));
    Append(ptrGeoOptionParams);

    TPtrC8 ptrMessageSize (KDhcpMessageSize, sizeof(KDhcpMessageSize));
    Append(ptrMessageSize);

    TPtrC8 ptrMsgSize (KDhcpEndOfMessage, sizeof(KDhcpEndOfMessage));
    Append(ptrMsgSize);

    TInt length = Length();
    SetLength(KDhcpPacketLength);

    // Ok, rest of bytes shall be zero.
    TPtr8 ptrMakeZeroBytes(&iBuffer[length],KDhcpPacketLength - length);
    ptrMakeZeroBytes.FillZ();
    }

// ---------------------------------------------------------------------------
// Generate a random transaction id
// ---------------------------------------------------------------------------
//
void TDhcpLocationRequestPacket::RandomiseXid ()
    {
    TRACESTRING( "TDhcpLocationRequestPacket::RandomiseXid" );
    TTime time;
    time.HomeTime();
    TInt64 seed = time.Int64();
    TUint random = Math::Rand( seed );
    iXid = random % KMaxTUint;
    TRACESTRING2( "TDhcpLocationRequestPacket::RandomiseXid, iXid %u" ,iXid );
    }
