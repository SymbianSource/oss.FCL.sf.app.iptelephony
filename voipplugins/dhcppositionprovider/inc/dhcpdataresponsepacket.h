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


#ifndef DHCPDATARESPONSEPACKET_H
#define DHCPDATARESPONSEPACKET_H

#include <e32base.h>
#include <in_sock.h>
#include "dhcpdatapacket.h"
#include "dhcpconstants.h"

/**
*  This class wraps the DHCP response message for location
*  query request
*
*  @lib
*  @since S60 3.2
*/
class TDhcpDataResponsePacket : public TDhcpBasePacket
    {
    public:

        /**
         * Default constructor.
         * @since  S60 3.2
         */
        TDhcpDataResponsePacket ();

        /**
         * Parses the DHCP ack message
         * @since  S60 3.2
         * @returns     KErrNone        If both or other option found.
         *              KErrNotFound    If magic cookie can't located or no support
         *                              options found from entered buffer.
         */
        TInt    ParseDHCPAck();

    public: // data

        /**
         * DHCP message identifier
         */
        TInt iXid;

        /**
         * Parsed geoConf data, length fixed to 16 bytes
         */
        TBuf8<KDhcpMaxResponseDhcpGeoConfLength> iGeoConf;

        /**
         * Parsed civic address data
         */
        TBuf8<KDhcpMaxDhcpCivicAddressLength> iCivicAddress;

    };

#endif // DHCPDATARESPONSEPACKET_H

