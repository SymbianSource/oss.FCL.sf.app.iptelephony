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
* Description:  Class representing the SVP MO session
*
*/



#ifndef SVPMOSESSION_H
#define SVPMOSESSION_H

#include <sipprofileregistryobserver.h>
#include <sipconnectionobserver.h>
#include <sipobserver.h>

#include <sip.h>
#include <sipprofile.h>
#include <sipprofileregistry.h>

#include <mcemanager.h>

#include "svpsessionbase.h"
#include "svpconsts.h"
#include "svputdefs.h"


class CRCSEProfileEntry;
class CSVPForwardProvider;
class CDesC8Array;

/**
 *  Representation of SVP Mo session
 *
 *  Inlcudes all the methods needed to establish SVP MO session. 
 *
 *  @lib svpcontroller.lib   
 *  @since S60 3.2
 */
class CSVPMoSession : public CSVPSessionBase, 
                      public MSIPObserver,
                      public MSIPProfileRegistryObserver
    {  
     
public:
    
    /**
    * Two-phased constructor.
    */
    static CSVPMoSession* NewL( 
        CMceManager& aMceManager,
        const TDesC8& aRecipient,
        CRCSEProfileEntry& aVoipProfile, 
        const CCCPCallParameters& aParameters,
        TMceTransactionDataContainer& aContainer,                                
		MSVPSessionObserver& aObserver,
		CSVPUtility& aSVPUtility,
		CSVPRtpObserver& aRtpObserver,
		TUint32& aSecurityStatus, 
		CDesC8Array* aUserHeaders = 0 );
    
    /**
    * Destructor.
    */
    virtual ~CSVPMoSession();
    
    
public: // methdos from MCCPCall   

    /**
    * From MCCPMoSession. Starts dialing to recipient
    * @since Series 60 3.2
    * @return 
    */
    TInt Dial();

    /**
    * From MCCPMoSession. Cancels (dial) request
    * @since Series 60 3.2
    * @return
    */
    TInt Cancel();
    
    /**
    * From MCCPCall Ends an ongoing call.
    * @since Series 60 3.2
    * @return
    */
    TInt HangUp();
    
    /**
    * From MCCPMoSession. Destructs session
    * @since Series 60 3.2
    * @return
    */
    TInt Release();
    
    /**
	* From MCCPCall Get Forward provider
	* @since Series 60 3.2
	* @return Pointer to MCCPForwardProvider if succesfull, NULL if not available
	*/
	MCCPForwardProvider* ForwardProviderL( const MCCPForwardObserver& aObserver );
    
    /**
    * Puts the specified session on hold.
    * @since Series60 3.2
    * @return 
    */
    TInt Hold();

    /**
    * Resumes held session.
    * @since Series60 3.2
    * @return 
    */
    TInt Resume();

    /**
    * Handles remote action after session is established.
    * @since Series60 3.2
    * @param None
    * @return
    */
    TInt IncomingRequest( CMceInSession* aUpdatedSession );
    
    /**
    * Returns boolean value, is the session mobile originated.
    * @since Series60 3.2
    * @param None
    * @return TBool ETrue if mobile originated, otherwise EFalse
    */
    TBool IsMobileOriginated() const;
    

                  
public: // methods from MSIPObserver

    /**
    * From MSIPObserver A SIP request has been received from the network. 
    * @since  Series 60 3.0
    * @param  aIapId The IapId from which the SIP request was received.  
    * @param  aTransaction Contains local address, remote address of a sip
    *         message, as well as optional SIP message method, headers and 
    *         body. The ownership is transferred.  
    * @return None
    */
     void IncomingRequest( TUint32 aIapId, 
                           CSIPServerTransaction* aTransaction );

    /**
    * From MSIPObserver The received SIP request time-outed and it has been
    * destroyed.  
    * @since  Series 60 3.0
    * @param  aSIPServerTransaction The time-outed transaction. Ownership is
    *         not transferred. 
    * @return None
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
    * @return None
    */ 
     void ProfileRegistryErrorOccurred( TUint32 aSIPProfileId,
                                        TInt aError );
    
    /**
    * From MSIPProfileRegistryObserver An event related to SIP Profile has
    * occurred.
    * @since  Series 60 3.0
    * @param  aProfileId SIP Profile ID. 
    * @param  aEvent An occurred event.
    * @return None
    */ 
    void ProfileRegistryEventOccurred( TUint32 aProfileId,
                                        TEvent aEvent ); 
                                        
public: // new public methods
    
    /**
    * Method for creating normal Mo session if secure sessio
    * creation has failed
    * @since  Series 60 3.2
    * @param  aMceManager Mce Manager. 
    */
    void CreateNonSecureSessionL( CMceManager& aMceManager );  
        
    /**
	* Returns pointer to array containing addresses to forward the call to.
	* Index zero contains the first address. If there are no addresses available
	* returns an empty array.
	* @since Series 60 3.2
	* @param none
	* @return Reference to array containing addresses
    * @leave KErrNotReady call is not in forwarding state
    * @leave KErrNotAllowed if call is not mobile originated
    * @leave system error code 
    * @pre Call state is MCCPCallObserver::ECCPStateForwarding and call type is MO
    * @pre Call MCCPForwardObserver::ECCPMultipleChoices event is received
	*/
	const CDesC8Array& GetForwardAddressChoicesL();

	/**
	* Forward call to address at given index.
	* @since Series 60 3.2
	* @param aIndex Index of address where the call is to be forwarded. Address is 
	* found in the array received from GetForwardAddressChoicesL. Index starts from zero.
	* @return none
	* @leave KErrArgument Index is not in array
    * @leave KErrNotReady Call is not in forwarding state
    * @leave KErrNotAllowed If call is not mobile originated
    * @pre Call state is MCCPCallObserver::ECCPStateForwarding and call type is MO
    * @pre Call MCCPForwardObserver::ECCPMultipleChoices event is received
	*/
	void ForwardToAddressL( const TInt aIndex );
	
    /**
    * Add an observer for forward related events.
    * Currently CCE will set only one observer.
    * @since S60 v3.2
    * @param aObserver Observer
    * @return none
    * @leave system error if observer adding fails
    */
    void AddForwardObserverL( const MCCPForwardObserver& aObserver );

    /**
    * Remove an observer.
    * @since S60 v3.2
    * @param none
    * @param aObserver Observer
    * @return KErrNone if removed succesfully. 
    * @return KErrNotFound if observer was not found.
    */
    TInt RemoveForwardObserver( const MCCPForwardObserver& aObserver );

    /**
    * Notify forwardobserver about forward event.
    * @since S60 3.2
    * @param aEventCode eventcode
    * @return none
    */
    void NotifyForwardEvent( TInt aEventCode );

 	/**
	* Add forward address for this session
	* @since S60 3.2
	* @param aFwdAddress 
	* @return Count of addresses, otherwise system wide error code
    * @leave system error if address adding fails
	*/
    TInt AddForwardAddressL( const TDesC8& aFwdAddress );

 	/**
	* Reset address list
	* @since S60 3.2
	* @return None
	*/
    void ResetForwardAddressChoices();

private: // new private methods

    /**
    * Method for completing sip uri
    * @since Series 60 3.2
    * @param aRecipient Recipient received from application
    * @param aRecipient SIP profile which is making the call
    * @param aSecurityStatus security preference 
    * @param aUserEqualsPhoneRequired defines user=phone parameter
    * @return HBufC8* constructed uri
    */
    HBufC8* CompleteUriL( const TDesC8& aRecipient, 
                          const CSIPProfile&,
                          const TUint32& aSecurityStatus,
                          TBool aUserEqualsPhoneRequired );

    /**
    * Dial method that leave
    * @since Series 60 3.2
    */
    void DialL();

    
protected:

    /**
    * C++ default constructor.
    */
    CSVPMoSession( TUint32 aSessionExpires, 
                   TMceTransactionDataContainer& aContainer,
                   MSVPSessionObserver& aObserver,
                   CSVPUtility& aSVPUtility,
                   CSVPRtpObserver& aRtpObserver );
                                      
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


private:

    /**
    * second-phase constructor
    */
    void ConstructL( CMceManager& aMceManager, 
                     CRCSEProfileEntry& aVoipProfile, 
                     const CCCPCallParameters& aParameters,
                     const TDesC8& aRecipient,
                     TUint32& aSecurityStatus,
                     CDesC8Array* aUserHeaders );
                     
private: // data

    /**
    * Session Expires value
    */
    TUint32 iSessionExpires;
          
    /**
    * User header array
    */
    CDesC8Array* iUserHeaders;
    
    /**
    * Forward observer
    * Not own.
    */
    MCCPForwardObserver* iForwardObserver;
    
    /**
    * Forward provider
    * Own.
    */
    CSVPForwardProvider* iForwardProvider;
    
    /**
    * Recipient address, needed for secure preferred -secure call setting
    */
    TBuf8< KSVPMaxUriLength > iSecondCallRecipient;
    
    /**
    * List for forward addresses
    */
    CDesC8Array* iForwardAddressList;
    
    /**
    * Pointer to Mce manager
    * Not own
    */
    CMceManager* iMceManager;
    
private:

    // For testing
    SVP_UT_DEFS
    
    };

#endif // SVPMOSESSION_H
