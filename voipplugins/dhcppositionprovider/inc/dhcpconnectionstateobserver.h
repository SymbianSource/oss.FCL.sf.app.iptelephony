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


#ifndef DHCPCONNECTIONSTATEOBSERVER1_H
#define DHCPCONNECTIONSTATEOBSERVER1_H

// INCLUDES
#include <e32base.h>

// CLASS DECLARATION

/**
* MDhcpConnectionStateObserver  State Observer interface
*/
class MDhcpConnectionStateObserver
    {
    public:

        /*
         * StateChangedL()
         *
         * Notify state change
         */
        virtual void StateChangedL() = 0;
    };

#endif // DHCPCONNECTIONSTATEOBSERVER_H

