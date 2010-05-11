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
* Description:  CCCHServerBase declaration
*
*/


#ifndef C_CCHSERVERBASE_H
#define C_CCHSERVERBASE_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <cchclientserver.h>
#include "cchwakeupeventobserver.h"
#include "cchconnmonhandlernotifier.h"

// CONSTANTS

// Security Policy
const TUint KCCHRangeCount = 2;

const TInt KCCHRanges[ KCCHRangeCount ] =
    {
    0,  /// Session
    24, // Non implemented requests (requests out of range)
    };

const TUint8 KCCHElementsIndex[ KCCHRangeCount ] =
    {
    0,
    CPolicyServer::ENotSupported,
    };

const CPolicyServer::TPolicyElement KCCHElements[] =
    {
        {
        _INIT_SECURITY_POLICY_C1( ECapabilityNetworkServices ),
            CPolicyServer::EFailClient
        },
        // Policy "0"; Fail call if ECapabilityNetworkServices not present
    };

const CPolicyServer::TPolicy KCCHPolicy =
    {
    CPolicyServer::EAlwaysPass,     // Connection attempts allowed
    KCCHRangeCount,                 // Range count
    KCCHRanges,                     // Ranges
    KCCHElementsIndex,              // Elements index
    KCCHElements,                   // Elements
    };

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CCCHRequestStorage;
class CCCHPluginHandler;
class CCCHSPSHandler;
class CCCHServiceHandler;
class CCCHConnMonHandler;
class CCchFeatureManager;
class CCchWakeUpEventNotifier;
class CCchStartupCounter;

// CLASS DECLARATION

/**
 *  CCCHServerBase declaration
 *  The server of CCH services.
 *  @lib cchserver.exe
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CCCHServerBase ) : public CPolicyServer,
			                          public MCchWakeUpEventObserver,
			                          public MCCHConnMonHandlerNotifier
    {

public: // Constructors and destructor

    /**
    * Two-phased constructor.
    */
    static CCCHServerBase* NewL();

    /**
     * Two-phased constructor.
     */
    static CCCHServerBase* NewLC();

    /**
     * Destructor.
     */
    virtual ~CCCHServerBase();

public: // New functions

    /**
     * Notify the server that a session has been created.
     * @since S60 3.2
     */
    void NotifySessionCreatedL();

    /**
     * Notify the server that a session has been closed.
     * @since S60 3.2
     */
    void NotifySessionClosed();

    /**
     * Panic the client.
     * @since S60 3.2
     * @param aMessage The panic message
     * @param aPanic The panic code.
     */
    void PanicClient( const RMessage2& aMessage, TInt aPanic );

    /**
     * Panic the server.
     * @since S60 3.2
     * @param aPanic The panic code.
     */
    void PanicServer( TInt aPanic );

    /**
     * Gives reference to container for object containers.
     * @since S60 3.2
     * @return Container for object containers.
     */
    CObjectConIx& ObjectContainerIx();

    /**
     * Return reference to CCCHRequestStorage
     * @since S60 3.2
     * @return Reference to CCCHRequestStorage
     */
    CCCHRequestStorage& RequestStorage();

    /**
     * Return reference to CCCHConnMonHandler
     * @since S60 3.2
     * @return Reference to CCCHConnMonHandler
     */
    CCCHConnMonHandler& ConnMonHandler();

    /**
     * Return reference to CCCHConnMonHandler
     * @since S60 3.2
     * @return Reference to CCCHConnMonHandler
     */
    CCCHSPSHandler& SPSHandler();
    
    /**
     * Return reference to CCCHPluginHandler
     * @since S60 3.2
     * @return Reference to CCCHPluginHandler
     */
    CCCHPluginHandler& PluginHandler();
    
    /**
     * Return reference to CCCHServiceHandler
     * @since S60 3.2
     * @return Reference to CCCHServiceHandler
     */
    CCCHServiceHandler& ServiceHandler();
    
    /**
     * Return reference to CCCHFeatureManager
     * @since S60 3.2
     * @return Reference to CCCHFeatureManager
     */
    CCchFeatureManager& FeatureManager();
    
    /**
     * Create startup counter.
     * @since S60 3.2
     */
    void CreateStartupCounterL();
             
    /**
     * Reset startup counter.
     * @since S60 3.2
     */
    void ResetStartupCounterL();

    /**
     * Indicates if the voip emergency note has been shown or not.
     * @since S60 3.2
     */
    TBool VoIPEmergencyNoteShown();
    
    /**
     * Sets the voip emergency note shown flag.
     * @since S60 3.2
     */
    void SetVoIPEmergencyNoteShown( TBool aShown );
    
    /**
     * Restarts server.
     * @since S60 3.2
     */
    void Restart();
    
public: // Methods from base classes

    /**
     * (From CServer2) Creates a new session for a client.
     *
     * @param aVersion (OUT) The version of the server.
     * @param aMessage IPC message
     * @return A new instance of CWLMSession.
     */
     virtual CSession2*  NewSessionL(
            const TVersion& aVersion,
            const RMessage2& aMessage ) const;
     
     /**
      * (From MCchWakeUpEventObserver) Creates a new session for a client.
      *
      * Switch server to monitoring mode
      */
     void WakeUp();

private: // From MCCHConnMonHandlerNotifier
    
    void NetworkScanningCompletedL( const TConnMonSNAPInfo& aSNAPs, TInt aError );
    
    void SNAPsAvailabilityChanged( TInt aError );
         
private:

    /**
     * C++ default constructor.
     */
    CCCHServerBase();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * Shutdowns server
     */
    void ShutDownServerL();

    /**
     * Initialize server and it's components
     */
    void InitServerObjectsL();
    
    /**
     * Switch server to monitoring mode
     */
    void StartMinimalServerL();
 
    TBool IsServerShutdownAllowedL();
    
    /**
     * Release all dynamic memory allocations and other resources
     */
    void ReleaseAllResources();
	
    /**
     * Starts service
     */
    void ServiceStartupL();
        
    template <class T>T& ConstructObject( CCCHServerBase* aThis, T*& aObject );
    template <class T>T& ConstructObject( T*& aObject );


private: // data

    /**
     * Session counter
     */
    TInt                iSessionCounter;

    /**
     * Object container factory for sessions
     */
    CObjectConIx*       iObjectConIx;

    /**
     * Storage for asynchronous requests.
     */
    CCCHRequestStorage* iRequestStorage;

    /**
     * Object for handling ConnMon
     */
    CCCHConnMonHandler* iConnMonHandler;

    /**
     * Object for handling Service Provider Settings
     */
    CCCHSPSHandler*     iSPSHandler;
    
    /**
     * Object for handling Plug-ins.
     */
    CCCHPluginHandler*  iPluginHandler;
    
    /**
     * Object for handling Services
     */
    CCCHServiceHandler* iServiceHandler;
    
    /**
     * Cch's feature manager
     */
    CCchFeatureManager* iFeatureManager;
    
    /**
     * Wake-up event notifer
     */
    CCchWakeUpEventNotifier *iWakeUpEventNotifier;
    
    /**
     * Counts startups
     */
    CCchStartupCounter* iStartupCounter;
    
    /**
     * Have we already shown the emergency call note?
     */    
    TBool iVoIPEmergencyNoteShown;
    
    /**
     * Have we already initialized the server objects?
     */    
    TBool iServerObjectsInit;  
    };

#endif // C_CCHSERVERBASE_H

// End of file
