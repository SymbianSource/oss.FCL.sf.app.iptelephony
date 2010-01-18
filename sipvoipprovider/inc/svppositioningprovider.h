/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides position information
*
*/


#ifndef C_SVPPOSITIONINGPROVIDER_H
#define C_SVPPOSITIONINGPROVIDER_H

#include <lbs.h>
#include "svputdefs.h"

class MSVPPositioningProviderObserver;

/**
 *  Provides position information
 *  Provides position information through Symbian Location FW. Actual 
 *  information comes via a DHCP query which is implemented in a separate 
 *  positioning module.
 *
 *  @lib svp.dll
 *  @since S60 3.2
 */
class CSVPPositioningProvider : public CActive
    {

public:

    /**
     * Two-phased constructor.
     * @param aPriority Active object priority
     * @param aObserver Observer
     */
    static CSVPPositioningProvider* NewL( 
        TPriority aPriority, MSVPPositioningProviderObserver& aObserver );
    
    static CSVPPositioningProvider* NewLC( 
        TPriority aPriority, MSVPPositioningProviderObserver& aObserver );
    
    /**
    * Destructor.
    */
    virtual ~CSVPPositioningProvider();

    /**
     * Opens positioning module
     *
     * @since S60 3.2
     * @param aModuleId Module to be opened
     */
    void OpenModuleL( TInt aModuleId );

    /**
     * Closes positioning module
     *
     * @since S60 3.2
     */
    void CloseModule();

    /**
     * Requests position information asynchronically
     *
     * @since S60 3.2
     * @param aIapId IAP ID
     * @param aApplicationName Identifies requestor
     * @param aTimeout Timeout for position request in microseconds
     */
    void MakePositioningRequestL( 
        TUint32 aIapId, 
        const TDesC& aApplicationName, 
        TUint32 aTimeout );

private:

    CSVPPositioningProvider( 
        TPriority aPriority, MSVPPositioningProviderObserver& aObserver );

    void ConstructL();
    
// from base class CActive

    void DoCancel();
    
    void RunL();
    
private: // data

    /**
     * Observer
     */
    MSVPPositioningProviderObserver& iObserver;

    /**
     * Position server
     */
    RPositionServer iPositionServer;

    /**
     * Positioner
     */
    RPositioner iPositioner;

    /**
     * Generic position information
     * Own.  
     */
    HPositionGenericInfo* iPositionGenericInfo;

private:

    // For testing
    SVP_UT_DEFS
    
    };

#endif // C_SVPPOSITIONINGPROVIDER_H
