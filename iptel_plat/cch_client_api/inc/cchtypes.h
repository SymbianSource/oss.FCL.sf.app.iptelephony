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
* Description:  Cch basic types
*
*/



#ifndef CCHTYPES_H
#define CCHTYPES_H

/**
 * Subservice types
 */
enum TCCHSubserviceType
    {
    ECCHUnknown = 0,
    ECCHVoIPSub,
    ECCHPresenceSub,
    ECCHIMSub,
    ECCHVMBxSub
    };

/**
 * State of subservice
 */
enum TCCHSubserviceState
    {
    ECCHUninitialized = 0,
    ECCHDisabled,
    ECCHConnecting,
    ECCHEnabled,
    ECCHDisconnecting
    };

/**
 * State of subservice
 */
enum TCchConnectionParameter
    {
    ECchInitial = 0,    // Type TInt, initial value
    ECchIapId,          // Type TInt, read/write 
    ECchSnapId,         // Type TInt, read/write
    ECchServiceInfo,    // Type Describtor, read only
    ECchSnapLocked,     // Type TInt, read only
    ECchUsername,       // Type Describtor, read/write, 
                        // TCCHSubserviceType ECCHUnknown has to be used for ECchUsername
    ECchPassword,       // Type Describtor, write only
                        // TCCHSubserviceType ECCHUnknown has to be used for ECchUsername
    ECchPasswordSet,    // Type TInt, read. True if password has been set, else false.
    ECchReserved         // Type TInt, read/write. Reserved (obsolote) When adding next new parameter  
                         // rename Reserved to that, implementation is ready then.   
    };    

#endif // CCHTYPES_H
