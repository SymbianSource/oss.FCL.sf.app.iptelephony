/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef M_CCHUITIMEROBSERVER_H
#define M_CCHUITIMEROBSERVER_H

#include "cchuitimer.h"

/**
 *  MCchUiTimerObserver class
 *
 *  Observer interface for notifying timer expiration.
 *
 *  @lib cch.lib
 *  @since S60 v5.0
 */
class MCchUiTimerObserver
    {
    public:
    
        /**
         * Should be called when the timer expires
         *
         * @since S60 v5.0
         */
        virtual void TimerExpired() = 0;            
    };
    
#endif // M_CCHUITIMEROBSERVER_H
