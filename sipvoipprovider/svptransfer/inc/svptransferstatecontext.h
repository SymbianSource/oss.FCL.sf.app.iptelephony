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
* Description:  Transfer state context class for state machine logic
*
*/


#ifndef SVPTRANSFERSTATECONTEXT_H
#define SVPTRANSFERSTATECONTEXT_H

#include <e32base.h>
#include <badesca.h>                // CDesC8ArrayFlat
#include "svputdefs.h"

class CSVPController;
class CSVPSessionBase;
class CSVPTransferStateBase;        // iCurrentState
class MSVPTransferObserver;         // iTransferObserver    
class CMceSession;
class TMceTransactionDataContainer; // iContainer
class CMceRefer;                    // iMceRefer
class CMceEvent;                    // iMceEvent

typedef TInt TSVPTransferStateIndex;

const TInt KSVPTransferStateArraySize = 5;
const TInt KSVPHeaderArrayGranularity = 2;

// State indices.
const TSVPTransferStateIndex KSVPTransferIdleStateIndex         = 0;
const TSVPTransferStateIndex KSVPTransferPendingStateIndex      = 1;
const TSVPTransferStateIndex KSVPTransferAcceptedStateIndex     = 2;
const TSVPTransferStateIndex KSVPTransferTerminatingStateIndex  = 3;
const TSVPTransferStateIndex KSVPTransferTerminatedStateIndex   = 4;

// Error codes
const TInt KSVPErrTransferErrorBase = -2300;
const TInt KSVPErrTransferStateError = KSVPErrTransferErrorBase - 1;
const TInt KSVPErrTransferInProgress = KSVPErrTransferErrorBase - 2;
const TInt KSVPErrTransferReferCreate = KSVPErrTransferErrorBase - 3;
const TInt KSVPErrTransferApplyState = KSVPErrTransferErrorBase - 4;
const TInt KSVPErrTransferReferStatusCodeNotHandled = 
                                        KSVPErrTransferErrorBase - 5;
const TInt KSVPErrTransferParamRefer = KSVPErrTransferErrorBase - 6;
const TInt KSVPErrTransferReferTo = KSVPErrTransferErrorBase - 7;



/**
 *  Transfer state contexts.
 *
 *
 *  @lib svp.dll
 *  @since S60 3.2
 */
