/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/


#ifndef T_SCPSUBSERVICE_H
#define T_SCPSUBSERVICE_H

#include <e32std.h>
#include <cchservice.h>

#include "scpdefs.h"
#include "scpsipconnectionobserver.h"

class MCchServiceObserver;
class MScpSubServiceObserver;
class CScpServiceHandlerBase;
class TScpSubServiceState;
class CScpServiceStorage;
class CScpProfileHandler;
class CScpService;

/**
 *  SCP sub service.
 *  @lib sipconnectionprovider.dll
 */
class CScpSubService : public CBase,
                       public MScpConnectionObserver
    {    
public:

    enum TEnableState
        {
        EScpNoRequest,
        EScpEnabled,
        EScpDisabled,
        EScpRefreshed
        };

    static CScpSubService* NewL( TInt aId, 
                                 TInt aSubServiceId,
                                 TCCHSubserviceType aSubServiceType,
                                 CScpService& aService );

        
    /**
     * Destructor
     */ 
    ~CScpSubService();

    /**
     * Enables this sub service
     */ 
    void EnableL();

    /**
     * Disables this sub service
     * @return Symbian error codes
     */ 
    TInt Disable();

    /**
     * Returns internal id of this subservice
     * @return Internal id
     */ 
    TInt Id() const;

    /**
     * Returns service id of this subservice
     * @return Sub service id
     */ 
    TInt SubServiceId() const;

    /**
     * Return state of the subservice
     * @return State
     */ 
    TCCHSubserviceState State() const;

    /**
     * Returns sub service type of the subservice
     * @return Sub service type
     */ 
    TCCHSubserviceType SubServiceType() const;

    /**
     * Returns service storage
     * @return Service storage
     */
    CScpServiceStorage& ServiceStorage() const;

    /**
     * Returns profile handler
     * @return Service storage
     */
    CScpProfileHandler& ProfileHandler() const;

    /**
     * Sets sip profile id for the subservice
     * @param aSipProfileId Sip profile id
     */ 
    void SetSipProfileId( TInt aSipProfileId );
    
    /**
     * Returns sip profile id of the subservice
     * @return sip profile id
     */ 
    TInt SipProfileId() const;

    /**
     * Sets enable requested state
     * @param aEnableRequestedState State
     */ 
    void SetEnableRequestedState( TEnableState aEnableRequestedState );  

    /**
     * Return info about if enable or disable is requested
     * @return Enable requested state
     */ 
    TEnableState EnableRequestedState() const;  

    /**
     * Raises the flag that sub service is succesfully disconnected
     * its service (e.g. unsubscribed message received from network).
     * When disconnected it's ok for this sub service to disable SIP 
     * profile
     * @param aSubServiceDisconnected Sub service disconnected
     */ 
    void SetSubServiceDisconnected( TBool aSubServiceDisconnected );

    /**
     * Return Sub service disconnceted flag
     * @return Sub service disconnected
     */ 
    TBool SubServiceDisconnected() const; 

    /**
     * Sets access point for the sub service
     * @param aApId Access point
     */ 
    void SetApId( TUint32 aApId );

    /**
     * Returns access point id of the subservice
     * @return Access point id
     */    
    TUint32 ApId() const;

    /**
     * Sets snap id or the subservice
     * @param aSnapId Snap id
     */ 
    void SetSnapId( TUint32 aSnapId );

    /**
     * Returns the iap type the subservice is using
     * @return Iap type
     */ 
    TScpIapType GetIapType() const;

    /**
     * Returns snap id id of the subservice
     * @return Snap id
     */ 
    TUint32 SnapId() const;

    /**
     * Returns service handler
     */
    CScpServiceHandlerBase& ServiceHandler() const;

    /**
     * Handle connection event. Based on event a corresponging 
     * method from current state is called.
     * @param aEvent Connection event
     */
    void HandleConnectionEvent( TScpConnectionEvent aEvent );

    /**
     * Returns last reported error from a state change
     * @return Last error
     */
    TInt LastReportedError() const;
    
    /**
     * Last reported error setter
     * @param aError error which to be set
     */
    void SetLastReportedError( const TInt aError );

    /**
     * Sets the sub service as reserved
     * @param aReserved Reserved
     */
    void SetReserved( TBool aReserved );

    /**
     * Returns reserved info of the sub service
     * @return ETrue if reserved
     */
    TBool IsReserved() const;
    
    /**
     * Returns roaming info of the sub service
     * @return ETrue if roaming
     */
    TBool IsRoaming() const;
    
    /**
     * Set subservice observer
     */
    void SetSubServiceObserver( MScpSubServiceObserver* aSubServiceObserver );
    
    /**
     * Enable counter getter.
     */
    TInt EnableCounter();
    
    /**
     * Reset enable counter.
     */
    void ResetEnableCounter();

private:

    /**
     * Constructor
     */ 
    CScpSubService( TInt aId, 
                    TInt aSubServiceId,
                    TCCHSubserviceType aType,
                    CScpService& aService );

    /**
     * 2nd phase constructor
     */
    void ConstructL();
   
    /**
     * TScpStates use this function to change the state
     * of the subservice.
     * @param aState New state
     * @param aError CCH errors
     */ 
    void ChangeState( TCCHSubserviceState aState, TInt aError );
    
    /**
     * Adds presencehandler to voip's subserviceobserver if 
     * presencesubservice exists.
     */
    void SetPresenceToObserveVoIP();

    /**
     * Enable occured.
     */
    void EnableOccured();

private:

    /**
     * Internal id.
     */       
    TInt iId;

    /**
     * Service id.
     */     
    TInt iSubServiceId;

    /**
     * Subservice type.
     */     
    TCCHSubserviceType iSubServiceType;

    /**
     * Service that contains this sub service
     */
    CScpService& iService;

    /**
     * Service handler. Owned.
     */ 
    CScpServiceHandlerBase* iServiceHandler;
    
    /**
     * Protocol profile id.
     */     
    TInt iSipProfileId;

    /**
     * Enable requested state
     */     
    TEnableState iEnableRequestedState;

    /**
     * Sub service disconnected
     */
    TBool iSubServiceDisconnected;
            
    /**
     * IAP id.
     */     
    TUint32 iApId;
    
    /**
     * Snap id.
     */     
    TUint32 iSnapId;

    /**
     * Last reported error
     */
    TInt iLastReportedError;

    /**
     * Reserved
     */
    TBool iReserved;
    
    /**
     * Indicates is roaming ongoing
     */
    TBool iRoaming;

    /**
     * Points to current sub service state.
     */
    TScpSubServiceState* iSubServiceState;
    
    /**
     * Sub service observer
     */
     MScpSubServiceObserver* iSubServiceObserver;
     
     /**
      * Counting enable requests, max count is sub service specific
      */
     TInt iEnableCounter;

private:

    // Needs access to call the private ChangeState
    friend class TScpSubServiceState;

#ifdef _DEBUG
    friend class T_CScpService;
    friend class T_CScpServiceStorage;
    friend class T_CScpServiceManager;
    friend class T_CScpVmbxHandler;
    friend class T_CScpVoipHandler;
    friend class T_CScpPresenceHandler;
    friend class T_CScpImHandler;
#endif
    };    

#endif // T_SCPDEFS_H
