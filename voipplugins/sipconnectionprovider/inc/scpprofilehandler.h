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


#ifndef C_SCPPPROFILEHANDLER_H
#define C_SCPPPROFILEHANDLER_H

#include <sipprofileregistryobserver.h>
#include <sipconnectionobserver.h>
#include <sipobserver.h>
#include <sipprofilealrobserver.h>

#include "scpsipconnectionobserver.h"
#include "scpalrobserver.h"
#include "scpdefs.h"
#include "scpsipconnection.h"
#include "scpalrobserver.h"

class CSIP;
class CSIPProfileRegistry;
class CSIPManagedProfileRegistry;
class CSIPManagedProfile;
class CSipProfileAlrController;
class CScpSipConnection;
class MIpVmbxObserver;
class CIpVmbxInterface;

/**
 *  Register/unregister protocol profiles.
 *
 *  @lib sipconnectionprovider.dll
 *  @since Series 60 3.2
 */
class CScpProfileHandler : public CBase,
                           public MSIPProfileRegistryObserver,
                           public MSIPObserver,
                           public MSipProfileAlrObserver,
                           public MScpSipConnectionObserver
    {
public:

    /**
     * Enumeration for ALR events.
     */
    enum TAlrEvent
        {
        EScpAlrAllowMigration,
        EScpAlrDisallowMigration,
        EScpAlrRefresh
        };
    
public:  // Constructors and destructor
    
    /**
     * Two-phased constructor.
     */
    static CScpProfileHandler* NewL();

    /**
     * Destructor.
     */
    virtual ~CScpProfileHandler();

public: 

    /**
     * Adds Sip profile observer.
     * @param aObserver SIP profile observer.
     */
    void AddObserverL( MScpSipConnectionObserver& aObserver );   
            
    /**
     * Remove observer from the array.
     * @param aObserver SIP profile observer.
     * @return KErrNone If succeed.
     */
    TInt RemoveObserver( MScpSipConnectionObserver& aObserver );   
    
    /**
     * Get current state of scp connection having given sip profile id
     * @param aProfileId SIP profile id.
     * @param aState Current state.
     * @return KErrNone if succeed.
     */
    TInt GetCurrentState( TUint32 aProfileId, 
        CScpSipConnection::TConnectionState& aState,
        TInt& aError ) const;    
    
    /**
     * Register profile to SIP.
     * @param aProfileId SIP Profile id to register.
     * @return Symbian error codes.
     */
    void RegisterProfileL( TUint32 aProfileId );

    /**
     * UnRegister profile to SIP.
     * @param aProfileId SIP Profile id to unregister.
     * @return Symbian error codes.
     */
    TInt UnregisterProfile( TUint32 aProfileId );

     /**
     * Cancels any ongoing registration
     * @param aProfileId SIP Profile id to unregister.
     * @return Symbian error codes.
     */
    TInt CancelRegistration( TUint32 aProfileId );
                                       
    /**
     * Return existence info of profile id.
     * @param aProfileId Protocol profile id.
     * @return ETrue if profile exist.
     */
    TBool ProfileExists( TUint32 aProfileId ) const;  
    
    /**
     * Uses SIP ALR controller for ALR events.
     * @param aProfileId  Profile id.
     * @param aEvent      New ALR event.
     * @param aIapId      IAP id.
     */
    void DoAlrEventL( TUint32 aProfileId,                      
                      TAlrEvent aEvent,
                      TUint32 aIapId = NULL );

    /**
     * Checks if sip connection for give profile id exists
     * @param aProfileId Profile id.
     * @return ETrue if exists
     */
    TBool SipConnectionExists( TUint32 aProfileId ) const;

    /**
     * Gets a asip connection for given profile id
     * @param aProfileId Profile id.
     * @return Sip connection. Ownership is not transferred.
     */
    CScpSipConnection* GetSipConnection( TUint32 aProfileId ) const;
       
    /**
     * Creates a sip connection for given profile id
     * @param aProfileId Profile id.
     * @return Sip connection. Ownership is transferred.
     */
    CScpSipConnection* CreateSipConnectionL( TUint32 aProfileId ) const;  

    /**
     * Sets a sip profile reserved state
     * @param aProfileId Sip profile id
     * @param aReserved Reserved
     */
    void SetSipProfileReserved( TUint32 aProfileId, TBool aReserved );

    /**
     * Return VMBX service provider interface
     * @param aObserver VMBX service provider observer
     * @return Interface
     */
    CIpVmbxInterface& VmbxInterfaceL( MIpVmbxObserver& aObserver );
    
    /**
     * Delete VMBX service provider interface, there is no other way to remove 
     * MIpVmbxObserver. Observer has to remove when deleting vmbx subservice.
     */
    void DeleteVmbxInterface();
    
    /**
     * Checks if a sip profile requires update
     * @param aProfileId SIP profile ID
     * @param aTerminalType indicates is terminal type defined
     * @param aWlanMac indicates is wlan mac defined
     * @param aStringLength length of free string
     */    
    void UpdateSipProfileL( TUint32 aProfileId, TBool aTerminalType,
        TBool aWlanMac, TInt astringLength );
 
    /**
     * Set username and/or passowrd.
     * @param aProfileId Profile id to use.
     * @param aUsername to set
     * @param aSetUsername If true, set username.
     * @param aPassword Password to set.
     * @param aSetPassword If true, set password.
     * @@return none 
     * @leave KErrArgument if passed values are incorrect, or other 
     * symbian error code. 
     */
    void SetUsernameAndPasswordL( TUint32 aProfileId,
                                  const TDesC8& aUsername,
                                  TBool aSetUsername,
                                  const TDesC8& aPassword,
                                  TBool aSetPassword );
    
    /**
     * Informs SIP to allow ALR migration
	 *
     * @param aProfileId SIP profile ID
     */    
    void StartAlrMigration( TUint32 aProfileId );
    
#ifdef _DEBUG
    void GetDebugInfo( TDes& aInfo ) const;
#endif                                          

        
private: // Functions from base classes

    /**
     * From MSIPObserver
     * A SIP request has been received from the network.        
     * @pre aTransaction != 0
     * @param aIapId The IapId from which the SIP request was received.
     * @param aTransaction contains local address, remote address of a SIP
     *        message, as well as optional SIP message method, headers and
     *        body. The ownership is transferred.
     */
    void IncomingRequest( TUint32 aIapId, CSIPServerTransaction* aTransaction );

    /**
     * From MSIPObserver
     * The received SIP request time-outed and it is invalid i.e. cannot be used
     * anymore.
     * This will be called if the user fails to create a SIP connection
     * and does not send an appropriate SIP response.
     * @param aTransaction The time-outed transaction.
     */
    void TimedOut( CSIPServerTransaction& aTransaction );


    /** 
     * From MSIPProfileRegistryObserver
     * An event related to SIP Profile has accorred
     * @param aProfileId a profile Id
     * @param aEvent an occurred event
     */
    void ProfileRegistryEventOccurred( TUint32 aProfileId,
                                       MSIPProfileRegistryObserver::TEvent aEvent );

    /**
     * From MSIPProfileRegistryObserver
     * An asynchronous error has occurred related to SIP profile
     * Event is send to those observers, who have the
     * corresponding profile instantiated.
     * @param aProfileId the id of failed profile 
     * @param aError an occurred error
     */
    void ProfileRegistryErrorOccurred( TUint32 aProfileId,
                                       TInt aError );
                                   
    /**
     * ALR related event occurred. See MSipProfileAlrObserver.
     * @param aEvent the event that occurred
     * @param aProfileId identifies the SIP profile related to the event
     * @param aSnapId the SNAP related to the event
     * @param aIapId the IAP related to the event
     */
    void AlrEvent( MSipProfileAlrObserver::TEvent aEvent,
                   TUint32 aProfileId,
                   TUint32 aSnapId,
                   TUint32 aIapId );
                           
    /**
     * ALR related error occurred. See MSipProfileAlrObserver.
     * @param aError the error that occurred
     * @param aProfileId identifies the SIP profile related to the error
     * @param aSnapId the SNAP related to the event
     * @param aIapId the IAP related to the error
     */
    void AlrError( TInt aError,
                   TUint32 aProfileId,
                   TUint32 aSnapId,
                   TUint32 aIapId );

    /**
     * Sip profile state changed
     * @param aProfileId Profile id.
     * @param aSipEvent A new event of profile.
     */  
    void HandleSipConnectionEvent( TUint32 aProfileId,
                                   TScpConnectionEvent aSipEvent );
    
    /**
     * Query is starting to ALR allowed.
     * Client can start ALR later if needed to do something first
     * 
     * @return ETrue if allowed.
     */
    TBool IsSipProfileAllowedToStartAlr();
   
private:

    /**
     * C++ default constructor.
     */
    CScpProfileHandler();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * Removes a sip connection from iSipConnections
     * @param aProfilId Sip profile id
     * @return Symbian error codes
     */
    TInt RemoveSipConnection( TUint32 aProfileId );

    /**
     * Checks if sip profile is reserved by some service
     * @param aProfileId Sip profile id
     * @return ETrue if reserved
     */
    TBool IsSipProfileReserved( TUint32 aProfileId ) const;

private:
    
    /**
     * SIP instance. Own.
     */
    CSIP* iSip;
    
    /**
     * SIP profile registry. Own.
     */
    CSIPProfileRegistry* iProfileRegistry;
    
    /**
     * SIP managed profile registry. Own.
     */
    CSIPManagedProfileRegistry* iManagedProfileRegistry;
    
    /**
     * SIP ALR controller. Own.
     */
    CSipProfileAlrController* iAlrController;

    /**
     * Sip connections.
     */
    RPointerArray< CScpSipConnection > iSipConnections;
        
    /**
     * SIP profile observers.
     */
    RPointerArray< MScpSipConnectionObserver > iObservers;

    /**
     * VMBx service provider interface. Created once, when needed.
     */
    CIpVmbxInterface* iVmbxInterface;

    /**
	 * New IapId received in ALR event
	 */
    TUint iNewAlrIapId;
    
    /**
     * Is ALR Migration Allowed To Start Immediately
     */
    TBool iAlrAllowedToStartImmediately;
    
// This need to be cleaned to separate macro/header
#ifdef _DEBUG
    friend class T_CScpServiceManager;
    friend class T_CScpProfileHandler;
    friend class T_CScpSipHelper;
    friend class T_CScpVmbxHandler;
    friend class T_CScpVoipHandler;
    friend class T_CScpPresenceHandler;
    friend class T_CScpSettingHandler;
    friend class T_CScpImHandler;
#endif
    };

#endif      // CSCPPPROFILEHANDLER_H   
            
// End of File
