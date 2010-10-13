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


#include <e32cmn.h>
#include <coecntrl.h>
#include <cmmanagerext.h>
#include <cmdestinationext.h>
#include <cmconnectionmethodext.h>
#include <cmconnectionmethoddef.h>
#include <cmpluginwlandef.h>
#include <WPASecuritySettingsUI.h>
#include <WEPSecuritySettingsUI.h>
#include <EapSettings.h>
#include <EapType.h>

#include "voipxmlutils.h"
#include "voipxmliaphandler.h"
#include "voipxmlprocessorlogger.h"

// ---------------------------------------------------------------------------
// CVoipXmlIapHandler::CVoipXmlIapHandler
// ---------------------------------------------------------------------------
//
CVoipXmlIapHandler::CVoipXmlIapHandler()
    {
    }

// ---------------------------------------------------------------------------
// CVoipXmlIapHandler::NewL
// ---------------------------------------------------------------------------
//
CVoipXmlIapHandler* CVoipXmlIapHandler::NewL()
    {
    CVoipXmlIapHandler* self = new ( ELeave ) CVoipXmlIapHandler;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CVoipXmlIapHandler::ConstructL
// ---------------------------------------------------------------------------
//
void CVoipXmlIapHandler::ConstructL()
    {
    iDestinationName          = HBufC::NewL( 0 );
    iDestinationId            = 0;
    iCurrentIap.iSsid         = NULL;
    iCurrentIap.iName         = NULL;
    iCurrentIap.iPreSharedKey = NULL;
    iCurrentIap.iSecurityType = CMManager::EWlanSecModeOpen;
    iCurrentIap.iNetworkMode  = CMManager::EInfra;
    iCurrentIap.iWepAuthMode  = CWEPSecuritySettings::EAuthOpen;
    iCurrentIap.iEapType      = EAPSettings::EEapNone;
    iCurrentIap.iEapUsername  = NULL;
    iCurrentIap.iEapPassword  = NULL;
    }

// ---------------------------------------------------------------------------
// CVoipXmlIapHandler::~CVoipXmlIapHandler
// ---------------------------------------------------------------------------
//
CVoipXmlIapHandler::~CVoipXmlIapHandler()
    {
    if ( iDestinationName )
        {
        delete iDestinationName;
        }
    ResetCurrentIap( ETrue );
    ResetTempIapArray( ETrue );
    }

// ---------------------------------------------------------------------------
// Sets SIP setting.
// ---------------------------------------------------------------------------
//
void CVoipXmlIapHandler::SetSetting( TInt aType, TInt aParam, 
    const TDesC& aValue )
    {
    // Ignore too long descriptors.
    if ( KMaxNodeValueLength < aValue.Length() )
        {
        return;
        }
    TInt intValue;

    switch ( aParam )
        {
        case EName:
            {
            TInt err( KErrNotFound );
            //lint -e{960} No need for else statement here
            if ( EDestination == aType )
                {
                delete iDestinationName;
                iDestinationName = NULL;
                TRAP( err, iDestinationName = aValue.AllocL() );
                }
            else if ( EWlan == aType && !iCurrentIap.iName )
                {
                TRAP( err, iCurrentIap.iName = aValue.AllocL() );
                }
            if ( KErrNone == err )
                {
                iSettingsSet = ETrue;
                }
            break;
            }
        case EType:
            {
            TBuf<KMaxNodeValueLength> value;
            value.Copy( aValue );
            value.UpperCase();

            if ( EEap == aType )
                {
                if ( 0 == value.Compare( KEapTypeLeap ) )
                    {
                    iCurrentIap.iEapType = EAPSettings::EEapLeap;
                    }
                }

            if ( EWlan != aType )
                {
                break;
                }
            //lint -e{960} No need for else statement here
            if ( 0 == value.Compare( KSecurityTypeWep ) )
                {
                iCurrentIap.iSecurityType = CMManager::EWlanSecModeWep;
                }
            else if ( 0 == value.Compare( KSecurityTypeWpa ) )
                {
                iCurrentIap.iSecurityType = CMManager::EWlanSecModeWpa;
                }
            else if ( 0 == value.Compare( KSecurityTypeWpa2 ) )
                {
                iCurrentIap.iSecurityType = CMManager::EWlanSecModeWpa2;
                }
            else if ( 0 == value.Compare( KSecurityType8021x ) )
                {
                iCurrentIap.iSecurityType = CMManager::EWlanSecMode802_1x;
                }
            break;
            }
        case ESsid:
            {
            if ( EWlan == aType && !iCurrentIap.iSsid )
                {
                iCurrentIap.iSsid = aValue.Alloc();
                if ( iCurrentIap.iSsid )
                    {
                    iSettingsSet = ETrue;
                    }
                }
            break;
            }
        case EHidden:
            {
            if ( EWlan == aType &&
                KErrNone == VoipXmlUtils::DesToInt( aValue, intValue ) )
                {
                iCurrentIap.iHidden = intValue;
                }
            break;
            }
        case ENetworkMode:
            {
            if ( EWlan != aType )
                {
                break;
                }
            TBuf<KMaxNodeValueLength> value;
            value.Copy( aValue );
            value.LowerCase();
            //lint -e{960} No need for else statement here
            if ( 0 == value.Compare( KNetworkModeInfra ) )
                {
                iCurrentIap.iNetworkMode = CMManager::EInfra;
                }
            else if ( 0 == value.Compare( KNetworkModeAdhoc ) )
                {
                iCurrentIap.iNetworkMode = CMManager::EAdhoc;
                }
            break;
            }
        case EPreSharedKey:
            {
            if ( EWlan == aType && !iCurrentIap.iPreSharedKey )
                {
                iCurrentIap.iPreSharedKey = aValue.Alloc();
                }
            break;
            }
        case EWepAuthMode:
            {
            if ( EWlan != aType )
                {
                break;
                }
            TBuf<KMaxNodeValueLength> value;
            value.Copy( aValue );
            value.LowerCase();
            //lint -e{960} No need for else statement here
            if ( 0 == value.Compare( KWepAuthModeOpen ) )
                {
                iCurrentIap.iWepAuthMode = CWEPSecuritySettings::EAuthOpen;
                }
            else if ( 0 == value.Compare( KWepAuthModeShared ) )
                {
                iCurrentIap.iWepAuthMode = CWEPSecuritySettings::EAuthShared;
                }
            break;
            }
        case ELength:
            {
            if ( EWepKey == aType && 
                KErrNone == VoipXmlUtils::DesToInt( aValue, intValue ))
                {
                iCurrentIap.iCurrentWepKey.iLength = intValue;
                }
            break;
            }
        case EData:
            {
            if ( EWepKey == aType && KMaxWepKeyDataLength >= aValue.Length() )
                {
                iCurrentIap.iCurrentWepKey.iData.Copy( aValue );
                }
            break;
            }
        case EUsername:
            {
            if ( EEap == aType )
                {
                iCurrentIap.iEapUsername = aValue.Alloc();
                }
            break;
            }
        case EPassword:
            {
            if ( EEap == aType )
                {
                iCurrentIap.iEapPassword = aValue.Alloc();
                }
            break;
            }
        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// Stores settings.
// ---------------------------------------------------------------------------
//
TInt CVoipXmlIapHandler::StoreSettings()
    {
    if ( !iSettingsSet )
        {
        // No settings to be stored => method not supported.
        return KErrNotSupported;
        }
    TRAPD( err, StoreSettingsL() );
    if ( KErrNone != err )
        {
        err = KErrCompletion;
        }
    return err;
    }

// ---------------------------------------------------------------------------
// Returns Destination ID.
// ---------------------------------------------------------------------------
//
TUint32 CVoipXmlIapHandler::SettingsId()
    {
    return iDestinationId;
    }

// ---------------------------------------------------------------------------
// Informs that currently deployed settings have ended.
// ---------------------------------------------------------------------------
//
void CVoipXmlIapHandler::SettingsEnd( TInt aType )
    {
    //lint -e{960} No need for else statement here
    if ( EWepKey == aType && iCurrentIap.iWepKeys.Count() < KMaxWepKeyCount )
        {
        TInt keyDataLength = iCurrentIap.iCurrentWepKey.iData.Length();
        TBool okToAdd( EFalse );
        switch ( keyDataLength )
            {
            case EWepKey64Hex:
                {
                iCurrentIap.iCurrentWepKey.iHex = ETrue;
                okToAdd = ETrue;
                break;
                }
            case EWepKey64Ascii:
                {
                iCurrentIap.iCurrentWepKey.iHex = EFalse;
                okToAdd = ETrue;
                break;
                }
            case EWepKey128Hex:
                {
                iCurrentIap.iCurrentWepKey.iHex = ETrue;
                okToAdd = ETrue;
                break;
                }
            case EWepKey128Ascii:
                {
                iCurrentIap.iCurrentWepKey.iHex = EFalse;
                okToAdd = ETrue;
                break;
                }
            default:
                break;
            }
        if ( okToAdd )
            {
            iCurrentIap.iWepKeys.Append( iCurrentIap.iCurrentWepKey );
            }
        iCurrentIap.iCurrentWepKey.iLength = 0;
        iCurrentIap.iCurrentWepKey.iData.Zero();
        }
    else if ( EWlan == aType )
        {
        TRAP_IGNORE( AddCurrentIapL() );
        ResetCurrentIap();
        }
    }

// ---------------------------------------------------------------------------
// Resets iCurrentIap members.
// ---------------------------------------------------------------------------
//
void CVoipXmlIapHandler::ResetCurrentIap( TBool aCloseArray )
    {
    if ( iCurrentIap.iName )
        {
        delete iCurrentIap.iName;
        iCurrentIap.iName = NULL;
        }
    if ( iCurrentIap.iPreSharedKey )
        {
        delete iCurrentIap.iPreSharedKey;
        iCurrentIap.iPreSharedKey = NULL;
        }
    if ( iCurrentIap.iSsid )
        {
        delete iCurrentIap.iSsid;
        iCurrentIap.iSsid = NULL;
        }
    iCurrentIap.iWepKeys.Reset();
    if ( aCloseArray )
        {
        iCurrentIap.iWepKeys.Close();
        }
    iCurrentIap.iHidden = EFalse;
    iCurrentIap.iSecurityType = CMManager::EWlanSecModeOpen;
    iCurrentIap.iNetworkMode = CMManager::EInfra;
    iCurrentIap.iWepAuthMode = CWEPSecuritySettings::EAuthOpen;

    iCurrentIap.iEapType = EAPSettings::EEapNone;
    if ( iCurrentIap.iEapUsername )
        {
        delete iCurrentIap.iEapUsername;
        iCurrentIap.iEapUsername = NULL;
        }
    if ( iCurrentIap.iEapPassword )
        {
        delete iCurrentIap.iEapPassword;
        iCurrentIap.iEapPassword = NULL;
        }
    }

// ---------------------------------------------------------------------------
// Resets iIaps members.
// ---------------------------------------------------------------------------
//
void CVoipXmlIapHandler::ResetTempIapArray( TBool aCloseArray )
    {
    const TInt count = iIaps.Count();
    for ( TInt counter = 0; counter < count; counter++ )
        {
        if ( iIaps[counter]->iName )
            {
            delete iIaps[counter]->iName;
            iIaps[counter]->iName = NULL;
            }
        if ( iIaps[counter]->iPreSharedKey )
            {
            delete iIaps[counter]->iPreSharedKey;
            iIaps[counter]->iPreSharedKey = NULL;
            }
        if ( iIaps[counter]->iSsid )
            {
            delete iIaps[counter]->iSsid;
            iIaps[counter]->iSsid = NULL;
            }
        iIaps[counter]->iWepKeys.Reset();
        if ( aCloseArray )
            {
            iIaps[counter]->iWepKeys.Close();
            }
        if ( iIaps[counter]->iEapUsername )
            {
            delete iIaps[counter]->iEapUsername;
            iIaps[counter]->iEapUsername = NULL;
            }
        if ( iIaps[counter]->iEapPassword )
            {
            delete iIaps[counter]->iEapPassword;
            iIaps[counter]->iEapPassword = NULL;
            }
        }
    iIaps.ResetAndDestroy();
    if ( aCloseArray )
        {
        iIaps.Close();
        }
    }

// ---------------------------------------------------------------------------
// Adds iCurrentIap into iIaps array, i.e. copies its values to a pointer
// and appends that one into the array.
// ---------------------------------------------------------------------------
//
void CVoipXmlIapHandler::AddCurrentIapL()
    {
    if ( !iCurrentIap.iSsid )
        {
        // If there is no SSID, we won't add the IAP to array.
        return;
        }
    TTemporaryIap* iap = new TTemporaryIap;
    iap->iSsid = HBufC::NewLC( KMaxNodeValueLength ); // CS:1
    iap->iName = HBufC::NewLC( KMaxNodeValueLength ); // CS:2
    iap->iPreSharedKey = HBufC::NewLC( KMaxNodeValueLength ); // CS:3
    iap->iEapUsername = HBufC::NewLC( KMaxNodeValueLength ); // CS:4
    iap->iEapPassword = HBufC::NewLC( KMaxNodeValueLength ); // CS:5

    iap->iSsid->Des().Copy( iCurrentIap.iSsid->Des() );
    if ( iCurrentIap.iName )
        {
        iap->iName->Des().Copy( iCurrentIap.iName->Des() );
        }
    else
        {
        iap->iName->Des().Copy( iCurrentIap.iSsid->Des() );
        }

    if ( iCurrentIap.iPreSharedKey )
        {
        iap->iPreSharedKey->Des().Copy( iCurrentIap.iPreSharedKey->Des() );
        }

    iap->iEapType = iCurrentIap.iEapType;
    if ( iCurrentIap.iEapUsername )
        {
        iap->iEapUsername->Des().Copy( iCurrentIap.iEapUsername->Des() );
        }
    if ( iCurrentIap.iEapPassword )
        {
        iap->iEapPassword->Des().Copy( iCurrentIap.iEapPassword->Des() );
        }

    iap->iHidden = iCurrentIap.iHidden;
    iap->iNetworkMode = iCurrentIap.iNetworkMode;
    iap->iSecurityType = iCurrentIap.iSecurityType;
    iap->iWepAuthMode = iCurrentIap.iWepAuthMode;
    const TInt count = iCurrentIap.iWepKeys.Count();
    for ( TInt counter = 0; counter < count; counter++ )
        {
        iap->iWepKeys.Append( iCurrentIap.iWepKeys[counter] );
        }
    iIaps.AppendL( iap );
    CleanupStack::Pop( 5, iap->iSsid );
    }

// ---------------------------------------------------------------------------
// Stores settings.
// ---------------------------------------------------------------------------
//
void CVoipXmlIapHandler::StoreSettingsL()
    {
    RCmManagerExt cmm;
    cmm.OpenLC(); // CS:1

    // First create all access points and store their ID's.
    const TInt iapCount = iIaps.Count();
    RArray<TUint32> iapIds;
    CleanupClosePushL( iapIds ); // CS:2
    for ( TInt counter = 0; counter < iapCount; counter++ )
        {
        TUint32 id = CreateIapL( cmm, *iIaps[counter] );
        iapIds.AppendL( id );
        }

    // Create a destination if one was configured.
    if ( iDestinationName->Des().Length() )
        {
        RArray<TUint32> destinationIds;
        // Get destination ID's for checking if name to be set is reserved.
        CleanupClosePushL( destinationIds ); // CS:3
        cmm.AllDestinationsL( destinationIds );
        HBufC* newName = HBufC::NewLC( KMaxNodeNameLength ); // CS:4
        newName->Des().Copy( iDestinationName->Des() );

        // Check that name is unique.
        const TInt destinationCount = destinationIds.Count();
        for ( TInt counter = 0; counter < destinationCount; counter++ )
            {
            RCmDestinationExt destination = cmm.DestinationL( 
                destinationIds[counter] );
            CleanupClosePushL( destination ); // CS:5
            HBufC* settingsName = destination.NameLC(); // CS:6
            TUint i( 1 ); // Add number to the name if name already in use.
            if ( 0 == newName->Des().Compare( settingsName->Des() ) )
                {
                // If the name is changed we need to begin the comparison
                // again from the first profile.
                newName->Des().Copy( iDestinationName->Des() );
                newName->Des().Append( KOpenParenthesis() );
                newName->Des().AppendNum( i );
                newName->Des().Append( KClosedParenthesis() );  
                counter = 0;
                i++;
                if ( KMaxProfileNames < i )
                    {
                    User::Leave( KErrBadName );
                    }
                }
            // settingsName, &destination
            CleanupStack::PopAndDestroy( 2, &destination ); // CS:4
            }
        RCmDestinationExt newDestination = cmm.CreateDestinationL( *newName );
        CleanupClosePushL( newDestination ); // CS:5
        // We need to run UpdateL in order to get destination ID.
        newDestination.UpdateL();
        iDestinationId = newDestination.Id();
        const TInt cmCount = iapIds.Count();
        for ( TInt counter = 0; counter < cmCount; counter++ )
            {
            RCmConnectionMethodExt connection = cmm.ConnectionMethodL( 
                iapIds[counter] );
            CleanupClosePushL( connection );
            newDestination.AddConnectionMethodL( connection );
            CleanupStack::PopAndDestroy( &connection );
            }
        newDestination.UpdateL();

        // &newDestination, newName, &destinationIds
        CleanupStack::PopAndDestroy( 3, &destinationIds ); // CS:2
        }
    // &iapIds, &cmm
    CleanupStack::PopAndDestroy( 2, &cmm ); // CS:0
    }

// ---------------------------------------------------------------------------
// Creates an actual IAP.
// ---------------------------------------------------------------------------
//
TUint32 CVoipXmlIapHandler::CreateIapL( RCmManagerExt& aCmManager, 
    TTemporaryIap aTempIap )
    {
    DBG_PRINT( "CVoipXmlIapHandler::CreateIapL - begin" );
    RCmConnectionMethodExt newConnMethod = 
        aCmManager.CreateConnectionMethodL( KUidWlanBearerType );
    CleanupClosePushL( newConnMethod ); // CS:1
    newConnMethod.SetStringAttributeL( CMManager::ECmName, 
        aTempIap.iName->Des() );
    newConnMethod.SetStringAttributeL( CMManager::EWlanSSID, 
        aTempIap.iSsid->Des() );
    newConnMethod.SetIntAttributeL( CMManager::EWlanConnectionMode, 
        aTempIap.iNetworkMode );
    if ( aTempIap.iHidden )
        {
        newConnMethod.SetBoolAttributeL( CMManager::EWlanScanSSID, ETrue );
        }
    newConnMethod.SetIntAttributeL( CMManager::EWlanSecurityMode, 
        aTempIap.iSecurityType );
    newConnMethod.UpdateL();
    TUint32 wlanId = newConnMethod.GetIntAttributeL( 
        CMManager::EWlanServiceId );
    TUint32 iapId = newConnMethod.GetIntAttributeL( CMManager::ECmIapId );
    TUint32 iapServiceId = newConnMethod.GetIntAttributeL( 
        CMManager::ECmIapServiceId );
    
    if ( aTempIap.iSecurityType == CMManager::EWlanSecModeWep )
        {
        DBG_PRINT( "   secMode WEP" );
        CMDBSession* db = CMDBSession::NewLC( CMDBSession::LatestVersion() );
        // CS:1
        CWEPSecuritySettings* wepSecSettings = 
            CWEPSecuritySettings::NewL();
        CleanupStack::PushL( wepSecSettings ); // CS:3
        const TInt wepKeyCount = aTempIap.iWepKeys.Count();
        for ( TInt counter = 0; counter < wepKeyCount; counter++ )
            {
            User::LeaveIfError( wepSecSettings->SetKeyDataL( 
                counter, aTempIap.iWepKeys[counter].iData, 
                aTempIap.iWepKeys[counter].iHex ) );
            }
        wepSecSettings->SaveL( wlanId, *db );
        // wepSecSettings, db
        CleanupStack::PopAndDestroy( 2, db ); // CS:1
        }
    else if ( CMManager::EWlanSecModeWpa == aTempIap.iSecurityType ||
        CMManager::EWlanSecModeWpa2 == aTempIap.iSecurityType )
        {
        DBG_PRINT( "   secMode WPA/WPA2" );
        CMDBSession* db = CMDBSession::NewLC( 
            CMDBSession::LatestVersion() ); // CS:2
        CWPASecuritySettings* wpaSecSettings = 
            CWPASecuritySettings::NewL( ESecurityModeWpa );
        CleanupStack::PushL( wpaSecSettings ); // CS:3

        if ( EAPSettings::EEapNone == aTempIap.iEapType )
            {
            DBG_PRINT( "   EapType none" );
            //If error happens then delete newConnMethod and leave
            TInt error = wpaSecSettings->SetWPAPreSharedKey( 
                aTempIap.iPreSharedKey->Des() );
            if ( error )
                {
                newConnMethod.DeleteL();
                User::Leave( error );
                }
            wpaSecSettings->SaveL( wlanId, *db, ESavingBrandNewAP, 0 );
            }
        else if ( EAPSettings::EEapLeap == aTempIap.iEapType )
            {
            DBG_PRINT( "   eapType Leap" );

            TBuf8<KMaxNodeValueLength> eapId;
            eapId.Copy( KEapLeapId, KEapChars );
            
            TInt error = wpaSecSettings->SetWPAEnabledEAPPlugin( eapId );
            if ( error )
                {
                newConnMethod.DeleteL();
                User::Leave( error );
                }
            wpaSecSettings->SaveL( wlanId, *db, ESavingBrandNewAP, 0 );

            CEapType* eapType = CEapType::NewL( eapId, ELan, wlanId );
            CleanupStack::PushL( eapType ); // CS:4
            EAPSettings* eapSettings = new (ELeave) EAPSettings();
            CleanupStack::PushL( eapSettings ); // CS:5
            eapSettings->iEAPType = EAPSettings::EEapLeap;
            eapSettings->iUsername.Copy( aTempIap.iEapUsername->Des() );
            eapSettings->iUsernamePresent = ETrue;
            eapSettings->iPassword.Copy( aTempIap.iEapPassword->Des() );
            eapSettings->iPasswordPresent = ETrue;
            eapType->SetConfigurationL( *eapSettings );
            CleanupStack::PopAndDestroy( 2, eapType ); // CS:3
            }
        // wpaSecSettings, db
        CleanupStack::PopAndDestroy( 2, db ); // CS:1
        }
    else if ( CMManager::EWlanSecMode802_1x == aTempIap.iSecurityType )
        {
        DBG_PRINT( "   secMode 802.1X" );
        CMDBSession* db = CMDBSession::NewLC( 
            CMDBSession::LatestVersion() ); // CS:1
        CWPASecuritySettings* wpaSecSettings = 
            CWPASecuritySettings::NewL( ESecurityMode8021x );
        CleanupStack::PushL( wpaSecSettings ); // CS:2
        TInt error = wpaSecSettings->SetWPAPreSharedKey( 
            aTempIap.iPreSharedKey->Des() );
        if ( error )
            {
            newConnMethod.DeleteL();
            User::Leave( error );
            }
        wpaSecSettings->SaveL( wlanId, *db, ESavingBrandNewAP, 0 );
        // wpaSecSettings, db
        CleanupStack::PopAndDestroy( 2, db ); // CS:1
        }
    CleanupStack::PopAndDestroy( &newConnMethod ); // CS:0
    
    DBG_PRINT( "CVoipXmlIapHandler::CreateIapL - end" );
    return iapId;
    }

// End of file.
