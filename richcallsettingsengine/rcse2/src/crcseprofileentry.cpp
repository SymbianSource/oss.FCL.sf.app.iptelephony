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
* Description:  Profile entry, which is stored to cenrep
*
*/



// INCLUDE FILES
#include	<featmgr.h>

#include    "crcseprofileentry.h"
#include    "rcsedefaults.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CRCSEProfile::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CRCSEProfileEntry* CRCSEProfileEntry::NewL()
    {
    CRCSEProfileEntry* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CRCSEProfile::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CRCSEProfileEntry* CRCSEProfileEntry::NewLC()
    {
    CRCSEProfileEntry* self = new (ELeave) CRCSEProfileEntry();
    CleanupStack::PushL( self );
    self->ConstructL();
    self->ResetDefaultValues();
    return self;
    }

// -----------------------------------------------------------------------------
// CRCSEProfileEntry::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CRCSEProfileEntry::ConstructL()
    {
    // Check VoIP support from feature manager
    FeatureManager::InitializeLibL();
    TBool support = FeatureManager::FeatureSupported( KFeatureIdCommonVoip );
    FeatureManager::UnInitializeLib();
    if (!support)
        {
        User::Leave(KErrNotSupported);
        }
    }

// -----------------------------------------------------------------------------
// CRCSEProfileEntry::CRCSEProfileEntry
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CRCSEProfileEntry::CRCSEProfileEntry()
    {
    }
    
// Destructor
EXPORT_C CRCSEProfileEntry::~CRCSEProfileEntry()
    {
    iIds.Reset();
    iIds.Close();
    iPreferredCodecs.Reset();
    iPreferredCodecs.Close();
    }
    
// -----------------------------------------------------------------------------
// CRCSEProfileEntry::ResetDefaultValues
// Resets profile entry to default settings.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRCSEProfileEntry::ResetDefaultValues()
    {
    iIds.Reset();
    
    iId                 = 0;
    iProviderName       = KRCSEDefaultProviderName;
    iSettingsName       = KRCSEDefaultSettingsName;
    
    // Codecs
    iPreferredCodecs.Reset();
    
    iStartMediaPort     = KRCSEDefaultStartMediaPort;
    iEndMediaPort       = KRCSEDefaultEndMediaPort;
    iSiqnalingQOS       = KRCSEDefaultSiqnalingQOS;
    iMediaQOS           = KRCSEDefaultMediaQOS;
    iInbandDTMF         = KRCSEDefaultInbandDTMF;
    iOutbandDTMF        = KRCSEDefaultOutbandDTMF;
    iHoldRingBack       = KRCSEDefaultHoldRingBack;
    iAutoComplete       = KRCSEDefaultAutoComplete;
    iCFNoAnswer         = KRCSEDefaultCFNoAnswer;
    iCFBusy             = KRCSEDefaultCFBusy;
    iCFUnconditional    = KRCSEDefaultCFUnconditional;
    iRedundancy         = KRCSEDefaultRedundancy;   
    iSecureCallPreference   = KRCSEDefaultSecureCallPreference;
    iVoIPProfileLock        = KRCSEDefaultVoIPProfileLock;
    iAdhocAllowed           = KRCSEDefaultAdhocAllowed;
    iSIPServerType          = KRCSEDefaultSIPServerType;
    iSBCType                = KRCSEDefaultSBCType;
    iSTUNServerType         = KRCSEDefaultSTUNServerType;
    iWLANAPType             = KRCSEDefaultWLANAPType;
    iPSTNGatewayType        = KRCSEDefaultPSTNGatewayType;
    iSecurityGatewayType    = KRCSEDefaultSecurityGatewayType;
    iRTCP                   = KRCSEDefaultRTCP;
    iSIPVoIPUAHTerminalType = KRCSEDefaultSIPVoIPUAHTerminalType;
    iSIPVoIPUAHeaderWLANMAC = KRCSEDefaultSIPVoIPUAHeaderWLANMAC;
    iSIPVoIPUAHeaderString  = KRCSEDefaultSIPVoIPUAHeaderString;
    iProfileLockedToIAP     = KRCSEDefaultProfileLockedToIAP;
    iVoIPPluginUID          = KRCSEDefaultVoIPPluginUID;
    iAllowVoIPoverWCDMA     = KRCSEDefaultAllowVoIPoverWCDMA;
    iAllowVoIPoverBT        = KRCSEDefaultAllowVoIPoverBT;
    iMeanCountOfVoIPDigits  = KRCSEDefaultMeanCountOfVoIPDigits;
    iIgnoreAddrDomainPart   = KRCSEDefaultIgnoreAddrDomainPart;

    iHandoverDialect            = KRCSEDefHandoverDialect;
    iPSTelephonyHOPreference    = KRCSEDefPSTelephonyHOPreference;
    iHOThresholdValueLL         = KRCSEDefHOThresholdValueLL;
    iHOThresholdValueHL         = KRCSEDefHOThresholdValueHL;
    iNumberOfMeasurementsAbove  = KRCSEDefNumberOfMeasurementsAbove;
    iNumberOfMeasurementsBelow  = KRCSEDefNumberOfMeasurementsBelow;
    iSmartScannInterParaHigh    = KRCSEDefSmartScannInterParaHigh;
    iSmartScannInterParaMedium  = KRCSEDefSmartScannInterParaMedium;
    iSmartScannInterParaLow     = KRCSEDefSmartScannInterParaLow;
    iSmartScannInterParaStatic  = KRCSEDefSmartScannInterParaStatic;

    iSmartScannDurationHighMode     = KRCSEDefSmartScannDurationHighMode;
    iSmartScannDurationMediumMode   = KRCSEDefSmartScannDurationMediumMode;
    iSmartScannDurationLowMode      = KRCSEDefSmartScannDurationLowMode;
    iHandoffNumber                  = KRCSEDefHandoffNumber;
    iHandbackNumber                 = KRCSEDefHandbackNumber;
    iHysterisisTimer                = KRCSEDefHysterisisTimer;
    iHandOffProcessTimer            = KRCSEDefHandOffProcessTimer;
    iDisconnectProcessTimer         = KRCSEDefDisconnectProcessTimer;
    iHandoffPrefix                  = KRCSEHandoffPrefix;
    iHandbackPrefix                 = KRCSEHandbackPrefix;
    iHandoverTones                  = KRCSEDefHandoverTones;
    iSupportSMSoverWLAN             = KRCSEDefSupportSMSoverWLAN;

    iServiceProviderId              = KRCSEDefServiceProviderId;
    iUserPhoneUriParameter          = KRCSEDefUserPhoneUriParam;
    iSIPConnTestAddress             = KRCSEDefSIPConnTestAddress;
    iNATSettingsStorageId           = KRCSEDefNATSettingsStorageId;
    iSIPMinSE                       = KRCSEDefSIPMinSE;
    iSIPSessionExpires              = KRCSEDefSIPSessionExpires;
    iNATProtocol                    = KRCSEDefNATProtocol;
    iNewServiceTableEntry           = KRCSENewServiceTableEntry;
    iSNAPId                         = KRCSEDefSNAPId;
    }

//  End of File  
