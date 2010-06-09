/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Session base class for SVP MO and MT sessions. Contains methods 
*                common for SVP MO and MT sessions.
*
*/


#ifndef SVPSESSIONBASE_H
#define SVPSESSIONBASE_H

#include <e32base.h>
#include <mccpcall.h>
#include <mccpssobserver.h>
#include <mccpdtmfobserver.h> 

#include "svpsessionobserver.h"
#include "svpholdobserver.h"
#include "svptransferobserver.h"
#include "svptimer.h"
#include "svptimerobserver.h"
#include "svppropertywatchobserver.h"
#include "svpvolumeupdateobserver.h"
#include "svpdtmfeventobserver.h"
#include "svputdefs.h"

// FORWARD DECLARATIONS
class TMceTransactionDataContainer;
class CSVPUtility;
class CSVPRtpObserver;
class CSVPHoldController;
class MCCPCallObserver;
class MCCPSsObserver;
class CSVPTransferController;
class CMceInRefer;
class CSVPVolumeObserver;
class CSVPPropertyWatch;
class CSVPDTMFEventGenerator;
class CMceSession;
class CMceMediaStream;
class CMceMediaSink;
class CMceMediaSource;
class CMceEvent;
class CMceRefer;
class CDesC8Array;

/**
 *  SVP Session base
 *
 *  Base class for SVP MT and MO sessions.
 *
 *  @lib svp.dll
 *  @since S60 3.2
 */
