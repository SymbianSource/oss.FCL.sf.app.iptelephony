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
* Description:  Single profile entry, which is stored to RCSE
*
*/



#ifndef CRCSEPROFILEENTRY_H
#define CRCSEPROFILEENTRY_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h>

// CONSTANTS
const TInt32 KNotSet = -1;

// OMA Provisioning Content 1.1 defines the minimum lengths of
// these constants. These constants are doubled.
const TInt KMaxProviderNameLength = 64;
const TInt KMaxSettingsNameLength = 32;
const TInt KMaxSettingsLength32   = 32;
const TInt KMaxSettingsUrlLength  = 256;

// struct TSettings defines SIP, SCCP and SIP profile specific VOIP settings ids.
struct TSettingIds
        {
        TInt iProfileType;
        TInt iProfileId;
        TInt iProfileSpecificSettingId;    
        };

// CLASS DECLARATION

/**
*  Profile entry, which is stored to RCSE.
*
*  @lib RCSE.lib
*  @since S60 3.0
*/
class CRCSEProfileEntry : public CBase
    {
    public: // Enums

    enum TOnOff
        {
        EOONotSet = KNotSet,
        EOff = 0,
        EOn = 1
        };

    // Profile types of Ids
    enum TProtocolType
        {
        EProtocolNone    = -1,
        EProtocolSIP     = 0,
        EProtocolSCCP    = 1,
        EProtocolLast,
        };

    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CRCSEProfileEntry* NewL();

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CRCSEProfileEntry* NewLC();

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CRCSEProfileEntry();
        
    public: // New functions
                
        /**
        * Resets entry to default values.
        * @since S60 3.0
        */
        void ResetDefaultValues();
        
    private:
    
        /**
        * C++ default constructor.
        * Initialises values to ENotSet.
        */
        CRCSEProfileEntry();
        
	    /**
    	* By default Symbian 2nd phase constructor is private.
    	*/
    	void ConstructL();

    public:     // Data

        // Identifier of profile. This values is set by CRCSEProfile, when
        // profile entry is got from CRCSEProfile.
        TUint32 iId;

        // Provider of the profile.
        TBuf<KMaxProviderNameLength> iProviderName;
        
        // Name of the settings.
        TBuf<KMaxSettingsNameLength> iSettingsName;

        // Preferred codec ids in priority order.
        RArray<TUint32> iPreferredCodecs;

        // Start media port.
        TUint32 iStartMediaPort;

        // End media port.
        TUint32 iEndMediaPort;

        // Siqnaling Quolity Of Service
        TInt32 iSiqnalingQOS;

        // Media Quality Of Service.
        TInt32 iMediaQOS;

        // Whether Inband DTMF is in use.
        TOnOff iInbandDTMF;

        // Whether outband DTMF in in use
        TOnOff iOutbandDTMF;

        // Hold ring back timer value
        TInt32 iHoldRingBack;

        // Auto complete
        TOnOff iAutoComplete;

        // Call forwarding no answer
        TOnOff iCFNoAnswer;
        
        // Call forwarding when busy
        TOnOff iCFBusy;
        
        // Call forwarding unconditional
        TOnOff iCFUnconditional;
        
        // Redundancy
        TOnOff iRedundancy;
        
        // SIP, SCCp and SIP profile specific VOIP settings ids.
        RArray<TSettingIds> iIds;

        // Secure call preference
        TUint32 iSecureCallPreference;
        
        // VoIP profile lock
        TUint32 iVoIPProfileLock;
        
        // Adhoc allowed
        TUint32 iAdhocAllowed;
        
        // SIP Server type
        TBuf<KMaxSettingsLength32> iSIPServerType;
        
        // Session Board Controller type
        TBuf<KMaxSettingsLength32> iSBCType;
        
        // STUN Server type
        TBuf<KMaxSettingsLength32> iSTUNServerType;
        
        // WLAN AP type
        TBuf<KMaxSettingsLength32> iWLANAPType;
        
        // PSTN Gateway type
        TBuf<KMaxSettingsLength32> iPSTNGatewayType;
        
        // Security Gateway type
        TBuf<KMaxSettingsLength32> iSecurityGatewayType;

        // RTCP, Real-Time Control Protocol reporting
        TUint32 iRTCP;
        
        // SIP VoIP User Agent header: terminal type display
        TUint32 iSIPVoIPUAHTerminalType;
        
        // SIP VoIP User Agent header WLAN MAC address display 
        TUint32 iSIPVoIPUAHeaderWLANMAC;
        
        // SIP VoIP User Agent header string
        TBuf<KMaxSettingsLength32> iSIPVoIPUAHeaderString;

        //Profile locked to IAP
        TOnOff iProfileLockedToIAP;
        
        //VoIP plugin UID
        TInt32 iVoIPPluginUID;

        // Allow VoIP over WCDMA
        TOnOff iAllowVoIPoverWCDMA;

        // Allow VoIP over Bluetooth
        TOnOff iAllowVoIPoverBT;

        // Meaningfull count of VoIP digits for call identification
        TInt32 iMeanCountOfVoIPDigits;

        // Ignore address domain part for VoIP calls
        TInt32 iIgnoreAddrDomainPart;

        // Handover dialect
        TInt32 iHandoverDialect;

        // PS Telephony handover preference
        TInt32 iPSTelephonyHOPreference;

        //Handover threshold value lower level 
        TInt32 iHOThresholdValueLL;

        // Handover threshold value higher level
        TInt32 iHOThresholdValueHL;

        // Number of measurements (above value)
        TInt32 iNumberOfMeasurementsAbove;

        // Number of measurements (below value)
        TInt32 iNumberOfMeasurementsBelow;

        // Smart scanning interval parameter high
        TInt32 iSmartScannInterParaHigh;

        // Smart scanning interval parameter medium
        TInt32 iSmartScannInterParaMedium;

        // Smart scanning interval parameter low
        TInt32 iSmartScannInterParaLow;

        // Smart scanning interval parameter: static
        TInt32 iSmartScannInterParaStatic;

        // Smart scanning duration in high mode
        TInt32 iSmartScannDurationHighMode;

        // Smart scanning duration in medium mode
        TInt32 iSmartScannDurationMediumMode;

        // Smart scanning duration in low mode
        TInt32 iSmartScannDurationLowMode;

        // Handoff number (CS->PS)
        TInt32 iHandoffNumber;

        // Handback number (PS -> CS)
        TInt32 iHandbackNumber;

        // Hysterisis timer
        TInt32 iHysterisisTimer;

        // Hand off/back process timer.
        TInt32 iHandOffProcessTimer;

        // Disconnect process timer
        TInt32 iDisconnectProcessTimer;

        // Handoff reroute prefix.
        TBuf<KMaxSettingsLength32> iHandoffPrefix;

        // Handback reroute prefix.
        TBuf<KMaxSettingsLength32> iHandbackPrefix;

        // Handover tones on/off
        TInt32 iHandoverTones;

        // Support for SMS over WLAN
        TInt32 iSupportSMSoverWLAN;

        // Service provider ID
        TUint32 iServiceProviderId;

        // URI's user=phone parameter
        TOnOff iUserPhoneUriParameter;

        // Connectivity test call address in registering
        TBuf<KMaxSettingsLength32> iSIPConnTestAddress;

        // NAT/FW Domain specific settings storage ID (CenRep key)
        TUint32 iNATSettingsStorageId;

        // The minimum allowed value for the session timer
        TInt32 iSIPMinSE;

        // The session interval for a SIP session
        TInt32 iSIPSessionExpires;

        // Used NAT protocol
        // STUN = 0x00000000, TURN = 0x00000001, ICE = 0x00000002
        TInt32 iNATProtocol;

        // Variable which indicates if Service table entry needs to be created
        // during a new VoIP profile creation. New service entry will be created
        // if value is EOONotSet or EOn. Otherwise existing service is used if
        // iServiceProviderId is set and valid and it's not used by any other
        // voip profile.
        TOnOff iNewServiceTableEntry;

        // Destination network id
        TUint32 iSNAPId;

         // Account creation URL. Used when service contains no login information
         // and the user wants to create a new account on the provider web page.
         TBuf<KMaxSettingsUrlLength> iAccountCreationUrl;
    };

#endif      // CRCSEPROFILEENTRY_H   
            
// End of File
