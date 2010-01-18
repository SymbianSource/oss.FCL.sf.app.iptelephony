/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
    TInt err( KErrNone );
    // Get entry
    CSPEntry* entry = CSPEntry::NewLC();
    err = iSettings->FindEntryL( aServiceId, *entry );
    CCHLOGSTRING2( "iSettings->FindEntryL err: %d", err );
    if ( KErrNone == err )
        {
        // Read service id and service name
        aService.iServiceId = aServiceId;
        RBuf buf;
        buf.Create( entry->GetServiceName(), KCCHMaxServiceNameLength );
        aService.iServiceName.Copy( buf );
        buf.Close();
        
        // get all service properties a.k.a subservices
        RPropertyArray properties;
        properties = entry->GetAllProperties();
        
        TServicePropertyName propertyName;
        TUint count( 0 );
        TInt pServiceId( 0 );
        TInt pSnap( 0 );
        TInt pIap( 0 );
	    TBool pSnapLocked( EFalse );
        
        for ( TInt i( 0 ); i < properties.Count() &&
            count < KCCHMaxSubservicesCount; i++ )
            {
            propertyName = properties[ i ]->GetName();
            
            // Check is subservice valid
            TBool valid( EFalse );
            TRAPD( error, valid = ValidSubserviceL( aServiceId, propertyName ) );
            if ( KErrNone == error && valid )
                {
    	        aService.iSubservices[ count ].iConnectionInfo.iServiceSelection.iType = 
                    propertyName == 
                        EPropertyVoIPSubServicePluginId ? ECCHVoIPSub : 
                    propertyName == 
                        EPropertyPresenceSubServicePluginId ? ECCHPresenceSub : 
                    propertyName == 
                        EPropertyIMSubServicePluginId ? ECCHIMSub : 
                    propertyName == 
                        EPropertyVMBXSubServicePluginId ? ECCHVMBxSub : 
                    ECCHUnknown;
                
                if ( ECCHUnknown != aService.iSubservices[ count ].iConnectionInfo.iServiceSelection.iType )
                    {
                    aService.iSubservices[ count ].iConnectionInfo.iServiceSelection.iServiceId = aServiceId;
                    aService.iSubservices[ count ].iState     = ECCHUninitialized;              
                    
                    GetSettingsIdL( aServiceId, 
                        aService.iSubservices[ count ].iConnectionInfo.iServiceSelection.iType, 
                        pServiceId );
                    aService.iSubservices[ count ].iSubserviceId = 
                        static_cast<TUint> ( pServiceId );
                    
                    GetSNAPIdL( aServiceId, 
                        aService.iSubservices[ count ].iConnectionInfo.iServiceSelection.iType, 
                        pSnap );
                    aService.iSubservices[ count ].iConnectionInfo.iSNAPId = 
                        static_cast<TUint32> ( pSnap );
                    
                    GetSNAPIdL( aServiceId, 
                        aService.iSubservices[ count ].iConnectionInfo.iServiceSelection.iType, 
                        pSnapLocked );
                    aService.iSubservices[ count ].iConnectionInfo.iSNAPLocked = 
                        static_cast<TUint32> ( pSnapLocked );
                    
                    GetIapIdL( aServiceId, 
                        aService.iSubservices[ count ].iConnectionInfo.iServiceSelection.iType, 
                        pIap );
                    aService.iSubservices[ count ].iConnectionInfo.iIapId  = 
                            static_cast<TUint32> ( pIap ); 
                        
                    count++;
                    }
                }
            }    
        }    
    else
        {
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
// CCCHSPSHandler::GetSNAPIdL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHSPSHandler::GetSNAPIdL( TUint32 aServiceId,
    TCCHSubserviceType aType,
    TInt& aSNAPId ) const
    {
    CCHLOGSTRING( "CCCHSPSHandler::GetSNAPIdL: IN" );
    CCHLOGSTRING2( "CCCHSPSHandler::GetSNAPIdL: aServiceId %d", aServiceId );
    CCHLOGSTRING2( "CCCHSPSHandler::GetSNAPIdL: aType %d", aType );
    
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
        // Read subservice's SNAP Id
        if ( propertyName == subproperties[ i ]->GetName() )
            {                
            subproperties[ i ]->GetValue( aSNAPId );
            i = subproperties.Count();               
            }
        }    
    
    subproperties.ResetAndDestroy( );
    CleanupStack::PopAndDestroy( &subproperties );
    CCHLOGSTRING2( "CCCHSPSHandler::GetSNAPIdL: aSNAPId %d", aSNAPId );
    }

// ---------------------------------------------------------------------------
// CCCHSPSHandler::GetIapIdL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHSPSHandler::GetIapIdL( TUint32 aServiceId,
    TCCHSubserviceType aType,
    TInt& aIapId ) const
    {
    CCHLOGSTRING( "CCCHSPSHandler::GetIapIdL: IN" );
    CCHLOGSTRING2( "CCCHSPSHandler::GetIapIdL: aServiceId %d", aServiceId );
    CCHLOGSTRING2( "CCCHSPSHandler::GetIapIdL: aType %d", aType );
    
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
        // Read subservice's IAP Id
        if ( propertyName == subproperties[ i ]->GetName() )
            {                
            subproperties[ i ]->GetValue( aIapId );
            i = subproperties.Count();               
            }
        }    
    
    subproperties.ResetAndDestroy( );
    CleanupStack::PopAndDestroy( &subproperties );
    CCHLOGSTRING2( "CCCHSPSHandler::GetIapIdL: aIapId %d", aIapId );    
    }

// ---------------------------------------------------------------------------
// CCCHSPSHandler::GetSettingsIdL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHSPSHandler::GetSettingsIdL( TUint32 aServiceId,
    TCCHSubserviceType aType,
    TInt& aId ) const
    {
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
        // Read subservice's Id
        if ( propertyName == subproperties[ i ]->GetName() )
            {                
            TInt id( 0 );
            subproperties[ i ]->GetValue( id );
            aId = id;
            i = subproperties.Count();               
            }
        }    
    
    subproperties.ResetAndDestroy( );
    CleanupStack::PopAndDestroy( &subproperties );
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
            
            // Until further notice IM property is always valid
            valid = ETrue;
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
            valid = ETrue;
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
