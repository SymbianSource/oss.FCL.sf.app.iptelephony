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
* Description:  Interface to SIP profiles.
*
*/

#include <sipprofileregistry.h>
#include <sipprofile.h>
#include <sipmanagedprofile.h>
#include <sipmanagedprofileregistry.h>
#include <sipconnection.h>
#include <commdb.h>
#include <ccherror.h>

#include "scpsipconnection.h"
#include "scplogger.h"
#include "scpsipconnectionobserver.h"
#include "scputility.h"
#include "voipeventlog.h"

// If EnableL fails a timeout for reporting error
const TInt KEnableTimeout = 120000000;

const TInt KUsernameMaxLength = 255;

// -----------------------------------------------------------------------------
// CScpSipConnection::CScpSipConnection
// -----------------------------------------------------------------------------
//
CScpSipConnection::CScpSipConnection( 
    TInt aProfileId, 
    CSIPProfileRegistry& aProfileRegistry,
    CSIPManagedProfileRegistry& aManagedProfileRegistry,
    CSIP& aSip ) :
    iProfileId( aProfileId ),
    iProfileRegistry( aProfileRegistry ),
    iManagedProfileRegistry( aManagedProfileRegistry ),
    iSip( aSip ),
    iRegistrationRequestState( ENoRequest ),
    iConnectionStateError( KErrNone ),
    iReserved( EFalse ),
    iIapAvailableOffered( EFalse )
    {
    SCPLOGSTRING3( "CScpSipConnection[0x%x]::CScpSipConnection profile id: %d", 
                   this, aProfileId );
    }

// -----------------------------------------------------------------------------
// CScpSipConnection::ConstructL
// -----------------------------------------------------------------------------
//
void CScpSipConnection::ConstructL()
    {
    SCPLOGSTRING2( "CScpSipConnection[0x%x]::ConstructL", this );

    iSipProfile = iProfileRegistry.ProfileL( iProfileId );

    if ( !iSipProfile )
        {
        User::Leave( KErrNotFound );
        }
    
    iEnableTimeoutTimer = CPeriodic::NewL( CPeriodic::EPriorityStandard );
    
    User::LeaveIfError( iConnectionMonitor.ConnectL() );
    }

// -----------------------------------------------------------------------------
// CScpSipConnection::NewL
// -----------------------------------------------------------------------------
//
CScpSipConnection* CScpSipConnection::NewL( 
    TInt aProfileId, 
    CSIPProfileRegistry& aProfileRegistry,
    CSIPManagedProfileRegistry& aManagedProfileRegistry,
    CSIP& aSip )
    {
    SCPLOGSTRING( "CScpSipConnection::NewL" );
    __ASSERT_DEBUG( aProfileId > 0, User::Panic( KNullDesC, KErrGeneral ) );

    CScpSipConnection* self = new (ELeave) CScpSipConnection( aProfileId,
                                                              aProfileRegistry,
                                                              aManagedProfileRegistry,
                                                              aSip );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }
    
// -----------------------------------------------------------------------------
// CScpSipConnection::~CScpSipConnection
// -----------------------------------------------------------------------------
//
CScpSipConnection::~CScpSipConnection()
    {  
    SCPLOGSTRING2( "CScpSipConnection[0x%x]::~CScpSipConnection", this );

    delete iSipConnection;
    iConnectionMonitor.Close();
    delete iEnableTimeoutTimer;
    delete iSipProfile;
    }

// -----------------------------------------------------------------------------
// CScpSipConnection::AddObserver
// -----------------------------------------------------------------------------
//
void CScpSipConnection::AddObserver( MScpSipConnectionObserver& aObserver )
    {
    SCPLOGSTRING3( "CScpSipConnection[0x%x]::AddObserverL: 0x%x",this, &aObserver );

    iObserver = &aObserver;
    } 

