/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CCCHSPSHandler implementation
*
*/


// INCLUDE FILES
#include <spsettings.h>
#include <spentry.h>
#include <spproperty.h>

#include "cchspshandler.h"
#include "cchlogger.h"

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES
// None

// CONSTANTS
// None

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ============================= LOCAL FUNCTIONS =============================

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// CCCHSPSHandler::CCCHSPSHandler
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CCCHSPSHandler::CCCHSPSHandler( CCCHServerBase& aServer ):
    iServer( aServer )
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// CCCHSPSHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CCCHSPSHandler::ConstructL()
    {
    iSettings = CSPSettings::NewL();
    }

// ---------------------------------------------------------------------------
// CCCHSPSHandler::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CCCHSPSHandler* CCCHSPSHandler::NewL( CCCHServerBase& aServer )
    {
    CCCHSPSHandler* self = CCCHSPSHandler::NewLC( aServer );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CCCHSPSHandler::NewLC
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CCCHSPSHandler* CCCHSPSHandler::NewLC( CCCHServerBase& aServer )
    {
    CCCHSPSHandler* self = new (ELeave) CCCHSPSHandler( aServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// Destructor
CCCHSPSHandler::~CCCHSPSHandler()
    {
    delete iSettings;
    iSettings = NULL;
    }

// ---------------------------------------------------------------------------
// CCCHSPSHandler::GetServicesCountL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CCCHSPSHandler::GetServicesCountL() const
    {
    CCHLOGSTRING2( "CCCHSPSHandler::GetServicesCountL : %d", 
        iSettings->SettingsCountL() );
            
    return iSettings->SettingsCountL();
    }

// ---------------------------------------------------------------------------
// CCCHSPSHandler::GetServiceIdsL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHSPSHandler::GetServiceIdsL( RArray<TUint>& aServiceIds ) const
    {
    CCHLOGSTRING( "CCCHSPSHandler::GetServiceIdsL: IN" );
    
    TRAP_IGNORE( iSettings->FindServiceIdsL( aServiceIds ) );
    
    CCHLOGSTRING2
        ( "CCCHSPSHandler::GetServiceIdsL: OUT: %d", aServiceIds.Count() );
    }

// ---------------------------------------------------------------------------
// CCCHSPSHandler::GetServiceInfoL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHSPSHandler::GetServiceInfoL( const TUint32 aServiceId, 
    TCCHService& aService ) const
    {
    CCHLOGSTRING( "CCCHSPSHandler::GetServiceInfoL: IN" );
    
    aService.iSubservices.Reset();
    
    // Get entry
    TInt err( KErrNone );
    CSPEntry* entry = CSPEntry::NewLC();
    err = iSettings->FindEntryL( aServiceId, *entry );
    
    if ( KErrNone == err )
        {
        // Read service id and service name
        aService.iServiceId = aServiceId;
        RBuf buf;
        CleanupClosePushL( buf );
        buf.CreateL( entry->GetServiceName(), KCCHMaxServiceNameLength );
        aService.iServiceName.Copy( buf );
        CleanupStack::PopAndDestroy( &buf );
        
        // Check which subservices are supported
        RPropertyNameArray propertyNameArray;
        CleanupClosePushL( propertyNameArray );
        propertyNameArray.AppendL( EPropertyVoIPSubServicePluginId );
        propertyNameArray.AppendL( EPropertyPresenceSubServicePluginId );
        propertyNameArray.AppendL( EPropertyIMSubServicePluginId );
        propertyNameArray.AppendL( EPropertyVMBXSubServicePluginId );
        
        for ( TInt i( 0 ) ; i < propertyNameArray.Count() ; i++ )
            {
            CSPProperty* property = CSPProperty::NewLC();
        
            err = iSettings->FindPropertyL( 
                aServiceId,
                propertyNameArray[ i ],
                *property );
            
            if ( !err && ValidSubserviceL( aServiceId, propertyNameArray[ i ] ) )
                {
                TCCHSubserviceType type = ECCHUnknown;        
                switch ( propertyNameArray[ i ] )
                    {
                    case EPropertyVoIPSubServicePluginId:
                        {
                        type = ECCHVoIPSub;
                        }
                        break;
                    case EPropertyPresenceSubServicePluginId:
                        {
                        type = ECCHPresenceSub;
                        }
                        break;
                    case EPropertyIMSubServicePluginId:
                        {
                        type = ECCHIMSub;
                        }
                        break;
                    case EPropertyVMBXSubServicePluginId:
                        {
                        type = ECCHVMBxSub;
                        }
                        break;
                    default:
                        {
                        type = ECCHUnknown;
                        break;
                        }
                    }
            
                CCHLOGSTRING2( "CCCHSPSHandler::GetServiceInfoL: subservice type=%d", type );
                
                aService.iSubservices[ i ].iConnectionInfo.iServiceSelection.iType = type;
                aService.iSubservices[ i ].iConnectionInfo.iServiceSelection.iServiceId = aServiceId;
                
                // These are just initialized here. Correct values are got from connectivity plugin
                aService.iSubservices[ i ].iState = ECCHUninitialized;
                aService.iSubservices[ i ].iSubserviceId = KErrNone;
                aService.iSubservices[ i ].iConnectionInfo.iSNAPId = KErrNone;
                aService.iSubservices[ i ].iConnectionInfo.iIapId = KErrNone;
                aService.iSubservices[ i ].iConnectionInfo.iSNAPLocked = EFalse;
                }
        
            CleanupStack::PopAndDestroy( property );
            }
        
        CleanupStack::PopAndDestroy( &propertyNameArray );
        }
    else
        {
        CCHLOGSTRING2( "CCCHSPSHandler::GetServiceInfoL: error getting entry err=%d", err );
        User::Leave( err );
        }
            
    CleanupStack::PopAndDestroy( entry );
    
    CCHLOGSTRING( "CCCHSPSHandler::GetServiceInfoL: OUT" );
    }

// ---------------------------------------------------------------------------
// CCCHSPSHandler::GetConnectivityPluginUidL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHSPSHandler::GetConnectivityPluginUidL( TUint32 aServiceId,
    TCCHSubserviceType aType, 
    TUid& aUid ) const
    {
    CCHLOGSTRING( "CCCHSPSHandler::GetConnectivityPluginUidL: IN" );
    CCHLOGSTRING2( "CCCHSPSHandler::GetConnectivityPluginUidL: aServiceId %d",
        aServiceId );
    CCHLOGSTRING2( "CCCHSPSHandler::GetConnectivityPluginUidL: aType %d",
        aType );
        
    TInt pUid( 0 );
    TInt error( KErrNone );
    TServicePropertyName propertyType( EPropertyUnknown ); 
    
    switch( aType )
        {        
        case ECCHVoIPSub:
            {
            propertyType = EPropertyVoIPSubServicePluginId;
            }
            break;
        case ECCHPresenceSub:
            {
            propertyType = EPropertyPresenceSubServicePluginId;
            }
            break;
        case ECCHIMSub:
            {
            propertyType = EPropertyIMSubServicePluginId;
            }
            break;
        case ECCHVMBxSub:
            {
            propertyType = EPropertyVMBXSubServicePluginId;
            }
            break;
        default:
            error = KErrNotFound;
            break;
        }
   
    if ( KErrNone == error )
        {
        CSPProperty* property = CSPProperty::NewLC();
        error = iSettings->FindPropertyL( aServiceId, 
            propertyType, *property );
            
        if ( KErrNone == error )
            {
            property->GetValue( pUid );
            // Convert TUint32 to TUid
            aUid = TUid::Uid( pUid );
            }
        else
            {
            CCHLOGSTRING2( "CCH: iSettings->FindPropertyL err: %d" , error );
            User::Leave( error );
            }
        CleanupStack::PopAndDestroy( property );
        }
        
    CCHLOGSTRING2( 
        "CCCHSPSHandler::GetConnectivityPluginUidL: aUid 0x%X", aUid );   
    }
        
// ---------------------------------------------------------------------------
// CCCHSPSHandler::LoadAtStartUpL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHSPSHandler::LoadAtStartUpL( TUint32 aServiceId,
    TCCHSubserviceType aType,
    TBool& aEnabled ) const
    {
    CCHLOGSTRING2( "CCCHSPSHandler::LoadAtStartUpL: aType %d", aType );   
    
    
    TSPItemType propertyType( EItemTypeNotDefined );
    TServicePropertyName propertyName( EPropertyUnknown );
    RPropertyArray subproperties;
    CleanupClosePushL( subproperties );
    
    ChangeToPropertyStyleL( aType, propertyType, propertyName );
           
    // Find correct subservice
    TInt err( iSettings->FindSubServicePropertiesL( 
        aServiceId, propertyType, subproperties ) );
    CCHLOGSTRING2( "CCH: iSettings->FindSubServicePropertiesL err: %d" , err );
    for ( TInt i( 0 ); i < subproperties.Count(); i++ )
        {
        // Read subservice's On/Off information 
        if ( propertyName == subproperties[ i ]->GetName() )
            {
            TOnOff onOff( EOONotSet );                
            subproperties[ i ]->GetValue( onOff );
            aEnabled = onOff == EOn;
            i = subproperties.Count();               
            }
        }    
    subproperties.ResetAndDestroy( );
    CleanupStack::PopAndDestroy( &subproperties );
    
    CCHLOGSTRING2( "CCCHSPSHandler::LoadAtStartUpL: aEnabled %d", aEnabled );  
    }

