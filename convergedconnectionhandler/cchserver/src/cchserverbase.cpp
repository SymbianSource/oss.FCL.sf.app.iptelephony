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
* Description:  CCCHServerBase implementation
*
*/


// INCLUDE FILES
#include "cchlogger.h"
#include "cchserverbase.h"
#include "cchsession.h"
#include "cchrequeststorage.h"
#include "cchconnmonhandler.h"
#include "cchspshandler.h"
#include "cchservicehandler.h"
#include "cchpluginhandler.h"
#include "cchfeaturemanager.h"
#include "cchwakeupeventnotifier.h"
#include "cchstartupcounter.h"
#include "cchactivescheduler.h"
#include "cchconnmonhandlernotifier.h"

#include <ecom/ecom.h>
#include <rconnmon.h>

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES
// None

// CONSTANTS
// None

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
GLDEF_C TInt E32Main();
#ifdef _DEBUG
GLDEF_C void MemUsage();
#endif
// FORWARD DECLARATIONS
// None

// ============================= LOCAL FUNCTIONS =============================

// ---------------------------------------------------------------------------
// InitServerL Creates CCHServer.
// Returns: None.
// ---------------------------------------------------------------------------
//
static void InitServerL()
    {
    CCHLOGSTRING("CCCHServerBase::InitServerL");
    
    // This is only for startup situation, because starter does not use 
    // connect metod, which in normal situation reserves the mutex 
    RMutex serverStartMutex;
    CleanupClosePushL( serverStartMutex );    
    TInt createError( serverStartMutex.CreateGlobal( KCCHServerStartMutex ) );
    if ( KErrNone != createError )
        {
        User::LeaveIfError( serverStartMutex.OpenGlobal( KCCHServerStartMutex ) );
        }

    serverStartMutex.Wait();
    CCHLOGSTRING( "CCCHServerBase::InitServerL: mutex available" );
    
    TFullName name;
    TFindServer findServer( KCCHServerName );
    // Check that the server does not exists.
    if( findServer.Next( name ) == KErrNotFound )
        {
        User::LeaveIfError( User::RenameThread( KCCHServerName ) );
    
        // create and install the active scheduler we need
        CCchActiveScheduler* scheduler = new (ELeave) CCchActiveScheduler;
        CleanupStack::PushL( scheduler );
        CActiveScheduler::Install( scheduler ); 
    
        // create the server (leave it on the cleanup stack)
        CCCHServerBase* server = NULL;
        TRAPD( err, server = CCCHServerBase::NewL() );
    
        if ( KErrNone != err )
            {
            CCHLOGSTRING2
                ("InitServerL: Server creation failed, error = %d", err );
                
            server->PanicServer( ECCHErrCreateServer );
            }
        
        // Scheduler needs server, because server might needs to be restarted 
        scheduler->SetServer( server );
        // Initialisation complete, now signal the client
        RProcess::Rendezvous( KErrNone );
        
        // Release and close mutex
        CCHLOGSTRING( "CCCHServerBase::InitServerL: Release the mutex" );
        serverStartMutex.Signal();
        serverStartMutex.Close();
        
        // Ready to run
        CCHLOGSTRING( "CCCHServerBase::InitServerL: Server Running..." );
        CActiveScheduler::Start();
    
        // Cleanup the mutex, scheduler and server
        delete server;
        server = NULL;
    
        CleanupStack::PopAndDestroy( scheduler );
        }
    else
        {
        CCHLOGSTRING( "CCCHServerBase::InitServerL: cch server already exists" );
        RProcess::Rendezvous( KErrAlreadyExists );
        // Release and close mutex
        CCHLOGSTRING( "CCCHServerBase::InitServerL: Release the mutex" );
        serverStartMutex.Signal();
        serverStartMutex.Close();
        }
    CleanupStack::PopAndDestroy( &serverStartMutex );
    }

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// CCCHServerBase::CCCHServerBase
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CCCHServerBase::CCCHServerBase() :
    CPolicyServer( EPriorityHigh, KCCHPolicy ),
    iSessionCounter( 0 )
    {
    CCHLOGSTRING( "CCCHServerBase::CCCHServerBase" );
    iVoIPEmergencyNoteShown = EFalse;
    }

