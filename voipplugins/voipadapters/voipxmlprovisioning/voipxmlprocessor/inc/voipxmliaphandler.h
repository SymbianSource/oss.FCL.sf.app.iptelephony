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
* Description:  IAP handler for VoIP XML processor
*
*/


#ifndef  VOIPXMLIAPHANDLER_H
#define  VOIPXMLIAPHANDLER_H

#include "voipxmlprocessordefaults.h"

class RCmManagerExt;

class TWepKey
    {
public:
    /**
     * ETrue if hexadecimal, EFalse if ASCII.
     */
    TBool iHex;
    /**
     * WEP key length.
     */
    TInt iLength;
    /**
     * WEP key data.
     */
    TBuf<KMaxWepKeyDataLength> iData;
    };

struct TTemporaryIap
    {
    /**
     * Name of the IAP.
     */
    HBufC* iName;
    /**
     * SSID of the IAP.
     */
    HBufC* iSsid;
    /**
     * Tells if the SSID is hidden or not.
     */
    TBool iHidden;
    /**
     * Network mode.
     */
    TInt iNetworkMode;
    /**
     * Network type.
     */
    TInt iSecurityType;
    /**
     * Pre-shared key.
     */
    HBufC* iPreSharedKey;
    /**
     * WEP authentication mode.
     */
    TInt iWepAuthMode;
    /**
     * Current WEP key.
     */
    TWepKey iCurrentWepKey;
    /**
     * Array of WEP keys.
     */
    RArray<TWepKey> iWepKeys;
    };

/**
*  VoipXmlIapHandler
*  IAP handler class.
* 
*  @lib voipxmlprocessor.lib
*  @since S60 v5.0
*/
class CVoipXmlIapHandler : public CBase
    {
#ifdef _DEBUG
    friend class UT_CVoipXmlIapHandler;
    friend class UT_CVoipXmlParamHandler;
#endif

public:

    static CVoipXmlIapHandler* NewL();
    ~CVoipXmlIapHandler();

public:

    /**
     * Sets IAP setting.
     *
     * @since S60 v5.0
     * @param aType Type of settings.
     * @param aParam Parameter to be set.
     * @param aValue Value of the setting.
     */
    void SetSetting( TInt aType, TInt aParam, const TDesC& aValue );

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
     * Returns destination ID.
     *
     * @since S60 v5.0
     * @return Destination ID.
     */
    TUint32 SettingsId();

    /**
     * Method to inform the handler that currently deployed settings
     * have ended. This way we will know when an IAP is ready to be
     * appended to iIaps array, or when a WEP key is ready to appended
     * to iWepKeys array.
     *
     * @since S60 v5.0
     * @param aType Type of settings.
     */
    void SettingsEnd( TInt aType );

private:

    /**
     * Resets iCurrentIap settings, i.e. deletes its member data.
     *
     * @since S60 v5.0
     * @param aCloseArray Tells if array inside iCurrentIap is to be closed.
     */
    void ResetCurrentIap( TBool aCloseArray = EFalse );

    /**
     * Resets iIaps array, i.e. deletes its items member data.
     *
     * @since S60 v5.0
     * @param aCloseArray Tells if arrays inside iIaps' 
     *                    members are to be closed.
     */
    void ResetTempIapArray( TBool aCloseArray = EFalse );

    /**
     * Adds current IAP settings to iIaps array.
     *
     * @since S60 v5.0
     */
    void AddCurrentIapL();

    /**
     * Stores settings.
     *
     * @since S60 v5.0
     */
    void StoreSettingsL();

    /**
     * Creates an access point.
     * 
     * @since S60 v5.0
     * @param aCmManager CMManager reference.
     * @param aTempIap TTemporaryIap from which actual IAP will be created.
     * @return IAP ID.
     */
    TUint32 CreateIapL( RCmManagerExt& aCmManager, TTemporaryIap aTempIap );
private:

    CVoipXmlIapHandler();
    void ConstructL();

private:

    /**
     * Tells if any settings have been set to temporary profile.
     */
    TBool iSettingsSet;

    /**
     * Name of destination. If this is empty when saving settings,
     * destination will not be created.
     * Own.
     */
    HBufC* iDestinationName;

    /**
     * ID of destination.
     */
    TUint32 iDestinationId;

    /**
     * Access point that is being currently handled.
     */
    TTemporaryIap iCurrentIap;

    /**
     * Array for temporary IAP's.
     */
    RPointerArray<TTemporaryIap> iIaps;
    };

#endif // VOIPXMLIAPHANDLER_H

// End of file.
