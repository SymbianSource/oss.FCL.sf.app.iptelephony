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

#ifndef C_SCPSIPCONNECTION_H
#define C_SCPSIPCONNECTION_H

#include <e32base.h>
#include <rconnmon.h>
#include <sipconnectionobserver.h>
#include <sipprofileregistryobserver.h>

#include "scpdefs.h"

class CSIPProfileRegistry;
class CSIPProfile;
class CSIPManagedProfileRegistry;
class MScpSipConnectionObserver;

/**
 *  Register/unregister protocol profiles.
 *
 *  @lib sipconnectionprovider.dll
 *  @since Series 60 3.2
 */
class CScpSipConnection : public CBase,
                          public MSIPConnectionObserver  
    {
public:

    enum TConnectionState
        {
        EUnknown = 0,
        ERegistering,
        ERegistered,
        EDeregistering,
        EDeregistered,
        };

    enum TRegistrationRequestState
        {
        ENoRequest = 0,
        ERegistrationRequested,
        EDeregistrationRequested
        };

    /**
     * Two-phased constructor.
     */
    static CScpSipConnection* NewL( TInt aProfileId, 
                                    CSIPProfileRegistry& aProfileRegistry,
                                    CSIPManagedProfileRegistry& aManagedProfileRegistry,
                                    CSIP& aSip );
    /**
     * Destructor.
     */
    virtual ~CScpSipConnection();

    /**
     * Return sip profile id of this connection
     * @return Profile id
     */  
    TInt ProfileId() const;

    /**
     * Enables the connection
     */  
    void EnableL();

    /**
     * Disables the connection.
     * @return Symbian error codes
     */  
    TInt Disable();
    
    /**
     * Disables the connection ungracefully.
     * @return Symbian error codes
     */  
    TInt ForceDisable();

    /**
     * Returns state of deregistration request
     * @return Registration state
     */
    TRegistrationRequestState RegistrationRequestState() const;

    /**
     * Get sip connection state
     * @return State
     */
    void GetState( TConnectionState& aState, TInt& aError ) const;

    /**
     * Adds observer for the connection
     * @param aObserver Observer
     */
    void AddObserver( MScpSipConnectionObserver& aObserver );

    /**
     * Removes observer from this connection
     * @param aObserver Observer
     * @return Symbian error codes
     */
    TInt RemoveObserver( MScpSipConnectionObserver& aObserver );

    /**
     * Gets Protocol Profile's IAP Id.
     * @return Iap id
     */                                     
    TInt GetIap( TUint32& aIapId ) const;

    /**
     * Gets Protocol Profile's SNAP Id.
     * @return Snap id.
     */                                     
    TInt GetSnap( TUint32& aSnapId ) const;

    /**
     * Gets Protocol Profile's Username..
     * @param aUsername  Username of profile.
     * @return KErrNone if succeed.
     */                                     
    TInt GetUsername( TDes8& aUsername ) const;

    /**
     * Gets Protocol Profile's domain.
     * @param aDomain  Domain of profile.
     * @return KErrNone if succeed.
     */                                     
    TInt GetDomain( TDes8& aDomain ) const;
                          
    /**
     * Gets Protocol Profile's User name from Contact header
     * @param aDomain  Domain of profile.
     * @return KErrNone if succeed.
     */                                     
    TInt GetContactHeaderUser( RBuf8& aContactHeaderUser ) const;

    /**
     * Get CSIPProfile
     * @return CSIPProfile
     */
    CSIPProfile& SipProfile() const;

    /**
     * Set Protocol Profile use this IAP.
     * @since Series 60 3.2
     * @param aIapId Iap Id.
     * @return KErrNone if succeed.
     */                                     
    TInt SetIap( TUint32 aIapId );   
                         
    /**
     * Set Protocol Profile use this SNAP.
     * @param aSnapId  Profile new SNAP Id.
     * @return KErrNone if succeed.
     */                                     
    TInt SetSnap( TUint32 aSnapId );  

    /**
     * Returns the type of the iap this connection's profile is using
     * @return Iap type
     */
    TScpIapType GetIapTypeL() const;

    /**
     * Profile registry error occurred
     * @param aProfileId Profile id
     * @param aError error code
     */ 
    void ProfileRegistryErrorOccurred( TInt aError );

    /**
     * Updates the sip connection for the used sip profile
     * @param aProfileId Profile id
     * @param aEvent Event
     */ 
    void ProfileRegistryEventOccurred( MSIPProfileRegistryObserver::TEvent aEvent );

    /**
     * Reserve/Free the used connection for a service's use.
     * For example a VoIP service might reserve the profile for a
     * duration of a VoIP call
     * @param aReserved Reserved
     */
    void SetReserved( TBool aReserved );

    /**
     * Returns information about if a connection is reserved or not
     * @return ETrue if reserved
     */
    TBool Reserved() const;

    /**
     * Raises the flag fot iap availability offered. Iap availability
     * offers may come from SIP during ALR. If the connection is reserved,
     * the ARL is not allowed and iap available offered is raised.
     * When the connection is freed the ALR can occur.
     */
    void SetIapAvailableOffered( TBool aIapAvailableOffered );

    /**
     * Returns the iap available offered flag
     * @return ETrue if iap available was offered
     */
    TBool IapAvailableOffered() const;

    /**
     * Sets flag that currenlty the profile is romaing
     */
    void SetProfileCurrentlyRoaming();
    
    /**
     * Handle ongoing alr
     */
    void HandleMigrationStarted();
    
    /**
     * Handle alr error
     * @param aError the error that occurred
     * @param aProfileId identifies the SIP profile related to the error
     * @param aSnapId the SNAP related to the event
     * @param aIapId the IAP related to the error
     */
    void HandleMigrationError( TInt aError,
        TUint32 aProfileId, TUint32 aSnapId, TUint32 aIapId ); 

    /**
     * Password check.
     * @return returns always ETrue because information cannot
     *         be checked from sip
     */
    TBool IsPasswordSet() const;
    
    /**
     * Username check.
     * Checks if digest username is set and username is in aor.
     * @return ETrue when username exists
     */
    TBool IsUsernameSetL() const;
    
    /**
     * Checks if SNAP connection method used by the SIP profile is
     * available
     * @return ETrue if available
     */
    TBool IsSnapConnectionAvailable( TInt aSnapId ) const;
    
    /**
     * Checks if IAP connection method used by sip profile is
     * available
     * @return ETrue if available
     */
    TBool IsIapConnectionAvailable( TInt aIapId ) const;
    
    /**
     * Gets bearer filttering setting used by sip profile.
     * 0 --> Not Specified.
     * 1 --> WLAN Bearer Only.
     * 2 --> Cellular Data Bearer Only.
     * @param aBearerFilttering bearer filttering setting
     * 
     * @return KErrNone if succeed.
     */
    TInt BearerFiltteringSetting( TUint32& aBearerFilttering ) const;

#ifdef _DEBUG
    void GetInfo( TDes& aInfo ) const;
#endif

private:

    // From MSIPConnectionObserver
    void IncomingRequest( CSIPServerTransaction* /*aTransaction*/ ) {};

    void IncomingRequest( CSIPServerTransaction* /*aTransaction*/,
                          CSIPDialog& /*aDialog*/ ) {};

    void IncomingResponse( CSIPClientTransaction& /*aTransaction*/ ) {};
    
    void IncomingResponse( CSIPClientTransaction& /*aTransaction*/,
                            CSIPDialogAssocBase& /*aDialogAssoc*/ ) {};

    void IncomingResponse( CSIPClientTransaction& /*aTransaction*/,
                            CSIPInviteDialogAssoc* /*aDialogAssoc*/ ) {};

    void IncomingResponse( CSIPClientTransaction& /*aTransaction*/,
                           CSIPRegistrationBinding& /*aRegistration*/ ) {};

    void ErrorOccured( TInt /*aError*/, CSIPTransactionBase& /*aTransaction*/ ) {};

    void ErrorOccured( TInt /*aError*/,
                       CSIPClientTransaction& /*aTransaction*/,
                       CSIPRegistrationBinding& /*aRegistration*/ ) {};

    void ErrorOccured( TInt /*aError*/,
                       CSIPTransactionBase& /*aTransaction*/,
                       CSIPDialogAssocBase& /*aDialogAssoc*/ ) {};

    void ErrorOccured( TInt /*aError*/, CSIPRefresh& /*aSIPRefresh*/ ) {};


    void ErrorOccured( TInt /*aError*/,                    
                       CSIPRegistrationBinding& /*aRegistration*/ ) {};

    void ErrorOccured( TInt /*aError*/,            
                       CSIPDialogAssocBase& /*aDialogAssoc*/) {};

    void InviteCompleted( CSIPClientTransaction& /*aTransaction*/ ) {};

    void InviteCanceled( CSIPServerTransaction& /*aTransaction*/ ) {};        

    void ConnectionStateChanged( CSIPConnection::TState aState );

    /**
     * Checks if connection method used by the sip profile is
     * available
     * @return ETrue if available
     */
    TBool IsNetworkConnectionAvailable() const;

private:

    /**
     * C++ default constructor.
     */
    CScpSipConnection( TInt aProfileId, 
                       CSIPProfileRegistry& aProfileRegistry,
                       CSIPManagedProfileRegistry& aManagedProfileRegistry,
                       CSIP& aSip );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();
    
    /**
     * Starts timeout timer
     * @param aFunction The function to call after the time out
     */
    void StartEnableTimeoutTimer( TInt (*aFunction)(TAny* aPtr) );

    /**
     * Cancels the disable timer
     */
    void CancelEnableTimeoutTimer();
    
    /**
     * Profile register timeout callback
     * @param aSelf this object
     */
    static TInt EnableTimeout( TAny* aSelf );
    
    /**
     * Handles sip profile register timeout
     */
    void HandleEnableTimeout();
    
    /**
     * Changes SIP profile from "Always on" to "When needed" if
     * the profile was originally set to "Always on"
     */
    void DisableAlwaysOnModeL();
    
    /**
     * Checks if Sip profile type is IMS
     */
    TBool SipProfileIMS() const;
    
private:

    /**
     * Sip profile id
     */
    TInt iProfileId;

    /**
     * Sip profile registry
     */
    CSIPProfileRegistry& iProfileRegistry;

    /**
     * Managed sip profile registry
     */
    CSIPManagedProfileRegistry& iManagedProfileRegistry;
  
    /**
     * SIP instance
     */  
    CSIP& iSip;
    
    /**
     * Sip profile. May change. Own
     */    
    CSIPProfile* iSipProfile;

    /**
     * Observer
     */
    MScpSipConnectionObserver* iObserver;

    /**
     * Registration request state
     */
    TRegistrationRequestState iRegistrationRequestState;

    /**
     * Registration faile flag
     */
    TInt iConnectionStateError;

    /**
     * Timeout timer. Owned.
     */
    CPeriodic* iEnableTimeoutTimer;

    /**
     * Connection reserved for services' usage
     */
    TBool iReserved;

    /**
     * Iap available offered
     */
    TBool iIapAvailableOffered;

    /**
     * Profile is currently roaming
     */
    TBool iProfileCurrentlyRoaming;

    /**
     * Connection monitor
     */
    RConnectionMonitor iConnectionMonitor;

    /**
     * SIP connection
     * Own.
     */
    CSIPConnection* iSipConnection;

// This need to be cleaned to separate macro/header
#ifdef _DEBUG
    friend class T_CScpSipConnection; 
    friend class T_CScpProfileHandler;
    friend class T_CScpServiceManager;
    friend class T_CScpVmbxHandler;
    friend class T_CScpVoipHandler;
#endif
    };

#endif      // C_SCPSIPCONNECTION_H   
            
// End of File
