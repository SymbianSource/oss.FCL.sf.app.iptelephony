/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef VOIPEVENTLOGCONSTANTS_H
#define VOIPEVENTLOGCONSTANTS_H

const TUid KCRUidVoIPEventLog = { 0x2000B608 };

_LIT( KVoIPEventLogSemaphore, "VoIPEventLogSemaphore" );
_LIT(KVoIPTimeStampFormat,"%F%D/%M/%Y %H:%T:%S");

const TInt KVELErrorCountKey = 0x00000000;
const TInt KVELLatestErrorIndexKey = 0x00000001;
const TInt KVELMaxErrorCountKey = 0x00000002;

const TInt KTimeStampStrLen = 19;
const TUint32 KColumnIncrement = 0x00010000;
const TUint32 KColumnMask = 0xFFFF0000;

/** First key in column of error code. */
const TUint32 KSPColumnErrorCode = 0x00010001;            // --> 0x00020000

/** First key in column of error text. */
const TUint32 KSPColumnErrorText = 0x00020001;          // --> 0x00030000

/** First key in column of time stamp. */
const TUint32 KSPColumnTimeStamp = 0x00030001;          // --> 0x00040000

#endif  // VOIPEVENTLOGCONSTANTS_H