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
* Description:  
*        class definition for CCchWlanExtension
*
*/


#ifndef CCHWLANEXTENSION_H
#define CCHWLANEXTENSION_H

// INCLUDES
#include <commdb.h>
#include <e32base.h>

// FORWARD DECLARATIONS
class CRepository;

// CLASS DECLARATION

/**
*  CCchWlanExtension class.
*  @since 3.2
*  Extension class for WLAN functionality.
*/
class CCchWlanExtension : public CBase
    {
public: // Constructors and destructor
    
    /**
    * Two-phased constructor
    */      
    static CCchWlanExtension* NewL( );

    /**
    * Destructor.
    */      
    ~CCchWlanExtension();

public:

    /**
    * Activates wlan scan if not activated
    */
    void EnableWlanScanL();
    
    /**
    * Activates wlan scan, disables it first
    */
    void ForceEnableWlanScanL();
    
    /**
    * Deactivates wlan scan
    */
    void DisableWlanScanL();

private:
        
    /**
    * Checks WLAN availability setting status.
    * @since S60 3.2
    * @return Show WLAN availability scan rate
    */ 
    TUint32 CheckAvailabilityStatusL();
    
    /**
    * Sets WLAN availability switch based on scan rate given.
    * @since S60 3.2
    * @param aScanRate for WLAN setting data value to be set
    */
    void SetAvailabilitySwitchL( TUint32 aScanRate );
        
private:
    
    /**
    * C++ default constructor
    */
    CCchWlanExtension( );

    /**
    * Symbian OS default constructor
    * @return void
    */
    void ConstructL();

private: // data
        
    // Pointer to Access Point Comms Database
    CCommsDatabase* iAPCommsDatabase;
    
    // Pointer to Access Point Comms Database
    CCommsDatabase* iWLANCommsDatabase;
    
    //Central repository
    CRepository* iCchRepository;
    
    //Last saved scan interval
    TUint32 iLastSavedScanInterval;
    };

#endif // CCHWLANEXTENSION_H

// End of File
