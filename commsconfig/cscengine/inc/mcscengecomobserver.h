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
* Description:  Observer interface for notifying ecom events.
*
*/


#ifndef M_CSCENGECOMOBSERVER_H
#define M_CSCENGECOMOBSERVER_H

/**
 *  MCSCEngEcomObserver class
 *
 *  Observer interface for notifying ecom events.
 *
 *  @lib cscengine.lib
 *  @since S60 v3.2
 */
class MCSCEngEcomObserver
    {
    public:
    
        /**
         * Observer interface for notifying ecom events.
         *
         * @since S60 v3.2
         */
        virtual void NotifyEcomEvent() = 0;
            
    };
    
#endif // M_CSCENGECOMOBSERVER_H
