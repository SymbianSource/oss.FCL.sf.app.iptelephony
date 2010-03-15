/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Device Management VoIP adapter
*
*/


// INCLUDE FILES
#include <f32file.h>
#include <sysutil.h>
#include <settingsinternalcrkeys.h>
#include <centralrepository.h>
#include <wlaninternalpskeys.h> // For GetWlanacAddress and GetWlanTypeL.
#include <badesca.h> // For CDesC8ArrayFlat used in CompleteOutstandingCmdsL.
#include <coemain.h> // For GetPhoneModelL.
#include <crcseprofileregistry.h>
#include <crcseprofileentry.h>
#include <crcseaudiocodecregistry.h>
#include <crcseaudiocodecentry.h>
#include <utf.h>
#include <sipmanagedprofile.h> // For SIP User Agent Header.
#include <sipprofileregistryobserver.h> // For SIP User Agent Header.
#include <sipmanagedprofileregistry.h> // For SIP User Agent Header.
#include <spdefinitions.h> // For Service Provider Settings.
#include <spsettings.h>    // For Service Provider Settings.
#include <spentry.h>       // For Service Provider Settings.
#include <spproperty.h>    // For Service Provider Settings.
#include <featmgr.h>
#include <nsmldmiapmatcher.h> // For fetching IAP.
#include <pathinfo.h> // For getting phone rom root path.
#include <cvimpstsettingsstore.h> // For IM tone path

#include "nsmldmvoipadapter.h"
#include "cdmvoipspsettings.h"
#include "CSIPProfileRegistryObserver.h" // For SIP User Agent Header.
#include "cipappphoneutils.h"            // For GetPhoneModelL, GetTerminalTypeL
#include "cipapputilsaddressresolver.h"  // For GetWlanMacAddress
#include "smldmvoipdebug.h"

// CONSTANTS

const TInt   KNSmlVoIPMaxUriLength     = 256;
const TInt   KDefaultResultSize        = 64;
const TInt   KNSmlVoIPMaxResultLength  = 256;
const TInt   KNSmlDMFatMinimumFileSize = 512;
const TUint  KTempStringlength = 200;
_LIT8( KSpaceMark,         " " );
_LIT8( KUserAgent,         "User-Agent" );
_LIT8( KColonMark,         ":" );

_LIT8( KNSmlDMVoIPDDFVersion,                "1.0" ); 
_LIT8( KNSmlDMVoIPNodeName,                  "VoIP" );
_LIT8( KNSmlDMVoIPPrefix,                    "VoIPId" );
_LIT8( KNSmlDMVoIPDynamicNode,               "" );
_LIT8( KNSmlDMVoIPSetID,                     "VoIPId" ); 
_LIT8( KNSmlDMVoIPProvID,                    "ProviderName" ); 
_LIT8( KNSmlDMVoIPName,                      "SettingsName" );
_LIT8( KNSmlDMVoIPPreferredCodecsNode,       "PreferredCodecs" );

// VoIP/x/CodecSettings
_LIT8( KNSmlDMVoIPCodecSettingsNode,         "CodecSettings" );
_LIT8( KNSmlDMVoIPCodecSettingsId,           "CodecSettingsId" );
_LIT8( KNSmlDMVoIPCodecId,                   "VoIPCodecId" );
_LIT8( KNSmlDMVoIPPreferredCodecId,          "PreferredCodecId" );
_LIT8( KNSmlDMVoIPMediaTypeName,             "MediaTypeName" );
_LIT8( KNSmlDMVoIPMediaSubTypeName,          "MediaSubTypeName" );
_LIT8( KNSmlDMVoIPJitterBufferSize,          "JitterBufferSize" );
_LIT8( KNSmlDMVoIPOctetAlign,                "OctetAlign" );
_LIT8( KNSmlDMVoIPModeSetNode,               "ModeSet" );
_LIT8( KNSmlDMVoIPMode,                      "Mode" );
_LIT8( KNSmlDMVoIPModeId,                    "ModeId" );
_LIT8( KNSmlDMVoIPModeChangePeriod,          "ModeChangePeriod" );
_LIT8( KNSmlDMVoIPModeChangeNeighbor,        "ModeChangeNeighbor" );
_LIT8( KNSmlDMVoIPPtime,                     "Ptime" );
_LIT8( KNSmlDMVoIPMaxPtime,                  "MaxPtime" );
_LIT8( KNSmlDMVoIPVAD,                       "VAD" ); 
_LIT8( KNSmlDMVoIPAnnexB,                    "AnnexB" );
_LIT8( KNSmlDMVoIPMaxRed,                    "MaxRed" );

// VoIP/x/SettingIds node
_LIT8( KNSmlDMVoIPSettingIdsNode,            "SettingIds" );
_LIT8( KNSmlDMVoIPSettingId,                 "SettingId" );
_LIT8( KNSmlDMVoIPProfileType,               "ProfileType" ); 
_LIT8( KNSmlDMVoIPProfileId,                 "ProfileId" );
_LIT8( KNSmlDMVoIPProfileSpecificId,         "ProfileSpecificId" );
_LIT8( KNSmlDMVoIPProfileTypeSIP,            "SIP" );
_LIT8( KNSmlDMVoIPProfileTypeSCCP,           "SCCP" );

// VoIP/x/
_LIT8( KNSmlDMVoIPSMPort,                    "StartMediaPort" );
_LIT8( KNSmlDMVoIPEMPort,                    "EndMediaPort" );
_LIT8( KNSmlDMVoIPMediaQos,                  "MediaQos" );
_LIT8( KNSmlDMVoIPDTMFIB,                    "InbandDTMF" );
_LIT8( KNSmlDMVoIPDTMFOB,                    "OutbandDTMF" );
_LIT8( KNSmlDMVoIPSecureCallPref,            "SecureCallPref" );
_LIT8( KNSmlDMVoIPRTCP,                      "RTCP" );
_LIT8( KNSmlDMVoIPUAHTerminalType,           "UAHTerminalType" );
_LIT8( KNSmlDMVoIPUAHWLANMAC,                "UAHWLANMAC" );
_LIT8( KNSmlDMVoIPUAHString,                 "UAHString" );
_LIT8( KNSmlDMVoIPProfileLockedToIAP,        "ProfileLockedToIAP" );
_LIT8( KNSmlDMVoIPPluginUID,                 "VoIPPluginUID" );
_LIT8( KNSmlDMVoIPAllowVoIPOverWCDMA,        "AllowVoIPOverWCDMA" );
_LIT8( KNSmlDMVoIPVoIPDigits,                "VoIPDigits" );
_LIT8( KNSmlDMVoIPURIDomainIgnoreRule,       "URIDomainIgnoreRule" );
_LIT8( KNSmlDMVoIPAutoAcceptBuddyRequest,    "AutoAcceptBuddyRequest" );
_LIT8( KNSmlDMVoIPUsedVoIPRelease,           "UsedVoIPRelease" );
_LIT8( KNSmlDMVoIPAddUserPhone,              "AddUserPhone" );
_LIT8( KNSmlDMVoIPProviderBookmarkUri,       "ProviderBookmarkURI" );
_LIT8( KNSmlDMVoIPSIPConnTestUri,            "SIPConnectivityTestURI" );
_LIT8( KNSmlDMVoIPNatFwProfileId,            "NATFWProfileId" );
_LIT8( KNSmlDMVoIPMinSessionInterval,        "MinSessionInterval" );
_LIT8( KNSmlDMVoIPSessionExpires,            "SessionExpires" );
_LIT8( KNSmlDMVoIPBrandingDataUri,           "BradingDataURI" );
_LIT8( KNSmlDMVoIPPresenceSettingsId,        "PresenceSettingsId" );
_LIT8( KNSmlDMVoIPUsedNatProtocol,           "UsedNATProtocol" );
_LIT8( KNSmlDMVoIPAutoEnable,                "AutoEnable" );
_LIT8( KNSmlDMVoIPSnapProfileId,             "SNAPProfileId" );
_LIT8( KNSmlDMVoIPEnableIm,                  "EnableIM" );

// VoIP/x/VoiceMailBox
_LIT8( KNSmlDMVoIPVmbxNode,                  "VoiceMailBox" );
_LIT8( KNSmlDMVoIPMwiUri,                    "MWI-URI" );
_LIT8( KNSmlDMVoIPListeningUri,              "ListeningURI" );
_LIT8( KNSmlDMVoIPUsedIAPId,                 "UsedIAPId" );
_LIT8( KNSmlDMVoIPReSubscribeInterval,       "ReSubscribeInterval" );
_LIT8( KNSmlDMVoIPUsedSIPProfileId,          "UsedSIPProfileId" );

// Explanations of parameters
_LIT8( KNSmlDMVoIPNodeNameExp,               "The interior object holds all VoIP objects" );
_LIT8( KNSmlDMVoIPDynamicNodeExp,            "Placeholder for settings ID's" );
_LIT8( KNSmlDMVoIPSetIDExp,                  "Settings ID number" );
_LIT8( KNSmlDMVoIPProvIDExp,                 "Provider ID" ); 
_LIT8( KNSmlDMVoIPNameExp,                   "Name of the settings" );

// CodecSettings
_LIT8( KNSmlDMVoIPCodecOrderExp,             "Codec order" );
_LIT8( KNSmlDMVoIPCodecSettingsNodeExp,      "CodecSettings" );
_LIT8( KNSmlDMVoIPCodecSettingsIdExp,        "CodecSettingsID" );
_LIT8( KNSmlDMVoIPMediaTypeNameExp,          "MediaTypeName" );
_LIT8( KNSmlDMVoIPMediaSubTypeNameExp,       "MediaSubTypeName" );
_LIT8( KNSmlDMVoIPJitterBufferSizeExp,       "JitterBufferSize" );
_LIT8( KNSmlDMVoIPOctetAlignExp,             "OctetAlign" );
_LIT8( KNSmlDMVoIPModeSetNodeExp,            "ModeSet" );
_LIT8( KNSmlDMVoIPModeSetIdExp,              "Mode Id" );
_LIT8( KNSmlDMVoIPModeChangePeriodExp,       "ModeChangePeriod" );
_LIT8( KNSmlDMVoIPModeChangeNeighborExp,     "ModeChangeNeighbor" );
_LIT8( KNSmlDMVoIPPtimeExp,                  "Ptime" );
_LIT8( KNSmlDMVoIPMaxPtimeExp,               "MaxPtime" );
_LIT8( KNSmlDMVoIPVADExp,                    "Voice activation detection" );
_LIT8( KNSmlDMVoIPAnnexBExp,                 "AnnexB" );
_LIT8( KNSmlDMVoIPMaxRedExp,                 "Max-red" );

// VoIP/x/
_LIT8( KNSmlDMVoIPSMPortExp,                 "Start media port" );
_LIT8( KNSmlDMVoIPEMPortExp,                 "End media port" );
_LIT8( KNSmlDMVoIPMediaQosExp,               "Media QOS" );
_LIT8( KNSmlDMVoIPDTMFIBExp,                 "DTMF in-band" );
_LIT8( KNSmlDMVoIPDTMFOBExp,                 "DTMF out-band" );
_LIT8( KNSmlDMVoIPSecureCallPrefExp,         "Secure call preference");
_LIT8( KNSmlDMVoIPRTCPExp,                   "Real-Time Control Protocol reporting" );
_LIT8( KNSmlDMVoIPUAHTerminalTypeExp,        "SIP VoIP User Agent header: terminal type display" );
_LIT8( KNSmlDMVoIPUAHWLANMACExp,             "SIP VoIP User Agent header: WLAN MAC address display " );
_LIT8( KNSmlDMVoIPUAHStringExp,              "SIP VoIP User Agent header: free string" );
_LIT8( KNSmlDMVoIPProfileLockedToIAPExp,     "Profile locked to IAP" );
_LIT8( KNSmlDMVoIPPluginUIDExp,              "VoIP plugin UID" );
_LIT8( KNSmlDMVoIPAllowVoIPOverWCDMAExp,     "Allow VoIP over WCDMA" );
_LIT8( KNSmlDMVoIPVoIPDigitsExp,             "Number of meaningful VoIP digits" );
_LIT8( KNSmlDMVoIPURIDomainIgnoreRuleExp,    "Domain part of URI ignore rule" );
_LIT8( KNSmlDMVoIPAutoAcceptBuddyRequestExp, "Auto-accept buddy request" );
_LIT8( KNSmlDMVoIPUsedVoIPReleaseExp,        "Used VoIP release" );
_LIT8( KNSmlDMVoIPAddUserPhoneExp,           "Add user=phone to all numbers" );
_LIT8( KNSmlDMVoIPProviderBookmarkUriExp,    "Provider bookmark URI" );
_LIT8( KNSmlDMVoIPSIPConnTestUriExp,         "SIP connectivity test URI" );
_LIT8( KNSmlDMVoIPNatFwProfileIdExp,         "NATFW profile ID" );
_LIT8( KNSmlDMVoIPMinSessionIntervalExp,     "Minimum value for session interval" );
_LIT8( KNSmlDMVoIPSessionExpiresExp,         "Session interval for SIP session" );
_LIT8( KNSmlDMVoIPBrandingDataUriExp,        "Brading data URI" );
_LIT8( KNSmlDMVoIPPresenceSettingsIdExp,     "Presence settings ID" );
_LIT8( KNSmlDMVoIPUsedNatProtocolExp,        "Used NAT Protocol" );
_LIT8( KNSmlDMVoIPAutoEnableExp,             "Auto enable the service" );
_LIT8( KNSmlDMVoIPSnapProfileIdExp,          "SNAP profile ID" );
_LIT8( KNSmlDMVoIPEnableImExp,               "Enable IM" );

// VoIP/x/VoiceMailBox
_LIT8( KNSmlDMVoIPVmbxNodeExp,               "VoiceMailBox node" );
_LIT8( KNSmlDMVoIPMwiUriExp,                 "MWI-URI" );
_LIT8( KNSmlDMVoIPListeningUriExp,           "Listening-URI" );
_LIT8( KNSmlDMVoIPUsedIAPIdExp,              "Used IAP ID" );
_LIT8( KNSmlDMVoIPReSubscribeIntervalExp,    "Re-rubscribe interval" );
_LIT8( KNSmlDMVoIPUsedSIPProfileIdExp,       "Used SIP profile ID" );

// VoIP/x/SettingIds node
_LIT8( KNSmlDMVoIPProfileTypeExp,            "ProfileType" ); 
_LIT8( KNSmlDMVoIPProfileIdExp,              "ProfileId" );
_LIT8( KNSmlDMVoIPProfileSpecificExp,        "ProfileSpecificId" );

// Other
_LIT8( KNSmlDMVoIPValueTrue,                 "True" );
_LIT8( KNSmlDMVoIPValueFalse,                "False" );
_LIT8( KNSmlDMVoIPValueNotSet,               "NotSet" );
_LIT8( KNSmlDefDMSIP,                        "SIP" );
_LIT8( KNSmlDefDMSCCP,                       "SCCP/SCCPAcc" );
_LIT8( KNSmlDMVoIPTextPlain,                 "text/plain" );
_LIT8( KNSmlDMVoIPSeparator,                 "/" );
_LIT8( KNSmlVoIPUriDotSlash,                 "./");
_LIT( KOpenParenthesis,                      "(" );
_LIT( KClosedParenthesis,                    ")" );
_LIT8( KDefaultSettingsName,                 "DMAdapterIns" );
_LIT8( KNSmlDMSIPSipID,                      "SIPId" );
_LIT8( KNSmlDMSCCPSccpID,                    "SCCPId" );
_LIT8( KNSmlDMNATFWDomain,                   "NATFW/DomainSpecific" );
_LIT8( KNSmlDMPresence,                      "OMA_PRESENCE" );
_LIT8( KNSmlDMSnapUriPrefix,                 "BearerManagementSNAP" );
const TInt KSixSegs   = 6;
const TInt KFiveSegs  = 5;
const TInt KFourSegs  = 4;
const TInt KThreeSegs = 3;
const TInt KTwoSegs   = 2;

const TUint8 KDMVoIPSeparator = '/';

