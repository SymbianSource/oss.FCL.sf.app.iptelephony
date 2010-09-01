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
* Description:  Handles all common logic for SVP and handles sessions.
*
*/

#ifndef SVPCONTROLLER_H
#define SVPCONTROLLER_H

#include <mcemanager.h>
#include <mcesessionobserver.h>
#include <mcedtmfobserver.h>
#include <mceinsessionobserver.h>
#include <mceinreferobserver.h>
#include <mceeventobserver.h>
#include <mcestreamobserver.h>
#include <mcereferobserver.h>
#include <mcetransactiondatacontainer.h>

#include <cconvergedcallprovider.h>
#include <mccpobserver.h> 
#include <mccpssobserver.h>
#include <mccpdtmfprovider.h>

#include <wlanmgmtclient.h>

#include "svpsessionobserver.h"
#include "svpemergencysession.h"
#include "svpconsts.h"
#include "svputdefs.h"

#include "svpsupplementaryservices.h"
#include "cipappphoneutils.h"               // KSVPMaxTerminalTypeLength

// FORWARD DECLARATIONS
class CSVPRtpObserver;
class CWlanMgmtClient;

/**
 *  SVP controller
 *
 *  Handles all common logic for SVP and handles sessions.
 *
 *  @lib svp.dll
 *  @since S60 3.2 
 */
