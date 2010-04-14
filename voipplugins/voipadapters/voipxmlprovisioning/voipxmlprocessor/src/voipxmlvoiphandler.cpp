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
* Description:  VoIP settings handler
*
*/


#include <crcseaudiocodecregistry.h>
#include <crcseprofileregistry.h>
#include <spsettings.h>
#include <spproperty.h>
#include <spdefinitions.h>
#include <sipmanagedprofile.h>
#include <sipmanagedprofileregistry.h>
#include <cipappphoneutils.h>           // SIP User-Agent header info.
#include <cipapputilsaddressresolver.h> // SIP User-Agent header info.
#include <pathinfo.h>                   // For getting phone rom root path.
#include <cvimpstsettingsstore.h>       // For IM tone path
#include <settingsinternalcrkeys.h>     // For default service.
#include <centralrepository.h>          // For default service.

#include "voipxmlvoiphandler.h"
#include "voipxmlprocessorlogger.h"
#include "voipxmlprocessordefaults.h"
#include "voipxmlutils.h"

const TInt32 KCCHPresenceSubServicePlugId = 0x1027545A;
// IM related constants.
const TUint32 KIMSubServicePluginId = 0x1027545A;
const TUint32 KIMLaunchUid          = 0x200255D0;
const TInt    KIMSettingsId         = 1;
// Default IM message tone
_LIT( KDefaultTone,       "Message 2.aac" );
// Brand related constants.
const TInt KBrandVersion = 1;

