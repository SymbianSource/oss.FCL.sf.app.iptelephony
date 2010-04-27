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
* Description:   SP Settings handler for Vcc settings, handles caching and 
                 storing data related to Vcc settings.
*
*/



#include <charconv.h>
#include <in_sock.h>
#include <spsettings.h>
#include <spentry.h>
#include <spproperty.h>
#include <badesca.h>
#include <e32cmn.h>

#include "vccspsettings.h"
#include "vccmiscutils.h"
#include "rubydebug.h"

/** VCC call provider plugin uid */
const TInt32 KVccCallProviderPlugId = 0x2000CFAA; 

/** Zero ID */
const TUint KVccZeroId = 0;

/** Service Id string length */
const TInt KVccServiceIdLength = 8;

/** Names array size */
const TInt KVccNameArraySize = 10;

// ======== MEMBER FUNCTIONS ========
// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CVccSPSettings::CVccSPSettings()
    {
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase Constructor
// ---------------------------------------------------------------------------
//
void CVccSPSettings::ConstructL()
    {
    RUBY_DEBUG_BLOCKL( "CVccSPSettings::ConstructL" );
    iSettings = CSPSettings::NewL();
    }
 
// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//   
EXPORT_C CVccSPSettings* CVccSPSettings::NewL()
    {
    RUBY_DEBUG_BLOCKL( "CVccSPSettings::NewL" );
    
    CVccSPSettings* self = new ( ELeave ) CVccSPSettings();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CVccSPSettings::~CVccSPSettings()
    {
    RUBY_DEBUG0( "CVccSPSettings::~CVccSPSettings START" );
    
    delete iProviderId;
    delete iServiceName;
    delete iVdi;
    delete iVdn;
    delete iPreferredDomain;
    delete iImmediateDt;
    delete iDtCsToPsAllowed;
    delete iDtPsToCsAllowed;
    delete iDtHeldWaitingCallsAllowed;
    delete iDtWlanHoTreshold;
    delete iDtWlanHoHysteresis;
    delete iDtWlanHoHysteresisTimerLow;
    delete iDtWlanHoHysteresisTimerHigh;
    delete iDtCsHoTreshold;
    delete iDtCsHoHysteresis;
    delete iDtCsHoHysteresisTimerLow;
    delete iDtCsHoHysteresisTimerHigh;
    delete iVoipServiceIdString;
    delete iDtAllowedWhenCsOriginated;
    
    delete iSettings;
    RUBY_DEBUG0( "CVccSPSettings::~CVccSPSettings EXIT" );
    }

// ---------------------------------------------------------------------------
// Stores cached Vcc settings.
// ---------------------------------------------------------------------------
//
EXPORT_C void CVccSPSettings::StoreL()
    {
    RUBY_DEBUG_BLOCK( "CVccSPSettings::StoreL" );
    RUBY_DEBUG1( "CVccSPSettings::StoreL instance %x", this );
 
    CSPEntry* entry = CSPEntry::NewLC();
    
    RUBY_DEBUG1( "-service name:[%S]",iServiceName );
   
    TInt error( KErrNone );
    RIdArray serviceids;
    CleanupClosePushL( serviceids );
    iSettings->FindServiceIdsL( serviceids );
    
    TBool foundEntry( EFalse );
          
      //look for VCC service
      for ( TInt i( 0 ); i < serviceids.Count() && !foundEntry; i++ )
            {
            CSPProperty* property = CSPProperty::NewLC(); 
            error = iSettings->FindPropertyL( serviceids[i], ESubPropertyVccVDI, *property ); 
   
            if( error == KErrNone )
                {
                RUBY_DEBUG0( "-service found" );
                iSettings->FindEntryL( serviceids[i], *entry );
                foundEntry = ETrue;
                }
            CleanupStack::PopAndDestroy( property );
            }
      
      if ( !foundEntry ) //if service not found then add new
          {
          //add entry fails if name is not set
           RUBY_DEBUG0( "add new service" ); 
           entry->SetServiceName( *iServiceName ); 
           User::LeaveIfError( iSettings->AddEntryL( *entry ));
          }
     CleanupStack::PopAndDestroy( &serviceids );
     iServiceId = entry->GetServiceId();

    ModifyVoipProfileL();
    
    TInt err = 0;
    /*
    
    CSPProperty* voipProperty = CSPProperty::NewLC(); 
    TInt err = iSettings->FindPropertyL( iVoipServiceId, 
    			EPropertyCallProviderPluginId,
    			*voipProperty );
    
    if( err == KErrNone)
    	{
    	voipProperty->SetValue( KVccCallProviderPlugId );
    	User::LeaveIfError( iSettings->AddOrUpdatePropertyL( iVoipServiceId, 
    						*voipProperty ) );
    	}
    CleanupStack::PopAndDestroy( voipProperty );
    
    voipProperty = CSPProperty::NewLC(); 
    err = iSettings->FindPropertyL( iVoipServiceId, 
                                         EPropertyServiceAttributeMask,
                                         *voipProperty );
    
    
    //Set bootstrap bit on, so that CCE cant unload VCC
    //Should be figured out how do it so that inactive VoIP service
    //is still unloaded, but VCC is not.
    if( err == KErrNone)
        {
        TInt mask;
        voipProperty->GetValue( mask );
                
        mask = mask | EBootstrapCallProvider;
                        
        voipProperty->SetValue( mask );
        User::LeaveIfError( iSettings->AddOrUpdatePropertyL( iVoipServiceId, 
                                *voipProperty ) );
        }
    CleanupStack::PopAndDestroy( voipProperty );
    */
    
    
    //check existing properties from entry
    if( iVdi != NULL )
        AddOrUpdatePropertyL( *entry, ESubPropertyVccVDI, *iVdi );
    
    if( iVdn != NULL )
        AddOrUpdatePropertyL( *entry, ESubPropertyVccVDN, *iVdn );
    
    if( iPreferredDomain != NULL )
        AddOrUpdatePropertyL( *entry, ESubPropertyVccPreferredDomain, 
                        *iPreferredDomain );
    
    if( iImmediateDt != NULL )
        AddOrUpdatePropertyL( *entry, ESubPropertyVccImmediateDomainTransfer, 
                        *iImmediateDt );
    
    if( iDtCsToPsAllowed != NULL )
        AddOrUpdatePropertyL( *entry, ESubPropertyVccDtCstoPsAllowed, 
                        *iDtCsToPsAllowed );
    
    if( iDtPsToCsAllowed != NULL )
        AddOrUpdatePropertyL( *entry, ESubPropertyVccDtPstoCsAllowed, 
                        *iDtPsToCsAllowed );

    if( iDtHeldWaitingCallsAllowed != NULL )
        AddOrUpdatePropertyL( *entry, ESubPropertyVccDtHeldWaitingCallsAllowed, 
                        *iDtHeldWaitingCallsAllowed );

    if( iDtWlanHoTreshold != NULL )    
        AddOrUpdatePropertyL( *entry,  ESubPropertyVccDtWLANHoTriggerLevel, 
                        *iDtWlanHoTreshold );

    if( iDtWlanHoHysteresis != NULL )
        AddOrUpdatePropertyL( *entry,  ESubPropertyVccDtWLANHoHysteresis, 
                        *iDtWlanHoHysteresis );

    if( iDtWlanHoHysteresisTimerLow != NULL )
        AddOrUpdatePropertyL( *entry, ESubPropertyVccDtWLANHoHysteresisTimerLow, 
                        *iDtWlanHoHysteresisTimerLow );

    if( iDtWlanHoHysteresisTimerHigh != NULL )
        AddOrUpdatePropertyL( *entry, ESubPropertyVccDtWLANHoHysteresisTimerHigh, 
                        *iDtWlanHoHysteresisTimerHigh );

    if( iDtCsHoTreshold != NULL )
        AddOrUpdatePropertyL( *entry, ESubPropertyVccDtCSHoTriggerLevel, 
                        *iDtCsHoTreshold );

    if( iDtCsHoHysteresis != NULL )
        AddOrUpdatePropertyL( *entry, ESubPropertyVccDtCSHoHysteresis, 
                        *iDtCsHoHysteresis );

    if( iDtCsHoHysteresisTimerLow != NULL )
        AddOrUpdatePropertyL( *entry, ESubPropertyVccDtCSHoHysteresisTimerLow, 
                        *iDtCsHoHysteresisTimerLow );

    if( iDtCsHoHysteresisTimerHigh != NULL )
        AddOrUpdatePropertyL( *entry, ESubPropertyVccDtCSHoHysteresisTimerHigh, 
                        *iDtCsHoHysteresisTimerHigh );     

    if( iVoipServiceIdString != NULL )
        AddOrUpdatePropertyL( *entry, ESubPropertyVccDtVoipServiceId, 
                        *iVoipServiceIdString );     
    
    if( iDtAllowedWhenCsOriginated != NULL )
        AddOrUpdatePropertyL( *entry, ESubPropertyVccDtAllowedWhenCsOriginated,
                        *iDtAllowedWhenCsOriginated );
            
    RUBY_DEBUG1( "iVoipServiceId [%d]", iVoipServiceId ); 
    
    // Initialize HO notification tone value to 0 (off), if not found or invalid   
    RBuf val;
    val.CreateL( KSPMaxDesLength );
    err = GetPropertyValue( *entry, ESubPropertyVccHoNotificationToneMode, 
                            val ); 
    
    if ( err || !( ConvertToIntL( val ) == 0 || ConvertToIntL( val ) == 1 ) )
        {
        // setting not found, or illegal value, set to default (0=off)TBuf<1> 
        // zero = _L( "0" );
        HBufC* tmp;
        TBuf<1> zero(_L("0"));
        tmp = zero.AllocL();
        AddOrUpdatePropertyL( *entry, ESubPropertyVccHoNotificationToneMode, 
                              *tmp );
        delete tmp;
        }
    val.Close();
 
    //get propertyarray
    RPropertyArray newProperties = entry->GetAllProperties();
    TInt propCount( entry->PropertyCount() );
    RUBY_DEBUG1( "service properties count [%d]", propCount ); 
    
    //add/update propertyarray to settings
    User::LeaveIfError( iSettings->AddOrUpdatePropertiesL( iServiceId, 
                                newProperties ));

    CleanupStack::PopAndDestroy( entry );
    }
    
// ---------------------------------------------------------------------------
// Returns the name of stored settings
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CVccSPSettings::ProviderId()
    {
    if( iProviderId != NULL )
        {
        RUBY_DEBUG0( "- VCC provider id exists" );
        return *iProviderId;
        }
    else
        {
        RUBY_DEBUG0( "-no VCC provider id" );
        return KNullDesC;
        }
    }

// ---------------------------------------------------------------------------
// Sets name of settings
// ---------------------------------------------------------------------------
//
EXPORT_C void CVccSPSettings::SetProviderIdL( const TDesC& aValue )
    {
    RUBY_DEBUG_BLOCK( "CVccSPSettings::SetProviderIdL" );
    
    // Delete the setting if already allocated
    delete iProviderId;
    iProviderId = NULL;
    iProviderId = aValue.AllocL();
    }
    
// ---------------------------------------------------------------------------
// Sets name of settings
// ---------------------------------------------------------------------------
//
EXPORT_C void CVccSPSettings::SetServiceNameL( const TDesC& aValue )
    {
    RUBY_DEBUG_BLOCK( "CVccSPSettings::SetNameL" );
    delete iServiceName;
    iServiceName = NULL;
    iServiceName = aValue.AllocL();
    }
    
// ---------------------------------------------------------------------------
// Returns name of stored settings
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CVccSPSettings::ServiceName()
    {
    if( iServiceName != NULL )
        {
        RUBY_DEBUG0( "-VCC service name exists" );
        return *iServiceName;
        }
    else
        {
        RUBY_DEBUG0( "-no VCC service name" );

        return KNullDesC;
        }
    }    
// ---------------------------------------------------------------------------
// Sets VDI
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CVccSPSettings::SetVdiL( const TDesC& aValue )
    {
    RUBY_DEBUG_BLOCK( "CVccSPSettings::SetVdiL" );
    
    TInt err = TVCCMiscUtils::URIValidL( aValue );
    if ( !err )
        {
        if( iVdi )
        	{
            delete iVdi;
        	iVdi = NULL;
        	}
        iVdi = aValue.AllocL();
        }
    
    return err;
    }

// ---------------------------------------------------------------------------
// Sets VDN
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CVccSPSettings::SetVdnL( const TDesC& aValue )
    {
    RUBY_DEBUG_BLOCK( "CVccSPSettings::SetVdnL" );
    
    TBool ok = TVCCMiscUtils::VDNValidL( aValue );
    
    if ( ok )
        {
        if( iVdn )
        	{
        	delete iVdn;
        	iVdi = NULL;
        	}
        
        iVdn = aValue.AllocL();
        }
    
    return ok;
    }

// ---------------------------------------------------------------------------
// Sets Preferred Domain
// ---------------------------------------------------------------------------
//
EXPORT_C void CVccSPSettings::SetPreferredDomainL( const TDesC& aValue )
    {
    RUBY_DEBUG_BLOCK( "CVccSPSettings::SetPreferredDomainL" );
    if ( iPreferredDomain )
        {
        delete iPreferredDomain;
        iPreferredDomain = NULL;
        }
    iPreferredDomain = aValue.AllocL();
    }

// ---------------------------------------------------------------------------
// Sets Immediate Domain Transfer
// ---------------------------------------------------------------------------
//
EXPORT_C void CVccSPSettings::SetImmediateDtL( const TDesC& aValue )
    {
    RUBY_DEBUG_BLOCK( "CVccSPSettings::SetImmediateDtL" );
    if ( iImmediateDt )
        {
        delete iImmediateDt;
        iImmediateDt = NULL;
        }
    
    iImmediateDt = aValue.AllocL();
    }

// ---------------------------------------------------------------------------
// Sets Domain Transfer Cs To Ps Allowed
// ---------------------------------------------------------------------------
//
EXPORT_C void CVccSPSettings::SetDtCsToPsAllowedL( const TDesC& aValue )
    {
    RUBY_DEBUG_BLOCK( "CVccSPSettings::SetDtCsToPsAllowedL" );
    if ( iDtCsToPsAllowed )
        {
        delete iDtCsToPsAllowed;
        iDtCsToPsAllowed = NULL;
        }
    iDtCsToPsAllowed = aValue.AllocL();
    }

// ---------------------------------------------------------------------------
// Sets Domain Transfer Ps To Cs Allowed
// ---------------------------------------------------------------------------
//
EXPORT_C void CVccSPSettings::SetDtPsToCsAllowedL( const TDesC& aValue )
    {
    RUBY_DEBUG_BLOCK( "CVccSPSettings::SetDtPsToCsAllowedL" );
    if ( iDtPsToCsAllowed )
        {
        delete iDtPsToCsAllowed;
        iDtPsToCsAllowed = NULL;
        }
    iDtPsToCsAllowed = aValue.AllocL();
    }
    
// ---------------------------------------------------------------------------
// Sets Domain Transfer allowed while Held and Waiting Calls 
// ---------------------------------------------------------------------------
//
EXPORT_C void CVccSPSettings::SetDtHeldWaitingCallsAllowedL( const TDesC& aValue )
    {
    RUBY_DEBUG_BLOCK( "CVccSPSettings::SetDtHeldWaitingCallsAllowedVccL" );
    if ( iDtHeldWaitingCallsAllowed )
        {
        delete iDtHeldWaitingCallsAllowed;
        iDtHeldWaitingCallsAllowed = NULL;
        }
    iDtHeldWaitingCallsAllowed = aValue.AllocL();
    }

// ---------------------------------------------------------------------------
//  Sets handover treshold value for WLAN
// ---------------------------------------------------------------------------
//
EXPORT_C void CVccSPSettings::SetDtWlanHoTresholdL( const TDesC& aValue )
    {
    RUBY_DEBUG_BLOCK( "CWPVccItem::SetDtWlanHoTresholdL" );
    if ( iDtWlanHoTreshold )
        {
        delete iDtWlanHoTreshold;
        iDtWlanHoTreshold = NULL;
        }
    iDtWlanHoTreshold = aValue.AllocL();
    }

// ---------------------------------------------------------------------------
//  Sets handover hysteresis value for WLAN
// ---------------------------------------------------------------------------
//
EXPORT_C void CVccSPSettings::SetDtWlanHoHysteresisL( const TDesC& aValue )
    {
    RUBY_DEBUG_BLOCK( "CWPVccItem::SetDtWlanHoHysteresisL" );
    if ( iDtWlanHoHysteresis )
        {
        delete iDtWlanHoHysteresis;
        iDtWlanHoHysteresis = NULL;
        }
    iDtWlanHoHysteresis = aValue.AllocL();
    }
// ---------------------------------------------------------------------------
//  Sets handover hysteresis timer value for WLAN lower limit
// ---------------------------------------------------------------------------
//
EXPORT_C void CVccSPSettings::SetDtWlanHoHysteresisTimerLowL(
                                                           const TDesC& aValue )
    {
    RUBY_DEBUG_BLOCK( "CWPVccItem::SetDtWlanHoHysteresisTimerLowL" );
    if ( iDtWlanHoHysteresisTimerLow )
        {
        delete iDtWlanHoHysteresisTimerLow;
        iDtWlanHoHysteresisTimerLow = NULL;
        }
    iDtWlanHoHysteresisTimerLow = aValue.AllocL();
    }
// ---------------------------------------------------------------------------
//  Sets handover hysteresis timer value for WLAN higher limit
// ---------------------------------------------------------------------------
//
EXPORT_C void CVccSPSettings::SetDtWlanHoHysteresisTimerHighL( 
                                                           const TDesC& aValue )
    {
    RUBY_DEBUG_BLOCK( "CWPVccItem::SetDtWlanHoHysteresisTimerHighL" );
    if ( iDtWlanHoHysteresisTimerHigh )
        {
        delete iDtWlanHoHysteresisTimerHigh;
        iDtWlanHoHysteresisTimerHigh = NULL;
        }
    iDtWlanHoHysteresisTimerHigh = aValue.AllocL();
    }
// ---------------------------------------------------------------------------
//  Sets handover treshold value for CS
// ---------------------------------------------------------------------------
//
EXPORT_C void CVccSPSettings::SetDtCsHoTresholdL( const TDesC& aValue )
    {
    RUBY_DEBUG_BLOCK( "CWPVccItem::SetDtCsHoTresholdL" );
    if ( iDtCsHoTreshold )
        {
        delete iDtCsHoTreshold;
        iDtCsHoTreshold = NULL;
        }
    iDtCsHoTreshold = aValue.AllocL();
    }

// ---------------------------------------------------------------------------
//  Sets handover hysteresis value for CS
// ---------------------------------------------------------------------------
//
EXPORT_C void CVccSPSettings::SetDtCsHoHysteresisL( const TDesC& aValue )
    {
    RUBY_DEBUG_BLOCK( "CWPVccItem::SetDtCsHoHysteresisL" );
    if ( iDtCsHoHysteresis )
        {
        delete iDtCsHoHysteresis;
        iDtCsHoHysteresis = NULL;
        }
    iDtCsHoHysteresis = aValue.AllocL();
    }

// ---------------------------------------------------------------------------
//  Sets handover hysteresis timer value for CS lower limit
// ---------------------------------------------------------------------------
//
EXPORT_C void CVccSPSettings::SetDtCsHoHysteresisTimerLowL( const TDesC& aValue )
    {
    RUBY_DEBUG_BLOCK( "CWPVccItem::SetDtCsHoHysteresisTimerLowL" );
    if ( iDtCsHoHysteresisTimerLow )
        {
        delete iDtCsHoHysteresisTimerLow;
        iDtCsHoHysteresisTimerLow = NULL;
        }
    iDtCsHoHysteresisTimerLow = aValue.AllocL();
    }

// ---------------------------------------------------------------------------
//  Sets handover hysteresis timer value for CS higher limit
// ---------------------------------------------------------------------------
//
EXPORT_C void CVccSPSettings::SetDtCsHoHysteresisTimerHighL( 
                                                           const TDesC& aValue )
    {
    RUBY_DEBUG_BLOCK( "CWPVccItem::SetDtCsHoHysteresisTimerHighL" );
    if ( iDtCsHoHysteresisTimerHigh )
        {
        delete iDtCsHoHysteresisTimerHigh;
        iDtCsHoHysteresisTimerHigh = NULL;
        }
    iDtCsHoHysteresisTimerHigh = aValue.AllocL();
    }
        
// ---------------------------------------------------------------------------
// Converts given value to int.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CVccSPSettings::ConvertToIntL( const TDesC& aValue )
    {
    TInt intValue;
    TLex temp( aValue );
    User::LeaveIfError( temp.Val( intValue ) );
    return intValue;
    }
    
// ---------------------------------------------------------------------------
// Finds service provider entry from spsettings
// if service entry was not found, a new entry is created.
// ---------------------------------------------------------------------------
//
EXPORT_C void CVccSPSettings::FindServiceEntryL( const TDesC& aServiceName, 
        CSPEntry& aServiceEntry )
    {
    RUBY_DEBUG_BLOCK( "CVccSPSettings::FindServiceEntryL" );
    
    RUBY_DEBUG1( "-service name:[%S]",&aServiceName );
    TInt error( KErrNone );
    RIdArray serviceids;
    CleanupClosePushL( serviceids );
    iSettings->FindServiceIdsL( serviceids );
    
    TInt count = serviceids.Count();
    RUBY_DEBUG1( "-serviceids count [%d]",count ); 

    TBool foundEntry( EFalse );
        
    for ( TInt i( 0 ); i < serviceids.Count() && !foundEntry; i++ )
          {
          CSPProperty* property = CSPProperty::NewLC(); 
          error = iSettings->FindPropertyL( serviceids[i], ESubPropertyVccVDI, *property ); 
 
          if( error == KErrNone )
              {
              RUBY_DEBUG0( "-service found" );
              iSettings->FindEntryL( serviceids[i], aServiceEntry );
              foundEntry = ETrue;
              }
          CleanupStack::PopAndDestroy( property );
          }
  
    if( aServiceEntry.GetServiceId() == KVccZeroId && !foundEntry )
        {
        RUBY_DEBUG0( "-no service found"); 
        }

    CleanupStack::PopAndDestroy( &serviceids );
     }

// ---------------------------------------------------------------------------
// Sets Voip service Id 
// ---------------------------------------------------------------------------
//
EXPORT_C void CVccSPSettings::SetVoipServiceIdL( const TDesC& aValue )
    {
    RUBY_DEBUG1( "CVccSPSettings::SetVoipServiceIdL instance %x", this );
    SetVoipServiceIdL( ConvertToIntL( aValue ) );
    RUBY_DEBUG1( "SetVoipServiceId:[%d]",iVoipServiceId );
    }

// ---------------------------------------------------------------------------
// Sets Voip service Id 
// ---------------------------------------------------------------------------
//
EXPORT_C void CVccSPSettings::SetVoipServiceIdL( TInt aValue )
    {
    TBuf<KVccServiceIdLength> serviceIdString;
		serviceIdString.Num(aValue);	
    
    if ( iVoipServiceIdString )
        {
        delete iVoipServiceIdString;
        iVoipServiceIdString = NULL;
        }
        
    iVoipServiceIdString = serviceIdString.AllocL();  
    iVoipServiceId = aValue; 
    
    RUBY_DEBUG1( "SetVoipServiceId:[%d]",iVoipServiceId );
    }

// ---------------------------------------------------------------------------
//  
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CVccSPSettings::FindPropertyL( TServiceId aServiceId,
                                   TServicePropertyName aPropertyName,
                                   CSPProperty& aProperty )
  {
    RUBY_DEBUG_BLOCK( "CVccSPSettings::FindPropertyL" );
    
    return iSettings->FindPropertyL( aServiceId, aPropertyName, aProperty );
  }

// ---------------------------------------------------------------------------
// reads entry from sp settings 
// ---------------------------------------------------------------------------
//    
EXPORT_C void CVccSPSettings::ReadSettingsL( const TDesC& aServiceName )
    {
    RUBY_DEBUG_BLOCK( "ReadSettingsL" );
    SetServiceNameL( aServiceName );
    
    CSPEntry* entry = CSPEntry::NewLC();
    
    RUBY_DEBUG1( "-service name:[%S]",iServiceName );
   
    FindServiceEntryL( *iServiceName, *entry );
    iServiceId = entry->GetServiceId();
    
    RUBY_DEBUG0( "service found" ); 
    
    RBuf val;
    val.CreateL( KSPMaxDesLength );

    TInt err = GetPropertyValue( *entry, ESubPropertyVccPreferredDomain, val );
    SetPreferredDomainL( val );
    RUBY_DEBUG2( "iPreferredDomain = %S, err %d ", iPreferredDomain, err );                

    err = GetPropertyValue( *entry, ESubPropertyVccDtCstoPsAllowed, val );
    SetDtCsToPsAllowedL( val );
    RUBY_DEBUG2( "iDtCsToPsAllowed = %S, err %d ", iDtCsToPsAllowed, err );                

    err = GetPropertyValue( *entry, ESubPropertyVccDtPstoCsAllowed, val );
    SetDtPsToCsAllowedL( val );
    RUBY_DEBUG2( "iDtPsToCsAllowed = %S, err %d ", iDtPsToCsAllowed, err );                
   
    err = GetPropertyValue( *entry, ESubPropertyVccImmediateDomainTransfer, 
                       val );
    SetImmediateDtL( val );
    RUBY_DEBUG2( "iImmediateDt = %S, err %d ", iImmediateDt, err );                

    err = GetPropertyValue( *entry, ESubPropertyVccDtHeldWaitingCallsAllowed, 
                 val );
    SetDtHeldWaitingCallsAllowedL( val );
    RUBY_DEBUG2( "iDtHeldWaitingCallsAllowed = %S, err %d ", 
                iDtHeldWaitingCallsAllowed, err );                
    
    err = GetPropertyValue( *entry, ESubPropertyVccDtCSHoTriggerLevel, val );
    SetDtCsHoTresholdL( val );
    RUBY_DEBUG2( "iDtCsHoTreshold = %S, err %d ", iDtCsHoTreshold, err );                

    err = GetPropertyValue( *entry, ESubPropertyVccDtCSHoHysteresis, 
            val );
    SetDtCsHoHysteresisL( val );
    RUBY_DEBUG2( "iDtCsHoHysteresis = %S, err %d ", iDtCsHoHysteresis, err );  

    err = GetPropertyValue( *entry, ESubPropertyVccDtCSHoHysteresisTimerLow, 
            val );
    SetDtCsHoHysteresisTimerLowL( val );
    RUBY_DEBUG2( "iDtCsHoHysteresisTimerLow = %S, err %d ", 
            iDtCsHoHysteresisTimerLow, err );                

    err = GetPropertyValue( *entry, ESubPropertyVccDtCSHoHysteresisTimerHigh, 
            val );
    SetDtCsHoHysteresisTimerHighL( val );
    RUBY_DEBUG2( "iDtCsHoHysteresisTimerHigh = %S, err %d ",
                 iDtCsHoHysteresisTimerHigh, err );                

    err = GetPropertyValue( *entry, ESubPropertyVccDtWLANHoTriggerLevel,  val );
    SetDtWlanHoTresholdL( val );
    RUBY_DEBUG2( "iDtWlanHoTreshold = %S, err %d ", iDtWlanHoTreshold, err );                
    
    err = GetPropertyValue( *entry, ESubPropertyVccDtWLANHoHysteresis, val );
    SetDtWlanHoHysteresisL( val );
    RUBY_DEBUG2( "iDtWlanHoHysteresis = %S, err %d ", iDtWlanHoHysteresis, err );   

    err = GetPropertyValue( *entry, ESubPropertyVccDtWLANHoHysteresisTimerLow, 
            val );
    SetDtWlanHoHysteresisTimerLowL( val );
    RUBY_DEBUG2( "iDtWlanHoHysteresisTimerLow = %S, err %d ", 
            iDtWlanHoHysteresisTimerLow, err );   

    err = GetPropertyValue( *entry, ESubPropertyVccDtWLANHoHysteresisTimerHigh, 
            val );
    SetDtWlanHoHysteresisTimerHighL( val );
    RUBY_DEBUG2( "iDtWlanHoHysteresisTimerHigh = %S, err %d ", 
            iDtWlanHoHysteresisTimerHigh, err );   

    err = GetPropertyValue( *entry, ESubPropertyVccDtVoipServiceId, val );
    SetVoipServiceIdL( val );
    RUBY_DEBUG2( "iVoipServiceId = %d, err0 %d ", iVoipServiceId, err );  
    
    err = GetPropertyValue( *entry, ESubPropertyVccDtAllowedWhenCsOriginated, val );
    SetDtAllowedWhenCsOriginated( val );
    RUBY_DEBUG2( "iDtAllowedWhenCsOriginated = %S, err %d ", 
                    iDtAllowedWhenCsOriginated, err );  

    val.Close();
    CleanupStack::PopAndDestroy( entry );
    }

// ---------------------------------------------------------------------------
// Return preferred domain
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CVccSPSettings::PreferredDomainL()
    {
    RUBY_DEBUG_BLOCK( "CVccSPSettings::PreferredDomainL" );
    return ConvertToIntL( *iPreferredDomain );
    }

// ---------------------------------------------------------------------------
// Return Immediate Dt
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CVccSPSettings::ImmediateDtL()
    {
    RUBY_DEBUG_BLOCK( "CVccSPSettings::ImmediateDtL" );
    return ConvertToIntL( *iImmediateDt );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CVccSPSettings::DtCsToPsAllowedL()
    {
    RUBY_DEBUG_BLOCK( "CVccSPSettings::DtCsToPsAllowedL" );
    return ConvertToIntL( *iDtCsToPsAllowed );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CVccSPSettings::DtPsToCsAllowedL()
    {
    RUBY_DEBUG_BLOCK( "CVccSPSettings::DtPsToCsAllowedL" );
    return ConvertToIntL( *iDtPsToCsAllowed );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CVccSPSettings::DtHeldWaitingCallsAllowedL()
    {
    RUBY_DEBUG_BLOCK( "CVccSPSettings::DtHeldWaitingCallsAllowedL" );
    return ConvertToIntL( *iDtHeldWaitingCallsAllowed );
    }
   
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt CVccSPSettings::CsHoTresholdL()
    {
    return ConvertToIntL( *iDtCsHoTreshold );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt CVccSPSettings::CsHoHysteresisL()
    {
    RUBY_DEBUG_BLOCK( "CVccSPSettings::CsHoHysteresisL" );
    return ConvertToIntL( *iDtCsHoHysteresis );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt CVccSPSettings::CsHoHysteresisTimerLowL()
    {
    RUBY_DEBUG_BLOCK( "CVccSPSettings::CsHoHysteresisTimerLowL" );
    return ConvertToIntL( *iDtCsHoHysteresisTimerLow );
    }
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt CVccSPSettings::CsHoHysteresisTimerHighL()
    {
    RUBY_DEBUG_BLOCK( "CVccSPSettings::CsHoHysteresisTimerHighL" );
    return ConvertToIntL( *iDtCsHoHysteresisTimerHigh );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt CVccSPSettings::PsHoTresholdL()
    {
    RUBY_DEBUG_BLOCK( "CVccSPSettings::PsHoTresholdL" );
    return ConvertToIntL( *iDtWlanHoTreshold );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt CVccSPSettings::PsHoHysteresisL()
    {
    RUBY_DEBUG_BLOCK( "CVccSPSettings::PsHoHysteresisL" );
    return ConvertToIntL( *iDtWlanHoHysteresis );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt CVccSPSettings::PsHoHysteresisTimerLowL()
    {
    RUBY_DEBUG_BLOCK( "CVccSPSettings::PsHoHysteresisTimerLowL" );
    return ConvertToIntL( *iDtWlanHoHysteresisTimerLow );
    }
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt CVccSPSettings::PsHoHysteresisTimerHighL()
    {
    return ConvertToIntL( *iDtWlanHoHysteresisTimerHigh );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt CVccSPSettings::VoipServiceId()
    {
    return iVoipServiceId;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C TInt CVccSPSettings::DtAllowedWhenCsOriginatedL()
    {
    return ConvertToIntL( *iDtAllowedWhenCsOriginated );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C void CVccSPSettings::SetDtAllowedWhenCsOriginated( const TDesC& aValue )
    {
    RUBY_DEBUG_BLOCK( "CVccSPSettings::SetDtAllowedWhenCsOriginated" );
    if ( iDtAllowedWhenCsOriginated )
        {
        delete iDtAllowedWhenCsOriginated;
        iDtAllowedWhenCsOriginated = NULL;
        }
    TRAP_IGNORE( iDtAllowedWhenCsOriginated = aValue.AllocL() );
    }
// ---------------------------------------------------------------------------
// Update old property or add new property to the service entry 
// ---------------------------------------------------------------------------
//
void CVccSPSettings::AddOrUpdatePropertyL( CSPEntry& aServiceEntry, 
    const TServicePropertyName aName, TInt aValue )
    {
   
    TInt err = aServiceEntry.UpdateProperty( aName, aValue ); 
    
    //add new to entry
    if( err == KErrNotFound )
        {
        CSPProperty* property = CSPProperty::NewLC(); 
        property->SetName( aName );
        property->SetValue( aValue );
        // New property, don't need to check return value
        // coverity[check_return] coverity[unchecked_value]
        aServiceEntry.AddPropertyL( *property );
        CleanupStack::PopAndDestroy( property );
        }    
    }

// ---------------------------------------------------------------------------
// Update old property or add new property to the service entry 
// ---------------------------------------------------------------------------
//
void CVccSPSettings::AddOrUpdatePropertyL( CSPEntry& aServiceEntry, 
    const TServicePropertyName aName, const TDesC& aValue )
    {
    TInt err = aServiceEntry.UpdateProperty( aName, aValue ); 
    
    //add new to entry
    if( err == KErrNotFound )
        {
        CSPProperty* property = CSPProperty::NewLC(); 
        property->SetName( aName );
        property->SetValue( aValue );
        // New property, don't need to check return value
        // coverity[check_return] coverity[unchecked_value]
        aServiceEntry.AddPropertyL( *property );
        CleanupStack::PopAndDestroy( property );
        }    
    }
    
// ---------------------------------------------------------------------------
// Try to get the value of property as integer
// ---------------------------------------------------------------------------
//    
TInt CVccSPSettings::GetPropertyValue( CSPEntry& aServiceEntry,
    const TServicePropertyName& aPropertyName,
    TInt& aValue )
    {
    RUBY_DEBUG_BLOCK( "GetPropertyValue" );
  
    const CSPProperty* property = NULL;
    TInt err = aServiceEntry.GetProperty( property, aPropertyName );
    if( property != NULL )                     
        err = property->GetValue( aValue ) ;
    
    return err;
     }

// ---------------------------------------------------------------------------
// Try to get the value of property as integer
// ---------------------------------------------------------------------------
//    
TInt CVccSPSettings::GetPropertyValue( CSPEntry& aServiceEntry,
    const TServicePropertyName& aPropertyName,
    TDes& aValue )
    {
    RUBY_DEBUG_BLOCK( "GetPropertyValue" );
  
    const CSPProperty* property = NULL;
    TInt err = aServiceEntry.GetProperty( property, aPropertyName );
    
    if( property != NULL )
        {
        err = property->GetValue( aValue ) ;
        }
    else
        {
        TBuf<1> zero(_L("0"));
        aValue.Copy( zero );
        }
    
    return err;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
void CVccSPSettings::ModifyVoipProfileL()
    {
    //Change callprovider plugin uid in VoIP setting
    CSPEntry* voipEntry = CSPEntry::NewLC();
    TRAPD( e, iSettings->FindEntryL( iVoipServiceId, *voipEntry ) );
  
    if (e == KErrNone)
        {
        RUBY_DEBUG0( "voip service found" ); 
             
        const CSPProperty* property = NULL;
        TInt err = voipEntry->GetProperty( property, 
                                           EPropertyServiceAttributeMask );
      
        if( property != NULL )
            {
            TInt mask;
            err = property->GetValue( mask ) ;
            RUBY_DEBUG1( "voip service mask %d", mask ); 
                 
            mask = mask | EBootstrapCallProvider;
            RUBY_DEBUG1( "voip service mask after mod %d", mask ); 
            voipEntry->UpdateProperty(EPropertyServiceAttributeMask, mask );
            voipEntry->UpdateProperty(EPropertyCallProviderPluginId, KVccCallProviderPlugId );
          
            // Update entry
            iSettings->UpdateEntryL(*voipEntry);
              
            RUBY_DEBUG0( "voip service updated" ); 
            }
        }
    CleanupStack::PopAndDestroy( voipEntry );
    }
