/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/


#ifndef T_SCPSTATECONTAINER_H
#define T_SCPSTATECONTAINER_H

#include <cchclientserver.h>

#include "scpdisabled.h"
#include "scpconnectingservice.h"
#include "scpenabled.h"
#include "scpdisconnecting.h"

class TScpSubServiceState;

/**
*  Service state container. Singleton.
*
*  @lib sipconnectionprovider.dll
*/
class TScpStateContainer
    {
public:

    /**
     * Constructor
     */
    TScpStateContainer();

    /**
     * Initializes the singleton. Should be called only once
     * during the lifetime of the dll.
     */
    void InitializeL();

    /**
     * Returns singleton instances of the service states
     * @param aState State type
     * @return State object
     */
    static TScpSubServiceState* Instance( TCCHSubserviceState aState );

private:

    /**
     * Disabled state
     */
    TScpDisabled iDisabled;

    /**
     * Connecting service state
     */
    TScpConnectingService iConnectingService;

    /**
     * Enabled state
     */
    TScpEnabled iEnabled;

    /**
     * Disconnecting state
     */
    TScpDisconnecting iDisconnecting;
    };

#endif      // T_SCPSTATECONTAINER_H   
            
// End of File
