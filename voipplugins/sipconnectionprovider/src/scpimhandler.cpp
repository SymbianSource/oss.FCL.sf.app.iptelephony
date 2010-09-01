/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: SCP IM Handler
*
*/


#include "scpimhandler.h"
#include "scpsubservice.h"
#include "scplogger.h"
#include "scpprofilehandler.h"
#include "csipclientresolverutils.h"

// Implementation UID of IM Resolver plugin
const TUid KResolver = { 0x2002133D };

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CScpImHandler::CCScpImHandler
// ---------------------------------------------------------------------------
//
CScpImHandler::CScpImHandler( CScpSubService& aSubService ) :
    CScpServiceHandlerBase( aSubService )
    {
    SCPLOGSTRING2( "CScpImHandler[0x%x]::CScpImHandler", this )
    }

// ---------------------------------------------------------------------------
// CScpImHandler::ConstructL
// ---------------------------------------------------------------------------
//
void CScpImHandler::ConstructL()
    {
    SCPLOGSTRING2( "CScpImHandler[0x%x]::ConstructL", this )
    BaseConstructL();
    iSipClientResolver = CSipClientResolverUtils::NewL();
    }

// ---------------------------------------------------------------------------
// CScpPresenceHandler::NewL
// ---------------------------------------------------------------------------
//
CScpImHandler* CScpImHandler::NewL( CScpSubService& aSubService )
    {
    SCPLOGSTRING( "CScpImHandler::NewL" )
    CScpImHandler* self = new ( ELeave ) CScpImHandler( aSubService );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CScpImHandler::~CScpImHandler
// ---------------------------------------------------------------------------
//
CScpImHandler::~CScpImHandler()
    {
    SCPLOGSTRING2( "CScpImHandler[0x%x]::~CScpImHandler", this )
    delete iSipClientResolver;
    SCPLOGSTRING( "CScpImHandler[0x%x]::~CScpImHandler - Out" )
    }

// ---------------------------------------------------------------------------
// From class CScpServiceHandlerBase
// CScpImHandler::EnableSubServiceL
// ---------------------------------------------------------------------------
//
void CScpImHandler::EnableSubServiceL()
    {
    SCPLOGSTRING4( "CScpImHandler[0x%x]::EnableSubServiceL: 0x%x type: %i", 
                   this, &iSubService, iSubService.SubServiceType() )
    
    __ASSERT_DEBUG( ECCHIMSub == iSubService.SubServiceType(), User::Panic(
        KNullDesC, KErrGeneral ) );
    
    CScpServiceHandlerBase::RegisterProfileL();
    }

// ---------------------------------------------------------------------------
// From class CScpServiceHandlerBase
// CScpImHandler::DisableSubService
// ---------------------------------------------------------------------------
//
TInt CScpImHandler::DisableSubService()
    {
    SCPLOGSTRING4( "CScpImHandler[0x%x]::DisableSubService: 0x%x type: %i", 
                   this, &iSubService, iSubService.SubServiceType() )
    __ASSERT_DEBUG( ECCHIMSub == iSubService.SubServiceType(), User::Panic(
        KNullDesC, KErrGeneral ) );
    
    TInt result( KErrNone );

    TCCHSubserviceState subServiceState = iSubService.State();

    if( ECCHDisabled != subServiceState )
        {
        TRAP( result, DeregisterContactHeaderL() );      

        DeregisterProfile();
        }
    else
        {
        result = KErrNotSupported;
        }

    return result;
    }

// ---------------------------------------------------------------------------
// From class CScpServiceHandlerBase
// CScpImHandler::SubServiceType
// ---------------------------------------------------------------------------
//
TCCHSubserviceType CScpImHandler::SubServiceType() const
    {
    SCPLOGSTRING2( "CScpImHandler[0x%x]::SubServiceType", this )
    return ECCHIMSub;
    }

// ---------------------------------------------------------------------------
// From class CScpServiceHandlerBase
// CScpImHandler::HandleSipConnectionEvent
// ---------------------------------------------------------------------------
//
void CScpImHandler::HandleSipConnectionEvent( TUint32 aProfileId,
    TScpConnectionEvent aEvent )
    {
    SCPLOGSTRING2( "CScpImHandler[0x%x]::HandleSipConnectionEvent", this )
    TRAP_IGNORE( HandleSipConnectionEventL( aProfileId, aEvent ) )
    SCPLOGSTRING( "CScpImHandler::HandleSipConnectionEvent - Out" )
    }

// ---------------------------------------------------------------------------
// Handle SIP connection event
// ---------------------------------------------------------------------------
//
void CScpImHandler::HandleSipConnectionEventL( const TUint32 aProfileId,
    TScpConnectionEvent aEvent )
    {
    SCPLOGSTRING4(
    "CScpImHandler[0x%x]::HandleSipConnectionEventL, profileID: %d event: %d",
        this, aProfileId, aEvent )
    
    if ( iSubService.SipProfileId() == aProfileId &&
        CScpSubService::EScpNoRequest != iSubService.EnableRequestedState() )
        {
        if ( EScpRegistered == aEvent && CScpSubService::EScpEnabled ==
            iSubService.EnableRequestedState() )
            {
            RBuf8 contactHeaderUser;
            contactHeaderUser.CleanupClosePushL();
            GetSipProfileContactHeaderUserL( contactHeaderUser );
            
            TInt32 uniqueId = iSubService.SipProfileId();
            TUid uniqueUid;
            uniqueUid.iUid = uniqueId;
            
            iSipClientResolver->RegisterClientWithUserL(
                uniqueUid, contactHeaderUser, KResolver );
            
            CleanupStack::PopAndDestroy( &contactHeaderUser );
            }
        
        iSubService.HandleConnectionEvent( aEvent );
        }
    }

// ---------------------------------------------------------------------------
// Get SIP profile contact header user
// ---------------------------------------------------------------------------
//
void CScpImHandler::GetSipProfileContactHeaderUserL(
    RBuf8& aContactHeaderUser ) const
    {
    SCPLOGSTRING2( "CScpImHandler[0x%x]::GetSipProfileContactHeaderUserL",
        this )
    
    TUint32 sipProfileId = iSubService.SipProfileId();
    CScpProfileHandler& profileHandler = iSubService.ProfileHandler();
    CScpSipConnection* sipConnection = profileHandler.CreateSipConnectionL(
        sipProfileId );
    CleanupStack::PushL( sipConnection );
    
    if( sipConnection )
        {
        User::LeaveIfError( sipConnection->GetContactHeaderUser(
            aContactHeaderUser ) );
        __ASSERT_DEBUG( aContactHeaderUser.Length() > 0, User::Panic(
            KNullDesC, KErrGeneral ) );
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    
    CleanupStack::PopAndDestroy( sipConnection );
    
    SCPLOGSTRING( "CScpImHandler::GetSipProfileContactHeaderUserL - Out" )
    }

// ---------------------------------------------------------------------------
// Deregister contact header
// ---------------------------------------------------------------------------
//
void CScpImHandler::DeregisterContactHeaderL()
    {
    SCPLOGSTRING2( "CScpImHandler[0x%x]::DeregisterContactHeaderL", this )

    RBuf8 contactHeaderUser;
    contactHeaderUser.CleanupClosePushL();

    GetSipProfileContactHeaderUserL( contactHeaderUser );
    
    TInt32 uniqueId = iSubService.SipProfileId();
    TUid uniqueUid;
    uniqueUid.iUid = uniqueId;
    
    iSipClientResolver->UnRegisterClientWithUserL( 
        uniqueUid,
        contactHeaderUser );
    
    CleanupStack::PopAndDestroy( &contactHeaderUser );
    
    SCPLOGSTRING( "CScpImHandler::DeregisterContactHeaderL - Out" )
    }
