/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Constants for SVP.
*
*/


#ifndef SVPCONSTS_H
#define SVPCONSTS_H

#include <e32base.h>

// SVP panic codes; panic code range -32600 -> -32649
const TInt KSVPanicSessionNotFound = -32601;
const TInt KSVPPanicBadArgument = -32602;

// SVP implementation Uid
const TUid KSVPImplementationUid = {0x10282589}; 

// SVP Uid
const TUid KSVPUid = { 0 };

// secure preference consts
const TUint32 KSVPStatusNonSecure = 0;
const TUint32 KSVPStatusSecurePreferred = 1;
const TUint32 KSVPStatusSecureMandatory = 2;

// codec SDP names
_LIT8( KTELEVENT, "telephone-event" ); 

_LIT8( KAMR,"AMR" );
_LIT8( KPCMA, "PCMA" );
_LIT8( KPCMU, "PCMU" );
_LIT8( KG729, "G729" );
_LIT8( KG711, "G711" );
_LIT8( KILBC, "iLBC" );
_LIT8( KCN, "CN" );
_LIT8( KRED, "red");
_LIT8( KAMRWB,"AMR-WB");

const TInt KSVPDefKeepAlivePl = 120;
const TInt KSVPDefKeepAlivePlMax = 127;
const TInt KSVPCNPayloadType = 13;

// timer constants
const TInt KSVPTerminationTimerExpired = 1;
const TInt KSVPInviteTimerExpired = 2;
const TInt KSVPExpiresTimeExpired = 3;
const TInt KSVPRemoteEndDiedExpired = 4;
const TInt KSVPHangUpTimerExpired = 5;
const TInt KSVPEmergencyExpired = 6;
const TInt KSVPHoldTimerExpired = 7;
const TInt KSVPResumeTimerExpired = 8;
const TInt KSVPReInviteTimerExpired = 9;
const TInt KSVPReferTimerExpired = 10;
const TInt KSVPICMPErrorTimerExpired = 11;
const TInt KSVPSinkResumeICMPErrorTimerExpired = 12;
const TInt KSVPSourceResumeICMPErrorTimerExpired = 13;
const TInt KSVPTerminatingTime = 4000;
const TInt KSVPMoHangupTerminatingTime = 500;
const TInt KSVPInviteTimer = 120000;
const TInt KSVPHoldExpirationTime = 10000;
const TInt KSVPResumeExpirationTime = 10000;
const TInt KSVPReferExpirationTime = 10000;
const TInt KSVPMilliSecondCoefficient = 1000;
const TUint KSvpStandbyTimerInMillisecs = 2000; // timer constant needed when creating rtp source
const TUint KSvpJitterBufferLength = 10;
const TUint KSvpJitterBufferThreshold = 3;
const TUint32 KSVPDefaultExpiresTime = 120;
const TUint32 KSVPDefaultSessionExpires = 1800;
const TInt KSVPICMPErrorTime = 5000;
const TUint KSVPSinkResumeICMPErrorTime = 1000;    // Time after resume and icmp error
const TUint KSVPSourceResumeICMPErrorTime = 1000;  // Time after resume and icmp error

// Recipient DTMF parsing
_LIT( KSVPDtmfAllValidChars, "0123456789pw*+#" );
_LIT( KSVPDtmfTelNumRange, "+0123456789" );
_LIT( KSVPDtmfDelimiterRange, "pw+" );

