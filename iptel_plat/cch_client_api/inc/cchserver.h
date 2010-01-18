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
* Description:  RCCHServer declaration
*
*/


#ifndef R_CCHSERVER_H
#define R_CCHSERVER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

/** @file CCHServer.h
 * API to access Converged Connection Handler server functionalities.
 * 
 * Client uses RCCHServer and RCCHClient classes to create client-server 
 * connection to Converged Connection Handler server. After successful 
 * connection client can access server functionalities trought RCCHClient. 
 * 
 * 
 * Example usage of API:  
 * @code
 * // Creating connection and subsession to server
 * #include <cchclient.h>
 * #include <cchserver.h>
 * 
 * RCCHServer  iServer;
 * RCCHClient  iClient;
 * 
 * iServer = new (ELeave) RCCHServer();
 * iClient = new (ELeave) RCCHClient();
 * 
 * // Start CCHServer and open session
 * User::LeaveIfError( iServer->Connect() );
 * // Open subsession
 * User::LeaveIfError( iClient->Open( *iServer ) );
 * ...
 * @endcode
 */
  
// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// CLASS DECLARATION
    
/**
 *  RCCHServer declaration
 *  This class hides inter-thread communication between client
 *  and server. Hides also asynchronous notification services.
 *  @lib cchclient.lib
 *  @since S60 3.2
 */
class RCCHServer : public RSessionBase
    {

public: // Constructors and destructor
    
    /**
     * C++ default constructor
     */
    IMPORT_C RCCHServer();

public: // New functions

    /**
     * Connect to CCH server
     * @since S60 3.2
     * @return General Symbian error code
     */
    IMPORT_C TInt Connect();
    
    /**
     * Disconnect from CCH server
     * @since S60 3.2
     */
    IMPORT_C void Disconnect();
    
    /**
     * Get version information
     * @since S60 3.2
     * @return Version information
     */
    TVersion Version() const;

protected:  // New functions
        
    /**
     * Start the server during Connect() if the server is not loaded.
     * @since S60 3.2
     * @return General Symbian error code.
     */
    TInt StartServer() const;

    };

#endif // R_CCHSERVER_H

// End of file
