/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


#include <spsettings.h>
#include <spentry.h>
#include <crcseprofileregistry.h>
#include <sipmanagedprofile.h>
#include <sipmanagedprofileregistry.h>
#include "csvtsettingshandler.h"

// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// ResetAndDestroy for RCSE profile entry array.
// ---------------------------------------------------------------------------
//
void ResetAndDestroy( TAny* aAny )
    {
    if ( aAny ) 
        {
        static_cast<RPointerArray<CRCSEProfileEntry>*>( aAny )
            ->ResetAndDestroy();
        }
    }


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSvtSettingsHandler::CSvtSettingsHandler
// ---------------------------------------------------------------------------
//
CSvtSettingsHandler::CSvtSettingsHandler( TUint aServiceId )
    : iServiceId( aServiceId )
    {

    }


// ---------------------------------------------------------------------------
// CSvtSettingsHandler::ConstructL
// ---------------------------------------------------------------------------
//
void CSvtSettingsHandler::ConstructL()
    {
    TBool isServiceConfigured = IsServiceConfiguredL( iServiceId );
    __ASSERT_ALWAYS( isServiceConfigured, User::Leave( KErrNotFound ) );
    }


// ---------------------------------------------------------------------------
// CSvtSettingsHandler::NewL
// ---------------------------------------------------------------------------
//
CSvtSettingsHandler* CSvtSettingsHandler::NewL( TUint aServiceId )
    {
    CSvtSettingsHandler* self = CSvtSettingsHandler::NewLC( aServiceId );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CSvtSettingsHandler::NewLC
// ---------------------------------------------------------------------------
//
CSvtSettingsHandler* CSvtSettingsHandler::NewLC( TUint aServiceId )
    {
    CSvtSettingsHandler* self = new( ELeave ) CSvtSettingsHandler( aServiceId );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CSvtSettingsHandler::~CSvtSettingsHandler
// ---------------------------------------------------------------------------
//
CSvtSettingsHandler::~CSvtSettingsHandler()
    {
    
    }


// ---------------------------------------------------------------------------
// CSvtSettingsHandler::DomainPartClippingSetting()
// ---------------------------------------------------------------------------
//
TInt CSvtSettingsHandler::DomainPartClippingSetting() const
    {
    const TInt KNoDomainPartClipping = 0;
    TInt setting( KNoDomainPartClipping );
    
    CRCSEProfileEntry* profile = NULL;
    TRAP_IGNORE( profile = RcseProfileForServiceL( iServiceId ) )
    if ( profile )
        {
        setting = profile->iIgnoreAddrDomainPart;
        delete profile;
        }
    
    return setting;
    }


// ---------------------------------------------------------------------------
// CSvtSettingsHandler::GetUserAorL()
// ---------------------------------------------------------------------------
//
void CSvtSettingsHandler::GetUserAorL( RBuf& aUserAor )
    {
    aUserAor = KNullDesC();
    
    TInt sipProfileId = SipProfileIdForServiceL( iServiceId );
    
    // fetch sip profile based on sip profile id
    CSIPManagedProfileRegistry* sipProfileRegistry = 
        CreateSipProfileRegistryLC();
    CSIPManagedProfile* profile = static_cast<CSIPManagedProfile*>
        ( sipProfileRegistry->ProfileL( sipProfileId ) );
    CleanupStack::PushL( profile );
    
    // Read user aor setting from fetched sip profile.
    const TDesC8* userAor = NULL;
    User::LeaveIfError( profile->GetParameter( KSIPUserAor, userAor ) );
    if ( userAor )
        {
        if ( aUserAor.Length() < userAor->Length() )
            {
            aUserAor.ReAllocL( userAor->Length() );
            }
        
        aUserAor.Copy( *userAor );
        }
    
    CleanupStack::PopAndDestroy( profile );
    CleanupStack::PopAndDestroy( sipProfileRegistry );
    }


// ---------------------------------------------------------------------------
// From class MSIPProfileRegistryObserver.
// Implementation currently not needed because SIP profile registry is used
// only in function scope.
// ---------------------------------------------------------------------------
//
void CSvtSettingsHandler::ProfileRegistryEventOccurred( 
        TUint32 /*aProfileId*/, TEvent /*aEvent*/ )
    {
    
    }


// ---------------------------------------------------------------------------
// From class MSIPProfileRegistryObserver.
// Implementation currently not needed because SIP profile registry is used
// only in function scope.
// ---------------------------------------------------------------------------
//
void CSvtSettingsHandler::ProfileRegistryErrorOccurred( 
        TUint32 /*aProfileId*/, TInt /*aError*/ )
    {
    
    }


// ---------------------------------------------------------------------------
// CSvtSettingsHandler::IsServiceConfiguredL
// ---------------------------------------------------------------------------
//
TBool CSvtSettingsHandler::IsServiceConfiguredL( TUint aServiceId ) const
    {
    CSPSettings* settings = CreateServiceProviderSettingsL();
    CleanupStack::PushL( settings );
    
    RIdArray ids;
    CleanupClosePushL( ids );
    TBool result = 
        ( KErrNone == settings->FindServiceIdsL( ids ) 
        && KErrNotFound != ids.Find( aServiceId ) );
    CleanupStack::PopAndDestroy( &ids );
    
    CleanupStack::PopAndDestroy( settings );
    return result;
    }


// ---------------------------------------------------------------------------
// CSvtSettingsHandler::CreateServiceProviderSettingsL()
// ---------------------------------------------------------------------------
//
CSPSettings* CSvtSettingsHandler::CreateServiceProviderSettingsL() const
    {
    return CSPSettings::NewL();
    }


// ---------------------------------------------------------------------------
// CSvtSettingsHandler::CreateRcseProfileRegistryLC()
// ---------------------------------------------------------------------------
//
CRCSEProfileRegistry* CSvtSettingsHandler::CreateRcseProfileRegistryLC() const
    {
    return CRCSEProfileRegistry::NewLC();
    }


// ---------------------------------------------------------------------------
// CSvtSettingsHandler::CreateRcseProfileRegistryLC()
// ---------------------------------------------------------------------------
//
CSIPManagedProfileRegistry* CSvtSettingsHandler::CreateSipProfileRegistryLC()
    {
    return CSIPManagedProfileRegistry::NewLC( *this );
    }


// ---------------------------------------------------------------------------
// CSvtSettingsHandler::RcseProfileForServiceL()
// ---------------------------------------------------------------------------
//
CRCSEProfileEntry* CSvtSettingsHandler::RcseProfileForServiceL( 
        TUint aServiceId ) const
    {
    CRCSEProfileEntry* rcseProfile = NULL;
    CRCSEProfileRegistry* registry = CreateRcseProfileRegistryLC();
    RPointerArray<CRCSEProfileEntry> profileEntries;
    CleanupStack::PushL( TCleanupItem( ResetAndDestroy, &profileEntries ) );
    
    registry->FindByServiceIdL( aServiceId, profileEntries  );
    if ( profileEntries.Count() && profileEntries[ 0 ] )
        {
        rcseProfile = profileEntries[ 0 ];
        profileEntries.Remove( 0 );
        }
    
    CleanupStack::PopAndDestroy( &profileEntries );
    CleanupStack::PopAndDestroy( registry );
    return rcseProfile;
    }


// ---------------------------------------------------------------------------
// CSvtSettingsHandler::SipProfileIdForServiceL()
// ---------------------------------------------------------------------------
//
TInt CSvtSettingsHandler::SipProfileIdForServiceL( TUint aServiceId ) const
    {
    TInt sipProfileId( KErrNotFound );
    
    CRCSEProfileEntry* rcseProfile = RcseProfileForServiceL( aServiceId );
    if ( NULL != rcseProfile )
        {
        const TInt KSipProfileIdIndex = 0;
        sipProfileId = rcseProfile->iIds[ KSipProfileIdIndex ].iProfileId;
        delete rcseProfile;        
        }
    
    return sipProfileId;
    }
