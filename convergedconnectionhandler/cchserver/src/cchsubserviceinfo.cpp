/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CCCHSubserviceInfo implementation
*
*/


// INCLUDE FILES
#include "cchsubserviceinfo.h"
#include "cchserverbase.h"
#include "cchrequeststorage.h"
#include "cchpluginhandler.h"
#include "cchlogger.h"

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES
// None

// CONSTANTS
// None

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ============================= LOCAL FUNCTIONS =============================

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// CCCHSubserviceInfo::CCCHSubserviceInfo
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CCCHSubserviceInfo::CCCHSubserviceInfo( 
    TUint aServiceId, 
    CCCHServerBase& aServer ):
    iServiceId( aServiceId ),
    iServer( aServer ),    
    iEnableAtStartUp( EFalse )
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// CCCHSubserviceInfo::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CCCHSubserviceInfo* CCCHSubserviceInfo::NewL( 
    TUint aServiceId, 
    CCCHServerBase& aServer )
    {
    CCCHSubserviceInfo* self = CCCHSubserviceInfo::NewLC( aServiceId, aServer );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CCCHSubserviceInfo::NewLC
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CCCHSubserviceInfo* CCCHSubserviceInfo::NewLC( 
    TUint aServiceId, 
    CCCHServerBase& aServer )
    {
    CCCHSubserviceInfo* self = 
        new (ELeave) CCCHSubserviceInfo( aServiceId, aServer );
    CleanupStack::PushL( self );
    return self;
    }

// Destructor
CCCHSubserviceInfo::~CCCHSubserviceInfo()
    {
    }


// ---------------------------------------------------------------------------
// CCCHSubserviceInfo::SetSubserviceId
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHSubserviceInfo::SetSubserviceId( const TUint aSubserviceId )
    {
    iSubserviceId = aSubserviceId;
    }

// ---------------------------------------------------------------------------
// CCCHSubserviceInfo::SetPluginUid
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHSubserviceInfo::SetPluginUid( const TUid aPluginUid )
    {
    CCHLOGSTRING2( "CCCHSubserviceInfo::SetPluginUid aPluginUid = 0x%X", 
        aPluginUid );
    iPluginUid = aPluginUid;    
    }

// ---------------------------------------------------------------------------
// CCCHSubserviceInfo::SetPluginUid
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TUid CCCHSubserviceInfo::GetPluginUid() const
    {
    return iPluginUid;
    }

// ---------------------------------------------------------------------------
// CCCHSubserviceInfo::SetType
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHSubserviceInfo::SetType( const TCCHSubserviceType aType )
    {
    iType = aType;
    }

// ---------------------------------------------------------------------------
// CCCHSubserviceInfo::GetType
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TCCHSubserviceType CCCHSubserviceInfo::Type()
    {
    GetServiceNetworkInfo();
    return iType;
    }
    
// ---------------------------------------------------------------------------
// CCCHSubserviceInfo::SetState
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHSubserviceInfo::SetState( const TCCHSubserviceState aState )
    {
    iState = aState;
    }

// ---------------------------------------------------------------------------
// CCCHSubserviceInfo::SetError
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHSubserviceInfo::SetError( const TInt aError )
    {
    iError = aError;
    }

// ---------------------------------------------------------------------------
// CCCHSubserviceInfo::SetIapId
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHSubserviceInfo::SetIapId( const TUint32 aIapId )
    {
    iIapId = aIapId;
    }

// ---------------------------------------------------------------------------
// CCCHSubserviceInfo::SetSNAPId
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHSubserviceInfo::SetSNAPId( const TUint32 aSNAPid )
    {
    iSNAPId = aSNAPid;
    }

// ---------------------------------------------------------------------------
// CCCHSubserviceInfo::Update
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHSubserviceInfo::Update()
    {
    CCHLOGSTRING( "CCCHSubserviceInfo::Update: IN" );

    // Read state from plugin
    GetServiceNetworkInfo();
    
    StatusChanged();
        
    CCHLOGSTRING( "CCCHSubserviceInfo::Update: OUT" );
    }

// ---------------------------------------------------------------------------
// CCCHSubserviceInfo::IsEnabled
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CCCHSubserviceInfo::IsEnabled() const
    {
    return ( iState != ECCHUninitialized && iState != ECCHDisabled );
    }

// ---------------------------------------------------------------------------
// CCCHSubserviceInfo::StatusChangedL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//    
void CCCHSubserviceInfo::StatusChanged()
    {
    CCHLOGSTRING( "CCCHSubserviceInfo::StatusChangedL: IN" );
    CCHLOGSTRING2( "CCCHSubserviceInfo::StatusChangedL: iServiceId %d", 
        iServiceId );
    CCHLOGSTRING2( "CCCHSubserviceInfo::StatusChangedL: iType %d", 
        iType );
    CCHLOGSTRING2( "CCCHSubserviceInfo::StatusChangedL: iState %d", 
        iState );
    CCHLOGSTRING2( "CCCHSubserviceInfo::StatusChangedL: iError %d", 
        iError );
        
    TServiceStatus serviceStatus;
    serviceStatus.iConnectionInfo.iServiceSelection.iServiceId = iServiceId;
    serviceStatus.iConnectionInfo.iServiceSelection.iType      = iType;
    serviceStatus.iConnectionInfo.iIapId                       = iIapId;
    serviceStatus.iConnectionInfo.iSNAPId                      = iSNAPId; 
    serviceStatus.iConnectionInfo.iSNAPLocked                  = iSNAPLocked; 
    serviceStatus.iState                                       = iState;
    serviceStatus.iError                                       = iError;
    
    // Send notify to clients
    iServer.RequestStorage().NotifyServiceStatesChange( serviceStatus );
    CCHLOGSTRING( "CCCHSubserviceInfo::StatusChangedL: OUT" );
    }


// ---------------------------------------------------------------------------
// CCCHSubserviceInfo::GetServiceNetworkInfo
// ---------------------------------------------------------------------------
//   
void CCCHSubserviceInfo::GetServiceNetworkInfo( )
    {
    iServer.PluginHandler().GetServiceNetworkInfo( iServiceId, iPluginUid,
            iType, iSNAPId, iIapId, iSNAPLocked, iPasswordSet );
    }


// ---------------------------------------------------------------------------
// CCCHSubserviceInfo::FillSubserviceInfo
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//    
void CCCHSubserviceInfo::FillSubserviceInfo( TCCHSubservice& aSubservice )
    {
    CCHLOGSTRING( "CCCHSubserviceInfo::FillSubserviceInfo: IN" );
    GetServiceNetworkInfo();

    aSubservice.iConnectionInfo.iServiceSelection.iServiceId    = iServiceId;
    aSubservice.iConnectionInfo.iServiceSelection.iType         = iType;
    aSubservice.iConnectionInfo.iIapId                          = iIapId;
    aSubservice.iConnectionInfo.iSNAPId                         = iSNAPId;
    aSubservice.iConnectionInfo.iSNAPLocked                     = iSNAPLocked;
    aSubservice.iSubserviceId                                   = iSubserviceId;
    aSubservice.iState                                          = iState;
    aSubservice.iError                                          = iError;
    CCHLOGSTRING( "CCCHSubserviceInfo::FillSubserviceInfo: OUT" );
    }

// ---------------------------------------------------------------------------
// CCCHSubserviceInfo::GetState
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//    
TCCHSubserviceState CCCHSubserviceInfo::GetState()
    {
    CCHLOGSTRING( "CCCHSubserviceInfo::GetState: IN" );
    iServer.PluginHandler().GetServiceState( iServiceId, iPluginUid, 
            iType, iState, iError );
    return iState;
    }

// ---------------------------------------------------------------------------
// CCCHSubserviceInfo::GetError
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//    
TInt CCCHSubserviceInfo::GetError()
    {
    CCHLOGSTRING( "CCCHSubserviceInfo::GetError: IN" );
    iServer.PluginHandler().GetServiceState( iServiceId, iPluginUid, 
            iType, iState, iError );
    return iError;
    }
    
// ---------------------------------------------------------------------------
// CCCHSubserviceInfo::SetStartupFlag
// (other items were commented in a header).
// ---------------------------------------------------------------------------
// 
void CCCHSubserviceInfo::SetStartupFlag( TBool aLoadAtStartUp )
    {
    iEnableAtStartUp = aLoadAtStartUp;
    }
    
// ---------------------------------------------------------------------------
// CCCHSubserviceInfo::GetStartUpFlag
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//     
TBool CCCHSubserviceInfo::GetStartUpFlag() const
    {
    return iEnableAtStartUp;
    }
    
// ========================== OTHER EXPORTED FUNCTIONS =======================

//  End of File
