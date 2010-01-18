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
* Description:   CCH monitor class definition
*
*/


#ifndef VCCCCHMONITOR_H
#define VCCCCHMONITOR_H

#include <e32base.h>
#include <cch.h>

#include "vcccchobserver.h"
#include "vccunittesting.h"

/**
 *  CCH monitor. Uses CCH to enable/disable CCH services.
 *
 *  @code
 *   MyClass::ConstructL()
 *       {
 *       iMyCch = Cch::NewL();
 *
 *       iMyService = iMyCch->GetService( iMyServiceId );
 *
 *       User::LeaveIfNull( iMyService );
 *
 *       iMyService->SetObserver( *this );
 *
 *       TInt error = iMyService->Enable( iMyServiceType );
 *
 *       if ( error != KErrNone )
 *           {
 *           User::Leave( error );
 *           }
 *       }
 *
 *   MyClass::ServiceStatusChanged(
 *       TInt aServiceId,
 *       CCHSubserviceType aType,
 *       const TCchServiceStatus& aServiceStatus )
 *       {
 *       if ( aServiceStatus.Error() != KErrNone )
 *           {
 *           HandleError();
 *           }
 *       else
 *           {
 *           HandleStateChange();
 *           }
 *       }
 *
 *   MyClass::~MyClass()
 *       {
 *       iMyService->Disable( iMyServiceType );
 *       delete iCch;
 *       }
 *
 *  @endcode
 *
 *  @lib vcchotrigger.dll
 *  @since S60 v3.2
 */
class CVccCchMonitor : public CBase,
                       public MCchServiceStatusObserver
    {

public:

    /**
     * Two-phased constructor.
     * @param aObserver Observer to be notified when status changes
     */
    static CVccCchMonitor* NewL( MVccCchObserver& aObserver );

    /**
     * Destructor.
     */
    virtual ~CVccCchMonitor();

    /**
     * Enable the service and start monitoring
     * the service state.
     *
     * @since S60 v3.2
     */
    void EnableServiceL();

    /**
     * Disable the service and stop monitoring the
     * service state
     *
     * @since S60 v3.2
     */
    void DisableService();

    /**
     * Get the last state of the CCH service.
     *
     * @since S60 v3.2
     * @return The status of the service: available or not
     */
    MVccCchObserver::TServiceStatus ServiceStatus() const;


// from base class MCchServiceStatusObserver

    /**
     * From MCchServiceStatusObserver
     * Signaled when service status or error changes
     *
     * @param aServiceId Id of the service
     * @param aType Service type
     * @param aServiceStatus Service status
     */
    void ServiceStatusChanged(
            TInt aServiceId,
            TCCHSubserviceType aType,
            const TCchServiceStatus& aServiceStatus );

private:

    /**
     * C++ constructor
     *
     * @since S60 v3.2
     * @param aObserver Observer to be notified when status changes.
     */
    CVccCchMonitor( MVccCchObserver& aObserver );

    /**
     * Symbian constructor
     *
     * @since S60 v3.2
     */
    void ConstructL();

    /**
     * Notify observer about CCH service status has changed.
     *
     * @since S60 v3.2
     */
    void NotifyObserver();


    /**
     * Enable the service (assioated to the iCchService)
     *
     * @since S60 v3.2
     */
    void DoEnableServiceL();


private: // data

    /**
     * Our observer.
     * Not own
     */
    MVccCchObserver& iObserver;

    /**
     * Should our obsever to be notified
     * ETrue: yes, EFalse: no.
     */
    TBool iNotifyObserver;

    /*
     * CCH
     * Own.
     */
    CCch* iCch;

    /*
     * CCH service
     * Not own.
     */
    CCchService* iCchService;

    /*
     * Service is reserved for us.
     */
    TBool iServiceReserved;

    /*
     * Must we disable the service upon exit.
     */
    TBool iDisableService;

    /**
     * The last state of the CCH service.
     */
    TCCHSubserviceState iCurrentCchState;

    /**
     * The last error received from the CCH service
     */
    TInt iLastCchError;

    /**
     * Service id of the VoIP we are using
     */
    TInt iServiceId;

    /*
     * Current status (own status reported to observer).
     */
    MVccCchObserver::TServiceStatus iCurrentStatus;


    VCC_UNITTEST( UT_CVccCchMonitor )
    };


#endif // VCCCCHMONITOR_H
