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
* Description:  Class representing the emergency session
*
*/



#ifndef SVPEMERGENCYSESSION_H
#define SVPEMERGENCYSESSION_H

#include <sipprofileregistryobserver.h>
#include <sipobserver.h>

#include <mccpemergencycall.h>
#include <mccpcallobserver.h>
#include <ccpdefs.h>    // error definitions
#include <mceoutsession.h>

#include "svplogger.h"
#include "svptimer.h"
#include "svptimerobserver.h"
#include "svppositioningproviderobserver.h"
#include "svpemergencyconnectionobserver.h"
#include "svpholdobserver.h"
#include "svpdtmfeventobserver.h"
#include "svputdefs.h"

class MCCPCallObserver;
class CSVPUtility;
class CSIP;
class CSIPProfileRegistry;
class CRCSEProfileEntry;
class CSVPPositioningProvider;
class CSVPEmergencyConnection;
class TMceTransactionDataContainer;
class CSVPHoldController;
class MCCPDTMFObserver;
class CSVPDTMFEventGenerator;


/**
 *  Class that handles emergency sessions
 *
 *  @lib svp.dll
 *  @since S60 3.2
 */
class CSVPEmergencySession : public CBase,
                             public MCCPEmergencyCall, 
                             public MSIPObserver,
                             public MSIPProfileRegistryObserver, 
                             public MSVPTimerObserver, 
                             public MSVPPositioningProviderObserver, 
                             public MSVPEmergencyConnectionObserver, 
                             public MSVPHoldObserver, 
                             public MSVPDTMFEventObserver
    {

public:
    
    /**
    * Two-phased constructor.
    */
    static CSVPEmergencySession* NewL( CMceManager& aMceManager, 
                                       TUint32 aVoipProfileId, 
                                       const TDesC& aAddress,
                                       const MCCPCallObserver& aObserver,
                                       CSVPUtility& aSVPUtility, 
                                       TBool aIsLastProfile, 
                                       TBool aIsDummy = EFalse );

    /**
    * Two-phased constructor for creating IAP session.
    */
    static CSVPEmergencySession* NewL( CMceManager& aMceManager, 
                                       const TDesC& aAddress,
                                       const MCCPCallObserver& aObserver,
                                       CSVPUtility& aSVPUtility, 
                                       TUint32 aIapId, 
                                       TBool aIsLastProfile, 
                                       TBool aIsDummy = EFalse );

    /**
    * Destructor.
    */
    virtual ~CSVPEmergencySession();

    /**
	* Returns CCP call state
	* @since S60 3.2
	* @return CCP call state
	*/
    MCCPCallObserver::TCCPCallState State() const;
    
    /**
	* Traps SessionStateChangedL
    * @since S60 3.2
	* @param aStatusCode Status code to be handled
	*/
    void SessionStateChanged( TInt aStatusCode );
    
    /**
	* Session connection state changed
	* @since S60 3.2
	* @param aSession SIP session
	* @param aActive  SIP connection state, ETrue if up, EFalse otherwise
	*/
    void SessionConnectionStateChanged( 
        CMceSession& aSession, TBool aActive );

    /**
	* Handles stream state changes
    * @since S60 3.2
	* @param aStream Stream that has changed
	*/
    void StreamStateChanged( CMceMediaStream& aStream );

    /**
    * Handles remote action after session is established, i.e. hold
    * @since Series60 3.2
    * @param aUpdatedSession
    * @param aContainer MCE transaction container
    */
    void IncomingRequestL( CMceInSession* aUpdatedSession, 
                           TMceTransactionDataContainer aContainer );

    /**
    * Returns ETrue, if hold controller exists
    * @since Series60 3.2
    * @return ETrue if hold controller exists
    */
    TBool HasHoldController() const;

    /**
    * Returns reference to hold controller
    * @since Series60 3.2
    * @return ETrue Refenrence to hold controller
    */    
    CSVPHoldController& HoldController() const;

    /**
    * Handles hold session state changes
    * @since Series60 3.2
    * @param aSession MCE session
    */    
    void HoldSessionStateChangedL( CMceSession& aSession );

    /**
    * Gets CCP DTMF observer 
    * @since Series 60 3.2
    * @return MCCPDTMFObserver CCP DTMF observer
    */
    const MCCPDTMFObserver& DtmfObserver();

    /**
    * Sets CCP DTMF observer to session
    * @since Series 60 3.2
    * @param aObserver Observer
    */
    void SetDtmfObserver( const MCCPDTMFObserver& aObserver );

	/**
	* Starts the transmission of a single DTMF tone across a
	* connected and active call.
	* @since S60 3.2
	* @param aTone Tone to be played.
	* @return Error code
	*/
	TInt StartDtmfTone( const TChar aTone );

	/**
	* Stops playing current DTMF tone.
	* @since S60 3.2
	* @return Error code
	*/
	TInt StopDtmfTone();

	/**
	* Plays DTMF string.
	* @since S60 3.2
	* @param aString String to be played.
	* @return Error code
	*/
	TInt SendDtmfToneString( const TDesC& aString );  

    /**
	* Cancels asynchronous DTMF string sending.
	* @since Series 60 3.2
	* @return Error code
	*/
	TInt CancelDtmfStringSending();


// from base class MCCPEmergencyCall

 	/**
	* Creates an emergency call
	* @since S60 3.2
	* @param aRecipient, not used
    * @return KErrNone or system wide error code
	*/
	TInt Dial( const TDesC& aRecipient );

	/**
	* Cancels ongoing dial request
	* @since S60 3.2
	* @return KErrNone or system wide error code
	*/
	TInt Cancel();

	/**
	* Ends an ongoing call
	* @since S60 3.2
	* @return KErrNone or system wide error code
	*/
	TInt HangUp();

	/**
	* Answers to an incoming call.
	* @since S60 3.2
	* @return KErrNone or system wide error code
	*/
	TInt Answer();
	

// from base class MSVPTimerObserver
    
    /**
    * Notifies, that the previously set timer has now expired.
    * Implemented by the class using the timer.
    * @since Series 60 3.2
    * @param aTimerId The ID of the timer. Observer can identify 
    *        timers from each other with this ID.
    */
    void TimedOut( TInt aTimerId );


// from base class MSIPObserver

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

     
// from base class MSIPProfileRegistryObserver

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


// from base class MSVPPositioningProviderObserver
    
    /**
     * Handles successful positioning requests
     *
     * @since S60 3.2
     * @param aPosition Acquired position information
     */
    void PositioningRequestComplete( const TDesC8& aPosition );
    
    /**
     * Handles failed positioning requests
     *
     * @since S60 3.2
     * @param aError Symbian error code
     */
    void PositioningErrorOccurred( TInt aError );


// from base class MSVPEmergencyConnectionObserver

    /**
     * SNAP connection is established
     *
     * @since S60 3.2
     */
    void SnapConnected();
    
    /**
     * IAP connection is established
     *
     * @since S60 3.2
     */    
    void Connected();

    /**
     * SIP proxy address is ready
     *
     * @since S60 3.2
     * @param aAddress SIP proxy address
     */    
    void SipProxyAddressReady( const TDesC16& aAddress );

    /**
     * Connection error has occurred
     *
     * @since S60 3.2
     * @param aError Symbian error code
     */    
    void ConnectionError( TInt aError );


// from base class MSVPHoldObserver

    /**
    * Notifies, that session has been holded.
    * @since Series 60 3.2
    */
    void SessionLocallyHeld();

    /**
    * Notifies, that session has been resumed.
    * @since Series 60 3.2
    */
    void SessionLocallyResumed();
    
    /**
    * Notifies, that session has been remotely held.
    * @since Series 60 3.2
    */
    void SessionRemoteHeld();

    /**
    * Notifies, that session has been remotely resumed.
    * @since Series 60 3.2
    */
    void SessionRemoteResumed();
    
    /**
    * Notifies, that hold request failed.
    * @since Series 60 3.2
    */
    void HoldRequestFailed();

    /**
    * Notifies, that resume request failed.
    * @since Series 60 3.2
    */
    void ResumeRequestFailed();


// from base class MSVPDTMFEventObserver

    /**
    * Sends DTMF events to CCE
    * @since S60 v3.2
    * @param aEvent DTMF event
    */
    void InbandDtmfEventOccurred( TSVPDtmfEvent aEvent );


private:

    /**
    * C++ default constructor.
    */
    CSVPEmergencySession( CSVPUtility& aSVPUtility, 
                          CMceManager& aMceManager, 
                          TBool aIsLastProfile, 
                          TBool aIsDummy );

    /**
    * second-phase constructor
    */
    void ConstructL( TUint32 aVoipProfileId, 
                     const TDesC& aAddress,
                     const MCCPCallObserver& aObserver, 
                     TBool aIsDummy );

    /**
    * Second-phase constructor for IAP session
    */
    void ConstructWithIapIdL( TUint32 aIapId, 
                              const TDesC& aAddress,
                              const MCCPCallObserver& aObserver, 
                              TBool aIsDummy );

    /**
	* Continues IAP EC construction
    * @since S60 3.2
	* @param aSipProxyAddress SIP proxy address
	*/
    void ContinueConstructWithIapIdL( const TDesC16& aSipProxyAddress );

 	/**
	* Deletes failed MCE session and creates a new one without LI. Used when 
	* 415 Unsupported Media Type received.
	* @since S60 3.2
	*/    
    void CreateNonLISessionL();

 	/**
 	* Creates unregistered MCE session
	* @since S60 3.2
	*/    
    void CreateUnregistedSessionL();

    /**
	* Handles emergency session state change
    * @since S60 3.2
	* @param aStatusCode Status code to be handled
	*/
    void SessionStateChangedL( TInt aMceStatusCode );

 	/**
	* Determines if dialing can be proceeded
	* @since S60 3.2
	*/
    void ProceedDial();    

 	/**
	* Establishes an emergency call
	* @since S60 3.2
	*/
    void DialL();

    /**
    * Constructs audiostreams for emergency session
    * @since Series 60 3.2
    */   
    void ConstructAudioStreamsL();

    /**
    * Sets keep-alive value
    * @since Series 60 3.2
    */   
    void SetKeepAlive();

    /**
    * Updates keepalive parameters for MCE session
    * @since Series 60 3.2 
    */
    void UpdateKeepAliveL();
    
    /**
     * Method for checking audio priorities in audio streams. Method checks
     * that correct audio priorities are used in inband/outband DTMF cases and
     * sets them accordingly to uplink side codecs.
     * @since S60 3.2
     * @param aAudioStreams Media streams of MCE session
     */
    void CheckMmfPrioritiesForDtmfL( 
        const RPointerArray<CMceMediaStream>& aAudioStreams ) const;

    /**
    * Starts SVP timer
    * @since Series 60 3.2
    * @param aMilliSeconds Timeout
    * @param aTimerId Timer ID
    */
    void StartTimerL( TInt aMilliSeconds, TInt aTimerId );

    /**
    * Stop timer
    * @since Series 60 3.2
    * @param aTimerId Timer ID
    */
    void StopTimer( TInt aTimerId );
    
    /**
    * Stops started timers
    * @since Series 60 3.2
    */
    void StopTimers();

    /**
    * Notify client about an error. ECCPEmergencyFailed if it is the last try 
    * of this plugin, normal CCP error otherwise.
    * @since Series 60 3.2
    * @param aError CCP error
    */
    void ErrorOccurred( TCCPError aError );

    /**
     * Initializes position provider and requests for position information. 
     * Traps leaves of RequestPositionL because emergency call must not fail 
     * due to this feature.
     *
     * @since S60 3.2
     * @param aIapId IAP ID
     */
    void RequestPosition( TUint32 aIapId );

    /**
     * Initializes position provider and requests for position information.
     *
     * @since S60 3.2
     * @param aIapId IAP ID
     */
    void RequestPositionL( TUint32 aIapId );

    /**
     * Requests for SIP proxy address
     *
     * @since S60 3.2
     */
    void RequestSipProxyAddressL();

    /**
     * Parses content ID from URI: removes "sip(s):" prefix, if one exists.
     * @since S60 3.2
     * @param aUri URI to be parsed
     * @return Parsed content ID
     */
    HBufC8* ParseContentIdL( const TDesC8& aUri );

    /**
     * Parses cid: adds "cid:" prefix and brackets.
     * @since S60 3.2
     * @param aContentId Content ID to be parsed
     * @return Parsed cid
     */
    HBufC8* ParseCidL( const TDesC8& aContentId );

    
private: // data

    /**
    * reference to SVP utility class
    */ 
    CSVPUtility& iSVPUtility;
    
    /**
    * Reference to MCE manager
    */     
    CMceManager& iMceManager;

    /**
    * Contains current session state
    */
    MCCPCallObserver::TCCPCallState iSessionState;

    /**
    * VoIP profile ID, KErrNotFound if creating an IAP session
    */
    TInt iVoipProfileId;

    /**
    * Id of SIP profile
    */
    TUint32 iSipProfileId;

    /**
    * IAP ID
    */
    TUint32 iIapId;

    /**
    * Session Expires value
    */
    TUint32 iSessionExpires;

    /**
    * Keep-alive value
    */
    TInt iKeepAlive;
    
    /**
    * Indicates that this the last profile to be tried with this plugin. If it 
    * fails, ECCPEmergencyFailed must be sent instead of normal error codes.
    */
    TBool iIsLastProfile;

    /**
    * Indicates that this session is dummy. ErrorOccurred is called on the 
    * next CCP API operation.
    */
    TBool iIsDummy;

    /**
    * Indicates that Dial() has been called
    */
    TBool iIsDialed;

    /**
    * Indicates that LI request is ready
    */
    TBool iIsLIRequestReady;

    /**
    * Session timers, need an array since there migh be more than one.
    */
    RPointerArray< CSVPTimer > iTimers;

    /**
    * Pointer to CCP call observer
    * Not own.
    */
    MCCPCallObserver* iEmergencyObserver;
    
    /**
    * Pointer to created emergency session
    * Own.
    */
    CMceSession* iEmergencySession;

    /**
    * Pointer to failed emergency session after received 415
    * Own.
    */    
    CMceSession* iFailedEmergencySession;

    /** 
    * User AOR. Member data because it is needed for creating location 
    * related SIP headers.
    */ 
    HBufC8* iUserAor;

    /** 
    * Recipient address.
    * Own.
    */ 
    HBufC8* iAddress;

    /** 
    * Recipient URI. Member data because it is needed for creating location 
    * related SIP headers.
    * Own.
    */ 
    HBufC8* iRecipientUri;

    /** 
    * Positioning provider
    * Own.
    */ 
    CSVPPositioningProvider* iPositioningProvider;
        
    /** 
    * Position information
    * Own.
    */ 
    HBufC8* iPositionInformation;

    /** 
    * Emergency connection.
    * Own.
    */ 
    CSVPEmergencyConnection* iEmergencyConnection;

    /** 
    * Hold Controller.
    * Own.
    */ 
    CSVPHoldController* iHoldController;

    /** 
    * Reference to DTMF observer.
    * Not own.
    */ 
    MCCPDTMFObserver* iDtmfObserver;

    /*
    * DTMF event generator, used in inband dtmf case.
    * Own.
    */
    CSVPDTMFEventGenerator* iDTMFEventGenerator;

    /**
     * Tone information. Updated everytime session state changes.
     */
    TCCPTone iTone;
    
    /** 
     * Contains DTMF tone sent. Used with inband DTMF. 
     */  
    TChar iDtmfTone;
       
    /** 
     * Contains DTMF string sent. Used with inband DTMF. 
     */
    HBufC* iDtmfString;
       
    /* 
    * Keeps count of the current tone sent. Used with inband DTMF
    */
    TLex iDtmfLex;  
    
private:

    // For testing
    SVP_UT_DEFS
    
    };

#endif // SVPEMERGENCYSESSION_H
