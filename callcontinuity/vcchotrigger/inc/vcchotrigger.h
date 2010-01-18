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
* Description:   Definition of the class which handles handover triggering.
*
*/



#ifndef C_VCCHOTRIGGER_H
#define C_VCCHOTRIGGER_H


#include <e32base.h>
#include <e32std.h>
#include <wlanmgmtcommon.h>

#include "vccsignallevelobserver.h"
#include "vcccchobserver.h"
#include "vcchopolicy.h"
#include "vccunittesting.h"

class CVccUiPsProperty;
class CVccWlanSignalLevelHandler;
class CVccGsmSignalLevelHandler;
class CVccHoPolicyReader;
class CVccCchMonitor;
class CVccEngPsProperty;
class CVccPerformer;

/**
 *  This class handles the triggering of the handover from
 *  one network to another.
 *
 *
 *  @code
 *   CMyClass::ConstructL()
 *       {
 *       iHoTrigger = CVccHoTrigger::NewL();
 *       }
 *
 *   CMyClass::FunctionL()
 *       {
 *       iHoTrigger->Start();
 *       }
 *
 *   CMyClass::DomainChanged()
 *       {
 *       // Use swapped the domain of an active call
 *       iHoTrigger->SetCurrentDomain( iActiveCallDomain );
 *       }
 *
 *   CMyClass::CallPutToHold()
 *       {
 *       // Stop all network monitoring.
 *       iHoTrigger->Stop();
 *       }
 *
 *   CMyClass::CallActivated()
 *       {
 *       // Start all network monitors
 *       iHoTrigger->Start();
 *       }
 *
 *   CMyClass::~CMyClass()
 *       {
 *       delete iHoTrigger;
 *       }
 *
 *  @endcode
 *
 *  @lib vcchotrigger.lib
 *  @since S60 v3.2
 */
