/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef C_CSCPPRESENCEHANDLER_H
#define C_CSCPPRESENCEHANDLER_H

#include <e32base.h>
#include <ximpbase.h>
#include <ximpcontextobserver.h>
#include <ximpcontextstate.h>
#include <spdefinitions.h>

#include "scpservicehandlerbase.h"
#include "scpsubserviceobserver.h"

class CScpSubService;
class CScpSubService;
class CScpServiceStorage;
class MPresenceInfo; 
class MXIMPContext;
class MXIMPClient;
class MPresenceFeatures;
class TScpReqId;

/**
*  Presence sub service handler.
*
*  @lib sipconnectionprovider.dll
*/
class CScpPresenceHandler : public CScpServiceHandlerBase,
                            public MXIMPContextObserver,
                            public MScpSubServiceObserver
                            
    {
    
public:
    /**
     * Enumeration for presence states
     */
    enum TScpPresenceState
        {
        EBinding = 0,
        EBindComplete,      /* Bind complete, publish not yet requested */
        EPublishing,        
        ESubscribing,       
        EPresenceOnline,    /* Bind, publish and subscribe complete -> presence online */
        EPresenceOffline,   /* Bind complete, presence offline */
        EUnBinding,
        ENoBind
        };

public:
     
    /**
     * Two-phased constructor
     * @param aSubService The presence subservice
     */
    static CScpPresenceHandler* NewL( CScpSubService& aSubService );
    
    /**
     * Destructor.
     */
    virtual ~CScpPresenceHandler();

public: // From CScpServiceHandlerBase
    
    /**
     * Enables service.
     */
    void EnableSubServiceL();

    /**
     * Disables service.
     * @return Symbian error code
     */
    TInt DisableSubService();
            
    /**
     * Returns type of sub service (VoIP, Vmbx, Presence).
     * @return Type of the sub service.
     */
    TCCHSubserviceType SubServiceType() const;
    
    /**
     * Handles sip connection event
     * @param aProfileId Sip profile id
     * @param aSipEvent Connection event
     */
    void HandleSipConnectionEvent( const TUint32 aProfileId,
                                   TScpConnectionEvent aSipEvent );
    
    /**
     * Query is starting to ALR allowed.
     * Client can start ALR later if needed to do something first
     * 
     * @return ETrue if allowed.
     */
    TBool IsSipProfileAllowedToStartAlr();
    
public: // From MScpSubServiceObserver
    /**
     * Handle SubService state change.
     * This function is for observing VoIP subservice.
     * If VoIP subservice connection is not OK, 
     * set presence state to offline.
     * @param aState subservice connection state
     * @param aError is the error of the service
     */    
    void HandleSubServiceChange( TCCHSubserviceState aState, TInt aError );    


public: // From MXIMPContextObserver                        

    /**
     * Handles presence context events.
     * @param aContext
     * @param aEvent Event
     */                             
    void HandlePresenceContextEvent( const MXIMPContext& aContext,
                                     const MXIMPBase& aEvent );

private:
    
    /**
     * C++ default constructor.
     * @param aSubService The presence subservice
     */
    CScpPresenceHandler( CScpSubService& aSubService );
    
    /**
     * Symbian second phase constructor.
     */
    void ConstructL();
    
    /**
     * Update XDM settings access point.
     * XDM ap should be same as sip profile.
     */      
    void UpdateXdmSettingsL();
    
    /**
     * Binds SCP's presence context.
     */                                 
    void ServerBindL();
    
    /**
     * Unbinds SCP's presence context.
     */
    void ServerUnBindL();
    
    /**
     * Stops publishing according to parameter and unbinds the presence
     * context. Leaves, if there are ongoing requests and publish offline
     * is not called.
     * @param aDoStopPublish if ETrue, users own presence status is 
     *  removed from presence server before unbinding the context
     */                             
    void HandleDeregistrationL( TBool aDoStopPublish );
    
    /**
     * Publishes presence according to given parameter.
     * @param aPublishOnline If ETrue, publish Online, if EFalse,
     *  publish Offline
     */
    void PublishPresenceL( TBool aPublishOnline );
    
    /**
     * Creates presence info item
     * @param aState presence state
     * @return The created presence info pointer
     */      
    MPresenceInfo* CreateInfoLC( TBool aState );

    /**
     * Changes service state to deregistered. If user requested
     * disabling the service, also profile deregistration is done.
     */                             
    void DeregisterNow();

    /**
     * Get property id.
     * @param aProperty, presence service property name
     * @param aValue, the id as a result of the query
     */
    void GetPresencePropertyIdL( TServicePropertyName aProperty, TInt& aValue ) const;

    /**
     * Force service disable
     * @param aSelf This object
     * @return 1
     */    
    static TInt ForcePresenceServiceDisable( TAny* aSelf );
    
    /**
     * Find VoIP subservice pointer
     * @return VoIP subservice pointer
     */
    CScpSubService* GetVoipSubService();
    
    /**
     * Handles the XIMP context state events.
     * @param   aEvent The XIMP event to handle
     */
    void HandleContextStateEvent( const MXIMPBase& aEvent );
    
    /**
     * Handles the XIMP request complete events
     * @param   aEvent The XIMP event to handle
     */
    void HandleRequestCompleteEvent( const MXIMPBase& aEvent );
    
    /**
     * Handles the Bind complete event
     */
    void HandleBindCompleteEvent();
    
    /**
     * Subscribe presentity group
     */
    void SubscribePresentityGroupL();
    
    /**
     * Unsubscribe presentity group
     */
    void UnsubscribePresentityGroupL();
     
    /**
     * Gets stored presence values if available
     * @param aAvailabilityEnum Holds stored availability value
     * @param aCustomMessage Holds stored custom message 
     */
    void GetStoredPresenceValuesL( TInt& aAvailabilityEnum, RBuf& aCustomMessage );
    
private: // data
    
    MXIMPClient* iPresClient;
    MXIMPContext* iPresenceCtx;
    MPresenceFeatures* iFeature;
    
    /**
     * Id for presence settings
     */
    TInt iPresenceSettingsId;

    /**
     * Tells the state of presence handler
     */
    TScpPresenceState iPresenceState;
    
    /**
     * Array for storing requests
     */
    RArray< TScpReqId > iReqIdArray;
    
    /**
     * Indicates do we have to make rebind
     */
    TBool iRebind;

    /**
     * Holds the last ximp error
     */
    TInt iLastXimpError;
    
    /**
     * Flag for indicating if service disabling should be done
     * after all Ximp requests are completed.
     */
    TBool iDisableAfterXimpRequestsCompleted;
    
#ifdef _DEBUG
    friend class T_CScpPresenceHandler;
#endif

    };

#endif      // C_CSCPPRESENCEHANDLER_H   
            
// End of File
