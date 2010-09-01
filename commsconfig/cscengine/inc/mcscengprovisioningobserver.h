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
* Description:  Observer interface for notifying when service plugin 
*              : is configured
*
*/



#ifndef M_CSCENGPROVISIONINGOBSERVER_H
#define M_CSCENGPROVISIONINGOBSERVER_H


#include "cscengservicepluginhandler.h"

/**
 *  MCSCEngProvisioningObserver class
 *
 *  Observer interface for notifying when service plug-in is configured
 *
 *  @lib
 *  @since S60 v3.2
 */
class MCSCEngProvisioningObserver
    {
    public:
    
        /**
         * Should be called when provisioning of the plugin is completed.
         *
         * @since S60 v3.2
         * @param aResponse plug-in response type
         * @param aIndex index
         * @param aPluginUid plug-in uid
         */
        virtual void NotifyServicePluginResponse( 
            const CCSCEngServicePluginHandler::TServicePluginResponse& aResponse, 
            const TInt aIndex, 
            const TUid& aPluginUid ) = 0;         
    };
    
#endif // M_CSCENGPROVISIONINGOBSERVER_H    
