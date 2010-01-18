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


#ifndef __RCSEPRIVATECRKEYS__
#define __RCSEPRIVATECRKEYS__

/** Central Repository UID of RCSE */
const TUid KCRUidRCSE = { 0x200100FD };

/** Central Repository UID of RCSE audio codecs */
const TUid KCRUidRCSECodec = { 0x2001B26B };


// Structure of VoIP profile table in repository. 
// Audio codec entries are stored equally.
// 
//----------------------------------------------------------------------------
//| KColProfileId | KColProviderName | KColSettingsName | ... | ... | KColxx |
//|     (int)     |     (des)        |     (des)        |     | ... | (des)  |
//----------------------------------------------------------------------------
//|       1       |   "Provider 1"   |  "Name x"        | ... | ... | "aaa"  |
//|       7       |   "Provider 2"   |  "Name y"        | ... | ... | "bbb"  |
//|       15      |   "Provider 3"   |  "Name z"        | ... | ... | "ccc"  |
//|       0       |      ""          |     ""           | ... | ... |        |
//|       0       |      ""          |     ""           | ... | ... |        |
//|       ...     |      ...         |     ...          | ... | ... |        |
//|       ...     |      ...         |     ...          | ... | ... |        |
//----------------------------------------------------------------------------



//************************ Single keys (actual keys) ************************

/** ID counter. Contains the next available ID for entry */
const TUint32 KRCSEIdCounter                = 0x00000000;

/** Notifier key for VoIP profiles */
//const TUint32 KRCSEVoIPProfileNotifier      = 0x00000010; // Not needed

/** Notifier key for Audio Codecs */
//const TUint32 KRCSEAudioCodecNotifier       = 0x00000011; // Not needed

//******************************** Key ranges *******************************

/** Range for general keys  */
const TUint32 KRCSESpecialKeys              = 0x00000000;   // --> 0x00000FFF


// Range for VoIP profile table (4096 rows)
const TUint32 KRCSEProfileTable             = 0x00001000;   // --> 0x0012FFFF


// Ranges for  Audio Codec table (4096 rows)
const TUint32 KRCSECodecTable               = 0x00001000;   // --> 0x0004FFFF

// Columns for VoIP profiles. Each column has 4096 rows
// Add new profile columns to the end of profile column list!
const TUint32 KColProfileId                     = KRCSEProfileTable;  // Int range!
const TUint32 KColProviderName                  = 0x00002000;
const TUint32 KColSettingsName                  = 0x00003000;
const TUint32 KColPreferredCodecs               = 0x00004000;
const TUint32 KColStartMediaPort                = 0x00005000;
const TUint32 KColEndMediaPort                  = 0x00006000;
const TUint32 KColSiqnalingQOS                  = 0x00007000;
const TUint32 KColMediaQOS                      = 0x00008000;
const TUint32 KColInbandDtmf                    = 0x00009000;
const TUint32 KColOutbandDtmf                   = 0x0000A000;   // 10

const TUint32 KColHoldRingBack                  = 0x0000B000;
const TUint32 KColAutoComplete                  = 0x0000C000;
const TUint32 KColCFNoAnswer                    = 0x0000D000;
const TUint32 KColCFBusy                        = 0x0000E000;
const TUint32 KColCFUnconditional               = 0x0000F000;
const TUint32 KColRedundancy                    = 0x00010000;
const TUint32 KColProtocolIds                   = 0x00011000;
const TUint32 KColSecureCallPreference          = 0x00012000;
const TUint32 KColVoIPProfileLock               = 0x00013000;
const TUint32 KColAdhocAllowed                  = 0x00014000;   // 20

const TUint32 KColSIPServerType                 = 0x00015000;
const TUint32 KColSBCType                       = 0x00016000;
const TUint32 KColSTUNServerType                = 0x00017000;
const TUint32 KColWLANAPType                    = 0x00018000;
const TUint32 KColPSTNGatewayType               = 0x00019000;
const TUint32 KColSecurityGatewayType           = 0x0001A000;
const TUint32 KColRTCP                          = 0x0001B000;
const TUint32 KColSIPVoIPUAHTerminalType        = 0x0001C000;
const TUint32 KColSIPVoIPUAHeaderWLANMAC        = 0x0001D000;
const TUint32 KColSIPVoIPUAHeaderString         = 0x0001E000;   // 30

const TUint32 KColProfileLockedToIAP            = 0x0001F000;
const TUint32 KColVoIPPluginUID                 = 0x00020000;
const TUint32 KColAllowVoIPoverWCDMA            = 0x00021000;
const TUint32 KColAllowVoIPoverBT               = 0x00022000;
const TUint32 KColMeanCountOfVoIPDigits         = 0x00023000;
const TUint32 KColIgnoreAddrDomainPart          = 0x00024000;
const TUint32 KColHandoverDialect               = 0x00025000;
const TUint32 KColPSTelephonyHOPreference       = 0x00026000;
const TUint32 KColHOThresholdValueLL            = 0x00027000;
const TUint32 KColHOThresholdValueHL            = 0x00028000;   // 40

