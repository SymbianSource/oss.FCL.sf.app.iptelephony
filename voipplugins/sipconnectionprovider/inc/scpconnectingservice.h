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


#ifndef T_SCPCONNECTINSERVICE_H
#define T_SCPCONNECTINSERVICE_H

#include <e32base.h>

#include "scpsubservicestate.h"

/**
*  Connecting Service state
*
*  @lib sipconnectionprovider.dll
*/
class TScpConnectingService : public TScpSubServiceState
    {
public:

    /**
     * C++ default constructor.
     */
    TScpConnectingService();

    /**
     * Returns state
     * @return State
     */
    TCCHSubserviceState State() const;

    /**
     * Enable
     * @param aSubService Sub service
     */
    void EnableL( CScpSubService& aSubService ) const;

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
     * Network not found
     * @param aSubService Sub service
     */
    void NetworkNotFound( CScpSubService& aSubService ) const;

    /**
     * Service connection failed
     * @param aSubService Sub service
     */
    void ServiceConnectionFailed( CScpSubService& aSubService ) const;

    /**
     * Service connected
     * @param aSubService Sub service
     */
    void ServiceConnected( CScpSubService& aSubService ) const;

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
     * Bandwidth limited
     * @param aSubService Sub service
     */
    void AuthenticationFailed( CScpSubService& aSubService ) const;

    /**
     * Service connection failed
     * @param aSubService Sub service
     */
    void ServiceInvalidSettings( CScpSubService& aSubService ) const;
    
    /**
     * Service connection canceled
     * @param aSubService Sub service
     */
    void ServiceConnectionCanceled( CScpSubService& aSubService ) const;
    
    /**
     * Bearer is not supported
     * @param aSubService Sub service
     */
    void BearerNotSupported( CScpSubService& aSubService ) const;
    };

#endif      // T_SCPCONNECTINSERVICE_H   
            
// End of File
