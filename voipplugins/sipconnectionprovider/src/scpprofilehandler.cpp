/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include <sip.h>
#include <sipprofileregistry.h>
#include <sipprofilealrcontroller.h>
#include <sipprofile.h>
#include <sipmanagedprofile.h>
#include <sipmanagedprofileregistry.h>
#include <ctsydomainpskeys.h>
#include <bamdesca.h>
#include <sipprofileregistryobserver.h> 
#include "ipvmbxinterface.h"
#include "scpprofilehandler.h"
#include "scplogger.h"
#include "scpsipconnection.h"
#include "cipappphoneutils.h"
#include "cipapputilsaddressresolver.h"

#include "scputility.h"

const TInt KTempStringlength = 200;
const TUint KSCPUserAgentStringLength = 12;
const TUint KSCPWlanMacAddressLength = 50;
const TUint KSCPMaxTerminalTypeLength = 64;
_LIT( KScpUserAgentString, "User-Agent: " );
_LIT8 ( KSCPWlanMacAddressFrmt, "-" );

// -----------------------------------------------------------------------------
// CScpProfileHandler::CScpProfileHandler
// -----------------------------------------------------------------------------
//
CScpProfileHandler::CScpProfileHandler()
    {
    }

// -----------------------------------------------------------------------------
// CScpProfileHandler::ConstructL
// -----------------------------------------------------------------------------
//
void CScpProfileHandler::ConstructL()
    {
    SCPLOGSTRING( "CScpProfileHandler::ConstructL" );

    iSip = CSIP::NewL( TUid::Null(), *this );
    iProfileRegistry = CSIPProfileRegistry::NewL( *iSip, *this );
    iManagedProfileRegistry = CSIPManagedProfileRegistry::NewL( *this );
    iAlrController = CSipProfileAlrController::NewL( *iProfileRegistry,
                                                     *this );
    }

