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


#ifndef DHCPDATASENDER_H
#define DHCPDATASENDER_H

#include <e32base.h>
#include <in_sock.h>
#include <es_sock.h>

#include "dhcpdatalocationrequest.h"

class CIPAppUtilsAddressResolver;

/**
 *  Sends a DHCP inform message to the network
 *
 *  @lib dhcppositionprovider.lib
 *  @since S60 3.2
 */
class CDhcpDataRequestor : public CActive
    {
    public:

        /**
         * Two-phased constructor.
         * @since S60 3.2
         * @param aSocket       Socket port where to send data
         */
        static CDhcpDataRequestor* NewL(RSocket& aSocket );

        /**
         * C++ destructor
         * @since S60 3.2
         */
        virtual ~CDhcpDataRequestor();

    public:

        /**
         * Make inform message for sendid purpose.
         * @param aSocket Socket port where to send data
         *
         */
        void MakeDhcpInformMsg (TInetAddr aLocalIpAddress);

        /**
         * Return  transaction to follow. Note:
         * Have to call after make dhcp inform message.
         * @since S60 3.2
         * @return      Transaction id to follow.
         */
        TUint32     TransActionIdToFollow ();

        /**
         * Set this active object to send the DHCP message
         * @since S60 3.2
         * @param aInetAddr
         * @param aStatus       Status to be notify after message
         *                      has been sent.
         * @param aIapAddress   Used
         * @return None
         */
        void SendDHCPMessageL ( TInetAddr aInetAddr, TUint32 aIpAddress,
                TRequestStatus& aStatus  );

    private:    // Methods

        /**
         * Finalize active object cancelation
         * @since S60 3.2
         */
        void DoCancel();

        /**
         * Finalize active object cancelation
         * @since S60 3.2
         */
        void RunL();

    private:

        /**
         * Symbian Os construtor which can leave
         *
         * @since S60 3.2
         */
        void  ConstructL();

        /**
         * C++ constructor
         * @since S60 3.2
         * @param aSocket Socket port where to send data
         *
         */
        CDhcpDataRequestor(RSocket& aSocket);

    private: // data
        CIPAppUtilsAddressResolver*     iIpAppUtilsResolver;

        // Used when make a query to the wlan subnet.
        RSocket& iSocket;

        /**
         * Filled in with amount of data sent before
         * completion.
         */
        TSockXfrLength          iLen;
        TRequestStatus* iClientStatus;

        //
        TDhcpLocationRequestPacket  iLocationRequestMsg;
        TInetAddr   iInetLclIpAddress;
        TUint32 iIapProfileId;
    };

#endif // DHCPDATASENDER_H
