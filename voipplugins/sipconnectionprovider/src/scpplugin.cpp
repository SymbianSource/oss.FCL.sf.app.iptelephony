/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#include <ecom.h>
#include <implementationproxy.h>
#include <e32debug.h>

#include "scpplugin.h"
#include "scpservicemanager.h"
#include "scplogger.h"
#include "sipconnectionprovideruids.hrh"

// -----------------------------------------------------------------------------
// CScpPlugin::ConstructL
// -----------------------------------------------------------------------------
//
void CScpPlugin::ConstructL()
    {
    SCPLOGSTRING( "CScpPlugin::ConstructL" );

    iServiceManager = CScpServiceManager::NewL( iServiceObserver );
    }


// -----------------------------------------------------------------------------
// CScpPlugin::CScpPlugin()
// -----------------------------------------------------------------------------
//
CScpPlugin::CScpPlugin( MCchServiceObserver& serviceObserver ) :
    iServiceObserver( serviceObserver )
    {
	}

	
// -----------------------------------------------------------------------------
// CScpPlugin::NewL
// -----------------------------------------------------------------------------
//
CScpPlugin* CScpPlugin::NewL( TAny* aParam )
	{
    SCPLOGSTRING2( "CScpPlugin::NewL aParam: 0x%x", aParam );
    __ASSERT_DEBUG( aParam, User::Panic( KNullDesC, KErrGeneral ) );

    MCchServiceObserver& serviceObserver = *((MCchServiceObserver*)(aParam)); 

    CScpPlugin* self = new ( ELeave ) CScpPlugin( serviceObserver );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
	}

// -----------------------------------------------------------------------------
// CScpPlugin::~CScpPlugin
// -----------------------------------------------------------------------------
//
CScpPlugin::~CScpPlugin()
	{
	delete iServiceManager;
	}
  
// -----------------------------------------------------------------------------
// CScpPlugin::EnableServiceL
// -----------------------------------------------------------------------------
//
void CScpPlugin::EnableServiceL( const TServiceSelection& aServiceSelection )
    {
    SCPLOGSTRING3( "CScpPlugin::EnableServiceL service id: %d type: %d",
                   aServiceSelection.iServiceId, aServiceSelection.iType );

    iServiceManager->EnableServiceL( aServiceSelection.iServiceId, 
                                     aServiceSelection.iType );
    }

// -----------------------------------------------------------------------------
// CScpPlugin::EnableServiceL
// -----------------------------------------------------------------------------
//
void CScpPlugin::EnableServiceL( const TServiceSelection& aServiceSelection,
                                 TUint aIapId )
    {
    SCPLOGSTRING4( "CScpPlugin::EnableServiceL service id: %d type: %d iap id: %d",
                   aServiceSelection.iServiceId, aServiceSelection.iType, aIapId );

    iServiceManager->EnableServiceL( aServiceSelection.iServiceId, 
                                     aServiceSelection.iType, 
                                     aIapId );
    }

// -----------------------------------------------------------------------------
// CScpPlugin::DisableServiceL
// -----------------------------------------------------------------------------
//
void CScpPlugin::DisableServiceL( const TServiceSelection& aServiceSelection )
    {
    SCPLOGSTRING3( "CScpPlugin::DisableServiceL service id: %d type: %d",
                   aServiceSelection.iServiceId, aServiceSelection.iType );

    iServiceManager->DisableServiceL( aServiceSelection.iServiceId, 
                                      aServiceSelection.iType );
    }

// -----------------------------------------------------------------------------
// CScpPlugin::IsAvailableL
// -----------------------------------------------------------------------------
//
TBool CScpPlugin::IsAvailableL( const TServiceSelection& aServiceSelection,
                                const RArray<TUint32>& aIapIdArray ) const
    {
    SCPLOGSTRING3( "CScpPlugin::IsAvailableL service id: %d type: %d",
                   aServiceSelection.iServiceId, aServiceSelection.iType );

    return iServiceManager->IsAvailableL( aServiceSelection.iServiceId, 
                                          aServiceSelection.iType, aIapIdArray );
    }
 
// -----------------------------------------------------------------------------
// CScpPlugin::GetServiceState
// -----------------------------------------------------------------------------
//
TInt CScpPlugin::GetServiceState( const TServiceSelection& aServiceSelection, 
                                  TCCHSubserviceState& aState ) const
    {
    SCPLOGSTRING3( "CScpPlugin::GetServiceState service id: %d type: %d",
                   aServiceSelection.iServiceId, aServiceSelection.iType );

    return iServiceManager->GetServiceState( aServiceSelection.iServiceId, 
                                             aServiceSelection.iType, aState );
    }
    
// -----------------------------------------------------------------------------
// CScpPlugin::GetServiceNetworkInfo
// -----------------------------------------------------------------------------
//
TInt CScpPlugin::GetServiceNetworkInfo( TServiceConnectionInfo& aConnectionInfo ) const
    {
    TServiceSelection& selection = aConnectionInfo.iServiceSelection;

    SCPLOGSTRING3( "CScpPlugin::GetServiceNetworkInfo service id: %d type: %d",
                   selection.iServiceId, selection.iType );

    // this should be changed to accept TServiceConnectionInfo 
    return iServiceManager->GetServiceNetworkInfo( selection.iServiceId,
                                                   selection.iType,
                                                   aConnectionInfo.iSNAPId,
                                                   aConnectionInfo.iIapId,
                                                   aConnectionInfo.iSNAPLocked,
                                                   aConnectionInfo.iPasswordSet );
    }
    
