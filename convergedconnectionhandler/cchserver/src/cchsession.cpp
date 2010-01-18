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
* Description:  CCCHSession implementation
*
*/


// INCLUDE FILES
#include "cchlogger.h"
#include "cchsession.h"
#include "cchserverbase.h"
#include "cchsubsession.h"
#include "cchclientserverinternal.h"

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
// CCCHSession::CCCHSession
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CCCHSession::CCCHSession( CCCHServerBase& aServer ) :
    CSession2(),
    iCCHServer( aServer )
    {
    TRAP_IGNORE( iCCHServer.NotifySessionCreatedL() );
    }

// ---------------------------------------------------------------------------
// CCCHSession::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CCCHSession::ConstructL()
    {
    iSubsessions = iCCHServer.ObjectContainerIx().CreateL();
    iObjectIx = CObjectIx::NewL();
    }

// ---------------------------------------------------------------------------
// CCCHSession::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CCCHSession* CCCHSession::NewL( CCCHServerBase& aServer )
    {
    CCCHSession* self = CCCHSession::NewLC( aServer );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CCCHSession::NewLC
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CCCHSession* CCCHSession::NewLC( CCCHServerBase& aServer )
    {
    CCCHSession* self = new (ELeave) CCCHSession( aServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// Destructor
CCCHSession::~CCCHSession()
    {   
    delete iObjectIx;
    iObjectIx = NULL;   

    iCCHServer.ObjectContainerIx().Remove( iSubsessions );
    iCCHServer.NotifySessionClosed(); 
    }

// ---------------------------------------------------------------------------
// CCCHSession::ServiceL
// Handles request received from client.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHSession::ServiceL( const RMessage2& aMessage )
    {
    CCHLOGSTRING( "CCCHSession::ServiceL: IN" );
    TInt command = aMessage.Function();
    switch ( command )
        {
        case ECCHOpenSubSession:
            {
            CCCHSubsession* subsession = CCCHSubsession::NewLC( iCCHServer );
            iSubsessions->AddL( subsession );
            const TInt handle( iObjectIx->AddL( subsession ) );
            CleanupStack::Pop( subsession );
            TPckg<TInt> handlePckg( handle );
            TRAPD( err, aMessage.WriteL( 3, handlePckg ) );
            if ( KErrNone != err )
                {
                // Panic client
                iObjectIx->Remove( handle );
                iCCHServer.PanicClient( aMessage, ECCHErrSubSessionOpen );
                }
            aMessage.Complete( KErrNone );
            }
            break;
        case ECCHCloseSubSession:
            {
            const TInt handle( aMessage.Int3() );
            if ( iObjectIx->At( handle ) )
                {
                iObjectIx->Remove( handle );
                }
            else
                {
                // Panic client. Handle was not valid
                iCCHServer.PanicClient( aMessage, ECCHErrSubSessionClose );
                }
            aMessage.Complete( KErrNone );
            }
            break;
        /**
         * CCH subsession functions
         */
        case ECCHSubscribeToEvents:
        case ECCHSubscribeToEventsCancel:
        case ECCHGetServices:
        case ECCHGetServicesCancel:
        case ECCHGetServiceState:
        case ECCHEnableService:
        case ECCHEnableServiceCancel:
        case ECCHDisableService:
        case ECCHDisableServiceCancel:
        case ECCHGetServiceInfo:
        case ECCHGetPreferredService:
        case ECCHSetConnectionInfo:
        case ECCHSetConnectionInfoCancel:
        case ECCHGetConnectionInfo:
        case ECCHGetConnectionInfoCancel:
        case ECCHReserveService:
        case ECCHFreeService:
        case ECCHIsReserved:
        case ECCHServiceCount:
        case ECCHServerRegister:
        case ECCHServerRegisterCancel:
            {
            CCCHSubsession* subsession = static_cast<CCCHSubsession*>(
                iObjectIx->At( aMessage.Int3() ) );
            if ( subsession )
                {
                subsession->ServiceL( aMessage );
                }
            else
                {
                // Panic client.
                iCCHServer.PanicClient( aMessage, ECCHBadDescriptor );
                }
            }
            break;
        default:
            {
            iCCHServer.PanicClient( aMessage, ECCHBadRequest );
            }

        }
    CCHLOGSTRING( "CCCHSession::ServiceL: OUT" );
    }

// ========================== OTHER EXPORTED FUNCTIONS =======================

//  End of File
