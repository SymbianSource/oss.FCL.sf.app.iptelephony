/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Observer class for IPVoiceMailEngine. Used to deliver
*                subscription status.
*
*/



#ifndef M_IPVMBXOBSERVER_H
#define M_IPVMBXOBSERVER_H

#include <e32def.h>

/**
 *  Observer class for IPVoiceMailEngine
 *
 *  This class must be herited to receive IPVoiceMailEngine's
 *  subscription status messages.
 *
 *  @lib IPVoiceMailAppEnginedll.lib
 */
class MIpVmbxObserver
    {

public:

    /** Subscribtion states */
    enum TVmbxMessage
        {
        ESubscribed,
        EUnsubscribed,
        ENoMemory,
        ESmsError,
        ENetworkError,
        EIncorrectSettings,
        EFatalNetworkError
        };

    /**
     * This function is called to inform status of subscribe and un-subscribe
     *
     * @param aSipProfileId Sip profile conserning about delivered message
     * @param aMessage Subscribtion status
     */
    virtual void HandleMessage( 
        TUint32 aServiceProviderId, 
        TVmbxMessage aMessage ) = 0;

    };


#endif // M_IPVMBXOBSERVER_H
