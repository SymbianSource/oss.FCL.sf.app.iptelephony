/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Holds default values for VoIP XML Processor.
*
*/


#ifndef VOIPXMLPROCESSORDEFAULTS_H
#define VOIPXMLPROCESSORDEFAULTS_H

#include <e32base.h>

// ==============================
// XML constants
// ==============================
//
// Node names.
_LIT( KNodeNokiaConfDoc,                  "nokia-confdoc" );
_LIT( KNodeConfDescription,               "confdescription" );
_LIT( KNodeConfBrandUrl,                  "confbrandurl" );
_LIT( KNodeSettings,                      "settings" );
_LIT( KNodeParam,                         "param" );
// Node attributes.
_LIT( KAttributeType,                     "type" );
_LIT( KAttributeName,                     "name" );
_LIT( KAttributeValue,                    "value" );
// Setting types (i.e. supported values for 'type' attribute).
_LIT( KSettingsTypeVoip,                  "voip" );
_LIT( KSettingsTypeSip,                   "sip" );
_LIT( KSettingsTypePresence,              "presence" );
_LIT( KSettingsTypeXdm,                   "xdm" );
_LIT( KSettingsTypeNatFw,                 "natfw" );
_LIT( KSettingsTypeWlan,                  "wlan" );
_LIT( KSettingsTypeCodec,                 "codec" );
_LIT( KSettingsTypeVmbx,                  "voicemailbox" );
_LIT( KSettingsTypeProxy,                 "proxy" );
_LIT( KSettingsTypeRegistrar,             "registrar" );
_LIT( KSettingsTypeAdditionalStun,        "additionalstun" );
_LIT( KSettingsTypeWepKey,                "wepkey" );
_LIT( KSettingsTypeDestination,           "destination" );
_LIT( KSettingsTypeEap,                   "eap" );
// Parameter names (i.e. supported values for 'value' attribute).
// Generic.
_LIT( KParamValueName,                    "name" );
_LIT( KParamValueUri,                     "uri" );
_LIT( KParamValueTransport,               "transport" );
_LIT( KParamValuePort,                    "port" );
_LIT( KParamValueUsername,                "username" );
_LIT( KParamValuePassword,                "password" );
_LIT( KParamValueType,                    "type" );
_LIT( KParamValueDomain,                  "domain" );
_LIT( KParamValueStartPort,               "startport" );
_LIT( KParamValueEndPort,                 "endport" );
// VoIP specific.
_LIT( KParamValueMediaQos,                "mediaqos" );
_LIT( KParamValueDtmfInband,              "dtmfinband" );
_LIT( KParamValueDtmfOutband,             "dtmfoutband" );
_LIT( KParamValueSecureCallPref,          "securecallpreference" );
_LIT( KParamValueVoipOverWcdma,           "allowvoipoverwcdma" );
_LIT( KParamValueRtcp,                    "rtcp" );
_LIT( KParamValueUserAgentTerminalType,   "useragentterminaltype" );
_LIT( KParamValueUserAgentWlanMac,        "useragentwlanmac" );
_LIT( KParamValueUserAgentFreeString,     "useragentfreestring" );
_LIT( KParamValueCallerIdDigits,          "calleriddigits" );
_LIT( KParamValueIgnoreDomainPart,        "ignoredomainpart" );
_LIT( KParamValueAutoAcceptBuddyRequests, "autoacceptbuddyrequests" );
_LIT( KParamValueAddUserPhone,            "adduserphone" );
_LIT( KParamValueProviderUrl,             "providerurl" );
_LIT( KParamValueMinSessionInterval,      "minsessioninterval" );
_LIT( KParamValueSessionInterval,         "sessioninterval" );
_LIT( KParamValueBrandingUri,             "brandinguri" );
_LIT( KParamValueAutoEnable,              "autoenable" );
_LIT( KParamValueEnableSipIm,             "enablesipim" );
_LIT( KParamValueBrandId,                 "brandid" );
_LIT( KParamValueDefault,                 "default" );
// Voice mailbox specific.
_LIT( KParamValueMwiUri,                  "mwiuri" );
_LIT( KParamValueListeningUri,            "listeninguri" );
_LIT( KParamValueReSubscribeInterval,     "resubscribeinterval" );
// Codec specific
_LIT( KParamValueJitterBuffer,            "jitterbuffer" );
_LIT( KParamValueOctetAlign,              "octetalign" );
_LIT( KParamValuePTime,                   "ptime" );
_LIT( KParamValueMaxPTime,                "maxptime" );
_LIT( KParamValueModeSet,                 "modeset" );
_LIT( KParamValueModeChangePeriod,        "modechangeperiod" );
_LIT( KParamValueModeChangeNeighbor,      "modechangeneighbor" );
_LIT( KParamValueMaxRed,                  "maxred" );
_LIT( KParamValueVad,                     "vad" );
_LIT( KParamValueAnnexB,                  "annexb" );
// SIP specific
_LIT( KParamValueSignalingQos,            "signalingqos" );
_LIT( KParamValuePublicUserId,            "publicuserid" );
_LIT( KParamValueSignalCompression,       "signalcompression" );
_LIT( KParamValueSecurityNegotiation,     "securitynegotiation" );
_LIT( KParamValueProfileLock,             "profilelock" );
_LIT( KParamValueAutoRegistration,        "autoregistration" );
// Proxy
_LIT( KParamValueLooseRouting,            "looserouting" );
// Presence specific
_LIT( KParamValueMaxObjectSize,           "maxobjectsize" );
_LIT( KParamValuePublishInterval,         "publishinterval" );
_LIT( KParamValueMaxSubscriptions,        "maxsubscriptions" );
_LIT( KParamValueMaxContacts,             "maxcontacts" );
_LIT( KParamValueDomainSyntax,            "domainsyntax" );
// NAT/FW sepcific
_LIT( KParamValueTcpRefreshInterval,      "tcprefreshinterval" );
_LIT( KParamValueUdpRefreshInterval,      "udprefreshinterval" );
_LIT( KParamValueCrlfRefresh,             "crlfrefresh" );
_LIT( KParamValueStunSharedSecret,        "stunsharedsecret" );
// Access point specific
_LIT( KParamSsid,                         "ssid" );
_LIT( KParamHidden,                       "hidden" );
_LIT( KParamNetworkMode,                  "networkmode" );
_LIT( KParamPreSharedKey,                 "presharedkey" );
_LIT( KParamWepAuthMode,                  "wepauthmode" );
// WEP
_LIT( KParamLength,                       "length" );
_LIT( KParamData,                         "data" );

