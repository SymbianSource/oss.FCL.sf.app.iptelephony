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
* Description:  CCCHPluginHandler implementation
*
*/


// INCLUDE FILES
#include <ecom/ecom.h>

#include "cchlogger.h"
#include "cchpluginhandler.h"
#include "cchplugin.h"
#include "cchservice.h"
#include "cchservicehandler.h"
#include "cchserverbase.h"

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
// CCCHPluginHandler::CCCHPluginHandler
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CCCHPluginHandler::CCCHPluginHandler( CCCHServerBase& aServer ) :
    iServer( aServer )
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// CCCHPluginHandler::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CCCHPluginHandler* CCCHPluginHandler::NewL( CCCHServerBase& aServer )
    {
    CCCHPluginHandler* self = new (ELeave) CCCHPluginHandler( aServer );
    return self;
    }

// Destructor
CCCHPluginHandler::~CCCHPluginHandler()
    {
    // Delete and close Plug-in array
    iPlugins.ResetAndDestroy();        
    }
    
// ---------------------------------------------------------------------------
// CCCHPluginHandler::LoadPluginsL
// 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHPluginHandler::LoadPluginsL()
    {
    CCHLOGSTRING( "CCCHPluginHandler::LoadPluginsL" );
    
    RImplInfoPtrArray pImplInfoArray;
    CleanupStack::PushL( TCleanupItem( ResetAndDestroy, &pImplInfoArray ) );
    
    REComSession::ListImplementationsL( 
        KCCHConnectivityPluginIFUid, pImplInfoArray );
            
    CCHLOGSTRING2( "Available Plug-ins count: %d", pImplInfoArray.Count() );
    
    TInt found( KErrNone );
    
    for ( TInt i( 0 ); i < pImplInfoArray.Count(); i++ )
        {
        found = Find( pImplInfoArray[ i ]->ImplementationUid() );
        
        if ( KErrNotFound == found )
            {
            CCCHPlugin* pPlugin = CCCHPlugin::NewL(
                    pImplInfoArray[ i ]->ImplementationUid() );
            
            iPlugins.Append( pPlugin );
                
            CCHLOGSTRING2( "    Append Plug-in with Uid:    0x%X", 
                pImplInfoArray[ i ]->ImplementationUid() );
            }            
        }
        
    CleanupStack::PopAndDestroy( &pImplInfoArray );
    }

// ---------------------------------------------------------------------------
// CCCHPluginHandler::LoadPluginsL
// 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHPluginHandler::LoadPluginsL( const TUid& aUid )
    {
    CCHLOGSTRING( "CCCHPluginHandler::LoadPluginsL" );
    CCHLOGSTRING2( "    Uid:        0x%X", aUid );
    
    TBool found( Find( aUid ) );
        
    if ( KErrNotFound == found )
        {
        CCCHPlugin* pPlugin = CCCHPlugin::NewL( aUid );
        
        iPlugins.Append( pPlugin );
            
        CCHLOGSTRING2( "    Append Plug-in with Uid:    0x%X", aUid );
        }
   }

// ---------------------------------------------------------------------------
// CCCHPluginHandler::UnloadPlugin
// 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHPluginHandler::UnloadPlugin( const TUid& aUid )
    {
    CCHLOGSTRING( "CCCHPluginHandler::UnloadPlugin" );
    CCHLOGSTRING2( "    Uid:        0x%X", aUid );
    
    TInt found( Find( aUid ) );
        
    if ( KErrNotFound != found )
        {
        CCCHPlugin* pPlugin = iPlugins[ found ];
        iPlugins.Remove( found );
        delete pPlugin;
            
        CCHLOGSTRING( "    Plugin unloaded.");
        }
     else
        {
        CCHLOGSTRING( "    Plugin not found, cannot unload." );
        }
   }
    
