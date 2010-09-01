/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implements VCC setting reader
*
*/



#include <spsettings.h>
#include <spproperty.h>
#include <spdefinitions.h>
#include <spentry.h>

#include "vccsettingsreader.h"
#include "vccdefinitions.h"

#include "rubydebug.h"

// CS Service Entry name in SP settings table
_LIT( KCSEntryName, "CS" );


// ---------------------------------------------------------------------------
// Returns the domain transfer number from sp settings
// ---------------------------------------------------------------------------
//
HBufC* VccSettingsReader::DomainTransferNumberL()
    {
    RUBY_DEBUG_BLOCK( "VccSettingsReader::GetDomainTransferNumberL" );

    return VccSettingsReader::PropertyValueByNameL( ESubPropertyVccVDN );
    }

// ---------------------------------------------------------------------------
// Returns the domain transfer URI from sp settings
// ---------------------------------------------------------------------------
//
HBufC* VccSettingsReader::DomainTransferUriL()
    {
    RUBY_DEBUG_BLOCK( "VccSettingsReader::GetDomainTransferUriL" );

    return VccSettingsReader::PropertyValueByNameL( ESubPropertyVccVDI );
    }

// ---------------------------------------------------------------------------
// Returns a VCC property value from SP-settings table by
// given property name
// ---------------------------------------------------------------------------
//
HBufC* VccSettingsReader::PropertyValueByNameL(
        TServicePropertyName aPropertyName )
    {
    RUBY_DEBUG_BLOCK( "VccSettingsReader::GetValueL" );

    // Create settings object and then the property one
    CSPSettings* settings = CSPSettings::NewLC();

    CSPProperty* property = CSPProperty::NewLC();

    // First we must find what is our vcc service id in sp-settings table
    // This is done by searching the vcc entry name.
    TInt vccServiceId( KErrNone );

     vccServiceId = VccSettingsReader::VccServiceIdL(); 
    
    // If the VCC service id was not found, leave.
    User::LeaveIfError( vccServiceId );

    settings->FindPropertyL( vccServiceId, aPropertyName, *property );

    HBufC* value = HBufC::NewLC( KMaxFileName );
    TPtr valuePtr = value->Des();

    TInt error = property->GetValue( valuePtr );

    User::LeaveIfError( error );

    // Cleanup and return
    CleanupStack::Pop( value );
    CleanupStack::PopAndDestroy( property );
    CleanupStack::PopAndDestroy( settings );

    RUBY_DEBUG2( "..Value of %d = %S", aPropertyName, value );

    return value;
    }

// ---------------------------------------------------------------------------
// Returns service id of the CS service
// ---------------------------------------------------------------------------
//
TInt VccSettingsReader::CSServiceIdL()
    {
    RUBY_DEBUG_BLOCK( "VccSettingsReader::CSServiceIdL" );

    // Get the service id by entry name
    return VccSettingsReader::ServiceIdByNameL( KCSEntryName );
    }

// ---------------------------------------------------------------------------
// Returns service id of the VoIP service
// ---------------------------------------------------------------------------
//
TInt VccSettingsReader::VoIPServiceIdL()
    {
    RUBY_DEBUG_BLOCK( "VccSettingsReader::VoIPServiceIdL" );

    // Get the VoIP service id.
    return VccSettingsReader::DoVoIPServiceIdL(
                    ESubPropertyVccDtVoipServiceId );
    }

// ---------------------------------------------------------------------------
// Returns the id of VCC enabled VoIP service.
// ---------------------------------------------------------------------------
//
TInt VccSettingsReader::DoVoIPServiceIdL(
                TServicePropertyName aPropertyName )
    {
    RUBY_DEBUG_BLOCK( "VccSettingsReader::DoVoIPServiceIdL" );

    // Create settings object and then the property one
    CSPSettings* settings = CSPSettings::NewLC();

    CSPProperty* property = CSPProperty::NewLC();

    // First we must find what is our vcc service id in sp-settings table
    // This is done by searching the vcc entry name.

    TInt vccServiceId( KErrNone );

    vccServiceId = VccSettingsReader::VccServiceIdL();
    
    // If the VCC service id was not found, leave.
    User::LeaveIfError( vccServiceId );

    // Now get the VCC enabled VoIP service id. The id is
    // saved into VCC settings.
    // First, get the property and then use the property to get its value.
    settings->FindPropertyL( vccServiceId, aPropertyName, *property );

    TInt voipId;
    TInt error = property->GetValue( voipId );

    User::LeaveIfError( error );

    // Cleanup and return
    CleanupStack::PopAndDestroy( property );
    CleanupStack::PopAndDestroy( settings );

    RUBY_DEBUG2( "..Value of %d = %d", aPropertyName, voipId );

    return voipId;
    }

