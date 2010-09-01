/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Representation of SVP MT session
*
*/


#ifndef SVPMTSESSION_H
#define SVPMTSESSION_H

#include <mccpcall.h>
#include "svpsessionbase.h"
#include "svputdefs.h"

/**
 *  Class which acts as SVP MT session.
 *  Contains all the SVP MT session specific methods.
 *
 *  @lib svp.dll
 *  @since S60 3.2
 */
class CSVPMtSession : public CSVPSessionBase              
    {

public:
    
    /**
    * Two-phased constructor.
    */
    static CSVPMtSession* NewL( CMceInSession* aMtSession,
                                TMceTransactionDataContainer& aContainer,
                                TUint32 aServiceId,
                                TUint32 aVoIPProfileId,
                                MSVPSessionObserver& aObserver,
                                CSVPUtility& aSVPUtility,
                                CSVPRtpObserver& aRtpObserver,
                                TInt aKeepAliveValue,
                                TBool aPreconditions );
    
    /**
    * Destructor.
    */
    virtual ~CSVPMtSession();
    

public: // from base classes: MCCPCall

    /**
    * Answers to an incoming session.
    * @since Series60 3.2
    * @return KErrNone if successful or system wide error code
    */
    TInt Answer();
        
    /**
    * Rejects an incoming call.
    * @since Series 60 3.2
    * @return KErrNone if successful or system wide error code
    */
    TInt Reject();

    /**
    * Reguests to queue the call. 
    * @since S60 3.2
    * @return KErrNone if successful or system wide error code
    */
    TInt Queue();

    /**
    * Send 180 Ringing
    * @since S60 3.2
    * @return KErrNone if successful or system wide error code
    */
    TInt Ringing();

    /**
    * From MCCPMtSession. Return boolean value, is incoming session forwarded or not.
    * @since Series60 3.2
    * @return ETrue if session is forwarded, else EFalse
    */
    TBool IsCallForwarded() const;
     
    /**
    * Returns boolean value, is the session mobile originated.
    * @since Series60 3.2
    * @return TBool ETrue if session is mobile originated, otherwise EFalse
    */
    TBool IsMobileOriginated() const;
    
    /** 
    * Calls sessions destructor
    * @since Series60 3.2
    * @return KErrNone if successful or system wide error code
    */
    TInt Release();

    /**
    * Puts the specified session on hold.
    * @since Series60 3.2
    * @return KErrNone if successful or system wide error code
    */
    TInt Hold();

    /**
    * Resumes the session.
    * @since Series60 3.2
    * @return KErrNone if successful or system wide error code
    */
    TInt Resume();
     
    /**
    * Handles remote action after session is established.
    * @since Series60 3.2
    * @param None
    * @return
    */
    TInt IncomingRequest( CMceInSession* aUpdatedSession );
    
private: // new methods

    /**
    * Handles sink/source enabling for Mt streams
    * @since Series 60 3.2
    * @param aStreams array of streams in Mt session
    */
    void HandleMtStreamsL( const RPointerArray< CMceMediaStream >& aStreams );
    
    /**
     * Worker functio to answer an incoming session.
     * @since Series60 3.2
     * @return void
     */
    void DoAnswerL();
    
// From CSVPSessionBase

    /**
     * Sets RTP keep-alive timer value and payload
     * @since Series 60 3.2
     * @param aSession MCE session
     * @param aSessionUpdateOngoing Flag determines whether session udpate is needed
     * @return void
     */
    void SetRtpKeepAliveL( CMceSession* aSession, 
                           TBool aSessionUpdateOngoing );
    
private:

    /**
    * C++ default constructor.
    */
    CSVPMtSession( CMceInSession* aMtSession,
                   TMceTransactionDataContainer& aContainer,
                   TUint32 aVoIPProfileId,
                   MSVPSessionObserver& aObserver,
                   CSVPUtility& aSVPUtility,
                   CSVPRtpObserver& aRtpObserver,
                   TInt aKeepAliveValue,
                   TBool aPreconditions );
    
    /**
     * Symbian 2nd phase construction
     */
    void ConstructL( TUint32 aServiceId );

private:

    // For testing
    SVP_UT_DEFS
    
    };

#endif // SVPMTSESSION_H