// Other.
const TInt KMaxNodeNameLength          = 32;
const TInt KMaxNodeValueLength         = 256;
const TInt KMaxProfileNameAppendLength = 4; // e.g. (10) as in Provider(10)
const TInt KMaxProfileNames            = 99;
_LIT( KOpenParenthesis,   "(" );
_LIT( KClosedParenthesis, ")" );
const TUint KComma = ',';

// ==============================
// VoIP constants
// ==============================
//
_LIT( KILbc, "ILBC" );
const TInt KDefaultResubscribe = 600;

// ==============================
// SIP constants
// ==============================
//
_LIT8( KIetf,               "IETF" );
_LIT8( KIms,                "IMS" );
_LIT8( KOpenParenthesis8,   "(" );
_LIT8( KClosedParenthesis8, ")" );
_LIT8( KSip,                "sip:" );
_LIT8( KSips,               "sips:" );
_LIT8( KTransport,          ";transport=" );
_LIT8( KLr,                 ";lr" );
_LIT8( KEscaped,            "%" );
_LIT8( KColon,              ":" );
_LIT8( KTcp,                "TCP" );
_LIT8( KUdp,                "UDP" );
_LIT8( KUserAgent,          "User-Agent" );
_LIT8( KSpace,              " " );
const TInt KMaxTerminalTypeLength            = 64;
const TInt KMaxWlanMacAddressLength          = 20;
const TInt KMaxUserAgentHeaderLength         = 200;
const TInt KUserAgentHeaderDataArrayInitSize = 4;
const TInt KDefaultSigQos                    = 40;

// ==============================
// XDM and Presence constants
// ==============================
//
const TInt KMaxObjectSize     = 5120;
const TInt KPublishInterval   = 60;
_LIT( KDefaultXdmUri,          "http://localhost" );
_LIT( KDefaultXdmAuthType,     "HTTP-DIGEST" );

