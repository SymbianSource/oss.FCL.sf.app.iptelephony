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
* Description:  Stores and loads CRCSEProfileEntry to/from data storage.
*
*/



// INCLUDE FILES
#include <sipmanagedprofileregistry.h>
#include <sipprofile.h>

#include <spsettings.h>
#include <spentry.h>
#include <spproperty.h>
#include <spdefinitions.h>
#include <featmgr.h>

#include <cenrepdatabaseutil.h>
#include <cenrepdatabaseproperty.h>

#include "crcseprofileregistry.h"
#include "crcseprofileentry.h"
#include "crcseaudiocodecregistry.h"
#include "rcsepanic.h"
#include "rcseconstants.h"
#include "rcsedefaults.h"
#include "rcseprivatecrkeys.h"
#include "rcselogger.h"


const TInt KArrayInitSize = 2;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CRCSEProfileRegistry::CRCSEProfileRegistry
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CRCSEProfileRegistry::CRCSEProfileRegistry()
    : CRCSERegistryBase( EVoIPProfile ),
      iSIPRegistry( NULL )
    {
    }

// -----------------------------------------------------------------------------
// CRCSEProfileRegistry::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CRCSEProfileRegistry::ConstructL()
    {
    // Check VoIP support from feature manager
    FeatureManager::InitializeLibL();
    TBool support = FeatureManager::FeatureSupported( KFeatureIdCommonVoip );
    FeatureManager::UnInitializeLib();
    if ( !support )
        {
        User::Leave( KErrNotSupported );
        }

    BaseConstructL();
    iSIPRegistry = CSIPManagedProfileRegistry::NewL( *this );
    }

