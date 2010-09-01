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
* Description:  Base class for hold contexts. Contains methods 
*                for creating the state array and controlling current state.
*
*/


#ifndef SVPHOLDCONTEXT_H
#define SVPHOLDCONTEXT_H

#include    <e32base.h>
#include    <mccpcallobserver.h>
#include    "svputdefs.h"

// Constants
const TInt KSVPHoldDirectionAttributeCount  = 4;    // "sendonly" etc.
const TInt KSVPMaxDirectionAttributeLength  = 10;   // "a=sendonly"
const TInt KSVPHoldStateArraySize           = 5;
const TInt KSVPHoldMaxLengthOfSDPMessage    = 1000;
const TInt KSVPHoldMaxLengthOfInetAddr      = 32;

// SDP Connection field content for oldway hold
_LIT8( KSVPHoldZeroAddr, "0.0.0.0" );

// SDP Direction attributes
_LIT8( KSVPHoldAttributeSendrecv, "a=sendrecv" );
_LIT8( KSVPHoldAttributeSendonly, "a=sendonly" );
_LIT8( KSVPHoldAttributeRecvonly, "a=recvonly" );
_LIT8( KSVPHoldAttributeInactive, "a=inactive" );

// Indexes for direction attributes
typedef TInt KSVPHoldAttributeIndex;
const KSVPHoldAttributeIndex KSVPHoldSendrecvIndex      = 0;
const KSVPHoldAttributeIndex KSVPHoldSendonlyIndex      = 1;
const KSVPHoldAttributeIndex KSVPHoldRecvonlyIndex      = 2;
const KSVPHoldAttributeIndex KSVPHoldInactiveIndex      = 3;

// State indices
typedef TInt TSVPHoldStateIndex;
const TSVPHoldStateIndex KSVPHoldConnectedStateIndex    = 0;
const TSVPHoldStateIndex KSVPHoldEstablishingStateIndex = 1;
const TSVPHoldStateIndex KSVPHoldOutStateIndex          = 2;
const TSVPHoldStateIndex KSVPHoldInStateIndex           = 3;
const TSVPHoldStateIndex KSVPHoldDHStateIndex           = 4;

/**
* Hold transition
*/
// session hold state, needed for dtmf for determining active session
enum TSVPSessionHoldState
    {
    ESVPConnected   = 0,
    ESVPEstablishing,
    ESVPOnHold
    };
    

/**
* Hold desired state change
*/    
enum TSVPHoldDesiredTransition
    {
    ESVPHoldNoRequest           = 0,
    ESVPHoldToHold              = 1,
    ESVPHoldToResume            = 2,
    ESVPHoldIncoming            = 3
    };

/**
* Hold request type
*/    
enum TSVPHoldRequestType
    {
    ESVPNoType                  = 0,
    ESVPLocalHold               = 1,
    ESVPLocalResume             = 2,
    ESVPLocalDoubleHold         = 3,
    ESVPLocalDoubleHoldResume   = 4,
    ESVPRemoteHold              = 5,
    ESVPRemoteResume            = 6,
    ESVPRemoteDoubleHold        = 7,
    ESVPRemoteDoubleHoldResume  = 8,
    ESVPRequestError            = 9 // causes state to resume previous
    };

/**
* Hold state
*/
enum TSVPHoldState
    {
    ESVPHoldConnected           = 0,
    ESVPHoldLocallyHolded       = 1,
    ESVPHoldRemoteHolded        = 2,
    ESVPHoldLocallyDoubleHolded = 3,
    ESVPHoldRemoteDoubleHolded  = 4
    };


class CSdpConnectionField;
class TInetAddr;
class CDesC8Array;
class MDesC8Array;
class CMceSession;
class TMceTransactionDataContainer;
class CSVPHoldStateBase;
class MSVPHoldObserver;
class CSVPHoldAttributeHandler;
class CSVPHoldMediaHandler;