// ==============================
// NAT/Firewall constants
// ==============================
//
const TInt KDefaultStunServerPort     = 3478;
const TInt KDefaultTcpRefreshInterval = 1200;
const TInt KDefaultUdpRefreshInterval = 28;
const TInt KDefaultStartPortRange     = 49152;
const TInt KDefaultEndPortRange       = 49200;
_LIT( KDefaultNatProtocol,             "nokia.stun" );

// ==============================
// WLAN access point constants.
// ==============================
//
const TInt KMaxWepKeyDataLength = 26;
const TInt KMaxWepKeyCount      = 4;
// Enumeration for WEP key lengths.
enum TWepKeyLengths
    {
    // Length of WEP encryption
    EWepEncryption64  = 64,
    EWepEncryption128 = 128,
    // Length of WEP key data
    EWepKey64Hex      = 10,
    EWepKey64Ascii    = 5,
    EWepKey128Hex     = KMaxWepKeyDataLength,
    EWepKey128Ascii   = 13
    };
_LIT( KNetworkModeInfra,     "infra" );
_LIT( KNetworkModeAdhoc,     "adhoc" );
_LIT( KSecurityTypeWep,      "WEP" );
_LIT( KSecurityTypeWpa,      "WPA" );
_LIT( KSecurityTypeWpa2,     "WPA2" );
_LIT( KSecurityType8021x,    "8021X" );
_LIT( KEapTypeLeap,          "LEAP" );
_LIT( KWepAuthModeOpen,      "open" );
_LIT( KWepAuthModeShared,    "shared" );

// ==============================
// General constants
// ==============================
//
// Enumeration for type of settings being deployed.
enum TSettingsType
    {
    EUnknown = KErrNotSupported,
    EVoip = 0,
    ESip,
    EPresence,
    EXdm,
    ENatFw,
    EDestination,
    EWlan,
    ECodec,
    EVmbx,
    EProxy,
    ERegistrar,
    EAdditionalStun,
    EWepKey,
    EEap
    };

// Enumeration for parameters.
enum TParameters
    {
    EUnSupported = KErrNotSupported,
    // Generic parameters
    EName = 0,
    EUri,
    ETransport,
    EPort,
    EUsername,
    EPassword,
    EType,
    EDomain,
    EStartPort,
    EEndPort,
    // VoIP parameters
    EMediaQos,
    EDtmfInband,
    EDtmfOutband,
    ESecureCallPreference,
    EAllowVoipOverWcdma,
    ERtcp,
    EUserAgentHeaderTerminalType,
    EUserAgentHeaderWlanMac,
    EUserAgentHeaderFreeString,
    ECallerIdDigits,
    EIgnoreDomainPart,
    EAutoAcceptBuddyRequests,
    EAddUserPhone,
    EProviderUrl,
    EMinSessionInterval,
    ESessionInterval,
    EBrandingUri,
    EAutoEnable,
    EEnableSipIm,
    EBrandId,
    EDefault,
    // Voice mailbox parameters
    EMwiUri,
    EListeningUri,
    EReSubscribeInterval,
    // Codec parameters
    EJitterBuffer,
    EOctetAlign,
    EPTime,
    EMaxPTime,
    EModeSet,
    EModeChangePeriod,
    EModeChangeNeighbor,
    EMaxRed,
    EVad,
    EAnnexb,
    // SIP parameters
    ESignalingQos,
    EPublicUserId,
    ESignalCompression,
    ESecurityNegotiation,
    EProfileLock,
    EAutoRegistration,
    // Proxy parameters
    ELooseRouting,
    // Presence parameters
    EMaxObjectSize,
    EPublishInterval,
    EMaxSubscriptions,
    EMaxContacts,
    EDomainSyntax,
    // NAT/Firewall parameters
    ETcpRefreshInterval,
    EUdpRefreshInterval,
    ECrlfRefresh,
    EStunSharedSecret,
    // WLAN access point parameters
    ESsid,
    EHidden,
    ENetworkMode,
    EPreSharedKey,
    EWepAuthMode,
    // WEP key parameters
    ELength,
    EData
    };

#endif // VOIPXMLPROCESSORDEFAULTS_H

// End of file
