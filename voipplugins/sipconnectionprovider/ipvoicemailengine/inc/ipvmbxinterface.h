/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Interface class to control voice mail engine
*
*/




#ifndef C_IPVMBXINTERFACE_H
#define C_IPVMBXINTERFACE_H

#include <e32base.h>

#include "ipvmbxobserver.h"
#include "ipvmbxconstants.h"


class CIpVmbxEngine;
class CSIPProfile;


/**
 *  Interface class for IPVoiceMailEngine
 *
 *  This class is used to instantiate
 *  IPVoiceMailEngine. It offers functions to to control
 *  subscription state to Voice mail servers.
 *
 *  @lib IPVoiceMailAppEnginedll.lib
 */

NONSHARABLE_CLASS( CIpVmbxInterface ) : public CBase
    {
    
public:

    /**
     * Two-phased constructor.
     * 
     * @param aEngineObserver Observer instance for status messages (SCP)
     * @return Interface to IPVME
     */
    IMPORT_C static CIpVmbxInterface* NewL( 
        MIpVmbxObserver& aEngineObserver );
    
    /**
    * Destructor.
    */
    virtual ~CIpVmbxInterface();

    /**
     * Subscribes service to ip voice mail server
     *
     * @param aServiceProviderId Service to be subscribed
     * @param aSipProfile Sip profile used for subscription
     */
    IMPORT_C void SubscribeL( 
        TUint32 aServiceProviderId, 
        CSIPProfile& aSipProfile );

    /**
     * Un-subscribes service from ip voice mail server
     *
     * @param aServiceProviderId Service to be un-subscribed
     */
    IMPORT_C void UnsubscribeL( TUint32 aServiceProviderId );

    /**
     * Check if service is subscribed to ip voice mail server
     *
     * @param aServiceProviderId Service which connection status will be 
     *        checked
     * @param aProfileSubscribed Is subscribed
     * @return KErrNotFound if instance does not exist
     */
    IMPORT_C TInt IsSubscribed( 
        TUint32 aServiceProviderId, 
        TBool& aProfileSubscribed ) const;
    
    /**
     * Send messages through observer when subscribe status of service 
     * is changed
     *
     * @param aServiceProviderId Service which state has changed
     * @param aMessage Service subscription state
     */
    void SendMessage( 
        TUint32 aServiceProviderId, 
        MIpVmbxObserver::TVmbxMessage aMessage );


private:

    /**
     * Constructor.
     * 
     * @param aEngineObserver Observer for messages (SCP).
     */
    CIpVmbxInterface( MIpVmbxObserver& aEngineObserver );

    void ConstructL();


private: // data

    /**
     * Engine class
     * Own.
     */
    CIpVmbxEngine* iIpVmbxEngine;
    
    /**
     * Observer instance (to SCP)
     */
    MIpVmbxObserver& iEngineObserver;

    
    EUNIT_IMPLEMENTATION
    };

#endif // C_IPVMBXINTERFACE_H
