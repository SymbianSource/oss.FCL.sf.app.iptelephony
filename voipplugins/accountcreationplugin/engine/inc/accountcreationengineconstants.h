/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Constants for accountcreationengine
*
*/


#ifndef ACCOUNTCREATIONENGINECONSTANTS_H
#define ACCOUNTCREATIONENGINECONSTANTS_H

// UID of this engine.
const TUid KAccountCreationEngineUid = { 0x2000BEF3 };

_LIT8( KXmlMimeType,  "text/xml" );
_LIT8( KSisMimeType,  "x-epoc/x-sisx-app" );
_LIT8( KIconMimeType, "image/png");

_LIT8( KActionMimeType, "application/vnd.wap.connectivity-wbxml" );
_LIT8( KSessionId, ";jsessionid=" );

const TInt KDefaultBufferSize = 256;
const TUint KMaxElementLength = 128;
const TUint KNameMaxLength    = 256;
const TUint KBufferSize       = 2048;
const TUint KMaxUriLength     = 256;
const TUint KMaxSessionIdString = 64;
const TInt KBitsPerByte = 8;

_LIT( KXmlFile, "\\data\\xmlfile.xml" );
_LIT( KSisFile, "\\data\\sisfile.sis" );

const TInt KFileBufferSize = 1024;

// Constant node types.
_LIT8( KServiceProvider, "Provider" ); // Element.
_LIT8( KSis, "sis" );                  // Element.

// Constants strings (elements) for parsing.
_LIT8( KProviderName,        "providerName" );
_LIT8( KProviderDescription, "providerDescription" );
_LIT8( KProviderType,        "providerType" );
_LIT8( KIconUrl,             "iconUrl" );
_LIT8( KActivateUrl,         "activateUrl" );
_LIT8( KCreateUrl,           "createUrl" );
_LIT8( KSisUrl,              "sisUrl" );
_LIT8( KReportUrl,           "reportUrl" );

_LIT8( KUserAgent, "Account Creation Plugin 1.0" );
_LIT8( KAccept, "*/*" ); // Types to be accepted.

// PhoneModule name
_LIT( KPhoneModuleName, "Phonetsy.tsy" );
_LIT( KPhoneName,       "DefaultPhone" );

// Common
_LIT( KAmperSand,     "&" );
_LIT( KQuestionMark,  "?" );
_LIT( KPlacing,       "=" );
_LIT( KCharacterTwo,  "2" );
_LIT( KHyph,          "-" );
_LIT8( KQuestionMark8,  "?" );

// Parameter names
_LIT( KMnc,        "MNC" );
_LIT( KMcc,        "MCC" );
_LIT( KNonce,      "Nonce" );
_LIT( KKeyInfo,    "Key" );
_LIT( KDeviceInfo, "deviceinfo" );
_LIT( KAction,     "action" );
_LIT( KsType,      "sType" );
_LIT( KcVersion,   "cVersion" );
_LIT( KUsername,   "username" );
_LIT( KPassword,   "password" );
_LIT( KTempPassword, "temppass" );
_LIT( KImei, "imei" );
_LIT( KVariant, "variant" );

// Parameter action value
_LIT( KValueCreate,   "create" );
_LIT( KValueActivate, "activate" );
_LIT( KValueEmpty,    "" );
_LIT( KValuecVersion, "email-0.1" );

// Max should be 3 for both in practically, just in case bigger values
const TUint KMaxMCC = 4;
const TUint KMaxMNC = 8;

#endif  // ACCOUNTCREATIONENGINECONSTANTS_H

// End of file.