// -----------------------------------------------------------------------------
// CRCSEProfileRegistry::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CRCSEProfileRegistry* CRCSEProfileRegistry::NewL()
    {
    CRCSEProfileRegistry* self = CRCSEProfileRegistry::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CRCSEProfileRegistry::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CRCSEProfileRegistry* CRCSEProfileRegistry::NewLC()
    {
    CRCSEProfileRegistry* self = new( ELeave ) CRCSEProfileRegistry;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CRCSEProfileRegistry::~CRCSEProfileRegistry()
    {
    RCSELOGSTRING( "CRCSEProfileRegistry::~CRCSEProfileRegistry() - IN" );

    delete iSIPRegistry;
    iSIPProfiles.ResetAndDestroy();

    RCSELOGSTRING( "CRCSEProfileRegistry::~CRCSEProfileRegistry() - OUT" );
    }
    
// -----------------------------------------------------------------------------
// CRCSEProfileRegistry::FindL
// Start search of profile entry by provider name.
// -----------------------------------------------------------------------------
//
EXPORT_C void CRCSEProfileRegistry::FindL( 
    const TDesC& aProviderName,
    CRCSEProfileEntry& aFoundEntry )
    {
    RCSELOGSTRING2( "CRCSEProfileRegistry::FindL(%S) - IN", &aProviderName );

    BeginL();

    RArray<TUint32> entryIds;
    CleanupClosePushL( entryIds );

    // Search IDs of entries that have this service ID
    FindIdsByValueL( KColProviderName,
                     aProviderName, 
                     entryIds );

    // Search whole entry if ID was found
    if ( 0 < entryIds.Count() )
        {
        FindEntryL( entryIds[ 0 ], aFoundEntry );
        }
    else
        {
        User::Leave( KErrNotFound );
        }

    // Check that SIP Ids are valid
    UpdateSIPProfileLinksL( aFoundEntry );
    
    CleanupStack::PopAndDestroy( &entryIds );

    EndL();

    RCSELOGSTRING2( "CRCSEProfileRegistry::FindL(%S) - OUT", &aProviderName );
    }

// -----------------------------------------------------------------------------
// CRCSEProfileRegistry::FindL
// Start search of profile entry by ID.
// -----------------------------------------------------------------------------
//
EXPORT_C void CRCSEProfileRegistry::FindL( 
    TUint32 aProfileId, 
    CRCSEProfileEntry& aFoundEntry )
    {
    RCSELOGSTRING2( "CRCSEProfileRegistry::FindL(id=%d) - IN", aProfileId );

    BeginL();

    FindEntryL( aProfileId, aFoundEntry );

    // Check that SIP Ids are valid
    UpdateSIPProfileLinksL( aFoundEntry );

    EndL();

    RCSELOGSTRING2( "CRCSEProfileRegistry::FindL(id=%d) - OUT", aProfileId );
    }
    
// -----------------------------------------------------------------------------
// CRCSEProfileRegistry::AddL
// Start addition of profile entry.
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CRCSEProfileRegistry::AddL( 
    const CRCSEProfileEntry& aNewEntry )
    {
    RCSELOGSTRING( "CRCSEProfileRegistry::AddL() - IN" );

    BeginL();

    TUint32 id( KNoEntryId );

    // Add entry to storage
    AddOrUpdateEntryL( id, aNewEntry );

    __ASSERT_ALWAYS( KNoEntryId < id, User::Leave( KErrCorrupt ) );

    // Create service settings or use existing
    CRCSEProfileEntry& modifyEntry 
        = const_cast<CRCSEProfileEntry&>( aNewEntry );

    // New service settings entry...
    if( ( CRCSEProfileEntry::EOONotSet == modifyEntry.iNewServiceTableEntry ||
          CRCSEProfileEntry::EOn == modifyEntry.iNewServiceTableEntry ) )
        {
        CreateDefaultServiceSettingsL( modifyEntry );
        if ( modifyEntry.iServiceProviderId != KRCSEDefServiceProviderId )
            {
            UpdateServiceIdL( id, modifyEntry.iServiceProviderId );
            }
        }

    // Use existing service settings entry if
    // - given service id is valid
    // - service id is not used by another profile
    // - provider name is valid
    else if( CRCSEProfileEntry::EOff == modifyEntry.iNewServiceTableEntry &&
             KSPNoId < modifyEntry.iServiceProviderId )
        {
        // Check that provide name is valid
        TBool providerExist( 
            NameExistsInServiceTableL( modifyEntry.iProviderName ) );

        if( !providerExist )
            {
            User::Leave( KErrNotFound );
            }

        // Check that service id is not used by another profile
        LeaveIfServiceIdInUseL( modifyEntry );
        }

    // Fail all other cases
    else
        {
        User::Leave( KErrArgument );
        }

    EndL();

    RCSELOGSTRING( "CRCSEProfileRegistry::AddL() - OUT" );

    return id;
    }

// -----------------------------------------------------------------------------
// CRCSEProfileRegistry::DeleteL
// Deletes a profile entry which profile id is aProfileId.
// -----------------------------------------------------------------------------
//
EXPORT_C void CRCSEProfileRegistry::DeleteL( 
    TUint32 aProfileId )
    {
    RCSELOGSTRING( "CRCSEProfileRegistry::DeleteL() - IN" );

    // Delete referenced settings first
    DeleteReferenceSettingsL( aProfileId );

    // Start transaction here since referenced settings are
    // deleted in own separate transaction
    BeginL();

    // Delete actual profile
    iCenRepDb->DeleteEntryL( aProfileId );

    EndL();

    RCSELOGSTRING( "CRCSEProfileRegistry::DeleteL() - OUT" );
    }

// -----------------------------------------------------------------------------
// CRCSEProfileRegistry::UpdateL
// Updates profile entry, which profile identifier is aId.
// -----------------------------------------------------------------------------
//
EXPORT_C void CRCSEProfileRegistry::UpdateL(
    TUint32 aId, const CRCSEProfileEntry& aUpdateData )
    {
    RCSELOGSTRING2( "CRCSEProfileRegistry::UpdateL(%d) - IN", aId );

    __ASSERT_ALWAYS( KNoEntryId < aId, User::Leave( KErrArgument ) );

    BeginL();

    AddOrUpdateEntryL( aId, aUpdateData );

    // Update also service settings if needed
    if ( aUpdateData.iIds.Count() && 
        aUpdateData.iIds[0].iProfileType == CRCSEProfileEntry::EProtocolSIP )
        {
        if ( aUpdateData.iServiceProviderId == KRCSEDefServiceProviderId )
            {
            CRCSEProfileEntry* entry = CRCSEProfileEntry::NewLC();
            CopyEntryValues( aUpdateData, *entry );
            CreateDefaultServiceSettingsL( *entry );
            UpdateServiceSettingsL( *entry );
            AddOrUpdateEntryL( aId, *entry );
            CleanupStack::PopAndDestroy( entry );
            }
        else
            {
            UpdateServiceSettingsL( aUpdateData );
            }
        }

    EndL();

    RCSELOGSTRING2( "CRCSEProfileRegistry::UpdateL(%d) - OUT", aId );
    }

// -----------------------------------------------------------------------------
// CRCSEProfileRegistry::GetDefaultProfile
// Resets a default profile values.
// -----------------------------------------------------------------------------
//
EXPORT_C void CRCSEProfileRegistry::GetDefaultProfile( 
    CRCSEProfileEntry& aDefaultProfile ) const
    {
    aDefaultProfile.ResetDefaultValues();
    }

// -----------------------------------------------------------------------------
// CRCSEProfileRegistry::GetAllIdsL
// Gets all profile identifiers.
// -----------------------------------------------------------------------------
//
EXPORT_C void CRCSEProfileRegistry::GetAllIdsL( RArray<TUint32>& aAllIds )
    {
    RCSELOGSTRING( "CRCSEProfileRegistry::GetAllIdsL() - IN" );

    BeginL();

    FindAllIdsL( aAllIds );
    
    EndL();

    RCSELOGSTRING( "CRCSEProfileRegistry::GetAllIdsL() - OUT" );
    }


// -----------------------------------------------------------------------------
// CRCSEProfileRegistry::FindByServiceIdL
// Gets all profiles matching to service ID.
// -----------------------------------------------------------------------------
//
EXPORT_C void CRCSEProfileRegistry::FindByServiceIdL( TUint32 aServiceId, 
    RPointerArray<CRCSEProfileEntry>& aFoundEntries )
    {
    RCSELOGSTRING( "CRCSEProfileRegistry::FindByServiceIdL() - IN" );

    BeginL();

    TBuf<KDesLength128> des;

    RArray<TUint32> entryIds;
    CleanupClosePushL( entryIds );

    // Search IDs of entries that have this service ID
    FindIdsByValueL( KColServiceProviderId,
                     ToDes( aServiceId, des ), 
                     entryIds );

    const TInt count( entryIds.Count() );
    RCSELOGSTRING2( " - Entries found: %d", count );

    // Search entries using found IDs
    for ( TInt i( 0 ); i < count; i++ )
        {
        CRCSEProfileEntry* entry = CRCSEProfileEntry::NewLC();
        TRAPD( e, FindEntryL( entryIds[ i ], *entry ) );

        if ( KErrNone == e )
            {
            aFoundEntries.AppendL( entry );
            CleanupStack::Pop( entry );
            }
        else
            {
            CleanupStack::PopAndDestroy( entry );
            }
        }

    CleanupStack::PopAndDestroy( &entryIds );

    // Update SIP profile links
    const TInt entryCount( aFoundEntries.Count() );
    for ( TInt k( 0 ); k < entryCount; ++k )
        {
        UpdateSIPProfileLinksL( *aFoundEntries[k] );
        }

    EndL();

    RCSELOGSTRING( "CRCSEProfileRegistry::FindByServiceIdL() - OUT" );
    }


// -----------------------------------------------------------------------------
// CRCSEProfileRegistry::FindBySIPSprofileIdL
// Gets all profiles based on SIP profile ID.
// -----------------------------------------------------------------------------
//
EXPORT_C void CRCSEProfileRegistry::FindBySIPProfileIdL( TUint32 aSipProfileId, 
    RPointerArray<CRCSEProfileEntry>& aFoundEntries )
    {
    RCSELOGSTRING( "CRCSEProfileRegistry::FindBySIPProfileIdL() - IN" );

    BeginL();

    // Find all profile IDs
    RArray<TUint32> voipIds;
    CleanupClosePushL( voipIds );
    FindAllIdsL( voipIds );

    // Check protocol ids (iIds) from each found profile ID.
    // Leave only those profile IDs where match found.
    CCenRepDatabaseProperty* property = CCenRepDatabaseProperty::NewLC();
    TInt count( voipIds.Count() );

    RArray<TSettingIds> prIds;
    CleanupClosePushL( prIds );

    for ( TInt i( 0 ); i < count; i++ )
        {
        iCenRepDb->FindPropertyL( voipIds[ i ], KColProtocolIds, *property );
        
        const TDesC& value = property->GetDesValue();

        ExtractProtocoIdsL( value, prIds );

        TBool found( EFalse );
        TInt prIdCount( prIds.Count() );
        for ( TInt j( 0 ); j < prIdCount && !found; j++ )
            {
            if ( prIds[ j ].iProfileType == CRCSEProfileEntry::EProtocolSIP && 
                 prIds[ j ].iProfileId == aSipProfileId )
                {
                found = ETrue;
                }
            }

        // Search profile if match found
        if ( found )
            {
            CRCSEProfileEntry* entry = CRCSEProfileEntry::NewLC();
            TRAPD( e, FindEntryL( voipIds[ i ], *entry ) );

            if ( KErrNone == e )
                {
                aFoundEntries.AppendL( entry );
                CleanupStack::Pop( entry );
                }
            else
                {
                CleanupStack::PopAndDestroy( entry );
                }
            }
        }


    CleanupStack::PopAndDestroy( &prIds );
    CleanupStack::PopAndDestroy( property );


    // Update SIP profile links
    TInt entryCount( aFoundEntries.Count() );
    for ( TInt k( 0 ); k < entryCount; ++k )
        {
        UpdateSIPProfileLinksL( *aFoundEntries[k] );
        }

    CleanupStack::PopAndDestroy( &voipIds );
    EndL();

    RCSELOGSTRING( "CRCSEProfileRegistry::FindBySIPProfileIdL() - OUT" );
    }

// ---------------------------------------------------------------------------
// CRCSEProfileRegistry::AddL
// Add bunch of entries to store
// ---------------------------------------------------------------------------
//
void CRCSEProfileRegistry::AddL( 
    RPointerArray<CRCSEProfileEntry>& aEntries )
    {
    RCSELOGSTRING2( 
        "CRCSEProfileRegistry::AddL( %d entries) - IN", aEntries.Count() );

    BeginL();

    TInt count( aEntries.Count() );

    for ( TInt i( 0 ); i < count; ++i )
        {
        // Create service settings if service ID is not available for entry
        if (  KRCSEDefServiceProviderId == aEntries[ i ]->iServiceProviderId )
            {
            CreateDefaultServiceSettingsL( *aEntries[ i ] );
            }

        TUint32 id( KNoEntryId );
        AddOrUpdateEntryL( id, *aEntries[ i ] );
        aEntries[ i ]->iId = id;
        }
        

    EndL();

    RCSELOGSTRING( "CRCSEProfileRegistry::AddL(entries) - OUT" );
    }

// -----------------------------------------------------------------------------
// CRCSEProfileRegistry::FindEntryL
// Do actual search of profile entry
// -----------------------------------------------------------------------------
//
void CRCSEProfileRegistry::FindEntryL( 
    TUint32 aProfileId, 
    CRCSEProfileEntry& aFoundEntry )
    {
    RCSELOGSTRING2( "CRCSEProfileRegistry::FindEntryL(id=%d) - IN", aProfileId );

    RIpAppPropArray properties;
	TCleanupItem cleanup( CRCSERegistryBase::CleanupDbPropArray, &properties );
	CleanupStack::PushL( cleanup );

    User::LeaveIfError( iCenRepDb->FindEntryL( aProfileId, properties ) );
    ConvertPropertiesToEntryL( aFoundEntry, properties );
    aFoundEntry.iId = aProfileId;
    
    CleanupStack::PopAndDestroy( &properties );

    RCSELOGSTRING2( "CRCSEProfileRegistry::FindEntryL(id=%d) - OUT", aProfileId );
    }

// -----------------------------------------------------------------------------
// CRCSEProfileRegistry::AddOrUpdateEntryL
// Adds new entry or updates values of existing enry
// -----------------------------------------------------------------------------
//
void CRCSEProfileRegistry::AddOrUpdateEntryL( 
    TUint32& aId, 
    const CRCSEProfileEntry& aEntry )
    {
    RCSELOGSTRING( "CRCSEProfileRegistry::AddOrUpdateEntryL() - IN" );

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

    RCSELOGSTRING( "CRCSEProfileRegistry::AddOrUpdateEntryL() - OUT" );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CRCSEProfileRegistry::ConvertPropertiesToEntryL( 
    CRCSEProfileEntry& aEntry,
    RIpAppPropArray& aProperties )
    {
    RCSELOGSTRING( "CRCSEProfileRegistry::ConvertPropertiesToEntryL() - IN" );

    aEntry.ResetDefaultValues();

    TBuf<KDesLength256> value;

    GetPropertyValueL( KColProviderName, aEntry.iProviderName, aProperties );

    GetPropertyValueL( KColSettingsName, aEntry.iSettingsName, aProperties );

    GetPropertyValueL( KColPreferredCodecs, value, aProperties );

    TLex lex2( value );
    while( !lex2.Eos() )
        {
        // Extract the number as unsigned int 32, which can be 
        // converted to value wanted (template)
        TUint32 num;
        lex2.Val( num, EDecimal );
        aEntry.iPreferredCodecs.AppendL( static_cast<TUint32>( num ) );
        // Skip characters to space char.
        lex2.SkipCharacters();
        if ( !lex2.Eos() ) // Check that End of string is not next char.
            {
            // Go over the space character.
            lex2.Inc( 1 );
            }
        }

    GetPropertyValueL( KColStartMediaPort, value, aProperties );
    aEntry.iStartMediaPort = ToTInt32L( value );

    GetPropertyValueL( KColEndMediaPort, value, aProperties );
    aEntry.iEndMediaPort = ToTInt32L( value );

    GetPropertyValueL( KColSiqnalingQOS, value, aProperties );
    aEntry.iSiqnalingQOS = ToTInt32L( value );

    GetPropertyValueL( KColMediaQOS, value, aProperties );
    aEntry.iMediaQOS = ToTInt32L( value );

    GetPropertyValueL( KColInbandDtmf, value, aProperties );
    aEntry.iInbandDTMF = static_cast<CRCSEProfileEntry::TOnOff>( ToTInt32L( value ) );

    GetPropertyValueL( KColOutbandDtmf, value, aProperties );
    aEntry.iOutbandDTMF = static_cast<CRCSEProfileEntry::TOnOff>( ToTInt32L( value ) );

    GetPropertyValueL( KColHoldRingBack, value, aProperties );
    aEntry.iHoldRingBack = ToTInt32L( value );

    GetPropertyValueL( KColAutoComplete, value, aProperties );
    aEntry.iAutoComplete = static_cast<CRCSEProfileEntry::TOnOff>( ToTInt32L( value ) );

    GetPropertyValueL( KColCFNoAnswer, value, aProperties );
    aEntry.iCFNoAnswer = static_cast<CRCSEProfileEntry::TOnOff>( ToTInt32L( value ) );

    GetPropertyValueL( KColCFBusy, value, aProperties );
    aEntry.iCFBusy = static_cast<CRCSEProfileEntry::TOnOff>( ToTInt32L( value ) );

    GetPropertyValueL( KColCFUnconditional, value, aProperties );
    aEntry.iCFUnconditional = static_cast<CRCSEProfileEntry::TOnOff>( ToTInt32L( value ) );

    GetPropertyValueL( KColRedundancy, value, aProperties );
    aEntry.iRedundancy = static_cast<CRCSEProfileEntry::TOnOff>( ToTInt32L( value ) );

    GetPropertyValueL( KColProtocolIds, value, aProperties );
    ExtractProtocoIdsL( value, aEntry.iIds );

    GetPropertyValueL( KColSecureCallPreference, value, aProperties );
    aEntry.iSecureCallPreference = ToTUint32L( value );

    GetPropertyValueL( KColVoIPProfileLock, value, aProperties );
    aEntry.iVoIPProfileLock = ToTUint32L( value );

    GetPropertyValueL( KColAdhocAllowed, value, aProperties );
    aEntry.iAdhocAllowed = ToTUint32L( value );

    GetPropertyValueL( KColSIPServerType, value, aProperties );
    aEntry.iSIPServerType.Copy( value );

    GetPropertyValueL( KColSBCType, value, aProperties );
    aEntry.iSBCType.Copy( value );;

    GetPropertyValueL( KColSTUNServerType, value, aProperties );
    aEntry.iSTUNServerType.Copy( value );

    GetPropertyValueL( KColWLANAPType, value, aProperties );
    aEntry.iWLANAPType.Copy( value );

    GetPropertyValueL( KColPSTNGatewayType, value, aProperties );
    aEntry.iPSTNGatewayType.Copy( value );

    GetPropertyValueL( KColSecurityGatewayType, value, aProperties );
    aEntry.iSecurityGatewayType.Copy( value );

    GetPropertyValueL( KColRTCP, value, aProperties );
    aEntry.iRTCP = ToTUint32L( value );

    GetPropertyValueL( KColSIPVoIPUAHTerminalType, value, aProperties );
    aEntry.iSIPVoIPUAHTerminalType = ToTUint32L( value );

    GetPropertyValueL( KColSIPVoIPUAHeaderWLANMAC, value, aProperties );
    aEntry.iSIPVoIPUAHeaderWLANMAC = ToTUint32L( value );

    GetPropertyValueL( KColSIPVoIPUAHeaderString, value, aProperties );
    aEntry.iSIPVoIPUAHeaderString.Copy( value );

    GetPropertyValueL( KColProfileLockedToIAP, value, aProperties );
    aEntry.iProfileLockedToIAP = static_cast<CRCSEProfileEntry::TOnOff>( ToTInt32L( value ) );

    GetPropertyValueL( KColVoIPPluginUID, value, aProperties );
    aEntry.iVoIPPluginUID = ToTInt32L( value );

    GetPropertyValueL( KColAllowVoIPoverWCDMA, value, aProperties );
    aEntry.iAllowVoIPoverWCDMA = static_cast<CRCSEProfileEntry::TOnOff>( ToTInt32L( value ) );

    GetPropertyValueL( KColAllowVoIPoverBT, value, aProperties );
    aEntry.iAllowVoIPoverBT = static_cast<CRCSEProfileEntry::TOnOff>( ToTInt32L( value ) );

    GetPropertyValueL( KColMeanCountOfVoIPDigits, value, aProperties );
    aEntry.iMeanCountOfVoIPDigits = ToTInt32L( value );

    GetPropertyValueL( KColIgnoreAddrDomainPart, value, aProperties );
    aEntry.iIgnoreAddrDomainPart = ToTInt32L( value );

    GetPropertyValueL( KColHandoverDialect, value, aProperties );
    aEntry.iHandoverDialect = ToTInt32L( value );

    GetPropertyValueL( KColPSTelephonyHOPreference, value, aProperties );
    aEntry.iPSTelephonyHOPreference = ToTInt32L( value );

    GetPropertyValueL( KColHOThresholdValueLL, value, aProperties );
    aEntry.iHOThresholdValueLL = ToTInt32L( value );

    GetPropertyValueL( KColHOThresholdValueHL, value, aProperties );
    aEntry.iHOThresholdValueHL = ToTInt32L( value );

    GetPropertyValueL( KColNumberOfMeasurementsAbove, value, aProperties );
    aEntry.iNumberOfMeasurementsAbove = ToTInt32L( value );

    GetPropertyValueL( KColNumberOfMeasurementsBelow, value, aProperties );
    aEntry.iNumberOfMeasurementsBelow = ToTInt32L( value );

    GetPropertyValueL( KColSmartScannInterParaHigh, value, aProperties );
    aEntry.iSmartScannInterParaHigh = ToTInt32L( value );

    GetPropertyValueL( KColSmartScannInterParaMedium, value, aProperties );
    aEntry.iSmartScannInterParaMedium = ToTInt32L( value );

    GetPropertyValueL( KColSmartScannInterParaLow, value, aProperties );
    aEntry.iSmartScannInterParaLow = ToTInt32L( value );

    GetPropertyValueL( KColSmartScannInterParaStatic, value, aProperties );
    aEntry.iSmartScannInterParaStatic = ToTInt32L( value );

    GetPropertyValueL( KColSmartScannDurationHighMode, value, aProperties );
    aEntry.iSmartScannDurationHighMode = ToTInt32L( value );

    GetPropertyValueL( KColSmartScannDurationMediumMode, value, aProperties );
    aEntry.iSmartScannDurationMediumMode = ToTInt32L( value );

    GetPropertyValueL( KColSmartScannDurationLowMode, value, aProperties );
    aEntry.iSmartScannDurationLowMode = ToTInt32L( value );

    GetPropertyValueL( KColHandoffNumber, value, aProperties );
    aEntry.iHandoffNumber = ToTInt32L( value );

    GetPropertyValueL( KColHandbackNumber, value, aProperties );
    aEntry.iHandbackNumber = ToTInt32L( value );

    GetPropertyValueL( KColHysterisisTimer, value, aProperties );
    aEntry.iHysterisisTimer = ToTInt32L( value );

    GetPropertyValueL( KColHandOffProcessTimer, value, aProperties );
    aEntry.iHandOffProcessTimer = ToTInt32L( value );

    GetPropertyValueL( KColDisconnectProcessTimer, value, aProperties );
    aEntry.iDisconnectProcessTimer = ToTInt32L( value );

    GetPropertyValueL( KColHandoffPrefix, value, aProperties );
    aEntry.iHandoffPrefix.Copy( value );

    GetPropertyValueL( KColHandbackPrefix, value, aProperties );
    aEntry.iHandbackPrefix.Copy( value );

    GetPropertyValueL( KColHandoverTones, value, aProperties );
    aEntry.iHandoverTones = ToTInt32L( value );

    GetPropertyValueL( KColSupportSMSoverWLAN, value, aProperties );
    aEntry.iSupportSMSoverWLAN = ToTInt32L( value );

    GetPropertyValueL( KColServiceProviderId, value, aProperties );
    aEntry.iServiceProviderId = ToTUint32L( value );

    GetPropertyValueL( KColUserPhoneUriParam, value, aProperties );
    aEntry.iUserPhoneUriParameter = static_cast<CRCSEProfileEntry::TOnOff>( ToTInt32L( value ) );

    GetPropertyValueL( KColSIPConnTestAddress, value, aProperties );
    aEntry.iSIPConnTestAddress.Copy( value );

    GetPropertyValueL( KColNATSettingsStorageId, value, aProperties );
    aEntry.iNATSettingsStorageId = ToTUint32L( value );

    GetPropertyValueL( KColSIPMinSE, value, aProperties );
    aEntry.iSIPMinSE = ToTInt32L( value );

    GetPropertyValueL( KColSIPSessionExpires, value, aProperties );
    aEntry.iSIPSessionExpires = ToTInt32L( value );

    GetPropertyValueL( KColNATProtocol, value, aProperties );
    aEntry.iNATProtocol = ToTInt32L( value );

    GetPropertyValueL( KColNewServiceTable, value, aProperties );
    aEntry.iNewServiceTableEntry = static_cast<CRCSEProfileEntry::TOnOff>( ToTInt32L( value ) );

    GetPropertyValueL( KColSNAPId, value, aProperties );
    aEntry.iSNAPId = ToTUint32L( value );
    
    GetPropertyValueL( KColCreationUrl, value, aProperties );
    aEntry.iAccountCreationUrl.Copy( value );

    RCSELOGSTRING( "CRCSEProfileRegistry::ConvertPropertiesToEntryL() - OUT" );
    }


// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CRCSEProfileRegistry::ConvertEntryToPropertiesL( 
    const CRCSEProfileEntry& aEntry,
    RIpAppPropArray& aProperties )
    {
    RCSELOGSTRING( "CRCSEProfileRegistry::ConvertEntryToPropertiesL() - IN" );

    // Descriptor for value conversion
    TBuf<KDesLength128> des;

    CreatePropertyL( KColProviderName,
                     aEntry.iProviderName,
                     aProperties );

    CreatePropertyL( KColSettingsName, 
                     aEntry.iSettingsName,
                     aProperties );

    TInt count = aEntry.iPreferredCodecs.Count();
    des.Zero();
    for ( TInt j( 0 ); j < count; j++ )
        {
        des.AppendNum( static_cast<TUint32>( aEntry.iPreferredCodecs[j] ) );
        if ( j < count - 1 )
            {
            des.Append( KSpace );
            }
        }

    CreatePropertyL( KColPreferredCodecs, 
                     des,
                     aProperties );

    CreatePropertyL( KColStartMediaPort, 
                     ToDes( aEntry.iStartMediaPort, des ),
                     aProperties );

    CreatePropertyL( KColEndMediaPort, 
                     ToDes( aEntry.iEndMediaPort, des ),
                     aProperties );

    CreatePropertyL( KColSiqnalingQOS, 
                     ToDes( aEntry.iSiqnalingQOS, des ),
                     aProperties );

    CreatePropertyL(  KColMediaQOS, 
                     ToDes( aEntry.iMediaQOS, des ),
                     aProperties );

    TInt32 inbandDTMF( aEntry.iInbandDTMF ); // TOnOff
    CreatePropertyL( KColInbandDtmf, 
                     ToDes( inbandDTMF, des ),
                     aProperties );

    TInt32 outbandDTMF( aEntry.iOutbandDTMF ); // TOnOff
    CreatePropertyL( KColOutbandDtmf, 
                     ToDes( outbandDTMF, des ),
                     aProperties );

    TInt32 holdRingBack( aEntry.iHoldRingBack ); // TOnOff
    CreatePropertyL( KColHoldRingBack, 
                     ToDes( holdRingBack, des ),
                     aProperties );

    TInt32 autoComplete = aEntry.iAutoComplete; // TOnOff
    CreatePropertyL( KColAutoComplete, 
                     ToDes( autoComplete, des ),
                     aProperties );

    TInt32 cfNoAnswer( aEntry.iCFNoAnswer ); // TOnOff
    CreatePropertyL( KColCFNoAnswer, 
                     ToDes( cfNoAnswer,  des ),
                     aProperties );

    TInt32 cfBusy( aEntry.iCFBusy ); // TOnOff
    CreatePropertyL( KColCFBusy, 
                     ToDes( cfBusy, des ),
                     aProperties );

    TInt32 cfUnconditional( aEntry.iCFUnconditional ); // TOnOff
    CreatePropertyL( KColCFUnconditional, 
                     ToDes( cfUnconditional, des ),
                     aProperties );

    TInt32 redundancy( aEntry.iRedundancy ); // TOnOff
    CreatePropertyL( KColRedundancy, 
                     ToDes( redundancy, des ),
                     aProperties );

    des.Zero();
    TBuf<KDesLength15> intDes; // for SIP profile ID
    count = aEntry.iIds.Count();

    if ( count )
        {
        for( TInt i = 0; i < count; i++ )
            {
            intDes.Zero();

            TInt profileType = aEntry.iIds[i].iProfileType;
            intDes.Num( profileType );
            des.Append( intDes );
            des.Append( KSpace );

            TInt profileId = aEntry.iIds[i].iProfileId;
            intDes.Num( profileId );
            des.Append( intDes );
            des.Append( KSpace );

            TInt profileSpecificId = aEntry.iIds[i].iProfileSpecificSettingId;
            intDes.Num( profileSpecificId );
            des.Append( intDes );
            des.Append( KSpace );
            }
        }

    CreatePropertyL( KColProtocolIds, 
                     des,
                     aProperties );

    CreatePropertyL( KColSecureCallPreference, 
                     ToDes( aEntry.iSecureCallPreference, des ),
                     aProperties );

    CreatePropertyL( KColVoIPProfileLock, 
                     ToDes( aEntry.iVoIPProfileLock, des ),
                     aProperties );

    CreatePropertyL( KColAdhocAllowed, 
                     ToDes( aEntry.iAdhocAllowed, des ),
                     aProperties );

    CreatePropertyL( KColSIPServerType, 
                     aEntry.iSIPServerType,
                     aProperties );

    CreatePropertyL( KColSBCType, 
                     aEntry.iSBCType,
                     aProperties );

    CreatePropertyL( KColSTUNServerType, 
                     aEntry.iSTUNServerType,
                     aProperties );

    CreatePropertyL( KColWLANAPType, 
                     aEntry.iWLANAPType,
                     aProperties );

    CreatePropertyL( KColPSTNGatewayType, 
                     aEntry.iPSTNGatewayType,
                     aProperties );

    CreatePropertyL( KColSecurityGatewayType, 
                     aEntry.iSecurityGatewayType,
                     aProperties );
  
    CreatePropertyL( KColRTCP, 
                     ToDes( aEntry.iRTCP, des ),
                     aProperties );

    CreatePropertyL( KColSIPVoIPUAHTerminalType, 
                     ToDes( aEntry.iSIPVoIPUAHTerminalType, des ),
                     aProperties );

    CreatePropertyL( KColSIPVoIPUAHeaderWLANMAC, 
                     ToDes( aEntry.iSIPVoIPUAHeaderWLANMAC, des ),
                     aProperties );

    CreatePropertyL( KColSIPVoIPUAHeaderString, 
                     aEntry.iSIPVoIPUAHeaderString,
                     aProperties );

    TInt32 profileLockedToIAP( aEntry.iProfileLockedToIAP ); // TOnOff
    CreatePropertyL( KColProfileLockedToIAP, 
                     ToDes( profileLockedToIAP, des ),
                     aProperties );

    CreatePropertyL( KColVoIPPluginUID, 
                     ToDes( aEntry.iVoIPPluginUID, des ),
                     aProperties );

    TInt32 allowVoIPoverWCDMA( aEntry.iAllowVoIPoverWCDMA ); // TOnOff
    CreatePropertyL( KColAllowVoIPoverWCDMA, 
                     ToDes( allowVoIPoverWCDMA, des ),
                     aProperties );

    TInt32 allowVoIPoverBT( aEntry.iAllowVoIPoverBT ); // TOnOff
    CreatePropertyL( KColAllowVoIPoverBT, 
                     ToDes( allowVoIPoverBT, des ),
                     aProperties );

    CreatePropertyL( KColMeanCountOfVoIPDigits,
                     ToDes( aEntry.iMeanCountOfVoIPDigits, des ),
                     aProperties );

    CreatePropertyL( KColIgnoreAddrDomainPart, 
                     ToDes( aEntry.iIgnoreAddrDomainPart, des ),
                     aProperties );

    CreatePropertyL( KColHandoverDialect, 
                     ToDes( aEntry.iHandoverDialect, des ),
                     aProperties );

    CreatePropertyL( KColPSTelephonyHOPreference, 
                     ToDes( aEntry.iPSTelephonyHOPreference, des ),
                     aProperties );

    CreatePropertyL( KColHOThresholdValueLL, 
                     ToDes( aEntry.iHOThresholdValueLL, des ),
                     aProperties );

    CreatePropertyL( KColHOThresholdValueHL, 
                     ToDes( aEntry.iHOThresholdValueHL, des ),
                     aProperties );

    CreatePropertyL( KColNumberOfMeasurementsAbove, 
                     ToDes( aEntry.iNumberOfMeasurementsAbove, des ),
                     aProperties );

    CreatePropertyL( KColNumberOfMeasurementsBelow, 
                     ToDes( aEntry.iNumberOfMeasurementsBelow, des ),
                     aProperties );

    CreatePropertyL( KColSmartScannInterParaHigh, 
                     ToDes( aEntry.iSmartScannInterParaHigh, des ),
                     aProperties );

    CreatePropertyL( KColSmartScannInterParaMedium, 
                     ToDes(  aEntry.iSmartScannInterParaMedium, des ),
                     aProperties );

    CreatePropertyL( KColSmartScannInterParaLow, 
                     ToDes( aEntry.iSmartScannInterParaLow, des ),
                     aProperties );

    CreatePropertyL( KColSmartScannInterParaStatic, 
                     ToDes( aEntry.iSmartScannInterParaStatic, des ),
                     aProperties );

    CreatePropertyL( KColSmartScannDurationHighMode, 
                     ToDes( aEntry.iSmartScannDurationHighMode, des ),
                     aProperties );

    CreatePropertyL( KColSmartScannDurationMediumMode, 
                     ToDes( aEntry.iSmartScannDurationMediumMode, des ),
                     aProperties );

    CreatePropertyL( KColSmartScannDurationLowMode, 
                     ToDes( aEntry.iSmartScannDurationLowMode, des ),
                     aProperties );

    CreatePropertyL( KColHandoffNumber, 
                     ToDes( aEntry.iHandoffNumber, des ),
                     aProperties );

    CreatePropertyL( KColHandbackNumber, 
                     ToDes( aEntry.iHandbackNumber, des ),
                     aProperties );

    CreatePropertyL( KColHysterisisTimer, 
                     ToDes( aEntry.iHysterisisTimer, des ),
                     aProperties );

    CreatePropertyL( KColHandOffProcessTimer, 
                     ToDes( aEntry.iHandOffProcessTimer, des ),
                     aProperties );

    CreatePropertyL( KColDisconnectProcessTimer, 
                     ToDes( aEntry.iDisconnectProcessTimer, des ),
                     aProperties );

    CreatePropertyL( KColHandoffPrefix, 
                     aEntry.iHandoffPrefix,
                     aProperties );

    CreatePropertyL( KColHandbackPrefix, 
                     aEntry.iHandbackPrefix,
                     aProperties );

    CreatePropertyL( KColHandoverTones, 
                     ToDes( aEntry.iHandoverTones, des ),
                     aProperties );

    CreatePropertyL( KColSupportSMSoverWLAN, 
                     ToDes( aEntry.iSupportSMSoverWLAN, des ),
                     aProperties );

    CreatePropertyL( KColServiceProviderId, 
                     ToDes( aEntry.iServiceProviderId, des ),
                     aProperties );

    TInt32 userPhoneUriParameter( aEntry.iUserPhoneUriParameter ); // TOnOff
    CreatePropertyL( KColUserPhoneUriParam, 
                     ToDes( userPhoneUriParameter, des ),
                     aProperties );

    CreatePropertyL( KColSIPConnTestAddress, 
                     aEntry.iSIPConnTestAddress,
                     aProperties );

    CreatePropertyL( KColNATSettingsStorageId, 
                     ToDes( aEntry.iNATSettingsStorageId, des ),
                     aProperties );

    CreatePropertyL( KColSIPMinSE, 
                     ToDes( aEntry.iSIPMinSE, des ),
                     aProperties );

    CreatePropertyL( KColSIPSessionExpires, 
                     ToDes( aEntry.iSIPSessionExpires, des ),
                     aProperties );

    CreatePropertyL( KColNATProtocol, 
                     ToDes( aEntry.iNATProtocol, des ),
                     aProperties );

    TInt32 newServiceTable( aEntry.iNewServiceTableEntry ); // TOnOff
    CreatePropertyL( KColNewServiceTable, 
                     ToDes( newServiceTable, des ),
                     aProperties );

    CreatePropertyL( KColSNAPId, 
                     ToDes( aEntry.iSNAPId, des ),
                     aProperties );

    CreatePropertyL( KColCreationUrl, 
                     aEntry.iAccountCreationUrl,
                     aProperties );
    
    RCSELOGSTRING( "CRCSEProfileRegistry::ConvertEntryToPropertiesL() - OUT" );
    }

// -----------------------------------------------------------------------------
// CRCSEProfileRegistry::
// -----------------------------------------------------------------------------
//
void CRCSEProfileRegistry::ExtractProtocoIdsL( 
    const TDesC& aDes, RArray<TSettingIds>& aArray )
    {
    TLex lex( aDes );
    
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
    }

// -----------------------------------------------------------------------------
// CRCSEProfileRegistry::UpdateSIPProfileLinksL
// Validates profile's SIP profile IDs and updates if necessary
// -----------------------------------------------------------------------------
//
void CRCSEProfileRegistry::UpdateSIPProfileLinksL( CRCSEProfileEntry& aEntry )
    {
    RCSELOGSTRING( "CRCSEProfileRegistry::UpdateSIPProfileLinksL - IN" );
    const TInt count( aEntry.iIds.Count() );
    TSettingIds value;
    RCSELOGSTRING2( "Protocol link count=%d", count );

    if ( 0 < count )
        {
        iSIPProfiles.ResetAndDestroy();

        TInt err(0);
        // Check the existence of linked SIP profiles
        for ( TInt i( count-1 ); 0 <= i ; i-- )
            {
            value = aEntry.iIds[i];

            if ( CRCSEProfileEntry::EProtocolSIP == value.iProfileType )
                {
                CSIPProfile* profile = NULL;
                TRAP( err, profile = iSIPRegistry->ProfileL( value.iProfileId ) );
                if ( KErrNotFound == err )
                    {
                    aEntry.iIds.Remove( i );
                    delete profile;
                    profile = NULL;
                    RCSELOGSTRING2( "SIP link removed, id=%d", value.iProfileId );
                    }
                else if ( KErrNone == err )
                    {
                    err = iSIPProfiles.Append( profile );
                    }
                else
                    {
                    delete profile;
                    profile = NULL;
                    }
                }
            }

        // Check that all profiles have same AOR.
        // If some AOR differs leave only the first link
        TInt profCount( iSIPProfiles.Count() );
        RCSELOGSTRING2( "SIP link count=%d", profCount );
        TBool ok( ETrue );
        if ( 1 < profCount )
            {
            const TDesC8* aor1;
            HBufC8* empty = KNullDesC8().AllocLC();
            err = iSIPProfiles[0]->GetParameter( KSIPUserAor, aor1 );

            if ( KErrNotFound == err )
                {
                aor1 = empty;
                }

            for ( TInt j( 1 ); j < profCount; j++ )
                {
                const TDesC8* aor2;
                err = iSIPProfiles[j]->GetParameter( KSIPUserAor, aor2 );

                if ( KErrNotFound == err )
                    {
                    if ( aor1->Compare( KNullDesC8() ) != 0 )
                        {
                        ok = EFalse;
                        }
                    }
                else
                    { 
                    if ( aor1->Compare( *aor2 ) != 0 )
                        {
                        ok = EFalse;
                        }
                    }
                }
            CleanupStack::PopAndDestroy( empty );

            if ( !ok )
                {
                RCSELOGSTRING( "AOR changed" );
                TInt newCount = aEntry.iIds.Count();
                TBool first( ETrue );
                for ( TInt k( 0 ); k < newCount; k++ )
                    {
                    value = aEntry.iIds[k];
                    if ( CRCSEProfileEntry::EProtocolSIP == value.iProfileType )
                        {
                        if ( first )
                            {
                            first = EFalse;
                            RCSELOGSTRING2( "Leave SIP link, index=%d", k );
                            }
                        else
                            {
                            aEntry.iIds.Remove( k );
                            newCount--;
                            k--;
                            RCSELOGSTRING2( "Remove SIP link, index=%d", k );
                            }
                        }
                    }
                }
            }

        iSIPProfiles.ResetAndDestroy();

        // Update entry if count of ids have changed
        if ( count != aEntry.iIds.Count() )
            {
            RCSELOGSTRING3( "Link ID count: initial=%d, final=%d",
                             count, aEntry.iIds.Count() );

            AddOrUpdateEntryL( aEntry.iId, aEntry );

            RCSELOGSTRING( "Entry updated" );
            }
        }

    RCSELOGSTRING( "CRCSEProfileRegistry::UpdateSIPProfileLinksL - OUT" );
    }

// -----------------------------------------------------------------------------
// Local helper method for searching default values of properties from service
// table and adding them to given service entry.
// -----------------------------------------------------------------------------
//
void FindAndAddDefaultPropertyL( 
    CSPSettings& aTable, 
    CSPEntry& aEntry, 
    CSPProperty& aProperty, 
    TServicePropertyName aName  )
    {
    TInt err = aTable.FindPropertyL( KDefaultVoIPService, aName, aProperty );

    if ( KErrNone == err )
        {
        aEntry.AddPropertyL( aProperty );
        }
    }

// -----------------------------------------------------------------------------
// CRCSEProfileRegistry::CreateDefaultServiceSettingsL
// Creates a voip-sip service entry with default content and return
// service ID for new profile
// -----------------------------------------------------------------------------
//
void CRCSEProfileRegistry::CreateDefaultServiceSettingsL( 
    CRCSEProfileEntry& aNewEntry )
    {
    RCSELOGSTRING( 
        "CRCSEProfileRegistry::CreateDefaultServiceSettingsL - IN" );

    // Do not create service if there is no SIP linkage.
    if ( !aNewEntry.iIds.Count() )
        {
        return;
        }
    else if ( aNewEntry.iIds[0].iProfileType != 
        CRCSEProfileEntry::EProtocolSIP )
        {
        return;
        }

    // Service table API
    CSPSettings* table = CSPSettings::NewLC();

    // New service entry for this provider name
    CSPEntry* entry = CSPEntry::NewLC();
    entry->SetServiceName( aNewEntry.iProviderName );

    CSPProperty* property = CSPProperty::NewLC();

    // Service Setup plug-in UID ( if VoIP plugin UID set )
    if ( KRCSEDefaultVoIPPluginUID != aNewEntry.iVoIPPluginUID )
        {
        property->SetName( EPropertyServiceSetupPluginId );
        property->SetValue( aNewEntry.iVoIPPluginUID );
        entry->AddPropertyL( *property );
        }

    // This property is needed by clients to check if VoIP service is used
    property->SetName( ESubPropertyVoIPSettingsId );
    property->SetValue( KVoIPSettingsID );
    entry->AddPropertyL( *property );
    
    // Service Attribute Mask
    FindAndAddDefaultPropertyL( 
        *table, *entry, *property, EPropertyServiceAttributeMask );

    // Phone Application CLI plug-in UID
    FindAndAddDefaultPropertyL( 
        *table, *entry, *property, EPropertyCLIPluginId );

    // Phone Application Call Logging plug-in UID.
    FindAndAddDefaultPropertyL( 
        *table, *entry, *property, EPropertyCallLoggingPluginId );

    // Phone Application Call State Indicator plug-in UID.
    FindAndAddDefaultPropertyL( 
        *table, *entry, *property, EPropertyCallStateIndicatorPluginId );

    // Phone Application Call Menu Handler plug-in UID.
    FindAndAddDefaultPropertyL( 
        *table, *entry, *property, EPropertyCallMenuHandlerPluginId );

    // Converged Call Engine Call Provider plug-in UID.
    FindAndAddDefaultPropertyL( 
        *table, *entry, *property, EPropertyCallProviderPluginId );

    // Virtual Phonebook Contact Store ID.
    FindAndAddDefaultPropertyL( 
        *table, *entry, *property, EPropertyContactStoreId );

    // Contact View plug-in UID
    FindAndAddDefaultPropertyL( 
        *table, *entry, *property, EPropertyContactViewPluginId );

    // Converged Connection Handler (CCH) VoIP Subservice plug-in UID
    FindAndAddDefaultPropertyL( 
        *table, *entry, *property, EPropertyVoIPSubServicePluginId );

    // Converged Connection Handler (CCH) VMBX Subservice plug-in UID
    FindAndAddDefaultPropertyL( 
        *table, *entry, *property, EPropertyVMBXSubServicePluginId );

    // Brand ID for VoIP
    FindAndAddDefaultPropertyL( 
        *table, *entry, *property, EPropertyBrandId );

    // Presence Service Connectivity plugin UID
    FindAndAddDefaultPropertyL( 
        *table, *entry, *property, EPropertyPCSPluginId );

    // Maximum Lenght of Log Contact Link
    FindAndAddDefaultPropertyL( 
        *table, *entry, *property, EPropertyLogCntLinkMaxLength );

    // Field Type of Presence Presentity ID
    FindAndAddDefaultPropertyL( 
        *table, *entry, *property, ESubPropertyPresencePresentityIDFieldType );

    // Add entry to store and set service ID
    User::LeaveIfError( table->AddEntryL( *entry ) );
    aNewEntry.iServiceProviderId 
        = static_cast<TUint32>( entry->GetServiceId() );

    CleanupStack::PopAndDestroy( property );
    CleanupStack::PopAndDestroy( entry );
    CleanupStack::PopAndDestroy( table );

    RCSELOGSTRING( 
        "CRCSEProfileRegistry::CreateDefaultServiceSettingsL - OUT" );
    }

// ---------------------------------------------------------------------------
// CRCSEProfileRegistry::DeleteReferenceSettingsL
// ---------------------------------------------------------------------------
//
void CRCSEProfileRegistry::DeleteReferenceSettingsL( TUint32 aProfileId )
    {
    RCSELOGSTRING( "CRCSEProfileRegistry::DeleteReferenceSettingsL - IN" );

    CRCSEProfileEntry* profileInfo = CRCSEProfileEntry::NewLC();

    TRAPD( e, FindL( aProfileId, *profileInfo ) );

    __ASSERT_ALWAYS( KErrNone == e || KErrNotFound == e, User::Leave( e ) );

    // Delete related audio codecs
    CRCSEAudioCodecRegistry* codecReg = CRCSEAudioCodecRegistry::NewLC();
    codecReg->DeleteL( profileInfo->iPreferredCodecs );
    CleanupStack::PopAndDestroy( codecReg );

    // Delete related service settings
    if( CRCSEProfileEntry::EOONotSet == profileInfo->iNewServiceTableEntry ||
        CRCSEProfileEntry::EOn == profileInfo->iNewServiceTableEntry )
        {
        RCSELOGSTRING( "Delete related service settings" );
        CSPSettings* serviceSettings = CSPSettings::NewLC();
        serviceSettings->DeleteEntryL( profileInfo->iServiceProviderId );
        CleanupStack::PopAndDestroy( serviceSettings );
        }

    CleanupStack::PopAndDestroy( profileInfo );

    RCSELOGSTRING( "CRCSEProfileRegistry::DeleteReferenceSettingsL - OUT" );
    }
// -----------------------------------------------------------------------------
// CRCSEProfileRegistry::NameExistsInServiceTableL
// -----------------------------------------------------------------------------
//
TBool CRCSEProfileRegistry::NameExistsInServiceTableL(
    const TDesC& aProviderName ) const
    {
    RCSELOGSTRING( "CRCSEProfileRegistry::NameExistsInServiceTableL- IN" );
    TBool result( EFalse );
    
    RIdArray serviceIds;
    CleanupClosePushL( serviceIds );
    CDesCArray* serviceNames = 
        new ( ELeave ) CDesCArrayFlat( KArrayInitSize );
    CleanupStack::PushL( serviceNames );
    CSPSettings* serviceSettings = CSPSettings::NewLC();
    TInt err(KErrNone);
    err = serviceSettings->FindServiceIdsL( serviceIds );
    if ( KErrNotFound == err )
        {
        CleanupStack::PopAndDestroy( serviceSettings );
        CleanupStack::PopAndDestroy( serviceNames );
        CleanupStack::PopAndDestroy( &serviceIds );
        return EFalse; 
        }
    User::LeaveIfError( err );
    User::LeaveIfError( 
        serviceSettings->FindServiceNamesL( serviceIds, *serviceNames ) );
    CleanupStack::PopAndDestroy( serviceSettings );
    TBool serviceFound( EFalse );
    for ( TInt i( 0 ) ; i < serviceNames->MdcaCount() && !serviceFound ; i++ )
        {
        if ( !aProviderName.Compare( serviceNames->MdcaPoint( i) ) )
            {
            result = ETrue;
            serviceFound = ETrue;
            }
        }
    CleanupStack::PopAndDestroy( serviceNames );	
    CleanupStack::PopAndDestroy( &serviceIds );
    
    RCSELOGSTRING( "CRCSEProfileRegistry::NameExistsInServiceTableL- OUT" );
    return result;
    }
    
// -----------------------------------------------------------------------------
// CRCSEProfileRegistry::LeaveIfServiceIdInUseL
// -----------------------------------------------------------------------------
//
void CRCSEProfileRegistry::LeaveIfServiceIdInUseL(
    const CRCSEProfileEntry& aEntry ) 
    {    
    RCSELOGSTRING( "CRCSEProfileRegistry::LeaveIfServiceIdInUseL - IN" );

    // Find all profile IDs
    RArray<TUint32> voipIds;
    CleanupClosePushL( voipIds );
    FindAllIdsL( voipIds );


    // Check service id from each profile
    CCenRepDatabaseProperty* property = CCenRepDatabaseProperty::NewLC();
    TBuf<KDesLength256> value;
    TInt count( voipIds.Count() );

    for ( TInt i( 0 ); i < count; i++ )
        {
        iCenRepDb->FindPropertyL( voipIds[ i ], KColProtocolIds, *property );

        __ASSERT_ALWAYS( 
            aEntry.iServiceProviderId != ToTUint32L( property->GetDesValue() ),
                User::Leave( KErrAlreadyExists ) );
        }

    CleanupStack::PopAndDestroy( property );
    CleanupStack::PopAndDestroy( &voipIds );

    RCSELOGSTRING( "CRCSEProfileRegistry::LeaveIfServiceIdInUseL - OUT" );
    }

// -----------------------------------------------------------------------------
// CRCSEProfileRegistry::UpdateServiceSettingsL
// -----------------------------------------------------------------------------
//
void CRCSEProfileRegistry::UpdateServiceSettingsL(
    const CRCSEProfileEntry& aEntry ) 
    {    
    RCSELOGSTRING( "CRCSEProfileRegistry::UpdateServiceSettingsL - IN" );

    // Do not update service if there is no SIP linkage.
    if ( !aEntry.iIds.Count() )
        {
        return;
        }
    else if ( aEntry.iIds[0].iProfileType != 
        CRCSEProfileEntry::EProtocolSIP )
        {
        return;
        }

    CSPSettings* serviceSettings = CSPSettings::NewLC();
    CSPEntry* serviceEntry = CSPEntry::NewLC();

    User::LeaveIfError( 
        serviceSettings->FindEntryL( 
            aEntry.iServiceProviderId, *serviceEntry ) );

    // Update service settings if service name has been changed
    if ( aEntry.iProviderName != serviceEntry->GetServiceName() )
        {
        serviceEntry->SetServiceName( aEntry.iProviderName );
        User::LeaveIfError( serviceSettings->UpdateEntryL( *serviceEntry ) );
        }

    CleanupStack::PopAndDestroy( serviceEntry );
    CleanupStack::PopAndDestroy( serviceSettings );

    RCSELOGSTRING( "CRCSEProfileRegistry::UpdateServiceSettingsL - OUT" );
    }

// -----------------------------------------------------------------------------
// CRCSEProfileRegistry::UpdateServiceIdL
// -----------------------------------------------------------------------------
//
void CRCSEProfileRegistry::UpdateServiceIdL( TUint32 aId, TUint32 aServiceId )
    {    
    RCSELOGSTRING( "CRCSEProfileRegistry::UpdateServiceIdL - IN" );

    TBuf<KDesLength128> des;

    RIpAppPropArray properties;

    CreatePropertyL( KColServiceProviderId,
                     ToDes( aServiceId, des ),
                     properties );

    iCenRepDb->AddOrUpdatePropertiesL( aId, properties );

    properties.ResetAndDestroy();

    RCSELOGSTRING( "CRCSEProfileRegistry::UpdateServiceIdL - OUT" );
    }

// ---------------------------------------------------------------------------
// From MSIPProfileRegistryObserver
// CRCSEProfileRegistry::ProfileRegistryEventOccurred
// SIP profile event
// ---------------------------------------------------------------------------
//
void CRCSEProfileRegistry::ProfileRegistryEventOccurred( 
    TUint32 /*aSIPProfileId*/,
    TEvent /*aEvent*/ )
    {
    RCSELOGSTRING( "CRCSEProfileRegistry::ProfileRegistryEventOccurred" );
    // Have to be implemented because MSIPProfileRegistryObserver
    // is passed to CSIPManagedProfileRegistry.
    }

// ---------------------------------------------------------------------------
// From MSIPProfileRegistryObserver
// CRCSEProfileRegistry::ProfileRegistryErrorOccured
// An asynchronous error has occurred related to SIP profile
// ---------------------------------------------------------------------------
//
void CRCSEProfileRegistry::ProfileRegistryErrorOccurred( 
    TUint32  /*aProfileId*/,
    TInt /*aError*/ )
    {
    RCSELOGSTRING( "CRCSEProfileRegistry::ProfileRegistryErrorOccurred" );
    // Have to be implemented because MSIPProfileRegistryObserver
    // is passed to CSIPManagedProfileRegistry.
    }

// -----------------------------------------------------------------------------
// CRCSEProfileRegistry::ResetAndDestroyEntries
// -----------------------------------------------------------------------------
//
void CRCSEProfileRegistry::ResetAndDestroyEntries( TAny* anArray )
	{
	RCSELOGSTRING( "CRCSEProfileRegistry::ResetAndDestroyEntries - IN" );
	
    RPointerArray<CRCSEProfileEntry>* array = 
        reinterpret_cast<RPointerArray<CRCSEProfileEntry>*>( anArray );
    if (array)
        {
        array->ResetAndDestroy();
        array->Close();
        }
    RCSELOGSTRING( "CRCSEProfileRegistry::ResetAndDestroyEntries - OUT" );
	}

// -----------------------------------------------------------------------------
// CRCSEProfileRegistry::CopyEntryValues
// -----------------------------------------------------------------------------
//
void CRCSEProfileRegistry::CopyEntryValues( 
    const CRCSEProfileEntry& aSourceEntry, 
    CRCSEProfileEntry& aDestinationEntry )
    {
    aDestinationEntry.ResetDefaultValues();
    aDestinationEntry.iIds.Reset();
    for ( TInt i = 0; i < aSourceEntry.iIds.Count(); i++ )
        {
        aDestinationEntry.iIds.Append( aSourceEntry.iIds[i] );
        }
    aDestinationEntry.iId = aSourceEntry.iId;
    aDestinationEntry.iProviderName = aSourceEntry.iProviderName;
    aDestinationEntry.iSettingsName = aSourceEntry.iSettingsName;
    aDestinationEntry.iPreferredCodecs.Reset();
    for ( TInt i = 0; i < aSourceEntry.iPreferredCodecs.Count(); i++ )
        {
        aDestinationEntry.iPreferredCodecs.Append( 
            aSourceEntry.iPreferredCodecs[i] );
        }
    aDestinationEntry.iStartMediaPort = aSourceEntry.iStartMediaPort;
    aDestinationEntry.iEndMediaPort = aSourceEntry.iEndMediaPort;
    aDestinationEntry.iMediaQOS = aSourceEntry.iMediaQOS;
    aDestinationEntry.iInbandDTMF = aSourceEntry.iInbandDTMF;
    aDestinationEntry.iOutbandDTMF = aSourceEntry.iOutbandDTMF;
    aDestinationEntry.iSecureCallPreference = 
        aSourceEntry.iSecureCallPreference;
    aDestinationEntry.iRTCP = aSourceEntry.iRTCP;
    aDestinationEntry.iSIPVoIPUAHTerminalType = 
        aSourceEntry.iSIPVoIPUAHTerminalType;
    aDestinationEntry.iSIPVoIPUAHeaderWLANMAC = 
        aSourceEntry.iSIPVoIPUAHeaderWLANMAC;
    aDestinationEntry.iSIPVoIPUAHeaderString = 
        aSourceEntry.iSIPVoIPUAHeaderString;
    aDestinationEntry.iProfileLockedToIAP = aSourceEntry.iProfileLockedToIAP;
    aDestinationEntry.iVoIPPluginUID = aSourceEntry.iVoIPPluginUID;
    aDestinationEntry.iAllowVoIPoverWCDMA = aSourceEntry.iAllowVoIPoverWCDMA;
    aDestinationEntry.iMeanCountOfVoIPDigits = 
        aSourceEntry.iMeanCountOfVoIPDigits;
    aDestinationEntry.iIgnoreAddrDomainPart = 
        aSourceEntry.iIgnoreAddrDomainPart;
    aDestinationEntry.iServiceProviderId = aSourceEntry.iServiceProviderId;
    aDestinationEntry.iUserPhoneUriParameter = 
        aSourceEntry.iUserPhoneUriParameter;
    aDestinationEntry.iSIPConnTestAddress = aSourceEntry.iSIPConnTestAddress;
    aDestinationEntry.iNATSettingsStorageId = 
        aSourceEntry.iNATSettingsStorageId;
    aDestinationEntry.iSIPMinSE = aSourceEntry.iSIPMinSE;
    aDestinationEntry.iSIPSessionExpires = aSourceEntry.iSIPSessionExpires;
    aDestinationEntry.iNATProtocol = aSourceEntry.iNATProtocol;
    aDestinationEntry.iNewServiceTableEntry = 
        aSourceEntry.iNewServiceTableEntry;
    aDestinationEntry.iSNAPId = aSourceEntry.iSNAPId;
    aDestinationEntry.iAccountCreationUrl = aSourceEntry.iAccountCreationUrl;
    }

//  End of File  


