/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CCCHServiceHandler implementation
*
*/


// INCLUDE FILES
#include <cchclientserver.h>
#include <spnotifychange.h>

#include "cchservicehandler.h"
#include "cchserverbase.h"
#include "cchspshandler.h"
#include "cchlogger.h"
#include "cchpluginhandler.h"
#include "cchuihandler.h"
#include "cchconnmonhandler.h"
#include "cchwlanextension.h"
#include "cchcommdbwatcher.h"
#include "cchcommdbwatcherobserver.h"

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES
// None

// CONSTANTS

const TInt KConnectionRecoveryMaxTries = 5;

const TInt KCCHFirstRecovery    = 15000000;
const TInt KCCHSecondRecovery   = 15000000;
const TInt KCCHThirdRecovery    = 30000000;
const TInt KCCHFourthRecovery   = 60000000;
const TInt KCCHFifthRecovery    = 60000000;

const TInt KCCHPluginUnloadTimeout = 5000000;

const TInt KCCHHandleNotifyDelay = 1000000;

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None.

// ============================= LOCAL FUNCTIONS =============================

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// CCCHServiceHandler::CCCHServiceHandler
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CCCHServiceHandler::CCCHServiceHandler( CCCHServerBase& aServer ) :
    iServer( aServer ),
    iConnectionRecoveryTry( 0 ),
    iCancelNotify( ETrue ) 
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// CCCHServiceHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CCCHServiceHandler::ConstructL()
    {
    iNotifier = CSPNotifyChange::NewL( *this );
    iCchUIHandler = CCchUIHandler::NewL( iServer, *this );
    iConnectionRecoveryTimer = CPeriodic::NewL( CPeriodic::EPriorityStandard );
    iCommDbWatcher = CCCHCommDbWatcher::NewL( *this );
    iPluginUnloadTimer = CPeriodic::NewL( CPeriodic::EPriorityStandard );
    iHandleNotifyDelayTimer = CPeriodic::NewL( CPeriodic::EPriorityStandard );
    iRecoveryInterval.Append( KCCHFirstRecovery );
    iRecoveryInterval.Append( KCCHSecondRecovery );
    iRecoveryInterval.Append( KCCHThirdRecovery );
    iRecoveryInterval.Append( KCCHFourthRecovery );
    iRecoveryInterval.Append( KCCHFifthRecovery );
    iWlanExtension = CCchWlanExtension::NewL( );
    }

