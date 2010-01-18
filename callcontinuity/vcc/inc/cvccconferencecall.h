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
* Description:   Internal
*
*/




#ifndef CVCCCONFERENCECALL_H
#define CVCCCONFERENCECALL_H

#include <e32base.h>
#include <mccpconferencecall.h>         // CCP API MCCPConferenceCall
#include <mccpconferencecallobserver.h> //CCP API MCCPConferenceCallObserver

class CVccPerformer;
class CConvergedCallProvider;

/**  Type definition for call arrays */
typedef RPointerArray<CVccPerformer> RVccCallArray;

/**
 *  Internal conference call object
 *  This object contains logic for VCC conference calls
 *
 *  @lib vccperformer.dll
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS(CVccConferenceCall) : public CBase,
                                        public MCCPConferenceCall,
                                        public MCCPConferenceCallObserver
    {
public:

    /**
     * Two-phased constructor.
     * @param aServiceId Service id
     * @param aObserver Reference to conference call observer in CCE
     * @param aCallArray Reference to CVccDirector's performer array
     */
    static CVccConferenceCall* NewL( const TUint32 aServiceId,
                               const MCCPConferenceCallObserver& aObserver,
                               RVccCallArray& aCallArray );
    
    /**
     * Two-phased constructor.
     * @param aConferenceCall
     * @param aCallArray Reference to CVccDirector's performer array
     */
    static CVccConferenceCall* NewL( MCCPConferenceCall& aConferenceCall, 
                                    RVccCallArray& aCallArray );

    /**
     * Destructor.
     */
    virtual ~CVccConferenceCall();
    
    /**
     * Singnals CS plugin that conference is released
     *
     * @since S60 v3.2
     */
    void ReleaseConference();

// from base class MCCPConferenceCall

    /** @see MCCPConferenceCall::ServiceId */
    TUint32 ServiceId() const;
    
    /** @see MCCPConferenceCall::HangUp */
    TInt HangUp();
    
    /** @see MCCPConferenceCall::Hold */
    TInt Hold();
       
    /** @see MCCPConferenceCall::Resume */
    TInt Resume();
    
    /** @see MCCPConferenceCall::Swap */
    TInt Swap();
    
    /** @see MCCPConferenceCall::AddCallL */
    void AddCallL( MCCPCall* aCall );

    /** @see MCCPConferenceCall::RemoveCallL */
    void RemoveCallL( MCCPCall* aCall );

    /** @see MCCPConferenceCall::CallCount */
    TInt CallCount() const;

    /** @see MCCPConferenceCall::GoOneToOneL */
    void GoOneToOneL( MCCPCall& aCall );

    /** @see MCCPConferenceCall::CurrentCallsToConferenceL */
    void CurrentCallsToConferenceL();
        
    /** @see MCCPConferenceCall::AddObserverL */
    void AddObserverL( const MCCPConferenceCallObserver& aObserver );
    
    /** @see MCCPConferenceCall::RemoveObserver */
    TInt RemoveObserver( const MCCPConferenceCallObserver& aObserver );
    
    /** @see MCCPConferenceCall::GetCallArray */
    TInt GetCallArray( RPointerArray<MCCPCall>& aCallArray );
    
// from MCCPConferenceCallObserver

    /** @see MCCPConferenceCallObserver::ErrorOccurred */
    void ErrorOccurred( TCCPConferenceCallError aError );

    /** @see MCCPConferenceCallObserver::ConferenceCallCapsChanged */
    void ConferenceCallCapsChanged( const TCCPConferenceCallCaps aCaps );

    /** @see MCCPConferenceCallObserver::ConferenceCallStateChanged */
    void ConferenceCallStateChanged( const TCCPConferenceCallState aState );
        
    /** @see MCCPConferenceCallObserver::ConferenceCallEventOccurred */
    void ConferenceCallEventOccurred( const TCCPConferenceCallEvent aEvent,
                                      MCCPCall* aReferredCall = NULL );
         
private:
    
    /**
     * Finds the call from call array
     *
     * @since S60 v3.2
     * @param aCall Call to be found from array
     * @return Pointer to found call or NULL if not found
     */
    CVccPerformer* MatchPluginToVccCall( MCCPCall* aCall );

private:

    /** 
     * C++ Constructor 
     * 
     * @param aServiceId Service id
     * @param aObserver Reference to conference call observer in CCE
     * @param aCallArray Reference to CVccDirector's performer array
     * 
     */
    CVccConferenceCall( const TUint32 aServiceId,
                        const MCCPConferenceCallObserver& aObserver,
                        RVccCallArray& aCallArray );

    /** 
    * C++ Constructor 
    * 
    * @param aConferenceCall Conference call object
    * @param aCallArray Reference to CVccDirector's performer array
    * 
    */
    CVccConferenceCall( MCCPConferenceCall& aConferenceCall,
                            RVccCallArray& aCallArray );
    
    /** 2nd phase constructor */
    void ConstructL();
    
private: // data

    /**
     * ServiceId for this conferencecall
     */
    TUint32 iServiceId;
    
    /**
     * Reference to conference call observer (CCE)
     * Not own.
     */
    MCCPConferenceCallObserver* iObserver;
    
    /**
     * Reference to CVccDirector's array of calls
     * Not own.
     */
    RVccCallArray& iCallArray;

    /**
     * Reference to CS plugin's conference object
     * Own.
     */
    MCCPConferenceCall* iConferenceCall;
    
    /**
     * Reference to CS plugin
     * Not own.
     */
    CConvergedCallProvider* iCsProvider;

    friend class T_CVccConferenceCall;
    };


#endif /*CVCCCONFERENCECALL_H*/
