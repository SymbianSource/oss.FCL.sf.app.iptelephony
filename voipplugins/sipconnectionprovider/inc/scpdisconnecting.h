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


#ifndef T_SCPDISCONNECTING_H
#define T_SCPDISCONNECTING_H

#include <e32base.h>

#include "scpsubservicestate.h"

/**
*  Disconnecting state
*
*  @lib sipconnectionprovider.dll
*/
class TScpDisconnecting : public TScpSubServiceState
    {
public:
    /**
     * C++ default constructor.
     */
    TScpDisconnecting();

    /**
     * Returns state
     * @return State
     */
    TCCHSubserviceState State() const;

    /**
     * Disable
     * @param aSubService Sub service
     */
    TInt Disable( CScpSubService& aSubService ) const;

    /**
     * Service disconnected
     * @param aSubService Sub service
     */
    void ServiceDisconnected( CScpSubService& aSubService ) const;

    /**
     * Network lost
     * @param aSubService Sub service
     */
    void NetworkLost( CScpSubService& aSubService ) const;
    };

#endif      // T_SCPDISCONNECTING_H   
            
// End of File
