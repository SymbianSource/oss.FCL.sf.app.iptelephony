/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Default RCSE settings.
*
*/


#ifndef RCSEDEFAULTS_H
#define RCSEDEFAULTS_H


//  INCLUDES
#include <crcseprofileentry.h>
#include <crcseaudiocodecentry.h>
#include <crcsesccpentry.h>


// CONSTANTS

_LIT( KRCSEDefaultMediaSubtypeName, "" );
_LIT( KRCSEDefaultMediaTypeName, "Audio" );
_LIT( KRCSEDefaultSCCPStack, "" );
_LIT( KRCSEDefaultTFTPAddress, "" );
_LIT( KRCSEDefaultSCCPNumber, "" );
_LIT( KRCSEDefaultSCCPCertificates, "" );
_LIT( KRCSEDefaultMusicServerAdd, "" );
_LIT( KRCSEDefaultCFUncondAdd, "" );
_LIT( KRCSEDefaultConferenceFactoryURI, "" );
_LIT( KRCSEDefaultMusicServerURI, "" );
_LIT( KRCSEDefaultVoiceMailBoxURI, "" );
_LIT( KRCSEDefaultCFNoAnsURI, "" );
_LIT( KRCSEDefaultCFBusyURI, "" );
_LIT( KRCSEDefaultCFUncondURI, "" );
_LIT( KRCSEDefaultSCCPProfileName, "" );
_LIT( KRCSEDefaultSCCPVoiceMailboxName, "" );
_LIT( KRCSEDefaultSCCPIntCallPrefix, "" );

_LIT( KRCSEDefaultTest, "" );

_LIT( KRCSEDefaultProviderName, "Provider" );
_LIT( KRCSEDefaultSettingsName, "VoIP settings" );
_LIT( KRCSEDefaultMusicServerAddress, "" );
_LIT( KRCSEDefaultVoiceMailbox, "" );

_LIT( KRCSEDefaultSIPServerType,         "" );
_LIT( KRCSEDefaultSBCType,               "" );
_LIT( KRCSEDefaultSTUNServerType,        "" );
_LIT( KRCSEDefaultWLANAPType,            "" );
_LIT( KRCSEDefaultPSTNGatewayType,       "" );
_LIT( KRCSEDefaultSecurityGatewayType,   "" );
_LIT( KRCSEDefaultSIPVoIPUAHeaderString, "" );
const TUint32           KRCSEDefaultSecureCallPreference   = 0;
const TUint32           KRCSEDefaultVoIPProfileLock        = 0;
const TUint32           KRCSEDefaultAdhocAllowed           = 1;
const TUint32           KRCSEDefaultRTCP                   = 0;
const TUint32           KRCSEDefaultSIPVoIPUAHTerminalType = 1;
const TUint32           KRCSEDefaultSIPVoIPUAHeaderWLANMAC = 0;
const TInt32            KRCSEDefaultVoIPPluginUID          = 0;
const TInt32            KRCSEDefaultMeanCountOfVoIPDigits  = 0;
const TInt32            KRCSEDefaultIgnoreAddrDomainPart   = 0;
const CRCSEProfileEntry::TOnOff KRCSEDefaultProfileLockedToIAP  = CRCSEProfileEntry::EOff;
const CRCSEProfileEntry::TOnOff KRCSEDefaultAllowVoIPoverWCDMA  = CRCSEProfileEntry::EOff;
const CRCSEProfileEntry::TOnOff KRCSEDefaultAllowVoIPoverBT     = CRCSEProfileEntry::EOff;

const TInt32            KRCSEDefHandoverDialect            = 0;
const TInt32            KRCSEDefPSTelephonyHOPreference    = 0;
const TInt32            KRCSEDefHOThresholdValueLL         = 0;
const TInt32            KRCSEDefHOThresholdValueHL         = 0;
const TInt32            KRCSEDefNumberOfMeasurementsAbove  = 0;
const TInt32            KRCSEDefNumberOfMeasurementsBelow  = 0;
const TInt32            KRCSEDefSmartScannInterParaHigh    = 6000;
const TInt32            KRCSEDefSmartScannInterParaMedium  = 30000;
const TInt32            KRCSEDefSmartScannInterParaLow     = 110000;
const TInt32            KRCSEDefSmartScannInterParaStatic  = 0;

const TInt32            KRCSEDefSmartScannDurationHighMode     = 60;
const TInt32            KRCSEDefSmartScannDurationMediumMode   = 900;
const TInt32            KRCSEDefSmartScannDurationLowMode      = 0;
const TInt32            KRCSEDefHandoffNumber                  = 0;
const TInt32            KRCSEDefHandbackNumber                 = 10000;
const TInt32            KRCSEDefHysterisisTimer                = 10000;
const TInt32            KRCSEDefHandOffProcessTimer            = 10000;
const TInt32            KRCSEDefDisconnectProcessTimer         = 5000;
_LIT( KRCSEHandoffPrefix,                     "" );
_LIT( KRCSEHandbackPrefix,                    "" );
const TInt32            KRCSEDefHandoverTones                  = 0;
const TInt32            KRCSEDefSupportSMSoverWLAN  = 0;

_LIT( KRCSEDefXCAPRootURI,                     "" );
_LIT( KRCSEDefXCAPAddressAutheticationProxy,   "" );
_LIT( KRCSEDefXCAPUserName,                    "" );
_LIT( KRCSEDefXCAPPassword,                    "" );
const TInt32            KRCSEDefUseSIPNamePassword                 = 0;
const TInt32            KRCSEDefAuthenticationMethod               = 0;
const TInt32            KRCSEDefTimeoutValueTerminalVoIPPresence   = 3600;

