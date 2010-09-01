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
* Description:  For handling interactions betweed UI and CMM.
*
*/

#include <cmpluginwlandef.h>
#include <cmdestinationext.h>
#include <cmconnectionmethodext.h>

#include "cscenglogger.h"
#include "cscengdestinationshandler.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCEngDestinationsHandler::CCSCEngDestinationsHandler()
    {    
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CCSCEngDestinationsHandler::ConstructL()
    {
    CSCENGDEBUG( "CCSCEngDestinationsHandler::ConstructL - begin" ); 
    
    // Create connection to Connection Method Manager.
    iCmManager.OpenL();
    
    CSCENGDEBUG( "CCSCEngDestinationsHandler::ConstructL - end" ); 
    }   
    
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CCSCEngDestinationsHandler* CCSCEngDestinationsHandler::NewL()
    {    
    CCSCEngDestinationsHandler* self = 
        new ( ELeave ) CCSCEngDestinationsHandler();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCEngDestinationsHandler::~CCSCEngDestinationsHandler()
    {
    CSCENGDEBUG( 
        "CCSCEngDestinationsHandler::~CCSCEngDestinationsHandler - begin" ); 
    
    iCmManager.Close();
    
    CSCENGDEBUG( 
        "CCSCEngDestinationsHandler::~CCSCEngDestinationsHandler - end" ); 
    }
    

// ---------------------------------------------------------------------------
// CCSCEngDestinationsHandler::AvailableSnapIdsL
// Returns available destination ids in the device.
// ---------------------------------------------------------------------------
//
EXPORT_C void CCSCEngDestinationsHandler::AvailableSnapIdsL( 
    RArray<TUint32>& aSnapIds )
    {
    iCmManager.AllDestinationsL( aSnapIds );
    }


// ---------------------------------------------------------------------------
// CCSCEngDestinationsHandler::SnapNameL
// Returns destination name based on destination id.
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC* CCSCEngDestinationsHandler::SnapNameL( TUint32 aSnapId )
    {
    // Get destination name based on destination id from CM Manager. 
    // Function leaves if destination wasn't found.
    HBufC* snapName = NULL;
    RArray<TUint32> snapIds;
    CleanupClosePushL( snapIds );
    AvailableSnapIdsL( snapIds );
    for ( TInt i = 0; i < snapIds.Count() && !snapName; i++ )
        {
        if ( aSnapId == snapIds[ i ] )
            {
            snapName = iCmManager.DestinationL( aSnapId ).NameLC();
            CSCENGDEBUG2( 
                "CCSCEngDestinationsHandler::SnapNameLC: snapName=%S", 
                    snapName );
            CleanupStack::Pop( snapName );
            }
        }
    CleanupStack::PopAndDestroy( &snapIds );
    
    if ( !snapName )
        {
        User::Leave( KErrNotFound );
        }
    
    return snapName;
    }

// ---------------------------------------------------------------------------
// CCSCEngDestinationsHandler::IsSnapInUseL
// Returns ETrue if snap is in use.
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CCSCEngDestinationsHandler::IsSnapInUseL( TUint32 aSnapId )
    {
    return iCmManager.DestinationL( aSnapId ).IsConnectedL();
    }
