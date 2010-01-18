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


#ifndef DHCPSOCKETLISTENER_H
#define DHCPSOCKETLISTENER_H

#include <e32base.h>
#include <in_sock.h>

#include "dhcpdataresponsepacket.h"

/**
*  Listen receiving DHCP socket messages
*
*  @lib dhcppositionprovider
*  @since S60 3.2
*/
class CDhcpPsySocketListener : public CActive
    {
    public:
        /**
         * Two-phased statical constructor which can leave
         *
         * @since S60 3.2
         */
        static CDhcpPsySocketListener* NewL( RSocket* aSocket );

        /**
         * Default destructor.
         *
         * @since S60 3.2
         */
        virtual ~CDhcpPsySocketListener();

    public:

        /**
         * Set this active object to waiting for UDP messages
         *
         * @since S60 3.0
         * @return None
         */
        void RcvDHCPMessage (TRequestStatus& aStatus);

        /**
         * Set this active object to waiting for UDP messages from
         * connectionless socket
         *
         * @since S60 3.0
         * @return None
         */
        void RcvDHCPMessageConnectionlessL (TRequestStatus& aStatus);

        /*
        * Set transaction to follow.
        *
        */
        void SetTransActionIdToFollow (TUint32 aXid);

    public:

        /*
        * Return location civic data for caller.
        *
        */
        const TDesC8&   LocationCivicInformation ();

        /*
        * Return location geographical information for caller
        *
        */
        const TDesC8&   GeoConfigDhcpInformation ();

    protected:

        /**
         * Cancellation of an outstanding request.
         */
         void DoCancel();

         /**
         * Handles an active object's request completion event.
         */
         void RunL();

    private:

        /*
         * 2nd phase Symbian style constructor which can leave.
        */
        void ConstructL();

        /**
         * C++ constructor
         *
         * @since S60 3.2
         */
        CDhcpPsySocketListener (RSocket* aSocket);

        /**
        * Check received message that was the message to request.
        * Checking is made by transaction id.
        */
        TBool CheckAckXid ();

    public:     // for timeout control

        /**
        * Called by static periodic timer callback
        */
        void CancelOutstandigRequest ();

    private:    // for timeout control

        /**
         * Activete timeout control for this object
         */
        void StartTimer ();

        /*
         * The callback function for CPeriodic.
         * param aAny A pointer to this object.
         */
         static TInt PeriodicTimerCallBack(TAny* aAny);

    private: // data

        /**
         * Received DHCP ACK package
         */
        TDhcpDataResponsePacket iAckPckg;

        /**
         * Socket
         * Not own.
         */
        RSocket* iSocket;

        /**
         * Filled in with amount of data sent before completion
         */
        TSockXfrLength iLen;

        /**
         * Address used if using connectionless socket
         */
        TInetAddr iAddress;

        /**
         * Flag indicating connectionlessness
         */
        TBool iIsConnectionless;

        // Received data buffer
        TPtr8 iRcvdDataBuffer;

        // Received data buffer.
        HBufC8* iRcvdDataHeapBufferPtr;

        // Transaction to be follow.
        TUint32 iXid;

        //
        TRequestStatus* iClientStatus;

        // To cancel outstanding request if needed.
        CPeriodic*  iPeriodicTimer;

    };

#endif // DHCPSOCKETLISTENER_H


