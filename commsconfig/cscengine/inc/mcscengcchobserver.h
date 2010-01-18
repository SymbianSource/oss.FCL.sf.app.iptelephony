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
* Description:  Observer interface for notifying service status changes.
*
*/


#ifndef M_CSCENGCCHOBSERVER_H
#define M_CSCENGCCHOBSERVER_H

#include <cch.h>

#include "cscengcchhandler.h"

/**
 *  MCSCEngCCHObserver class
 *
 *  Observer interface for notifying service status changes.
 *
 *  @lib cscengine.lib
 *  @since S60 v5.0
 */
class MCSCEngCCHObserver
    {
    public:
    
        /**
         * Should be called when service status changes.
         *
         * @since S60 v5.0
         * @param aServiceId Id of the service
         * @param aType Service type
         * @param aServiceStatus Service status
         */
        virtual void ServiceStatusChanged(
                TUint aServiceId, 
                TCCHSubserviceType aType, 
                const TCchServiceStatus& aServiceStatus ) = 0;            
    };
    
#endif // M_CSCENGCCHOBSERVER_H