/**
*  Base class for session contexts.
*
*  @lib svp.dll
*  @since Series 60 3.2
*/
class CSVPHoldContext : public CBase
    {
public:  // Constructors and destructor
    
    /**
    * Two-phased constructor.
    */
    static CSVPHoldContext* NewL(
                        CMceSession& aSession,
                        TMceTransactionDataContainer& aContainer,
                        MSVPHoldObserver* iObserver,
                        TBool aIsMobileOriginated );
    
    /**
    * Destructor.
    */
    virtual ~CSVPHoldContext();

public: // New functions
    
    /**
    * Sets the current state for the context.
    * Leaves with Error code if state transition is not acceptable.
    * @since Series 60 3.2
    * @param aContext Hold context
    * @param aNewState New current state
    */
    void SetCurrentStateL( CSVPHoldContext& aContext,
                           TSVPHoldStateIndex aNewState );

    /**
    * Returns the current state of the context
    * @since Series 60 3.2
    * @return TSVPHoldStateIndex the current state, index to it.
    */
    TSVPHoldStateIndex CurrentState() const;
    
    /**
    * Applies the current state of the contex. This causes a
    * state transition to next state of the state machine. The 
    * current state sets the next state. Leaves on error
    * @since Series 60 3.2
    * @param aSession Session to be modified
    * @param aTransition Desired operation
    */
    void ApplyCurrentStateL( CMceSession* aSession,
                             TSVPHoldDesiredTransition aTransition );

    /**
    * Applies the current state of the contex. This causes a
    * state transition to next state of the state machine. The 
    * current state sets the next state. Leaves on error
    * @since Series 60 3.2
    */
    void ApplyCurrentStateL();

    /**
    * Sets the reference to session object
    * state leaves.
    * @since Series 60 3.2
    * @param aSession The session object
    */
    void SetSessionObject( CMceSession* aSession );
    
    /**
    * Returns reference to session object
    * state leaves.
    * @since Series 60 3.2
    * @return Pointer to session object
    */
    CMceSession* SessionObject();
    
    /**
    * Returns type of hold request. This is valid during the complete
    * state transition process, until next stable state is reached.
    * @since Series 60 3.2
    * @return Hold request type
    */
    TSVPHoldRequestType HoldRequest();
    
    /**
    * Returns ETrue if hold/resume remote request.
    * @since Series 60 3.2
    * @return Boolean value
    */
    TBool IsRemoteRequest();
        
    /**
    * After completed & succesful IN / OUT Request, returns occurred event.
    * @since Series 60 3.2
    * @return Occurred event to be informed to client application
    */
    MCCPCallObserver::TCCPCallEvent HoldEvent();
    
    /**
    * Sets first attempt flag
    * @since Series 60 3.2
    * @param aValue Boolean value for -flag
    */
    void SetFirstAttempt( TBool aValue );
    
    /**
    * Returns value of first attempt -flag
    * @since Series 60 3.2
    * @return Boolean value of first attempt -flag
    */
    TBool FirstAttempt();
    
    /**
    * Sets remote request not processed -flag
    * @since Series 60 3.2
    * @param aValue Boolean value for flag
    */
    void SetRemoteRequestNotProcessed( TBool aValue );
    
    /**
    * Returns value of the remote request not processed -flag
    * @since Series 60 3.2
    * @return Boolean value of the remote request not processed -flag
    */
    TBool RemoteRequestNotProcessed();
            
    /**
    * Sets audio handling allowed -flag
    * @since Series 60 3.2
    * @param aValue Boolean value for flag
    */
    void SetAudioHandlingAllowed( TBool aValue );

    /**
    * Returns value of the audio handling allowed -flag
    * @since Series 60 3.2
    * @return Boolean value of the audio handling allowed -flag
    */
    TBool AudioHandlingAllowed();
    
    /**
    * Returns reference to hold observer.
    * @since Series 60 3.2
    * @return Reference to hold observer.
    */
    MSVPHoldObserver& HoldObserver();
        
    /**
    * Returns reference to attribute handler.
    * @since Series 60 3.2
    * @return Reference to attribute handler.
    */
    CSVPHoldAttributeHandler& AttributeHandler();
    
    /**
    * Returns reference to media handler.
    * @since Series 60 3.2
    * @return Reference to media handler.
    */
    CSVPHoldMediaHandler& MediaHandler();
    
    /**
    * Solves type of incoming request and returns next state of state machine.
    * @since Series 60 3.2
    * @param aSession Incoming updated MCE session
    * @param aAttributeLines Attribute lines from incoming request.
    * @param aCheckOldwayHold ETrue if oldwayhold need to be taken into account
    * @return TSVPHoldRequestType Request type
    */
    TSVPHoldRequestType SolveRequestL( CMceSession& aSession,
                                       MDesC8Array* aAttributeLines,
                                       TBool aCheckOldwayHold = EFalse );
                                       
    /**
    * Sets response status code
    * @since Series 60 3.2
    * @param aStatusCode Status code of response received
    */
    void SetResponseStatusCode( TInt aStatusCode );
    
    /**
    * Gets response status code
    * @since Series 60 3.2
    * @return Status code
    */
    TInt ResponseStatusCode();
    
    /**
    * Sets force hold flag value
    * @since Series 60 3.2
    * @param aForceHold Boolean value
    */
    void SetForceHold( TBool aForceHold );
    
    /**
    * Gets force hold flag value
    * @since Series 60 3.2
    * @return Boolean value
    */
    TBool ForceHoldOccured();
    
    /**
    * Handles and controls hold operation based on response status
    * @since Series 60 3.2
    * @param aNextState Valid hold state machine state after function call
    * @return Boolean value ETrue if response needed special handling
    */
    TBool SpecialResponseHandling( TSVPHoldStateIndex& aNextState );
    
    /**
    * Sets hold/resume request timed out -flag,
    * called when hold/resume request has expired.
    * @since Series 60 3.2
    */
    void TimedOut();
    
    /**
    * Returns RollBack -boolean value
    * @since Series 60 3.2
    * @return Boolean value
    */
    TBool HoldRolledBack();
    
    /**
    * Sets crossover situation on/off
    * @since Series 60 3.2
    * @param ETrue if CrossOver situation, EFalse otherways
    * @return
    */
    void CrossOver( TBool aActive );

    /**
    * Returns Hold failed -boolean value. After this function call
    * flag is cleared.
    * @since Series 60 3.2
    * @return ETrue if hold failed, EFalse otherways
    */
    TBool HoldFailed();

    /**
    * Returns Resume failed -boolean value
    * @since Series 60 3.2
    * @return ETrue if resume failed, EFalse otherways
    */
    TBool ResumeFailed();
    
    /**
    * Returns IsMobileOriginated -boolean value
    * @since Series 60 3.2
    * @return ETrue if MobileOriginated, EFalse otherways
    */
    TBool IsMobileOriginated();

    /**
    * Sets mute state to Hold Controller
    * @since Series 60 3.2
    * @param aMuted Boolean value for mute value
    */
    void Muted( TBool aMuted );

    /**
    * Returns mute state
    * @since Series 60 3.2
    * @return Boolean value for mute value
    */
    TBool Muted();
    
    /**
    * Sets CallRequestFailed status ETrue/EFalse
    * @since Series 60 3.2
    * @param aCallRequest Boolean value for call on hold request failed
    */
    void SetCallRequestFailed( TBool aCallRequest );
    

    /**
    * Returns CallRequestFailed -boolean value
    * @since Series 60 3.2
    * @return ETrue if call on hold request failed, EFalse otherways
    */
    TBool CallRequestFailed();
    
    
protected:  // New functions

    /**
    * Creates the states belonging to this context
    * @since Series 60 3.2
    */
    void InitializeStateArrayL();

    /**
    * Checkes if a state transition to a given state is accepted.
    * @since Series 60 3.2
    * @param TSVPHoldStateIndex aStateIndex
    * @return TBool, boolean value, is the transition valid.
    */
    TBool IsStateTransitionAccepted( 
                        const TSVPHoldStateIndex aStateIndex );

    /**
    * C++ default constructor.
    */
    CSVPHoldContext( TMceTransactionDataContainer& aContainer,
                     TBool aIsMobileOriginated );

    /**
    * By default Symbian 2nd phase constructor is private. 
    * Now it is protected.
    */
    void ConstructL( CMceSession& aSession,
                     MSVPHoldObserver* iObserver );


private:     // New functions
    
    /**
    * Returns ETrue if establishing state is next, EFalse otherways
    */
    TBool ToEstablishing();
    
    /**
    * Returns hold request type
    */
    TSVPHoldRequestType RequestType( 
            TSVPHoldDesiredTransition aTransition );
            
    /**
    * Returns hold request type
    */
    TSVPHoldRequestType RequestToHold();
    
    /**
    * Returns hold request type
    */
    TSVPHoldRequestType RequestToResume();
    
    /**
    * Returns hold request type
    */
    TSVPHoldRequestType RequestIncoming();
                         
    /**
    * Updates right establishing state based on the direction of the request
    */
    void UpdateEstablishingStateL();
    
    /**
    * 
    */
    TSVPHoldRequestType SolveRequestType( 
            TInt aAttributeIndex,
            TSVPHoldRequestType aPresumedRequest );
                                                               
    /**
    * This method is used in mobile terminating(MT) calls, to check SDP
    * body for Connection field, and to check the content of that field.
    *
    * @since    Series60 3.2
    * @param    aSession Incoming updated MCE session
    * @param    aPresumedRequest Enumeration for hold request type,
    *                            assumed hold type passed as parameter.
    * @return                    Enumerated value of hold request, after
    *                            oldway hold is checked from the content.
    */
    TSVPHoldRequestType 
    CheckOldwayHoldL( CMceSession& session,
                      TSVPHoldRequestType aPresumedRequest );
    
    /**
    * Rolls hold session state back to previous state
    */
    void RollBack( TSVPHoldStateIndex& aNextState );

        
protected:  // Data

    // State array
    RPointerArray< CSVPHoldStateBase >* iStates;

    // Observer
    MSVPHoldObserver* iObserver;
    

private:    // Data

    // Current state
    CSVPHoldStateBase* iCurrentState;
    
    // Current hold request type. Changed by SolveRequestType() and
    // in special cases, e.g. when 2-phased hold needed (MO oldway)
    TSVPHoldRequestType iHoldRequest;
    
    // Pointer to session object (not owned)
    CMceSession* iSession;
    
    // Boolean value of request direction
    TBool iOutgoing;
    
    // Instance of attribute handler
    CSVPHoldAttributeHandler* iAttributeHandler;
    
    // Instance of media handler
    CSVPHoldMediaHandler* iMediaHandler;
    
    // First attempt (not MO oldway hold)
    TBool iFirstAttempt;
    
    // Flag for remote request in process
    TBool iRemoteRequestNotProcessed;
    
    // Data container for mce session
    TMceTransactionDataContainer& iContainer;
    
    // Hold event
    MCCPCallObserver::TCCPCallEvent iHoldEvent;
    
    // Indicates that audio handling is allowed
    TBool iAllowAudioHandling;
    
    // Status code of the response
    TInt iResponseStatusCode;
    
    // Force hold occured flag;
    TBool iForceHoldOccured;
    
    // Flag for timed out hold/resume request
    TBool iTimedOut;
    
    // Flag for preventing audio enable/disable
    TBool iRollBack;
    
    // Flag for failed hold
    TBool iHoldFail;
    
    // Flag for failed resume (causes session to be terminated)
    TBool iResumeFail;
    
    // Flag for SVP session direction
    TBool iIsMobileOriginated;
    
    // Flag for mute
    TBool iMuted;
    
    // Flag for cross over (simultaneous requests)
    TBool iCrossOver;
    
    // Flag for callbackrequest
    TBool iCallRequest;


private:
    
    // For testing
    SVP_UT_DEFS
    
    };

#endif      // SVPHOLDCONTEXT_H 
            
