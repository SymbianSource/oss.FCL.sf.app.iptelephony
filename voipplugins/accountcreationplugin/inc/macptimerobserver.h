/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header file for MAcpTimerObserver
*
*/


#ifndef M_ACPTIMEROBSERVER_H
#define M_ACPTIMEROBSERVER_H

#include "acptimer.h"

/**
 *  MAcpTimerObserver class
 *
 *  Observer interface for notifying timer expiration.
 *
 *  @lib accountcreationplugin.lib
 *  @since S60 v5.0
 */
class MAcpTimerObserver
    {
    public:
    
        /**
         * Should be called when the timer expires
         *
         * @since S60 v5.0
         */
        virtual void TimerExpired() = 0;            
    };
    
#endif // M_ACPTIMEROBSERVER_H
