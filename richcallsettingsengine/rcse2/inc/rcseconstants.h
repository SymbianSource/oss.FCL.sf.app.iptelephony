/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Constants
*
*/



#ifndef RCSECONSTANTS
#define RCSECONSTANTS


//============================================================================
// CONSTANTS

const TInt KDesLength256 = 256;    //KMaxProviderNameLength;
const TInt KDesLength128 = 128;
const TInt KDesLength64  = 64;
const TInt KDesLength15  = 15;

// Timeout in microseconds for waiting a semaphore
const TInt KRCSETimeOut = 5000000;  

// Identifies entry row in settings table
_LIT( KVoIPEntry, "VoIPEntry" );
_LIT( KCodecEntry, "CodecEntry" );

// Space character
_LIT( KSpace, " " );

// Profile specific settings names.
_LIT( KProtocolIds,                 "PRID"        );
_LIT( KSiqnalingQOS,                "SQOS"        );
_LIT( KSettingsName,                "SN"          );
_LIT( KAutoComplete,                "AC"          );
_LIT( KEndMediaPort,                "EMP"         );
_LIT( KHoldRingBack,                "HRB"         );
_LIT( KInbandDtmf,                  "IDTMF"       );
_LIT( KMediaQOS,                    "MQOS"        );
_LIT( KOutbandDtmf,                 "ODTMF"       );
_LIT( KPreferredCodecs,             "PC"          );
_LIT( KProfileId,                   "PI"          );
_LIT( KProviderName,                "PN"          );
_LIT( KRedundancy,                  "R"           );
_LIT( KStartMediaPort,              "SMP"         );
_LIT( KCFNoAnswer,                  "CFNA"        );
_LIT( KCFBusy,                      "CFB"         );
_LIT( KCFUnconditional,             "CFU"         );
_LIT( KSecureCallPreference,        "SCP"         );
_LIT( KVoIPProfileLock,             "VPL"         );
_LIT( KAdhocAllowed,                "AHA"         );
_LIT( KSIPServerType,               "SST"         );
_LIT( KSBCType,                     "SBCT"        );
_LIT( KSTUNServerType,              "STUNST"      );
_LIT( KWLANAPType,                  "WLANAPT"     );
_LIT( KPSTNGatewayType,             "PSTNGT"      );
_LIT( KSecurityGatewayType,         "SGT"         );
_LIT( KRTCP,                        "RTCP"        );
_LIT( KSIPVoIPUAHTerminalType,      "UATT"        );
_LIT( KSIPVoIPUAHeaderWLANMAC,      "UAWMAC"      );
_LIT( KSIPVoIPUAHeaderString,       "UAHS"        );
_LIT( KProfileLockedToIAP,          "PLTI"        );
_LIT( KVoIPPluginUID,               "VUID"        );
_LIT( KAllowVoIPoverWCDMA,          "AWCDMA"      );
_LIT( KAllowVoIPoverBT,             "ABT"         );
_LIT( KMeanCountOfVoIPDigits,       "MCVP"        );
_LIT( KIgnoreAddrDomainPart,        "IADR"        );

_LIT( KHandoverDialect,             "HD"    );
_LIT( KPSTelephonyHOPreference,     "PSHOP" );
_LIT( KHOThresholdValueLL,          "HOLL"  );
_LIT( KHOThresholdValueHL,          "HOHL"  );
_LIT( KNumberOfMeasurementsAbove,   "NMA"   );
_LIT( KNumberOfMeasurementsBelow,   "NMB"   );
_LIT( KSmartScannInterParaHigh,     "SSPH"  );
_LIT( KSmartScannInterParaMedium,   "SSPM"  );
_LIT( KSmartScannInterParaLow,      "SSPL"  );
_LIT( KSmartScannInterParaStatic,   "SSPS"  );

_LIT( KSmartScannDurationHighMode,      "SSHG"  );
_LIT( KSmartScannDurationMediumMode,    "SSMM"  );
_LIT( KSmartScannDurationLowMode,       "SSLM"  );
_LIT( KHandoffNumber,                   "HON"   );
_LIT( KHandbackNumber,                  "HBN"   );
_LIT( KHysterisisTimer,                 "HT"    );
_LIT( KHandOffProcessTimer,             "HOPT"  );
_LIT( KDisconnectProcessTimer,          "DPT"   );
_LIT( KHandoffPrefix,                   "HOP"   );
_LIT( KHandbackPrefix,                  "HBP"   );
_LIT( KHandoverTones,                   "HTO"   );
_LIT( KSupportSMSoverWLAN,              "SSW"   );