// ---------------------------------------------------------------------------
// Returns the service id by given service entry name
// ---------------------------------------------------------------------------
//
TInt VccSettingsReader::ServiceIdByNameL( const TDesC& aEntryName )
    {
    RUBY_DEBUG_BLOCK( "VccSettingsReader::ServiceIdL" );
    RUBY_DEBUG1( "..Entry name to search = %S", &aEntryName );

    //  Array of service provider IDs
    // is defined as: typedef RArray<TServiceId> RIdArray
    // in spdefinitions.h
    RIdArray serviceIdArray;

    // KErrNotFound is the default return value if the entry
    // is not found.
    TInt serviceId( KErrNotFound );

    CleanupClosePushL( serviceIdArray );

    CSPSettings* settings = CSPSettings::NewLC();

    TInt error( KErrNone );

    // This should leave if error occurs
    error = settings->FindServiceIdsL( serviceIdArray );

    User::LeaveIfError( error );

    TBool foundEntry( EFalse );

    for ( TInt i( 0 ); i < serviceIdArray.Count() && !foundEntry; i++ )
        {
        CSPEntry* entry = CSPEntry::NewLC();
        error = settings->FindEntryL( serviceIdArray[i], *entry );

        User::LeaveIfError( error );

        if ( entry->GetServiceName() == aEntryName )
            {
            serviceId = entry->GetServiceId();
            foundEntry = ETrue;
            }

        CleanupStack::PopAndDestroy( entry );
        }
    CleanupStack::PopAndDestroy( settings );
    CleanupStack::PopAndDestroy( &serviceIdArray );

    RUBY_DEBUG1( "..Service id = %d", serviceId );

    return serviceId;
    }

// ---------------------------------------------------------------------------
// Returns service id of the VCC service
// ---------------------------------------------------------------------------
//
TInt VccSettingsReader::VccServiceIdL()
    {
    RUBY_DEBUG_BLOCK( "VccSettingsReader::VccServiceIdL" );

    TInt vccServiceId( KErrNone );
    TInt error( KErrNone );
    
    //  Array of service provider IDs
    // is defined as: typedef RArray<TServiceId> RIdArray
    // in spdefinitions.h
    RIdArray serviceIdArray;
    
    CleanupClosePushL( serviceIdArray );
    
    CSPSettings* settings = CSPSettings::NewLC();
    
    // This should leave if error occurs
    error = settings->FindServiceIdsL( serviceIdArray );
    
    User::LeaveIfError( error );
    
    TBool foundEntry( EFalse );
    
    for ( TInt i( 0 ); i < serviceIdArray.Count() && !foundEntry; i++ )
          {
          CSPProperty* property = CSPProperty::NewLC(); 
          error = settings->FindPropertyL( serviceIdArray[i], ESubPropertyVccVDI, *property ); 
       
          if( error == KErrNone )
              {
              vccServiceId =  serviceIdArray[i];
              foundEntry = ETrue;
              }
             CleanupStack::PopAndDestroy( property );
          }
    CleanupStack::PopAndDestroy( settings );
    CleanupStack::PopAndDestroy( &serviceIdArray );
    RUBY_DEBUG1( "VccService id = %d", vccServiceId );
    
    return vccServiceId;
    }

// ---------------------------------------------------------------------------
// Returns service id VCC should use for specified call provider plugin 
// ---------------------------------------------------------------------------
//
TInt VccSettingsReader::ServiceIdL( TInt aCallProviderPluginUid )
    {
    RUBY_DEBUG_BLOCK( "VccSettingsReader::ServiceIdL" );
    TInt id( KErrNone );
    
    switch ( aCallProviderPluginUid )
    	{
    	case KVccCallProviderPlugId:
    		{
    		id = VccSettingsReader::VccServiceIdL();
    		}break;
        case KCSCallProviderPlugId:
            {
            id = VccSettingsReader::CSServiceIdL();
            }break;
        default:
            {
            id = VccSettingsReader::VoIPServiceIdL();
            }break;
       	}
    
    return id;
    }