// -----------------------------------------------------------------------------
// CScpSipConnection::RemoveObserver
// -----------------------------------------------------------------------------
//
TInt CScpSipConnection::RemoveObserver( MScpSipConnectionObserver& aObserver )
    {
    SCPLOGSTRING3( "CScpSipConnection[0x%x]::RemoveObserver: 0x%x", this, &aObserver );

    if ( &aObserver == iObserver )
        {
        iObserver = NULL;
        return KErrNone;
        }

    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CScpSipConnection::ProfileId
// -----------------------------------------------------------------------------
//
TInt CScpSipConnection::ProfileId() const
    {
    SCPLOGSTRING2( "CScpSipConnection[0x%x]::ProfileId", this );

    return iProfileId;
    }

// -----------------------------------------------------------------------------
// CScpSipConnection::EnableL
// -----------------------------------------------------------------------------
//
void CScpSipConnection::EnableL()
    {
    SCPLOGSTRING2( "CScpSipConnection[0x%x]::EnableL", this );

    if ( iRegistrationRequestState != ERegistrationRequested )
        {
        iConnectionStateError = KErrNone;
        iRegistrationRequestState = ERegistrationRequested;

        TUint32 iapId( 0 );
        TUint32 snapId( 0 );

        TInt result = GetSnap( snapId );

        if ( result == KErrNotFound || snapId == 0 )
            {
            SCPLOGSTRING2( "CScpSipConnection[0x%x]::EnableL - SNAP not found", this );
            result = GetIap( iapId );
            }
   
        // If username is not set leave directly with KCCHErrorAuthenticationFailed
        if ( !IsUsernameSetL() )
            {
            SCPLOGSTRING2( "CScpSipConnection[0x%x]::EnableL - AuthenticationFailed !!!", this );
            User::Leave( KCCHErrorAuthenticationFailed );
            }
        
        iProfileRegistry.EnableL( *iSipProfile, *this );
          
        if ( !iSipConnection && 0 != iapId )
             {
             iSipConnection = CSIPConnection::NewL( iSip, iapId, *this );
             }
        
        TBool connectionAvailable = IsNetworkConnectionAvailable();
        
        if ( connectionAvailable )
            {
            TBool registered( EFalse );
            User::LeaveIfError( iSipProfile->GetParameter( KSIPProfileRegistered, 
                                                           registered ) );

            if ( !registered )
                {
                StartEnableTimeoutTimer( CScpSipConnection::EnableTimeout );
                }
            }        
        else
            {
            iConnectionStateError = KCCHErrorNetworkLost;
            }
        }
    }

// -----------------------------------------------------------------------------
// CScpSipConnection::Disable
// -----------------------------------------------------------------------------
//
TInt CScpSipConnection::Disable()
    {
    SCPLOGSTRING2( "CScpSipConnection[0x%x]::Disable", this );

    iRegistrationRequestState = EDeregistrationRequested;
    
    CancelEnableTimeoutTimer();
    
    delete iSipConnection;
    iSipConnection = NULL;
    
    TInt err = iProfileRegistry.Disable( *iSipProfile );
    TRAPD( err1, DisableAlwaysOnModeL() );
       
    if ( err == KErrNone )
        {
        err = err1;
        }

    return err;
    }

// -----------------------------------------------------------------------------
// CScpSipConnection::DisableAlwaysOnModeL()
// -----------------------------------------------------------------------------
//
void CScpSipConnection::DisableAlwaysOnModeL()
    {
    SCPLOGSTRING2( "CScpSipConnection[0x%x]::DisableAlwaysOnModeL", this );

    TBool autoRegistration( EFalse );
    User::LeaveIfError( iSipProfile->GetParameter( KSIPAutoRegistration, 
                                                   autoRegistration ) );

    if ( autoRegistration )
        {
        CSIPManagedProfile* profile = static_cast<CSIPManagedProfile*> ( iSipProfile );
        User::LeaveIfError( profile->SetParameter( KSIPAutoRegistration, EFalse ) );
        iManagedProfileRegistry.SaveL( *profile );
        }
    }

// -----------------------------------------------------------------------------
// CScpSipConnection::RegistrationRequestState
// -----------------------------------------------------------------------------
//
CScpSipConnection::TRegistrationRequestState 
    CScpSipConnection::RegistrationRequestState() const
    {
    SCPLOGSTRING2( "CScpSipConnection[0x%x]::RegistrationRequestState", this );

    return iRegistrationRequestState;
    }

// -----------------------------------------------------------------------------
// CScpSipConnection::GetState
// -----------------------------------------------------------------------------
//
void CScpSipConnection::GetState( CScpSipConnection::TConnectionState& aState,
                                  TInt& aError ) const
    {
    SCPLOGSTRING2( "CScpSipConnection[0x%x]::GetState", this );

    aError = iConnectionStateError;

    TBool registered( EFalse );
    TInt result = iSipProfile->GetParameter( KSIPProfileRegistered, registered );
    
    if ( result == KErrNone )
        {
        switch ( iRegistrationRequestState )
            {
            case ENoRequest:
                if( registered )
                    {
                    aState = ERegistered;
                    }
                else
                    {
                    aState = EDeregistered;
                    }
                break;

            case ERegistrationRequested:
                if ( registered )
                    {
                    aState = ERegistered;
                    }
                else
                    {
                    aState = ERegistering;
                    }
                break;

            case EDeregistrationRequested:
                if ( registered )
                    {
                    aState = EDeregistering;
                    }
                else
                    {
                    aState = EDeregistered;
                    }
                break;

            default:
                __ASSERT_DEBUG( EFalse, User::Panic( KNullDesC, KErrGeneral ) );
                break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CScpSipConnection::ProfileRegistryEventOccurred
// -----------------------------------------------------------------------------
//
void CScpSipConnection::ProfileRegistryEventOccurred( 
    MSIPProfileRegistryObserver::TEvent aEvent )
    {
    SCPLOGSTRING3( "CScpSipConnection[0x%x]::ProfileRegistryEventOccurred event: %d", 
                   this, aEvent );
    
    TScpConnectionEvent event( EScpUnknown );
    
    switch ( aEvent )
        {
        case MSIPProfileRegistryObserver::EProfileRegistered:
            {
            CancelEnableTimeoutTimer();
            
            delete iSipConnection;
            iSipConnection = NULL;
            
            iConnectionStateError = KErrNone;
            event = EScpRegistered;
            }
            break;

        case MSIPProfileRegistryObserver::EProfileDeregistered:
            {
            CancelEnableTimeoutTimer();
                
            if ( iRegistrationRequestState == EDeregistrationRequested )
                {
                iConnectionStateError = KErrNone;
                event = EScpDeregistered;            
                }
            else
                {

                // SIP will still notify the client with 
                // MSIPProfileRegistryObserver::ProfileRegistryEventOccurred( EProfileDeregistered ) 
                // but the reason for this event can be checked by the client
                // by calling CSIPProfileRegistryBase::LastRegistrationError.
                // If this function returns KErrDisconnected,
                // the client can determine that the reason for the event was that the network connection was dropped.

                TInt lastRegistrationError =
                    iProfileRegistry.LastRegistrationError( *iSipProfile );
                
                SCPLOGSTRING2( "    -> last registration error: %d", lastRegistrationError );
                
                if (  KErrDisconnected != lastRegistrationError )
                    {
                    if ( iProfileCurrentlyRoaming )
                        {
                        iProfileCurrentlyRoaming = EFalse;
                        iConnectionStateError = KCCHErrorNetworkLost;
                        event = EScpNetworkLost;
                        }
                    else
                        {
                        iConnectionStateError = KErrNone;
                        event = EScpDeregistered;
                        } 
                    }
                else
                    {
                    iConnectionStateError = KCCHErrorNetworkLost;
                    event = EScpNetworkLost;
                    }
                }
            }
            break;

        case MSIPProfileRegistryObserver::EProfileUpdated:
            {
            // Should we start EnableTimeoutTimer if registration
            // was requested?
            
            if ( iProfileCurrentlyRoaming )
                {
                iProfileCurrentlyRoaming = EFalse;
                iConnectionStateError = KErrNone;
                event = EScpRegistered;
                }
            }
            break;
            
        default:
            break;
        }

    if ( event != EScpUnknown && iObserver )
        {
        SCPLOGSTRING3( "id: %d HandleSipConnectionEvent event: %d", 
                       iProfileId, event );

        iObserver->HandleSipConnectionEvent( iProfileId, event );
        }
    }

// -----------------------------------------------------------------------------
// CScpSipConnection::ProfileRegistryErrorOccurred
// -----------------------------------------------------------------------------
//
void CScpSipConnection::ProfileRegistryErrorOccurred( TInt aError )
    {
    SCPLOGSTRING3( "CScpSipConnection[0x%x]::ProfileRegistryErrorOccurred error: %d", 
                   this, aError );
    
    CancelEnableTimeoutTimer();

    iConnectionStateError = TScpUtility::ConvertToCchError( aError );
    TScpConnectionEvent event = EScpUnknown;
    if ( iConnectionStateError == KCCHErrorAuthenticationFailed )
        {
        event = EScpAuthenticationFailed;
        }
    else if ( iConnectionStateError == KCCHErrorInvalidSettings )
        {
        event = EScpInvalidSettings;
        }
    else
        {
        if ( KErrCancel == aError )
            {
            iConnectionStateError = KErrCancel;
            event = EScpRegistrationCanceled;
            iRegistrationRequestState = EDeregistrationRequested;
            }
        else
            {
            iConnectionStateError = KCCHErrorLoginFailed;
            event = EScpRegistrationFailed;
            }
        }    
    
    TInt error = KErrNone;
    CVoipEventLog* eventLog = NULL;
    
    TRAP( error, eventLog = CVoipEventLog::NewL() );
    
    if ( KErrNone == error )
        {
        eventLog->WriteError( aError );
        }
    
    delete eventLog;
    
    iObserver->HandleSipConnectionEvent( iProfileId, event );    
    }

// -----------------------------------------------------------------------------
// CScpSipConnection::StartEnableTimeoutTimer
// -----------------------------------------------------------------------------
//
void CScpSipConnection::StartEnableTimeoutTimer( TInt (*aFunction)(TAny* aPtr) )
    {
    SCPLOGSTRING2( "CScpSipConnection[0x%x]::StartEnableTimeoutTimer", this );
    __ASSERT_DEBUG( !iEnableTimeoutTimer->IsActive(), 
                    User::Panic( KNullDesC, KErrGeneral ) );

    if ( !iEnableTimeoutTimer->IsActive() )
        {
        iEnableTimeoutTimer->Start( KEnableTimeout, 
                              0, 
                              TCallBack( aFunction, this ) );
        }
    }

// -----------------------------------------------------------------------------
// CScpSipConnection::CancelEnableTimeoutTimer
// -----------------------------------------------------------------------------
//
void CScpSipConnection::CancelEnableTimeoutTimer()
    {
    SCPLOGSTRING2( "CScpSipConnection[0x%x]::CancelEnableTimeoutTimer", this );

    if ( iEnableTimeoutTimer->IsActive() )
        {
        iEnableTimeoutTimer->Cancel();
        }
    }

// -----------------------------------------------------------------------------
// CScpSipConnection::EnableTimeout
// -----------------------------------------------------------------------------
//
TInt CScpSipConnection::EnableTimeout( TAny* aSelf )
    {
    SCPLOGSTRING( "CScpSipConnection::EnableTimeout" );

    CScpSipConnection* self = static_cast<CScpSipConnection*>( aSelf );
    
    self->HandleEnableTimeout();

    return 0;
    }

// -----------------------------------------------------------------------------
// CScpSipConnection::HandleEnableTimeout
// -----------------------------------------------------------------------------
//
void CScpSipConnection::HandleEnableTimeout()
    {
    SCPLOGSTRING2( "CScpSipConnection[0x%x]::HandleEnableTimeout",
                   this );
    ProfileRegistryErrorOccurred( EScpRegistrationFailed );
    }

// -----------------------------------------------------------------------------
// CScpSipConnection::GetIap
// -----------------------------------------------------------------------------
//
TInt CScpSipConnection::GetIap( TUint32& aIapId ) const
    {
    SCPLOGSTRING2( "CScpSipConnection[0x%x]::GetIap", this );
    
    return iSipProfile->GetParameter( KSIPAccessPointId, aIapId ); 
    }

// -----------------------------------------------------------------------------
// CScpSipConnection::GetSnap
// -----------------------------------------------------------------------------
//
TInt CScpSipConnection::GetSnap( TUint32& aSnapId ) const
    {
    SCPLOGSTRING2( "CScpSipConnection[0x%x]::GetSnap", this );

    return iSipProfile->GetParameter( KSIPSnapId, aSnapId );          
    }

// -----------------------------------------------------------------------------
// CScpSipConnection::GetUsername
// -----------------------------------------------------------------------------
//
TInt CScpSipConnection::GetUsername( TDes8& aUsername ) const
    {
    SCPLOGSTRING2( "CScpSipConnection[0x%x]::GetUsername", this );

    TInt result( KErrNotFound );
    const TDesC8* aor( NULL );

    // Get SIP's Username    
    result = iSipProfile->GetParameter( KSIPUserAor, aor );
    
    if ( result == KErrNone && aor )
        {
        if ( aor->Length() <= aUsername.MaxLength() )
            {
            aUsername.Copy( *aor );
            }
        else
            {
            result = KErrOverflow;
            }
        }

    return result;                
    }

// -----------------------------------------------------------------------------
// CScpSipConnection::GetDomain
// -----------------------------------------------------------------------------
//
TInt CScpSipConnection::GetDomain( TDes8& aDomain ) const
    {
    SCPLOGSTRING2( "CScpSipConnection[0x%x]::GetDomain", this );

    TInt result( KErrNotFound );
    const TDesC8* domain( NULL );

    // Get SIP's Username    
    result = iSipProfile->GetParameter( KSIPRegistrar, KSIPServerAddress, domain );
    
    if ( result == KErrNone && domain )
        {
        if ( domain->Length() <= aDomain.MaxLength() )
            {
            aDomain.Copy( *domain );
            }
        else
            {
            result = KErrOverflow;
            }
        }

    return result;                
    }

// -----------------------------------------------------------------------------
// CScpSipConnection::GetContactHeaderUser
// -----------------------------------------------------------------------------
//
TInt CScpSipConnection::GetContactHeaderUser( RBuf8& aContactHeaderUser ) const
    {
    SCPLOGSTRING2( "CScpSipConnection[0x%x]::GetContactHeader", this );

    TInt result( KErrNotFound );
    const TDesC8* contactHeaderUser( NULL );

    // Get SIP's Username    
    result = iSipProfile->GetParameter( KSIPContactHeaderUser, contactHeaderUser );
    
    if ( result == KErrNone && contactHeaderUser )
        {
        result = aContactHeaderUser.ReAlloc( contactHeaderUser->Length() );
        if ( KErrNone == result )
            {
            aContactHeaderUser.Copy( *contactHeaderUser );
            }
        }

    return result;                
    }

// -----------------------------------------------------------------------------
// CScpSipConnection::SipProfile
// -----------------------------------------------------------------------------
//
CSIPProfile& CScpSipConnection::SipProfile() const
    {
    SCPLOGSTRING2( "CScpSipConnection[0x%x]::SipProfile", this );
    
    return *iSipProfile;
    }

// -----------------------------------------------------------------------------
// CScpSipConnection::SetIap
// -----------------------------------------------------------------------------
//
TInt CScpSipConnection::SetIap( TUint32 aProfileIapId )
    {
    SCPLOGSTRING3( "CScpSipConnection[0x%x]::SetIap iap: %d",
                   this, aProfileIapId );

    TInt result( KErrNone );

    // If snap exists, iap doesn't have an effect
    TUint32 snapId( 0 );
    result = GetSnap( snapId );

    if ( result == KErrNotFound ||
        snapId == 0 )
        {
        // Managed profile so we can change it
        CSIPManagedProfile* profile = static_cast<CSIPManagedProfile*> ( iSipProfile );
        
        // Add new IAP Id to SIP Profile
        result = profile->SetParameter( KSIPAccessPointId, aProfileIapId );
        if ( KErrNone == result )
            {
            // Save changes
            TRAP( result, iManagedProfileRegistry.SaveL( *profile ) );
            }
        }
    else
        {
        result = KErrNotSupported;
        }

    return result; 
    }

// -----------------------------------------------------------------------------
// CScpSipConnection::SetIap
// -----------------------------------------------------------------------------
//
TInt CScpSipConnection::SetSnap( TUint32 aProfileSnapId )
    {
    SCPLOGSTRING3( "CScpSipConnection[0x%x]::SetSnap snap: %d", 
                   this, aProfileSnapId );
        
    // Managed profile so we can change it
    CSIPManagedProfile* profile = static_cast<CSIPManagedProfile*> ( iSipProfile );
    
    // Add new SNAP Id to SIP Profile
    TInt result = profile->SetParameter( KSIPSnapId, 
                                         aProfileSnapId );

    if ( KErrNone == result )
        {
        // Save changes
        TRAP( result, iManagedProfileRegistry.SaveL( *profile ) );
        }

    return result; 
    }

// -----------------------------------------------------------------------------
// CScpSipConnection::GetIapTypeL
// -----------------------------------------------------------------------------
//
TScpIapType CScpSipConnection::GetIapTypeL() const
    {
    SCPLOGSTRING2( "CScpSipConnection[0x%x]::GetIapTypeL", this );
    
    TScpIapType type = EScpUnknownType;

    CCommsDatabase* commsDb = CCommsDatabase::NewL( EDatabaseTypeIAP );
    CleanupStack::PushL( commsDb );

    CCommsDbTableView* iapTable = commsDb->OpenTableLC( TPtrC( IAP ) );

    if ( iapTable->GotoFirstRecord() == KErrNone )
        { 
        TUint32 profilesIap = 0;
        User::LeaveIfError( GetIap( profilesIap ) );

        do 
            {
            TUint32 uid = 0;
            iapTable->ReadUintL( TPtrC( COMMDB_ID ), uid );

            if ( uid == profilesIap )
                {
                // The length should be defined somewhere
                TBuf<256> serviceType;
                iapTable->ReadTextL( TPtrC( IAP_SERVICE_TYPE ), serviceType );

                if ( serviceType == TPtrC( INCOMING_GPRS ) ||
                    serviceType == TPtrC( OUTGOING_GPRS ) ||
                    serviceType == TPtrC( INCOMING_WCDMA) ||
                    serviceType == TPtrC( OUTGOING_WCDMA) )
                    {
                    type = EScpGprs;
                    }
                else if ( serviceType == TPtrC( LAN_SERVICE ) )
                    {
                    type = EScpWlan;
                    }
                else
                    {
                    type = EScpUnknownType;
                    }

                break;
                }

            }
        while( iapTable->GotoNextRecord() == KErrNone );
    }

    CleanupStack::PopAndDestroy( iapTable );
    CleanupStack::PopAndDestroy( commsDb );    

    SCPLOGSTRING2( "GetIapTypeL result: %d", type );

    return type;
    }

// -----------------------------------------------------------------------------
// CScpSipConnection::SetReserved
// -----------------------------------------------------------------------------
//
void CScpSipConnection::SetReserved( TBool aReserved )
    {
    SCPLOGSTRING3( "CScpSipConnection[0x%x]::SetReserved: %d", this, aReserved );
    
    iReserved = aReserved;
    }

// -----------------------------------------------------------------------------
// CScpSipConnection::Reserved
// -----------------------------------------------------------------------------
//
TBool CScpSipConnection::Reserved() const
    {
    SCPLOGSTRING3( "CScpSipConnection[0x%x]::Reserved: %d", this, iReserved );

    return iReserved;
    }

// -----------------------------------------------------------------------------
// CScpSipConnection::SetIapAvailableOffered
// -----------------------------------------------------------------------------
//
void CScpSipConnection::SetIapAvailableOffered( TBool aIapAvailableOffered )   
    {
    SCPLOGSTRING3( "CScpSipConnection[0x%x]::SetIapAvailableOffered: %d", 
    this, aIapAvailableOffered );
    
    iIapAvailableOffered = aIapAvailableOffered;
    }

// -----------------------------------------------------------------------------
// CScpSipConnection::IapAvailableOffered
// -----------------------------------------------------------------------------
//
TBool CScpSipConnection::IapAvailableOffered() const
    {
    SCPLOGSTRING3( "CScpSipConnection[0x%x]::IapAvaiableOffered: %d", 
    this, iIapAvailableOffered );
    
    return iIapAvailableOffered;
    }

// -----------------------------------------------------------------------------
// CScpSipConnection::IapAvailableOffered
// -----------------------------------------------------------------------------
//
void CScpSipConnection::SetProfileCurrentlyRoaming()
    {
    SCPLOGSTRING( "CScpSipConnection[0x%x]::SetProfileCurrentlyRoaming" );
    
    iProfileCurrentlyRoaming = ETrue;
    }

// -----------------------------------------------------------------------------
// CScpSipConnection::IsPasswordSet
// -----------------------------------------------------------------------------
//
TBool CScpSipConnection::IsPasswordSet() const
    {
    SCPLOGSTRING( "CScpSipConnection::IsPasswordSet" );

    // no support exist in sip for getting is password set information */
    SCPLOGSTRING( "CScpSipConnection::IsPasswordSet: pwd is set is always returned" );
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CScpSipConnection::IsUsernameSetL
// -----------------------------------------------------------------------------
//
TBool CScpSipConnection::IsUsernameSetL() const
    {
    SCPLOGSTRING( "CScpSipConnection::IsUsernameSetL" );
 
    TBool userNameSet( EFalse );
    
    // check from aor
    RBuf8 username;
    CleanupClosePushL( username );
    username.CreateL( KUsernameMaxLength );
    
    TInt err = GetUsername( username );

    if ( !err && username.Length() )
        {
        RBuf8 formattedUsername;
        TScpUtility::RemovePrefixAndDomain( username, formattedUsername );
        
        if ( formattedUsername.Length() )
            {
            userNameSet = ETrue;
            }   
        
        formattedUsername.Zero();
        formattedUsername.Close();
        }
    else if ( KErrNone == err && SipProfileIMS() )
        {
        SCPLOGSTRING( "CScpSipConnection::IsUsernameSetL - SipProfileIMS" );
        // Check if this is IMS case; then we can think username is set
        // (SIP stack takes care of SIM handling)
        userNameSet = ETrue;
        }
    
    else
        {
        SCPLOGSTRING( "CScpSipConnection::IsUsernameSetL - ELSE!" );
        }
        
    CleanupStack::PopAndDestroy( &username );
    
    return userNameSet;
    }

// -----------------------------------------------------------------------------
// CScpSipConnection::SipProfileIMS
// -----------------------------------------------------------------------------
//
TBool CScpSipConnection::SipProfileIMS() const
    {
    SCPLOGSTRING( "CScpSipConnection::SipProfileIMS" );
    if( TSIPProfileTypeInfo::EIms == iSipProfile->Type().iSIPProfileClass )
        {
        SCPLOGSTRING( "CScpSipConnection::SipProfileIMS - ETrue" );
        return ETrue;
        }
    else
        {
        SCPLOGSTRING( "CScpSipConnection::SipProfileIMS - EFalse" );
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CScpSipConnection::HandleMigrationStarted
// -----------------------------------------------------------------------------
//
void CScpSipConnection::HandleMigrationStarted()
    {
    if( iObserver )
        {
        SCPLOGSTRING2( "    -> roaming - id: %d", iProfileId );

        iObserver->HandleSipConnectionEvent( iProfileId, EScpRoaming );
        }
    }

// -----------------------------------------------------------------------------
// CScpSipConnection::HandleMigrationError
// -----------------------------------------------------------------------------
//
void CScpSipConnection::HandleMigrationError(
    TInt /*aError*/,
    TUint32 /*aProfileId*/,
    TUint32 /*aSnapId*/,
    TUint32 /*aIapId*/ )
    {
    // Migration has failed -> check is sip still registered via old iap 
    // and if is, set state back to registered
    
    TScpConnectionEvent event( EScpUnknown );
    
    if ( iProfileCurrentlyRoaming && iObserver )
        {
        iProfileCurrentlyRoaming = EFalse;
        iConnectionStateError = KErrNone;
        event = EScpRegistered;
        SCPLOGSTRING3( "id: %d HandleSipConnectionEvent event: %d", 
                       iProfileId, event );
        iObserver->HandleSipConnectionEvent( iProfileId, event );
        }
    }

// -----------------------------------------------------------------------------
// CScpSipConnection::IsNetworkConnectionAvailable
// -----------------------------------------------------------------------------
//
TBool CScpSipConnection::IsNetworkConnectionAvailable() const
    {
    SCPLOGSTRING2( "CScpSipConnection[0x%x]::IsNetworkConnectionAvailable", this );

    TBool isAvailable = EFalse;

    TUint32 snapId = 0;
    TInt result = GetSnap( snapId );

    if ( result == KErrNone ||
        result == KErrNotFound )
        {
        if ( snapId != 0 )
            {
            isAvailable = IsSnapConnectionAvailable( snapId );
            }   
        else
            {
            TUint32 iapId = 0;
            result = GetIap( iapId );

            if( result == KErrNone )
                {
                isAvailable = IsIapConnectionAvailable( iapId );
                }
            }
        }
    
    return isAvailable;
    }

// -----------------------------------------------------------------------------
// CScpSipConnection::IsSnapConnectionAvailable
// -----------------------------------------------------------------------------
//
TBool CScpSipConnection::IsSnapConnectionAvailable( TInt aSnapId ) const
    {
    SCPLOGSTRING2( "CScpSipConnection[0x%x]::IsSnapConnectionAvailable", this );

    TBool isAvailable = EFalse;

    TConnMonSNAPInfoBuf snapInfoBuf;
    TRequestStatus status;
    iConnectionMonitor.GetPckgAttribute( EBearerIdAll, 0, KSNAPsAvailability,
                                         snapInfoBuf, status );

    User::WaitForRequest( status );

    for ( TInt i = 0; i < snapInfoBuf().Count(); i++ )
        {
        if ( aSnapId == snapInfoBuf().iSNAP[ i ].iSNAPId )
            {
            SCPLOGSTRING( "SNAP IS STILL ALIVE!!!" );
            isAvailable = ETrue;
            break;
            } 
        }

    return isAvailable;
    }

// -----------------------------------------------------------------------------
// CScpSipConnection::IsIapConnectionAvailable
// -----------------------------------------------------------------------------
//
TBool CScpSipConnection::IsIapConnectionAvailable( TInt aIapId ) const
    {
    SCPLOGSTRING2( "CScpSipConnection[0x%x]::IsIapConnectionAvailable", this );

    TBool isAvailable = EFalse;

    TConnMonIapInfoBuf iapInfoBuf;
    TRequestStatus status;
    iConnectionMonitor.GetPckgAttribute( EBearerIdAll, 0, KIapAvailability,
                                         iapInfoBuf, status );

    User::WaitForRequest( status );

    for ( TInt i = 0; i < iapInfoBuf().Count(); i++ )
        {
        if  ( aIapId == iapInfoBuf().iIap[ i ].iIapId )
            {
            SCPLOGSTRING( "IAP IS STILL ALIVE!!!" );
            isAvailable = ETrue;
            break;
            } 
        }

    return isAvailable;
    }

// -----------------------------------------------------------------------------
// CScpSipConnection::ConnectionStateChanged
// -----------------------------------------------------------------------------
//
void CScpSipConnection::ConnectionStateChanged( CSIPConnection::TState aState )
    {
    SCPLOGSTRING2( "CScpSipConnection[0x%x]::ConnectionStateChanged - IN", this );
    
    switch ( aState )
            {
            case CSIPConnection::EInactive:
                {
                SCPLOGSTRING2( "CScpSipConnection[0x%x]::ConnectionStateChanged: EInactive", this );
                
                TUint32 iapId( 0 );
                TInt result = GetIap( iapId );
                    
                if ( KErrNone == result && iSipConnection &&
                        iapId == iSipConnection->IapId() &&
                        KErrNone == iConnectionStateError )
                    {
                    delete iSipConnection;
                    iSipConnection = NULL;
                    
                    iConnectionStateError = KCCHErrorLoginFailed;
                    iObserver->HandleSipConnectionEvent( iProfileId, EScpRegistrationFailed );
                    }
                }
                break;
                
            default:
                {
                SCPLOGSTRING2( "CScpSipConnection[0x%x]::ConnectionStateChanged: default", this );
                }
                break;
            }
    SCPLOGSTRING2( "CScpSipConnection[0x%x]::ConnectionStateChanged - OUT", this );
    }

//  End of File  
