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
* Description:  
*
*/


#ifndef T_SCPDEFS_H
#define T_SCPDEFS_H

#include <e32std.h>

enum TScpConnectionEvent
    {
    EScpUnknown = 0,
    EScpNetworkFound,           // = CSIPConnection::ConnectionEvent(EActive)
    EScpNetworkLost,            // = CSIPConnection::ConnectionEvent(EInactive)
    EScpNetworkNotFound,        // = CSIPConnection::ConnectionEvent(EUnavailable)
    EScpRegistered,             // = CSIPProfileRegistry::EventOccurred(ERegistered)
    EScpDeregistered,           // = CSIPProfileRegistry::EventOccurred(EDeregistered)
    EScpRegistrationFailed,     // = CSIPProfileRegistry::ErrorOccurred(-34)
    EScpInvalidSettings,        // = CSIPProfileRegistry::ErrorOccurred(-6)
    EScpAuthenticationFailed,   // = KErrSIPForbidden
    EScpRoaming,                // = MSipProfileAlrObserver::EMigrationStarted
    EScpRegistrationCanceled,   // = CSIPProfileRegistry::ErrorOccurred(-3)
    EScpRegistrationPending     // = XIMP Bind / VMBX Subscribe(-11)
    };

enum TScpIapType
    {
    EScpUnknownType = 0,
    EScpGprs,
    EScpWlan
    };

enum TScpAccessPointType
    {
    EScpUnknownAccessPointType = 0,
    EScpIap,
    EScpSnap
    };

#endif // T_SCPDEFS_H
