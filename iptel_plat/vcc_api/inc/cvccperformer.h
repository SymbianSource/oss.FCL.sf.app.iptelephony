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
* Description:   CCP reference plug-in implementation.
*
*/

#ifndef CVCCPERFORMER_H
#define CVCCPERFORMER_H

//  INCLUDES
#include <e32base.h>
#include <e32cmn.h> // HBufC
#include <mccpcallobserver.h> //CCP API
#include <mccpcscall.h>//CCP API
#include <cccpcallparameters.h>
#include <cccecallparameters.h>

#include "cconvergedcallprovider.h"
#include "tvccstate.h"

class CVccEngPsPropertyListener;
class CVccEngPsProperty;
class CVccHoTrigger;
class CVccTransferProvider;

/**
* The class acts as a proxy forward the CCE. 
* It forwards the call to the actual MCCP interface implementation 
* and initiates new call if instructed to do the handover.
* 
*  @lib vccperformer.dll
*  @since S60 3.2
* 
*/
class CVccPerformer : public CBase,
                   	  public MCCPCSCall,
                   	  public MCCPCallObserver
                   
    {
public:

 	/*
 	* The c'tor
 	* @param aProviders Array or call providers
 	* @param aState State of the call to be created
 	* @param aVccHoTrigger Reference to handover trigger object
 	* @since S60 3.2
 	*/
    static CVccPerformer* NewL(RPointerArray<CConvergedCallProvider>& aProviders, 
    						   TVccState& aState, CVccHoTrigger& aVccHoTrigger,
    						   TBool aCsOriginated );
    
    /*
    * dtor
    */
    ~CVccPerformer();
    
    /**
     * Starts Handover procedure
     * @since S60 3.2
     * @param aValue Type and direction of the possible HO
     * @return void
     */
    void SwitchL( const TInt aValue );

    /**
    * Gets the observer for the call events
    * @since Series60 3.2
    * @return Call event observer 
    */
    MCCPCallObserver* CallObserver();
    
    
    /**
    * Makes run-time decision what technology to use for the call
    * @since Series60 3.2
    * @param aParameters Call parameters
    * @param aRecipient Recipient
    * @param aObserver Reference to call observer
    * @return Reference to created call object or leaves with Symbian error code
    */
    MCCPCall* CreatePrimaryCallL(const CCCPCallParameters& aParameters,
                          const TDesC& aRecipient,
                          const MCCPCallObserver& aObserver );
                          
    /**
    * handles incoming call
    * @since Series60 3.2
    * @param aCall Reference to call object
    * @return reference to this object
    */
    
    MCCPCall* IncomingCall( MCCPCall* aCall );
    
    /**
    * handles 3rd party mo call
    * @since Series60 3.2
    * @param aCall Reference to call object
    * @return reference to this object
    */
    MCCPCall* MoCall( MCCPCall* aCall );
    
    /**
    * Gets the actual implementation of the active call
    * @since Series60 3.2
    * @return reference to call object
    */
    MCCPCall* PrimaryCall();
    
    /**
    * returns cs provider
    * @since Series60 3.2
    * @return reference to CS call provider
    */
    CConvergedCallProvider& CsProvider();
    
    /**
    * returns ps provider
    * @since Series60 3.2
    * @return reference to PS call provider
    */
    CConvergedCallProvider& PsProvider();
                              
    /**
    * Changing the call object
    * @since Series60 3.2
    * @param aCall Reference to a new call object
    */                                            
    void SetCall( MCCPCall* aCall );                

    /**
    * Changing the call object
    * @since Series60 3.2
    * @param aCall Reference to a new call object
    * leaves with system error code
    */                                            
    void SetCallL( MCCPCall* aCall );                

                          
    /**
    * Releases call
    * @since Series60 3.2
    * @param aCall Call to be removed
    * @return Symbian error code
    */
    TInt ReleaseCall( MCCPCall& aCall );
    
    /**
    * Informat that the domain has been changed.
    * @since Series60 3.2
    */
    void HandoverReady();           

    /**
    * Creates an implementation for making handover
    * @since Series60 3.2
    * @param aParameters Call parameters
    * @param aRecipient Recipient
    * @param aObserver Reference to call observer
    */
    void CreateSecondaryCallL(const CCCPCallParameters& aParameters,
                               const TDesC& aRecipient,
                               const MCCPCallObserver& aObserver);
    /**
    * Returns created local implementation
    * @since Series60 3.2
    * @return local implementation for hand-over, NULL if not created.
    */                         
    MCCPCall* SecondaryCall();
    
    /**
    * Updates the current state with the next state
    * @since Series60 3.2
    * @param aState reference to a new call state
    */
    void SetState( TVccState& aState );
    
    /**
    * Returns current state name
    * @since Series60 3.2
    * @return current state, see state headers for the state names
    */
    TUint StateName() const;
    
    /**
    * Gets the notifier
    * @since Series60 3.2
    * @return the notifier to be used for the reporting hand-over process
    */
    CVccEngPsProperty& Notifier();
    
    /**
     * Activates the trigger if call is connected, stops otherwise
     * @since Series60 3.2
     * @param aActivation activate or deactivate trigger
     */
    void ActivateTrigger( TBool aActivation );
    
    /**
     * PS key is updated via this method, so that UI item is removed from 
     * in call Options menu when needed
     * @since Series60 3.2
     * @param aAllow to show or hide HO option in menu
     */
    void AllowHo( TBool aAllow );

    /**
     * PS key is updated via this method, so that UI item is removed from 
     * in call Options menu when needed
     * @since Series60 3.2
     * @param aAllow to show or hide HO option in menu
     * @leave with system wide error if fails 
     */
    void AllowHoL( TBool aAllow );
    
    /**
     * Checks own state after conference and starts trigger if state is connected.
     * @since Series60 3.2
     */
    void ConferenceEnded();    

    /**
     * Release secondary call leg, if needed, before calling destructor.
     * @since Series60 3.2
     */
    void ReleaseSecondaryCallLeg();
    
    /**
     * Save original RemoteParty and RemotePartyName, so that call bubble
     * stays intact also after handover
     */
    void SetRemoteParties();
    
    /**
     * Finds provider index number from array
     * @param aVoipIndex true if voip index should be searched, false if
     *                   cs index
     * @return index number from which the provider can be found, KErrNotFound
     *         if provider not found from array
     */
    TInt VoipProviderIndex( TBool aVoipIndexWanted );
    
      
// from base class MCCPCall    
    /**
    * @see MCCPCall::Answer()
    */
    TInt Answer();
      
    /**
    * @see MCCPCall::Reject()
    */
    TInt Reject();

    /**
    * @see MCCPCall::Queue()
    */
    TInt Queue();

    /**
    * @see MCCPCall::Dial()
    */
    TInt Dial();

    /**
    * @see MCCPCall::Ringing()
    */
    TInt Ringing();
      
    /**
    * @see MCCPCall::HangUp()
    */
    TInt HangUp();
      
    /**
    * @see MCCPCall::Cancel()
    */
    TInt Cancel();
      
    /**
    * @see MCCPCall::Hold()
    */
    TInt Hold();
      
    /**
    * @see MCCPCall::Resume()
    */
    TInt Resume();
      
    /**
    * @see MCCPCall::Swap()
    */
    TInt Swap();
      
    /**
    * @see MCCPCall::RemoteParty()
    */
    const TDesC& RemoteParty() const;

    /**
    * @see MCCPCall::RemotePartyName()
    */
    const TDesC& RemotePartyName();

    /**
    * @see MCCPCall::DialledParty()
    */
    const TDesC& DialledParty() const;
      
    /**
    * @see MCCPCall::IsCallForwarded()
    */
    TBool IsCallForwarded() const;
     
    /**
    * @see MCCPCall::IsMobileOriginated()
    */
    TBool IsMobileOriginated() const;
      
    /**
    * @see MCCPCall::State()
    */
    MCCPCallObserver::TCCPCallState State() const;
      
    /**
    * @see MCCPCall::Uid()
    */
    TUid Uid() const;
      
    /**
    * @see MCCPCall::Caps()
    */
    MCCPCallObserver::TCCPCallControlCaps Caps() const;     
    
    /**
    * @see MCCPCall::SecureSpecified()
    */
    TBool SecureSpecified() const;
    
    /**
    * @see MCCPCall::IsSecured()
    */
    TBool IsSecured() const;
      
    /**
	* @see MCCPCall::SetParameters()
	*/
    void SetParameters( const CCCPCallParameters& aNewParams );

    /**
	* @see MCCPCall::Parameters()
	*/
    const CCCPCallParameters& Parameters() const;
      
    /**
	* @see MCCPCall::Tone()
    */
    TCCPTone Tone() const;
      
	/**
	* @see MCCPCall::ForwardProviderL()
	*/
	MCCPForwardProvider* ForwardProviderL( const MCCPForwardObserver& aObserver );

	/**
    * @see MCCPCall::TransferProviderL()
    */
	MCCPTransferProvider* TransferProviderL( const MCCPTransferObserver& aObserver );
      
    /**
    * @see MCCPCall::AddObserverL()
    */
    void AddObserverL( const MCCPCallObserver& aObserver );

    /**
    * @see MCCPCall::RemoveObserver()
    */
    TInt RemoveObserver( const MCCPCallObserver& aObserver );
    
//from base class MCCPCSCall
    
    /**
    * @see MCCPCSCall::Dial()
    */
	TInt Dial( const TDesC8& aCallParams );

    /**
    * @see MCCPCSCall::NoFDNCheck()
    */
	void NoFDNCheck();

    /**
    * @see MCCPCSCall::GetMobileCallInfo()
    */
	TInt GetMobileCallInfo( TDes8& aCallInfo ) const;

    /**
    * @see MCCPCSCall::SwitchAlternatingCall()
    */
	TInt SwitchAlternatingCall();

    /**
    * @see MCCPCSCall::GetMobileDataCallCaps()
    */
	TInt GetMobileDataCallCaps( TDes8& aCaps ) const;
	
    /**
    * @see MCCPCSCall::LogDialedNumber()
    */
    TBool LogDialedNumber() const;
    
    
//from base class MCCPCallObserver
    
    /**
    * @see MCCPCallObserver::ErrorOccurred()
    */
	void ErrorOccurred( const TCCPError aError, MCCPCall* aCall );

    /**
    * @see MCCPCallObserver::CallStateChanged()
    */
	void CallStateChanged( const MCCPCallObserver::TCCPCallState aState, 
	                       MCCPCall* aCall );

    /**
    * @see MCCPCallObserver::CallStateChangedWithInband()
    */
	void CallStateChangedWithInband( const MCCPCallObserver::TCCPCallState aState,
	                                 MCCPCall* aCall );

    /**
    * @see MCCPCallObserver::CallEventOccurred()
    */
	void CallEventOccurred( const MCCPCallObserver::TCCPCallEvent aEvent, 
	                        MCCPCall* aCall  );

    /**
    * @see MCCPCallObserver::CallCapsChanged()
    */
	void CallCapsChanged( const TUint32 aCapsFlags, MCCPCall* aCall );
	
private:

    /**
    * constructor.
    */
    CVccPerformer(RPointerArray<CConvergedCallProvider>& aProviders,
    			  TVccState& aState, CVccHoTrigger& aVccHoTrigger,
    			  TBool aCsOriginated);

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();
    
       /**
    * Removes SIP-uri from remoteparty number if call is VoIP call
    */
    void ParseRemoteParty();

private: // data
    
    /**
     * Call provider array
     * Not own
     */
	RPointerArray<CConvergedCallProvider>& iProviders;

    
    /**
     * actual call implementation, can be changed run-time
     * Own
     */
	MCCPCall* iPrimaryCall; 
	
    /**
     * actual call implementation, created to perfome a handover
     * Own
     */
	MCCPCall* iSecondaryCall; 
	
    /**
     * remembering the call observer because it needs to change
     * Not own
     */
	MCCPCallObserver* iCallObs;
	
    /**
     * for informing the states
     * Own
     */
	CVccEngPsProperty* iVccPsp;
	
    /**
     * current state in case of the hand-over
     * Own
     */
	TVccState* iCurrentState;
	
    /**
     * trigger for initiating automatic HOs
     * Not own
     */
	CVccHoTrigger& iVccHoTrigger;
	
	/**
	 * Originating domain of the call
	 * */
	TBool iCsOriginated;

	/**
	 * Transfer provider for handling call transfer 
	 * Own
	 */
	CVccTransferProvider* iTransferProvider;

	/**
	 *For storing the remote party information 
	 */

	TBuf<100> iOrigRemotePartyName;
	TBuf<100> iOrigRemoteParty;
	
	TBool iRemotePartiesSet;
	friend class T_CVccPerformer;
    };

#endif      // CVCCPERFORMER_H

// End of File