// ---------------------------------------------------------------------------
// CCCHServerBase::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CCCHServerBase::ConstructL()
    {
    CCHLOGSTRING( "CCCHServerBase::ConstructL" );
    iWakeUpEventNotifier = CCchWakeUpEventNotifier::NewL( *this );
    
    InitServerObjectsL();
    //Start server
    StartL( KCCHServerName );
    }

// ---------------------------------------------------------------------------
// CCCHServerBase::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CCCHServerBase* CCCHServerBase::NewL()
    {
    CCCHServerBase* self = CCCHServerBase::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CCCHServerBase::NewLC
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CCCHServerBase* CCCHServerBase::NewLC()
    {
    CCCHServerBase* self = new (ELeave)CCCHServerBase();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// Destructor
CCCHServerBase::~CCCHServerBase()
    { 
    delete iStartupCounter;
    delete iWakeUpEventNotifier;
    
    ReleaseAllResources();
    REComSession::FinalClose();
    }

// ---------------------------------------------------------------------------
// CCCHServerBase::NewSessionL
// This is called by RSessionBase (base class for client interface).
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CSession2* CCCHServerBase::NewSessionL( const TVersion& aVersion,
    const RMessage2& /* aMessage */ ) const
    {
    CCHLOGSTRING( "CCCHServerBase::NewSessionL" );

    TVersion version( KCCHServMajorVersionNumber,
                      KCCHServMinorVersionNumber,
                      KCCHServBuildVersionNumber );

    if ( !User::QueryVersionSupported( version , aVersion ) )
        {
        User::Leave( KErrNotSupported );
        }
    CCCHSession* session = 
        CCCHSession::NewL( const_cast<CCCHServerBase&>( *this ) );
    return session;
    }

// ---------------------------------------------------------------------------
// CCCHServerBase::ShutdownServerL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHServerBase::ShutDownServerL()
    {
    CCHLOGSTRING( "CCCHServerBase::ShutDownServerL - IN" );
         
    if ( !iSessionCounter && IsServerShutdownAllowedL() )
        {
        CCHLOGSTRING( "CCCHServerBase::ShutDownServerL - Allowed" );
        // if session counter is zero -> close server
        // MSI: Shouldn't we continue in minimal mode?
        // CActiveScheduler::Current()->Stop();
        TRAP_IGNORE( StartMinimalServerL() );
        }
    CCHLOGSTRING( "CCCHServerBase::ShutDownServerL - OUT" );
    }

// ---------------------------------------------------------------------------
// CCCHServerBase::InitServerL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHServerBase::InitServerObjectsL()
    {
    CCHLOGSTRING( "CCCHServerBase::InitServerObjectsL: IN" );

    ServiceHandler().InitServiceHandlerL();

    if ( iServiceHandler->IsStartupFlagSet() )
        {
        // is snap ready to proceed startup
        ConnMonHandler().ScanNetworks( ETrue, this );
        }
    else
        {
        // Startup flag is not ON, we have to be sure that startup counter 
        // is truly zero for the next time when startup flag is ON
        ResetStartupCounterL();
        StartMinimalServerL();
        }
    CCHLOGSTRING( "CCCHServerBase::InitServerObjectsL: OUT" );
    }

// ---------------------------------------------------------------------------
// CCCHServerBase::ServiceStartupL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHServerBase::ServiceStartupL()
    {
    CCHLOGSTRING( "CCCHServerBase::ServiceStartupL: IN" );
    // Start monitoring startup flag registration, this may set startup
    // flag to OFF if crashes happens more than KCCHMaxStartupCount during
    // startup flag registration
    TRAP_IGNORE( CreateStartupCounterL() );
    // If CCH cannot load the Plug-ins, CCH can still
    // try to load them later
    TRAP_IGNORE( iServiceHandler->LoadPluginsL() );
    RequestStorage().ScanNetworks();
    // initialization is now done. update states and send notification to
    // all clients
    iServerObjectsInit = ETrue;
    iServiceHandler->UpdateL();
    CCHLOGSTRING( "CCCHServerBase::ServiceStartupL: OUT" );
    }

// ---------------------------------------------------------------------------
// CCCHServerBase::StartMinimalServerL
// Start server in settings monitoring mode
// ---------------------------------------------------------------------------
//
void CCCHServerBase::StartMinimalServerL()
    {
    CCHLOGSTRING( "CCCHServerBase::StartMinimalServerL IN" );

    iWakeUpEventNotifier->StartL();
    ReleaseAllResources();
    CCHLOGSTRING( "CCCHServerBase::StartMinimalServerL OUT" );
    }

// ---------------------------------------------------------------------------
// CCCHServerBase::ReleaseAllResources
// Start server in settings monitoring mode
// ---------------------------------------------------------------------------
//
void CCCHServerBase::ReleaseAllResources()
    {
    CCHLOGSTRING( "CCCHServerBase::ReleaseAllResources" );

    delete iServiceHandler;
    iServiceHandler = NULL;
    delete iSPSHandler;
    iSPSHandler = NULL;
    delete iConnMonHandler;
    iConnMonHandler = NULL;
    delete iRequestStorage;
    iRequestStorage = NULL;
    delete iPluginHandler;
    iPluginHandler = NULL;
    delete iFeatureManager;
    iFeatureManager = NULL;
    delete iObjectConIx;
    iObjectConIx = NULL;
    }


// ---------------------------------------------------------------------------
// CCCHServerBase::NotifySessionCreatedL
// Increase session counter
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHServerBase::NotifySessionCreatedL()
    {
    CCHLOGSTRING( "CCCHServerBase::NotifySessionCreatedL" );
    
    iWakeUpEventNotifier->Stop();
    
    if ( iSessionCounter == 0 && IsServerShutdownAllowedL() )
    	{
        ServiceHandler().InitServiceHandlerL();
    	}
    iSessionCounter++;
    }

// ---------------------------------------------------------------------------
// CCCHServerBase::NotifySessionClosed
// Decrease session counter
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHServerBase::NotifySessionClosed()
    {
    CCHLOGSTRING( "CCCHServerBase::NotifySessionClosed" );
    if ( 0 < iSessionCounter )
        {
        iSessionCounter--;
        }
    if ( 0 >= iSessionCounter )
        {
        iSessionCounter = 0;        
        TRAP_IGNORE( ShutDownServerL() );
        }
    }

// ---------------------------------------------------------------------------
// CCCHServerBase::PanicClient
// Panic client with given error code.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHServerBase::PanicClient(
    const RMessage2& aMessage,
    TInt aPanic )
    {
    CCHLOGSTRING2("CCCHServerBase::PanicClient: Panic = %d", aPanic );
    _LIT( KTxtServer, "CCH Server Client" );
    aMessage.Panic( KTxtServer, aPanic );
    CCHLOGSTRING("CCCHServerBase::PanicClient exiting" );
    }

// ---------------------------------------------------------------------------
// PanicServer implements server panic handler.
// Panic server with given error code.
// Returns: None.
// ---------------------------------------------------------------------------
//
void CCCHServerBase::PanicServer(
    TInt aPanic )
    {
    CCHLOGSTRING2("CCCHServerBase::PanicServer: Panic = %d", aPanic );
    _LIT( KTxtServerPanic, "CCH Server" );
    User::Panic( KTxtServerPanic, aPanic );
    CCHLOGSTRING("CCCHServerBase::PanicServer exit" );
    }

// ---------------------------------------------------------------------------
// CCCHServerBase::ConstructObject
// Create requested object and/or return reference to it.
// ---------------------------------------------------------------------------
//
template <class T>T& CCCHServerBase::ConstructObject( 
    CCCHServerBase* aThis,
    T*& aObject )
    {
    TInt error = KErrNone;
    if ( !aObject )
        {
        TRAP( error, aObject = T::NewL( *aThis ) );
        }
    
    if ( KErrNone != error )
        {
        // Failed to create object, have to Panic!
        PanicServer( ECCHErrCreateServer );
        }
    return *aObject;
    }

// ---------------------------------------------------------------------------
// CCCHServerBase::ConstructObject
// Create requested object and/or return reference to it.
// ---------------------------------------------------------------------------
//
template <class T>T& CCCHServerBase::ConstructObject( 
    T*& aObject )
    {
    TInt error = KErrNone;
    if ( !aObject )
        {
        TRAP( error, aObject = T::NewL() );
        }
    
    if ( KErrNone != error )
        {
        // Failed to create object, have to Panic!
        PanicServer( ECCHErrCreateServer );
        }
    return *aObject;
    }

TBool CCCHServerBase::VoIPEmergencyNoteShown()
	{
	return iVoIPEmergencyNoteShown;
	}

void CCCHServerBase::SetVoIPEmergencyNoteShown( TBool aShown )
	{
	iVoIPEmergencyNoteShown = aShown;
	}

// ---------------------------------------------------------------------------
// CCCHServerBase::ObjectContainerIx
// Return object container index.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CObjectConIx& CCCHServerBase::ObjectContainerIx()
    {
    return ConstructObject<CObjectConIx>( iObjectConIx );
    }

// ---------------------------------------------------------------------------
// CCCHServerBase::RequestStorage
// Return instance of request storage.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CCCHRequestStorage& CCCHServerBase::RequestStorage()
    {
    return ConstructObject<CCCHRequestStorage>( this, iRequestStorage );
    }

// ---------------------------------------------------------------------------
// CCCHServerBase::ConnMonHandler
// Return instance of ConnMonHandler.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CCCHConnMonHandler& CCCHServerBase::ConnMonHandler()
    {
    return ConstructObject<CCCHConnMonHandler>( this, iConnMonHandler );
    }
    
// -----------------------------------------------------------------------------
// CCCHServerBase::PluginHandler
// Return instance of plug-in handler.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCCHPluginHandler& CCCHServerBase::PluginHandler()
    {
    return ConstructObject<CCCHPluginHandler>( this, iPluginHandler );
    }

// ---------------------------------------------------------------------------
// CCCHServerBase::SPSHandler
// Return instance of SPSHandler.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CCCHSPSHandler& CCCHServerBase::SPSHandler()
    {
    return ConstructObject<CCCHSPSHandler>( this, iSPSHandler );
    }
    
// ---------------------------------------------------------------------------
// CCCHServerBase::ServiceHandler
// Return instance of ServiceHandler.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CCCHServiceHandler& CCCHServerBase::ServiceHandler()
    {
    return ConstructObject<CCCHServiceHandler>( this, iServiceHandler );
    }

// ---------------------------------------------------------------------------
// CCCHServerBase::FeatureManager
// Return instance of FeatureManager.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CCchFeatureManager& CCCHServerBase::FeatureManager()
    {
    return ConstructObject<CCchFeatureManager>( iFeatureManager );
    }
        
// ---------------------------------------------------------------------------
// CCCHServerBase::WakeUp
// Creates all required objects needed by full server mode.
// ---------------------------------------------------------------------------
//
void CCCHServerBase::WakeUp()
    {
    CCHLOGSTRING( "CCCHServerBase::WakeUp IN" );
    if ( iSessionCounter == 0 && !iServerObjectsInit )
    	{
	    TRAPD( err, InitServerObjectsL() );
	    if ( KErrNone != err )
	    	{
	        // Failed to create required objects, have to Panic!
	        PanicServer( ECCHErrCreateServer );
	    	}
    	}
    CCHLOGSTRING( "CCCHServerBase::WakeUp OUT" );
    }

// ---------------------------------------------------------------------------
// CCCHServerBase::CreateStartupCounterL
// 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHServerBase::CreateStartupCounterL()
    {
    if ( !iStartupCounter )
        {
        iStartupCounter = CCchStartupCounter::NewL( *this );
        }
    iStartupCounter->StartupOccuredL();
    }

// ---------------------------------------------------------------------------
// CCCHServerBase::ResetStartupCounterL
// 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHServerBase::ResetStartupCounterL()
    {
    if ( !iStartupCounter )
        {
        iStartupCounter = CCchStartupCounter::NewL( *this );
        }
    iStartupCounter->ResetStartupCounter();
    delete iStartupCounter;
    iStartupCounter = NULL;
    }

// ---------------------------------------------------------------------------
// CCCHServerBase::Restart
// 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHServerBase::Restart()
    {
    CCHLOGSTRING( "CCCHServerBase::Restart IN" );
    
    // Send notify to our client
    TRAP_IGNORE( iServiceHandler->HandleRestartL() );
    
    // Do we have to make reregistration
    if ( iServiceHandler->IsStartupFlagSet() )
        {
        delete iServiceHandler;
        iServiceHandler = NULL;
        delete iPluginHandler;
        iPluginHandler = NULL;
        TRAP_IGNORE( InitServerObjectsL() );
        }
    
    CCHLOGSTRING( "CCCHServerBase::Restart OUT" );
    }

// ---------------------------------------------------------------------------
// CCCHServerBase::IsServerShutdownAllowedL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CCCHServerBase::IsServerShutdownAllowedL()
    {
    CCHLOGSTRING( "CCCHServerBase::IsServerShutdownAllowedL" );
    
    TBool shutDownAllowed( ETrue );
    
    if ( iSPSHandler && iServiceHandler )  
        {
        // Get service IDs
        RArray<TUint> serviceIds;
        CleanupClosePushL( serviceIds );
        
        iSPSHandler->GetServiceIdsL( serviceIds );
        
        TCCHSubserviceState state( ECCHUninitialized );
  
        for ( TInt i( 0 ); i < serviceIds.Count(); i++ )
            {
            state = iServiceHandler->ServiceState( serviceIds[i] );

            if ( ECCHDisabled != state && ECCHUninitialized != state )
                {
                shutDownAllowed = EFalse;
                }
            }
    
        CleanupStack::PopAndDestroy( &serviceIds ); 
        }

    CCHLOGSTRING2("CCCHServerBase::IsServerShutdownAllowedL: shutDownAllowed = %d", shutDownAllowed );
    return shutDownAllowed;
    }

// ---------------------------------------------------------------------------
// From MCCHConnMonHandlerNotifier
// CCCHServerBase::NetworkScanningCompletedL
// ---------------------------------------------------------------------------
//
void CCCHServerBase::NetworkScanningCompletedL(
    const TConnMonSNAPInfo& aSNAPs, TInt aError )
    {
    CCHLOGSTRING2( "CCCHServerBase::NetworkScanningCompletedL error = %d", aError );

    if ( KErrNone == aError && aSNAPs.iCount  )
        {
        ServiceStartupL();
        }
    else if ( KErrNone == aError || KErrNotReady == aError )
        {
        // No SNAPs available. Start listen to availability change
        ConnMonHandler().SetSNAPsAvailabilityChangeListener( this );
        }
    else
        {
        // exceptional error occured
        ResetStartupCounterL();
        StartMinimalServerL();
        }
    }

// ---------------------------------------------------------------------------
// From MCCHConnMonHandlerNotifier
// CCCHServerBase::SNAPsAvailabilityChanged
// ---------------------------------------------------------------------------
//
void CCCHServerBase::SNAPsAvailabilityChanged( TInt aError )
    {
    CCHLOGSTRING2( "CCCHServerBase::SNAPsAvailabilityChanged error = %d", aError );
    
    // Stop event receiving
    ConnMonHandler().SetSNAPsAvailabilityChangeListener( NULL );
    
    if ( KErrNone == aError || KErrTimedOut == aError )
        {
        ServiceStartupL();
        }
    else
        {
        // exceptional error occured
        ResetStartupCounterL();
        StartMinimalServerL();
        }
    }

// ========================== OTHER EXPORTED FUNCTIONS =======================

// ---------------------------------------------------------------------------
// E32Main implements the executable entry function.
// Note that because the target type of the CCHServer module
// is EXEDLL, the entry point has different signature depending
// on the build platform.
// Creates a cleanup stack and runs the server.
// Returns: Zero
// ---------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    TInt error( KErrNoMemory );
    __UHEAP_MARK;
    CCHLOGSTRING( "CCCHServerBase E32Main" );
    CTrapCleanup* cleanup = CTrapCleanup::New();

    
    if ( cleanup )
        {
        TRAP( error, InitServerL() );
        delete cleanup;
        }

    __UHEAP_MARKEND;

    return error;
    }

//  End of File
