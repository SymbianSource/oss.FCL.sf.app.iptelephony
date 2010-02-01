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
* Description:  NAT/Firewall handler for VoIP XML processor
*
*/


#ifndef  VOIPXMLNATFWHANDLER_H
#define  VOIPXMLNATFWHANDLER_H

#include "voipxmlprocessordefaults.h"

struct TAdditionalStun
    {
//public:

    /**
     * STUN server address.
     */
    TBuf8<KMaxNodeValueLength> iStunSrvAddr;

    /**
     * STUN server port.
     */
    TInt iStunSrvPort;

    /**
     * STUN server username.
     */
    TBuf8<KMaxNodeValueLength> iStunSrvUsername;

    /**
     * STUN server password.
     */
    TBuf8<KMaxNodeValueLength> iStunSrvPassword;
    };

/**
*  VoipXmlNatFwHandler
*  NAT/Framwrork handler class.
* 
*  @lib voipxmlprocessor.lib
*  @since S60 v5.0
*/
class CVoipXmlNatFwHandler : public CBase
    {
#ifdef _DEBUG
    friend class UT_CVoipXmlNatFwHandler;
    friend class UT_CVoipXmlParamHandler;
#endif

public:

    static CVoipXmlNatFwHandler* NewL();
    ~CVoipXmlNatFwHandler();

public:

    /**
     * Sets NAT/Firewall setting.
     *
     * @since S60 v5.0
     * @param aParam Parameter to be set.
     * @param aValue Value of the setting.
     */
    void SetSetting( TInt aType, TInt aParam, const TDesC8& aValue );

    /**
     * Stores settings to Central Repository (actually calls StoreSettingsL).
     *
     * @since S60 v5.0
     * @return KErrNone if successful,
     *         KErrNotSupported if no settings to be stored,
     *         KErrCompletion if settings could not be stored.
     */
    TInt StoreSettings();

    /**
     * Method to inform the handler that currently deployed settings
     * have ended. This way we will know when iCurrentAdditionalStunServer
     * server should be appended to iAdditionalStunServers array.
     *
     * @since S60 v5.0
     * @param aType Type of settings.
     */
    void SettingsEnd( TInt aType );

private:

    /**
     * Commits actual Central Repository storage.
     *
     * @since S60 v5.0
     */
    void StoreSettingsL();

private:

    CVoipXmlNatFwHandler();
    void ConstructL();

private:

    /**
     * Tells if any settings have been set to internal members.
     */
    TBool iSettingsSet;

    /**
     * Domain name.
     * Own.
     */
    HBufC8* iDomain;

    /**
     * STUN server address.
     * Own.
     */
    HBufC8* iStunSrvAddr;

    /**
     * STUN server port.
     */
    TInt iStunSrvPort;

    /**
     * TCP refresh interval.
     */
    TInt iTcpRefreshInterval;

    /**
     * UDP refresh interval.
     */
    TInt iUdpRefreshInterval;

    /**
     * CRLF refresh.
     */
    TInt iCrlfRefresh;

    /**
     * STUN server username.
     * Own.
     */
    HBufC8* iStunSrvUsername;

    /**
     * STUN server password.
     * Own.
     */
    HBufC8* iStunSrvPassword;

    /**
     * STUN shared secret.
     */
    TInt iStunSharedSecret;

    /**
     * Start port of STUN port range.
     */
    TInt iStartPortRange;

    /**
     * End port of STUN port range.
     */
    TInt iEndPortRange;

    /**
     * NAT protocol
     * Own.
     */
    HBufC8* iNatProtocol;

    /**
     * Additional STUN server data that is being currently modified.
     * After modifications are done, it is appended to iAdditionalStunServers.
     */
    TAdditionalStun iCurrentAdditionalStunServer;

    /**
     * Array for holding additional STUN server data.
     */
    RPointerArray<TAdditionalStun> iAdditionalStunServers;
    };

#endif // VOIPXMLNATFWHANDLER_H

// End of file.
