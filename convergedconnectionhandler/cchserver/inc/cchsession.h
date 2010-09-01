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
* Description:  CCCHSession declaration
*
*/


#ifndef C_CCHSESSION_H
#define C_CCHSESSION_H

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

// CLASS DECLARATION

/**
 *  CCCHSession declaration
 *  Server side counterpart for client's Session classes.
 *  Each connection has own session in server.
 *  @lib cchserver.exe
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CCCHSession ) : public CSession2
    {

public: // Constructors and destructor

    /**
    * Two-phased constructor.
    */
    static CCCHSession* NewL( CCCHServerBase& aServer );

    /**
     * Two-phased constructor.
     */
    static CCCHSession* NewLC( CCCHServerBase& aServer );

    /**
     * Destructor.
     */
    virtual ~CCCHSession();

public: // New functions

    /**
     * Service dispatcher from CSession.
     * @since S60 3.2
     * @param aMessage Message received form client.
     */
    void ServiceL( const RMessage2& aMessage );

private:

    /**
     * C++ default constructor.
     */
    CCCHSession( CCCHServerBase& aServer );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private: // data

    /**
     * Handle to server
     */
    CCCHServerBase&             iCCHServer;

    /**
     * Subsessions
     */
    CObjectCon*                 iSubsessions;

    /**
     * Subsession id generator.
     */
    CObjectIx*                  iObjectIx;
    
    };

#endif // C_CCHSESSION_H

// End of file
