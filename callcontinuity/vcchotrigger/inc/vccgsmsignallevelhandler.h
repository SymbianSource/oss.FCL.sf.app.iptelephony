/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   ?Description
*
*/



#ifndef C_VCCGSMSIGNALLEVELHANDLER_H
#define C_VCCGSMSIGNALLEVELHANDLER_H

#include <e32base.h>
#include <etel.h>

#include "vccsignallevelhandler.h"
#include "vccsignallevelobserver.h"
#include "vccgsmnotifications.h"
#include "vccunittesting.h"

/**
 *  VCC GSM signal level handler.
 *
 *  @code
 *  void CExampleClass::ConstructL()
 *      {
 *      //Scan time for low and high 5 sec, low strength -85, high -70 dBm
 *      TSignalLevelParams params = { 5e6, 5e6, 85, 70 };
 *
 *      //
 *      iGsmHandler = CVccGsmSignalLevelHandler::NewL( *this, params );
 *      }
 *
 *  void CExampleClass::StartMonitoringL()
 *      {
 *      // Start monitoring gsm signal levels.
 *      iGsmHandler->StartL();
 *      }
 *
 *  void CExampleClass::StopMonitoringL()
 *      {
 *      // Stop monitoring.
 *      iGsmHandler->StopL();
 *      }
 *
 *  // Called when signal level changes.
 *  void CExampleClass::SignalLevelL(
 *      const CVccSignalLevelHandler* aHandlerInstance,
 *      TInt32 aSignalStrength,
 *      TSignalStrengthClass aClass )
 *      {
 *      if ( iGsmHandler == aHandlerInstance )
 *          {
 *          // GSM signal changed, do whatever is needed
 *          MakeHandoverDecision( aSignalStrength, aClass );
 *          }
 *      }
 *
 *
 *  @endcode
 *
 *  @lib vcchotrigger.dll
 *  @since S60 v3.2
 */
class CVccGsmSignalLevelHandler : public CVccSignalLevelHandler,
                                  public MVccGsmNotifications
    {
public:

    /**
     * Two-phased constructor.
     * @param aObserver Observer of this signal handler
     * @param aParams Parameters for signal monitoring
     */
    static CVccGsmSignalLevelHandler * NewL( MVccSignalLevelObserver & aObserver,
    const TSignalLevelParams& aParams );

    /**
     * Two-phased constructor.
     * @param aObserver Observer of this signal handler
     * @param aParams Parameters for signal monitoring
     */
    static CVccGsmSignalLevelHandler * NewLC(
        MVccSignalLevelObserver & aObserver,
        const TSignalLevelParams& aParams );

    /**
    * Destructor.
    */
    virtual ~CVccGsmSignalLevelHandler();

private:

    /**
     * C++ constructor
     * @param aObserver Reference to signal level observer
     * @param aParams Reference to signal level parameters
     */
    CVccGsmSignalLevelHandler( MVccSignalLevelObserver & aObserver,
                               const TSignalLevelParams& aParams );

    /**
     * Symbian second-phase constructor
     */
    void ConstructL();

private:

// from base class CVccSignalLevelHandler

    /**
    * @see CVccSignalLevelHandler::GetStrength()
    */
    void GetStrength();

    /**
    * @see CVccSignalLevelHandler::EnableNotificationsL()
    */
    void EnableNotificationsL();

    /**
    * @see CVccSignalLevelHandler::DisableNotifications()
    */
    void DisableNotifications();

    /**
    * @see CVccSignalLevelHandler::CancelGetStrength()
    */
    void CancelGetStrength();

    /**
    * @see CVccSignalLevelHandler::NotifyChanges()
    */
    void NotifyChanges( TInt32 aSignalStrength,
                        MVccSignalLevelObserver::TSignalStrengthClass aClass );
    
// from base class MVccGsmNotifications

    /**
    * @see MVccGsmNotifications::GsmStrengthChanged()
    */
    void GsmStrengthChanged( TInt32 aStrength );


private: // data
     
    /**
     * Etel server
     * Own.
     */
    RTelServer iEtelServer;
    
    /**
     * Phone based functionality
     * Own.
     */
    RMobilePhone iPhone;
    
    /**
     * Gsm signal level notifier 
     * Own.
     */
    CVccGsmNotifications* iGsmNotifications;
    
    /** Value of the GSM indication bar */
    TInt8 iBar;
    
    VCC_UNITTEST( UT_CVccGsmSignalLevelHandler )
    };

#endif  // C_VCCGSMSIGNALLEVELHANDLER_H
