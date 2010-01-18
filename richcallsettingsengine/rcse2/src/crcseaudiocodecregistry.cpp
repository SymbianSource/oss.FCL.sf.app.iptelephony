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
* Description:  Stores and loads CRCSEAudioCodecRegistry to/from data storage.
*
*/



// INCLUDE FILES
#include <featmgr.h>

#include "crcseaudiocodecregistry.h"
#include "crcseaudiocodecentry.h"
#include "rcsepanic.h"
#include "rcseconstants.h"
#include "rcseprivatecrkeys.h"
#include "rcselogger.h"

// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CRCSEAudioCodecRegistry::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CRCSEAudioCodecRegistry* CRCSEAudioCodecRegistry::NewLC()
    {
    CRCSEAudioCodecRegistry* self = new( ELeave ) CRCSEAudioCodecRegistry();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CRCSEAudioCodecRegistry::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CRCSEAudioCodecRegistry* CRCSEAudioCodecRegistry::NewL()
    {
    CRCSEAudioCodecRegistry* self = CRCSEAudioCodecRegistry::NewLC();
    CleanupStack::Pop( self );
    return self;
    }
    
// -----------------------------------------------------------------------------
// CRCSEAudioCodecRegistry::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CRCSEAudioCodecRegistry::ConstructL()
    {
    // Check VoIP support from feature manager
    FeatureManager::InitializeLibL();
    TBool support = FeatureManager::FeatureSupported( KFeatureIdCommonVoip );
    FeatureManager::UnInitializeLib();
    if (!support)
        {
        User::Leave(KErrNotSupported);
        }

    BaseConstructL();
    }

// -----------------------------------------------------------------------------
// CRCSEAudioCodecRegistry::CRCSEAudioCodecRegistry
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CRCSEAudioCodecRegistry::CRCSEAudioCodecRegistry()
    : CRCSERegistryBase( EAudioCodecSetting )
    {
    }

// Destructor
EXPORT_C CRCSEAudioCodecRegistry::~CRCSEAudioCodecRegistry()
    {
    RCSELOGSTRING( 
        "CRCSEAudioCodecRegistry::~CRCSEAudioCodecRegistry() - IN/OUT" );
    }

// -----------------------------------------------------------------------------
// CRCSEAudioCodecRegistry::AddL
// Start addition of audio codec entry.
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CRCSEAudioCodecRegistry::AddL( 
    const CRCSEAudioCodecEntry& aNewEntry )
    {
    RCSELOGSTRING( "CRCSEAudioCodecRegistry::AddL() - IN" );

    BeginL();

    TUint32 id( KNoEntryId );

    // Add entry to storage
    AddOrUpdateEntryL( id, aNewEntry );

    __ASSERT_ALWAYS( KNoEntryId < id , User::Leave( KErrCorrupt ) );

    EndL();

    RCSELOGSTRING( "CRCSEAudioCodecRegistry::AddL() - OUT" );
    return id;
    }

// -----------------------------------------------------------------------------
// CRCSEAudioCodecRegistry::FindL
// Start search of audio codec entry by ID.
// -----------------------------------------------------------------------------
//
EXPORT_C void CRCSEAudioCodecRegistry::FindL( 
    TUint32 aCodecId, 
    CRCSEAudioCodecEntry& aFoundEntry )
    {
    RCSELOGSTRING2( "CRCSEAudioCodecRegistry::FindL(%d) - IN", aCodecId );

    BeginL();

    FindEntryL( aCodecId, aFoundEntry );

    EndL();

    RCSELOGSTRING2( "CRCSEAudioCodecRegistry::FindL(%d) - OUT", aCodecId );
    }

// -----------------------------------------------------------------------------
// CRCSEAudioCodecRegistry::DeleteL
// Deletes a codec entry which codec id is aCodecId.
// -----------------------------------------------------------------------------
//
EXPORT_C void CRCSEAudioCodecRegistry::DeleteL( TUint32 aCodecId )
    {
    RCSELOGSTRING2( "CRCSEAudioCodecRegistry::DeleteL() - IN", aCodecId );

    BeginL();

    iCenRepDb->DeleteEntryL( aCodecId );

    EndL();

    RCSELOGSTRING2( "CRCSEAudioCodecRegistry::DeleteL() - OUT", aCodecId );
    }

// -----------------------------------------------------------------------------
// CRCSEAudioCodecRegistry::UpdateL
// Updates codec entry, which codec identifier is aCodecId.
// -----------------------------------------------------------------------------
//
EXPORT_C void CRCSEAudioCodecRegistry::UpdateL(
    TUint32 aCodecId,
    const CRCSEAudioCodecEntry& aUpdateData )
    {
    RCSELOGSTRING2( "CRCSEAudioCodecRegistry::UpdateL() - IN", aCodecId );

    __ASSERT_ALWAYS( KNoEntryId < aCodecId, User::Leave( KErrArgument ) );

    BeginL();

    AddOrUpdateEntryL( aCodecId, aUpdateData );

    EndL();

    RCSELOGSTRING2( "CRCSEAudioCodecRegistry::UpdateL() - OUT", aCodecId );
    }

// -----------------------------------------------------------------------------
// CRCSEAudioCodecRegistry::GetDefaultCodec
// Resets a default profile values.
// -----------------------------------------------------------------------------
//
EXPORT_C void CRCSEAudioCodecRegistry::GetDefaultCodec( 
    CRCSEAudioCodecEntry& aDefaultCodec ) const
    {
    aDefaultCodec.ResetDefaultCodecValues();
    }

// -----------------------------------------------------------------------------
// CRCSEAudioCodecRegistry::GetAllIdsL
// Gets all codec identifiers.
// -----------------------------------------------------------------------------
//
EXPORT_C void CRCSEAudioCodecRegistry::GetAllCodecIdsL( RArray<TUint32>& aAllIds )
    {
    RCSELOGSTRING( "CRCSEAudioCodecRegistry::GetAllCodecIdsL() - IN" );

    BeginL();

    FindAllIdsL( aAllIds );

    EndL();

    RCSELOGSTRING( "CRCSEAudioCodecRegistry::GetAllCodecIdsL() - OUT" );
    }

// ---------------------------------------------------------------------------
// CRCSEAudioCodecRegistry::AddL
// Add bunch of entries to store
// ---------------------------------------------------------------------------
//
void CRCSEAudioCodecRegistry::AddL( 
    RPointerArray<CRCSEAudioCodecEntry>& aEntries )
    {
    RCSELOGSTRING2( 
        "CRCSEAudioCodecRegistry::AddL(%d entries) - IN", aEntries.Count() );

    BeginL();

    TInt count( aEntries.Count() );

    for ( TInt i( 0 ); i < count; ++i )
        {
        TUint32 id( KNoEntryId );
        AddOrUpdateEntryL( id, *aEntries[ i ] );
        aEntries[ i ]->iCodecId = id;
        }

    EndL();

    RCSELOGSTRING( "CRCSEAudioCodecRegistry::AddL(entries) - OUT" );
    }

// ---------------------------------------------------------------------------
// CRCSEAudioCodecRegistry::DeleteL
// Delete bunch of entries from store in one trasaction.
// ---------------------------------------------------------------------------
//
void CRCSEAudioCodecRegistry::DeleteL( const RArray<TUint32>& aIds )
    {
    TInt count( aIds.Count() );

    RCSELOGSTRING2( 
        "CRCSEAudioCodecRegistry::DeleteL(%d entries) - IN", count );

    BeginL();

    for( TInt i = 0; i < count; i++ )
        {
        TRAPD( e, iCenRepDb->DeleteEntryL( aIds[ i ] ) );
        __ASSERT_ALWAYS( 
            KErrNone == e || KErrNotFound == e, User::Leave( e ) );
        }

    EndL();

    RCSELOGSTRING( "CRCSEAudioCodecRegistry::DeleteL(entries) - OUT" );
    }



// -----------------------------------------------------------------------------
// CRCSEAudioCodecRegistry::FindEntryL
// Do actual search of audio codec entry
// -----------------------------------------------------------------------------
//
void CRCSEAudioCodecRegistry::FindEntryL( 
    TUint32 aCodecId, 
    CRCSEAudioCodecEntry& aFoundEntry )
    {
    RCSELOGSTRING2( "CRCSEAudioCodecRegistry::FindEntryL(%d) - IN", aCodecId );

    RIpAppPropArray properties;
	TCleanupItem cleanup( CRCSERegistryBase::CleanupDbPropArray, &properties );
	CleanupStack::PushL( cleanup );

    User::LeaveIfError( iCenRepDb->FindEntryL( aCodecId, properties ) );
    ConvertPropertiesToEntryL( aFoundEntry, properties );
    aFoundEntry.iCodecId = aCodecId;

    CleanupStack::PopAndDestroy( &properties );

    RCSELOGSTRING2( "CRCSEAudioCodecRegistry::FindEntryL(%d) - OUT", aCodecId );
    }

// -----------------------------------------------------------------------------
// CRCSEAudioCodecRegistry::AddOrUpdateEntryL
// Adds new entry or updates values of existing enry
// -----------------------------------------------------------------------------
//
void CRCSEAudioCodecRegistry::AddOrUpdateEntryL( 
    TUint32& aId, 
    const CRCSEAudioCodecEntry& aEntry )
    {
    RCSELOGSTRING( "CRCSEAudioCodecRegistry::AddOrUpdateEntryL() - IN" );

    TInt id( aId );

    RIpAppPropArray properties;

	TCleanupItem cleanup( CRCSERegistryBase::CleanupDbPropArray, &properties );
	CleanupStack::PushL( cleanup );

    ConvertEntryToPropertiesL( aEntry, properties );
    
    if ( KNoEntryId == id ) // Add new one
        {
        RCSELOGSTRING( "-- Add --" );
        iCenRepDb->AddEntryL( id, properties );
        }
    else                    // Update existing entry
        {
        RCSELOGSTRING( "-- Update --" );
        User::LeaveIfError( iCenRepDb->UpdateEntryL( id, properties ) );
        }

    aId = id;

    CleanupStack::PopAndDestroy( &properties );
    
    RCSELOGSTRING( "CRCSEAudioCodecRegistry::AddOrUpdateEntryL() - OUT" );
    }



// -----------------------------------------------------------------------------
// CRCSEAudioCodecRegistry::GetEntryL
// Get values for audio codec entry from repository
// -----------------------------------------------------------------------------
//
void CRCSEAudioCodecRegistry::ConvertPropertiesToEntryL( 
    CRCSEAudioCodecEntry& aEntry,
    RIpAppPropArray& aProperties  )
    {
    RCSELOGSTRING( 
        "CRCSEAudioCodecRegistry::ConvertPropertiesToEntryL() - IN" );

    aEntry.ResetDefaultCodecValues();

    TBuf<KDesLength256> value;

    //TInt32 id( KNoEntryId );
    //GetL( aRow, KColAudioCodecId, id );
    //aEntry.iCodecId = id;

    GetPropertyValueL( KColMediaTypeName, value, aProperties );
    aEntry.iMediaTypeName.Copy( value );
    
    GetPropertyValueL( KColMediaSubTypeName, value, aProperties );
    aEntry.iMediaSubTypeName.Copy( value );

    GetPropertyValueL( KColJitterBufferSize, value, aProperties );
    aEntry.iJitterBufferSize = ToTInt32L( value );

    GetPropertyValueL( KColOctetAlign, value, aProperties );
    aEntry.iOctetAlign = static_cast<CRCSEAudioCodecEntry::TOnOff>( ToTInt32L( value ) );

    GetPropertyValueL( KColModeSet, value, aProperties );

    TLex lex( value );
    while( !lex.Eos() )
        {
        // Extract the number as unsigned int 32, which can be 
        // converted to value wanted (template)
        TUint32 num;
        lex.Val( num, EDecimal );
        aEntry.iModeSet.AppendL( static_cast<TUint32>( num ) );
        // Skip characters to space char.
        lex.SkipCharacters();
        if ( !lex.Eos() ) // Check that End of string is not next char.
            {
            // Go over the space character.
            lex.Inc( 1 );
            }
        }

    GetPropertyValueL( KColModeChangePeriod, value, aProperties );
    aEntry.iModeChangePeriod = ToTInt32L( value );

    GetPropertyValueL( KColModeChangeNeighbor, value, aProperties );
    aEntry.iModeChangeNeighbor = static_cast<CRCSEAudioCodecEntry::TOnOff>( ToTInt32L( value ) );

    GetPropertyValueL( KColPtime, value, aProperties );
    aEntry.iPtime = ToTInt32L( value );

    GetPropertyValueL( KColMaxptime, value, aProperties );
    aEntry.iMaxptime = ToTInt32L( value );

    GetPropertyValueL( KColCrc, value, aProperties );
    aEntry.iCrc = static_cast<CRCSEAudioCodecEntry::TOnOff>( ToTInt32L( value ) );

    GetPropertyValueL( KColRobustSorting, value, aProperties );
    aEntry.iRobustSorting = static_cast<CRCSEAudioCodecEntry::TOnOff>( ToTInt32L( value ) );

    GetPropertyValueL( KColInterLeaving, value, aProperties );
    aEntry.iInterLeaving = ToTInt32L( value );

    GetPropertyValueL( KColChannels, value, aProperties );

    TLex lex2( value );
    while( !lex2.Eos() )
        {
        // Extract the number as unsigned int 32, which can be 
        // converted to value wanted (template)
        TUint32 num;
        lex2.Val( num, EDecimal );
        aEntry.iChannels.AppendL( static_cast<TUint32>( num ) );
        // Skip characters to space char.
        lex2.SkipCharacters();
        if ( !lex2.Eos() ) // Check that End of string is not next char.
            {
            // Go over the space character.
            lex2.Inc( 1 );
            }
        }

    GetPropertyValueL( KColVAD, value, aProperties );
    aEntry.iVAD = static_cast<CRCSEAudioCodecEntry::TOnOff>( ToTInt32L( value ) );

    GetPropertyValueL( KColDTX, value, aProperties );
    aEntry.iDTX = static_cast<CRCSEAudioCodecEntry::TOnOff>( ToTInt32L( value ) );

    GetPropertyValueL( KColSamplingRate, value, aProperties );
    aEntry.iSamplingRate = ToTInt32L( value );

    GetPropertyValueL( KColAnnexb, value, aProperties );
    aEntry.iAnnexb = static_cast<CRCSEAudioCodecEntry::TOnOff>( ToTInt32L( value ) );

    GetPropertyValueL( KColModeChangeCapability, value, aProperties );
    aEntry.iModeChangeCapability = ToTInt32L( value );

    GetPropertyValueL( KColMaxRed, value, aProperties );
    aEntry.iMaxRed = ToTInt32L( value );

    RCSELOGSTRING( "CRCSEAudioCodecRegistry::GetEntryValuesL() - OUT" );
    }


// -----------------------------------------------------------------------------
// CRCSEProfileRegistry::AddEntryL
// Do actual addition or update of entry
// -----------------------------------------------------------------------------
//
void CRCSEAudioCodecRegistry::ConvertEntryToPropertiesL( 
    const CRCSEAudioCodecEntry& aEntry,
    RIpAppPropArray& aProperties )
    {
    RCSELOGSTRING(
         "CRCSEAudioCodecRegistry::ConvertEntryToPropertiesL() - IN" );

    // Descriptor for value conversion
    TBuf<KDesLength128> des;

    CreatePropertyL( KColMediaTypeName,
                     aEntry.iMediaTypeName,
                     aProperties );

    CreatePropertyL( KColMediaSubTypeName,
                     aEntry.iMediaSubTypeName,
                     aProperties );

    CreatePropertyL( KColJitterBufferSize,
                     ToDes( aEntry.iJitterBufferSize, des ),
                     aProperties );

    TInt32 octetAlign( aEntry.iOctetAlign ); // TOnOff
    CreatePropertyL( KColOctetAlign,
                     ToDes( octetAlign, des ),
                     aProperties );

    TInt msCount = aEntry.iModeSet.Count(); // Array type
    des.Zero();
    for ( TInt i( 0 ); i < msCount; i++ )
        {
        des.AppendNum( static_cast<TUint32>( aEntry.iModeSet[i] ) );
        if ( i < msCount - 1 )
            {
            des.Append( KSpace );
            }
        }

    CreatePropertyL( KColModeSet,
                     des,
                     aProperties );

    CreatePropertyL( KColModeChangePeriod, 
                     ToDes( aEntry.iModeChangePeriod, des ),
                     aProperties );

    TInt32 modeChangeNeighbor( aEntry.iModeChangeNeighbor ); // TOnoff
    CreatePropertyL( KColModeChangeNeighbor,
                     ToDes( modeChangeNeighbor, des ),
                     aProperties );

    CreatePropertyL( KColPtime,
                     ToDes( aEntry.iPtime, des ),
                     aProperties );

    CreatePropertyL( KColMaxptime,
                     ToDes( aEntry.iMaxptime, des ),
                     aProperties );

    TInt32 crc( aEntry.iCrc ); // TOnOff
    CreatePropertyL( KColCrc,
                     ToDes( crc, des ),
                     aProperties );

    TInt32 robustSorting( aEntry.iRobustSorting ); // TOnOff
    CreatePropertyL( KColRobustSorting,
                     ToDes( robustSorting, des ),
                     aProperties );

    CreatePropertyL( KColInterLeaving,
                     ToDes( aEntry.iInterLeaving, des ),
                     aProperties );

    TInt cCount = aEntry.iChannels.Count(); // Array type
    des.Zero();
    for ( TInt j( 0 ); j < cCount; j++ )
        {
        des.AppendNum( static_cast<TUint32>( aEntry.iChannels[j] ) );
        if ( j < cCount - 1 )
            {
            des.Append( KSpace );
            }
        }

    CreatePropertyL( KColChannels,
                     des,
                     aProperties );

    TInt32 vad( aEntry.iVAD ); // TOnOff
    CreatePropertyL( KColVAD,
                     ToDes( vad, des ),
                     aProperties );

    TInt32 dtx( aEntry.iDTX );
    CreatePropertyL( KColDTX,
                     ToDes( dtx, des ),
                     aProperties );

    CreatePropertyL( KColSamplingRate,
                     ToDes( aEntry.iSamplingRate, des ),
                     aProperties );

    TInt32 annexB( aEntry.iAnnexb ); // TOnOff
    CreatePropertyL( KColAnnexb,
                     ToDes( annexB, des ),
                     aProperties );

    CreatePropertyL( KColModeChangeCapability,
                     ToDes( aEntry.iModeChangeCapability, des ),
                     aProperties );

    CreatePropertyL( KColMaxRed,
                     ToDes( aEntry.iMaxRed, des ),
                     aProperties );

    RCSELOGSTRING(
         "CRCSEAudioCodecRegistry::ConvertEntryToPropertiesL() - OUT" );
    }


//  End of File  
