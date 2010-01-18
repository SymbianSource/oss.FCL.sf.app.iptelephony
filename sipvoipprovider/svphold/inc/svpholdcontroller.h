/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Hold controller, interface class for handling hold
*
*/


#ifndef SVPHOLDCONTROLLER_H
#define SVPHOLDCONTROLLER_H

#include    <e32base.h>
#include    <mccpcallobserver.h>
#include    "svpholdcontext.h"
#include    "svputdefs.h"

class CMceSession;
class MSVPHoldObserver;
class CSVPSessionBase;

const TInt KErrSVPHoldErrorBase                 = -2000;
const TInt KErrSVPHoldStateError                = KErrSVPHoldErrorBase - 1;
const TInt KErrSVPHoldInProgress                = KErrSVPHoldErrorBase - 2;
const TInt KErrSVPHoldResumeInProgress          = KErrSVPHoldErrorBase - 3;
const TInt KErrSVPHoldRequestPending            = KErrSVPHoldErrorBase - 4;
const TInt KErrSVPHoldNotHoldRequest            = KErrSVPHoldErrorBase - 5;
const TInt KErrSVPHoldUnacceptableResponseAttribute = KErrSVPHoldErrorBase - 6;
const TInt KErrSVPHoldLocalOldwayholdNeeded     = KErrSVPHoldErrorBase - 7;
const TInt KErrSVPHoldRequestProcessingNotReady = KErrSVPHoldErrorBase - 8;


