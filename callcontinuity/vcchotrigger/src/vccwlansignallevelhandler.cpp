/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of the WLAN signal level handler
*
*/



#include <wlanmgmtclient.h>
#include "vccwlansignallevelhandler.h"
#include "rubydebug.h"
#include "vccengpsproperty.h"
#include <ccpdefs.h>

// Min. signal strength.
static const TInt32 KStrengthMin = 110;

// ---------------------------------------------------------------------------
// C++ destructor.
// ---------------------------------------------------------------------------
//
CVccWlanSignalLevelHandler::~CVccWlanSignalLevelHandler()
    {
    // Cancel any request, if outstanding
    Cancel();
#ifndef __WINS__
    delete iWlanMgmt;
#endif
    }

// ---------------------------------------------------------------------------
// Symbian constructor.
// ---------------------------------------------------------------------------
//
CVccWlanSignalLevelHandler* CVccWlanSignalLevelHandler::NewL(
    MVccSignalLevelObserver& aObserver,
    const TSignalLevelParams& aParams, CVccEngPsProperty& aPsProperty )
    {
    CVccWlanSignalLevelHandler* self =
        CVccWlanSignalLevelHandler::NewLC( aObserver, aParams, aPsProperty );

    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------------------------
// Symbian constructor.
// ---------------------------------------------------------------------------
//
CVccWlanSignalLevelHandler* CVccWlanSignalLevelHandler::NewLC(
    MVccSignalLevelObserver& aObserver,
    const TSignalLevelParams& aParams, CVccEngPsProperty& aPsProperty )
    {
    CVccWlanSignalLevelHandler * self =
        new ( ELeave ) CVccWlanSignalLevelHandler( aObserver, aParams, aPsProperty );

    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// C++ constructor.
// ---------------------------------------------------------------------------
//
CVccWlanSignalLevelHandler::CVccWlanSignalLevelHandler(
    MVccSignalLevelObserver& aObserver,
    const TSignalLevelParams& aParams, CVccEngPsProperty& aPsProperty )
    : CVccSignalLevelHandler( aObserver, aParams ), iVccPsp( aPsProperty )
    {
    }

// ---------------------------------------------------------------------------
// Symbian second-phase constructor.
// ---------------------------------------------------------------------------
//
void CVccWlanSignalLevelHandler::ConstructL()
    {
    RUBY_DEBUG_BLOCKL( "CVccWlanSignalLevelHandler::ConstructL" );

    CVccSignalLevelHandler::ConstructL();
#ifndef __WINS__
    //WlanMgmtClient is not started when the phone is not f. ex. labeled
    TRAP_IGNORE( iWlanMgmt = CWlanMgmtClient::NewL() );
#endif
    }

// ---------------------------------------------------------------------------
// Enable notifications.
// ---------------------------------------------------------------------------
//
void CVccWlanSignalLevelHandler::EnableNotificationsL()
    {
    RUBY_DEBUG_BLOCK( "CVccWlanSignalLevelHandler::EnableNotificationsL" );
#ifndef __WINS__
    if( iWlanMgmt )
        {
        TInt error = iWlanMgmt->UpdateRssNotificationBoundary(
            iParams.iLowLevel,
            iParams.iLowLevel - iParams.iHighLevel );

        User::LeaveIfError( error );

        iWlanMgmt->ActivateNotificationsL( *this );
        }
#endif
    }

// ---------------------------------------------------------------------------
// Disable notifications.
// ---------------------------------------------------------------------------
//
void CVccWlanSignalLevelHandler::DisableNotifications()
    {
    RUBY_DEBUG_BLOCK( "CVccWlanSignalLevelHandler::DisableNotificationsL" );
#ifndef __WINS__
    if( iWlanMgmt )
        {
        iWlanMgmt->CancelNotifications();
        }
#endif
    }

// ---------------------------------------------------------------------------
// Get signal strength.
// ---------------------------------------------------------------------------
//
void CVccWlanSignalLevelHandler::GetStrength()
    {

    RUBY_DEBUG_BLOCK( "CVccWlanSignalLevelHandler::GetStrength" );

    iStrength = KStrengthMin;
#ifndef __WINS__
    if( iWlanMgmt )
        {
        iWlanMgmt->GetConnectionSignalQuality( iStrength );
        }
#endif
    RUBY_DEBUG1( " -strength = %d", iStrength );

    // Because the RMobilePhone used in GSM is asynchronous we need here
    // to signal that the request is complete (in order to continue
    // processing stuff in RunL)

    TRequestStatus *sP = &iStatus;

    User::RequestComplete( sP, KErrNone );
    }

// ---------------------------------------------------------------------------
// Cancel outstanding GetStrength.
// We do not need to do anything since WLAN GetStrength is synchronous.
// ---------------------------------------------------------------------------
//
void CVccWlanSignalLevelHandler::CancelGetStrength()
    {
    RUBY_DEBUG_BLOCK( "CVccWlanSignalLevelHandler::CancelGetStrength" );
    }

// ---------------------------------------------------------------------------
// Handles indication of changed RSS value.
// ---------------------------------------------------------------------------
//
void CVccWlanSignalLevelHandler::RssChanged( TWlanRssClass aRssClass, TUint aRss )
    {
    RUBY_DEBUG_BLOCK( "CVccWlanSignalLevelHandler::RssChanged" );
    RUBY_DEBUG2( " -class = %d rss = %d", aRssClass, aRss);

    // Do some basic check
    // Zero (0) is not acceptable strength (too good?).

    iStrength = aRss ? aRss : KStrengthMin;
    StrengthChanged();
    }

// ---------------------------------------------------------------------------
// Handles BSSID has changed (i.e. AP handover) situation.
// ---------------------------------------------------------------------------
//
void CVccWlanSignalLevelHandler::BssidChanged( TWlanBssid& aNewBSSID )
    {
    RUBY_DEBUG_BLOCK( "CVccWlanSignalLevelHandler::BssidChanged" );
    RUBY_DEBUG1( " -aNewBSSID = %S", &aNewBSSID );

    iStrength = KStrengthMin;
#ifndef __WINS__
    if( iWlanMgmt )
        {
        iWlanMgmt->GetConnectionSignalQuality( iStrength );
        }
#endif

    StrengthChanged();
    }

// ---------------------------------------------------------------------------
//  Handles lost of one or more networks
// ---------------------------------------------------------------------------
//
void CVccWlanSignalLevelHandler::OldNetworksLost()
    {
    RUBY_DEBUG_BLOCK( "CVccWlanSignalLevelHandler::OldNetworksLost" );

    iStrength = KStrengthMin;
#ifndef __WINS__
    if( iWlanMgmt )
        {
        iWlanMgmt->GetConnectionSignalQuality( iStrength );
        }
#endif
    StrengthChanged();
    
    TVccHoStatus hoStatus( EVccHoStateUnknown );
    iVccPsp.GetCurrentHoStatus( hoStatus );
    
    RUBY_DEBUG1("Current HoStatus; %d", hoStatus);
    
    if( hoStatus == EVccCsToPsHoStarted || hoStatus == EVccCsToPsHoInprogress 
                                         || hoStatus == EVccHoUnavailable )
        {
        iVccPsp.NotifySubscriberL( EVccCsToPsHoFailure, ECCPErrorNetworkOutOfOrder );
        }
    }

// ---------------------------------------------------------------------------
// Notify observer that the signal level has been changed.
// ---------------------------------------------------------------------------
//
void CVccWlanSignalLevelHandler::NotifyChanges(
    TInt32 aSignalStrength,
    MVccSignalLevelObserver::TSignalStrengthClass aClass )
    {
    RUBY_DEBUG_BLOCK( "CVccWlanSignalLevelHandler::NotifyChanges" );
    RUBY_DEBUG1( " -New strength = -%d dBm", aSignalStrength );

    iObserver.WlanSignalChanged( aSignalStrength, aClass );
    }
