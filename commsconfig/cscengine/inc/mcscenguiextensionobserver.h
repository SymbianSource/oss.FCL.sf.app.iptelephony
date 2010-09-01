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
* Description:  bserver interface for notifying ui extension plug-in events
*
*/



#ifndef M_CSCENGUIEXTENSIONOBSERVER_H
#define M_CSCENGUIEXTENSIONOBSERVER_H

#include "cscenguiextensionpluginhandler.h"

/**
 *  MCSCEngUiExtensionObserver class
 *
 *  Observer interface for notifying ui extension plug-in events
 *
 *  @lib
 *  @since S60 v3.2
 */
class MCSCEngUiExtensionObserver
    {
    public:
    
        /**
         * Should be called when exited from ui extension
         *
         * @since S60 v3.2
         */
        virtual void NotifyUiExtensionPluginResponse( 
            const CCSCEngUiExtensionPluginHandler::TUiExtensionPluginResponse& aResponse,
            const TInt aIndex, 
            const TUid& aPluginUid ) = 0;  
    };
            
#endif // M_CSCENGUIEXTENSIONOBSERVER_H