// ---------------------------------------------------------------------------
// Default constructor.
// ---------------------------------------------------------------------------
//
CVoipXmlVoipHandler::CVoipXmlVoipHandler()
    {
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CVoipXmlVoipHandler::ConstructL()
    {
    DBG_PRINT( "CVoipXmlVoipHandler::ConstructL begin" );
    iRegistry      = CRCSEProfileRegistry::NewL();
    iCodecRegistry = CRCSEAudioCodecRegistry::NewL();
    iEntry         = CRCSEProfileEntry::NewL();
    ResetTempCodec();
    ResetTempSpSettings();
    iSettingsSet = EFalse;
    DBG_PRINT( "CVoipXmlVoipHandler::ConstructL end" );
    }

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CVoipXmlVoipHandler* CVoipXmlVoipHandler::NewL()
    {
    DBG_PRINT( "CVoipXmlVoipHandler::NewL begin" );
    CVoipXmlVoipHandler* self = new ( ELeave ) CVoipXmlVoipHandler;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    DBG_PRINT( "CVoipXmlVoipHandler::NewL end" );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CVoipXmlVoipHandler::~CVoipXmlVoipHandler()
    {
    DBG_PRINT( "CVoipXmlVoipHandler::~CVoipXmlVoipHandler begin" );
    delete iRegistry;
    delete iCodecRegistry;
    delete iEntry;
    ResetTempCodec( ETrue );
    ResetTempSpSettings();
    DBG_PRINT( "CVoipXmlVoipHandler::~CVoipXmlVoipHandler end" );
    }

// ---------------------------------------------------------------------------
// Sets VoIP setting.
// ---------------------------------------------------------------------------
//
void CVoipXmlVoipHandler::SetSetting( TInt aType, TInt aParam, 
    const TDesC& aValue )
    {
    // Ignore too long descriptors.
    if ( KMaxNodeValueLength < aValue.Length() )
        {
        return;
        }

    switch ( aType )
        {
        case EVoip:
            TRAP_IGNORE( SetCoreSettingL( aParam, aValue ) );
            break;
        case ECodec:
            TRAP_IGNORE( SetCodecSettingL( aParam, aValue ) );
            break;
        case EVmbx:
            TRAP_IGNORE( SetVmbxSettingL( aParam, aValue ) );
            break;
        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// Stores settings to RCSE.
// ---------------------------------------------------------------------------
//
TInt CVoipXmlVoipHandler::StoreSettings()
    {
    TInt err( KErrNone );
    TBool ok( ETrue );

    if ( !iSettingsSet )
        {
        // No settings to be stored => method not supported.
        err = KErrNotSupported;
        ok = EFalse;
        }
    else
        {
        TUint32 profileId( KErrNone );
        // Add default codec set if no codecs defined.
        if ( 0 == iEntry->iPreferredCodecs.Count() )
            {
            TRAP_IGNORE( AddDefaultCodecsL() );
            }
        TRAP( err, profileId = iRegistry->AddL( *iEntry ) );
        if ( KErrNone == err )
            {
            // Adding profile entry to registry OK. Let's load the profile
            // from registry so that we'll get all the values registry has
            // added to the entry (AddL takes entry as const reference).
            TRAP_IGNORE( iRegistry->FindL( profileId, *iEntry ) );
            iServiceId = iEntry->iServiceProviderId;
            }
        else
            {
            err = KErrCompletion;
            ok = EFalse;
            }
        }
    if ( !ok )
        {
        const TInt count = iEntry->iPreferredCodecs.Count();
        for ( TInt counter = 0; counter < count; counter++ )
            {
            TRAP_IGNORE( iCodecRegistry->DeleteL( 
                iEntry->iPreferredCodecs[counter] ) );
            }
        }
    return err;
    }

// ---------------------------------------------------------------------------
// Returns the service ID.
// ---------------------------------------------------------------------------
//
TUint32 CVoipXmlVoipHandler::SettingsId()
    {
    return iServiceId;
    }

// ---------------------------------------------------------------------------
// Sets a codec to RCSE and resets the temp codec.
// ---------------------------------------------------------------------------
//
void CVoipXmlVoipHandler::SettingsEnd( TInt aType )
    {
    if ( ECodec == aType )
        {
        TRAP_IGNORE( SetCodecToRcseL() );
        }
    }

// ---------------------------------------------------------------------------
// Saves linkage information.
// ---------------------------------------------------------------------------
//
void CVoipXmlVoipHandler::LinkSettings( TInt aType, TUint32 aSettingsId )
    {
    switch ( aType )
        {
        case ESip:
            {
            TRAP_IGNORE( SetSipInfoL( aSettingsId ) );
            break;
            }
        case EPresence:
            {
            iPresenceId = aSettingsId;
            break;
            }
        case EDestination:
            {
            iDestinationId = aSettingsId;
            break;
            }
        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// Finalizes settings saving.
// ---------------------------------------------------------------------------
//
TInt CVoipXmlVoipHandler::FinalizeSettings()
    {
    TInt err( KErrNone );
    TRAP( err, SetSpSettingsL() );
    if ( KErrNone != err )
        {
        // ParamHandler is only intrested in KErrNone and KErrCompletion.
        err = KErrCompletion;
        }
    if ( iDefault )
        {
        TRAP_IGNORE( SetAsDefaultL() );
        }
    return err;
    }

// ---------------------------------------------------------------------------
// Returns the service tab ID of this VoIP service.
// ---------------------------------------------------------------------------
//
TUint32 CVoipXmlVoipHandler::ServiceTabIdL()
    {
    DBG_PRINT( "CVoipXmlVoipHandler::ServiceTabIdL begin" );
    TInt tabId( KErrNone );
    CSPSettings* spSettings = CSPSettings::NewLC(); // CS:1
    CSPProperty* property = CSPProperty::NewLC(); // CS:2
    TInt err = spSettings->FindPropertyL( 
        iServiceId, EPropertyContactViewId, *property );
    User::LeaveIfError( property->GetValue( tabId ) );
    CleanupStack::PopAndDestroy( 2, spSettings ); // CS:0
    DBG_PRINT2( "CVoipXmlVoipHandler::ServiceTabIdL end (return %d)", tabId );
    return (TUint32)tabId;
    }

// ---------------------------------------------------------------------------
// From class MSIPProfileRegistryObserver.
// ---------------------------------------------------------------------------
//
void CVoipXmlVoipHandler::ProfileRegistryEventOccurred( 
    TUint32 /*aSIPProfileId*/, TEvent /*aEvent*/ )
    {
    }

// ---------------------------------------------------------------------------
// From class MSIPProfileRegistryObserver. 
// ---------------------------------------------------------------------------
//
void CVoipXmlVoipHandler::ProfileRegistryErrorOccurred( 
    TUint32 /*aSIPProfileId*/, TInt /*aError*/ )
    {
    }

// ---------------------------------------------------------------------------
// Sets VoIP core setting to temporary storage.
// ---------------------------------------------------------------------------
//
void CVoipXmlVoipHandler::SetCoreSettingL( TInt aParam, const TDesC& aValue )
    {
    TInt intValue( KErrNotFound );
    switch ( aParam )
        {
        case EName:
            {
            TBuf<KMaxSettingsNameLength> name( KNullDesC );
            name.Copy( aValue );
            TRAP_IGNORE( ValidateProfileNameL( name ) );
            iEntry->iProviderName.Copy( name );
            iEntry->iSettingsName.Copy( name );
            iSettingsSet = ETrue;
            break;
            }
        case EStartPort:
            {
            if ( KErrNone == VoipXmlUtils::DesToInt( aValue, intValue ) )
                {
                iEntry->iStartMediaPort = intValue;
                iSettingsSet = ETrue;
                }
            break;
            }
        case EEndPort:
            {
            if ( KErrNone == VoipXmlUtils::DesToInt( aValue, intValue ) )
                {
                iEntry->iEndMediaPort = intValue;
                iSettingsSet = ETrue;
                }
            break;
            }
        case EMediaQos:
            {
            if ( KErrNone == VoipXmlUtils::DesToInt( aValue, intValue ) )
                {
                iEntry->iMediaQOS = intValue;
                iSettingsSet = ETrue;
                }
            break;
            }
        case EDtmfInband:
            {
            if ( KErrNone == VoipXmlUtils::DesToInt( aValue, intValue ) )
                {
                TEntryOnOff value;
                if ( !intValue )
                    {
                    value = CRCSEProfileEntry::EOff;
                    }
                else if ( KNotSet == intValue )
                    {
                    value = CRCSEProfileEntry::EOONotSet;
                    }
                else
                    {
                    value = CRCSEProfileEntry::EOn;
                    }
                iEntry->iInbandDTMF = value;
                iSettingsSet = ETrue;
                }
            break;
            }
        case EDtmfOutband:
            {
            if ( KErrNone == VoipXmlUtils::DesToInt( aValue, intValue ) )
                {
                TEntryOnOff value;
                if ( !intValue )
                    {
                    value = CRCSEProfileEntry::EOff;
                    }
                else if ( KErrNotFound == intValue )
                    {
                    value = CRCSEProfileEntry::EOONotSet;
                    }
                else
                    {
                    value = CRCSEProfileEntry::EOn;
                    }
                iEntry->iOutbandDTMF = value;
                iSettingsSet = ETrue;
                }
            break;
            }
        case ESecureCallPreference:
            {
            if ( KErrNone == VoipXmlUtils::DesToInt( aValue, intValue ) )
                {
                iEntry->iSecureCallPreference = intValue;
                iSettingsSet = ETrue;
                }
            break;
            }
        case EAllowVoipOverWcdma:
            {
            if ( KErrNone == VoipXmlUtils::DesToInt( aValue, intValue ) )
                {
                TEntryOnOff value;
                if ( !intValue )
                    {
                    value = CRCSEProfileEntry::EOff;
                    }
                else if ( KErrNotFound == intValue )
                    {
                    value = CRCSEProfileEntry::EOONotSet;
                    }
                else
                    {
                    value = CRCSEProfileEntry::EOn;
                    }
                iEntry->iAllowVoIPoverWCDMA = value;
                iSettingsSet = ETrue;
                }
            break;
            }
        case ERtcp:
            {
            if ( KErrNone == VoipXmlUtils::DesToInt( aValue, intValue ) )
                {
                iEntry->iRTCP = intValue;
                iSettingsSet = ETrue;
                }
            break;
            }
        case EUserAgentHeaderTerminalType:
            {
            if ( KErrNone == VoipXmlUtils::DesToInt( aValue, intValue ) )
                {
                iEntry->iSIPVoIPUAHTerminalType = intValue;
                iSettingsSet = ETrue;
                }
            break;
            }
        case EUserAgentHeaderWlanMac:
            {
            if ( KErrNone == VoipXmlUtils::DesToInt( aValue, intValue ) )
                {
                iEntry->iSIPVoIPUAHeaderWLANMAC = intValue;
                iSettingsSet = ETrue;
                }
            break;
            }
        case EUserAgentHeaderFreeString:
            {
            if ( KMaxSettingsLength32 >= aValue.Length() )
                {
                iEntry->iSIPVoIPUAHeaderString.Copy( aValue );
                iSettingsSet = ETrue;
                }
            break;
            }
        case ECallerIdDigits:
            {
            if ( KErrNone == VoipXmlUtils::DesToInt( aValue, intValue ) )
                {
                iEntry->iMeanCountOfVoIPDigits = intValue;
                iSettingsSet = ETrue;
                }
            break;
            }
        case EIgnoreDomainPart:
            {
            if ( KErrNone == VoipXmlUtils::DesToInt( aValue, intValue ) )
                {
                iEntry->iIgnoreAddrDomainPart = intValue;
                iSettingsSet = ETrue;
                }
            break;
            }
        case EAutoAcceptBuddyRequests:
            {
            if ( KErrNone == VoipXmlUtils::DesToInt( aValue, intValue ) )
                {
                iSpSettings.iAutoAcceptBuddies = (TBool)intValue;
                iSettingsSet = ETrue;
                }
            break;
            }
        case EAddUserPhone:
            {
            if ( KErrNone == VoipXmlUtils::DesToInt( aValue, intValue ) )
                {
                TEntryOnOff value;
                if ( !intValue )
                    {
                    value = CRCSEProfileEntry::EOff;
                    }
                else if ( KErrNotFound == intValue )
                    {
                    value = CRCSEProfileEntry::EOONotSet;
                    }
                else
                    {
                    value = CRCSEProfileEntry::EOn;
                    }
                iEntry->iUserPhoneUriParameter = value;
                iSettingsSet = ETrue;
                }
            break;
            }
        case EProviderUrl:
            {
            if ( !iSpSettings.iProviderUrl )
                {
                iSpSettings.iProviderUrl = aValue.AllocL();
                iSettingsSet = ETrue;
                }
            break;
            }
        case EMinSessionInterval:
            {
            if ( KErrNone == VoipXmlUtils::DesToInt( aValue, intValue ) )
                {
                iEntry->iSIPMinSE = intValue;
                iSettingsSet = ETrue;
                }
            break;
            }
        case ESessionInterval:
            {
            if ( KErrNone == VoipXmlUtils::DesToInt( aValue, intValue ) )
                {
                iEntry->iSIPSessionExpires = intValue;
                iSettingsSet = ETrue;
                }
            break;
            }
        case EBrandingUri:
            {
            if ( !iSpSettings.iBrandingUri )
                {
                iSpSettings.iBrandingUri = aValue.AllocL();
                iSettingsSet = ETrue;
                }
            break;
            }
        case EAutoEnable:
            {
            if ( KErrNone == VoipXmlUtils::DesToInt( aValue, intValue ) )
                {
                iSpSettings.iAutoEnable = (TBool)intValue;
                iSettingsSet = ETrue;
                }
            break;
            }
        case EEnableSipIm:
            {
            if ( KErrNone == VoipXmlUtils::DesToInt( aValue, intValue ) )
                {
                iSpSettings.iEnableSipIm = (TBool)intValue;
                iSettingsSet = ETrue;
                }
            break;
            }
        case EBrandId:
            {
            if ( !iSpSettings.iBrandId )
                {
                iSpSettings.iBrandId = aValue.AllocL();
                iSettingsSet = ETrue;
                }
            break;
            }
        case EDefault:
            {
            if ( KErrNone == VoipXmlUtils::DesToInt( aValue, intValue ) )
                {
                iDefault = (TBool)intValue;
                iSettingsSet = ETrue;
                }
            break;
            }
        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// Sets codec setting to temporary storage.
// ---------------------------------------------------------------------------
//
void CVoipXmlVoipHandler::SetCodecSettingL( TInt aParam, const TDesC& aValue )
    {
    TInt intValue;
    switch ( aParam )
        {
        // Codec parameters
        case EName:
            {
            if ( iCurrentCodec.iName )
                {
                break;
                }
            TBuf<KMaxNodeValueLength> value;
            value.Copy( aValue );
            value.UpperCase();
            if ( 0 == value.Compare( KILbc ) )
                {
                value.Copy( KAudioCodeciLBC );
                }
            iCurrentCodec.iName = value.AllocL();
            break;
            }
        case EJitterBuffer:
            {
            if ( KErrNone == VoipXmlUtils::DesToInt( aValue, intValue ) )
                {
                iCurrentCodec.iJitterBuffer = intValue;
                }
            break;
            }
        case EOctetAlign:
            {
            if ( KErrNone == VoipXmlUtils::DesToInt( aValue, intValue ) )
                {
                if ( !intValue )
                    {
                    iCurrentCodec.iOctetAlign = CRCSEAudioCodecEntry::EOff;
                    }
                else if ( KErrNotFound == intValue )
                    {
                    iCurrentCodec.iOctetAlign = 
                        CRCSEAudioCodecEntry::EOONotSet;
                    }
                else
                    {
                    iCurrentCodec.iOctetAlign = CRCSEAudioCodecEntry::EOn;
                    }
                }
            break;
            }
        case EPTime:
            {
            if ( KErrNone == VoipXmlUtils::DesToInt( aValue, intValue ) )
                {
                iCurrentCodec.iPtime = intValue;
                }
            break;
            }
        case EMaxPTime:
            {
            if ( KErrNone == VoipXmlUtils::DesToInt( aValue, intValue ) )
                {
                iCurrentCodec.iMaxPtime = intValue;
                }
            break;
            }
        case EModeSet:
            {
            TBuf<KMaxNodeNameLength> string;
            string.Copy( aValue );
            while ( string.Length() )
                {
                TInt offset = string.Locate( KComma );
                TBuf<KMaxNodeNameLength> helpString( KNullDesC );
                if ( KErrNotFound == offset )
                    {
                    if ( KErrNone == VoipXmlUtils::DesToInt( 
                        string, intValue ) )
                        {
                        iCurrentCodec.iModeSet.AppendL( intValue );
                        }
                    string.Zero();
                    }
                else if ( !offset )
                    {
                    string.Delete( 0, 1 );
                    }
                else
                    {
                    helpString.Copy( string.Left( offset ) );
                    if ( KErrNone == VoipXmlUtils::DesToInt( helpString,
                        intValue ) )
                        {
                        iCurrentCodec.iModeSet.AppendL( intValue );
                        }
                    offset++;
                    string.Delete( 0, offset );
                    }
                }
            break;
            }
        case EModeChangePeriod:
            {
            if ( KErrNone == VoipXmlUtils::DesToInt( aValue, intValue ) )
                {
                iCurrentCodec.iModeChangePeriod = intValue;
                }
            break;
            }
        case EModeChangeNeighbor:
            {
            if ( KErrNone == VoipXmlUtils::DesToInt( aValue, intValue ) )
                {
                if ( !intValue )
                    {
                    iCurrentCodec.iModeChangeNeighbor = 
                        CRCSEAudioCodecEntry::EOff;
                    }
                else if ( KErrNotFound == intValue )
                    {
                    iCurrentCodec.iModeChangeNeighbor = 
                        CRCSEAudioCodecEntry::EOONotSet;
                    }
                else
                    {
                    iCurrentCodec.iModeChangeNeighbor = 
                        CRCSEAudioCodecEntry::EOn;
                    }
                }
            break;
            }
        case EMaxRed:
            {
            if ( KErrNone == VoipXmlUtils::DesToInt( aValue, intValue ) )
                {
                iCurrentCodec.iMaxRed = intValue;
                }
            break;
            }
        case EVad:
            {
            if ( KErrNone == VoipXmlUtils::DesToInt( aValue, intValue ) )
                {
                if ( !intValue )
                    {
                    iCurrentCodec.iVad = CRCSEAudioCodecEntry::EOff;
                    }
                else if ( KErrNotFound == intValue )
                    {
                    iCurrentCodec.iVad = CRCSEAudioCodecEntry::EOONotSet;
                    }
                else
                    {
                    iCurrentCodec.iVad = CRCSEAudioCodecEntry::EOn;
                    }
                }
            break;
            }
        case EAnnexb:
            {
            if ( KErrNone == VoipXmlUtils::DesToInt( aValue, intValue ) )
                {
                if ( !intValue )
                    {
                    iCurrentCodec.iAnnexb = CRCSEAudioCodecEntry::EOff;
                    }
                else if ( KErrNotFound == intValue )
                    {
                    iCurrentCodec.iAnnexb = CRCSEAudioCodecEntry::EOONotSet;
                    }
                else
                    {
                    iCurrentCodec.iAnnexb = CRCSEAudioCodecEntry::EOn;
                    }
                }
            break;
            }
        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// Sets voice mailbox setting to temporary storage.
// ---------------------------------------------------------------------------
//
void CVoipXmlVoipHandler::SetVmbxSettingL( TInt aParam, const TDesC& aValue )
    {
    switch ( aParam )
        {
        case EMwiUri:
            {
            if ( !iSpSettings.iMwiUri )
                {
                iSpSettings.iMwiUri = aValue.AllocL();
                }
            break;
            }
        case EListeningUri:
            {
            if ( !iSpSettings.iListeningUri )
                {
                iSpSettings.iListeningUri = aValue.AllocL();
                }
            break;
            }
        case EReSubscribeInterval:
            {
            TInt intValue;
            if ( KErrNone == VoipXmlUtils::DesToInt( aValue, intValue ) )
                {
                iSpSettings.iResubrcribe = intValue;
                }
            break;
            }
        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// Checks if name is unique and modifies if needed.
// ---------------------------------------------------------------------------
//
void CVoipXmlVoipHandler::ValidateProfileNameL( TDes& aName )
    {
    const TInt maxModifyLength = 
        KMaxSettingsNameLength - KMaxProfileNameAppendLength;

    RArray<TUint32> voipIds;
    CleanupClosePushL( voipIds ); // CS:1
    iRegistry->GetAllIdsL( voipIds );

    const TInt count( voipIds.Count() );

    HBufC* newName = HBufC::NewLC( KMaxSettingsNameLength ); // CS:2
    newName->Des().Copy( aName.Left( maxModifyLength ) );

    TUint i( 1 ); // Add number to the name if name already in use.
    TBool changed( EFalse );

    // Go through each profile and see if the name of the new profile    
    // matches one of the existing names. If it does change it and
    // check the new name again.
    for ( TInt index = 0; index < count; index++ )
        {
        CRCSEProfileEntry* profile = CRCSEProfileEntry::NewLC(); // CS:3
        TBuf<KMaxSettingsNameLength> loadedName;
        iRegistry->FindL( voipIds[index], *profile );
        loadedName.Copy( profile->iSettingsName );
        if ( 0 == newName->Des().Compare( loadedName ) )
            {
            // If the name is changed we need to begin the comparison
            // again from the first profile.
            newName->Des().Copy( aName.Left( maxModifyLength ) );
            newName->Des().Append( KOpenParenthesis() );
            newName->Des().AppendNum( i );
            newName->Des().Append( KClosedParenthesis() );  
            index = 0;
            i++;
            if ( KMaxProfileNames < i )
                {
                User::Leave( KErrBadName );
                }
            changed = ETrue;
            }
        CleanupStack::PopAndDestroy( profile ); // CS:2
        }

    // Change setting only if it was changed.
    if ( changed )
        {
        aName.Copy( newName->Des() );
        }

    // newName, &voipIds
    CleanupStack::PopAndDestroy( 2, &voipIds ); // CS:0
    }

// ---------------------------------------------------------------------------
// Sets temporary codec to RCSE.
// ---------------------------------------------------------------------------
//
void CVoipXmlVoipHandler::SetCodecToRcseL()
    {
    // Nameless codecs are not supported.
    if ( !iCurrentCodec.iName )
        {
        ResetTempCodec();
        return;
        }

    CRCSEAudioCodecEntry* entry = CRCSEAudioCodecEntry::NewLC();
    // First set default values...
    entry->SetDefaultCodecValueSet( iCurrentCodec.iName->Des() );
    // ...and then replace them with the ones defined in settings XML.
    if ( KErrNotFound != iCurrentCodec.iJitterBuffer )
        {
        entry->iJitterBufferSize = iCurrentCodec.iJitterBuffer;
        }
    if ( KErrNotFound != iCurrentCodec.iPtime )
        {
        entry->iPtime = iCurrentCodec.iPtime;
        }
    if ( KErrNotFound != iCurrentCodec.iMaxPtime )
        {
        entry->iMaxptime = iCurrentCodec.iMaxPtime;
        }
    if ( CRCSEAudioCodecEntry::EOONotSet != iCurrentCodec.iOctetAlign )
        {
        entry->iOctetAlign = iCurrentCodec.iOctetAlign;
        }
    if ( CRCSEAudioCodecEntry::EOONotSet != 
        iCurrentCodec.iModeChangeNeighbor )
        {
        entry->iModeChangeNeighbor = iCurrentCodec.iModeChangeNeighbor;
        }
    if ( KErrNotFound != iCurrentCodec.iModeChangePeriod )
        {
        entry->iModeChangePeriod = iCurrentCodec.iModeChangePeriod;
        }
    if ( KErrNotFound != iCurrentCodec.iMaxRed )
        {
        entry->iMaxRed = iCurrentCodec.iMaxRed;
        }
    if ( CRCSEAudioCodecEntry::EOONotSet != iCurrentCodec.iVad )
        {
        entry->iVAD = iCurrentCodec.iVad;
        }
    if ( CRCSEAudioCodecEntry::EOONotSet != iCurrentCodec.iAnnexb )
        {
        entry->iAnnexb = iCurrentCodec.iAnnexb;
        }
    if ( iCurrentCodec.iModeSet.Count() )
        {
        entry->iModeSet.Reset();
        const TInt count = iCurrentCodec.iModeSet.Count();
        for ( TInt counter = 0; counter < count; counter++ )
            {
            entry->iModeSet.AppendL( iCurrentCodec.iModeSet[counter] );
            }
        }

    TUint32 codecId = iCodecRegistry->AddL( *entry );
    CleanupStack::PopAndDestroy( entry );
    iEntry->iPreferredCodecs.AppendL( codecId );
    ResetTempCodec();
    }

// ---------------------------------------------------------------------------
// Resets temporary codec settings.
// ---------------------------------------------------------------------------
//
void CVoipXmlVoipHandler::ResetTempCodec( TBool aCloseArray )
    {
    if ( iCurrentCodec.iName )
        {
        delete iCurrentCodec.iName;
        iCurrentCodec.iName = NULL;
        }
    iCurrentCodec.iJitterBuffer       = KErrNotFound;
    iCurrentCodec.iOctetAlign         = CRCSEAudioCodecEntry::EOONotSet;
    iCurrentCodec.iPtime              = KErrNotFound;
    iCurrentCodec.iMaxPtime           = KErrNotFound;
    iCurrentCodec.iModeChangePeriod   = KErrNotFound;
    iCurrentCodec.iModeChangeNeighbor = CRCSEAudioCodecEntry::EOONotSet;
    iCurrentCodec.iMaxRed             = KErrNotFound;
    iCurrentCodec.iVad                = CRCSEAudioCodecEntry::EOONotSet;
    iCurrentCodec.iAnnexb             = CRCSEAudioCodecEntry::EOONotSet;
    iCurrentCodec.iModeSet.Reset();
    if ( aCloseArray )
        {
        iCurrentCodec.iModeSet.Close();
        }
    }

// ---------------------------------------------------------------------------
// Resets temporary service provider settings.
// ---------------------------------------------------------------------------
//
void CVoipXmlVoipHandler::ResetTempSpSettings()
    {
    iSpSettings.iAutoAcceptBuddies = EFalse;
    iSpSettings.iAutoEnable        = EFalse;
    iSpSettings.iResubrcribe = KDefaultResubscribe;
    if ( iSpSettings.iBrandingUri )
        {
        delete iSpSettings.iBrandingUri;
        iSpSettings.iBrandingUri = NULL;
        }
    if ( iSpSettings.iListeningUri )
        {
        delete iSpSettings.iListeningUri;
        iSpSettings.iListeningUri = NULL;
        }
    if ( iSpSettings.iMwiUri )
        {
        delete iSpSettings.iMwiUri;
        iSpSettings.iMwiUri = NULL;
        }
    if ( iSpSettings.iProviderUrl )
        {
        delete iSpSettings.iProviderUrl;
        iSpSettings.iProviderUrl = NULL;
        }
    if ( iSpSettings.iBrandId )
        {
        delete iSpSettings.iBrandId;
        iSpSettings.iBrandId = NULL;
        }
    }

// ---------------------------------------------------------------------------
// Sets SIP related VoIP settings.
// ---------------------------------------------------------------------------
//
void CVoipXmlVoipHandler::SetSipInfoL( TUint32 aSipId )
    {
    // First set SIP information to RCSE so that
    // an entry in service table will be created.
    TSettingIds settingIds;
    settingIds.iProfileType = CRCSEProfileEntry::EProtocolSIP; // SIP.
    settingIds.iProfileId = aSipId;
    // Never reference to profile specific settings.
    settingIds.iProfileSpecificSettingId = KNotSet;
    iEntry->iIds.AppendL( settingIds );
    iRegistry->UpdateL( iEntry->iId, *iEntry );

    // After update, we'll need to load the profile again from registry
    // so that we'll get all the values registry has added to the entry
    // (UpdateL takes entry as const reference).
    TUint32 profileId = iEntry->iId;
    iRegistry->FindL( profileId, *iEntry );
    iServiceId = iEntry->iServiceProviderId;

    // Compile and set User-Agent header.
    CSIPManagedProfileRegistry* sipReg = CSIPManagedProfileRegistry::NewLC( 
        *this ); // CS:1
    CSIPManagedProfile* sipProf = static_cast<CSIPManagedProfile*>( 
        sipReg->ProfileL( aSipId ) );

    if ( sipProf )
        {
        CleanupStack::PushL( sipProf ); // CS:2

        TBuf8<KMaxUserAgentHeaderLength> userAgentString( KNullDesC8 );

        if ( iEntry->iSIPVoIPUAHTerminalType || 
            iEntry->iSIPVoIPUAHeaderWLANMAC || 
            iEntry->iSIPVoIPUAHeaderString.Length() != 0 )
            {
            // Set this to be an user-agent param.
            userAgentString.Append( KUserAgent );
            userAgentString.Append( KColon );
            userAgentString.Append( KSpace );

            // Set terminal type if eanbled.
            if ( iEntry->iSIPVoIPUAHTerminalType )
                {
                CIpAppPhoneUtils* util = CIpAppPhoneUtils::NewLC(); // CS:3
                TBuf<KMaxTerminalTypeLength> terminalType( KNullDesC );
                util->GetTerminalTypeL( terminalType );
                userAgentString.Append( terminalType );
                userAgentString.Append( KSpace );
                CleanupStack::PopAndDestroy( util ); // CS:2
                }

            // Set WLAN MAC address if enabled.
            if ( iEntry->iSIPVoIPUAHeaderWLANMAC )
                {
                CIPAppUtilsAddressResolver* resolver = 
                    CIPAppUtilsAddressResolver::NewLC(); // CS:3
                _LIT8( KFormatType, "-" );
                TBuf8<KMaxWlanMacAddressLength> wlanMACAddress( KNullDesC8 );
                resolver->GetWlanMACAddress( wlanMACAddress, KFormatType );
                userAgentString.Append( wlanMACAddress );
                userAgentString.Append( KSpace );
                CleanupStack::PopAndDestroy( resolver ); // CS:2
                }

            // Set free string if enabled.
            if ( iEntry->iSIPVoIPUAHeaderString.Length() )
                {
                userAgentString.Append( iEntry->iSIPVoIPUAHeaderString );
                }
            }

        // Set new user agent header data to profile.
        CDesC8ArrayFlat* array = new ( ELeave ) 
            CDesC8ArrayFlat( KUserAgentHeaderDataArrayInitSize );
        CleanupStack::PushL( array ); // CS:3
        array->AppendL( userAgentString );
        sipProf->SetParameter( KSIPHeaders, *array );
        sipReg->SaveL( *sipProf );
        // array, profile
        CleanupStack::PopAndDestroy( 2, sipProf ); // CS:1
        }
    CleanupStack::PopAndDestroy( sipReg ); // CS:0
    }

// ---------------------------------------------------------------------------
// Sets SPSettings to service table.
// ---------------------------------------------------------------------------
//
void CVoipXmlVoipHandler::SetSpSettingsL()
    {
    TInt serviceId = iEntry->iServiceProviderId;
    if ( 0 == serviceId || !iEntry->iIds.Count() )
        {
        return;
        }
    CSPSettings* spSettings = CSPSettings::NewLC(); // CS:1
    CSPProperty* property = CSPProperty::NewLC(); // CS:2

    // ==============================
    // Voice mailbox settings.
    // ==============================
    //

    TBool vmbx( EFalse );

    // Set MWI URI if present.
    //lint -e{960} No need for else statement here
    if ( iSpSettings.iMwiUri )
        {
        property->SetName( ESubPropertyVMBXMWIAddress );
        property->SetValue( iSpSettings.iMwiUri->Des() );
        spSettings->AddOrUpdatePropertyL( serviceId, *property );
        // Set the same value to Listening URI if it has no value.
        if ( !iSpSettings.iListeningUri )
            {
            iSpSettings.iListeningUri = iSpSettings.iMwiUri->Des().AllocL();
            }
        vmbx = ETrue;
        }
    // If there is no MWI URI but listening URI is present, set it as MWI URI.
    else if ( iSpSettings.iListeningUri )
        {
        property->SetName( ESubPropertyVMBXMWIAddress );
        property->SetValue( iSpSettings.iListeningUri->Des() );
        spSettings->AddOrUpdatePropertyL( serviceId, *property );
        vmbx = ETrue;
        }

    // Set Listening URI if present. This is a separate statement since 
    // the Listening URI may have been configured when setting MWI URI.
    if ( iSpSettings.iListeningUri )
        {
        property->SetName( ESubPropertyVMBXListenAddress );
        property->SetValue( iSpSettings.iListeningUri->Des() );
        spSettings->AddOrUpdatePropertyL( serviceId, *property );
        vmbx = ETrue;
        }

    if ( vmbx )
        {
        property->SetName( ESubPropertyVMBXMWISubscribeInterval );
        property->SetValue( iSpSettings.iResubrcribe );
        spSettings->AddOrUpdatePropertyL( serviceId, *property );

        property->SetName( ESubPropertyVMBXSettingsId );
        property->SetValue( iEntry->iIds[0].iProfileId );
        spSettings->AddOrUpdatePropertyL( serviceId, *property );
        }

    // ==============================
    // "Core" Service Provider Settings.
    // ==============================
    //

    if ( iSpSettings.iProviderUrl )
        {
        property->SetName( EPropertyServiceBookmarkUri );
        property->SetValue( iSpSettings.iProviderUrl->Des() );
        spSettings->AddOrUpdatePropertyL( serviceId, *property );
        }

    if ( iSpSettings.iBrandingUri )
        {
        property->SetName( ESubPropertyVoIPBrandDataUri );
        property->SetValue( iSpSettings.iBrandingUri->Des() );
        spSettings->AddOrUpdatePropertyL( serviceId, *property );
        }

    if ( iPresenceId )
        {
        property->SetName( ESubPropertyPresenceSettingsId );
        property->SetValue( iPresenceId );
        spSettings->AddOrUpdatePropertyL( serviceId, *property );

        // Converged Connection Handler (CCH) Presence Subservice plug-in UID
        property->SetName( EPropertyPresenceSubServicePluginId );
        property->SetValue( KCCHPresenceSubServicePlugId );
        spSettings->AddOrUpdatePropertyL( serviceId, *property );

        // SIP IM enabled
        if ( iSpSettings.iEnableSipIm )
            {
            // Set IM as enabled.
            property->SetName( ESubPropertyIMEnabled );
            property->SetValue( EOn );
            spSettings->AddOrUpdatePropertyL( serviceId, *property );
            // Set IM launch UID.
            property->SetName( ESubPropertyIMLaunchUid );
            property->SetValue( KIMLaunchUid );
            spSettings->AddOrUpdatePropertyL( serviceId, *property );
            // The value only needs to be different from 0,
            // no-one actually uses it.
            property->SetName( ESubPropertyIMSettingsId );
            property->SetValue( KIMSettingsId );
            spSettings->AddOrUpdatePropertyL( serviceId, *property );
            // Set IM plugin UID.
            property->SetName( EPropertyIMSubServicePluginId );
            property->SetValue( KIMSubServicePluginId );
            spSettings->AddOrUpdatePropertyL( serviceId, *property );
            // Set default IM tone.
            TFileName toneFile;
            toneFile.Copy( PathInfo::RomRootPath() );
            toneFile.Append( PathInfo::DigitalSoundsPath() );
            toneFile.Append( KDefaultTone );
            MVIMPSTSettingsStore* vimpStSettings =
                CVIMPSTSettingsStore::NewLC(); // CS:3
            vimpStSettings->SetL( serviceId,
                EServiceToneFileName, toneFile );
            // Pop vimpStSettings (can't use M object as argument).
            CleanupStack::PopAndDestroy(); // CS:2
            }
        }

    if ( iSpSettings.iAutoAcceptBuddies )
        {
        property->SetName( ESubPropertyPresenceRequestPreference );
        property->SetValue( EOn );
        spSettings->AddOrUpdatePropertyL( serviceId, *property );
        }

    if ( iDestinationId )
        {
        property->SetName( ESubPropertyVoIPPreferredSNAPId );
        property->SetValue( iDestinationId );
        spSettings->AddOrUpdatePropertyL( serviceId, *property );
        if ( iSpSettings.iEnableSipIm )
            {
            property->SetName( ESubPropertyIMPreferredSNAPId );
            property->SetValue( iDestinationId );
            spSettings->AddOrUpdatePropertyL( serviceId, *property );
            }
        }

    if ( iSpSettings.iBrandId )
        {
        // Brand version.
        property->SetName( EPropertyBrandVersion );
        property->SetValue( KBrandVersion );
        spSettings->AddOrUpdatePropertyL( serviceId, *property );
        // Brand language.
        property->SetName( EPropertyBrandLanguage );
        property->SetValue( ELangInternationalEnglish );
        spSettings->AddOrUpdatePropertyL( serviceId, *property );
        // Brand ID.
        property->SetName( EPropertyBrandId );
        property->SetValue( iSpSettings.iBrandId->Des() );
        spSettings->AddOrUpdatePropertyL( serviceId, *property );
        }

    if ( iSpSettings.iAutoEnable )
        {
        property->SetName( ESubPropertyVoIPEnabled );
        property->SetValue( EOn );
        spSettings->AddOrUpdatePropertyL( serviceId, *property );
        // Also set voicemailbox on if it's defined.
        if ( vmbx )
            {
            property->SetName( ESubPropertyVMBXEnabled );
            property->SetValue( EOn );
            spSettings->AddOrUpdatePropertyL( serviceId, *property );
            }
        // Also set presence on if there is a presence link.
        if ( iPresenceId )
            {
            property->SetName( ESubPropertyPresenceEnabled );
            property->SetValue( EOn );
            spSettings->AddOrUpdatePropertyL( serviceId, *property );
            }
        }

    // property, spSettings
    CleanupStack::PopAndDestroy( 2, spSettings ); // CS:0
    }

// ---------------------------------------------------------------------------
// Creates default codecs.
// ---------------------------------------------------------------------------
//
void CVoipXmlVoipHandler::AddDefaultCodecsL()
    {
    CRCSEAudioCodecEntry* codec = CRCSEAudioCodecEntry::NewLC();

    TUint32 codecId( KErrNone );

    codec->SetDefaultCodecValueSet( KAudioCodecAMRWB() );
    codecId = iCodecRegistry->AddL( *codec );
    iEntry->iPreferredCodecs.AppendL( codecId );

    codec->SetDefaultCodecValueSet( KAudioCodecAMR() );
    codecId = iCodecRegistry->AddL( *codec );
    iEntry->iPreferredCodecs.AppendL( codecId );

    codec->SetDefaultCodecValueSet( KAudioCodecPCMU() );
    codecId = iCodecRegistry->AddL( *codec );
    iEntry->iPreferredCodecs.AppendL( codecId );

    codec->SetDefaultCodecValueSet( KAudioCodecPCMA() );
    codecId = iCodecRegistry->AddL( *codec );
    iEntry->iPreferredCodecs.AppendL( codecId );

    codec->SetDefaultCodecValueSet( KAudioCodeciLBC() );
    codecId = iCodecRegistry->AddL( *codec );
    iEntry->iPreferredCodecs.AppendL( codecId );

    codec->SetDefaultCodecValueSet( KAudioCodecG729() );
    codecId = iCodecRegistry->AddL( *codec );
    iEntry->iPreferredCodecs.AppendL( codecId );

    codec->SetDefaultCodecValueSet( KAudioCodecCN() );
    codecId = iCodecRegistry->AddL( *codec );
    iEntry->iPreferredCodecs.AppendL( codecId );

    CleanupStack::PopAndDestroy( codec );
    }

// ---------------------------------------------------------------------------
// Sets the service as default service and preferred telephony as PS.
// ---------------------------------------------------------------------------
//
void CVoipXmlVoipHandler::SetAsDefaultL()
    {
    // Set the preferred service ID and preferred telephony as PS.
    CRepository* repository = CRepository::NewL( KCRUidRichCallSettings );
    repository->Set( KRCSPSPreferredService, (TInt)iServiceId );
    repository->Set( KRCSEPreferredTelephony, 1 );
    delete repository;
    }

//  End of File
