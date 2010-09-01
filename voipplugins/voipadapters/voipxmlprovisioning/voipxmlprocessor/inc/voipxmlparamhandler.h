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
* Description:  Class definition of VoIP XML parameter handler.
*
*/


#ifndef VOIPXMLPARAMHANDLER_H
#define VOIPXMLPARAMHANDLER_H

#include <e32base.h>

class CVoipXmlSipHandler;
class CVoipXmlVoipHandler;
class CVoipXmlXdmHandler;
class CVoipXmlPresenceHandler;
class CVoipXmlNatFwHandler;
class CVoipXmlIapHandler;

/**
*  CVoipXmlParamHandler 
*  Handles parameters and passes them to settings handlers.
* 
*  @lib voipxmlprocessor.lib
*  @since S60 v5.0
*/
NONSHARABLE_CLASS( CVoipXmlParamHandler ) : public CBase
    {
#ifdef _DEBUG
    friend class UT_CVoipXmlParamHandler;
    friend class UT_CVoipXmlParser;
#endif

public:

    static CVoipXmlParamHandler* NewL();
    
    static CVoipXmlParamHandler* NewLC(); 

    virtual ~CVoipXmlParamHandler();

    /**
     * Marks what type of settings are to be deployed.
     * 
     * @since S60 v5.0
     * @param aType Type of settings that are to be deployed.
     */
    void SettingsStart( const TDesC& aType );

    /**
     * Tells that settings are deployed and previous type should be used. 
     */
    void SettingsEnd();

    /**
     * Handles XML parameters and passes the handled
     * ones to settings handlers.
     * 
     * @since S60 v5.0
     * @param aParam Parameter name.
     * @param avalue Parameter value.
     */
    void HandleParameter( const TDesC& aParam, const TDesC& aValue );

    /**
     * Stores transiently saved settings.
     *
     * @since S60 v5.0
     * @return KErrNone if successful,
     *         KErrNotSupported if no SIP or VoIP settings to be stored,
     *         KErrCompletion if error occurred in storing settings.
     */
    TInt StoreSettings();

    /**
     * Returns the service tab ID.
     * 
     * @since S60 v5.0
     * @return Service tab ID.
     */
    TUint32 ServiceTabId();

private:

    /**
     * Converts parameter name to enumerated one.
     *
     * @since S60 v5.0
     * @param aParam Parameter name.
     * @return Enumerated parameter value.
     */
    TInt ConvertParameter( const TDesC& aParam );

private:

    CVoipXmlParamHandler();

    void ConstructL();

    
private: // data

    /**
     * Type of settings that are currently being deployed.
     */
    TInt iSettingsType;

    /**
     * Type of previously deployed settings.
     */
    TInt iPreviousSettingsType;

    /**
     * SIP settings handler.
     * Own.
     */
    CVoipXmlSipHandler* iSipHandler;

    /**
     * VoIP settings handler.
     * Own.
     */
    CVoipXmlVoipHandler* iVoipHandler;

    /**
     * XDM settings handler.
     * Own.
     */
    CVoipXmlXdmHandler* iXdmHandler;

    /**
     * Presence settings handler.
     * Own.
     */
    CVoipXmlPresenceHandler* iPresenceHandler;

    /**
     * NAT/Firewall settings handler.
     * Own.
     */
    CVoipXmlNatFwHandler* iNatFwHandler;

    /**
     * Internet Access Point settings handler.
     * Own.
     */
    CVoipXmlIapHandler* iIapHandler;
    };

#endif // VOIPXMLPARAMHANDLER_H

// End of File

