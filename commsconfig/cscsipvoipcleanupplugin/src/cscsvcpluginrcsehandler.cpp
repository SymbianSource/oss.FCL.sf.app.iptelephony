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
* Description:  
*
*/


#include <e32base.h>
#include <crcseprofileregistry.h>

#include "cscsvcpluginlogger.h"
#include "cscsvcpluginrcsehandler.h"


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCSvcPluginRcseHandler::CCSCSvcPluginRcseHandler()
    {
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CCSCSvcPluginRcseHandler::ConstructL()
    {    
    CSCSVCPLUGINDEBUG("CCSCSvcPluginRcseHandler::ConstructL - begin");
    
    iRcseProfileRegistry = CRCSEProfileRegistry::NewL();
    
    CSCSVCPLUGINDEBUG("CCSCSvcPluginRcseHandler::ConstructL - end");
    }

    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCSvcPluginRcseHandler* CCSCSvcPluginRcseHandler::NewL()
    {
    CCSCSvcPluginRcseHandler* self = 
        new ( ELeave ) CCSCSvcPluginRcseHandler();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCSvcPluginRcseHandler::~CCSCSvcPluginRcseHandler()
    {
    CSCSVCPLUGINDEBUG(
        "CCSCSvcPluginRcseHandler::~CCSCSvcPluginRcseHandler - begin");
    
    delete iRcseProfileRegistry; 
    
    CSCSVCPLUGINDEBUG(
        "CCSCSvcPluginRcseHandler::~CCSCSvcPluginRcseHandler - end");
    }


// ---------------------------------------------------------------------------
// Returns VoIP profile id for corresponding EasyVoIP plugin.
// Also returns SIP profile reference ids if any 
// was found from VoIP profile entry.
// ---------------------------------------------------------------------------
//
void CCSCSvcPluginRcseHandler::GetProfileIdsL( 
    TUint32 aServiceId, 
    TUint32& aProfileId,
    RArray<TUint32>& aSipProfileIds )
    {
    CSCSVCPLUGINDEBUG( "CCSCSvcPluginRcseHandler::GetProfileIdsL - begin");
    
    RPointerArray<CRCSEProfileEntry> entries;
    TCleanupItem clItem( ResetAndDestroy, &entries );
    CleanupStack::PushL( clItem );

    // Get VoIP setting id
    iRcseProfileRegistry->FindByServiceIdL( aServiceId, entries );
                            
    if ( entries.Count() )
        {
        aProfileId = entries[ 0 ]->iId;
                            
	    // Check profile for SIP references and append ids to array.
	    for ( TInt j = 0; j < entries[ 0 ]->iIds.Count(); j++ )
	        {
	        TUint32 id = entries[ 0 ]->iIds[ j ].iProfileId;
	        User::LeaveIfError( aSipProfileIds.Append( id ) );
	        }
        }
    else
        {
        User::Leave( KErrNotFound );
        }
        
    CleanupStack::PopAndDestroy(); // clItem    
    
    CSCSVCPLUGINDEBUG( "CCSCSvcPluginRcseHandler::GetProfileIdsL - end");
    }


// ---------------------------------------------------------------------------
// Destroys VoIP profile from RCSE based on VoIP profile id.
// ---------------------------------------------------------------------------
//
void CCSCSvcPluginRcseHandler::RemoveProfileL(
    TUint32 aProfileId )
    {    
    CSCSVCPLUGINDEBUG( "CCSCSvcPluginRcseHandler::RemoveProfileL - begin");
    
    // Destroy VoIP profile from RCSE.
    CRCSEProfileEntry* entry = CRCSEProfileEntry::NewLC();
    iRcseProfileRegistry->FindL( aProfileId, *entry );
    iRcseProfileRegistry->DeleteL( aProfileId );
    CleanupStack::PopAndDestroy( entry );
    
    CSCSVCPLUGINDEBUG( "CCSCSvcPluginRcseHandler::RemoveProfileL - end");
    }


// ---------------------------------------------------------------------------
// For deleting RPointerArray in case of leave
// ---------------------------------------------------------------------------
//
void CCSCSvcPluginRcseHandler::ResetAndDestroy( TAny* aPointerArray )
    {
    CSCSVCPLUGINDEBUG( "CCSCSvcPluginRcseHandler::ResetAndDestroy - begin");
    
    if ( aPointerArray )
        {
        RPointerArray<CRCSEProfileEntry>* array =
            static_cast<RPointerArray<CRCSEProfileEntry>*>( aPointerArray );
        array->ResetAndDestroy();
        array->Close();
        }
    
    CSCSVCPLUGINDEBUG( "CCSCSvcPluginRcseHandler::ResetAndDestroy - end");
    }

