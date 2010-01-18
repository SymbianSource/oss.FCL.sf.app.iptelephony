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
* Description:  Class description file for CDhcpConnectionEngine
*
*/


#ifndef DHCPCONNECTIONENGINE_H
#define DHCPCONNECTIONENGINE_H

#include <e32base.h>
#include <es_sock.h>
#include <commdbconnpref.h> // For starting AP
#include "dhcpconnectionstateobserver.h"

class CDhcpConnectionProgressNotifier;

/**
 *  Locate and activate proper connection method wlan
 * internet usage
 *
 *  @lib
 *  @since S60 3.2
 */
class CDhcpConnectionEngine : public CActive,
    public MDhcpConnectionStateObserver
    {
    public:

        /**
         * Two-phased constructor.
         * @since S60 3.2
         *
         */
        static CDhcpConnectionEngine* NewL (RConnection& aConnection);

        /**
         * C++ destructor
         * @since S60 3.2
        */
        virtual ~CDhcpConnectionEngine ();

    public:

        /**
         * Check is we are already connected to the specified IAP or not.
         *
         * @since S60 3.2
         *
         * @param aIAPconnection An id to the connection which
         *                       will be activated.
         *
         */

        TBool IsConnectedL(TUint32 aIAPConnection);

        /**
        * Connects to the specified WLAN IAP.
        * @since S60 3.2
        * @param aIAPconnection An id to the connection which
        *                       will be activated.
        * @param aStatus        TRequestStatus to be signaled.
        *
        */
        void ConnectL (TUint aIAPConnection, TRequestStatus& aStatus);

    protected:

        /**
         * Cancellation of an outstanding request.
         */
        void DoCancel();

        /**
        * Handles an active object's request completion event.
        */
        void RunL();

    protected:

        /**
        * Handles a progress notifier state changing.
        */
        void StateChangedL ();

    private:    // methods

        /**
         * Finalise Symbian Os style construction.
         *
         * @since S60 3.2
         */
        void    ConstructL();

        /**
         * C++ constructor
         * @since S60 3.2
         *
         */
        CDhcpConnectionEngine (RConnection& aConnection);

        /**
         * SelfCall
         *
         * @since S60 3.2
         */
        void SelfCall ();

    private:    // Declarations.

        enum TConnectionStates
            {
            EIdle,
            ETurnConnectionOn,
            EConnectionStartup,
            };

    private:
        // Connection which we are used to.
        RConnection& iConnection;

        MDhcpConnectionStateObserver* iObserver;

        CDhcpConnectionProgressNotifier* iProgressNotifier;

        // Must be member varible since other wise confirmation
        // shown on the screen.
        TCommDbConnPref iPreferences;

        // State of this active object
        TConnectionStates  iStates;

        TRequestStatus* iClientStatus;

        // Used connection id
        TUint32 iIapProfileId;
    };

#endif // DHCPCONNECTIONENGINE_H
