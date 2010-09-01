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
* Description:  Presence handler for VoIP XML processor
*
*/


#ifndef  VOIPXMLPRESENCEHANDLER_H
#define  VOIPXMLPRESENCEHANDLER_H

#include <pressettingsset.h>

class TPresSettingsSet;

/**
*  VoipXmlPresenceHandler
*  Presence handler class.
* 
*  @lib voipxmlprocessor.lib
*  @since S60 v5.0
*/
class CVoipXmlPresenceHandler : public CBase
    {
#ifdef _DEBUG
    friend class UT_CVoipXmlPresenceHandler;
    friend class UT_CVoipXmlParamHandler;
#endif

public:

    static CVoipXmlPresenceHandler* NewL();
    ~CVoipXmlPresenceHandler();

public:

    /**
     * Sets XDM setting.
     *
     * @since S60 v5.0
     * @param aParam Parameter to be set.
     * @param aValue Value of the setting.
     */
    void SetSetting( TInt aParam, const TDesC& aValue );

    /**
     * Stores settings to 'XDM registry', i.e. creates an XDM collection.
     *
     * @since S60 v5.0
     * @return KErrNone if successful,
     *         KErrNotSupported if no settings to be stored,
     *         KErrCompletion if settings could not be stored.
     */
    TInt StoreSettings();

    /**
     * Returns the settings ID.
     *
     * @since S60 v5.0
     * @return XDM settings ID.
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
     * Finalizes settings, i.e. stores the profile with linkage information.
     *
     * @since S60 v5.0
     * @return KErrNone if successful,
     *         KErrGeneral if settings could not be finalized.
     */
    TInt FinalizeSettings();

private:

    /**
     * Creates a provider name for Presence profile. aName is used
     * and replaced with a parenthised ordered number if the name is
     * already found in Presence storage. 
     *
     * @since S60 v5.0
     * @param aName Provider name to be set.
     */
    void CreateProviderNameL( TDes& aName );

private:

    CVoipXmlPresenceHandler();
    void ConstructL();

private:

    /**
     * XDM settings collection object.
     * Own.
     */
    TPresSettingsSet iProfile;

    /**
     * Tells if any settings have been set to temporary profile.
     */
    TBool iSettingsSet;

    /**
     * XDM collection ID.
     */
    TUint32 iProfileId;
    };

#endif // VOIPXMLPRESENCEHANDLER_H

// End of file.