// constants for URI handling and SIP headers
_LIT8( KSVPSipPrefix, "sip:" );
_LIT( KSVPSipPrefix2, "sip:" );
const TInt KSVPSipPrefixLength = 4;
_LIT8( KSVPSipsPrefix, "sips:" );
_LIT( KSVPSipsPrefix2, "sips:" );
const TInt KSVPSipsPrefixLength = 5;
_LIT8( KSVPAt, "@" );
_LIT( KSVPAt2, "@" );
const TInt KSVPAtLength = 1;
_LIT8( KSVPSemiCln, ";" );
_LIT8( KSVPCln, ":" );
_LIT( KSVPCln2, ":" );
_LIT8( KSVPHphn, "-" );
_LIT8( KSVPComma, "," );
_LIT( KSVPQuotationMark, "\"" );
_LIT8 (KSVPUserEqualsPhone, ";user=phone" );
const TInt KSVPUserEqualsPhoneLenght = 11; 
const TInt KSVPLRLength = 3;
_LIT8( KSVPLooseRouting, ";lr" );
const TInt KSVPSingleBracketLength = 1; // only one bracket
const TInt KSVPDoubleBracketLength = 2; // always opening and closing bracket
_LIT8( KSVPLeftBracketMark, "<");
_LIT( KSVPLeftBracketMark2, "<" );
_LIT8( KSVPRightBracketMark, ">");
_LIT( KSVPRightBracketMark2, ">" );
_LIT8( KSVPAuthidentity, "?X-Sipx-Authidentity=");
_LIT8( KSVPAuthidentity2, "?X-sipX-Authidentity=");
_LIT8( KSVPAuthidentity3, "&X-sipX-Authidentity=");

// uri length
const TUint KSVPMaxUriLength = 100;
// codec name length, max
const TUint KSVPMaxCodecNameLength = 8;
const TUint8 KSVPLineFeed2 = '\n';
const TUint8 KSVPHyphen        = '-';
_LIT( KSVPSpace2, " " );
_LIT8( KSVPLineFeed,           "\n" );

_LIT8( KSVPOpeningBracket, "(" );
_LIT8( KSVPClosingBracket, ")" );
_LIT( KSVPCSeqPrefix, "CSeq:" );
const TInt KSVPCSeqPrefixLength = 5; 

// user=phone 
_LIT8(KSVPPlus, "+");
_LIT8( KUserEqualsPhone, "user=phone" ); // user = phone
const TInt KUserEqualsPhoneParamLength = 11; // user = phone
const TInt KSemicolonLength = 1; // user = phone
const TInt KSVPMinUserInfoLength = 2; // user = phone

_LIT8( KTelPrefix, "tel:" );
const TInt KTelPrefixLength = 4;

_LIT( KSVPSemiColon, ";" );
_LIT( KSVPLogFieldDelimiter, "\t" );
const TUint8 KSVPLeftBracket = '<';
const TUint KSVPUserEqualsPhoneLength = 11;
_LIT( KSVPName,"SVP" );


_LIT8( KSVPTLS, "tls" );

const TUint KSVPMaxUAFreeTextLength   = 32;
const TUint KSVPTempStringlength      = 200;
const TUint KSVPAcceptLangStringLength= 19; // example: Accept-language: en
const TUint KSVPExpiresHeaderLength   = 12; // example: Expires: 120
const TUint KSVPQuesReplacesTxtLength = 10;
const TUint KSVPReferredByLength      = 12;
const TUint KSVPToTagLength           = 12;
const TUint KSVPFromTagLength         = 14;
const TUint KSVPTagLength             = 5;

_LIT8( KSVPAcceptLanguage,      "Accept-Language:" );
_LIT8( KSVPExpiresHeader,       "Expires:" );
_LIT8( KSVPSessionExpires,      "Session-Expires:" );
_LIT8( KSVPRefresher,           ";refresher" );
_LIT8( KSVPMinSessionExpires,   "Min-SE:" );
const TInt KSVPMinExpiresLenght = 8;
_LIT8( KSVPExpires,             "120");
_LIT8( KSVPUserAgent,           "User-Agent:" );
_LIT8( KSVPSpaceMark,           " " );
_LIT8( KSVPReferredBy,          "Referred-By:" );
_LIT8( KSVPCallId_replaces,     "Call-ID: " );
_LIT8( KSVP_tag,                ";tag=" );
_LIT8( KSVPTo_tag,              "%3Bto-tag%3D" );
_LIT8( KSVPFrom_tag,            "%3Bfrom-tag%3D" );
_LIT8( KSVPFromPrompt, 			"From: ");
_LIT8( KSVPPIdentity,           "P-Preferred-Identity: ");

const TUint KSVPFromPromptStringlength = 6;

_LIT8( KSVPReplacesColonTxt,    "Replaces:" );
_LIT8( KSVPQuesReplacesTxt,     "?Replaces=" );

