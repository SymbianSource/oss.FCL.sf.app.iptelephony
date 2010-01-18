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


#ifndef T_SCPENABLED_H
#define T_SCPENABLED_H

#include <e32base.h>

#include "scpsubservicestate.h"

/**
*  Enabled state
*
*  @lib sipconnectionprovider.dll
*/
class TScpEnabled : public TScpSubServiceState
    {
public:
    /**
     * C++ default constructor.
     */
    TScpEnabled();

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
     * Network lost
     * @param aSubService Sub service
     */
    void NetworkLost( CScpSubService& aSubService ) const;

    /**
     * Service connected
     * @param aSubService Sub service
     */
    void ServiceConnected( CScpSubService& aSubService ) const;

    /**
     * Service connection failed
     * @param aSubService Sub service
     */
    void ServiceConnectionFailed( CScpSubService& aSubService ) const;

    /**
     * Service disconnected
     * @param aSubService Sub service
     */
    void ServiceDisconnected( CScpSubService& aSubService ) const;

    /**
     * Bandwidth limited
     * @param aSubService Sub service
     */
    void BandwidthLimited( CScpSubService& aSubService ) const;
    
    /**
     * Roaming ongoing
     * @param aSubService Sub service
     */
    void Roaming( CScpSubService& aSubService ) const;
    };

#endif      // T_SCPENABLED_H   
            
// End of File