// -----------------------------------------------------------------------------
// CScpPlugin::SetSnapId
// -----------------------------------------------------------------------------
//
TInt CScpPlugin::SetSnapId( const TServiceSelection& aServiceSelection,
                            TUint aSnapId )
    {
    SCPLOGSTRING4( "CScpPlugin::SetSnapId service id: %d type: %d Snap %d",
                   aServiceSelection.iServiceId, aServiceSelection.iType, aSnapId );

    return iServiceManager->SetSnapId( aServiceSelection.iServiceId, 
                                       aServiceSelection.iType, 
                                       aSnapId );
    }

// -----------------------------------------------------------------------------
// CScpPlugin::SetIapId
// -----------------------------------------------------------------------------
//
TInt CScpPlugin::SetIapId( const TServiceSelection& aServiceSelection,
                           TUint aIapId )
    {
    SCPLOGSTRING4( "CScpPlugin::SetIapId service id: %d type: %d iap id %d",
                   aServiceSelection.iServiceId, aServiceSelection.iType, aIapId );

    return iServiceManager->SetIapId( aServiceSelection.iServiceId, 
                                      aServiceSelection.iType, 
                                      aIapId );
    }
    
// -----------------------------------------------------------------------------
// CScpPlugin::GetServiceInfo
// -----------------------------------------------------------------------------
//
void CScpPlugin::GetServiceInfoL( const TServiceSelection& aServiceSelection,
                                  RBuf& aBuffer ) const
    {
    SCPLOGSTRING3( "CScpPlugin::GetServiceInfoL service id: %d type: %d",
                   aServiceSelection.iServiceId, aServiceSelection.iType );

    iServiceManager->GetServiceInfoL( aServiceSelection.iServiceId, 
                                      aServiceSelection.iType, 
                                      aBuffer );
    }

// -----------------------------------------------------------------------------
// CScpPlugin::ReserveService
// -----------------------------------------------------------------------------
//
TInt CScpPlugin::ReserveService( const TServiceSelection& aServiceSelection )
    {
    return iServiceManager->SetServiceReserved( ETrue,
                                                aServiceSelection.iServiceId, 
                                                aServiceSelection.iType );
    }

// -----------------------------------------------------------------------------
// CScpPlugin::FreeService
// -----------------------------------------------------------------------------
//
TInt CScpPlugin::FreeService( const TServiceSelection& aServiceSelection )
    {
    return iServiceManager->SetServiceReserved( EFalse,
                                                aServiceSelection.iServiceId, 
                                                aServiceSelection.iType );
    }

// -----------------------------------------------------------------------------
// CScpPlugin::IsReserved
// -----------------------------------------------------------------------------
//
TBool CScpPlugin::IsReserved( const TServiceSelection& aServiceSelection ) const
    {
    return iServiceManager->IsReserved( aServiceSelection.iServiceId, 
                                        aServiceSelection.iType );
    }

// -----------------------------------------------------------------------------
// GetConnectionParameter
// -----------------------------------------------------------------------------
//
TInt CScpPlugin::GetConnectionParameter( 
                             const TServiceSelection& aServiceSelection,
                             TCchConnectionParameter aParameter,
                             TInt& aValue ) const
    {
    SCPLOGSTRING3( "CScpPlugin::GetConnectionParameter (TInt return) service id: %d type: %d",
                   aServiceSelection.iServiceId, aServiceSelection.iType );

    TRAPD( error, iServiceManager->GetConnectionParameterL( aServiceSelection, aParameter, aValue ) );
    return error;
    }

// -----------------------------------------------------------------------------
// GetConnectionParameter
// -----------------------------------------------------------------------------
//
TInt CScpPlugin::GetConnectionParameter( 
                             const TServiceSelection& aServiceSelection, 
                             TCchConnectionParameter aParameter,
                             RBuf& aValue ) const
    {
    SCPLOGSTRING3( "CScpPlugin::GetConnectionParameter (TBuf return) service id: %d type: %d",
                   aServiceSelection.iServiceId, aServiceSelection.iType );

    TRAPD( error, iServiceManager->GetConnectionParameterL( aServiceSelection, aParameter, aValue ) );
    return error;
    }

// -----------------------------------------------------------------------------
// SetConnectionParameter
// -----------------------------------------------------------------------------
//
TInt CScpPlugin::SetConnectionParameter( 
                             const TServiceSelection& aServiceSelection, 
                             TCchConnectionParameter aParameter,
                             TInt aValue )
    {
    SCPLOGSTRING3( "CScpPlugin::SetConnectionParameter(TInt) service id: %d type: %d",
                   aServiceSelection.iServiceId, aServiceSelection.iType );
    
    TRAPD( error, iServiceManager->SetConnectionParameterL( aServiceSelection, aParameter, aValue ) );
    return error;
    }

// -----------------------------------------------------------------------------
// SetConnectionParameter
// -----------------------------------------------------------------------------
//
TInt CScpPlugin::SetConnectionParameter( 
                     const TServiceSelection& aServiceSelection, 
                     TCchConnectionParameter aParameter,
                     const TDesC& aValue )
    {
    SCPLOGSTRING3( "CScpPlugin::SetConnectionParameter(TDesC) service id: %d type: %d",
                   aServiceSelection.iServiceId, aServiceSelection.iType );

    TRAPD( error, iServiceManager->SetConnectionParameterL( aServiceSelection, aParameter, aValue ); )  
    return error;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//
// Rest of the file is for ECom initialization. 
//

// Map the interface UIDs
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( KSIPConnectivityPluginImplUid, CScpPlugin::NewL )
    };


// -----------------------------------------------------------------------------
// ImplementationGroupProxy implements for ECom
// Exported proxy for instantiation method resolution
// Returns: ImplementationTable
// -----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
    return ImplementationTable;
    }



// End of file

