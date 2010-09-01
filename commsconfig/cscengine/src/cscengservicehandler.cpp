/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  For CSC needed Service Provider Settings handling
*
*/


#include <spentry.h>
#include <spproperty.h>
#include <spsettings.h>
#include <spnotifychange.h>
#include <spsettingsvoiputils.h>
#include <centralrepository.h>
#include <settingsinternalcrkeys.h>

#include "cscenglogger.h"
#include "cscengservicehandler.h"
#include "mcscengserviceobserver.h"

const TInt KDummyServiceCount = 1;// only remove cs service, VoIP service is not returned
const TInt KArrayInitSize = 2;

// SCP-plugin uid for Sip VoIP
const TInt32 KSipVoIPSubServicePlugId = 0x1027545A;
// Preferred Telephony value is PS call
const TInt KRCSEInternetCallPreferred = 1;
// Preferred Telephony value is CS call
const TInt KRCSECSCallPreferred = 0;

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCEngServiceHandler::CCSCEngServiceHandler()
    {    
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CCSCEngServiceHandler::ConstructL( MCSCEngServiceObserver* aObserver )
    {
    CSCENGDEBUG( "CCSCEngServiceHandler::ConstructL - begin" ); 
    
    iObserver = aObserver;
    iServiceSettings = CSPSettings::NewL();
    iNotifier = CSPNotifyChange::NewL( *this );
    iSettingsVoIPUtils = CSPSettingsVoIPUtils::NewL();
           
    RArray<TUint> serviceIds;
    iNotifier->NotifyChangeL( serviceIds );
    
    CSCENGDEBUG( "CCSCEngServiceHandler::ConstructL - end" ); 
    }   
    
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CCSCEngServiceHandler* CCSCEngServiceHandler::NewL( 
    MCSCEngServiceObserver* aObserver )
    {    
    CCSCEngServiceHandler* self = new (ELeave) CCSCEngServiceHandler;
    CleanupStack::PushL( self );
    self->ConstructL( aObserver );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCEngServiceHandler::~CCSCEngServiceHandler()
    {
    CSCENGDEBUG( 
        "CCSCEngServiceHandler::~CCSCEngCenrepHandler - begin" ); 
    
    iServiceName.Close();
    delete iSettingsVoIPUtils;
    delete iServiceSettings;
    delete iNotifier;
    
    CSCENGDEBUG( 
        "CCSCEngServiceHandler::~CCSCEngCenrepHandler - end" ); 
    }
    

// ---------------------------------------------------------------------------
// For getting all service ids
// ---------------------------------------------------------------------------
//
EXPORT_C void CCSCEngServiceHandler::GetAllServiceIdsL( 
    RArray<TUint>& aServiceIds )
    {
    CSCENGDEBUG( "CCSCEngServiceHandler::GetAllServiceIds - begin" ); 
    
    iServiceSettings->FindServiceIdsL( aServiceIds );
    
    // Remove 'dummy service's' from service ids list.
    if ( KDummyServiceCount <= aServiceIds.Count() )
        {
        for ( TUint i = 0; i < KDummyServiceCount; i++ )
            {
            aServiceIds.Remove( 0 );
            }
        }
        
    CSCENGDEBUG( "CCSCEngServiceHandler::GetAllServiceIds - end" ); 
    }


// ---------------------------------------------------------------------------
// For getting service provider settings count
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCSCEngServiceHandler::ServiceIdCountL()
    {
    CSCENGDEBUG2( 
        "CCSCEngServiceHandler::ServiceIdCount COUNT=%d", 
            iServiceSettings->SettingsCountL() );
    
    return iServiceSettings->SettingsCountL();
    }


// ---------------------------------------------------------------------------
// For deleting service
// ---------------------------------------------------------------------------
//
EXPORT_C void CCSCEngServiceHandler::DeleteServiceL( TUint aServiceId )
    {
    CSCENGDEBUG( "CCSCEngServiceHandler::DeleteServiceL - begin" );
    
    User::LeaveIfError( iServiceSettings->DeleteEntryL( aServiceId ) );
    
    CSCENGDEBUG( "CCSCEngServiceHandler::DeleteServiceL - end" );
    }


// ---------------------------------------------------------------------------
// For getting service name
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC16& CCSCEngServiceHandler::ServiceNameL( 
    TUint aServiceId )
    {
    CSCENGDEBUG( "CCSCEngServiceHandler::ServiceNameL - begin" );
    
    iServiceName.Zero();
    RArray<TUint> serviceIds;
    CleanupClosePushL( serviceIds );
    
    GetAllServiceIdsL( serviceIds );
    
    CDesCArray* serviceNames = 
        new ( ELeave ) CDesCArrayFlat( KArrayInitSize );
    CleanupStack::PushL( serviceNames );
    
    iServiceSettings->FindServiceNamesL( serviceIds, *serviceNames );
    
    TBool serviceFound( EFalse );
    for ( TInt i( 0 ) ; i < serviceIds.Count() && !serviceFound ; i++ )
        {
        if ( aServiceId == serviceIds[ i ] )
            {
            serviceFound = ETrue;
            iServiceName.Close();
            iServiceName.CreateL( serviceNames->MdcaPoint( i ) );
            }
        }
    
    if ( !serviceFound )
        {
        User::Leave( KErrNotFound ); // leave if service not found
        }
    
    CleanupStack::PopAndDestroy( serviceNames );	
    CleanupStack::PopAndDestroy( &serviceIds );
    
    CSCENGDEBUG( "CCSCEngServiceHandler::ServiceNameL - end" );
            
    return iServiceName;
    }

// ---------------------------------------------------------------------------
// For getting brand id
// ---------------------------------------------------------------------------
//
EXPORT_C void CCSCEngServiceHandler::BrandIdL( 
    TUint aServiceId, TDes16& aBrandId )
    {
    CSCENGDEBUG( "CCSCEngServiceHandler::BrandIdL - begin" ); 
    
    CSPProperty* property = CSPProperty::NewLC();
    
    User::LeaveIfError( iServiceSettings->FindPropertyL( 
        aServiceId, EPropertyBrandId, *property ) );
       
    User::LeaveIfError( property->GetValue( aBrandId ) );
    
    CleanupStack::PopAndDestroy( property );
    
    CSCENGDEBUG2( "CCSCEngServiceHandler::BrandIdL BRANDID=%S", &aBrandId );
    
    CSCENGDEBUG( "CCSCEngServiceHandler::BrandIdL - end" ); 
    }      

// ---------------------------------------------------------------------------
// For getting snap id
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCSCEngServiceHandler::SnapIdL( TUint aServiceId )
    {
    CSCENGDEBUG( "CCSCEngServiceHandler::SnapIdL - begin" ); 
    
    CSPProperty* property = CSPProperty::NewLC();
    TInt snapId( 0 );
    
    User::LeaveIfError( iServiceSettings->FindPropertyL( 
        aServiceId, ESubPropertyVoIPPreferredSNAPId, *property ) );
       
    User::LeaveIfError( property->GetValue( snapId ) );
    
    CleanupStack::PopAndDestroy( property );
    
    CSCENGDEBUG( "CCSCEngServiceHandler::SnapIdL - end" ); 
    
    return snapId;
    }  

// ---------------------------------------------------------------------------
// For getting service setup plug-in id
// ---------------------------------------------------------------------------
//
EXPORT_C TInt32 CCSCEngServiceHandler::ServiceSetupPluginIdL( 
    TUint aServiceId )
    {
    CSCENGDEBUG( "CCSCEngServiceHandler::ServiceSetupPluginId - begin" ); 
    
    CSPProperty* property = CSPProperty::NewLC();
     
    User::LeaveIfError( iServiceSettings->FindPropertyL( 
        aServiceId, EPropertyServiceSetupPluginId, *property ) );
    
    TInt serviceSetupPluginId( 0 );   
    User::LeaveIfError( property->GetValue( serviceSetupPluginId ) ); 
          
    CleanupStack::PopAndDestroy( property );
    
    CSCENGDEBUG( "CCSCEngServiceHandler::ServiceSetupPluginId - end" ); 
    
    return serviceSetupPluginId;
    }      


// ---------------------------------------------------------------------------
// For getting service큦 phonebook tab view id
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCSCEngServiceHandler::ServiceTabViewIdL( TUint aServiceId )
    {
    CSPProperty* property = CSPProperty::NewLC();
     
    User::LeaveIfError( iServiceSettings->FindPropertyL( 
        aServiceId, EPropertyContactViewId, *property ) );
    
    TInt tabViewId( 0);                   
    User::LeaveIfError( property->GetValue( tabViewId ) );     
    
    CleanupStack::PopAndDestroy( property );

    CSCENGDEBUG2( "CCSCEngServiceHandler::ServiceTabViewIdL: %d", tabViewId );
    
    return tabViewId;
    }      

// ---------------------------------------------------------------------------
// For getting service큦 IM settings id.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCSCEngServiceHandler::ImSettingsIdL( TUint aServiceId )
    {
    CSCENGDEBUG( "CCSCEngServiceHandler::ImSettingsIdL - begin" ); 
    
    CSPProperty* property = CSPProperty::NewLC();
     
    User::LeaveIfError( iServiceSettings->FindPropertyL( 
        aServiceId, ESubPropertyIMSettingsId, *property ) );
    
    TInt settingsId( 0 );                   
    User::LeaveIfError( property->GetValue( settingsId ) );    
    
    CleanupStack::PopAndDestroy( property );

    CSCENGDEBUG( "CCSCEngServiceHandler::ImSettingsIdL - end" );
    
    return settingsId;
    }  

// ---------------------------------------------------------------------------
// For setting service큦 IM settings id.
// ---------------------------------------------------------------------------
//
EXPORT_C void CCSCEngServiceHandler::SetImSettingsIdL(
    TUint aServiceId, TInt aSettingsId )
    {
    CSCENGDEBUG( "CCSCEngServiceHandler::SetImSettingsIdL - begin" ); 
    
    CSPProperty* property = CSPProperty::NewLC();
    
    User::LeaveIfError( 
        property->SetName( ESubPropertyIMSettingsId ) );
    User::LeaveIfError( property->SetValue( aSettingsId ) );
    
    iServiceSettings->AddOrUpdatePropertyL( aServiceId, *property );
    
    CleanupStack::PopAndDestroy( property );
    
    CSCENGDEBUG( "CCSCEngServiceHandler::SetImSettingsIdL - end" );
    } 

// ---------------------------------------------------------------------------
// For getting service큦 presence request preference setting
// ---------------------------------------------------------------------------
//
EXPORT_C TOnOff CCSCEngServiceHandler::PresenceReqPrefL( TUint aServiceId )
    {
    CSCENGDEBUG( "CCSCEngServiceHandler::PresenceReqPrefL" ); 
    
    CSPProperty* property = CSPProperty::NewLC();
    
    User::LeaveIfError( iServiceSettings->FindPropertyL( 
        aServiceId, ESubPropertyPresenceRequestPreference , *property ) );
    
    TOnOff onOff = EOn;    
    User::LeaveIfError( property->GetValue( onOff ) );
    
    CleanupStack::PopAndDestroy( property );
    
    return onOff;
    } 


// ---------------------------------------------------------------------------
// For setting service큦 presence request preference setting
// ---------------------------------------------------------------------------
//
EXPORT_C void CCSCEngServiceHandler::SetPresenceReqPrefL( 
    TUint aServiceId, TOnOff aOnOff )
    {
    CSCENGDEBUG2( 
        "CCSCEngServiceHandler::SetPresenceReqPrefL: aOnOff=%d", aOnOff ); 
    
    CSPProperty* property = CSPProperty::NewLC();
    User::LeaveIfError( 
        property->SetName( ESubPropertyPresenceRequestPreference ) );
    User::LeaveIfError( property->SetValue( aOnOff ) );
    
    iServiceSettings->AddOrUpdatePropertyL( aServiceId, *property );
    CleanupStack::PopAndDestroy( property );
    } 


// ---------------------------------------------------------------------------
// For getting service큦 handover notification tone preference setting
// ---------------------------------------------------------------------------
//
EXPORT_C TOnOff CCSCEngServiceHandler::HandoverNotifTonePrefL( 
    TUint aServiceId )
    {
    CSCENGDEBUG( "CCSCEngServiceHandler::HandoverNotifTonePrefL" ); 

    CSPProperty* property = CSPProperty::NewLC();
         
    User::LeaveIfError( iServiceSettings->FindPropertyL( 
        aServiceId, ESubPropertyVccHoNotificationToneMode , *property ) );
         
    RBuf value( 0 );  
    CleanupClosePushL( value );
    value.CreateL( 1 );
    User::LeaveIfError( property->GetValue( value ) );
         
    // convert rbuf to int
    TInt intValue;
    TLex temp( value );
    User::LeaveIfError( temp.Val( intValue ) );
      
    CleanupStack::PopAndDestroy( &value );
    CleanupStack::PopAndDestroy( property );
         
    TOnOff onOff = EOONotSet;
      
    switch ( intValue )
        {
        case 0:
            {
            onOff = EOff;
            }
            break;
        case 1:
            {
            onOff = EOn;
            }
            break;
        default:
            {
            onOff = EOONotSet;
            }
            break;
        }

    CSCENGDEBUG2( "CCSCEngServiceHandler::HandoverNotifTonePrefL: %d", onOff ); 
      
    return onOff;
    }
        

// ---------------------------------------------------------------------------
// For setting service큦 handover notification tone preference setting
// ---------------------------------------------------------------------------
//
EXPORT_C void CCSCEngServiceHandler::SetHandoverNotifTonePrefL( 
    TUint aServiceId, TOnOff aOnOff )
    {
    CSPProperty* property = CSPProperty::NewLC();
    User::LeaveIfError( 
        property->SetName( ESubPropertyVccHoNotificationToneMode ) );
    
    RBuf value;
    CleanupClosePushL( value );
    value.CreateL( 1 );
    value.AppendNum( aOnOff );
    User::LeaveIfError( property->SetValue( value ) );
       
    iServiceSettings->AddOrUpdatePropertyL( aServiceId, *property );
    
    CleanupStack::PopAndDestroy( &value );
    CleanupStack::PopAndDestroy( property );
    }
        

// ---------------------------------------------------------------------------
// For finding out if service supports VCC
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CCSCEngServiceHandler::IsVccSupportedL( TUint aServiceId )
    {
    TBool supported( EFalse );
    
    TInt vccServiceId( KErrNone );
    TInt error( KErrNone );
        
    //  Array of service provider IDs
    // is defined as: typedef RArray<TServiceId> RIdArray
    // in spdefinitions.h
    RIdArray serviceIdArray;     
    CleanupClosePushL( serviceIdArray );

    // This should leave if error occurs
    error = iServiceSettings->FindServiceIdsL( serviceIdArray );    
    User::LeaveIfError( error );
            
    TInt serviceId( KErrNone );
    TBool foundEntry( EFalse );
        
    for ( TInt i( 0 ); i < serviceIdArray.Count() && !foundEntry; i++ )
        {
        CSPProperty* property = CSPProperty::NewLC(); 
        error = iServiceSettings->FindPropertyL(
            serviceIdArray[i], ESubPropertyVccVDI, *property ); 
           
        if( error == KErrNone )
            {            
            vccServiceId =  serviceIdArray[i];
            foundEntry = ETrue;
            }
        CleanupStack::PopAndDestroy( property );
        }
    
    CleanupStack::PopAndDestroy( &serviceIdArray );
    
    if ( vccServiceId )
        {
        CSPProperty* property2 = CSPProperty::NewLC(); 
        
        iServiceSettings->FindPropertyL( 
            vccServiceId,
            ESubPropertyVccDtVoipServiceId,
            *property2 );

        TInt error = property2->GetValue( serviceId );
        User::LeaveIfError( error );
        CleanupStack::PopAndDestroy( property2 );
        }    
    
    if ( serviceId == aServiceId )
        {
        supported = ETrue;
        }
    
    CSCENGDEBUG2( "CCSCEngServiceHandler::IsVccSupportedL: %d", supported );
    
    return supported;
    }


// ---------------------------------------------------------------------------
// For finding out if service supports SIP/VoIP
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CCSCEngServiceHandler::IsSipVoIPL( TUint aServiceId )
    {
    TBool sipVoIP( EFalse );
    TInt uid( 0 );
    
    CSPProperty* property = CSPProperty::NewLC();
    
    User::LeaveIfError( iServiceSettings->FindPropertyL( 
        aServiceId, EPropertyVoIPSubServicePluginId , *property ) );
    
    User::LeaveIfError( property->GetValue( uid ) );
    
    if ( KSipVoIPSubServicePlugId == uid )
        {
        sipVoIP = ETrue;
        }
    
    CSCENGDEBUG2( "CCSCEngServiceHandler::IsSipVoIP: %d", sipVoIP );
    
    CleanupStack::PopAndDestroy( property );
    
    return sipVoIP;
    } 

// ---------------------------------------------------------------------------
// For finding out if service is the preferred service
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CCSCEngServiceHandler::IsPreferredService( TUint aServiceId )
    const
    {
    TUint service( 0 );
    TInt ret = iSettingsVoIPUtils->GetPreferredService( service );
    
    return ( KErrNone == ret && service == aServiceId );
    } 

// ---------------------------------------------------------------------------
// For finding out if voip is preferred telephony value
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CCSCEngServiceHandler::IsPreferredTelephonyVoip() const
    {
    return iSettingsVoIPUtils->IsPreferredTelephonyVoIP();
    } 

// ---------------------------------------------------------------------------
// Sets the preferred telephony value
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCSCEngServiceHandler::SetVoipAsPrefTelephony( 
    TBool aValue,
    TUint aServiceId ) const
    {
    TRAPD( err, SetVoipAsPrefTelephonyL( aValue, aServiceId ) );
    return err;
    }
    
// ---------------------------------------------------------------------------
// Sets the preferred telephony value
// ---------------------------------------------------------------------------
//
void CCSCEngServiceHandler::SetVoipAsPrefTelephonyL( 
    TBool aValue, 
    TUint aServiceId ) const
    {  
    TInt value( aValue ? KRCSEInternetCallPreferred : KRCSECSCallPreferred );
    TInt serviceId( aServiceId );
          
    CRepository* repository = CRepository::NewLC( KCRUidRichCallSettings );
    
    User::LeaveIfError( repository->Set( KRCSEPreferredTelephony, value ) );
    User::LeaveIfError( repository->Set( KRCSPSPreferredService, serviceId ) );
    CleanupStack::PopAndDestroy( repository );
    }

// ---------------------------------------------------------------------------
// From class MSPNotifyChangeObserver
// Notifies change in service table
// ---------------------------------------------------------------------------
//
void CCSCEngServiceHandler::HandleNotifyChange( TUint /*aServiceId*/ )
    {
    CSCENGDEBUG( "CCSCEngServiceHandler::HandleNotifyChange - begin" );
   
    iObserver->NotifyServiceChange();
     
    CSCENGDEBUG( "CCSCEngServiceHandler::HandleNotifyChange - end" );  
    }

