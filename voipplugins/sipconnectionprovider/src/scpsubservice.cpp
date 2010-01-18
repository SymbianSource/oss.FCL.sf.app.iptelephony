/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "scpsubservice.h"
#include "scpservice.h"
#include "scplogger.h"
#include "scputility.h"
#include "scpservicehandlerbase.h"
#include "scpprofilehandler.h"
#include "scpservicestorage.h"
#include "scpstatecontainer.h"
#include "scpvoiphandler.h"
#include "scpvmbxhandler.h"
#include "scppresencehandler.h"
#include "scpimhandler.h"
#include "scpsubserviceobserver.h"

// -----------------------------------------------------------------------------
// CScpSubService::NewL
// -----------------------------------------------------------------------------
//
CScpSubService* CScpSubService::NewL( TInt aId, 
                                      TInt aSubServiceId,
                                      TCCHSubserviceType aSubServiceType,
                                      CScpService& aService )
    {
    SCPLOGSTRING( "CScpSubService::CScpSubService" );

    CScpSubService* self = new ( ELeave ) CScpSubService( aId, 
                                                          aSubServiceId,
                                                          aSubServiceType,
                                                          aService );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CScpSubService::ConstructL
// -----------------------------------------------------------------------------
//
void CScpSubService::ConstructL()
    {
    SCPLOGSTRING2( "CScpSubService[0x%x]::ConstructL", this );

    switch( iSubServiceType )
        {
        case ECCHVoIPSub:
            iServiceHandler = CScpVoipHandler::NewL( *this );
            SetPresenceToObserveVoIP();
            break;
 
        case ECCHVMBxSub:
            iServiceHandler = CScpVmbxHandler::NewL( *this );  
            break;

        case ECCHIMSub:
            iServiceHandler = CScpImHandler::NewL( *this );
            break;

        case ECCHPresenceSub:
            iServiceHandler = CScpPresenceHandler::NewL( *this );
            break;

        default:
            __ASSERT_DEBUG( EFalse, User::Panic( KNullDesC, KErrGeneral ) );
            User::Leave( KErrNotFound );
            break;
        }
    }

// -----------------------------------------------------------------------------
// CScpSubService::CScpSubService
// -----------------------------------------------------------------------------
//
CScpSubService::CScpSubService( TInt aId, 
                                TInt aSubServiceId,
                                TCCHSubserviceType aSubServiceType,
                                CScpService& aService ) :
    iId( aId ),
    iSubServiceId( aSubServiceId ),
    iSubServiceType( aSubServiceType ),
    iService( aService ),
    iSipProfileId( 0 ),
    iEnableRequestedState( EScpNoRequest ),
    iSubServiceDisconnected( EFalse ),
    iApId( 0 ),
    iSnapId( 0 ),
    iLastReportedError( KErrNone ),
    iReserved( EFalse ),
    iSubServiceState( TScpStateContainer::Instance( ECCHDisabled ) )
    {
    SCPLOGSTRING3( "CScpSubService[0x%x]::CScpSubService type: %d", 
                   this, aSubServiceType );
    }

// -----------------------------------------------------------------------------
// CScpSubService::~CScpSubService
// -----------------------------------------------------------------------------
//
CScpSubService::~CScpSubService()
    {
    SCPLOGSTRING2( "CScpSubService[0x%x]::~CScpSubService", this );

    delete iServiceHandler;
    }

// -----------------------------------------------------------------------------
// CScpSubService::EnableL
// -----------------------------------------------------------------------------
//
void CScpSubService::EnableL()
    {
    SCPLOGSTRING2( "CScpSubService[0x%x]::EnableL", this );

    iSubServiceState->EnableL( *this );
    EnableOccured();
    }

// -----------------------------------------------------------------------------
// CScpSubService::EnableOccured
// -----------------------------------------------------------------------------
//
void CScpSubService::EnableOccured()
    {
    switch ( iSubServiceType )
        {
        case ECCHPresenceSub:
            {
            iEnableCounter++;
            }
            break;
            
        case ECCHVoIPSub:
        case ECCHIMSub:
        case ECCHVMBxSub:
        case ECCHUnknown:
        default:
            break;
        }
        
    SCPLOGSTRING3( "CScpSubService[0x%x]::EnableOccured : %d", this, iEnableCounter );
    }

// -----------------------------------------------------------------------------
// CScpSubService::EnableCounter
// -----------------------------------------------------------------------------
//
TInt CScpSubService::EnableCounter()
    {
    SCPLOGSTRING3( "CScpSubService[0x%x]::EnableCounter : %d", this, iEnableCounter );
    return iEnableCounter;
    }

// -----------------------------------------------------------------------------
// CScpSubService::ResetEnableCounter
// -----------------------------------------------------------------------------
//
void CScpSubService::ResetEnableCounter()
    {
    SCPLOGSTRING2( "CScpSubService[0x%x]::ResetEnableCounter", this );
    iEnableCounter = 0;
    }

// -----------------------------------------------------------------------------
// CScpSubService::Disable
// -----------------------------------------------------------------------------
//
TInt CScpSubService::Disable()
    {
    SCPLOGSTRING2( "CScpSubService[0x%x]::Disable", this );

    return iSubServiceState->Disable( *this );
    }

// -----------------------------------------------------------------------------
// CScpSubService::Id
// -----------------------------------------------------------------------------
//
TInt CScpSubService::Id() const
    {
    SCPLOGSTRING2( "CScpSubService[0x%x]::Id", this );

    return iId;
    }

// -----------------------------------------------------------------------------
// CScpSubService::SubServiceId
// -----------------------------------------------------------------------------
//
TInt CScpSubService::SubServiceId() const
    {
    SCPLOGSTRING2( "CScpSubService[0x%x]::SubServiceId", this );

    return iSubServiceId;
    }

// -----------------------------------------------------------------------------
// CScpSubService::ServiceStorage
// -----------------------------------------------------------------------------
//
CScpServiceStorage& CScpSubService::ServiceStorage() const
    {
    SCPLOGSTRING2( "CScpSubService[0x%x]::ServiceStorage", this );

    return iService.ServiceStorage();
    }

// -----------------------------------------------------------------------------
// CScpSubService::ProfileHandler
// -----------------------------------------------------------------------------
//
CScpProfileHandler& CScpSubService::ProfileHandler() const
    {
    SCPLOGSTRING2( "CScpSubService[0x%x]::ProfileHandler", this );

    return iService.ProfileHandler();
    }

// -----------------------------------------------------------------------------
// CScpSubService::State
// -----------------------------------------------------------------------------
//
TCCHSubserviceState CScpSubService::State() const
    {
    SCPLOGSTRING2( "CScpSubService[0x%x]::State: %d", this );

    return iSubServiceState->State();
    }

// -----------------------------------------------------------------------------
// CScpSubService::SubServiceType
// -----------------------------------------------------------------------------
//
TCCHSubserviceType CScpSubService::SubServiceType() const
    {
    SCPLOGSTRING2( "CScpSubService[0x%x]::Type", this );

    return iSubServiceType;
    }

// -----------------------------------------------------------------------------
// CScpSubService::SetSipProfileId
// -----------------------------------------------------------------------------
//
void CScpSubService::SetSipProfileId( TInt aSipProfileId )
    {
    SCPLOGSTRING3( "CScpSubService[0x%x]::SetSipProfileId profile id", 
                   this, aSipProfileId );

    iSipProfileId = aSipProfileId;
    }

// -----------------------------------------------------------------------------
// CScpSubService::SipProfileId
// -----------------------------------------------------------------------------
//
TInt CScpSubService::SipProfileId() const
    {
    SCPLOGSTRING2( "CScpSubService[0x%x]::SipProfileId", this );

    return iSipProfileId;
    }

// -----------------------------------------------------------------------------
// CScpSubService::EnableRequestedState
// -----------------------------------------------------------------------------
//
void CScpSubService::SetEnableRequestedState( TEnableState aEnableRequestedState )
    {
    SCPLOGSTRING2( "CScpSubService[0x%x]::SetEnableRequestedState", this );

    iEnableRequestedState = aEnableRequestedState;
    }

// -----------------------------------------------------------------------------
// CScpSubService::EnableRequestedState
// -----------------------------------------------------------------------------
//
CScpSubService::TEnableState CScpSubService::EnableRequestedState() const
    {
    SCPLOGSTRING2( "CScpSubService[0x%x]::EnableRequestedState", this );

    return iEnableRequestedState;
    }

// -----------------------------------------------------------------------------
// CScpSubService::SetSubServiceDisconnected
// -----------------------------------------------------------------------------
//
void CScpSubService::SetSubServiceDisconnected( TBool aSubServiceDisconnected )
    {
    SCPLOGSTRING3( "CScpSubService[0x%x]::SetSubServiceDisconnected: %d", this, 
                   aSubServiceDisconnected );

    iSubServiceDisconnected = aSubServiceDisconnected;
    }

// -----------------------------------------------------------------------------
// CScpSubService::SubServiceDisconnected
// -----------------------------------------------------------------------------
//
TBool CScpSubService::SubServiceDisconnected() const
    {
    SCPLOGSTRING3( "CScpSubService[0x%x]::SubServiceDisconnected: %d", this, 
                   iSubServiceDisconnected );

    return iSubServiceDisconnected;
    }

// -----------------------------------------------------------------------------
// CScpSubService::SetApId
// -----------------------------------------------------------------------------
//
void CScpSubService::SetApId( TUint32 aApId )
    {
    SCPLOGSTRING3( "CScpSubService[0x%x]::SetApId ap id: %d", this, aApId );

    iApId = aApId;
    }

// -----------------------------------------------------------------------------
// CScpSubService::ApId
// -----------------------------------------------------------------------------
//
TUint32 CScpSubService::ApId() const
    {
    SCPLOGSTRING2( "CScpSubService[0x%x]::ApId", this );

    return iApId;
    }

// -----------------------------------------------------------------------------
// CScpSubService::SetSnapId
// -----------------------------------------------------------------------------
//
void CScpSubService::SetSnapId( TUint32 aSnapId )
    {
    SCPLOGSTRING3( "CScpSubService[0x%x]::SetSnapId snap id: %d", this, aSnapId );

    iSnapId = aSnapId;
    }

// -----------------------------------------------------------------------------
// CScpSubService::SnapId
// -----------------------------------------------------------------------------
//
TUint32 CScpSubService::SnapId() const
    {
    SCPLOGSTRING2( "CScpSubService[0x%x]::SnapId", this );

    return iSnapId;
    }

// -----------------------------------------------------------------------------
// CScpSubService::GetIapType
// -----------------------------------------------------------------------------
//
TScpIapType CScpSubService::GetIapType() const
    {
    SCPLOGSTRING2( "CScpSubService[0x%x]::GetIapType", this );

    TScpIapType type = EScpUnknownType;

    // Check that the sip profile is not using WLAN iap
    CScpProfileHandler& profileHandler = iService.ProfileHandler();

    CScpSipConnection* sipConnection = NULL;

    if( profileHandler.SipConnectionExists( iSipProfileId ) )
        {
        sipConnection = profileHandler.GetSipConnection( iSipProfileId );
        }
    else
        {
        TRAP_IGNORE( sipConnection = 
                     profileHandler.CreateSipConnectionL( iSipProfileId ) );
        }

    __ASSERT_DEBUG( sipConnection, User::Panic( KNullDesC, KErrGeneral ) );

    if( sipConnection )
        {
        TRAP_IGNORE( type = sipConnection->GetIapTypeL() );
        }

    return type;
    }      
            
// -----------------------------------------------------------------------------
// CScpSubService::ServiceHandler
// -----------------------------------------------------------------------------
//
CScpServiceHandlerBase& CScpSubService::ServiceHandler() const
    {
    SCPLOGSTRING2( "CScpSubService[0x%x]::ServiceHandler", this );

    return *iServiceHandler;
    }

// -----------------------------------------------------------------------------
// CScpSubService::LastReportedError
// -----------------------------------------------------------------------------
//
TInt CScpSubService::LastReportedError() const
    {
    SCPLOGSTRING3( "CScpSubService[0x%x]::LastReportedError: %d", 
                   this, iLastReportedError );

    return iLastReportedError;
    }

// -----------------------------------------------------------------------------
// CScpSubService::SetLastReportedError
// -----------------------------------------------------------------------------
//
void CScpSubService::SetLastReportedError(
    const TInt aError )
    {
    SCPLOGSTRING3( "CScpSubService[0x%x]::SetLastReportedError: %d", 
                   this, aError );

    iLastReportedError = aError;
    }

// -----------------------------------------------------------------------------
// CScpSubService::SetReserved
// -----------------------------------------------------------------------------
//
void CScpSubService::SetReserved( TBool aReserved )
    {
    SCPLOGSTRING3( "CScpSubService[0x%x]::SetReserved: %d", this, aReserved );

    iReserved = aReserved;

    CScpProfileHandler& profileHandler = iService.ProfileHandler();
    profileHandler.SetSipProfileReserved( iSipProfileId, iReserved );
    }

// -----------------------------------------------------------------------------
// CScpSubService::IsReserved
// -----------------------------------------------------------------------------
//
TBool CScpSubService::IsReserved() const
    {
    SCPLOGSTRING3( "CScpSubService[0x%x]::IsReserved: %d", this, iReserved );

    return iReserved;
    }

// -----------------------------------------------------------------------------
// CScpSubService::IsRoaming
// -----------------------------------------------------------------------------
//
TBool CScpSubService::IsRoaming() const
    {
    SCPLOGSTRING3( "CScpSubService[0x%x]::IsRoaming: %d", this, iRoaming );

    return iRoaming;
    }

// -----------------------------------------------------------------------------
// CScpSubService::ChangeState
// -----------------------------------------------------------------------------
//
void CScpSubService::ChangeState( TCCHSubserviceState aState, TInt aError )
    {
    SCPLOGSTRING5( "CScpSubService[0x%x]::ChangeState from %d to %d error: %d", this,
                    iSubServiceState->State(), aState, aError );
    SCPLOGSTRING3( "CScpSubService[0x%x]::ChangeState type %d", this, iSubServiceType );
    __ASSERT_DEBUG( aState != ECCHUninitialized, User::Panic( KNullDesC, KErrGeneral ) );

    __ASSERT_DEBUG( iSubServiceState->State() == ECCHDisabled ? 
                    aState != ECCHDisconnecting : ETrue, 
                    User::Panic( KNullDesC, KErrGeneral ) );
    __ASSERT_DEBUG( iSubServiceState->State() == ECCHConnecting ? 
                    aState != ECCHDisabled || KErrCancel == aError : ETrue, 
                    User::Panic( KNullDesC, KErrGeneral ) );
    __ASSERT_DEBUG( iSubServiceState->State() == ECCHDisconnecting ? 
                    aState == ECCHDisabled : ETrue, 
                    User::Panic( KNullDesC, KErrGeneral ) );
    __ASSERT_DEBUG( iSubServiceState->State() == ECCHEnabled ? 
                    aState != ECCHDisabled : ETrue, 
                    User::Panic( KNullDesC, KErrGeneral ) );

    if ( aState != iSubServiceState->State() ||
        ( aState == iSubServiceState->State() && iLastReportedError != aError ) )
        {
        iLastReportedError = aError;
        
        switch( aState )
            {
            case ECCHConnecting:
                {
                iSubServiceState = 
                    TScpStateContainer::Instance( ECCHConnecting );
                }
                break;

            case ECCHEnabled:
                {
                iSubServiceState = 
                    TScpStateContainer::Instance( ECCHEnabled );
                }
                break;

            case ECCHDisconnecting:
                {
                iSubServiceState = 
                    TScpStateContainer::Instance( ECCHDisconnecting );
                }
                break;

            case ECCHDisabled:
                {
                iSubServiceState = 
                    TScpStateContainer::Instance( ECCHDisabled );
                }
                break;

            default:
                __ASSERT_DEBUG( EFalse, User::Panic( KNullDesC, KErrGeneral ) );
                break;

            }

        TBool sendEvent = ETrue;
        
        // During connection refreshing we don't send events about disabling
        if ( iEnableRequestedState == EScpRefreshed )
            {
            if ( aState == ECCHDisconnecting || aState == ECCHDisabled )
                {
                sendEvent = EFalse;
                }
            }
            
        if ( sendEvent )
            {
            TServiceSelection selection( iSubServiceId, iSubServiceType );

            MCchServiceObserver& observer = iService.ServiceObserver();
            observer.ServiceStateChanged( selection,
                                          aState,
                                          aError );
            
            if ( ECCHDisabled == aState )
                {
                ResetEnableCounter();
                // No need to hold the last reported error, if all 
                // subservices are in disabled state
                if ( iService.IsAllSubservicesDisabled() )
                    {
                    iService.ChangeLastReportedErrors( KErrNone );
                    }
                }
            }
                                          
        if ( iSubServiceObserver ) 
            {
            SCPLOGSTRING2( "CScpSubService[0x%x]::ChangeState HandleSubServiceChange", this );
            iSubServiceObserver->HandleSubServiceChange( aState, aError );
            }
        }
    }

// -----------------------------------------------------------------------------
// CScpSubService::HandleConnectionEvent
// -----------------------------------------------------------------------------
//
void CScpSubService::HandleConnectionEvent( TScpConnectionEvent aEvent )
    {
    SCPLOGSTRING4( "CScpSubService[0x%x]::HandleConnectionEvent event: %d state: %d", 
                   this, aEvent, iSubServiceState->State() );
    __ASSERT_DEBUG( aEvent != EScpUnknown, User::Panic( KNullDesC, KErrGeneral ) );

    // All other events than EScpRoaming finish roaming
    iRoaming = EFalse;
    
    switch ( aEvent )
        {
        case EScpNetworkFound:
            {
            iSubServiceState->NetworkFound( *this );
            }
            break;

        case EScpNetworkNotFound:
            {
            iSubServiceState->NetworkNotFound( *this );
            }
            break;

        case EScpNetworkLost:
            {
            iSubServiceState->NetworkLost( *this );
            }
            break;

        case EScpRegistered:
            {
            iSubServiceState->ServiceConnected( *this );
            }
            break;

        case EScpRegistrationFailed:
            {
            iSubServiceState->ServiceConnectionFailed( *this );
            }
            break;
        case EScpInvalidSettings:
            {
            iSubServiceState->ServiceInvalidSettings( *this );
            }
            break;
        case EScpDeregistered:
            {
            iSubServiceState->ServiceDisconnected( *this );
            }
            break;

        case EScpBandwidthLimited:
            {
            iSubServiceState->BandwidthLimited( *this );
            }
            break;

        case EScpAuthenticationFailed:
            {
            iSubServiceState->AuthenticationFailed( *this );
            }
            break;
            
        case EScpRoaming:
            {
            iRoaming = ETrue;
            iSubServiceState->Roaming( *this );
            }
            break;
            
        case EScpRegistrationCanceled:
            {
            iSubServiceState->ServiceConnectionCanceled( *this );
            }
            break;
            
        case EScpBearerNotSupported:
            {
            iSubServiceState->BearerNotSupported( *this );
            }
            break;
            
        case EScpRegistrationPending:
            {
            iSubServiceState->RegistrationPending( *this );
            }
            break;
            
        default:
            __ASSERT_DEBUG( EFalse, User::Panic( KNullDesC, KErrGeneral ) );
            break;
        }

    // Remove user disabled sub services if correct
    // network event received
    if ( iEnableRequestedState == EScpDisabled &&
         ( aEvent == EScpDeregistered ||
           aEvent == EScpRegistrationFailed ||
           aEvent == EScpNetworkLost ||
           aEvent == EScpNetworkNotFound ) )
        {
        CScpServiceStorage& storage = ServiceStorage();
        storage.RemoveDisabledServices();
        }
    else if ( iEnableRequestedState == EScpRefreshed &&
             aEvent == EScpDeregistered )
        {
        // We can only ingnore the error
        TRAP_IGNORE( EnableL() );
        }
    }

// -----------------------------------------------------------------------------
// CScpSubService::SetSubServiceObserver
// -----------------------------------------------------------------------------
//
void CScpSubService::SetSubServiceObserver( MScpSubServiceObserver* aSubServiceObserver )
    {
    SCPLOGSTRING3( "CScpSubService[0x%x]::SetSubServiceObserver aSubServiceObserver[0x%x]", 
        this, aSubServiceObserver );
        
    iSubServiceObserver = aSubServiceObserver;
    }
  
// -----------------------------------------------------------------------------
// CScpSubService::SetPresenceToObserveVoIPL
// -----------------------------------------------------------------------------
// 
void CScpSubService::SetPresenceToObserveVoIP()
    {
    SCPLOGSTRING2( "CScpSubService[0x%x]::SetPresenceToObserveVoIP", this );
    CScpSubService* presenceSubService = iService.GetSubServiceByType( ECCHPresenceSub );
    if ( presenceSubService )
        {
        SCPLOGSTRING( "CScpSubService::SetPresenceToObserveVoIPL - presenceSubService exists" );
        CScpPresenceHandler* presenceHandler =
            static_cast<CScpPresenceHandler*>( &presenceSubService->ServiceHandler() );
          SetSubServiceObserver( presenceHandler );
        }
    }

// End of file