/**
*  Hold controller.
*  @lib svp.dll
*  @since Series 60 3.2
*/
class CSVPHoldController : public CBase
    {
public:  // Constructors and destructor
    
    /**
    * Two-phased constructor.
    */
    static CSVPHoldController* NewL(                             
                        CMceSession& aSession,
                        TMceTransactionDataContainer& aContainer,
                        MSVPHoldObserver* aObserver,
                        TBool aIsMobileOriginated );

    /**
    * Two-phased constructor.
    */
    static CSVPHoldController* NewLC( 
                        CMceSession& aSession,
                        TMceTransactionDataContainer& aContainer,
                        MSVPHoldObserver* aObserver,
                        TBool aIsMobileOriginated );
    
    /**
    * Destructor.
    */
    virtual ~CSVPHoldController();

public:  // Methods

    /**
    * Holds session. If hold fails then observer method is called
    * @since Series 60 3.2
    * @param aSession Session object. Ownership not transferred
    * @return Error code
    */
    TInt HoldSession( CMceSession* aSession );

    /**
    * Resumes session. If resume fails then observer method is called
    * @since Series 60 3.2
    * @param aSession Session object. Ownership not transferred
    * @return Error code
    */
    TInt ResumeSession( CMceSession* aSession );
    
    /**
    * Retries previous failed request
    * @since Series 60 3.2
    * @param aSession Session object. Ownership not transferred
    * @return Error code
    */
    TInt RetryHoldRequest( CMceSession* aSession );
    
    /**
    * This method must be called after hold/resume expiration time is
    * exceeded.
    * @since Series 60 3.2
    */
    void TimedOut();
    
    /**
    * Examines incoming request for the case of hold/resume.
    * @since Series 60 3.2
    * @param aSession Session object. Ownership not transferred
    * @return KErrSVPHoldNot if is other type than hold/resume,
    *         system wide error code or KErrNone if succesful.
    *         If KErrNone, observer methods are called.
    */
    TInt IncomingRequest( CMceSession* aSession );
    
    /**
    * Examines response for hold request.
    * @since Series 60 3.2
    * @param aSession Session object. Ownership not transferred
    * @param aStatusCode Status code of the response
    * @return KErrSVPHoldNotHoldRequest if is other type than hold/resume,
    *         system wide error code or KErrNone if succesful.
    *         If KErrNone, observer methods are called.
    */
    TInt IncomingResponse( CMceSession* aSession, TInt aStatusCode );
        
    /**
    * Returns ETrue if holding/resuming session is ongoing.
    * @since Series 60 3.2
    * @return ETrue if holding/resuming of session is ongoing 
    */
    TBool HoldInProgress() const;
    
    /**
    * Returns ETrue if holding/resuming error caused hold state
    * to roll back to its previous state
    * @since Series 60 3.2
    * @return ETrue if holding/resuming rolleed back 
    */
    TBool HoldRolledBack() const;
    
    /**
    * Returns Hold failed -boolean value. After this function call
    * flag is cleared.
    * @since Series 60 3.2
    * @return ETrue if hold failed, EFalse otherways
    */
    TBool HoldFailed();

    /**
    * Returns Resume failed -boolean value. If resume has failed, session
    * must be terminated, and it cannot do before MCE session state is
    * in connected state.
    * @since Series 60 3.2
    * @return ETrue if resume failed, EFalse otherways
    */
    TBool ResumeFailed();
    
    /**
    * Continues processing of local hold/resume request.
    * @since Series 60 3.2
    * @param aSession MCE session
    * @return error code
    */
    TInt ContinueHoldProcessing( CMceSession& aSession );
    
    /**
    * Hold / Resume request has failed
    * @since Series 60 3.2
    * @param aSession MCE session
    * @param aStatusCode reason
    * @param aBase SVP session base
    * @return error code
    */
    TInt RequestFailed( CMceSession& aSession, TInt aStatusCode, CSVPSessionBase& aBase );
    
    /**
    * Checks if currently crossover situation
    * @since Series 60 3.2
    * @param aBase SVP session base
    */
    void CheckCrossOver( CSVPSessionBase& aBase );
    
    /**
    * Returns session hold state
    * @since Series 60 3.2
    * @return Current hold state (Holded only if local stream on hold)
    */
    TSVPSessionHoldState HoldState() const;

    /**
    * Returns session hold request type
    * @since Series 60 3.2
    * @return Current hold request type
    */
    TSVPHoldRequestType HoldRequest() const;

    /**
    * After completed & succesful IN / OUT Request, returns occurred event.
    * @since Series 60 3.2
    * @return Occurred event to be informed to client application
    */
    MCCPCallObserver::TCCPCallEvent HoldEvent();
    
    /**
    * Sets mute state to Hold Controller
    * @since Series 60 3.2
    * @param aMuted Boolean value for mute value
    */
    void Muted( TBool aMuted );

    /**
    * Refreshes hold state in local hold state when non-hold re-invite received
    * @since Series 60 3.2
    * @param
    */
    void RefreshHoldStateL();

private:

    /**
    * C++ default constructor.
    */
    CSVPHoldController();

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL( CMceSession& aSession,
                     TMceTransactionDataContainer& aContainer,
                     MSVPHoldObserver* aObserver,
                     TBool aIsMobileOriginated );
    
    /**
    * HoldAllowed
    */
    TBool HoldAllowed();
    
    /**
    * ResumeAllowed
    */
    TBool ResumeAllowed();
    
    /**
    * IncomingRequestAllowed
    */
    TBool IncomingRequestAllowed();
        
    /**
    * IncomingResponseAllowed
    */
    TBool IncomingResponseAllowed();

    /**
    * Refresh hold state
    * @since S60 3.2
    * @param aMediaStream Representation of MCE media stream
    */
    void RefreshL( CMceMediaStream& aMediaStream );

private: // data

    // Context of hold
    CSVPHoldContext* iContext;

    // Previous hold state    
    TSVPHoldState iPreviousHoldState;
    
    // Flag for readyness of hold request
    TBool iHoldRequestCompleted;
    
    // Desired hold request for re-Invite crossover case
    TSVPHoldDesiredTransition iHoldRequest;
    
    // Reinvite crossover flag (race condition)
    TBool iReinviteCrossover;

private:

    // For testing
    SVP_UT_DEFS
		
    };

#endif      // SVPHOLDCONTROLLER_H  
            