// VoiceMailBox related constants.
const TInt KDefaultReSubscribe = 600;
// IM related constants.
const TUint32 KIMSubServicePluginId = 0x1027545A; // KSIPConnectivityPluginImplUid
const TUint32 KIMLaunchUid          = 0x200255D0;
const TInt    KIMSettingsId         = 1;
// Default IM message tone
_LIT( KDefaultTone,       "Message 2.aac" );

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CNSmlDmVoIPAdapter::ConstructL()
    {
    DBG_PRINT( "CNSmlDmVoIPAdapter::ConstructL(): begin" );
    // Check VoIP support from feature manager
    FeatureManager::InitializeLibL();
    TBool support = FeatureManager::FeatureSupported( KFeatureIdCommonVoip );
    FeatureManager::UnInitializeLib();

    if ( !support )
        {
        DBG_PRINT( "CNSmlDmVoIPAdapter::ConstructL(): no support" );        
        User::Leave( KErrNotSupported );
        }
    DBG_PRINT( "CNSmlDmVoIPAdapter::ConstructL(): end" );
    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter* CNSmlDmVoIPAdapter::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CNSmlDmVoIPAdapter* CNSmlDmVoIPAdapter::NewL(
    MSmlDmCallback* aDmCallback )
    {
    DBG_PRINT("CNSmlDmVoIPAdapter::NewL(): begin");
    CNSmlDmVoIPAdapter* self = new (ELeave) CNSmlDmVoIPAdapter( aDmCallback );
    self->ConstructL();
    self->iDmCallback = aDmCallback;
    DBG_PRINT("CNSmlDmVoIPAdapter::NewL(): end");
    return self;
    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::~CNSmlDmVoIPAdapter
// ---------------------------------------------------------------------------
//
CNSmlDmVoIPAdapter::~CNSmlDmVoIPAdapter()
    {
    DBG_PRINT("CNSmlDmVoIPAdapter::~CNSmlDmVoIPAdapter(): begin");

    iCodecEntries.ResetAndDestroy();
    iCodecEntries.Close();

    iProfileEntries.ResetAndDestroy();
    iProfileEntries.Close();

    if ( iCRCSEAudioCodec )
        {
        delete iCRCSEAudioCodec;
        }

    if ( iCRCSEProfile )
        {
        delete iCRCSEProfile; 
        }

    iProfileModifs.Close();
    iCodecModifs.Close();

    iSPSettings.ResetAndDestroy();
    iSPSettings.Close();

    DBG_PRINT("CNSmlDmVoIPAdapter::~CNSmlDmVoIPAdapter(): end");
    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::LoadProfilesL
// ---------------------------------------------------------------------------
//
void CNSmlDmVoIPAdapter::LoadProfilesL()
    {
    DBG_PRINT("CNSmlDmVoIPAdapter::LoadProfilesL(): begin");

    if ( !iCRCSEProfile )
        {
        iCRCSEProfile = CRCSEProfileRegistry::NewL();
        }

    if ( !iCRCSEAudioCodec )
        {
        iCRCSEAudioCodec = CRCSEAudioCodecRegistry::NewL();
        }

    // Load profiles to pointerarray
    RArray<TUint32> allIds;
    iCRCSEProfile->GetAllIdsL( allIds ); 
    TInt idCount = allIds.Count();
    for ( TInt counter = 0; counter < idCount; counter++ )
        {
        CRCSEProfileEntry* profile = CRCSEProfileEntry::NewLC();
        iCRCSEProfile->FindL( allIds[counter], *profile );
        // Ownership change to iProfileEntries:
        iProfileEntries.AppendL( profile );
        CleanupStack::Pop( profile );
        }

    allIds.Reset();

    // Load audio codecs information to pointerarray
    iCRCSEAudioCodec->GetAllCodecIdsL( allIds );
    idCount = allIds.Count();
    for ( TInt counter = 0; counter < idCount; counter++ )
        {
        CRCSEAudioCodecEntry* codec = CRCSEAudioCodecEntry::NewLC();
        iCRCSEAudioCodec->FindL( allIds[counter], *codec );
        // Ownership change to iCodecEntries:
        iCodecEntries.AppendL( codec );
        CleanupStack::Pop( codec );
        }
    allIds.Close();
    DBG_PRINT("CNSmlDmVoIPAdapter::LoadProfilesL(): end");
    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::CNSmlDmVoIPAdapter
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CNSmlDmVoIPAdapter::CNSmlDmVoIPAdapter( TAny* aEcomArguments ):
    CSmlDmAdapter( aEcomArguments )
    {
    DBG_PRINT("CNSmlDmVoIPAdapter::CNSmlDmVoIPAdapter(aEcomArguments): begin");
    DBG_PRINT("CNSmlDmVoIPAdapter::CNSmlDmVoIPAdapter(aEcomArguments): end");   
    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::DDFVersionL
// Inserts DDF version of the adapter to aDDFVersion
// ---------------------------------------------------------------------------
//
void CNSmlDmVoIPAdapter::DDFVersionL( CBufBase& aDDFVersion )
    {
    DBG_PRINT("CNSmlDmVoIPAdapter::DDFVersionL(TDes& aDDFVersion): begin");
    aDDFVersion.InsertL( 0, KNSmlDMVoIPDDFVersion );
    DBG_PRINT("CNSmlDmVoIPAdapter::DDFVersionL(TDes& aDDFVersion): end");
    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::DDFStructureL
// Builds the DDF structure of adapter
// ---------------------------------------------------------------------------
//
void CNSmlDmVoIPAdapter::DDFStructureL( MSmlDmDDFObject& aDDF )
    {
    DBG_PRINT("CNSmlDmVoIPAdapter::DDFStructureL(): begin");

    TSmlDmAccessTypes accessTypesGet;
    accessTypesGet.SetGet();

    TSmlDmAccessTypes accessTypesGetAdd;
    accessTypesGetAdd.SetGet();
    accessTypesGetAdd.SetAdd();

    TSmlDmAccessTypes accessTypesGetReplaceAdd;
    accessTypesGetReplaceAdd.SetGet();
    accessTypesGetReplaceAdd.SetReplace();
    accessTypesGetReplaceAdd.SetAdd();

    TSmlDmAccessTypes accessTypesAll;
    accessTypesAll.SetGet();
    accessTypesAll.SetDelete();
    accessTypesAll.SetAdd();
    accessTypesAll.SetReplace();

    // VoIP
    MSmlDmDDFObject& voip = aDDF.AddChildObjectL( KNSmlDMVoIPNodeName );
    voip.SetAccessTypesL( accessTypesGetAdd );
    voip.SetOccurenceL( MSmlDmDDFObject::EOne );
    voip.SetScopeL( MSmlDmDDFObject::EPermanent );
    voip.SetDFFormatL( MSmlDmDDFObject::ENode );
    voip.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    voip.SetDescriptionL( KNSmlDMVoIPNodeNameExp );

    // dynamic settings ID node
    MSmlDmDDFObject& idNode = voip.AddChildObjectGroupL();
    idNode.SetAccessTypesL( accessTypesAll );
    idNode.SetOccurenceL( MSmlDmDDFObject::EZeroOrMore );
    idNode.SetScopeL( MSmlDmDDFObject::EDynamic );
    idNode.SetDFFormatL( MSmlDmDDFObject::ENode );
    idNode.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    idNode.SetDescriptionL( KNSmlDMVoIPDynamicNode );

    // VoIPId
    MSmlDmDDFObject& setID = idNode.AddChildObjectL( 
        KNSmlDMVoIPSetID );
    setID.SetAccessTypesL( accessTypesGet );
    setID.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    setID.SetScopeL( MSmlDmDDFObject::EDynamic );
    setID.SetDFFormatL( MSmlDmDDFObject::EInt );
    setID.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    setID.SetDescriptionL( KNSmlDMVoIPSetIDExp );

    // ProviderName
    MSmlDmDDFObject& provID = idNode.AddChildObjectL( 
        KNSmlDMVoIPProvID );
    provID.SetAccessTypesL( accessTypesGetReplaceAdd );
    provID.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    provID.SetScopeL( MSmlDmDDFObject::EDynamic );
    provID.SetDFFormatL( MSmlDmDDFObject::EChr );
    provID.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    provID.SetDescriptionL( KNSmlDMVoIPProvIDExp );

    // SettingsName
    MSmlDmDDFObject& name = idNode.AddChildObjectL( 
        KNSmlDMVoIPName );
    name.SetAccessTypesL( accessTypesGetReplaceAdd );
    name.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    name.SetScopeL( MSmlDmDDFObject::EDynamic );
    name.SetDFFormatL( MSmlDmDDFObject::EChr );
    name.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    name.SetDescriptionL( KNSmlDMVoIPNameExp );

    // PreferredCodecs node
    MSmlDmDDFObject& codecOrder = idNode.AddChildObjectL( 
        KNSmlDMVoIPPreferredCodecsNode );
    codecOrder.SetAccessTypesL( accessTypesGet );
    codecOrder.SetOccurenceL( MSmlDmDDFObject::EOne );
    codecOrder.SetScopeL( MSmlDmDDFObject::EDynamic );
    codecOrder.SetDFFormatL( MSmlDmDDFObject::ENode );
    codecOrder.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    codecOrder.SetDescriptionL( KNSmlDMVoIPCodecOrderExp );

    // PreferredCodecs dynamic
    MSmlDmDDFObject& codecDyn = codecOrder.AddChildObjectGroupL();
    codecDyn.SetAccessTypesL( accessTypesGet );
    codecDyn.SetOccurenceL( MSmlDmDDFObject::EZeroOrMore );
    codecDyn.SetScopeL( MSmlDmDDFObject::EDynamic );
    codecDyn.SetDFFormatL( MSmlDmDDFObject::ENode );
    codecDyn.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    codecDyn.SetDescriptionL( KNSmlDMVoIPCodecOrderExp );

    // PreferredCodecs/PreferredCodecId
    MSmlDmDDFObject& preferredCodecID = codecDyn.AddChildObjectL( 
        KNSmlDMVoIPPreferredCodecId );
    preferredCodecID.SetAccessTypesL( accessTypesGetReplaceAdd );
    preferredCodecID.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    preferredCodecID.SetScopeL( MSmlDmDDFObject::EDynamic );
    preferredCodecID.SetDFFormatL( MSmlDmDDFObject::EInt );
    preferredCodecID.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    preferredCodecID.SetDescriptionL( KNSmlDMVoIPCodecSettingsIdExp );

    // CodecSettings node
    MSmlDmDDFObject& codecNode = idNode.AddChildObjectL( 
        KNSmlDMVoIPCodecSettingsNode );
    codecNode.SetAccessTypesL( accessTypesGetAdd );
    codecNode.SetOccurenceL( MSmlDmDDFObject::EOne );
    codecNode.SetScopeL( MSmlDmDDFObject::EDynamic );
    codecNode.SetDFFormatL( MSmlDmDDFObject::ENode );
    codecNode.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    codecNode.SetDescriptionL( KNSmlDMVoIPCodecSettingsNodeExp );

    // CodecSettings dynamic
    MSmlDmDDFObject& codecSetDyn = codecNode.AddChildObjectGroupL();
    codecSetDyn.SetAccessTypesL( accessTypesAll );
    codecSetDyn.SetOccurenceL( MSmlDmDDFObject::EZeroOrMore );
    codecSetDyn.SetScopeL( MSmlDmDDFObject::EDynamic );
    codecSetDyn.SetDFFormatL( MSmlDmDDFObject::ENode );
    codecSetDyn.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    codecSetDyn.SetDescriptionL( KNSmlDMVoIPDynamicNodeExp );

    // CodecSettings/VoIPCodecId
    MSmlDmDDFObject& codecsetID = codecSetDyn.AddChildObjectL( 
        KNSmlDMVoIPCodecId );
    codecsetID.SetAccessTypesL( accessTypesGet );
    codecsetID.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    codecsetID.SetScopeL( MSmlDmDDFObject::EDynamic );
    codecsetID.SetDFFormatL( MSmlDmDDFObject::EInt );
    codecsetID.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    codecsetID.SetDescriptionL( KNSmlDMVoIPCodecSettingsIdExp );

    // CodecSettings/MediaTypeName
    MSmlDmDDFObject& mediaTypeName = codecSetDyn.AddChildObjectL( 
        KNSmlDMVoIPMediaTypeName );
    mediaTypeName.SetAccessTypesL( accessTypesGetReplaceAdd );
    mediaTypeName.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    mediaTypeName.SetScopeL( MSmlDmDDFObject::EDynamic );
    mediaTypeName.SetDFFormatL( MSmlDmDDFObject::EChr );
    mediaTypeName.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    mediaTypeName.SetDescriptionL( KNSmlDMVoIPMediaTypeNameExp );

    // CodecSettings/MediaSubTypeName
    MSmlDmDDFObject& mediaSubTypeName = codecSetDyn.AddChildObjectL( 
        KNSmlDMVoIPMediaSubTypeName );
    mediaSubTypeName.SetAccessTypesL( accessTypesGetReplaceAdd );
    mediaSubTypeName.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    mediaSubTypeName.SetScopeL( MSmlDmDDFObject::EDynamic );
    mediaSubTypeName.SetDFFormatL( MSmlDmDDFObject::EChr );
    mediaSubTypeName.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    mediaSubTypeName.SetDescriptionL( KNSmlDMVoIPMediaSubTypeNameExp );

     // CodecSettings/JitterBufferSize
    MSmlDmDDFObject& jitterBufferSize = codecSetDyn.AddChildObjectL( 
        KNSmlDMVoIPJitterBufferSize );
    jitterBufferSize.SetAccessTypesL( accessTypesGetReplaceAdd );
    jitterBufferSize.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    jitterBufferSize.SetScopeL( MSmlDmDDFObject::EDynamic );
    jitterBufferSize.SetDFFormatL( MSmlDmDDFObject::EInt );
    jitterBufferSize.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    jitterBufferSize.SetDescriptionL( KNSmlDMVoIPJitterBufferSizeExp );

     // CodecSettings/OctetAlign
    MSmlDmDDFObject& octetAlign = codecSetDyn.AddChildObjectL( 
        KNSmlDMVoIPOctetAlign );
    octetAlign.SetAccessTypesL( accessTypesGetReplaceAdd );
    octetAlign.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    octetAlign.SetScopeL( MSmlDmDDFObject::EDynamic );
    octetAlign.SetDFFormatL( MSmlDmDDFObject::EBool );
    octetAlign.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    octetAlign.SetDescriptionL( KNSmlDMVoIPOctetAlignExp );

    // CodecSettings/ModeSet node
    MSmlDmDDFObject& modeSetNode = codecSetDyn.AddChildObjectL( 
        KNSmlDMVoIPModeSetNode );
    modeSetNode.SetAccessTypesL( accessTypesGetAdd );
    modeSetNode.SetOccurenceL( MSmlDmDDFObject::EOne );
    modeSetNode.SetScopeL( MSmlDmDDFObject::EDynamic );
    modeSetNode.SetDFFormatL( MSmlDmDDFObject::ENode );
    modeSetNode.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    modeSetNode.SetDescriptionL( KNSmlDMVoIPModeSetNodeExp );

    // ModeSet dynamic
    MSmlDmDDFObject& modeSetDyn = modeSetNode.AddChildObjectGroupL();
    modeSetDyn.SetAccessTypesL( accessTypesAll );
    modeSetDyn.SetOccurenceL( MSmlDmDDFObject::EZeroOrMore );
    modeSetDyn.SetScopeL( MSmlDmDDFObject::EDynamic );
    modeSetDyn.SetDFFormatL( MSmlDmDDFObject::ENode );
    modeSetDyn.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    modeSetDyn.SetDescriptionL( KNSmlDMVoIPModeSetIdExp );

     // ModeSet/x/Mode
    MSmlDmDDFObject& modeSetId = modeSetDyn.AddChildObjectL( 
        KNSmlDMVoIPMode );
    modeSetId.SetAccessTypesL( accessTypesGetReplaceAdd );
    modeSetId.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    modeSetId.SetScopeL( MSmlDmDDFObject::EDynamic );
    modeSetId.SetDFFormatL( MSmlDmDDFObject::EInt );  // int
    modeSetId.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    modeSetId.SetDescriptionL( KNSmlDMVoIPModeChangePeriodExp );

     // CodecSettings/ModeChangePeriod
    MSmlDmDDFObject& modeChangePer = codecSetDyn.AddChildObjectL( 
        KNSmlDMVoIPModeChangePeriod );
    modeChangePer.SetAccessTypesL( accessTypesGetReplaceAdd );
    modeChangePer.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    modeChangePer.SetScopeL( MSmlDmDDFObject::EDynamic );
    modeChangePer.SetDFFormatL( MSmlDmDDFObject::EInt );
    modeChangePer.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    modeChangePer.SetDescriptionL( KNSmlDMVoIPModeChangePeriodExp );

     // CodecSettings/ModeChangeNeighbor
    MSmlDmDDFObject& modeChange = codecSetDyn.AddChildObjectL( 
        KNSmlDMVoIPModeChangeNeighbor );
    modeChange.SetAccessTypesL( accessTypesGetReplaceAdd );
    modeChange.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    modeChange.SetScopeL( MSmlDmDDFObject::EDynamic );
    modeChange.SetDFFormatL( MSmlDmDDFObject::EBool );
    modeChange.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    modeChange.SetDescriptionL( KNSmlDMVoIPModeChangeNeighborExp );

     // CodecSettings/Ptime
    MSmlDmDDFObject& ptime = codecSetDyn.AddChildObjectL( 
        KNSmlDMVoIPPtime );
    ptime.SetAccessTypesL( accessTypesGetReplaceAdd );
    ptime.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    ptime.SetScopeL( MSmlDmDDFObject::EDynamic );
    ptime.SetDFFormatL( MSmlDmDDFObject::EInt );  // int
    ptime.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    ptime.SetDescriptionL( KNSmlDMVoIPPtimeExp );

     // CodecSettings/MaxPtime
    MSmlDmDDFObject& maxptime = codecSetDyn.AddChildObjectL( 
        KNSmlDMVoIPMaxPtime );
    maxptime.SetAccessTypesL( accessTypesGetReplaceAdd );
    maxptime.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    maxptime.SetScopeL( MSmlDmDDFObject::EDynamic );
    maxptime.SetDFFormatL( MSmlDmDDFObject::EInt );
    maxptime.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    maxptime.SetDescriptionL( KNSmlDMVoIPMaxPtimeExp );

     // CodecSettings/VAD
    MSmlDmDDFObject& vad = codecSetDyn.AddChildObjectL( 
        KNSmlDMVoIPVAD );
    vad.SetAccessTypesL( accessTypesGetReplaceAdd );
    vad.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    vad.SetScopeL( MSmlDmDDFObject::EDynamic );
    vad.SetDFFormatL( MSmlDmDDFObject::EBool ); // bool
    vad.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    vad.SetDescriptionL( KNSmlDMVoIPVADExp );

    // CodecSettings/AnnexB
    MSmlDmDDFObject& annexB = codecSetDyn.AddChildObjectL( 
        KNSmlDMVoIPAnnexB );
    annexB.SetAccessTypesL( accessTypesGetReplaceAdd );
    annexB.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    annexB.SetScopeL( MSmlDmDDFObject::EDynamic );
    annexB.SetDFFormatL( MSmlDmDDFObject::EBool );  // bool
    annexB.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    annexB.SetDescriptionL( KNSmlDMVoIPAnnexBExp ); 

    // CodecSettings/MaxRed
    MSmlDmDDFObject& maxRed = codecSetDyn.AddChildObjectL(
        KNSmlDMVoIPMaxRed );
    maxRed.SetAccessTypesL( accessTypesGetReplaceAdd );
    maxRed.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    maxRed.SetScopeL( MSmlDmDDFObject::EDynamic );
    maxRed.SetDFFormatL( MSmlDmDDFObject::EInt );
    maxRed.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    maxRed.SetDescriptionL( KNSmlDMVoIPMaxRedExp );

    // SettingsIds node
    MSmlDmDDFObject& settingIds = idNode.AddChildObjectL( 
        KNSmlDMVoIPSettingIdsNode );
    settingIds.SetAccessTypesL( accessTypesGetAdd );
    settingIds.SetOccurenceL( MSmlDmDDFObject::EOne );
    settingIds.SetScopeL( MSmlDmDDFObject::EDynamic );
    settingIds.SetDFFormatL( MSmlDmDDFObject::ENode );
    settingIds.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    settingIds.SetDescriptionL( KNSmlDMVoIPDynamicNode );

    // settingsIds dynamic
    MSmlDmDDFObject& settingIdsDynID = settingIds.AddChildObjectGroupL();
    settingIdsDynID.SetAccessTypesL( accessTypesAll );
    settingIdsDynID.SetOccurenceL( MSmlDmDDFObject::EZeroOrMore );
    settingIdsDynID.SetScopeL( MSmlDmDDFObject::EDynamic );
    settingIdsDynID.SetDFFormatL( MSmlDmDDFObject::ENode );
    settingIdsDynID.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    settingIdsDynID.SetDescriptionL( KNSmlDMVoIPDynamicNodeExp );

    // SettingIds/ProfileType
    MSmlDmDDFObject& profileType = settingIdsDynID.AddChildObjectL( 
        KNSmlDMVoIPProfileType );
    profileType.SetAccessTypesL( accessTypesGetReplaceAdd );
    profileType.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    profileType.SetScopeL( MSmlDmDDFObject::EDynamic );
    profileType.SetDFFormatL( MSmlDmDDFObject::EChr );
    profileType.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    profileType.SetDescriptionL( KNSmlDMVoIPProfileTypeExp ); 

    // SettingIds/ProfileId
    MSmlDmDDFObject& profileId = settingIdsDynID.AddChildObjectL( 
        KNSmlDMVoIPProfileId );
    profileId.SetAccessTypesL( accessTypesGetReplaceAdd );
    profileId.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    profileId.SetScopeL( MSmlDmDDFObject::EDynamic );
    profileId.SetDFFormatL( MSmlDmDDFObject::EChr );
    profileId.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    profileId.SetDescriptionL( KNSmlDMVoIPProfileIdExp );

    // SettingIds/ProfileSpecificId
    MSmlDmDDFObject& profileSpecific = settingIdsDynID.AddChildObjectL( 
        KNSmlDMVoIPProfileSpecificId );
    profileSpecific.SetAccessTypesL( accessTypesGetReplaceAdd );
    profileSpecific.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    profileSpecific.SetScopeL( MSmlDmDDFObject::EDynamic );
    profileSpecific.SetDFFormatL( MSmlDmDDFObject::EInt );
    profileSpecific.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    profileSpecific.SetDescriptionL( KNSmlDMVoIPProfileSpecificExp );

    // VoIP/<x>/VoiceMailBox
    MSmlDmDDFObject& vmbx = idNode.AddChildObjectL( 
        KNSmlDMVoIPVmbxNode );
    vmbx.SetAccessTypesL( accessTypesGetAdd );
    vmbx.SetOccurenceL( MSmlDmDDFObject::EOne );
    vmbx.SetScopeL( MSmlDmDDFObject::EDynamic );
    vmbx.SetDFFormatL( MSmlDmDDFObject::ENode );
    vmbx.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    vmbx.SetDescriptionL( KNSmlDMVoIPVmbxNodeExp );

    // VoIP/<x>/VoiceMailBox/MWI-URI
    MSmlDmDDFObject& mwiUri = vmbx.AddChildObjectL( 
        KNSmlDMVoIPMwiUri );
    mwiUri.SetAccessTypesL( accessTypesGetReplaceAdd );
    mwiUri.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    mwiUri.SetScopeL( MSmlDmDDFObject::EDynamic );
    mwiUri.SetDFFormatL( MSmlDmDDFObject::EChr );
    mwiUri.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    mwiUri.SetDescriptionL( KNSmlDMVoIPMwiUriExp );

    // VoIP/<x>/VoiceMailBox/ListeningURI
    MSmlDmDDFObject& listenUri = vmbx.AddChildObjectL( 
        KNSmlDMVoIPListeningUri );
    listenUri.SetAccessTypesL( accessTypesGetReplaceAdd );
    listenUri.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    listenUri.SetScopeL( MSmlDmDDFObject::EDynamic );
    listenUri.SetDFFormatL( MSmlDmDDFObject::EChr );
    listenUri.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    listenUri.SetDescriptionL( KNSmlDMVoIPListeningUriExp );

    // VoIP/<x>/VoiceMailBox/UsedIAPId
    MSmlDmDDFObject& usedIapId = vmbx.AddChildObjectL( 
        KNSmlDMVoIPUsedIAPId );
    usedIapId.SetAccessTypesL( accessTypesGetReplaceAdd );
    usedIapId.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    usedIapId.SetScopeL( MSmlDmDDFObject::EDynamic );
    usedIapId.SetDFFormatL( MSmlDmDDFObject::EChr );
    usedIapId.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    usedIapId.SetDescriptionL( KNSmlDMVoIPUsedIAPIdExp );

    // VoIP/<x>/VoiceMailBox/ReSubscribeInterval
    MSmlDmDDFObject& reSubscribe = vmbx.AddChildObjectL( 
        KNSmlDMVoIPReSubscribeInterval );
    reSubscribe.SetAccessTypesL( accessTypesGetReplaceAdd );
    reSubscribe.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    reSubscribe.SetScopeL( MSmlDmDDFObject::EDynamic );
    reSubscribe.SetDFFormatL( MSmlDmDDFObject::EInt );
    reSubscribe.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    reSubscribe.SetDescriptionL( KNSmlDMVoIPReSubscribeIntervalExp );

    // VoIP/<x>/VoiceMailBox/UsedSIPProfileId
    MSmlDmDDFObject& sipId = vmbx.AddChildObjectL( 
        KNSmlDMVoIPUsedSIPProfileId );
    sipId.SetAccessTypesL( accessTypesGetReplaceAdd );
    sipId.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    sipId.SetScopeL( MSmlDmDDFObject::EDynamic );
    sipId.SetDFFormatL( MSmlDmDDFObject::EChr );
    sipId.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    sipId.SetDescriptionL( KNSmlDMVoIPUsedSIPProfileIdExp );

    // StartMediaPort
    MSmlDmDDFObject& smport = idNode.AddChildObjectL( 
        KNSmlDMVoIPSMPort );
    smport.SetAccessTypesL( accessTypesGetReplaceAdd );
    smport.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    smport.SetScopeL( MSmlDmDDFObject::EDynamic );
    smport.SetDFFormatL( MSmlDmDDFObject::EInt );
    smport.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    smport.SetDescriptionL( KNSmlDMVoIPSMPortExp );

    // EndMediaPort
    MSmlDmDDFObject& emport = idNode.AddChildObjectL( 
        KNSmlDMVoIPEMPort );
    emport.SetAccessTypesL( accessTypesGetReplaceAdd );
    emport.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    emport.SetScopeL( MSmlDmDDFObject::EDynamic );
    emport.SetDFFormatL( MSmlDmDDFObject::EInt );
    emport.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    emport.SetDescriptionL( KNSmlDMVoIPEMPortExp );

    // MediaQos
    MSmlDmDDFObject& mediaQos = idNode.AddChildObjectL( 
        KNSmlDMVoIPMediaQos );
    mediaQos.SetAccessTypesL( accessTypesGetReplaceAdd );
    mediaQos.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    mediaQos.SetScopeL( MSmlDmDDFObject::EDynamic );
    mediaQos.SetDFFormatL( MSmlDmDDFObject::EInt );
    mediaQos.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    mediaQos.SetDescriptionL( KNSmlDMVoIPMediaQosExp );

    // DTMFIB
    MSmlDmDDFObject& dtmfbib = idNode.AddChildObjectL( 
        KNSmlDMVoIPDTMFIB );
    dtmfbib.SetAccessTypesL( accessTypesGetReplaceAdd );
    dtmfbib.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    dtmfbib.SetScopeL( MSmlDmDDFObject::EDynamic );
    dtmfbib.SetDFFormatL( MSmlDmDDFObject::EBool );
    dtmfbib.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    dtmfbib.SetDescriptionL( KNSmlDMVoIPDTMFIBExp );

    // DTMFOB
    MSmlDmDDFObject& dtmfob = idNode.AddChildObjectL( 
        KNSmlDMVoIPDTMFOB );
    dtmfob.SetAccessTypesL( accessTypesGetReplaceAdd );
    dtmfob.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    dtmfob.SetScopeL( MSmlDmDDFObject::EDynamic );
    dtmfob.SetDFFormatL( MSmlDmDDFObject::EBool );
    dtmfob.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    dtmfob.SetDescriptionL( KNSmlDMVoIPDTMFOBExp );

    // SecureCallPref
    MSmlDmDDFObject& secureCallPref = idNode.AddChildObjectL(
        KNSmlDMVoIPSecureCallPref );
    secureCallPref.SetAccessTypesL( accessTypesGetReplaceAdd );
    secureCallPref.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    secureCallPref.SetScopeL( MSmlDmDDFObject::EDynamic );
    secureCallPref.SetDFFormatL( MSmlDmDDFObject::EInt );
    secureCallPref.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    secureCallPref.SetDescriptionL( KNSmlDMVoIPSecureCallPrefExp );

    // RTCP
    MSmlDmDDFObject& rtcp = idNode.AddChildObjectL(
        KNSmlDMVoIPRTCP );
    rtcp.SetAccessTypesL( accessTypesGetReplaceAdd );
    rtcp.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    rtcp.SetScopeL( MSmlDmDDFObject::EDynamic );
    rtcp.SetDFFormatL( MSmlDmDDFObject::EInt );
    rtcp.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    rtcp.SetDescriptionL( KNSmlDMVoIPRTCPExp );

    // UAHTerminalType
    MSmlDmDDFObject& uahTerminalType = idNode.AddChildObjectL(
        KNSmlDMVoIPUAHTerminalType );
    uahTerminalType.SetAccessTypesL( accessTypesGetReplaceAdd );
    uahTerminalType.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    uahTerminalType.SetScopeL( MSmlDmDDFObject::EDynamic );
    uahTerminalType.SetDFFormatL( MSmlDmDDFObject::EInt );
    uahTerminalType.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    uahTerminalType.SetDescriptionL( KNSmlDMVoIPUAHTerminalTypeExp );

    // UAHWLANMAC
    MSmlDmDDFObject& uahWLANMAC = idNode.AddChildObjectL(
        KNSmlDMVoIPUAHWLANMAC );
    uahWLANMAC.SetAccessTypesL( accessTypesGetReplaceAdd );
    uahWLANMAC.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    uahWLANMAC.SetScopeL( MSmlDmDDFObject::EDynamic );
    uahWLANMAC.SetDFFormatL( MSmlDmDDFObject::EInt );
    uahWLANMAC.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    uahWLANMAC.SetDescriptionL( KNSmlDMVoIPUAHWLANMACExp );

    // UAHString
    MSmlDmDDFObject& uahString = idNode.AddChildObjectL(
        KNSmlDMVoIPUAHString );
    uahString.SetAccessTypesL( accessTypesGetReplaceAdd );
    uahString.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    uahString.SetScopeL( MSmlDmDDFObject::EDynamic );
    uahString.SetDFFormatL( MSmlDmDDFObject::EChr );
    uahString.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    uahString.SetDescriptionL( KNSmlDMVoIPUAHStringExp );

    // ProfileLockedToIAP
    MSmlDmDDFObject& profileLockedToIAP = idNode.AddChildObjectL(
        KNSmlDMVoIPProfileLockedToIAP );
    profileLockedToIAP.SetAccessTypesL( accessTypesGetReplaceAdd );
    profileLockedToIAP.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    profileLockedToIAP.SetScopeL( MSmlDmDDFObject::EDynamic );
    profileLockedToIAP.SetDFFormatL( MSmlDmDDFObject::EBool );
    profileLockedToIAP.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    profileLockedToIAP.SetDescriptionL( KNSmlDMVoIPProfileLockedToIAPExp );

    // VoIPPluginUID
    MSmlDmDDFObject& voipPluginUID = idNode.AddChildObjectL(
        KNSmlDMVoIPPluginUID );
    voipPluginUID.SetAccessTypesL( accessTypesGetReplaceAdd );
    voipPluginUID.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    voipPluginUID.SetScopeL( MSmlDmDDFObject::EDynamic );
    voipPluginUID.SetDFFormatL( MSmlDmDDFObject::EInt );
    voipPluginUID.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    voipPluginUID.SetDescriptionL( KNSmlDMVoIPPluginUIDExp );

    // AllowVoIPOverWCDMA
    MSmlDmDDFObject& voipOverWCDMA = idNode.AddChildObjectL(
        KNSmlDMVoIPAllowVoIPOverWCDMA );
    voipOverWCDMA.SetAccessTypesL( accessTypesGetReplaceAdd );
    voipOverWCDMA.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    voipOverWCDMA.SetScopeL( MSmlDmDDFObject::EDynamic );
    voipOverWCDMA.SetDFFormatL( MSmlDmDDFObject::EBool );
    voipOverWCDMA.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    voipOverWCDMA.SetDescriptionL( KNSmlDMVoIPAllowVoIPOverWCDMAExp );

    // VoIPDigits
    MSmlDmDDFObject& voipDigits = idNode.AddChildObjectL(
        KNSmlDMVoIPVoIPDigits );
    voipDigits.SetAccessTypesL( accessTypesGetReplaceAdd );
    voipDigits.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    voipDigits.SetScopeL( MSmlDmDDFObject::EDynamic );
    voipDigits.SetDFFormatL( MSmlDmDDFObject::EInt );
    voipDigits.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    voipDigits.SetDescriptionL( KNSmlDMVoIPVoIPDigitsExp );

    // URIDomainIgnoreRule
    MSmlDmDDFObject& uriDomainIgn = idNode.AddChildObjectL(
        KNSmlDMVoIPURIDomainIgnoreRule );
    uriDomainIgn.SetAccessTypesL( accessTypesGetReplaceAdd );
    uriDomainIgn.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    uriDomainIgn.SetScopeL( MSmlDmDDFObject::EDynamic );
    uriDomainIgn.SetDFFormatL( MSmlDmDDFObject::EInt );
    uriDomainIgn.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    uriDomainIgn.SetDescriptionL( KNSmlDMVoIPURIDomainIgnoreRuleExp );

    // AutoAcceptBuddyRequest
    MSmlDmDDFObject& aaBuddyReq = idNode.AddChildObjectL(
        KNSmlDMVoIPAutoAcceptBuddyRequest );
    aaBuddyReq.SetAccessTypesL( accessTypesGetReplaceAdd );
    aaBuddyReq.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    aaBuddyReq.SetScopeL( MSmlDmDDFObject::EDynamic );
    aaBuddyReq.SetDFFormatL( MSmlDmDDFObject::EBool );
    aaBuddyReq.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    aaBuddyReq.SetDescriptionL( KNSmlDMVoIPAutoAcceptBuddyRequestExp );

    // UsedVoIPRelease
    MSmlDmDDFObject& voipRel = idNode.AddChildObjectL(
        KNSmlDMVoIPUsedVoIPRelease );
    voipRel.SetAccessTypesL( accessTypesGet );
    voipRel.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    voipRel.SetScopeL( MSmlDmDDFObject::EDynamic );
    voipRel.SetDFFormatL( MSmlDmDDFObject::EChr );
    voipRel.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    voipRel.SetDescriptionL( KNSmlDMVoIPUsedVoIPReleaseExp );

    // AddUserPhone
    MSmlDmDDFObject& addUserPhone = idNode.AddChildObjectL(
        KNSmlDMVoIPAddUserPhone );
    addUserPhone.SetAccessTypesL( accessTypesGetReplaceAdd );
    addUserPhone.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    addUserPhone.SetScopeL( MSmlDmDDFObject::EDynamic );
    addUserPhone.SetDFFormatL( MSmlDmDDFObject::EInt );
    addUserPhone.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    addUserPhone.SetDescriptionL( KNSmlDMVoIPAddUserPhoneExp );

    // ProviderBookmarkURI
    MSmlDmDDFObject& providerBkmarkUri = idNode.AddChildObjectL(
        KNSmlDMVoIPProviderBookmarkUri );
    providerBkmarkUri.SetAccessTypesL( accessTypesGetReplaceAdd );
    providerBkmarkUri.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    providerBkmarkUri.SetScopeL( MSmlDmDDFObject::EDynamic );
    providerBkmarkUri.SetDFFormatL( MSmlDmDDFObject::EChr );
    providerBkmarkUri.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    providerBkmarkUri.SetDescriptionL( KNSmlDMVoIPProviderBookmarkUriExp );

    // SIPConnectivityTestURI
    MSmlDmDDFObject& sipConnTestUri = idNode.AddChildObjectL( 
        KNSmlDMVoIPSIPConnTestUri );
    sipConnTestUri.SetAccessTypesL( accessTypesGetReplaceAdd );
    sipConnTestUri.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    sipConnTestUri.SetScopeL( MSmlDmDDFObject::EDynamic );
    sipConnTestUri.SetDFFormatL( MSmlDmDDFObject::EChr );
    sipConnTestUri.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    sipConnTestUri.SetDescriptionL( KNSmlDMVoIPSIPConnTestUriExp );

    // NATFWProfileId
    MSmlDmDDFObject& natfwId = idNode.AddChildObjectL(
        KNSmlDMVoIPNatFwProfileId );
    natfwId.SetAccessTypesL( accessTypesGetReplaceAdd );
    natfwId.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    natfwId.SetScopeL( MSmlDmDDFObject::EDynamic );
    natfwId.SetDFFormatL( MSmlDmDDFObject::EChr );
    natfwId.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    natfwId.SetDescriptionL( KNSmlDMVoIPNatFwProfileIdExp );

    // MinSessionInterval
    MSmlDmDDFObject& minSe = idNode.AddChildObjectL(
        KNSmlDMVoIPMinSessionInterval );
    minSe.SetAccessTypesL( accessTypesGetReplaceAdd );
    minSe.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    minSe.SetScopeL( MSmlDmDDFObject::EDynamic );
    minSe.SetDFFormatL( MSmlDmDDFObject::EInt );
    minSe.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    minSe.SetDescriptionL( KNSmlDMVoIPMinSessionIntervalExp );

    // SessionExpires
    MSmlDmDDFObject& sessionExpires = idNode.AddChildObjectL(
        KNSmlDMVoIPSessionExpires );
    sessionExpires.SetAccessTypesL( accessTypesGetReplaceAdd );
    sessionExpires.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    sessionExpires.SetScopeL( MSmlDmDDFObject::EDynamic );
    sessionExpires.SetDFFormatL( MSmlDmDDFObject::EInt );
    sessionExpires.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    sessionExpires.SetDescriptionL( KNSmlDMVoIPSessionExpiresExp );

    // BradingDataURI
    MSmlDmDDFObject& brandingUri = idNode.AddChildObjectL(
        KNSmlDMVoIPBrandingDataUri );
    brandingUri.SetAccessTypesL( accessTypesGetReplaceAdd );
    brandingUri.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    brandingUri.SetScopeL( MSmlDmDDFObject::EDynamic );
    brandingUri.SetDFFormatL( MSmlDmDDFObject::EInt );
    brandingUri.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    brandingUri.SetDescriptionL( KNSmlDMVoIPBrandingDataUriExp );

    // PresenceSettingsID
    MSmlDmDDFObject& presenceId = idNode.AddChildObjectL(
        KNSmlDMVoIPPresenceSettingsId );
    presenceId.SetAccessTypesL( accessTypesGetReplaceAdd );
    presenceId.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    presenceId.SetScopeL( MSmlDmDDFObject::EDynamic );
    presenceId.SetDFFormatL( MSmlDmDDFObject::EInt );
    presenceId.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    presenceId.SetDescriptionL( KNSmlDMVoIPPresenceSettingsIdExp );

    // UsedNatProtocol
    MSmlDmDDFObject& natProtocol = idNode.AddChildObjectL(
        KNSmlDMVoIPUsedNatProtocol );
    natProtocol.SetAccessTypesL( accessTypesGetReplaceAdd );
    natProtocol.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    natProtocol.SetScopeL( MSmlDmDDFObject::EDynamic );
    natProtocol.SetDFFormatL( MSmlDmDDFObject::EInt );
    natProtocol.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    natProtocol.SetDescriptionL( KNSmlDMVoIPUsedNatProtocolExp );

    // AutoEnableService
    MSmlDmDDFObject& autoEnable = idNode.AddChildObjectL(
        KNSmlDMVoIPAutoEnable );
    autoEnable.SetAccessTypesL( accessTypesGetReplaceAdd );
    autoEnable.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    autoEnable.SetScopeL( MSmlDmDDFObject::EDynamic );
    autoEnable.SetDFFormatL( MSmlDmDDFObject::EBool );
    autoEnable.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    autoEnable.SetDescriptionL( KNSmlDMVoIPAutoEnableExp );
    
    // SNAPProfileId
    MSmlDmDDFObject& snapId = idNode.AddChildObjectL(
        KNSmlDMVoIPSnapProfileId );
    snapId.SetAccessTypesL( accessTypesGetReplaceAdd );
    snapId.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    snapId.SetScopeL( MSmlDmDDFObject::EDynamic );
    snapId.SetDFFormatL( MSmlDmDDFObject::EChr );
    snapId.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    snapId.SetDescriptionL( KNSmlDMVoIPSnapProfileIdExp );

    // EnableIM
    MSmlDmDDFObject& enableIm = idNode.AddChildObjectL(
        KNSmlDMVoIPEnableIm );
    enableIm.SetAccessTypesL( accessTypesGetReplaceAdd );
    enableIm.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    enableIm.SetScopeL( MSmlDmDDFObject::EDynamic );
    enableIm.SetDFFormatL( MSmlDmDDFObject::EBool );
    enableIm.AddDFTypeMimeTypeL( KNSmlDMVoIPTextPlain );
    enableIm.SetDescriptionL( KNSmlDMVoIPEnableImExp );
    
    DBG_PRINT("CNSmlDmVoIPAdapter::DDFStructureL(): end");
    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::UpdateLeafObjectL
// Updates value of leaf object
// ---------------------------------------------------------------------------
//
void CNSmlDmVoIPAdapter::UpdateLeafObjectL( 
    const TDesC8& aURI, 
    const TDesC8& aLUID, 
    const TDesC8& aObject, 
    const TDesC8& /*aType*/, 
    const TInt aStatusRef )
    {
    DBG_PRINT("CNSmlDmVoIPAdapter::UpdateLeafObjectL(): begin");

    __ASSERT_ALWAYS( iDmCallback != NULL, User::Leave( KErrArgument ) );

    // Load VoIP profile information if not loaded yet
    if ( !iProfileEntries.Count() )
        {
        LoadProfilesL();
        }

    CSmlDmAdapter::TError retValue = CSmlDmAdapter::EOk;
    TPtrC8 uriSeg = LastURISeg( aURI );
    TPtrC8 uriTmp = RemoveLastURISeg( aURI );
    TPtrC8 secondLastUriSeg = LastURISeg( uriTmp );

    // Check that luid mapping exist 
    if ( !aLUID.Length() )
        {
        iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
        DBG_PRINT("CNSmlDmVoIPAdapter::UpdateLeafObjectL(): no LUID end");
        return;
        }

     // Find profile and get settings
    iProfileID = FindProfileLocation( aLUID );

    // If profile is not found return error.
    if ( KErrNotFound == iProfileID )
        {
        retValue = CSmlDmAdapter::ENotFound;
        iDmCallback->SetStatusL( aStatusRef, retValue );
        DBG_PRINT("CNSmlDmVoIPAdapter::UpdateLeafObjectL(): not found end");
        return;
        }

    CDMVoIPSPSettings* spSettings = CDMVoIPSPSettings::NewLC(); // CS:1
    spSettings->iRcseProfileId = iProfileEntries[iProfileID]->iId;
    TInt spSettingsId( KErrNotFound );
    // Check if service provider settings for this VoIP profile are already
    // updated to be stored.
    for ( TInt counter = 0; counter < iSPSettings.Count(); counter++ )
        {
        if ( iProfileEntries[iProfileID]->iId == 
            iSPSettings[counter]->iRcseProfileId )
            {
            spSettingsId = counter;
            }
        }
    TBool addSpSettings( EFalse );

    // Check that profile is not already to be saved.
    if ( KErrNotFound == iProfileModifs.Find( iProfileID ) )
        {
        iProfileModifs.Insert( iProfileID, iProfileModifs.Count() );
        }

    // VoIP/x/ProviderName
    if ( KNSmlDMVoIPProvID() == uriSeg )
        {
        CRCSEProfileEntry* tmpEntryForProvider = CRCSEProfileEntry::NewLC();
        tmpEntryForProvider->iProviderName.Copy( aObject );
        CheckDuplicateProviderL( *tmpEntryForProvider );
        iProfileEntries[iProfileID]->iProviderName.Copy( 
            tmpEntryForProvider->iProviderName );
        CleanupStack::PopAndDestroy( tmpEntryForProvider );
        }
    // VoIP/x/SettingsName
    else if ( KNSmlDMVoIPName() == uriSeg )
        {
        CRCSEProfileEntry* tmpEntryForName = CRCSEProfileEntry::NewLC();
        tmpEntryForName->iSettingsName.Copy( aObject );
        CheckDuplicateNameL( *tmpEntryForName );
        iProfileEntries[iProfileID]->iSettingsName.Copy( 
            tmpEntryForName->iSettingsName );
        CleanupStack::PopAndDestroy( tmpEntryForName ); 
        }
    // VoIP/x/SMPort
    else if ( KNSmlDMVoIPSMPort() == uriSeg )
        {
        iProfileEntries[iProfileID]->iStartMediaPort = 
            DesToInt( aObject );
        }
    // VoIP/x/EMPort
    else if ( KNSmlDMVoIPEMPort() == uriSeg )
        {
        iProfileEntries[iProfileID]->iEndMediaPort = DesToInt( aObject );
        }
    // VoIP/x/MediaQos
    else if ( KNSmlDMVoIPMediaQos() == uriSeg )
        {
        iProfileEntries[iProfileID]->iMediaQOS = DesToInt( aObject );
        }
    // VoIP/x/DTMFIB
    else if ( KNSmlDMVoIPDTMFIB() == uriSeg )
        {
        if ( KNSmlDMVoIPValueTrue() == aObject )
            {
            iProfileEntries[iProfileID]->iInbandDTMF = CRCSEProfileEntry::EOn;
            }
        else if ( KNSmlDMVoIPValueFalse() == aObject )
            {
            iProfileEntries[iProfileID]->iInbandDTMF = 
                CRCSEProfileEntry::EOff;
            }
        else
            {
            retValue = CSmlDmAdapter::EInvalidObject;   
            }
        }
    // VoIP/x/DTMFOB
    else if ( KNSmlDMVoIPDTMFOB() == uriSeg )
        {
        if ( KNSmlDMVoIPValueTrue() == aObject )
            {
            iProfileEntries[iProfileID]->iOutbandDTMF = 
                CRCSEProfileEntry::EOn;
            }
        else if ( KNSmlDMVoIPValueFalse() == aObject )
            {
            iProfileEntries[iProfileID]->iOutbandDTMF = 
                CRCSEProfileEntry::EOff;
            }
        else
            {
            retValue = CSmlDmAdapter::EInvalidObject;   
            }
        }
    // VoIP/x/SecureCallPref
    else if ( KNSmlDMVoIPSecureCallPref() == uriSeg )
        {
        iProfileEntries[iProfileID]->
            iSecureCallPreference = DesToInt( aObject );
        }
    // VoIP/x/RTCP
    else if ( KNSmlDMVoIPRTCP() == uriSeg )
        {
        iProfileEntries[iProfileID]->iRTCP = DesToInt( aObject );
        }
    // VoIP/x/UAHTerminalType
    else if ( KNSmlDMVoIPUAHTerminalType() == uriSeg )
        {
        iProfileEntries[iProfileID]->
            iSIPVoIPUAHTerminalType = DesToInt( aObject );
        }
    // VoIP/x/UAHWLANMAC
    else if ( KNSmlDMVoIPUAHWLANMAC() == uriSeg )
        {
        iProfileEntries[iProfileID]->
            iSIPVoIPUAHeaderWLANMAC = DesToInt( aObject );
        }
    // VoIP/x/UAHString
    else if ( KNSmlDMVoIPUAHString() == uriSeg )
        {
        iProfileEntries[iProfileID]->
            iSIPVoIPUAHeaderString.Copy( aObject );
        }
    // VoIP/x/ProfileLockedToIAP
    else if ( KNSmlDMVoIPProfileLockedToIAP() == uriSeg )
        {
        if ( KNSmlDMVoIPValueTrue() == aObject )
            {
            iProfileEntries[iProfileID]->
                iProfileLockedToIAP = CRCSEProfileEntry::EOn;
            }
        else if ( KNSmlDMVoIPValueFalse() == aObject )
            {
            iProfileEntries[iProfileID]->
                iProfileLockedToIAP = CRCSEProfileEntry::EOff;
            }
        else
            {
            retValue = CSmlDmAdapter::EInvalidObject;   
            }
        }
    // VoIP/x/VoIPPluginUID
    else if ( KNSmlDMVoIPPluginUID() == uriSeg )
        {
        iProfileEntries[iProfileID]->iVoIPPluginUID = 
            DesToTUint( aObject );
        }
    // VoIP/x/AllowVoIPOverWCDMA
    else if ( KNSmlDMVoIPAllowVoIPOverWCDMA() == uriSeg )
        {
        if ( KNSmlDMVoIPValueTrue() == aObject )
            {
            iProfileEntries[iProfileID]->
                iAllowVoIPoverWCDMA = CRCSEProfileEntry::EOn;
            }
        else if ( KNSmlDMVoIPValueFalse() == aObject )
            {
            iProfileEntries[iProfileID]->
                iAllowVoIPoverWCDMA = CRCSEProfileEntry::EOff;
            }
        else
            {
            retValue = CSmlDmAdapter::EInvalidObject;   
            }
        }
    // VoIP/x/VoIPDigits
    else if ( KNSmlDMVoIPVoIPDigits() == uriSeg )
        {
        iProfileEntries[iProfileID]->iMeanCountOfVoIPDigits = 
            DesToInt( aObject );
        }
    // VoIP/x/URIDomainIgnoreRule
    else if ( KNSmlDMVoIPURIDomainIgnoreRule() == uriSeg )
        {
        iProfileEntries[iProfileID]->iIgnoreAddrDomainPart = 
            DesToInt( aObject );
        }
    // VoIP/x/AddUserPhone
    else if ( KNSmlDMVoIPAddUserPhone() == uriSeg )
        {
        if ( KNSmlDMVoIPValueFalse() == aObject )
            {
            iProfileEntries[iProfileID]->iUserPhoneUriParameter =
            CRCSEProfileEntry::EOff;
            }
        else if ( KNSmlDMVoIPValueTrue() == aObject )
            {
            iProfileEntries[iProfileID]->iUserPhoneUriParameter =
            CRCSEProfileEntry::EOn;
            }
        else
            {
            retValue = CSmlDmAdapter::EInvalidObject;
            }
        }
    // VoIP/x/ProviderBookmarkURI
    else if ( KNSmlDMVoIPProviderBookmarkUri() == uriSeg )
        {
        if ( KErrNotFound == spSettingsId )
            {
            delete spSettings->iServiceProviderBookmark;
            spSettings->iServiceProviderBookmark = NULL;
            spSettings->iServiceProviderBookmark = HBufC::NewL( 
                aObject.Length() );
            spSettings->iServiceProviderBookmark->Des().Copy( aObject );
            addSpSettings = ETrue;
            }
        else
            {
            delete iSPSettings[spSettingsId]->iServiceProviderBookmark;
            iSPSettings[spSettingsId]->iServiceProviderBookmark = NULL;
            iSPSettings[spSettingsId]->iServiceProviderBookmark = HBufC::NewL(
                aObject.Length() );
            iSPSettings[spSettingsId]->iServiceProviderBookmark->Des().Copy( 
                aObject );
            }
        }
    // VoIP/x/SIPConnectivityTestURI
    else if ( KNSmlDMVoIPSIPConnTestUri() == uriSeg )
        {
        iProfileEntries[iProfileID]->iSIPConnTestAddress.Copy( 
            aObject );
        }
    // VoIP/x/NATFWProfileId
    else if ( KNSmlDMVoIPNatFwProfileId() == uriSeg )
        {
        TUint32 natId = NatFwIdL( aObject );
        if ( KErrNone == natId )
            {
            retValue = CSmlDmAdapter::EInvalidObject;
            }
        else
            {
            iProfileEntries[iProfileID]->iNATSettingsStorageId = natId;
            }
        }
    // VoIP/x/MinSessionInterval
    else if ( KNSmlDMVoIPMinSessionInterval() == uriSeg )
        {
        iProfileEntries[iProfileID]->iSIPMinSE = DesToInt( aObject );
        }
    // VoIP/x/SessionExpires
    else if ( KNSmlDMVoIPSessionExpires() == uriSeg )
        {
        iProfileEntries[iProfileID]->iSIPSessionExpires = 
            DesToInt( aObject );
        }
    // VoIP/x/BradingDataURI
    else if ( KNSmlDMVoIPBrandingDataUri() == uriSeg )
        {
        if ( KErrNotFound == spSettingsId )
            {
            delete spSettings->iBrandingDataUri;
            spSettings->iBrandingDataUri = NULL;
            spSettings->iBrandingDataUri = HBufC::NewL( aObject.Length() );
            spSettings->iBrandingDataUri->Des().Copy( aObject );
            addSpSettings = ETrue;
            }
        else
            {
            delete iSPSettings[spSettingsId]->iBrandingDataUri;
            iSPSettings[spSettingsId]->iBrandingDataUri = NULL;
            iSPSettings[spSettingsId]->iBrandingDataUri = 
                HBufC::NewL( aObject.Length() );
            iSPSettings[spSettingsId]->iBrandingDataUri->Des().Copy( 
                aObject );
            }
        }
    // VoIP/x/PresenceSettingsID
    else if ( KNSmlDMVoIPPresenceSettingsId() == uriSeg )
        {
        if ( KErrNotFound == spSettingsId )
            {
            spSettings->iPresenceId = PresenceIdL( aObject );
            addSpSettings = ETrue;
            }
        else
            {
            iSPSettings[spSettingsId]->iPresenceId = PresenceIdL( aObject );
            }
        }
    // VoIP/x/UsedNATProtocol
    else if ( KNSmlDMVoIPUsedNatProtocol() == uriSeg )
        {
        iProfileEntries[iProfileID]->iNATProtocol = DesToInt( aObject );
        }
    // VoIP/x/AutoEnable
    else if ( KNSmlDMVoIPAutoEnable() == uriSeg )
        {
        if ( KErrNotFound == spSettingsId )
            {
            spSettings->iAutoEnableService = DesToInt( aObject );
            addSpSettings = ETrue;
            }
        else
            {
            iSPSettings[spSettingsId]->iAutoEnableService =  
                DesToInt( aObject );
            }
        }
    // VoIP/x/SNAPProfileId
    else if ( KNSmlDMVoIPSnapProfileId() == uriSeg )
        {
        if ( KErrNotFound == spSettingsId )
            {
            spSettings->iSnapId = SnapIdL( aObject );
            addSpSettings = ETrue;
            }
        else
            {
            iSPSettings[spSettingsId]->iSnapId = SnapIdL( aObject );
            }
        }
    // VoIP/x/EnableIM
    else if ( KNSmlDMVoIPEnableIm() == uriSeg )
        {
        if ( KErrNotFound == spSettingsId )
            {
            if ( KNSmlDMVoIPValueFalse() == aObject )
                {
                spSettings->iImEnabled = EFalse;
                addSpSettings = ETrue;
                }
            else if ( KNSmlDMVoIPValueTrue() == aObject )
                {
                spSettings->iImEnabled = ETrue;
                addSpSettings = ETrue;
                }
            else
                {
                retValue = CSmlDmAdapter::EInvalidObject;
                }
            }
        else
            {
            if ( KNSmlDMVoIPValueFalse() == aObject )
                {
                iSPSettings[spSettingsId]->iImEnabled = EFalse;
                }
            else if ( KNSmlDMVoIPValueTrue() == aObject )
                {
                iSPSettings[spSettingsId]->iImEnabled = ETrue;
                }
            else
                {
                retValue = CSmlDmAdapter::EInvalidObject;
                }
            }
        }

    // ==============================
    // SettingIds node (update)
    // ==============================
    //

    // VoIP/x/SettingIds
    if ( KNSmlDMVoIPSettingId() == LastURISeg( aLUID ).Left( 
        KNSmlDMVoIPSettingId().Length() ) 
        && KFiveSegs == NumOfURISegs( aURI ) )
        {
        retValue = UpdateSettingIdsSpecificObjectL( aLUID, uriSeg, aObject );
        }

    // ==============================
    // PreferredCodecs node (update)
    // ==============================
    //

    // VoIP/x/PreferredCodecs node
    if ( KNSmlDMVoIPPreferredCodecId() == uriSeg
        && KFiveSegs == NumOfURISegs( aURI ) ) 
        {
        // Check that codec ID is found from codec entries before setting it.
        // Preferred codec ID setting is only for knowing and changing an
        // already existing codec priority.
        if ( FindCodecLocation( DesToInt( aObject ) ) != KErrNotFound &&
            DesToInt( aLUID.Right( 10 ) ) <= iProfileEntries[iProfileID]->
            iPreferredCodecs.Count() )
            {
            iProfileEntries[iProfileID]->iPreferredCodecs[ 
                DesToInt( aLUID.Right( 10 ))] = DesToInt( aObject );
            }
        else
            {
            // Codec is not found from codecsettings or
            // invalid LUID is passed to the adapter.
            retValue = CSmlDmAdapter::EInvalidObject;
            }

        }

    // ==============================
    // CodecSettings node (update)
    // ==============================
    //
    if ( KNSmlDMVoIPCodecSettingsId() == LastURISeg( aLUID ).
        Left( KNSmlDMVoIPCodecSettingsId().Length() )
        && KFiveSegs == NumOfURISegs( aURI ) )
        {
        retValue = UpdateCodecSpecificObject( aLUID, uriSeg, aObject );
        }

    // ==============================
    // VoiceMailBox node (update)
    // ==============================
    //
    if ( KNSmlDMVoIPVmbxNode() == secondLastUriSeg && 
        KFourSegs == NumOfURISegs( aURI ) )
        {
        if ( KNSmlDMVoIPMwiUri() == uriSeg )
            {
            if ( KErrNotFound == spSettingsId )
                {
                delete spSettings->iVmbxMwiUri;
                spSettings->iVmbxMwiUri = NULL;
                spSettings->iVmbxMwiUri = HBufC::NewL( aObject.Length() );
                spSettings->iVmbxMwiUri->Des().Copy( aObject );
                addSpSettings = ETrue;
                }
            else
                {
                delete iSPSettings[spSettingsId]->iVmbxMwiUri;
                iSPSettings[spSettingsId]->iVmbxMwiUri = NULL;
                iSPSettings[spSettingsId]->iVmbxMwiUri = HBufC::NewL( 
                    aObject.Length() );
                iSPSettings[spSettingsId]->iVmbxMwiUri->Des().Copy( aObject );
                }
            }
        else if ( KNSmlDMVoIPListeningUri() == uriSeg )
            {
            if ( KErrNotFound == spSettingsId )
                {
                delete spSettings->iVmbxListenUri;
                spSettings->iVmbxListenUri = NULL;
                spSettings->iVmbxListenUri = HBufC::NewL( aObject.Length() );
                spSettings->iVmbxListenUri->Des().Copy( aObject );
                addSpSettings = ETrue;
                }
            else
                {
                delete iSPSettings[spSettingsId]->iVmbxListenUri;
                iSPSettings[spSettingsId]->iVmbxListenUri = NULL;
                iSPSettings[spSettingsId]->iVmbxListenUri = HBufC::NewL( 
                    aObject.Length() );
                iSPSettings[spSettingsId]->iVmbxListenUri->Des().Copy( 
                    aObject );
                }
            }
        else if ( KNSmlDMVoIPUsedIAPId() == uriSeg )
            {
            TInt iapId = ConRefL( aObject );
            if ( KErrNotFound == iapId )
                {
                retValue = CSmlDmAdapter::EInvalidObject;
                }
            else
                {
                if ( KErrNotFound == spSettingsId )
                    {
                    spSettings->iVmbxIapId = iapId;
                    addSpSettings = ETrue;
                    }
                else
                    {
                    iSPSettings[spSettingsId]->iVmbxIapId = iapId;
                    }
                }
            }
        else if ( KNSmlDMVoIPReSubscribeInterval() == uriSeg )
            {
            if ( KErrNotFound == spSettingsId )
                {
                spSettings->iReSubscribeInterval = DesToInt( aObject );
                addSpSettings = ETrue;
                }
            else
                {
                iSPSettings[spSettingsId]->iReSubscribeInterval = 
                    DesToInt( aObject );
                }
            }
        else if ( KNSmlDMVoIPUsedSIPProfileId() == uriSeg )
            {
            TUint32 profileId = SetSipRefL( aObject ); 
            if ( profileId > 0 )
                {
                if ( KErrNotFound == spSettingsId )
                    {
                    spSettings->iVmbxSipId = profileId;
                    addSpSettings = ETrue;
                    }
                else
                    {
                    iSPSettings[spSettingsId]->iVmbxSipId = profileId;
                    }
                }
            else
                {
                retValue = CSmlDmAdapter::EInvalidObject;
                }
            }
        else if ( KNSmlDMVoIPAutoAcceptBuddyRequest() == uriSeg )
            {
            if ( KErrNotFound == spSettingsId )
                {
                spSettings->iAutoAcceptBuddy = DesToInt( aObject );
                addSpSettings = ETrue;
                }
            else
                {
                iSPSettings[spSettingsId]->iAutoAcceptBuddy = 
                    DesToInt( aObject );
                }
            }
        else
            {
            retValue = CSmlDmAdapter::EInvalidObject;
            }
        }

    if ( addSpSettings )
        {
        // Append Service Provider Settings to array for saving them later.
        // iSPSettings array will own the object now. 
        iSPSettings.AppendL( spSettings );
        CleanupStack::Pop( spSettings ); // CS:0
        }
    else
        {
        // Local Service Provider Settings not needed.
        CleanupStack::PopAndDestroy( spSettings ); // CS:0
        }

    iDmCallback->SetStatusL( aStatusRef, retValue );

    DBG_PRINT("CNSmlDmVoIPAdapter::UpdateLeafObjectL(): end");
    }
    
// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::UpdateCodecSpecificObject
// Updates Codec specific object.
// ---------------------------------------------------------------------------
//
CSmlDmAdapter::TError CNSmlDmVoIPAdapter::UpdateCodecSpecificObject(
    const TDesC8& aLUID, const TDesC8& aUriSeg, const TDesC8& aObject )
    {
    DBG_PRINT("CNSmlDmVoIPAdapter::UpdateCodecSpecificObject(): begin");

    CSmlDmAdapter::TError retValue = CSmlDmAdapter::EOk;

    // Get codec ID used by these settings.
    TUint32 codecId = DesToInt( aLUID.Right( 10 ) );
    TInt codecPos = FindCodecLocation( codecId );
    if ( KErrNotFound != codecPos )
        {
        // Add profile to be saved if not done already.
        if ( KErrNotFound == iCodecModifs.Find( codecPos ) )
            {
            iCodecModifs.Insert( codecPos, iCodecModifs.Count() );
            }
        //CodecId
        if ( KNSmlDMVoIPCodecId() == aUriSeg ) 
            {
            iCodecEntries[codecPos]->iCodecId = DesToInt( aObject );
            }
        //MediaTypeName
        if ( KNSmlDMVoIPMediaTypeName() == aUriSeg ) 
            {
            iCodecEntries[codecPos]->iMediaTypeName.Copy( aObject );
            }
        //MediaSubTypeName
        if ( KNSmlDMVoIPMediaSubTypeName() == aUriSeg ) 
            {
            iCodecEntries[codecPos]->
                iMediaSubTypeName.Copy( aObject );
            }
        //JitterBufferSize
        if ( KNSmlDMVoIPJitterBufferSize() == aUriSeg ) 
            {
            iCodecEntries[codecPos]->iJitterBufferSize = 
                DesToInt( aObject );
            }
        //OctetAlign
        if ( KNSmlDMVoIPOctetAlign() == aUriSeg ) 
            {
            if ( KNSmlDMVoIPValueTrue() == aObject )
                {
                iCodecEntries[codecPos]->iOctetAlign = 
                    CRCSEAudioCodecEntry::EOn;
                }
            else if ( KNSmlDMVoIPValueFalse() == aObject )
                {
                iCodecEntries[codecPos]->iOctetAlign = 
                    CRCSEAudioCodecEntry::EOff;
                }
            else
                {
                retValue = CSmlDmAdapter::EInvalidObject;
                }
            }
        //ModeChangePeriod
        if ( KNSmlDMVoIPModeChangePeriod() == aUriSeg ) 
            {
            iCodecEntries[codecPos]->iModeChangePeriod = 
                DesToInt( aObject );
            }
        //ModeChangeNeighbor                
        if ( KNSmlDMVoIPModeChangeNeighbor() == aUriSeg ) 
            {
            if ( KNSmlDMVoIPValueTrue() == aObject )
                {
                iCodecEntries[codecPos]->iModeChangeNeighbor = 
                    CRCSEAudioCodecEntry::EOn;
                }
            else if ( KNSmlDMVoIPValueFalse() == aObject )
                {
                iCodecEntries[codecPos]->iModeChangeNeighbor = 
                    CRCSEAudioCodecEntry::EOff;
                }
            else
                {
                retValue = CSmlDmAdapter::EInvalidObject;
                }
            }
        //Ptime
        if ( KNSmlDMVoIPPtime() == aUriSeg ) 
            {
            iCodecEntries[codecPos]->iPtime = DesToInt( aObject );
            }
        //Maxptime
        if ( KNSmlDMVoIPMaxPtime() == aUriSeg ) 
            {
            iCodecEntries[codecPos]->iMaxptime = DesToInt( aObject );
            }
        //VAD                
        if ( KNSmlDMVoIPVAD() == aUriSeg ) 
            {
            if ( KNSmlDMVoIPValueTrue() == aObject )
                {
                iCodecEntries[codecPos]->iVAD = 
                    CRCSEAudioCodecEntry::EOn;
                }
            else if ( KNSmlDMVoIPValueFalse() == aObject )
                {
                iCodecEntries[codecPos]->iVAD = 
                    CRCSEAudioCodecEntry::EOff;
                }
            else
                {
                retValue = CSmlDmAdapter::EInvalidObject;
                }
            }
        //AnnexB                
        if ( KNSmlDMVoIPAnnexB() == aUriSeg ) 
            {
            if ( KNSmlDMVoIPValueTrue() == aObject )
                {
                iCodecEntries[codecPos]->iAnnexb =
                    CRCSEAudioCodecEntry::EOn;
                }
            else if ( KNSmlDMVoIPValueFalse() == aObject )
                {
                iCodecEntries[codecPos]->iAnnexb =
                    CRCSEAudioCodecEntry::EOff;
                }
            else
                {
                retValue = CSmlDmAdapter::EInvalidObject;
                }
            }
        // MaxRed
        if ( KNSmlDMVoIPMaxRed() == aUriSeg )
            {
            iCodecEntries[codecPos]->iMaxRed = DesToInt( aObject );
            }

        // ==============================
        // ModeSet node ( update )
        // ==============================
        //
        if ( KNSmlDMVoIPMode() == aUriSeg )  
            {
            // Add profile to be saved if not done already.
            if ( KErrNotFound == iCodecModifs.Find( codecPos ) )
                {
                iCodecModifs.Insert( codecPos, iCodecModifs.Count() );
                }
            iCodecEntries[codecPos]->iModeSet[ 
                DesToInt( aLUID.Right( 10 ))] = DesToInt( aObject );
            }
        }
    else
        {
        // codecsetting not found even in luid exists
        retValue = CSmlDmAdapter::EError;
        }
    DBG_PRINT("CNSmlDmVoIPAdapter::UpdateCodecSpecificObject(): end");

    return retValue;
    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::UpdateSettingIdsSpecificObjectL
// Updates SettingIds specific object.
// ---------------------------------------------------------------------------
//
CSmlDmAdapter::TError CNSmlDmVoIPAdapter::UpdateSettingIdsSpecificObjectL(
    const TDesC8& aLUID, const TDesC8& aUriSeg, const TDesC8& aObject )
    {
    DBG_PRINT("CNSmlDmVoIPAdapter::UpdateSettingIdsSpecificObjectL(): start");

    CSmlDmAdapter::TError retValue = CSmlDmAdapter::EOk;

    // Get SIP Specific ID used by these settings.

    // Add profile to be saved if not done already.
    if ( KErrNotFound == iProfileModifs.Find( iProfileID ) )
        {
        iProfileModifs.Insert( iProfileID, iProfileModifs.Count() );
        }
    //ProfileType
    if ( KNSmlDMVoIPProfileType() == aUriSeg )
        {
        if ( KNSmlDMVoIPProfileTypeSIP() == aObject )
            {
            iProfileEntries[iProfileID]->iIds[DesToInt( 
                LastURISeg( aLUID ).Right( 10 ))].iProfileType = 0;
            }
        else if ( KNSmlDMVoIPProfileTypeSCCP() == aObject )
            {
            iProfileEntries[iProfileID]->iIds[DesToInt( 
                LastURISeg( aLUID ).Right( 10 ))].iProfileType = 1;
            }
        else
            {
            retValue = CSmlDmAdapter::EInvalidObject;   
            }
        }
    // ProfileId
    if ( KNSmlDMVoIPProfileId() == aUriSeg )
        {
        // Handle SIP type.
        if ( 0 == iProfileEntries[iProfileID]->iIds[DesToInt( 
            LastURISeg( aLUID ).Right( 10 ))].iProfileType )
            {
            TUint32 profileId = SetSipRefL( aObject ); 
            if ( profileId > 0 )
                {
                // Set profileId to settingsids.
                iProfileEntries[iProfileID]->
                    iIds[DesToInt( LastURISeg( aLUID ).Right( 10 ))].
                    iProfileId = profileId;
                retValue = CSmlDmAdapter::EOk;
                }
            else
                {
                retValue = CSmlDmAdapter::EInvalidObject;
                }
            }  
        // Handle SCCP type
        else if ( 1 == iProfileEntries[iProfileID]->iIds[DesToInt( 
            LastURISeg( aLUID ).Right( 10 ))].iProfileType )
            {
            TUint32 profileId = SetSccpRefL( aObject ); 

            // profileId can be zero
            if ( profileId > 0 || 0 == profileId )
                {
                // set profileId to settingsids
                iProfileEntries[iProfileID]->iIds[DesToInt( 
                    LastURISeg( aLUID ).Right( 10 ))].
                    iProfileId = profileId;
                retValue = CSmlDmAdapter::EOk;
                }
            else
                {
                retValue = CSmlDmAdapter::EInvalidObject;
                }
            }
        else
            {
            // no SCCP or SIP type found
            retValue = CSmlDmAdapter::EError;
            }
        }
    // ProfileSpecificId
    if ( KNSmlDMVoIPProfileSpecificId() == aUriSeg )
        {
        // SIPSpecific settings have been removed so set
        // SIPSpecific ID as not found.
        iProfileEntries[iProfileID]->iIds[DesToInt( 
            LastURISeg( aLUID ).Right( 10 ))].
            iProfileSpecificSettingId = KErrNotFound;

        // Set status to OK so we won't break backwards compatibility.
        // Some DM servers may react to receiving other status than OK
        // when sending settings they think are supported.
        retValue = CSmlDmAdapter::EOk;
        }

    DBG_PRINT("CNSmlDmVoIPAdapter::UpdateSettingIdsSpecificObjectL(): end");

    return retValue;
    }
// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::DeleteObjectL
// This deletes VoIP profiles
// ---------------------------------------------------------------------------
//
void CNSmlDmVoIPAdapter::DeleteObjectL( 
    const TDesC8& aURI, 
    const TDesC8& aLUID, 
    const TInt aStatusRef )
    {
    DBG_PRINT("CNSmlDmVoIPAdapter::DeleteObjectL( ): begin");

    __ASSERT_ALWAYS( iDmCallback != NULL, User::Leave( KErrArgument ) );

    // Load VoIP profile information if not loaded yet
    if ( !iProfileEntries.Count() )
        {
        LoadProfilesL();
        }

    CSmlDmAdapter::TError retValue = CSmlDmAdapter::EError;

    // Get URI parameters.
    TBuf8<KNSmlVoIPMaxUriLength> uri = aURI;
    TPtrC8 uriSeg = LastURISeg( aURI );
    TPtrC8 uriTmp = RemoveLastURISeg( aURI );
    TPtrC8 secondLastUriSeg = LastURISeg( uriTmp );

    // Copy LUID as last segment if exists.
    if ( 0 < aLUID.Length() )
        {
        uri.Copy ( aLUID );
        }
    iProfileID = FindProfileLocation( uri );
    if ( KErrNotFound == iProfileID )
        {
        iProfileID = FindProfileLocation( aURI );
        if ( KErrNotFound == iProfileID )
            {
            retValue = CSmlDmAdapter::ENotFound;
            iDmCallback->SetStatusL( aStatusRef, retValue );
            DBG_PRINT("CNSmlDmVoIPAdapter::DeleteObjectL( ): ENotFound end");
            return;
            }
        } 

    // ==============================
    // VoIP/x (delete)
    // ==============================
    //
    if ( secondLastUriSeg == KNSmlDMVoIPNodeName
        && aLUID.Find( KNSmlDMVoIPPrefix ) != KErrNotFound )
        {
        // Delete profile from RCSE.
        iCRCSEProfile->DeleteL( iProfileEntries[iProfileID]->iId );

        // Delete profile also from memory and compress array.
        delete iProfileEntries[iProfileID];
        iProfileEntries[iProfileID] = NULL;
        iProfileEntries.Remove( iProfileID );
        iProfileEntries.Compress();
        iDmCallback->SetMappingL( aURI, KNullDesC8 ); 
        retValue = CSmlDmAdapter::EOk;

        // Fall back to CS preferred when no more VoIP profiles
        TInt profileCount = iProfileEntries.Count();
        if ( KErrNone == profileCount )
            {
            SetTelephonyPreferenceL( ECSPreferred );
            }
        }

    // ==============================
    // CodecSettings node ( delete ) 
    // ==============================
    //
    else if ( KNSmlDMVoIPCodecSettingsNode() == secondLastUriSeg  
        && KFourSegs == NumOfURISegs( aURI ) 
        && aLUID.Find( KNSmlDMVoIPCodecSettingsId ) != KErrNotFound )
        {
        // Get codec ID used by this setting.
        TUint32 codecId = DesToInt( aLUID.Right(10) );
        TInt codecPos = FindCodecLocation( codecId );    
        if ( KErrNotFound != codecPos )
            {
            // Delete codec settings from RCSE.
            iCRCSEAudioCodec->DeleteL( codecId );
            TInt preferredPos = iProfileEntries[iProfileID]->
                iPreferredCodecs.Find( codecId );

            // Remove codec settings ID from profiles iPreferredCodecs.
            iProfileEntries[iProfileID]->iPreferredCodecs.Remove(
                preferredPos );
            iProfileEntries[iProfileID]->iPreferredCodecs.Compress();

            // Remove codec settings from iCodecEntries and update
            // Profile back to RCSE. 
            delete iCodecEntries[codecPos];
            iCodecEntries[codecPos] = NULL;
            iCodecEntries.Remove( codecPos );
            iCodecEntries.Compress();
            iCRCSEProfile->UpdateL( iProfileEntries[iProfileID]->
                iId, *iProfileEntries[iProfileID] );
            retValue = CSmlDmAdapter::EOk;
            }
        else
            {
            retValue = CSmlDmAdapter::EError;    
            }
        }

    // ==============================
    // VoIP/x/SettingIds ( delete ) 
    // ==============================,
    //
    else if ( KNSmlDMVoIPSettingIdsNode() == secondLastUriSeg  
        && KFourSegs == NumOfURISegs( aURI ) 
        && aLUID.Find( KNSmlDMVoIPSettingId ) != KErrNotFound )
        {
        iProfileEntries[iProfileID]->iIds.Remove( 
            DesToInt( aLUID.Right( 10 ) ) );
        iProfileEntries[iProfileID]->iIds.Compress();
        iCRCSEProfile->UpdateL( iProfileEntries[iProfileID]->
            iId, *iProfileEntries[iProfileID] );
        retValue = CSmlDmAdapter::EOk;
        }

    // ==============================
    // VoIP/x/CodecSettings/x/ModeSet Node ( delete ) 
    // ==============================
    //
    else if ( KNSmlDMVoIPModeSetNode() == secondLastUriSeg 
        && KSixSegs == NumOfURISegs( aURI )  
        && aLUID.Find( KNSmlDMVoIPModeId ) != KErrNotFound )
        {
        // get codecid used by this setting
        TUint32 codecId = DesToInt( RemoveLastURISeg( aLUID ).Right( 10 ) );
        TInt codecPos = FindCodecLocation( codecId );
        if ( KErrNotFound != codecPos )
            {
            // remove modeset from iCodecEntries    
            iCodecEntries[codecPos]->
                iModeSet.Remove( DesToInt( aLUID.Right( 10 ) ) );
            iCodecEntries[codecPos]->
                iModeSet.Compress();

            // update changes also to rcse    
            iCRCSEAudioCodec->UpdateL( codecId, *iCodecEntries[codecPos] );
            iDmCallback->SetMappingL( aURI, KNullDesC8 );     
            retValue = CSmlDmAdapter::EOk;
            }
        else
            {
            retValue = CSmlDmAdapter::EError;   
            }
        }
    else
        {
        retValue = CSmlDmAdapter::EInvalidObject;
        }
    iDmCallback->SetStatusL( aStatusRef, retValue ); 
    DBG_PRINT("CNSmlDmVoIPAdapter::DeleteObjectL( ): end");
    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::FetchLeafObjectL
// Fetches the values of leaf objects.
// ---------------------------------------------------------------------------
//
void CNSmlDmVoIPAdapter::FetchLeafObjectL( 
    const TDesC8& aURI, 
    const TDesC8& aLUID, 
    const TDesC8& aType, 
    const TInt aResultsRef, 
    const TInt aStatusRef )
    {
    DBG_PRINT("CNSmlDmVoIPAdapter::FetchLeafObjectL(): begin");

    __ASSERT_ALWAYS( iDmCallback != NULL, User::Leave( KErrArgument ) );

    // Load VoIP profile information if not loaded yet
    if ( !iProfileEntries.Count() )
        {
        LoadProfilesL();
        }  

    CBufBase *result = CBufFlat::NewL( KDefaultResultSize );
    CleanupStack::PushL( result );
    CSmlDmAdapter::TError status = FetchObjectL( aURI, aLUID, *result );

    if ( CSmlDmAdapter::EOk == status )
        {
        iDmCallback->SetResultsL( aResultsRef, *result, aType );
        }
    iDmCallback->SetStatusL( aStatusRef, status );

    CleanupStack::PopAndDestroy( result ); //result
    result = NULL;
    DBG_PRINT("CNSmlDmVoIPAdapter::FetchLeafObjectL(): end");
    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::FetchObjectL()
// Fetches the values of leaf objects.
// ---------------------------------------------------------------------------
//
CSmlDmAdapter::TError CNSmlDmVoIPAdapter::FetchObjectL( 
    const TDesC8& aURI, const TDesC8& aLUID,
    CBufBase& result )
    {
    DBG_PRINT("CNSmlDmVoIPAdapter::FetchObjectL(): start");
    CSmlDmAdapter::TError retValue = CSmlDmAdapter::EOk;
    typedef CRCSEProfileEntry Entry;

    // Check that LUID mapping exists.
    if ( !aLUID.Length() > 0 )
        {
        DBG_PRINT("CNSmlDmVoIPAdapter::FetchLeafObjectL(internal)()():ENotFound end");
        return CSmlDmAdapter::ENotFound;
        }

    TBuf8<KNSmlVoIPMaxResultLength> segmentResult;
    TPtrC8 uriSeg = LastURISeg( aURI );

    TPtrC8 uriTmp = RemoveLastURISeg( aURI );
    TPtrC8 secondLastUriSeg = LastURISeg( uriTmp );

    // Find profile and get its settings.
    iProfileID = FindProfileLocation( aLUID );

    // If profile is not found return error.
    if ( KErrNotFound == iProfileID )
        {
        DBG_PRINT( "CNSmlDmVoIPAdapter::FetchObjectL(): ENotFound end" );
        return CSmlDmAdapter::ENotFound;
        }

    // Set SPS ready for getting values.
    CSPSettings* spSettings = CSPSettings::NewLC(); // CS:1
    TUint32 serviceId = iProfileEntries[iProfileID]->iServiceProviderId;

    //VoIP/x/VoIPId
    if ( KNSmlDMVoIPSetID() == uriSeg )
        {
        segmentResult.AppendNumFixedWidthUC( 
            iProfileEntries[iProfileID]->iId, EDecimal, 10 );
        }
    //VoIP/x/ProviderName
    else if ( KNSmlDMVoIPProvID() == uriSeg )
        {
        segmentResult.Copy( iProfileEntries[iProfileID]->
            iProviderName );
        }
    //VoIP/x/SettingsName
    else if ( KNSmlDMVoIPName() == uriSeg )
        {
        segmentResult.Copy( iProfileEntries[iProfileID]->
            iSettingsName );
        }
    //VoIP/x/SMPort
    else if ( KNSmlDMVoIPSMPort() == uriSeg )
        {
        segmentResult.Num( iProfileEntries[iProfileID]->
            iStartMediaPort );
        }
    //VoIP/x/EMPort
    else if ( KNSmlDMVoIPEMPort() == uriSeg )
        {
        segmentResult.Num( iProfileEntries[iProfileID]->
            iEndMediaPort );
        }
    //VoIP/x/MediaQos
    else if ( KNSmlDMVoIPMediaQos() == uriSeg )
        {
        segmentResult.Num( iProfileEntries[iProfileID]->
            iMediaQOS );
        }
    //VoIP/x/DTMFIB
    else if ( KNSmlDMVoIPDTMFIB() == uriSeg )
        {
        if ( Entry::EOONotSet == iProfileEntries[iProfileID]->
            iInbandDTMF )
            {
            segmentResult.Copy( KNSmlDMVoIPValueNotSet );
            }
        if ( Entry::EOn == iProfileEntries[iProfileID]->iInbandDTMF )
            {
            segmentResult.Copy( KNSmlDMVoIPValueTrue );
            }
        if ( Entry::EOff == iProfileEntries[iProfileID]->iInbandDTMF )
            {
            segmentResult.Copy( KNSmlDMVoIPValueFalse );
            }
        }
    //VoIP/x/DTMFOB
    else if ( KNSmlDMVoIPDTMFOB() == uriSeg )
        {
        if ( Entry::EOONotSet == iProfileEntries[iProfileID]->
            iOutbandDTMF )
            {
            segmentResult.Copy( KNSmlDMVoIPValueNotSet );
            }
        if ( Entry::EOff == iProfileEntries[iProfileID]->iOutbandDTMF )
            {
            segmentResult.Copy( KNSmlDMVoIPValueFalse );
            }
        if ( Entry::EOn == iProfileEntries[iProfileID]->iOutbandDTMF )
            {
            segmentResult.Copy( KNSmlDMVoIPValueTrue );
            }
        }
    // VoIP/x/SecureCallPref
    else if ( KNSmlDMVoIPSecureCallPref() == uriSeg )
        {
        segmentResult.Num( iProfileEntries[iProfileID]->
            iSecureCallPreference );
        }
    // VoIP/x/RTCP
    else if ( KNSmlDMVoIPRTCP() == uriSeg )
        {
        segmentResult.Num( iProfileEntries[iProfileID]->iRTCP );
        }
   // VoIP/x/UAHTerminalType
    else if ( KNSmlDMVoIPUAHTerminalType() == uriSeg )
        {
        segmentResult.Num( iProfileEntries[iProfileID]->
            iSIPVoIPUAHTerminalType );
        }
    // VoIP/x/UAHWLANMAC
    else if ( KNSmlDMVoIPUAHWLANMAC() == uriSeg )
        {
        segmentResult.Num( iProfileEntries[iProfileID]->
            iSIPVoIPUAHeaderWLANMAC );
        }
    // VoIP/x/UAHString
    else if ( KNSmlDMVoIPUAHString() == uriSeg )
        {
        segmentResult.Copy( iProfileEntries[iProfileID]->
            iSIPVoIPUAHeaderString );
        }
    // VoIP/x/ProfileLockedToIAP
    else if ( KNSmlDMVoIPProfileLockedToIAP() == uriSeg )
        {
        if ( Entry::EOONotSet == iProfileEntries[iProfileID]->
            iProfileLockedToIAP )
            {
            segmentResult.Copy( KNSmlDMVoIPValueNotSet );
            }
        if ( Entry::EOn == iProfileEntries[iProfileID]->
            iProfileLockedToIAP )
            {
            segmentResult.Copy( KNSmlDMVoIPValueTrue );
            }
        if ( Entry::EOff == iProfileEntries[iProfileID]->
            iProfileLockedToIAP )
            {
            segmentResult.Copy( KNSmlDMVoIPValueFalse );
            }
        }
    // VoIP/x/VoIPPluginUID
    else if ( KNSmlDMVoIPPluginUID() == uriSeg )
        {
        segmentResult.Num( iProfileEntries[iProfileID]->iVoIPPluginUID );
        }
    // VoIP/x/AllowVoIPOverWCDMA
    else if ( KNSmlDMVoIPAllowVoIPOverWCDMA() == uriSeg )
        {
        if ( Entry::EOONotSet == iProfileEntries[iProfileID]->
            iAllowVoIPoverWCDMA )
            {
            segmentResult.Copy( KNSmlDMVoIPValueNotSet );
            }
        if ( Entry::EOn == iProfileEntries[iProfileID]->
            iAllowVoIPoverWCDMA )
            {
            segmentResult.Copy( KNSmlDMVoIPValueTrue );
            }
        if ( Entry::EOff == iProfileEntries[iProfileID]->
            iAllowVoIPoverWCDMA )
            {
            segmentResult.Copy( KNSmlDMVoIPValueFalse );
            }
        }
    // VoIP/x/VoIPDigits
    else if ( KNSmlDMVoIPVoIPDigits() == uriSeg )
        {
        segmentResult.Num( iProfileEntries[iProfileID]->
            iMeanCountOfVoIPDigits );
        }
    // VoIP/x/URIDomainIgnoreRule
    else if ( KNSmlDMVoIPURIDomainIgnoreRule() == uriSeg )
        {
        segmentResult.Num( iProfileEntries[iProfileID]->
            iIgnoreAddrDomainPart );
        }
    // VoIP/x/AutoAcceptBuddyRequest
    else if ( KNSmlDMVoIPAutoAcceptBuddyRequest() == uriSeg )
        {
        CSPProperty* autoAcceptBuddy = CSPProperty::NewLC();
        TInt err = spSettings->FindPropertyL( serviceId, 
            ESubPropertyPresenceRequestPreference, *autoAcceptBuddy );
        if ( KErrNone != err )
            {
            retValue = CSmlDmAdapter::EError;
            }
        else
            {
            TInt autoAccept;
            err = autoAcceptBuddy->GetValue( autoAccept );
            if ( KErrNone == err )
                {
                segmentResult.Num( autoAccept );
                }
            else
                {
                retValue = CSmlDmAdapter::EError;
                }
            }
        CleanupStack::PopAndDestroy( autoAcceptBuddy );
        }
    // VoIP/x/UsedVoIPRelease
    else if ( KNSmlDMVoIPUsedVoIPRelease() == uriSeg )
        {
        TBuf16<KMaxUriLength> voipRelease( KNullDesC16 );
        voipRelease.Copy( spSettings->GetSIPVoIPSWVersion() );
        if ( !voipRelease.Length() )
            {
            retValue = CSmlDmAdapter::EError;
            }
        else
            {
            segmentResult.Copy( voipRelease );
            }
        }
    // VoIP/x/AddUserPhone
    else if ( KNSmlDMVoIPAddUserPhone() == uriSeg )
        {
        if ( Entry::EOONotSet == iProfileEntries[iProfileID]->
            iUserPhoneUriParameter )
            {
            segmentResult.Copy( KNSmlDMVoIPValueNotSet );
            }
        else if ( Entry::EOn == iProfileEntries[iProfileID]->
            iUserPhoneUriParameter )
            {
            segmentResult.Copy( KNSmlDMVoIPValueTrue );
            }
        else if ( Entry::EOff == iProfileEntries[iProfileID]->
            iUserPhoneUriParameter )
            {
            segmentResult.Copy( KNSmlDMVoIPValueFalse );
            }
        }
    // VoIP/x/ProviderBookmarkURI
    else if ( KNSmlDMVoIPProviderBookmarkUri() == uriSeg )
        {
        CSPProperty* providerBkmark = CSPProperty::NewLC();
        TInt err = spSettings->FindPropertyL( serviceId, 
            EPropertyServiceBookmarkUri, *providerBkmark );
        if ( KErrNone != err )
            {
            retValue = CSmlDmAdapter::EError;
            }
        else
            {
            TBuf16<KNSmlVoIPMaxUriLength> bkmark;
            err = providerBkmark->GetValue( bkmark );
            if ( KErrNone == err )
                {
                segmentResult.Copy( bkmark );
                }
            else
                {
                retValue = CSmlDmAdapter::EError;
                }
            }
        CleanupStack::PopAndDestroy( providerBkmark );
        }
    // VoIP/x/SIPConnectivityTestURI
    else if ( KNSmlDMVoIPSIPConnTestUri() == uriSeg )
        {
        segmentResult.Copy( iProfileEntries[iProfileID]->
            iSIPConnTestAddress );
        }
    // VoIP/x/NATFWProfileId
    else if ( KNSmlDMVoIPNatFwProfileId() == uriSeg )
        {
        TBuf16<KMaxUriLength> natfwProfile;
        retValue = GetNatFwUriL( natfwProfile, 
            iProfileEntries[iProfileID]->iNATSettingsStorageId );
        if ( CSmlDmAdapter::EOk == retValue )
            {
            segmentResult.Copy( natfwProfile );
            }
        }
    // VoIP/x/MinSessionInterval
    else if ( KNSmlDMVoIPMinSessionInterval() == uriSeg )
        {
        segmentResult.Num( iProfileEntries[iProfileID]->iSIPMinSE );
        }
    // VoIP/x/SessionExpires
    else if ( KNSmlDMVoIPSessionExpires() == uriSeg )
        {
        segmentResult.Num( iProfileEntries[iProfileID]->
            iSIPSessionExpires );
        }
    // VoIP/x/BradingDataURI
    else if ( KNSmlDMVoIPBrandingDataUri() == uriSeg )
        {
        CSPProperty* brandingUri = CSPProperty::NewLC();
        TInt err = spSettings->FindPropertyL( serviceId, 
            ESubPropertyVoIPBrandDataUri, *brandingUri );
        if ( KErrNone != err )
            {
            retValue = CSmlDmAdapter::EError;
            }
        else
            {
            TBuf16<KNSmlVoIPMaxUriLength> brandUri;
            err = brandingUri->GetValue( brandUri );
            if ( KErrNone == err )
                {
                segmentResult.Copy( brandUri );
                }
            else
                {
                retValue = CSmlDmAdapter::EError;
                }
            }
        CleanupStack::PopAndDestroy( brandingUri );
        }
    // VoIP/x/PresenceSettingsId
    else if ( KNSmlDMVoIPPresenceSettingsId() == uriSeg )
        {
        CSPProperty* presenceId = CSPProperty::NewLC();
        TInt err = spSettings->FindPropertyL( serviceId, 
            ESubPropertyPresenceSettingsId, *presenceId );
        if ( KErrNone != err )
            {
            retValue = CSmlDmAdapter::EError;
            }
        else
            {
            TInt presId;
            err = presenceId->GetValue( presId );
            if ( KErrNone == err )
                {
                TBuf16<KMaxUriLength> presenceUri;
                retValue = GetPresenceUriL( presenceUri, (TUint32)presId );
                if ( EOk == retValue )
                    {
                    segmentResult.Copy( presenceUri );
                    }
                }
            else
                {
                retValue = CSmlDmAdapter::EError;
                }
            }
        CleanupStack::PopAndDestroy( presenceId );
        }
    // VoIP/x/UsedNATProtocol
    else if ( KNSmlDMVoIPUsedNatProtocol() == uriSeg )
        {
        segmentResult.Num( iProfileEntries[iProfileID]->iNATProtocol );
        }
    // VoIP/x/AutoEnable
    else if ( KNSmlDMVoIPAutoEnable() == uriSeg )
        {
        CSPProperty* autoEnableService = CSPProperty::NewLC();
        TInt err = spSettings->FindPropertyL( serviceId, 
            ESubPropertyVoIPEnabled, *autoEnableService );
        if ( KErrNone != err )
            {
            retValue = CSmlDmAdapter::EError;
            }
        else
            {
            TInt autoEnable;
            err = autoEnableService->GetValue( autoEnable );
            if ( KErrNone == err )
                {
                segmentResult.Num( autoEnable );
                }
            else
                {
                retValue = CSmlDmAdapter::EError;
                }
            }
        CleanupStack::PopAndDestroy( autoEnableService );
        }
    // VoIP/x/SnapProfileId
    else if ( KNSmlDMVoIPSnapProfileId() == uriSeg )
        {
        CSPProperty* snapProfileId = CSPProperty::NewLC();
        TInt err = spSettings->FindPropertyL( serviceId, 
            ESubPropertyVoIPPreferredSNAPId, *snapProfileId );
        if ( KErrNone != err )
            {
            retValue = CSmlDmAdapter::EError;
            }
        else
            {
            TInt snapId;
            err = snapProfileId->GetValue( snapId );
            if ( KErrNone == err )
                {
                TBuf8<KMaxUriLength> snapUri;
                retValue = GetSnapUriL( snapUri, (TUint32)snapId );
                if ( EOk == retValue )
                    {
                    segmentResult.Copy( snapUri );
                    }
                }
            else
                {
                retValue = CSmlDmAdapter::EError;
                }
            }
        CleanupStack::PopAndDestroy( snapProfileId );
        }
    // VoIP/x/EnableIM
    else if ( KNSmlDMVoIPEnableIm() == uriSeg )
        {
        CSPProperty* enableIm = CSPProperty::NewLC();
        TInt err = spSettings->FindPropertyL( serviceId, 
            ESubPropertyIMEnabled, *enableIm );
        if ( KErrNotFound == err )
            {
            segmentResult.Copy( KNSmlDMVoIPValueFalse );
            }
        else if ( KErrNone != err )
            {
            retValue = CSmlDmAdapter::EError;
            }
        else
            {
            TInt enableValue;
            err = enableIm->GetValue( enableValue );
            if ( KErrNone == err && 0 < enableValue )
                {
                segmentResult.Copy( KNSmlDMVoIPValueTrue );
                }
            else if ( KErrNone == err && !enableValue )
                {
                segmentResult.Copy( KNSmlDMVoIPValueFalse );
                }
            else
                {
                retValue = CSmlDmAdapter::EError;
                }
            }
        CleanupStack::PopAndDestroy( enableIm );
        }

    // ==============================
    // CodecSettings node ( fetch )
    // ==============================
    //
    // VoIP/x/PreferredCodecs node
    if ( KNSmlDMVoIPPreferredCodecId() == uriSeg.Left(
        KNSmlDMVoIPPreferredCodecId().Length() ) ) 
        {
        TInt preferredPos = DesToInt( LastURISeg( aLUID ).Right( 10 ) );
        segmentResult.Num( iProfileEntries[iProfileID]->
            iPreferredCodecs[preferredPos]);
        }

    // VoIP/x/CodecSettings node    
    if ( KNSmlDMVoIPCodecSettingsId() == LastURISeg( aLUID ).
        Left( KNSmlDMVoIPCodecSettingsId().Length() )
        && KFiveSegs == NumOfURISegs( aURI ) )
        {
        retValue = FetchCodecObject( aLUID, uriSeg, segmentResult );
        }

    // ==============================
    // SettingIds node ( fetch )
    // ==============================
    //
    if ( KNSmlDMVoIPSettingId() == LastURISeg( aLUID ).
        Left( KNSmlDMVoIPSettingId().Length() ) 
        && KFiveSegs == NumOfURISegs( aURI ) )
        {

        //ProfileType
        if ( KNSmlDMVoIPProfileType() == uriSeg )
            {
            if ( 0 == iProfileEntries[iProfileID]->iIds[DesToInt( 
                LastURISeg( aLUID ).Right( 10 ))].iProfileType )
                {
                segmentResult.Copy( KNSmlDMVoIPProfileTypeSIP );
                }
            if ( 1 == iProfileEntries[iProfileID]->iIds[DesToInt( 
                LastURISeg( aLUID ).Right( 10 ))].iProfileType )
                {
                segmentResult.Copy( KNSmlDMVoIPProfileTypeSCCP );
                }
            }

        // ProfileId
        if ( KNSmlDMVoIPProfileId() == uriSeg )
            {
            if ( 0 == iProfileEntries[iProfileID]->iIds[DesToInt( 
                LastURISeg( aLUID ).Right( 10 ))].iProfileType )
                {
                retValue = GetSipIdL( result, iProfileEntries[iProfileID]->
                    iIds[DesToInt( LastURISeg( aLUID ).Right( 10 ))].
                    iProfileId );
                }
            if ( 1 == iProfileEntries[iProfileID]->iIds[DesToInt( 
                LastURISeg( aLUID ).Right( 10 ))].iProfileType )
                {
                // Get SCCP ID from sccp adapter if exists
                retValue = GetSccpIdL( result, iProfileEntries[iProfileID]->
                    iIds[DesToInt( LastURISeg( aLUID ).Right( 10 ))].
                    iProfileId );
                }
            }

        // ProfileSpecificId
        if ( KNSmlDMVoIPProfileSpecificId() == uriSeg )
            {
            segmentResult.Num( iProfileEntries[iProfileID]->iIds[
                DesToInt( LastURISeg( aLUID ).Right( 10 ) ) ].
                iProfileSpecificSettingId );
            }
        }

    // ==============================
    // VoiceMailBox node (fetch)
    // ==============================
    //
    if ( KNSmlDMVoIPVmbxNode() == secondLastUriSeg &&
        KFourSegs == NumOfURISegs( aURI ) )
        {
        // VoIP/x/VoiceMailBox/MWI-URI
        if ( KNSmlDMVoIPMwiUri() == uriSeg )
            {
            CSPProperty* mwiUri = CSPProperty::NewLC();
            TInt err = spSettings->FindPropertyL( serviceId, 
                ESubPropertyVMBXMWIAddress, *mwiUri );
            if ( KErrNone != err )
                {
                retValue = CSmlDmAdapter::EError;
                }
            else
                {
                TBuf16<KNSmlVoIPMaxUriLength> mwiAddr;
                err = mwiUri->GetValue( mwiAddr );
                if ( KErrNone == err )
                    {
                    segmentResult.Copy( mwiAddr );
                    }
                else
                    {
                    retValue = CSmlDmAdapter::EError;
                    }
                }
            CleanupStack::PopAndDestroy( mwiUri );
            }

        // VoIP/x/VoiceMailBox/ListeningURI
        if ( KNSmlDMVoIPListeningUri() == uriSeg )
            {
            CSPProperty* listeningUri = CSPProperty::NewLC();
            TInt err = spSettings->FindPropertyL( serviceId, 
                ESubPropertyVMBXListenAddress, *listeningUri );
            if ( KErrNone != err )
                {
                retValue = CSmlDmAdapter::EError;
                }
            else
                {
                TBuf16<KNSmlVoIPMaxUriLength> listenUri;
                err = listeningUri->GetValue( listenUri );
                if ( KErrNone == err )
                    {
                    segmentResult.Copy( listenUri );
                    }
                else
                    {
                    retValue = CSmlDmAdapter::EError;
                    }
                }
            CleanupStack::PopAndDestroy( listeningUri );
            }

        // VoIP/x/VoiceMailBox/UsedIAPId
        if ( KNSmlDMVoIPUsedIAPId() == uriSeg )
            {
            CSPProperty* iapId = CSPProperty::NewLC();
            TInt err = spSettings->FindPropertyL( serviceId, 
                ESubPropertyVMBXPreferredIAPId, *iapId );
            if ( KErrNone != err )
                {
                retValue = CSmlDmAdapter::EError;
                }
            else
                {
                TInt iap;
                err = iapId->GetValue( iap );
                if ( KErrNone == err )
                    {
                    retValue = GetConRefL( result, iap );
                    if ( CSmlDmAdapter::EOk != retValue )
                        {
                        result.Reset();
                        }
                    }
                else
                    {
                    retValue = CSmlDmAdapter::EError;
                    }
                
                }
            CleanupStack::PopAndDestroy( iapId );
            }

        // VoIP/x/VoiceMailBox/ReSubscribeInterval
        if ( KNSmlDMVoIPReSubscribeInterval() == uriSeg )
            {
            CSPProperty* reSubscrInterval = CSPProperty::NewLC();
            TInt err = spSettings->FindPropertyL( serviceId, 
                ESubPropertyVMBXMWISubscribeInterval, *reSubscrInterval );
            if ( KErrNone != err )
                {
                retValue = CSmlDmAdapter::EError;
                }
            else
                {
                TInt reSubscribe;
                err = reSubscrInterval->GetValue( reSubscribe );
                if ( KErrNone == err )
                    {
                    segmentResult.Num( reSubscribe );
                    }
                else
                    {
                    retValue = CSmlDmAdapter::EError;
                    }
                }
            CleanupStack::PopAndDestroy( reSubscrInterval );
            }

        // VoIP/x/VoiceMailBox/UsedSIPProfileId
        if ( KNSmlDMVoIPUsedSIPProfileId() == uriSeg )
            {
            CSPProperty* sipProfileId = CSPProperty::NewLC();
            TInt err = spSettings->FindPropertyL( serviceId, 
                ESubPropertyVMBXSettingsId, *sipProfileId );
            if ( KErrNone != err )
                {
                retValue = CSmlDmAdapter::EError;
                }
            else
                {
                TInt sipId;
                err = sipProfileId->GetValue( sipId );
                if ( KErrNone == err )
                    {
                    retValue = GetSipIdL( result, sipId );
                    if ( CSmlDmAdapter::EOk != retValue )
                        {
                        result.Reset();
                        }
                    }
                else
                    {
                    retValue = CSmlDmAdapter::EError;
                    }
                }
            CleanupStack::PopAndDestroy( sipProfileId );
            }
        }

    result.InsertL( result.Size(), segmentResult );       
    CleanupStack::PopAndDestroy( spSettings );

    DBG_PRINT("CNSmlDmVoIPAdapter::FetchObjectL(): end");
    return retValue;
    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::FetchCodecObject
// Fetches the values of Codec specific leaf objects.
// ---------------------------------------------------------------------------
//
CSmlDmAdapter::TError CNSmlDmVoIPAdapter::FetchCodecObject( 
    const TDesC8& aLUID, const TDesC8& aUriSeg, TDes8& aSegmentResult )
    {
    DBG_PRINT("CNSmlDmVoIPAdapter::FetchCodecObject(): begin");
    
    CSmlDmAdapter::TError retValue = CSmlDmAdapter::EOk;

    TUint32 codecId = DesToInt( aLUID.Right( 10 ) );
    TInt codecPos = FindCodecLocation( codecId );
    if ( KErrNotFound != codecPos )
        {
        //CodecId
        if ( KNSmlDMVoIPCodecId() == aUriSeg ) 
            {
            aSegmentResult.Num ( iCodecEntries[codecPos]->iCodecId );
            }
        //MediaTypeName
        if ( KNSmlDMVoIPMediaTypeName() == aUriSeg ) 
            {
            aSegmentResult.Copy ( iCodecEntries[codecPos]->
                iMediaTypeName );
            }
        //MediaSubTypeName
        if ( KNSmlDMVoIPMediaSubTypeName() == aUriSeg ) 
            {
            aSegmentResult.Copy ( iCodecEntries[codecPos]->
                iMediaSubTypeName );
            }
        //JitterBufferSize
        if ( KNSmlDMVoIPJitterBufferSize() == aUriSeg ) 
            {
            if ( KNotSet < iCodecEntries[codecPos]->iJitterBufferSize )
                {
                aSegmentResult.Num ( iCodecEntries[codecPos]->
                    iJitterBufferSize );
                }
            }
        //OctetAlign            
        if ( KNSmlDMVoIPOctetAlign() == aUriSeg ) 
            {
            if ( CRCSEAudioCodecEntry::EOn == 
                iCodecEntries[codecPos]->iOctetAlign )
                {
                aSegmentResult.Copy( KNSmlDMVoIPValueTrue );
                }
            if ( CRCSEAudioCodecEntry::EOff == 
                iCodecEntries[codecPos]->iOctetAlign )
                {
                aSegmentResult.Copy( KNSmlDMVoIPValueFalse );
                }
            }
        //ModeChangePeriod
        if ( KNSmlDMVoIPModeChangePeriod() == aUriSeg ) 
            {
            if ( KNotSet < iCodecEntries[codecPos]->
                iModeChangePeriod )
                {
                aSegmentResult.Num( iCodecEntries[codecPos]->
                    iModeChangePeriod );
                }
            }
        //ModeChangeNeighbor          
        if ( KNSmlDMVoIPModeChangeNeighbor() == aUriSeg ) 
            {
            if ( CRCSEAudioCodecEntry::EOn == 
                iCodecEntries[codecPos]->iModeChangeNeighbor )
                {
                aSegmentResult.Copy( KNSmlDMVoIPValueTrue );
                }
            if ( CRCSEAudioCodecEntry::EOff == 
                iCodecEntries[codecPos]->iModeChangeNeighbor )
                {
                aSegmentResult.Copy( KNSmlDMVoIPValueFalse );
                }
            }
        //Ptime
        if ( KNSmlDMVoIPPtime() == aUriSeg )  
            {
            if ( KNotSet < iCodecEntries[codecPos]->iPtime )
                {
                aSegmentResult.Num( iCodecEntries[codecPos]->iPtime );
                }
            }
        //Maxptime
        if ( KNSmlDMVoIPMaxPtime() == aUriSeg ) 
            {
            if ( KNotSet < iCodecEntries[codecPos]->iMaxptime )
                {
                aSegmentResult.Num( 
                    iCodecEntries[codecPos]->iMaxptime );
                }
            }
        //VAD                
        if ( KNSmlDMVoIPVAD() == aUriSeg ) 
            {
            if ( CRCSEAudioCodecEntry::EOn ==
                iCodecEntries[codecPos]->iVAD )
                {
                aSegmentResult.Copy( KNSmlDMVoIPValueTrue );
                }
            if ( CRCSEAudioCodecEntry::EOff == 
                iCodecEntries[codecPos]->iVAD )
                {
                aSegmentResult.Copy( KNSmlDMVoIPValueFalse );
                }
            }
        //AnnexB                
        if ( KNSmlDMVoIPAnnexB() == aUriSeg ) 
            {
            if ( CRCSEAudioCodecEntry::EOn == 
                iCodecEntries[codecPos]->iAnnexb )
                {
                aSegmentResult.Copy( KNSmlDMVoIPValueTrue );
                }
            if ( CRCSEAudioCodecEntry::EOff == 
                iCodecEntries[codecPos]->iAnnexb )
                {
                aSegmentResult.Copy( KNSmlDMVoIPValueFalse );
                }
            }
        // MaxRed
        if ( KNSmlDMVoIPMaxRed() == aUriSeg )
            {
            if ( CRCSEAudioCodecEntry::EOONotSet < 
                iCodecEntries[codecPos]->iMaxRed )                
                {
                aSegmentResult.Num( iCodecEntries[codecPos]->iMaxRed );
                }
            }

        // ==============================
        // ModeSet node ( fetch )
        // ==============================
        //
        if ( KNSmlDMVoIPMode() == aUriSeg )  
            {
            aSegmentResult.Num ( iCodecEntries[codecPos]->
                iModeSet[DesToInt( aLUID.Right( 10 ))] );
            }
        }
    else
        {
        // Codec not found even if LUID exists.
        retValue = CSmlDmAdapter::EError;    
        }   

    DBG_PRINT("CNSmlDmVoIPAdapter::FetchCodecObject(): begin");

    return retValue;
    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::ChildURIListL
// Inserts the list of children objects of the node to dm-module
// ---------------------------------------------------------------------------
//
void  CNSmlDmVoIPAdapter::ChildURIListL( 
    const TDesC8& aURI,
    const TDesC8& aLUID, 
    const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList, 
    const TInt aResultsRef, 
    const TInt aStatusRef )
    {
    DBG_PRINT( "CNSmlDmVoIPAdapter::ChildURIListL(): start" );

    __ASSERT_ALWAYS( iDmCallback != NULL, User::Leave( KErrArgument ) );

    // Load VoIP profile information if not loaded yet
    if ( !iProfileEntries.Count() )
        {
        LoadProfilesL();
        }

    CSmlDmAdapter::TError retValue = CSmlDmAdapter::EOk;

    CBufBase *currentURISegmentList = CBufFlat::NewL( 1 );
    CleanupStack::PushL( currentURISegmentList );

    // get URI parameters
    TBuf8<KNSmlVoIPMaxUriLength> uri( KNullDesC8 );
    TBuf8<KNSmlVoIPMaxUriLength> mappingInfo( KNullDesC8 );
    TPtrC8 uriTmp = RemoveLastURISeg( aURI );
    TPtrC8 secondLastUriSeg = LastURISeg( uriTmp );
    TBuf8<KSmlMaxURISegLen> segmentName;
    TInt howManyObjects = iProfileEntries.Count();

    iProfileID = FindProfileLocation( aLUID );

    // VoIP
    if ( KNSmlDMVoIPNodeName() == LastURISeg( aURI ) )   
        {
        for ( TInt counter = 0; counter < howManyObjects ; counter++ )
            {
            TBool notInList = ETrue;
            TInt currentline = 0;
            TUint ddId ( iProfileEntries[counter]->iId );
            while ( notInList && currentline < aPreviousURISegmentList.Count() )
                {
                TInt profileLocation = FindProfileLocation(
                    aPreviousURISegmentList.At( currentline ).iURISegLUID );
                if ( profileLocation != KErrNotFound )
                    {
                    TUint32 profileID( 
                        iProfileEntries[profileLocation]->iId );   
                    if ( profileID == ddId ) 
                        {
                        notInList = EFalse;
                        break;
                        }
                    }
                currentline++;
                }
            if ( notInList )
                {
                // VoIP/x
                segmentName.Copy( KNSmlDMVoIPPrefix );

                // Add profileID number to identify profile
                segmentName.AppendNumFixedWidthUC( 
                    iProfileEntries[counter]->iId, EDecimal, 10 );
                currentURISegmentList->InsertL( currentURISegmentList->
                    Size(), segmentName );
                currentURISegmentList->InsertL( currentURISegmentList->
                    Size(), KNSmlDMVoIPSeparator );

                // set luid mapping to dm-module
                mappingInfo.Copy ( KNSmlDMVoIPPrefix );
                TUint val ( iProfileEntries[counter]->iId );
                mappingInfo.AppendNumFixedWidthUC( val, EDecimal, 10 );
                uri.Copy( KNSmlDMVoIPNodeName );
                uri.Append ( KNSmlDMVoIPSeparator );
                uri.Append ( KNSmlDMVoIPPrefix );
                uri.AppendNumFixedWidthUC( val, EDecimal, 10 );
                iDmCallback->SetMappingL( uri, mappingInfo ); 
                }
            else
                { // Add those in PreviousList which are on database
                currentURISegmentList->InsertL( currentURISegmentList->Size(),
                    aPreviousURISegmentList.At(currentline).iURISeg );
                currentURISegmentList->InsertL( currentURISegmentList->Size(),
                     KNSmlDMVoIPSeparator );
                }
            }
        retValue = CSmlDmAdapter::EOk;
        iDmCallback->SetStatusL( aStatusRef, retValue );
        iDmCallback->SetResultsL( aResultsRef, *currentURISegmentList,
            KNullDesC8 );
        CleanupStack::PopAndDestroy( currentURISegmentList ); 
        currentURISegmentList = NULL;
        return;
        }

    if ( KErrNotFound == iProfileID )
        {
        retValue = CSmlDmAdapter::ENotFound;
        iDmCallback->SetStatusL( aStatusRef, retValue );
        CleanupStack::PopAndDestroy( currentURISegmentList ); 
        currentURISegmentList = NULL;
        return;
        }

    // VoIP/x/PreferredCodecs
    if ( KNSmlDMVoIPPreferredCodecsNode() == LastURISeg( aURI ) )   
        {
        TInt preferredCount = iProfileEntries[iProfileID]->
            iPreferredCodecs.Count();
        for ( TInt counter = 0; counter < preferredCount ; counter++ )
            { 
            TBool inList = EFalse;
            
            // We need to rebuild LUID mapping if someone other has added or
            // deleted codecs, because there is no usable link to each codec.
            if ( aPreviousURISegmentList.Count() == preferredCount )
                {
                inList = ETrue;    
                }
            if ( !inList )
                {
                segmentName.Copy( KNSmlDMVoIPPreferredCodecId );
                segmentName.AppendNumFixedWidthUC( counter + 1, EDecimal, 3 );
                currentURISegmentList->InsertL( currentURISegmentList->
                    Size(), segmentName );
                currentURISegmentList->InsertL( currentURISegmentList->
                    Size(), KNSmlDMVoIPSeparator );

                // Set LUID mapping to dm-module.
                mappingInfo.Copy ( aLUID ); 
                mappingInfo.Append ( KNSmlDMVoIPSeparator );
                mappingInfo.Append ( KNSmlDMVoIPPreferredCodecId );
                mappingInfo.AppendNumFixedWidthUC( counter, EDecimal, 10 );
                uri.Copy( aURI );
                uri.Append ( KNSmlDMVoIPSeparator );
                uri.Append ( KNSmlDMVoIPPreferredCodecId );
                uri.AppendNumFixedWidthUC( counter + 1, EDecimal, 3 );
                iDmCallback->SetMappingL( uri, mappingInfo ); 
                }
            else
                { // Add those in PreviousList which are on database
                currentURISegmentList->InsertL( currentURISegmentList->Size(),
                    aPreviousURISegmentList.At( counter ).iURISeg );
                currentURISegmentList->InsertL( currentURISegmentList->Size(),
                     KNSmlDMVoIPSeparator );
                }
            }
        }

    // VoIP/x/PreferredCodecs/x/
    else if ( KNSmlDMVoIPPreferredCodecsNode() == secondLastUriSeg
        && KFourSegs == NumOfURISegs( aURI ) )
        {
        segmentName.Copy( KNSmlDMVoIPPreferredCodecId );
        currentURISegmentList->InsertL( currentURISegmentList->
                Size(), segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->
                Size(), KNSmlDMVoIPSeparator );        
        }

    // VoIP/x/
    else if ( KNSmlDMVoIPCodecSettingsNode() == LastURISeg( aURI ) )   
        {
        for ( TInt counter = 0; 
            counter < iProfileEntries[iProfileID]->iPreferredCodecs.Count(); 
            counter++ ) 
            { 
            TBool notInList = ETrue;
            TInt currentline = 0;
            TUint32 ddId = iProfileEntries[iProfileID]->
                iPreferredCodecs[counter];
            while ( notInList && currentline < 
                aPreviousURISegmentList.Count() )
                {
                if ( DesToInt( aPreviousURISegmentList.At( currentline ).
                    iURISegLUID.Right( 10 ) ) == ddId ) 
                    {
                    notInList = EFalse;
                    break;
                    }
                currentline++;
                }
            if ( notInList )
                {
                segmentName.Copy( KNSmlDMVoIPCodecSettingsId );
                segmentName.AppendNumFixedWidthUC( counter + 1, EDecimal, 3 );
                currentURISegmentList->InsertL( currentURISegmentList->
                    Size(), segmentName );
                currentURISegmentList->InsertL( currentURISegmentList->
                    Size(), KNSmlDMVoIPSeparator );

                // Set LUID mapping to dm-module.
                mappingInfo.Copy ( aLUID ); 
                mappingInfo.Append ( KNSmlDMVoIPSeparator );
                mappingInfo.Append ( KNSmlDMVoIPCodecSettingsId );
                mappingInfo.AppendNumFixedWidthUC( ddId, EDecimal, 10 );
                uri.Copy( aURI );
                uri.Append ( KNSmlDMVoIPSeparator );
                uri.Append ( KNSmlDMVoIPCodecSettingsId );
                uri.AppendNumFixedWidthUC( counter + 1, EDecimal, 3 );
                iDmCallback->SetMappingL( uri, mappingInfo ); 
                }
            else
                { // Add those from PreviousList which are on database
                currentURISegmentList->InsertL( currentURISegmentList->Size(),
                    aPreviousURISegmentList.At(currentline).iURISeg );
                currentURISegmentList->InsertL( currentURISegmentList->Size(),
                     KNSmlDMVoIPSeparator );
                }
            }
        }

    // ==============================
    // CodecSettings node
    // ==============================
    //
    else if ( KNSmlDMVoIPCodecSettingsNode() == secondLastUriSeg 
        && KFourSegs == NumOfURISegs( aURI ) )
        {
        CodecChildListL( currentURISegmentList );
        }

    // ==============================
    // ModeSet node
    // ==============================
    //
    else if ( KNSmlDMVoIPModeSetNode() == LastURISeg( aURI ) )   
        {
        TUint32 codecId;
        codecId = DesToInt( aLUID.Right( 10 ) );
        TInt codecPos = FindCodecLocation( codecId );    
        if ( KErrNotFound != codecPos )
            {
            TInt modesetCount = iCodecEntries[codecPos]->iModeSet.Count();
            for ( TInt counter = 0; counter < modesetCount; counter++ )
                { 
                TBool inList = EFalse;

                // We need to rebuild LUID mapping if someone other has added
                // or deleted, because there is no usable link to each ModeSet.
                if ( aPreviousURISegmentList.Count() == modesetCount )
                    {
                    inList = ETrue;    
                    }
                if ( !inList )
                    {
                    segmentName.Copy( KNSmlDMVoIPModeId );
                    segmentName.AppendNumFixedWidthUC( counter + 1, EDecimal,
                        3 );
                    currentURISegmentList->InsertL( currentURISegmentList->
                        Size(), segmentName );
                    currentURISegmentList->InsertL( currentURISegmentList->
                        Size(), KNSmlDMVoIPSeparator );

                    // Set LUID mapping to DM module.
                    mappingInfo.Copy ( aLUID );
                    mappingInfo.Append ( KNSmlDMVoIPSeparator );
                    mappingInfo.Append ( KNSmlDMVoIPModeId );
                    mappingInfo.AppendNumFixedWidthUC( counter, EDecimal, 10 );
                    uri.Copy( aURI );
                    uri.Append ( KNSmlDMVoIPSeparator );
                    uri.Append ( KNSmlDMVoIPModeId );
                    uri.AppendNumFixedWidthUC( counter + 1, EDecimal, 3 );
                    iDmCallback->SetMappingL( uri, mappingInfo ); 
                    }
                else
                    { // Add those in PreviousList which are on database.
                    currentURISegmentList->InsertL( currentURISegmentList->
                        Size(), aPreviousURISegmentList.At( counter ).iURISeg );
                    currentURISegmentList->InsertL( currentURISegmentList->
                        Size(), KNSmlDMVoIPSeparator );
                    }
                }
            }
        else
            {
            retValue = CSmlDmAdapter::EError;    
            }    
        
        }       

    else if ( KNSmlDMVoIPModeSetNode() == secondLastUriSeg 
        && KSixSegs == NumOfURISegs( aURI ) )
        {
        segmentName.Copy( KNSmlDMVoIPMode );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMVoIPSeparator );
        }       

    // ==============================
    // SettingIds node
    // ==============================
    //
    // VoIP/x/SettingIds
    else if ( KNSmlDMVoIPSettingIdsNode() == LastURISeg( aURI ) )    
        {
        for ( TInt counter = 0; counter < 
            iProfileEntries[iProfileID]->iIds.Count(); counter++ )
            { 
            TBool inList = EFalse;

            // We need to rebuild luid mapping if someone other has made
            // some changes because there is no usable link to settingids.
            if ( aPreviousURISegmentList.Count() == 
                iProfileEntries[iProfileID]->iIds.Count() )
                {
                inList = ETrue;    
                }
            if ( !inList )
                {
                segmentName.Copy( KNSmlDMVoIPSettingId );
                segmentName.AppendNumFixedWidthUC( counter + 1, EDecimal, 3 );
                currentURISegmentList->InsertL( currentURISegmentList->
                    Size(), segmentName );
                currentURISegmentList->InsertL( currentURISegmentList->
                    Size(), KNSmlDMVoIPSeparator );

                // set luid mapping to dm-module
                mappingInfo.Copy ( aLUID );     
                mappingInfo.Append ( KNSmlDMVoIPSeparator );
                mappingInfo.Append ( KNSmlDMVoIPSettingId );
                mappingInfo.AppendNumFixedWidthUC( counter, EDecimal, 10 );
                uri.Copy( aURI );
                uri.Append ( KNSmlDMVoIPSeparator );
                uri.Append ( KNSmlDMVoIPSettingId );
                uri.AppendNumFixedWidthUC( counter + 1, EDecimal, 3 );
                iDmCallback->SetMappingL( uri, mappingInfo ); 
                }
            else
                { // Add those in PreviousList which are on database
                currentURISegmentList->InsertL( currentURISegmentList->Size(),
                    aPreviousURISegmentList.At( counter ).iURISeg );
                currentURISegmentList->InsertL( currentURISegmentList->Size(),
                     KNSmlDMVoIPSeparator );
                }
            }
        }  

    // ==============================
    // SettingId leaf
    // ==============================
    //
    else if ( KNSmlDMVoIPSettingIdsNode() == secondLastUriSeg
        && KFourSegs == NumOfURISegs( aURI ) )
        {
        // ProfileType
        segmentName.Copy( KNSmlDMVoIPProfileType );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMVoIPSeparator );

        // ProfileId
        segmentName.Copy( KNSmlDMVoIPProfileId );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMVoIPSeparator );

        // ProfileSpecific
        segmentName.Copy( KNSmlDMVoIPProfileSpecificId );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMVoIPSeparator );
        }  

    // ==============================
    // VoIP/x/VoiceMailBox node
    // ==============================
    //
    else if ( KNSmlDMVoIPVmbxNode() == LastURISeg( aURI ) && 
        KThreeSegs == NumOfURISegs( aURI ) )
        {
        // VoIP/x/VoiceMailBox/MWI-URI
        segmentName.Copy( KNSmlDMVoIPMwiUri );
        currentURISegmentList->InsertL( currentURISegmentList->Size(),
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMVoIPSeparator );
        // VoIP/x/VoiceMailBox/ListeningURI
        segmentName.Copy( KNSmlDMVoIPListeningUri );
        currentURISegmentList->InsertL( currentURISegmentList->Size(),
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMVoIPSeparator );
        // VoIP/x/VoiceMailBox/UsedIAPId
        segmentName.Copy( KNSmlDMVoIPUsedIAPId );
        currentURISegmentList->InsertL( currentURISegmentList->Size(),
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMVoIPSeparator );
        // VoIP/x/VoiceMailBox/ReSubscribeInterval
        segmentName.Copy( KNSmlDMVoIPReSubscribeInterval );
        currentURISegmentList->InsertL( currentURISegmentList->Size(),
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMVoIPSeparator );
        // VoIP/x/VoiceMailBox/UsedSIPProfileId
        segmentName.Copy( KNSmlDMVoIPUsedSIPProfileId );
        currentURISegmentList->InsertL( currentURISegmentList->Size(),
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMVoIPSeparator );
        }

    // ==============================
    // VoIP/x/ node
    // ==============================
    //
    else if ( KNSmlDMVoIPNodeName() == secondLastUriSeg 
        && KTwoSegs == NumOfURISegs( aURI ) )
        {
        VoipChildListL( currentURISegmentList );
        }
    else 
        {
        // if none of asked nodes found return error.
        retValue = CSmlDmAdapter::ENotFound;
        }
    iDmCallback->SetStatusL( aStatusRef, retValue );
    if ( CSmlDmAdapter::EOk == retValue )
        {
        iDmCallback->SetResultsL( aResultsRef, *currentURISegmentList, 
            KNullDesC8 );
        }
    CleanupStack::PopAndDestroy( currentURISegmentList ); 
    currentURISegmentList = NULL;
    DBG_PRINT("CNSmlDmVoIPAdapter::ChildURIListL(): end");
    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::CodecChildListL
// Inserts Codec specific leaf objects.
// ---------------------------------------------------------------------------
//
void CNSmlDmVoIPAdapter::CodecChildListL(
    CBufBase* const aCurrentURISegmentList )
    {
    DBG_PRINT("CNSmlDmVoIPAdapter::CodecChildListL(): start");

    TBuf8<KSmlMaxURISegLen> segmentName;

    // VoIP/x/CodecSettings/CodecSettingsIdXXX/VoIPCodecId
    segmentName.Copy( KNSmlDMVoIPCodecId );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        segmentName );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        KNSmlDMVoIPSeparator );

    // VoIP/x/CodecSettings/CodecSettingsIdXXX/MediaTypeName
    segmentName.Copy( KNSmlDMVoIPMediaTypeName );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        segmentName );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        KNSmlDMVoIPSeparator );

    // VoIP/x/CodecSettings/CodecSettingsIdXXX/MediaSubTypeName
    segmentName.Copy( KNSmlDMVoIPMediaSubTypeName );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        segmentName );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        KNSmlDMVoIPSeparator );

    // VoIP/x/CodecSettings/CodecSettingsIdXXX/JitterBufferSize
    segmentName.Copy( KNSmlDMVoIPJitterBufferSize );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        segmentName );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        KNSmlDMVoIPSeparator );

    // VoIP/x/CodecSettings/CodecSettingsIdXXX/OctetAlign
    segmentName.Copy( KNSmlDMVoIPOctetAlign );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        segmentName );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        KNSmlDMVoIPSeparator );

    // VoIP/x/CodecSettings/CodecSettingsIdXXX/ModeSet
    segmentName.Copy( KNSmlDMVoIPModeSetNode );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        segmentName );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        KNSmlDMVoIPSeparator );

    // VoIP/x/CodecSettings/CodecSettingsIdXXX/ModeChangePeriod
    segmentName.Copy( KNSmlDMVoIPModeChangePeriod );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        segmentName );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        KNSmlDMVoIPSeparator );

    // VoIP/x/CodecSettings/CodecSettingsIdXXX/ModeChangeNeighbor
    segmentName.Copy( KNSmlDMVoIPModeChangeNeighbor );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        segmentName );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        KNSmlDMVoIPSeparator );

    // VoIP/x/CodecSettings/CodecSettingsIdXXX/Ptime
    segmentName.Copy( KNSmlDMVoIPPtime );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        segmentName );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        KNSmlDMVoIPSeparator );

    // VoIP/x/CodecSettings/CodecSettingsIdXXX/MaxPtime
    segmentName.Copy( KNSmlDMVoIPMaxPtime );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        segmentName );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        KNSmlDMVoIPSeparator );

    // VoIP/x/CodecSettings/CodecSettingsIdXXX/VAD
    segmentName.Copy( KNSmlDMVoIPVAD );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        segmentName );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        KNSmlDMVoIPSeparator );

    // VoIP/x/CodecSettings/CodecSettingsIdXXX/AnnexB
    segmentName.Copy( KNSmlDMVoIPAnnexB );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        segmentName );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(),
        KNSmlDMVoIPSeparator );

    // VoIP/x/CodecSettings/CodecSettingsIdXXX/MaxRed
    segmentName.Copy( KNSmlDMVoIPMaxRed );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(),
        segmentName );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(),
        KNSmlDMVoIPSeparator );

    DBG_PRINT("CNSmlDmVoIPAdapter::CodecChildListL(): end");
    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::VoipChildListL