class CSVPTransferStateContext : public CBase
    {

public:

    /**
     * Two-phased constructor.
     */
    static CSVPTransferStateContext* NewL( 
                                CMceSession* aMceSession,
                                CSVPSessionBase* aSVPSession,
                                TMceTransactionDataContainer& aContainer, 
                                MSVPTransferObserver& aObserver );

    /**
     * Destructor.
     */
    virtual ~CSVPTransferStateContext();
    
    /**
     * Sets the current state for the context.
     * @since S60 3.2
     * @param aNewState New current state
     * @return Leaves with Error code if state transition is not
     *          acceptable.
     */
    void SetCurrentStateL( TSVPTransferStateIndex aNewState );

    /**
     * Returns the current state of the context
     * @since S60 3.2
     * @return TSVPTransferStateIndex Index to the current state.
     */
    TSVPTransferStateIndex CurrentState() const;

    /**
     * Applies the current state of the contex. This causes a
     * state transition to next state of the state machine. The 
     * current state sets the next state. Leaves on error
     * @since S60 3.2
     */
    void ApplyCurrentStateL();
               
    /**
     * Returns reference to the transfer observer.
     * @since S60 3.2
     * @return Reference to the transfer observer
     */    
    MSVPTransferObserver& TransferObserver();
       
    /**
    * Sets the reference to the sip (mce) session object
    * @param aSession The session object
    * @return None
    */
    void SetMceSessionObject( CMceSession* aSession );
                       
    /**
    * Returns the sip (mce) session object.
    * @since S60 3.2
    * @return The sip (mce) session object
    */
    CMceSession* MceSessionObject();
    
    /**
     * Sets the MCE refer object.
     * @since Series 60 3.2
     * @param aRefer New received refer
     */
    void SetMceRefer( CMceRefer* aRefer );
    
    /**
     * Returns pointer to the MCE refer object.
     * @since S60 3.2
     * @return Pointer to the MCE refer object
     */
    CMceRefer* MceRefer();

    /**
     * Check if attended or unattended transfer.
     * @since S60 3.2
     * @return Boolean true (=attended) or false
     */
    TBool IsAttended();

    /**
     * Check if session is on remote hold. If yes, this is Snom or EyeBeam
     * as unattended transferer case and we send OK notify
     * to proceed unattended transfer.
     * @since S60 3.2
     * @return TBool true if session is remote hold
     */
    TBool CheckIsSessionRemoteHold();

    /**
    * Set transfer data. Add Replaces and Referred-By headers.
    * Update transfer target url to sips: if secure status 1 or 2,
    * otherwise url updated to sip:
    * @since S60 3.2
    * @param aUserAgentHeaders On completion contains collected transfer headers.
    * @param aSecureStatus secure status
    */
    void SetTransferDataL( CDesC8Array* aUserAgentHeaders, TInt aSecureStatus );

    /**
     * Set attended/unattended.
     * @since S60 3.2
     * @param aAttended boolean true (=attended) or false
     */
    void SetAttended( const TBool aAttended );

    /**
     * Check if incoming transfer.
     * @since S60 3.2
     * @return Boolean true (=incoming) or false
     */
    TBool IsIncoming();

    /**
     * Set incoming "refer to" -string.
     * @since S60 3.2
     * @param aReferTo Reference to the string
     */    
    void SetIncomingReferToL( const TDesC8& aReferTo );

	/**
	* Sets the Referred-By data
	* @since S60 3.2
	* @param aReferredBy reference to the Referred-By data to store
	*/
    void SetIncomingReferredByL( const TDesC8& aReferredBy );

    /**
     * Return incoming "Referred By" -string.
     * @since S60 3.2
     * @return Reference to the string
     */    
    const TDesC& IncomingReferredBy();
    
    /**
     * Return incoming "refer to" -string.
     * @since S60 3.2
     * @return Reference to the string
     */    
    const TDesC& IncomingReferTo();

    /**
     * Reset/clear incoming "refer to" -string.
     * @since S60 3.2
     */    
    void ResetIncomingReferTo();       

    /**
     * Reset/clear incoming "Referred-By" -string.
     * @since S60 3.2
     */    
    void ResetIncomingReferredBy();       

    /**
     * Set incoming "Replaces" -string.
     * @since S60 3.2
     * @param aString Reference to the string
     */    
    void SetIncomingReplacesL( const TDesC8& aString );

    /**
     * Return incoming "Replaces" -string.
     * @since S60 3.2
     * @return Reference to the string
     */    
    const TDesC& IncomingReplaces();
    
    /**
     * Reset/clear incoming "Replaces" -string.
     * @since S60 3.2
     */    
    void ResetIncomingReplaces();       
    
    /**
     * Store the parameters for the mce out refer.
     * @since S60 3.2
     * @param aTargetSession target receiver of the transferred call, 
     * null if address is given. 
     * @param aTarget transfer target address, KNullDesC if call is given.
     * @param aAttendedTransfer boolean true if attended transfer.
     */
    void SetTransferParmsL( CSVPSessionBase* aTargetSession, 
                            const TDesC& aTarget,
                            const TBool aAttendedTransfer );

    /**
     * Execute the refer, creates mce out event.
     * @since S60 3.2
     * @return KErrNone if ok, SVP transfer error on failure
     */
    void ExecuteReferL();

    /**
     * Sets the MCE event object.
     * @since Series 60 3.2
     * @param aEvent New received event
     */
    void SetMceEvent( CMceEvent* aEvent );
    
    /**
     * Returns pointer to the MCE event object.
     * @since S60 3.2
     * @return Pointer to the MCE event object
     */
    CMceEvent* MceEvent();   
    
    /**
     * Stop the refer timer.
     * @since Series 60 3.2
     */
    void StopReferTimer();
    
    
protected:  // New functions
       
    /**
     * Creates the states belonging to this context
     * @since S60 3.2
     */
    void InitializeStateArrayL();

    /**
     * Check if state transition to a new state is acceptable.
     * @since S60 3.2
     * @param aStateIndex New state to enter
     * @return Boolean value, is the transition valid.
     */
    TBool IsStateTransitionAccepted( 
                            const TSVPTransferStateIndex aStateIndex );

    /**
     * C++ default constructor.
     */
    CSVPTransferStateContext( CMceSession* aMceSession,
                              CSVPSessionBase* aSVPSession,                              
                              TMceTransactionDataContainer& aContainer, 
                              MSVPTransferObserver& aObserver );
    
    /**
     * By default Symbian 2nd phase constructor is private. 
     * Now it is protected.
     */ 
    void ConstructL();

  
private: // New Functions

    /**
     * Set up headers for refer call.
     * @since S60 3.2
     * @return Pointer to the new created headers data     
     */
    CDesC8Array* SetupHeadersL();
    
    /**
     * Create mce out refer using target address.
     * @since S60 3.2
     * @param aTarget Address of the target. 
     */
    void CreateMceOutReferL( const TDesC& aTarget );

    /**
     * Create mce out refer using session.
     * @since S60 3.2
     * @param aTargetSession Pointer to the target session.      
     */
    void CreateMceOutReferL( CSVPSessionBase* aTargetSession );

    /**
    * Method for completing referTo
    * @since Series 60 3.2
    * @param aReferTo referto
    * @return HBufC8* constructed aReferTo
    */
    HBufC8* CompleteReferToL( const TDesC8& aReferTo );

    /**
    * Method for completing referred by
    * @since Series 60 3.2
    * @param aReferredBy Referred By
    * @return HBufC8* constructed aReferredBy
    */
    HBufC8* CompleteReferredByL( const TDesC8& aReferredBy );

    /**
    * Method for completing Replaces
    * @since Series 60 3.2
    * @param aString Replaces string
    * @return HBufC8* constructed Replaces string
    */
    HBufC8* CompleteReplacesL( const TDesC8& aString );

    /**
    * Checks, if the URI given as parameter has
    * the "<" character.
    * @since S60 3.2
    * @param aUri The URI to be checked, non-modifiable descriptor.
    * @return TInt, position of the character,
    *         KErrNotFound, if not found.
    */
    TInt CheckLeftBracket( const TDesC8& aUri ) const;

    /**
    * Removes the "<" character and all characters 
    * before the position from the URI given as parameter.
    * @since S60 3.2
    * @param aUri The URI to be altered, modifiable descriptor.
    * @param aPosition, position of string.
    * @return None
    */
    void RemoveLeftBracket( HBufC8*& aUri, TInt aPosition ) const;

    /**
    * Checks, if the URI given as parameter has
    * the ">" character.
    * @since S60 3.2
    * @param aUri The URI to be checked, non-modifiable descriptor.
    * @return TInt, position of the character,
    *         KErrNotFound, if not found.
    */
    TInt CheckRightBracket( const TDesC8& aUri ) const;

    /**
    * Removes the all characters from the URI after the position.
    * @since S60 3.2
    * @param aUri, the URI to be altered, modifiable descriptor.
    * @param aPosition, position where removing starts.
    * @return None
    */
    void CutStringFromPosition( HBufC8*& aUri, TInt aPosition ) const;

    /**
    * Checks, if the URI given as parameter has
    * the "?X-Sipx-Authidentity=" string.
    * Some server might add that to Refer-To header.
    * @since S60 3.2
    * @param aUri The URI to be checked, non-modifiable descriptor.
    * @return TInt, position of the character,
    *         KErrNotFound, if not found.
    */
    TInt CheckAuthidentity( const TDesC8& aUri ) const;

    /**
    * Checks, if the URI given as parameter has the "?Replaces=" text.
    * @since S60 3.2
    * @param aUri The URI to be checked, non-modifiable descriptor.
    * @return TInt, position of the chareacter,
    *         KErrNotFound, if not found.
    */
    TInt CheckReplacesTxt( const TDesC8& aUri ) const;

    /**
    * Checks, if the URI given as parameter has the "Referred-By:" text.
    * @since S60 3.2
    * @param aReferredBy The URI to be checked, non-modifiable descriptor.
    * @return TInt, position of the chareacter,
    *         KErrNotFound, if not found.
    */
    TInt CheckReferredByTxt( const TDesC8& aReferredBy ) const;

    /**
    * Removes the "Referred-By:" text
    * @since S60 3.2
    * @param aReferredBy The text to be altered, modifiable descriptor.
    * @param aPosition, position of string.
    * @return None
    */
    void RemoveReferredByTxt( HBufC8*& aReferredBy, TInt aPosition ) const;

    /**
    * Take the "?Replaces=" text and text after it.
    * @since S60 3.2
    * @param aUri The URI to be altered, modifiable descriptor.
    * @param aPosition, position of string.
    * @return None
    */
    void TakeReplacesTxt( HBufC8*& aString, TInt aPosition ) const;

    /**
    * Checks, if the URI given as parameter has
    * the ";user=phone" text.
    * @since S60 3.2
    * @param aUri The URI to be checked, non-modifiable descriptor.
    * @return TInt, position of the chareacter,
    *         KErrNotFound, if not found.
    */
    TInt CheckUserEqualsPhone( const TDesC8& aUri ) const;

    /**
    * Removes the ";user=phone" text and text after it.
    * @since S60 3.2
    * @param aUri The URI to be altered, modifiable descriptor.
    * @param aPosition, position of string.
    * @return None
    */
    void RemoveUserEqualsPhone( HBufC8*& aUri, TInt aPosition ) const;

    /**
    * Removes all extra parameters from given address
    * @since S60 3.2
    * @param aUri The URI to be altered, modifiable descriptor.
    * @return None
    */
    void RemoveExtraParameters( HBufC8*& aUri ) const;

    /**
     * Update transfer target url to sips: if secure status 1 or 2.
     * Otherwise url updated to sip:
     * @since S60 3.2
     * @param aSecureStatus secure status
     */    
    void UpdateTransferTargetL( TInt aSecureStatus );

private: // data
  
    /**
     * Pointer to the mce session object.
     * Not own.
     */    
    CMceSession* iMceSession;

    /**
     * Pointer to the SVP session object.
     * Not own.
     */    
    CSVPSessionBase* iSVPSession;

    /**
     * Pointer to the target of attended transfer.
     * Not own.
     */    
    CSVPSessionBase* iTargetSession;

    /**
     * Data container for mce session.
     */
    TMceTransactionDataContainer& iContainer;

    /**
     * Reference to the transfer observer.
     */    
    MSVPTransferObserver& iTransferObserver;     
   
    /**
     * Transfer state array.
     * Own.
     */
    RPointerArray< CSVPTransferStateBase >* iStates;

    /**
     * State object pointer to indicate current state.
     */
    CSVPTransferStateBase* iCurrentState;
   
    /**
     * Mce refer, either in (MT) or out (MO) type.
     * Own. 
     */
    CMceRefer* iMceRefer;   

    /**
     * Mce event, either in (MT) or out (MO) type.
     * Own. 
     */
    CMceEvent* iMceEvent;

    /**
     * Attended (consultative) / unattended (blind) transfer.
     */
    TBool iAttended;      
   
    /**
     * Incoming "refer to" parameter string
     * Indicates also the incoming refer handling.
     */    
    HBufC* iIncomingReferTo;
   
    /**
     * Incoming "Referred By" parameter string
     */    
    HBufC* iIncomingReferredBy;
    
    /**
     * Incoming "Replaces" parameter string
     */    
    HBufC* iIncomingReplaces;

private:

    // For testing
    SVP_UT_DEFS
    
    };

#endif // SVPTRANSFERSTATECONTEXT_H