class CSVPController : public CConvergedCallProvider,
                       public MSVPSessionObserver, 
                       public MMceSessionObserver,
                       public MMceEventObserver,
                       public MMceReferObserver,
                       public MMceInSessionObserver,
                       public MMceInReferObserver,
                       public MMceStreamObserver,
                       public MMceDtmfObserver,
                       public MCCPDTMFProvider, 
                       public MSIPObserver,
                       public MSIPProfileRegistryObserver
    {

public: // new functions

    /**
    * Two-phased constructor.
    */
    static CSVPController* NewL();
  
    /**
    * Destructor.
    */
    virtual ~CSVPController();
   
public: // functions from base classes

    /**
    * From SVPSessionObserver. Removes session from session array
    * @since S60 3.2
    * @param aSession Session to be removed from session array
    */
    void RemoveFromArray( CSVPSessionBase &aSession );
    
    /**
    * From SVPSessionObserver. Handles session cleanup
    * when Mt session creation fails in early stages
    * @since S60 3.2
    * @param aSession Session to be removed
    */
    void TerminateSession( CMceInSession &aSession );

    /**
    * From MMceSessionObserver. The state of the session has changed.
    * @since Series 60 3.2
    * @param aSession, the session that has changed.
    * @param aContainer, if present, holds details of
    *        transaction causing state change.
    */
    void SessionStateChanged(
    			CMceSession& aSession,
    			TMceTransactionDataContainer* aContainer );

    /**
    * From MMceSessionObserver.The state of the connection used by the session has changed.
    * @since Series 60 3.2
    * @param aSession, the session that has changed.
    * @param aActive, ETrue if connection active, EFalse if connection inactive.
    */
    void SessionConnectionStateChanged(
                CMceSession& aSession,
                TBool aActive );
    
    /**
    * From MMceSessionObserver.An error has occurred concerning a specific session.
    * Note, that each error causes the session state to be ETerminated.
    * @since Series 60 3.2
    * @param aSession, The session raising the error.
    * @param aError, Error code
    */
	void Failed( CMceSession& aSession, TInt aError );

	/**
    * From MMceSessionObserver.An session update has been failed concerning a specific session.
    * Note, that each error causes the session state to be ETerminated.
    * @since Series 60 3.2
    * @param aSession, The session raising the error.
    * @param aContainer, if present, holds details of
    *        transaction causing update.
    */
	void UpdateFailed(
    			CMceSession& aSession,
    			TMceTransactionDataContainer* aContainer );
    			
     /**
    * From MMceEventObserver. The state of the event has changed.
    * @since Series 60 3.2
    * @param aEvent, the event that has changed.
    * @param aContainer, if present, holds details of
    *        transaction causing state change.
    */
    void EventStateChanged(
    			CMceEvent& aEvent,
				TMceTransactionDataContainer* aContainer );

	/**
	* From MMceEventObserver. The state of the event has changed.
	* @since Series 60 3.2
	* @param aEvent, event received notification.
    * @param aContainer, if present, holds details of
    *        transaction causing state change.
	*/
	void NotifyReceived(
				CMceEvent& aEvent,
				TMceTransactionDataContainer* aContainer );
				
				
    /**
    * From MMceEventObserver. The state of the event used by the refer has changed.
    * @since Series 60 3.2
    * @param aEvent, the event that has changed.
    * @param aActive, ETrue if connection active, EFalse if connection inactive.
    */
    void EventConnectionStateChanged(
                CMceEvent& aEvent,
                TBool aActive );
    
	/**
	* From MMceEventObserver. An error has occurred concerning a specific SIP event.
	* Note, that each error causes the event state to be ETerminated.
	* @param aEvent, The event raising the error.
	* @param aError, Error code
	*/
    void Failed(CMceEvent& aEvent, TInt aError );
	
	/**
    * From MMceInSessionObserver. Incoming session invitation. 
    * The CMceInSession is given to the
	* application as a callback.
    * @since Series 60 3.2
    * @param aSession, pointer to the incoming session. Ownership is
	*	     transferred.
    * @param aContainer, if present, holds details of
    *        transaction causing state change.
    */
	void IncomingSession(
                CMceInSession* aSession,
				TMceTransactionDataContainer* aContainer );
				
				
    /**
    * From MMceInSessionObserver.Incoming session update. 
    * The new updated CMceInSession is given to the
	* application as a callback.
    * @since Series 60 3.2
    * @param aOrigSession, the original session to be updated. 
    *        This instance cannot be used anymore, 
    *        all actions done using aUpdatedSession instance.
    * @param aUpdatedSession, pointer to the new updated session. Ownership is
	*	     transferred.
    * @param aContainer, if present, holds details of
    *        update transaction.
    */
	void IncomingUpdate(
				CMceSession& aOrigSession, 
				CMceInSession* aUpdatedSession,
				TMceTransactionDataContainer* aContainer );
				
	/**
    * From MMceInReferObserver. New incoming refer received.
	* @since Series 60 3.2
	* @param aRefer, the new inbound refer. Ownership is
	*		 transferred.
    * @param aReferTo SIP specific Refer-to header 
    * @param aContainer, if present, holds details of
    *        transaction causing state change.
    */
    void IncomingRefer(
				CMceInRefer* aRefer,
				const TDesC8& aReferTo,
				TMceTransactionDataContainer* aContainer );
				
	/**
    * From MMceStreamObserver. The state of the stream has changed.
    * @since Series 60 3.2
    * @param aStream, the stream that has changed.
    */
    void StreamStateChanged( 
                   CMceMediaStream& aStream);

    /**
    * From MMceStreamObserver. The state of the sink has changed.
    * @since Series 60 3.2
    * @param aStream, the stream that uses the sink.
    * @param aSink, the sink that has changed.
    */
    void StreamStateChanged(
                    CMceMediaStream& aStream,
                    CMceMediaSink& aSink );

    /**
    * From MMceStreamObserver. The state of the source has changed.
    * @since Series 60 3.2
    * @param aStream, the stream that uses the source.
    * @param aSource, the source that has changed.
    */
    void StreamStateChanged(
                    CMceMediaStream& aStream,
                    CMceMediaSource& aSource );
    
    
    /**
    * From MMceReferObserver. state of the refer has changed.
    * @param aRefer, the refer that has changed.
    * @param aContainer, if present, holds details of
    *        transaction causing state change.
    */
    void ReferStateChanged(
    			CMceRefer& aRefer,
				TMceTransactionDataContainer* aContainer );
		
    /**
    * From MMceReferObserver. The state of the connection used by the refer has changed.
    * @param aRefer, the refer that has changed.
    * @param aActive, ETrue if connection active, EFalse if connection inactive.
    */
    void ReferConnectionStateChanged(
                CMceRefer& aRefer,
                TBool aActive );
            
	/**
	* From MMceReferObserver. An error has occurred concerning a specific SIP refer.
	* Note, that each error causes the event state to be ETerminated.
	* @param aRefer, The refer raising the error.
	* @param aError, Error code
	*/
	void Failed( CMceRefer& aRefer, TInt aError );


// from MConvergedCallProvider 
	/**
	* Initializes plug-in. All required parameters must be passed
	* with this function in order to use services.
	* Simplifies pluging construct removing need to pass parameters to ECom interface
	* @since Series60 3.2
	* @param aMonitor CCP observer
	* @param aSsObserver SS observer
	*/
	void InitializeL( const TUint32 aServiceId,
					  const MCCPObserver& aObserver, 
					  const MCCPSsObserver& aSsObserver );
    
   	/**
    * Initializes plug-in. All required parameters must be passed
    * with this function in order to use services.
    * Simplifies pluging construct removing need to pass parameters to ECom interface
    * @since Series60 3.2
    * @param aMonitor CCP observer
    * @param aSsObserver SS observer
    */
    void InitializeL( const MCCPObserver& aMonitor,
                      const MCCPSsObserver& aSsObserver );

    /**
    * Creates a new session and checks secure status from profile. Leaves with
    * ECCPErrorNetworkBusy if not enough WLAN bandwith.
    * @since Series60 3.2
    * @param aParameters Call parameters, SVP uses Service ID parameter
    * @param aRecipient Recipients address/number
    * @param aObserver Observer
    * @return MCCPCall
    */
    MCCPCall* NewCallL( const CCCPCallParameters& aParameters,
					    const TDesC& aRecipient,
					    const MCCPCallObserver& aObserver );
                                
    /**
    * Returns Uid of plug-in which is on use.
    * @since Series60 3.2
    * @return Uid of plug-in
    */                        
    const TUid& Uid() const;
                           
    /**
    * Creates a new Emergency call and add user-agent header.
    * @since Series60 3.2
    * @param aServiceId Service ID, obsolete!
    * @param aAddress Emergency number
    * @param aObserver Observer
    * @return MCCPEmergencyCall
    */
    MCCPEmergencyCall* NewEmergencyCallL( const TUint32 aServiceId,
                                          const TDesC&  aAddress,
                                          const MCCPCallObserver& aObserver );
                                          
    /**
    * Creates a new conference session.
    * @since Series60 3.2
    * @param aObserver Observer
    * @return MCCPConferenceCall
    */
    MCCPConferenceCall* NewConferenceL( const TUint32 aServiceId,
                              const MCCPConferenceCallObserver& aObserver );
                             
    /**
    * Releases call
    * @since Series60 3.2
    * @param aCall Call to be removed
    * @return Symbian error code
    */
    TInt ReleaseCall( MCCPCall& aCall );

    /**
    * Releases emergency call
    * @since Series60 3.2
    * @param aCall Call to be removed
    * @return Symbian error code
    */
    TInt ReleaseEmergencyCall( MCCPEmergencyCall& aCall );

    /**
    * Releases conference call
    * @since Series60 3.2
    * @param aCall Call to be removed
    * @return Symbian error code
    */
    TInt ReleaseConferenceCall( MCCPConferenceCall& aCall );

    /**
    * Notifies Plug-In about transfer status.
    * @since Series 60 3.2
    * @param aAccept Boolean value is transfer accepted or not.
    * @return None
    */                      
    void AcceptTransfer( TBool aAccept );


    /**
    * Notification from UI to Multiple choices query
    * multiple choices response
    * @since Series 60 3.2
    * @param aIndex index of address where the call is to be forwarded
    * @return KErrNone if succesfull
    */  
    TInt ForwardCallToAddressL( const TInt aIndex );
    
    /**
    * Get Plug-In capabilities
    * @since Series 60 3.2
    * @return TUint32 On return contains the capability flags of plug-In
    */
    TUint32 Caps() const;
    
	/**
	* Get DTMF provider
	* @since Series 60 3.2
	* @param aObserver CCP Dtmf observer for informing events
	* @return Pointer to MCCPDTMFProvider if succesfull, NULL if not available
	*/
	MCCPDTMFProvider* DTMFProviderL( const MCCPDTMFObserver& aObserver );

	/**
	* Get extension provider
	* @since Series 60 3.2
    * @param aObserver observer for extension(custom) events
	* @return Pointer to MCCPExtensionProvider if succesfull, NULL if not available
	*/
	MCCPExtensionProvider* ExtensionProviderL( const MCCPExtensionObserver& aObserver );
    
    /**
    * Add an observer for DTMF related events.
    * Plug-in dependent feature if duplicates or more than one observers 
    * are allowed or not. Currently CCE will set only one observer.
    * @since S60 v3.2
    * @param aObserver Observer
    * @leave system error if observer adding fails
    */
    void AddObserverL( const MCCPDTMFObserver& aObserver );

    /**
    * Remove an observer.
    * @since S60 v3.2
    * @param aObserver Observer
    * @return KErrNone if removed succesfully. KErrNotFound if observer was not found.
    * Any other system error depending on the error.
    */
    TInt RemoveObserver( const MCCPDTMFObserver& aObserver );

    
// from MCCPDTMFProvider

    /**
	* Cancels asynchronous DTMF string sending.
	* @return KErrNone if successful, otherwise system wide error code
	* @since Series 60 3.2
	*/
	TInt CancelDtmfStringSending();

	/**
	* Starts the transmission of a single DTMF tone across a
	* connected and active call.
	* @since S60 3.2
	* @param aTone Tone to be played.
	* @return KErrNone if successful, otherwise another system wide error code
	*/
	TInt StartDtmfTone( const TChar aTone );

	/**
	* Stops playing current DTMF tone.
	* @since S60 3.2
    * @param none
	* @return KErrNone if successfull, otherwise another system wide error code
	*/
	TInt StopDtmfTone();

	/**
	* Plays DTMF string.
	* @since S60 3.2
	* @param aString String to be played.
	* @return KErrNone if successful, otherwise another system wide error code
	* KErrArgument if the specified string contains illegal DTMF characters
	*/
	TInt SendDtmfToneString( const TDesC& aString );

	/**
	* Continue or cancel sending DTMF string which was stopped with 'w'-character
	* in string.
	* @since S60 3.2
	* @param aContinue ETrue if sending of the DTMF string should continue,
	* EFalse if the rest of the DTMF string is to be discarded.
	* @return KErrNone if succesfull, otherwise another system wide error code
	*/
	TInt ContinueDtmfStringSending( const TBool aContinue );   
	
	
// from Mce DTMF observer
public: 

    /**
    * Incoming DTMF tone received.
    * @param aSession Session that tone was received.
    * @param aStream Stream that tone was received.
    * @param aTone Received tone.
    */
    void DtmfToneReceived( CMceSession& aSession,
                           CMceAudioStream& aStream,
                           const TChar& aTone );
                                                               
    /**
    * DTMF event received.
    * @param aSession Session that event was received.
    * @param aStream Stream that event was received.
    * @param aSource Source that event was received.
    * @param aEvent Received event.
    */
    void DtmfEventReceived( CMceSession& aSession,
                            CMceAudioStream& aStream,
                            CMceMediaSource& aSource,
                            TMceDtmfEvent aEvent );
                                  		
    /**
    * DTMF error occured. If error occurs, DTMFs becomes
    * unavailable for that source.
    * @param aSession Session that event was received.
    * @param aStream Stream that event was received.
    * @param aStream Stream that event was received.
    * @param aError Error code.
    */
    void DtmfErrorOccured( CMceSession& aSession,
                           CMceAudioStream& aStream,
                           CMceMediaSource& aSource,
                           TInt aError );


public: // methods from MSIPObserver

    /**
    * From MSIPObserver A SIP request has been received from the network. 
    * @since  Series 60 3.0
    * @param  aIapId The IapId from which the SIP request was received.  
    * @param  aTransaction Contains local address, remote address of a sip
    *         message, as well as optional SIP message method, headers and 
    *         body. The ownership is transferred.  
    */
     void IncomingRequest( TUint32 aIapId, 
                           CSIPServerTransaction* aTransaction );

    /**
    * From MSIPObserver The received SIP request time-outed and it has been
    * destroyed.  
    * @since  Series 60 3.0
    * @param  aSIPServerTransaction The time-outed transaction. Ownership is
    *         not transferred. 
    */
     void TimedOut( CSIPServerTransaction& aSIPServerTransaction );

     
public: // methods from MSIPProfileRegistryObserver

    /**
    * From MSIPProfileRegistryObserver An asynchronous error has occurred
    * related to SIP profile Event is send to those observers, who have the
    * corresponding profile instantiated. 
    * @since  Series 60 3.0
    * @param  aSIPProfileId SIP Profile ID. 
    * @param  aError Error code. 
    */ 
     void ProfileRegistryErrorOccurred( TUint32 aSIPProfileId,
                                        TInt aError );
    
    /**
    * From MSIPProfileRegistryObserver An event related to SIP Profile has
    * occurred.
    * @since  Series 60 3.0
    * @param  aProfileId SIP Profile ID. 
    * @param  aEvent An occurred event.
    */ 
    void ProfileRegistryEventOccurred( TUint32 aProfileId,
                                        TEvent aEvent );


private: // new methods
    
    /**
     * Checks min session expires offered
     * Responds with 422 if smaller than provisioned
     * @Since Series 60 3.2
     * @param aSession Incoming session
     * @param aHeaders Incoming session SIP headers
     * @param aMinSE Minimum session expires provisioned to phone
     */
    void CheckMinSessionExpiresL( CMceInSession& aSession, 
                                  CDesC8Array& aHeaders,
                                  TInt aMinSE );

    /**
     * Handles cases where re-INVITE is received without SDP
     * @Since Series 60 3.2
     * @param aSessionIndex Session that is under update
     * @param aUpdatedSession Updated session received from Mce
     */
    void IncomingUpdateNoSdpHandlerL( TInt aSessionIndex, 
                                      CMceInSession* aUpdatedSession );


    /**
     * Handles cases where re-INVITE is received with SDP
     * @Since Series 60 3.2
     * @param aSessionIndex Session that is under update
     * @param aOrigSession Session to be updated
     * @param aUpdatedSession Updated session received from Mce
     */
    void IncomingNormalUpdate( TInt aSessionIndex,
                               CMceSession& aOrigSession,
                               CMceInSession* aUpdatedSession );

    /**
    * Fetches expires time from incoming INVITE
    * @Since Series 60 3.2
    * @param aExpiresTime time in expires header
    */   
    void FetchExpiresTime( TUint32& aExpiresTime, CDesC8Array& aHeaders ) const;
    
    /**
    * Checks if state change call back reported error in session creation
    * Used only for secure session 
    * @since S60 3.2
    * @param aStatusCode Status code contains the response or system wide error code
    * @return TBool ETrue if faield, EFalse otherwise
    */
    TBool CheckIfSecureFailed( TInt aStatusCode ) const;

    /**
    * Method is called when normal Mo session needs to created
    * after secure session creation has failed 
    * @since S60 3.2
    * @param aSessionIndex Session which is to be created again non-securely
    */
    void CreateNonSecureSessionL( TInt aSessionIndex ); 

    /**
    * Creates new SVP out session
    * @since Series 60 3.2
    * @param aRecipient, Recipient address
    * @param aVoIPProfile, VoIP profile entry
    * @param aParameters, Call parameters
    * @param aObserver, Observer
    * @return MCCPCall 
    */
    MCCPCall* CreateNewSessionL( 
        TDesC8& aRecipient, 
        CRCSEProfileEntry& aVoipProfile, 
        const CCCPCallParameters& aParameters,
        const MCCPCallObserver& aObserver );
    
    /**
    * Method for matching sessions.
    * @since Series 60 3.2
    * @param aSession Session object
    * @return Index of session. Function calls panic if session is not found.
    */
    TInt FindSVPSession( const CMceSession& aSession ) const;
    
    /**
    * Handles incoming sessions 
    * @since Series 60 3.2
    * @param aSession Session object
    * @param aContainer In this case contains data concerning incoming session 
    * @return 
    */
    void IncomingSessionHandlerL( CMceInSession* aSession,
			                      TMceTransactionDataContainer* aContainer);
			                   
    /**
    * Handles incoming session updates in case when it is not hold case,
    * e.g. in codec renegotiation case
    * @param aOrigSession, the original session to be updated. 
    *        This instance cannot be used anymore, 
    *        all actions done using aUpdatedSession instance.
    * @param aUpdatedSession, the new updated session.
	* @return
    */
    void UpdateSessionL( CMceSession& aOrigSession,
                         CMceInSession& aUpdatedSession );

   	/**
    * Audio codecs are set in existing stream.
    * If there isn't existing stream, new stream will be created.
    * @since Series 60 3.2
    * @param aVoIPProfile VoIP profile entry
    * @param aMtSession MtSession object.
    * @return 
    */
    void CheckStreamsL( CRCSEProfileEntry& aVoipProfile, 
                        CMceSession& aMtSession,
                        TInt aKeepAliveValue,
                        TBool aSessionUpdateOngoing = EFalse );

    /**
    * Extract call id from user headers.
    * @since S60 3.2
    * @param aUserHeaders   User headers.
    * @param aCallId        On completion, contains call id.
    * @return ETrue if text Replaces: found, else EFalse
    * @leave KErrArgument if header doesn't include colonmarks.
    */
    TBool GetCallIdFromUserHeadersL( const CDesC8Array& aUserHeaders,
                                   TDes8& aCallId );

    /**
    * Check is incoming session transfer target case or normal incoming call.
    * @since S60 3.2
    * @param aHeaders Headers of incoming session. 
    */
    void IsTransferTargetCaseL( CDesC8Array* aHeaders );
    
    /**
     * Check Supported and Require headers if Provisional Response
     * ACKnowledgement (PRACK) method is supported for Preconditions.
     * @since S60 3.2
     * @param aHeaders Headers of incoming session
     * @return ETrue if Precondition is required and PRACK is supported, else EFalse
     */
    TBool IsPreconditionRequired( CDesC8Array& aHeaders );

    /**
    * Check and store data (FromHeader, ToHeader, CallId) from the headers
    * to the SessionBase, if CallId not already existing.
    * @since Series 60 3.2
    * @param aSVPSession
    * @param aContainer Contains data concerning changed session  
    */
    void CheckHeadersData( CSVPSessionBase* aSVPSession,
                           TMceTransactionDataContainer* aContainer );

    /**
    * Check and store contact data from the headers to the MoSession
    * @since Series 60 3.2
    * @param aSVPSession
    * @param aContainer Contains data concerning changed session  
    */
    TInt CheckContactData( CSVPSessionBase* aSVPSession, 
                                TMceTransactionDataContainer* aContainer );

    /**
    * Handles incoming refers 
    * @since Series 60 3.2
    * @param aRefer coming refer
    * @param aReferTo Refer-to header.
    * @param aContainer In this case contains data concerning incoming session 
    * @return void
    */
    void IncomingReferHandlerL( CMceInRefer* aRefer,
                                    const TDesC8& aReferTo,
                                    TMceTransactionDataContainer* aContainer);

    /**
    * Creates new SVP out session in transfer case
    * @since Series 60 3.2
    * @param aSessionIndex, SVP session index, receiver of refer.
    * @param aAttended, ETrue if attended EFalse if unattended
    */
    void CreateNewTransferSessionL( 
        TInt aSessionIndex,
        TBool aAttended );

    /**
    * Handles 3xx call forward events
    * @since Series 60 3.2
    * @param aStatusCode Call forward response code
    * @param aSessionIndex SVP session index
    * @param aContainer Contains data concerning changed session  
    */
    void HandleCallForward( TInt aStatusCode,
                            TInt aSessionIndex, 
                            TMceTransactionDataContainer* aContainer );
    
    /**
     * Finalizes the session creation. Adds DTMF observer to created session
     * and adds the session into the session array.
     * @since S60 v3.2
     * @param aSession Session whose construction to finalize.
     * @return void
     */
    void FinalizeSessionCreationL( CSVPSessionBase* aSession );
    
    /**
     * Executes 'ErrorOccurred' callback(Cb) to CCP Monitor if not NULL.
     * @since S60 v3.2
     * @param aError the error code
     * @return System wide error code.
     */
    TInt ExecCbErrorOccurred( MCCPObserver::TCCPError aError );
    
    /**
     * Executes 'IncomingCall' callback(Cb) to CCP Monitor if not NULL.
     * @since S60 v3.2
     * @param aCall Pointer to the newly created incoming call.
     * @return System wide error code.
     */
    TInt ExecCbIncomingCall( MCCPCall* aCall );
    
    /**
     * Executes 'IncomingCall' callback(Cb) to CCP Monitor if not NULL.
     * @since S60 v3.2
     * @param aCall Pointer to the newly created incoming call.
     * @param aTempCall Reference to the held call.
     * @return System wide error code.
     */
    TInt ExecCbIncomingCall( MCCPCall* aCall, MCCPCall& aTempCall );
    
    /**
     * Executes 'CallCreated' callback(Cb) to CCP Monitor if not NULL.
     * @since S60 v3.2
     * @param aNewTransferCall The call that has been created with the new transfer target.
     * @param aOriginator Pointer to originator of the transfer request.
     * @param aAttended Transfer requested was attented (ETrue) or un-attented(EFalse)
     * @return System wide error code.
     */
    TInt ExecCbCallCreated( MCCPCall* aNewTransferCall, MCCPCall* aOriginator, TBool aAttended );

    /**
    * Evaluates and removes the Dual-tone multi-frequency (DTMF) part from recipient's number.
    * Leaves with KErrArgument if invalid argument given.
    * @since S60 3.2
    * @param aRecipient The address/number to be evaluated, a non-modifiable descriptor.
    * @return Pointer to the modified recipient address/number heap descriptor. 
    */
    HBufC* ParseRecipientDtmfSuffixL( const TDesC& aRecipient ) const;

    /**
    * Checks that the given recipient address/number contains only valid DTMF characters.
    * Called from ParseRecipientDtmfSuffixL method.
    * Leaves with KErrArgument if invalid argument given.
    * @since S60 3.2
    * @param aRecipient The address/number to be checked, a non-modifiable descriptor.
    * @return ETrue when aRecipient contains only valid dtmf chars, EFalse if not.
    */
    TBool IsValidDtmfRecipientL( const TDesC& aRecipient ) const;

    /**
    * Checks what call event will be sent after attended transfer cases.
    * @since S60 3.2
    * @param aNewSVPSession The new established session.
    * @param aOldSVPSession The old transfered session.
    */
    void CSVPController::CheckCallEventToBeSent( CSVPSessionBase* aNewSVPSession, 
                                                 CSVPSessionBase* aOldSVPSession ) const;

private:

    /**
    * C++ default constructor.
    */
    CSVPController();
    
    /**
    * second-phase constructor
    */
    void ConstructL();
    
private: // data
    
    /**
     * pointer to Mce manager
     */
    CMceManager* iMceManager;
    
    /*
     * instance of SVP utility class.
     * own.
     */
    CSVPUtility* iSVPUtility;
    
    /**
     * transaction container, holds detailed data about transaction
     * TMceTransactionDataContainer* iTransactionDataContainer;
     */
    TMceTransactionDataContainer iContainer;
    
    /**
     * array of created SVP sessions
     */
    RPointerArray< CSVPSessionBase > iSessionArray;
    
    /**
     * pointer to created SVP emergency session
     */
    CSVPEmergencySession* iEmergencySession;
    
    /**
     * CCP observer
     */
    MCCPObserver* iCCPMonitor;
    
    /**
     * CCP Supplementary Services observer
     */
    MCCPSsObserver* iCCPSsObserver;
    
    /**
     * CCP session observer
     */ 
    MCCPCallObserver* iCCPSessionObserver;
    
    /**
     * CCP DTMF observer
     */
    const MCCPDTMFObserver* iCCPDtmfObserver;
    
    /**
     * Terminal type
     */
    TBuf< KSVPMaxTerminalTypeLength > iTerminalType;

    /**
     * WLAN MAC address
     */
    TBuf< KSVPWlanMacAddressLength > iWlanMacAddress;
    
    /**
     * Hold call index in attended transfer target case.
     */
    TInt32 iHoldCallIndex;
    
    /**
     * Original call index in transfer case.
     */
    TInt32 iIncomingReferCallIndex;
    
    /**
     * Untried profile IDs for emergency call
     */
    RArray<TUint32> iEmergencyProfileIds;
    
    /**
     * Untried IAP IDs for emergency call
     */
    RArray<TUint> iEmergencyIapIds;

    /**
     * Flag to indicate that Dtmf string sending is started
     * This is needed, because Mce provides only two Dtmf events
     */
    TBool iDtmfStringSending;
    
    /**
     * Flag to indicate first character of the Dtmf string is send
     * This is needed, because Mce provides only two Dtmf events
     */
    TBool iFirstDtmfSent;
    
    /**
     * Supplementary services.
     * Own.
     */
    CSVPSupplementaryServices* iSuppServices;
    
    /**
     * Flag for prevent wrong handling after SessionStateChanged -callback
     * in MT Re-Invite case that is not hold/resume. ETrue when e.g. codec
     * renegotiation ongoing, EFalse otherways
     */
    TBool iSessionUpdateOngoing;

    /**
     * Flag that indicates that handling of hold re-INVITE 
     * without is SDP is ongoing
     */
    TBool iEmptyReInvite;
    
    /**
     * This flag is set after Failed() callback is received to
     * prevent handling of subsequent state change callbacks
     * because session termination started when Failed() was received 
     */
    TBool iFailed;
    
    /**
     * RTP observer
     * Own
     */
    CSVPRtpObserver* iRtpObserver;
    
    /**
     * Contains single DTMF tone sent. Used with outband 
     */ 
    TChar iDtmfTone;
    
    /**
     * Contains DTMF string sent. Used with outband DTMF 
     */
    HBufC* iDtmfString;
    
    /** 
     * Keeps count of the current tone sent. Used with outband DTMF 
     */
    TLex iDtmfStringLex;
    
    /**
     * This flag is set ETrue if preconditions are required in SIP headers 
     */
    TBool iPreconditions;

    /**
     * WLAN management client used for creating virtual WLAN traffic streams. 
     * Own.
     */
    CWlanMgmtClient* iWlanMgmt;

    /**
     * ID of the created WLAN virtual traffic stream during an MO call.
     */
    TUint iTrafficStreamId;

    /**
     * This flag is used for stream creation/deletion logic during an MO call.
     */
    TBool iTrafficStreamCreated;
   
private:
    
    // For testing
    SVP_UT_DEFS
    
    };

#endif // CSVPTRANSFERCONTROLLER_H
