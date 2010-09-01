/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "rcsedbimporter.h"
#include "rcseconstants.h"
#include "crcseprofileregistry.h"
#include "crcseaudiocodecregistry.h"
#include "rcsedefaultdbnames.h"
#include "rcselogger.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CRCSEDbImporter::CRCSEDbImporter()
    {
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CRCSEDbImporter::ConstructL()
    {
    RCSELOGSTRING( "CRCSEDbImporter::ConstructL() - IN" );
    User::LeaveIfError( iDbSession.Connect() );
    RCSELOGSTRING( "CRCSEDbImporter::ConstructL() - OUT" );
    }


// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CRCSEDbImporter* CRCSEDbImporter::NewL()
    {
    CRCSEDbImporter* self = CRCSEDbImporter::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
CRCSEDbImporter* CRCSEDbImporter::NewLC()
    {
    CRCSEDbImporter* self = new( ELeave ) CRCSEDbImporter;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CRCSEDbImporter::~CRCSEDbImporter()
    {
    ReleaseImportLock();

    iDb.Close();
    iDbSession.Close();
    iProfiles.ResetAndDestroy();
    iCodecs.ResetAndDestroy();
    iCodecIds.Close();
    }

// ---------------------------------------------------------------------------
// Check if imported (restored) database file exists.
// ---------------------------------------------------------------------------
//
TBool CRCSEDbImporter::ImportExists()
    {
    RCSELOGSTRING( "CRCSEDbImporter::ImportExists() - IN" );

    TBool profileImport( EFalse );
    TBool codecImport( EFalse );

    RDbs dbSession;
    RDbNamedDatabase db;

    TInt err( dbSession.Connect() );

    if ( KErrNone != err )
        {
        return EFalse;
        }
    else
        {
        // Check voip profiles
        err = db.Open( dbSession, KDbName, KDatabaseUID );
        db.Close();

        if ( KErrNone == err )
            {
            profileImport = ETrue;
            }

        // Check audio codecs
        err = db.Open( dbSession, KDbCodecName, KDatabaseUID );
        db.Close();

        if ( KErrNone == err )
            {
            codecImport = ETrue;
            }
        }

    dbSession.Close();

    RCSELOGSTRING2( "CRCSEDbImporter::ImportExists(%d) - OUT", 
        profileImport && codecImport );

    // Return ETrue only when both of imports exist
    return profileImport && codecImport;
    }


// ---------------------------------------------------------------------------
// Check if import is already running.
// ---------------------------------------------------------------------------
//
TBool CRCSEDbImporter::IsImportRunning()
    {
    RCSELOGSTRING( "CRCSEDbImporter::IsImportRunning() - IN" );

    TBool ret( EFalse );

    RDbs dbSession;
    RDbNamedDatabase db;

    TInt err( dbSession.Connect() );

    if ( KErrNone != err )
        {
        ret = EFalse;
        }
    else
        {
        // Check if temporary table exists
        err = db.Open( dbSession, KDbTempName, KDatabaseUID );
        db.Close();

        if ( KErrNone == err )
            {
            ret = ETrue;
            }
        }

    dbSession.Close();

    RCSELOGSTRING2( "CRCSEDbImporter::IsImportRunning(%d) - OUT", ret );
    
    return ret;
    }


// ---------------------------------------------------------------------------
// Imports RCSE entries from db file and stores them to repository.
// ---------------------------------------------------------------------------
//
void CRCSEDbImporter::ImportAndStoreL()
    {
    RCSELOGSTRING( "CRCSEDbImporter::ImportL() - IN" );

    SetImportLock();

    // Import settings data
    ImportVoIPProfilesL();
    ImportAudioCodecsL();

    // Store audio codecs
    StoreAudioCodecsL();

    // Update and store VoIP profiles
    UpdatePreferredCodecs();
    StoreVoIPProfilesL();

    DeleteImports();
    ReleaseImportLock();

    RCSELOGSTRING( "CRCSEDbImporter::ImportL() - OUT" );
    }


// ---------------------------------------------------------------------------
// Set locking data base file to prevent disturb during the import operation.
// ---------------------------------------------------------------------------
//
void CRCSEDbImporter::SetImportLock()
    {
    RCSELOGSTRING( "CRCSEDbImporter::SetImportLock() - IN" );

    // Create temporary db that indicates ongoing operation
    iDb.Create( iDbSession, KDbTempName, KDatabaseUID );
    iDb.Close();

    RCSELOGSTRING( "CRCSEDbImporter::SetImportLock() - OUT" );
    }


// ---------------------------------------------------------------------------
// Release locking data base file.
// ---------------------------------------------------------------------------
//
void CRCSEDbImporter::ReleaseImportLock()
    {
    RCSELOGSTRING( "CRCSEDbImporter::ReleaseImportLock() - IN" );

    // Delete temporary locking db
    TInt err( iDb.Open( iDbSession, KDbTempName, KDatabaseUID ) );

    if ( KErrNone == err )
        {
        iDb.Destroy();
        iDb.Close();
        RCSELOGSTRING( "Temporary db deleted" );
        }
    else
        {
        RCSELOGSTRING2( "Temporary db not deleted, err=%d", err );
        }

    RCSELOGSTRING( "CRCSEDbImporter::ReleaseImportLock() - OUT" );
    }


// ---------------------------------------------------------------------------
// Import VoIP profiles from database to array.
// ---------------------------------------------------------------------------
//
void CRCSEDbImporter::ImportVoIPProfilesL()
    {
    RCSELOGSTRING( "CRCSEDbImporter::ImportVoIPProfilesL() - IN" );
    User::LeaveIfError( iDb.Open( iDbSession, KDbName, KDatabaseUID ) );

    // Create sql statement
    HBufC* statement = HBufC::NewLC(
        KSQLSelectAllStatement().Length() + 
        KProfileTableName().Length() );

    statement->Des().Format( 
        KSQLSelectAllStatement, &KProfileTableName );

    // Launch query
    RDbView view;
    User::LeaveIfError( view.Prepare( iDb, *statement ) );
    CleanupClosePushL( view );
    User::LeaveIfError( view.EvaluateAll() );
    view.FirstL();

    // Browse all result rows trough
    while ( view.AtRow() )
        {
        view.GetL(); // Get the data row
        const TInt cols( view.ColCount() + 1 );

        CRCSEProfileEntry* entry = CRCSEProfileEntry::NewLC();

        // Map the data from current row to profile entry.
        for( TInt i = 1; i < cols; i++ )
            {
            MapColumnToEntryL( i, view, *entry );
            }

        iProfiles.AppendL( entry );
        CleanupStack::Pop( entry );
        view.NextL();
        }

    CleanupStack::PopAndDestroy( &view );
    CleanupStack::PopAndDestroy( statement );

    iDb.Close();
    RCSELOGSTRING( "CRCSEDbImporter::ImportVoIPProfilesL() - OUT" );
    }

// ---------------------------------------------------------------------------
// Import audio codecs from database to array.
// ---------------------------------------------------------------------------
//
void CRCSEDbImporter::ImportAudioCodecsL()
    {
    RCSELOGSTRING( "CRCSEDbImporter::ImportAudioCodecsL() - IN" );

    User::LeaveIfError( iDb.Open( iDbSession, KDbCodecName, KDatabaseUID ) );

    // Create sql statement
    HBufC* statement = HBufC::NewLC(
        KSQLSelectAllStatement().Length() + 
        KAudioCodecTableName().Length() );

    statement->Des().Format( 
        KSQLSelectAllStatement, &KAudioCodecTableName );

    // Launch query
    RDbView view;
    User::LeaveIfError( view.Prepare( iDb, *statement ) );
    CleanupClosePushL( view );
    User::LeaveIfError( view.EvaluateAll() );
    view.FirstL();

    // Browse all result rows trough
    while ( view.AtRow() )
        {
        view.GetL(); // Get the data row
        const TInt cols( view.ColCount() + 1 );

        CRCSEAudioCodecEntry* entry = CRCSEAudioCodecEntry::NewLC();

        // Map the data from current row to audio codec entry.
        for( TInt i = 1; i < cols; i++ )
            {
            MapColumnToEntryL( i, view, *entry );
            }

        // Append entry and its ID
        iCodecs.AppendL( entry );

        TRCSEIdPair idPair;
        idPair.iOldId =  entry->iCodecId;
        iCodecIds.Append( idPair );

        CleanupStack::Pop( entry );
        view.NextL();
        }

    CleanupStack::PopAndDestroy( &view );
    CleanupStack::PopAndDestroy( statement );

    iDb.Close();

    RCSELOGSTRING( "CRCSEDbImporter::ImportAudioCodecsL() - OUT" );
    }


// ---------------------------------------------------------------------------
// Delete imported (restored) rcse database files.
// ---------------------------------------------------------------------------
//
void CRCSEDbImporter::DeleteImports()
    {
    RCSELOGSTRING( "CRCSEDbImporter::DeleteImports() - IN" );

    TInt err( 0 );

    // Delete VoIP profile import
    err = iDb.Open( iDbSession, KDbName, KDatabaseUID );

    if ( KErrNone == err )
        {
        iDb.Destroy();
        iDb.Close();
        RCSELOGSTRING( "Profile import deleted" );
        }
    else
        {
        RCSELOGSTRING2( "Profile import not deleted, err=%d", err );
        }

    // Delete audio codec import
    err = iDb.Open( iDbSession, KDbCodecName, KDatabaseUID );

    if ( KErrNone == err )
        {
        iDb.Destroy();
        iDb.Close();
        RCSELOGSTRING( "Codec import deleted" );
        }
    else
        {
        RCSELOGSTRING2( "Codec import not deleted, err=%d", err );
        }

    RCSELOGSTRING( "CRCSEDbImporter::DeleteImports() - OUT" );
    }


// ---------------------------------------------------------------------------
// Map query result from a given column to profile entry.
// -----------------------------------------------------------------------------
//
void CRCSEDbImporter::MapColumnToEntryL( 
    TInt aColumnIndex, 
    const RDbView& aView, 
    CRCSEProfileEntry& aEntry )
    {
    // Get the column definition
    const TDbCol col( aView.ColDef( aColumnIndex ) );

    // Maps the values using the names of columns
    if ( col.iName == KProfileId )
        {
        aEntry.iId = aView.ColUint32( aColumnIndex );
        }   
    else if ( col.iName == KProtocolIds )
        {        
        const TDesC& ids = aView.ColDes( aColumnIndex );
        // Extracts the numbers from ids to iIds.
        GetProfileIdsFromDescriptorL( ids, aEntry.iIds );  
        }
    else if ( col.iName == KProviderName )
        {
        aEntry.iProviderName.Copy( aView.ColDes( aColumnIndex ) );
        }
    else if ( col.iName == KSettingsName )
        {
        aEntry.iSettingsName.Copy( aView.ColDes( aColumnIndex ) );
        }
    else if ( col.iName == KAutoComplete )
        {
        aEntry.iAutoComplete = static_cast<CRCSEProfileEntry::TOnOff>(
            aView.ColInt32( aColumnIndex ) );
        }
    else if ( col.iName == KEndMediaPort )
        {
        aEntry.iEndMediaPort = aView.ColInt32( aColumnIndex );
        }
    else if ( col.iName == KHoldRingBack )
        {
        aEntry.iHoldRingBack = aView.ColInt32( aColumnIndex );
        }
    else if ( col.iName == KInbandDtmf)
        {
        aEntry.iInbandDTMF = static_cast<CRCSEProfileEntry::TOnOff>( 
            aView.ColInt32( aColumnIndex ) );
        }
    else if ( col.iName == KMediaQOS )
        {
        aEntry.iMediaQOS = aView.ColInt32( aColumnIndex );
        }
    else if ( col.iName == KOutbandDtmf )
        {
        aEntry.iOutbandDTMF = static_cast<CRCSEProfileEntry::TOnOff>
            ( aView.ColInt32( aColumnIndex ) );
        }
    else if ( col.iName == KPreferredCodecs )
        {
        const TDesC& codecs = aView.ColDes( aColumnIndex );
        // Extracts the numbers from codecs to iPreferredCodecs.
        GetNumbersFromDescriptorL( codecs, aEntry.iPreferredCodecs );
        }
    else if ( col.iName == KSiqnalingQOS )
        {
        aEntry.iSiqnalingQOS = aView.ColInt32( aColumnIndex );
        }
    else if ( col.iName == KStartMediaPort )
        {
        aEntry.iStartMediaPort = aView.ColInt32( aColumnIndex );
        }
    else if ( col.iName == KCFNoAnswer )
        {
        aEntry.iCFNoAnswer = static_cast<CRCSEProfileEntry::TOnOff>
                ( aView.ColInt32( aColumnIndex ) );
        }
    else if ( col.iName == KCFBusy )
        {
        aEntry.iCFBusy = static_cast<CRCSEProfileEntry::TOnOff>
                ( aView.ColInt32( aColumnIndex ) );
        }
    else if ( col.iName == KCFUnconditional )
        {
        aEntry.iCFUnconditional = static_cast<CRCSEProfileEntry::TOnOff>(
            aView.ColInt32( aColumnIndex ) );
        }
    else if ( col.iName == KRedundancy )
        {
        aEntry.iRedundancy = static_cast<CRCSEProfileEntry::TOnOff>(
            aView.ColInt32( aColumnIndex ) );
        }
    else if ( col.iName == KSecureCallPreference )
        {
        aEntry.iSecureCallPreference = aView.ColUint32( aColumnIndex );
        }
    else if ( col.iName == KVoIPProfileLock )
        {
        aEntry.iVoIPProfileLock = aView.ColUint32( aColumnIndex );
        }
    else if ( col.iName == KAdhocAllowed )
        {
        aEntry.iAdhocAllowed = aView.ColUint32( aColumnIndex );
        }
    else if ( col.iName == KSIPServerType )
        {
        aEntry.iSIPServerType.Copy( aView.ColDes( aColumnIndex ) );
        }
    else if ( col.iName == KSBCType )
        {
        aEntry.iSBCType.Copy( aView.ColDes( aColumnIndex ) );
        }
    else if ( col.iName == KSTUNServerType )
        {
        aEntry.iSTUNServerType.Copy( aView.ColDes( aColumnIndex ) );
        }
    else if ( col.iName == KWLANAPType )
        {
        aEntry.iWLANAPType.Copy( aView.ColDes( aColumnIndex ) );
        }
    else if ( col.iName == KPSTNGatewayType )
        {
        aEntry.iPSTNGatewayType.Copy( aView.ColDes( aColumnIndex ) );
        }
    else if ( col.iName == KSecurityGatewayType )
        {
        aEntry.iSecurityGatewayType.Copy( aView.ColDes( aColumnIndex ) );
        }
    else if ( col.iName == KRTCP )
        {
        aEntry.iRTCP = aView.ColUint32( aColumnIndex );
        }
    else if ( col.iName == KSIPVoIPUAHTerminalType )
        {
        aEntry.iSIPVoIPUAHTerminalType = aView.ColUint32( aColumnIndex );
        }
    else if ( col.iName == KSIPVoIPUAHeaderWLANMAC )
        {
        aEntry.iSIPVoIPUAHeaderWLANMAC = aView.ColUint32( aColumnIndex );
        }
    else if ( col.iName == KSIPVoIPUAHeaderString )
        {
        aEntry.iSIPVoIPUAHeaderString.Copy( aView.ColDes( aColumnIndex ) );
        }
    else if ( col.iName == KProfileLockedToIAP )
        {
        aEntry.iProfileLockedToIAP = static_cast<CRCSEProfileEntry::TOnOff>(
            aView.ColInt32( aColumnIndex ) );
        }
    else if ( col.iName == KVoIPPluginUID )
        {
        aEntry.iVoIPPluginUID = aView.ColInt32( aColumnIndex );
        }
    else if ( col.iName == KAllowVoIPoverWCDMA )
        {
        aEntry.iAllowVoIPoverWCDMA = static_cast<CRCSEProfileEntry::TOnOff>(
            aView.ColInt32( aColumnIndex ) );
        }
    else if ( col.iName == KAllowVoIPoverBT )
        {
        aEntry.iAllowVoIPoverBT = static_cast<CRCSEProfileEntry::TOnOff>(
            aView.ColInt32( aColumnIndex ) );
        }
    else if ( col.iName == KMeanCountOfVoIPDigits )
        {
        aEntry.iMeanCountOfVoIPDigits = aView.ColInt32( aColumnIndex );
        }
    else if ( col.iName == KIgnoreAddrDomainPart )
        {
        aEntry.iIgnoreAddrDomainPart = aView.ColInt32( aColumnIndex );
        }

    else if ( col.iName == KHandoverDialect )
        {
        aEntry.iHandoverDialect = aView.ColInt32( aColumnIndex );
        }
    else if ( col.iName == KPSTelephonyHOPreference )
        {
        aEntry.iPSTelephonyHOPreference = aView.ColInt32( aColumnIndex );
        }
    else if ( col.iName == KHOThresholdValueLL )
        {
        aEntry.iHOThresholdValueLL = aView.ColInt32( aColumnIndex );
        }
    else if ( col.iName == KHOThresholdValueHL )
        {
        aEntry.iHOThresholdValueHL = aView.ColInt32( aColumnIndex );
        }
    else if ( col.iName == KNumberOfMeasurementsAbove )
        {
        aEntry.iNumberOfMeasurementsAbove = aView.ColInt32( aColumnIndex );
        }
    else if ( col.iName == KNumberOfMeasurementsBelow )
        {
        aEntry.iNumberOfMeasurementsBelow = aView.ColInt32( aColumnIndex );
        }
    else if ( col.iName == KSmartScannInterParaHigh )
        {
        aEntry.iSmartScannInterParaHigh = aView.ColInt32( aColumnIndex );
        }
    else if ( col.iName == KSmartScannInterParaMedium )
        {
        aEntry.iSmartScannInterParaMedium = aView.ColInt32( aColumnIndex );
        }
    else if ( col.iName == KSmartScannInterParaLow )
        {
        aEntry.iSmartScannInterParaLow = aView.ColInt32( aColumnIndex );
        }
    else if ( col.iName == KSmartScannInterParaStatic )
        {
        aEntry.iSmartScannInterParaStatic = aView.ColInt32( aColumnIndex );
        }

    else if ( col.iName == KSmartScannDurationHighMode )
        {
        aEntry.iSmartScannDurationHighMode = aView.ColInt32( aColumnIndex );
        }
    else if ( col.iName == KSmartScannDurationMediumMode )
        {
        aEntry.iSmartScannDurationMediumMode = aView.ColInt32( aColumnIndex );
        }
    else if ( col.iName == KSmartScannDurationLowMode )
        {
        aEntry.iSmartScannDurationLowMode = aView.ColInt32( aColumnIndex );
        }
    else if ( col.iName == KHandoffNumber )
        {
        aEntry.iHandoffNumber = aView.ColInt32( aColumnIndex );
        }
    else if ( col.iName == KHandbackNumber )
        {
        aEntry.iHandbackNumber = aView.ColInt32( aColumnIndex );
        }
    else if ( col.iName == KHysterisisTimer )
        {
        aEntry.iHysterisisTimer = aView.ColInt32( aColumnIndex );
        }
    else if ( col.iName == KHandOffProcessTimer )
        {
        aEntry.iHandOffProcessTimer = aView.ColInt32( aColumnIndex );
        }
    else if ( col.iName == KDisconnectProcessTimer )
        {
        aEntry.iDisconnectProcessTimer = aView.ColInt32( aColumnIndex );
        }
    else if ( col.iName == KHandoffPrefix )
        {
        aEntry.iHandoffPrefix.Copy( aView.ColDes( aColumnIndex ) );
        }
    else if ( col.iName == KHandbackPrefix )
        {
        aEntry.iHandbackPrefix.Copy( aView.ColDes( aColumnIndex ) );
        }
    else if ( col.iName == KHandoverTones )
        {
        aEntry.iHandoverTones = aView.ColInt32( aColumnIndex );
        }
    else if ( col.iName == KSupportSMSoverWLAN )
        {
        aEntry.iSupportSMSoverWLAN = aView.ColInt32( aColumnIndex );
        }
    else if ( col.iName == KServiceProviderId )
        {
        aEntry.iServiceProviderId = aView.ColUint32( aColumnIndex );
        }
    else if ( col.iName == KUserPhoneUriParam )
        {
        aEntry.iUserPhoneUriParameter = static_cast<CRCSEProfileEntry::TOnOff>( 
            aView.ColInt32( aColumnIndex ) );
        }
    else if ( col.iName == KSIPConnTestAddress )
        {
        aEntry.iSIPConnTestAddress.Copy( aView.ColDes( aColumnIndex ) );
        }
    else if ( col.iName == KNATSettingsStorageId )
        {
        aEntry.iNATSettingsStorageId = aView.ColUint32( aColumnIndex );
        }
    else if ( col.iName == KSIPMinSE )
        {
        aEntry.iSIPMinSE = aView.ColInt32( aColumnIndex );
        }
    else if ( col.iName == KSIPSessionExpires )
        {
        aEntry.iSIPSessionExpires = aView.ColInt32( aColumnIndex );
        }
    else if ( col.iName == KNATProtocol )
        {
        aEntry.iNATProtocol = aView.ColInt32( aColumnIndex );
        }
    else if ( col.iName == KNewServiceTable )
        {
        aEntry.iNewServiceTableEntry = static_cast<CRCSEProfileEntry::TOnOff>(
            aView.ColInt32( aColumnIndex ) );
        }
    else if ( col.iName == KSNAPId )
        {
        aEntry.iSNAPId = aView.ColUint32( aColumnIndex );
        }
    else
        {
        // Skip unknown column.
        RCSELOGSTRING2( 
            "CRCSEProfileRegistry::MapColumnToEntryL() - unknown %d",
                 aColumnIndex );
        }

    }


// -----------------------------------------------------------------------------
// Map query result from a given column to audio codec entry.
// -----------------------------------------------------------------------------
//
void CRCSEDbImporter::MapColumnToEntryL( 
    TInt aColumnIndex, 
    const RDbView& aView, 
    CRCSEAudioCodecEntry& aCodecEntry )
    {    
    // Get the column definition
    const TDbCol col( aView.ColDef( aColumnIndex ) );

    // Maps the values using the names of columns
    if ( col.iName == KAudioCodecId )
        {
        aCodecEntry.iCodecId = aView.ColUint32( aColumnIndex );
        }
    else if ( col.iName == KMediaTypeName )
        {
        aCodecEntry.iMediaTypeName.Copy( aView.ColDes( aColumnIndex ) );
        }
    else if ( col.iName == KMediaSubTypeName )
        {
        aCodecEntry.iMediaSubTypeName.Copy( aView.ColDes( aColumnIndex ) );
        }
    else if ( col.iName == KJitterBufferSize )
        {
        aCodecEntry.iJitterBufferSize = aView.ColInt32( aColumnIndex );
        }
    else if ( col.iName == KOctetAlign )
        {
        aCodecEntry.iOctetAlign = static_cast<CRCSEAudioCodecEntry::TOnOff>( 
        aView.ColInt32( aColumnIndex ) );
        }
    else if ( col.iName == KModeSet )
        {
        const TDesC& modeset = aView.ColDes( aColumnIndex );
        GetNumbersFromDescriptorL( modeset, aCodecEntry.iModeSet );
        }        
    else if ( col.iName == KModeChangePeriod )
        {
        aCodecEntry.iModeChangePeriod = aView.ColInt32( aColumnIndex );
        }
    else if ( col.iName == KModeChangeNeighbor )
        {
        aCodecEntry.iModeChangeNeighbor = static_cast<CRCSEAudioCodecEntry::TOnOff>( 
        aView.ColInt32( aColumnIndex ) );
        }
    else if ( col.iName == KPtime )
        {
        aCodecEntry.iPtime = aView.ColInt32( aColumnIndex );
        }
    else if ( col.iName == KMaxptime )
        {
        aCodecEntry.iMaxptime = aView.ColInt32( aColumnIndex );
        }
    else if ( col.iName == KCrc )
        {
        aCodecEntry.iCrc = static_cast<CRCSEAudioCodecEntry::TOnOff>( 
        aView.ColInt32( aColumnIndex ) );
        }
    else if ( col.iName == KRobustSorting )
        {
        aCodecEntry.iRobustSorting = static_cast<CRCSEAudioCodecEntry::TOnOff>( 
        aView.ColInt32( aColumnIndex ) );
        }    
    else if ( col.iName == KInterLeaving )
        {
        aCodecEntry.iInterLeaving = aView.ColInt32( aColumnIndex );
        }    
    else if ( col.iName == KChannels )
        {
        const TDesC& channels = aView.ColDes( aColumnIndex );
        GetNumbersFromDescriptorL( channels, aCodecEntry.iChannels );
        }                    
    else if ( col.iName == KVAD )
        {
        aCodecEntry.iVAD = static_cast<CRCSEAudioCodecEntry::TOnOff>( 
        aView.ColInt32( aColumnIndex ) );
        }
    else if ( col.iName == KDTX )
        {
        aCodecEntry.iDTX = static_cast<CRCSEAudioCodecEntry::TOnOff>( 
        aView.ColInt32( aColumnIndex ) );
        }
    else if ( col.iName == KSamplingRate )
        {
        aCodecEntry.iSamplingRate = aView.ColInt32( aColumnIndex );
        }
    else if ( col.iName == KAnnexb )
        {
        aCodecEntry.iAnnexb = static_cast<CRCSEAudioCodecEntry::TOnOff>(
            aView.ColInt32( aColumnIndex) );
        }
    else if ( col.iName == KModeChangeCapability )
        {
        aCodecEntry.iModeChangeCapability = aView.ColInt32( aColumnIndex );
        }
    else if ( col.iName == KMaxRed )
        {
        aCodecEntry.iMaxRed = aView.ColInt32( aColumnIndex );
        }
    else
        {
        // Skip unknown column.
        RCSELOGSTRING2( 
            "CRCSEAudioCodecRegistry::MapColumnToEntryL() - unknown %d",
                 aColumnIndex );
        }

    }

// -----------------------------------------------------------------------------
// Read TSettingIds structs from given descriptor to array.
// -----------------------------------------------------------------------------
//
void CRCSEDbImporter::GetProfileIdsFromDescriptorL(
    const TDesC& aNumbers,
    RArray<TSettingIds>& aArray )
    {
    CleanupClosePushL( aArray );
    
    TLex lex( aNumbers );
    
    TSettingIds value;
    
    // Reset original array
    aArray.Reset();
    
    while( !lex.Eos() )
        {
        lex.Val( value.iProfileType );
        // Skip characters to space char.
        lex.SkipCharacters();
        lex.Inc( 1 );
        lex.Val( value.iProfileId );
        lex.SkipCharacters();
        lex.Inc( 1 );
        lex.Val( value.iProfileSpecificSettingId );
        
        aArray.AppendL( value );
        if ( !lex.Eos() ) // Check that End of string is not next char.
            {
            // Go over the space character.
            lex.Inc( 1 );
            }
        } 
    
    CleanupStack::Pop( &aArray );
    }


// -----------------------------------------------------------------------------
// Read IDs from given descriptor to array.
// -----------------------------------------------------------------------------
//
template<class T>
void CRCSEDbImporter::GetNumbersFromDescriptorL(
    const TDesC& aNumbers,
    RArray<T>& aArray )
    {
    CleanupClosePushL( aArray );
    TLex lex( aNumbers );

    // Reset original array
    aArray.Reset();
    
    while( !lex.Eos() )
        {
        // Extract the number as unsigned int 32, which can be 
        // converted to value wanted (template)
        TUint32 value;
        lex.Val( value, EDecimal );
        aArray.AppendL( static_cast<T>( value ) );
        // Skip characters to space char.
        lex.SkipCharacters();
        if ( !lex.Eos() ) // Check that End of string is not next char.
            {
            // Go over the space character.
            lex.Inc( 1 );
            }
        }
    
    CleanupStack::Pop( &aArray );
    }


// -----------------------------------------------------------------------------
// Update new preferred audio codec IDs to imported VoIP profiles.
// -----------------------------------------------------------------------------
//
void CRCSEDbImporter::UpdatePreferredCodecs()
    {
    RCSELOGSTRING( "CRCSEDbImporter::UpdatePreferredCodecs() - IN" );

    // Update preferred codec IDs for each profile
    const TInt count( iProfiles.Count() );
    for( TInt i( 0 ); i < count; ++i )
        {
        // Browse preferred codecs array
        const TInt codecCount( iProfiles[i]->iPreferredCodecs.Count() );
        for ( TInt j( 0 ); j < codecCount; ++j )
            {
            // Search new codec ID from array
            TUint32 oldId = iProfiles[i]->iPreferredCodecs[j];
            TBool found( EFalse );

            const TInt idCount( iCodecIds.Count() );
            for ( TInt k( 0 ); k < idCount && !found; ++k )
                {
                found = ( oldId == iCodecIds[k].iOldId );

                // Set new ID if it was found
                if ( found )
                    {
                    iProfiles[i]->iPreferredCodecs[j] = iCodecIds[k].iNewId;
                    RCSELOGSTRING4( 
                        "Match %d. Codec index %d updated for %d. profile",
                            k, j, i + 1 );
                    }
                else
                    {
                    RCSELOGSTRING2( "Skip %d", k );
                    }
                }
            }
        }

    RCSELOGSTRING( "CRCSEDbImporter::UpdatePreferredCodecs() - OUT" );
    }



// -----------------------------------------------------------------------------
// Store imported voip profiles.
// -----------------------------------------------------------------------------
//
void CRCSEDbImporter::StoreVoIPProfilesL()
    {
    RCSELOGSTRING( "CRCSEDbImporter::StoreVoIPProfilesL() - IN" );

    CRCSEProfileRegistry* reg = CRCSEProfileRegistry::NewLC();
    reg->AddL( iProfiles );
    CleanupStack::PopAndDestroy( reg );

    RCSELOGSTRING( "CRCSEDbImporter::StoreVoIPProfilesL() - OUT" );
    }


// -----------------------------------------------------------------------------
// Store imported audio codecs.
// -----------------------------------------------------------------------------
//
void CRCSEDbImporter::StoreAudioCodecsL()
    {
    RCSELOGSTRING( "CRCSEDbImporter::StoreAudioCodecsL() - IN" );

    CRCSEAudioCodecRegistry* reg = CRCSEAudioCodecRegistry::NewLC();
    reg->AddL( iCodecs );
    CleanupStack::PopAndDestroy( reg );

    // Update codec ID pairs
    const TInt codecCount( iCodecs.Count() );
    const TInt codecIdCount( iCodecIds.Count() );

    __ASSERT_ALWAYS( codecCount == codecIdCount, User::Leave( KErrCorrupt ) );

    for ( TInt i( 0 ); i < codecCount; ++i )
        {
        iCodecIds[ i ].iNewId = iCodecs[i]->iCodecId;
        }

    RCSELOGSTRING( "CRCSEDbImporter::StoreAudioCodecsL() - OUT" );
    }














