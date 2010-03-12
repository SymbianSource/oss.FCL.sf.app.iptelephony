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
* Description:  VoIP settings XML parameter handler.
*
*/


#include "voipxmlparamhandler.h"
#include "voipxmlsiphandler.h"
#include "voipxmlvoiphandler.h"
#include "voipxmlxdmhandler.h"
#include "voipxmlpresencehandler.h"
#include "voipxmlnatfwhandler.h"
#include "voipxmliaphandler.h"
#include "voipxmlprocessordefaults.h"

// ---------------------------------------------------------------------------
// Default constructor.
// ---------------------------------------------------------------------------
//
CVoipXmlParamHandler::CVoipXmlParamHandler()
    {
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CVoipXmlParamHandler::ConstructL()
    {
    iSettingsType         = EUnknown;
    iPreviousSettingsType = EUnknown;
    iSipHandler      = CVoipXmlSipHandler::NewL();
    iVoipHandler     = CVoipXmlVoipHandler::NewL();
    iXdmHandler      = CVoipXmlXdmHandler::NewL();
    iPresenceHandler = CVoipXmlPresenceHandler::NewL();
    iNatFwHandler    = CVoipXmlNatFwHandler::NewL();
    iIapHandler      = CVoipXmlIapHandler::NewL();
    }

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CVoipXmlParamHandler* CVoipXmlParamHandler::NewL()
    {
    CVoipXmlParamHandler* self = CVoipXmlParamHandler::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
CVoipXmlParamHandler* CVoipXmlParamHandler::NewLC()
    {
    CVoipXmlParamHandler* self = new( ELeave ) CVoipXmlParamHandler;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CVoipXmlParamHandler::~CVoipXmlParamHandler()
    {
    delete iSipHandler;
    delete iVoipHandler;
    delete iXdmHandler;
    delete iPresenceHandler;
    delete iNatFwHandler;
    delete iIapHandler;
    }

// ---------------------------------------------------------------------------
// Marks the currently deployed settings.
// ---------------------------------------------------------------------------
//
void CVoipXmlParamHandler::SettingsStart( const TDesC& aType )
    {
    TBuf<KMaxNodeNameLength> type( KNullDesC );
    type.Copy( aType );
    type.LowerCase();
    iPreviousSettingsType = iSettingsType;
    if ( KSettingsTypeVoip() == type )
        {
        iSettingsType = EVoip;
        }
    else if ( KSettingsTypeSip() == type )
        {
        iSettingsType = ESip;
        }
    else if ( KSettingsTypePresence() == type )
        {
        iSettingsType = EPresence;
        }
    else if ( KSettingsTypeXdm() == type )
        {
        iSettingsType = EXdm;
        }
    else if ( KSettingsTypeNatFw() == type )
        {
        iSettingsType = ENatFw;
        }
    else if ( KSettingsTypeCodec() == type )
        {
        iSettingsType = ECodec;
        }
    else if ( KSettingsTypeVmbx() == type )
        {
        iSettingsType = EVmbx;
        }
    else if ( KSettingsTypeProxy() == type )
        {
        iSettingsType = EProxy;
        }
    else if ( KSettingsTypeRegistrar() == type )
        {
        iSettingsType = ERegistrar;
        }
    else if ( KSettingsTypeAdditionalStun() == type )
        {
        iSettingsType = EAdditionalStun;
        }
    else if ( KSettingsTypeWlan() == type )
        {
        iSettingsType = EWlan;
        }
    else if ( KSettingsTypeWepKey() == type )
        {
        iSettingsType = EWepKey;
        }
    else if ( KSettingsTypeDestination() == type )
        {
        iSettingsType = EDestination;
        }
    else if ( KSettingsTypeEap() == type )
        {
        iSettingsType = EEap;
        }
    else
        {
        iSettingsType = EUnknown;
        }
    }

// ---------------------------------------------------------------------------
// Marks the currently deployed settings as 'ready', i.e. sets current
// settings type to previous one and informs settings handlers.
// ---------------------------------------------------------------------------
//
void CVoipXmlParamHandler::SettingsEnd()
    {
    iVoipHandler->SettingsEnd( iSettingsType );
    iNatFwHandler->SettingsEnd( iSettingsType );
    iIapHandler->SettingsEnd( iSettingsType );
    iSettingsType = iPreviousSettingsType;
    iPreviousSettingsType = EUnknown;
    }

// ---------------------------------------------------------------------------
// Handles XML parameters, i.e. converts parameter names to enum and sets
// the values to setting handlers.
// ---------------------------------------------------------------------------
//
void CVoipXmlParamHandler::HandleParameter( const TDesC& aParam, 
    const TDesC& aValue )
    {
    switch ( iSettingsType )
        {
        case EVoip:
        case EVmbx:
        case ECodec:
            {
            TInt param = ConvertParameter( aParam );
            iVoipHandler->SetSetting( iSettingsType, param, aValue );
            }
            break;
        case ESip:
        case EProxy:
        case ERegistrar:
            {
            TInt param = ConvertParameter( aParam );
            TBuf8<KMaxNodeValueLength> value;
            value.Copy( aValue );
            iSipHandler->SetSetting( iSettingsType, param, value );
            }
            break;
        case EPresence:
            {
            TInt param = ConvertParameter( aParam );
            iPresenceHandler->SetSetting( param, aValue );
            }
            break;
        case EXdm:
            {
            TInt param = ConvertParameter( aParam );
            iXdmHandler->SetSetting( param, aValue );
            }
            break;
        case ENatFw:
        case EAdditionalStun:
            {
            TInt param = ConvertParameter( aParam );
            TBuf8<KMaxNodeValueLength> value;
            value.Copy( aValue );
            iNatFwHandler->SetSetting( iSettingsType, param, value );
            }
            break;
        case EWlan:
        case EWepKey:
        case EDestination:
        case EEap:
            {
            TInt param = ConvertParameter( aParam );
            iIapHandler->SetSetting( iSettingsType, param, aValue );
            }
            break;
        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// Stores settings, i.e. sends store commands to settings handlers.
// ---------------------------------------------------------------------------
//
TInt CVoipXmlParamHandler::StoreSettings()
    {
    // First send 'basic' store commands.
    TInt err( KErrNone );
    err = iSipHandler->StoreSettings();
    if ( KErrNone != err )
        {
        return err;
        }
    err = iVoipHandler->StoreSettings();
    if ( KErrNone != err )
        {
        return err;
        }
    err = iXdmHandler->StoreSettings();
    // If there were no settings, ignore error code.
    if ( KErrNotSupported == err )
        {
        err = KErrNone;
        }
    err = iPresenceHandler->StoreSettings();
    // If there were no settings, ignore error code.
    if ( KErrNotSupported == err )
        {
        err = KErrNone;
        }

    err = iNatFwHandler->StoreSettings();
    // If there were no settings, ignore error code.
    if ( KErrNotSupported == err )
        {
        err = KErrNone;
        }

    err = iIapHandler->StoreSettings();
    // If there were no settings, ignore error code.
    if ( KErrNotSupported == err )
        {
        err = KErrNone;
        }

    // Send linkage information.
    iSipHandler->LinkSettings( EDestination, iIapHandler->SettingsId() );
    iVoipHandler->LinkSettings( ESip, iSipHandler->SettingsId() );
    iVoipHandler->LinkSettings( EPresence, iPresenceHandler->SettingsId() );
    iVoipHandler->LinkSettings( EDestination, iIapHandler->SettingsId() );
    iPresenceHandler->LinkSettings( ESip, iSipHandler->SettingsId() );
    iPresenceHandler->LinkSettings( EXdm, iXdmHandler->SettingsId() );

    // Send finalizing commands.
    iSipHandler->FinalizeSettings(); // Ignore error code.
    err = iVoipHandler->FinalizeSettings();
    iPresenceHandler->FinalizeSettings(); // Ignore error code.
    return err;
    }

// ---------------------------------------------------------------------------
// Returns the VoIP service ID.
// ---------------------------------------------------------------------------
//
TUint32 CVoipXmlParamHandler::ServiceTabId()
    {
    TUint32 tabId( KErrNone );
    TRAP_IGNORE( tabId = iVoipHandler->ServiceTabIdL() );
    return tabId;
    }

// ---------------------------------------------------------------------------
// Converts parameter name to enumerated parameter.
// ---------------------------------------------------------------------------
//
TInt CVoipXmlParamHandler::ConvertParameter( const TDesC& aParam )
    {
    TBuf<KMaxNodeNameLength> name( KNullDesC );
    name.Copy( aParam );
    name.LowerCase();
    // ==============================
    // Generic parameters.
    // ==============================
    //
    if ( KParamValueName() == name )
        {
        return EName;
        }
    else if ( KParamValueUri() == name )
        {
        return EUri;
        }
    else if ( KParamValueTransport() == name )
        {
        return ETransport;
        }
    else if ( KParamValuePort() == name )
        {
        return EPort;
        }
    else if ( KParamValueUsername() == name )
        {
        return EUsername;
        }
    else if ( KParamValuePassword() == name )
        {
        return EPassword;
        }
    else if ( KParamValueType() == name )
        {
        return EType;
        }
    else if ( KParamValueDomain() == name )
        {
        return EDomain;
        }
    else if ( KParamValueStartPort() == name )
        {
        return EStartPort;
        }
    else if ( KParamValueEndPort() == name )
        {
        return EEndPort;
        }
    // ==============================
    // VoIP 'core' parametes.
    // ==============================
    //
    else if ( KParamValueMediaQos() == name )
        {
        return EMediaQos;
        }
    else if ( KParamValueDtmfInband() == name )
        {
        return EDtmfInband;
        }
    else if ( KParamValueDtmfOutband() == name )
        {
        return EDtmfOutband;
        }
    else if ( KParamValueSecureCallPref() == name )
        {
        return ESecureCallPreference;
        }
    else if ( KParamValueVoipOverWcdma() == name )
        {
        return EAllowVoipOverWcdma;
        }
    else if ( KParamValueRtcp() == name )
        {
        return ERtcp;
        }
    else if ( KParamValueUserAgentTerminalType() == name )
        {
        return EUserAgentHeaderTerminalType;
        }
    else if ( KParamValueUserAgentWlanMac() == name )
        {
        return EUserAgentHeaderWlanMac;
        }
    else if ( KParamValueUserAgentFreeString() == name )
        {
        return EUserAgentHeaderFreeString;
        }
    else if ( KParamValueCallerIdDigits() == name )
        {
        return ECallerIdDigits;
        }
    else if ( KParamValueIgnoreDomainPart() == name )
        {
        return EIgnoreDomainPart;
        }
    else if ( KParamValueAutoAcceptBuddyRequests() == name )
        {
        return EAutoAcceptBuddyRequests;
        }
    else if ( KParamValueAddUserPhone() == name )
        {
        return EAddUserPhone;
        }
    else if ( KParamValueProviderUrl() == name )
        {
        return EProviderUrl;
        }
    else if ( KParamValueMinSessionInterval() == name )
        {
        return EMinSessionInterval;
        }
    else if ( KParamValueSessionInterval() == name )
        {
        return ESessionInterval;
        }
    else if ( KParamValueBrandingUri() == name )
        {
        return EBrandingUri;
        }
    else if ( KParamValueAutoEnable() == name )
        {
        return EAutoEnable;
        }
    else if ( KParamValueEnableSipIm() == name )
        {
        return EEnableSipIm;
        }
    else if ( KParamValueBrandId() == name )
        {
        return EBrandId;
        }
    // ==============================
    // VoIP voicemailbox parameters.
    // ==============================
    //
    else if ( KParamValueMwiUri() == name )
        {
        return EMwiUri;
        }
    else if ( KParamValueListeningUri() == name )
        {
        return EListeningUri;
        }
    else if ( KParamValueReSubscribeInterval() == name )
        {
        return EReSubscribeInterval;
        }
    // ==============================
    // VoIP codec parameters.
    // ==============================
    //
    else if ( KParamValueJitterBuffer() == name )
        {
        return EJitterBuffer;
        }
    else if ( KParamValueOctetAlign() == name )
        {
        return EOctetAlign;
        }
    else if ( KParamValuePTime() == name )
        {
        return EPTime;
        }
    else if ( KParamValueMaxPTime() == name )
        {
        return EMaxPTime;
        }
    else if ( KParamValueModeSet() == name )
        {
        return EModeSet;
        }
    else if ( KParamValueModeChangePeriod() == name )
        {
        return EModeChangePeriod;
        }
    else if ( KParamValueModeChangeNeighbor() == name )
        {
        return EModeChangeNeighbor;
        }
    else if ( KParamValueMaxRed() == name )
        {
        return EMaxRed;
        }
    else if ( KParamValueVad() == name )
        {
        return EVad;
        }
    else if ( KParamValueAnnexB() == name )
        {
        return EAnnexb;
        }
    // ==============================
    // SIP parameters.
    // ==============================
    //
    else if ( KParamValueSignalingQos() == name )
        {
        return ESignalingQos;
        }
    else if ( KParamValuePublicUserId() == name )
        {
        return EPublicUserId;
        }
    else if ( KParamValueSignalCompression() == name )
        {
        return ESignalCompression;
        }
    else if ( KParamValueSecurityNegotiation() == name )
        {
        return ESecurityNegotiation;
        }
    else if ( KParamValueProfileLock() == name )
        {
        return EProfileLock;
        }
    else if ( KParamValueAutoRegistration() == name )
        {
        return EAutoRegistration;
        }
    else if ( KParamValueLooseRouting() == name )
        {
        return ELooseRouting;
        }
    // ==============================
    // Presence parameters.
    // ==============================
    //
    else if ( KParamValueMaxObjectSize() == name )
        {
        return EMaxObjectSize;
        }
    else if ( KParamValuePublishInterval() == name )
        {
        return EPublishInterval;
        }
    else if ( KParamValueMaxSubscriptions() == name )
        {
        return EMaxSubscriptions;
        }
    else if ( KParamValueMaxContacts() == name )
        {
        return EMaxContacts;
        }
    else if ( KParamValueDomainSyntax() == name )
        {
        return EDomainSyntax;
        }
    // ==============================
    // NAT/Firewall parameters.
    // ==============================
    //
    else if ( KParamValueTcpRefreshInterval() == name )
        {
        return ETcpRefreshInterval;
        }
    else if ( KParamValueUdpRefreshInterval() == name )
        {
        return EUdpRefreshInterval;
        }
    else if ( KParamValueCrlfRefresh() == name )
        {
        return ECrlfRefresh;
        }
    else if ( KParamValueStunSharedSecret() == name )
        {
        return EStunSharedSecret;
        }
    // ==============================
    // WLAN IAP parameters.
    // ==============================
    //
    else if ( KParamSsid() == name )
        {
        return ESsid;
        }
    else if ( KParamHidden() == name )
        {
        return EHidden;
        }
    else if ( KParamNetworkMode() == name )
        {
        return ENetworkMode;
        }
    else if ( KParamPreSharedKey() == name )
        {
        return EPreSharedKey;
        }
    else if ( KParamWepAuthMode() == name )
        {
        return EWepAuthMode;
        }
    else if ( KParamLength() == name )
        {
        return ELength;
        }
    else if ( KParamData() == name )
        {
        return EData;
        }
    // ==============================
    // Not supported.
    // ==============================
    //
    else
        {
        return EUnSupported;
        }
    }

// End of file.