_LIT( KServiceProviderId,                "SPID"  );
_LIT( KUserPhoneUriParam,                "UPP"   );
_LIT( KSIPConnTestAddress,               "CTEST" );
_LIT( KNATSettingsStorageId,             "NSID"  );
_LIT( KSIPMinSE,                         "MINSE" );
_LIT( KSIPSessionExpires,                "SESEX" );
_LIT( KNATProtocol,                      "NAT"   );
_LIT( KNewServiceTable,                  "NEWST" );
_LIT( KSNAPId,                           "SNAP"  );

// SCCP settings names.
_LIT( KSCCPSettingId,               "SSI"         );
_LIT( KSCCPProfileName,             "SNAME"       );
_LIT( KVOIPLogin,                   "VPL"         );
_LIT( KAccessPoint,                 "AP"          );
_LIT( KCallManagerAdd0,             "CMA0"        );
_LIT( KCallManagerAdd1,             "CMA1"        );
_LIT( KCallManagerAdd2,             "CMA2"        );
_LIT( KCallManagerAdd3,             "CMA3"        );
_LIT( KCallManagerAdd4,             "CMA4"        );
_LIT( KStackVersion,                "SPS"         );
_LIT( KDHCPTFTPEnabled,             "DCTT"        );
_LIT( KTFTPServerAddress,           "TTSA"        );
_LIT( KPhoneNumber,                 "SPN"         );
_LIT( KSCCPCertificates,            "SSF"         );
_LIT( KMusicServerAdd,              "MS"          );
_LIT( KCFUncondAdd,                 "CFUA"        );
_LIT( KSCCPVmbx,                    "SVMBX"       );
_LIT( KSCCPIntCallPrefix,           "SICP"        );

// SIP Profile settings names.
_LIT( KVOIPSIPProfileSpecificId,    "VSPSI"       );
_LIT( KSIPProfileId2,               "SPI"         );
_LIT( KConferencingFactoryURI,      "CFURI"       );
_LIT( KMusicServerURI,              "MSURI"       );
_LIT( KIPVoiceMailBoxURI,           "IVMB"        );
_LIT( KCFNoAnsURI,                  "CFNAU"       );
_LIT( KCFBusyURI,                   "CFBURI"      );
_LIT( KCFUncondURI,                 "CFUURI"      );

// Audio Codec settings names.
_LIT( KAudioCodecId,                "ACI"         );
_LIT( KMediaTypeName,               "MTN"         );
_LIT( KMediaSubTypeName,            "MSTN"        );
_LIT( KJitterBufferSize,            "JBS"         );
_LIT( KOctetAlign,                  "OA"          );
_LIT( KModeSet,                     "ME"          );
_LIT( KModeChangePeriod,            "MCP"         );
_LIT( KModeChangeNeighbor,          "MCN"         );
_LIT( KPtime,                       "PT"          );
_LIT( KMaxptime,                    "MPT"         );
_LIT( KCrc,                         "CRC"         );
_LIT( KRobustSorting,               "RS"          );
_LIT( KInterLeaving,                "IL"          );
_LIT( KChannels,                    "CH"          );
_LIT( KVAD,                         "VAD"         );
_LIT( KDTX,                         "DTX"         );
_LIT( KSamplingRate,                "SR"          );
_LIT( KAnnexb,                      "AB"          );
_LIT( KModeChangeCapability,        "MCC"         );
_LIT( KMaxRed,                      "MR"          );



// Constants for supporting restore from db

// CONSTANTS
// Security uid, which identifies the security policy file used for this
// database.
_LIT( KDatabaseUID, "SECURE[10202869]" );

// Profile table name in db.
_LIT( KProfileTableName, "profile" );

// Audio Codec table name in Audio db.
_LIT( KAudioCodecTableName, "audiocodec" );

// SCCP table name in SCCP db.
_LIT( KSCCPSettingTableName, "sccpsetting" );

// SIP Profile table name in SIP db.
_LIT( KSIPTableName, "sipprofiles" );

//  SQL statement formats
_LIT( KSQLSelectAllStatement, "SELECT * FROM %S" );


#endif      // RCSECONSTANTS   
            
// End of File
