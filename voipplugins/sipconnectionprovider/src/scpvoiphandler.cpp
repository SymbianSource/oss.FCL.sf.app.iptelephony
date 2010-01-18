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
* Description:  Voip handler.
*
*/


#include <e32property.h>
#include <NetworkHandlingDomainPSKeys.h>
#include <spdefinitions.h>
#include "scpvoiphandler.h"
#include "scpprofilehandler.h"
#include "scplogger.h"
#include "scpsubservice.h"
#include "scpservicestorage.h"
#include "scpsettinghandler.h"
#include "scpsipconnection.h"
#include "scputility.h"
#include "scppropertynotifier.h"
#include "csipclientresolverutils.h"

// : These need to be in some common header
const TUid KResolver = { 0x10283140 };


// -----------------------------------------------------------------------------
// CScpVoipHandler::CCScpVoipHandler
// -----------------------------------------------------------------------------
//
CScpVoipHandler::CScpVoipHandler( CScpSubService& aSubService ) :
    CScpServiceHandlerBase( aSubService )
    {
    SCPLOGSTRING2( "CScpVoipHandler[0x%x]::CScpVoipHandler", this );
    }

// -----------------------------------------------------------------------------
// CScpVoipHandler::ConstructL
// -----------------------------------------------------------------------------
//
void CScpVoipHandler::ConstructL()
    {
    SCPLOGSTRING2( "CScpVoipHandler[0x%x]::ConstructL", this );
    
    BaseConstructL();
    iNetworkMode = CScpPropertyNotifier::NewL( KPSUidNetworkInfo, 
        KNWTelephonyNetworkMode, *this ) ;
    iSipClientResolver = CSipClientResolverUtils::NewL();
    }

