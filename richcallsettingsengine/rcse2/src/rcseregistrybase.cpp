/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Base for RCSE registry classes
*
*/


//#include <cenrepdatabaseutil.h>
//#include <cenrepdatabaseproperty.h>


#include "rcseregistrybase.h"
#include "rcsebackupobserver.h"
#include "rcsedefaultdbnames.h"
#include "rcseconstants.h"
#include "rcsepanic.h"
#include "rcseprivatecrkeys.h"
#include "rcselogger.h"
#include "rcsedbimporter.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CRCSERegistryBase::CRCSERegistryBase( TRCSERegistryType aType )
    : iType( aType )
    {
    }

// ---------------------------------------------------------------------------
// BaseConstructL
// ---------------------------------------------------------------------------
//
void CRCSERegistryBase::BaseConstructL()
    {
    RCSELOGSTRING2( "CRCSERegistryBase::BaseConstructL(type=%d) - IN", iType );
    RCSELOGTHREAD();

    // Set proper table and mask
    switch ( iType )
        {
        case EVoIPProfile:
            {
            iCenRepDb = CCenRepDatabaseUtil::NewL( KCRUidRCSE,
                                                   KRCSEProfileTable,
                                                   KRCSEColIncrement,
                                                   KRCSEColumnMask,
                                                   KRCSEIdCounter,
                                                   KRCSEColumnCountVoIP );
            break;
            }
        case EAudioCodecSetting:
            {
            iCenRepDb = CCenRepDatabaseUtil::NewL( KCRUidRCSECodec,
                                                   KRCSECodecTable,
                                                   KRCSEColIncrement,
                                                   KRCSEColumnMask,
                                                   KRCSEIdCounter,
                                                   KRCSEColumnCountCodec );

            break;
            }
        default:
            {
            User::Leave( KErrNotSupported );
            }
        };

    iBackupObserver = CRCSEBackupObserver::NewL( *this );

    TInt err( 0 );

    // Check if Backup operation is running.
    if ( iBackupObserver->IsBackupOperationRunning() )
        {
        RCSELOGSTRING( "Backup active!" );
        iBackupActive = ETrue;
        }

    if ( !iBackupActive &&
         CRCSEDbImporter::ImportExists() &&
         !CRCSEDbImporter::IsImportRunning() )
        {
        TRAP( err, ImportSettingsL() );
        }

    RCSELOGSTRING( "CRCSERegistryBase::BaseConstructL() - OUT" );
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CRCSERegistryBase::~CRCSERegistryBase()
    {
    RCSELOGSTRING2( 
        "CRCSERegistryBase::~CRCSERegistryBase(type=%d) - IN", iType );
    RCSELOGTHREAD();

    if ( iBackupObserver )
        {
        iBackupObserver->Cancel();
        delete iBackupObserver;
        iBackupObserver = NULL;
        }


    delete iCenRepDb;

    RCSELOGSTRING( "CRCSERegistryBase::~CRCSERegistryBase() - OUT" );
    }

// ---------------------------------------------------------------------------
// From class MRCSEBackupObserver.
// Handle backup events.
// ---------------------------------------------------------------------------
//
void CRCSERegistryBase::HandleBackupOperationEventL()
	{
	RCSELOGSTRING( "CRCSERegistryBase::HandleBackupOperationEventL() - IN" );
    RCSELOGTHREAD();

	if ( iBackupObserver->IsBackupOperationRunning() )
        {
        RCSELOGSTRING( "Backup active!" );
        iBackupActive = ETrue;
        }
    else
        {
        RCSELOGSTRING( "Backup not active!" );
        iBackupActive = EFalse;
		}

    RCSELOGSTRING( "CRCSERegistryBase::HandleBackupOperationEventL() - OUT" );
	}

// -----------------------------------------------------------------------------
// CRCSERegistryBase::BeginL
// Prepares read/write operation.
// -----------------------------------------------------------------------------
//
void CRCSERegistryBase::BeginL()
    {
    RCSELOGSTRING( "CRCSERegistryBase::BeginL() - IN" );
    RCSELOGTHREAD();

    if ( iBackupActive )
        {
        User::Leave( KErrNotReady );
        }

    else 
        {
        RCSELOGSTRING( "Begin transaction" );

        // Begin transaction. Method leaves 2 items in cleanup stack
        // and they will be removed in CommitTransaction
        iCenRepDb->BeginTransactionL(); 
        }

    RCSELOGSTRING( "CRCSERegistryBase::BeginL() - OUT" );
    }

// -----------------------------------------------------------------------------
// CRCSERegistryBase::EndL
// Finalize performed operation
// -----------------------------------------------------------------------------
//
void CRCSERegistryBase::EndL()
    {
    RCSELOGSTRING( "CRCSERegistryBase::EndL() - IN" );
    RCSELOGTHREAD();

    User::LeaveIfError( iCenRepDb->CommitTransaction() );

    RCSELOGSTRING( "CRCSERegistryBase::EndL() - OUT" );
    }

// -----------------------------------------------------------------------------
// CRCSERegistryBase::ToDes
// Converts TInt32 to descriptor.
// -----------------------------------------------------------------------------
//
TDesC& CRCSERegistryBase::ToDes( TInt32 aFrom, TDes& aTo )
    {
    aTo.Zero();
    aTo.AppendNum( aFrom );
    return aTo;
    }

// -----------------------------------------------------------------------------
// CRCSERegistryBase::ToDes
// Converts TUint32 to descriptor.
// -----------------------------------------------------------------------------
//
TDesC& CRCSERegistryBase::ToDes( TUint32 aFrom, TDes& aTo )
    {
    aTo.Zero();
    aTo.AppendNum( aFrom );
    return aTo;
    }

// -----------------------------------------------------------------------------
// CRCSERegistryBase::ToDes
// Converts descriptor to TInt32.
// -----------------------------------------------------------------------------
//
TInt32 CRCSERegistryBase::ToTInt32L( const TDesC& aFrom )
    {
    TInt32 value;
    TLex16 convert( aFrom );
    User::LeaveIfError( convert.Val( value ) );
    return value;
    }

// -----------------------------------------------------------------------------
// CRCSERegistryBase::ToDes
// Converts descriptor to TUint32.
// -----------------------------------------------------------------------------
//
TUint32 CRCSERegistryBase::ToTUint32L( const TDesC& aFrom)
    {
    TUint32 value;
    TLex16 convert( aFrom );
    User::LeaveIfError( convert.Val( value, EDecimal ) );
    return value;
    }

// -----------------------------------------------------------------------------
// CRCSERegistryBase::ImportSettingsL
// Import settings from db files and store to cenrep.
// -----------------------------------------------------------------------------
//
void CRCSERegistryBase::ImportSettingsL()
    {
    RCSELOGSTRING( "CRCSERegistryBase::ImportSettingsL() - IN" );
    RCSELOGTHREAD();

    CRCSEDbImporter* importer = CRCSEDbImporter::NewLC();
    importer->ImportAndStoreL();
    CleanupStack::PopAndDestroy( importer );

    RCSELOGSTRING( "CRCSERegistryBase::ImportSettingsL() - OUT" );
    }

// -----------------------------------------------------------------------------
// CRCSERegistryBase::CreatePropertyL
// Create a new property and appends it to given property array
// -----------------------------------------------------------------------------
//
void CRCSERegistryBase::CreatePropertyL( 
    TUint aPropertyName, 
    const TDesC& aPropertyValue, 
    RIpAppPropArray& aPropertyArray )
    {
    CCenRepDatabaseProperty* property = CCenRepDatabaseProperty::NewLC();
    property->SetName( aPropertyName );
    User::LeaveIfError( property->SetValue( aPropertyValue ) );
    aPropertyArray.AppendL( property );
    CleanupStack::Pop( property );
    }

// -----------------------------------------------------------------------------
// CRCSERegistryBase::GetPropertyValueL
// Gets value of requested property from array
// -----------------------------------------------------------------------------
//
void CRCSERegistryBase::GetPropertyValueL( 
    TUint aPropertyName, 
    TDes& aPropertyValue, 
    RIpAppPropArray& aPropertyArray )
    {
    aPropertyValue.Zero();

    const TInt count( aPropertyArray.Count() );
    for ( TInt i( 0 ); i < count; i++ )
        {
        if ( aPropertyArray[i]->GetName() == aPropertyName )
            {
            aPropertyValue = aPropertyArray[i]->GetDesValue();
            //delete aPropertyArray[i];
            //aPropertyArray.Remove( i );
            i = count; // Stop the loop
            }
        }
    }

// -----------------------------------------------------------------------------
// CRCSERegistryBase::FindIdsByValueL
// Search entries that have this property and requested value.
// -----------------------------------------------------------------------------
//
void CRCSERegistryBase::FindIdsByValueL( 
    TUint aPropertyName, 
    const TDesC& aPropertyValue, 
    RArray<TUint32>& aIdArray )
    {    
    RArray<TInt> allEntries;
    CleanupClosePushL( allEntries );

    iCenRepDb->FindEntryIdsL( allEntries );
    
    CCenRepDatabaseProperty* property = CCenRepDatabaseProperty::NewLC();
    
    const TInt allCount( allEntries.Count() );
    
    for ( TInt i( 0 ); i < allCount; i++ )
        {        
        TInt err = iCenRepDb->FindPropertyL( allEntries[i], aPropertyName,
                *property );
        
        if ( KErrNoMemory == err )
            {
            User::Leave( err );
            }
        
        if ( KErrNone == err )
            {            
            // If properties match, add id to the id array.
            if ( !property->GetDesValue().Compare( aPropertyValue ) )
                {                
                aIdArray.AppendL( allEntries[i] );
                }
            }
        }
    
    CleanupStack::PopAndDestroy( property );
    CleanupStack::PopAndDestroy( &allEntries );
    }

// -----------------------------------------------------------------------------
// CRCSERegistryBase::FindAllIdsL
// Search all ids of entries from cenrep db.
// -----------------------------------------------------------------------------
//
void CRCSERegistryBase::FindAllIdsL( RArray<TUint32>& aIdArray )
    {
    CleanupClosePushL( aIdArray );
    aIdArray.Reset();

    RArray<TInt> ids;
    CleanupClosePushL( ids );

    iCenRepDb->FindEntryIdsL( ids );

    // TInt to TUint
    const TInt count( ids.Count() );
    for ( TInt i( 0 ); i < count; i++ )
        {
        aIdArray.AppendL( ids[ i ] );
        }

    CleanupStack::PopAndDestroy( &ids );
    CleanupStack::Pop( &aIdArray );
    }

// -----------------------------------------------------------------------------
// CRCSERegistryBase::CleanupDbPropArray
// Custom cleanup for cenrep db property array.
// -----------------------------------------------------------------------------
//
void CRCSERegistryBase::CleanupDbPropArray( TAny* aPointer )
	{
	RIpAppPropArray* array = static_cast<RIpAppPropArray*>( aPointer );
	array->ResetAndDestroy();
	}


