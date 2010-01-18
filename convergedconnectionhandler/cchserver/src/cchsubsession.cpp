/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CCCHSubsession implementation
*
*/


// INCLUDE FILES
#include <cchclientserver.h>

#include "cchlogger.h"
#include "cchsubsession.h"
#include "cchserverbase.h"
#include "cchrequeststorage.h"
#include "cchpluginhandler.h"
#include "cchservicehandler.h"

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
// None

// FORWARD DECLARATIONS
// None

// ============================= LOCAL FUNCTIONS =============================

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// CCCHSubsession::CCCHSubsession
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CCCHSubsession::CCCHSubsession( CCCHServerBase& aServer ) :
    iServer( aServer ),
    iMessage( NULL ),
    iQueueEvents( EFalse )
    {
    }

// ---------------------------------------------------------------------------
// CCCHSubsession::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CCCHSubsession* CCCHSubsession::NewL( CCCHServerBase& aServer )
    {
    CCCHSubsession* self = CCCHSubsession::NewLC( aServer );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CCCHSubsession::NewLC
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CCCHSubsession* CCCHSubsession::NewLC( CCCHServerBase& aServer )
    {
    CCCHSubsession* self = new (ELeave)CCCHSubsession( aServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// Destructor
CCCHSubsession::~CCCHSubsession()
    {
    iServer.RequestStorage().RemoveRequestsBySession( this );
    iServer.RequestStorage().RemoveSession( this );
    iEventQueue.Close();
 
    delete iRegisterMessage;
    delete iMessage;            
    }


// ---------------------------------------------------------------------------
// CCCHSubsession::ConstructL
// ---------------------------------------------------------------------------
//
void CCCHSubsession::ConstructL(  )
    {
    CCHLOGSTRING( "CCCHSubsession::ConstructL: IN" );
    
    iServer.RequestStorage().AddSession( this );
    
    CCHLOGSTRING( "CCCHSubsession::ConstructL: OUT" );
    }

// ---------------------------------------------------------------------------
// CCCHSubsession::ServiceEventOccured
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHSubsession::ServiceEventOccured(
    TServiceStatus aNewStatus )
    {
    CCHLOGSTRING( "CCCHSubsession::ServiceEventOccured: IN" );
    //if are we queuing
    if( iQueueEvents )
        {
        //check if we can accept this event
        if ( 0 == iSubscribedService.iServiceId ||        // all services
        		iSubscribedService.iServiceId == aNewStatus.ServiceId() )  // specified service
            {
            if ( iSubscribedService.iType == ECCHUnknown ||   
            		iSubscribedService.iType == aNewStatus.Type() )        // subservice specified
                {
                iEventQueue.Append( aNewStatus );
                CompleteMessage( );
                }
            }
        }
    
    CCHLOGSTRING( "CCCHSubsession::ServiceEventOccured: OUT" );
    }

// ---------------------------------------------------------------------------
// CCCHSubsession::CompleteMessage
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHSubsession::CompleteMessage( )
    {
    CCHLOGSTRING( "CCCHSubsession::CompleteMessage: IN" );
    //if there is pending SubscribeToEvents and we have something in queue
    if( iMessage && 0 < iEventQueue.Count() )
        {
        TInt error( KErrNone );
        TPckgBuf<TServiceStatus> serviceStatus;
        
        // while we append to queue, we consume from the start (fifo)
        serviceStatus() = iEventQueue[ 0 ];   
        CCHLOGSTRING2( "CCCHSubsession::CompleteMessage: type  : %d", serviceStatus().Type() );
        CCHLOGSTRING2( "CCCHSubsession::CompleteMessage: state : %d", serviceStatus().iState );
        // Write status to IPC message
        TRAP( error, iMessage->WriteL( 2, serviceStatus ) );    
        
        //complete & delete message 
        iMessage->Complete( error );
        delete iMessage;
        iMessage = NULL;
        
        // Remove consumed event from queue
        iEventQueue.Remove( 0 );
        }
    CCHLOGSTRING( "CCCHSubsession::CompleteMessage: OUT" );
    } 
       
// ---------------------------------------------------------------------------
// CCCHSubsession::ServiceL
// Handles request received from client.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHSubsession::ServiceL( const RMessage2& aMessage )
    {
    CCHLOGSTRING( "CCCHSubsession::ServiceL: IN" );
    TInt command = aMessage.Function();
    switch ( command )
        {
        case ECCHServerRegister:
            {
            if( !iRegisterMessage )
                {
                iRegisterMessage = new (ELeave) RMessage2( aMessage );
                }
            
            break;
            }
            
        case ECCHServerRegisterCancel:
            {
            if( iRegisterMessage )
                {
                iRegisterMessage->Complete( KErrCancel );
                delete iRegisterMessage;
                iRegisterMessage = NULL;
                }
            aMessage.Complete( KErrNone );
            break;
            }
        case ECCHSubscribeToEvents:
            {
            CCHLOGSTRING
                ( "CCCHSubsession::ServiceL :ECCHSubscribeToEvents" );
            iMessage = new (ELeave) RMessage2( aMessage );
            //set the service (or set of services) we are interested in
            iSubscribedService.iServiceId = iMessage->Int0();
            iSubscribedService.iType = static_cast<TCCHSubserviceType>(
                                                    iMessage->Int1() );
            //start queuing
            iQueueEvents = ETrue;
            //try to complete message
            CompleteMessage( );
            }
            break;
        case ECCHSubscribeToEventsCancel:
            {
            CCHLOGSTRING
                ( "CCCHSubsession::ServiceL :ECCHSubscribeToEventsCancel" );
            //if ECCHSubscribeToEvents request is pending, complete & delete it
            if( iMessage )
                {
                iMessage->Complete( KErrCancel );    
                delete iMessage;
                iMessage = NULL;
                }
            // Complete cancel request
            aMessage.Complete( KErrNone );
            //stop & reset the queue
            iQueueEvents = EFalse;
            iEventQueue.Reset();
            }
            break;
        case ECCHGetServices:
            {
            CCHLOGSTRING
                ( "CCCHSubsession::ServiceL :ECCHGetServices" );
            iServer.RequestStorage().AddRequestL( ECCHGetServices,
                aMessage, this );
            }
            break;
        case ECCHGetServicesCancel:
            {
            CCHLOGSTRING
                ( "CCCHSubsession::ServiceL :ECCHGetServicesCancel" );
            iServer.RequestStorage().CompleteRequest( ECCHGetServices,
                this, KErrCancel );
            aMessage.Complete( KErrNone );
            }
            break;
        case ECCHGetServiceState:
            {
            CCHLOGSTRING
                ( "CCCHSubsession::ServiceL :ECCHGetServiceState" );
            TInt err( KErrNone );
            err = iServer.ServiceHandler().GetServiceState( aMessage );
            aMessage.Complete( err );
            }
            break;
        case ECCHEnableService:
            {
            CCHLOGSTRING
                ( "CCCHSubsession::ServiceL :ECCHEnableService" );
            iServer.RequestStorage().AddRequestL( ECCHEnableService,
                aMessage, this );
            }
            break;
        case ECCHEnableServiceCancel:
            {
            CCHLOGSTRING
                ( "CCCHSubsession::ServiceL :ECCHEnableServiceCancel" );
            iServer.RequestStorage().CompleteRequest( ECCHEnableService,
                this, KErrCancel );
            aMessage.Complete( KErrNone );
            }
            break;
        case ECCHDisableService:
            {
            CCHLOGSTRING
                ( "CCCHSubsession::ServiceL :ECCHDisableService" );
            iServer.RequestStorage().AddRequestL( ECCHDisableService,
                aMessage, this );
            }
            break;
        case ECCHDisableServiceCancel:
            {
            CCHLOGSTRING
                ( "CCCHSubsession::ServiceL :ECCHDisableServiceCancel" );
            iServer.RequestStorage().CompleteRequest( ECCHDisableService,
                this, KErrCancel );
            aMessage.Complete( KErrNone );
            }
            break;
        case ECCHGetServiceInfo:
            {
            CCHLOGSTRING
                ( "CCCHSubsession::ServiceL :ECCHGetServiceInfo" );
            TInt err( KErrNone );
            err = iServer.ServiceHandler().GetServiceInfo( aMessage );
            aMessage.Complete( err );
            }
            break;
        case ECCHGetPreferredService:
            {
            CCHLOGSTRING
                ( "CCCHSubsession::ServiceL :ECCHGetPreferredService" );
            aMessage.Complete( KErrNotSupported );
            }
            break;
        case ECCHSetConnectionInfo:
            {
            CCHLOGSTRING
                ( "CCCHSubsession::ServiceL :ECCHSetConnectionInfo" );
            iServer.RequestStorage().AddRequestL( ECCHSetConnectionInfo,
                aMessage, this );
            }
            break;
        case ECCHSetConnectionInfoCancel:
            {
            CCHLOGSTRING
                ( "CCCHSubsession::ServiceL :ECCHSetConnectionInfoCancel" );
            iServer.RequestStorage().CompleteRequest( ECCHSetConnectionInfo,
                this, KErrCancel );
            aMessage.Complete( KErrNone );
            }
            break;
        case ECCHGetConnectionInfo:
            {
            CCHLOGSTRING
                ( "CCCHSubsession::ServiceL :ECCHGetConnectionInfo" );
            iServer.RequestStorage().AddRequestL( ECCHGetConnectionInfo,
                aMessage, this );
            }
            break;
        case ECCHGetConnectionInfoCancel:
            {
            CCHLOGSTRING
                ( "CCCHSubsession::ServiceL :ECCHGetConnectionInfoCancel" );
            iServer.RequestStorage().CompleteRequest( ECCHGetConnectionInfo,
                this, KErrCancel );
            aMessage.Complete( KErrNone );
            }
            break;
        case ECCHServiceCount:
            {
            iServer.ServiceHandler().ServiceCountL( aMessage );
            aMessage.Complete( KErrNone );
            }
            break;
        case ECCHReserveService:
            {
            CCHLOGSTRING
                ( "CCCHSubsession::ServiceL :ECCHReserveService" );
            TInt err( KErrNone );
            err = iServer.ServiceHandler().ReserveService( aMessage );
            aMessage.Complete( err );
            }
            break;
        case ECCHFreeService:
            {
            CCHLOGSTRING
                ( "CCCHSubsession::ServiceL :ECCHFreeService" );
            TInt err( KErrNone );
            err = iServer.ServiceHandler().FreeService( aMessage );
            aMessage.Complete( err );
            }
            break;
        case ECCHIsReserved:
            {
            CCHLOGSTRING
                ( "CCCHSubsession::ServiceL :ECCHIsReserved" );
            iServer.ServiceHandler().IsReserved( aMessage );
            aMessage.Complete( KErrNone );
            }
            break;
        default:
            {
            CCHLOGSTRING( "CCCHSubsession::ServiceL :ECCHBadRequest" );
            iServer.PanicClient( aMessage, ECCHBadRequest );
            }
        }
    CCHLOGSTRING( "CCCHSubsession::ServiceL: OUT" );
    }

// ========================== OTHER EXPORTED FUNCTIONS =======================

//  End of File
