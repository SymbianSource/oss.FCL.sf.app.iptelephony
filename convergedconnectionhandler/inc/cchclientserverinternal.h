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
* Description:  CCH Client / Server common internal header
*
*/


#ifndef CCHCLIENTSERVERINTERNAL_H
#define CCHCLIENTSERVERINTERNAL_H

// INCLUDES
#include <e32base.h>

// CONSTANTS
_LIT( KCCHServerExe, "CCHServer.exe" );
_LIT( KCCHServerName, "!CCHServer" );
_LIT( KCCHServerStartMutex, "CCHServerStartMutex" );

const TUid  KCCHServerUid  = { 0x10275456 };

/** 
 * Version number 
 */
const TUint KCCHServMajorVersionNumber = 0;
const TUint KCCHServMinorVersionNumber = 0;
const TUint KCCHServBuildVersionNumber = 1;

/**
 * Number of message slots in the server.
 * Needed message slots can be defined as follows:
 * total slots = n+2, where n is number of independent
 * asynchronous services offered by the server.
 */
const TUint KCCHMessageSlots = 9;

// DATA TYPES

/**
 * Commands from client to server
 */
enum TCCHCommands
    {
    ECCHOpenSubSession = 0,
    ECCHCloseSubSession,
    ECCHSubscribeToEvents,
    ECCHSubscribeToEventsCancel,
    ECCHGetServices,
    ECCHGetServicesCancel,
    ECCHGetServiceState,
    ECCHEnableService,
    ECCHEnableServiceCancel,
    ECCHDisableService,
    ECCHDisableServiceCancel,
    ECCHGetServiceInfo,
    ECCHGetPreferredService,
    ECCHSetConnectionInfo,
    ECCHSetConnectionInfoCancel,
    ECCHGetConnectionInfo,
    ECCHGetConnectionInfoCancel,
    ECCHReserveService,
    ECCHFreeService,
    ECCHIsReserved,
    ECCHServiceCount,
    ECCHServerRegister,
    ECCHServerRegisterCancel
    };

/**
 * Server panic codes
 */
enum TCCHServerPanic
    {
    ECCHErrCreateServer = 100,
    ECCHErrStartServer = 101,
    
    ECCHErrSubSessionOpen = 102,
    ECCHErrSubSessionClose = 103,
    
    ECCHBadRequest = 104,
    ECCHBadDescriptor = 105,
    };

#endif // CCHCLIENTSERVERINTERNAL_H

// End of File