class CSVPSessionBase : public CBase,
                        public MCCPCall,
                        public MSVPTransferObserver,
                        public MSVPHoldObserver,
                        public MSVPTimerObserver,
                        public MSVPPropertyWatchObserver,
                        public MSVPVolumeUpdateObserver,
                        public MSVPDTMFEventObserver
                       
    {

public:
    
    /**
    * C++ default constructor.
    */
    CSVPSessionBase( TMceTransactionDataContainer& aContainer,
                     MSVPSessionObserver& aObserver, 
                     CSVPUtility& aSVPUtility,
                     CSVPRtpObserver& aRtpObserver );
    
    /**
    * Destructor.
    */
    virtual ~CSVPSessionBase();
    
public: // new functions    
    
    /**
     * Saves updated Mce session in cases where re-INVITE without
     * SDP is received
     * @Since S60 3.2
     * @param aUpdatedSession Session that is under update
     */
    void SetUpdatedSession( CMceInSession* aUpdatedSession );
    
    /**
    * Releases temporary secure session stored 
    * after secure session trial failed
    * @since Series 60 3.2
    */
    void ReleaseTempSecure();
    
    /**
    * Returns Mce session 
    * @Since S60 3.2
    * @param
    * @return instance of session
    */
    const CMceSession& Session() const;
    
    /**
    * Construcst audiostreams for MT and MO session
    * @since Series 60 3.2
    * @param None
    * @return 
    */   
    void ConstructAudioStreamsL();

    /**
    * Starts SVP timer
    * @since Series 60 3.2
    * @param aMilliSeconds
    * @param aTimerId
    * @return 
    */
    void StartTimerL( TInt aMilliSeconds, TInt aTimerId );

    /**
     * Stop specified SVP timer
     * @since Series 60 3.2
     * @param aTimerId The ID of the timer to stop.
     */
     void StopTimer( TInt aTimerId );
     
     /**
     * Stops started timers
     * @since Series 60 3.2
     * @return 
     */
     void StopTimers();
     
     /**
     * Returns correct timer value (RFC 3261 chapter 14.1)
     * @since Series 60 3.2
     * @return 
     */
     TInt ReinviteCrossoverTime();     
     
    /**
     * Sets observer for supplementary services events
     * @since Series 60 3.2
     * @return
     */ 
    void SetSsObserver( const MCCPSsObserver& aObserver );
    
    /**
     * Gets supplementary services events observer of session
     * @since Series 60 3.2
     * @return MCCPSsObserver supplementary services events observer
     */ 
    const MCCPSsObserver& GetSsObserver( );
    
    /**
    * Sets CCP DTMF observer to session
    * @since Series 60 3.2
    * @return 
    */
    void SetDtmfObserver( const MCCPDTMFObserver& aObserver );
    
    /**
    * Gets CCP DTMF observer 
    * @since Series 60 3.2
    * @return MCCPDTMFObserver CCP DTMF observer
    */
    const MCCPDTMFObserver& DtmfObserver();
    
    /**
    * Handles session state changes 
    * @since Series 60 3.2
    * @param aStatusCode Status code
    * @return 
    */
    void SessionStateChanged( TInt aStatusCode );

    /**
    * Handles session state changed announcement.
    * @since Series60 3.2
    * @param aSession MCE session
    * @return void
    */
    void HandleSessionStateChanged( CMceSession& aSession );
    
    /**
     * Handles the state change of a given stream.
     * @since S60 v3.2
     * @param aStream Stream whose state has changed.
     * @return void
     **/
    void HandleStreamStateChange( CMceMediaStream& aStream );
    
    /**
     * Handles the state change of a given stream & sink.
     * @since S60 v3.2
     * @param aStream Stream whose state has changed.
     * @param aSink Sink which is either Enabled or Disabled.
     * @return void
     **/
    void HandleStreamStateChange( CMceMediaStream& aStream,
            CMceMediaSink& aSink );
    
    /**
     * Handles the state change of a given stream & source
     * @since S60 v3.2
     * @param aStream Stream whose state has changed.
     * @param aSource Source which is either Enabled or Disabled.
     * @return void
     **/
    void HandleStreamStateChange( CMceMediaStream& aStream,
            CMceMediaSource& aSource );
    
    /**
    * Handles remote action after session is established.
    * @since Series60 3.2
    * @param None
    * @return
    */
    TInt IncomingRequest( CMceInSession& aUpdatedSession );
    
    /**
    * Returns Hold session 
    * @Since S60 3.2
    * @param None
    * @return Reference to the session hold controller
    */
    CSVPHoldController& HoldController() const;
    
    /**
    * Finds out if the session has hold controller
    * @since Series60 3.2
    * @param None
    * @return ETrue if session has hold controller, EFalse otherways
    */
    TBool HasHoldController() const;

    /**
     * Returns ETrue if session is muted
     * @since Series 60 3.2
     * @return TBool iMuted value
     */
    TBool IsSessionMuted() const;

    /**
    * Handles the event state changes.
    * @since Series 60 3.2
    * @param aEvent, the event that has changed.
    * @param aStatusCode, status code
	* @return KErrNone if succesfull, otherwise error code
    */
    TInt EventStateChanged( CMceEvent& aEvent,
				            TInt aStatusCode ); 

    /**
    * Handles the received notify.
    * @since Series 60 3.2
    * @param aEvent, the event that has changed.
    * @param aContainer, if present, holds details of received notify.
    */
    void NotifyReceived( CMceEvent& aEvent,
                            TMceTransactionDataContainer* aContainer );
				
    /**
    * Handles the refer state changes.
    * @since Series 60 3.2
    * @param aRefer, the refer that has changed.
    * @param aStatusCode, status code
    */
    void ReferStateChanged( CMceRefer& aRefer,
				            TInt aStatusCode ); 
    

    /**
    * Return the data container.
    * When session is the target session of the attended transfer.
    * @since Series 60 3.2
    * @return the mce data container
    */
    TMceTransactionDataContainer& MceTransactionDataContainer(); 
    
    /**
     * New incoming refer received.
     * @since Series 60 3.2
	 * @param aRefer, the new inbound refer. Ownership is
	 *	      transferred.
     * @param aReferTo SIP specific Refer-to header 
     * @param aContainer, if present, holds details of incoming refer.
     */
    void IncomingReferL( CMceInRefer* aRefer,
                        const TDesC8& aReferTo,
                        TMceTransactionDataContainer* aContainer );
    
    /**
     * Checks the MCE refer object.
     * @since S60 3.2
     * @param aRefer - refer to check
     * @return TBool true if same MCE refer object
     */
    TBool IsMceRefer( CMceRefer& aRefer ); 
    
    /**
     * Indicates whether call is secured or not
     * @since S60 3.2
     * @return TBool ETrue if call is secured
     */
    TBool SecureMandatory() const;
    
    /**
     * Is secure call prefered
     * @since S60 3.2
     * @return TBool ETrue is secure call is prefered
     */    
    TBool SecurePreferred() const;
    
    /**
     * Set secure mandatody flag true or false
     * @since S60 3.2
     * @param aSecMandatory flag value
     */   
    void SetSecureMandatory( TBool aSecMandatory );
    
    /**
     * Sets secure preferred flag true or false
     * @since S60 3.2
     * @param aSecPreferred flag value
     */    
    void SetSecurePreferred( TBool aSecPreferred );
    
    /**
     * Sets call event to be sent, used in secure and attended transfer cases.
     * @since S60 3.2
     * @param aCallEventToBeSent event value
     */   
    void SetCallEventToBeSent( MCCPCallObserver::TCCPCallEvent aCallEventToBeSent );

    /**
     * Checks if transfer is attended or unattended.
     * @since S60 3.2
     * @return TBool true if attended case
     */
    TBool IsAttended( ); 

    /**
    * The new recipient of the call can be fetched via this method.
    * @since S60 3.2
    * @param none
    * @return New recipient for the call after transfer
    */
    const TDesC& TransferTarget() const;

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
    * Send notify incoming transfer case.
	* @param aStatusCode code coming from mce
    * @since Series60 3.2
    */
    void SendNotify( TInt aStatusCode );
    
    /**
    * Finds out if the session has incoming transfer ongoing
    * @since Series60 3.2
    * @return ETrue if incoming transfer ongoing, EFalse otherways
    */
    TBool IsIncomingTransfer();
    
	/**
	* Sets the FromHeader data
	* @since S60 3.2
	* @param aFromHeader reference to the FromHeader data to store
	* @return KErrNone if succesfull, otherwise another system wide error code
	*/
    TInt SetFromHeader( const TDesC8& aFromHeader );

	/**
    * Returns the FromHeader data. 
	* @since S60 3.2
	* @return pointer to FromHeader data
	*/
    TDesC8* FromHeader();

	/**
	* Sets the ToHeader data
	* @since S60 3.2
	* @param aToHeader reference to the ToHeader data to store
	* @return KErrNone if succesfull, otherwise another system wide error code
	*/
    TInt SetToHeader( const TDesC8& aToHeader );

	/**
    * Returns the ToHeader data. 
	* @since S60 3.2
	* @return pointer to ToHeader data
	*/
    TDesC8* ToHeader();
    
	/**
	* Sets the CallId data
	* @since S60 3.2
	* @param aCallId reference to the CallId data to store
	* @return KErrNone if succesfull, otherwise another system wide error code
	*/
    TInt SetCallId( const TDesC8& aCallId );

	/**
	* Returns the CallId data. 
	* @since S60 3.2
	* @return pointer to CallId data
	*/
    TDesC8* CallId();    
    
    /**
	* Sets the CSeq header data
	* @since S60 3.2
	* @param aCSeq reference to the CSeq header data to store
	* @return KErrNone if succesfull, otherwise another system wide error code
	*/
    TInt SetCSeqHeader( const TDesC8& aCSeq );
    
    /**
    * Returns the CSeqHeader data. 
	* @since S60 3.2
	* @return reference to CSeqHeader data
	*/
    const TDesC& CSeqHeader() const;    
    
    /**
    * Method for setting crypto context to secure session
    * @since S60 3.2
    */
    void SetCryptoContextL();
  
	/**
    * Gets CCP session observer 
    * @since Series 60 3.2
    * @return CCP session observer.
    */
    MCCPCallObserver& GetCCPSessionObserver( );
    
    /**
    * Handles update failure 
    * @since Series 60 3.2
    * @param aSession MCE session reference
    * @param aStatusCode Status code of the response received
    * @return None
    */
    void UpdateFailed( CMceSession& aSession, TInt aStatusCode );
    
    /**
     * Returns session keepalive timer value
     * @since Series 60 3.2
     * @return TInt returns keepalive timer value
     */
    TInt GetKeepAliveTime();
    
    /**
     * SVP received INVITE without SDP, this flag is set to
     * prevent extra incomingcall callback to CCE
     * @since Series 60 3.2
     * @return none
     */
    void SetEmptyReInvite();
    
    /**
     * Returns ETrue if empty re-INVITE was received by SVP
     * @since Series 60 3.2
     * @return TBool Flag value
     */
    TBool IsEmptyReInvite();
    
    /**
     * Returns ETrue if iSessionState is still in initial state.
     * @since Series 60 3.2
     * @return TBool Flag value
     */
    TBool IsIdle();

    /**
     * Returns ETrue if anonymous call is on.
     * @since Series 60 9.2
     * @return TBool Flag value
     */
    TBool IsCLIROnL() const;

// dtmf

    /**
	* Cancels asynchronous DTMF string sending.
	* @since Series 60 3.2
	* @return KErrNone is successful, otherwiee antoher system wide error code
	*/
	TInt CancelDtmfStringSending();

	/**
	* Starts the transmission of a single DTMF tone across a
	* connected and active call.
	* @since S60 3.2
	* @param aTone Tone to be played.
	* @return KErrNone if succesfull, otherwise another system wide error code
	*/
	TInt StartDtmfTone( const TChar aTone );

	/**
	* Stops playing current DTMF tone.
	* @since S60 3.2
    * @param none
	* @return KErrNone if succesfull, otherwise another system wide error code
	*/
	TInt StopDtmfTone();

	/**
	* Plays DTMF string.
	* @since S60 3.2
	* @param aString String to be played.
	* @return KErrNone if succesfull, otherwise another system wide error code
	* KErrArgument if the specified string contains illegal DTMF characters
	*/
	TInt SendDtmfToneString( const TDesC& aString );  
    
	// from SVPDTMFEventObserver
    void InbandDtmfEventOccurred( TSVPDtmfEvent aEvent );
    
public: // from base classes: timerobserver
    
    /**
    * Notifies, that the previously set timer has now expired.
    * Implemented by the class using the timer.
    * @since Series 60 3.2
    * @param TInt aTimerId The ID of the timer. Observer can identify 
    * timers from each other with this ID.
    * @return 
    */
    void TimedOut( TInt aTimerId ); 

public: // from MSVPVolumeUpdateObserver
 
    /**
     * CSVPVolumeObserver calls this when volume level changes
     * changed. 
     *
     * @since S60 3.2
     * @param aVolume New volume level
     */
     void VolumeChanged( TInt aVolume );
    
public: // from base classes: MCCPCall
		
    /**
    * Return if call secure status is specified or not.
    * @since Series 60 5.0
    * @param None
    * @return TBool ETrue if security is specified, else EFalse.
    */
    TBool SecureSpecified() const;
	    
	/**
    * Sets CCP call observer 
    * @since Series 60 3.2
    * @param aObserver observer to be set
    * @return 
    */
    void AddObserverL( const MCCPCallObserver& aObserver );
    
    /**
    * Remove an observer.
    * @since S60 v3.2
    * @param none
    * @param aObserver Observer
    * @return KErrNone if removed succesfully. KErrNotFound if observer was not found.
    * Any other system error depending on the error.
    */
    TInt RemoveObserver( const MCCPCallObserver& aObserver );
        
    /**
    * Answers to an incoming call.
    * @since Series60 3.2
    * @return KErrNotSupported
    */
    virtual TInt Answer();

    /**
    * Starts dialing to recipient
    * @since Series60 3.2
    * @return KErrNone or system wide error code
    */
    virtual TInt Dial();

	/**
	* Cancels ongoing dial or answer request
	* @since Series60 3.2
	* @return KErrNone or system wide error code
	*/
	virtual TInt Cancel();

    /**
    * Ends an ongoing call.
    * @since Series60 3.2
    * @return KErrNone or system wide error code
    */
    virtual TInt HangUp();

    /**
    * Rejects an incoming call.
    * @since Series 60 3.2
    * @return KErrNone or system wide error code
    */
    virtual TInt Reject();

    /**
    * Reguests plug-in to queue the call. 
    * @since S60 3.2
    * @return KErrNone or system wide error code
    */
    virtual TInt Queue();

    /**
    * Call is ringning
    * @since S60 3.2
    * @return KErrNone or system wide error code 
    */
    virtual TInt Ringing();

    /**
    * Puts the specified call on hold.
    * @since Series60 3.2
    * @return KErrNone or system wide error code 
    */
    virtual TInt Hold();

    /**
    * Resumes the call.
    * @since Series60 3.2
    * @return KErrNone or system wide error code
    */
    virtual TInt Resume();
    
    /**
    * Swaps a connected call to its opposite state, either active or on hold
    * @since Series60 3.2
    * @return KErrNone or system wide error code 
    */
    virtual TInt Swap();

    /**
    * Releases all bindings to call and calls destructor
    * @since Series60 3.2
    * @return KErrNone  
    */
    virtual TInt Release();

    /**
    * Returns the recipient of the call.
    * @since Series60 3.2
    * @return recipient address,
    */
    virtual const TDesC& RemoteParty() const;

    /**
    * Returns display name of the recipient of the call.
    * @since Series60 3.2
    * @return Recipient's display name
    */
    virtual const TDesC& RemotePartyName();

    /**
    * Returns the original dialled party.
    * @since Series60 3.2
    * @return Dialled party,
    */
    virtual const TDesC& DialledParty() const;

	/**
    * Return boolean value, is incoming call forwarded or not.
    * @since Series60 3.2
    * @return ETrue if call is forwarded, else EFalse
    */
    virtual TBool IsCallForwarded() const;

    /**
    * Returns boolean value, is the call mobile originated.
    * @since Series60 3.2
    * @return ETrue if call is mobile originated, otherwise EFalse
    */
    virtual TBool IsMobileOriginated() const;

    /**
    * Returns the state of the call.
    * @since Series60 3.2
    * @return TCCPCallState The current state of the call
    */
    virtual MCCPCallObserver::TCCPCallState State() const;

    /**
    * Returns TUid of the Plug-In used for this call
    * @since Series60 3.2
    * @return TUid Implementation UID of the Plug-In
    */
    virtual TUid Uid() const;

    /**
    * Returns call control caps for this call This tells for CCE what call can
    * currently do.
    * @since Series60 3.2
    * @return Call control caps defined in MCCPCallObserver::TCCPCallControlCaps.
    */
    virtual MCCPCallObserver::TCCPCallControlCaps Caps() const;

    /**
    * Get call's secure status.
    * @since Series 60 3.2
    * @return TBool ETrue if security is enabled, else EFalse.
    */
    virtual TBool IsSecured() const;

    /**
	* Get Conference extension methods
	* @since Series 60 3.2
	* @return Pointer to MCCPCSCallConfernece if succesfull, NULL if not available
	*/
	virtual MCCPConferenceCall* ConferenceProviderL( const MCCPConferenceCallObserver& aObserver );
   
    /**
	* Get Forward provider
	* @since Series 60 3.2
	* @return Pointer to MCCPForwardProvider if succesfull, NULL if not available
	*/
	virtual MCCPForwardProvider* ForwardProviderL( const MCCPForwardObserver& aObserver );
	
	
	 /**
    * Get Call transfer provider
    * @since Series 60 3.2
    * @return Pointer to MCCPTransferProvider if succesfull, NULL if not available
    */
	MCCPTransferProvider* TransferProviderL( const MCCPTransferObserver& aObserver );
	
	
	/**
	* Set call parameters.
	* @since S60 3.2
	* @param aNewParams New call paramater information.
	*/
    void SetParameters( const CCCPCallParameters& aNewParams );

    /**
	* Cet call parameters.
	* @since S60 3.2
	* @param None
	* @return Current call paramater information.
	*/
    virtual const CCCPCallParameters& Parameters() const;

    /**
    * Returns currently used tone for the call. Used in case tones are needed to be played.
	* See defenition for TCCPTone.
    * @since S60 3.2
    * @param None.
    * @return TCCPTone Used tone.
    */
    TCCPTone Tone() const;

public: // from SVPHoldObserver

    /**
    * Notifies, that session has been holded.
    * @since Series 60 3.2
    */
    virtual void SessionLocallyHeld();

    /**
    * Notifies, that session has been resumed.
    * @since Series 60 3.2
    */
    virtual void SessionLocallyResumed();
    
    /**
    * Notifies, that session has been remotely held.
    * @since Series 60 3.2
    */
    virtual void SessionRemoteHeld();

    /**
    * Notifies, that session has been remotely resumed.
    * @since Series 60 3.2
    */
    virtual void SessionRemoteResumed();
    
    /**
    * Notifies, that hold request failed.
    * @since Series 60 3.2
    */
    virtual void HoldRequestFailed();
 
    /**
    * Notifies, that resume request failed.
    * @since Series 60 3.2
    */
    virtual void ResumeRequestFailed();

         
public: // from MSVPTransferObserver
         
    /**
    * From MSVPTransferObserver
    * Notifier for succesful transfer.
    * @since Series60 3.2
    * @param aNotifyCode The transfer notify code
    */
    virtual void TransferNotification( TInt aNotifyCode );

    /**
    * From MSVPTransferObserver
    * Notifier for transfer failure.
    * @since Series60 3.2
    * @param aError The error Id.    
    */
    virtual void TransferFailed( TInt aError );     
    
public: // from MSVPPropertyWatchObserver   
    
    /**
    * CSVPPropertyWatch calls this when some property under watching was
    * changed. 
    *
    * @since S60 3.2
    * @param aKey Property which was changed
    * @param aValue New value of the property
    */
    void ValueChangedL( TInt aKey, TInt aValue );
    
    /**
    * CSVPPropertyWatch calls this when property was deleted.
    *
    * @since S60 3.2
    * @param aKey Property which was deleted
    */
    void PropertyDeleted( TInt aKey );

protected: // new methods

    /**
     * Updates keepalive parameters for session
     * @since Series 60 3.2
     * @param aSession Session to be updated
     * @param aSessionUpdateOngoing Determines whether session needs to be updated
     * @return 
     */
    void UpdateKeepAliveL( CMceSession& aSession, TBool aSessionUpdateOngoing );

    /**
     * Worker function for session state change, called from
     * SessionStateChanged().
     * @since S60 v3.2
     * @param aOrigStatus Original status code
     * @param aError Mapped TCCPError
     * @param aModStatus Modified status code
     * @return void
     */
    virtual void SessionStateChangedL( TInt aOrigCode, TCCPError aError,
        TInt aModStatus );
        
    /**
     * Initializes mute P&S property watcher and CenRep volume watcher
     * @Since S60 3.2
     * @param
     * @return instance of session
     */
    void InitializePropertyWatchingL();
    
    /**
     * Sets RTP keep-alive timer value and payload
     * @since Series 60 3.2
     * @param aSession MCE session
     * @param aSessionUpdateOngoing Determines whether session needs to be updated
     * @return void
     */
    virtual void SetRtpKeepAliveL( CMceSession* aSession,
                                   TBool aSessionUpdateOngoing );
    
    /**
     * Executes session observer callback(Cb) if not NULL.
     * @since Series 60 3.2
     * @param aError the error code.
     * @return System wide error code.
     */
    TInt ExecCbErrorOccurred( TCCPError aError );
    
    /**
     * Sets new internal call state, and executes session observer callback(Cb)
     * if not NULL.
     * @since Series 60 3.2
     * @param aNewState new state
     * @return System wide error code.
     */
    TInt ExecCbCallStateChanged( MCCPCallObserver::TCCPCallState aNewState );
    
    /**
     * Executes session observer callback(Cb) if not NULL.
     * @since Series 60 3.2
     * @param aEvent the new event.
     * @return System wide error code.
     */
    TInt ExecCbCallEventOccurred( MCCPCallObserver::TCCPCallEvent aEvent );

    /**
     * Executes supplementary services(Ss) observer callback(Cb) if not NULL.
     * Events can be e.g. call forward related events
     * that are not call related to an existing call
     * @since Series 60 3.2
     * @param aEvent the new event.
     * @return System wide error code.
     */
    TInt ExecCbSsEventOccurred( MCCPSsObserver::TCCPSsCallForwardEvent aEvent );


private: // new methods
    
    /**
     * Method for checking audio priorities in audio streams. Method checks
     * that correct audio priorities are used in inband/outband DTMF cases and
     * sets them accordingly to uplink side codecs.
     * @since S60 v3.2
     * @param aAudioStreams. Media streams in iSession.
     * @return void
     */
    void CheckMmfPrioritiesForDtmfL(
        const RPointerArray<CMceMediaStream>& aAudioStreams ) const;
        
    /**
     * Method for seek remote party
     * Handle possible leave
     * @since Series 60 3.2
     * @return void
     * @leave system error if searching fails
     */
    void RemotePartyL() const;

    /**
     * Volume level changed
     * Handle possible leave
     * @since S60 3.2
     * @param aVolume New volume level
     * @return void
     * @leave system error if volume settings fails
     */
    void VolumeChangedL( TInt aVolume );
    
    /**
     * Performs mute / unmute
     * @since S60 3.2
     * @param aStreams Media streams
     * @param aValue Mute change value
     * @return void
     * @leave system error if mute/unmute fails
     */
    void PerformMuteChangeL( const RPointerArray< CMceMediaStream >& aStreams,
                             TInt aValue );

    /**
     * Sets Flag that indicates Error in UpLink (ICMP -3) happens the 1st Time
     * @since Series 60 3.2
     * @return none
     */
    void SetErrorInULandDLFirstTime( TBool aFirstTime );

    /**
     * Returns is the error in Uplink has happened the only 1st time
     * @since Series 60 3.2
     * @return TBool Flag value
     */
    TBool IsErrorInULandDLFirstTime();
    
    /**
     * Returns ETrue if both UpLink and DownLink are disabled
     * @since Series 60 3.2
     * @return TBool Flag value
     */
    TBool IsBothStreamsDisabledL() const;

    
protected: // protected data
    
    /**
     * Mce session
     * Own.
     */ 
    CMceSession* iSession;
    
    /**
     * Temporary secure session, needed so that old session deletion can be
     * performed painlessly.
     * Own.
     */ 
    CMceSession* iTempSecSession;
    
    /**
     * reference to SVP utility class
     */ 
    CSVPUtility& iSVPUtility;
    
    /**
     * reference to SVP Rtp observer class. Needed for RTCP resetting.
     */
    CSVPRtpObserver& iRtpObserver;
    
    /**
     * SIP profile id 
     */
    TUint32 iSipProfileId;
    
    /**
     * Converged call provider call observer
     */
    MCCPCallObserver* iCCPSessionObserver;
    
    /**
     * Observer for supplementary services events 
     */
    MCCPSsObserver* iCCPSsObserver;
    
    /**
     * Flag that indicates whether established session is secured or not
     */
    TBool iSecured;

    /**
     * Call event which will be sent in securepreferred or attended transfer cases.
     */
    MCCPCallObserver::TCCPCallEvent iCallEventToBeSent;

    /**
     * Id of VoIP profile
     */
    TUint32 iVoIPProfileId;
    
    /**
     * Keep-alive timer value
     */
    TInt iKeepAliveValue;
    
    /**
     * Call parameters
     */
    CCCPCallParameters* iCallParameters;
    
    /**
     * User tries to call to sips: -uri, secpref 0 is used
     * this flag is needed for event notification
     */
    TBool iTLSNotInUse;
    
    /**
     * Prevents UI notes if error response received to BYE request
     */
    TBool iAlreadyTerminating;
    
    /**
     * This flag indicates whether preconditions
	 * are required in SIP headers or not 
     */
    TBool iPreconditions;
    
private:    // private data
    
    /**
     * Transaction data container, not owned.
     */
    TMceTransactionDataContainer& iContainer;
    
    /*
     * Instance of DTMF event generator, used in inband dtmf case
     * Own.
     */
    CSVPDTMFEventGenerator* iEventGenerator;
    
    /**
     * Instance of volume observer, observer CenRep
     * Own.
     */
    CSVPVolumeObserver* iVolObserver;
    
    /**
     * Instance of property watcher using P&S system
     * Own.
     */
    CSVPPropertyWatch* iMutePropertyWatch;
    
    /**
     * Session timers, need an array since there migh be more than one.
     */
    RPointerArray<CSVPTimer> iTimers;
    
    /**
     * Contains current session state
     */
    MCCPCallObserver::TCCPCallState iSessionState;
    
    /**
     * From header of the mce session, from headers
     * Own.
     */  
    HBufC8* iFromHeader;
    
    /**
     * To header of the mce session, from headers
     * Own.
     */    
    HBufC8* iToHeader;
    
    /**
     * Call id of the mce session, from headers
     * Own.
     */
    HBufC8* iCallId;
    
    /**
     * CSeq header of the mce session, from headers
     * Own.
     */
    HBufC* iCSeqHeader;
    
    /**
     * The address of the recipient of the SIP session.
     * Own.
     */
    mutable HBufC* iRecipient;

    /**
     * Display name of the recipient of the SIP session.
     * Own.
     */
    HBufC* iDisplayName;
    
    /**
     * Pointer to a SVP hold controller.
     * Own.
     */
    CSVPHoldController* iHoldController;
    
    /**
     * Pointer to transfer controller.
     * Own.
     */    
    CSVPTransferController* iTransferController;
    
    /**
     * Terminating timeout time repeat control value when incoming transfer
     * ongoing.
     */ 
    TInt iTerminatingRepeat;
    
    /**
     * CCP DTMF observer
     */
    MCCPDTMFObserver* iCCPDtmfObserver;
    
    /**
     * SVP session observer
     */
    MSVPSessionObserver& iObserver;
    
    /**
     * Flag to indicate secure preference value 1 -> secure is preferred
     */
    TBool iSecurePreferred;
    
    /**
     * Flag to indicate secure preference value 2 -> secure call mandatory
     */
    TBool iSecureMandatory;
    
    /**
     * Mute flag
     */
    TBool iMuted;
    
    /**
    * Flag that indicates that handling of received re-INVITE 
    * without is SDP is ongoing
    */
    TBool iEmptyReInvite;
    
    /**
     * Tone information. Updated everytime session state changes.
     */
    TCCPTone iTone;
    
    /* 
     * Contains DTMF tone sent. Used with inband DTMF. 
     */  
    TChar iDtmfTone;
    
    /* 
     * Contains DTMF string sent. Used with inband DTMF. 
     */
    HBufC* iDtmfString;
    
    /* 
     * Keeps count of the current tone sent. Used with inband DTMF
     */
    TLex iDtmfLex;  

    /**
    * Flag that indicates that the case MediaStreams are 
    * Disabled (ICMP -3 error) happens 1st time
    */
    TBool iErrorInULandDLFirstTime;

    /**
     * Flag that indicates whether earlymedia event should 
	 * be sent again in ringing case
     */
    TBool iEarlyMediaOngoing;
    
private:
    
	// For testing
    SVP_UT_DEFS
    
    };

#endif // SVPSESSIONBASE_H
