/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Base class of sub service handlers.
*
*/


#ifndef C_CSCSERVICEHANDLERBASE_H
#define C_CSCSERVICEHANDLERBASE_H

#include <e32base.h>
#include <cchclientserver.h>

#include "scpsipconnectionobserver.h"
#include "scpdefs.h"
#include "scpsipconnection.h"

class CScpSubService;

/**
*  Base class of sub service handlers.
*
*  @lib sipconnectionprovider.dll
*/
class CScpServiceHandlerBase : public CBase,
                               public MScpSipConnectionObserver
                               
    {
public:  // Constructors and destructor

    /**
     * Enables service.
     */
    virtual void EnableSubServiceL() = 0;

    /**
     * Disables service.
     */
    virtual TInt DisableSubService() = 0;
                   
    /**
     * Returns type of sub service (VoIP, Vmbx, Presence).
     * @return Type of the sub service.
     */
    virtual TCCHSubserviceType SubServiceType() const = 0;
    
    /**
    * Destructor.
    */
    virtual ~CScpServiceHandlerBase();

protected:
                                  
    /**
    * C++ default constructor.
    */
    CScpServiceHandlerBase( CScpSubService& aSubService );

    /**
     * Base construction
     */
    void BaseConstructL();

    /**
     * Registers the sub service's profile
     * @param aSubService Registered sub service
     * @return State of the registration
     */
    void RegisterProfileL();

    /**
     * Deregisters the sub service's profile
     * @param aSubService Deregistered sub service
     */
    void DeregisterProfile();

    /**
     * Starts force disable timer
     * @param aFunction The function to call after the time out
     */
    void StartForcedDisableTimer( TInt (*aFunction)(TAny* aPtr) );

    /**
     * Cancels the disable timer
     */
    void CancelDisableTimer();

private:

    /**
     * Handle sip connection event
     * @param aProfileId Sip profile id
     * @param aSipEvent Sip event
     */
    virtual void HandleSipConnectionEvent( const TUint32 aProfileId,
                                           TScpConnectionEvent aSipEvent ) = 0;
    
    /**
     * From base class MScpSipConnectionObserver
     * Query is starting to ALR allowed.
     * Client can start ALR later if needed to do something first
     * 
     * @return ETrue if allowed.
     */
    virtual TBool IsSipProfileAllowedToStartAlr();
    
    /**
     * Forced sip profile disable call back
     * @param aSelf this object
     */
    static TInt ForceSipProfileDisable( TAny* aSelf );

    /**
     * Handles sip profile forced disable timeout
     * Sends Deregistered event for the subservice
     */
    void HandleSipProfileForcedDisable();

protected:

    /**
     * Sub service
     */
    CScpSubService& iSubService;

private:

    /**
     * Disable timer. Owned.
     */
    CPeriodic* iDisableTimer;

#ifdef _DEBUG
    friend class T_CScpServiceManager;
    friend class T_CScpVmbxHandler;
#endif
    };

#endif      // C_CSCSERVICEHANDLERBASE_H   
            
// End of File
