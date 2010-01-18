/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Observer interface for notifying connection events.
*
*/


#ifndef M_CSCENGCONNECTIONOBSERVER_H
#define M_CSCENGCONNECTIONOBSERVER_H

#include "cscengconnectionhandler.h"

/**
 *  MCSCEngConnectionObserver class
 *
 *  Observer interface for notifying connection events
 *
 *  @lib
 *  @since S60 v3.2
 */
class MCSCEngConnectionObserver
    {
    public:
    
        /**
         * Should be called when connection is closed
         *
         * @since S60 v3.2
         * @param aConnectionEvent connection event
         */
        virtual void NotifyConnectionEvent( 
            CCSCEngConnectionHandler::TConnectionEvent aConnectionEvent ) = 0;
        
    };
    
#endif // M_CSCENGCONNECTIONOBSERVER_H
