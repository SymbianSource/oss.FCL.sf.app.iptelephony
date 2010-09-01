/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implements CSCSettingsUiDialog methods
*
*/


#include <avkon.rsg>
#include <stringloader.h>
#include <aknglobalnote.h>
#include <cscsettingsui.rsg>
#include <aknglobalmsgquery.h>

#include "cscsettingsuilogger.h"
#include "cscsettingsuidialog.h"
#include "mcscsettingsuidialogobserver.h"

const TInt KMsgQueryTextMaxLength = 512;
_LIT( KNextLine, "\n" );
_LIT( KInvalidConnectionAddress, "::1" );

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCSettingsUiDialog::CCSCSettingsUiDialog(
    MCSCSettingsUiDialogObserver& aObserver )
    : CActive ( EPriorityHigh ),
    iObserver( aObserver )
    {
    }
    
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CCSCSettingsUiDialog::ConstructL()
    {
    CSCSETUIDEBUG( "CCSCSettingsUiDialog::ConstructL - begin" );
    
    CActiveScheduler::Add( this );
    
    CSCSETUIDEBUG( "CCSCSettingsUiDialog::ConstructL - end" );
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCSettingsUiDialog* CCSCSettingsUiDialog::NewL(
    MCSCSettingsUiDialogObserver& aObserver )
    {
    CCSCSettingsUiDialog* self = CCSCSettingsUiDialog::NewLC( aObserver );
    CleanupStack::Pop( self );    
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCSettingsUiDialog* CCSCSettingsUiDialog::NewLC(
    MCSCSettingsUiDialogObserver& aObserver )
    {
    CCSCSettingsUiDialog* self = 
        new ( ELeave ) CCSCSettingsUiDialog( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCSettingsUiDialog::~CCSCSettingsUiDialog()
    {
    Cancel();
    delete iMsgQuery;
    }

// -----------------------------------------------------------------------------
// CCSCSettingsUiDialog::LaunchMessageQueryL
// Launches a service information message query.
// -----------------------------------------------------------------------------
//
void CCSCSettingsUiDialog::LaunchMessageQueryL(
    const TDesC& aUsername,
    const TDesC& aDomain,
    const TDesC& aAddress,
    const TDesC& aSsid,
    const TDesC& aMac,
    TBool aDisabled )
    {
    CSCSETUIDEBUG( "CCSCSettingsUiDialog::LaunchMessageQueryL" );
    
    if ( !IsActive() )
        {
        // Create data to be shown in service information message query.
        HBufC* queryText = HBufC::NewLC( KMsgQueryTextMaxLength );
        CreateTextForMsgQueryL(
            queryText,
            aUsername,
            aDomain,
            aAddress,
            aSsid,
            aMac,
            aDisabled );
        
        // Create header text to be shown in information message query.
        HBufC* headerText = StringLoader::LoadLC(
            R_CSCSETTINGSUI_SERVICE_INFO_TEXT );
        
        if ( iMsgQuery )
            {
            delete iMsgQuery;
            iMsgQuery = NULL;
            }
        
        // Show message query with appropriate header and data.
        iMsgQuery = CAknGlobalMsgQuery::NewL();
        iMsgQuery->ShowMsgQueryL( 
            iStatus, 
            *queryText, 
            R_AVKON_SOFTKEYS_OK_EMPTY, 
            *headerText, 
            KNullDesC );  
    
        SetActive();
        CleanupStack::PopAndDestroy( headerText );
        CleanupStack::PopAndDestroy( queryText );
        }
    }


// -----------------------------------------------------------------------------
// CCSCSettingsUiDialog::DestroyMessageQuery
// Destroys a service information message query.
// -----------------------------------------------------------------------------
//
void CCSCSettingsUiDialog::DestroyMessageQuery()
    {  
    Cancel();
    }


// ---------------------------------------------------------------------------
// From CActive
// CCSCSettingsUiDialog::RunL
// ---------------------------------------------------------------------------
//
void CCSCSettingsUiDialog::RunL()
    {
    }
    
    
// ---------------------------------------------------------------------------
// From CActive
// CCSCSettingsUiDialog::DoCancel
// ---------------------------------------------------------------------------
//
void CCSCSettingsUiDialog::DoCancel()
    {
    iMsgQuery->CancelMsgQuery();
    }


// -----------------------------------------------------------------------------
// CCSCSettingsUiDialog::CreateTextForMsgQueryL
// Creates text for service information message query.
// -----------------------------------------------------------------------------
//
void CCSCSettingsUiDialog::CreateTextForMsgQueryL( 
    HBufC* aText,
    const TDesC& aUsername,
    const TDesC& aDomain,
    const TDesC& aAddress,
    const TDesC& aSsid,
    const TDesC& aMac,
    TBool aDisabled ) const
    {  
    CSCSETUIDEBUG( "CCSCSettingsUiDialog::CreateTextForMsgQueryL - begin" );
    
    if ( aText == NULL )
        {
        User::Leave( KErrBadHandle );
        }
    
    // Load resources to memory.
    HBufC* unavailable = StringLoader::LoadLC(
        R_CSCSETTINGSUI_SERV_INFO_VALUE_UNAVAILABLE );
    HBufC* unavailbleWhenOffile = StringLoader::LoadLC(
        R_CSCSETTINGSUI_SERV_INFO_VALUE_UNAVAILABLE_WHEN_OFFLINE );
    
    // Add username information to query text.
    HBufC* resource = StringLoader::LoadLC( 
        R_CSCSETTINGSUI_SERVICE_INFO_USERNAME_TEXT );
    CreateItemToMsgQuery( aText, resource, unavailbleWhenOffile, aUsername );
    CleanupStack::PopAndDestroy( resource );
    
     // Add domain information to query text.    
    resource = StringLoader::LoadLC( 
        R_CSCSETTINGSUI_SERVICE_INFO_DOMAIN_TEXT ); 
    
    if ( aDisabled )
        {
        CreateItemToMsgQuery( aText, resource, unavailable, KNullDesC );
        }
    else
        {
        CreateItemToMsgQuery( aText, resource, unavailbleWhenOffile, aDomain );
        }
        
    CleanupStack::PopAndDestroy( resource );
        
    // Add connection address information to query text.
    resource = StringLoader::LoadLC( 
        R_CSCSETTINGSUI_SERVICE_INFO_ADDRESS_TEXT );    
    if ( !aAddress.Compare( KInvalidConnectionAddress ) && !aDisabled )
        {
        CreateItemToMsgQuery( 
            aText, resource, unavailable, KNullDesC );
        }
    else if ( !aDisabled )
        {
        CreateItemToMsgQuery( 
            aText, resource, unavailable, aAddress );
        }    
    else
        {
        CreateItemToMsgQuery( 
            aText, resource, unavailable, KNullDesC );
        }
    
    CleanupStack::PopAndDestroy( resource );
        
    // Add WLAN SSID information to query text.
    resource = StringLoader::LoadLC( 
        R_CSCSETTINGSUI_SERVICE_INFO_WLAN_SSID_TEXT );
    
    if ( aDisabled )
        {
        CreateItemToMsgQuery( aText, resource, unavailable, KNullDesC );
        }
    else
        {
        CreateItemToMsgQuery( aText, resource, unavailable, aSsid );
        }
    
    CleanupStack::PopAndDestroy( resource );
        
    // Add WLAN MAC address information to query text.
    resource = StringLoader::LoadLC( 
        R_CSCSETTINGSUI_SERVICE_INFO_WLAN_MAC_TEXT );
    
    CreateItemToMsgQuery( aText, resource, unavailable, aMac );
        
    CleanupStack::PopAndDestroy( resource );
    
    // PopAndDestroy resources.
    CleanupStack::PopAndDestroy( unavailbleWhenOffile );
    CleanupStack::PopAndDestroy( unavailable );
        
    CSCSETUIDEBUG( "CCSCSettingsUiDialog::CreateTextForMsgQueryL - end" );
    }
    
// -----------------------------------------------------------------------------
// CCSCSettingsUiDialog::CreateItemToMsgQuery
// Creates an item for service information message query.
// -----------------------------------------------------------------------------
//
void CCSCSettingsUiDialog::CreateItemToMsgQuery( 
    HBufC* aText,
    HBufC* aItem,
    HBufC* aAdditionalValue,
    const TDesC& aData ) const
    {  
    if ( aText && aItem )
        {
        aText->Des().Append( *aItem );
        aText->Des().Append( KNextLine );        
        if ( aData.Length() )
            {
            aText->Des().Append( aData );
            }
        else if ( aAdditionalValue )
            {
            aText->Des().Append( *aAdditionalValue );
            }
        else
            {
            // nothing to do
            }
                
        aText->Des().Append( KNextLine );        
        }
    }
