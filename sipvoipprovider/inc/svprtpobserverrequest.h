/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  RTP observer for SVP
*
*/


#ifndef SVPRTPOBSERVERREQUEST_H
#define SVPRTPOBSERVERREQUEST_H

// INCLUDES
#include <e32base.h>
#include "svptimerobserver.h"

// FORWARD DECLARATIONS
class CSVPSessionBase;
class CMceSession;
class CMceMediaStream;
class CSVPTimer;

// CLASS DECLARATION

/**
 * CSVPRtpObserverRequest
 * 
 */
class CSVPRtpObserverRequest : public CBase, public MSVPTimerObserver
    {
public:     // Data types
    
    // Enumeration to determine RTCP activitytype
    enum TReceivedRtcpPacket
        {
        ERtcpRrPacket = 0,
        ERtcpSrPacket = 1
        };
    
public:     // Constructors and destructor

    /**
     * Destructor.
     */
    ~CSVPRtpObserverRequest();

    /**
     * Two-phased constructor.
     */
    static CSVPRtpObserverRequest* NewL( CSVPSessionBase& aSession );

    /**
     * Two-phased constructor.
     */
    static CSVPRtpObserverRequest* NewLC(  CSVPSessionBase& aSession  );
    
public:     // New functions
    
    /**
     * Returns the SVP session associated to this request.
     * @since S60 v3.2
     * @return CSVPSessionBase
     */
    const CSVPSessionBase* SvpSession() const;
    
    /**
     * Returns the MCE session associated to this request.
     * @since S60 v3.2
     * @return CMceSession
     */
    const CMceSession* MceSession() const;
    
    /**
     * Called when RTCP RR or SR packet has been received.
     * @since S60 v3.2
     * @aPacket RTCP packet type
     * @aSession MCE session
     * @aStream MCE media stream.
     */
    void RtcpPacketReceived( TReceivedRtcpPacket aPacket,
        CMceSession& aSession, CMceMediaStream& aStream );
    
    /**
     * Called when associated session's state has changed.
     * @since S60 3.2
     */
    void Reset();


public:     // Functions from base classes
    
    // From MSVPTimerObserver
    void TimedOut( TInt aTimerId );
    
private:     // New functions
    
    /**
     * Determines the RTCP timeout value based on the media stream and
     * it's associated codecs. Returns default value (35 seconds) if
     * error is encountered.
     * @since S60 v3.2
     * @param aStream Stream from where to search the timeout value
     * @return TInt RTCP timeout value in milliseconds
     */
    TInt DetermineTimeoutValue( CMceMediaStream& aStream ) const;

    /**
     * Returns ETrue if the associated session is currently in Hold state 
     * @since S60 v3.2.3
     * @return TBool value
     */
    TBool IsSessionInHoldState();
    
private:    // Consturctors

    /**
     * Constructor for performing 1st stage construction
     */
    CSVPRtpObserverRequest( CSVPSessionBase& aSession );

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL();

private:    // Data
    
    /**
     * SVP session associated to this observing request
     * Not own
     */
    CSVPSessionBase& iSession;
    
    /**
     * Timer for determining RTCP timeout.
     * Own
     */
    CSVPTimer* iTimer;
    
    /**
     * Flag to indicate that 'disconnect' state has been informed.
     */
    TBool iDisconnectSent;

public:     // For testing purposes
    friend class UT_CSVPRtpObserverRequest;
    };

#endif // SVPRTPOBSERVERREQUEST_H

