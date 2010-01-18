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


#ifndef C_CSCPVMBXHANDLER_H
#define C_CSCPVMBXHANDLER_H

#include <e32base.h>

#include "scpservicehandlerbase.h"
#include "ipvmbxobserver.h"

class CScpSubService;
class CScpServiceStorage;

/**
*  Vmbx sub service handler.
*
*  @lib sipconnectionprovider.dll
*  @since Series 60 3.2
*/
class CScpVmbxHandler : public CScpServiceHandlerBase,
                        public MIpVmbxObserver 
    {
public:  // Constructors and destructor
 
    static CScpVmbxHandler* NewL( CScpSubService& aSubService );
    
    /**
     * Destructor.
     */
    virtual ~CScpVmbxHandler();

private: // From base class
    
    /**
     * Enables service.
     */
    void EnableSubServiceL();

    /**
     * Checks if another vmbx sub service is already enabled
     * @return ETrue if enabled
     */
    TBool IsAnotherVmbxSubServiceAlreadyEnabled() const;

    /**
     * Disables service.
     */
    TInt DisableSubService();

     /**
     * Handles messages from Vmbx interface
     * @param aServiceId Message concers the given service id
     * @param aMessage Message
     */
    void HandleMessage( TUint32 aServiceId, TVmbxMessage aMessage );

    /**
     * Handlers sip connection event
     * @param aProfileId Sip profile id
     * @param aEvent Connection event
     */
    void HandleSipConnectionEvent( TUint32 aProfileId,
                                   TScpConnectionEvent aEvent );

    /**
     * Returns type of sub service (VoIP, Vmbx, Presence).
     * @return Type of the sub service.
     */
    TCCHSubserviceType SubServiceType() const;

private:

    /**
     * C++ default constructor.
     */
    CScpVmbxHandler( CScpSubService& aSubService );

    /**
     * 2nd phase constructor.
     */    
    void ConstructL();

    /**
     * Disable sub service.
     */    
    void UnsubscribeL();

    /**
     * SubscribeL
     */  
    void SubscribeL();

    /**
     * Force Vmbx service disable
     * @param aSelf This object
     */    
    static TInt ForceVmbxServiceDisable( TAny* aSelf );
    
private: // data
    
    /**
     * Indicates do we have to make resubscribe
     */
    TBool iResubscribe;
    
#ifdef _DEBUG
    friend class T_CScpVmbxHandler;
    friend class T_CScpServiceManager;
#endif
    };

#endif      // C_CSCPVMBXHANDLER_H   
            
// End of File
