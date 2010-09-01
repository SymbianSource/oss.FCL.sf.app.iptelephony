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
* Description:  
*
*/


#ifndef M_CCHSERVICEOBSERVER_H
#define M_CCHSERVICEOBSERVER_H

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
// None

// CLASS DECLARATION
    
/**
 *  CCHServiceObserver declaration.
 *  @since S60 3.2
 */
class MCchServiceObserver
    {

public: // New functions

    /**
     * CCchService implementation have to use this callback
     * method when service state changed. Otherwise CCH doesn't
     * forward notifications to the clients.
     * @since S60 3.2
     * @param aServiceId
     * @param aSubService Sub service type. 
     * @param aState a new state of the service.
     * @param aError KErrNone if no error occured, otherwise see CCHError.h.
     */
     virtual void ServiceStateChanged(
                    const TServiceSelection aServiceSelection,
                    TCCHSubserviceState aState, 
                    TInt aError ) = 0;

private:
    
    };

#endif // M_CCHSERVICEOBSERVER_H

// End of file
