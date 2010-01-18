/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  RCSE Settings Import from DBMS
*
*/



#ifndef C_RCSEDBIMPORTER_H
#define C_RCSEDBIMPORTER_H


#include <e32base.h>
#include <d32dbms.h>

#include "crcseprofileentry.h"
#include "crcseaudiocodecentry.h"


/**  Struct for mapping old and new ids of entries */
class TRCSEIdPair
    {
public:
    TUint32 iOldId;
    TUint32 iNewId;
    };

/**
 *  Utility class that imports rcse settings from database and
 *  stores them to central repository.
 *
 *  Enables the restoring of voip settings from previous SW versions.
 *
 *  @code
 *   if ( CRCSEDbImporter::ImportExists() && 
          !CRCSEDbImporter::IsImportRunning() )
        {
        CRCSEDbImporter* importer = CRCSEDbImporter::NewLC();
        importer->ImportAndStoreL();
        CleanupStack::PopAndDestroy( importer );
        }

 *  @endcode
 *
 *  @lib rcse.lib
 *  @since S60 v3.2
 */
class CRCSEDbImporter : public CBase
    {

public:

    static CRCSEDbImporter* NewL();

    static CRCSEDbImporter* NewLC();

    /**
    * Destructor.
    */
    virtual ~CRCSEDbImporter();

    /**
     * Check if there is imported (restored) rcse database file.
     *
     * @since S60 v3.2
     * @return ETrue if imported db file found
     */
    static TBool ImportExists();

    /**
     * Check if there is import operation already started.
     *
     * @since S60 v3.2
     * @return ETrue if import is already ongoing
     */
    static TBool IsImportRunning();

    /**
     * Imports rcse entries from database and stores them to repository.
     *
     * @since S60 v3.2
     */
    void ImportAndStoreL();

private:

    CRCSEDbImporter();

    void ConstructL();

    /**
     * Sets lock that prevents other instances to start import operation.
     *
     * @since S60 v3.2
     */
    void SetImportLock();
    
    /**
     * Release import lock.
     *
     * @since S60 v3.2
     */
    void ReleaseImportLock();

    /**
     * Imports RCSE profile entries from database to the iProfiles array.
     *
     * @since S60 v3.2
     */
    void ImportVoIPProfilesL();

    /**
     * Imports RCSE audio codec entries from database to the iCodecs array.
     *
     * @since S60 v3.2
     */
    void ImportAudioCodecsL();

    /**
     * Deletes all imported db files
     *
     * @since S60 v3.2
     */
    void DeleteImports();

    /**
     * Maps data from database query to profile entry.
     *
     * @since S60 v3.2
     * @param aColumnIndex Column that is mappped.
     * @param aView Query results.
     * @param aEntry Data from requested column is mapped and
     *               stored to this entry.
     */
    void MapColumnToEntryL( TInt aColumnIndex, 
                            const RDbView& aView, 
                            CRCSEProfileEntry& aEntry );

    /**
     * Maps data from database query to audio codec entry.
     *
     * @since S60 v3.2
     * @param aColumnIndex Column that is mappped.
     * @param aView Query results.
     * @param aEntry Data from requested column is mapped and
     *               stored to this entry.
     */
    void MapColumnToEntryL( TInt aColumnIndex, 
                            const RDbView& aView, 
                            CRCSEAudioCodecEntry& aEntry );


    /**
     * Reads TSettingIds structs from given descriptor to array.
     *
     * @since S60 v3.2
     * @param aNumbers Descriptor containing TSettingIds structs.
     * @param aArray Container for TSettingIds structs.
     */
    void GetProfileIdsFromDescriptorL( const TDesC& aNumbers,
                                       RArray<TSettingIds>& aArray );



    /**
     * Reads IDs from given descriptor to array.
     *
     * @since S60 v3.2
     * @param aNumbers Descriptor containing list of IDs.
     * @param aArray Container for IDs.
     */
    template<class T>
    void GetNumbersFromDescriptorL( const TDesC& aNumbers,
                                    RArray<T>& aArray );


    /**
     * Updates new preferred audio codec IDs to imported VoIP 
     * profile entries.
     *
     * @since S60 v3.2
     */
    void UpdatePreferredCodecs();

    /**
     * Stores imported voip profiles.
     *
     * @since S60 v3.2
     */
    void StoreVoIPProfilesL();

    /**
     * Stores imported audio codecs.
     *
     * @since S60 v3.2
     */
    void StoreAudioCodecsL();



private: // data

    /**
     * Database object.
     * Own.
     */
    RDbNamedDatabase iDb;

    /**
     * Database session.
     * Own.
     */
    RDbs iDbSession;

    /**
     * Container for imported VoIP profiles.
     * Own.
     */
    RPointerArray<CRCSEProfileEntry> iProfiles;

    /**
     * Container for imported Audio Codecs.
     * Own.
     */
    RPointerArray<CRCSEAudioCodecEntry> iCodecs;

    /**
     * An array for pairs of old and new codec IDs
     * Own.
     */
    RArray<TRCSEIdPair> iCodecIds;

private:
    // For testing
    #ifdef TEST_EUNIT
        friend class UT_CRCSEDbImporter;
    #endif                                  



    };



#endif // C_RCSEDBIMPORTER_H