// -----------------------------------------------------------------------------
// CScpVoipHandler::NewL
// -----------------------------------------------------------------------------
//
CScpVoipHandler* CScpVoipHandler::NewL( CScpSubService& aSubService )
    {
    SCPLOGSTRING( "CScpVoipHandler::NewL" );

    CScpVoipHandler* self = new(ELeave) CScpVoipHandler( aSubService );
    CleanupStack::PushL( self );    
    self->ConstructL();    
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CScpVoipHandler::~CScpVoipHandler
// -----------------------------------------------------------------------------
//
CScpVoipHandler::~CScpVoipHandler()
    {
    SCPLOGSTRING2( "CScpVoipHandler[0x%x]::~CScpVoipHandler", this );
    delete iSipClientResolver;
    delete iNetworkMode;
    }

// -----------------------------------------------------------------------------
// CScpVoipHandler::HandlePropertyChanged
// -----------------------------------------------------------------------------
//
void CScpVoipHandler::HandlePropertyChanged( TInt /* aPropertyValue */)
    {
    CScpSipConnection::TConnectionState sipState = CScpSipConnection::EUnknown;
    TInt sipError = KErrNotFound;
    CScpSipConnection* sipConnection = 
        iSubService.ProfileHandler().GetSipConnection( 
            iSubService.SipProfileId() );
    if ( sipConnection )
        {
        sipConnection->GetState( sipState, sipError );
        }
    if (  ECCHEnabled == iSubService.State() && 
        KErrNone == sipError && CScpSipConnection::ERegistered == sipState )
        {
        HandleSipConnectionEvent( iSubService.SipProfileId(), EScpRegistered );
        }
    }
    
// -----------------------------------------------------------------------------
// CScpVoipHandler::EnableSubServiceL
// -----------------------------------------------------------------------------
//
void CScpVoipHandler::EnableSubServiceL()
    {
    SCPLOGSTRING4( "CScpVoipHandler[0x%x]::EnableSubServiceL: 0x%x type: %i", 
                   this, &iSubService, iSubService.SubServiceType() );
    __ASSERT_DEBUG( iSubService.SubServiceType() == ECCHVoIPSub,
                    User::Panic( KNullDesC, KErrGeneral ) );
    
    CScpServiceHandlerBase::RegisterProfileL();
    }

// -----------------------------------------------------------------------------
// CScpVoipHandler::DisableSubService
// -----------------------------------------------------------------------------
//
TInt CScpVoipHandler::DisableSubService()
    {
    SCPLOGSTRING4( "CScpVoipHandler[0x%x]::DisableSubService: 0x%x type: %i", 
                   this, &iSubService, iSubService.SubServiceType() );
    __ASSERT_DEBUG( iSubService.SubServiceType() == ECCHVoIPSub,
                    User::Panic( KNullDesC, KErrGeneral ) ); 

    TInt result = KErrNone;

    TCCHSubserviceState subServiceState = iSubService.State();

    if( subServiceState != ECCHDisabled )
        {
        TRAP( result, DeregisterContactHeaderL() )        

        DeregisterProfile();
        }
    else
        {
        result = KErrNotSupported;
        }

    return result;
    }

// -----------------------------------------------------------------------------
// CScpVoipHandler::DeregisterContactHeaderL
// -----------------------------------------------------------------------------
//
void CScpVoipHandler::DeregisterContactHeaderL()
    {
    SCPLOGSTRING2( "CScpVoipHandler[0x%x]::DeregisterContactHeaderL", this );

    RBuf8 contactHeaderUser;
    contactHeaderUser.CleanupClosePushL();

    GetSipProfileContactHeaderUserL( contactHeaderUser );
    
    iSipClientResolver->UnRegisterClientWithUserL( 
        GetCallProviderImplementationUidL(), contactHeaderUser );

    CleanupStack::PopAndDestroy( &contactHeaderUser );
    }

// -----------------------------------------------------------------------------
// CScpVoipHandler::SubServiceType
// -----------------------------------------------------------------------------
//
TCCHSubserviceType CScpVoipHandler::SubServiceType() const
    {
    SCPLOGSTRING2( "CScpVoipHandler[0x%x]::SubServiceType", this );

    return ECCHVoIPSub;
    }


// -----------------------------------------------------------------------------
// CScpVoipHandler::HandleSipConnectionEvent
// -----------------------------------------------------------------------------
//
void CScpVoipHandler::HandleSipConnectionEvent( TUint32 aProfileId,
                                                TScpConnectionEvent aEvent )
    {
    TRAP_IGNORE( HandleSipConnectionEventL( aProfileId,aEvent ));
    }
// -----------------------------------------------------------------------------
// CScpVoipHandler::HandleSipConnectionEvent
// -----------------------------------------------------------------------------
//
void CScpVoipHandler::HandleSipConnectionEventL( TUint32 aProfileId,
                                                TScpConnectionEvent aEvent )
    {
    SCPLOGSTRING4( "CScpVoipHandler[0x%x]::HandleSipConnectionEvent id: %d event: %d",
                   this, aProfileId, aEvent );

    if( iSubService.SipProfileId() == aProfileId &&
        iSubService.EnableRequestedState() != CScpSubService::EScpNoRequest )
        {
        // When in 2G mode with GPRS iap we are enabled but can't
        // create or receive any VoIP calls
        if( aEvent == EScpRegistered &&
            iSubService.EnableRequestedState() == CScpSubService::EScpEnabled )
            {
            RBuf8 contactHeaderUser;
            contactHeaderUser.CleanupClosePushL();
            GetSipProfileContactHeaderUserL( contactHeaderUser );
            
            HBufC* string = HBufC::NewLC(contactHeaderUser.Length());
            string->Des().Copy(contactHeaderUser);
         
            SCPLOGSTRING2( "CScpVoipHandler::HandleSipConnectionEventL(), contactHeaderUser %S", string );
            
            CleanupStack::PopAndDestroy(string);

            TInt networkMode = KErrNotFound;
            TInt result = iNetworkMode->GetValue( networkMode );

            if( result == KErrNone &&
                networkMode == ENWNetworkModeGsm &&
                iSubService.GetIapType() == EScpGprs )
                {
                aEvent = EScpBandwidthLimited;    
                iSipClientResolver->UnRegisterClientWithUserL( 
                    GetCallProviderImplementationUidL(), contactHeaderUser );
                }
            else if ( KErrNone == result &&
                EScpGprs == iSubService.GetIapType() &&
                !iSubService.ServiceStorage().SettingsHandler().IsVoIPOverWcdmaAllowedL( iSubService.SubServiceId() ) )
                {
                aEvent = EScpBearerNotSupported;
                SCPLOGSTRING( "CScpVoipHandler::HandleSipConnectionEventL(), VoIP over 3G not allowed" );
                iSipClientResolver->UnRegisterClientWithUserL( 
                    GetCallProviderImplementationUidL(), contactHeaderUser );
                }
            else
                {
                iSipClientResolver->RegisterClientWithUserL( 
                    GetCallProviderImplementationUidL(), contactHeaderUser, KResolver );
                SCPLOGSTRING( "CScpVoipHandler::HandleSipConnectionEventL(), client resolver" );
                }

            CleanupStack::PopAndDestroy( &contactHeaderUser );
            }

        iSubService.HandleConnectionEvent( aEvent );
        }
    }

// -----------------------------------------------------------------------------
// CScpVoipHandler::GetSipProfileContactHeaderL
// -----------------------------------------------------------------------------
//
void CScpVoipHandler::GetSipProfileContactHeaderUserL( 
    RBuf8& aContactHeaderUser ) const
    {
    SCPLOGSTRING2( "CScpVoipHandler[0x%x]::GetSipProfileContactHeaderL", this );

    TUint32 sipProfileId = iSubService.SipProfileId();

    CScpProfileHandler& profileHandler = iSubService.ProfileHandler();
    CScpSipConnection* sipConnection = 
        profileHandler.CreateSipConnectionL( sipProfileId );
    CleanupStack::PushL( sipConnection );
    
    if( sipConnection )
        {
        User::LeaveIfError( sipConnection->GetContactHeaderUser( aContactHeaderUser ) );
        __ASSERT_DEBUG( aContactHeaderUser.Length() > 0, 
        User::Panic( KNullDesC, KErrGeneral ) );
        }
    else
        {
        User::Leave( KErrNotFound );
        }

    CleanupStack::PopAndDestroy( sipConnection );
    }
    
// -----------------------------------------------------------------------------
// CScpVoipHandler::CallProviderImplementationUid
// -----------------------------------------------------------------------------
//   
TUid CScpVoipHandler::GetCallProviderImplementationUidL()
    {
    SCPLOGSTRING2( "CScpVoipHandler[0x%x]::GetCallProviderImplementationUidL", this );

    CScpServiceStorage& serviceStorage = iSubService.ServiceStorage();
    CScpSettingHandler& settingHandler = serviceStorage.SettingsHandler();

    TInt impUid = 0;
    settingHandler.GetSPSettingsIntPropertyL( iSubService.SubServiceId(),
                                              EPropertyCallProviderPluginId,
                                              impUid );
   
    TUid implementationUid; 
    implementationUid.iUid = impUid;

    return implementationUid;
    }
    
//  End of File  
