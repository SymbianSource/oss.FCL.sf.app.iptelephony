/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Base class of protocol profile handlers.
*
*/


#include "scpservicehandlerbase.h"
#include "scpprofilehandler.h"
#include "scputility.h"
#include "scplogger.h"
#include "scpservicestorage.h"
#include "scpsubservice.h"

// If sub service disconnection fails a timeout for
// forced disabled is used.
const TInt KDisableTimeout = 5000000;

// -----------------------------------------------------------------------------
// CScpServiceHandlerBase::CScpServiceHandlerBase
// -----------------------------------------------------------------------------
//
CScpServiceHandlerBase::CScpServiceHandlerBase( CScpSubService& aSubService ) :
    iSubService( aSubService )
    {
    SCPLOGSTRING2( "CScpServiceHandlerBase[0x%x]::CScpServiceHandlerBase", this); 
    }

// -----------------------------------------------------------------------------
// CScpServiceHandlerBase::~CScpServiceHandlerBase
// -----------------------------------------------------------------------------
//
CScpServiceHandlerBase::~CScpServiceHandlerBase()
    {
    SCPLOGSTRING2( "CScpServiceHandlerBase[0x%x]::~CScpServiceHandlerBase",this); 

    delete iDisableTimer;

    CScpProfileHandler& profileHandler = iSubService.ProfileHandler();
    profileHandler.RemoveObserver( *this );
    }

// -----------------------------------------------------------------------------
// CScpServiceHandlerBase::BaseConstructL
// -----------------------------------------------------------------------------
//
void CScpServiceHandlerBase::BaseConstructL()
    {
    SCPLOGSTRING2( "CScpServiceHandlerBase[0x%x]::BaseConstructL", this );

    CScpProfileHandler& profileHandler = iSubService.ProfileHandler();
    profileHandler.AddObserverL( *this );
    
    iDisableTimer = CPeriodic::NewL( 0 );
    }

// -----------------------------------------------------------------------------
// From base class MScpSipConnectionObserver
// CScpServiceHandlerBase::IsSipProfileAllowedToStartAlr
// -----------------------------------------------------------------------------
//
TBool CScpServiceHandlerBase::IsSipProfileAllowedToStartAlr()
    {
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CScpServiceHandlerBase::RegisterProfileL
// -----------------------------------------------------------------------------
//
void CScpServiceHandlerBase::RegisterProfileL()
    {
    SCPLOGSTRING2( "CScpServiceHandlerBase[0x%x]::RegisterProfileL", this );

    CScpProfileHandler& profileHandler = iSubService.ProfileHandler();
    profileHandler.RegisterProfileL( iSubService.SipProfileId() );

    CScpSipConnection::TConnectionState currentState = CScpSipConnection::EUnknown;
    TInt error = KErrNone;

    profileHandler.GetCurrentState( iSubService.SipProfileId(), 
                                     currentState,
                                     error );

    __ASSERT_DEBUG( currentState != CScpSipConnection::EUnknown, 
                    User::Panic( KNullDesC, KErrGeneral ) );

    TScpConnectionEvent event = TScpUtility::ConvertToConnectionEvent( currentState,
                                                                       error );
    if ( event == EScpRegistered )
        {
        // Sip profile was already registered
        HandleSipConnectionEvent( iSubService.SipProfileId(), EScpRegistered );
        }
    else
        {
        iSubService.HandleConnectionEvent( event );
        }
    }

// -----------------------------------------------------------------------------
// CScpServiceHandlerBase::DeregisterProfile
// -----------------------------------------------------------------------------
//
void CScpServiceHandlerBase::DeregisterProfile()
    {
    SCPLOGSTRING2( "CScpServiceHandlerBase[0x%x]::DeregisterProfile", this );

    // At this point all the service related disconnection operations
    // have been finished. Now the SIP profile needs to be disabled
    iSubService.SetSubServiceDisconnected( ETrue );

    TInt sipProfileId = iSubService.SipProfileId(); 
    CScpServiceStorage& storage = iSubService.ServiceStorage();

    // The SIP profile can be only disabled if
    // - None of the sub services are in enable state with the same profile
    // - None of the sub services are doing disconnection with the same profile
    if ( !storage.IsSubServiceEnabled( sipProfileId ) &&
         storage.AreAllSubServicesDisconnected( sipProfileId ) )
        {  
        CScpSipConnection::TConnectionState currentState = 
            CScpSipConnection::EUnknown;
        TInt error( KErrNone );        

        CScpProfileHandler& profileHandler = iSubService.ProfileHandler();
        profileHandler.GetCurrentState( sipProfileId, 
                                         currentState,
                                         error );        

        if ( currentState == CScpSipConnection::ERegistering )
            {
            profileHandler.CancelRegistration( sipProfileId );
            }
        else
            {
            error = profileHandler.UnregisterProfile( sipProfileId );
            if ( KErrNotFound == error )
                {
                SCPLOGSTRING3(
                        "CScpServiceHandlerBase[0x%x]::DeregisterProfile - No profile with ID: %d",
                        this,
                        sipProfileId );
                }
            }
        }

    StartForcedDisableTimer( CScpServiceHandlerBase::ForceSipProfileDisable );
    }

// -----------------------------------------------------------------------------
// CScpServiceHandlerBase::StartForcedDisableTimer
// -----------------------------------------------------------------------------
//
void CScpServiceHandlerBase::StartForcedDisableTimer( TInt (*aFunction)(TAny* aPtr) )
    {
    SCPLOGSTRING2( "CScpServiceHandlerBase[0x%x]::StartForcedDisableTimer", this );
    __ASSERT_DEBUG( !iDisableTimer->IsActive(), 
                    User::Panic( KNullDesC, KErrGeneral ) );

    if ( !iDisableTimer->IsActive() )
        {
        iDisableTimer->Start( KDisableTimeout, 
                              0, 
                              TCallBack( aFunction, this ) );
        }
    }

// -----------------------------------------------------------------------------
// CScpServiceHandlerBase::CancelDisableTimer
// -----------------------------------------------------------------------------
//
void CScpServiceHandlerBase::CancelDisableTimer()
    {
    SCPLOGSTRING2( "CScpServiceHandlerBase[0x%x]::CancelDisableTimer", this );

    if ( iDisableTimer->IsActive() )
        {
        iDisableTimer->Cancel();
        }
    }

// -----------------------------------------------------------------------------
// CScpServiceHandlerBase::HandleSipProfileForcedDisable
// -----------------------------------------------------------------------------
//
void CScpServiceHandlerBase::HandleSipProfileForcedDisable()
    {
    SCPLOGSTRING2( 
        "CScpServiceHandlerBase[0x%x]::HandleSipProfileForcedDisable", this );

    HandleSipConnectionEvent( iSubService.SipProfileId(), EScpDeregistered );
    }

// -----------------------------------------------------------------------------
// CScpServiceHandlerBase::ForceSipProfileDisable
// -----------------------------------------------------------------------------
//
TInt CScpServiceHandlerBase::ForceSipProfileDisable( TAny* aSelf )
    {
    SCPLOGSTRING( "CScpServiceHandlerBase::ForceSipProfileDisable" );

    CScpServiceHandlerBase* self = static_cast<CScpServiceHandlerBase*>( aSelf );
    
    self->CancelDisableTimer();
    self->HandleSipProfileForcedDisable();

    return 1;
    }


//  End of File  
