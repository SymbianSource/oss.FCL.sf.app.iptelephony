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


#ifndef DHCPWLANQUERYHANDLER_H
#define DHCPWLANQUERYHANDLER_H

#include <e32base.h>
#include <es_sock.h>
#include <in_sock.h>

#include "dhcppsyrequesterprogressobserver.h"
#include "dhcpconstants.h"

class CDhcpPsySocketListener;
class CDhcpDataRequestor;
class CDhcpConnectionEngine;
class CDhcpPsyDhcpServerIPResolver;

/**
*  Class for handling the DHCP Messages sending and receiving.
*
*  @lib dhcppositionprovider
*  @since S60 3.2
*/
class CDhcpWlanQueryHandler : public CActive
    {
    public:

        /**
         * Two-phased statical constructor which can leave
         *
         * @since S60 3.2
         */
        static CDhcpWlanQueryHandler* NewL(MDhcpPsyRequesterProgressObserver *aObserver);

        /**
         * C++ destructor
         *
         * @since S60 3.2
         */
        virtual ~CDhcpWlanQueryHandler();

    public:

        /**
         * Make DHCP location query to WLAN network
         * @param aIAPProfileId     Id to used IAP profile
         *
         */
        void MakeLocationQueryL (TUint32 aIAPProfileId );

        /**
        * Return location data for requestor.
         *
         * @returns   Address to location data object
         */
        HBufC8* LocationData ();

    protected:  // From CActive

        /**
        * Handles an active object's request completion event.
        */
        void RunL();

        /**
        * Cancellation of an outstanding request.
        */
        void DoCancel();

        /**
        * Handles a leave occurring in the RunL()
        */
        TInt RunError( TInt aErr );


    private:

        /**
         * C++ constructor
         *
         * @since S60 3.2
         */
        CDhcpWlanQueryHandler(MDhcpPsyRequesterProgressObserver *aObserver);

        /**
         * Symbian Os construtor which can leave
         *
         * @since S60 3.2
         */
        void ConstructL();

        /**
         * Release location based reserved resources
         *
         * @since S60 3.2
         */
        void Flush ();

    private:

        enum TDhcpWLanQueryStates
            {
            EDhpWlanIdle,
            EAttachToConnection,
            EDhcpIpAddressQuery,
            ESendDhcpLocatinQuery,
            EReceiveDhcpLocationAck,
            EWlanMakeQueryToConnectionIoctl,
            EStopExecution
            };

    private: // data

        // Address of psy progress observer object.
        MDhcpPsyRequesterProgressObserver *iPsyProgressObserver;

        /**
        * Socket server
        */
        RSocketServ iSocketServer;

        /**
        * Connection
        */
        RConnection iConnection;

        /**
        * Sockets
        */
        RSocket iSocket;

        //
        TDhcpWLanQueryStates iState;

        //
        CDhcpDataRequestor* iLctnDataRequestor;

        //
        CDhcpPsySocketListener* iLctnDataListener;

        //
        CDhcpPsyDhcpServerIPResolver* iDhcpResolver;

        //
        CDhcpConnectionEngine*    iConnectionEngine;


        // Owned
        HBufC8*     iLocationBufferData;

        //
        TInetAddr   iInetLclIpAddress;

        // Used IAP
        TUint32 iIapProfileId;

    };

#endif // DHCPWLANQUERYHANDLER_H