// Inserts VoIP leaf objects.
// ---------------------------------------------------------------------------
//
void CNSmlDmVoIPAdapter::VoipChildListL(
    CBufBase* const aCurrentURISegmentList )
    {
    DBG_PRINT("CNSmlDmVoIPAdapter::VoipChildListL(): start");

    TBuf8<KSmlMaxURISegLen> segmentName;

    // VoIP/x/VoIPID
    segmentName.Copy( KNSmlDMVoIPSetID );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(),
        segmentName );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        KNSmlDMVoIPSeparator );

    // VoIP/x/ProviderName
    segmentName.Copy( KNSmlDMVoIPProvID );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        segmentName );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        KNSmlDMVoIPSeparator );

    // VoIP/x/SettingsName
    segmentName.Copy( KNSmlDMVoIPName );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        segmentName );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        KNSmlDMVoIPSeparator );

    // VoIP/x/PreferredCodecs
    segmentName.Copy( KNSmlDMVoIPPreferredCodecsNode );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        segmentName ); 
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        KNSmlDMVoIPSeparator ); 

    // VoIP/x/CodecSettings
    segmentName.Copy( KNSmlDMVoIPCodecSettingsNode );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        segmentName ); 
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        KNSmlDMVoIPSeparator ); 

    // VoIP/x/SettingIds
    segmentName.Copy( KNSmlDMVoIPSettingIdsNode );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        segmentName ); 
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        KNSmlDMVoIPSeparator );

    // VoIP/x/StartMediaPort
    segmentName.Copy( KNSmlDMVoIPSMPort );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        segmentName ); 
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        KNSmlDMVoIPSeparator );

    // VoIP/x/EndMediaPort
    segmentName.Copy( KNSmlDMVoIPEMPort );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        segmentName ); 
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        KNSmlDMVoIPSeparator );

    // VoIP/x/VoiceMailBox
    segmentName.Copy( KNSmlDMVoIPVmbxNode );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        segmentName ); 
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        KNSmlDMVoIPSeparator );

    // VoIP/x/MegiaQos
    segmentName.Copy( KNSmlDMVoIPMediaQos );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        segmentName ); 
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        KNSmlDMVoIPSeparator );

    // VoIP/x/InbandDTMF
    segmentName.Copy( KNSmlDMVoIPDTMFIB );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        segmentName ); 
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        KNSmlDMVoIPSeparator );

    // VoIP/x/OutbandDTMF
    segmentName.Copy( KNSmlDMVoIPDTMFOB );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        segmentName ); 
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        KNSmlDMVoIPSeparator );

    // VoIP/x/SecureCallPref
    segmentName.Copy( KNSmlDMVoIPSecureCallPref );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        segmentName );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        KNSmlDMVoIPSeparator );

    // VoIP/x/RTCP
    segmentName.Copy( KNSmlDMVoIPRTCP );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        segmentName );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        KNSmlDMVoIPSeparator );

    // VoIP/x/UAHTerminalType
    segmentName.Copy( KNSmlDMVoIPUAHTerminalType );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        segmentName );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        KNSmlDMVoIPSeparator );

    // VoIP/x/UAHWLANMAC
    segmentName.Copy( KNSmlDMVoIPUAHWLANMAC );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        segmentName );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        KNSmlDMVoIPSeparator );

    // VoIP/x/UAHString
    segmentName.Copy( KNSmlDMVoIPUAHString );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        segmentName );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        KNSmlDMVoIPSeparator );

    // VoIP/x/ProfileLockedToIAP
    segmentName.Copy( KNSmlDMVoIPProfileLockedToIAP );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        segmentName );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        KNSmlDMVoIPSeparator );

    // VoIP/x/VoIPPluginUID
    segmentName.Copy( KNSmlDMVoIPPluginUID );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        segmentName );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        KNSmlDMVoIPSeparator );

    // VoIP/x/AllowVoIPoverWCDMA
    segmentName.Copy( KNSmlDMVoIPAllowVoIPOverWCDMA );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        segmentName );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
        KNSmlDMVoIPSeparator );

    // VoIP/x/VoIPDigits
    segmentName.Copy( KNSmlDMVoIPVoIPDigits );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(),
        segmentName );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(),
        KNSmlDMVoIPSeparator );

    // VoIP/x/URIDomainIgnoreRule
    segmentName.Copy( KNSmlDMVoIPURIDomainIgnoreRule );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(),
        segmentName );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(),
        KNSmlDMVoIPSeparator );

    // VoIP/x/AutoAcceptBuddyRequest
    segmentName.Copy( KNSmlDMVoIPAutoAcceptBuddyRequest );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(),
        segmentName );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(),
        KNSmlDMVoIPSeparator );

    // VoIP/x/UsedVoIPRelease
    segmentName.Copy( KNSmlDMVoIPUsedVoIPRelease );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(),
        segmentName );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(),
        KNSmlDMVoIPSeparator );

    // VoIP/x/AddUserPhone
    segmentName.Copy( KNSmlDMVoIPAddUserPhone );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(),
        segmentName );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(),
        KNSmlDMVoIPSeparator );

    // VoIP/x/ProviderBookmarkURI
    segmentName.Copy( KNSmlDMVoIPProviderBookmarkUri );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(),
        segmentName );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(),
        KNSmlDMVoIPSeparator );

    // VoIP/x/SIPConnectivityTestURI
    segmentName.Copy( KNSmlDMVoIPSIPConnTestUri );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(),
        segmentName );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(),
        KNSmlDMVoIPSeparator );

    // VoIP/x/NATFWProfileId
    segmentName.Copy( KNSmlDMVoIPNatFwProfileId );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(),
        segmentName );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(),
        KNSmlDMVoIPSeparator );

    // VoIP/x/MinSessionInterval
    segmentName.Copy( KNSmlDMVoIPMinSessionInterval );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(),
        segmentName );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(),
        KNSmlDMVoIPSeparator );

    // VoIP/x/SessionExpires
    segmentName.Copy( KNSmlDMVoIPSessionExpires );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(),
        segmentName );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(),
        KNSmlDMVoIPSeparator );

    // VoIP/x/BradingDataURI
    segmentName.Copy( KNSmlDMVoIPBrandingDataUri );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(),
        segmentName );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(),
        KNSmlDMVoIPSeparator );

    // VoIP/x/PresenceSettingsId
    segmentName.Copy( KNSmlDMVoIPPresenceSettingsId );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(),
        segmentName );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(),
        KNSmlDMVoIPSeparator );

    // VoIP/x/UsedNATProtocol
    segmentName.Copy( KNSmlDMVoIPUsedNatProtocol );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(),
        segmentName );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(),
        KNSmlDMVoIPSeparator );

    // VoIP/x/AutoEnable
    segmentName.Copy( KNSmlDMVoIPAutoEnable );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(),
        segmentName );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(),
        KNSmlDMVoIPSeparator );

    // VoIP/x/SnapProfileId
    segmentName.Copy( KNSmlDMVoIPSnapProfileId );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(),
        segmentName );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(),
        KNSmlDMVoIPSeparator );

    // VoIP/x/EnableIM
    segmentName.Copy( KNSmlDMVoIPEnableIm );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(),
        segmentName );
    aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(),
        KNSmlDMVoIPSeparator );

    DBG_PRINT("CNSmlDmVoIPAdapter::VoipChildListL(): end");
    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::AddNodeObjectL