// -----------------------------------------------------------------------------
// CScpProfileHandler::NewL
// -----------------------------------------------------------------------------
//
CScpProfileHandler* CScpProfileHandler::NewL()
    {
    CScpProfileHandler* self =  new ( ELeave ) CScpProfileHandler();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
 
// -----------------------------------------------------------------------------
// CScpProfileHandler::~CScpProfileHandler
// -----------------------------------------------------------------------------
//
CScpProfileHandler::~CScpProfileHandler()
    {  
    SCPLOGSTRING( "CScpProfileHandler::~CScpProfileHandler" );

    iObservers.Close();
    iSipConnections.ResetAndDestroy();
    iSipConnections.Close();

    delete iVmbxInterface;
    delete iAlrController;
    delete iManagedProfileRegistry;
    delete iProfileRegistry;    
    delete iSip;
    }

// -----------------------------------------------------------------------------
// CScpProfileHandler::AddObserverL
// -----------------------------------------------------------------------------
//
void CScpProfileHandler::AddObserverL( MScpSipConnectionObserver& aObserver )
    {
    SCPLOGSTRING2( "CScpProfileHandler::AddObserverL: 0x%x", &aObserver );

    TInt result = iObservers.Find( &aObserver );

    if ( result == KErrNotFound )
        {
        iObservers.AppendL( &aObserver );
        }
    else
        {
        User::Leave( KErrAlreadyExists );
        }
    }
    
// -----------------------------------------------------------------------------
// CScpProfileHandler::RemoveObserver
// -----------------------------------------------------------------------------
//
TInt CScpProfileHandler::RemoveObserver( MScpSipConnectionObserver& aObserver )
    {
    SCPLOGSTRING2( "CScpProfileHandler::RemoveObserver: 0x%x", &aObserver );

    TInt result = iObservers.Find( &aObserver );
    
    if ( result != KErrNotFound )
        {
        iObservers.Remove( result );
        return KErrNone;
        }

    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CScpProfileHandler::GetCurrentState
// -----------------------------------------------------------------------------
//
TInt CScpProfileHandler::GetCurrentState( TUint32 aProfileId, 
    CScpSipConnection::TConnectionState& aState,
    TInt& aError ) const
    {
    SCPLOGSTRING( "CScpProfileHandler::GetCurrentState" );

    TInt result( KErrNone );

    // Go through own sip profiles and check state
    // If not found, create SipConnection and check state
    CScpSipConnection* sipConnection = GetSipConnection( aProfileId );

    TBool sipConnectionCreated = EFalse;

    if( !sipConnection )
        {
        sipConnectionCreated = ETrue;
        TRAP( result, sipConnection = CreateSipConnectionL( aProfileId ) );
        }

    if( result == KErrNone )
        {
        sipConnection->GetState( aState, aError );
        }

    if( sipConnectionCreated )
        {
        delete sipConnection;
        }
       
    return result;
    }

// -----------------------------------------------------------------------------
// CScpProfileHandler::RegisterProfileL
// -----------------------------------------------------------------------------
//
void CScpProfileHandler::RegisterProfileL( TUint32 aProfileId )
    {
    SCPLOGSTRING2("CScpProfileHandler::RegisterProfileL profile id: %d", aProfileId );

    // Check if sip connection already exists
    TBool found( EFalse );
    CScpSipConnection* sipConnection( NULL );
    
    for( TInt i=0; i<iSipConnections.Count(); i++ )
        {
        sipConnection = iSipConnections[ i ];

        if( sipConnection->ProfileId() == aProfileId )
            {
            found = ETrue;
            sipConnection->EnableL();
            break;
            }    
        }

    // Otherwise create it
    if( !found )
        {
        sipConnection = CreateSipConnectionL( aProfileId );
        CleanupStack::PushL( sipConnection );        
        // try enabling
        sipConnection->AddObserver( *this );
        sipConnection->EnableL();
        // add to array
        iSipConnections.AppendL( sipConnection );
        CleanupStack::Pop( sipConnection );
        }
    }

// -----------------------------------------------------------------------------
// CScpProfileHandler::SipConnectionExists
// -----------------------------------------------------------------------------
//
TBool CScpProfileHandler::SipConnectionExists( TUint32 aProfileId ) const
    {
    SCPLOGSTRING2("CScpProfileHandler::SipConnectionExists profile id: %d", aProfileId );

    return ( GetSipConnection( aProfileId ) != NULL );
    }

// -----------------------------------------------------------------------------
// CScpProfileHandler::GetSipConnection
// -----------------------------------------------------------------------------
//
CScpSipConnection* CScpProfileHandler::GetSipConnection( TUint32 aProfileId ) const
    {
    SCPLOGSTRING2("CScpProfileHandler::GetSipConnection profile id: %d", aProfileId );

    CScpSipConnection* sipConnection( NULL );
    
    for( TInt i=0; i<iSipConnections.Count(); i++ )
        {
        sipConnection = iSipConnections[ i ];

        if( sipConnection->ProfileId() == aProfileId )
            {
            return sipConnection;
            }    
        }

    return NULL;
    }

// -----------------------------------------------------------------------------
// CScpProfileHandler::CreateSipConnectionL
// -----------------------------------------------------------------------------
//
CScpSipConnection* CScpProfileHandler::CreateSipConnectionL( TUint32 aProfileId ) const
    {
    SCPLOGSTRING2("CScpProfileHandler::CreateSipConnectionL profile: %d", aProfileId);

    return CScpSipConnection::NewL( aProfileId,
                                    *iProfileRegistry,
                                    *iManagedProfileRegistry,
                                    *iSip );
    }

// -----------------------------------------------------------------------------
// CScpProfileHandler::UnRegisterProfile
// -----------------------------------------------------------------------------
//
TInt CScpProfileHandler::UnregisterProfile( TUint32 aProfileId )
    {
    SCPLOGSTRING2("CScpProfileHandler::UnregisterProfile id: %d", aProfileId );

    CScpSipConnection* sipConnection( NULL );    

    for( TInt i=0; i<iSipConnections.Count(); i++ )
        {
        sipConnection = iSipConnections[ i ];

        if( sipConnection->ProfileId() == aProfileId )
            {
            return sipConnection->Disable();
            }
        }

    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CScpProfileHandler::CancelRegistration
// -----------------------------------------------------------------------------
//
TInt CScpProfileHandler::CancelRegistration( TUint32 aProfileId )
    {
    SCPLOGSTRING2("CScpProfileHandler::UnregisterProfile id: %d", aProfileId );

    TInt result = UnregisterProfile( aProfileId );

    if( result == KErrNone ||
        result == KErrNotFound || 
        result == KErrArgument )
        {
        result = RemoveSipConnection( aProfileId );
        }

    return result;
    }

// -----------------------------------------------------------------------------
// CScpProfileHandler::IncomingRequest
// -----------------------------------------------------------------------------
//
void CScpProfileHandler::IncomingRequest( TUint32 /*aIapId*/,
                                          CSIPServerTransaction* /*aTransaction*/ )
    {
    SCPLOGSTRING( "CScpProfileHandler::IncomingRequest" );
    }

// -----------------------------------------------------------------------------
// CScpProfileHandler::TimedOut
// -----------------------------------------------------------------------------
//
void CScpProfileHandler::TimedOut(
    CSIPServerTransaction& /*aTransaction*/ )
    {
    SCPLOGSTRING( "CScpProfileHandler::TimedOut" );
    }

// -----------------------------------------------------------------------------
// CScpProfileHandler::RemoveSipConnection
// -----------------------------------------------------------------------------
//
TInt CScpProfileHandler::RemoveSipConnection( TUint32 aProfileId )
    {
    SCPLOGSTRING2( "CScpProfileHandler::RemoveSipConnection id: %i", 
                   aProfileId );

    TInt ret = ( KErrNotFound );
    CScpSipConnection* sipConnection( NULL );
    
    sipConnection =  GetSipConnection( aProfileId );
    
    if ( sipConnection )
        {
        TInt index = iSipConnections.Find( sipConnection );
        iSipConnections.Remove( index );
        delete sipConnection;
        ret = KErrNone;
        }
    return ret;
    }


// -----------------------------------------------------------------------------
// CScpProfileHandler::ProfileRegistryEventOccurred
// -----------------------------------------------------------------------------
//
void CScpProfileHandler::ProfileRegistryEventOccurred(
    TUint32 aProfileId,
    MSIPProfileRegistryObserver::TEvent aEvent )
    {
    SCPLOGSTRING3( "CScpProfileHandler::ProfileRegistryEventOccurred id: %i event: %d", 
                   aProfileId, aEvent );

    CScpSipConnection* sipConnection = GetSipConnection( aProfileId );
    
    if( sipConnection )
        {
        sipConnection->ProfileRegistryEventOccurred( aEvent );
        
        // Deal with sip profile removals
        switch ( aEvent )
            {
            case EProfileDeregistered:
                {
                // Remove sip connection only if user has requested deregistration
                CScpSipConnection::TRegistrationRequestState state =
                    sipConnection->RegistrationRequestState();

                if( state == CScpSipConnection::EDeregistrationRequested )
                    {
                    RemoveSipConnection( aProfileId );
                    }
                }
                break;

            case EProfileDestroyed:
                {
                RemoveSipConnection( aProfileId );
                }
                break;
               
            default:
                break;
            }
        }      
    }

// -----------------------------------------------------------------------------
// CScpProfileHandler::ProfileRegistryErrorOccurred
// -----------------------------------------------------------------------------
//
void CScpProfileHandler::ProfileRegistryErrorOccurred( TUint32 aProfileId,
                                                       TInt aError )
    {
    SCPLOGSTRING3( "CScpProfileHandler::ProfileRegistryErrorOccurred profile:%i error: %d", 
                   aProfileId, aError );

    CScpSipConnection* sipConnection = GetSipConnection( aProfileId );
    
    if( sipConnection )
        {
        sipConnection->ProfileRegistryErrorOccurred( aError );
        }
    }
      
// -----------------------------------------------------------------------------
// CScpProfileHandler::ProfileExist
// -----------------------------------------------------------------------------
//
TBool CScpProfileHandler::ProfileExists( TUint32 aProfileId ) const
    {    
    SCPLOGSTRING2( "CScpProfileHandler::ProfileExists id: %d", aProfileId );

    CSIPProfile* profile( NULL );
    TRAPD( result, profile = iProfileRegistry->ProfileL( aProfileId ) );

    if( result == KErrNone && profile )
        {
        delete profile;
        return ETrue;
        }

    return EFalse;             
    }

// -----------------------------------------------------------------------------
// CScpProfileHandler::SetSipProfileReserved
// -----------------------------------------------------------------------------
//
void CScpProfileHandler::SetSipProfileReserved( TUint32 aProfileId, TBool aReserved )
    {
    SCPLOGSTRING3( "CScpProfileHandler::SetSipProfileReserved id: %d reserved: %d", 
                   aProfileId, aReserved );

    CScpSipConnection* sipConnection = GetSipConnection( aProfileId );
    
    if( sipConnection )
        {
        sipConnection->SetReserved( aReserved );

        // If profile was freed and iap availability alr event had come
        // before, let ALR controller to know that the profile is up
        // for ALR
        if( aReserved == EFalse && 
            sipConnection->IapAvailableOffered() )
            {
            TRAP_IGNORE( iAlrController->RefreshIapAvailabilityL( aProfileId ) );
    
            // Reset the iap availability offered flag
            sipConnection->SetIapAvailableOffered( EFalse );
            }
        }
    }

// -----------------------------------------------------------------------------
// CScpProfileHandler::StartAlrMigration
// -----------------------------------------------------------------------------
//
void CScpProfileHandler::StartAlrMigration( TUint32 aProfileId )
    {
    SCPLOGSTRING2( "CScpProfileHandler::StartMigrtion profile: %i", aProfileId );
    SCPLOGSTRING2( "CScpProfileHandler::StartMigrtion iNewAlrIapId: %i", iNewAlrIapId );
    
    TRAP_IGNORE(
       iAlrController->AllowMigrationL( aProfileId, iNewAlrIapId ) );
    
    CScpSipConnection* sipConnection( NULL );
    sipConnection = GetSipConnection( aProfileId );
    
    if ( sipConnection )
        {
        sipConnection->SetProfileCurrentlyRoaming();
        }
    
    SCPLOGSTRING( "CScpProfileHandler::StartMigrtion -exit" );
    }

// -----------------------------------------------------------------------------
// CScpProfileHandler::AlrEvent
// -----------------------------------------------------------------------------
//
void CScpProfileHandler::AlrEvent( MSipProfileAlrObserver::TEvent aEvent,
                                   TUint32 aProfileId,
#ifdef _DEBUG
                                   TUint32 aSnapId,
#else
                                   TUint32 /*aSnapId*/,
#endif
                                   TUint32 aIapId )
    {
    SCPLOGSTRING2("CScpProfileHandler::AlrEvent profile:%i", aProfileId);
    SCPLOGSTRING2("CScpProfileHandler::AlrEvent aEvent:%i", (TInt)aEvent);
    SCPLOGSTRING2("CScpProfileHandler::AlrEvent aSnapId:%i", aSnapId);
    SCPLOGSTRING2("CScpProfileHandler::AlrEvent aIapId:%i", aIapId);

    CScpSipConnection* sipConnection( NULL );
    
    switch( aEvent )
        {
        case MSipProfileAlrObserver::EIapAvailable:
            {
            TBool reserved( EFalse );
            sipConnection = GetSipConnection( aProfileId );
    
            if( sipConnection )
                {
                reserved = sipConnection->Reserved();

                if( reserved )
                    {
                    TRAPD( result, 
                        iAlrController->DisallowMigrationL( aProfileId, aIapId ) );

                    if( result == KErrNone )
                        {
                        // Raise the iap availability offered flag
                        sipConnection->SetIapAvailableOffered( ETrue );
                        }
                    }
                else
                    {
                    iAlrAllowedToStartImmediately = ETrue;

                    for ( TInt i = 0; i < iObservers.Count(); i++ )
                        {
                        if ( !iObservers[ i ]->IsSipProfileAllowedToStartAlr() )
                            {
                            iAlrAllowedToStartImmediately = EFalse;
                            break;
                            }
                        }
                    if ( iAlrAllowedToStartImmediately )
                        {
                        TRAP_IGNORE(
                            iAlrController->AllowMigrationL( aProfileId, aIapId ) );
                        sipConnection->SetProfileCurrentlyRoaming();
                        }
                    else
                        {
                        iNewAlrIapId = aIapId;
                        sipConnection->HandleMigrationStarted();
                        }
                    }
                }
            break;
            }

        // For possible beyond use (E.g. Maybe we should change CHH state
        // during iap migration -> VoIP call not possible if iap migration
        // ongoing.)
        case MSipProfileAlrObserver::EMigrationStarted:
            {
            sipConnection = GetSipConnection( aProfileId );
            
            if( sipConnection && iAlrAllowedToStartImmediately )
                {
                sipConnection->HandleMigrationStarted();
                }
            break;
            }
        
        case MSipProfileAlrObserver::EMigrationCompleted:
        default:
            {
            break;
            }            
        }
    }
    
// -----------------------------------------------------------------------------
// CScpProfileHandler::AlrError
// -----------------------------------------------------------------------------
//
void CScpProfileHandler::AlrError( 
    TInt aError,
    TUint32 aProfileId,
    TUint32 aSnapId,
    TUint32 aIapId )
    {
    SCPLOGSTRING("CScpProfileHandler::AlrError");
    SCPLOGSTRING2("     profile id: %d", aProfileId);
    SCPLOGSTRING2("     error:      %d", aError);
    
    CScpSipConnection* sipConnection = GetSipConnection( aProfileId );
    if( sipConnection )
        {
        sipConnection->HandleMigrationError( 
            aError, aProfileId, aSnapId, aIapId );
        }
    }

// -----------------------------------------------------------------------------
// CScpProfileHandler::HandleSipConnectionEvent
// -----------------------------------------------------------------------------
//
void CScpProfileHandler::HandleSipConnectionEvent( TUint32 aProfileId,
                                                   TScpConnectionEvent aSipEvent )
    {
    SCPLOGSTRING3( 
        "CScpProfileHandler::HandleSipConnectionEvent profile id: %d sip event: %d",
        aProfileId, aSipEvent );

    // Make a copy of the observers array, since observers
    // may be removed during the event handling
    RPointerArray< MScpSipConnectionObserver > observers;
    
    for ( TInt i=0; i < iObservers.Count(); i++ )
        {      
        observers.Append( iObservers[ i ] );
        }

    // Send notify to observers
    for( TInt i=0; i < observers.Count(); i++ )
        {
        //check that the observer is still valid before triggering notify
        if ( KErrNotFound != iObservers.Find( observers[ i ] ) )
            {
            observers[ i ]->HandleSipConnectionEvent( aProfileId, aSipEvent );
            }
        }

    observers.Close();
        
    // if registration failed, time to delete connection
    if ( aSipEvent == EScpRegistrationFailed )
        {
        RemoveSipConnection( aProfileId );        
        }
    }

// -----------------------------------------------------------------------------
// CScpProfileHandler::IsSipProfileAllowedToStartAlr
// -----------------------------------------------------------------------------
//
TBool CScpProfileHandler::IsSipProfileAllowedToStartAlr()
    {
    return ETrue;
    }
    
// -----------------------------------------------------------------------------
// CScpProfileHandler::VmbxInterfaceL
// -----------------------------------------------------------------------------
//
CIpVmbxInterface& CScpProfileHandler::VmbxInterfaceL( MIpVmbxObserver& aObserver )
    {
    SCPLOGSTRING( "CScpProfileHandler::VmbxInterfaceL" );

    // Todo: save observer and set MIpVmbxObserver to this class
	// problem when several vmbx services, current observer logic isn't work
    
    if( !iVmbxInterface )
        {
        iVmbxInterface = CIpVmbxInterface::NewL( aObserver );
        }

    return *iVmbxInterface;
    }

// -----------------------------------------------------------------------------
// CScpProfileHandler::DeleteVmbxInterface
// -----------------------------------------------------------------------------
//
void CScpProfileHandler::DeleteVmbxInterface()
    {
    SCPLOGSTRING( "CScpProfileHandler::DeleteVmbxInterface" );

    if( iVmbxInterface )
        {
        delete iVmbxInterface;
        iVmbxInterface = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CScpProfileHandler::UpdateSipProfile
// -----------------------------------------------------------------------------
//
void CScpProfileHandler::UpdateSipProfileL( 
    TUint32 aProfileId,
    TBool aTerminalType,
    TBool aWlanMac,
    TInt aStringLength )
    {
    SCPLOGSTRING( "CScpProfileHandler::UpdateSipProfileL IN" );
    SCPLOGSTRING2( " profile id:    %d", aProfileId );
    SCPLOGSTRING2( " terminal type: %d", aTerminalType );
    SCPLOGSTRING2( " wlan mac:      %d", aWlanMac );
    SCPLOGSTRING2( " string length: %d", aStringLength );
    
    // Do not update, if terminal type is not defined in user agent header
    if ( !aTerminalType )
        {
        return;
        }
    
    const MDesC8Array* array;
    CSIPProfile* sipProfile = iProfileRegistry->ProfileL( aProfileId );
    CleanupStack::PushL( sipProfile );
    User::LeaveIfError( sipProfile->GetParameter( KSIPHeaders, array ) );

    TBuf<KSCPMaxTerminalTypeLength> terminalType( KNullDesC );
    TBuf<KTempStringlength> tempHeader( KNullDesC );
    
    // 1. Get the terminal type and seek is user agent header's 
    // terminal type same, if so no need to continue
    CIpAppPhoneUtils* libIpAppPhoneUtils = CIpAppPhoneUtils::NewLC();
    libIpAppPhoneUtils->GetTerminalTypeL( terminalType );   
    CleanupStack::PopAndDestroy( libIpAppPhoneUtils );
    
    for( TInt t( 0 ); t < array->MdcaCount(); t++ )
        {
        tempHeader.Copy( array->MdcaPoint( t ) );
        if( tempHeader.Find( terminalType ) != KErrNotFound )
            {
            SCPLOGSTRING(" terminal type found");
            CleanupStack::PopAndDestroy( sipProfile );
            return;
            }
        }
    
    // 2. If appropriate terminal type not found, we need to update it
    if ( array->MdcaCount() )
        {
        TBuf8<KTempStringlength> userAgentHeader;
        CDesC8ArrayFlat* uahArray = new ( ELeave ) CDesC8ArrayFlat( 1 );
        CleanupStack::PushL( uahArray );
        //Find the User-Agent string
        for( TInt t( 0 ); t < array->MdcaCount(); t++ )
            {
            tempHeader.Copy( array->MdcaPoint( t ) );
            // user-Agent found, now change it
            if( tempHeader.Find( KScpUserAgentString ) != KErrNotFound )
                {
                // 3. now we have the right user agent header which is not correct
                // replace the old terminal type 
                TInt len( KErrNone );
                // Get mac if defined
                if ( aWlanMac )
                    {
                    TBuf8<KSCPWlanMacAddressLength> wlanMacAddress( KNullDesC8 );
                    TBuf<KSCPWlanMacAddressLength>  tempAddr( KNullDesC );
                    CIPAppUtilsAddressResolver* addressResolver = 
                        CIPAppUtilsAddressResolver::NewLC();    
                    User::LeaveIfError( addressResolver->GetWlanMACAddress( 
                        wlanMacAddress, KSCPWlanMacAddressFrmt ) );
                    CleanupStack::PopAndDestroy( addressResolver );
                    tempAddr.Copy( wlanMacAddress );
                    
                    // leave the space before MAC ADDR (-1)
                    len = tempHeader.Find( tempAddr ) - KSCPUserAgentStringLength - 1;
                    }
                else
                    {
                    // leave the space before free string (-1)
                    len = tempHeader.Length() - KSCPUserAgentStringLength - aStringLength - 1;
                    }
                // make sure that the len is correct value(not below zero)
                if ( 0 >= len )
                    {
                    User::Leave( KErrGeneral );
                    }
                tempHeader.Replace( KSCPUserAgentStringLength, len, terminalType );
                SCPLOGSTRING2(" header:        %S", &tempHeader);
                userAgentHeader.Append( tempHeader );
                uahArray->AppendL( userAgentHeader );
                }
            else
                {
                //if this entry is not user agent, leave it untouched
                uahArray->AppendL( array->MdcaPoint( t ) );    
                }
            }

        CSIPManagedProfile* managedProfile = static_cast<CSIPManagedProfile*>( 
            iProfileRegistry->ProfileL( aProfileId ) );
        CleanupStack::PushL( managedProfile );
        //Update the new user-agent to sipprofile
        User::LeaveIfError( 
            managedProfile->SetParameter( KSIPHeaders, *uahArray ) );
        iManagedProfileRegistry->SaveL( *managedProfile );
        CleanupStack::PopAndDestroy( managedProfile );
        CleanupStack::PopAndDestroy( uahArray );
        }        
    CleanupStack::PopAndDestroy( sipProfile );    
    SCPLOGSTRING( "CScpProfileHandler::UpdateSipProfileL OUT" );
    }

// -----------------------------------------------------------------------------
// CScpProfileHandler::SetUsernameAndPasswordL
// -----------------------------------------------------------------------------
//
void CScpProfileHandler::SetUsernameAndPasswordL( TUint32 aProfileId,
                                                  const TDesC8& aUsername,
                                                  TBool aSetUsername,
                                                  const TDesC8& aPassword,
                                                  TBool aSetPassword )
    {
    SCPLOGSTRING( "CScpProfileHandler::SetUsernameAndPasswordL" );

    CSIPManagedProfile* profile = static_cast<CSIPManagedProfile*>
                         (iProfileRegistry->ProfileL( aProfileId ));
    CleanupStack::PushL( profile );

    SCPLOGSTRING( "CScpProfileHandler::SetUsernameAndPasswordL sip profile availble" );
    
    if( aSetUsername )
        {
        RBuf8 formattedUsername;
        CleanupClosePushL( formattedUsername );
        
        if( !TScpUtility::CheckSipUsername( aUsername ) )
            {
            // Since username is returned with prefix and domain, it needs
            // to be possible to also set it with prefix and domain. 
            // Strip prefix and domain from user aor if found.
            // Prefix might be sip or sips so search by colon.
            // If username is empty, leave with KErrArgument
            if ( aUsername == KNullDesC8 )
                {
                User::Leave( KErrArgument );
                }
            else
                {
                User::LeaveIfError( TScpUtility::RemovePrefixAndDomain( 
                    aUsername, formattedUsername ) );
                }
            }
        else
            {
            formattedUsername.CreateL( aUsername.Length() );
            formattedUsername.Copy( aUsername );
            }
        
        TBool validAorExists( EFalse );       
        const TDesC8* aorPtr( NULL );
        
        TInt err = profile->GetParameter( KSIPUserAor, aorPtr );
        
        TInt loc( KErrNotFound );
        if ( !err && aorPtr->Length() )
            {
            loc = aorPtr->Find( KAt() );
            
            if ( KErrNotFound != loc )
                {         
                TPtrC8 ptr = aorPtr->Right( 1 );
                
                if ( ptr.Compare( KAt ) != 0 )
                    {                    
                    // @ is found and there is something in domain part
                    validAorExists = ETrue;
                    }              
                }
            }
       
       HBufC8* newAor = NULL;
       TBuf8<KSipSchemeMaxLength> prefix;
       TScpUtility::GetValidPrefix( aUsername, prefix );
       
       if ( validAorExists )
           {           
           SCPLOGSTRING( "CScpProfileHandler::SetUsernameAndPasswordL valid aor" );
           TPtrC8 domainPart = aorPtr->Mid( loc );
           
           newAor = HBufC8::NewLC( prefix.Length() + 
                                   domainPart.Length() + 
                                   formattedUsername.Length() );
           newAor->Des().Append( prefix );
           newAor->Des().Append( formattedUsername );
           newAor->Des().Append( domainPart );
           }
       else
           {         
           SCPLOGSTRING( "CScpProfileHandler::SetUsernameAndPasswordL no valid aor" );
           // Check if given username contains valid aor
           TInt loc = aUsername.Find( KAt() );
           
           if ( KErrNotFound != loc )
               {               
               TPtrC8 ptrRight = aorPtr->Right( 1 );
               TPtrC8 ptrLeft = aorPtr->Left( 1 );
               
               if ( ( ptrRight.Compare( KAt ) != 0 ) && 
                   ( ptrLeft.Compare( KAt ) != 0 ) )
                   {                   
                   // both sides of @ have data --> aor is valid
                   newAor = HBufC8::NewLC( aUsername.Length() );
                   newAor->Des().Copy( aUsername );
                   }
               else
                   {
                   User::Leave( KErrArgument );
                   }
               }
           else
               {               
               User::Leave( KErrArgument );
               }    
           }

        SCPLOGSTRING( "CScpProfileHandler::SetUsernameAndPasswordL set new aor value" );
        
        User::LeaveIfError( profile->SetParameter( KSIPUserAor, newAor->Des() ) );
        CleanupStack::PopAndDestroy( newAor );
        
        // Add digest username & password
        SCPLOGSTRING( "CScpProfileHandler:SetUsernameAndPasswordL outbound & digest username" );
        User::LeaveIfError( profile->SetParameter( KSIPOutboundProxy,
                                                   KSIPDigestUserName,
                                                   formattedUsername ) );


        SCPLOGSTRING( "CScpProfileHandler:SetUsernameAndPasswordL registrar & digest username" );
        User::LeaveIfError( profile->SetParameter( KSIPRegistrar,
                                                   KSIPDigestUserName,
                                                   formattedUsername ) );
        CleanupStack::PopAndDestroy( &formattedUsername );                                                   
        }

    if( aSetPassword )
        {
        if( !TScpUtility::CheckSipPassword( aPassword ) )
            {
            User::Leave( KErrArgument );
            }
        SCPLOGSTRING( "CScpProfileHandler:SetUsernameAndPasswordL outbound & digest pwd" );
        User::LeaveIfError( profile->SetParameter( KSIPOutboundProxy,
                                                   KSIPDigestPassword,
                                                   aPassword ) );    
        SCPLOGSTRING( "CScpProfileHandler:SetUsernameAndPasswordL registrar & digest pwd" );
        User::LeaveIfError( profile->SetParameter( KSIPRegistrar,
                                                   KSIPDigestPassword,
                                                   aPassword ) );
        }
    
    SCPLOGSTRING( "CScpProfileHandler:SetUsernameAndPasswordL save sip changes" );

    // Save changes
    TRAPD( err, iManagedProfileRegistry->SaveL( *profile ) );

    CleanupStack::PopAndDestroy( profile );    
    
    SCPLOGSTRING2( "CScpProfileHandler:SetUsernameAndPasswordL save sip err: %d", err );
    User::LeaveIfError( err );
    }



// -----------------------------------------------------------------------------
// CScpProfileHandler::GetDebugInfo
// -----------------------------------------------------------------------------
//
#ifdef _DEBUG
void CScpProfileHandler::GetDebugInfo( TDes& aInfo ) const
    {
    TInt connections = iSipConnections.Count();
    TInt registeredConnections( 0 );
    TInt registeringConnections( 0 );
    TInt unregisteringConnections( 0 );
    TInt unregisteredConnections( 0 );
    
    for( TInt i=0; i<iSipConnections.Count(); i++ )
        {
        CScpSipConnection* sipConnection = iSipConnections[ i ];
        CScpSipConnection::TConnectionState state;
        TInt error;

        sipConnection->GetState( state, error );

        switch( state )
            {
            case CScpSipConnection::ERegistered:
                registeredConnections++;
                break;

            case CScpSipConnection::ERegistering:
                registeringConnections++;
                break;

            case CScpSipConnection::EDeregistering:
                unregisteringConnections++;
                break;

            case CScpSipConnection::EDeregistered:
                unregisteredConnections++;
                break;

            default:
                break;
            }
        }

    TBuf< 255 > buffer;
    buffer.Format( _L( "Connections: %d\n Registered: %d\n Registering: %d\n Unregistering: %d\n Unregistered: %d\n" ),
                        connections, registeredConnections, registeringConnections,
                        unregisteringConnections, unregisteredConnections );

    aInfo.Append( buffer );
    }
#endif
            
// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
