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
* Description:  SIP handler for VoIP XML processor
*
*/


#ifndef  VOIPXMLSIPHANDLER_H
#define  VOIPXMLSIPHANDLER_H

#include <sipprofileregistryobserver.h>
#include <sipprofiletypeinfo.h>

class CSIPManagedProfile;
class CSIPManagedProfileRegistry;
class CSIPProfile;
class TSIPProfileTypeInfo;

/**
*  VoipXmlSipHandler
*  SIP handler class.
* 
*  @lib voipxmlprocessor.lib
*  @since S60 v5.0
*/
class CVoipXmlSipHandler : public CBase,
                           public MSIPProfileRegistryObserver
    {
#ifdef _DEBUG
    friend class UT_CVoipXmlSipHandler;
    friend class UT_CVoipXmlParser;
    friend class UT_CVoipXmlParamHandler;
#endif

public:
    enum TTransportType
        {
        EAutomatic = 0,
        EUdp,
        ETcp
        };
public:

    static CVoipXmlSipHandler* NewL();
    ~CVoipXmlSipHandler();

public:

    /**
     * Sets SIP setting.
     *
     * @since S60 v5.0
     * @param aType Type of setting (SIP 'core', proxy or registrar)
     * @param aParam Parameter to be set.
     * @param aValue Value of the setting.
     */
    void SetSetting( TInt aType, TInt aParam, const TDesC8& aValue );

    /**
     * Stores settings to SIP managed profile registry.
     *
     * @since S60 v5.0
     * @return KErrNone if successful,
     *         KErrNotSupported if no settings to be stored,
     *         KErrCompletion if settings could not be stored.
     */
    TInt StoreSettings();

    /**
     * Returns the profile ID.
     *
     * @since S60 v5.0
     * @return SIP profile ID.
     */
    TUint32 SettingsId();

    /**
     * Gives linkage information.
     *
     * @since S60 v5.0
     * @param aType Type of settings.
     * @param aSettingsId Settings ID.
     */
    void LinkSettings( TInt aType, TUint32 aSettingsId );

    /**
     * Finalizes settings, i.e. sets Destination ID to SIP profile.
     *
     * @since S60 v5.0
     * @return KErrNone if successful,
     *         KErrCompletion if settings could not be finalized.
     */
    TInt FinalizeSettings();

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
     * Creates a provider name for SIP profile. aName is used
     * and replaced with a parenthised ordered number if the name is
     * already found in SIP stack. 
     *
     * @since S60 v5.0
     * @param aName Provider name to be set.
     */
    void CreateProviderNameL( const TDesC8& aName );

    /**
     * Handles public User ID modification if needed.
     * Possible modifications are escaping and adding sip: prefix.
     *
     * @since S60 v5.0
     * @param aValue Value to be checked and modified if needed.
     */
    void ModifyPuidL( TDes8& aValue );

    /**
     * Validates transport type.
     *
     * @since S60 v5.0
     * @param aValue Value to be validated.
     * @return Validated transport type.
     */
    TTransportType ValidateTransport( const TDesC8& aValue );

private:

    CVoipXmlSipHandler();
    void ConstructL();

private:

    /**
     * SIP profile object.
     * Own.
     */
    CSIPManagedProfile* iProfile;

    /**
     * SIP managed profile registry object.
     * Own.
     */
    CSIPManagedProfileRegistry* iRegistry;

    /**
     * Profile type.
     */
    TSIPProfileTypeInfo iProfileType;

    /**
     * Proxy URI.
     * Own.
     */
    HBufC8* iProxyUri;

    /**
     * Proxy transport type.
     */
    TTransportType iProxyTransport;

    /**
     * Proxy port number.
     */
    TInt iProxyPort;

    /**
     * Loose routing.
     */
    TBool iLr;

    /**
     * Registrar URI.
     * Own.
     */
    HBufC8* iRegistrarUri;

    /**
     * Registrar transport type.
     */
    TTransportType iRegistrarTransport;

    /**
     * Registrar server port number.
     */
    TInt iRegistrarPort;

    /**
     * Tells if any settings have been set to temporary profile.
     */
    TBool iSettingsSet;

    /**
     * SIP profile ID.
     */
    TUint32 iProfileId;
    };

#endif // VOIPXMLSIPHANDLER_H

// End of file.
