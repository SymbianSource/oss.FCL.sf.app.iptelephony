/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CCchStartupCounter declaration
*
*/


#ifndef C_CCHSTARTUPCOUNTER_H
#define C_CCHSTARTUPCOUNTER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <cchclientserver.h>

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
class CRepository;

// CLASS DECLARATION

/**
 *  CCchStartupCounter declaration
 *  Class handles CCH server's startup situation
 *  @lib cchserver.exe
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CCchStartupCounter ) : public CBase
    {
    
public: // Constructors and destructor

    /**
    * Two-phased constructor.
    */
    static CCchStartupCounter* NewL( CCCHServerBase& aServer );

    /**
     * Destructor.
     */
    virtual ~CCchStartupCounter();
    
public: // New functions
    
    /**
     * Startup has occured.
     * @since S60 3.2
     */
    void StartupOccuredL();
        
    /**
     * Reset startup counter cenrep-value.
     * @since S60 3.2
     */
    void ResetStartupCounter();
    
private:
    
    /**
     * Handles startup exceptions.
     * @since S60 3.2
     */
    void HandleStartupExceptionsL();
    
private:

    /**
     * C++ default constructor.
     */
    CCchStartupCounter( CCCHServerBase& aServer );
    
    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();
    
private: // data

    /**
     * Server handle
     */
    CCCHServerBase& iServer;
    
    /**
     * Startup counter repository
     */
    CRepository*    iStartupRepository;
    };

#endif // C_CCHSTARTUPCOUNTER_H

// End of file
