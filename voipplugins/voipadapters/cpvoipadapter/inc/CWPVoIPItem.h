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
* Description:  Holds and stores one VoIP item.
*
*/


#ifndef CWPVOIPITEM_H
#define CWPVOIPITEM_H

//  INCLUDES
#include <e32base.h> // CBase.
#include <crcseprofileentry.h> // TOnOff, RCSE entry
#include "CWPVoIPCodec.h" // VoIP codecs.

// CONSTANTS
const TUint KMaxTerminalTypeLength = 64;
const TUint KWlanMacAddressLength  = 50;
const TUint KPhoneModelLength      = 20;
const TUint KUAHLength             = 200;

// MACROS

// DATA TYPES
typedef CRCSEProfileEntry VoIPProfileEntry;

// FORWARD DECLARATIONS
class CWPCharacteristic;
class CWPVoIPCodec;
class CCoeEnv;

// CLASS DECLARATION
/**
 *  This class keeps transiently the VoIP setting values and stores the 
 *  values via interface provided by RCSE and SPS.
 *
 *  @lib CWPVoIPAdapter.lib
 *  @since S60 3.0
 */
class CWPVoIPItem : public CBase
    {    
    #ifdef _DEBUG
    /**
     * Friend class for unit testing.
     */
    friend class UT_CWPVoIPItem;
    friend class UT_CWPVoIPAdapter;
    #endif

    public: // Data types 
        
        /**  For setting telephony preference */
        enum TTelephonyPreference
            {
            ECSPreferred,
            EPSPreferred
            };

        /** For telling which settings storage ID is related to. */
        enum TSettingsType
            {
            ESIP,
            ESCCP,
            ENATFW,
            EPresence,
            ESNAP
            };

    public: // Constructor and destructor
        
        static CWPVoIPItem* NewL();
        virtual ~CWPVoIPItem();

    public: // New functions

        /**
         * Setter for AppRef.
         *
         * @since S60 3.2
         * @param aAppRef.
         */
        void SetAppRefL( const TDesC8& aAppRef );

        /**
         * Getter for AppRef.
         *
         * @since S60 3.2
         * @return AppRef.
         */        
        HBufC8* AppRef();

        /**
         * Setter for provider.
         *
         * @since S60 3.0
         * @param aPROVIDERID.
         */
        void SetProviderIdL( const TDesC& aPROVIDERID );

        /**
         * Setter for user viewable name of the setting.
         *
         * @since S60 3.0
         * @param aNAME - value is TCP or UDP.
         */
        void SetNameL( const TDesC& aNAME );

        /**
         * Setter for TO-APPREF parameter values.
         *
         * @since S60 3.0
         * @param aTOAPPREF (i.e.actual CP parameter TO-APPREF).
         */
        void AddToAppRefL( const TDesC8& aTOAPPREF );

        /**
         * Setter for start media port.
         *
         * @since S60 3.0
         * @param aSMPORT.
         */
        void SetStartMediaPort( TInt aSMPORT );

        /**
         * Setter for end media port.
         *
         * @since S60 3.0
         * @param aEMPORT.
         */
        void SetEndMediaPort( TInt aEMPORT );

        /**
         * Setter for media QoS.
         *
         * @since S60 3.0
         * @param aMediaQoSValue.
         */
        void SetMediaQoS( TInt aMEDIAQOS );

        /**
         * Setter for switching DTMF in-band on/off.
         *
         * @since S60 3.0
         * @param aDTMFInBand (derived from CP parameter NODTMFIB, where
         * "NO" means negation).
         */
        void SetDTMFInBand( TInt aDTMFInBand );

        /**
         * Setter for DTMF out-of-band parameter.
         *
         * @since S60 3.0
         * @param aDTMFOutBandValue (derived from CP parameter 
         * NODTMFOOB, where "NO" means negation).
         */
        void SetDTMFOutBand( TInt aDTMFOutBandValue );

        /**
         * Setter for VoIP codecs.
         * Takes ownership of aVoIPCodec.
         *
         * @since S60 3.0
         * @param aVoIPCodec.
         */
        void AddCodecL( const CWPVoIPCodec* aVoIPCodec );

        /**
         * Setter for iSecureCallPreference.
         *
         * @since S60 3.0
         * @return aSecureCallPreference.
         */
        void SetSecureCallPref( TInt aSecureCallPref );

        /**
         * Setter for iRTCP.
         *
         * @since S60 3.0
         * @param aRTCP.
         */
        void SetRTCP( TInt aRTCP );

        /**
         * Setter for iUAHTerminalType.
         *
         * @since S60 3.0
         * @param aUAHTerminalType SIP User Agent Header:
         *                         terminal type display.
         */
        void SetUAHTerminalType( TInt aUAHTerminalType );

        /**
         * Setter for iUAHWLANMAC.
         *
         * @since S60 3.0
         * @param aUAHWLANMAC SIP User Agent Header: WLAN MAC address display.
         */
        void SetUAHWLANMAC( TInt aUAHWLANMAC );

        /**
         * Setter for iUAHString.
         *
         * @since S60 3.0
         * @param aUAHString SIP User Agent Header: free string.
         */
        void SetUAHStringL( const TDesC& aUAHString );

        /**
         * Setter for iProfileLockedToIAP.
         *
         * @since S60 3.0
         * @param aProfileLockedToIAP Is profile locked to a pre-defined IAP.
         */
        void SetProfileLockedToIAP( TInt aProfileLockedToIAP );

        /**
         * Setter for iVoIPPluginUid.
         *
         * @since S60 3.0
         * @param aVoIPPluginUid VoIP plugin UID.
         */
        void SetVoIPPluginUid( TInt aVoIPPluginUid );

        /**
         * Setter for iAllowVoIPOverWCDMA.
         *
         * @since S60 3.0
         * @param aAllowVoIPOverWCDMA Allow VoIP over WCDMA.
         */
        void SetAllowVoIPOverWCDMA( TInt aAllowVoIPOverWCDMA );

        /**
         * Setter for iVoIPDigits.
         *
         * @since S60 3.0
         * @param aVoIPDigits Number of meaningful VoIP digits for call
         *                    identification.
         */
        void SetVoIPDigits( TInt aVoIPDigits );

        /**
         * Setter for iDomainPartIgnoreRule.
         *
         * @since S60 3.0
         * @param aDomainPartIgnoreRule Rule for ignoring domain part of URI 
         *                              for VoIP calls.
         */
        void SetDomainPartIgnoreRule( TInt aDomainPartIgnoreRule );

        /**
         * Setter for iAddUserPhoneToAllNumbers.
         *
         * @since S60 3.2
         * @param aAddUserPhone The indication whether or not to add 
         *        user=phone to all numbers.
         */
        void SetAddUserPhone( TInt32 aAddUserPhone );

        /**
         * Setter for iSIPConnTestAddress.
         *
         * @since S60 3.2
         * @param aSIPConnTestAddress Connectivity test call address 
         *        in registering.
         */
        void SetSipConnTestAddressL( const TDesC& aSIPConnTestAddress );

         /**
          * Setter for iServiceProviderBookmark.
          *
          * @since S60 3.2
          * @param aServiceProviderBookmark Absolute URI for service 
          *        provider bookmark.
          */         
        void SetServiceProviderBookmarkL( 
            const TDesC& aServiceProviderBookmark );

        /**
         * Setter for iSIPMinSE.
         *
         * @since S60 3.2
         * @param aSIPMinSE The minimum allowed value for the session timer.
         */
        void SetSipMinSe( TInt32 aSIPMinSE );

        /**
         * Setter for iSIPSessionExpires.
         *
         * @since S60 3.2
         * @param aSIPMinSE The session interval for a SIP session.
         */
        void SetSipSessionExpires( TInt32 aSIPSessionExpires );

        /**
         * Setter for iIPVoiceMailBoxURI.
         *
         * @since S60 3.
         * @param aIPVoiceMailBoxURI IP Voice Mail Box address.
         */
        void SetIPVoiceMailBoxURIL( const TDesC& aIPVoiceMailBoxURI );

        /**
         * Setter for iVmbxListenAddress.
         *
         * @since S60 3.2
         * @param aVmbxListenAddress VoiceMailBox listening address.
         */
        void SetVoiceMailBoxListenURIL( const TDesC& aVmbxListenAddress );

        /**
         * Setter for iReSubscribeInterval.
         *
         * @since S60 3.2
         * @param aReSubscribeInterval Interval for sending re-SUBSCRIBE.
         */
        void SetReSubscribeInterval( TInt32 aReSubscribeInterval );

        /**
         * Setter for iBrandingDataAddress.
         *
         * @since S60 3.2
         * @param aBrandingDataAddress URI to branding data xml.
         */
        void SetBrandingDataAddressL( const TDesC& aVmbxListenAddress );

        /**
         * Setter for iAutoAcceptBuddy.
         *
         * @since S60 3.2
         * @param aAutoAcceptBuddy for defining auto accept buddy presence
         *        subscription request.
         */
        void SetAutoAcceptBuddyRequest( TInt32 aAutoAcceptBuddyRequest );

        /**
         * Setter for iAutoEnableService.
         *
         * @since S60 3.2
         * @param aAutoEnableService for defining 
         *        auto enable state of service.
         */
        void SetAutoEnableService( TInt32 aAutoEnableService );

        /**
         * Setter for NAPDEF (IAP settings).
         *
         * @since S60 3.2
         * @param aNapDef NAPDEF (IAP settings) characteristic.
         */
        void SetNapDef( CWPCharacteristic* aNapDef );

        /**
         * Setter for TO-NAPID parameter.
         *
         * @since S60 3.2
         * @param aNapDefName TO-NAPID value.
         */
        void SetToNapIdL( const TDesC8& ToNapId );

        /**
         * Setter for IAP ID.
         *
         * @since S60 3.2
         * @param aIapId IAP ID.
         */
        void SetIapId( TUint32 aIapId );

        /**
         * Setter for storage ID's (APPREF/TO-APPREF linkage).
         *
         * @since S60 3.2
         * @param aAppId APPID value.
         * @param aStorageId Storage ID.
         * @return ETrue if storage ID was set,
                   EFalse if storage ID was not set.
         */
        TBool SetStorageId( TSettingsType aProfileType, TUint32 aStorageId, 
            const TDesC8& aAppRef );

        /**
         * Sets TO-APPREF value to internal variable.
         *
         * @since S60 3.2
         * @param aToAppRef TO-APPREF value.
         */
        void SetVoipUrisToAppRefL( const TDesC8& aToAppRef );

        /**
         * Setter for iUsedNatProtocol.
         *
         * @since S60 3.2
         * @param aUsedNatProtocol for indicating which NAT protocol should
         *        be used.         
         */
         void SetUsedNatProtocol( TInt32 aUsedNatProtocol);

        /**
         * Getter for user viewable name of VoIP settings item.
         *
         * @since S60 3.0
         * @return Settings name.
         */
        const TDesC& Name() const; 
        
        /** 
         * Getter for iUAHString.
         *
         * @since S60 3.0
         * @return SIP VoIP User Agent header: free string.
         */
        const TDesC& UAHString() const;

        /**
         * Getter for NAPDEF (IAP settings).
         *
         * @since S60 3.2
         * @return NAPDEF characteristic.
         */
        CWPCharacteristic* NapDef();

        /**
         * Getter for TO-NAPID value.
         *
         * @since S60 3.2.
         * @return TO-NAPID value.
         */
        const TDesC8& ToNapId() const;

        /**
         * Returns TO-APPREF value.
         *
         * @since S60 3.2
         * @return TO-APPREF value.
         */
        const TDesC8& VoipUrisToAppRef() const;

        /**
         * Saves the VoIP settings and returns the uid of the stored new VoIP 
         * profile.
         *
         * @since S60 3.0
         * @return VoIP profile ID.
         */
        TUint StoreL();

        /**
         * Return profile ID as a descriptor.
         *
         * @since 3.0
         * @return ID of the VoIP item (VoIP profile ID).
         */
        const TDesC8& SaveData() const;

        /**
         * Make update for data that is received after the actual StoreL save 
         * method call. Data received after that can be: SIP profile ids in 
         * VoIP profile settings and in the SIP specific VoIP settings items 
         * and SCCP profile ids. SIP and SCCP ids are always received from the
         * other adapters via CWPAdapter::SettingsSaved mehod that can be
         * called after call to StoreL.
         *
         * @since S60 3.0
         */
        void SavingFinalizedL();

        /**
         * Sets CS/PS telephony preference.
         *
         * @since S60 3.0
         * @param aTelephonyPreference state to set
         */
        void SetTelephonyPreferenceL( const TTelephonyPreference& 
            aTelephonyPreference ) const;

        /**
         * Gather SIP User-Agent header.
         *
         * @since S60 3.0
         * @param aUserAgentHeader User Agent Header.
         */
        void GetUserAgentHeaderL( TBuf8<KUAHLength>& aUserAgentHeader ) const;

        /**
         * Getter for RCSE storage ID.
         *
         * @since S60 3.2
         * @return RCSE storage ID.
         */
        TUint32 ItemId();

        /**
         * Sets IM enabled flag to true.
         *
         * @since S60 3.2
         */
        void EnableIm();

    private:

        CWPVoIPItem();
        void ConstructL();

    private: // New functions

        /**
         * Check for duplicate provider and settingsname and renames if same.
         *
         * @since S60 3.0
         * @param aProfile Profile to check.
         * @return ETrue if operation was done; otherwise EFalse.
         */
        TBool CheckDuplicatesL( CRCSEProfileEntry& aEntry ) const;

        /**
         * Get Terminal Type for SIP User Agent Header.
         *
         * @since S60 3.0
         * @param aTeminalType Terminal type.
         */
        void GetTerminalTypeL( 
            TBuf<KMaxTerminalTypeLength>& aTerminalType ) const;

        /**
         * Get WLAN MAC address for SIP User Agent Header.
         *
         * @since S60 3.0
         * @param aMac WLAN MAC address.
         */
        void GetWlanMacAddressL( TBuf<KWlanMacAddressLength>& aMac ) const;

        /**
         * Cleanup method for array.
         *
         * @since S60 3.1
         * @param aParameter Parameter to be cleaned.
         */
        static void CleanupArrayItem( TAny* aArray );

        /**
         * Creates default codecs.
         *
         * @since S60 3.2
         * @param aProfile Profile to append new codecs.
         */
        void AddDefaultCodecsL( CRCSEProfileEntry& aProfileEntry );

    private: // Data.

        /*
         * VoIP codecs.
         * Own.
         */
         RPointerArray<CWPVoIPCodec> iCodecs;

        /*
         * Identifies the item.
         */
        TUint iItemId;

        /* Provider of settings. See OMA ProvCont document.
         * CP parameter: PROVIDER-ID.
         * Own.
         */
        HBufC* iProviderId;

        /* Name of VoIP settings visible to user when viewing the provisioning
         * message. See OMA ProvCont document.
         * CP parameter: NAME
         * Own.
         */
        HBufC* iName;

        /* All APPREFs that are referenced from this VoIP item's TO-APPREF 
         * parameter.
         * Own.
         */
        RPointerArray<HBufC8> iToAppRefs;

        /* Start media port.See w9013.txt or VoIP software settings document.
         * CP parameter: SMPORT.
         */
        TInt32 iStartMediaPort;

        /* End media port.See w9013.txt or VoIP software settings document.
         * CP parameter: EMPORT.
         */
        TInt32 iEndMediaPort;

        /* Quality of service for the media.See w9013.txt or VoIP software 
         * settings document.
         * CP parameter: MEDIAQOS.
         */
        TInt32 iMediaQoS;

        /* In band. See w9013.txt or VoIP software settings document.
         * CP parameter: NODTMFIB
         */
        TInt32 iDTMFInBand;

        /* Out-of-band. See w9013.txt or VoIP software settings document.
         * CP parameter: NODTMFOOB.
         */
        TInt32 iDTMFOutBand;

        /* VoIP id as descriptor to be returned by the SaveData method to the 
         * framework via adapter SaveDataL.
         * Own.
         */
        HBufC8* iSavedProfileId;

        /**
         * Secure call preference. See w9013.txt or VoIP software settings
         * document.
         */
        TInt iSecureCallPref;

        /**
         * RTCP, Real-Time Control Protocol reporting. See w9013.txt or VoIP
         * software settings document.
         */
        TInt iRTCP;

        /**
         * SIP VoIP User Agent header: terminal type display. See w9013.txt or
         * VoIP software settings document.
         */
        TInt iUAHTerminalType;

        /**
         * SIP VoIP User Agent header WLAN MAC address display. See w9013.txt
         * or VoIP software settings document.
         */
        TInt iUAHWLANMAC;

        /**
         * SIP VoIP User Agent header: free string. See w9013.txt or VoIP
         * software settings document.
         * Own.
         */
        HBufC* iUAHString;

        /**
         * Profile locked to IAP. See w9013.txt or VoIP software settings
         * document.
         */
        TInt iProfileLockedToIAP;

        /**
         * VoIP plugin UID. See w9013.txt or VoIP software settings document.
         */
        TInt iVoIPPluginUid;

        /**
         * Allow VoIP over WCDMA. See w9013.txt or VoIP software settings
         * document.
         */
        TInt iAllowVoIPOverWCDMA;

        /**
         * Number of meaningful VoIP digits for call identification.
         * See w9013.txt.
         */
        TInt iVoIPDigits;

        /**
         * Ignore domain part of address (URI) for VoIP calls rule.
         * See w9013.txt.
         */
        TInt iDomainPartIgnoreRule;

        /**
         * Flag to prevent set vmbx sip id if no vmbx settings are used.         
         */
        TBool iVmbxSettingsExist;

        /**
         * Whether or not to add user=phone to all numbers.
         * See w9013.txt.
         */
        TInt32 iAddUserPhoneToAllNumbers;

        /**
         * Connectivity test call address in registering.
         * See w9013.txt.
         * Own.
         */   
        HBufC* iSIPConnTestAddress;

        /**
         * Absolute URI for service provider bookmark.
         * See w9013.txt.
         * Own.
         */   
        HBufC* iServiceProviderBookmark;

        /**
         * The minimum allowed value for the session timer.
         * See w9013.txt.
         */
        TInt32 iSIPMinSE;

        /**
         * The session interval for a SIP session.
         * See w9013.txt.
         */
        TInt32 iSIPSessionExpires;

        /**
         * VoiceMailBox: MWI-URI.
         * See w9013.txt.
         * Own.
         */
        HBufC* iIPVoiceMailBoxURI;

        /**
         * VoiceMailBox: VoiceMailBox listening address.
         * See w9013.txt.
         * Own.
         */
        HBufC* iVmbxListenAddress;

        /**
         * VoiceMailBox: Interval for sending re-SUBSCRIBE.
         * See w9013.txt.
         */
        TInt32 iReSubscribeInterval;

        /**
         * VoiceMailBox: NAPDEF.
         * See w9013.txt.
         * Not own.
         */
        CWPCharacteristic* iNapDef;

        /**
         * VoiceMailBox: IAP ID.
         * See w9013.txt.
         */
        TUint32 iVmbxIapId;

        /**
         * URI to branding data xml.
         * See w9013.txt
         * Own.
         */   
        HBufC* iBrandingDataAddress;

        /**
         * Auto accept buddy presence subscription request.
         * see w9013.txt.
         */
        TInt32 iAutoAcceptBuddyRequest;

        /**
         * Auto enable state of service.
         * see w9013.txt.
         */
        TInt32 iAutoEnableService;

        /**
         * TO-NAPID value.
         * See w9013.txt
         * Own.
         */
        HBufC8* iToNapId;

        /**
         * SIP profile ID for VoIP profile.
         */
        TUint32 iSipId;

        /**
         * SCCP profile ID for VoIP profile.
         */
        TUint32 iSccpId;

        /**
         * SIP profile ID for VoiceMailBox.
         */
        TUint32 iVmbxSipId;

        /**
         * NATFW profile ID for VoIP profile.
         */
        TUint32 iNatFwId;

        /**
         * Presence settings ID for service settings.
         */
        TUint32 iPresenceId;

        /**
         * Boolean for telling if SIP ID is already set.
         */
        TBool iSipIdSet;

        /**
         * Boolean for telling if SCCP ID is already set.
         */
        TBool iSccpIdSet;

        /**
         * Boolean for telling if VMBX SIP ID is already set.
         */
        TBool iVmbxSipIdSet;

        /**
         * Boolean for telling if NATFW ID is already set.
         */
        TBool iNatFwIdSet;

        /**
         * Boolean for telling if Presence ID is already set.
         */
        TBool iPresenceIdSet;

        /**
         * TO-APPREF value under characteristic VOIPURIS.
         * Own.
         */
        HBufC8* iVoipUrisToAppRef;

        /**
         * Indicates which NAT protocol should be used.
         * see w9013.txt.
         */
        TInt32 iUsedNatProtocol;

        /**
         * Boolean for telling if SNAP ID is already set.
         */
        TBool iSnapIdSet;

        /**
         * SNAP profile ID for VoIP profile.
         */
        TUint32 iSnapId;        

        /**
         * APPREF of this settings item.
         * Own.
         */
        HBufC8* iAppRef;

        /**
         * Tells if IM is enabled or not.
         */
        TBool iImEnabled;
    };

#endif      // CWPVOIPITEM_H
            
// End of File
