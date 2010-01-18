/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Handles Vcc settings in provisioning.
*
*/



#include <e32base.h>
#include <commdb.h>
#include <CWPAdapter.h>
#include <CWPCharacteristic.h>
#include <CWPParameter.h>
#include <utf.h>
#include <wpvccadapterrsc.rsg>
#include <crcseprofileentry.h>
#include <crcseprofileregistry.h>
#include <WPAdapterUtil.h>

#include "wpvccadapter.h"
#include "rubydebug.h"
#include "vccspsettings.h"
#include "wpvccadapteruids.h"
#include <cch.h>
#include "vccsettingsreader.h"
#include <sipmanagedprofileregistry.h>
#include <sipprofile.h>
#include <spsettings.h>
#include <spproperty.h>
#include <cmmanagerext.h>
#include <cmdestinationext.h>

/** Name of *this adapter */
_LIT( KVccResourceFileNameWithoutSuffix,  "wpvccadapterrsc" );

/** Provisioning file format specific settings */
// OMA Experimental values, registered by OMNA only for advisory purposes
// range 0x4000 to 0x1FFFFF
_LIT( KVccAppID,        "w9054" );        
_LIT8( KVoIPAppID8,     "w9013" );  	  // OMA CP registration document for VoIP
_LIT( KVdi,             "VDI" );          // 
_LIT( KVdn,             "VDN" );          // 
_LIT( KPreferredDomain, "PREF-DOMAIN" );  // 
_LIT( KImmediateDT,     "IMMEDIATE-DOMAIN-TRANSFER" );   // 
_LIT( KDtCsToPsAllowed, "DT-CS-TO-PS-ALLOWED" );      // 
_LIT( KDtPsToCsAllowed, "DT-PS-TO-CS-ALLOWED" );     // 
_LIT( KDtHeldwaitingCallsAllowed, "DT-HELD-WAITING-CALLS-ALLOWED" );     // 

_LIT( KWlanHOTreshold, "WLAN-HO-THRESHOLD-VALUE" );
_LIT( KWlanHOHysteresis, "WLAN-HO-HYSTERESIS-VALUE" );
_LIT( KWlanHysteresisTimerLow, "WLAN-HYSTERESIS-TIMER-LOW" );
_LIT( KWlanHysteresisTimerHigh, "WLAN-HYSTERESIS-TIMER-HIGH" );

_LIT( KCSHOTreshold, "CS-HO-THRESHOLD-VALUE" );
_LIT( KCSHOHysteresis, "CS-HO-HYSTERESIS-VALUE" );
_LIT( KCSHysteresisTimerLow, "CS-HYSTERESIS-TIMER-LOW" );
_LIT( KCSHysteresisTimerHigh, "CS-HYSTERESIS-TIMER-HIGH" );
_LIT( KDtAllowedWhenCsOriginated, "DT-ALLOWED-WHEN-CS-ORIGINATED" );

_LIT16( KDefaultName, "VCC application settings" );


// -----------------------------------------------------------------------------
// ResetAndDestroyEntries
// -----------------------------------------------------------------------------
//
void ResetAndDestroyProfileEntries( TAny* anArray )
    {

    RPointerArray<CRCSEProfileEntry>* array =
        reinterpret_cast<RPointerArray<CRCSEProfileEntry>*>( anArray );

    if (array)
        {
        array->ResetAndDestroy();
        array->Close();
        }
    }

