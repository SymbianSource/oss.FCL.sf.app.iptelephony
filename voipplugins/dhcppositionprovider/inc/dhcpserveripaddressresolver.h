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


#ifndef DHCPSERVERIPADDRESSRESOLVER_H
#define DHCPSERVERIPADDRESSRESOLVER_H

#include <e32base.h>
#include <in_sock.h>
#include <comms-infras/netcfgextndhcpmsg.h> // TDhcp4RawOptionDataPckg

#include "dhcpconstants.h"

/**
*  Resolve ip address of DHCP server from WLAN network.
*
*  @lib dhcppositionprovider
*  @since S60 3.2
*/
class CDhcpPsyDhcpServerIPResolver : public CActive
    {
    public:

        /**
         * Two-phased statical constructor which can leave
         *
         * @since S60 3.2
         */
        static CDhcpPsyDhcpServerIPResolver* NewL( RConnection& aConnection, TRequestStatus& aStatus );

        /**
         * Default destructor.
         *
         * @since S60 3.2
         */
        virtual ~CDhcpPsyDhcpServerIPResolver();

    public:

        /**
         *
         * Returns ip address of the dhcp server at WLAN network.
         *
         * @since S60 3.2
         * @return const TDesC8&
         */
        TInetAddr&  DhcpServerInetAddress ();

    public:

        /**
        * Called by static periodic timer callback
        */
        void CancelOutstandigRequest ();

    private:

        /**
         * C++ constructor
         *
         * @since S60 3.2
         */
        CDhcpPsyDhcpServerIPResolver( RConnection& aConnection, TRequestStatus& aStatus );

        /**
         * Finalize construction and launches the adddress to the WLAN network
         */
         void ConstructL();

    protected:

        /**
        * Cancellation of an outstanding request.
        */
        void DoCancel();

        /**
        * Handles an active object's request completion event.
        */
        void RunL();

    private:        // Methods

        /*
        * The callback function.
        * param aAny A pointer to this class.
        */
        static TInt PeriodicTimerCallBack(TAny* aAny);

        /*
        * Complete client request.
        */
        void  CompleteRequest(TInt aStatusCode);

    private: // data

        //
        RConnection& iConnection;

        // To cancel outstanding request if needed.
        CPeriodic*  iPeriodicTimer;

        // If true, we have received response from network
        TBool   iDhcpResponseReceived;

        // Client TRequestStatus to be notify after request
        //  has been completed.
        TRequestStatus& iClientStatus;

        //
        TConnectionAddrBuf iPckg;

        // Container for ip addres of the dhcp server.
        TInetAddr   iInetAddress;

    };

#endif // DHCPSERVERIPADDRESSRESOLVER_H


