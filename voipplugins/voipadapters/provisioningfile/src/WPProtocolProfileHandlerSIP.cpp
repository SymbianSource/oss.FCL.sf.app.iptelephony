/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handling SIP profiles.
*
*/


// INCLUDE FILES
#include    "WPProtocolProfileHandlerSIP.h"
#include    "wpprovisioningfileconst.h"
#include    "wpprovisioningfiledebug.h"

#include    <sip.h>
#include    <sipprofileregistry.h>
#include    <sipprofile.h>

#include    <sipmanagedprofile.h>
#include    <sipmanagedprofileregistry.h>

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWPProtocolProfileHandlerSIP::CWPProtocolProfileHandlerSIP
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWPProtocolProfileHandlerSIP::CWPProtocolProfileHandlerSIP()
    {
    }

// -----------------------------------------------------------------------------
// CWPProtocolProfileHandlerSIP::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWPProtocolProfileHandlerSIP::ConstructL()
    {
    FLOG( "[ProvisioningFile] CWPProtocolProfileHandlerSIP::ConstructL" );
    
    iSIP = CSIP::NewL( KProvisioningFileUid, *this );
    
    iSIPProfileRegistry = CSIPProfileRegistry::NewL( *iSIP, *this );

    // Get SIP profile pointers
    iSIPProfileRegistry->ProfilesL( iSIPProfiles );
    }

// -----------------------------------------------------------------------------
// CWPProtocolProfileHandlerSIP::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWPProtocolProfileHandlerSIP* CWPProtocolProfileHandlerSIP::NewL()
    {
    CWPProtocolProfileHandlerSIP* self = 
        CWPProtocolProfileHandlerSIP::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CWPProtocolProfileHandlerSIP::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWPProtocolProfileHandlerSIP* CWPProtocolProfileHandlerSIP::NewLC()
    {
    CWPProtocolProfileHandlerSIP* self = 
        new (ELeave) CWPProtocolProfileHandlerSIP();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

    
// Destructor
CWPProtocolProfileHandlerSIP::~CWPProtocolProfileHandlerSIP()
    {
    FLOG( "[ProvisioningFile] CWPProtocolProfileHandlerSIP::~CWPProtocolProfileHandlerSIP" );
    
    delete iSIPProfileRegistry;
    iSIPProfileRegistry = NULL;

    delete iSIP;
    iSIP = NULL;
    
    // Close SIP Profile id array
    iSIPProfileIds.Reset();
    iSIPProfileIds.Close();

    // Close SIP Profile array
    for ( TInt i = iSIPProfiles.Count()-1; i >= 0; i-- )
        {
        delete iSIPProfiles[i];
        }
    iSIPProfiles.Close();
    }

// -----------------------------------------------------------------------------
// CWPProtocolProfileHandlerSIP::IncomingRequest
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CWPProtocolProfileHandlerSIP::IncomingRequest(
    TUint32 /*aIapId*/,
    CSIPServerTransaction* /*aTransaction*/ )
    {
    FLOG( "[ProvisioningFile] CWPProtocolProfileHandlerSIP::IncomingRequest 1" );
    }

// -----------------------------------------------------------------------------
// CWPProtocolProfileHandlerSIP::TimedOut
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CWPProtocolProfileHandlerSIP::TimedOut(
    CSIPServerTransaction& /*aTransaction*/ )
    {
    FLOG( "[ProvisioningFile] CWPProtocolProfileHandlerSIP::TimedOut" );
    }
    
// -----------------------------------------------------------------------------
// CWPProtocolProfileHandlerSIP::ProfileRegistryEventOccurred
// Handle profile events.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CWPProtocolProfileHandlerSIP::ProfileRegistryEventOccurred(
    TUint32 /*aProfileId*/,
    MSIPProfileRegistryObserver::TEvent /*aEvent*/ )
    {
    FLOG( "[ProvisioningFile] CWPProtocolProfileHandlerSIP::ProfileRegistryEventOccurred" );
    }

// -----------------------------------------------------------------------------
// CWPProtocolProfileHandlerSIP::ProfileRegistryErrorOccurred
// Handle SIP profile error events
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CWPProtocolProfileHandlerSIP::ProfileRegistryErrorOccurred(
    TUint32 /*aProfileId*/,
    TInt /*aError*/ )
    {
    FLOG( "[ProvisioningFile] CWPProtocolProfileHandlerSIP::ProfileRegistryErrorOccurred" );
    }

// -----------------------------------------------------------------------------
// CWPProtocolProfileHandlerSIP::SetProfilesIapL
// Set Protocol Profile use this IAP.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CWPProtocolProfileHandlerSIP::SetProfilesIapL(
    TInt aProfileId, 
    TUint32 aProfileIapId )
    {
    FLOG( "[ProvisioningFile] CWPProtocolProfileHandlerSIP::SetProfilesIapL" );
    
    // Set new IAP Id and Update SIP profile table
    CSIPProfile* updatedProfile = NULL;
    TInt index( 0 );
    
    // SIP engine for saving
    CSIPManagedProfileRegistry* sipEngine = 
        CSIPManagedProfileRegistry::NewLC( *this );
        
    // Take right SIP profile
    updatedProfile = FindSIPProfileByIdL( iSIPProfileIds[aProfileId], index );
    
    // Take managed SIP Profile
    CSIPManagedProfile* profile = 
        static_cast<CSIPManagedProfile*> ( updatedProfile );
    
    // Add new Iap Id to SIP Profile
    User::LeaveIfError( profile->SetParameter( KSIPAccessPointId, 
                                               aProfileIapId ) );
    
    // Save changes
    sipEngine->SaveL( *profile );
    
    // Update the SIP table
    iSIPProfiles.Remove( index );
    iSIPProfiles.Insert( updatedProfile, index );
                
    CleanupStack::PopAndDestroy( sipEngine );
    }

