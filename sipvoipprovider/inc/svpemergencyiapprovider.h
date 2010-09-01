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
* Description:  Provides IAP IDs for emergency
*
*/


#ifndef C_SVPEMERGENCYIAPPROVIDER_H
#define C_SVPEMERGENCYIAPPROVIDER_H

#include <rconnmon.h> // For RConnectionMonitor
#include "svputdefs.h"

/**
 *  Provides IAP IDs by using RConnectionMonitor
 *
 *  @lib svp.dll
 *  @since S60 3.2
 */
class CSVPEmergencyIapProvider : public CActive
    {

public:

    /**
     * Two-phased constructor.
     * @param aPriority Active object priority
     */
    static CSVPEmergencyIapProvider* NewL( TPriority aPriority );
    
    static CSVPEmergencyIapProvider* NewLC( TPriority aPriority );
    
    /**
     * Destructor.
     */
    virtual ~CSVPEmergencyIapProvider();

    /**
     * Requests for IAP IDs
     *
     * @since S60 3.2
     * @param aIapIds Array of IAP IDs to be filled
     * @return Error code
     */
    TInt RequestIapIds( RArray<TUint>& aIapIds );

private:

    CSVPEmergencyIapProvider( TPriority aPriority );

    void ConstructL();
    
// from base class CActive

    void DoCancel();
    
    void RunL();
    
private: // data

    /**
     * Contains IAP info when request complete
     */
    TConnMonIapInfoBuf iIapInfoBuf;

    /**
     * Contains error code of request
     */
    TInt iError;
    
    /**
     * Connection monitor
     */        
    RConnectionMonitor iConnectionMonitor;

    /**
     * Wait object.
     * Own.
     */
    CActiveSchedulerWait* iWait;

private:

    // For testing
    SVP_UT_DEFS
    
    };

#endif // C_SVPEMERGENCYIAPPROVIDER_H
