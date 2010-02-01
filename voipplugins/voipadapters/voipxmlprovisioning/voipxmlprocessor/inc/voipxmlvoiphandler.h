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
* Description:  VoIP handler for VoIP XML processor
*
*/


#ifndef VOIPXMLVOIPHANDLER_H
#define VOIPXMLVOIPHANDLER_H

#include <e32base.h>
#include <sipprofileregistryobserver.h>
#include <crcseprofileentry.h>
#include <crcseaudiocodecentry.h>

class CSPSettings;
class CRCSEProfileRegistry;
class CRCSEAudioCodecRegistry;
class CRCSEProfileEntry;
class CSPProperty;

typedef CRCSEAudioCodecEntry::TOnOff TCodecOnOff;
typedef CRCSEProfileEntry::TOnOff TEntryOnOff;

struct TTemporaryCodec
    {
    /**
     * Codec name.
     */
    HBufC* iName;
    /**
     * Jitter buffer size.
     */
    TInt iJitterBuffer;
    /**
     * Octet-align.
     */
    TCodecOnOff iOctetAlign;
    /**
     * PTime value.
     */
    TInt iPtime;
    /**
     * MaxPtime value.
     */
    TInt iMaxPtime;
    /**
     * Mode-set values.
     */
    RArray<TUint32> iModeSet;
    /**
     * Mode-change-period value.
     */
    TInt iModeChangePeriod;
    /**
     * Mode-change-neighbor value.
     */
    TCodecOnOff iModeChangeNeighbor;
    /**
     * Max-red value.
     */
    TInt iMaxRed;
    /**
     * VAD value.
     */
    TCodecOnOff iVad;
    /**
     * Annexb value.
     */
    TCodecOnOff iAnnexb;
    };

struct TSpSettings
    {
    /**
     * Tells if buddy requests are automatically accepted.
     */
    TBool iAutoAcceptBuddies;
    /**
     * Provider bookmark URL.
     */
    HBufC* iProviderUrl;
    /**
     * Branding URI.
     */
    HBufC* iBrandingUri;
    /**
     * Tells if the service will be automatically enabled.
     */
    TBool iAutoEnable;
    /**
     * Message Waiting Indicator URI for voice mailbox.
     */
    HBufC* iMwiUri;
    /**
     * Listening URI for voice mailbox.
     */
    HBufC* iListeningUri;
    /**
     * Re-subscribe interval for voice mailbox.
     */
    TInt iResubrcribe;
    /**
     * Tells if SIP IM is enabled. IM will be set as enabled only if 
     * a presence ID is received via LinkSettings() method.
     */
    TBool iEnableSipIm;
    /**
     * Brand ID.
     */
    HBufC* iBrandId;
    };