const TUint32           KRCSEDefServiceProviderId                  = 0;
const CRCSEProfileEntry::TOnOff KRCSEDefUserPhoneUriParam  = CRCSEProfileEntry::EOff;
_LIT(                   KRCSEDefSIPConnTestAddress,                "" );
const TUint32           KRCSEDefNATSettingsStorageId               = 0;
const TInt32            KRCSEDefSIPMinSE                           = 90;
const TInt32            KRCSEDefSIPSessionExpires                  = 1800;
const TInt32            KRCSEDefNATProtocol                        = KNotSet;
const CRCSEProfileEntry::TOnOff KRCSENewServiceTableEntry          = CRCSEProfileEntry::EOn;
const TUint32           KRCSEDefSNAPId                             = 0;

// iPreferredCodecs is an array and default value is set in ResetDefaultValues()

const TUint32                       KRCSEDefaultStartMediaPort         = 49152;
const TUint32                       KRCSEDefaultEndMediaPort           = 65534;
const TInt32                        KRCSEDefaultSiqnalingQOS           = 40;
const TInt32                        KRCSEDefaultMediaQOS               = 46;
const CRCSEProfileEntry::TOnOff     KRCSEDefaultInbandDTMF             = CRCSEProfileEntry::EOn;
const CRCSEProfileEntry::TOnOff     KRCSEDefaultOutbandDTMF            = CRCSEProfileEntry::EOn;
const TInt32                        KRCSEDefaultHoldRingBack           = 3;
const CRCSEProfileEntry::TOnOff     KRCSEDefaultAutoComplete           = CRCSEProfileEntry::EOff;
const CRCSEProfileEntry::TOnOff     KRCSEDefaultVoiceActivation        = CRCSEProfileEntry::EOn;
const CRCSEProfileEntry::TOnOff     KRCSEDefaultRedundancy             = CRCSEProfileEntry::EOff;
const CRCSEProfileEntry::TOnOff     KRCSEDefaultVoipPreferred          = CRCSEProfileEntry::EOff;

const CRCSEProfileEntry::TOnOff     KRCSEDefaultCFNoAnswer             = CRCSEProfileEntry::EOff;
const CRCSEProfileEntry::TOnOff     KRCSEDefaultCFBusy                 = CRCSEProfileEntry::EOff;
const CRCSEProfileEntry::TOnOff     KRCSEDefaultCFUnconditional        = CRCSEProfileEntry::EOff;

// CRCSEAudioCodecEntry default values.
const TInt32                            KRCSEDefaultJitterBufferSize   = 200;
const TInt32                            KRCSEDefaultMaxptime           = 200;
const CRCSEAudioCodecEntry::TOnOff      KRCSEDefaultOctetAlign         = CRCSEAudioCodecEntry::EOONotSet;
const CRCSEAudioCodecEntry::TOnOff      KRCSEDefaultModeChangeNeighbor = CRCSEAudioCodecEntry::EOONotSet;
const CRCSEAudioCodecEntry::TOnOff      KRCSEDefaultCRC                = CRCSEAudioCodecEntry::EOONotSet;
const CRCSEAudioCodecEntry::TOnOff      KRCSEDefaultRobustSorting      = CRCSEAudioCodecEntry::EOONotSet;
const CRCSEAudioCodecEntry::TOnOff      KRCSEDefaultVAD                = CRCSEAudioCodecEntry::EOONotSet;
const CRCSEAudioCodecEntry::TOnOff      KRCSEDefaultDTX                = CRCSEAudioCodecEntry::EOONotSet;
const CRCSEAudioCodecEntry::TOnOff      KRCSEDefaultAnnexb             = CRCSEAudioCodecEntry::EOff;
const TInt32                            KRCSEDefaultModeChangeCapability = KNotSet;
const TInt32                            KRCSEDefaultMaxRed               = KNotSet;

// CRCSESCCPEntry default values.
const CRCSESCCPEntry::TManualAutomatic   KRCSEDefaultVOIPLogin         = CRCSESCCPEntry::EAutomatic;
const CRCSESCCPEntry::TOnOff             KRCSEDefaultDHCPTFTP          = CRCSESCCPEntry::EOn;



// Default service provider settings for VoIP profile
const TInt32 KPhoneAppCLIPlugId                    = 0x10282EB2;
const TInt32 KPhoneAppCallLogPlugId                = 0x10282EB4;
const TInt32 KPhoneAppCallStateIndPlugId           = 0x10282EB8;
const TInt32 KPhoneAppCallMenuHandlerPlugId        = 0x10282EB6;
const TInt32 KCCECallProviderPlugId                = 0x10282589;
const TInt32 KCCHVoIPSubServicePlugId              = 0x1027545A;
const TInt32 KCCHVMBXSubServicePlugId              = 0x1027545A;
const TInt32 KPresenceServiceConnPlugId            = 0x10282EF2;
const TInt32 KContactViewPlugId                    = 0x20012423;

const TInt32 KLogCntLinkMaxLength                  = 256;
const TInt32 KPresencePresentityIDFieldType        = 52;

_LIT( KContactStoreId, "cntdb://c:contacts.cdb" );
_LIT( KVoIPBrandId, "10282EC0" );

const TUint KDefaultVoIPService = 2;

const TUint KVoIPSettingsID = 1;

#endif      // RCSEDEFAULTS_H 
            
// End of File
