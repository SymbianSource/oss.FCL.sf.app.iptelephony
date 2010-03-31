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

const TInt KWlanPollIntervalLowSignal= 1000000;
const TInt KWlanPollIntervalHighSignal= 5000000;

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
    iManualHoDone = EFalse;
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


// ---------------------------------------------------------------------------
// Handles getting the signal strength and notifying the observer about
// strength changes.
// ---------------------------------------------------------------------------

void CVccWlanSignalLevelHandler::RunL()
    {
    RUBY_DEBUG_BLOCK( "CVccWlanSignalLevelHandler::RunL" );

    // Zero (0) is not acceptable.
       if ( !iStrength )
           {
           RUBY_DEBUG0( " -0 strength not acceptable, setting to KStrengthMin");
           iStrength = KStrengthMin;
           }

       RUBY_DEBUG3( " -iStrength = %d iState = %d iOp = %d", iStrength, iState, iOperation );

       switch ( iOperation )
           {
           case EOperationGet:
               {

               // We are in the Get-mode to get the signal strength.
               // If the strength is < than the high level (== the strength
               // is good), start timer to check if we are still in good level
               // after the timer completes.
               // The same is done if we have a low level (== bad).

               RUBY_DEBUG0( " -EOperationGet");

               if ( iStrength <= iParams.iHighLevel && iStrength > 0 )
                   {
                   RUBY_DEBUG0( "  set state = EStrengthHigh, op = EOperationWait" );
                   
                   After( iParams.iHighTimeout );
                   iState = EStrengthHigh;
                   iOperation = EOperationWait;
                   }
               else if ( iStrength >= iParams.iLowLevel )
                   {
                   RUBY_DEBUG0( "  set state = EStrengtLow, op = EOperationWait" );
                   After( iParams.iLowTimeout );
                   iState = EStrengthLow;
                   iOperation = EOperationWait;
                   }
               else
                   {
                   RUBY_DEBUG0( "  strength between low and high, set op = EOperationNone" );
                  
                   //WLAN signal is almost weak, check again with low interval
                   After( KWlanPollIntervalLowSignal );
                   iOperation = EOperationNone;
                   iState = EStrengthLow;
                   // PCLint
                   }
               break;
               }

           case EOperationWait:
               {

               // Timer has completed. Check the signal level again.

               RUBY_DEBUG0( " -EOperationWait" );
               RUBY_DEBUG0( "  set op = EOperationComplete" );
               GetStrength();

               SetActive();

               iOperation = EOperationComplete;

               break;
               }

           case EOperationComplete:
               {
               // Checking signal strength is now done.
               // Notify our observer (if needed).

               RUBY_DEBUG1( " -EOperationComplete, iStrength = %d", iStrength );

               // Do we have a good signal level?
               if ( iStrength <= iParams.iHighLevel && iStrength > 0 && iState == EStrengthHigh )
                   {
                   RUBY_DEBUG0( " -if ( iStrength <= iParams.iHighLevel" );
                   NotifyChanges( iStrength, MVccSignalLevelObserver::ESignalClassNormal );
                   }
               // Or do we have a bad signal level?
               else if ( iStrength >= iParams.iHighLevel && iState == EStrengthLow )
                   {
                   RUBY_DEBUG0( " -else if ( iStrength >= iParams.iHighLevel" );
                   NotifyChanges( iStrength, MVccSignalLevelObserver::ESignalClassWeak );
                   }
               else
                   {
                   // PCLint
                   }
               
               TTimeIntervalMicroSeconds32 interval;
                                              
                    if( iState == EStrengthHigh )
                        {
                         RUBY_DEBUG0( "  high interval" );
                        interval = KWlanPollIntervalHighSignal;
                         }
                    else
                          {
                          RUBY_DEBUG0( "  low interval" )
                           interval = KWlanPollIntervalLowSignal;
                          }
                               
                    After( interval );
       
               iState = EStrengthUnknown;
               iOperation = EOperationNone;
             
               break;
               }

           case EOperationNone:
               {
               RUBY_DEBUG0( " -EOperationNone" );
               
               //if manual HO is done then there is no need for checking signal strength so much,
               //so stopping the loop
               if( !iManualHoDone )
                   {
                   RUBY_DEBUG0( " -call GetStrength()" );
                   GetStrength();
                   SetActive();
                   RUBY_DEBUG1( " -iStrength = %d, set op = EOperationGet \
                                        state = EStrengthUnknown", iStrength );
                   iOperation = EOperationGet;
                   }
                break;
         
               }
               
                
           default:
               {
               break;
               }
           }
       
    }

// ---------------------------------------------------------------------------
// Lets Wlan Signal Level Handler know is manual handover done or not done
// 
// ---------------------------------------------------------------------------


void CVccWlanSignalLevelHandler::SetManualHoDone( TBool aValue )
    {
    RUBY_DEBUG_BLOCK( "CVccWlanSignalLevelHandler::SetManualHoDone" );
    iManualHoDone = aValue;
    }


// ---------------------------------------------------------------------------
// Cancel outstanding requests.
// ---------------------------------------------------------------------------
//
void CVccWlanSignalLevelHandler::DoCancel()
    {
    RUBY_DEBUG_BLOCK( "CVccSignalLevelHandler::DoCancel" );
    switch ( iOperation )
        {
        case EOperationWait:
        case EOperationNone:
            {
            RUBY_DEBUG0( "EOperationWait / EOperationNone" );
            CTimer::DoCancel();

            break;
            }


        default:
            {
            break;
            }
        }
    }
