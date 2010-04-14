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
* Description:  
*
*/


#include <spproperty.h>
#include <spsettings.h>
#include <cmmanagerext.h>
#include <XdmSettingsApi.h>
#include <pressettingsapi.h>
#include <cmpluginwlandef.h>
#include <cmdestinationext.h>
#include <XdmSettingsCollection.h>

#include "cscsvcpluginlogger.h"
#include "cscsvcpluginhandler.h"
#include "cscsvcpluginsiphandler.h"
#include "cscsvcpluginrcsehandler.h"
#include "cscsvcplugincenrephandler.h"

// At-character in SIP profile public username.
_LIT8( KSvcPluginAtCharacter, "@" );

// Maximum lengths of the setting values.
const TInt KSvcPluginSettingLength = 256;


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCSvcPluginHandler::CCSCSvcPluginHandler()
    {    
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CCSCSvcPluginHandler::ConstructL()
    {
    CSCSVCPLUGINDEBUG("CCSCSvcPluginHandler::ConstructL - begin");
        
    iRcseHandler = CCSCSvcPluginRcseHandler::NewL();
    iSipHandler = CCSCSvcPluginSipHandler::NewL();
    iCenRepHandler = CCSCSvcPluginCenrepHandler::NewL();
    
    // Create descriptor array for domain names.
    iDomainNames = new ( ELeave ) CDesC8ArrayFlat( 1 );
    
    CSCSVCPLUGINDEBUG("CCSCSvcPluginHandler::ConstructL - end");
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCSvcPluginHandler* CCSCSvcPluginHandler::NewL()
    {
    CCSCSvcPluginHandler* self = new (ELeave) CCSCSvcPluginHandler;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCSvcPluginHandler::~CCSCSvcPluginHandler()
    {
    CSCSVCPLUGINDEBUG("CCSCSvcPluginHandler::~CCSCSvcPluginHandler - begin");
    
    iIapIds.Close();
    iSipProfileIds.Close();

    if ( iDomainNames )
        {
        iDomainNames->Reset();
        }
    delete iDomainNames;
    delete iRcseHandler;
    delete iSipHandler;
    delete iCenRepHandler;
    
    CSCSVCPLUGINDEBUG("CCSCSvcPluginHandler::~CCSCSvcPluginHandler - end");
    }


// ---------------------------------------------------------------------------
// For settings removation.
// ---------------------------------------------------------------------------
//
void CCSCSvcPluginHandler::RemoveSettingsL( TUint32 aServiceId )
    {
    CSCSVCPLUGINDEBUG("CCSCSvcPluginHandler::RemoveSettingsL - begin");
    
    // Search data based on service id
    SearchSettingsDataL( aServiceId );
        
    // Unregister. (SIP profiles are set to when needed mode.)
    for ( TInt i = 0; i < iSipProfileIds.Count(); i++ )
        {
        iSipHandler->SetRegistrationModeL( iSipProfileIds[ i ], EFalse );
        }
    
    // Remove all profiles/settings.
    RemoveProfilesL( aServiceId );
    
    CSCSVCPLUGINDEBUG("CCSCSvcPluginHandler::RemoveSettingsL - end");
    }

// ---------------------------------------------------------------------------
// Searches all settings data from databases. 
// ---------------------------------------------------------------------------
//
void CCSCSvcPluginHandler::SearchSettingsDataL( TUint32 aServiceId )
    {
    CSCSVCPLUGINDEBUG("CCSCSvcPluginHandler::SearchSettingsDataL - begin");
    
    // Reset member variables to default values.
    iVoIPProfileId = KErrNone;
    iSNAPId = KErrNone;
    iPresenceId = KErrNone;
    iSipProfileIds.Reset();
    iIapIds.Reset();
    iDomainNames->Reset();
       
    // Get SNAP ID from service table
    CSPSettings* spSettings = CSPSettings::NewLC();
    CSPProperty* property = CSPProperty::NewLC();
    
    TInt err =  spSettings->FindPropertyL( 
        aServiceId, ESubPropertyVoIPPreferredSNAPId, *property );
    
    if ( !err )
        {
        property->GetValue( iSNAPId );
        }
    
    TInt err2( KErrNone );
    err2 =  spSettings->FindPropertyL( 
        aServiceId, ESubPropertyPresenceSettingsId, *property );
    
    if ( !err2 )
        {
        property->GetValue( iPresenceId );
        }
        
    CleanupStack::PopAndDestroy( property );
    CleanupStack::PopAndDestroy( spSettings );
        
    // Get VoIP profile id and SIP reference ids from RCSE handler.
    iRcseHandler->GetProfileIdsL( 
        aServiceId,
        iVoIPProfileId,
        iSipProfileIds );
    
    for ( TInt i = 0; i < iSipProfileIds.Count(); i++ )
        {
        // Get Access Point Id from SIP profile and append it to id array.
        TUint32 accessPointId = 0;
        iSipHandler->GetApIdL( iSipProfileIds[ i ], accessPointId );
        
        if ( accessPointId )
            {
            iIapIds.Append( ( TUint32 ) accessPointId );
            }
            
        // Get Domain name from SIP profile and append it to desc array.
        TBuf8<KSvcPluginSettingLength> publicUsername( KNullDesC8 );
        iSipHandler->GetUserAorL( iSipProfileIds[ i ], publicUsername );

        TInt location = publicUsername.Find( KSvcPluginAtCharacter );
        
        if ( KErrNotFound != location )
            {
            iDomainNames->AppendL( publicUsername.Mid( location + 1 ) );
            }
        }
    
    CSCSVCPLUGINDEBUG("CCSCSvcPluginHandler::SearchSettingsDataL - end");
    }
    
// ---------------------------------------------------------------------------
// Removes settings.
// ---------------------------------------------------------------------------
//
void CCSCSvcPluginHandler::RemoveProfilesL( TUint32 aServiceId )
    {
    CSCSVCPLUGINDEBUG("CCSCSvcPluginHandler::RemoveProfilesL - begin");
        
    // Destroy VoIP profile from RCSE. 
    iRcseHandler->RemoveProfileL( iVoIPProfileId );
    
    // Remove entry from service table
    CSPSettings* spSettings = CSPSettings::NewLC();
    spSettings->DeleteEntryL( aServiceId );
    CleanupStack::PopAndDestroy( spSettings );
          
    // Destroy SIP profiles from SIP Stack.
    for ( TInt i = 0; i < iSipProfileIds.Count(); i++ )
        {
        iSipHandler->RemoveProfileL( iSipProfileIds[ i ] );
        }
    
    // Destroy IAP specific NAT/FW settings.
    for ( TInt i = 0; i < iIapIds.Count(); i++ )
        {
        // Trapped, because function leaves 
        // if iap spesific keys wasn't found from repostiory.
        TRAP_IGNORE( iCenRepHandler->RemoveIapSpecificNatfwSettingsL( 
            iIapIds[ i ] ) );
        }
      
    // Destroy Domain specific NAT/FW settings.
    for ( TInt i = 0; i < iDomainNames->Count(); i++ )
        {
        // Trapped, because function leaves 
        // if domain spesific keys wasn't found from repostiory.
        TRAP_IGNORE( iCenRepHandler->RemoveDomainSpecificNatfwSettingsL( 
            iDomainNames->MdcaPoint( i ) ) );
        }
    
     // Delete SNAP
    if ( iSNAPId )
        {
        RCmManagerExt manager;
        CleanupClosePushL( manager );
        manager.OpenL();     
        RCmDestinationExt dest;
        CleanupClosePushL( dest );
        dest = manager.DestinationL ( iSNAPId );
        
        // Delete SNAP only if it´s not Internet SNAP.
        TUint32 internetSnapId( KErrNone );
        internetSnapId = dest.MetadataL( CMManager::ESnapMetadataInternet );
        if( internetSnapId == KErrNone )
            {
            // May leave if some connection method inside SNAP is in use
            TRAP_IGNORE( dest.DeleteLD() );
            }
        
        CleanupStack::PopAndDestroy( &dest );        
        CleanupStack::PopAndDestroy( &manager );
        
        CSCSVCPLUGINDEBUG( 
            "CEgpSettingsController::RemoveProfilesL SNAP deleted" );
        }
    
     // Delete presence and xdm settings
    if ( iPresenceId )
        {
        TPresSettingsSet presSettingSet;
                
        // Delete xmd settings
        PresSettingsApi::SettingsSetL( iPresenceId, presSettingSet );        
        TXdmSettingsApi::RemoveCollectionL( presSettingSet.iXDMSetting );
              
        // Delete presence settings
        TInt err = PresSettingsApi::RemoveSetL( iPresenceId );
        
        CSCSVCPLUGINDEBUG2( 
         "CCSCSvcPluginHandler::RemoveProfilesL presence and xdm deleted, ERR=%d", err );
        }
    
    CSCSVCPLUGINDEBUG("CCSCSvcPluginHandler::RemoveProfilesL - begin");
    }    
