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


#include <e32cmn.h>
#include <coecntrl.h>
#include <sipmanagedprofile.h>
#include <sipmanagedprofileregistry.h>
#include <sysutil.h>
#include <wlaninternalpskeys.h>
#include <pathinfo.h>
#include <Authority16.h>
#include <stringloader.h>
#include <escapeutils.h>

#include "voipxmlutils.h"
#include "voipxmlsiphandler.h"
#include "voipxmlprocessorlogger.h"
#include "voipxmlprocessordefaults.h"

// ---------------------------------------------------------------------------
// CVoipXmlSipHandler::CVoipXmlSipHandler
// ---------------------------------------------------------------------------
//
CVoipXmlSipHandler::CVoipXmlSipHandler()
    {
    }

// ---------------------------------------------------------------------------
// CVoipXmlSipHandler::NewL
// ---------------------------------------------------------------------------
//
CVoipXmlSipHandler* CVoipXmlSipHandler::NewL()
    {
    CVoipXmlSipHandler* self = new ( ELeave ) CVoipXmlSipHandler;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CVoipXmlSipHandler::ConstructL
// ---------------------------------------------------------------------------
//
void CVoipXmlSipHandler::ConstructL()
    {
    DBG_PRINT( "CVoipXmlSipHandler::ConstructL begin" );

    // SIP Managed Profile Registry.
    iRegistry = CSIPManagedProfileRegistry::NewL( *this );

    // Create an empty SIP profile in which all settings will be set.
    iProfileType.iSIPProfileClass = TSIPProfileTypeInfo::EInternet;
    iProfileType.iSIPProfileName.Copy( KIetf() );
    iProfile = iRegistry->CreateL( iProfileType );

    iProxyUri           = HBufC8::NewL( KMaxNodeValueLength );
    iProxyTransport     = EAutomatic;
    iProxyPort          = KErrNotFound;
    iLr                 = EFalse;
    iRegistrarUri       = HBufC8::NewL( KMaxNodeValueLength );
    iRegistrarTransport = EAutomatic;
    iRegistrarPort      = KErrNotFound;

    const TInt ipTosShift( 2 );
    const TUint32 tosBits( KDefaultSigQos << ipTosShift );
    iProfile->SetParameter( KSIPSoIpTOS, tosBits );

    DBG_PRINT( "CVoipXmlSipHandler::ConstructL end" );
    }

// ---------------------------------------------------------------------------
// CVoipXmlSipHandler::~CVoipXmlSipHandler
// ---------------------------------------------------------------------------
//
CVoipXmlSipHandler::~CVoipXmlSipHandler()
    {
    delete iProfile;
    delete iRegistry;
    delete iProxyUri;
    delete iRegistrarUri;
    }

// ---------------------------------------------------------------------------
// Sets SIP setting.
// ---------------------------------------------------------------------------
//
void CVoipXmlSipHandler::SetSetting( TInt aType, TInt aParam, 
    const TDesC8& aValue )
    {
    // Ignore too long descriptors.
    if ( KMaxNodeValueLength < aValue.Length() )
        {
        return;
        }

    switch ( aParam )
        {
        case EName:
            {
            TRAPD( err, CreateProviderNameL( aValue ) );
            if ( KErrNone == err )
                {
                iSettingsSet = ETrue;
                }
            break;
            }
        case ESignalingQos:
            {
            // We need to do bitshifting on the IP TOS, because it's the 
            // upper 6 bits that are set and settings provide us the IP TOS
            // as the lower 6 bits.
            // The lower 2 bits are reserver for explicit congestion
            // notification. 
            // See also more from:
            // Symbian Developer Library 
            //    => in_sock.h Global variables
            //       => KSoIpTOS
            const TInt ipTosShift( 2 );
            TInt value( KErrNotFound );
            VoipXmlUtils::Des8ToInt( aValue, value );
            if ( 0 <= value )
                {
                const TUint32 tosBits( value << ipTosShift );
                iProfile->SetParameter( KSIPSoIpTOS, tosBits );
                iSettingsSet = ETrue;
                }
            break;
            }
        case EType:
            {
            TBuf8<KMaxNodeValueLength> value;
            value.Copy( aValue );
            value.UpperCase();
            if ( 0 == value.Compare( KIms() ) )
                {
                iProfileType.iSIPProfileClass = TSIPProfileTypeInfo::EIms;
                value.LowerCase();
                iProfileType.iSIPProfileName.Copy( value );
                iProfile->SetType( iProfileType );
                iSettingsSet = ETrue;
                }
            else if ( 0 == value.Compare( KIetf() ) )
                {
                iProfileType.iSIPProfileClass = 
                    TSIPProfileTypeInfo::EInternet;
                iProfileType.iSIPProfileName.Copy( value );
                iProfile->SetType( iProfileType );
                iSettingsSet = ETrue;
                }
            else
                {
                iProfileType.iSIPProfileClass = TSIPProfileTypeInfo::EOther;
                iProfileType.iSIPProfileName.Copy( aValue );
                iProfile->SetType( iProfileType );
                iSettingsSet = ETrue;
                }
            break;
            }
        case EPublicUserId:
            {
            TBuf8<KMaxNodeValueLength> value;
            value.Copy( aValue );
            TRAPD( err, ModifyPuidL( value ) );
            if ( KErrNone == err )
                {
                iProfile->SetParameter( KSIPUserAor, value );
                iSettingsSet = ETrue;
                }
            break;
            }
        case ESignalCompression:
            {
            TInt value;
            if ( KErrNone == VoipXmlUtils::Des8ToInt( aValue, value ) )
                {
                iProfile->SetParameter( KSIPSigComp, (TBool)value );
                iSettingsSet = ETrue;
                }
            break;
            }
        case ESecurityNegotiation:
            {
            TInt value;
            if ( KErrNone == VoipXmlUtils::Des8ToInt( aValue, value ) )
                {
                iProfile->SetParameter( 
                    KSIPSecurityNegotiation, (TBool)value );
                iSettingsSet = ETrue;
                }
            break;
            }
        case EProfileLock:
            {
            TInt value;
            if ( KErrNone == VoipXmlUtils::Des8ToInt( aValue, value ) )
                {
                iProfile->SetParameter( KSIPProfileLocked, (TBool)value );
                iSettingsSet = ETrue;
                }
            break;
            }
        case EAutoRegistration:
            {
            TInt value;
            if ( KErrNone == VoipXmlUtils::Des8ToInt( aValue, value ) )
                {
                iProfile->SetParameter( KSIPAutoRegistration, (TBool)value );
                iSettingsSet = ETrue;
                }
            break;
            }
        case EUri:
            {
            TBuf8<KMaxNodeValueLength> tempBuf( KNullDesC8 );
            if ( 0 != aValue.Find( KSip() ) &&
                0 != aValue.Find( KSips() ) )
                {
                tempBuf.Copy( KSip() );
                }
            tempBuf.Append( aValue );
            if ( EProxy == aType )
                {
                iProxyUri->Des().Copy( tempBuf );
                iSettingsSet = ETrue;
                }
            else if ( ERegistrar == aType )
                {
                iRegistrarUri->Des().Copy( tempBuf );
                iSettingsSet = ETrue;
                }
            break;
            }
        case ETransport:
            {
            if ( EProxy == aType )
                {
                iProxyTransport = ValidateTransport( aValue );
                iSettingsSet = ETrue;
                }
            else if ( ERegistrar == aType )
                {
                iRegistrarTransport = ValidateTransport( aValue );
                iSettingsSet = ETrue;
                }
            break;
            }
        case EPort:
            {
            TInt value;
            TInt err = VoipXmlUtils::Des8ToInt( aValue, value );
            if ( EProxy == aType && KErrNone == err )
                {
                iProxyPort = value;
                iSettingsSet = ETrue;
                }
            else if ( ERegistrar == aType && KErrNone == err )
                {
                iRegistrarPort = value;
                iSettingsSet = ETrue;
                }
            break;
            }
        case ELooseRouting:
            {
            TInt value;
            if ( EProxy == aType && 
                KErrNone == VoipXmlUtils::Des8ToInt( aValue, value ) )
                {
                iLr = (TBool)value;
                iSettingsSet = ETrue;
                }
            break;
            }
        case EUsername:
            {
            if ( EProxy == aType )
                {
                if ( TSIPProfileTypeInfo::EInternet == 
                    iProfileType.iSIPProfileClass )
                    {
                    iProfile->SetParameter( KSIPOutboundProxy, 
                        KSIPDigestUserName, aValue );
                    iSettingsSet = ETrue;
                    }
                else
                    {
                    iProfile->SetParameter( KSIPPrivateIdentity, aValue );
                    iSettingsSet = ETrue;
                    }
                }
            else if ( ERegistrar == aType )
                {
                if ( TSIPProfileTypeInfo::EInternet == 
                    iProfileType.iSIPProfileClass )
                    {
                    iProfile->SetParameter( KSIPRegistrar, KSIPDigestUserName,
                        aValue );
                    iSettingsSet = ETrue;
                    }
                else
                    {
                    iProfile->SetParameter( KSIPPrivateIdentity, aValue );
                    iSettingsSet = ETrue;
                    }
                }
            break;
            }
        case EPassword:
            {
            if ( EProxy == aType )
                {
                iProfile->SetParameter( KSIPOutboundProxy, KSIPDigestPassword,
                    aValue );
                iSettingsSet = ETrue;
                }
            else if ( ERegistrar == aType )
                {
                iProfile->SetParameter( KSIPRegistrar, KSIPDigestPassword,
                    aValue );
                iSettingsSet = ETrue;
                }
            break;
            }
        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// Stores settings to SIP managed profile registry.
// ---------------------------------------------------------------------------
//
TInt CVoipXmlSipHandler::StoreSettings()
    {
    if ( !iSettingsSet )
        {
        // No settings to be stored => method not supported.
        return KErrNotSupported;
        }

    TBuf8<KMaxNodeValueLength> tempBuf( KNullDesC8 );

    // ==============================
    // Set components to proxy URI.
    // ==============================
    //
    if ( KErrNotFound != iProxyPort )
        {
        tempBuf.Copy( KColon() );
        tempBuf.AppendNum( iProxyPort );
        }
    if ( EUdp == iProxyTransport )
        {
        tempBuf.Append( KTransport() );
        tempBuf.Append( KUdp() );
        }
    else if ( ETcp == iProxyTransport )
        {
        tempBuf.Append( KTransport() );
        tempBuf.Append( KTcp() );
        }
    if ( iLr )
        {
        tempBuf.Append( KLr() );
        }

    if ( iProxyUri->Des().MaxLength() >= 
        ( iProxyUri->Des().Length() + tempBuf.Length() ) )
        {
        iProxyUri->Des().Append( tempBuf );
        }
    iProfile->SetParameter( KSIPOutboundProxy, KSIPServerAddress, 
        iProxyUri->Des() );

    // ==============================
    // Set components to registrar URI.
    // ==============================
    //
    tempBuf.Zero();
    if ( KErrNotFound != iRegistrarPort )
        {
        tempBuf.Copy( KColon() );
        tempBuf.AppendNum( iRegistrarPort );
        }
    if ( EUdp == iRegistrarTransport )
        {
        tempBuf.Append( KTransport() );
        tempBuf.Append( KUdp() );
        }
    else if ( ETcp == iRegistrarTransport )
        {
        tempBuf.Append( KTransport() );
        tempBuf.Append( KTcp() );
        }
    if ( iRegistrarUri->Des().MaxLength() >= 
        ( iRegistrarUri->Des().Length() + tempBuf.Length() ) )
        {
        iRegistrarUri->Des().Append( tempBuf );
        }
    iProfile->SetParameter( KSIPRegistrar, KSIPServerAddress, 
        iRegistrarUri->Des() );

    TRAPD( err, iRegistry->SaveL( *iProfile ) );
    if ( KErrNone != err )
        {
        // No need for specifying what went wrong because paramhandler
        // is only intrested in KErrNotSupported, KErrCompletion and KErrNone.
        err = KErrCompletion;
        }
    // Store SIP settings ID for later use.
    iProfile->GetParameter( KSIPProfileId, iProfileId );
    return err;
    }

// ---------------------------------------------------------------------------
// Returns the profile ID if the profile saved in StoreSettings.
// ---------------------------------------------------------------------------
//
TUint32 CVoipXmlSipHandler::SettingsId()
    {
    return iProfileId;
    }

// ---------------------------------------------------------------------------
// Saves linkage information.
// ---------------------------------------------------------------------------
//
void CVoipXmlSipHandler::LinkSettings( TInt aType, TUint32 aSettingsId )
    {
    if ( EDestination == aType && aSettingsId )
        {
        iProfile->SetParameter( KSIPSnapId, aSettingsId );
        }
    }

// ---------------------------------------------------------------------------
// Finalizes settings saving.
// ---------------------------------------------------------------------------
//
TInt CVoipXmlSipHandler::FinalizeSettings()
    {
    TRAPD( err, iRegistry->SaveL( *iProfile ) );
    if ( KErrNone != err )
        {
        // ParamHandler is only intrested in KErrNone and KErrCompletion.
        err = KErrCompletion;
        }
    return err;
    }

// ---------------------------------------------------------------------------
// From class MSIPProfileRegistryObserver.
// ---------------------------------------------------------------------------
//
void CVoipXmlSipHandler::ProfileRegistryEventOccurred( 
    TUint32 /*aSIPProfileId*/, TEvent /*aEvent*/ )
    {
    }

// ---------------------------------------------------------------------------
// From class MSIPProfileRegistryObserver. 
// ---------------------------------------------------------------------------
//
void CVoipXmlSipHandler::ProfileRegistryErrorOccurred( 
    TUint32 /*aSIPProfileId*/, TInt /*aError*/ )
    {
    }

// ---------------------------------------------------------------------------
// CVoipXmlSipHandler::CreateProviderNameL
// Checks if duplicate named SIP profiles. Renames if same.
// ---------------------------------------------------------------------------
//
void CVoipXmlSipHandler::CreateProviderNameL( const TDesC8& aName )
    {
    DBG_PRINT( "CVoipXmlSipHandler::CreateProviderNameL begin" );

    RPointerArray<CSIPProfile> profiles;
    CleanupResetAndDestroyL( profiles ); // CS:1

    // Get all profiles based on profile types.
    iRegistry->ProfilesL( profiles );
    const TInt profileCount = profiles.Count();

    // Go through loaded profiles and check for name duplicates.
    HBufC8* name = HBufC8::NewLC( KMaxNodeNameLength ); // CS:2
    name->Des().Copy( aName );
    TUint i( 1 ); // Add number to the name if name already in use.
    const TInt count( profiles.Count() );
    for ( TInt counter = 0; counter < count; counter++ )
        {
        CSIPManagedProfile* profile = 
            static_cast<CSIPManagedProfile*>( profiles[counter] );
        const TDesC8* existingName;
        profile->GetParameter( KSIPProviderName, existingName );
        if ( 0 == existingName->Compare( *name ) )
            {
            name->Des().Copy( aName );
            name->Des().Append( KOpenParenthesis8() );
            name->Des().AppendNum( i );
            name->Des().Append( KClosedParenthesis8() );  
            counter = 0;
            i++;
            if ( KMaxProfileNames < i )
                {
                User::Leave( KErrBadName );
                }
            }
        }
    iProfile->SetParameter( KSIPProviderName, name->Des() );

    // name, &profiles
    CleanupStack::PopAndDestroy( 2, &profiles ); // CS:0
    DBG_PRINT( "CVoipXmlSipHandler::CreateProviderNameL end" );
    }

// ---------------------------------------------------------------------------
// Modifies public user ID, i.e. escapes and adds sip: prefix if necessary.
// ---------------------------------------------------------------------------
//
void CVoipXmlSipHandler::ModifyPuidL( TDes8& aValue )
    {
    if ( KErrNotFound == aValue.Find( KEscaped() ) )
        {
        HBufC8* tempPuid = EscapeUtils::EscapeEncodeL( aValue,
            EscapeUtils::EEscapeNormal );
        aValue.Copy( tempPuid->Des() );
        delete tempPuid;
        tempPuid = NULL;
        }
    if ( 0 != aValue.Find( KSip() ) && 0 != aValue.Find( KSips() ) )
        {
        if ( aValue.MaxLength() >= ( aValue.Length() + KSip().Length() ) )
            {
            aValue.Insert( 0, KSip() );
            }
        else
            {
            User::Leave( KErrTooBig );
            }
        }
    }

// ---------------------------------------------------------------------------
// Validates transport type.
// ---------------------------------------------------------------------------
//
CVoipXmlSipHandler::TTransportType CVoipXmlSipHandler::ValidateTransport( 
    const TDesC8& aValue )
    {
    TBuf8<KMaxNodeValueLength> value;
    value.Copy( aValue );
    value.UpperCase();
    if ( 0 == value.Compare( KUdp() ) )
        {
        return EUdp;
        }
    else if ( 0 == value.Compare( KTcp() ) )
        {
        return ETcp;
        }
    else
        {
        return EAutomatic;
        }
    }

// End of file.
