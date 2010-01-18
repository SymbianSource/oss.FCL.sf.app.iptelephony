/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CCchActiveScheduler declaration
*
*/

#ifndef C_CCHACTIVESCHEDULER_H
#define C_CCHACTIVESCHEDULER_H

// INCLUDES
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

/**
 * Class provides active scheduler for CCH server
 *  @lib cchserver.exe
 *  @since S60 3.2
 */
class CCchActiveScheduler : public CActiveScheduler
    {

public:

    /**
     * Default constructor
     */
    CCchActiveScheduler();

    /**
     * Destructor
     */
    ~CCchActiveScheduler();
    
public:
    
    /**
     * Server setter
     * @param aServer pointer to server
     */
    void SetServer( CCCHServerBase* aServer );
    
public: 

    /**
     * Error occurred while scheduling
     * @param aError error code
     */
    void Error( TInt aError ) const;
    
private:
    
    /**
     * Server handle
     */
    CCCHServerBase* iServer;
    };

#endif // C_CCHACTIVESCHEDULER_H

// End of file
