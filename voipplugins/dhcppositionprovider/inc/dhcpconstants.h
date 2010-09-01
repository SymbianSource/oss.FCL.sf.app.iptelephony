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
* Description:  Hold common definations for dhcppositionprovider psy
*
*/


#ifndef DHCPCONSTANTS_H
#define DHCPCONSTANTS_H

#include <e32base.h>

const   TInt    KDhcpMaxAddrLength = 0x100;     // 256dec
const   TInt    KDhcpMaxDhcpCivicAddressLength = 0x100;     // 256dec

const   TInt    KDhcpDefaultSrvPort = 0x43;
const   TInt    KDhcpDefaultCliPort = 0x44;                 // 68dec
const   TInt    KDhcpMessageGeoSpatialCoordinates = 0x7B;   // 123dec
const   TInt    KDhcpMessageCivicAddress = 0x63;            // 99dec
const   TInt    KDhcpMaxResponseDhcpGeoConfLength = 0x10;   // 16dec

const   TUint8  KDhcpMagicCookie[]  = {0x63, 0x82, 0x53, 0x63} ;
const   TUint8  KDhcpDhcpMsgInform[] =  {0x35, 0x01, 0x08};
const   TUint8  KDhcpDhcpParameterRequestList[] = {0x37, 0x02};
const   TUint8  KDhcpGeoOptionParams[] =  {0x7B, 0x63};
const   TUint8  KDhcpMessageSize[] = {0x39, 0x02, 0x02, 0x40};
const   TUint8  KDhcpEndOfMessage[] =  {0xFF};

const   TInt    KDhcpMaxLocInfoLength = 0x800;      // 2048dec
const   TInt    KDhcpMaxOverheadLength = 0x4b0;     // 1200dec

const TInt KDhcpMacByteSeparatorChr = '-';

_LIT( KDhcpUdp, "udp");

_LIT8( KDhcpIpv4AddressFrmt, "%d.%d.%d.%d");

const TInt KDhcpIPAddressLength = 0x27; // Max chars what ipv6 can contain without
                                        // compacting it.
#endif  // DHCPCONSTANTS_H

