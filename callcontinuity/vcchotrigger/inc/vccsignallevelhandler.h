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
* Description:   Base class for the signal level handling
*
*/



#ifndef VCCSIGNALLEVELHANDLER_H
#define VCCSIGNALLEVELHANDLER_H

#include <e32base.h>

#include "vccsignallevelobserver.h"
#include "vccunittesting.h"
#include "vccsignallevelparams.h"

/**
 *  Base class for signal level handling.
 *  Other classes are derived from this class.
 *
 *  @code
 *
 *  class CVccXXXSignalLevelHandler : public CVccSignalLevelHandler,
 *                                    public MXXXOtherClass
 *
 *  @endcode
 *
 *  @lib vcchotrigger.dll
 *  @since S60 v3.2
 */
class CVccSignalLevelHandler : public CTimer
    {

protected:

    /** Level of the signal */
    enum TStrengthState
        {
        EStrengthUnknown,
        EStrengthLow,
        EStrengthHigh
        };

    /** Current operation mode */
    enum TOperation
        {
        EOperationNone,
        EOperationGet,
        EOperationWait,
        EOperationComplete
        };

public:

    /**
    * Destructor.
    */
    virtual ~CVccSignalLevelHandler();

    /**
     * Start signal level observing
     *
     * @since S60 3.2
     */
    void StartL();

    /**
     * Stop signal level observing
     *
     * @since S60 3.2
     */
    void Stop();

    /**
     * Set the notification parameters
     *
     * @since S60 3.2
     * @param aParams Notification parameters
     */
    virtual void SetParams( const TSignalLevelParams& aParams );

protected:

    /**
     * C++ constructor
     *
     * @since S60 3.2
     * @param aObserver Observer to be notified when signal level changes
     * @param aParams Parameters at which level notifications should be given
     * as well the timeout how long the signal must at the specified level or
     * above before notification is given.
     */
    CVccSignalLevelHandler(
            MVccSignalLevelObserver& aObserver,
            const TSignalLevelParams& aParams );

    /**
     * Get the current Received Signal Strength Indicator (RSSI)
     *
     * @since S60 3.2
     */
    virtual void GetStrength() = 0;

    /**
     * Activate the notification service
     *
     * @since S60 3.2
     */
    virtual void EnableNotificationsL() = 0;

    /**
     * Cancel the notification service
     *
     * @since S60 3.2
     */
    virtual void DisableNotifications() = 0;

    /**
     * Cancel outstanding signal strentgh request.
     *
     * @since S60 3.2
     */
    virtual void CancelGetStrength() = 0;

    /**
     * Notifies the observer about signal level changes.
     *
     * @since S60 3.2
     * @param aRss RSS level in absolute dBm values
     * @param aRssClass specifies the current class of the received signal
     */
    virtual void NotifyChanges( TInt32 aSignalStrength,
         MVccSignalLevelObserver::TSignalStrengthClass aClass ) = 0;

    /**
     * Handles singal strength changes which are notified by the signal level observer
     *
     * @since S60 3.2
     */
    void StrengthChanged();

// from base class CTimer

    /**
     * @see CTimer::ConstructL()
     */
    void ConstructL();

    /**
     * From CTimer::DoCancel()
     */
    void DoCancel();


private:

// from base class CActive

    /**
     * From CActive::RunL()
     */
    void RunL();


protected: // data

    /**
     * Observer
     * Not own
     */
    MVccSignalLevelObserver& iObserver;

    /**
     * Parameters defining timers and signal levels
     */
    TSignalLevelParams iParams;

    /**
     * Current signal level in dBm
     */
    TInt32 iStrength;

    /**
     * Flag to determine if the observing/notifications are on or off
     */
    TBool iNotificationsOn;

    /* Current operation
    */
   TOperation iOperation;

     /* Current signal state
     */
    TStrengthState iState;
 

    VCC_UNITTEST( UT_CVccWlanSignalLevelHandler )
    VCC_UNITTEST( UT_CVccGsmSignalLevelHandler )
    };

#endif // VCCSIGNALLEVELHANDLER_H
