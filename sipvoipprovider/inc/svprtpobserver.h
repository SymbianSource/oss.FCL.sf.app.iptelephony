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


#ifndef SVPRTPOBSERVER_H
#define SVPRTPOBSERVER_H

// INCLUDES
#include <e32base.h>
#include <mcesession.h>
#include <mcertpobserver.h>

// FORWARD DECLARATIONS
class CSVPSessionBase;
class CSVPRtpObserverRequest;

// CLASS DECLARATION

/**
 * CSVPRtpObserver
 * 
 */
class CSVPRtpObserver : public CBase, public MMceRtpObserver
    {
public:     // Constructors and destructor

    /**
     * Destructor.
     */
    ~CSVPRtpObserver();

    /**
     * Two-phased constructor.
     */
    static CSVPRtpObserver* NewL();

    /**
     * Two-phased constructor.
     */
    static CSVPRtpObserver* NewLC();

public:     // From MMceRtpObserver
    
    void SRReceived( CMceSession& aSession,
					CMceMediaStream& aStream );
    
    void RRReceived( CMceSession& aSession,
					CMceMediaStream& aStream );
    
    void InactivityTimeout( CMceMediaStream& aStream,
					CMceRtpSource& aSource );
    
    void SsrcAdded( CMceMediaStream& aStream,
					CMceRtpSource& aSource,
					TUint aSsrc );
    
    void SsrcRemoved( CMceMediaStream& aStream,
					 CMceRtpSource& aSource,
					 TUint aSsrc );
    
public:     // New functions
    
    /**
     * Adds a SVP session for observing RTCP (or RTCP timeouts).
     * @since S60 v3.2
     * @param aSession SVP session.
     */
    void AddSessionForObservingL( CSVPSessionBase* aSession );
    
    /**
     * Removes a SVP session from RTCP observer.
     * @since S60 v3.2
     * @param aSession SVP session.
     * @return TInt KErrNotFound if given session is not found,
     *              KErrNone otherwise.
     */
    TInt RemoveSessionFromObserving( CSVPSessionBase* aSession );
    
    /**
     * Resets(stops) session specific RTCP observing timers.
     * @since S60 3.2
     * @param aSession SVP session.
     * @return TInt KErrNotFound if given session is not found,
     *              KErrNone otherwise.
     */
    TInt ResetSessionInObserving( CSVPSessionBase* aSession );


private:    // New functions
    
    /**
     * Finds a observer request by SVP session.
     * @since S60 v3.2
     * @param aSession SVP session
     * @return CSVPRtpObserverRequest or NULL if associated request not found
     */
    CSVPRtpObserverRequest* FindBySvpSession(
        const CSVPSessionBase* aSession );
    
    /**
     * Finds a observer request by MCE session.
     * @since S60 v3.2
     * @param aSession SVP session
     * @return CSVPRtpObserverRequest or NULL if associated request not found
     */
    CSVPRtpObserverRequest* FindByMceSession( const CMceSession* aSession );
    
private:    // Consturctors

    /**
     * Constructor for performing 1st stage construction
     */
    CSVPRtpObserver();

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL();

private:    // Data
    
    /**
     * Request array.
     * Own
     */
    RPointerArray<CSVPRtpObserverRequest> iRequestArray;
    
    };

#endif // SVPRTPOBSERVER_H

