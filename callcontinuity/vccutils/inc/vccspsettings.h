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
* Description:   SP settings handler functions for Vcc settings.
*
*/



#ifndef C_VCCSPSETTINGS_H
#define C_VCCSPSETTINGS_H

// INCLUDES
#include <e32base.h>
#include <spdefinitions.h>

#include "vccunittesting.h"

class CWPCharacteristic;
class CSPEntry;
class CSPProperty;
class CSPSettings;

/**
 * CVccSPSettings handles saving of Vcc settings.
 *
 * Settings are received from CVccAdapter (an adapter in Provisioning framework)
 * and VCC DM Adapter and stored to SPsettings
 *
 *  @code
 *   
 *    _LIT( KServiceName, "VCC" );
 *    _LIT( KVdiValue, "sip:2323" );
 *   
 *   CVccSPSettings* vccSettings = CVccSPSettings::NewL();
 *   
 *   //Store VDI
 *   vccSettings->SetServiceNameL( KServiceName );
 *   vccSettings->SetVdiL( KVdiValue );
 *   vccSettings->StoreL();
 *
 *   //Read Preferred domain from store 
 *   vccSettings->ReadSettingsL( KServiceName );
 *   TInt prefDom = vccSettings->PreferredDomain();
 *  
 *   delete vccSettings;
 *  @endcode
 * 
 * @lib vccutils.dll
 * @since S60 v3.2
 */