_LIT8(KSVPMyAnonymousAddress,    "\"Anonymous\" <sip:anonymous@anonymous.invalid>");
_LIT8(KSVPMyAnonymousSecAddress, "\"Anonymous\" <sips:anonymous@anonymous.invalid>");
_LIT(KSVPAnonymous,              "Anonymous");
_LIT8(KSVPAnonymousAddressWithoutName,    "<sip:anonymous@anonymous.invalid>");
_LIT8(KSVPAnonymousSecAddressWithoutName, "<sips:anonymous@anonymous.invalid>");
_LIT8(KSVPAnonymousName8,                 "Anonymous");
_LIT(KSVPPrivateNumber, "Private");

_LIT8( KSVPEmpty,        "");
_LIT8( KSVPPrivacy,      "Privacy:" );
_LIT8( KSVPId,           "Id" );
_LIT8( KSVPNone,         "None" );
_LIT8( KSVPSupported,    "Supported:" );
_LIT8( KSVPRequire,      "Require:" );
_LIT8( KSVPPrecondition, "precondition" );
_LIT8( KSVP100rel,       "100rel" );
const TInt KSVPClirOff = 0;
const TInt KSVPClirOn = 1;
const TInt KSVPClirDefault = 2;
const TUint KSVPPrivacyLength   = 13;  // example: Privacy: None

const TUint8 KSVPQuestionMark  = '?';
const TUint8 KSVPColonMark     = ':';
const TUint8 KSVPSemiColonMark = ';'; 

const TUint KSVPMaxLangCodeLength = 4; 

//Logging
const TInt KSvpMaxDebugBufferSize = 256;

// ipapputils
const TUint KSVPWlanMacAddressLength = 50;
_LIT8 ( KSVPWlanMacAddressFrmt, "-" );

const TInt KSVPContactArrayGranularity = 5;

_LIT8( KSVPMessageSipfrag, "message/sipfrag;version=2.0");
_LIT8( KSVPSubsStateTerminated,   "Subscription-State: terminated;reason=noresource");

_LIT( KSVPS60,"S60" );
_LIT( KSVPCopyright, "?" );
_LIT(KSVPSalesModelFileName, "\\resource\\versions\\model.txt");

// Emergency constants
const TInt KSVPEmergencyTimeout = 17000;
// Location information constants
const CActive::TPriority KSVPEmergencyPositioningPriority = 
    CActive::EPriorityStandard;
const TInt KSVPEmergencyDhcpDefaultPsyModuleId = 0x10283114;
_LIT( KSVPEmergencyApplicationName, "svp.dll" ); // Requestor identifier
const TUint32 KSVPEmergencyPositioningTimeout = 5000000; // In microseconds
// Location related header constants
_LIT8( KSVPEmergencyPriorityEmergency, "Priority: emergency" );
_LIT8( KSVPEmergencyGeolocation, "Geolocation: " );
_LIT8( KSVPEmergencyTagInsertedBy, ";inserted-by=" );
_LIT8( KSVPEmergencyTagRecipientEndpoint, ";recipient=endpoint" );
_LIT8( KSVPEmergencyAcceptApplicationSdp, "Accept: application/sdp" );
_LIT8( KSVPEmergencyApplicationPidfXml, "application/pidf+xml" );
_LIT8( KSVPEmergencyContentIdName, "Content-ID: " );
_LIT8( KSVPCidPrefix, "cid:" );


const TInt KSVPAnonymousBarringOff = 0;
const TInt KSVPAnonymousBarringOn  = 1;
const TInt KSVPDoNotDisturbOff     = 0;
const TInt KSVPDoNotDisturbOn      = 1;

const TInt KSVPErrDnDRejection           = -10;
const TInt KSVPErrAnonymousCallRejection = -11;
const TInt KSVPErrCodecMismatch          = -12;
const TInt KSVPErrWrongMinSE             = -13;

const TInt KSVPMceArrayIndexOriginator = 0;

const TInt KSVPDefaultUDPRefreshInterval = 28; // 28 seconds is default

// DATA TYPES
enum TSupplementaryServicesEvent
    {
    ESVPSSDefault,
    ESVPSSAnonymousBarring,
    ESVPSSDoNotDisturb
    };
    
const TUint KSVPWlanTrafficStreamParamUserPriority = 6;

#endif // SVPCONSTS_H
