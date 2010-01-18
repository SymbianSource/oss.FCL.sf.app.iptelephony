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
* Description:  Offers API for storing, getting and updating SCCP setting entry.
*
*/



#ifndef CRCSESCCPSETTINGREGISTRY_H
#define CRCSESCCPSETTINGREGISTRY_H

//  INCLUDES
#include <e32base.h>
#include <rcseregistrybase.h>

// FORWARD DECLARATIONS
class CRCSESCCPEntry;

// CLASS DECLARATION

/**
*  Offers getting, setting and updating of SCCP setting.
*
*  @lib RCSE.lib
*  @since S60 3.0
*/
class CRCSESCCPSettingRegistry: public CRCSERegistryBase
    {
    public: // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CRCSESCCPSettingRegistry* NewL();

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CRCSESCCPSettingRegistry* NewLC();
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CRCSESCCPSettingRegistry();

    public: // New functions

        /**
        * Search a SCCP Setting, which id is same as aSCCPSettingId. 
        * Leaves with KErrNotFound if such setting is not found.
        * Default values are reseted to aFoundEntry before the search.
        * @since S60 3.0
        * @param aSCCPSettingId Search criteria for SCCP setting.
        * @param aFoundEntry A found SCCP setting entry.
        */
        IMPORT_C void FindL( TUint32 aSCCPSettingId, 
            CRCSESCCPEntry& aFoundEntry );

        /**
        * Gets all profile identifiers which are stored to RCSE. If there are
        * no profiles stored, aAllIds will contain zero entries.
        * @since S60 3.0
        * @param aAllIds Array, where the SCCP identifiers are stored.
        */
        IMPORT_C void GetAllIdsL( RArray<TUint32>& aAllIds );

        /**
        * Adds new profile entry.
        * @since S60 3.0
        * @param aNewEntry New entry.
        * @return SCCP identifier of the added SCCP setting.
        */
        IMPORT_C TUint32 AddL( const CRCSESCCPEntry& aNewEntry);
        
        /**
        * Deletes a profile, which profile id is same than aProfileId. 
        * @since S60 3.0
        * @param aId SCCP identifier
        */
        IMPORT_C void DeleteL( TUint32 aId );

        /**
        * Updates profile, which profile identifier is same than aProfileId.
        * Only settings that are set in aUpdateData are updated in profile.
        * Leaves with KErrNotFound if profile is not found.
        * @since S60 3.0
        * @param aId Profile identifier.
        * @param aUpdateData Updated data, which are updated to SCCP setting entry.
        */
        IMPORT_C void UpdateL( TUint32 aId, 
            const CRCSESCCPEntry& aUpdateData );

        /**
        * Resets default values for SCCP profile. Default profile is a profile,
        * which can be used when no other profile is available.
        * @since S60 3.0
        * @param aDefaultProfile Default SCCP settings are set to this.
        */
        IMPORT_C void GetDefaultProfile( 
            CRCSESCCPEntry& aDefaultProfile ) const;

    private:

        /**
        * C++ default constructor.
        */
        CRCSESCCPSettingRegistry();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        /**
        * Maps data from database to aEntry. 
        * @since S60 3.0
        * @param aColumnIndex Index to a column in database row.
        * @param aView View to database to access values in column.
        * @param aEntry Profile entry settings are set to here.
        */
        void MapColumnToEntry( TInt aColumnIndex, const RDbView& aView, 
            CRCSESCCPEntry& aEntry );

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
        * @param aEntry Profile entry, which single setting is set.
        * @param aSqlQuery SQL query used to get the data from database.
        */
        void SetDataFromDbToEntryL( CRCSESCCPEntry& aEntry,
            const TDesC& aSqlQuery );
    };

#endif      // CRCSESCCPSETTINGREGISTRY_H   
            
// End of File