class CVccSPSettings : public CBase
    {

public:

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CVccSPSettings* NewL();

    /**
    * Destructor.
    */
    IMPORT_C virtual ~CVccSPSettings();

    /**
     * Saves Vcc settings.
     *
     * @since S60 3.2
     */
    IMPORT_C void StoreL();

    /**
     * Returns provider name of stored settings
     *
     * @since S60 3.2
     * @return Constant descriptor reference to name of stored settings
     */
    IMPORT_C const TDesC& ProviderId();

    /**
     * Sets name of stored settings
     *
     * @since S60 3.2
     * @param aValue Given value
     */
    IMPORT_C void SetProviderIdL( const TDesC& aValue );

    /**
     * Sets name of stored settings
     *
     * @since S60 3.2
     * @param aValue Given value
     */
    IMPORT_C void SetServiceNameL( const TDesC& aValue );
    
    /**
     * Sets name of stored settings
     *
     * @since S60 3.2
     * @param aValue Given value
     */
    IMPORT_C const TDesC& ServiceName();
    
    /**
     * Sets VDI
     *
     * @since S60 3.2
     * @param aValue Given value
     * @return KErrNone if VDN is valid and set into member variable,
     *         Symbian error code if VDN is not valid thus not set into member 
     *         variable, otherwise leaves in system error cases.
     */
    IMPORT_C TInt SetVdiL( const TDesC& aValue );

    /**
     * Sets VDN
     *
     * @since S60 3.2
     * @param aValue Given value
     * @return ETrue if VDN is valid and set into member variable,
     *         EFalse if VDN is not valid thus not set into member variable,
     *         otherwise leaves in system error cases.
     */
    IMPORT_C TBool SetVdnL( const TDesC& aValue );

    /**
     * Sets preferred domain
     *
     * @since S60 3.2
     * @param aValue Given value
     */
    IMPORT_C void SetPreferredDomainL( const TDesC& aValue );
    
    /**
     * Sets immediate domain transfer
     *
     * @since S60 3.2
     * @param aValue Given value
     */
    IMPORT_C void SetImmediateDtL( const TDesC& aValue );

    /**
     * Sets Domain transfer from CS to Ps is allowed
     *
     * @since S60 3.2
     * @param aValue Given value
     */
    IMPORT_C void SetDtCsToPsAllowedL( const TDesC& aValue );

    /**
     * Sets Domain transfer from PS to CS is allowed
     *
     * @since S60 3.2
     * @param aValue Given value
     */
    IMPORT_C void SetDtPsToCsAllowedL( const TDesC& aValue );

    /**
     * Sets domain transfer allowed while held and waiting calls 
     *
     * @since S60 3.2
     * @param aValue Given value
     */
    IMPORT_C void SetDtHeldWaitingCallsAllowedL( const TDesC& aValue );
    
    /**
     * Sets WLAN handover treshold value 
     *
     * @since S60 3.2
     * @param aValue Given value
     */
    IMPORT_C void SetDtWlanHoTresholdL( const TDesC& aValue );
   
    /**
     * Sets WLAN handover hysteresis value  
     *
     * @since S60 3.2
     * @param aValue Given value
     */
    IMPORT_C void SetDtWlanHoHysteresisL( const TDesC& aValue );
   
    /**
     * Sets WLAN handover hysteresis timer for low limit 
     *
     * @since S60 3.2
     * @param aValue Given value
     */
    IMPORT_C void SetDtWlanHoHysteresisTimerLowL( const TDesC& aValue );
   
    /**
     * Sets WLAN handover hysteresis timer for high limit 
     *
     * @since S60 3.2
     * @param aValue Given value
     */
    IMPORT_C void SetDtWlanHoHysteresisTimerHighL( const TDesC& aValue );
   
    /**
     * Sets CS handover treshold value 
     *
     * @since S60 3.2
     * @param aValue Given value
     */
    IMPORT_C void SetDtCsHoTresholdL( const TDesC& aValue );
   
    /**
     * Sets CS handover hysteresis value  
     *
     * @since S60 3.2
     * @param aValue Given value
     */
    IMPORT_C void SetDtCsHoHysteresisL( const TDesC& aValue );
   
    /**
     * Sets CS handover hysteresis timer for low limit 
     *
     * @since S60 3.2
     * @param aValue Given value
     */
    IMPORT_C void SetDtCsHoHysteresisTimerLowL( const TDesC& aValue );
   
    /**
     * Sets CS handover hysteresis timer for high limit 
     *
     * @since S60 3.2
     * @param aValue Given value
     */
    IMPORT_C void SetDtCsHoHysteresisTimerHighL( const TDesC& aValue );
   
    /**
     * Sets Voip service id 
     *
     * @since S60 3.2
     * @param aValue Given value
     */
    IMPORT_C void SetVoipServiceIdL( const TDesC& aValue );

    /**
     * Sets Voip service id 
     *
     * @since S60 3.2
     * @param aValue Given value
     */
	IMPORT_C void SetVoipServiceIdL( TInt aValue );

    
    /**
     * Converts given TDesC value to TInt.
     *
     * @param aValue     value to be converted
     * @return the TInt value
     */
     IMPORT_C TInt ConvertToIntL( const TDesC& aValue );

     /**
      * Finds entry from service provider settings with given service name.
      *
      * @param aServiceName     name of the service entry
      * @param aServiceEntry    entry which will contain current properties 
      */
      IMPORT_C void FindServiceEntryL( const TDesC& aServiceName, 
                                       CSPEntry& aServiceEntry );
      
      /**
       * Finds property data from SP Settings table.
       *
       * @since S60 3.2 
       * @param aServiceId       Service ID 
       * @param aPropertyName    name of the property
       * @param aProperty        Property to store data
       * @return error code 
       */
      IMPORT_C TInt FindPropertyL( TServiceId aServiceId, 
                                   TServicePropertyName aPropertyName,
                                   CSPProperty& aProperty );      
    
      /**
       * Searches service entry with given name and sets found values to
       *   
       *
       * @since S60 3.2
       * @param aValue Given value
       */
      IMPORT_C void ReadSettingsL( const TDesC& aServiceName );
       
      /**
       * Gets preferred domain
       *
       * @since S60 3.2
       * @return current value 
       */
      IMPORT_C TInt PreferredDomainL();
    
      /**
       * Gets immediate domain transfer
       *
       * @since S60 3.2
       * @return current value 
       */
      IMPORT_C TInt ImmediateDtL();

      /**
       * Gets Domain transfer from CS to Ps is allowed
       *
       * @since S60 3.2
       */
      IMPORT_C TInt DtCsToPsAllowedL();

      /**
       * Gets Domain transfer from PS to CS is allowed
       *
       * @since S60 3.2
       * @return current value
       */
      IMPORT_C TInt DtPsToCsAllowedL();

      /**
       * Gets domain transfer allowed while held and waiting calls 
       *
       * @since S60 3.2
       * @return current value
       */
      IMPORT_C TInt DtHeldWaitingCallsAllowedL();
   
      /**
       * Threshold for good signal level ? in GSM
       *
       * @since S60 3.2
       * @return current value
       */
      IMPORT_C TInt CsHoTresholdL();
      
      /**
       * GSM hysterisis value
       *
       * @since S60 3.2
       * @return current value
       */
      IMPORT_C TInt CsHoHysteresisL();
      
      /**
       * GSM hysterisis timer low signal 
       *
       * @since S60 3.2
       * @return current value
       */
      IMPORT_C TInt CsHoHysteresisTimerLowL();
      
      /**
       * GSM hysterisis timer value for high signal   
       *
       * @since S60 3.2
       * @return current value
       */
      IMPORT_C TInt CsHoHysteresisTimerHighL();
      /**
       * Threshold for good signal level ? in WLAN
       *
       * @since S60 3.2
       * @return current value
       */
      IMPORT_C TInt PsHoTresholdL();
      
      /**
       * WLAN hysterisis value
       *
       * @since S60 3.2
       * @return current value
       */
      IMPORT_C TInt PsHoHysteresisL();
      
      /**
       * WLAN hysterisis timer low signal 
       *
       * @since S60 3.2
       * @return current value
       */
      IMPORT_C TInt PsHoHysteresisTimerLowL();
      
      /**
       * WLAN hysterisis timer value for high signal   
       *
       * @since S60 3.2
       * @return current value
       */
      IMPORT_C TInt PsHoHysteresisTimerHighL();
      
      /**
       * Voip service which is used by VCC    
       *
       * @since S60 3.2
       * @return current value
       */
      IMPORT_C TInt VoipServiceId();
      
      /**
       * Is handover allowed when CS originated original call
       *
       * @since S60 3.2
       * @return current value
       */
      IMPORT_C TInt DtAllowedWhenCsOriginatedL();
      
      IMPORT_C void SetDtAllowedWhenCsOriginated( const TDesC& aValue );

private:
    VCC_UNITTEST( T_WpVccAdapter )
    VCC_UNITTEST( T_CVccSPSettings )

    /**
    * Symbian 2nd phase constructor
    */
    void ConstructL();

    /**
    * C++ Constructor
    */
    CVccSPSettings();
    
    /**
    * Adds new integer type property or updates the old property 
    * of the given service entry
    *
    * @param aServiceEntry    entry which contains the properties 
    * @param aName            name of the property
    * @param aValue           value of the property
    */    
    void AddOrUpdatePropertyL( CSPEntry& aServiceEntry, 
                               const TServicePropertyName aName, 
                               TInt aValue );
    /**
    * Adds new desc type property or updates the old property 
    * of the given service entry
    *
    * @param aServiceEntry    entry which contains the properties 
    * @param aName            name of the property
    * @param aValue           value of the property
    */                               
    void AddOrUpdatePropertyL( CSPEntry& aServiceEntry, 
                               const TServicePropertyName aName, 
                               const TDesC& aValue );
    /**
    * Finds the given property and extracts it's TInt value.
    *
    * @param aServiceEntry    entry which contains the properties 
    * @param aPropertyName    name of the property 
    * @param aValue           value of the property
    */  
    TInt GetPropertyValue( CSPEntry& aServiceEntry, 
                            const TServicePropertyName& aPropertyName,
                            TInt& aValue );     
    TInt GetPropertyValue( CSPEntry& aServiceEntry, 
                                const TServicePropertyName& aPropertyName,
                                TDes& aValue );     
    /**
    * Change provider UID and set bootstrap -bit on in VoIP profile
    *
    */ 
    void ModifyVoipProfileL();

private:

    VCC_UNITTEST( T_CWPVccItem )
    VCC_UNITTEST( T_CVccSPSettings )

    /**
     * Provider id vcc settings 
     * Own.
     */
    HBufC* iProviderId;
    
    /**
     * service name vcc settings 
     * Own.
     */
    HBufC* iServiceName;
    
    /**
     * service id
     */
    TServiceId iServiceId;

    /**
     * VDI
     * Own.
     */
    HBufC* iVdi;

    /**
     * VDI
     * Own.
     */
    HBufC* iVdn;

    /**
     * Preferred Domain
     * Own.
     */
    HBufC* iPreferredDomain;
    
    /**
     * Immediate Domain transfer
     * Own.
     */
    HBufC* iImmediateDt;
    
    /**
     * Domain transfer from CS to PS allowed
     * Own.
     */    
    HBufC* iDtCsToPsAllowed;
    
    /**
     * Domain transfer from PS to CS allowed
     * Own.
     */    
    HBufC* iDtPsToCsAllowed;
    
    /**
     * Domain transfer allowed while Held/Waiting calls 
     * Own.
     */    
    HBufC* iDtHeldWaitingCallsAllowed;
   
    /**
     *  WLAN HO treshold value
     * Own.
     */    
    HBufC* iDtWlanHoTreshold;
   
    /**
     *  WLAN HO hysteresis
     * Own.
     */    
    HBufC* iDtWlanHoHysteresis;
    
    /**
     *  WLAN HO hysteresis timer used for low limit
     * Own.
     */    
    HBufC* iDtWlanHoHysteresisTimerLow;
    
    /**
     *  WLAN HO hysteresis timer used for high limit
     * Own.
     */    
    HBufC* iDtWlanHoHysteresisTimerHigh;
    
    /**
     * CS HO treshold value
     * Own.
     */    
    HBufC* iDtCsHoTreshold;
   
    /**
     * CS HO hysteresis 
     * Own.
     */    
    HBufC* iDtCsHoHysteresis;
    
    /**
     * CS HO hysteresis timer used for low limit 
     * Own.
     */    
    HBufC* iDtCsHoHysteresisTimerLow;
    
    /**
     *  CS HO hysteresis timer used for high limit
     * Own.
     */    
    HBufC* iDtCsHoHysteresisTimerHigh; 
   
    /*
    * SP settings
    * Own.
    */ 
    CSPSettings* iSettings;

    /**
	* Voip service id to be used in conjuction with VCC
	*/
    TServiceId iVoipServiceId;
    
    /*
    * VoIP service ID
    * Own.
    */ 
    HBufC* iVoipServiceIdString;
    
    /*
    * Is handover allowed when call orginated in CS domain
    * Own.
    */ 
    HBufC* iDtAllowedWhenCsOriginated;
};

#endif /*C_VCCSPSETTINGS_H*/
