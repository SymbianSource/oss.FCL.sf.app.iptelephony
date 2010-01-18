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


#ifndef DHCPDATALOCATIONREQUEST_H
#define DHCPDATALOCATIONREQUEST_H

#include <e32base.h>
#include <in_sock.h>

#include "dhcpdatapacket.h"

/**
*  This class wraps the DHCP location request message.
*
*  @lib sspcontroller
*  @since S60 4.0
*/
class TDhcpLocationRequestPacket : public TDhcpBasePacket
    {
    public:

        /**
         * Default constructor.
         * @since  S60 3.2
         */
        TDhcpLocationRequestPacket();

        /**
         * Constructs DHCP location request message.
         * @since S60 3.2
         * @param aLocalIp local IP address
         * @param aMacAddr local hardware address
         * @return
         */
        void MakeDhcpInformMsg ( TUint32 aLocalIp, const TDesC8& aMacAddr  );

    public: // data

        /*
        * Return used transaction id in this dhcp message.
        * @since S60 3.2
        *
        * @return   Transaction id to follow.
        */
        TUint32 TransactionId ();

    private:

        /*
        *
        * Generate random transaction identifier for the DHCPInform-message.
        *
        * @since S60 3.2
        *
        * @return   Transaction id to follow.
        */
        void    RandomiseXid ();

    private:
        /**
         * Transaction identifier for Dhcp inform message.
        */
        TUint32 iXid;

    };

#endif // DHCPDATALOCATIONREQUEST_H
