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
* Description:  CCCHSubsession declaration
*
*/


#ifndef C_CCHSUBSESSION_H
#define C_CCHSUBSESSION_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CCCHServerBase;
class TCCHStorageEntry;
class TCCHServiceSelection;
// CLASS DECLARATION

/**
 *  CCCHSubsession declaration
 *  Subsession for services.
 *  @lib cchserver.exe
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CCCHSubsession ) : public CObject
    {

public: // Constructors and destructor

    /**
    * Two-phased constructor.
    */
    static CCCHSubsession* NewL( CCCHServerBase& aServer );

    /**
     * Two-phased constructor.
     */
    static CCCHSubsession* NewLC( CCCHServerBase& aServer );

    /**
     * Destructor.
     */
    virtual ~CCCHSubsession();

public: // New functions

    /**
     * Handles the messages.
     * @since S60 3.2
     * @param aMessage IPC message
     */
    void ServiceL( const RMessage2& aMessage );
    
    /**
     * Service event occured.
     * @since S60 3.2
     * @param aNewStatus Service which status has changed.
     */
    void ServiceEventOccured( TServiceStatus aNewStatus );
    
private:

    /**
     * C++ default constructor.
     */
    CCCHSubsession( CCCHServerBase& aServer );
    
    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();
    
    /**
     * Try complete.
     */
    void CompleteMessage();
    
private: // data

    /**
     * Handle to server
     */
    CCCHServerBase& iServer;
    
    /**
     * Currently pending request
     */
    RMessage2* iMessage;
    
    /**
     * Register message;
     */
    RMessage2* iRegisterMessage; 
    
    /**
     * Type of service we are listening to
     */
    TServiceSelection iSubscribedService;
    
    /**
     * Event queue.
     */
    RArray<TServiceStatus> iEventQueue;
    
    /**
     * Do we queue events.
     */
    TBool iQueueEvents;
    };

#endif // C_CCHSUBSESSION_H

// End of file
