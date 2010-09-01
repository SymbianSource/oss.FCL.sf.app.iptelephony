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
* Description:  MCCHSPSNotifier declaration
*
*/


#ifndef M_CCHSPSNOTIFIER_H
#define M_CCHSPSNOTIFIER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <spdefinitions.h>

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
 *  Service notifier for CCH server
 *  @lib cchserver.exe
 *  @since S60 3.2
 */
class MCCHSPSNotifier
    {

public: // New functions

    /**
     * Service table notifier.
     * @since S60 3.2
     * @param aServiceIds New Service Id -array
     */
    virtual void ServiceTableChangeNotifyL( RIdArray& aServiceIds ) = 0;

private:

    };

#endif // M_CCHSPSNOTIFIER_H

// End of file