// ---------------------------------------------------------------------------
// CCCHPluginHandler::EnableServiceL
// 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//    
void CCCHPluginHandler::EnableServiceL( 
    const TUint32 aServiceId,
    const RArray<TUid>& aUids, 
    const TCCHSubserviceType aType,
    MCCHServiceNotifier* aNotifier,
    const TUint32 aIapId,
    const TBool /*aConnectivityCheck*/ )
    {
    CCHLOGSTRING( "CCCHPluginHandler::EnableServiceL" );
    CCHLOGSTRING2( "    Service Id: %d", aServiceId );
    CCHLOGSTRING2( "    Iap Id:     %d", aIapId );
        
    TInt found( KErrNone );
    
    for ( TInt i( 0 ); i < aUids.Count(); i++ )
        {
        CCHLOGSTRING2( "    Uid:        0x%X", aUids[ i ] );
        
        TRAP_IGNORE( LoadPluginsL( aUids[ i ] ) );
        found = Find( aUids[ i ] );
        if ( KErrNotFound != found )
            {
            if ( aNotifier )
                {
                iPlugins[ found ]->SetServiceNotifier( aNotifier );    
                }
            
            TCCHSubserviceState state = ECCHUninitialized;
            const CCCHPlugin* plugin = iPlugins[ found ];
        	plugin->Plugin()->GetServiceState( 
        			            TServiceSelection( aServiceId, aType ), 
        			            state ); 
        	
            // check current service state
        	if( state==ECCHDisconnecting )
        		{
        		// return KErrNotReady back to client
        		User::Leave( KErrNotReady );
        		}
        	
       		!aIapId ? plugin->Plugin()->EnableServiceL( 
       				            TServiceSelection( aServiceId, aType ) ) :
        			  plugin->Plugin()->EnableServiceL( 
        					    TServiceSelection( aServiceId, aType ), 
        					    aIapId );
            }
        }
    }
// ---------------------------------------------------------------------------
// CCCHPluginHandler::DisableServiceL
// 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//    
void CCCHPluginHandler::DisableServiceL( 
    const TUint32 aServiceId,
    const RArray<TUid>& aUids,
    const TCCHSubserviceType aType ) const
    {
    CCHLOGSTRING( "CCCHPluginHandler::DisableServiceL" );
    CCHLOGSTRING2( "    Service Id: %d", aServiceId );
    
    TInt found( KErrNone );
    
    for ( TInt i( 0 ); i < aUids.Count(); i++ )
        {
        CCHLOGSTRING2( "    Uid:        0x%X", aUids[ i ] );
        
        found = Find( aUids[ i ] );
        
        if ( KErrNotFound != found )
            {
        	TRAP_IGNORE(iPlugins[ found ]->Plugin()->DisableServiceL( 
                    TServiceSelection( aServiceId, aType ) ) );
            }
        }
    }

// ---------------------------------------------------------------------------
// CCCHPluginHandler::GetServiceState
// 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//    
void CCCHPluginHandler::GetServiceState( 
    const TUint32 aServiceId, 
    const TUid aUid,
    const TCCHSubserviceType aType,
    TCCHSubserviceState& aState,
    TInt& aError )
    {
    CCHLOGSTRING( "CCCHPluginHandler::GetServiceState" );
    CCHLOGSTRING2( "CCCHPluginHandler::GetServiceState: Service Id: %d", aServiceId );
    CCHLOGSTRING2( "CCCHPluginHandler::GetServiceState: Uid:        0x%X", aUid );
    
    TInt found( Find( aUid ) );
    
    if ( KErrNotFound != found )
        {
        aError = iPlugins[ found ]->Plugin()->GetServiceState( 
            TServiceSelection( aServiceId, aType ), aState );
        }
        
    CCHLOGSTRING3( "CCCHPluginHandler::GetServiceState: State, error:      %d, %d", aState, aError );
    }

// ---------------------------------------------------------------------------
// CCCHPluginHandler::ReserveService
// 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//    
TInt CCCHPluginHandler::ReserveService( 
    const TUint32 aServiceId,const TUid aUid, const TCCHSubserviceType aType  ) 
    {
    CCHLOGSTRING( "CCCHPluginHandler::ReserveService" );
    CCHLOGSTRING2( "    Service Id: %d", aServiceId );
    CCHLOGSTRING2( "    Uid:        0x%X", aUid );
    
    TInt found( Find( aUid ) );
    
    if ( KErrNotFound != found )
        {
        found = iPlugins[ found ]->Plugin()->ReserveService( 
            TServiceSelection( aServiceId, aType ) );
        }
        
    return found;
    }

