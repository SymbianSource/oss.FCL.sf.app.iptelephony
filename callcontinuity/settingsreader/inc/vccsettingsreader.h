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
* Description:   Definition of the vccsettingsreader class.
*
*/




#ifndef VCCSETTINGSREADER_H
#define VCCSETTINGSREADER_H

#include <e32base.h>
#include <spdefinitions.h>
#include <e32cmn.h>

class CSPSettings;
class CSPProperty;

/**
 *  Defines settings reader for VCC
 *
 *  @code
 *   HBufC* domainNumber;
 *      
 *   domainNumber = VccSettingsReader::DomainTransferNumber();
 *   if ( domainNumber->Length() > 0 )
 *       {
 *       // Number found, use it
 *       DoSomethingWithTheNumber( domainNumber );
 *       }
 * 
 *  delete domainNumber;
 *  ----
 * 
 *   HBufC* domainUri;
 * 
 *   domainUri = VccSettingsReader::DomainTransferUriL();
 *   if ( domainUri->Length() > 0 )
 *       {
 *       // Uri found, make a call using it
 *       CallUsingUriL( domainUri );
 *       }
 *   
 *   delete domainUri;
 *   ---
 *   TInt csServiceId = VccSettingsReader::CSServiceIdL();
 *   MakeCSCallL( csServiceId );
 *   ---
 *   TInt voipServiceId = VccSettingsReader::VoIPServiceIdL();
 *   OpenVoIPCallL( voipServiceId );
 * 
 *   // Get the voip service id which is vcc enabled and
 *   // start that service so the use can do handover using
 *   // that voip service.
 * 
 *   TInt vccEnabledVoipId = VccSettingsReader::VoIPServiceIdL();
 * 
 *   EnableVoipServiceUsingVccEnabledVoip( vccEnabledVoipId );
 *   
 *  @endcode
 *
 *  @since S60 v3.2
 */
class VccSettingsReader
 {
 public:

    /**
     * Gets the domain transfer number (VDN) from settings
     * Note that the caller must destroy the object returned
     * The method leaves if underlying SPSettings leave.
     * The caller should check if the returned object has length > 0
     * meaning something was found.
     * 
     * @since S60 v3.2
     * @return Pointer to the buffer containing the VDN. If length
     * of the returned buffer is zero, VDN was not found.
     */
     static HBufC* DomainTransferNumberL();

    /**
     * Gets the domain transfer URI (VDI) from settings
     * Note that the caller must destroy the object returned
     * The method leaves if SPSettings leave.
     * 
     * @since S60 v3.2
     * @return Pointer to the buffer containing the VDI. If length
     * of the returned buffer is zero, VDI was not found.
     */
     static HBufC* DomainTransferUriL();

     /**
      * Gets the CS service id from the SP-settings table
      * by searching an entry which has name "CS"
      * The method leaves if SPSettings leave.
      * 
      * @since S60 v3.2
      * @return Service id if entry is found, otherwise KErrNotFound
      */     
     static TInt CSServiceIdL();
     
     /**
      * Gets the VoIP service id from the SP-settings table
      * The method leaves if SPSettings leave.
      * The VoIP service id is get from the VCC service (VCC settings
      * will get the VoIP service id when settings are saved into SP settings
      * table).
      * 
      * @since S60 v3.2
      * @return Service id if entry is found, otherwise KErrNotFound
      */     
     static TInt VoIPServiceIdL();
     
     /**
      * Gets the VCC service id from the SP-settings table
      * The method leaves if SPSettings leave.
      * 
      * @since S60 v3.2
      * @return Service id if entry is found, otherwise KErrNotFound
      */     
     static TInt VccSettingsReader::VccServiceIdL();
     
     /**
      * Gets service id that is mapped for the specified VCC call provider 
      * plugin from the SP-settings table 
      * 
      * The method leaves if SPSettings leave.
      * 
      * @since S60 v3.2
      * @return Service id if entry is found, otherwise KErrNotFound
      */     
     static TInt VccSettingsReader::ServiceIdL( TInt aCallProviderPluginUid );
private:

    /**
     * Gets property value from SP settings
     *
     * @since S60 v3.2
     * @param aPropertyName Name of the property
     * @return A pointer to the buffer containing fetched value.
     * Note that the caller must destroy the object returned
     */
    static HBufC* PropertyValueByNameL( TServicePropertyName aPropertyName );

    /**
     * Gets the service id of given entry name
     *
     * @since S60 v3.2
     * @param aEntryName The name of the entry, e.g. "CS","VCC".
     * @return Service id if the entry was found, otherwise KErrNotFound
     */
    static TInt ServiceIdByNameL( const TDesC& aEntryName );

    /**
     * Gets the VoIP service id from the SP-settings table
     * The method leaves if SPSettings leave.
     * The VoIP service id is get from the VCC service (VCC settings
     * will get the VoIP service id when settings are saved into SP settings
     * table).
     * 
     * @since S60 v3.2
     * @param aPropertyName The name of VCC property holding VoIP service id.
     * @return Service id if property is found, otherwise KErrNotFound
     */          
    static TInt DoVoIPServiceIdL( TServicePropertyName aPropertyName );
    
    /**
     * Gets service ids where property is same as given.
     * 
     * @since S60 v3.2
     * @param aServiceIds Array which is filled with service ids
     * @param aPropertyName The name of the property
     */              
    static void GetServiceIdsByPropertyNameL(
            RIdArray& aServiceIds,
            TServicePropertyName aPropertyName );
    
    
    // No copy, assigment or creation of this class.
    
    /**
     * Assigment operator
     * 
     * @since S60 v3.2
     */
    VccSettingsReader& operator = ( const VccSettingsReader& );
    
    /**
     * Default C++ constructor
     * 
     * @since S60 v3.2
     */
    VccSettingsReader();
    
    
    /**
     * Copy constructor
     * 
     * @since S60 v3.2
     */
    VccSettingsReader( const VccSettingsReader& );
    
 };

#endif // VCCSETTINGSREADER_H
