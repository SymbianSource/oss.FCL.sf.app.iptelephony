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


#ifndef T_SCPDISABLED_H
#define T_SCPDISABLED_H

#include <e32base.h>

#include "scpsubservicestate.h"

/**
*  Disabled state
*
*  @lib sipconnectionprovider.dll
*/
class TScpDisabled : public TScpSubServiceState
    {
public:

    /**
     * C++ default constructor.
     */
    TScpDisabled();

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
     * Enable
     * @param aSubService Sub service
     */
    void NetworkFound( CScpSubService& aSubService ) const;

    /**
     * Network not found
     * @param aSubService Sub service
     */
    void NetworkNotFound( CScpSubService& aSubService ) const;

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
    };

#endif      // T_SCPDISABLED_H   
            
// End of File
