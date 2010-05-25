/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include <crcseprofileregistry.h>
#include <spsettings.h>
#include <spentry.h>
#include <spproperty.h>
#include <spsettingsvoiputils.h>
#include <pressettingsapi.h>
#include <xdmsettingsapi.h>
#include <xdmsettingscollection.h>

#include "scpsettinghandler.h"
#include "scpservice.h"
#include "scpsubservice.h"
#include "scplogger.h"
#include "scputility.h"
#include "scpservicehandlerbase.h"
#include "sipconnectionprovideruids.hrh"
#include "scpprofilehandler.h"

// -----------------------------------------------------------------------------
// CScpSettingHandler::CScpSettingHandler
// -----------------------------------------------------------------------------
//
CScpSettingHandler::CScpSettingHandler( CScpProfileHandler& aProfileHandler ) :
    iProfileHandler( aProfileHandler )
    {
    SCPLOGSTRING( "CScpSettingHandler::CScpSettingHandler" );
    }

// -----------------------------------------------------------------------------
// CScpSettingHandler::NewL
// -----------------------------------------------------------------------------
//
CScpSettingHandler* CScpSettingHandler::NewL( CScpProfileHandler& aProfileHandler )
    {
    SCPLOGSTRING( "CScpSettingHandler::NewL()" );

    CScpSettingHandler* self = new( ELeave ) CScpSettingHandler( aProfileHandler );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CScpSettingHandler::~CScpSettingHandler
// -----------------------------------------------------------------------------
//
CScpSettingHandler::~CScpSettingHandler()
    {
    SCPLOGSTRING( "CScpSettingHandler::~CScpSettingHandler" );

    delete iSpsVoIPUtils;
    delete iSpSettings;
    delete iRcseProfileEntry;
    delete iRcseProfileRegistry;
    }

// -----------------------------------------------------------------------------
// CScpSettingHandler::ConstructL
// -----------------------------------------------------------------------------
//
void CScpSettingHandler::ConstructL()
    {
    SCPLOGSTRING("CScpSettingHandler::ConstructL" );

    iRcseProfileRegistry = CRCSEProfileRegistry::NewL();
    iRcseProfileEntry = CRCSEProfileEntry::NewL();

    iSpSettings = CSPSettings::NewL(); 
    iSpsVoIPUtils = CSPSettingsVoIPUtils::NewL();
    }

// -----------------------------------------------------------------------------
// CScpSettingHandler::GetSipProfileIdByTypeL
// -----------------------------------------------------------------------------
//
void CScpSettingHandler::GetSipProfileIdByTypeL( TUint32 aServiceId,
                                                 TCCHSubserviceType aSubServiceType, 
                                                 TUint32& aSipId ) const
    {
    SCPLOGSTRING3( "CScpSettingHandler::GetSipProfileIdByTypeL service id :%i type %d", 
                    aServiceId, aSubServiceType );

    switch ( aSubServiceType )
        {
        case ECCHVoIPSub:
        case ECCHIMSub:
            {
            GetVoipSipProfileIdL( aServiceId, aSipId );
            }
            break;

        case ECCHVMBxSub:
            {
            GetVmbxSipProfileIdL( aServiceId, aSipId );
            }
            break;

        case ECCHPresenceSub:
            {
            GetPresenceSipProfileIdL( aServiceId, aSipId );
            }
            break;

        case ECCHUnknown:
            {
            GetUnknownSipProfileIdL( aServiceId, aSipId );
            }
            break;
    
        default:
            __ASSERT_DEBUG( EFalse, User::Panic( KNullDesC, KErrGeneral ) );
            break;
        }
    }
    
// -----------------------------------------------------------------------------
// CScpSettingHandler::GetSPSettingsIntPropertyL
// -----------------------------------------------------------------------------
//    
 void CScpSettingHandler::GetSPSettingsIntPropertyL( TUint32 aServiceId,
                                                     TServicePropertyName aPropertyName, 
                                                     TInt& aProperty ) const
    {
    SCPLOGSTRING( "CScpSettingHandler::GetSPSettingsIntPropertyL" );

    CSPEntry* entry = CSPEntry::NewLC();
    TInt result = iSpSettings->FindEntryL( aServiceId, *entry );

    if ( result == KErrNone )
        {
        // Get asked property 
        const CSPProperty* property = NULL;
        result = entry->GetProperty( property, aPropertyName );

        if ( result == KErrNone && property )
            {
            TInt value( 0 );
            property->GetValue( value );
            aProperty = value;
            }
        else
            {
            User::Leave( KErrNotFound );
            }
        }
    else
        {
        User::Leave( KErrNotFound );
        }

    CleanupStack::PopAndDestroy( entry );        
    }
    
// -----------------------------------------------------------------------------
// CScpSettingHandler::GetVoipSipProfileIdL
// -----------------------------------------------------------------------------
//
void CScpSettingHandler::GetVoipSipProfileIdL( TUint32 aServiceId, 
                                               TUint32& aSipId ) const
    {
    SCPLOGSTRING( "CScpSettingHandler::GetVoipSipProfileIdL" );

    RPointerArray<CRCSEProfileEntry> entries;

    // Push entries to cleanup stack
    CleanupStack::PushL( TCleanupItem( TScpUtility::ResetAndDestroyEntries, 
                                       &entries ) );

    iRcseProfileRegistry->FindByServiceIdL( aServiceId, entries );
    
    if ( entries.Count() )
        {            
        CRCSEProfileEntry* entry = entries[0];
        if ( entry->iIds.Count() > 0 )
            {
            aSipId = entry->iIds[0].iProfileId;
            }
        else
            {
            User::Leave( KErrNotFound );
            }
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    
    CleanupStack::PopAndDestroy( &entries );
    }

// -----------------------------------------------------------------------------
// CScpSettingHandler::GetVmbxSipProfileIdL
// -----------------------------------------------------------------------------
//
void CScpSettingHandler::GetVmbxSipProfileIdL( TUint32 aServiceId, 
                                               TUint32& aSipId ) const
    {
    SCPLOGSTRING( "CScpSettingHandler::GetVmbxSipProfileIdL" );

    TInt sipId;
    GetSPSettingsIntPropertyL( aServiceId, ESubPropertyVMBXSettingsId, sipId );
    aSipId = sipId;
    }

// -----------------------------------------------------------------------------
// CScpSettingHandler::GetPresenceSipProfileIdL
// -----------------------------------------------------------------------------
//
void CScpSettingHandler::GetPresenceSipProfileIdL( TUint32 aServiceId, 
                                                   TUint32& aSipId ) const
    {
    SCPLOGSTRING( "CScpSettingHandler::GetPresenceSipProfileIdL" );

    CSPEntry* entry = CSPEntry::NewLC();
    TInt result = iSpSettings->FindEntryL( aServiceId, *entry );
    if ( result == KErrNone )
        {
        // Get presence setting id for presence service
        const CSPProperty* property = NULL;
        result = entry->GetProperty( property, ESubPropertyPresenceSettingsId );

        if ( result == KErrNone && property )
            {
            TInt presId;
            property->GetValue( presId );

            //Found out what is sip id in presencesettings
            TPresSettingsSet mySet;
            User::LeaveIfError( PresSettingsApi::SettingsSetL( presId, mySet ));

            TInt sipId( 0 );
            sipId = mySet.iSipProfile;
            aSipId = (TUint32)(sipId);
            }
        else
            {
            User::Leave( KErrNotFound );
            }         
        }
    else
        {
        User::Leave( KErrNotFound );
        }

    CleanupStack::PopAndDestroy( entry );
    }

// -----------------------------------------------------------------------------
// CScpSettingHandler::GetUnknownSipProfileIdL
// -----------------------------------------------------------------------------
//
void CScpSettingHandler::GetUnknownSipProfileIdL( TUint32 aServiceId, 
                                                  TUint32& aSipId ) const
    {
    SCPLOGSTRING( "CScpSettingHandler::GetVoipSipProfileIdL" );
    
    TInt result( KErrNone );
    TUint32 genericSipId( KErrNone );
    RArray< TInt > sipIds;
    CleanupClosePushL( sipIds );
    
    TRAP( result, GetSipProfileIdByTypeL( aServiceId, ECCHVoIPSub, genericSipId ) );
    
    if ( result == KErrNone )
        {
        sipIds.AppendL( genericSipId );
        }
    else if ( KErrNoMemory == result )
        {
        User::Leave( KErrNoMemory );
        }

    TRAP( result, GetSipProfileIdByTypeL( aServiceId, ECCHVMBxSub, genericSipId ) );

    if ( result == KErrNone )
        {
        sipIds.AppendL( genericSipId );
        }
    else if ( KErrNoMemory == result )
        {
        User::Leave( KErrNoMemory );
        }
    
    TRAP( result, GetSipProfileIdByTypeL( aServiceId, ECCHPresenceSub, genericSipId ) );

    if ( result == KErrNone )
        {
        sipIds.AppendL( genericSipId );
        }
    else if ( KErrNoMemory == result )
        {
        User::Leave( KErrNoMemory );
        }
    
    // Check if all the sip profiles ids for different subservice types
    // are the same
    if ( sipIds.Count() > 0 )
        {
        result = KErrNone;
        aSipId = sipIds[0];

        for( TInt i=0; i<sipIds.Count(); i++ )
            {
            if( sipIds[i] != aSipId )
                {
                result = KErrArgument;
                break;
                }
            }
        }
    else
        {        
        // There wasn't any sip profile ids
        result = KErrNotFound;
        }

    CleanupStack::PopAndDestroy( &sipIds );

    User::LeaveIfError( result );
    }

// -----------------------------------------------------------------------------
// CScpSettingHandler::UpdateSettingsL
// -----------------------------------------------------------------------------
//
void CScpSettingHandler::UpdateSettingsL( CScpService& aService,
                                          TCCHSubserviceType aSubServiceType ) const
    {
    SCPLOGSTRING4( "CScpSettingHandler::UpdateSettingsL: 0x%x id: %i type: %d", 
                   &aService, aService.ServiceId(), aSubServiceType );
    __ASSERT_DEBUG( aService.ServiceId() > KErrNotFound, 
                    User::Panic( KNullDesC, KErrNotFound ) );

    if ( !aService.ContainsSubServiceType( ECCHVoIPSub ) &&
        ( aSubServiceType == ECCHVoIPSub || aSubServiceType == ECCHUnknown ) )
        {
        SCPLOGSTRING( "CScpSettingHandler::UpdateSettingsL voip " );
        UpdateVoIPSettingsL( aService );
        }

    if ( !aService.ContainsSubServiceType( ECCHIMSub ) &&
        ( ECCHIMSub == aSubServiceType || ECCHUnknown == aSubServiceType ) )
        {
        SCPLOGSTRING( "CScpSettingHandler::UpdateSettingsL IM" );
        UpdateImSettingsL( aService );
        }

    if ( !aService.ContainsSubServiceType( ECCHVMBxSub ) &&
        ( aSubServiceType == ECCHVMBxSub || aSubServiceType == ECCHUnknown ) )
        {
        SCPLOGSTRING( "CScpSettingHandler::UpdateSettingsL vmbx " );
        UpdateVmbxSettingsL( aService );
        }

    if ( !aService.ContainsSubServiceType( ECCHPresenceSub ) &&
        (aSubServiceType == ECCHPresenceSub || aSubServiceType == ECCHUnknown ) )
        {
        SCPLOGSTRING( "CScpSettingHandler::UpdateSettingsL presence x " );
        UpdatePresenceSettingsL( aService );
        }
 
    if ( aService.SubServiceCount() == 0 )
        {
        User::Leave( KErrNotFound );
        }   
    }

// -----------------------------------------------------------------------------
// CScpSettingHandler::UpdateXdmUsernameL
// -----------------------------------------------------------------------------
//
void CScpSettingHandler::UpdateXdmUsernameL( CScpService& aService, 
    TCCHSubserviceType aSubServiceType, const TDesC8& aUsername )
    {
    SCPLOGSTRING2( "CScpSettingHandler::UpdateXdmUsernameL, service id: %d", 
        aService.ServiceId() );
    
    if ( aService.ContainsSubServiceType( ECCHPresenceSub ) &&
        ( aSubServiceType == ECCHPresenceSub || 
        aSubServiceType == ECCHUnknown ) )
        {
        CSPEntry* entry = CSPEntry::NewLC();
        User::LeaveIfError( iSpSettings->FindEntryL( 
            aService.ServiceId(), *entry ) );
                   
        if ( ArePresenceSettingsValid( *entry ) )
            {                
            TInt presenceSettingsId = GetPresenceSettingsId( *entry );
                
            if ( presenceSettingsId != KErrNotFound )
                {                    
                // Find out the sip profile that the presence is using
                TPresSettingsSet mySet;
                TRAPD( result, 
                    PresSettingsApi::SettingsSetL( 
                         presenceSettingsId, mySet ) );
                    
                if ( KErrNone == result && mySet.iSipProfile > 0 )
                    {                        
                    // check XDM settings are valid
                    CXdmSettingsCollection* xdmCollection = NULL;
                    TRAPD( xdmResult, xdmCollection = 
                        TXdmSettingsApi::SettingsCollectionL(
                             mySet.iXDMSetting ) );
                        
                    if ( KErrNone == xdmResult && xdmCollection )
                        {
                        CleanupStack::PushL( xdmCollection );
                            
                        if ( iProfileHandler.ProfileExists( 
                            mySet.iSipProfile ) )
                            {
                            RBuf8 formattedUsername;
                            CleanupClosePushL( formattedUsername );
                                
                            if( !TScpUtility::CheckSipUsername( aUsername ) )
                                {
                                // Since username is returned with prefix and
                                // domain, it needs to be possible to also set
                                // it with prefix and domain.  Strip prefix
                                // and domain from user aor if found. Prefix
                                // might be sip or sips so search by colon.
                                User::LeaveIfError( 
                                    TScpUtility::RemovePrefixAndDomain(
                                        aUsername, formattedUsername ) );
                                }
                            else
                                {
                                formattedUsername.CreateL( 
                                    aUsername.Length() );
                                formattedUsername.Copy( aUsername );
                                }
 
                            RBuf userName;
                            CleanupClosePushL( userName );
                            userName.CreateL( formattedUsername.Length() );
                            userName.Copy( formattedUsername );
                            
                            // Update username to XDM-setting.
                            TXdmSettingsApi::UpdatePropertyL( 
                                mySet.iXDMSetting,
                                userName,
                                EXdmPropAuthName );
                                
                            CleanupStack::PopAndDestroy( &userName );
                            CleanupStack::PopAndDestroy( &formattedUsername );
                            
                            SCPLOGSTRING( 
                            "CScpSettingHandler::UpdateXdmUsernameL - DONE" );
                            }
                        else
                            {
                            User::Leave( KErrNotFound );
                            }
                            
                        CleanupStack::PopAndDestroy( xdmCollection );
                        }
                    }
                }
            }
        CleanupStack::PopAndDestroy( entry );
        }
 
    if ( aService.SubServiceCount() == 0 )
        {        
        User::Leave( KErrNotFound );
        }
    }

// -----------------------------------------------------------------------------
// CScpSettingHandler::UpdateXdmPasswordL
// -----------------------------------------------------------------------------
//
void CScpSettingHandler::UpdateXdmPasswordL( CScpService& aService, 
    TCCHSubserviceType aSubServiceType, const TDesC8& aPassword )
    {
    SCPLOGSTRING2( "CScpSettingHandler::UpdateXdmPasswordL, service id: %d",
        aService.ServiceId() );
    
    if ( aService.ContainsSubServiceType( ECCHPresenceSub ) &&
        ( aSubServiceType == ECCHPresenceSub || 
        aSubServiceType == ECCHUnknown ) )
        {
        SCPLOGSTRING( "CScpSettingHandler::UpdateXdmPasswordL, presence" );

        CSPEntry* entry = CSPEntry::NewLC();
        User::LeaveIfError( iSpSettings->FindEntryL( 
            aService.ServiceId(), *entry ) );
        
        if ( ArePresenceSettingsValid( *entry ) )
            {
            TInt presenceSettingsId = GetPresenceSettingsId( *entry );

            if ( presenceSettingsId != KErrNotFound )
                {
                // Find out the sip profile that the presence is using
                TPresSettingsSet mySet;

                TRAPD( result, 
                    PresSettingsApi::SettingsSetL(
                        presenceSettingsId, mySet ) );
                       
                if ( KErrNone == result && mySet.iSipProfile > 0 )
                    {
                    // check XDM settings are valid
                    CXdmSettingsCollection* xdmCollection = NULL;
                    TRAPD( xdmResult, xdmCollection = 
                        TXdmSettingsApi::SettingsCollectionL(
                            mySet.iXDMSetting ) );
                        
                    if ( KErrNone == xdmResult && xdmCollection )
                        {
                        CleanupStack::PushL( xdmCollection );
                                 
                        if ( iProfileHandler.ProfileExists( 
                            mySet.iSipProfile ) )
                            {
                            RBuf passwordBuf;
                            CleanupClosePushL( passwordBuf );
                            passwordBuf.CreateL( aPassword.Length() );
                            passwordBuf.Copy( aPassword );
                                
                            // Update password to XDM-setting
                            TXdmSettingsApi::UpdatePropertyL( 
                                mySet.iXDMSetting,
                                passwordBuf,
                                EXdmPropAuthSecret );    
                                    
                            CleanupStack::PopAndDestroy( &passwordBuf );
                            
                            SCPLOGSTRING( 
                            "CScpSettingHandler::UpdateXdmPasswordL - DONE" );
                            }
                        else
                            {
                            User::Leave( KErrNotFound );
                            }

                        CleanupStack::PopAndDestroy( xdmCollection );
                        }
                    }
                }
            }
        CleanupStack::PopAndDestroy( entry );
        }
 
    if ( aService.SubServiceCount() == 0 )
        {
        User::Leave( KErrNotFound );
        }           
    }

// -----------------------------------------------------------------------------
// CScpSettingHandler::UpdatePresenceSettingsL
// -----------------------------------------------------------------------------
//
void CScpSettingHandler::UpdatePresenceSettingsL( CScpService& aService ) const
    {
    SCPLOGSTRING( "CScpSettingHandler::UpdatePresenceSettingsL" );

    CSPEntry* entry = CSPEntry::NewLC();
    
    if ( iSpSettings->FindEntryL( aService.ServiceId(), *entry ) == KErrNone )
        {
        if ( ArePresenceSettingsValid( *entry ) )
            {
            TInt presenceSettingsId = GetPresenceSettingsId( *entry );

            if ( presenceSettingsId != KErrNotFound )
                {
                // Find out the sip profile that the presence is using
                TPresSettingsSet mySet;

                TRAPD( result, 
                       PresSettingsApi::SettingsSetL( presenceSettingsId, mySet ) );
                   
                if ( KErrNone == result && mySet.iSipProfile > 0 )
                    {
                    // check XDM settings are valid
                    CXdmSettingsCollection* xdmCollection = NULL;
                    TRAPD( xdmResult, xdmCollection = 
                        TXdmSettingsApi::SettingsCollectionL( mySet.iXDMSetting ) );
                    
                    if ( KErrNone == xdmResult && xdmCollection )
                        {
                        CleanupStack::PushL( xdmCollection );
                                                
                        if ( iProfileHandler.ProfileExists( mySet.iSipProfile ) )
                            {
                            CScpSubService& presenceService = aService.AddSubServiceL( ECCHPresenceSub );

                            presenceService.SetSipProfileId( mySet.iSipProfile );
                            }
                        else
                            {
                            User::Leave( KErrNotFound );
                            }

                        CleanupStack::PopAndDestroy( xdmCollection );
                        }
                    }
                }
            }
        }
    CleanupStack::PopAndDestroy( entry );
    }

// -----------------------------------------------------------------------------
// CScpSettingHandler::ArePresenceSettingsValid
// -----------------------------------------------------------------------------
//
TBool CScpSettingHandler::ArePresenceSettingsValid( const CSPEntry& aEntry ) const
    {
    SCPLOGSTRING( "CScpSettingHandler::ArePresenceSettingsValid" );

    const CSPProperty* property = NULL;

    // Make sure that the this is the correct plugin
    TInt result = aEntry.GetProperty( property, EPropertyPresenceSubServicePluginId );
    
    if ( result == KErrNone && property )
        {
        TInt pluginId = 0;
        property->GetValue( pluginId );

        SCPLOGSTRING2( "Presence sub service plugin UID: 0x%x", pluginId );

        if ( pluginId == KSIPConnectivityPluginImplUid )
            {
            return ETrue;
            }
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CScpSettingHandler::GetPresenceSettingsId
// -----------------------------------------------------------------------------
//
TInt CScpSettingHandler::GetPresenceSettingsId( const CSPEntry& aEntry ) const
    {
    SCPLOGSTRING( "CScpSettingHandler::GetPresenceSettingsId" );

    const CSPProperty* property = NULL;
    TInt result = aEntry.GetProperty( property, ESubPropertyPresenceSettingsId );

    if ( property && result == KErrNone )
        {
        TInt presenceSettingsId = 0;
        result = property->GetValue( presenceSettingsId );

        if ( result == KErrNone )
            {
            return presenceSettingsId;
            }
        }

    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CScpSettingHandler::UpdateVoIPSettingsL
// -----------------------------------------------------------------------------
//
void CScpSettingHandler::UpdateVoIPSettingsL( CScpService& aService ) const
    {
    SCPLOGSTRING( "CScpSettingHandler::UpdateVoIPSettingsL" );

    CSPEntry* entry = CSPEntry::NewLC();
    
    if ( iSpSettings->FindEntryL( aService.ServiceId(), *entry ) == KErrNone )
        {
        if ( AreVoipSettingsValid( *entry ) )
            {
            RPointerArray<CRCSEProfileEntry> entries;
            // Push entries to cleanup stack
            CleanupStack::PushL( TCleanupItem( TScpUtility::ResetAndDestroyEntries, 
                                               &entries ) );
                                  
            iRcseProfileRegistry->FindByServiceIdL( aService.ServiceId(), entries );
            
            TInt idsCount = entries.Count();

            if ( idsCount > 0 )
                {
                CRCSEProfileEntry* profileEntry = entries[ 0 ];
                
                if ( profileEntry && profileEntry->iIds.Count() )
                    {
                    TSettingIds& settingId = profileEntry->iIds[ 0 ];
                    
                    __ASSERT_DEBUG( settingId.iProfileId > 0, 
                                    User::Panic( KNullDesC, KErrGeneral ) );            

                    if ( iProfileHandler.ProfileExists( settingId.iProfileId ) )
                        {
                        CScpSubService& voipService = aService.AddSubServiceL( ECCHVoIPSub );
            
                        voipService.SetSipProfileId( settingId.iProfileId );
                        }
                    else
                        {
                        User::Leave( KErrNotFound );
                        }
                    }
                }
            
            CleanupStack::PopAndDestroy( &entries );
            }
        }

    CleanupStack::PopAndDestroy( entry );
    }

// -----------------------------------------------------------------------------
// CScpSettingHandler::AreVoipSettingsValid
// -----------------------------------------------------------------------------
//
TBool CScpSettingHandler::AreVoipSettingsValid( const CSPEntry& aEntry ) const
    {
    SCPLOGSTRING( "CScpSettingHandler::AreVoipSettingsValid" );

    const CSPProperty* property = NULL;

    // Make sure that the this is the correct plugin
    TInt result = aEntry.GetProperty( property, EPropertyVoIPSubServicePluginId );
    
    if ( result == KErrNone && property )
        {
        TInt pluginId = 0;
        property->GetValue( pluginId );

        SCPLOGSTRING3( "VoIP sub service plugin UID: 0x%x this 0x%x", pluginId, KSIPConnectivityPluginImplUid );

        if ( pluginId == KSIPConnectivityPluginImplUid )
            {
            return ETrue;
            }
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CScpSettingHandler::UpdateVmbxSettingsL
// -----------------------------------------------------------------------------
//
void CScpSettingHandler::UpdateVmbxSettingsL( CScpService& aService ) const
    {
    SCPLOGSTRING( "CScpSettingHandler::UpdateVmbxSettingsL" );

    CSPEntry* entry = CSPEntry::NewLC();
    
    if ( iSpSettings->FindEntryL( aService.ServiceId(), *entry ) == KErrNone )
        {   
        if( AreVmbxSettingsValid( *entry ) )
            {
            const CSPProperty* property = NULL;

            // Get sip profile id for the vmbx service
            TInt result = entry->GetProperty( property, ESubPropertyVMBXSettingsId );

            if ( result == KErrNone && property )
                {
                TInt sipProfileId( 0 );
                property->GetValue( sipProfileId );
            
                __ASSERT_DEBUG( sipProfileId > 0, User::Panic( KNullDesC, KErrGeneral ) );
                
                if ( iProfileHandler.ProfileExists( sipProfileId ) )
                    {
                    CScpSubService& vmbxService = aService.AddSubServiceL( ECCHVMBxSub );
                
                    vmbxService.SetSipProfileId( sipProfileId );
                    }
                else
                    {
                    User::Leave( KErrNotFound );
                    }
                }
            }
        }

    CleanupStack::PopAndDestroy( entry );
    }

// -----------------------------------------------------------------------------
// CScpSettingHandler::AreVmbxSettingsValid
// -----------------------------------------------------------------------------
//
TBool CScpSettingHandler::AreVmbxSettingsValid( const CSPEntry& aEntry ) const
    {
    SCPLOGSTRING( "CScpSettingHandler::AreVmbxSettingsValid" );

    const CSPProperty* property = NULL;

    // Make sure that the this is the correct plugin
    TInt result = aEntry.GetProperty( property, EPropertyVMBXSubServicePluginId );
    
    if ( result == KErrNone && property )
        {
        TInt pluginId = 0;
        property->GetValue( pluginId );

        SCPLOGSTRING2( "VMBx sub service plugin UID: 0x%x", pluginId );

        if ( pluginId == KSIPConnectivityPluginImplUid )
            {
            // Make sure that the MWI address is found
            result = aEntry.GetProperty( property, ESubPropertyVMBXMWIAddress );
            
            if ( result == KErrNone && property )
                {
                return ETrue;
                }
            }
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CScpSettingHandler::ServiceExistsL
// -----------------------------------------------------------------------------
//
TBool CScpSettingHandler::ServiceExistsL( TUint32 aServiceId ) const
    {
    SCPLOGSTRING2( "CScpSettingHandler::ServiceExistsL id: %d", aServiceId );

    TBool serviceExists( EFalse );

    RIdArray serviceIds;
    iSpSettings->FindServiceIdsL( serviceIds );

    for ( TInt i=0; i<serviceIds.Count(); i++ )
        {
        if ( aServiceId == serviceIds[ i ] )
            {
            serviceExists = ETrue;
            break;
            }
        }
    
    serviceIds.Close();
    return serviceExists;
    }
    
// -----------------------------------------------------------------------------
// CScpSettingHandler::GetServiceId
// -----------------------------------------------------------------------------
//
TInt CScpSettingHandler::GetServiceIds( TUint32 aSipId, 
                                        RArray< TUint32 >& aServiceIds ) const
    {
    SCPLOGSTRING2( "CScpSettingHandler::GetServiceIds sip id: %d", aSipId );

    TRAPD( result, GetServiceIdsL( aSipId, aServiceIds ) );

    return result;
    }

// -----------------------------------------------------------------------------
// CScpSettingHandler::GetServiceIdsL
// -----------------------------------------------------------------------------
//
void CScpSettingHandler::GetServiceIdsL( TUint32 aSipId, 
                                         RArray< TUint32 >& aServiceIds ) const
    {
    SCPLOGSTRING2( "CScpSettingHandler::GetServiceIdsL sip id: %d", aSipId );
    
    CleanupClosePushL( aServiceIds );

    RArray<TUint32> profileIdArray;
    CleanupClosePushL( profileIdArray );    

    iRcseProfileRegistry->GetAllIdsL( profileIdArray );

    TBool found( EFalse );
    
    //VoIP profile count
    for ( TInt i=0; i < profileIdArray.Count() && !found; i++ )
        {
        // profileEntry is reseted by rcse before the search
        iRcseProfileRegistry->FindL( profileIdArray[i], 
                                     *iRcseProfileEntry );

        for ( TInt j=0; j < iRcseProfileEntry->iIds.Count(); j++ )
            {
            if ( aSipId == iRcseProfileEntry->iIds[j].iProfileId )
                {
                aServiceIds.Append( profileIdArray[i] );
                }
            }
        }

    if ( !aServiceIds.Count() )
        {
        User::Leave( KErrNotFound );
        }

    CleanupStack::PopAndDestroy( &profileIdArray );
    CleanupStack::Pop( &aServiceIds );
    }

// -----------------------------------------------------------------------------
// CScpSettingHandler::IsVoIPSupported
// -----------------------------------------------------------------------------
//
TBool CScpSettingHandler::IsVoIPSupported() const
    {
    return iSpsVoIPUtils->IsVoIPSupported();
    }

// -----------------------------------------------------------------------------
// CScpSettingHandler::IsUahTerminalTypeDefinedL
// -----------------------------------------------------------------------------
//
TBool CScpSettingHandler::IsUahTerminalTypeDefinedL(
    TUint32 aServiceId ) const
    {
    TBool defined( EFalse );
    RPointerArray<CRCSEProfileEntry> entries;

    // Push entries to cleanup stack
    CleanupStack::PushL( TCleanupItem( TScpUtility::ResetAndDestroyEntries, 
                                       &entries ) );

    iRcseProfileRegistry->FindByServiceIdL( aServiceId, entries );
    
    if ( entries.Count() )
        {            
        // SIP VoIP User Agent header: terminal type display
        defined = entries[0]->iSIPVoIPUAHTerminalType;
        }
    
    CleanupStack::PopAndDestroy( &entries );
    return defined;
    }

// -----------------------------------------------------------------------------
// CScpSettingHandler::IsUahWLANMacDefinedL
// -----------------------------------------------------------------------------
//
TBool CScpSettingHandler::IsUahWLANMacDefinedL(
    TUint32 aServiceId ) const
    {
    TBool defined( EFalse );
    RPointerArray<CRCSEProfileEntry> entries;

    // Push entries to cleanup stack
    CleanupStack::PushL( TCleanupItem( TScpUtility::ResetAndDestroyEntries, 
                                       &entries ) );

    iRcseProfileRegistry->FindByServiceIdL( aServiceId, entries );
    
    if ( entries.Count() )
        {            
        // SIP VoIP User Agent header WLAN MAC address display
        defined = entries[0]->iSIPVoIPUAHeaderWLANMAC;
        }
    
    CleanupStack::PopAndDestroy( &entries );
    return defined;
    }

// -----------------------------------------------------------------------------
// CScpSettingHandler::UahStringLengthL
// -----------------------------------------------------------------------------
//
TInt CScpSettingHandler::UahStringLengthL(
    TUint32 aServiceId ) const
    {
    TBool length( KErrNone );
    RPointerArray<CRCSEProfileEntry> entries;

    // Push entries to cleanup stack
    CleanupStack::PushL( TCleanupItem( TScpUtility::ResetAndDestroyEntries, 
                                       &entries ) );

    iRcseProfileRegistry->FindByServiceIdL( aServiceId, entries );
    
    if ( entries.Count() )
        {            
        // SIP VoIP User Agent header string
        length = entries[0]->iSIPVoIPUAHeaderString.Length();
        }
    
    CleanupStack::PopAndDestroy( &entries );
    return length;
    }

// -----------------------------------------------------------------------------
// CScpSettingHandler::IsVoIPOverWcdmaAllowedL
// -----------------------------------------------------------------------------
//
TBool CScpSettingHandler::IsVoIPOverWcdmaAllowedL( 
    TUint32 aServiceId ) const
    {
    SCPLOGSTRING( "CScpSettingHandler::IsVoIPOverWcdmaAllowedL IN" );

    TBool response( EFalse );
    RPointerArray<CRCSEProfileEntry> entries;

    // Push entries to cleanup stack
    CleanupStack::PushL( TCleanupItem( TScpUtility::ResetAndDestroyEntries, 
                                       &entries ) );

    iRcseProfileRegistry->FindByServiceIdL( aServiceId, entries );
    
    if ( entries.Count() )
        {            
        response = CRCSEProfileEntry::EOn == entries[ 0 ]->iAllowVoIPoverWCDMA;
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    
    CleanupStack::PopAndDestroy( &entries );
    
    SCPLOGSTRING2( 
        "CScpSettingHandler::IsVoIPOverWcdmaAllowedL : allowed : %d OUT", 
            response );
    return response;
    }

// -----------------------------------------------------------------------------
// CScpSettingHandler::UpdateImSettingsL
// -----------------------------------------------------------------------------
//
void CScpSettingHandler::UpdateImSettingsL( CScpService& aService ) const
    {
    SCPLOGSTRING( "CScpSettingHandler::UpdateImSettingsL" );

    CSPEntry* entry = CSPEntry::NewLC();
    
    if ( KErrNone == iSpSettings->FindEntryL( aService.ServiceId(), *entry ) )
        {
        if ( AreImSettingsValid( *entry ) )
            {
            RPointerArray<CRCSEProfileEntry> entries;
            // Push entries to cleanup stack
            CleanupStack::PushL( TCleanupItem(
                TScpUtility::ResetAndDestroyEntries, &entries ) );
            
            iRcseProfileRegistry->FindByServiceIdL( aService.ServiceId(),
                entries );
            TInt idsCount = entries.Count();
    
            if ( idsCount > 0 )
                {
                CRCSEProfileEntry* profileEntry = entries[ 0 ];
                
                if ( profileEntry && profileEntry->iIds.Count() )
                    {
                    TSettingIds& settingId = profileEntry->iIds[ 0 ];
                    
                    __ASSERT_DEBUG( settingId.iProfileId > 0,
                        User::Panic( KNullDesC, KErrGeneral ) );
    
                    if ( iProfileHandler.ProfileExists( settingId.iProfileId ) )
                        {
                        CScpSubService& imSubService = aService.AddSubServiceL(
                            ECCHIMSub );
                        imSubService.SetSipProfileId( settingId.iProfileId );
                        }
                    else
                        {
                        User::Leave( KErrNotFound );
                        }
                    }
                }
            
            CleanupStack::PopAndDestroy( &entries );
            }
        }
    
    CleanupStack::PopAndDestroy( entry );
    }

// -----------------------------------------------------------------------------
// CScpSettingHandler::AreImSettingsValid
// -----------------------------------------------------------------------------
//
TBool CScpSettingHandler::AreImSettingsValid( const CSPEntry& aEntry ) const
    {
    SCPLOGSTRING( "CScpSettingHandler::AreImSettingsValid" );
    
    const CSPProperty* property( NULL );
    TBool imFeatureEnabled = ( KErrNone == aEntry.GetProperty( 
        property, ESubPropertyIMLaunchUid ) ) && property;
    if ( !imFeatureEnabled )
        {
        return EFalse;
        }
    
    // Make sure that this is the correct plugin
    TInt result = aEntry.GetProperty( property,
        EPropertyIMSubServicePluginId );
    
    if ( KErrNone == result && property )
        {
        TInt pluginId = 0;
        property->GetValue( pluginId );
        
        SCPLOGSTRING3( "IM Subservice plugin UID: 0x%x this 0x%x",
            pluginId, KSIPConnectivityPluginImplUid );
        
        if ( KSIPConnectivityPluginImplUid == pluginId )
            {
            return ETrue;
            }
        }
    
    return EFalse;
    }

//  End of File  
