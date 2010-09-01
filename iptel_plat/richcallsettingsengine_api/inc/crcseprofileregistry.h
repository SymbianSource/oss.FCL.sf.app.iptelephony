/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Offers API for storing, getting and updating profiles.
*
*/



#ifndef CRCSEPROFILEREGISTRY_H
#define CRCSEPROFILEREGISTRY_H

//  INCLUDES
#include <e32base.h>
#include <rcseregistrybase.h>
#include <crcseprofileentry.h>
#include <sipprofileregistryobserver.h>



// FORWARD DECLARATION
class CSIPManagedProfileRegistry;
class CSIPProfile;


// CLASS DECLARATION

/**
*  Offers getting, setting and updating of profiles.
*
*  @lib RCSE.lib
*  @since S60 3.0
*/
class CRCSEProfileRegistry: public CRCSERegistryBase,
                            public MSIPProfileRegistryObserver
    {    
public:  // Constructors and destructor
    
    /**
    * Two-phased constructor.
    */
    IMPORT_C static CRCSEProfileRegistry* NewL();

    /**
    * Two-phased constructor.
    */
    IMPORT_C static CRCSEProfileRegistry* NewLC();
    
    /**
    * Destructor.
    */
    IMPORT_C virtual ~CRCSEProfileRegistry();

public: // New functions

    /**
    * Search a profile, which provider name setting is same as 
    * aProviderName. Leaves with KErrNotFound if such profile
    * is not found. Default values are reseted to aFoundEntry
    * before the search.
    *
    * @since S60 v3.0
    * @param aProviderName Search criteria for profiles.
    * @param aFoundEntry A found profile entry, which provider name is 
    * same than aProviderName.
    */
    IMPORT_C void FindL( const TDesC& aProviderName, 
        CRCSEProfileEntry& aFoundEntry );

    /**
    * Search a profile, which profile id is same as aProfileId. 
    * Leaves with KErrNotFound if such profile is not found.
    * Default values are reseted to aFoundEntry before the search.
    *
    * @since S60 v3.0.
    * @param aProfileId Search criteria for profiles.
    * @param aFoundEntry A found profile entry, which provider name is 
    * same than aProviderName.
    */
    IMPORT_C void FindL( TUint32 aProfileId, 
        CRCSEProfileEntry& aFoundEntry );

    /**
    * Gets all profile identifiers which are stored to RCSE. If there are
    * no profiles stored, aAllIds will contain zero entries.
    *
    * @since S60 v3.0
    * @param aAllIds Array, where the profile identifiers are stored.
    */
    IMPORT_C void GetAllIdsL( RArray<TUint32>& aAllIds );

    /**
    * Adds new profile entry.
    *
    * @since S60 v3.0
    * @param aNewEntry New entry.
    * @return Profile identifier of the added profile.
    */
    IMPORT_C TUint32 AddL( const CRCSEProfileEntry& aNewEntry );

    /**
    * Deletes a profile, which profile id is same than aProfileId. 
    *
    * @since S60 v3.0
    * @param aProfileId Profile identifier
    */
    IMPORT_C void DeleteL( TUint32 aProfileId );
    /**
    * Updates profile, which profile identifier is same than aProfileId.
    * Only settings that are set in aUpdateData are updated in profile.
    * Leaves with KErrNotFound if profile is not found.
    *
    * @since S60 v3.0
    * @param aId Profile identifier.
    * @param aUpdateData Updated data, which are updated to profile.
    */
    IMPORT_C void UpdateL( TUint32 aId, 
        const CRCSEProfileEntry& aUpdateData );

    /**
    * Resets default values for profile. Default profile is a profile, 
    * which can be used when no other profile is available.
    *
    * @since S60 v3.0
    * @param aDefaultProfile Default profile settings are set to this.
    */
    IMPORT_C void GetDefaultProfile( 
        CRCSEProfileEntry& aDefaultProfile ) const;

    /**
    * Search a profiles, where service provider id is same as aServiceId.
    * Content of aFoundEntries array is deleted before the search.
    * On return, aFoundEntries is empty if requested entries are not found.
    *
    * @since S60 v3.2.
    * @param aServiceId Search criteria for profiles.
    * @param aFoundEntries Container for found profiles
    */
    IMPORT_C void FindByServiceIdL( TUint32 aServiceId, 
        RPointerArray<CRCSEProfileEntry>& aFoundEntries );

    /**
    * Search a profiles that have requested aSipProfileId.
    * Content of aFoundEntries array is deleted before the search.
    * On return, aFoundEntries is empty if requested entries are not found.
    *
    * @since S60 v3.2.
    * @param aSipProfileId Search criteria for profiles.
    * @param aFoundEntries Container for found profiles
    */
    IMPORT_C void FindBySIPProfileIdL( TUint32 aSipProfileId, 
        RPointerArray<CRCSEProfileEntry>& aFoundEntries );

    /**
    * Adds bunch of profile entries to store in one transaction.
    * Creates new service provider setting entry to service table
    * if iServiceProviderId has not been set in given profile entry.
    *
    * @since S60 v3.2
    * @param aEntries 
    */
    void AddL( RPointerArray<CRCSEProfileEntry>& aEntries );


    /** 
    * From MSIPProfileRegistryObserver
    * SIP profile information event.
    *
    * @since S60 v3.1
    * @param aSIPProfileId id for profile
    * @param aEvent type of information event
    */
    void ProfileRegistryEventOccurred( TUint32 aSIPProfileId,
                                       TEvent aEvent );

    /**
    * From MSIPProfileRegistryObserver
    * An asynchronous error has occurred related to SIP profile.
    *
    * @since S60 v3.1
    * @param aSIPProfileId the id of failed profile
    * @param aError a error code
    * @return none
    */
    void ProfileRegistryErrorOccurred( TUint32 aSIPProfileId, 
                                       TInt aError );


private:

    /**
    * C++ default constructor.
    */
    CRCSEProfileRegistry();

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();

    /**
    * Do actual search of requested entry.
    * Leaves with KErrNotFound if such profile is not found.
    * Default values are reseted to aFoundEntry before the search.
    *
    * @since S60 v3.0.
    * @param aProfileId Search criteria for profiles.
    * @param aFoundEntry A found profile entry
    * @leave KErrNotFound if profile doesn't exist.
    */
    void FindEntryL( TUint32 aProfileId, CRCSEProfileEntry& aFoundEntry );

    /**
    * Adds or updates settings entry to storage depending of the aId
    * parameter. Entry is stored as new entry if aId parameter have
    * been set to KNoEntryId, otherwise existing entry is updated.
    *
    * @since S60 v3.2
    * @param aId ID of entry if exists. 
    * @param aEntry Entry to be added to storage.
    * @param aUpdate Determines add or update operation.
    *                New entry is added when value is set to EFalse (default).
    * @return ID of entry.
    */
    void AddOrUpdateEntryL( TUint32& aId, const CRCSEProfileEntry& aEntry );

    /**
    * Convert xxx
    * .
    *
    * @since S60 v3.2.
    * @param aEntry
    * @param aProperties
    */
    void ConvertPropertiesToEntryL( CRCSEProfileEntry& aEntry,
                                    RIpAppPropArray& aProperties );


    /**
    * Convert xxx
    * .
    *
    * @since S60 v3.2.
    * @param aEntry
    * @param aProperties
    */
    void ConvertEntryToPropertiesL( const CRCSEProfileEntry& aEntry,
                                    RIpAppPropArray& properties );

    /**
    * Extract protocol ids from descriptor. 
    *
    * @since S60 v3.2
    * @param aDes Contains TSettingIds structs.
    * @param aArray Contains extracted TSettingIds struct when return.
    */
    void ExtractProtocoIdsL( const TDesC& aDes, RArray<TSettingIds>& aArray );

    /**
    * Validates that linked SIP profiles are up to date.
    * Removes invalid links and updates entry to data base
    *
    * @since S60 v3.1
    * @param aEntry Profile entry fetched from data base
    * @return 
    */
    void UpdateSIPProfileLinksL( CRCSEProfileEntry& aEntry );

    /**
    * Creates and stores service provider settings entry with
    * default values, when new profile is created.
    * Service ID of stored service settings is set to profile.
    *
    * @since S60 v3.2.
    * @param aNewEntry New entry.
    */
    void CreateDefaultServiceSettingsL( CRCSEProfileEntry& aNewEntry );

    /**
    * Deletes service settings and audio codec settings referenced
    * from profile.
    *
    * @since S60 v3.2.
    * @param aProfileId Id of deleted VoIP profile
    */
    void DeleteReferenceSettingsL( TUint32 aProfileId );

    /**
    * Checks if Service table exist with rerenced entry name.
    * @since S60 3.2.
    * @param aProviderName.
    */        
    TBool NameExistsInServiceTableL( const TDesC& aProviderName ) const;
    
    /**
    * Checks if service id is used for profile.
    * @since S60 3.2.
    * @param aEntry.
    */        
    void LeaveIfServiceIdInUseL( const CRCSEProfileEntry& aEntry );

    /**
    * Updates service settings related to aEntry if needed
    * @since S60 3.2.
    * @param aEntry.
    */        
    void UpdateServiceSettingsL( const CRCSEProfileEntry& aEntry );

    /**
    * Updates service id related to given entry id.
    * @since S60 3.2.
    * @param aId.
    * @param aServiceId New service id.
    */        
    void UpdateServiceIdL(  TUint32 aId, TUint32 aServiceId );

    /**
    * Implementation for cleanup item.
    * Resets and destroys array of the entries. 
    * @param anArray RPointerArray pointer.
    */
    static void ResetAndDestroyEntries( TAny* anArray );

    /**
    * Copies values from a const entry to a modifiable one.
    * @since S60 3.2.
    * @param aSourceEntry Entry whose values are to be copied.
    * @param aDestinationEntry Entry to whom values are copied.
    */
    void CopyEntryValues( const CRCSEProfileEntry& aSourceEntry, 
        CRCSEProfileEntry& aDestinationEntry );
private:     // Data

    /**
    * SIP registry ( from SIP Profile Client )
    * Own.
    */
    CSIPManagedProfileRegistry* iSIPRegistry;

    /**
    * SIP profiles ( from SIP Profile Client )
    * Own.
    */
    RPointerArray<CSIPProfile> iSIPProfiles;


private:

    // For testing
    #ifdef TEST_EUNIT
        friend class UT_CRCSEProfileRegistry;
    #endif                                  
    };

#endif      // CRCSEPROFILEREGISTRY_H   
            
// End of File