class CVccHoTrigger : public CBase,
                      public MVccSignalLevelObserver,
                      public MVccCchObserver
    {

public:


    /** Domain types */
    enum TCallDomainType
        {
        /** PS domain */
        ECallDomainTypePS,
        /** CS domain */
        ECallDomainTypeCS
        };


    /**
     * Two-phased constructor.
     */
    IMPORT_C static CVccHoTrigger* NewL();

    /**
     * Destructor.
     */
    virtual ~CVccHoTrigger();


    /**
     * Stop all network monitors
     *
     * @since S60 v3.2
     */
    IMPORT_C void Stop();
    
    /**
     * Stop all network monitors
     *
     * @since S60 v3.2
     */
    IMPORT_C void Stop( CVccPerformer& aStopper );

    /**
     * Start all network monitors
     *
     * @since S60 v3.2
     */
    IMPORT_C void Start( CVccPerformer& aWhoStartedMe, TBool aCsOriginated );

    /**
     * Set the current domain of the active call
     *
     * @since S60 v3.2
     * @param aDomainType is the domain of the current (active) call.
     */
    IMPORT_C void SetCurrentDomainType( TCallDomainType aDomainType );

    /**
     * Set the preferred domain of the active call
     * 
     * @since S60 v3.2
     * @param aDomainType is the call´s preferred domain type.
     */
    IMPORT_C void SetPreferredDomainType( TVccHoPolicyPreferredDomain aDomainType );

    
    /**
     * Set immediate domain transfer setting
     * 
     * @since S60 v3.2
     * @param aDomainType is the call´s preferred domain type.
     */
    IMPORT_C void SetImmediateDomainTransfer( TBool aImmediateDT );
        
// from base class MVccSignalLevelObserver

    /**
     * @see MVccSignalLevelObserver::WlanSignalChanged()
     */
    void WlanSignalChanged( TInt32 aSignalStrength,
            TSignalStrengthClass aClass );

    /**
     * @see MVccSignalLevelObserver::GsmSignalChanged()
     */
    void GsmSignalChanged( TInt32 aSignalStrength,
            TSignalStrengthClass aClass );

// from base class MVccCchMonitorObserver

    /**
     * @see MVccCchMonitorObserver::CchServiceStatusChanged
     */
    void CchServiceStatusChanged( TServiceStatus aStatus );
    
public:
    /**
     * When manual handover is started no automatic handovers should
     * be made.
     *
     * @since S60 3.2
     */
    IMPORT_C void ManualHoCallStarted();

    /**
     * After manual handover call has been released, automatic
     * handovers can be made again.
     *
     * @since S60 3.2
     */
    IMPORT_C void ManualHoCallReleased();

    /**
     * During conference call HO is not allowed
     *
     * @since S60 3.2
     */
    IMPORT_C void HoNotAllowedL();
    
    /**
     * After conference call HO is allowed again
     *
     * @since S60 3.2
     */
    IMPORT_C void HoAllowed();
    
    /**
     * Reads the settings to get the info if ho is allowed when cs originated call
     * @since s60 3.2
     */
    IMPORT_C void ReadHoAllowedWhenCsOriginatedSettingL();
    
private:

    /**
     * C++ constructor
     *
     * @since S60 v3.2
     */
    CVccHoTrigger();

    /**
     * Symbian second-phase constructor
     *
     * @since S60 v3.2
     */
    void ConstructL();
    
    /**
     * Check if PS or CS are available (to do handover).
     * 
     * @since S60 v3.2
     * @return ETrue if service is available, else return EFalse
     */
    TBool ServicesAvailable();
    
    /**
     * Update (write) service status state to P&S.
     * 
     * @since S60 v3.2
     */
    void UpdatePsKeysL();

    /**
     * Initiates the actual handover if its ok to start ho.
     * 
     * @since S60 v3.2
     */
    void TriggerHo();
    
    /**
     * Initiate immediate ho.
     * 
     * @since S60 v3.2
     * @return ETrue if immediate HO was initiated, EFalse otherwise
     */
    TBool DoImmediateHo();

private: // data


    /**
     * VCC P&S key manager for UI side
     * Own.
     */
    CVccUiPsProperty* iProperty;
    
    /**
     * PS property writer to write TVccHoStatus values
     * Own.
     */
    CVccEngPsProperty* iEngPsProperty;

    /**
     * VCC Wlan signal level observer
     * Own.
     */
    CVccWlanSignalLevelHandler* iWlanSignalLevelHandler;

    /**
     * VCC GSM signal level observer
     * Own.
     */
    CVccGsmSignalLevelHandler* iGsmSignalLevelHandler;

    /**
     * Call domain type
     */
    TCallDomainType iDomainType;


    /**
     * GSM signal class
     */
    TSignalStrengthClass iGsmClass;

    /**
     * Previous GSM signal class
     */
    TSignalStrengthClass iPreviousGsmClass;
    
    /**
     * WLAN signal class.
     */
    TSignalStrengthClass iWlanClass;
    
    /**
     * Previous WLAN signal class.
     */
    TSignalStrengthClass iPreviousWlanClass;
    
    /**
     * CenRep reader for VCC HO policy
     * Own.
     */
    CVccHoPolicyReader* iHoPolicyReader;
    
    /** VCC HO policy */
    TVccHoPolicy iPolicy;
    
    /**
     * CCH monitor.
     * Own
     */
    CVccCchMonitor* iCchMonitor;
    
    /**
     * CCH status (PS service availability)
     */
    MVccCchObserver::TServiceStatus iCchServiceStatus;
    
    /**
     * CS status (i.e. signal level)
     */
    TInt iCsSignalLevel;
    
    /**
     * Are we stopped or active (i.e. monitoring)
     */
    TBool iStarted;
    
    /**
     * Has manual ho been made
     */
    TBool iManualHoDone;
    
    /**
     * Ho allowed or not (conference, video, data, multimedia sharing)
     */
    TBool iHoNotAllowed;
    
    /**
     * Performer that started trigger
     * Not own.
     */
    CVccPerformer* iWhoStartedMe;
    
    /*
     * Is ho allowed if the originating domain of the 
     * original call was CS
     */
    TBool iHoAllowedIfCsOriginated;
    
    friend class T_CVccPerformer;
    
    VCC_UNITTEST( UT_CVccHoTrigger )
    };

#endif //  C_VCCHOTRIGGER_H