// ---------------------------------------------------------------------------
// CCCHPluginHandler::FreeService
// 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//    
TInt CCCHPluginHandler::FreeService( 
    const TUint32 aServiceId,const TUid aUid, const TCCHSubserviceType aType  )
    {
    CCHLOGSTRING( "CCCHPluginHandler::FreeService" );
    CCHLOGSTRING2( "    Service Id: %d", aServiceId );
    CCHLOGSTRING2( "    Uid:        0x%X", aUid );
    
    TInt found( Find( aUid ) );
    
    if ( KErrNotFound != found )
        {
        found = iPlugins[ found ]->Plugin()->FreeService( 
            TServiceSelection( aServiceId, aType ) );
        }
        
    return found;
    }

// ---------------------------------------------------------------------------
// CCCHPluginHandler::IsReserved
// 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//    
TBool CCCHPluginHandler::IsReserved( 
    const TUint32 aServiceId,const TUid aUid, 
    const TCCHSubserviceType aType  ) const
    {
    CCHLOGSTRING( "CCCHPluginHandler::ReferenceCount" );
    CCHLOGSTRING2( "    Service Id: %d", aServiceId );
    CCHLOGSTRING2( "    Uid:        0x%X", aUid );
    
    TInt found( Find( aUid ) );
    TBool ret(EFalse);
    if ( KErrNotFound != found )
        {
        ret = iPlugins[ found ]->Plugin()->IsReserved( 
            TServiceSelection( aServiceId, aType ) );
        }
        
    return ret;
    }

// ---------------------------------------------------------------------------
// CCCHPluginHandler::GetServiceNetworkInfo
// 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//    
TInt CCCHPluginHandler::GetServiceNetworkInfo( 
        const TUint32 aServiceId,
        const TUid aUid,
        const TCCHSubserviceType aType,
        TUint32& aSnapId, 
        TUint32& aIapId,
        TBool& aSNAPLocked,
        TBool& aPasswordSet )
        {
        CCHLOGSTRING( "CCCHPluginHandler::GetServiceNetworkInfo" );
        CCHLOGSTRING2( "    Service Id: %d", aServiceId );
        CCHLOGSTRING2( "    Uid:        0x%X", aUid );
        
        TUint snapId( 0 );
        TUint iapId( 0 );

        TRAP_IGNORE( LoadPluginsL( aUid ) );
        TInt found( Find( aUid ) );
        TServiceConnectionInfo info(aServiceId, aType, snapId, iapId );
        if ( KErrNotFound != found )
            {
            found = iPlugins[ found ]->Plugin()->GetServiceNetworkInfo( 
                info );
            }
            
        aSnapId = info.iSNAPId;   
        aIapId = info.iIapId;
        aSNAPLocked = info.iSNAPLocked;
        aPasswordSet = info.iPasswordSet;
        
        CCHLOGSTRING2( "    SNAP Id:    %d", aSnapId );
        CCHLOGSTRING2( "    IAP Id:     %d", aIapId );
        
        return found;
    }

// ---------------------------------------------------------------------------
// CCCHPluginHandler::SetSnapIdL
// 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHPluginHandler::SetSnapIdL( 
    const TUint32 aServiceId,
    const RArray<TUid>& aUids,
    const TCCHSubserviceType aType,
    const TUint32 aSnapId )
    {
    CCHLOGSTRING( "CCCHPluginHandler::SetSnapId" );
    CCHLOGSTRING2( "    Service Id: %d", aServiceId );
    CCHLOGSTRING2( "    SNAP Id:    %d", aSnapId );
    
    TInt found( KErrNone );
    
    for ( TInt i( 0 ); i < aUids.Count(); i++ )
        {
        CCHLOGSTRING2( "    Uid:        0x%X", aUids[ i ] );
        
        TRAP_IGNORE( LoadPluginsL( aUids[ i ] ) );
        found = Find( aUids[ i ] );
        
        if ( KErrNotFound != found )
            {
            iPlugins[ found ]->Plugin()->SetSnapId( 
                    TServiceSelection( aServiceId, aType ), aSnapId );
            }
        }
    }

