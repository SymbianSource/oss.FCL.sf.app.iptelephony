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
* Description:  Creates SMS message if Event in MailBox has occurred
*
*/



#ifndef IPVMBXENGINE_H
#define IPVMBXENGINE_H

#include <msvapi.h>
#include <mcetransactiondatacontainer.h>
#include <stringresourcereader.h>

#include "ipvmbxinterface.h"
#include "ipvmbxbase.h"
#include "ipvmbxparsetype.h"


class CSIPProfile;
class CSPSettings;
class CIpVmbxBase;
class TIpVmbxEventMonitor;


/**
 *  Creates SMS message if Event in MailBox has occurred.
 *
 *  @lib IpVmbxAppEngine.lib
 */
NONSHARABLE_CLASS( CIpVmbxEngine ) :
    public CBase,
    public MMsvSessionObserver
    {

public:  // Constructors and destructor

    /** Internal state messages */
    enum TIpVmbxMessages
        {
        EEngineUndefined,
        EEngineSubscribed,
        EEngineTerminated,
        EEngineSubscribeRejected,
        EEngineIncorrectAccount,
        EEngineSmsError,
        EEngineSmsOom,
        EEngineNetworkLost,
        EEngineNetworkError,
        EEngineFatalNetworkError
        };

    /**
     * Two-phased constructor.
     *
     * @param aInterface Instance to interface for status message sending
     */
    IMPORT_C static CIpVmbxEngine* NewL( CIpVmbxInterface& aInterface );

    /**
     * Destructor.
     */
    virtual ~CIpVmbxEngine();


public: // New functions

    /**
     * Starts subscription to MailBox when SIP profile has registered. Only
     * one of these parameters can be used at same time.
     *
     * @param aServiceProviderId Service provider id of mail settings
     * @param aSipProfileId Connected SIP profile id
     */
    void SubscribeProfileL(
        TUint32 aServiceProviderId,
        CSIPProfile& aSipProfile );

    /**
     * Registration status of profile.
     *
     * @param aServiceProviderId Provider id which status is queried
     * @param aProfileSubscribe Status of subscription
     * @return KErrNotFound if profile does not exist
     */
    TInt ProfileSubscribed(
        TUint32 aServiceProviderId,
        TBool& aProfileSubscribed );

    /**
     * Resolve address of base class using recipient address
     *
     * @param aId Recipient
     * @return Address of base class
     */
    CIpVmbxBase* SubscriptionByRecipient( const TDesC8& aRecipient8 );

    /**
     * EventMonitor has received event and the event
     * will be gained and checked if it requires any action
     *
     * @param aRecipient Recipient of event
     */
#ifdef EUNIT_TEST_IMPLEMENTATION
    virtual void EventReceivedL( const TDesC8& aRecipient8 );
#else
    void EventReceivedL( const TDesC8& aRecipient8 );
#endif

    /**
     * Disconnects IPVME from voice mail -server
     *
     * @param aServiceProviderId Provider id to be unregistered
     */
    void UnsubscribeProfileL( TUint32 aServiceProviderId );

    /**
     * Cleanup base class and free resources
     *
     * @pre Unsubscribe has been sent to voice mail -server
     */
    void CleanVmbxBase();

    /**
     * Send status messages to observers
     *
     * @param aServiceProviderId Service provider id
     * @param aMessage MIpVmbxObserver status messages
     */
#ifdef EUNIT_TEST_IMPLEMENTATION

    virtual void HandleMessage(
        TUint32 aServiceProviderId,
        CIpVmbxEngine::TIpVmbxMessages aMessage );

#else

    void HandleMessage(
        TUint32 aServiceProviderId,
        CIpVmbxEngine::TIpVmbxMessages aMessage );

#endif

    /**
     * Fetch instance to base class using service id
     *
     * @param aServiceProviderId Service id to indentify base class
     * @return Instance to base
     */
    CIpVmbxBase* SubscriptionByProvider( TUint32 aServiceProviderId );


protected:

// from base class MMsvSessionObserver

    /**
     * From base class MMsvSessionObserver
     * For notification of events from a Message Server session
     *
     * @since Series 60 3.0
     * @param aEvent Indicates the event type,
     * @param*aArg1 Event type-specific argument value
     * @param*aArg2 Event type-specific argument value
     * @param*aArg3 Event type-specific argument value
     */
    virtual void HandleSessionEventL( TMsvSessionEvent aEvent, TAny *aArg1,
        TAny *aArg2, TAny *aArg3 );


private:

    void ConstructL();

    /**
     * C++ default constructor.
     *
     * @param aInterface Instance to interface for status message sending
     */
    CIpVmbxEngine( CIpVmbxInterface& aInterface );

    /**
     * For parsing the content of NOTIFY and some optional messages.
     * If function completes without errors, supplied content is cut so that
     * only possible optional header data is left.
     *
     * @param aCreateSms ETrue if user has to be informed (SMS).
     * @param aContent Notify content to be parsed
     * @param aTotalMessages Total messages
     * @param aNewMessages New messages
     * @param aFrom Voice mail account
     */
    void ParseNotifyContentL(
        TBool& aCreateSms,
        TDes8& aContent8,
        TDes8& aTotalMessages8,
        TDes8& aNewMessages8,
        TDes8& aFrom8 ) const;

    /**
     * Creates SMS message and sends it
     *
     * @param aFrom MBX uri
     * @param aMessageBody Generated message
     */
    void CreateSMSMessageL( const TDesC8& aFrom8, const TDesC8& aMessageBody8 );

    /**
     * For parsing optional headers from content of NOTIFY.
     *
     * @param aContent Notify content
     * @param aMessageBody Message body for optional parameters
     */
    void ParseOptionalHeadersL(
        const TDesC8& aContent8,
        TDes8& aMessageBody8 ) const;

    /**
     * Read localized content to message body
     *
     * @param aTotalMessages Total messages
     * @param aNewMessages New messages
     * @param aMessagesBody Message body
     */
    void ReadResourcesL(
            const TDesC8& aTotalMessages8,
            const TDesC8& aNewMessages8,
            TDes8& aMessagesBody8 );

    /**
     * Creates and verifies message body
     *
     * @param aContent Notify content
     * @param aTotal Total messages
     * @param aNew New messages
     * @param aFrom MBX uri
     * @param aMessageBody Message body
     */
    void CreateMessageBodyL(
        const TDesC8& aContent8,
        const TDesC8& aTotal8,
        const TDesC8& aNew8,
        TDes8& aFrom8,
        TDes8& aMessageBody8 );

    /**
     * General optional parameter parser
     *
     * @param aTagPtr8 Source text
     * @param aResourceReader Instance to resource reader( to prevent
     *        double instantiation )
     * @return Translation, NULL if not found.
     */
    HBufC* TranslateTagL(
        const TDesC8& aTagPtr8,
        CStringResourceReader& aResourceReader ) const;

    /**
     * Fetch message part from whole message. It gets first part from
     * provided string and checks validity of message part.
     *
     * @param aContent8 Source text
     * @return Message part, empty if content was not valid.
     */
    TPtrC8 FetchMessagePartL( const TDesC8& aContent8 ) const;

    /**
     * Check name header from message part.
     *
     * @param aNameHeader8 Source text
     */
    void TestNamePartL( const TDesC8& aNameHeader8 ) const;

    /**
     * Check value header from message part.
     *
     * @param aValueHeader8 Source text
     */
    void TestValuePartL( const TDesC8& aValueHeader8 ) const;


private: // data

    /**
     * MCE server connection
     * Own.
     */
    CMceManager* iMceManager;

    /**
     * Event content
     */
    TMceTransactionDataContainer iEventData;

    /**
     * MCE Event observer
     * Own.
     */
    TIpVmbxEventMonitor* iIpVmbxEventMonitor;

    /**
     * Interface
     */
    CIpVmbxInterface& iInterface;

    /**
     * Vmbx parameters
     * Own.
     */
    CSPSettings* iServiceSettings;

    /**
     *  Array of current Base classes
     */
    RPointerArray< CIpVmbxBase > iVmbxBaseArray;

    /**
     *  Indicates if VME is already running
     */
    TBool iBasicServicesRunning;


    EUNIT_IMPLEMENTATION
    };

#endif      // IPVMBXENGINE_H
