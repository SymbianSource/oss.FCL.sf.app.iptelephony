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


#include <e32debug.h>
#include <centralrepository.h>
#include <unsafprotocolsinternalcrkeys.h>

#include "cscsvcpluginlogger.h"
#include "cscsvcplugincenrephandler.h"


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCSvcPluginCenrepHandler::CCSCSvcPluginCenrepHandler()
    {
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CCSCSvcPluginCenrepHandler::ConstructL()
    {    
    CSCSVCPLUGINDEBUG("CCSCSvcPluginCenrepHandler::ConstructL - begin");
    
    iNatfwRepository = CRepository::NewL( KCRUidUNSAFProtocols );
    
    CSCSVCPLUGINDEBUG("CCSCSvcPluginCenrepHandler::ConstructL - end");
    }

    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCSvcPluginCenrepHandler* CCSCSvcPluginCenrepHandler::NewL()
    {
    CCSCSvcPluginCenrepHandler* self = 
        new ( ELeave ) CCSCSvcPluginCenrepHandler();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCSvcPluginCenrepHandler::~CCSCSvcPluginCenrepHandler()
    {
    CSCSVCPLUGINDEBUG(
        "CCSCSvcPluginCenrepHandler::~CCSCSvcPluginCenrepHandler - begin");
    
    delete iNatfwRepository;
    
    CSCSVCPLUGINDEBUG(
        "CCSCSvcPluginCenrepHandler::~CCSCSvcPluginCenrepHandler - end");
    }


// ---------------------------------------------------------------------------
// Destroys IAP spesific NAT/FW settings based on IAP id.
// ---------------------------------------------------------------------------
//
void CCSCSvcPluginCenrepHandler::RemoveIapSpecificNatfwSettingsL(
    TInt aIapId )
    {
    CSCSVCPLUGINDEBUG(
       "CCSCSvcPluginCenrepHandler::RemoveIapSpecificNatfwSettingsL - begin");
    
    // Get IAP specific keys from Central Repository.
    RArray<TUint32> keys;
    CleanupClosePushL( keys );
    User::LeaveIfError(
        iNatfwRepository->FindEqL( 
            KUNSAFProtocolsIAPIdMask, 
            KUNSAFProtocolsFieldTypeMask, 
            aIapId, 
            keys ) );
    
    for ( TInt i( 0 ) ; i < keys.Count() ; i++ )
        {
        TUint32 errorKey( 0 );
        iNatfwRepository->Delete( keys[ i ], KUNSAFProtocolsIAPIdMask, errorKey );
        }
     
    CleanupStack::PopAndDestroy( &keys );  
    
    CSCSVCPLUGINDEBUG(
       "CCSCSvcPluginCenrepHandler::RemoveIapSpecificNatfwSettingsL - end");
    }


// ---------------------------------------------------------------------------
// Destroys Domain spesific NAT/FW settings based on domain name.
// ---------------------------------------------------------------------------
//
void CCSCSvcPluginCenrepHandler::RemoveDomainSpecificNatfwSettingsL(
    const TDesC8& aDomainName )
    {  
    CSCSVCPLUGINDEBUG(
    "CCSCSvcPluginCenrepHandler::RemoveDomainSpecificNatfwSettingsL - begin");
    
    // Get domain specific keys from Central Repository.
    RArray<TUint32> keys;
    CleanupClosePushL( keys );
    User::LeaveIfError( 
        iNatfwRepository->FindEqL( 
            KUNSAFProtocolsDomainMask, 
            KUNSAFProtocolsFieldTypeMask, 
            aDomainName, 
            keys ) );
    
    for ( TInt i( 0 ) ; i < keys.Count() ; i++ )
        {
        TUint32 errorKey( 0 );
        iNatfwRepository->Delete( keys[ i ], KUNSAFProtocolsTableMask, errorKey );
        }
         
    CleanupStack::PopAndDestroy( &keys );
    
    CSCSVCPLUGINDEBUG(
      "CCSCSvcPluginCenrepHandler::RemoveDomainSpecificNatfwSettingsL - end");
    }

