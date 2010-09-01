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
* Description:  Settings handler class for svtmatching.
*
*/


#include <spsettings.h>
#include <spdefinitions.h>
#include <spproperty.h>
#include <crcseprofileregistry.h>

#include "svtsettingshandler.h"
#include "svtcleanup.h"


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CSvtSettingsHandler::CSvtSettingsHandler()
    {
    }

  
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CSvtSettingsHandler* CSvtSettingsHandler::NewL()
    {
    CSvtSettingsHandler* self = new ( ELeave ) CSvtSettingsHandler;
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CSvtSettingsHandler::~CSvtSettingsHandler()
    {

    }

// ---------------------------------------------------------------------------
// Initializes settings
// ---------------------------------------------------------------------------
//
void CSvtSettingsHandler::InitializeSettingsL( TUint aServiceId )
    {
    CRCSEProfileRegistry* registry = CreateRcseRegistryLC();
    
    RPointerArray<CRCSEProfileEntry> entries;
    // Cleanup for pointer array
    CleanupResetAndDestroy< 
        RPointerArray< CRCSEProfileEntry > >::PushL( entries );
    
    // Find voip settings by service id
    registry->FindByServiceIdL( aServiceId, entries );
    if ( 0 == entries.Count() )
        {
        User::Leave( KErrNotFound );
        }
    
    // Ignore domain part setting from rcse
    iIgnoreDomainPart = entries[0]->iIgnoreAddrDomainPart;
    // Meaningful digits for matching
    iMeaningfulDigits = entries[0]->iMeanCountOfVoIPDigits;
    // Service id
    iServiceId = aServiceId;
    
    CleanupStack::PopAndDestroy( &entries );
    CleanupStack::PopAndDestroy( registry );
    }

// ---------------------------------------------------------------------------
// Returns value of meaningful digits
// ---------------------------------------------------------------------------
//
TInt CSvtSettingsHandler::MeaningfulDigits() const
    {
    return iMeaningfulDigits;
    }

// ---------------------------------------------------------------------------
// Returns value of ignore domain part setting
// ---------------------------------------------------------------------------
//
TUint CSvtSettingsHandler::IgnoreDomainPartValue() const
    {
    return iIgnoreDomainPart;
    }

// ---------------------------------------------------------------------------
// Gets contact store uris
// ---------------------------------------------------------------------------
//
TInt CSvtSettingsHandler::GetContactStoreUris( CDesCArray& aStoreUris ) const
    {
    TRAPD( ret, GetContactStoreUrisL( aStoreUris ) );
    return ret;
    }

// ---------------------------------------------------------------------------
// Gets contact store uris
// ---------------------------------------------------------------------------
//
void CSvtSettingsHandler::GetContactStoreUrisL( CDesCArray& aStoreUris ) const
    {
    // Get contact store id from Service Table
    //KSPMaxDesLength from spdefinitions.h
    HBufC* cntStoreId = HBufC::NewLC( KSPMaxDesLength );
    CSPSettings* spSettings = CreateSpSettingsLC(); 
    
    CSPProperty* property = CSPProperty::NewLC();
    User::LeaveIfError( spSettings->FindPropertyL( 
            TServiceId( iServiceId ), EPropertyContactStoreId, *property ) ); 
                           
               
    TPtr storeUri( cntStoreId->Des() );
    User::LeaveIfError( property->GetValue( storeUri ) );   
    
    aStoreUris.AppendL( storeUri );
    
    CleanupStack::PopAndDestroy( property ); 
    CleanupStack::PopAndDestroy( spSettings ); 
    CleanupStack::PopAndDestroy( cntStoreId ); 
    }

// ---------------------------------------------------------------------------
// Returns new instance of rcse registry
// ---------------------------------------------------------------------------
//
CRCSEProfileRegistry* CSvtSettingsHandler::CreateRcseRegistryLC() const
    {
    return CRCSEProfileRegistry::NewLC();
    }

// ---------------------------------------------------------------------------
// Returns new instance of SPSettings
// ---------------------------------------------------------------------------
//
CSPSettings* CSvtSettingsHandler::CreateSpSettingsLC() const
    {
    return CSPSettings::NewLC();
    }
