/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  RCCHServer implementation
*
*/


// INCLUDE FILES
#include <cchclientserver.h>

#include "cchserver.h"
#include "cchlogger.h" 
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
const TUint8 KCCHServerStartAttempts = 2;

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ============================= LOCAL FUNCTIONS =============================

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// RCCHServer::RCCHServer
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
EXPORT_C RCCHServer::RCCHServer() : RSessionBase()
    {
    CCHLOGSTRING( "RCCHServer::RCCHClient" );    
    }

// ---------------------------------------------------------------------------
// RCCHServer::Connect
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RCCHServer::Connect()
    {
    CCHLOGSTRING( "RCCHServer::Connect" );
    TInt error( KErrNone );
    TUint8 retry = KCCHServerStartAttempts;
    
    for ( ;; )
        {
        error = CreateSession( KCCHServerName, 
                               Version(), 
                               KCCHMessageSlots );

        CCHLOGSTRING2
            ("RCCHServer CreateSession result: %d", error );
        
        if ( KErrNotFound != error && KErrServerTerminated != error )
            {
            break;
            }
        
        // Need to restart server
        if ( 0 == --retry )
            {
            break;
            }                    
        error = StartServer();
        CCHLOGSTRING2
            ( "RCCHServer::Connect, StartServer() returned %d", error );
        
        if ( KErrNone != error && KErrAlreadyExists != error )
            {
            break;
            }
        }

    return error;    
    }

// ---------------------------------------------------------------------------
// RCCHServer::Disconnect
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RCCHServer::Disconnect()
    {
    CCHLOGSTRING( "RCCHServer::Disconnect" );
    RHandleBase::Close();
    }

// ---------------------------------------------------------------------------
// RCCHServer::Version
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TVersion RCCHServer::Version() const
    {
    return ( TVersion( KCCHServMajorVersionNumber,
                       KCCHServMinorVersionNumber,
                       KCCHServBuildVersionNumber ) );
    }

// ---------------------------------------------------------------------------
// RCCHServer::StartServer
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RCCHServer::StartServer() const
    {
    CCHLOGSTRING( "RCCHServer::StartServer" );
    TInt error( KErrNone );
    RProcess server;
    const TUidType serverUid( KNullUid, KNullUid, KCCHServerUid );
    error = server.Create( KCCHServerExe, KNullDesC, serverUid );
    
    if ( KErrNone == error )
        {
        TRequestStatus status;
        server.Rendezvous( status );
        
        if ( status != KRequestPending )
            {
            server.Kill( 0 );   // abort startup
            }
        else
            {
            server.Resume();    // logon OK - start the server
            }
            
        CCHLOGSTRING( "RCCHServer::StartServer(): Started" );
        
        User::WaitForRequest( status );     // wait for start or death
        
        // we can't use the 'exit reason' if the server panicked as this
        // is the panic 'reason' and may be '0' which cannot be distinguished
        // from KErrNone
        
        error = 
            ( server.ExitType() == EExitPanic ) ? KErrGeneral : status.Int();
        server.Close();
        }
    return error;
    }
    
// ========================== OTHER EXPORTED FUNCTIONS =======================

//  End of File
