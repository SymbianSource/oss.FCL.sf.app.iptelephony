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
* Description:  Offers API for storing, getting and updating SIP settings.
*
*/



#ifndef CRCSESIPPSETTINGREGISTRYREGISTRY_H
#define CRCSESIPPSETTINGREGISTRYREGISTRY_H

//  INCLUDES
#include <e32base.h>
#include <rcseregistrybase.h>

// FORWARD DECLARATIONS
class CRCSESIPSettingEntry;

// CLASS DECLARATION

/**
*  Offers getting, setting and updating of sip settings.
*
*  @lib RCSE.lib
*  @since S60 3.0
*/
class CRCSESIPSettingRegistry: public CRCSERegistryBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CRCSESIPSettingRegistry* NewL();

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CRCSESIPSettingRegistry* NewLC();
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CRCSESIPSettingRegistry();

    public: // New functions
 
        /**
        * Search a SIP setting, which profile id is same as aSIPProfileId. 
        * Leaves with KErrNotFound if such profile is not found.
        * Default values are reseted to aFoundEntry before the search.
        * @since S60 3.0.
        * @param aSIPProfileId Search criteria for SIP setting.
        * @param aFoundEntry A found SIP setting entry.
        */
        IMPORT_C void FindL( TUint32 aSIPProfileId, 
            CRCSESIPSettingEntry& aFoundEntry );

        /**
        * Gets all SIP setting identifiers which are stored to RCSE.
        * If there are no profiles stored, aAllIds will contain zero entries.
        * @since S60 3.0
        * @param aAllIds Array, where the SIP setting identifiers are stored.
        */
        IMPORT_C void GetAllIdsL( RArray<TUint32>& aAllIds );

        /**
        * Adds new profile entry.
        * @since S60 3.0
        * @param aNewEntry New entry.
        * @return SIP setting identifier of the added setting.
        */
        IMPORT_C TUint32 AddL( const CRCSESIPSettingEntry& aNewEntry );

        /**
        * Deletes a profile, which profile id is same than aProfileId. 
        * @since S60 3.0
        * @param aId SIP setting identifier
        */
        IMPORT_C void DeleteL( TUint32 aId );

        /**
        * Updates SIP setting, which identifier is same than aId.
        * Only settings that are set in aUpdateData are updated in profile.
        * Leaves with KErrNotFound if setting is not found.
        * @since S60 3.0
        * @param aId setting identifier.
        * @param aUpdateData Updated data, which are updated to SIP Setting.
        */
        IMPORT_C void UpdateL( TUint32 aId, 
            const CRCSESIPSettingEntry& aUpdateData );

        /**
        * Resets default values for SIP profile. Default profile is a profile,
        * which can be used when no other profile is available.
        * @since S60 3.0
        * @param aDefaultProfile Default profile settings are set to this.
        */
        IMPORT_C void GetDefaultProfile( 
            CRCSESIPSettingEntry& aDefaultProfile ) const;
            
    private:

        /**
        * C++ default constructor.
        */
        CRCSESIPSettingRegistry();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        /**
        * Maps data from database to aEntry. 
        * @since S60 3.0
        * @param aColumnIndex Index to a column in database row.
        * @param aView View to database to access values in column.
        * @param aEntry SIP entry settings are set to here.
        */
        void MapColumnToEntry( TInt aColumnIndex, const RDbView& aView, 
            CRCSESIPSettingEntry& aEntry );

        /**
        * Initializes database view.
        * @since S60 3.0
        * @param aView View to be initialized-
        * @param aSqlStatement SQL statement which initializes the database.
        */
        void InitializeDbViewL( RDbView& aView, 
            const TDesC& aSqlStatement );

        /**
        * Initializes the database with aSqlQuery and sets data
        * from database to aEntry.
        * @since S60 3.0
        * @param aEntry SIP setting entry, which single setting is set.
        * @param aSqlQuery SQL query used to get the data from database.
        */
        void SetDataFromDbToEntryL( CRCSESIPSettingEntry& aEntry,
            const TDesC& aSqlQuery );
    };

#endif      // CRCSESIPPSETTINGREGISTRYREGISTRY_H
            
// End of File