// ======== MEMBER FUNCTIONS ========
// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CWPVccAdapter::CWPVccAdapter() : CWPAdapter()
    {
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CWPVccAdapter::ConstructL()
    {
    RUBY_DEBUG_BLOCKL( "CWPVccAdapter::ConstructL" );
    
    TFileName fileName;
    
    // ReadHBufCL needs the path where the resource file is.
    // Actually, the WPAdapterUtil only uses the drive part of the string
    // (which is normally "z:")
    // The resource file name must not contain the suffix since it is
    // added.
    
    Dll::FileName( fileName );
    
    iTitle = WPAdapterUtil::ReadHBufCL(
        fileName, KVccResourceFileNameWithoutSuffix, R_QTN_SM_HEAD_VCC );
     }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CWPVccAdapter* CWPVccAdapter::NewL()
    {
    RUBY_DEBUG_BLOCKL( "CWPVccAdapter::NewL" );
    CWPVccAdapter* self = new( ELeave ) CWPVccAdapter;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CWPVccAdapter::~CWPVccAdapter()
    {
    RUBY_DEBUG0( "CWPVccAdapter::~CWPVccAdapter() - ENTER" );
   
    iToAppReferences.Close();
    delete iTitle;
    delete iAppID;
    delete iVccItem;
    delete iNewVccItem;
 
    RUBY_DEBUG0( "CWPVccAdapter::~CWPVccAdapter() - EXIT" );
    }

// ---------------------------------------------------------------------------
// Not currently supported.
// ---------------------------------------------------------------------------
//
TInt CWPVccAdapter::ContextExtension( MWPContextExtension*& /*aExtension*/ )
    {
    RUBY_DEBUG_BLOCK( "CWPVccAdapter::ContextExtension" );
    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// Not currently supported.
// ---------------------------------------------------------------------------
//
const TDesC8& CWPVccAdapter::SaveDataL( TInt /*aIndex*/ ) const
    {
    RUBY_DEBUG_BLOCK( "CWPVccAdapter::SaveDataL" );
    User::Leave( KErrNotSupported );
    // Nothing returned cause we leave.
    return KNullDesC8;
    }

// ---------------------------------------------------------------------------
// Not currently supported.
// ---------------------------------------------------------------------------
//
void CWPVccAdapter::DeleteL( const TDesC8& /*aSaveData*/ )
    {
    RUBY_DEBUG_BLOCK( "CWPVccAdapter::DeleteL" );
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// Returns UID of the adapter (*this).
// ---------------------------------------------------------------------------
//
TUint32 CWPVccAdapter::Uid() const
    {
    RUBY_DEBUG_BLOCK( "CWPVccAdapter::Uid" );
    return KProvisioningVccAdapterImplementationUid; //iDtor_ID_Key.iUid;
    }

// ---------------------------------------------------------------------------
// Used to query for the detail information about the Vcc profile.
// Not currently supported.
// ---------------------------------------------------------------------------
//
TInt CWPVccAdapter::DetailsL( TInt, MWPPairVisitor& )
    {
    RUBY_DEBUG_BLOCK( "CWPVccAdapter::DetailsL" );
    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// Returns the amount of Vcc data items (always 1 if data got, otherwise 0).
// ---------------------------------------------------------------------------
//
TInt CWPVccAdapter::ItemCount() const
    {
    RUBY_DEBUG_BLOCK( "CWPVccAdapter::ItemCount" );
    return iVccItem ? 1 : 0;
    }

// ---------------------------------------------------------------------------
// Returns summary title of the Vcc profile received in configuration
// message.
// ---------------------------------------------------------------------------
//
const TDesC16& CWPVccAdapter::SummaryTitle( TInt /*aIndex*/ ) const
    {
    RUBY_DEBUG_BLOCK( "CWPVccAdapter::SummaryTitle" );
    return *iTitle;
    }

// ---------------------------------------------------------------------------
// Returns summary text of the Vcc profile received in configuration
// message. Returns NULL descriptor if no settings received.
// ---------------------------------------------------------------------------
//
const TDesC16& CWPVccAdapter::SummaryText( TInt /*aIndex*/ ) const
    {
    RUBY_DEBUG_BLOCK( "CWPVccAdapter::SummaryText" );

	if ( iVccItem )
	    {
	    RUBY_DEBUG0( "-vccitem Name" );
	    return iVccItem->ServiceName();
	    }
    else
        {
  
        RUBY_DEBUG0( " - default text NO ITEM!" );
        return KDefaultName;
        }
    }

// ---------------------------------------------------------------------------
// Saves Vcc item (whole profile). If no settings are received, leaves with
// errorcode KErrGeneral.
// ---------------------------------------------------------------------------
//
void CWPVccAdapter::SaveL( TInt /*aIndex*/ )
    {
    RUBY_DEBUG_BLOCK( "CWPVccAdapter::SaveL" );
    
    TRAP_IGNORE( RemoveVCCSettingsL() );
    
	// Lets do the saving when framework calls savingFinalisedL method
    }

// ---------------------------------------------------------------------------
// Tells if current profile can be set as a default profile. Since Vcc
// currently supports only single profile, return value is always false.
// ---------------------------------------------------------------------------
//
TBool CWPVccAdapter::CanSetAsDefault( TInt /*aIndex*/ ) const
    {
    RUBY_DEBUG_BLOCK( "CWPVccAdapter::CanSetAsDefault" );
    return EFalse;
    }

// ---------------------------------------------------------------------------
// Sets the Vcc item on place pointed by index given in parameter as default.
// Since Vcc currently supports only single profile and CanSetAsDefault method
// always returns false, this method should never be called. It is unsupported
// and does nothing. Derivation of this method is done only to fulfill the
// demands of s60 Coding conventions, Symbian OS, C++ and other entities
// setting boundaries for specific software as *this.
// ---------------------------------------------------------------------------
//
void CWPVccAdapter::SetAsDefaultL( TInt /*aIndex*/ )
    {
    RUBY_DEBUG_BLOCK( "CWPVccAdapter::SetAsDefaultL" );
    // This shouldn't be called because CanSetAsDefault
    // always returns EFalse
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// This method is called by the provisioning framework to "visit" an adapter
// plugin (*this). Plugin then checks if the visiting aCharacteristic are
// of the particular type that this plugin wants to handle, and if so, it
// invites the visitor deeper in to more intimate series of visits through
// overloaded VisitL method with CWPParameter& visitor aboard.
// Generally this plugin is interested in two type of visitors, one called
// KWPApplication (carrying application level data), and other called
// KWPNapDef (carrying data needed to establish new IAPs).
// ---------------------------------------------------------------------------
//
void CWPVccAdapter::VisitL( CWPCharacteristic& aCharacteristic )
    {
    RUBY_DEBUG_BLOCK( "CWPVccAdapter::VisitL char" );
    
    switch ( aCharacteristic.Type() )
        {
        case KWPApplication:
            {
            RUBY_DEBUG0( " - KWPApplication create new item" );
            iNewVccItem = CVccSPSettings::NewL();
        
            iCurrentCharacteristicType = aCharacteristic.Type() ;
            aCharacteristic.AcceptL( *this );
        
            if( iAppID && *iAppID == KVccAppID )
                {
                RUBY_DEBUG0( "- VCC AppId used" );
                delete iVccItem;
                iVccItem = iNewVccItem;
                }
            else
                {
                delete iNewVccItem;
                }
            iNewVccItem = NULL;   
            }break;
        default:
            {
            }break;
        }

    }

// ---------------------------------------------------------------------------
// This "visit" method should be called by all params in CWPCharacteristic&
// visitors invited welcome in VisitL( CWPCharacteristic& ) overload. The
// information in visiting params are then stored to VccItem datastructure for
// future use.
// ---------------------------------------------------------------------------
//
void CWPVccAdapter::VisitL( CWPParameter& aParameter )
    {
    RUBY_DEBUG0( "VisitL param" );
    
    switch( aParameter.ID() )
        {
        case EWPParameterAppID:
            {
	        RUBY_DEBUG0( "- got EWPParameterAppID");
            delete iAppID; 
	        iAppID = NULL;
	        iAppID = aParameter.Value().AllocL();
            
            
            }break;
        case EWPNamedParameter:
            {
            RUBY_DEBUG0( "- got EWPNamedParameter populate VCC item" );
            
            if( !iNewVccItem )
                {
                RUBY_DEBUG0( "- NO VCC item " );
                return;
                }
            
            TPtrC value( aParameter.Value() );

            if ( aParameter.Name().Compare( KVdi )  == KErrNone )
                {
                RUBY_DEBUG1( "- got VDI:%S" , 
                             &value );
                iNewVccItem->SetVdiL( value );
                }
            else if ( aParameter.Name().Compare( KVdn )  == KErrNone  )
                {
                RUBY_DEBUG1( "- got VDN:%S" ,
                             &value );
#if defined( _DEBUG )                
                TInt val = aParameter.Name().Compare( KVdn );
                TBool valb (!val);
#endif                
                RUBY_DEBUG1( "- got val:%d", val );
                RUBY_DEBUG1( "- got valb:%d", valb );
                
                iNewVccItem->SetVdnL( value );
                }
            else if ( aParameter.Name().Compare( KPreferredDomain ) == KErrNone )
                {
                RUBY_DEBUG1( "- got Preferred Domain:%S" ,
                             &value );
                iNewVccItem->SetPreferredDomainL( value );
                }
            else if ( aParameter.Name().Compare( KImmediateDT )  == KErrNone  )
                {
                RUBY_DEBUG1( "- got Immediate DT:%S" , 
                            &value );
                iNewVccItem->SetImmediateDtL( value );
                }
            else if ( aParameter.Name().Compare( KDtCsToPsAllowed ) == KErrNone )
                {
                RUBY_DEBUG1( "- got DT from CS to PS allowed:%S" ,
                             &value );
                      
                iNewVccItem->SetDtCsToPsAllowedL( value );
                }
            else if ( aParameter.Name().Compare( KDtPsToCsAllowed ) == KErrNone )
                {
                RUBY_DEBUG1( "- got DT from CS to PS allowed:%S" ,
                             &value );
                      
                iNewVccItem->SetDtPsToCsAllowedL( value );
                }
            else if ( aParameter.Name().Compare( 
                            KDtHeldwaitingCallsAllowed )  == KErrNone )
                {
                RUBY_DEBUG1( "- got DT from CS to PS allowed:%S" ,
                             &value );
                      
                iNewVccItem->SetDtHeldWaitingCallsAllowedL( value );
                }
            else if ( aParameter.Name().Compare( 
                            KWlanHOTreshold )  == KErrNone )
                {
                RUBY_DEBUG1( "- got DT from WLAN HO Treshold value:%S" ,
                             &value );
                     
                iNewVccItem->SetDtWlanHoTresholdL( value );
                }
            else if ( aParameter.Name().Compare( 
                            KWlanHOHysteresis )  == KErrNone )
                {
                RUBY_DEBUG1( "- got DT from WLAN HO hysteresis value:%S" ,
                             &value );
                      
                iNewVccItem->SetDtWlanHoHysteresisL( value );
                }
            else if ( aParameter.Name().Compare( 
                            KWlanHysteresisTimerLow )  == KErrNone )
                {
                RUBY_DEBUG1( "- got DT from WLAN Hysteresis timer low:%S" ,
                             &value );
                      
                iNewVccItem->SetDtWlanHoHysteresisTimerLowL( value );
                }
            else if ( aParameter.Name().Compare( 
                            KWlanHysteresisTimerHigh )  == KErrNone )
                {
                RUBY_DEBUG1( "- got DT from WLAN Hysteresis timer high:%S" ,
                             &value );
                      
                iNewVccItem->SetDtWlanHoHysteresisTimerHighL( value );
                }

            else if ( aParameter.Name().Compare( 
                            KCSHOTreshold )  == KErrNone )
                {
                RUBY_DEBUG1( "- got DT from CS HO Treshold value:%S" ,
                             &value );
                     
                iNewVccItem->SetDtCsHoTresholdL( value );
                }
            else if ( aParameter.Name().Compare( 
                            KCSHOHysteresis )  == KErrNone )
                {
                RUBY_DEBUG1( "- got DT from CS HO hysteresis value:%S" ,
                             &value );
                      
                iNewVccItem->SetDtCsHoHysteresisL( value );
                }
            else if ( aParameter.Name().Compare( 
                            KCSHysteresisTimerLow )  == KErrNone )
                {
                RUBY_DEBUG1( "- got DT from CS Hysteresis timer low:%S" ,
                             &value );
                      
                iNewVccItem->SetDtCsHoHysteresisTimerLowL( value );
                }
            else if ( aParameter.Name().Compare( 
                            KCSHysteresisTimerHigh )  == KErrNone )
                {
                RUBY_DEBUG1( "- got DT from CS Hysteresis timer high:%S" ,
                             &value );
                      
                iNewVccItem->SetDtCsHoHysteresisTimerHighL( value );
                }
            else if( aParameter.Name().Compare( 
                        KDtAllowedWhenCsOriginated )  == KErrNone )
                {
                RUBY_DEBUG1( "- got DT allowed when CS originated:%S" ,
                                             &value );
                iNewVccItem->SetDtAllowedWhenCsOriginated( value );
                }
            else
                {
                RUBY_DEBUG0( "- no such VCC param" );    
                }    
            break;
            }
        case EWPParameterProviderID: // APPLICATION/PROVIDER-ID
            {
            if( iNewVccItem != NULL )
                {
                RUBY_DEBUG0( "- got EWPParameterProviderID" );
                
                iNewVccItem->SetProviderIdL( aParameter.Value() );
                
                }
                
            } break;

        case EWPParameterName: // APPLICATION/NAME
            {
            if( iNewVccItem != NULL )
                {
                RUBY_DEBUG0( "- got EWPParameterName" );
                iNewVccItem->SetServiceNameL( aParameter.Value() );

                }

            } break;
            
        case EWPParameterToAppRef:
            {
            RUBY_DEBUG1("-got EWPParameterToAppRef: %S", &aParameter.Value());
            iToAppReferences.Append(aParameter.Value());
            break;
            }
      default:
            {
            RUBY_DEBUG0( "- got unknown" );
        
            } break;
        }
    }

// ---------------------------------------------------------------------------
// This method is called when the provisioning framework finds a link to
// special characteristic belonging to characteristics accepted by *this
// plugin. The linked characteristics are stored to VccItem data structure
// for future use in a need-to-store basis.
// ---------------------------------------------------------------------------
//
void CWPVccAdapter::VisitLinkL( CWPCharacteristic& /*aLink*/ )
    {
    RUBY_DEBUG_BLOCK( "CWPVccAdapter::VisitLinkL" );
    User::Leave( KErrNotSupported );
    }


// -----------------------------------------------------------------------------
// CWPVccAdapter::::SettingsSavedL
//
// -----------------------------------------------------------------------------
void CWPVccAdapter::SettingsSavedL(const TDesC8& aAppIdOfSavingItem,
                                                const TDesC8& aAppRef, 
                                                const TDesC8& aStorageIdValue)
    {
    RUBY_DEBUG_BLOCK( "CWPVccAdapter::SettingsSavedL" );
   
#ifdef _DEBUG
    HBufC* tmp = HBufC::NewLC( aAppIdOfSavingItem.Length() );
    tmp->Des().Copy( aAppIdOfSavingItem );
    RUBY_DEBUG1( "aAppIdOfSavingItem = %S", &(*tmp) );
    CleanupStack::PopAndDestroy( tmp );
    
    tmp = HBufC::NewLC( aAppRef.Length() );
    tmp->Des().Copy( aAppRef );
    RUBY_DEBUG1( "aAppRef = %S", &(*tmp) );
    CleanupStack::PopAndDestroy( tmp );
    
    tmp = HBufC::NewLC( aStorageIdValue.Length() );
    tmp->Des().Copy( aStorageIdValue );
    RUBY_DEBUG1( "aStorageIdValue = %S", &(*tmp) );
    CleanupStack::PopAndDestroy( tmp );
#endif
        
     // Check which application reference is being offered
     for(TInt j=0; j<iToAppReferences.Count(); j++)
            {
            const TDesC& toAppRef = iToAppReferences[j];
            HBufC* appRef16 = HBufC::NewLC(aAppRef.Length());
            appRef16->Des().Copy(aAppRef);
            
            RUBY_DEBUG1( "Checking toAppRef: %S", &toAppRef );
    
            
            if( toAppRef.Compare(*appRef16) == 0 )
                {
    			RUBY_DEBUG0( "APPREF matched with TO-APPREF" );
                
                if( (aAppIdOfSavingItem.Compare( KVoIPAppID8 ) == 0) && iVccItem != NULL )
                	{
            		TLex8 temp( aStorageIdValue );
            		User::LeaveIfError( temp.Val( iVoipProfileId ) );
                	}
                }
            else
            	{
            	RUBY_DEBUG0( "APPREF did not match with TO-APPREF" );
                
            	}
                
            CleanupStack::PopAndDestroy(appRef16);
            }
                
    }
    
// ---------------------------------------------------------------------------
// CWPVoIPAdapter::SavingFinalizedL()
//
// ---------------------------------------------------------------------------
//
void CWPVccAdapter::SavingFinalizedL()
    {
    RUBY_DEBUG0( "CWPVccAdapter::SavingFinalizedL IN" );
    if ( iVccItem )
        {
        CRCSEProfileRegistry* cRCSEProfileRegistry = CRCSEProfileRegistry::NewLC(); // CS:1
        CRCSEProfileEntry* foundEntry = CRCSEProfileEntry::NewLC(); // CS:2
        cRCSEProfileRegistry->FindL( iVoipProfileId, *foundEntry );
        RUBY_DEBUG1( "SavingFinalizedL::ServiceId=%d", foundEntry->iServiceProviderId );
                                
        iVccItem->SetVoipServiceIdL( foundEntry->iServiceProviderId );
        iVccItem->StoreL();

        CleanupStack::PopAndDestroy( foundEntry ); // CS:1
        CleanupStack::PopAndDestroy( cRCSEProfileRegistry ); // CS:0
        }
    
    //Remove SNAP
    TRAP_IGNORE(RemoveNetworkDestinationL() );
    
    RUBY_DEBUG0( "CWPVccAdapter::SavingFinalizedL OUT" );
    }
    
// ---------------------------------------------------------------------------
// CWPVccAdapter::RemoveVCCSettingsL()
//
// ---------------------------------------------------------------------------
//
void CWPVccAdapter::RemoveVCCSettingsL()
    {
    RUBY_DEBUG_BLOCK( "CWPVccAdapter::RemoveVCCSettingsL" );
    // Get the Voip service id. Get it always to if the SP
    // settings are changed, we get updated values.
    TInt VoIPServiceId(KErrNotFound);
    TRAPD(error, VoIPServiceId = VccSettingsReader::VoIPServiceIdL() );
    RUBY_DEBUG1( " -VoIP service id: %d", VoIPServiceId );  
    
    if ( error != KErrNone )
           {
           RUBY_DEBUG0( " -VoIP service not found - LEAVE" );
           User::Leave( KErrNotFound );
           }
     
    //Get current VCC enabled VoIP service name before saving new one
    //it will be used to delete the network destination
    
    CSPSettings* settings = CSPSettings::NewLC();
    CSPProperty* property = CSPProperty::NewLC();
    
    //Get VoIP service name store it
    error = settings->FindPropertyL( VoIPServiceId, EServiceName, *property );
      
    error = property->GetValue( iOldVoipServiceName );
    User::LeaveIfError( error );
    
     CleanupStack::PopAndDestroy( property );
     CleanupStack::PopAndDestroy( settings );
    
   
     CCchService* cchService;
      // Get the service 
     CCch* cch = CCch::NewL();
    //ownership not transfered so no need to delete
     cchService = cch->GetService( VoIPServiceId );
                
     if( cchService )
         {
         RUBY_DEBUG0( " -Disable service" );
         (void) cchService->Disable( ECCHVoIPSub );
         }
     delete cch;
     
     
     // Get VoIP profile ID from VCC settings (use Service ID)
     // Convert Service Id to VoipProfileID
     CRCSEProfileRegistry* cRCSEProfileRegistry = CRCSEProfileRegistry::NewLC();
     RPointerArray<CRCSEProfileEntry> foundEntries;
     CleanupStack::PushL( TCleanupItem( ResetAndDestroyProfileEntries, &foundEntries ) );

     cRCSEProfileRegistry->FindByServiceIdL( VoIPServiceId, foundEntries );
     
     const TInt count( foundEntries.Count() );
     TSettingIds value;
 
     CSIPManagedProfileRegistry* SIPRegistry;
     SIPRegistry = CSIPManagedProfileRegistry::NewLC(*this);

     TInt err(0);
       
     //remove SIP settings
     if ( count > 0 )
        {
        // Check the existence of linked SIP profiles
           for ( TInt i( count-1 ); 0 <= i ; i-- )
                {
                 value = foundEntries[0]->iIds[i];
             
                 if ( CRCSEProfileEntry::EProtocolSIP == value.iProfileType )
                     {
                     CSIPProfile* profile = NULL;
                     TRAP( err, profile = SIPRegistry->ProfileL( value.iProfileId ) );
                     if ( KErrNotFound == err )
                         {
                         delete profile;
                         profile = NULL;
                         RUBY_DEBUG0 ("SIP profile not found");
                         }
                     else
                         {
                         SIPRegistry->DestroyL(*profile);
                         RUBY_DEBUG0 ("Sip profile destroyed");
                         delete profile;
                         profile = NULL;
                         }
                  }
               }
      
         TInt voipId = foundEntries[0]->iId;
       
         // Remove VoIP settings
        cRCSEProfileRegistry->DeleteL(voipId);
        }
    CleanupStack::PopAndDestroy( SIPRegistry ); 
    CleanupStack::PopAndDestroy(); // foundEntries
    CleanupStack::PopAndDestroy( cRCSEProfileRegistry );
    }
// ---------------------------------------------------------------------------
// From MSIPProfileRegistryObserver
// CWPVccAdapter::ProfileRegistryEventOccurred
// SIP profile event
// ---------------------------------------------------------------------------
//
void CWPVccAdapter::ProfileRegistryEventOccurred( 
    TUint32 /*aSIPProfileId*/,
    TEvent /*aEvent*/ )
    {
    RUBY_DEBUG0( "CWPVccAdapter::ProfileRegistryEventOccurred" );
    // Have to be implemented because MSIPProfileRegistryObserver
    // is passed to CSIPManagedProfileRegistry.
    }

// ---------------------------------------------------------------------------
// From MSIPProfileRegistryObserver
// CWPVccAdapter::ProfileRegistryErrorOccured
// An asynchronous error has occurred related to SIP profile
// ---------------------------------------------------------------------------
//
void CWPVccAdapter::ProfileRegistryErrorOccurred( 
    TUint32  /*aProfileId*/,
    TInt /*aError*/ )
    {
    RUBY_DEBUG0( "CRCSEProfileRegistry::ProfileRegistryErrorOccurred" );
    // Have to be implemented because MSIPProfileRegistryObserver
    // is passed to CSIPManagedProfileRegistry.
    }

// ---------------------------------------------------------------------------
// CWPVccAdapter::RemoveNetworkDestinationL()
//
// ---------------------------------------------------------------------------
//

void CWPVccAdapter::RemoveNetworkDestinationL()
    {
    RUBY_DEBUG_BLOCK( "CWPVccAdapter::RemoveNetworkDestination" );

    //remove destination 
    RCmManagerExt cmManager;
    cmManager.OpenL();
    CleanupClosePushL(cmManager);
    
    RArray<TUint32> destinations;
    CleanupClosePushL(destinations);
    cmManager.AllDestinationsL(destinations);
   
    RCmDestinationExt destination;
    HBufC *destinationName;
    
    for( TInt i = 0; i < destinations.Count(); i++)
          {
           destination = cmManager.DestinationL(destinations[i]);
           CleanupClosePushL(destination);
           destinationName = destination.NameLC();
                          
           if (destinationName->Compare( iOldVoipServiceName ) == 0 )
               {
               CleanupStack::PopAndDestroy( destinationName ); // Ownership is transferred to item    
               destinationName = NULL;
               CleanupStack::Pop(); // destination
               destination.DeleteLD();
               RUBY_DEBUG0( "deleted network destination");
               }
           else
               {
               RUBY_DEBUG0("destination NOT found");
               CleanupStack::PopAndDestroy( destinationName ); // Ownership is transferred to item    
               destinationName = NULL;
               CleanupStack::PopAndDestroy(); // destination
               }
            
           }
        CleanupStack::PopAndDestroy(); // destinations
        CleanupStack::PopAndDestroy(); //cmManager  
    }