// ---------------------------------------------------------------------------
// CCCHServiceHandler::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CCCHServiceHandler* CCCHServiceHandler::NewL( CCCHServerBase& aServer )
    {
    CCCHServiceHandler* self = CCCHServiceHandler::NewLC( aServer );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CCCHServiceHandler::NewLC
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CCCHServiceHandler* CCCHServiceHandler::NewLC( CCCHServerBase& aServer )
    {
    CCCHServiceHandler* self = new (ELeave) CCCHServiceHandler( aServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// Destructor
CCCHServiceHandler::~CCCHServiceHandler()
    {
    if ( iCchUIHandler )
        {
        iCchUIHandler->Destroy();
        }

    delete iWlanExtension;
    delete iConnectionRecoveryTimer;
    delete iPluginUnloadTimer;
    delete iHandleNotifyDelayTimer;
    delete iCommDbWatcher;

    if( iNotifier )
        {
        iNotifier->Cancel();
        delete iNotifier;
        iNotifier = NULL;    
        }

    iServiceIds.Close();
    iServer.PluginHandler().RemoveServiceNotifier( this );
    iServices.ResetAndDestroy();
    iServicesInRecovery.Close();
    iRecoveryInterval.Close();
    }

// ---------------------------------------------------------------------------
// CCCHServiceHandler::InitServiceHandlerL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHServiceHandler::InitServiceHandlerL()
    {
    CCHLOGSTRING2( "CCCHServiceHandler::InitServiceHandlerL [0x%x]: IN",this );
    
    RArray<TUint> serviceIds;
    CleanupClosePushL( serviceIds );
    iServer.SPSHandler().GetServiceIdsL( serviceIds );
    for ( TInt i( 0 ); i < serviceIds.Count(); i++ )
        {        
        if ( KErrNotFound == FindService( serviceIds[ i ] ) )
            {
            TCCHService service;
            TRAPD( error, iServer.SPSHandler().GetServiceInfoL( 
                serviceIds[ i ], service ) );
            if ( KErrNone == error )
                {
                AddServiceL( service );    
                }                    
            }
        }
    CleanupStack::PopAndDestroy( &serviceIds );
    // Subscribe to service change notifies    
    EnableNotifyChange();
    
    CCHLOGSTRING( "CCCHServiceHandler::InitServiceHandlerL: OUT" );
    }

// ---------------------------------------------------------------------------
// CCCHServiceHandler::AddServiceL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHServiceHandler::AddServiceL( TCCHService& aService )
    {
    CCHLOGSTRING( "CCCHServiceHandler::AddServiceL: IN" );
    
    CCCHServiceInfo* cchServiceInfo = CCCHServiceInfo::NewLC( iServer );
    cchServiceInfo->SetServiceId( aService.iServiceId );
    cchServiceInfo->SetName( aService.iServiceName );
    CCHLOGSTRING2( "CCCHServiceHandler::AddServiceL: adding service with %d subservices",aService.iSubservices.Count() );
    for ( TInt i( 0 ); i < aService.iSubservices.Count(); i++ )
        {
        cchServiceInfo->AddSubserviceL( aService.iSubservices[ i ] );
        }
        
    CleanupStack::Pop( cchServiceInfo );
    iServices.Append( cchServiceInfo );
    
    CCHLOGSTRING( "CCCHServiceHandler::AddServiceL: OUT" );
    }

// ---------------------------------------------------------------------------
// CCCHServiceHandler::UpdateL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHServiceHandler::UpdateL( TBool aReadSettingTable )
    {
    CCHLOGSTRING( "CCCHServiceHandler::UpdateL: IN" );
    
    // Read settings from service table
    if ( aReadSettingTable )
        {
        RArray<TUint> serviceIds;
        CleanupClosePushL( serviceIds );
        
        TInt index( KErrNotFound );
        iServer.SPSHandler().GetServiceIdsL( serviceIds );
        
        for ( TInt i( 0 ); i < serviceIds.Count(); i++ )
            {        
            index = FindService( serviceIds[ i ] );
            if ( KErrNotFound != index )
                {
                iServices[ index ]->UpdateL( ETrue );
                }
            else
                {
                CCHLOGSTRING2
                    ( "CCCHSeviceHandler::UpdateL: \
                        New service found: ServiceId %d", serviceIds[ i ] );
                TCCHService service;
                TRAPD( error, iServer.SPSHandler().GetServiceInfoL( 
                    serviceIds[ i ], service ) );
                if ( KErrNone == error )
                    {
                    AddServiceL( service );
                    }                
                }
            }
        if ( serviceIds.Count() < iServices.Count() )
            {
            // Remove service's which does not exist anymore            
            for ( TInt i( 0 ); i < iServices.Count(); i++ )
                {            
                if ( KErrNotFound == serviceIds.Find( 
                    iServices[ i ]->GetServiceId( ) ) )
                    {
                    CCHLOGSTRING2
                        ( "CCCHSeviceHandler::UpdateL: \
                        service removed: ServiceId %d", 
                        iServices[ i ]->GetServiceId() );
                    delete iServices[ i ];
                    iServices.Remove( i );
                    i--;
                    StartPluginUnloadTimer( );
                    }
                }
            }
        CleanupStack::PopAndDestroy( &serviceIds );
        }
    else
        {
        for ( TInt i( 0 ); i < iServices.Count(); i++ )
            {
            iServices[ i ]->UpdateL();
            }    
        }
    
    CCHLOGSTRING( "CCCHServiceHandler::UpdateL: OUT" );
    }

// ---------------------------------------------------------------------------
// CCCHServiceHandler::IsStartupFlagSet
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CCCHServiceHandler::IsStartupFlagSet() const
    {
    TBool ret( EFalse );

    for ( TInt i( 0 ); i < iServices.Count() && !ret; i++ )
        {
        ret = iServices[ i ]->StartupFlagSet();
        }
        
    CCHLOGSTRING2( "CCCHServiceHandler::IsStartupFlagSet: %d", ret );
    return ret;
    }


// ---------------------------------------------------------------------------
// CCCHServiceHandler::IsRecoveryNeeded
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CCCHServiceHandler::IsRecoveryNeeded( 
                               const TCCHSubserviceState aState, 
                               TInt aError )
    {
    TBool recover( EFalse );
    CCHLOGSTRING( "CCCHServiceHandler::IsRecoveryNeeded: IN" );
    if( !iServer.FeatureManager().OfflineMode() &&
        ECCHConnecting == aState && ( 
        KCCHErrorNetworkLost == aError || 
        KCCHErrorServiceNotResponding == aError ) &&
        !iConnectionRecoveryTimer->IsActive() )
        {
        recover = ETrue;            
        }
    CCHLOGSTRING2( "CCCHServiceHandler::IsRecoveryNeeded: OUT, value=%d", recover );
    return recover;
    }

// ---------------------------------------------------------------------------
// CCCHServiceHandler::StateChangedL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHServiceHandler::StateChangedL( const TUint aServiceId,
                                        const TCCHSubserviceType aType,
                                        const TCCHSubserviceState aState,
                                        const TInt aError )
    {
    
    CCHLOGSTRING( "CCCHServiceHandler::StateChanged: IN" );
    CCHLOGSTRING2( "CCCHServiceHandler::StateChanged: service id;        %d",aServiceId );
    CCHLOGSTRING2( "CCCHServiceHandler::StateChanged: service type;      %d",aType );
    CCHLOGSTRING2( "CCCHServiceHandler::StateChanged: service new state; %d",aState );
    CCHLOGSTRING2( "CCCHServiceHandler::StateChanged: service error;     %d",aError );
    TInt index( FindService( aServiceId ) );
    if ( KErrNotFound != index )
        {
        iServices[ index ]->SetErrorL( aType, aError );
        iServices[ index ]->SetStateL( aType, aState );
        //toggle voip indicator & emergency note if needed
        if( ECCHVoIPSub == aType )
            {
            iCchUIHandler->UpdateUI( );
            }
        
        if ( ECCHEnabled == aState && !iCchUIHandler->IsCostWarningSeen() )
            {
            // Check for possible first usage of GPRS (roaming cost warning)
            TServiceConnectionInfo serviceConnInfo( aServiceId, aType, 0, 0 );
            GetConnectionInfo( serviceConnInfo );
            
            if ( serviceConnInfo.iIapId )
                {
                // Stop monitoring connectivity changes and take new connections
                RArray<TUint> iaps;
                CleanupClosePushL( iaps );
                TBool gprs( EFalse );
                iServer.ConnMonHandler().StopMonitoringConnectionChanges( iaps );
                
                // Are we connected via VPN
                if ( iCommDbWatcher->IsVpnApL( serviceConnInfo.iIapId ) )
                    {
                    // Remove iaps whom are not linked to vpn(snap or iap)
                    TRAP_IGNORE( iCommDbWatcher->RemoveOtherThanVpnIapsL( 
                        iaps, serviceConnInfo.iIapId ) );
                                
                    for ( TInt i( 0 ); i < iaps.Count(); i++ )
                        {
                        CCHLOGSTRING2( "CCCHServiceHandler::StateChanged: new connections: %d",iaps[ i ] );
                        
                        // if any new connection is gprs connection show note if not showed already
                        if( !iCommDbWatcher->IsVpnApL( iaps[ i ] ) && !iCommDbWatcher->IsWlanApL( iaps[ i ] ) )
                            {
                            gprs = ETrue;
                            break;
                            }
                        }
                    }
                else if( !iCommDbWatcher->IsWlanApL( serviceConnInfo.iIapId ) )
                    {      
                    gprs = ETrue;
                    }
                CleanupStack::PopAndDestroy( &iaps );
                
                if ( gprs )
                    {
                    iCchUIHandler->CheckGprsFirstUsageL();
                    }
                }
            }
        
        //unload useless plugins    
        if( ECCHDisabled == aState )
            {
            StartPluginUnloadTimer( );
            }
        //start fast recovery if not started
        else if( IsRecoveryNeeded( aState, aError ) )
            {
            CCHLOGSTRING( "CCCHServiceHandler::StateChangedL network lost, starting recovery" );
            iServicesInRecovery.Append( TServiceSelection( aServiceId, aType ) );
            StartConnectionRecoveryTimer( CCCHServiceHandler::ConnectionRecoveryEvent );                
            }
        //stop fast recovery
        if( ECCHConnecting != aState && 
                KErrNone == aError &&
                0 < iServicesInRecovery.Count() )
            {
            CCHLOGSTRING( "CCCHServiceHandler::StateChangedL checking if recovery is still needed" );
            TInt idx = iServicesInRecovery.Find( TServiceSelection( aServiceId, aType ) );
            if( KErrNotFound != idx )
                {
                CCHLOGSTRING( "CCCHServiceHandler::StateChangedL this service has recovered, recovery still on..." );
                iServicesInRecovery.Remove( idx );
                }
            if( 0 == iServicesInRecovery.Count() )
                {
                CCHLOGSTRING( "CCCHServiceHandler::StateChangedL all services recovered, stopping recovery" );
                CancelConnectionRecoveryTimer();
                }
            }
        
        if ( ECCHConnecting != aState || 
            ECCHConnecting == aState && KErrNone != aError )
            {
            // reset startup counter
            TBool startupFlag( EFalse );
            iServer.SPSHandler().LoadAtStartUpL( aServiceId, aType, startupFlag );
            if ( startupFlag )
                {
                iServer.ResetStartupCounterL();
                }
            }
        }
    else
        {
        User::Leave( index );
        }
    CCHLOGSTRING( "CCCHServiceHandler::StateChanged: OUT" );
        
    }

// -----------------------------------------------------------------------------
// CCCHServiceHandler::StartConnectionRecoveryTimer
// -----------------------------------------------------------------------------
//
void CCCHServiceHandler::StartConnectionRecoveryTimer( TInt (*aFunction)(TAny* aPtr) )
    {
    CCHLOGSTRING2( "CCCHServiceHandler[0x%x]::StartConnectionRecoveryTimer; IN", this );
    CancelConnectionRecoveryTimer();
    if( !iConnectionRecoveryTimer->IsActive() )
        {
        iConnectionRecoveryTry = 0;
        iConnectionRecoveryTimer->Start( iRecoveryInterval[ iConnectionRecoveryTry ] , 
                              0, 
                              TCallBack( aFunction, this ) );
        CCHLOGSTRING( "CCCHServiceHandler::StartConnectionRecoveryTimer; recovery timer started");
        
        }
    CCHLOGSTRING( "CCCHServiceHandler::StartConnectionRecoveryTimer; OUT");
    }

// -----------------------------------------------------------------------------
// CCCHServiceHandler::CancelConnectionRecoveryTimer
// -----------------------------------------------------------------------------
//
void CCCHServiceHandler::CancelConnectionRecoveryTimer()
    {
    CCHLOGSTRING2( "CCCHServiceHandler[0x%x]::CancelConnectionRecoveryTimer; IN", this );

    if( iConnectionRecoveryTimer->IsActive() )
        {
        iConnectionRecoveryTimer->Cancel();
        }
    CCHLOGSTRING( "CCCHServiceHandler::CancelConnectionRecoveryTimer; OUT" );        
    }

// -----------------------------------------------------------------------------
// CCCHServiceHandler::ConnectionRecoveryEvent
// -----------------------------------------------------------------------------
//
TInt CCCHServiceHandler::ConnectionRecoveryEvent( TAny* aSelf )
    {
    CCHLOGSTRING( "CCCHServiceHandler::ConnectionRecoveryEvent; IN" );
    CCCHServiceHandler* self = static_cast<CCCHServiceHandler*>( aSelf );
    self->HandleConnectionRecovery();
    CCHLOGSTRING( "CCCHServiceHandler::ConnectionRecoveryEvent; OUT" );
    return 0;
    }

// -----------------------------------------------------------------------------
// CCCHServiceHandler::HandleConnectionRecovery
// -----------------------------------------------------------------------------
//
void CCCHServiceHandler::HandleConnectionRecovery()
    {
    CCHLOGSTRING2( "CCCHServiceHandler[0x%x]::HandleConnectionRecovery IN",
                   this );
    iConnectionRecoveryTry++;
    CCHLOGSTRING2( "CCCHServiceHandler::HandleConnectionRecovery; recovery try:%d",iConnectionRecoveryTry );
    iServer.ConnMonHandler().ScanNetworks( ETrue );    
    CancelConnectionRecoveryTimer();
    
    if( KConnectionRecoveryMaxTries > iConnectionRecoveryTry )
        {
        CCHLOGSTRING( "CCCHServiceHandler::HandleConnectionRecovery; recovery continues" );
        iConnectionRecoveryTimer->Start( iRecoveryInterval[ iConnectionRecoveryTry ], 
                              0,
                              TCallBack( CCCHServiceHandler::ConnectionRecoveryEvent, this ) );
        }
    else
        {
        CCHLOGSTRING( "CCCHServiceHandler::HandleConnectionRecovery; max tries reached, recovery canceled" );
        }                   
    CCHLOGSTRING( "CCCHServiceHandler::HandleConnectionRecovery OUT" );
    }

// -----------------------------------------------------------------------------
// CCCHServiceHandler::StartPluginUnloadTimer
// -----------------------------------------------------------------------------
//
void CCCHServiceHandler::StartPluginUnloadTimer( )
    {
    CCHLOGSTRING2( "CCCHServiceHandler[0x%x]::StartPluginUnloadTimer; IN", this );
    CancelPluginUnloadTimer();
    if( !iPluginUnloadTimer->IsActive() )
        {
        iPluginUnloadTimer->Start( KCCHPluginUnloadTimeout , 
                              0, 
                              TCallBack( CCCHServiceHandler::PluginUnloadEvent, this ) );
        CCHLOGSTRING( "CCCHServiceHandler::StartPluginUnloadTimer; plugin unload timer started");
        
        }
    CCHLOGSTRING( "CCCHServiceHandler::StartPluginUnloadTimer; OUT");
    }

// -----------------------------------------------------------------------------
// CCCHServiceHandler::CancelPluginUnloadTimer
// -----------------------------------------------------------------------------
//
void CCCHServiceHandler::CancelPluginUnloadTimer()
    {
    CCHLOGSTRING2( "CCCHServiceHandler[0x%x]::CancelPluginUnloadTimer; IN", this );

    if( iPluginUnloadTimer->IsActive() )
        {
        iPluginUnloadTimer->Cancel();
        }
    CCHLOGSTRING( "CCCHServiceHandler::CancelPluginUnloadTimer; OUT" );        
    }

// -----------------------------------------------------------------------------
// CCCHServiceHandler::StartHandleNotifyDelayTimer
// -----------------------------------------------------------------------------
//
void CCCHServiceHandler::StartHandleNotifyDelayTimer( )
    {
    CCHLOGSTRING2( "CCCHServiceHandler[0x%x]::StartHandleNotifyDelayTimer; IN", this );
    
    CancelHandleNotifyDelayTimer();
    if( !iHandleNotifyDelayTimer->IsActive() )
        {
        iHandleNotifyDelayTimer->Start( 
            KCCHHandleNotifyDelay , 
            0, 
            TCallBack( CCCHServiceHandler::HandleNotifyEvent, this ) );
        
        CCHLOGSTRING( "CCCHServiceHandler::StartHandleNotifyDelayTimer; timer started");
        }
    
    CCHLOGSTRING( "CCCHServiceHandler::StartHandleNotifyDelayTimer; OUT" );   
    }

// -----------------------------------------------------------------------------
// CCCHServiceHandler::CancelHandleNotifyDelayTimer
// -----------------------------------------------------------------------------
//
void CCCHServiceHandler::CancelHandleNotifyDelayTimer()
    {
    CCHLOGSTRING2( "CCCHServiceHandler[0x%x]::CancelHandleNotifyDelayTimer; IN", this );

    if( iHandleNotifyDelayTimer->IsActive() )
        {
        iHandleNotifyDelayTimer->Cancel();
        }
    
    CCHLOGSTRING( "CCCHServiceHandler::CancelHandleNotifyDelayTimer; OUT" );        
    }

// -----------------------------------------------------------------------------
// CCCHServiceHandler::PluginUnloadEvent
// -----------------------------------------------------------------------------
//
TInt CCCHServiceHandler::PluginUnloadEvent( TAny* aSelf )
    {
    CCHLOGSTRING( "CCCHServiceHandler::PluginUnloadEvent; IN" );
    CCCHServiceHandler* self = static_cast<CCCHServiceHandler*>( aSelf );
    self->HandlePluginUnload();
    CCHLOGSTRING( "CCCHServiceHandler::PluginUnloadEvent; OUT" );
    return 0;
    }

// -----------------------------------------------------------------------------
// CCCHServiceHandler::HandleNotifyEvent
// -----------------------------------------------------------------------------
//
TInt CCCHServiceHandler::HandleNotifyEvent( TAny* aSelf )
    {
    CCHLOGSTRING( "CCCHServiceHandler::HandleNotifyEvent; IN" );
  
    CCHLOGSTRING( "CCCHServiceHandler::PluginUnloadEvent; IN" );
    CCCHServiceHandler* self = static_cast<CCCHServiceHandler*>( aSelf );
    self->HandleDelayedNotifyEvent();
    CCHLOGSTRING( "CCCHServiceHandler::HandleNotifyEvent; OUT" );
    return 0;
    }

// -----------------------------------------------------------------------------
// CCCHServiceHandler::HandlePluginUnload
// -----------------------------------------------------------------------------
//
void CCCHServiceHandler::HandlePluginUnload()
    {
    CCHLOGSTRING2( "CCCHServiceHandler[0x%x]::HandlePluginUnload IN",
                   this );
    CancelPluginUnloadTimer();
    TRAP_IGNORE( UnloadDisabledPluginsL( ) );
    
    CCHLOGSTRING( "CCCHServiceHandler::HandlePluginUnload OUT" );
    }

// -----------------------------------------------------------------------------
// CCCHServiceHandler::HandleDelayedNotifyEvent
// -----------------------------------------------------------------------------
//
void CCCHServiceHandler::HandleDelayedNotifyEvent()
    {
    CCHLOGSTRING2( "CCCHServiceHandler[0x%x]::HandleDelayedNotifyEvent IN",
                   this );
    
    CancelHandleNotifyDelayTimer();
    
    TServiceSelection selection;
    selection.iServiceId = iDelayedHandleNotifyServiceId;
    TInt index( ServiceExist( selection ) );

    TRAPD( err, 
        {
        if ( KErrNotFound != index )
            {
            iServices[ index ]->UpdateL( ETrue );
            }
        else
            {
            TCCHService service;
            iServer.SPSHandler().GetServiceInfoL( 
                iDelayedHandleNotifyServiceId, service );
            AddServiceL( service );
            } 
        } );

    //Check if service is already marked as enabled, and enable it
    if ( KErrNone == err )
        {
        index = ServiceExist( selection );
        if ( KErrNotFound != index )
            {
            if( iServices[ index ]->StartupFlagSet() && 
                ECCHEnabled != iServices[ index ]->GetState() )
                {
                iCancelNotify = EFalse;
                EnableService( selection, EFalse );
                iCancelNotify = ETrue;
                }
            }
        }
        
     // If service has removed, err must be other than none 
     // and we have to update all services
    if ( KErrNone != err )
        {
        TRAP_IGNORE( UpdateL( ETrue ) );
        }
                
    iDelayedHandleNotifyServiceId = 0;
    CCHLOGSTRING( "CCCHServiceHandler::HandleDelayedNotifyEvent OUT" );
    }

// ---------------------------------------------------------------------------
// CCCHServiceHandler::Exists
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CCCHServiceHandler::Exists( TCCHSubserviceType aType, 
                                  TCCHSubserviceState aState,
                                  TInt aError ) const
    {
    CCHLOGSTRING3( "CCCHServiceHandler::Exists(%d,%d)",aType,aState );
    TBool bReturn( EFalse );
    for( TInt i( 0 ); i < iServices.Count(); i++ )
        {
        for ( TInt j( 0 ); j < iServices[ i ]->SubserviceCount(); j++ )
            {
            TCCHSubserviceState state = ECCHUninitialized;
            TInt err = iServices[ i ]->GetStatus(iServices[ i ]->GetSubserviceType( j ), state );
            if( err == KErrNone )
                {
                if( aType == iServices[ i ]->GetSubserviceType( j ) && 
                    aState == state &&
                    aError == iServices[ i ]->GetError(iServices[ i ]->GetSubserviceType( j ) ) )
                    {
                    bReturn = ETrue;
                    break;
                    }
                }
            }
        }
    CCHLOGSTRING2( "CCCHServiceHandler::Exists returns: %d",bReturn );        
    return bReturn;
    }

// ---------------------------------------------------------------------------
// CCCHServiceHandler::LoadPluginsL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHServiceHandler::LoadPluginsL()
    {
    CCHLOGSTRING( "CCCHServiceHandler::LoadPluginsL: IN" );
    
    // Get plugin uids to array;
    RArray<TUid> uids;
    CleanupClosePushL( uids );
    
    TUid pluginUid( KNullUid );
        
    for( TInt i( 0 ); i < iServices.Count(); i++ )
        {     
        for ( TInt j( 0 ); j < iServices[ i ]->SubserviceCount(); j++ )
            {
            if ( iServices[ i ]->StartupFlagSet( j ) )
                {
                pluginUid = iServices[ i ]->GetUidL( j );
    
                if ( KErrNotFound == uids.Find( pluginUid ) && 
                    KNullUid != pluginUid )
                    {
                    uids.Append( pluginUid );

                    TRAP_IGNORE( 
                        iServer.PluginHandler().LoadPluginsL( pluginUid ) );
                    
                    // Set notifier to loaded Plug-ins                        
                    iServer.PluginHandler().SetServiceNotifier( this );                        
                    }
                // Enable subservice
                TServiceSelection selection;
                selection.iServiceId = iServices[ i ]->GetServiceId();
                selection.iType = iServices[ i ]->GetSubserviceType( j );
                EnableService( selection, EFalse );
                }           
            }
        }        
    CleanupStack::PopAndDestroy( &uids );    
    
    CCHLOGSTRING( "CCCHServiceHandler::LoadPluginsL: OUT" );
    }

// ---------------------------------------------------------------------------
// CCCHServiceHandler::UnloadDisabledPluginsL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHServiceHandler::UnloadDisabledPluginsL( )
    {
    CCHLOGSTRING( "CCCHServiceHandler::UnloadDisabledPluginsL: IN" );
    
    // Get plugin uids to array;
    RArray<TUid> uidsToBeUnloaded;
    RArray<TUid> uidsNotToBeUnloaded;
    CleanupClosePushL( uidsToBeUnloaded );
    CleanupClosePushL( uidsNotToBeUnloaded );
    
    
    // ...let's go through services & sub services
    for ( TInt ixSer=0; ixSer < iServices.Count(); ixSer++ )
        {
        for ( TInt ixSub=0; ixSub < iServices[ ixSer ]->SubserviceCount(); ixSub++ )
            {
            TUid pluginUid( KNullUid );
            TInt error( KErrNone );
            TRAP( error, pluginUid = iServices[ ixSer ]->GetUidL( ixSub ));
            // if subservice was found
            if( KErrNone == error )
                {
                
                TCCHSubserviceState state = ECCHUninitialized;
                
                // subservice was found, so don't need to check return value
                // coverity[check_return] coverity[unchecked_value]
                iServices[ ixSer ]->GetStatus(
                     iServices[ ixSer ]->GetSubserviceType( ixSub ), state );
                     
                // if non disabled subservice found, add it to non-unload arry                     
                if( ECCHDisabled != state )
                    {
                    uidsNotToBeUnloaded.Append( pluginUid );
                    }
                    
                else
                    {
                    if( KErrNotFound == uidsToBeUnloaded.Find( pluginUid ) )
                        {
                        // the others go to unload array
                        uidsToBeUnloaded.Append( pluginUid );        
                        }
                    }
                }
            }
        }
    iServer.PluginHandler().RemoveServiceNotifier( this );
    for( TInt i=0; i<uidsToBeUnloaded.Count(); i++ )
        {
        //unload plugin & set state if not found from non-unloadable array
        if( KErrNotFound == uidsNotToBeUnloaded.Find( uidsToBeUnloaded[i] ) )
            {
            CCHLOGSTRING2(
                "CCCHServiceHandler::UnloadDisabledPluginsL; unloading plugin UID:0x%x",
                    uidsToBeUnloaded[i] );
            iServer.PluginHandler().UnloadPlugin( uidsToBeUnloaded[i] );
            }
        }
    iServer.PluginHandler().SetServiceNotifier( this );
    CleanupStack::PopAndDestroy( &uidsNotToBeUnloaded );   
    CleanupStack::PopAndDestroy( &uidsToBeUnloaded );    
    CCHLOGSTRING( "CCCHServiceHandler::UnloadDisabledPluginsL: OUT" );
    }

// ---------------------------------------------------------------------------
// CCCHServiceHandler::EnableService
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CCCHServiceHandler::EnableService( 
    const TServiceSelection aServiceSelection, 
    const TBool aConnectivityCheck )
    {
    CCHLOGSTRING( "CCCHServiceHandler::EnableService: IN" );
    TInt error( ServiceExist( aServiceSelection ) );
    TBool connectionOk( EFalse );
    if ( KErrNotFound != error )
        {
        TInt index = error;
        TRAP( error, connectionOk = IsServiceConnectivityDefinedL( aServiceSelection ) );
        if( KErrNone == error )
            {
            if( connectionOk )
                {
                // Start monitoring connectivity changes
                iServer.ConnMonHandler().StartMonitoringConnectionChanges();
                
                if( iCancelNotify )
                    {
                    DisableNotifyChange();
                    }
         
                CCHLOGSTRING3( "CCCHServiceHandler::EnableService: service:%d, type:%d",
                        aServiceSelection.iServiceId, aServiceSelection.iType );
    
                // If client wants to enable VoIP subservice and VoIP are not supported
                // send KErrNotSupported notify to the client
                if ( ( ECCHUnknown == aServiceSelection.iType ||
                       ECCHVoIPSub == aServiceSelection.iType ) &&
                       iServices[ index ]->SubserviceExist( ECCHVoIPSub ) &&
                       !iServer.FeatureManager().VoIPSupported() )
                    {
                    CCHLOGSTRING( "CCCHServiceHandler::EnableService: No support for VoIP" );
                    TRAP_IGNORE( iServices[ index ]->SetErrorL( ECCHVoIPSub, KErrNotSupported ) );
                    TRAP_IGNORE( iServices[ index ]->SetStateL( ECCHVoIPSub, ECCHDisabled ) );
                    // Enable one by one all subservices except VoIP
                    if ( ECCHUnknown == aServiceSelection.iType )
                        {
                        for ( TInt i( 0 ); i < iServices[ index ]->SubserviceCount(); i++ )
                            {
                            if ( ECCHVoIPSub != iServices[ index ]->GetSubserviceType( i ) )
                                {
                                TRAP( error, iServices[ index ]->EnableL(
                                    iServices[ index ]->GetSubserviceType( i ),
                                        aConnectivityCheck, this ) );
                                }
                            }
                        }
                    }
                else
                    {
                    TRAP( error, iServices[ index ]->EnableL( 
                        aServiceSelection.iType, aConnectivityCheck, this ) );
                    }
                if( iCancelNotify )
                    {
                    EnableNotifyChange();
                    }
                if( HasWlanIap( aServiceSelection ) && KErrNone == error )
                    {
                    CCHLOGSTRING( "CCCHServiceHandler::EnableService: Launching WLAN scan..." );
                    TRAP_IGNORE( iWlanExtension->EnableWlanScanL() );
                    }
                if ( error )
                    {
                    RArray<TUint> iaps;
                    iServer.ConnMonHandler().StopMonitoringConnectionChanges( iaps );
                    iaps.Close();
                    }
                // There could be new Plug-ins after EnableL, so we must set 
                // notifier to loaded Plug-ins
                iServer.PluginHandler().SetServiceNotifier( this );
                }
            else
                {
                error = KCCHErrorAccessPointNotDefined;
                }
            CCHLOGSTRING2
                    ( "CCCHServiceHandler::EnableService: error: %d", error );
            }
        }
    CCHLOGSTRING( "CCCHServiceHandler::EnableService: OUT" );
    return error;
    }

// ---------------------------------------------------------------------------
// CCCHServiceHandler::DisableService
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CCCHServiceHandler::DisableService( 
    const TServiceSelection aServiceSelection )
    {
    CCHLOGSTRING( "CCCHServiceHandler::DisableService: IN" );
    TInt error( ServiceExist( aServiceSelection ) );
    
    if ( KErrNotFound != error )
        {
        TInt index( error );
        if( iCancelNotify )
            {
            DisableNotifyChange();
            }

        TRAP( error, iServices[ index ]->DisableL(
            aServiceSelection.iType ) );
        
        DisableWlanScan();
        if( iCancelNotify )
            {
            EnableNotifyChange();            
            }
            CCHLOGSTRING2
                ( "CCCHServiceHandler::DisableService: error: %d", error );
        }
    CCHLOGSTRING2(
        "CCCHServiceHandler::DisableService: return %d OUT", error );
    return error;
    }
    
// ---------------------------------------------------------------------------
// CCCHServiceHandler::DisableWlanScan
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHServiceHandler::DisableWlanScan( )
    {
    CCHLOGSTRING( "CCCHServiceHandler::DisableWlanScan: IN" );
    
    TBool disableWlanScan( ETrue );
    TCCHSubserviceState serviceState( ECCHUninitialized );
    TCCHSubserviceType serviceType( ECCHUnknown );
    TInt serviceId( 0 );
    
     // ...let's go through services & sub services
    for ( TInt ixSer = 0; ixSer < iServices.Count(); ixSer++ )
        {
        for ( TInt ixSub = 0; ixSub < iServices[ ixSer ]->SubserviceCount(); ixSub++ )
            {
            serviceType = iServices[ ixSer ]->GetSubserviceType( ixSub );
            TInt err = iServices[ ixSer ]->GetStatus( serviceType, serviceState );
            if( err == KErrNone )
                {
                // if found even one enabled/connecting service, do not deactivate scan
                if( ECCHEnabled == serviceState || ECCHConnecting == serviceState )
                    {
                    serviceId =  iServices[ ixSer ]->GetServiceId();
                    
                    TServiceConnectionInfo serviceConnInfo( serviceId, serviceType, 0, 0 );
                    GetConnectionInfo( serviceConnInfo );
                    // existing snap overrides iap
                    if( serviceConnInfo.iSNAPId != 0 )
                        {
                        TInt wlanIaps( 0 );
                        TInt allIaps( 0 );
                        TInt error( KErrNone );
                        TRAP( error, 
                            {
                            wlanIaps = iCommDbWatcher->GetIapCountFromSnap( serviceConnInfo.iSNAPId, ETrue, ETrue );
                            allIaps = iCommDbWatcher->GetIapCountFromSnap( serviceConnInfo.iSNAPId, EFalse );
                            } );
                        
                        // in case of possibility -> WLAN ALR we need to keep the scan active
                        // in case of possibility -> do not disable wlan scan if there is only one iap 
                        // and the iap is wlan
                        if( KErrNone == error )
                            {
                            if( 1 <= allIaps && 0 < wlanIaps )
                                {
                                disableWlanScan = EFalse;
                                break;
                                }
                            }
                        }
                    else if( serviceConnInfo.iIapId != 0 )
                        {
                        TInt error( KErrNone );
                        TBool isWlanIap( EFalse );
                        
                        TRAP( error, isWlanIap = iCommDbWatcher->IsWlanApL( serviceConnInfo.iIapId ) )
                        CCHLOGSTRING2( "CCCHServiceHandler::DisableWlanScan: error: %d", error );
                        
                        if( KErrNone == error)
                            {
                            //in case the iap is wlan type, scan is not deactivated
                            if( isWlanIap )
                                {
                                disableWlanScan = EFalse;
                                break;
                                }
                            }
                        }
                    }
                }
            }
        if( !disableWlanScan )
            {
            break;
            }
        }            
    if( disableWlanScan )
        {
        CCHLOGSTRING( "CCCHServiceHandler::DisableWlanScan disabling wlan scan..." );
        
        TRAP_IGNORE( iWlanExtension->DisableWlanScanL() ) ;
        }
    CCHLOGSTRING( "CCCHServiceHandler::DisableWlanScan: OUT" );
    }
    
// ---------------------------------------------------------------------------
// CCCHServiceHandler::SetConnectionInfo
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CCCHServiceHandler::SetConnectionInfo( 
    const TServiceConnectionInfo aServiceConnInfo )
    {
    CCHLOGSTRING( "CCCHServiceHandler::SetConnectionInfo: IN" );
    TInt error( KErrNotFound );
    CCHLOGSTRING2( "CCCHServiceHandler::SetConnectionInfo: serviceId %d",
        aServiceConnInfo.iServiceSelection.iServiceId );
    CCHLOGSTRING2( "CCCHServiceHandler::SetConnectionInfo: iType %d",
        aServiceConnInfo.iServiceSelection.iType );
    CCHLOGSTRING2( "CCCHServiceHandler::SetConnectionInfo: iSNAPId %d",
        aServiceConnInfo.iSNAPId );
    CCHLOGSTRING2( "CCCHServiceHandler::SetConnectionInfo: iIapId %d",
        aServiceConnInfo.iIapId );
    CCHLOGSTRING2( "CCCHServiceHandler::SetConnectionInfo: iReserved %d",
        aServiceConnInfo.iReserved );

    if ( KErrNotFound != 
        ( error = FindService( aServiceConnInfo.iServiceSelection.iServiceId ) ) )
        {
        TInt index( error );
        TRAP( error, iServices[ index ]->SetConnectionInfoL( 
            aServiceConnInfo ) );
        StartPluginUnloadTimer( );
        CCHLOGSTRING2
            ( "CCCHServiceHandler::SetConnectionInfo: error: %d", error );
        }
    CCHLOGSTRING( "CCCHServiceHandler::SetConnectionInfo: OUT" );
    return error;
    }
    
// ---------------------------------------------------------------------------
// CCCHServiceHandler::GetConnectionInfo
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CCCHServiceHandler::GetConnectionInfo( 
    TServiceConnectionInfo& aServiceConnInfo )
    {
    CCHLOGSTRING( "CCCHServiceHandler::GetConnectionInfo: IN" );
    TInt error( KErrNotFound );
    
    if ( KErrNotFound != 
        ( error = FindService( aServiceConnInfo.ServiceId() ) ) )
        {
        TInt index( error );
        TRAP( error, iServices[ index ]->GetConnectionInfoL( 
                aServiceConnInfo ) );
        StartPluginUnloadTimer( );
        CCHLOGSTRING2
            ( "CCCHServiceHandler::GetConnectionInfo: error: %d", error );
        }
    CCHLOGSTRING( "CCCHServiceHandler::GetConnectionInfo: OUT" );
    return error;
    }

// ---------------------------------------------------------------------------
// CCCHServiceHandler::UsesWlanIapL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CCCHServiceHandler::UsesWlanIap( 
    const TServiceSelection& aServiceSelection )
    {
    CCHLOGSTRING( "CCCHServiceHandler::UsesWlanIap: IN" );
    TBool isWlanIap( EFalse );
    TServiceConnectionInfo serviceConnInfo(
            aServiceSelection.iServiceId, aServiceSelection.iType, 0, 0 );
    CCHLOGSTRING3( "CCCHServiceHandler::UsesWlanIap: service:%d, type:%d",
            aServiceSelection.iServiceId, aServiceSelection.iType );
    
    TInt error = GetConnectionInfo( serviceConnInfo );
    
    if( KErrNone == error )
        {
        
        CCHLOGSTRING3( "CCCHServiceHandler::UsesWlanIap: snap:%d, snap:%d",
                serviceConnInfo.iSNAPId, serviceConnInfo.iIapId );
        if( 0 != serviceConnInfo.iSNAPId )
            {
            CCHLOGSTRING( "CCCHServiceHandler::UsesWlanIapL: SNAP in use" );
            TInt count( 0 );
            TRAP( error, count = iCommDbWatcher->GetIapCountFromSnap( serviceConnInfo.iSNAPId, ETrue ));
            if( KErrNone == error && 0 < count )
                {
                isWlanIap = ETrue;
                }
            }
        else if( 0 != serviceConnInfo.iIapId )
            {
            CCHLOGSTRING( "CCCHServiceHandler::UsesWlanIapL: IAP in use" );
           
            TRAP( error, isWlanIap = iCommDbWatcher->IsWlanApL( serviceConnInfo.iIapId ) );
            CCHLOGSTRING2( "CCCHServiceHandler::UsesWlanIap: error: %d", error );
            }


        }
    CCHLOGSTRING2( "CCCHServiceHandler::UsesWlanIap: OUT, value=%d", isWlanIap );
    return isWlanIap;
    }

// ---------------------------------------------------------------------------
// CCCHServiceHandler::IsConnectionDefined
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CCCHServiceHandler::IsConnectionDefinedL( 
    const TServiceSelection& aServiceSelection ) 
    {
    CCHLOGSTRING( "CCCHServiceHandler::IsConnectionDefined: IN" );
    TBool isOk( EFalse );
    __ASSERT_DEBUG( aServiceSelection.iType != ECCHUnknown, User::Panic( KNullDesC, KErrGeneral ) );
    TServiceConnectionInfo serviceConnInfo(
            aServiceSelection.iServiceId, aServiceSelection.iType, 0, 0 );
    CCHLOGSTRING3( "CCCHServiceHandler::IsConnectionDefined: service:%d, type:%d",
            aServiceSelection.iServiceId, aServiceSelection.iType );
    
    TInt error = GetConnectionInfo( serviceConnInfo );
    User::LeaveIfError( error );
    
    CCHLOGSTRING3( "CCCHServiceHandler::IsConnectionDefined: snap:%d, iap:%d",
            serviceConnInfo.iSNAPId, serviceConnInfo.iIapId );
    if( 0 != serviceConnInfo.iSNAPId )
        {
        TInt count( 0 );
        count = iCommDbWatcher->GetIapCountFromSnap( serviceConnInfo.iSNAPId, EFalse, ETrue );
        CCHLOGSTRING( "CCCHServiceHandler::IsConnectionDefined: SNAP in use" );
        if( KErrNone == error && 0 < count )
            {
            isOk = ETrue;
            }
        }
    else if( 0 != serviceConnInfo.iIapId )
        {
        CCHLOGSTRING( "CCCHServiceHandler::IsConnectionDefined: IAP in use" );
        isOk = ETrue;
        }

    CCHLOGSTRING2( "CCCHServiceHandler::IsConnectionDefined: OUT, value=%d", isOk );
    return isOk;
    }

// ---------------------------------------------------------------------------
// CCCHServiceHandler::IsServiceConnectivityDefined
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CCCHServiceHandler::IsServiceConnectivityDefinedL( 
    const TServiceSelection& aServiceSelection ) 
    {
    CCHLOGSTRING( "CCCHServiceHandler::IsServiceConnectivityDefined: IN" );
    
    TBool isOk( EFalse );
    TInt error( KErrNone );
    TInt index = FindService( aServiceSelection.iServiceId );
    if ( KErrNotFound != index )
        {
        if( ECCHUnknown == aServiceSelection.iType )
            {
            for( TInt i = 0; i < iServices[ index ]->SubserviceCount(); i++ )
                {
                TRAP( error, isOk = IsConnectionDefinedL( 
                        TServiceSelection( aServiceSelection.iServiceId, 
                        iServices[ index ]->GetSubserviceType( i ) ) ) ) ;
                if( isOk )
                    {
                    error = KErrNone;
                    break;
                    }
                }
            }
        else
            {
            TRAP( error, isOk = IsConnectionDefinedL( aServiceSelection ) );
            }    
        }
    CCHLOGSTRING3( 
        "CCCHServiceHandler::IsServiceConnectivityDefined: OUT, value=%d, error=%d", isOk, error );
    User::LeaveIfError( error );
    return isOk;
    }


// ---------------------------------------------------------------------------
// CCCHServiceHandler::HasWlanIap
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CCCHServiceHandler::HasWlanIap( 
    const TServiceSelection& aServiceSelection )
    {
    CCHLOGSTRING( "CCCHServiceHandler::HasWlanIap: IN" );
    
    TBool isWlanIap( EFalse );
    TInt index = FindService( aServiceSelection.iServiceId );
    if ( KErrNotFound != index )
        {
        if( ECCHUnknown == aServiceSelection.iType )
            {
            for( TInt i = 0; i < iServices[ index ]->SubserviceCount(); i++ )
                {
                isWlanIap = UsesWlanIap( 
                        TServiceSelection( aServiceSelection.iServiceId, 
                        iServices[ index ]->GetSubserviceType( i ) ) ) ;
                if( isWlanIap )
                    {
                    break;
                    }
                }
            }
        else
            {
            isWlanIap = UsesWlanIap( aServiceSelection );
            }    
        StartPluginUnloadTimer( );
        }
    CCHLOGSTRING2( "CCCHServiceHandler::HasWlanIap: OUT, value=%d", isWlanIap );
    return isWlanIap;
    }

// ---------------------------------------------------------------------------
// CCCHServiceHandler::ServiceCountL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHServiceHandler::ServiceCountL( RMessage2 aMessage ) const
    {
    CCHLOGSTRING( "CCCHServiceHandler::ServiceCountL: IN" );
    
    TPckgBuf<TUint32> pckgServiceCount;
    aMessage.ReadL( 0, pckgServiceCount );
    TCCHSubserviceType type( 
        static_cast<TCCHSubserviceType>( aMessage.Int1() ) );
    
    if ( ECCHUnknown == type )
        {
        // Count of services
        pckgServiceCount() = iServer.SPSHandler().GetServicesCountL();
        // Service count back to the client
        aMessage.WriteL( 0, pckgServiceCount );
        }
    else
        {
        // Count of services which contains subservices
        pckgServiceCount() = ServiceCount( type );
        // Service count back to the client
        aMessage.WriteL( 0, pckgServiceCount );
        }        
    
    CCHLOGSTRING( "CCCHServiceHandler::ServiceCountL: OUT" );
    }

// ---------------------------------------------------------------------------
// CCCHServiceHandler::GetServicesL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHServiceHandler::GetServicesL( RMessage2 aMessage )
    {
    CCHLOGSTRING( "CCCHServiceHandler::GetServicesL: IN" );
    // outstanding request for service, but SpSettings notify timer still running
    if( iHandleNotifyDelayTimer->IsActive() )
        {
        CCHLOGSTRING( "CCCHServiceHandler::GetServicesL - Forced handling of notify" );
        iHandleNotifyDelayTimer->Cancel();
        HandleDelayedNotifyEvent();
        }
    TInt count =  iServices.Count();
    TInt index( KErrNotFound );
    if ( count )
        {
        CCHLOGSTRING2( "CCCHServiceHandler::GetServicesL service count:%d ", count );
        TUint32 serviceId = aMessage.Int0();
        TCCHSubserviceType type ( ECCHUnknown );
        type = static_cast<TCCHSubserviceType>( aMessage.Int1() );
        
        if ( KErrNotFound == aMessage.Int0() )
            {
            serviceId = KErrNone;
            }
        else if ( serviceId != KErrNone )
            {
            count = 1;
            }
        
        
        CArrayFixFlat<TCCHService>* serviceArray = 
            new( ELeave )CArrayFixFlat<TCCHService>( count );
        CleanupStack::PushL( serviceArray );
        
        TCCHService service;
        // Get all services
        if ( KErrNone == serviceId )
            {
            CCHLOGSTRING( "CCCHServiceHandler::GetServicesL KErrNone == serviceId" );
            if ( type == ECCHUnknown )
                {
                for ( TInt i( 0 ); i < count; i++ )
                    {
                    iServices[ i ]->FillServiceInfo( service );
                    serviceArray->AppendL( service );
                    }    
                }
            else
                {
                for ( TInt i( 0 ); i < count; i++ )
                    {
                    if ( iServices[ i ]->SubserviceExist( type ) )
                        {
                        iServices[ i ]->FillServiceInfo( service );
                        serviceArray->AppendL( service );
                        }                    
                    }
                }
            }
        // Get specified service
        else 
            {
            if ( type == ECCHUnknown )
                {
                index = FindService( serviceId );
                CCHLOGSTRING2( "CCCHServiceHandler::GetServicesL index:%d", index );

                if ( KErrNotFound != index )
                    {
                    iServices[ index ]->FillServiceInfo( service );
                    serviceArray->AppendL( service );
                    }
                }
            else
                {
            CCHLOGSTRING( "CCCHServiceHandler::GetServicesL Else" );
                TServiceSelection selection( serviceId, type ); 
                index = ServiceExist( selection );
                if ( KErrNotFound != index )
                    {
                    iServices[ index ]->FillServiceInfo( service );
                    serviceArray->AppendL( service );
                    }
                }
            }
        
        TUint32 length = serviceArray->Count() * sizeof( TCCHService );
        
        if ( length )
            {
            TPtrC8 servicePtr;
            servicePtr.Set( 
                reinterpret_cast<TText8*>( &( *serviceArray )[ 0 ] ), 
                    length );
            aMessage.WriteL( 2, servicePtr );                     
            }
        else
            {
            User::Leave( KErrNotFound );
            }            
            
        CleanupStack::PopAndDestroy( serviceArray );
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    CCHLOGSTRING( "CCCHServiceHandler::GetServicesL: OUT" );
    }

// ---------------------------------------------------------------------------
// CCCHServiceHandler::GetServiceState
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//    
TInt CCCHServiceHandler::GetServiceState( const RMessage2 aMessage )
    {
    CCHLOGSTRING( "CCCHServiceHandler::GetServiceStateL: IN" );
        
    TUint32 serviceId( 0 );
    TCCHSubserviceType type ( ECCHUnknown );
    TPckgBuf<TCCHSubserviceState> status;
    serviceId = aMessage.Int0();
    type = static_cast<TCCHSubserviceType>( aMessage.Int1() );
    
    TInt error( FindService( serviceId ) );
    TInt errorState = KErrNone;
    
    if ( KErrNotFound != error )
        {
        TInt index( error );
        TRAP( error, 
            {
            aMessage.ReadL( 2, status );
            errorState = iServices[ index ]->GetStatus( type, status() );
            if( errorState == KErrNone )
                {
                aMessage.WriteL( 2, status );
                }
            } );
        if( KErrNone == error )
            {
            if( errorState == KErrNone )
                {
                error = iServices[ index ]->GetError( type );
                }
            else
                {
                error = errorState;
                }
            }
        StartPluginUnloadTimer( );
        }
    
    CCHLOGSTRING2( 
        "CCCHServiceHandler::GetServiceStateL: return %d OUT", error );
    return error;
    }

// ---------------------------------------------------------------------------
// CCCHServiceHandler::GetServiceState
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//    
TCCHSubserviceState CCCHServiceHandler::ServiceState(
        const TUint aServiceId )
    {
    CCHLOGSTRING( "CCCHServiceHandler::ServiceState: IN" );
    
    TCCHSubserviceState state( ECCHUninitialized );
    TInt error( FindService( aServiceId ) );
    
    if ( KErrNotFound != error )
        {
        state = iServices[ error ]->GetState();
        }
    
    CCHLOGSTRING2( 
        "CCCHServiceHandler::ServiceState: return %d OUT", state );
    return state;
    }

// ---------------------------------------------------------------------------
// CCCHServiceHandler::GetServiceInfo
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//    
TInt CCCHServiceHandler::GetServiceInfo( const RMessage2 aMessage )
    {
    CCHLOGSTRING( "CCCHServiceHandler::GetServiceInfo: IN" );
    TBuf<KCCHMaxProtocolNameLength> buffer;

    TPckgBuf<TServiceConnectionInfo> serviceConnInfoPckg;

    TInt error = KErrNone;
    TRAP( error, 
        { 
        aMessage.ReadL( 0, serviceConnInfoPckg );
        } );
        
    if( error ) 
        {
        return error;
        }
   
    TServiceConnectionInfo conninfo;
     
    conninfo.iServiceSelection.iServiceId = serviceConnInfoPckg().iServiceSelection.iServiceId;
    conninfo.iServiceSelection.iType      = serviceConnInfoPckg().iServiceSelection.iType;
    conninfo.iServiceSelection.iParameter = serviceConnInfoPckg().iServiceSelection.iParameter;
  
    error = ServiceExist( conninfo.iServiceSelection );
    
    if ( KErrNotFound != error )
        {
        TInt index( error );

        if( conninfo.iServiceSelection.iParameter == ECchServiceInfo )
            {
            TRAP( error, 
                {
                iServices[ index ]->GetServiceInfoL( 
                    buffer, conninfo.iServiceSelection.iType );
                aMessage.WriteL( 1, buffer );
                } );
            }
        StartPluginUnloadTimer( );
        }
    CCHLOGSTRING2( "CCCHServiceHandler::GetServiceInfo: return %d OUT", error );
    return error;
    }

// ---------------------------------------------------------------------------
// CCCHServiceHandler::ReserveService
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//    
TInt CCCHServiceHandler::ReserveService( const RMessage2 aMessage ) 
    {
    CCHLOGSTRING( "CCCHServiceHandler::ReserveService: IN" );
    
    TServiceSelection serviceSelection(aMessage.Int0(), 
        static_cast<TCCHSubserviceType>( aMessage.Int1() ) );
    
    TInt error( ServiceExist( serviceSelection ) );
    
    if ( KErrNotFound != error )
        {
        TInt index( error );
        TRAP( error, 
            {
            iServices[ index ]->ReserveServiceL( 
                serviceSelection.iType );
            } );
        }
    CCHLOGSTRING2( "CCCHServiceHandler::ReserveService: return %d OUT", error );
    return error;
    }

// ---------------------------------------------------------------------------
// CCCHServiceHandler::FreeService
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//    
TInt CCCHServiceHandler::FreeService( const RMessage2& aMessage )
    {
    CCHLOGSTRING( "CCCHServiceHandler::FreeService: IN" );
    
    TServiceSelection serviceSelection(aMessage.Int0(), 
        static_cast<TCCHSubserviceType>( aMessage.Int1() ) );
    
    TInt error( ServiceExist( serviceSelection ) );
    
    if ( KErrNotFound != error )
        {
        TInt index( error );
        TRAP( error, 
            {
            iServices[ index ]->FreeServiceL( 
                serviceSelection.iType );
            } );
        }
    CCHLOGSTRING2( "CCCHServiceHandler::FreeService: return %d OUT", error );        
    return error;
    }

// ---------------------------------------------------------------------------
// CCCHServiceHandler::IsReserved
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHServiceHandler::IsReserved( RMessage2 aMessage ) const
    {
    CCHLOGSTRING( "CCCHServiceHandler::IsReserved: IN" );
    
    
    TServiceSelection serviceSelection(aMessage.Int0(),
        static_cast<TCCHSubserviceType>( aMessage.Int1() ) );
    TInt error( KErrNone );
            
    TPckgBuf<TBool> pckgIsReserved;
    TRAP( error, aMessage.ReadL( 2, pckgIsReserved ) );
    
    error = ServiceExist( serviceSelection );
    
    if ( KErrNotFound != error )
        {
        TInt index( error );
        TRAP( error, 
            {
            pckgIsReserved() = iServices[ index ]->IsReservedL( 
                serviceSelection.iType );
            } );
        }
        
    
    if ( KErrNotFound != error )
        {
        // Service's reference count back to the client
        TRAP( error, aMessage.WriteL( 2, pckgIsReserved ) );
        }
     
    
    CCHLOGSTRING( "CCCHServiceHandler::IsReserved: OUT" );
    }
    
// ---------------------------------------------------------------------------
// CCCHServiceHandler::ServiceExist
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CCCHServiceHandler::ServiceExist( TServiceSelection aSelection ) const
    {
    CCHLOGSTRING( "CCCHServiceHandler::ServiceExist: IN" );
    TBool exist( EFalse );
    TInt index( FindService( aSelection.iServiceId ) );
    
    if ( KErrNotFound != index )
        {
        exist = iServices[ index ]->SubserviceExist( aSelection.iType );
        }    
    CCHLOGSTRING( "CCCHServiceHandler::ServiceExist: OUT" );
    return !exist ? KErrNotFound : index;
    }

// ---------------------------------------------------------------------------
// CCCHServiceHandler::FindService
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CCCHServiceHandler::FindService( TUint32 aServiceId ) const
    {
    TBool exist( EFalse );
    TInt index( KErrNotFound );
    CCHLOGSTRING2( "CCCHServiceHandler::FindService: serviceId %d",
        aServiceId );
    for ( TInt i( 0 ); i < iServices.Count() && !exist; i++ )
        {
        exist = iServices[ i ]->GetServiceId() == aServiceId;
        index = i;
        }
    return !exist ? KErrNotFound : index;
    }
// ---------------------------------------------------------------------------
// CCCHServiceHandler::HandleNotifyChange
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHServiceHandler::HandleNotifyChange( TServiceId aServiceId )
    {
    CCHLOGSTRING( "CCCHServiceHandler::HandleNotifyChange IN" );
    
    CancelHandleNotifyDelayTimer();
    StartHandleNotifyDelayTimer();
    iDelayedHandleNotifyServiceId = aServiceId;
    }

// ---------------------------------------------------------------------------
// CCCHServiceHandler::EnableNotifyChange
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHServiceHandler::EnableNotifyChange()
    {
    TRACE_ENTRY_POINT;
    // Subscribe to service change notifies    
    TRAP_IGNORE( iNotifier->NotifyChangeL( iServiceIds ) );
    TRACE_EXIT_POINT;
    }
// ---------------------------------------------------------------------------
// CCCHServiceHandler::DisableNotifyChange
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHServiceHandler::DisableNotifyChange()
    {
    TRACE_ENTRY_POINT;
    iNotifier->Cancel();
    TRACE_EXIT_POINT;
    }
// ---------------------------------------------------------------------------
// CCCHServiceHandler::ServiceCount
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CCCHServiceHandler::ServiceCount( const TCCHSubserviceType aType ) const
    {
    CCHLOGSTRING2( "CCCHServiceHandler::ServiceCount: type: %d",aType );
    
    TInt count( 0 );
    
    for ( TInt i( 0 ); i < iServices.Count(); i++ )
        {
        if ( KErrNotFound != iServices[ i ]->FindSubservice( aType ) )
            {
            count++;    
            }
        }
    
    CCHLOGSTRING2( "CCCHServiceHandler::ServiceCount: count: %d", count );
    
    return count;
    }

// ---------------------------------------------------------------------------
// CCCHServiceHandler::HandleError
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHServiceHandler::HandleError( TInt /*aError*/ )
    {
    
    }

// ---------------------------------------------------------------------------
// CCCHServiceHandler::HandleWLANIapAdded
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHServiceHandler::HandleWLANIapAdded( TInt aSNAPId )
    {
    CCHLOGSTRING( "CCCHServiceHandler::HandleWLANIapAdded: IN" );
    TBool startScan( EFalse );
    TCCHSubserviceState serviceState( ECCHUninitialized );
    TCCHSubserviceType serviceType( ECCHUnknown );
        
    // go through all services & subservices
    for( TInt i = 0; i < iServices.Count(); i++ )
        {
        for ( TInt j = 0; j < iServices[ i ]->SubserviceCount(); j++ )
            {
            serviceType = iServices[ i ]->GetSubserviceType( j );
            TInt error = iServices[ i ]->GetStatus( serviceType, serviceState );
            
            if( KErrNone == error )
                {
                // if we have a connecting or enabled service 
                if( !iServer.FeatureManager().OfflineMode() &&
                    ( ECCHConnecting == serviceState || ECCHEnabled == serviceState ) )
                    {
                    TUint serviceId = iServices[ i ]->GetServiceId();
                    TServiceConnectionInfo serviceConnInfo( serviceId, serviceType, 0, 0 );
                    GetConnectionInfo( serviceConnInfo );
                    
                    // with a snap with new wlan iap, we start wlan scan
                    if( aSNAPId == serviceConnInfo.iSNAPId && 0 != aSNAPId )
                        {
                        startScan = ETrue;                    
                        }
                    }
                }
            }
        }
    if( startScan )        
        {
        CCHLOGSTRING( "CCCHServiceHandler::HandleWLANIapAdded; starting wlan scan" );
        TRAP_IGNORE( iWlanExtension->ForceEnableWlanScanL() );
        iServer.ConnMonHandler().ScanNetworks( ETrue );
        }
	CCHLOGSTRING( "CCCHServiceHandler::HandleWLANIapAdded: OUT" );
	}

// ---------------------------------------------------------------------------
// CCCHServiceHandler::GetSubserviceTypesL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHServiceHandler::GetSubserviceTypesL( 
    const TUint aServiceId, 
    RArray<TCCHSubserviceType>& aSubserviceTypes ) const
    {
    TInt index( FindService( aServiceId ) );
    if ( KErrNotFound != index )
        {
        aSubserviceTypes.Reset();
        for ( TInt i( 0 ); i < iServices[ index ]->SubserviceCount(); i++ )
            {
            CCHLOGSTRING2( "    Service contains subservice: %d", 
                iServices[ index ]->GetSubserviceType( i ) );
            
            aSubserviceTypes.Append( 
                    iServices[ index ]->GetSubserviceType( i ) );
            }
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    }
    
// ---------------------------------------------------------------------------
// CCCHServiceHandler::SetStartupFlag
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CCCHServiceHandler::SetStartupFlag(
    TServiceSelection aSelection,
    TBool aLoadAtStartUp ) const
    {
    TInt exist( FindService( aSelection.iServiceId ) );
    if ( KErrNotFound != exist )
        {
        exist = iServices[ exist ]->SetStartupFlag( 
            aSelection.iType, aLoadAtStartUp ); 
        }
    return exist;
    }

// ---------------------------------------------------------------------------
// CCCHServiceHandler::HandleRestartL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHServiceHandler::HandleRestartL()
    {
    // In restart situation all services and subservices goes to 
    // disabled state, after restart we will make reregistration 
    CCHLOGSTRING( "CCHServiceHandler::HandleRestartL() IN" );
    for ( TInt i( 0 ); i < iServices.Count(); i++ )
        {
        for ( TInt j( 0 ); j < iServices[ i ]->SubserviceCount(); j++ )
            {
            iServices[ i ]->GetSubserviceL( 
                iServices[ i ]->GetSubserviceType( j ) ).SetError( KErrCancel );
            iServices[ i ]->GetSubserviceL( 
                iServices[ i ]->GetSubserviceType( j ) ).SetState( ECCHDisabled );
            iServices[ i ]->GetSubserviceL( 
                iServices[ i ]->GetSubserviceType( j ) ).StatusChanged();
            }
        }
    CCHLOGSTRING( "CCHServiceHandler::HandleRestartL() OUT" );
    }

// ========================== OTHER EXPORTED FUNCTIONS =======================

//  End of File
