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
* Description:   Definition of the WLAN signal level handler
*
*/



#ifndef VCCWLANSIGNALLEVELHANDLER_H
#define VCCWLANSIGNALLEVELHANDLER_H

#include <e32base.h>
#include <wlanmgmtcommon.h>

#include "vccsignallevelhandler.h"
#include "vccsignallevelobserver.h"

class CWlanMgmtClient;
class CVccEngPsProperty;

/**
 *  WLAN signal level observer
 *
 *
 *  @code
 *
 *  TSignalLevelParams params = { 8e6, 8e6, 80, 40 };
 *
 *  CMyClass::ConstructL()
 *      {
 *      iSignalLevelHandler = CVccWlanSignalLevelHandler::NewL( *this, params );
 *      iSignalLevelHandler->StartL();
 *      }
 *
 *  CMyClass::StopMonitoring()
 *      {
 *      iSignalLevelHandler->Stop();
 *      }
 *
 *  CMyClass::SetNewParametersAndStartL()
 *      {
 *      TSignalLevelParams params = { 8e6, 10e6, 80, 20 };
 *
 *      iSignalLevelHandler->SetParams( params );
 *
 *      // Start using new parameters
 *      iSignalLevelHandler->StartL();
 *      }
 *
 *  CMyClass::~CMyClass()
 *      {
 *      delete iSignalLevelHandler;
 *      }
 *
 *
 *  CMyClass::WlanSignalChanged(
 *      TInt32 aRss,
 *      TSignalRssClass aRssClass )
 *      {
 *      // Signal level changed. Must do something
 *      if ( aRssClass == ESignalClassWeak )
 *          {
 *          // Do handover if the signal is very weak
 *          if ( aRss > EReallyBadSignalLevel )
 *              {
 *              }
 *          }
 *      }
 *
 *
 *
 *  @endcode
 *
 *  @lib vcchotrigger.dll
 *  @since S60 v3.2
 */
class CVccWlanSignalLevelHandler : public CVccSignalLevelHandler,
                                   public MWlanMgmtNotifications
    {
public:


    /**
     * Two-phased constructor.
     * @param aObserver Observer which will be notified upon signal changes.
     * @param aParams Parameters for the timer and signal levels.
     */
    static CVccWlanSignalLevelHandler * NewL(
            MVccSignalLevelObserver& aObserver,
            const TSignalLevelParams& aParams, CVccEngPsProperty& aPsProperty );
    /**
     * Two-phased constructor.
     * @param aObserver Observer which will be notified upon signal changes.
     * @param aParams Parameters for the timer and signal levels.
     */
    static CVccWlanSignalLevelHandler* NewLC(
            MVccSignalLevelObserver& aObserver,
            const TSignalLevelParams& aParams, CVccEngPsProperty& aPsProperty );

    /**
    * Destructor.
    */
    virtual ~CVccWlanSignalLevelHandler();
    
    /**
    *  for telling WLAN Signal Level Handler that manual HO is done or not done
    */
    void SetManualHoDone( TBool aValue );

private:

    /**
     * Symbian second-phase constructor
     *
     * @since S60 3.2
     */
    void ConstructL();

    /**
     * C++ constructor
     *
     * @since S60 3.2
     * @param aObserver Observer which will be notified upon signal changes.
     * @param aParams Parameters for the timer and signal levels.
     */
    CVccWlanSignalLevelHandler( MVccSignalLevelObserver& aObserver,
                               const TSignalLevelParams& aParams, CVccEngPsProperty& aPsProperty);

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


// from base class MWlanMgmtNotifications

    /**
     * @see MWlanMgmtNotifications::RssChanged()
     */
    void RssChanged( TWlanRssClass aRssClass, TUint aRss );

    /**
     * @see MWlanMgmtNotifications::BssidChanged()
     */
    void BssidChanged( TWlanBssid& aNewBSSID );

    /**
     * @see MWlanMgmtNotifications::OldNetworksLost()
     */
    void OldNetworksLost();
    
    /**
     * Overriding this because WLAN signal level handler needs to observe the signal level
     * more frequently than GSM Signal Level Handler
     */
    void RunL();
    
    //overriding this because specific actions needed for cancellation for WLAN
    void DoCancel(); 
    
private: // data

    /**
     * WLAN management client.
     * Own.
     */
    CWlanMgmtClient* iWlanMgmt;
    
    /**
     * for informing the states
     */
    CVccEngPsProperty& iVccPsp;
    
    /**
     * indicates is manual HO done or not
     */
    TBool iManualHoDone;
    };

#endif // VCCWLANSIGNALLEVELHANDLER_H