// ---------------------------------------------------------------------------
// CCCHPluginHandler::SetIapIdL
// 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHPluginHandler::SetIapIdL( 
    const TUint32 aServiceId,
    const RArray<TUid>& aUids,
    const TCCHSubserviceType aType,
    const TUint32 aIapId )
    {
    CCHLOGSTRING( "CCCHPluginHandler::SetIapId" );
    CCHLOGSTRING2( "    Service Id: %d", aServiceId );
    CCHLOGSTRING2( "    IAP Id:     %d", aIapId );
    
    TInt found( KErrNone );
    
    for ( TInt i( 0 ); i < aUids.Count(); i++ )
        {
        CCHLOGSTRING2( "    Uid:        0x%X", aUids[ i ] );
        
        TRAP_IGNORE( LoadPluginsL( aUids[ i ] ) );
        found = Find( aUids[ i ] );
        
        if ( KErrNotFound != found )
            {
            iPlugins[ found ]->Plugin()->SetIapId( 
                    TServiceSelection( aServiceId, aType ), aIapId );
            }
        }
    }

// ---------------------------------------------------------------------------
// CCCHPluginHandler::SetUsernameL
// 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHPluginHandler::SetUsernameL( const TUint32 aServiceId,
        const RArray<TUid>& aUids,
        const TCCHSubserviceType aType,
        const TDesC& aUsername )
    {
    CCHLOGSTRING( "CCCHPluginHandler::SetUsernameL" );
    CCHLOGSTRING2( "    Service Id: %d", aServiceId );
    CCHLOGSTRING2( "    Username:     %S", &aUsername );
    
    TInt found( KErrNone );
    
    for ( TInt i( 0 ); i < aUids.Count(); i++ )
        {
        CCHLOGSTRING2( "    Uid:        0x%X", aUids[ i ] );
        
        LoadPluginsL( aUids[ i ] );
        found = Find( aUids[ i ] );
        
        if ( KErrNotFound != found )
            {
            User::LeaveIfError( iPlugins[ found ]->Plugin()->SetConnectionParameter( 
                    TServiceSelection( aServiceId, aType ),
                    ECchUsername, aUsername ) );
            }
        else
            {
            User::Leave( KErrNotFound );
            }
        }
    }

// ---------------------------------------------------------------------------
// CCCHPluginHandler::SetPasswordL
// 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHPluginHandler::SetPasswordL( const TUint32 aServiceId,
        const RArray<TUid>& aUids,
        const TCCHSubserviceType aType,
        const TDesC& aPassword )
    {
    CCHLOGSTRING( "CCCHPluginHandler::SetPasswordL" );
    CCHLOGSTRING2( "    Service Id: %d", aServiceId );
    CCHLOGSTRING2( "    Password:     %S", &aPassword );
    
    TInt found( KErrNone );
    
    for ( TInt i( 0 ); i < aUids.Count(); i++ )
        {
        CCHLOGSTRING2( "    Uid:        0x%X", aUids[ i ] );
        
        LoadPluginsL( aUids[ i ] );
        found = Find( aUids[ i ] );
        
        if ( KErrNotFound != found )
            {
            User::LeaveIfError( iPlugins[ found ]->Plugin()->SetConnectionParameter( 
                    TServiceSelection( aServiceId, aType ),
                    ECchPassword, aPassword ) );
            }
        else
            {
            User::Leave( KErrNotFound );
            }
        }
    }

// ---------------------------------------------------------------------------
// CCCHPluginHandler::GetConnectionParameter
// 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CCCHPluginHandler::GetConnectionParameter(  
                     const TUid aUid,
                     const TServiceSelection& aServiceSelection, 
                     RBuf& aValue )
    {
    CCHLOGSTRING( "CCCHPluginHandler::GetConnectionParameter" );
    CCHLOGSTRING2( "    UID: %d", aUid );
    CCHLOGSTRING2( "    Service Id: %d", aServiceSelection.iServiceId );
    
    TRAP_IGNORE( LoadPluginsL( aUid ) );
    TInt retval( Find( aUid ) );
    
    if ( KErrNotFound != retval )
        {
        retval = iPlugins[ retval ]->Plugin()->GetConnectionParameter( 
                       aServiceSelection, aServiceSelection.iParameter,
                       aValue );
        }
   
    CCHLOGSTRING2( "CCCHPluginHandler::GetConnectionParameter: returning %d", retval );
    return retval;                
    }

