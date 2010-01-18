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
* Description:  Transfer controller class for blind and attended transfers
*
*/



#ifndef SVPTRANSFERCONTROLLER_H
#define SVPTRANSFERCONTROLLER_H

#include <e32base.h>
#include <mccpcall.h>                       // MCCPCall
#include <mccptransferprovider.h>           // MCCPTransferProvider
#include <mccptransferobserver.h>           // MCCPTransferObserver
#include "svputdefs.h"

class CSVPController;
class CSVPSessionBase;
class CMceSession;
class TMceTransactionDataContainer;
class CMceRefer;
class CMceInRefer;
class CMceEvent;
class CSVPTransferStateContext;
class MSVPTransferObserver;

/**
 *  Transfer controller
 *
 *  Handles blind and attended transfers
 *
 *  @lib svp.dll
 *  @since S60 3.2
 */
class CSVPTransferController : public CBase,
                               public MCCPTransferProvider
    {

public: // New methods

    /**
     * Two-phased constructor.
     */
    static CSVPTransferController* NewL(
                        CMceSession* aMceSession,
                        CSVPSessionBase* aSVPSession,
                        TMceTransactionDataContainer& aContainer, 
                        MSVPTransferObserver& aObserver );

    static CSVPTransferController* NewLC( 
                        CMceSession* aMceSession,
                        CSVPSessionBase* aSVPSession,
                        TMceTransactionDataContainer& aContainer, 
                        MSVPTransferObserver& aObserver );
    
    /**
     * Destructor.
     */
    virtual ~CSVPTransferController();

    /**
     * Handle mce event state changes.
     * The state of the event has changed.
     * @since S60 3.2
     * @param aEvent, event which state changed.
     * @param aStatusCode, status code.
     */
    void HandleEventStateChangedL( CMceEvent& aEvent,
				                  TInt aStatusCode );

    /**
     * Handle mce event observer notify events.
     * The state of the event has changed.
     * @since S60 3.2
     * @param aEvent, event received notification.
     * @param aContainer, if present, holds details of
     *        transaction causing state change.
     */
    void NotifyReceivedL( CMceEvent& aEvent,
                         TMceTransactionDataContainer* aContainer );

    /**
     * Handle the refer state change events.
     * @since S60 3.2
     * @param aRefer, the refer that has changed.
     * @param aStatusCode, status code
     */
    void HandleReferStateChangeL( CMceRefer& aRefer,
				                  TInt aStatusCode );

    /**
     * New incoming refer received.
     * @since S60 3.2
     * @param aRefer, the new inbound refer. Ownership is
	 *        transferred.
     * @param aReferTo SIP specific Refer-to header 
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
     * Checks if transfer is attended or unattended.
     * @since S60 3.2
     * @return TBool true if attended case
     */
    TBool IsAttended( );

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
    * Sets the reference to the sip (mce) session object
    * @since S60 3.2
    * @param aSession The session object
    * @return None
    */
    void SetMceSessionObject( CMceSession* aSession );

    /**
    * Send notify using the MCE event object
    * @since S60 3.2
    * @param aStatusCode code came from mce
    */
    void SendNotifyL( TInt aStatusCode );
    
    /**
    * Check if ingoming transfer is ongoing
    * @since S60 3.2
    * @return TBool true if incoming transfer case
    */
    TBool IsIncomingTransfer();
    
    /**
    * Terminate transfer due refer timer timeout
    * @since S60 3.2
    */
    void TerminateTransfer();

    /**
    * The Leaving version of TerminateTransfer
    * @since S60 3.2
    */
    void TerminateTransferL();

public: // From MCCPTransferProvider

    /**
    * Attended transfer to given call recipient.
    * There exist a call between A-B and A-C. A wants to transfer the call to B-C. 
    * A itself will not be inline in case the transfer is made succesfully. 
    * In above case C will be the aTransferTargetCall. B is the recipient of the 
    * current call between A-B - called on A's call object transfer provider. 
    * The A-B call is on hold.
    * @since S60 3.2
    * @param aTransferTargetCall - target receiver of the transferred call. 
    *        In this case there exist a call 
    * @return In case of an immediate error returns system error code,
    *       if request has been started returns KErrNone.
    */
    TInt AttendedTransfer( MCCPCall& aTransferTargetCall );

    /**
    * Attended transfer to given address. There does not need to be 
    * a current call between the given transfer target.
    * @since S60 3.2
    * @param aTransferTarget - transfer target address
    * @return In case of an immediate error returns system error code,
    *       if request has been started returns KErrNone.
    */
    TInt AttendedTransfer( const TDesC& aTransferTarget );

    /**
	* Unattended transfer. Call is requested to be transferred to given address.
    * @since S60 3.2
    * @param aTarget Address of the target
    * @return In case of an immediate error returns system error code,
    *       if request has been started returns KErrNone.
    */
    TInt UnattendedTransfer( const TDesC& aTransferTarget );

    /**
    * Notifies call about transfer status.
    * @since S60 3.2
    * @param aAccept Boolean value is transfer accepted or not.
    * @return In case of an immediate error returns system error code,
    *       if request has been started returns KErrNone.
    */
    TInt AcceptTransfer( const TBool aAccept );

    /**
    * When the other end of the call has requested call transfer to third party it is notified to 
    * CCPCallCall::EventOccurred(ECCPRemoteTransferring).
    * The new recipient of the call can be fetched via this method.
    * @since S60 3.2
    * @param none
    * @return New recipient for the call after transfer
    */
    const TDesC& TransferTarget() const;

    /**
    * Add an observer for transfer related events.
    * Plug-in dependent feature if duplicates or more than one observers 
    * are allowed or not. Currently CCE will set only one observer.
    * @since S60 v3.2
    * @param aObserver Observer
    * @return none
    * @leave system error if observer adding fails
    */
    void AddObserverL( const MCCPTransferObserver& aObserver );

    /**
    * Remove an observer.
    * @since S60 v3.2
    * @param aObserver Observer
    * @return KErrNone if removed succesfully. KErrNotFound if observer was not found.
    */
    TInt RemoveObserver( const MCCPTransferObserver& aObserver );
	  
    
private:

    /**
     * C++ default constructor.
     */
    CSVPTransferController();
    
    /**
     * Second-phase constructor.
     */
    void ConstructL( CMceSession* aMceSession,
                     CSVPSessionBase* aSVPSession,                     
                     TMceTransactionDataContainer& aContainer,
                     MSVPTransferObserver& aObserver );

    /**
    * Private transfer functionality.
    * @since S60 3.2
    * @param aAccept Boolean value is transfer accepted or not.
    */
    void TransferL( MCCPCall* aCall, 
                    const TDesC& aTarget, 
                    const TBool aAttendedTransfer );
    

private: // data

    /**
     * Transfer state context.
     * Own.
     */
    CSVPTransferStateContext* iTransferContext;
    
    /**
    * Transfer observer
    */
    MCCPTransferObserver* iCCPTransferObserver; 

    /**
     * 202 Accepted to the refer received, waiting still the notify
     */
    TBool iAccepted;      
    

private:

    // For testing
    SVP_UT_DEFS
		
    };

#endif // CSVPTRANSFERCONTROLLER_H
