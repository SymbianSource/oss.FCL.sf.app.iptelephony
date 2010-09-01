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


#ifndef DHCPDATAPACKET_H
#define DHCPDATAPACKET_H

#include <e32base.h>
#include <in_sock.h>

const TInt KDhcpPacketLength = 0x240; // RFC2131

/**
*  This class wraps send and received DHCP message.
*
*  @lib dhcppositionprovider
*  @since S60 3.2
*/
class TDhcpBasePacket : public TPtr8
    {
    public:
        /**
         * Default constructor.
         * @since  S60 3.2
         */
        TDhcpBasePacket ();

    protected: // data

        /**
         * Buffer holding the DHCP message
         */
        TUint8 iBuffer[KDhcpPacketLength];

    #ifdef  _DEBUG
    public:

        /**
         * Method to print trace log about data.
         */
        void PrintTraceLog ();

    #endif
    };

#endif // DHCPDATAPACKET_H
