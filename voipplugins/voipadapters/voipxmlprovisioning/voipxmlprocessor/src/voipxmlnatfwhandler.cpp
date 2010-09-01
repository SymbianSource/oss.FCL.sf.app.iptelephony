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


#include <centralrepository.h>
#include <unsafprotocolsinternalcrkeys.h>

#include "voipxmlutils.h"
#include "voipxmlnatfwhandler.h"
#include "voipxmlprocessorlogger.h"
//#include "voipxmlprocessordefaults.h"

// ---------------------------------------------------------------------------
// CVoipXmlNatFwHandler::CVoipXmlNatFwHandler
// ---------------------------------------------------------------------------
//
CVoipXmlNatFwHandler::CVoipXmlNatFwHandler()
    {
    }

// ---------------------------------------------------------------------------
// CVoipXmlNatFwHandler::NewL
// ---------------------------------------------------------------------------
//
CVoipXmlNatFwHandler* CVoipXmlNatFwHandler::NewL()
    {
    CVoipXmlNatFwHandler* self = new ( ELeave ) CVoipXmlNatFwHandler;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CVoipXmlNatFwHandler::ConstructL
// ---------------------------------------------------------------------------
//
void CVoipXmlNatFwHandler::ConstructL()
    {
    iDomain             = HBufC8::NewL( KMaxNodeValueLength );
    iStunSrvAddr        = HBufC8::NewL( KMaxNodeValueLength );
    iStunSrvUsername    = HBufC8::NewL( KMaxNodeValueLength );
    iStunSrvPassword    = HBufC8::NewL( KMaxNodeValueLength );
    iNatProtocol        = HBufC8::NewL( KMaxNodeValueLength );
    iNatProtocol->Des().Copy( KDefaultNatProtocol );
    iStunSrvPort        = KDefaultStunServerPort;
    iTcpRefreshInterval = KDefaultTcpRefreshInterval;
    iUdpRefreshInterval = KDefaultUdpRefreshInterval;
    iStartPortRange     = KDefaultStartPortRange;
    iEndPortRange       = KDefaultEndPortRange;
    iCurrentAdditionalStunServer.iStunSrvPort = KDefaultStunServerPort;
    }

// ---------------------------------------------------------------------------
// CVoipXmlNatFwHandler::~CVoipXmlNatFwHandler
// ---------------------------------------------------------------------------
//
CVoipXmlNatFwHandler::~CVoipXmlNatFwHandler()
    {
    delete iDomain;
    delete iStunSrvAddr;
    delete iStunSrvUsername;
    delete iStunSrvPassword;
    delete iNatProtocol;
    iAdditionalStunServers.ResetAndDestroy();
    iAdditionalStunServers.Close();
    }

// ---------------------------------------------------------------------------
// Sets a NAT/Firewall setting.
// ---------------------------------------------------------------------------
//
void CVoipXmlNatFwHandler::SetSetting( TInt aType, TInt aParam,
    const TDesC8& aValue )
    {
    // Ignore too long descriptors.
    if ( KMaxNodeValueLength < aValue.Length() )
        {
        return;
        }

    TInt intVal( KErrNotFound );
    switch ( aParam )
        {
        case EDomain:
            {
            iDomain->Des().Copy( aValue );
            iSettingsSet = ETrue;
            break;
            }
        case EUri:
            {
            //lint -e{960} No need for else statement here
            if ( ENatFw == aType )
                {
                iStunSrvAddr->Des().Copy( aValue );
                iSettingsSet = ETrue;
                }
            else if ( EAdditionalStun == aType )
                {
                iCurrentAdditionalStunServer.iStunSrvAddr.Copy( aValue );
                iSettingsSet = ETrue;
                }
            break;
            }
        case EPort:
            {
            //lint -e{960} No need for else statement here
            if ( ENatFw == aType && KErrNone == VoipXmlUtils::Des8ToInt( 
                aValue, intVal ) )
                {
                iStunSrvPort = intVal;
                iSettingsSet = ETrue;
                }
            else if ( EAdditionalStun == aType && 
                KErrNone == VoipXmlUtils::Des8ToInt( aValue, intVal ) )
                {
                iCurrentAdditionalStunServer.iStunSrvPort = intVal;
                iSettingsSet = ETrue;
                }
            break;
            }
        case ETcpRefreshInterval:
            {
            if ( KErrNone == VoipXmlUtils::Des8ToInt( aValue, intVal ) )
                {
                iTcpRefreshInterval = intVal;
                iSettingsSet = ETrue;
                }
            break;
            }
        case EUdpRefreshInterval:
            {
            if ( KErrNone == VoipXmlUtils::Des8ToInt( aValue, intVal ) )
                {
                iUdpRefreshInterval = intVal;
                iSettingsSet = ETrue;
                }
            break;
            }
        case ECrlfRefresh:
            {
            if ( KErrNone == VoipXmlUtils::Des8ToInt( aValue, intVal ) )
                {
                iCrlfRefresh = intVal;
                iSettingsSet = ETrue;
                }
            break;
            }
        case EUsername:
            {
            //lint -e{960} No need for else statement here
            if ( ENatFw == aType )
                {
                iStunSrvUsername->Des().Copy( aValue );
                iSettingsSet = ETrue;
                }
            else if ( EAdditionalStun == aType )
                {
                iCurrentAdditionalStunServer.iStunSrvUsername.Copy( aValue );
                iSettingsSet = ETrue;
                }
            break;
            }
        case EPassword:
            {
            //lint -e{960} No need for else statement here
            if ( ENatFw == aType )
                {
                iStunSrvPassword->Des().Copy( aValue );
                iSettingsSet = ETrue;
                }
            else if ( EAdditionalStun == aType )
                {
                iCurrentAdditionalStunServer.iStunSrvPassword.Copy( aValue );
                iSettingsSet = ETrue;
                }
            break;
            }
        case EStunSharedSecret:
            {
            if ( KErrNone == VoipXmlUtils::Des8ToInt( aValue, intVal ) )
                {
                iStunSharedSecret = intVal;
                iSettingsSet = ETrue;
                }
            break;
            }
        case EStartPort:
            {
            if ( KErrNone == VoipXmlUtils::Des8ToInt( aValue, intVal ) )
                {
                iStartPortRange = intVal;
                iSettingsSet = ETrue;
                }
            break;
            }
        case EEndPort:
            {
            if ( KErrNone == VoipXmlUtils::Des8ToInt( aValue, intVal ) )
                {
                iEndPortRange = intVal;
                iSettingsSet = ETrue;
                }
            break;
            }
        case EType:
            {
            iNatProtocol->Des().Copy( aValue );
            break;
            }
        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// Stores settings to Central Repository.
// ---------------------------------------------------------------------------
//
TInt CVoipXmlNatFwHandler::StoreSettings()
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
// Appends currently modified additional STUN server to internal array.
// ---------------------------------------------------------------------------
//
void CVoipXmlNatFwHandler::SettingsEnd( TInt aType )
    {
    if ( EAdditionalStun == aType )
        {
        TAdditionalStun* temp = new TAdditionalStun;
        temp->iStunSrvAddr.Copy( iCurrentAdditionalStunServer.iStunSrvAddr );
        temp->iStunSrvPort = iCurrentAdditionalStunServer.iStunSrvPort;
        temp->iStunSrvUsername.Copy( 
            iCurrentAdditionalStunServer.iStunSrvUsername );
        temp->iStunSrvPassword.Copy( 
            iCurrentAdditionalStunServer.iStunSrvPassword );
        iAdditionalStunServers.Append( temp );
        iCurrentAdditionalStunServer.iStunSrvAddr.Copy( KNullDesC8 );
        iCurrentAdditionalStunServer.iStunSrvPort = KDefaultStunServerPort;
        iCurrentAdditionalStunServer.iStunSrvUsername.Copy( KNullDesC8 );
        iCurrentAdditionalStunServer.iStunSrvPassword.Copy( KNullDesC8 );
        }
    }

// ---------------------------------------------------------------------------
// Commits Central Repository storage.
// ---------------------------------------------------------------------------
//
void CVoipXmlNatFwHandler::StoreSettingsL()
    {
    CRepository* rep = CRepository::NewLC( KCRUidUNSAFProtocols ); // CS:1

    RArray<TUint32> keys;
    CleanupClosePushL( keys ); // CS:2

    // Get next free Domain key.
    rep->FindL( KUNSAFProtocolsDomainMask, 
        KUNSAFProtocolsFieldTypeMask, keys );
    TInt keyCount = keys.Count();
    TInt tmp = 0;
    if ( !keyCount )
        {
        tmp = KUNSAFProtocolsDomainTableMask;
        }
    else
        {
        tmp = keys[keyCount - 1] + 1;
        }

    rep->FindEqL( KUNSAFProtocolsDomainMask, KUNSAFProtocolsFieldTypeMask,
        iDomain->Des(), keys );
    if ( keys.Count() )
        {
        tmp = keys[0];
        }

    TUint32 currentKey = tmp|KUNSAFProtocolsDomainMask;
    currentKey &= KUNSAFProtocolsKeyMask;

    TUint32 currentDomainKey = tmp|KUNSAFProtocolsFieldTypeMask;
    currentDomainKey ^= KUNSAFProtocolsFieldTypeMask;

    TUint32 stunKey = KUNSAFProtocolsSubTableFieldTypeMask;
    stunKey ^= KUNSAFProtocolsSubTableFieldTypeMask;
    stunKey |= currentDomainKey;

    // Delete all existing additional STUN servers if there are any.
    RArray<TUint32> stunKeys;
    CleanupClosePushL( stunKeys ); // CS:3
    TInt err = rep->FindL( 
        currentDomainKey|KUNSAFProtocolsSTUNAddressMask, 
        KUNSAFProtocolsSubTableFieldTypeMask, stunKeys );
    const TInt stunKeyCount = stunKeys.Count();
    for ( TInt counter = 0 ; counter < stunKeyCount; counter++ )
        {
        TUint32 key = KUNSAFProtocolsSTUNAddressMask^
            stunKeys[counter];
        rep->Delete( key|KUNSAFProtocolsSTUNAddressMask );
        rep->Delete( key|KUNSAFProtocolsSTUNPortMask );
        rep->Delete( key|KUNSAFProtocolsSTUNUsernameMask );
        rep->Delete( key|KUNSAFProtocolsSTUNPasswordMask );
        }
    CleanupStack::PopAndDestroy( &stunKeys ); // CS:2

    // Set new keys.

    // Domain
    User::LeaveIfError( rep->Set( currentKey|KUNSAFProtocolsDomainMask,
        iDomain->Des() ));

    // STUN server address.
    User::LeaveIfError( rep->Set( 
        KUNSAFProtocolsSTUNServerMask|currentDomainKey, 
        iStunSrvAddr->Des() ) );
    // Set the same value into STUN server table.
    User::LeaveIfError( rep->Set( 
        KUNSAFProtocolsSTUNAddressMask|stunKey,
        iStunSrvAddr->Des() ) );

    // STUN server port.
    User::LeaveIfError( rep->Set( 
        KUNSAFProtocolsSTUNServerPortMask|currentDomainKey, 
        iStunSrvPort ) );
    // Set the same value into STUN server table.
    User::LeaveIfError( rep->Set( 
        KUNSAFProtocolsSTUNPortMask|stunKey,
        iStunSrvPort ) );

    // TCP refresh interval.
    User::LeaveIfError( rep->Set( 
        KUNSAFProtocolsDomainIntervalTCPMask|currentDomainKey, 
        iTcpRefreshInterval ) );

    // UDP refresh interval.
    User::LeaveIfError( rep->Set( 
        KUNSAFProtocolsDomainIntervalUDPMask|currentDomainKey, 
        iUdpRefreshInterval ) );

    // CRLF refresh.
    User::LeaveIfError( rep->Set( 
        KUNSAFProtocolsDomainEnableCRLFRefresh|currentDomainKey,
        iCrlfRefresh ) );

    // STUN server username
    User::LeaveIfError( rep->Set( 
        KUNSAFProtocolsSTUNUsernameMask|stunKey, 
        iStunSrvUsername->Des() ) );

    // STUN server password
    User::LeaveIfError( rep->Set( 
        KUNSAFProtocolsSTUNPasswordMask|stunKey, 
        iStunSrvPassword->Des() ) );

    // STUN shared secret
    User::LeaveIfError( rep->Set( currentDomainKey|
        KUNSAFProtocolsDomainSharedSecretNotSupported,
        !iStunSharedSecret ) );

    // Start port range
    User::LeaveIfError( rep->Set( 
        currentDomainKey|KUNSAFProtocolsPortPoolStartPortMask,
        iStartPortRange ) );

    // End port range.
    User::LeaveIfError( rep->Set( 
        currentDomainKey|KUNSAFProtocolsPortPoolEndPortMask,
        iEndPortRange ) );

    // Used NAT protocol.
    User::LeaveIfError( rep->Set(
        currentDomainKey|KUNSAFProtocolsUsedNATProtocolMask,
        iNatProtocol->Des() ) );

    // ==============================
    // Additional STUN servers
    // ==============================
    //
    const TInt count = iAdditionalStunServers.Count();
    for ( TInt counter = 0; counter < count; counter++ )
        {
        stunKey |= KUNSAFProtocolsSubTableFieldTypeMask;
        stunKey++;
        stunKey |= KUNSAFProtocolsSubTableFieldTypeMask;
        stunKey ^= KUNSAFProtocolsSubTableFieldTypeMask;
        stunKey |= currentDomainKey;

        // STUNServerAddress
        User::LeaveIfError( rep->Create( 
            KUNSAFProtocolsSTUNAddressMask|stunKey,
            iAdditionalStunServers[counter]->iStunSrvAddr ) );

        // STUNServerPort
        User::LeaveIfError( rep->Create( 
            KUNSAFProtocolsSTUNPortMask|stunKey,
            iAdditionalStunServers[counter]->iStunSrvPort ) );

        // STUNServerUsername
        User::LeaveIfError( rep->Create( 
            KUNSAFProtocolsSTUNUsernameMask|stunKey,
            iAdditionalStunServers[counter]->iStunSrvUsername ) );

        // STUNServerPassword
        User::LeaveIfError( rep->Create( 
            KUNSAFProtocolsSTUNPasswordMask|stunKey,
            iAdditionalStunServers[counter]->iStunSrvPassword ) );
        }

    // &keys, rep
    CleanupStack::PopAndDestroy( 2, rep ); // CS:0
    }

// End of file.
