/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of the CCH monitor.
*
*/



#include <cch.h>
#include "vcccchmonitor.h"
#include "vccsettingsreader.h"
#include "rubydebug.h"

// ---------------------------------------------------------------------------
// Symbian OS static constructor
// ---------------------------------------------------------------------------
//
CVccCchMonitor* CVccCchMonitor::NewL( MVccCchObserver& aObserver )
    {
    RUBY_DEBUG_BLOCKL( "CVccCchMonitor::NewL" );

    CVccCchMonitor* self = new ( ELeave ) CVccCchMonitor( aObserver );

    CleanupStack::PushL( self );

    self->ConstructL();

    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------------------------
// C++ destructor
// ---------------------------------------------------------------------------
//
CVccCchMonitor::~CVccCchMonitor()
    {
    RUBY_DEBUG0( "CVccCchMonitor::~CVccCchMonitor START" );

    // If we have reserved the used service, free it before disabling.
    // Do not mind the return values, just keep going.

    if ( iCchService )
        {
        if ( iServiceReserved )
            {
            RUBY_DEBUG0( " -Free and disable service" );

            (void) iCchService->Free( ECCHVoIPSub );
            (void) iCchService->Disable( ECCHVoIPSub );

            // We did disabled here.
            iDisableService = EFalse;
            }

        // If we enabled the service, we must disable it also
        // ( even if it was not reserved for us).
        // Do not mind the return value, just keep going.

        if ( iDisableService )
            {
            RUBY_DEBUG0( " -Disable service" );
            (void) iCchService->Disable( ECCHVoIPSub );
            }

        // Remove observer
        iCchService->RemoveObserver();

        // We DO NOT OWN the service, DO NOT DELETE IT
        iCchService = NULL;
        }

    // Free the CCH itself
    delete iCch;
    RUBY_DEBUG0( "CVccCchMonitor::~CVccCchMonitor STOP" );
    }

// ---------------------------------------------------------------------------
// Symbian OS 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CVccCchMonitor::ConstructL()
    {
    RUBY_DEBUG_BLOCKL( "CVccCchMonitor::ConstructL" );

    // First we create the CCH object and get the VoIP id
    // which is VCC enabled.
    //
    // If we don't get the VoIP id we leave since
    // without the VoIP id we cannot get the right VoIP service.
    //
    // If everything goes well, we try to enable the service.
    // All errors etc. are received thru the CCH observer interface.
    //

    // Initialize some own flags.

    // Do not notify our observer (yet).
    iNotifyObserver = EFalse;

    // Do not disable service when we exit.
    iDisableService = EFalse;

    // Intitial CCH state.
    iCurrentCchState = ECCHUninitialized;

    // No errors so far.
    iLastCchError = KErrNone;

    // And service is not available (yet).
    iCurrentStatus = MVccCchObserver::EServiceUnavailable;


    // Create CCH
    iCch = CCch::NewL();

    // We do not have the service yet.
    iCchService = NULL;
    }

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CVccCchMonitor::CVccCchMonitor( MVccCchObserver& aObserver )
    : iObserver( aObserver )
    {
    RUBY_DEBUG_BLOCK( "CVccCchMonitor::CVccCchMonitor" );
    }

// ---------------------------------------------------------------------------
// Enable the VoIP service and start monitoring the state of the CCH.
// ---------------------------------------------------------------------------
//
void CVccCchMonitor::EnableServiceL()
    {
    RUBY_DEBUG_BLOCK( "CVccCchMonitor::EnableServiceL" );

    // CCH must be created.
    __ASSERT_DEBUG( iCch, User::Leave( KErrArgument ) );

    // Get the Voip service id. Get it always to if the SP
    // settings are chagned, we get updated values.

    iServiceId = VccSettingsReader::VoIPServiceIdL();

    if ( iServiceId == KErrNotFound )
        {
        RUBY_DEBUG1( " -VoIP service id (%d) not found - LEAVE", iServiceId );
        User::Leave( KErrNotFound );
        }

    // Get the service if not already available

    if ( !iCchService )
        {

        // Get the service. The ownership is not transfered
        iCchService = iCch->GetService( iServiceId );

        RUBY_DEBUG1( " -CCH service fetched, addr = %x (LEAVE if NULL)",
                iCchService );

        // CCH cannot find the service if it returns NULL
        if ( !iCchService )
            {
            User::Leave( KErrNotFound );
            }

        // We got the service.
        // Set us as observer to get notifications.

        iCchService->SetObserver( *this );
        }


    // Start sending notifications to our observer.
    iNotifyObserver = ETrue;

    // And then enable the VoIP service.
    DoEnableServiceL();
    }

// ---------------------------------------------------------------------------
// Disable the service, this means that
// we stop sending notifications to our observer.
// The service is not disabled, we just do not notify our obsserver.
// The service is disabled when we exit.
// ---------------------------------------------------------------------------
//
void CVccCchMonitor::DisableService()
    {
    RUBY_DEBUG_BLOCK( "CVccCchMonitor::DisableService" );

    // We do not disable the service.
    // It can be done here but if we start and stop it
    // many times within a short period of time,
    // it does not make sense.
    //
    // Service is released (and disabled if needed)
    // when we exit (see destructor).
    // So here just flag to stop sending notifications to
    // our observer.
    iCchService = NULL;
    iNotifyObserver = EFalse;
    }

// ---------------------------------------------------------------------------
// From MCchServiceStatusObserver
// Handles CCH service or error status changes.
// ---------------------------------------------------------------------------
//
void CVccCchMonitor::ServiceStatusChanged(
        TInt aServiceId,
        TCCHSubserviceType aType,
        const TCchServiceStatus& aServiceStatus )
    {
    RUBY_DEBUG_BLOCK( "CVccCchMonitor::ServiceStatusChanged" );

    // The CCH sends notifications to all
    // so service id and the type must be checked.

    if ( !( aServiceId == iServiceId && aType == ECCHVoIPSub ) )
        {
        return;
        }

    // Get the state and the error.
    iCurrentCchState = aServiceStatus.State();
    iLastCchError = aServiceStatus.Error();

    RUBY_DEBUG2( " -State = %d, error = %d", iCurrentCchState, iLastCchError );


    // If we are observing, notify our observer.
    if ( iNotifyObserver == EFalse )
        {
        return;
        }
    else
        {
        NotifyObserver();
        }
    }

// ---------------------------------------------------------------------------
// Send notifications to our observer.
// ---------------------------------------------------------------------------
//
void CVccCchMonitor::NotifyObserver()
    {
    RUBY_DEBUG_BLOCK( "CVccCchMonitor::NotifyObserver" );


    // This method should be called only when
    // the state of the cch service is updated
    // to iCurrentCchState member variable.

    // If CCH state != enabled or there is an error, service is not working
    //
    // Note that the service stays enabled until someone disables it.

    if ( ( iCurrentCchState != ECCHEnabled ) ||
        ( iLastCchError != KErrNone ) )
        {
        RUBY_DEBUG0( " -Service unavailable" );
        iCurrentStatus = MVccCchObserver::EServiceUnavailable;
        }
    else
        {
        RUBY_DEBUG0( " -Service available" );
        iCurrentStatus = MVccCchObserver::EServiceAvailable;
        }

    iObserver.CchServiceStatusChanged( iCurrentStatus );
    }

// ---------------------------------------------------------------------------
// Enable the CCH service we are using (ECCHVoIPSub).
// ---------------------------------------------------------------------------
//
void CVccCchMonitor::DoEnableServiceL()
    {
    RUBY_DEBUG_BLOCK( "CVccCchMonitor::DoEnableServiceL" );

    // Enable the service
    //
    // Depending of the current state of the CCH service:
    //
    // Do nothing if
    // - ECCHConnecting
    //
    //
    // Try to enable if
    // - ECCHDisconnecting
    // - ECCHDisabled
    // - ECCHUninitialized
    //
    // Notify our observer if
    // - ECCHEnabled


    // Check the state of the service.
    // The state is got from the status object of the service.


    TCchServiceStatus serviceStatus;
    TInt errorValue( KErrNone );

    // Get the status.
    errorValue = iCchService->GetStatus( ECCHVoIPSub, serviceStatus );

    RUBY_DEBUG2( " -GetStatus returned = %d, service status = %d",
            errorValue, serviceStatus.State() );


    // If we can't get the status, leave

    if ( errorValue != KErrNone )
        {
        RUBY_DEBUG0( " -GetStatus returned error, can't enable - LEAVE" );

        User::Leave( errorValue );
        }


    // Okay, get the state and error (from status).
    iCurrentCchState = serviceStatus.State();
    iLastCchError = serviceStatus.Error();

    RUBY_DEBUG1( " -Service state = %d", iCurrentCchState );


    switch( iCurrentCchState )
        {
        case ECCHUninitialized:
        // Fall-through intended here
        case ECCHDisabled:
        // Fall-through intended here
        case ECCHDisconnecting:
            {
            RUBY_DEBUG0( " -Try to enable service, LEAVE if error" );

            // Enable, leave if errors,
            User::LeaveIfError( iCchService->Enable( ECCHVoIPSub ) );

            // Service should be enabled in a while,
            // we must disable it upon exiting since it was not enabled
            // (we are one who enabled it so must disable it).

            iDisableService = ETrue;
            break;
            }

        case ECCHConnecting:
            {
            // Service is connecting (by someone else
            // or we have been called more than twice).
            // We should get the notification when it is enabled
            // and ready for the use.
            // Do nothing.
            break;
            }

        case ECCHEnabled:
            {
            RUBY_DEBUG0( " -Service is already enabled - do nothing" );

            // Enabled already. We can notify our observer
            // with the status of the service.

            NotifyObserver();
            break;
            }

        default:
            {
            break;
            }

        } // End of switch


    // So good so far...
    // Check if we can reserve (if not done already)
    // the service for us.
    // This prevents someone to disable it during a call

    if ( !iServiceReserved )
        {
        TBool isReserved( EFalse );

        iCchService->IsReserved( ECCHVoIPSub, isReserved );

        if ( !isReserved )
            {
            RUBY_DEBUG0( " -Try to reserve the service for us" );

            // It is free. Try to reserve it.

            if ( iCchService->Reserve( ECCHVoIPSub ) == KErrNone )
                {
                RUBY_DEBUG0( " -Service reserved for us" );
                iServiceReserved = ETrue;
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// Return the current status of the service availability
// ---------------------------------------------------------------------------
//
MVccCchObserver::TServiceStatus CVccCchMonitor::ServiceStatus() const
    {
    RUBY_DEBUG_BLOCK( "CVccCchMonitor::ServiceStatus" );

    RUBY_DEBUG1( " -Status = %d", iCurrentStatus );

    return iCurrentStatus;
    }