// ---------------------------------------------------------------------------
// CCCHPluginHandler::GetServiceInfo
// 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CCCHPluginHandler::GetServiceInfo( 
    const TUint32 aServiceId,
    const TUid aUid,
    const TCCHSubserviceType aType,
    TDes& aBuffer )
    {
    CCHLOGSTRING( "CCCHPluginHandler::GetServiceInfo" );
    CCHLOGSTRING2( "    Service Id: %d", aServiceId );
    CCHLOGSTRING2( "    Uid:        0x%X", aUid );
    
    TRAP_IGNORE( LoadPluginsL( aUid ) );
    TInt found( Find( aUid ) );
    
    if ( KErrNotFound != found )
        {
        RBuf buf;
        TRAPD( error, iPlugins[ found ]->Plugin()->
            GetServiceInfoL( TServiceSelection( aServiceId, aType ), 
            buf ) ); 
        aBuffer.Copy( buf );
        buf.Close();
        found = error;
        }
            
    CCHLOGSTRING2( "    Info:       %S", &aBuffer );
    return found;
    }

// ---------------------------------------------------------------------------
// CCCHPluginHandler::IsAvailableL
// 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CCCHPluginHandler::IsAvailableL( 
    const TUint aServiceId,
    const TUid aUid,
    const TCCHSubserviceType aType,
    RArray<TUint32>& aIapIds ) const
    {
    CCHLOGSTRING( "CCCHPluginHandler::IsAvailableL" );
    CCHLOGSTRING2( "    Service Id: %d", aServiceId );
    CCHLOGSTRING2( "    Uid:        0x%X", aUid );
    
    TInt index( Find( aUid ) );
    
    User::LeaveIfError( index );
    
    return iPlugins[ index ]->Plugin()->IsAvailableL( 
        TServiceSelection( aServiceId, aType ), aIapIds );
    }
                                             
// ---------------------------------------------------------------------------
// CCCHPluginHandler::SetServiceNotifier
// 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHPluginHandler::SetServiceNotifier( 
    MCCHServiceNotifier* aObserver )
    {
    CCHLOGSTRING( "CCCHPluginHandler::SetServiceNotifier" );
            
    for ( TInt i( 0 ); i < iPlugins.Count(); i++ )
        {
        iPlugins[ i ]->SetServiceNotifier( aObserver );
        }
    }

// ---------------------------------------------------------------------------
// CCCHPluginHandler::RemoveServiceNotifier
// 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHPluginHandler::RemoveServiceNotifier( 
    MCCHServiceNotifier* aObserver )
    {
    CCHLOGSTRING( "CCCHPluginHandler::RemoveServiceNotifier" );
            
    for ( TInt i( 0 ); i < iPlugins.Count(); i++ )
        {
        iPlugins[ i ]->RemoveServiceNotifier( aObserver );
        }
    }

// ---------------------------------------------------------------------------
// CCCHPluginHandler::Find
// 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CCCHPluginHandler::Find( const TUid& aUid ) const
    {
    CCHLOGSTRING( "CCCHPluginHandler::Find" );
    
    TInt index( KErrNotFound );
    
    for ( TInt i( 0 ); i < iPlugins.Count() && KErrNotFound == index; i++ )
        {
        if ( iPlugins[ i ]->CheckUid( aUid ) )
            {
            index = i;
            }
        }
    
    CCHLOGSTRING2( "CCCHPluginHandler::Find: index: %d", index ); 
           
    return index;
    }
    
// ---------------------------------------------------------------------------
// CCCHPluginHandler::ResetAndDestroy
// 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHPluginHandler::ResetAndDestroy( TAny* aArray )
    {
    RImplInfoPtrArray* array = reinterpret_cast<RImplInfoPtrArray*>( aArray );
    array->ResetAndDestroy();
    }
                                         
// ========================== OTHER EXPORTED FUNCTIONS =======================

//  End of File