// Adds a new node.
// ---------------------------------------------------------------------------
//
void CNSmlDmVoIPAdapter::AddNodeObjectL( 
    const TDesC8& aURI, 
    const TDesC8& aParentLUID, 
    const TInt aStatusRef )
    {
    DBG_PRINT("CNSmlDmVoIPAdapter::AddNodeObjectL(): begin");

    __ASSERT_ALWAYS( iDmCallback != NULL, User::Leave( KErrArgument ) );

    CSmlDmAdapter::TError retValue = EOk;

    // Load VoIP profile information if not loaded yet
    if ( !iProfileEntries.Count() )
        {
        LoadProfilesL();
        }
    // Find profile from array, profile deleted from elsewhere if not found
    if ( aParentLUID.Length() > 0 )
        {
        TInt profileID = FindProfileLocation ( aParentLUID );
        if ( KErrNotFound == profileID )
            {
            retValue = CSmlDmAdapter::ENotFound;
            }
        else
            {
            retValue = CSmlDmAdapter::EAlreadyExists;
            }
        }

    // Get last URI segment. 
    TBuf8<KNSmlVoIPMaxUriLength> uri = aURI;
    TBuf8<KNSmlVoIPMaxUriLength> mappingInfo ( KNullDesC8 );
    TPtrC8 uriSeg = LastURISeg( aURI );
    TPtrC8 uriTmp = RemoveLastURISeg( aURI );
    TPtrC8 secondLastUriSeg = LastURISeg( uriTmp );

     // Find profile
    iProfileID = FindProfileLocation( aParentLUID );

    // ==============================
    // VoIP node ( add )
    // ==============================
    //
    // VoIP/x/VoIP
    if ( KNSmlDMVoIPNodeName() == secondLastUriSeg 
        && ( EOk == retValue || ENotFound == retValue ) )
        {
        // Check OOD before saving new profile.
        RFs fs;
        User::LeaveIfError( fs.Connect() );
        CleanupClosePushL( fs ); // CS:1

        // VoIP settings are stored in three files.
        if ( SysUtil::FFSSpaceBelowCriticalLevelL( 
            &fs, KNSmlDMFatMinimumFileSize * 3 ) )
            {
            // Prevent writing in OOD.
            CleanupStack::PopAndDestroy( &fs ); // CS:0
            iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EDiskFull );
            DBG_PRINT("CNSmlDmVoIPAdapter::AddNodeObjectL(): DISK FULL end");
            return;
            }
        CleanupStack::PopAndDestroy( &fs ); // CS:0
        CRCSEProfileEntry* newProfileEntry = CRCSEProfileEntry::NewLC();
        iCRCSEProfile->GetDefaultProfile( *newProfileEntry );
        if ( uriSeg.Length() < KMaxSettingsNameLength )
            {
            newProfileEntry->iSettingsName.Copy( uriSeg );
            }
        newProfileEntry->iProviderName.Copy( KDefaultSettingsName );
        CheckDuplicateNameL( *newProfileEntry );
        CheckDuplicateProviderL( *newProfileEntry );

        CRCSEAudioCodecEntry* newAudioCodecEntry1 = 
            CRCSEAudioCodecEntry::NewLC();
        newAudioCodecEntry1->SetDefaultCodecValueSet( KAudioCodecAMRWB() );
        newAudioCodecEntry1->iMediaSubTypeName = KAudioCodecAMRWB;
        TUint32 codecId = iCRCSEAudioCodec->AddL( *newAudioCodecEntry1 );
        newProfileEntry->iPreferredCodecs.Append( codecId );
        newAudioCodecEntry1->iCodecId = codecId;
        iCodecEntries.AppendL( newAudioCodecEntry1 );
        CleanupStack::Pop( newAudioCodecEntry1 );

        CRCSEAudioCodecEntry* newAudioCodecEntry2 = 
            CRCSEAudioCodecEntry::NewLC();
        newAudioCodecEntry2->SetDefaultCodecValueSet( KAudioCodecAMR() );
        newAudioCodecEntry2->iMediaSubTypeName = KAudioCodecAMR;        
        codecId = iCRCSEAudioCodec->AddL( *newAudioCodecEntry2 );
        newProfileEntry->iPreferredCodecs.Append( codecId );
        newAudioCodecEntry2->iCodecId = codecId; 
        iCodecEntries.AppendL( newAudioCodecEntry2 );
        CleanupStack::Pop( newAudioCodecEntry2 );

        CRCSEAudioCodecEntry* newAudioCodecEntry3 = 
            CRCSEAudioCodecEntry::NewLC();
        newAudioCodecEntry3->SetDefaultCodecValueSet( KAudioCodecPCMU() );
        newAudioCodecEntry3->iMediaSubTypeName = KAudioCodecPCMU;
        codecId = iCRCSEAudioCodec->AddL( *newAudioCodecEntry3 );
        newProfileEntry->iPreferredCodecs.Append( codecId );
        newAudioCodecEntry3->iCodecId = codecId;
        iCodecEntries.AppendL( newAudioCodecEntry3 );
        CleanupStack::Pop( newAudioCodecEntry3 );

        CRCSEAudioCodecEntry* newAudioCodecEntry4 = 
            CRCSEAudioCodecEntry::NewLC();
        newAudioCodecEntry4->SetDefaultCodecValueSet( KAudioCodecPCMA() );
        newAudioCodecEntry4->iMediaSubTypeName = KAudioCodecPCMA;
        codecId = iCRCSEAudioCodec->AddL( *newAudioCodecEntry4 );
        newProfileEntry->iPreferredCodecs.Append( codecId );
        newAudioCodecEntry4->iCodecId = codecId; 
        iCodecEntries.AppendL( newAudioCodecEntry4 );
        CleanupStack::Pop( newAudioCodecEntry4 );

        CRCSEAudioCodecEntry* newAudioCodecEntry5 = 
            CRCSEAudioCodecEntry::NewLC();
        newAudioCodecEntry5->SetDefaultCodecValueSet( KAudioCodeciLBC() );
        newAudioCodecEntry5->iMediaSubTypeName = KAudioCodeciLBC;
        codecId = iCRCSEAudioCodec->AddL( *newAudioCodecEntry5 );
        newProfileEntry->iPreferredCodecs.Append( codecId );
        newAudioCodecEntry5->iCodecId = codecId; 
        iCodecEntries.AppendL( newAudioCodecEntry5 );
        CleanupStack::Pop( newAudioCodecEntry5 );

        CRCSEAudioCodecEntry* newAudioCodecEntry6 = 
            CRCSEAudioCodecEntry::NewLC();
        newAudioCodecEntry6->SetDefaultCodecValueSet( KAudioCodecG729() );
        newAudioCodecEntry6->iMediaSubTypeName = KAudioCodecG729;
        codecId = iCRCSEAudioCodec->AddL( *newAudioCodecEntry6 );
        newProfileEntry->iPreferredCodecs.Append( codecId );
        newAudioCodecEntry6->iCodecId = codecId;
        iCodecEntries.AppendL( newAudioCodecEntry6 );
        CleanupStack::Pop( newAudioCodecEntry6 );

        CRCSEAudioCodecEntry* newAudioCodecEntry7 = 
            CRCSEAudioCodecEntry::NewLC();
        newAudioCodecEntry7->SetDefaultCodecValueSet( KAudioCodecCN() );
        newAudioCodecEntry7->iMediaSubTypeName = KAudioCodecCN;
        codecId = iCRCSEAudioCodec->AddL( *newAudioCodecEntry7 );
        newProfileEntry->iPreferredCodecs.Append( codecId );
        newAudioCodecEntry7->iCodecId = codecId;
        iCodecEntries.AppendL( newAudioCodecEntry7 );
        CleanupStack::Pop( newAudioCodecEntry7 );

        // Add SettingIds entry.
        TSettingIds idEntry;
        idEntry.iProfileType = -1;
        // SIPSpecific not supported => ID not found.
        idEntry.iProfileSpecificSettingId = KErrNotFound;
        idEntry.iProfileId = -1;
        newProfileEntry->iIds.Append( idEntry ); 

        // Add new VoIP profile.
        TUint32 profileID = iCRCSEProfile->AddL( *newProfileEntry );
        newProfileEntry->iId = profileID;
        iProfileEntries.AppendL( newProfileEntry );

        // Add LUID mapping to first SettingIds of profile.
        uri.Copy( aURI );
        uri.Append( KNSmlDMVoIPSeparator );
        uri.Append( KNSmlDMVoIPSettingIdsNode );
        uri.Append( KNSmlDMVoIPSeparator );
        uri.Append( KNSmlDMVoIPSettingId );
        uri.AppendNumFixedWidthUC( 1, EDecimal, 3 );
        mappingInfo.Copy( KNSmlDMVoIPPrefix );
        mappingInfo.AppendNumFixedWidthUC( profileID, EDecimal, 10 );
        mappingInfo.Append( KNSmlDMVoIPSeparator );
        mappingInfo.Append( KNSmlDMVoIPSettingId );
        mappingInfo.AppendNumFixedWidthUC( 0, EDecimal, 10 );
        iDmCallback->SetMappingL( uri, mappingInfo ); 

        // Add LUID mapping for CodecSettings and preferredCodecs.
        for ( TInt counter = 0; counter < newProfileEntry->
            iPreferredCodecs.Count(); counter++ )
            {
            uri.Copy( aURI );
            uri.Append( KNSmlDMVoIPSeparator );
            uri.Append( KNSmlDMVoIPCodecSettingsNode );
            uri.Append( KNSmlDMVoIPSeparator );
            uri.Append( KNSmlDMVoIPCodecSettingsId );
            uri.AppendNumFixedWidthUC( counter + 1, EDecimal, 3 );
            mappingInfo.Copy( KNSmlDMVoIPPrefix );
            mappingInfo.AppendNumFixedWidthUC( profileID, EDecimal, 10 );
            mappingInfo.Append( KNSmlDMVoIPSeparator );
            mappingInfo.Append( KNSmlDMVoIPCodecSettingsId );
            mappingInfo.AppendNumFixedWidthUC( newProfileEntry->
                iPreferredCodecs[counter], EDecimal, 10 );
            iDmCallback->SetMappingL( uri, mappingInfo ); 

            uri.Copy( aURI );
            uri.Append( KNSmlDMVoIPSeparator );
            uri.Append( KNSmlDMVoIPPreferredCodecsNode );
            uri.Append( KNSmlDMVoIPSeparator );
            uri.Append( KNSmlDMVoIPPreferredCodecId );
            uri.AppendNumFixedWidthUC( counter + 1, EDecimal, 3 );
            mappingInfo.Copy( KNSmlDMVoIPPrefix );
            mappingInfo.AppendNumFixedWidthUC( profileID, EDecimal, 10 );
            mappingInfo.Append( KNSmlDMVoIPSeparator );
            mappingInfo.Append( KNSmlDMVoIPPreferredCodecId );
            mappingInfo.AppendNumFixedWidthUC( counter, EDecimal, 10 );
            iDmCallback->SetMappingL( uri, mappingInfo ); 
            }

        // Set LUID mapping to DM module.
        mappingInfo.Copy ( KNSmlDMVoIPPrefix );
        mappingInfo.AppendNumFixedWidthUC( profileID, EDecimal, 10 );
        iDmCallback->SetMappingL( aURI, mappingInfo ); 

        CleanupStack::Pop( newProfileEntry );

        retValue = CSmlDmAdapter::EOk;
        }

    if ( KErrNotFound != iProfileID )
        {
        // ==============================
        // CodecSettings node (add)
        // ==============================
        //
        if ( KNSmlDMVoIPCodecSettingsNode() == secondLastUriSeg &&
            KErrNotFound == aParentLUID.Find( KNSmlDMVoIPCodecSettingsId ) )
            {
            CRCSEAudioCodecEntry *newAudioCodecEntry = 
                CRCSEAudioCodecEntry::NewLC();
            iCRCSEAudioCodec->GetDefaultCodec( *newAudioCodecEntry );
            TUint32 codecId = iCRCSEAudioCodec->AddL( *newAudioCodecEntry );
            newAudioCodecEntry->iCodecId = codecId;
            iCodecEntries.AppendL( newAudioCodecEntry );

            // Add new codec ID to iPreferredCodecs of current VoIP profile.
            iProfileEntries[iProfileID]->
                iPreferredCodecs.Append( codecId );

            // Update VoIP profile back to permanent store.    
            iCRCSEProfile->UpdateL( iProfileEntries[iProfileID]->
                iId, *iProfileEntries[iProfileID] );
            CleanupStack::Pop( newAudioCodecEntry );

            // Add LUID mapping to new codecsettings entry.
            mappingInfo.Copy( KNSmlDMVoIPPrefix );
            TUint val( iProfileEntries[iProfileID]->iId );
            mappingInfo.AppendNumFixedWidthUC( val, EDecimal, 10 );
            mappingInfo.Append( KNSmlDMVoIPSeparator );
            mappingInfo.Append( KNSmlDMVoIPCodecSettingsId );
            mappingInfo.AppendNumFixedWidthUC( codecId, EDecimal, 10 );
            iDmCallback->SetMappingL( aURI, mappingInfo );

            // Add LUID mapping to new preferredcodecs entry.
            uri.Copy( RemoveLastURISeg( RemoveLastURISeg( aURI ) ) );
            uri.Append( KNSmlDMVoIPSeparator );
            uri.Append( KNSmlDMVoIPPreferredCodecsNode );
            uri.Append( KNSmlDMVoIPSeparator );
            uri.Append( KNSmlDMVoIPPreferredCodecId );
            uri.AppendNumFixedWidthUC( 
                iProfileEntries[iProfileID]->iPreferredCodecs.Count(),
                EDecimal, 3 );
            mappingInfo.Copy( KNSmlDMVoIPPrefix );
            mappingInfo.AppendNumFixedWidthUC( iProfileID, EDecimal, 10 );
            mappingInfo.Append( KNSmlDMVoIPSeparator );
            mappingInfo.Append( KNSmlDMVoIPPreferredCodecId );
            mappingInfo.AppendNumFixedWidthUC( 
                iProfileEntries[iProfileID]->iPreferredCodecs.Count() - 1,
                EDecimal, 10 );
            iDmCallback->SetMappingL( uri, mappingInfo );    
            retValue = CSmlDmAdapter::EOk;
            }

        // ==============================
        // ModeSet node (add)
        // ==============================
        //
        if ( KNSmlDMVoIPModeSetNode() == secondLastUriSeg
            && KErrNotFound == aParentLUID.Find( KNSmlDMVoIPModeId ) )
            {
            // Get codec ID used by these settings.
            TUint32 codecId = DesToInt( aParentLUID.Right( 10 ) );
            TInt codecPos = FindCodecLocation( codecId );
            if ( KErrNotFound != codecPos )
                {
                iCodecEntries[codecPos]->iModeSet.Append( 1 );
                iCRCSEAudioCodec->UpdateL( codecId,
                    *iCodecEntries[codecPos] );

                // All LUID mapping to new mode-set item.
                mappingInfo.Copy( aParentLUID );
                mappingInfo.Append( KNSmlDMVoIPSeparator );
                mappingInfo.Append( KNSmlDMVoIPModeId );
                mappingInfo.AppendNumFixedWidthUC( 
                    iCodecEntries[codecPos]->iModeSet.Count() - 1,
                    EDecimal, 10 );
                iDmCallback->SetMappingL( aURI, mappingInfo ); 
                retValue = CSmlDmAdapter::EOk;    
                }
            else
                {
                retValue = CSmlDmAdapter::EError;    
                }
            }

        // ==============================
        // SettingIds node (add)  
        // ==============================
        //
        if ( KNSmlDMVoIPSettingIdsNode() == secondLastUriSeg &&
            KErrNotFound == aParentLUID.Find( KNSmlDMVoIPSettingId ) )
            {
            TSettingIds idEntry;
            idEntry.iProfileType = 0;
            idEntry.iProfileSpecificSettingId = -1;
            idEntry.iProfileId = 0;
            iProfileEntries[iProfileID]->iIds.Append( idEntry ); 
            iCRCSEProfile->UpdateL( iProfileEntries[iProfileID]->iId, 
                *iProfileEntries[iProfileID] );
            mappingInfo.Copy( KNSmlDMVoIPPrefix );
            TUint val( iProfileEntries[iProfileID]->iId );
            mappingInfo.AppendNumFixedWidthUC( val, EDecimal, 10 );
            mappingInfo.Append( KNSmlDMVoIPSeparator );
            mappingInfo.Append( KNSmlDMVoIPSettingId );
            mappingInfo.AppendNumFixedWidthUC( 
                iProfileEntries[iProfileID]->iIds.Count() - 1, EDecimal, 10 );
            iDmCallback->SetMappingL( aURI, mappingInfo );       
            retValue = CSmlDmAdapter::EOk;
            }
        }

    // Return status ok for existing leaf nodes that do not need
    // to be added separately.    
    if ( aParentLUID.Length() > 0 && 
        retValue != CSmlDmAdapter::ENotFound &&
        ( KNSmlDMVoIPCodecSettingsNode() == uriSeg
        || KNSmlDMVoIPModeSetNode() == uriSeg
        || KNSmlDMVoIPSettingIdsNode() == uriSeg
        || KNSmlDMVoIPSettingIdsNode() == uriSeg
        || KNSmlDMVoIPVmbxNode() == uriSeg
        ) )
        {
        retValue = CSmlDmAdapter::EOk;   
        }
    iDmCallback->SetStatusL( aStatusRef, retValue );

    DBG_PRINT("CNSmlDmVoIPAdapter::AddNodeObjectL(): end");
    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::UpdateLeafObjectL 
