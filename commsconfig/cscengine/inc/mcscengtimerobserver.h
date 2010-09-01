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
* Description:  Observer interface for notifying timer expiration
*
*/


#ifndef M_CSCENGTIMEROBSERVER_H
#define M_CSCENGTIMEROBSERVER_H

#include "cscengtimer.h"

/**
 *  MCSCEngTimerObserver class
 *
 *  Observer interface for notifying timer expiration.
 *
 *  @lib cscengine.lib
 *  @since S60 v3.2
 */
class MCSCEngTimerObserver
    {
    public:
    
        /**
         * Should be called when the timer expires
         *
         * @since S60 v3.2
         */
        virtual void TimerExpired() = 0;            
    };
    
#endif // M_CSCENGTIMEROBSERVER_H