// ---------------------------------------------------------------------------
// CCCHSPSHandler::SetLoadAtStartUpL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHSPSHandler::SetLoadAtStartUpL( const TUint32 aServiceId,
    const TCCHSubserviceType aType,
    const TBool aOnOff ) const
    {
    CCHLOGSTRING2( "CCCHSPSHandler::SetLoadAtStartUpL: aType %d", aType );      
    CCHLOGSTRING2( "CCCHSPSHandler::SetLoadAtStartUpL: aOnOff %d", aOnOff );   
    TSPItemType propertyType( EItemTypeNotDefined );
    TServicePropertyName propertyName( EPropertyUnknown );
    RPropertyArray subproperties;
    CleanupClosePushL( subproperties );
    
    TOnOff onOff( EOONotSet );
    onOff = aOnOff ? EOn : EOff;
    
    // Get property name
    ChangeToPropertyStyleL( aType, propertyType, propertyName );
   
    CSPProperty* propLoadAtStartUp = CSPProperty::NewLC();
    
    propLoadAtStartUp->SetName( propertyName );
    propLoadAtStartUp->SetValue( onOff );
    
    subproperties.Append( propLoadAtStartUp );
    CleanupStack::Pop( propLoadAtStartUp );
    TInt err( iSettings->AddOrUpdatePropertiesL( aServiceId, subproperties ) );
    CCHLOGSTRING2( "CCH: iSettings->AddOrUpdatePropertiesL err: %d" , err );
    subproperties.ResetAndDestroy();
    CleanupStack::PopAndDestroy( &subproperties );
    }

