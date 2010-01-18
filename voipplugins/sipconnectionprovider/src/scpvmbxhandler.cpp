/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Vmbx handler.
*
*/


#include "ipvmbxinterface.h"
#include "scpvmbxhandler.h"
#include "scplogger.h"
#include "scpservice.h"
#include "scpsubservice.h"
#include "scpservicestorage.h"
#include "scputility.h"
#include "scpsipconnection.h"
#include "scpprofilehandler.h"

// -----------------------------------------------------------------------------
// CScpVmbxHandler::CCScpVmbxHandler
// -----------------------------------------------------------------------------
//
CScpVmbxHandler::CScpVmbxHandler( CScpSubService& aSubService ) :
    CScpServiceHandlerBase( aSubService )
    {
    SCPLOGSTRING2( "CScpVmbxHandler[0x%x]::CScpVmbxHandler", this );
    }

// -----------------------------------------------------------------------------
// CScpVmbxHandler::ConstructL
// -----------------------------------------------------------------------------
//
void CScpVmbxHandler::ConstructL()
    {
    SCPLOGSTRING2( "CScpVmbxHandler[0x%x]::ConstructL", this );

    BaseConstructL();
    }

// -----------------------------------------------------------------------------
// CScpVmbxHandler::NewL
// -----------------------------------------------------------------------------
//
CScpVmbxHandler* CScpVmbxHandler::NewL( CScpSubService& aSubService )
    {
    SCPLOGSTRING( "CScpVmbxHandler::NewL" );

    CScpVmbxHandler* self = new(ELeave) CScpVmbxHandler( aSubService );
    CleanupStack::PushL( self );    
    self->ConstructL();    
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CScpVmbxHandler::~CScpVmbxHandler
// -----------------------------------------------------------------------------
//
CScpVmbxHandler::~CScpVmbxHandler()
    {
    SCPLOGSTRING2( "CScpVmbxHandler[0x%x]::~CScpVmbxHandler", this );
    
    if ( !IsAnotherVmbxSubServiceAlreadyEnabled() )
        {
        iSubService.ProfileHandler().DeleteVmbxInterface();
        }
    }

// -----------------------------------------------------------------------------
// CScpVmbxHandler::EnableSubServiceL
// -----------------------------------------------------------------------------
//
void CScpVmbxHandler::EnableSubServiceL()
    {
    SCPLOGSTRING4( "CScpVmbxHandler[0x%x]::EnableSubServiceL: 0x%x type: %i", 
                   this, &iSubService, iSubService.SubServiceType() );
    __ASSERT_DEBUG( iSubService.SubServiceType() == ECCHVMBxSub,
                    User::Panic( KNullDesC, KErrGeneral ) );

    // Only one enabled vmbx service is allowed at a time.
    if( IsAnotherVmbxSubServiceAlreadyEnabled() )
        {
        User::Leave( KErrInUse );
        }

    CScpServiceHandlerBase::RegisterProfileL();
    }

// -----------------------------------------------------------------------------
// CScpVmbxHandler::IsAnotherVmbxSubServiceAlreadyEnabled
// -----------------------------------------------------------------------------
//
TBool CScpVmbxHandler::IsAnotherVmbxSubServiceAlreadyEnabled() const
    {
    SCPLOGSTRING2( "CScpVmbxHandler[0x%x]::IsAnotherVmbxSubServiceAlreadyEnabled", this );

    TBool result = EFalse;

    CScpServiceStorage& storage = iSubService.ServiceStorage();
    RArray<TInt> ids;

    storage.GetSubServiceIds( ECCHVMBxSub, ids );

    TInt count( ids.Count() );

    for ( TInt i( 0 ); i < count; i++ )
        {
        CScpSubService* subService = storage.GetSubService( ids[i] );

        if ( subService && ECCHDisabled != subService->State() )
            {
            result = ETrue;
            break;
            }
        }

    ids.Close();
    return result;
    }

// -----------------------------------------------------------------------------
// CScpVmbxHandler::DisableSubService
// -----------------------------------------------------------------------------
//
TInt CScpVmbxHandler::DisableSubService()
    {
    SCPLOGSTRING4( "CScpVmbxHandler[0x%x]::DisableSubService: 0x%x type: %i", 
                   this, &iSubService, iSubService.SubServiceType() );
    __ASSERT_DEBUG( iSubService.SubServiceType() == ECCHVMBxSub,
                    User::Panic( KNullDesC, KErrGeneral ) );  

    TInt result = KErrNone;

    if( iSubService.State() != ECCHDisabled )
        {
        TRAP( result, UnsubscribeL() );

        if( result == KErrNone )
            {
            // Deregister if still connecting and subscribed message 
            // not yet received
            if( iSubService.State() == ECCHConnecting )
                {
                DeregisterProfile();
                }
            else
                {
                StartForcedDisableTimer( CScpVmbxHandler::ForceVmbxServiceDisable );
                }
            }
        else
            {
            DeregisterProfile();
            }
        }
    else
        {
        result = KErrNotSupported;
        }

    return result;
    }

// -----------------------------------------------------------------------------
// CScpVmbxHandler::UnsubscribeL
// -----------------------------------------------------------------------------
//
void CScpVmbxHandler::UnsubscribeL()
    {
    SCPLOGSTRING2( "CScpVmbxHandler[0x%x]::UnsubscribeL", this );

    CScpProfileHandler& profileHandler = iSubService.ProfileHandler();

    // Unsubscribe is not called when refreshing the connection
    if( iSubService.EnableRequestedState() != CScpSubService::EScpRefreshed )
        {
        CIpVmbxInterface& vmbxInterface = profileHandler.VmbxInterfaceL( *this );
        vmbxInterface.UnsubscribeL( iSubService.SubServiceId() );
        }
    }

// -----------------------------------------------------------------------------
// CScpVmbxHandler::HandleMessage
// -----------------------------------------------------------------------------
//
void CScpVmbxHandler::HandleMessage( TUint32 aServiceId, TVmbxMessage aMessage )
    {
    SCPLOGSTRING4( "CScpVmbxHandler[0x%x]::HandleMessage: id: %d message: %d", 
                   this, aServiceId, aMessage );

    TInt result = KErrNone;
    
    if( iSubService.SubServiceId() == aServiceId )
        {
        switch( aMessage )
            {
            case ESubscribed:
                {
                iSubService.HandleConnectionEvent( EScpRegistered );
                }
                break;

            case EUnsubscribed:
                {
                // Check if disable was requested
                if( iSubService.EnableRequestedState() == CScpSubService::EScpDisabled )
                    {
                    CancelDisableTimer();
                    DeregisterProfile();
                    }
                else
                    {
                    if ( iResubscribe )
                        {
                        iResubscribe = EFalse;
                        TRAP( result, SubscribeL() )
                        SCPLOGSTRING2( "Subscribe result: %d", result ); 
                        
                        if( result == KErrNone )
                            {
                            // Still connecting the service
                            iSubService.HandleConnectionEvent( EScpNetworkFound );
                            }
                        else if ( KErrAlreadyExists == result )
                            {
                            iSubService.HandleConnectionEvent( EScpRegistrationPending );
                            }
                        else
                            {
                            iSubService.HandleConnectionEvent( EScpRegistrationFailed );
                            }
                        }
                    else
                        {
                        // Connection to VMBx server lost
                        iSubService.HandleConnectionEvent( EScpDeregistered );
                        }
                    }
                }
                break;

            case ENetworkError:
                {
                // Network lost errors may have already been reported
                if( iSubService.LastReportedError() == KErrNone )
                    {
                    // Connection to VMBx server lost
                    iSubService.HandleConnectionEvent( EScpRegistrationFailed );
                    }
                break;
                }

            case EIncorrectSettings:
                {
                iSubService.HandleConnectionEvent( EScpRegistrationFailed );
                }
                break;

            case ENoMemory:
            case ESmsError:
                {
                // :
                // Error handling for VMBx errors
                // In network error case we should change the state to connecting +
                // "service not respondig" error
                SCPLOGSTRING( "Error message from VMBx interface" ) 
                }
                break;

            default:
                {
                __ASSERT_DEBUG( EFalse, User::Panic( KNullDesC, KErrGeneral ) );
                }
            }
        }
    }
    
// -----------------------------------------------------------------------------
// CScpVmbxHandler::SubServiceType
// -----------------------------------------------------------------------------
//
TCCHSubserviceType CScpVmbxHandler::SubServiceType() const
    {
    SCPLOGSTRING2( "CScpVmbxHandler[0x%x]::SubServiceType", this );

    return ECCHVMBxSub;
    }

// -----------------------------------------------------------------------------
// CScpVmbxHandler::HandleSipConnectionEvent
// -----------------------------------------------------------------------------
//
void CScpVmbxHandler::HandleSipConnectionEvent( TUint32 aProfileId,
                                                TScpConnectionEvent aEvent )
    {
    SCPLOGSTRING4( "CScpVmbxHandler[0x%x]::HandleSipConnectionEvent id: %d event: %d",
                   this, aProfileId, aEvent );
    
    TInt result = KErrNone;
    
    if( iSubService.SipProfileId() == aProfileId &&
        iSubService.EnableRequestedState() != CScpSubService::EScpNoRequest )
        {
        if ( EScpRoaming == aEvent )
            {
            SCPLOGSTRING( "CScpVmbxHandler - EScpRoaming -> unsubscribe" );
            TRAP( result, UnsubscribeL() );
            SCPLOGSTRING2( "CScpVmbxHandler - unsubscribe error: %d", result );
            }
        
        if( aEvent == EScpRegistered &&
            iSubService.EnableRequestedState() == CScpSubService::EScpEnabled )
            {
            TRAP( result, SubscribeL() )

            SCPLOGSTRING2( "Subscribe result: %d", result ); 
        
            if( result == KErrNone )
                {
                // Still connecting the service
                aEvent = EScpNetworkFound;
                }
            else
                {
                if ( iSubService.IsRoaming() && KErrAlreadyExists == result )
                    {
                    // There's still unsubscribe ongoing, we have to wait
                    // ipvoicemailengine event EUnsubscribed and try to 
                    // make subscribe again
                    SCPLOGSTRING( "Resubscribe later" ); 
                    iResubscribe = ETrue;
                    // Still connecting the service
                    aEvent = EScpNetworkFound;
                    }
                else
                    {
                    aEvent = EScpRegistrationFailed;
                    }
                }
            }
        else if( aEvent == EScpDeregistered &&
                 iSubService.EnableRequestedState() == CScpSubService::EScpDisabled ||
                 iSubService.EnableRequestedState() == CScpSubService::EScpRefreshed )
            {
            CancelDisableTimer();
            }

        iSubService.HandleConnectionEvent( aEvent );
        }        
    }

// -----------------------------------------------------------------------------
// CScpVmbxHandler::SubscribeL
// -----------------------------------------------------------------------------
//
void CScpVmbxHandler::SubscribeL()
    {
    SCPLOGSTRING2( "CScpVmbxHandler[0x%x]::SubscribeL", this );

    TUint32 sipProfileId = iSubService.SipProfileId();
    CScpProfileHandler& profileHandler = iSubService.ProfileHandler();

    CScpSipConnection* sipConnection = profileHandler.GetSipConnection( sipProfileId );

    if( sipConnection )
        {
        CIpVmbxInterface& vmbxInterface = profileHandler.VmbxInterfaceL( *this );

        // It must be possible to add observers for the vmbx interface

        vmbxInterface.SubscribeL( iSubService.SubServiceId(), 
                                  sipConnection->SipProfile() );
        }
    else
        {
        User::LeaveIfError( KErrNotFound );
        }
    }

// -----------------------------------------------------------------------------
// CScpVmbxHandler::ForceDisable
// -----------------------------------------------------------------------------
//
TInt CScpVmbxHandler::ForceVmbxServiceDisable( TAny* aSelf )
    {
    SCPLOGSTRING( "CScpVmbxHandler[0x%x]::ForceVmbxServiceDisable" );

    CScpVmbxHandler* self = static_cast<CScpVmbxHandler*>( aSelf );

    self->CancelDisableTimer();
    self->DeregisterProfile();

    return 1;
    }


//  End of File  