// -----------------------------------------------------------------------------
// CWPProtocolProfileHandlerSIP::FindSIPProfileByIdL
// Find SIP Profile Id from local SIP Profile pointer array
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPProfile* CWPProtocolProfileHandlerSIP::FindSIPProfileByIdL(
    TUint32 aProfileId,
    TInt& aIndex )
    {
    FLOG( "[ProvisioningFile] CWPProtocolProfileHandlerSIP::FindSIPProfileByIdL" );
    
    CSIPProfile* result = NULL;
    TUint32 sipId = 0;
    const TInt profilesCount = iSIPProfiles.Count();

    for ( TInt i = 0; i < profilesCount; i++ )
        {
        sipId = 0;
        iSIPProfiles[i]->GetParameter( KSIPProfileId, sipId );
        if ( sipId == aProfileId )
            {
            result = iSIPProfiles[i];
            aIndex = i;
            break;
            }
        }

    if ( !result )
        {
        User::Leave( KErrNotFound );
        }

    return result;
    }

// -----------------------------------------------------------------------------
// CWPProtocolProfileHandlerSIP::SetProvisioningIdL
// Set provisioning Protocol Profiles Id in array.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CWPProtocolProfileHandlerSIP::SetProvisioningIdL()    
    {
    FLOG( "[ProvisioningFile] CWPProtocolProfileHandlerSIP::SetProvisioningIdL" );
    
    TUint32 sipId( 0 );
    TBool idExist( EFalse );
    
    RArray<TUint32> oldIdArray;
    CleanupClosePushL( oldIdArray );
        
    // delete old SIP Profile array
    for ( TInt i = 0; i < iSIPProfiles.Count(); i++ )
        {
        iSIPProfiles[i]->GetParameter( KSIPProfileId, sipId );
        User::LeaveIfError( oldIdArray.Append( sipId ) );
        delete iSIPProfiles[i];
        iSIPProfiles[i] = NULL;
        }
    iSIPProfiles.Close();
            
    // Get SIP profile pointers
    iSIPProfileRegistry->ProfilesL( iSIPProfiles );
    
    if ( oldIdArray.Count() < iSIPProfiles.Count() )
        {
        for ( TInt j = 0; j < iSIPProfiles.Count(); j++ )
            {
            idExist = EFalse;
            TUint32 newSipId( 0 );
            User::LeaveIfError( iSIPProfiles[j]->GetParameter( KSIPProfileId, newSipId ) );
            
            for ( TInt k = 0; k < oldIdArray.Count(); k++ )
                {
                if ( oldIdArray[k] == newSipId )
                    {
                    idExist = ETrue;
                    k = oldIdArray.Count();
                    }
                }
                
            if ( !idExist )
                {
                // Save Id to Profile Id array
                User::LeaveIfError( iSIPProfileIds.Append( newSipId ) );
                }
            }
        }
        
    CleanupStack::PopAndDestroy( &oldIdArray );        
    FLOG( "[ProvisioningFile] *** CWPProtocolProfileHandlerSIP::SetProvisioningIdL out" );
    }
    
// -----------------------------------------------------------------------------
// CWPProtocolProfileHandlerSIP::ProvisionedCount
// Get provisioned Protocol Profiles count.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TInt CWPProtocolProfileHandlerSIP::ProvisionedCount()    
    {
    FLOG( "[ProvisioningFile] CWPProtocolProfileHandlerSIP::GetProvisionedCount" );
    
    return iSIPProfileIds.Count();
    }    
    
//  End of File  
