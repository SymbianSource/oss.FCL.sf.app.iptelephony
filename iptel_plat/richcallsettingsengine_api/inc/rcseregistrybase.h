/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Base class for RCSE registry classes
*
*/


#ifndef CRCSEREGISTRYBASE_H
#define CRCSEREGISTRYBASE_H

//#include <centralrepository.h>
#include <e32base.h>
#include <d32dbms.h>
#include <mrcsebackupobserver.h>

#include <cenrepdatabaseutil.h>
const TInt KMaxNameLength = 100;


class CRCSEBackupObserver;

/**
 *  Base class for RCSE registry classes
 *
 *  Encapsules settings repository connections and handles backup events.
 *
 *  @lib rcse.lib
 *  @since S60 v3.0
 */
class CRCSERegistryBase : public CBase, public MRCSEBackupObserver
    {
protected:

    /**  Registry type */
    enum TRCSERegistryType
        {
        EUnknown,
        EVoIPProfile,
        ESIPSetting,        // Not supported
        EAudioCodecSetting,
        ESCCPProfile        // Not supported
        };

    /**  Item types in settings table */
    enum TItemType
        {
        EItemUnknown    = 0,            // Unknown item
        EItemDeleted    = 1,            // Row (item) deleted
        EItemVoIPEntry  = 2,            // VoIP profile entry
        EItemCodecEntry = 3,            // Audio codec entry
        EItemAttribute  = 4,            // Attribute of entry
        };


public:

    virtual ~CRCSERegistryBase();

    /**
     * From MRCSEBackupObserver.
     * Handle backup observer event.
     *
     * @since S60 v3.0
     */
    void HandleBackupOperationEventL();

protected:

    CRCSERegistryBase( TRCSERegistryType aType );

    void BaseConstructL();

    /**
    * Prepares operation. Checks that there is not backup/restore
    * opearation ongoing and begins transaction.
    *    
    * @since S60 v3.2
    * @leave KErrNotReady if there is backuoperation ongoing.
    * @leave KErrLocked if there is write operation already ongoing.
    */
    void BeginL();

    /**
    * Ends user operations. Commits transaction.
    *
    * @since S60 v3.2
    * @param aType Type of performed operation
    */
    void EndL();

    /**
    * Helper method that converts TInt32 type to descriptor.
    *
    * @since S60 v3.2
    * @param aFrom TInt32 value to be converted.
    * @param aTo Descriptor containing the converted value.
    * @return Reference to descriptor containing the converted value.
    */
    TDesC& ToDes( TInt32 aFrom, TDes& aTo );

    /**
    * Helper method that converts TUint32 type to descriptor.
    *
    * @since S60 v3.2
    * @param aFrom TUint32 value to be converted.
    * @param aTo Descriptor containing the converted value.
    * @return Reference to descriptor containing the converted value.
    */
    TDesC& ToDes( TUint32 aFrom, TDes& aTo );

    /**
    * Helper method that converts descriptor type to TInt32.
    *
    * @since S60 v3.2
    * @param aFrom Descriptor to be converted.
    * @return Converted TInt32 value.
    */
    TInt32 ToTInt32L( const TDesC& aFrom );

    /**
    * Helper method that converts descriptor type to TUint32.
    *
    * @since S60 v3.2
    * @param aFrom Descriptor to be converted.
    * @return Converted TUint32 value.
    */
    TUint32 ToTUint32L( const TDesC& aFrom );

    /**
    * Imports rcse settings from restored database file 
    *
    * @since S60 v3.2
    */
    void ImportSettingsL();

    /**
    * Create CenRep db property to given property array.
    *
    * @since S60 v3.2
    * @param aPropertyName Name of created property.
    * @param aPropertyValue Value of created property.
    * @param aPropertyArray Container of properties.
    */
    void CreatePropertyL( TUint aPropertyName,
                          const TDesC& aPropertyValue,
                          RIpAppPropArray& aPropertyArray );

    /**
    * Find value of property from given property array.
    *
    * @since S60 v3.2
    * @param aPropertyName Name of requested property.
    * @param aPropertyValue Value of requested property.
    * @param aPropertyArray Container of properties.
    */
    void GetPropertyValueL( TUint aPropertyName,
                            TDes& aPropertyValue,
                            RIpAppPropArray& aPropertyArray );

    /**
    * Create CenRep db property to given property array.
    *
    * @since S60 v3.2
    * @param aPropertyName Searh criteria.
    * @param aPropertyValue Searh criteria.
    * @param aIdArray Contains all found entry IDs.
    */
    void FindIdsByValueL( TUint aPropertyName,
                          const TDesC& aPropertyValue,
                          RArray<TUint32>& aIdArray );

    /**
    * Search all entry IDs from CenRep database.
    *
    * @since S60 v3.2
    * @param aIdArray Contains all found entry IDs.
    */
    void FindAllIdsL( RArray<TUint32>& aIdArray );

    /**
     * Custom cleanup for cenrep db property array.
     *
     * @since S60 v3.2
     * @param aPointer Array pointer
     */
    static void CleanupDbPropArray( TAny* aPointer );


protected: // data

    /**
     * Registry type of this object
     */
    TRCSERegistryType iType;

    /**
     * Backup observer
     * Own.
     */
    CRCSEBackupObserver* iBackupObserver;

    /**
     * Backup flag
     */
    TBool iBackupActive;

    /**
     * Interface to RCSE's settings storage
     * Own.
     */
    CCenRepDatabaseUtil* iCenRepDb;


    // For testing
    #ifdef TEST_EUNIT
        friend class UT_CRCSERegistryBase;
    #endif                                  

    };

#endif // CRCSEREGISTRYBASE_H
