/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Class to get the settings that define when ho should be 
*                allowed.
*
*/



#ifndef C_VCCHOPOLICYREADER_H
#define C_VCCHOPOLICYREADER_H

#include <e32base.h>

#include "vcchopolicy.h"
#include "vccsignallevelparams.h"
#include "vccunittesting.h"

class CVccSPSettings;

/**
 *  Reads settings that define when handover can be performed from cenreps.
 *
 *  @lib vcchotrigger.lib
 *  @since S60 v3.2
 */
class CVccHoPolicyReader: public CBase
    {
public:
    
    /**
     * Two-phased constructor.
     */
    static CVccHoPolicyReader* NewL();    
    
    /**
     * Two-phased constructor.
     */
    static CVccHoPolicyReader* NewLC();    
   
    /**
    * Destructor.
    */
    virtual ~CVccHoPolicyReader();
    
    /**
    * Handover policy
    * @since S60 3.2
    * @return VCC HO policy
    */
    TVccHoPolicy HoPolicy() const;
    
    /**
    * Read Ho policy settings from repository
    * @since S60 3.2
    */
    void ReadSettingsL();
    
    /**
     * Get CS signal level parameters
     * @since S60 3.2
     * @return CS signal level parameters
     */
    TSignalLevelParams CsSignalLevelParams() const;

    /**
     * Get PS signal level parameters
     * @since S60 3.2
     * @return PS signal level parameters
     */
    TSignalLevelParams PsSignalLevelParams() const;
    
private:

    /**
     * C++ Constructor
     */
    CVccHoPolicyReader();

    /**
     * 2nd phase constructor
     */
    void ConstructL();

    /*
    * Defines policy for preferred domain
    * @since S60 3.2
    */
    void ResolvePreferredDomainL();

    /*
    * Defines policy for immediate domain transfer 
    * @since S60 3.2
    */
    void ResolveImmediateHoL();
    
    /*
    * Defines policy for allowed handover direction 
    * @since S60 3.2
    */
    void ResolveHoDirectionL();
    
    /*
    * Defines policy is handover allowed when  
    * mobile device has held or waiting calls
    * @since S60 3.2
    */
    void ResolveHoInHeldWaitingCallsL();

    /*
    * Defines policy for signal levels
    * @since S60 3.2
    */
    void ResolveSignalLevelParametersL();
    
    /*
     * Defines can HO be made if call is CS originated
     */
    void ResolveDtAllowedWhenCsOriginatedL();
    
private: //data
    
    /**
    * Reads handover specific settings from sp table
    * Own. 
    */
    CVccSPSettings* iSpSettings;
   
    /*
    * Contains settings that affect when handover 
    * can be performed.
    */ 
    TVccHoPolicy   iPolicy;
    
    /** PS signal level parameters */
    TSignalLevelParams iPsParams;
    
    /** CS signal level parameters */
    TSignalLevelParams iCsParams;
    
    VCC_UNITTEST( UT_CVccHoPolicy )
    };

#endif //  C_VCCHOPOLICYREADER_H
