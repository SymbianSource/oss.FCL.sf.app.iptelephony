/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Creates SMS message if Event in MailBox has occurred
*
*/



#ifndef IPVMBXBASE_H
#define IPVMBXBASE_H

#include <e32base.h>
#include <sipprofile.h>

#include "ipvmbxconstants.h"


class CMceOutEvent;
class CMceManager;
class CSIPProfile;
class CIpVmbxEngine;


/**
 *  Creates SMS message if Event in MailBox has occurred.
 *
 *  @lib ipvmbxengine.dll
 */
NONSHARABLE_CLASS( CIpVmbxBase ) : public CBase
    {

public:

    /** States for base class */
    enum TIpVmbxBaseStates
        {
        ESubscribing,
        ERegistered,
        ETerminating,
        EDisabled
        };

    /**
     * Two-phased constructor.
     *
     * @param aVmbxEngine Instance to engine
     * @param aVoiceMailUri Address of voice mail account
     * @param aMceManager Manager instance of IPVME
     * @return Created base class
     */
    static CIpVmbxBase* NewL(
        CIpVmbxEngine& aVmbxEngine,
        TDesC8& aVoiceMailUri8,
        CMceManager& aMceManager );

    /**
     * Destructor.
     */
    virtual ~CIpVmbxBase();

    /**
     * Set changing parameters for spescfic voice mail account.
     *
     * @param aServiceProviderId Service provider id
     * @param aSipProfile Sip profile
     */
    void Initialize(
        TUint32 aServiceProviderId,
        CSIPProfile& aSipProfile );

    /**
     * Subscribes to mail server. If refresh value is not entered,
     * defaults to KReSubscribe.
     *
     * @param aReSubscribe Parameter for refresh initiated by IPVME
     */
    void SubscribeL( TInt aReSubscribe );

    /**
     * Unsubscribes from mail server.
     */
    void TerminateEventL();

    /**
     * Cleans MceEvent instance.
     */
    void DeleteEvent();

    /**
     * Update status of voice mails.
     */
    static TInt ReSubscribe( TAny* aThis );

    /**
     * Cancel resubscribe timer
     */
    void Cancel();

    /**
     * Get service provider data
     *
     * @return Service id
     */
    TUint32 ServiceProviderId() const;

    /**
     * Account uri to which app base has connected
     *
     * @return MBX account
     */
    const TDesC8& VmbxUrl() const;

    /**
     * Sets base state to connected state and starts
     * resubscription process.
     */
    void SetStateRegistered();

    /**
     * State of base class
     *
     * @return State of base class
     */
    CIpVmbxBase::TIpVmbxBaseStates State() const;

    /**
     * Save current message state of mailbox
     *
     * @param Total messages
     * @param New messages
     */
    void SetAccountMessageCount( TInt aTotalCount, TInt aNewCount );

    /**
     * Get previous message state of mailbox
     *
     * @param Total messages
     * @param New messages
     */
    void AccountMessageCount( TInt& aTotalCount, TInt& aNewCount);

private:

    void ConstructL( TDesC8& aVoiceMailUri8 );

    /**
     * C++ default constructor.
     *
     * @param aVmbxEngine Engine instance to send status messages
     * @param aVoiceMailUri Uri which this base class will subscribe
     * @param aMceManager Required manager instance for outbound connections
     */
    CIpVmbxBase(
        CIpVmbxEngine& aVmbxEngine,
        CMceManager& aMceManager );

private:    // Data

    /**
     * SIP profile information
     * Not own.
     */
    CSIPProfile* iSipProfile;

    /**
     * MO events
     * Own.
     */
    CMceOutEvent* iMceOutEvent;

    /**
     * MCE server connection
     */
    CMceManager& iMceManager;

    /**
     * Re SUBSCRIBE timer
     * Own.
     */
    CDeltaTimer* iReSubscribe;

    /**
     * Re SUBSCRIBE period
     */
    TTimeIntervalMicroSeconds iReSubscribePeriod;

    /**
     * State of base class
     */
    TIpVmbxBaseStates iState;

    /**
     * Id of service this base class is using
     */
    TUint32 iServiceProviderId;

    /**
     * Uri this base class is monitoring
     */
    TBuf8< KMaxIpVoiceMailBoxUriLength > iVmbxUri8;

    /**
     * Server total messages
     */
    TInt iTotalMessageCount;

    /**
     * Server new messages
     */
    TInt iNewMessageCount;

    /**
     * Amount of pending resubscribes
     */
    TInt iPendingResubscribeCount;

    /**
     * Message sender
     */
    CIpVmbxEngine& iVmbxEngine;

    /**
     * Re subscribe callback
     */
    TDeltaTimerEntry iUpdateEvent;


    EUNIT_IMPLEMENTATION
    };

#endif // IPVMBXBASE_H