const TUint32 KColNumberOfMeasurementsAbove     = 0x00029000;
const TUint32 KColNumberOfMeasurementsBelow     = 0x0002A000;
const TUint32 KColSmartScannInterParaHigh       = 0x0002B000;
const TUint32 KColSmartScannInterParaMedium     = 0x0002C000;
const TUint32 KColSmartScannInterParaLow        = 0x0002D000;
const TUint32 KColSmartScannInterParaStatic     = 0x0002E000;
const TUint32 KColSmartScannDurationHighMode    = 0x0002F000;
const TUint32 KColSmartScannDurationMediumMode  = 0x00030000;
const TUint32 KColSmartScannDurationLowMode     = 0x00031000;
const TUint32 KColHandoffNumber                 = 0x00032000;   // 50

const TUint32 KColHandbackNumber                = 0x00033000;
const TUint32 KColHysterisisTimer               = 0x00034000;
const TUint32 KColHandOffProcessTimer           = 0x00035000;
const TUint32 KColDisconnectProcessTimer        = 0x00036000;
const TUint32 KColHandoffPrefix                 = 0x00037000;
const TUint32 KColHandbackPrefix                = 0x00038000;
const TUint32 KColHandoverTones                 = 0x00039000;
const TUint32 KColSupportSMSoverWLAN            = 0x0003A000;
const TUint32 KColServiceProviderId             = 0x0003B000;
const TUint32 KColUserPhoneUriParam             = 0x0003C000;   // 60

const TUint32 KColSIPConnTestAddress            = 0x0003D000;
const TUint32 KColNATSettingsStorageId          = 0x0003E000;
const TUint32 KColSIPMinSE                      = 0x0003F000;
const TUint32 KColSIPSessionExpires             = 0x00040000;
const TUint32 KColNATProtocol                   = 0x00041000;
const TUint32 KColNewServiceTable               = 0x00042000;
const TUint32 KColSNAPId                        = 0x00043000;
const TUint32 KColCreationUrl                   = 0x00044000; // = KRCSELastColumnVoIP


// Columns for Audio Codecs. Each column has 4096 rows
// Add new codec columns to end of the audio codec list!
const TUint32 KColAudioCodecId                  = KRCSECodecTable;  // Int range!
const TUint32 KColMediaTypeName                 = 0x00002000;
const TUint32 KColMediaSubTypeName              = 0x00003000;
const TUint32 KColJitterBufferSize              = 0x00004000;
const TUint32 KColOctetAlign                    = 0x00005000;
const TUint32 KColModeSet                       = 0x00006000;
const TUint32 KColModeChangePeriod              = 0x00007000;
const TUint32 KColModeChangeNeighbor            = 0x00008000;
const TUint32 KColPtime                         = 0x00009000;
const TUint32 KColMaxptime                      = 0x0000A000;   // 10

const TUint32 KColCrc                           = 0x0000B000;
const TUint32 KColRobustSorting                 = 0x0000C000;
const TUint32 KColInterLeaving                  = 0x0000D000;
const TUint32 KColChannels                      = 0x0000E000;
const TUint32 KColVAD                           = 0x0000F000;
const TUint32 KColDTX                           = 0x00010000;
const TUint32 KColSamplingRate                  = 0x00011000;
const TUint32 KColAnnexb                        = 0x00012000;
const TUint32 KColModeChangeCapability          = 0x00013000;
const TUint32 KColMaxRed                        = 0x00014000; // KRCSELastColumnCodec


//********************************* Key masks *******************************

/** Increment for rows in table */
const TUint32 KRCSERowIncrement                 = 0x00000001;

const TUint32 KRCSEColIncrement                 = 0x00001000;

/** Query mask for all rows in setting tables */
const TUint32 KRCSEColumnMask                   = 0xFFFFF000;


//************************ Other key related constants **********************

/** Last column in current setting table */
const TInt KRCSELastColumnVoIP = KColCreationUrl;
const TInt KRCSELastColumnCodec = KColMaxRed;

/** Amount of columns in current setting tables */
const TInt KRCSEColumnCountVoIP  = 68;  // Cols until KRCSELastColumnVoIP
const TInt KRCSEColumnCountCodec = 20;  // Cols until KRCSELastColumnCodec

/** Amount of rows in setting tables */
const TInt KRCSERowCountVoIP  = 4096; // = 0x00001000
const TInt KRCSERowCountCodec = 4096; // = 0x00001000


/** ID not set */
const TInt KNoEntryId = 0;


#endif      // __RCSEPRIVATECRKEYS__

