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
* Description:  Header file for connection progress notifier
*
*/


#ifndef DHCPCONNECTIONPROGRESSNOTIFIER_H_
#define DHCPCONNECTIONPROGRESSNOTIFIER_H_

#include <es_sock.h>
#include "dhcpconnectionstateobserver.h"

/**
*  This class keep track connection progress.
*
*  @lib dhcppositionprovider.dll
*  @since 3.2
*/
class CDhcpConnectionProgressNotifier : public CActive
    {
    public:

        /**
         * Two-phased constructor
         *
         * @param aConnection   Reference to RConnection object, which hold information about
         *                      ongoing connection to the wlan access point
         * @since 3.2
         */
        static CDhcpConnectionProgressNotifier* NewL (RConnection& aConnection,
                MDhcpConnectionStateObserver* aObserver);

        /*
        * Destructor
        *
        * @since S60 3.2
        */
        virtual ~CDhcpConnectionProgressNotifier();

    public:

        /*
         * Get current connection status
         *
         * @since S60 3.2
         */
        TInt    GetState();

    protected:

         /**
         * Cancellation of an outstanding request.
         *
         * @since S60 3.2
         */
         void DoCancel ();

         /**
         * Handles an active object's request completion event.
         *
         * @since S60 3.2
         */
         void RunL ();

    private:

        /**
         * C++ constructor
         *
         * @since S60 3.2
         */
        CDhcpConnectionProgressNotifier (RConnection& aConnection,
                MDhcpConnectionStateObserver* aObserver);

        /**
         * Contructor which can leave
         *
         * @since S60 3.2
         */
         void ConstructL ();

    private:
        // Member variables
        RConnection& iConnection;
        MDhcpConnectionStateObserver* iObserver;
        TNifProgressBuf iProgress;
        TInt iState;

    };

#endif  // DHCPCONNECTIONPROGRESSNOTIFIER_H_
