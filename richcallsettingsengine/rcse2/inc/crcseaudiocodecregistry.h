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
* Description:  Offers API for storing, getting and updating Audio Codec entry.
*
*/



#ifndef CRCSEAUDIOCODECREGISTRY_H
#define CRCSEAUDIOCODECREGISTRY_H

//  INCLUDES
#include <e32base.h>
#include <rcseregistrybase.h>

// FORWARD DECLARATIONS
class CRCSEAudioCodecEntry;

// CLASS DECLARATION

/**
*  Offers getting, setting and updating of Audio codec entries.
*
*  @lib RCSE.lib
*  @since S60 v3.0
*/
class CRCSEAudioCodecRegistry: public CRCSERegistryBase
    {
public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    */
    IMPORT_C static CRCSEAudioCodecRegistry* NewLC();
    
    /**
    * Two-phased constructor.
    */
    IMPORT_C static CRCSEAudioCodecRegistry* NewL();        
    
    /**
    * Destructor.
    */       
    IMPORT_C virtual ~CRCSEAudioCodecRegistry();
    
public: // New functions

    /**
    * Adds new audio codec entry.
    *
    * @since S60 v3.0
    * @param aCodecEntry New entry.
    * @return Codec identifier of the added audio codec entry.
    */

    IMPORT_C TUint32 AddL(const CRCSEAudioCodecEntry& aCodecEntry );
               
    /**
    * Gets all codec identifiers which are stored to RCSE. If there are
    * no codecs stored, aAllIds will contain zero entries.
    *
    * @since S60 v3.0
    * @param aAllIds Array, where the codec identifiers are stored.
    */
    IMPORT_C void GetAllCodecIdsL( RArray<TUint32>& aAllIds );
    
    /**
    * Search a Audio Codec entry, which id is same as aCodecId. 
    * Leaves with KErrNotFound if such profile is not found.
    * Default values are reseted to aFoundEntry before the search.
    * the new values are set to it.
    *
    * @since S60 v3.0
    * @param aCodecId Search criteria for Audio Codec.
    * @param aFoundEntry A found audio codec entry.
    */
    IMPORT_C void FindL( TUint32 aCodecId, 
        CRCSEAudioCodecEntry& aFoundEntry );
        
    /**
    * Deletes a codec, which codec id is same than aCodecId. 
    *
    * @since S60 v3.0
    * @param aCodecId Audio Codec identifier
    */
    IMPORT_C void DeleteL( TUint32 aCodecId );
    
    /**
    * Updates codec, which codec identifier is same than aCodecId.
    * Only settings that are set in aUpdateData are updated in codec.
    * Leaves with KErrNotFound if codec is not found.
    *
    * @since S60 v3.0
    * @param aCodecId codec identifier.
    * @param aUpdateData Updated data, which are updated to codec.
    */
    IMPORT_C void UpdateL( TUint32 aCodecId, 
        const CRCSEAudioCodecEntry& aUpdateData );

    /**
    * Gets default codec. Default codec is a codec, which
    * can be used when no other codec is available. AFoundEntrys 
    * buffers and arrays are freed, before the new values are set to it.
    *
    * @since S60 v3.0
    * @param aDefaultCodec Default codec settings are set to this.
    */
    IMPORT_C void GetDefaultCodec( 
        CRCSEAudioCodecEntry& aDefaultCodec ) const;


    /**
    * Adds bunch of audio codec entries to store in one transaction.
    *
    * @since S60 v3.2
    * @param aEntries 
    */
    void AddL( RPointerArray<CRCSEAudioCodecEntry>& aEntries );

    /**
    * Delete bunch of audio codec entries from store in one transaction.
    *
    * @since S60 v3.2
    * @param aIds Codec IDs
    */
    void DeleteL( const RArray<TUint32>& aIds );

protected:  // New functions

    /**
    * C++ default constructor.
    */
    CRCSEAudioCodecRegistry();

private:

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
    * @leave KErrNotFound if audio codec doesn't exist.
    */
    void FindEntryL( TUint32 aProfileId, CRCSEAudioCodecEntry& aFoundEntry );

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
    void AddOrUpdateEntryL( TUint32& aId,
                            const CRCSEAudioCodecEntry& aEntry );


    /**
    * Convert xxx
    * .
    *
    * @since S60 v3.2.
    * @param aEntry
    * @param aProperties
    */
    void ConvertPropertiesToEntryL( CRCSEAudioCodecEntry& aEntry,
                                    RIpAppPropArray& aProperties );


    /**
    * Convert xxx
    * .
    *
    * @since S60 v3.2.
    * @param aEntry
    * @param aProperties
    */
    void ConvertEntryToPropertiesL( const CRCSEAudioCodecEntry& aEntry,
                                    RIpAppPropArray& properties );


    // For testing
    #ifdef TEST_EUNIT
        friend class UT_CRCSEAudioCodecRegistry;
    #endif                                  

    };

#endif      // CRCSEAUDIOCODECREGISTRY_H   
            
// End of File
