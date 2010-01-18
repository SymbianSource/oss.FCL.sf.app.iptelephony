/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MCCHServiceNotifier declaration
*
*/


#ifndef M_CCHSERVICENOTIFIER_H
#define M_CCHSERVICENOTIFIER_H

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
// None

// CLASS DECLARATION
    
/**
 *  MCCHServiceNotifier declaration.
 *  Service state notifier. 
 *  @lib cchserver.exe
 *  @since S60 3.2
 */
class MCCHServiceNotifier
    {

public: // New functions

    /**
     * Service state notifier.
     * @since S60 3.2
     * @param aServiceId Service which state has changed
     * @param aType Subservice's type. 
     * @param aState A new state of the service.
     */
    virtual void StateChangedL( const TUint aServiceId,
                                const TCCHSubserviceType aType,
                                const TCCHSubserviceState aState,
                                const TInt aError ) = 0;

private:
    
    };

#endif // M_CCHSERVICENOTIFIER_H

// End of file
