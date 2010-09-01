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


#ifndef T_SCPSUBSERVICESTATE_H
#define T_SCPSUBSERVICESTATE_H

#include <e32base.h>
#include <cchclientserver.h>

class CScpSubService;

/**
*  TScpSubServiceState
*
*  @lib sipconnectionprovider.dll
*/
class TScpSubServiceState
    {
public:

    /**
     * Constructor
     */
    TScpSubServiceState();

    /**
     * Returns state
     * @return State
     */
    virtual TCCHSubserviceState State() const = 0;

    /**
     * Returns state
     * @return State
     */
    virtual void EnableL( CScpSubService& aSubService ) const;

    /**
     * Disable
     * @param aSubService Sub service
     */
    virtual TInt Disable( CScpSubService& aSubService ) const;

    /**
     * Network found
     * @param aSubService Sub service
     */
    virtual void NetworkFound( CScpSubService& aSubService ) const;

    /**
     * Network lost
     * @param aSubService Sub service
     */
    virtual void NetworkLost( CScpSubService& aSubService ) const;

    /**
     * Network not found
     * @param aSubService Sub service
     */
    virtual void NetworkNotFound( CScpSubService& aSubService ) const;

    /**
     * Service connection failed
     * @param aSubService Sub service
     */
    virtual void ServiceConnectionFailed( CScpSubService& aSubService ) const;

    /**
     * Service connection failed
     * @param aSubService Sub service
     */
    virtual void ServiceInvalidSettings( CScpSubService& aSubService ) const;
        
    /**
     * Service connected
     * @param aSubService Sub service
     */
    virtual void ServiceConnected( CScpSubService& aSubService ) const;

    /**
     * Service disconnected
     * @param aSubService Sub service
     */
    virtual void ServiceDisconnected( CScpSubService& aSubService ) const;

    /**
     * Authentication failed
     * @param aSubService Sub service
     */
    virtual void AuthenticationFailed( CScpSubService& aSubService ) const;
    
    /**
     * Roaming ongoing
     * @param aSubService Sub service
     */
    virtual void Roaming( CScpSubService& aSubService ) const;
    
    /**
     * Service connection canceled
     * @param aSubService Sub service
     */
    virtual void ServiceConnectionCanceled( CScpSubService& aSubService ) const;
    
    /**
     * Registration pending
     * @param aSubService Sub service
     */
    virtual void RegistrationPending( CScpSubService& aSubService ) const;
    
    
protected:


    void ChangeState( CScpSubService& aSubService, 
                      TCCHSubserviceState aState,
                      TInt aError ) const;
    };

#endif      // T_SCPSUBSERVICESTATE_H   
            
// End of File
