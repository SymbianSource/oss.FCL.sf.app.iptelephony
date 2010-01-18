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
* Description:  CCCHRequestStorage declaration
*
*/


#ifndef C_CCHREQUESTSTORAGE_H
#define C_CCHREQUESTSTORAGE_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include "cchclientserverinternal.h"

// CONSTANTS
// None

// MACROS
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CCCHServerBase;
class CCCHSubsession;

// DATA TYPES
/**
 * Storage entry structure.
 */
class TCCHStorageEntry
    {
public:
    /**
     * Request type
     */
    TCCHCommands            iRequest; 
    /**
     * IPC message
     */
    RMessage2*              iMessage;
    /**
     * Pointer to subsession. Not own
     */
    const CCCHSubsession*   iSubsession;
    };

// CLASS DECLARATION

/**
 *  CCCHRequestStorage declaration
 *  Storage for asynchronous requests.
 *  @lib cchserver.exe
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CCCHRequestStorage ) : public CBase
    {
public: // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CCCHRequestStorage* NewL( CCCHServerBase& aServer );

    /**
     * Two-phased constructor.
     */
    static CCCHRequestStorage* NewLC( CCCHServerBase& aServer );

    /**
     * Destructor.
     */
    virtual ~CCCHRequestStorage();

public: // New functions

    /**
     * Add new request into storage.
     * @since S60 3.2
     * @param aRequest IPC of request.
     * @param aMessage Message object of request.
     * @param aSubsession Pointer to subsession of request.
     */
    void AddRequestL( TCCHCommands aRequest,
                      const RMessage2& aMessage,
                      const CCCHSubsession* aSubsession );

    /**
     * Complete and remove request from storage.
     * @since S60 3.2
     * @param aRequest IPC of request.
     * @param aSubSession Pointer to subsession of request.
     * @param aError Request is completed with this value.
     * @return KErrNone if successfull, KErrNotFound if request is not found.
     */
    TInt CompleteRequest( TCCHCommands aRequest,
                          const CCCHSubsession* aSubsession,
                          TInt aError );
    
    /**
     * Complete and remove request from storage. Also writes
     * TAny back to the client
     * @since S60 3.2
     * @param aRequest IPC of request.
     * @param aSubSession Pointer to subsession of request.
     * @param aParams TAny parameter, which returns back to the client
     * @param aError Request is completed with this value.
     * @return KErrNone if successfull, KErrNotFound if request is not found.
     */
    TInt CompleteRequestL( TCCHCommands aRequest,
                           const CCCHSubsession* aSubsession,
                           TAny* aParams,
                           TInt aError );
                                                    
    /**
     * Remove all requests by session pointer from storage.
     * @since S60 3.2
     * @param aSubSession Subsession pointer.
     * @return KErrNone if successfull, KErrNotFound if no requests are found.
     */
    TInt RemoveRequestsBySession( const CCCHSubsession* aSubsession );
    
    /**
     * Add session to request storage.
     * @since S60 3.2
     * @param aSubSession Subsession pointer.
     */
    void AddSession( CCCHSubsession* aSubsession );
    
    /**
     * Remove session from request storage.
     * @since S60 3.2
     * @param aSubSession Subsession pointer.
     * @return KErrNone if successfull, KErrNotFound if no requests are found.
     */
    TInt RemoveSession( const CCCHSubsession* aSubsession );
    
    /**
     * Notify client about subservice state change
     * @since S60 3.2
     * @param aNewState Service's new information, which writes 
     * back to the client
     */
    void NotifyServiceStatesChange( TServiceStatus aNewStatus );

    /**
     * Starts to scan available network
     * @since S60 3.2
     */
    void ScanNetworks();

    /**
     * Cancels scanning request.
     * @since S60 3.2
     */
    void ScanNetworksCancel();

private:

    /**
     * C++ default constructor.
     */
    CCCHRequestStorage( CCCHServerBase& aServer );

    /**
     * Delete request from storage
     * @since S60 3.2
     * @param aIndex Index of request in storage.
     */
    void DeleteRequest( TInt aIndex );

private: // data

    /**
     * Server handle
     */
    CCCHServerBase&             iServer;

    /**
     * Array of requests.
     */
    RArray<TCCHStorageEntry>    iRequests;
    
    /**
     * Array of subsessions.
     */
    RArray<CCCHSubsession*>     iSubsessions;

    };

#endif // C_CCHREQUESTSTORAGE_H

// End of file