/**
*  VoIP handler class.
*  Handles VoIP related settings.
*
*  @lib voipxmlprocessor.lib
*  @since S60 v5.0
*/
class CVoipXmlVoipHandler : public CBase, public MSIPProfileRegistryObserver
    {
#ifdef _DEBUG
    friend class UT_CVoipXmlVoipHandler;
    friend class UT_CVoipXmlParser;
    friend class UT_CVoipXmlParamHandler;
#endif

public:

    /**
     * Two-phased constructor.
     */
    static CVoipXmlVoipHandler* NewL();

    /**
     * Destructor.
     */
    virtual ~CVoipXmlVoipHandler();

    /**
     * Sets VoIP setting.
     *
     * @since S60 v5.0
     * @param aType Type of setting (VoIP 'core', vmbx or codec)
     * @param aParam Parameter to be set.
     * @param aValue Value of the setting.
     */
    void SetSetting( TInt aType, TInt aParam, const TDesC& aValue );

    /**
     * Stores settings to RCSE.
     *
     * @since S60 v5.0
     * @return KErrNone if successful,
     *         KErrNotSupported if no settings to be stored,
     *         KErrCompletion if settings could not be stored.
     */
    TInt StoreSettings();

    /**
     * Returns the service ID.
     *
     * @since S60 v5.0
     * @return VoIP service ID.
     */
    TUint32 SettingsId();

    /**
     * Method to inform the handler that currently deployed settings
     * have ended. This way we will know when a codec is ready to be
     * put to RCSE.
     *
     * @since S60 v5.0
     * @param aType Type of settings.
     */
    void SettingsEnd( TInt aType );

    /**
     * Gives linkage information.
     *
     * @since S60 v5.0
     * @param aType Type of settings.
     * @param aSettingsId Settings ID.
     */
    void LinkSettings( TInt aType, TUint32 aSettingsId );

    /**
     * Finalizes settings, i.e. sets all the settings that are bound to
     * settings linkage (e.g. SIP User-Agent header and 
     * settings stored in SPSettings).
     *
     * @since S60 v5.0
     * @return KErrNone if successful,
     *         KErrCompletion if settings could not be finalized.
     */
    TInt FinalizeSettings();

    /**
     * Returns the service tab ID.
     *
     * @since S60 v5.0
     * @return Service tab ID.
     */
    TUint32 ServiceTabIdL();

protected:

// from base class MSIPProfileRegistryObserver

    /** 
     * From MSIPProfileRegistryObserver 
     * SIP profile information event.
     *
     * @since S60 v5.0
     * @param aSIPProfileId SIP profile ID.
     * @param aEvent Type of SIP event
     */
    void ProfileRegistryEventOccurred( TUint32 /*aSIPProfileId*/, 
        TEvent /*aEvent*/ );

    /**
     * From MSIPProfileRegistryObserver
     * An asynchronous error has occurred related to SIP profile.
     *
     * @since S60 v5.0
     * @param aSIPProfileId SIP profile ID.
     * @param aError Error code.
     */
    void ProfileRegistryErrorOccurred( TUint32 /*aSIPProfileId*/,
        TInt /*aError*/ );

private:

    /**
     * Sets VoIP core setting to temporary storage.
     *
     * @param aParam Parameter to be set.
     * @param aValue Value of the setting.
     * @since S60 v5.0
     */
    void SetCoreSettingL( TInt aParam, const TDesC& aValue );

    /**
     * Sets codec setting to temporary storage.
     *
     * @param aParam Parameter to be set.
     * @param aValue Value of the setting.
     * @since S60 v5.0
     */
    void SetCodecSettingL( TInt aParam, const TDesC& aValue );

    /**
     * Sets voice mailbox setting to temporary storage.
     *
     * @param aParam Parameter to be set.
     * @param aValue Value of the setting.
     * @since S60 v5.0
     */
    void SetVmbxSettingL( TInt aParam, const TDesC& aValue );

    /**
     * Validates profile name and modifies it if necessary.
     *
     * @since S60 v5.0
     * @param aName Name to be validated.
     */
    void ValidateProfileNameL( TDes& aName );

    /**
     * Sets temporary codec to RCSE.
     *
     * @since S60 v5.0
     */
    void SetCodecToRcseL();

    /**
     * Reset temporary codec settings (iCurrentCodec)
     *
     * @since S60 v5.0
     */
    void ResetTempCodec( TBool aCloseArray = EFalse );

    /**
     * Resets temporary service provider settings (iSpSettings)
     * 
     * @since S60 v5.0
     */
    void ResetTempSpSettings();

    /**
     * Sets SIP related VoIP settings (links VoIP profile to SIP and sets
     * SIP User-Agent header).
     * 
     * @since S60 v5.0
     * @param aSipId SIP settings ID.
     */
    void SetSipInfoL( TUint32 aSipId );

    /**
     * Sets SPSettings to service table.
     *
     * @since S60 v5.0
     */
    void SetSpSettingsL();

    /**
     * Adds default codec set to iEntry.
     *
     * @since S60 v5.0
     */
    void AddDefaultCodecsL();

private:

    CVoipXmlVoipHandler();
    void ConstructL();

private:

    /**
     * RCSE profile registry.
     * Own.
     */
    CRCSEProfileRegistry* iRegistry;

    /**
     * RCSE audio codec registry
     * Own.
     */
    CRCSEAudioCodecRegistry* iCodecRegistry;

    /**
     * RCSE profile entry.
     * Own.
     */
    CRCSEProfileEntry* iEntry;

    /**
     * Tells if any settings have been set to temporary profile.
     */
    TBool iSettingsSet;

    /**
     * VoIP service ID.
     */
    TUint32 iServiceId;

    /**
     * Current codec that is being modified.
     */
    TTemporaryCodec iCurrentCodec;

    /**
     * SPSettings that are temporarily stored and .
     */
    TSpSettings iSpSettings;

    /**
     * Presence settings ID. This is received in LinkSettings method and
     * will be stored to SPSettings in FinalizeSettings method.
     */
    TUint32 iPresenceId;

    /**
     * Destination (SNAP) ID. This is received in LinkSettings method and
     * will be stored to SPSettings in FinalizedSettings method.
     */
    TUint32 iDestinationId;
    };

#endif // VOIPXMLVOIPHANDLER_H

//  End of File