// ---------------------------------------------------------------------------
// CCCHSPSHandler::ChangeToPropertyStyleL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHSPSHandler::ChangeToPropertyStyleL( 
    TCCHSubserviceType aType,
    TSPItemType& aPropertyType,
    TServicePropertyName& aPropertyName ) const
    {
    switch( aType )
        {
        case ECCHVoIPSub:
            {
            aPropertyType = EItemTypeVoIPSubProperty;
            aPropertyName = ESubPropertyVoIPEnabled;
            }
            break;
        case ECCHPresenceSub:
            {
            aPropertyType = EItemTypePresenceSubProperty;
            aPropertyName = ESubPropertyPresenceEnabled;
            }
            break;
        case ECCHIMSub:
            {            
            aPropertyType = EItemTypeIMSubProperty;
            aPropertyName = ESubPropertyIMEnabled;
            }
            break;
        case ECCHVMBxSub:
            {
            aPropertyType = EItemTypeVMBXSubProperty;
            aPropertyName = ESubPropertyVMBXEnabled;
            }
            break;
        default:
            {
            User::Leave( KErrNotFound );
            }            
            break;
        }        
    }

// ---------------------------------------------------------------------------
// CCCHSPSHandler::ValidSubserviceL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CCCHSPSHandler::ValidSubserviceL( 
    const TUint32 aServiceId,
    TServicePropertyName aPropertyName ) const
    {
    CCHLOGSTRING2
        ( "CCCHSPSHandler::ValidSubservice aServiceId: %d" , aServiceId );
        
    TBool valid( EFalse );
    TInt propertyId( KErrNone );
    
    CSPProperty* property = CSPProperty::NewLC();
    
    switch( aPropertyName )
        {
        case EPropertyVoIPSubServicePluginId:
            {
            CCHLOGSTRING( "Subservice: EPropertyVoIPSubServicePluginId" );
            
            // Until further notice VoIP property is always valid
            valid = ETrue;
            break;
            }
        case EPropertyPresenceSubServicePluginId:
            {
            CCHLOGSTRING( "Subservice: EPropertyPresenceSubServicePluginId" );
            
            valid = KErrNone != iSettings->FindPropertyL( aServiceId, 
                ESubPropertyPresenceSettingsId, *property ) ? EFalse :
                    KErrNone == property->GetValue( propertyId );
                                
            CCHLOGSTRING2( "CCH:     property id: %d" , propertyId );
            break;
            }
        case EPropertyIMSubServicePluginId:
            {
            CCHLOGSTRING( "Subservice: EPropertyIMSubServicePluginId" );
            
            valid = KErrNone != iSettings->FindPropertyL( aServiceId, 
                ESubPropertyIMSettingsId, *property ) ? EFalse :
                    KErrNone == property->GetValue( propertyId );
            
            CCHLOGSTRING2( "CCH:     property id: %d" , propertyId );
            break;
            }
        case EPropertyVMBXSubServicePluginId:
            {
            CCHLOGSTRING( "Subservice: EPropertyVMBXSubServicePluginId" );
            
            valid = KErrNone != iSettings->FindPropertyL( aServiceId, 
                ESubPropertyVMBXSettingsId, *property ) ? EFalse :
                    KErrNone == property->GetValue( propertyId );
            
            CCHLOGSTRING2( "CCH:     property id: %d" , propertyId );
            break;
            }
        default:
            {
            // Let other properties go through
            valid = EFalse;
            break;
            }
        }
    
    CleanupStack::PopAndDestroy( property );
    
    CCHLOGSTRING2
        ( "CCCHSPSHandler::ValidSubservice valid: %d" , valid );
                    
    return valid;        
    }
        
// ========================== OTHER EXPORTED FUNCTIONS =======================

//  End of File