// For streaming large object data.
// ---------------------------------------------------------------------------
//
void CNSmlDmVoIPAdapter::UpdateLeafObjectL( 
    const TDesC8& /*aURI*/, 
    const TDesC8& /*aLUID*/, 
    RWriteStream*& /*aStream*/, 
    const TDesC8& /*aType*/, 
    const TInt aStatusRef )
    {
    DBG_PRINT("CNSmlDmVoIPAdapter::UpdateLeafObjectL(): stream: begin");
    __ASSERT_ALWAYS( iDmCallback != NULL, User::Leave( KErrArgument ) );
    iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    DBG_PRINT("CNSmlDmVoIPAdapter::UpdateLeafObjectL(): stream: end");
    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::FetchLeafObjectSizeL
// Fetches leaf object size of aURI.
// ---------------------------------------------------------------------------
//
void CNSmlDmVoIPAdapter::FetchLeafObjectSizeL( 
    const TDesC8& aURI, 
    const TDesC8& aLUID, 
    const TDesC8& aType, 
    const TInt aResultsRef, 
    const TInt aStatusRef )
    {
    DBG_PRINT("CNSmlDmVoIPAdapter::FetchLeafObjectSizeL(): begin");

    __ASSERT_ALWAYS( iDmCallback != NULL, User::Leave( KErrArgument ) );

    CBufBase *object = CBufFlat::NewL( 1 );
    CleanupStack::PushL( object );
    CSmlDmAdapter::TError retValue = FetchObjectL( aURI, aLUID, *object );

    TInt objSizeInBytes = object->Size();
    TBuf8<16> stringObjSizeInBytes;
    stringObjSizeInBytes.Num( objSizeInBytes );
    object->Reset();
    object->InsertL( 0, stringObjSizeInBytes );

    iDmCallback->SetStatusL( aStatusRef, retValue );
    iDmCallback->SetResultsL( aResultsRef, *object, aType );
    CleanupStack::PopAndDestroy( object ); 
    object = NULL;
    DBG_PRINT("CNSmlDmVoIPAdapter::FetchLeafObjectSizeL(): end");
    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::ExecuteCommandL  
// ---------------------------------------------------------------------------
//
void CNSmlDmVoIPAdapter::ExecuteCommandL( 
    const TDesC8& /*aURI*/, 
    const TDesC8& /*aLUID*/, 
    const TDesC8& /*aArgument*/, 
    const TDesC8& /*aType*/, 
    const TInt aStatusRef )
    {
    DBG_PRINT("CNSmlDmVoIPAdapter::ExecuteCommandL(): begin");
    __ASSERT_ALWAYS( iDmCallback != NULL, User::Leave( KErrArgument ) );
    iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    DBG_PRINT("CNSmlDmVoIPAdapter::ExecuteCommandL(): end");
    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::ExecuteCommandL  
// ---------------------------------------------------------------------------
//
void CNSmlDmVoIPAdapter::ExecuteCommandL( 
    const TDesC8& /*aURI*/, 
    const TDesC8& /*aParentLUID*/, 
    RWriteStream*& /*aStream*/, 
    const TDesC8& /*aType*/, 
    const TInt aStatusRef )
    {
    DBG_PRINT("CNSmlDmVoIPAdapter::ExecuteCommandL(): stream: begin");
    __ASSERT_ALWAYS( iDmCallback != NULL, User::Leave( KErrArgument ) );
    iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    DBG_PRINT("CNSmlDmVoIPAdapter::ExecuteCommandL(): stream: end");
    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::CopyCommandL  
// ---------------------------------------------------------------------------
//
void CNSmlDmVoIPAdapter::CopyCommandL( 
    const TDesC8& /*aTargetURI*/, 
    const TDesC8& /*aTargetLUID*/, 
    const TDesC8& /*aSourceURI*/, 
    const TDesC8& /*aSourceLUID*/, 
    const TDesC8& /*aType*/, 
    TInt aStatusRef )
    {
    DBG_PRINT("CNSmlDmVoIPAdapter::CopyCommandL(): begin");
    __ASSERT_ALWAYS( iDmCallback != NULL, User::Leave( KErrArgument ) );
    iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    DBG_PRINT("CNSmlDmVoIPAdapter::CopyCommandL(): end");
    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::StartAtomicL
// For signaling adapter of beginning of the atomic section
// ---------------------------------------------------------------------------
//
void CNSmlDmVoIPAdapter::StartAtomicL()
    {
    DBG_PRINT("CNSmlDmVoIPAdapter::StartAtomicL(): begin");
    DBG_PRINT("CNSmlDmVoIPAdapter::StartAtomicL(): end");
    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::CommitAtomicL
// The CommitAtomicL call is issued only if all the commands in the Atomic were successful 
// ---------------------------------------------------------------------------
//
void CNSmlDmVoIPAdapter::CommitAtomicL()
    {
    DBG_PRINT("CNSmlDmVoIPAdapter::CommitAtomicL(): begin");
    DBG_PRINT("CNSmlDmVoIPAdapter::CommitAtomicL(): end");
    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::RollbackAtomicL
// For signaling adapter to roll back all atomic commands
// ---------------------------------------------------------------------------
//
void CNSmlDmVoIPAdapter::RollbackAtomicL()
    {
    DBG_PRINT("CNSmlDmVoIPAdapter::RollbackAtomicL(): begin");
    DBG_PRINT("CNSmlDmVoIPAdapter::RollbackAtomicL(): end");
    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::StreamingSupport
// For telling framework whether streaming large object data is supported
// ---------------------------------------------------------------------------
//
TBool CNSmlDmVoIPAdapter::StreamingSupport( TInt& /*aItemSize*/ )
    {
    DBG_PRINT("CNSmlDmVoIPAdapter::StreamingSupport(): begin");
    DBG_PRINT("CNSmlDmVoIPAdapter::StreamingSupport(): end");
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::StreamCommittedL()
// For telling adapter when streaming large object is finished.
// ---------------------------------------------------------------------------
//
void CNSmlDmVoIPAdapter::StreamCommittedL()
    {
    DBG_PRINT("CNSmlDmVoIPAdapter::StreamCommittedL(): begin");
    DBG_PRINT("CNSmlDmVoIPAdapter::StreamCommittedL(): end");
    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::CompleteOutstandingCmdsL()
// When all buffered dm commands are sent to adapter, set SIP User Agent 
// Header into SIPManagedProfile, and save all changes.
// ---------------------------------------------------------------------------
//
void CNSmlDmVoIPAdapter::CompleteOutstandingCmdsL()
    {
    DBG_PRINT("CNSmlDmVoIPAdapter::CompleteOutstandingCmdsL(): begin");

    // Save only profiles that have changed. If there are such profiles,
    // save changed profiles back to permanent store
    if ( iProfileEntries.Count() )
        {
        for ( TInt counter = 0; counter < iProfileModifs.Count(); counter++ )
            {
            for ( TInt index = 0; index < iProfileEntries[counter]->
                iIds.Count(); index++ )
                {
                if ( ( 0 == iProfileEntries[counter]->
                    iIds[index].iProfileType ) 
                    && iProfileEntries[counter]->
                        iIds[index].iProfileId > 1
                    && ( iProfileEntries[counter]->
                        iSIPVoIPUAHTerminalType ||
                        iProfileEntries[counter]->
                        iSIPVoIPUAHeaderWLANMAC ||
                        iProfileEntries[counter]->
                        iSIPVoIPUAHeaderString.Length() > 0 ) )
                    {
                    // Create objects that allow the creation of
                    // CSIPManagedProfile object.
                    CSIPProfile* sipProf = NULL;
                    CSIPProfileRegistryObserver* sipRegObs =
                        CSIPProfileRegistryObserver::NewLC(); // CS:1
                    CSIPManagedProfileRegistry* sipProfReg = 
                        CSIPManagedProfileRegistry::NewLC( 
                            *sipRegObs ); // CS:2
                    TRAPD( err, ( sipProf = sipProfReg->ProfileL( 
                        iProfileEntries[counter]->
                        iIds[index].iProfileId ) ) );
                    if ( KErrNone == err )
                        {
                        CSIPManagedProfile* sipManProf = 
                            static_cast<CSIPManagedProfile*>( sipProf );
                        CleanupStack::PushL( sipManProf ); // CS:3
                        // Compile the User Agent Header into a TBuf string.
                        TBuf8<KTempStringlength> userAgentHeader;
                        TBuf<KMaxTerminalTypeLength> tempTerminalType;
                        TBuf<KWlanMacAddressLength> tempWlanMac;
                        TBuf<KMaxSettingsLength32> tempFreeString;
                        tempFreeString = iProfileEntries[counter]->
                            iSIPVoIPUAHeaderString;

                        userAgentHeader.Append( KUserAgent );
                        userAgentHeader.Append( KColonMark );
                        userAgentHeader.Append( KSpaceMark );

                        if ( iProfileEntries[counter]->
                            iSIPVoIPUAHTerminalType )
                            {
                            GetTerminalTypeL( tempTerminalType );
                            userAgentHeader.Append( tempTerminalType );
                            userAgentHeader.Append( KSpaceMark );
                            }
                        if ( iProfileEntries[counter]->
                            iSIPVoIPUAHeaderWLANMAC )
                            {
                            GetWlanMacAddressL( tempWlanMac );
                            userAgentHeader.Append( tempWlanMac );
                            userAgentHeader.Append( KSpaceMark );
                            }
                        if ( tempFreeString.Length() > 0 )
                            {
                            userAgentHeader.Append( tempFreeString );
                            }

                        // Create an array for setting the user agent header.
                        // Granularity is 1 since only one item is appended
                        // into the array.
                        CDesC8ArrayFlat* uahArray = new ( ELeave ) 
                            CDesC8ArrayFlat( 1 );
        	            CleanupStack::PushL( uahArray ); // CS:4
        	            uahArray->AppendL( userAgentHeader );
                        if ( userAgentHeader.Length() > 0 )
                            {
                            User::LeaveIfError( 
                                sipManProf->SetParameter( 
                                KSIPHeaders, *uahArray ) );
                            }

                        sipProfReg->SaveL( *sipManProf );
                        uahArray->Reset();
                        // uahArray, sipManProf 
                        CleanupStack::PopAndDestroy( 2, sipManProf ); // CS:2
                        }// if
                    // sipProfReg, sipRegObs
                    CleanupStack::PopAndDestroy( 2, sipRegObs ); // CS:0
                    sipProf = NULL;
                    }// if
                }// for
            iCRCSEProfile->UpdateL( 
                iProfileEntries[iProfileModifs[counter]]->iId,
                *iProfileEntries[iProfileModifs[counter]] );
            }
        iProfileModifs.Reset();
        }

    if ( iCodecEntries.Count() )
        {
        for ( TInt counter = 0; counter < iCodecModifs.Count(); counter++ )
            {
            iCRCSEAudioCodec->UpdateL( iCodecEntries[iCodecModifs[counter]]->
                iCodecId, *iCodecEntries[iCodecModifs[counter]] );
            }
        iCodecModifs.Reset();
        }  

    // Since CRCSEProfileRegistry::UpdateL takes CRCSEProfileEntry as const,
    // the service ID'S in iProfileEntries array items have not changed.
    // Hence load all profiles again from CRCSEProfileRegistry.
    iCodecEntries.ResetAndDestroy();
    iProfileEntries.ResetAndDestroy();
    LoadProfilesL();


    const TInt spSettingsCount = iSPSettings.Count();
    if ( spSettingsCount )
        {
        // Set SPS ready for setting values.
        CSPSettings* spSettings = CSPSettings::NewLC(); // CS:1

        for ( TInt counter = 0; counter < spSettingsCount; counter++ )
            {
            TUint32 serviceId = 
                iProfileEntries[iProfileID]->iServiceProviderId;

            TInt err( KErrNone );
            CSPEntry* spEntry = CSPEntry::NewLC(); // CS:2
            err = spSettings->FindEntryL( serviceId, *spEntry );
            User::LeaveIfError( err );

            CSPProperty* property = CSPProperty::NewLC(); // CS:3

            // VMBX MWI-URI.
            if ( iSPSettings[counter]->iVmbxMwiUri->Des().Length() )
                {
                err = property->SetName( ESubPropertyVMBXMWIAddress );
                property->SetValue( 
                    iSPSettings[counter]->iVmbxMwiUri->Des() );
                if ( KErrNone == err )
                    {
                    err = spEntry->AddPropertyL( *property );
                    if ( KErrAlreadyExists == err )
                        {
                        User::LeaveIfError( spEntry->UpdateProperty( 
                            ESubPropertyVMBXMWIAddress, 
                            iSPSettings[counter]->iVmbxMwiUri->Des() ) );
                        }
                    else
                        {
                        User::LeaveIfError( err );
                        }
                    }

                // Let's check if listening URI has been or is about
                // to be set. If not, set same as in MWI address.
                CSPProperty* listeningUri = CSPProperty::NewLC();
                err = spSettings->FindPropertyL( serviceId, 
                    ESubPropertyVMBXListenAddress, *listeningUri );
                if ( KErrNone != err && 
                    !iSPSettings[counter]->iVmbxListenUri->Des().Length() )
                    {
                    delete iSPSettings[counter]->iVmbxListenUri;
                    iSPSettings[counter]->iVmbxListenUri = NULL;
                    iSPSettings[counter]->iVmbxListenUri = 
                        iSPSettings[counter]->iVmbxMwiUri->Des().AllocL();
                    }
                CleanupStack::PopAndDestroy( listeningUri );

                // Let's check if re-subscribe interval has been or is
                // about to be set. If not, set default value.
                CSPProperty* reSubscrInterval = CSPProperty::NewLC();
                err = spSettings->FindPropertyL( serviceId, 
                    ESubPropertyVMBXMWISubscribeInterval, *reSubscrInterval );
                if ( KErrNone != err && 
                    iSPSettings[counter]->iReSubscribeInterval == KErrNotFound )
                    {
                    iSPSettings[counter]->iReSubscribeInterval = 
                        KDefaultReSubscribe;
                    }
                CleanupStack::PopAndDestroy( reSubscrInterval );
                }

            // VMBX ListeningURI.
            if ( iSPSettings[counter]->iVmbxListenUri->Des().Length() )
                {
                err = property->SetName( ESubPropertyVMBXListenAddress );
                property->SetValue( 
                    iSPSettings[counter]->iVmbxListenUri->Des() );
                if ( KErrNone == err )
                    {
                    err = spEntry->AddPropertyL( *property );
                    if ( KErrAlreadyExists == err )
                        {
                        User::LeaveIfError( spEntry->UpdateProperty( 
                            ESubPropertyVMBXListenAddress, 
                            iSPSettings[counter]->iVmbxListenUri->Des() ) );
                        }
                    else
                        {
                        User::LeaveIfError( err );
                        }
                    }
                }

            // VMBX IAP ID.
            if ( KErrNone != iSPSettings[counter]->iVmbxIapId )
                {
                err = property->SetName( ESubPropertyVMBXPreferredIAPId );
                property->SetValue( iSPSettings[counter]->iVmbxIapId );
                if ( KErrNone == err )
                    {
                    err = spEntry->AddPropertyL( *property );
                    if ( KErrAlreadyExists == err )
                        {
                        User::LeaveIfError( spEntry->UpdateProperty( 
                            ESubPropertyVMBXPreferredIAPId, 
                            iSPSettings[counter]->iVmbxIapId ) );
                        }
                    else
                        {
                        User::LeaveIfError( err );
                        }
                    }
                }

            // VMBX re-SUBSCRIBE interval.
            if ( KErrNotFound != iSPSettings[counter]->iReSubscribeInterval )
                {
                err = property->SetName( 
                    ESubPropertyVMBXMWISubscribeInterval );
                property->SetValue( 
                    iSPSettings[counter]->iReSubscribeInterval );
                if ( KErrNone == err )
                    {
                    err = spEntry->AddPropertyL( *property );
                    if ( KErrAlreadyExists == err )
                        {
                        User::LeaveIfError( spEntry->UpdateProperty( 
                            ESubPropertyVMBXMWISubscribeInterval, 
                            iSPSettings[counter]->iReSubscribeInterval ) );
                        }
                    else
                        {
                        User::LeaveIfError( err );
                        }
                    }
                }

            // VMBX SIP ID.
            if ( KErrNone != iSPSettings[counter]->iVmbxSipId )
                {
                err = property->SetName( ESubPropertyVMBXSettingsId );
                property->SetValue( iSPSettings[counter]->iVmbxSipId );
                if ( KErrNone == err )
                    {
                    err = spEntry->AddPropertyL( *property );
                    if ( KErrAlreadyExists == err )
                        {
                        User::LeaveIfError( spEntry->UpdateProperty( 
                            ESubPropertyVMBXSettingsId, 
                            iSPSettings[counter]->iVmbxSipId ) );
                        }
                    else
                        {
                        User::LeaveIfError( err );
                        }
                    }
                }

            // Service provider bookmark URI.
            if ( iSPSettings[counter]->iServiceProviderBookmark->
                Des().Length() )
                {
                err = property->SetName( EPropertyServiceBookmarkUri );
                property->SetValue( 
                    iSPSettings[counter]->iServiceProviderBookmark->Des() );
                if ( KErrNone == err )
                    {
                    err = spEntry->AddPropertyL( *property );
                    if ( KErrAlreadyExists == err )
                        {
                        User::LeaveIfError( spEntry->UpdateProperty( 
                            EPropertyServiceBookmarkUri, 
                            iSPSettings[counter]->
                            iServiceProviderBookmark->Des() ) );
                        }
                    else
                        {
                        User::LeaveIfError( err );
                        }
                    }
                }

            // Branding data URI.
            if ( iSPSettings[counter]->iBrandingDataUri->Des().Length() )
                {
                err = property->SetName( ESubPropertyVoIPBrandDataUri );
                property->SetValue( 
                    iSPSettings[counter]->iBrandingDataUri->Des() );
                if ( KErrNone == err )
                    {
                    err = spEntry->AddPropertyL( *property );
                    if ( KErrAlreadyExists == err )
                        {
                        User::LeaveIfError( spEntry->UpdateProperty( 
                            ESubPropertyVoIPBrandDataUri, 
                            iSPSettings[counter]->iBrandingDataUri->Des() ) );
                        }
                    else
                        {
                        User::LeaveIfError( err );
                        }
                    }
                }

            // Presence settings ID.
            if ( KErrNone != iSPSettings[counter]->iPresenceId )
                {
                err = property->SetName( ESubPropertyPresenceSettingsId );
                property->SetValue( 
                    iSPSettings[counter]->iPresenceId );
                if ( KErrNone == err )
                    {
                    err = spEntry->AddPropertyL( *property );
                    if ( KErrAlreadyExists == err )
                        {
                        User::LeaveIfError( spEntry->UpdateProperty( 
                            ESubPropertyPresenceSettingsId, 
                            iSPSettings[counter]->iPresenceId ) );
                        }
                    else
                        {
                        User::LeaveIfError( err );
                        }
                    }
                }

            // Presence auto accept buddy request.
            if ( KErrNotFound != iSPSettings[counter]->iAutoAcceptBuddy )
                {
                TOnOff value( static_cast<TOnOff>( 
                    iSPSettings[counter]->iAutoAcceptBuddy ) );
                err = property->SetName( 
                    ESubPropertyPresenceRequestPreference );
                property->SetValue( value );
                if ( KErrNone == err )
                    {
                    err = spEntry->AddPropertyL( *property );
                    if ( KErrAlreadyExists == err )
                        {
                        User::LeaveIfError( spEntry->UpdateProperty( 
                            ESubPropertyPresenceRequestPreference, value ) );
                        }
                    else
                        {
                        User::LeaveIfError( err );
                        }
                    }
                }

            // Auto enable service.
            if ( KErrNotFound != iSPSettings[counter]->iAutoEnableService )
                {
                TOnOff value( static_cast<TOnOff>( 
                    iSPSettings[counter]->iAutoEnableService ) );
                err = property->SetName( ESubPropertyVoIPEnabled );
                err = property->SetValue( value );
                if ( KErrNone == err )
                    {
                    err = spEntry->AddPropertyL( *property );
                    if ( KErrAlreadyExists == err )
                        {
                        User::LeaveIfError( spEntry->UpdateProperty( 
                            ESubPropertyVoIPEnabled, value ) ) ;
                        }
                    else
                        {
                        User::LeaveIfError( err );
                        }
                    }
                // enable presence
                if ( KErrNone != iSPSettings[counter]->iPresenceId )
                    {
                    err = property->SetName( ESubPropertyPresenceEnabled );
                    property->SetValue( value );
                    if ( KErrNone == err )
                        {
                        err = spEntry->AddPropertyL( *property );
                        if ( KErrAlreadyExists == err )
                            {
                            User::LeaveIfError( spEntry->UpdateProperty( 
                                ESubPropertyPresenceEnabled, value ) );
                            }
                        else
                            {
                            User::LeaveIfError( err );
                            }
                        }
                    }
                // enable voicemailbox
                if ( KErrNone != iSPSettings[counter]->iVmbxIapId )
                    {
                    err = property->SetName( ESubPropertyVMBXEnabled );
                    property->SetValue( value );
                    if ( KErrNone == err )
                        {
                        err = spEntry->AddPropertyL( *property );
                        if ( KErrAlreadyExists == err )
                            {
                            User::LeaveIfError( spEntry->UpdateProperty( 
                                ESubPropertyVMBXEnabled, value ) );
                            }
                        else
                            {
                            User::LeaveIfError( err );
                            }
                        }
                    }
                }

            // SNAP Profile ID.
            if ( KErrNone != iSPSettings[counter]->iSnapId )
                {
                err = property->SetName( ESubPropertyVoIPPreferredSNAPId );
                property->SetValue( iSPSettings[counter]->iSnapId );
                if ( KErrNone == err )
                    {
                    err = spEntry->AddPropertyL( *property );
                    if ( KErrAlreadyExists == err )
                        {
                        User::LeaveIfError( spEntry->UpdateProperty( 
                            ESubPropertyVoIPPreferredSNAPId, 
                            iSPSettings[counter]->iSnapId ) );
                        }
                    else
                        {
                        User::LeaveIfError( err );
                        }
                    }
                }

            // Enable IM
            if ( KErrNotFound != iSPSettings[counter]->iImEnabled )
                {
                // IM is enabled => add or set IM properties.
                if ( iSPSettings[counter]->iImEnabled )
                    {
                    // IM enabled.
                    property->SetName( ESubPropertyIMEnabled );
                    property->SetValue( EOn );
                    err = spEntry->AddPropertyL( *property );
                    if ( KErrAlreadyExists == err )
                        {
                        User::LeaveIfError( spEntry->UpdateProperty( 
                            ESubPropertyIMEnabled, EOn ) );
                        }

                    // IM launch UID.
                    property->SetName( ESubPropertyIMLaunchUid );
                    property->SetValue( KIMLaunchUid );
                    err = spEntry->AddPropertyL( *property );
                    if ( KErrAlreadyExists == err )
                        {
                        User::LeaveIfError( spEntry->UpdateProperty( 
                            ESubPropertyIMLaunchUid, KIMLaunchUid ) );
                        }

                    // IM settings ID.
                    property->SetName( ESubPropertyIMSettingsId );
                    // The value only needs to be different from 0,
                    // no-one actually uses it.
                    property->SetValue( KIMSettingsId );
                    err = spEntry->AddPropertyL( *property );
                    if ( KErrAlreadyExists == err )
                        {
                        User::LeaveIfError( spEntry->UpdateProperty( 
                            ESubPropertyIMSettingsId, KIMSettingsId ) );
                        }

                    // IM sub-service plugin UID.
                    property->SetName( EPropertyIMSubServicePluginId );
                    property->SetValue( KIMSubServicePluginId );
                    err = spEntry->AddPropertyL( *property );
                    if ( KErrAlreadyExists == err )
                        {
                        User::LeaveIfError( spEntry->UpdateProperty( 
                            EPropertyIMSubServicePluginId, 
                            KIMSubServicePluginId ) );
                        }

                    // IM preferred SNAP ID.
                    const CSPProperty* snap;
                    spEntry->GetProperty( snap, 
                        ESubPropertyVoIPPreferredSNAPId );
                    TInt snapId( KErrNone );
                    err = snap->GetValue( snapId );
                    if ( KErrNone == err )
                        {
                        property->SetName( ESubPropertyIMPreferredSNAPId );
                        err = property->SetValue( snapId );
                        if ( KErrNone == err )
                            {
                            err = spEntry->AddPropertyL( *property );
                           if ( KErrAlreadyExists == err )
                                {
                                User::LeaveIfError( spEntry->UpdateProperty( 
                                    ESubPropertyIMPreferredSNAPId, 
                                    snapId ) );
                                }
                            }
                        }

                    // IM default tone.
                    TFileName toneFile;
                    toneFile.Copy( PathInfo::RomRootPath() );
                    toneFile.Append( PathInfo::DigitalSoundsPath() );
                    toneFile.Append( KDefaultTone );
                    MVIMPSTSettingsStore* vimpStSettings =
                        CVIMPSTSettingsStore::NewLC(); // CS:4
                    User::LeaveIfError( vimpStSettings->SetL( serviceId,
                        EServiceToneFileName, toneFile ) );
                    // Pop vimpStSettings (can't use M object as argument).
                    CleanupStack::PopAndDestroy(); // CS:3
                    }
                // IM enabled is set to false => delete IM properties.
                else
                    {
                    spEntry->DeleteProperty( ESubPropertyIMEnabled );
                    spEntry->DeleteProperty( ESubPropertyIMLaunchUid );
                    spEntry->DeleteProperty( ESubPropertyIMSettingsId );
                    spEntry->DeleteProperty( EPropertyIMSubServicePluginId );
                    spEntry->DeleteProperty( ESubPropertyIMPreferredSNAPId );
                    }
                }

            err = spSettings->UpdateEntryL( *spEntry );
            // property, spEntry
            CleanupStack::PopAndDestroy( 2, spEntry ); // CS:1
            } // for
        CleanupStack::PopAndDestroy( spSettings ); // CS:0
        iSPSettings.ResetAndDestroy();
        }

    DBG_PRINT("CNSmlDmVoIPAdapter::CompleteOutstandingCmdsL(): end");   
    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::FindProfileLocation
// Finds profileID from aURI and returns profile location in array.
// ---------------------------------------------------------------------------
//
TInt CNSmlDmVoIPAdapter::FindProfileLocation( const TDesC8& aURI ) const
    {
    TInt id( 0 );
    id = aURI.Find( KNSmlDMVoIPPrefix );
    if ( id != KErrNotFound )
        {
        id = DesToInt( aURI.Mid( id + KNSmlDMVoIPPrefix().Length(), 10 ) ); 
        for ( TInt counter = 0; counter < iProfileEntries.Count(); counter++ )
            {
            if ( iProfileEntries[counter]->iId == id )
                {
                return counter;
                }
            }
        }
    return KErrNotFound;
    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::DesToInt
// Converts a 8 bit descriptor to int.
// ---------------------------------------------------------------------------
//
TInt CNSmlDmVoIPAdapter::DesToInt( const TDesC8& aDes ) const
    {
    TLex8 lex( aDes );
    TInt value = 0;
    lex.Val( value );
    return value;
    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::DesToTUint
// Converts a 8bit descriptor to TUint.
// ---------------------------------------------------------------------------
//
TUint CNSmlDmVoIPAdapter::DesToTUint( const TDesC8& aDes ) const
    {
    TLex8 lex( aDes );
    TUint value = 0;
    lex.Val( value );
    return value;
    }

// ---------------------------------------------------------------------------
// TPtrC8 CNSmlDmVoIPAdapter::RemoveLastURISeg
// returns parent uri, i.e. removes last uri segment
// ---------------------------------------------------------------------------
//
const TPtrC8 CNSmlDmVoIPAdapter::RemoveLastURISeg( const TDesC8& aURI ) const
    {
    TInt i ( 0 );
    if ( KErrNotFound != aURI.Find( KNSmlDMVoIPSeparator ) )
        {
        for ( i = aURI.Length() - 1; i >= 0; i-- )
            {
            if ( KDMVoIPSeparator == aURI[i]  )
                {
                break;
                }
            }
        }
    return aURI.Left( i );
    }

// ---------------------------------------------------------------------------
// TInt CNSmlDmVoIPAdapter::NumOfURISegs
// For getting the number of uri segs
// ---------------------------------------------------------------------------
//
TInt CNSmlDmVoIPAdapter::NumOfURISegs( const TDesC8& aURI ) const
    {
    TInt numOfURISegs = 1;
    for ( TInt i = 0; i < aURI.Length(); i++ )
        {
        if ( KDMVoIPSeparator == aURI[i]  )
            {
            numOfURISegs++;
            }
        }
    return numOfURISegs;
    }

// ---------------------------------------------------------------------------
// TPtrC8 CNSmlDmVoIPAdapter::LastURISeg
// Returns only the last uri segment
// ---------------------------------------------------------------------------
//
const TPtrC8 CNSmlDmVoIPAdapter::LastURISeg( const TDesC8& aURI ) const
    {
    TInt i( 0 );
    if ( KErrNotFound != aURI.Find( KNSmlDMVoIPSeparator ) )
        {
        for ( i = aURI.Length() - 1; i >= 0; i-- )
            {
            if ( KDMVoIPSeparator == aURI[i] )
                {
                break;
                }
            }
        }
    if ( 0 == i )
        {
        return aURI;
        }
    else
        {
        return aURI.Mid( i + 1 );
        }
    }

// ---------------------------------------------------------------------------
// TPtrC8 CNSmlDmVoIPAdapter::RemoveDotSlash
// return uri without dot and slash in start
// ---------------------------------------------------------------------------
//
const TPtrC8 CNSmlDmVoIPAdapter::RemoveDotSlash( const TDesC8& aURI ) const
    {
    if ( 0 == aURI.Find( KNSmlVoIPUriDotSlash ) )
        {
        return aURI.Right( aURI.Length() - KNSmlVoIPUriDotSlash().Length() );
        }
    else
        {
        return aURI;
        }
    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::SetSipRefL
// Set sip reference value.
// ---------------------------------------------------------------------------
//
TUint32 CNSmlDmVoIPAdapter::SetSipRefL( const TDesC8& aObject ) const
    {
    DBG_PRINT("CNSmlDmVoIPAdapter::SetSipRef() : begin");
    __ASSERT_ALWAYS( iDmCallback != NULL, User::Leave( KErrArgument ) );

    CBufBase* fetchResult = CBufFlat::NewL( 1 );
    CleanupStack::PushL( fetchResult );

    // Make all SIP profiles known
    CSmlDmAdapter::TError errorStatus;
    iDmCallback->FetchLinkL( KNSmlDefDMSIP, *fetchResult, errorStatus );

    // Fetch SIP profile ID.
    TBuf8<KNSmlVoIPMaxResultLength> object; 
    object.Copy( aObject );
    object.Append( KNSmlDMVoIPSeparator );
    object.Append( KNSmlDMSIPSipID );
    iDmCallback->FetchLinkL( object, *fetchResult, errorStatus );
    TUint32 profileId = DesToInt( fetchResult->Ptr( 0 ) );    
    CleanupStack::PopAndDestroy( fetchResult );
    fetchResult = NULL;
    DBG_PRINT("CNSmlDmVoIPAdapter::SetSipRef() : end");
    return profileId;
    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::GetSipIdL
// Get URI for given sip profile ID.
// ---------------------------------------------------------------------------
//
MSmlDmAdapter::TError CNSmlDmVoIPAdapter::GetSipIdL( CBufBase& aObject,
    TUint32 aID ) const
    {
    DBG_PRINT("CNSmlDmVoIPAdapter::GetSipIdL() : begin");

    __ASSERT_ALWAYS( iDmCallback != NULL, User::Leave( KErrArgument ) );

    CBufBase* result = CBufFlat::NewL( 1 );
    CleanupStack::PushL( result ); 
    CSmlDmAdapter::TError errorStatus;
    TBuf8<KNSmlVoIPMaxResultLength> object;
    iDmCallback->FetchLinkL( KNSmlDefDMSIP, *result, errorStatus );
    result->Compress();
    HBufC8* childList = HBufC8::NewLC( result->Size() );
    TInt uriSegCount( 0 );
    if ( result->Size() > 0 )
        {
        childList->Des().Copy( result->Ptr(0) );

        // Delete last / mark
        childList->Des().Delete( childList->Length() - 1, 1 );    
        uriSegCount = NumOfURISegs( childList->Des() );
        }

    for ( TInt counter = 0; counter < uriSegCount; counter++ )
        {
        result->Reset();
        object.Copy( KNSmlDefDMSIP );
        object.Append( KNSmlDMVoIPSeparator );
        object.Append( LastURISeg( childList->Des() ) );

        // Make SIP profile settings known
        iDmCallback->FetchLinkL( object, *result, errorStatus );
        result->Compress();
        result->Reset();

        // Get SIP id
        object.Append( KNSmlDMVoIPSeparator );
        object.Append( KNSmlDMSIPSipID );
        iDmCallback->FetchLinkL( object, *result, errorStatus );
        if ( aID == DesToInt( result->Ptr( 0 ) ) )
            {
            aObject.InsertL( aObject.Size(), RemoveLastURISeg( object ) );
            break; 
            }
        childList->Des().Copy( RemoveLastURISeg ( childList->Des() ) );    
        }
    // childList, result 
    CleanupStack::PopAndDestroy( 2, result ); //childList
    result = NULL;
    childList = NULL;
    DBG_PRINT("CNSmlDmVoIPAdapter::GetSipIdL() : end");
    return errorStatus;
    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::SetSccpRefL
// Set SCCP reference value. 
// ---------------------------------------------------------------------------
//
TUint CNSmlDmVoIPAdapter::SetSccpRefL( const TDesC8& aObject ) const
    {
    DBG_PRINT("CNSmlDmVoIPAdapter::SetSccpRef() : begin");

    __ASSERT_ALWAYS( iDmCallback != NULL, User::Leave( KErrArgument ) );

    CBufBase* fetchResult = CBufFlat::NewL( 1 );
    CleanupStack::PushL( fetchResult );

    // Make all SCCP profiles known
    CSmlDmAdapter::TError errorStatus;
    iDmCallback->FetchLinkL( KNSmlDefDMSCCP, *fetchResult, errorStatus );

    // Find profile asked
    iDmCallback->FetchLinkL( aObject, *fetchResult, errorStatus );

    // Fetch SCCP profile ID.
    TBuf8<KNSmlVoIPMaxResultLength> object; 
    object.Copy( aObject );
    object.Append( KNSmlDMVoIPSeparator );
    object.Append( KNSmlDMSCCPSccpID ); 
    iDmCallback->FetchLinkL( object, *fetchResult, errorStatus );
    TUint32 profileId = DesToInt( fetchResult->Ptr( 0 ) );    
    CleanupStack::PopAndDestroy( fetchResult );
    fetchResult = NULL;
    DBG_PRINT("CNSmlDmVoIPAdapter::SetSccpRef() : end");
    return profileId;
    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::GetSccpIdL
// Get URI for given sccp profile ID.
// ---------------------------------------------------------------------------
//
MSmlDmAdapter::TError CNSmlDmVoIPAdapter::GetSccpIdL( CBufBase& aObject,
    TUint32 aID ) const
    {
    DBG_PRINT("CNSmlDmVoIPAdapter::GetSccpIdL() : begin");

    __ASSERT_ALWAYS( iDmCallback != NULL, User::Leave( KErrArgument ) );

    CBufBase* result = CBufFlat::NewL( 1 );
    CleanupStack::PushL( result ); 
    CSmlDmAdapter::TError errorStatus;
    TBuf8<KNSmlVoIPMaxResultLength> object;
    iDmCallback->FetchLinkL( KNSmlDefDMSCCP, *result, errorStatus );
    result->Compress();
    HBufC8* childList = HBufC8::NewLC( result->Size() );
    TInt uriSegCount ( 0 );
    if ( result->Size() > 0 )
        {
        childList->Des().Copy( result->Ptr(0) );

        // Delete last / mark
        childList->Des().Delete( childList->Length() - 1, 1 );    
        uriSegCount = NumOfURISegs( childList->Des() );
        }

    for ( TInt counter = 0; counter < uriSegCount; counter++ )
        {
        result->Reset();

        // Make SCCP profile settings known
        object.Copy( KNSmlDefDMSCCP );
        object.Append( KNSmlDMVoIPSeparator );
        object.Append( LastURISeg( childList->Des() ) );
        iDmCallback->FetchLinkL( object, *result, errorStatus );
        result->Compress();
        result->Reset();

        // Get SCCPId
        object.Append( KNSmlDMVoIPSeparator );
        object.Append( KNSmlDMSCCPSccpID );
        iDmCallback->FetchLinkL( object, *result, errorStatus );
        if ( aID == DesToInt ( result->Ptr( 0 ) ) )
            {
            aObject.InsertL( aObject.Size(), RemoveLastURISeg( object ) );
            break; 
            }
        childList->Des().Copy( RemoveLastURISeg ( childList->Des() ) );    
        }
    // childList, result 
    CleanupStack::PopAndDestroy( 2, result ); //childList
    result = NULL;
    childList = NULL;
    DBG_PRINT("CNSmlDmVoIPAdapter::GetSccpIdL() : end");
    return errorStatus;
    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::CheckDuplicateNameL
// Checks if duplicate named sip profiles. Renames if same.
// ---------------------------------------------------------------------------
//
TBool CNSmlDmVoIPAdapter::CheckDuplicateNameL( CRCSEProfileEntry& aEntry )
    {
    TBool isValid( EFalse );
    HBufC* newName = HBufC::NewLC( KMaxSettingsNameLength * 2 );
    newName->Des().Copy( aEntry.iSettingsName );

    TInt count( iProfileEntries.Count() );

    for ( TInt n = 0; n < count; n++ )
        {
        const TDesC& existingName = iProfileEntries[n]->iSettingsName;
        if ( 0 == existingName.Compare( aEntry.iSettingsName )
            && iProfileID != n ) // Skip the current profile.
            {
            TBool isUnique( EFalse );

            for ( TInt i = 1; !isUnique; i++ )
                {
                TBool found( EFalse );

                newName->Des().Copy( aEntry.iSettingsName );
                newName->Des().Append( KOpenParenthesis() );
                newName->Des().AppendNum( i );
                newName->Des().Append( KClosedParenthesis() );
                for ( TInt m = 0; m < count; m++ )
                    {
                    if ( 0 == iProfileEntries[m]->iSettingsName.Compare( 
                        newName->Des() ) )
                        {
                        found = ETrue;
                        }
                    } // for
                if ( !found )
                    {
                    isUnique = ETrue;
                    }
                } // for
            } // if
        } // for

    // Change setting only if length is smaller than max length.
    if ( newName->Length() < KMaxSettingsNameLength )
        {
        aEntry.iSettingsName.Copy( newName->Des() );
        isValid = ETrue;
        }
    CleanupStack::PopAndDestroy( newName );  
    return isValid;  
    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::CheckDuplicateProviderL
// Checks if duplicate provider name. Renames if same.
// ---------------------------------------------------------------------------
//
TBool CNSmlDmVoIPAdapter::CheckDuplicateProviderL( CRCSEProfileEntry& aEntry )
    {
    TBool isValid( EFalse );
    HBufC* newName = HBufC::NewLC( KMaxProviderNameLength * 2 );
    newName->Des().Copy( aEntry.iProviderName );

    TInt count( iProfileEntries.Count() );

    for ( TInt n = 0; n < count; n++ )
        {
        const TDesC& existingName = iProfileEntries[n]->iProviderName;
        if ( 0 == existingName.Compare( aEntry.iProviderName )
            && iProfileID != n ) // Skip the current profile.
            {
            TBool isUnique( EFalse );

            for ( TInt i = 1; !isUnique; i++ )
                {
                TBool found( EFalse );

                newName->Des().Copy( aEntry.iProviderName );
                newName->Des().Append( KOpenParenthesis() );
                newName->Des().AppendNum( i );
                newName->Des().Append( KClosedParenthesis() );
                for ( TInt m = 0; m < count; m++ )
                    {
                    if ( 0 == iProfileEntries[m]->iProviderName.Compare( 
                        newName->Des() ) )
                        {
                        found = ETrue;
                        }
                    }
                if ( !found )
                    {
                    isUnique = ETrue;
                    }
                }
            }
        }

    // Change setting only if length is smaller than max length.
    if ( newName->Length() < KMaxProviderNameLength )
        {
        aEntry.iProviderName.Copy( newName->Des() );
        isValid = ETrue;
        }

    CleanupStack::PopAndDestroy( newName ); 
    return isValid;  
    }

// ---------------------------------------------------------------------------
// TInt CNSmlDmVoIPAdapter::FindCodecLocation
// For getting codec position.
// ---------------------------------------------------------------------------
//
TInt CNSmlDmVoIPAdapter::FindCodecLocation( TUint32 aID ) const
    {
    for ( TInt i = 0; i < iCodecEntries.Count(); i++ )
        {
        if ( iCodecEntries[i]->iCodecId == aID )
            {
            return i;   
            }
        }
    return KErrNotFound;
    }

// ---------------------------------------------------------------------------
// void CNSmlDmVoIPAdapter::SetTelephonyPreferenceL
// For setting telephony preference.
// ---------------------------------------------------------------------------
//
void CNSmlDmVoIPAdapter::SetTelephonyPreferenceL ( const TTelephonyPreference& 
    aTelephonyPreference )
    {
    // Central Repository for richcall settings
    CRepository* rep = CRepository::NewLC( KCRUidRichCallSettings );
    if ( EPSPreferred == aTelephonyPreference )
        {
        rep->Set( KRCSEPreferredTelephony, EPSPreferred );
        }
    else if ( ECSPreferred == aTelephonyPreference )
        {
        rep->Set( KRCSEPreferredTelephony, ECSPreferred );
        }
    CleanupStack::PopAndDestroy( rep );
    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::GetTerminalTypeL
// Collect terminal type used in SIP User Agent Header.
// ---------------------------------------------------------------------------
//
void CNSmlDmVoIPAdapter::GetTerminalTypeL(
    TBuf<KMaxTerminalTypeLength>& aTerminalType ) const
    {
    aTerminalType.Zero(); // Reset before use

    CIpAppPhoneUtils* phoneUtils = CIpAppPhoneUtils::NewLC();
    phoneUtils->GetTerminalTypeL( aTerminalType ); // Gets phone model + type
    CleanupStack::PopAndDestroy( phoneUtils );
    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::GetWlanMacAddressL
// Gets WLAN MAC address used in SIP User Agent Header (same as *#62209526#).
// ---------------------------------------------------------------------------
//
void CNSmlDmVoIPAdapter::GetWlanMacAddressL( TBuf<KWlanMacAddressLength>& 
    aMac ) const
    {
    aMac.Zero(); // Reset before use

    CIPAppUtilsAddressResolver* resolver = CIPAppUtilsAddressResolver::NewLC();

    TBuf8<KWlanMacAddressLength> wlanmac;
    _LIT8( KFormat, "-");
    resolver->GetWlanMACAddress( wlanmac, KFormat );
    //wlanmac buffer contains now the wlan mac address like 00-15-a0-99-10-ec
    CleanupStack::PopAndDestroy( resolver );
    aMac.Copy( wlanmac );

    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::GetNatFwUriL
// Get NAT/FW profile URI.
// ---------------------------------------------------------------------------
//
MSmlDmAdapter::TError CNSmlDmVoIPAdapter::GetNatFwUriL( 
    TBuf16<KMaxUriLength>& aObject, TInt aProfileId ) const
    {
    DBG_PRINT("CNSmlDmVoIPAdapter::GetNatFwUriL : begin");

    __ASSERT_ALWAYS( iDmCallback != NULL, User::Leave( KErrArgument ) );

    CBufBase* result = CBufFlat::NewL( 1 ); // CS:1
    CleanupStack::PushL( result );
    CSmlDmAdapter::TError errorStatus;

    // Get URIs to all DomainSpecific NAT/FW settings.
    iDmCallback->FetchLinkL( KNSmlDMNATFWDomain(), *result, errorStatus );

    if ( CSmlDmAdapter::EOk != errorStatus )
        {
        CleanupStack::PopAndDestroy( result );
        return errorStatus;
        }

    result->Compress();
    HBufC8* childList = HBufC8::NewLC( result->Size() ); // CS:2
    TInt uriSegCount( 0 );

    if ( result->Size() )
        {
        childList->Des().Copy( result->Ptr(0) );
        uriSegCount = NumOfURISegs( childList->Des() );
        }

    for ( TInt index = 0; index < uriSegCount; index++ )
        {
        HBufC8* uri = HBufC8::NewLC( ( KMaxUriLength * KTwoSegs ) );// CS:3
        uri->Des().Copy( KNSmlDMNATFWDomain );
        uri->Des().Append( KNSmlDMVoIPSeparator() );
        uri->Des().Append( LastURISeg( childList->Des() ) );
        HBufC8* luid = HBufC8::NewLC( KMaxUriLength ); // CS:4
        luid->Des().Copy( iDmCallback->GetLuidAllocL( uri->Des() )->Des() );
        if ( luid->Des().Length() && 
            aProfileId == DesToTUint( LastURISeg( luid->Des() ) ) )
            {
            aObject.Copy( uri->Des() );
            // luid, uri
            CleanupStack::PopAndDestroy( 2, uri ); // CS:2
            break;
            }
        // luid, uri
        CleanupStack::PopAndDestroy( 2, uri ); // CS:2
        childList->Des().Copy( RemoveLastURISeg( childList->Des() ) );
        }

    // childList, result
    CleanupStack::PopAndDestroy( 2, result ); // CS:0
    DBG_PRINT("CNSmlDmVoIPAdapter::GetNatFwUriL : end");
    return errorStatus;
    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::NatFwIdL
// Get NAT/FW profile ID.
// ---------------------------------------------------------------------------
//
TUint32 CNSmlDmVoIPAdapter::NatFwIdL( const TDesC8& aObject ) const
    {
    DBG_PRINT("CNSmlDmVoIPAdapter::NatFwIdL : begin");

    __ASSERT_ALWAYS( iDmCallback != NULL, User::Leave( KErrArgument ) );

    TUint32 profileId( KErrNone );
    HBufC8* luid = HBufC8::NewLC( KMaxUriLength ); // CS:1
    luid->Des().Copy( iDmCallback->GetLuidAllocL( aObject )->Des() );
    if ( luid->Des().Length() )
        {
        profileId = DesToTUint( LastURISeg( luid->Des() ) );
        }
    CleanupStack::PopAndDestroy( luid ); // CS:0
    DBG_PRINT("CNSmlDmVoIPAdapter::NatFwIdL : end");
    return profileId;
    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::GetConRefL
// Get DM URI for IAP.
// ---------------------------------------------------------------------------
//
MSmlDmAdapter::TError CNSmlDmVoIPAdapter::GetConRefL( CBufBase& aObject, 
    TInt aIapId ) const
    {
    DBG_PRINT("CNSmlDmVoIPAdapter::GetConRefL() : begin");

    __ASSERT_ALWAYS( iDmCallback != NULL, User::Leave( KErrArgument ) );

    TBool found = EFalse;
    CNSmlDMIAPMatcher* iapMatch = CNSmlDMIAPMatcher::NewL( iDmCallback );
    CleanupStack::PushL( iapMatch ); // CS:1
    HBufC8* uri8 = iapMatch->URIFromIAPIdL( aIapId );
    CleanupStack::PushL( uri8 ); // CS:2
    if ( uri8 )
        {
        aObject.InsertL( aObject.Size(), uri8->Des() );
        found = ETrue;
        }
    // uri8, iapMatch
    CleanupStack::PopAndDestroy( 2, iapMatch ); // CS:0
    DBG_PRINT("CNSmlDmVoIPAdapter::GetConRefL() : end");
    if ( !found )
        {
        return CSmlDmAdapter::ENotFound;
        }
    return CSmlDmAdapter::EOk;
    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::ConRefL
// Get IAP ID (connection reference value) from given URI.
// ---------------------------------------------------------------------------
//
TInt CNSmlDmVoIPAdapter::ConRefL( const TDesC8& aObject ) const
    {
    DBG_PRINT("CNSmlDmVoIPAdapter::ConRefL() : begin");

    __ASSERT_ALWAYS( iDmCallback != NULL, User::Leave( KErrArgument ) );

    CNSmlDMIAPMatcher* iapMatch = CNSmlDMIAPMatcher::NewL( iDmCallback );
    CleanupStack::PushL( iapMatch );
    TInt iapId = iapMatch->IAPIdFromURIL( aObject );
    CleanupStack::PopAndDestroy( iapMatch ); 
    DBG_PRINT("CNSmlDmVoIPAdapter::ConRefL() : end");
    return iapId;
    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::GetPresenceUriL
// Get Presence profile DM URI.
// ---------------------------------------------------------------------------
//
MSmlDmAdapter::TError CNSmlDmVoIPAdapter::GetPresenceUriL( 
    TBuf16<KMaxUriLength>& aObject, TUint32 aProfileId ) const
    {
    DBG_PRINT("CNSmlDmVoIPAdapter::GetPresenceUriL : begin");

    __ASSERT_ALWAYS( iDmCallback != NULL, User::Leave( KErrArgument ) );

    CBufBase* result = CBufFlat::NewL( 1 );
    CleanupStack::PushL( result ); // CS:1
    CSmlDmAdapter::TError errorStatus;

    // Get URIs to all Presence settings.
    iDmCallback->FetchLinkL( KNSmlDMPresence(), *result, errorStatus );

    if ( CSmlDmAdapter::EOk != errorStatus )
        {
        CleanupStack::PopAndDestroy( result );
        return errorStatus;
        }

    result->Compress();
    HBufC8* childList = HBufC8::NewLC( result->Size() ); // CS:2
    TInt uriSegCount( 0 );

    if ( result->Size() )
        {
        childList->Des().Copy( result->Ptr( 0 ) );
        uriSegCount = NumOfURISegs( childList->Des() );
        }

    // Get LUIDs for URIs.
    for ( TInt index = 0; index < uriSegCount; index++ )
        {
        HBufC8* uri = HBufC8::NewLC( KMaxUriLength ); // CS:3
        HBufC8* luid = HBufC8::NewLC( KMaxUriLength ); // CS:4

        uri->Des().Copy( KNSmlDMPresence() );
        uri->Des().Append( KNSmlDMVoIPSeparator() );
        uri->Des().Append( LastURISeg( childList->Des() ) );
        luid->Des().Copy( iDmCallback->GetLuidAllocL( uri->Des() )->Des() );
        if ( luid->Des().Length() && 
            aProfileId == DesToTUint( LastURISeg( luid->Des() ) ) )
            {
            aObject.Copy( uri->Des() );
            // luid, uri
            CleanupStack::PopAndDestroy( 2, uri );  // CS:2
            break;
            }
        childList->Des().Copy( RemoveLastURISeg( childList->Des() ) );
        // luid, uri
        CleanupStack::PopAndDestroy( 2, uri );  // CS:2
        }
    // childList, result
    CleanupStack::PopAndDestroy( 2, result );     // CS:0
    DBG_PRINT("CNSmlDmVoIPAdapter::GetPresenceUriL : end");
    return errorStatus;
    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::PresenceIdL
// Get Presence profile ID.
// ---------------------------------------------------------------------------
//
TUint32 CNSmlDmVoIPAdapter::PresenceIdL( const TDesC8& aObject ) const
    {
    DBG_PRINT("CNSmlDmVoIPAdapter::PresenceIdL : begin");

    __ASSERT_ALWAYS( iDmCallback != NULL, User::Leave( KErrArgument ) );

    HBufC8* luid = HBufC8::NewLC( KMaxUriLength ); // CS:1
    luid->Des().Copy( iDmCallback->GetLuidAllocL( aObject )->Des() );
    if ( luid->Des().Length() )
        {
        TUint32 profileId = DesToTUint( LastURISeg( luid->Des() ) );
        CleanupStack::PopAndDestroy( luid ); // CS:0
        DBG_PRINT("CNSmlDmVoIPAdapter::PresenceIdL : end");
        return profileId;
        }
    CleanupStack::PopAndDestroy( luid ); // CS:0
    DBG_PRINT("CNSmlDmVoIPAdapter::PresenceIdL : end");
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::SnapIdL
// Gets SNAP ID based on URI.
// ---------------------------------------------------------------------------
//
TInt CNSmlDmVoIPAdapter::SnapIdL( const TDesC8& aUri )
    {
    DBG_PRINT( "CNSmlDmVoIPAdapter::SnapIdL - begin" );
    TInt snapId( KErrNotFound );

    HBufC8* luid = HBufC8::NewLC( KNSmlVoIPMaxUriLength ); // CS:1
    luid->Des().Copy( iDmCallback->GetLuidAllocL( aUri )->Des() );

    if ( luid->Des().Length() )
        {
        snapId = DesToInt( LastURISeg( luid->Des() ) );
        }
    CleanupStack::PopAndDestroy( luid ); // CS:0
    DBG_PRINT( "CNSmlDmVoIPAdapter::SnapIdL - end" );
    return snapId;
    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::GetSnapUriL
// Set SNAP URI based on SNAP ID.
// ---------------------------------------------------------------------------
//
MSmlDmAdapter::TError CNSmlDmVoIPAdapter::GetSnapUriL( TDes8& aObject, 
    TInt aSnapId )
    {
    __ASSERT_ALWAYS( iDmCallback != NULL, User::Leave( KErrArgument ) );

    DBG_PRINT( "CNSmlDmSIPAdapter::GetSnapUriL - begin" );

    CBufBase* result = CBufFlat::NewL( 1 );
    CleanupStack::PushL( result ); // CS:1
    CSmlDmAdapter::TError status;

    // Get URIs to all SNAP settings.
    iDmCallback->FetchLinkL( KNSmlDMSnapUriPrefix(), *result, status );

    if ( CSmlDmAdapter::EOk != status )
        {
        CleanupStack::PopAndDestroy( result );
        DBG_PRINT( "CNSmlDmVoIPAdapter::GetSnapUriL - Fetch SNAP error end" );
        return status;
        }

    result->Compress();
    HBufC8* childList = HBufC8::NewLC( result->Size() ); // CS:2
    TInt uriSegCount( 0 );

    if ( result->Size() )
        {
        childList->Des().Copy( result->Ptr( 0 ) );
        uriSegCount = NumOfURISegs( childList->Des() );
        }

    // Get LUIDs for URIs.
    for ( TInt index = 0; index < uriSegCount; index++ )
        {
        HBufC8* uri = HBufC8::NewLC( KNSmlVoIPMaxUriLength ); // CS:3
        HBufC8* luid = HBufC8::NewLC( KNSmlVoIPMaxUriLength ); // CS:4

        uri->Des().Copy( KNSmlDMSnapUriPrefix() );
        uri->Des().Append( KNSmlDMVoIPSeparator() );
        uri->Des().Append( LastURISeg( childList->Des() ) );
        luid->Des().Copy( iDmCallback->GetLuidAllocL( uri->Des() )->Des() );
        if ( luid->Des().Length() && 
            aSnapId == DesToInt( LastURISeg( luid->Des() ) ) )
            {
            aObject.Copy( uri->Des() );
            // luid, uri
            CleanupStack::PopAndDestroy( 2, uri );  // CS:2
            break;
            }
        childList->Des().Copy( RemoveLastURISeg( childList->Des() ) );
        // luid, uri
        CleanupStack::PopAndDestroy( 2, uri );  // CS:2
        }
    // childList, result
    CleanupStack::PopAndDestroy( 2, result );     // CS:0
    DBG_PRINT( "CNSmlDmVoIPAdapter::GetSnapUriL - end" );
    return status;
    }

// End of file.
