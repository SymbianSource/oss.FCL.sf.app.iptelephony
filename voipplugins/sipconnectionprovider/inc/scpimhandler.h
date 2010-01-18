/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: SCP IM Handler
*
*/


#ifndef C_SCPIMHANDLER_H
#define C_SCPIMHANDLER_H


#include "scpservicehandlerbase.h"

class CSipClientResolverUtils;


/**
 *  Class for handling IM sub service.
 *
 *  @code
 *   CScpSubService* imSubService = CScpSubService::NewL( newId,
 *       ServiceId(), ECCHIMSub, *service );
 *   CScpServiceHandlerBase* imSubServiceHandler = CScpImHandler::NewL(
 *       *imSubService );
 *   imSubServiceHandler->EnableSubServiceL();
 *  @endcode
 *
 *  @lib sipconnectionprovider.dll
 *  @since S60 v5.1
 */
class CScpImHandler : public CScpServiceHandlerBase
    {

public:

    /**
     * Two-phased constructor.
     * @param aSubService sub service
     */
    static CScpImHandler* NewL( CScpSubService& aSubService );

    /**
     * Destructor.
     */
    virtual ~CScpImHandler();

// from base class CScpServiceHandlerBase

    /**
     * From CScpServiceHandlerBase
     * Defined in a base class
     */
    void EnableSubServiceL();

    /**
     * From CScpServiceHandlerBase
     * Defined in a base class
     */
    TInt DisableSubService();
            
    /**
     * From CScpServiceHandlerBase
     * Defined in a base class
     */
    TCCHSubserviceType SubServiceType() const;
    
    /**
     * From CScpServiceHandlerBase
     * Defined in a base class
     */
    void HandleSipConnectionEvent( const TUint32 aProfileId,
        TScpConnectionEvent aSipEvent );

private:

    CScpImHandler( CScpSubService& aSubService );

    void ConstructL();

    /**
     * Handle SIP connection event.
     *
     * @since S60 v5.1
     * @param aProfileId SIP profile ID
     * @param aSipEvent  connection event
     */
    void HandleSipConnectionEventL( const TUint32 aProfileId,
        TScpConnectionEvent aSipEvent );

    /**
     * Find SIP profile contact header user name for the given sub service.
     * 
     * @since S60 v5.1
     * @param aContactHeader found user name is inserted here
     */
    void GetSipProfileContactHeaderUserL( RBuf8& aContactHeader ) const;

    /**
     * Remove the the routing used for SIP profile's contact header 
     * from SIP Client routing table.
     *
     * @since S60 v5.1
     */
    void DeregisterContactHeaderL();

private: // data

    /**
     * SIP Client Resolver
     * Own.
     */
    CSipClientResolverUtils* iSipClientResolver;

#ifdef _DEBUG
    friend class T_CScpImHandler;
#endif
    };

#endif // C_SCPIMHANDLER_H
