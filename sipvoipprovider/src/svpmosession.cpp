/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Representation of SVP Mo session         
*
*/

#include <mcesecureoutsession.h>
#include <badesca.h>
#include <mccpforwardobserver.h>
#include <uriutilscommon.h>      // TUriUtilsError
#include <sipcodecerr.h>
#include <crcseprofileentry.h>
#include <crcseprofileregistry.h>

#include "svpmosession.h"
#include "svpuriparser.h"
#include "svpforwardprovider.h"
#include "svpsipconsts.h"
#include "svplogger.h"
#include "svputility.h"
#include "svpcleanupresetanddestroy.h"


// ---------------------------------------------------------------------------
// CSVPMoSession::CSVPMoSession
// ---------------------------------------------------------------------------
//
CSVPMoSession::CSVPMoSession( TUint32 aSessionExpires, 
                              TMceTransactionDataContainer& aContainer,
                              MSVPSessionObserver& aObserver, 
                              CSVPUtility& aSVPUtility,
                              CSVPRtpObserver& aRtpObserver ) 
    : CSVPSessionBase( aContainer, aObserver, aSVPUtility, aRtpObserver ),
      iSessionExpires( aSessionExpires )
    {
    SVPDEBUG1( "CSVPMoSession::CSVPMoSession In" )
    SVPDEBUG1( "CSVPMoSession::CSVPMoSession Out" )
    }

// ---------------------------------------------------------------------------
// CSVPMoSession::ConstructL
// ---------------------------------------------------------------------------
//
void CSVPMoSession::ConstructL( 
    CMceManager& aMceManager, 
    CRCSEProfileEntry& aVoipProfile, 
    const CCCPCallParameters& aParameters,
    const TDesC8& aRecipient,
    TUint32& aSecurityStatus,
    CDesC8Array* aUserHeaders )
    {
    SVPDEBUG1( "CSVPMoSession::ConstructL In" )
    
    TBool userEqphone = EFalse;
    TInt err = KErrNone;
    
    __ASSERT_ALWAYS( aVoipProfile.iIds.Count(), User::Leave( KErrArgument ) );
    iSipProfileId = aVoipProfile.iIds[ 0 ].iProfileId; // save Sip profile id
    iVoIPProfileId = aVoipProfile.iId; // save VoIP profile id
    
    iSecondCallRecipient.Copy( aRecipient ); // save recipient
    SVPDEBUG2( "CSVPMoSession::ConstructL iSecondCallRecipient length: %d",
        iSecondCallRecipient.Length() )
    
    // Copy call parameters
     iCallParameters = aParameters.CloneL();

    // create SIP and ProfileRegistry for URI handling
    CSIP* sip = CSIP::NewL( KSVPUid, *this );
    CleanupStack::PushL( sip );
    SVPDEBUG1( "CSVPMoSession::ConstructL sip CREATED" )

    CSIPProfileRegistry* sipProfileRegistry = CSIPProfileRegistry::NewL( *sip, *this );
    CleanupStack::PushL( sipProfileRegistry );
    SVPDEBUG1( "CSVPMoSession::ConstructL sipProfileRegistry CREATED" )

    // retrieve SIP profile by using sip profile id
    CSIPProfile* profile = sipProfileRegistry->ProfileL( iSipProfileId );
    CleanupStack::PushL( profile );

    // Not secure call event is sent later according this 
    iCallEventToBeSent = MCCPCallObserver::ECCPSecureNotSpecified;

    if ( KSVPStatusNonSecure != aSecurityStatus )
        {
        // set secure status to 0 if no security mechanism found from SIP profile
        iSVPUtility.ResolveSecurityMechanismL( *profile, aSecurityStatus );
        }
    
    // Check CLIR setting and add relevant information in fromheader.
    HBufC8* fromheader = iSVPUtility.SetMoFromHeaderLC( aSecurityStatus );
    
    // check if user=phone needs to be added to outgoing INVITE
    if ( CRCSEProfileEntry::EOn == aVoipProfile.iUserPhoneUriParameter )
        {
        userEqphone = ETrue;
        }
    
    // complete sip uri according to the security status
    HBufC8* uri = CompleteUriL( aRecipient,
                                *profile,
                                aSecurityStatus,
                                userEqphone );
    CleanupStack::PushL( uri );
    
    // Add P-Preferred-Identity header if CLIR is on  
    if ( iSVPUtility.IsCLIROnL() )
        {
        HBufC8* userAor = NULL;
        const MDesC8Array* aors( NULL ); // Array of registered address of records
        
        TInt result = profile->GetParameter( KSIPRegisteredAors, aors );
        
        if ( !aors || aors->MdcaCount() == KErrNone )
            {
            SVPDEBUG1( "CSVPMoSession::CompleteUriL - registered aors array empty" )
            const TDesC8* userAorStr = NULL;
            User::LeaveIfError( profile->GetParameter( KSIPUserAor, userAorStr ) );
            SVPDEBUG2( "CSVPMoSession::CompleteUriL - KSIPUserAor result: %d", result )
            userAor = userAorStr->AllocLC();
            }
        else
            {
            userAor = aors->MdcaPoint( 0 ).AllocLC();
            }    

        iSVPUtility.AddPPreferredIdentityToUserHeadersL( 
            *aUserHeaders, *userAor );
        CleanupStack::PopAndDestroy(); //userAor
        }
    
    // create Mce out session 
    if ( KSVPStatusNonSecure == aSecurityStatus )
        {
        // create non-secure session
        // must be trapped, so that leave code can be changed to KErrArgument
        // if uri contains error-causing characters
        TRAP( err, iSession = CMceOutSession::NewL( aMceManager, 
                *profile, *uri, fromheader ) );
        SVPDEBUG2( "CSVPMoSession::ConstructL NonSecure trapped: %d", err )
        
        if ( KErrNone == err )
            {
            // set iSecured flag so UI can show right icon
            iSecured = EFalse;
            }
        }
    else if ( KSVPStatusSecurePreferred == aSecurityStatus )
        {
        TRAP( err, iSession = CMceSecureOutSession::NewL(
                aMceManager, *profile, *uri, fromheader ) );
        SVPDEBUG2( "CSVPMoSession::ConstructL SecurePreferred trapped: %d", err )
        
        if ( KErrNone == err )
            {
            // set crypto contexts
            SetCryptoContextL();
            // set secured flag to ETrue to indicate that this is secure session
            SetSecurePreferred( ETrue );
            // set iSecured flag so UI can show right icon
            iSecured = ETrue;
            }
        }
    else
        {
        TRAP( err, iSession = CMceSecureOutSession::NewL(
                aMceManager, *profile, *uri, fromheader ) );
        SVPDEBUG2( "CSVPMoSession::ConstructL SetSecureMandatory trapped: %d", err )
        
        if ( KErrNone == err )
            {
            // set crypto contexts
            SetCryptoContextL();
            // set secure mandatory flag ETrue, only secure session allowed       
            SetSecureMandatory( ETrue );
            // set iSecured flag so UI can show right icon
            iSecured = ETrue;
            }
        }
    
    if ( KUriUtilsErrInvalidUserInfo == err ||      // -5016
         KUriUtilsErrInvalidHost == err     ||      // -5017
         KUriUtilsErrInvalidPort == err     ||      // -5018
         KUriUtilsErrInvalidHeaders == err  ||      // -5022
         KErrSipCodecTokenizer == err       ||      // -17751
         KErrSipCodecFromOrToParams == err )        // -17775
        {
        User::Leave( KErrArgument );
        }
    else if ( KErrNone != err )
        {
        User::Leave( KErrDisconnected );
        }
    else
        {
        //For PC-Lint Note: #961
        }
    
    CleanupStack::PopAndDestroy( uri );
    CleanupStack::Pop( fromheader );
    
    // Get keep-alive timer value
    TUint32 iapId = 0;
    TBool found = EFalse;
    
    profile->GetParameter( KSIPAccessPointId, iapId );
    TRAPD( errKeepAlive, found = iSVPUtility.GetKeepAliveByIapIdL( iapId, iKeepAliveValue ) );
    SVPDEBUG3( "CSVPMoSession::ConstructL:GetKeepAliveByIapIdL: errKeepAlive = %d found = %d",
            errKeepAlive, found )
    
    if ( !found )
        {
        const TDesC8* aor;
        profile->GetParameter( KSIPUserAor, aor );
        TRAP( errKeepAlive, found = iSVPUtility.GetKeepAliveByAORL( *aor, iKeepAliveValue ) );
        SVPDEBUG3( "CSVPMoSession::ConstructL:GetKeepAliveByAORL: errKeepAlive = %d found = %d",
                errKeepAlive, found )
        }
    
    CleanupStack::PopAndDestroy( profile );
    CleanupStack::PopAndDestroy( sipProfileRegistry );
    CleanupStack::PopAndDestroy( sip );
    
    // constructs audio streams for this session and adds codecs to streams and
    // streams to session
    ConstructAudioStreamsL();
    InitializePropertyWatchingL();
    
    // Create forward provider
    iForwardProvider = CSVPForwardProvider::NewL( *this );
    iForwardAddressList = new (ELeave) CDesC8ArrayFlat( KSVPContactArrayGranularity );
    
    // Cant leave anymore, store heap data
    iUserHeaders = aUserHeaders; // save userheaders
    iMceManager = &aMceManager; // not own
    
    SVPDEBUG1( "CSVPMoSession::ConstructL Out" )
    }

// ---------------------------------------------------------------------------
// CSVPMoSession::NewL
// ---------------------------------------------------------------------------
//
CSVPMoSession* CSVPMoSession::NewL( 
    CMceManager& aMceManager,
    const TDesC8& aRecipient,
    CRCSEProfileEntry& aVoipProfile,  
    const CCCPCallParameters& aParameters,
    TMceTransactionDataContainer& aContainer,                                    
	MSVPSessionObserver& aObserver,
	CSVPUtility& aSVPUtility,
	CSVPRtpObserver& aRtpObserver,
	TUint32& aSecurityStatus,
	CDesC8Array* aUserHeaders )
    {
    CSVPMoSession* self = new( ELeave ) CSVPMoSession(
    		aVoipProfile.iSIPSessionExpires, aContainer, aObserver,
    		aSVPUtility, aRtpObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aMceManager, 
                      aVoipProfile,
                      aParameters, 
                      aRecipient, 
                      aSecurityStatus,
                      aUserHeaders );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CSVPMoSession::~CSVPMoSession
// ---------------------------------------------------------------------------
//
CSVPMoSession::~CSVPMoSession()
    {
    SVPDEBUG1( "CSVPMoSession::~CSVPMoSession In" )
   
    delete iForwardProvider;
    
    if ( iUserHeaders )
        {
        iUserHeaders->Reset();
        delete iUserHeaders;
        }

    if ( iForwardAddressList )
        {
        iForwardAddressList->Reset();
        delete iForwardAddressList;
        }
    
    SVPDEBUG1( "CSVPMoSession::~CSVPMoSession Out" )
    }
  
// ---------------------------------------------------------------------------
// CSVPMoSession::CompleteUriL
// ---------------------------------------------------------------------------
//  
HBufC8* CSVPMoSession::CompleteUriL( const TDesC8& aRecipient, 
                                     const CSIPProfile& aSIPProfile,
                                     const TUint32& aSecurityStatus,
                                     TBool aUserEqualsPhoneRequired )
    {
    SVPDEBUG1( "CSVPMoSession::CompleteUriL In" )

    // create instance of uri parser
    CSVPUriParser* uriParser = CSVPUriParser::NewLC();
    
    HBufC8* userAor( NULL );
    const MDesC8Array* aors( NULL ); // Array of registered address of records
    
    TInt result = aSIPProfile.GetParameter( KSIPRegisteredAors, aors );
    
    if ( !aors || aors->MdcaCount() == KErrNone )
        {
        SVPDEBUG1( "CSVPMoSession::CompleteUriL - registered aors array empty" )
        const TDesC8* userAorStr = NULL;
        result = aSIPProfile.GetParameter( KSIPUserAor, userAorStr );
        SVPDEBUG2( "CSVPMoSession::CompleteUriL - KSIPUserAor result: %d", result )
        userAor = HBufC8::NewLC( userAorStr->Length() );
        userAor->Des().Copy( *userAorStr );
        }
    else
        {
        #ifdef _DEBUG
            TBuf<KSvpMaxDebugBufferSize> tmpUri;
            for ( TInt i = 0; i < aors->MdcaCount(); i++ )
                {
                tmpUri.Copy( aors->MdcaPoint( i ) );
                SVPDEBUG3( "CSVPMoSession::CompleteUriL - registered AOR[%d]: %S", i, &tmpUri )
                }
        #endif
        userAor = HBufC8::NewLC( aors->MdcaPoint( 0 ).Length() );
        userAor->Des().Copy( aors->MdcaPoint( 0 ) );
        }
    // set service id to uri parser, needed for setting user=phone
    uriParser->SetUserEqualsPhoneRequiredL( aUserEqualsPhoneRequired );
    
    // complete URI by security status preference
    HBufC8* uri = NULL;
    if ( KSVPStatusNonSecure == aSecurityStatus )
        {
        SVPDEBUG1( "CSVPMoSession::CompleteUriL Completing SIP URI..." )
        
        // user tries to call to sips: -uri, secpref -> 0
        // so iTLSNotInUse is set, so that user can be notified
        // about non secure session
        if ( KErrNotFound != aRecipient.Find( KSVPSipsPrefix ) )
        	{
        	iTLSNotInUse = ETrue;
        	}
        uri = uriParser->CompleteSipUriL( aRecipient, *userAor );
        }
    else
        {
        SVPDEBUG1( "CSVPMoSession::CompleteUriL Completing SIPS URI..." )
        uri = uriParser->CompleteSecureSipUriL( aRecipient, *userAor );
        }
    CleanupStack::PopAndDestroy( userAor );
    CleanupStack::PopAndDestroy( uriParser );
    
    SVPDEBUG1( "CSVPMoSession::CompleteUriL Out" )
    return uri;
    }
    


// ---------------------------------------------------------------------------
// CSVPMoSession::CreateNonSecureSessionL
// ---------------------------------------------------------------------------
//    
void CSVPMoSession::CreateNonSecureSessionL( CMceManager& aMceManager )
    {
    SVPDEBUG1( "CSVPMoSession::CreateNonSecureSessionL In" )
    
    TBool userEqphone = EFalse;
    
    // save old secure session first, will be deleted later from SVPController
    delete iTempSecSession;
    iTempSecSession = iSession;
    
    // set iSecured flag so UI can show right icon
    iSecured = EFalse;

    // set correct event to be sent later on.
    iCallEventToBeSent = MCCPCallObserver::ECCPNotSecureCall;

    // clear secure preferred flag, needed when deciding whether 
    // old session should be deleted
    SetSecurePreferred( EFalse );
    
    // create SIP and ProfileRegistry for URI handling
    CSIP* sip = CSIP::NewL( KSVPUid, *this );
    CleanupStack::PushL( sip );
    CSIPProfileRegistry* sipProfileRegistry = 
        CSIPProfileRegistry::NewL( *sip, *this );
    CleanupStack::PushL( sipProfileRegistry );
    
    // retrieve SIP profile by using sip profile id
    CSIPProfile* profile = sipProfileRegistry->ProfileL( iSipProfileId );
    CleanupStack::PushL( profile );
    
    // secure preference to 0
    TUint32 securePreference = 0;
    
    RPointerArray< CRCSEProfileEntry > entryArray;
    CleanupResetAndDestroy< RPointerArray<CRCSEProfileEntry> >::PushL( entryArray );
    CRCSEProfileRegistry* reg = CRCSEProfileRegistry::NewLC();
    reg->FindByServiceIdL( iCallParameters->ServiceId(), entryArray ); // Get VoIP profile by service id
    
    // Take first entry from array
    __ASSERT_ALWAYS( entryArray.Count(), User::Leave( KErrArgument ) );
    CRCSEProfileEntry* entry = entryArray[0];
    
    // check if user=phone needs to be added to outgoing INVITE
    if ( CRCSEProfileEntry::EOn == entry->iUserPhoneUriParameter )
        {
        userEqphone = ETrue;
        }
    
    // cleanup
    CleanupStack::PopAndDestroy( 2, &entryArray );    // reg, entryArray
    
    // complete sip uri according to the security status
    HBufC8* uri = CompleteUriL( iSecondCallRecipient,
                                *profile,
                                securePreference,
                                userEqphone );
    CleanupStack::PushL( uri );
    
    // nullify iSession member, but do not delete
    iSession = NULL;
    iSession = CMceOutSession::NewL( aMceManager, *profile, *uri );
    
    CleanupStack::PopAndDestroy( uri );
    CleanupStack::PopAndDestroy( profile );
    CleanupStack::PopAndDestroy( 2 ); // sip & sipProfileRegistry
    
    // constructs audio streams for this Mo session 
    // adds codecs to streams and streams to session 
    ConstructAudioStreamsL();
    
    // Dial, finally
    User::LeaveIfError( Dial() );
    
    SVPDEBUG1( "CSVPMoSession::CreateNonSecureSessionL Out" )
    }

// ---------------------------------------------------------------------------
// CSVPMoSession::Dial
// ---------------------------------------------------------------------------
//    
TInt CSVPMoSession::Dial()
    {
    SVPDEBUG1("CSVPMoSession::Dial In" )  
    
    TRAPD( errDial, DialL() );
    
    SVPDEBUG2("CSVPMoSession::Dial Out return=%d", errDial )
    return errDial;
    }

// ---------------------------------------------------------------------------
// CSVPMoSession::DialL
// ---------------------------------------------------------------------------
//    
void CSVPMoSession::DialL()
    {
    SVPDEBUG1( "CSVPMoSession::DialL In" )
    
    ExecCbCallStateChanged( MCCPCallObserver::ECCPStateDialling );
    
    // inform client about non secure session 
    if ( iTLSNotInUse )
        {
        ExecCbCallEventOccurred( MCCPCallObserver::ECCPNotSecureSessionWithSips );
        }

    if ( iUserHeaders && iUserHeaders->MdcaCount() )
        {
        CDesC8ArrayFlat* userHeaders = new ( ELeave ) CDesC8ArrayFlat( 
            iUserHeaders->MdcaCount() );
        CleanupStack::PushL( userHeaders ); // CS:1
        for ( TInt i = 0; i < iUserHeaders->MdcaCount(); i++ )
            {
            userHeaders->AppendL( iUserHeaders->MdcaPoint( i ) );
            }
        static_cast<CMceOutSession*>( iSession )->EstablishL( 
            iSessionExpires, userHeaders, NULL, NULL );
        CleanupStack::Pop( userHeaders ); // CS:0, ownership transferred
        }
    else
        {
        static_cast<CMceOutSession*>( iSession )->EstablishL(
            iSessionExpires, NULL, NULL, NULL );
        }

    // start INVITE timer in case remote end does not repond
    StartTimerL( KSVPInviteTimer, KSVPInviteTimerExpired );
    
    SVPDEBUG1( "CSVPMoSession::DialL Out" )
    }

// ---------------------------------------------------------------------------
// CSVPMoSession::Cancel
// ---------------------------------------------------------------------------
//  
TInt CSVPMoSession::Cancel()
    {
    SVPDEBUG1( "CSVPMoSession::Cancel() Cancel dial" )
    TRAPD( cancelErr, static_cast<CMceOutSession*>( iSession )->CancelL() );
    return cancelErr;
    }

// ---------------------------------------------------------------------------
// CSVPMoSession::HangUp
// ---------------------------------------------------------------------------
//  
TInt CSVPMoSession::HangUp()
    {
    SVPDEBUG1( "CSVPMoSession::HangUp In" )
    
    if ( CMceSession::EOffering == iSession->State() )
        {
        SVPDEBUG1( "CSVPMoSession::HangUp CancelL" )
        TRAPD( err, static_cast<CMceOutSession*>( iSession )->CancelL() );
        
        ExecCbCallStateChanged( MCCPCallObserver::ECCPStateDisconnecting );
        
        if( !err )
            {
            TRAPD( errTimer, StartTimerL( KSVPMoHangupTerminatingTime,
                    KSVPHangUpTimerExpired ) );
            
            if ( errTimer )
                {
                SVPDEBUG2("CSVPSessionBase::HangUp - timer leave code=%d", errTimer )
                ExecCbCallStateChanged( MCCPCallObserver::ECCPStateIdle );
                }
            else
                {
                iAlreadyTerminating = ETrue;
                }
            }
        else
            {
            ExecCbCallStateChanged( MCCPCallObserver::ECCPStateIdle );
            }
        
        SVPDEBUG2( "CSVPMoSession::HangUp Out return=%d", err )
        return err;
        }
    else
        {
        TInt status = CSVPSessionBase::HangUp();
        SVPDEBUG2( "CSVPMoSession::HangUp Out return=%d", status )
        return status;
        }
    }

// ---------------------------------------------------------------------------
// CSVPMoSession::Release
// ---------------------------------------------------------------------------
//  
TInt CSVPMoSession::Release()
    {
    SVPDEBUG1( "CSVPMoSession::Release" )
    delete this;
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CSVPMoSession::Hold
// ---------------------------------------------------------------------------
//  
TInt CSVPMoSession::Hold()
    {
    SVPDEBUG1( "CSVPMoSession::Hold" )
    return CSVPSessionBase::Hold();
    }

// ---------------------------------------------------------------------------
// CSVPMoSession::Resume
// ---------------------------------------------------------------------------
//  
TInt CSVPMoSession::Resume()
    {
    SVPDEBUG1( "CSVPMoSession::Resume" )
    return CSVPSessionBase::Resume();
    }

// ---------------------------------------------------------------------------
// CSVPMoSession::IsMobileOriginated
// ---------------------------------------------------------------------------
//  
TBool CSVPMoSession::IsMobileOriginated() const
    {
    SVPDEBUG1( "CSVPMoSession::IsMobileOriginated = ETrue" )
    return ETrue;
    }

// ---------------------------------------------------------------------------
// CSVPMoSession::SessionStateChangedL
// ---------------------------------------------------------------------------
//
void CSVPMoSession::SessionStateChangedL( TInt aOrigStatus,
    TCCPError aError, TInt aModStatus )
    {
    SVPDEBUG1( "CSVPMoSession::SessionStateChangedL In" )
    SVPDEBUG2( "CSVPMoSession::SessionStateChangedL aOrigStatus=%d",
        aOrigStatus )
    SVPDEBUG2( "CSVPMoSession::SessionStateChangedL aError=%d",
        aError )
    SVPDEBUG2( "CSVPMoSession::SessionStateChangedL aModStatus=%d",
        aModStatus )

    // Check security level, cancel secure mandatory call if the control path 
    // is unsecure and the response is 1XX or 2XX
    CMceSession::TControlPathSecurityLevel secLevel = 
        iSession->ControlPathSecurityLevel();
    
    SVPDEBUG2( "CSVPMoSession::SessionStateChangedL, security level=%d", secLevel )
    
    if ( SecureMandatory() && CMceSession::EControlPathUnsecure == secLevel )
        {
        switch ( aOrigStatus )
            {
            case KSVPRingingVal:
            case KSVPForwardedVal:
            case KSVPQueuedVal:
            case KSVPSessionProgressVal:
            case KSVPOKVal:
            case KSVPAcceptedVal:
                {
                SVPDEBUG1( "CSVPMoSession::SessionStateChangedL, path not secure, cancel call" )
                HangUp();
                ExecCbErrorOccurred( ECCPSecureCallFailed );
                return;
                }
            default:
                {
                break;
                }
            }
        }
    
    if ( KSVPForwardedVal == aOrigStatus )
        {
        SVPDEBUG1( "CSVPMoSession::SessionStateChangedL 181 Call is Being Forwarded received" )
        
        if ( iForwardObserver )
            {
            iForwardObserver->ForwardEventOccurred(
                    MCCPForwardObserver::ECCPRemoteForwarding );
            }
        }
    
    CSVPSessionBase::SessionStateChangedL( aOrigStatus, aError, aModStatus );
    SVPDEBUG1("CSVPMoSession::SessionStateChangedL Out" )
    }
    
// ---------------------------------------------------------------------------
// CSVPMoSession::ForwardProviderL
// ---------------------------------------------------------------------------
// 
MCCPForwardProvider* CSVPMoSession::ForwardProviderL( const MCCPForwardObserver& aObserver )
    {
    SVPDEBUG1( "CSVPMoSession::ForwardProviderL" )
    iForwardObserver = const_cast< MCCPForwardObserver* >( &aObserver );
    return iForwardProvider;
    }

// ---------------------------------------------------------------------------
// CSVPMoSession::GetForwardAddressChoicesL
// ---------------------------------------------------------------------------
// 
const CDesC8Array& CSVPMoSession::GetForwardAddressChoicesL()
    {
    SVPDEBUG1( "CSVPMoSession::GetForwardAddressChoicesL" )
    return *iForwardAddressList;    
    }
    
// ---------------------------------------------------------------------------
// CSVPMoSession::ForwardToAddressL
// ---------------------------------------------------------------------------
// 
void CSVPMoSession::ForwardToAddressL( const TInt aIndex )
    {
    SVPDEBUG1( "CSVPMoSession::ForwardToAddressL In" )
    SVPDEBUG2( "CSVPMoSession::ForwardToAddressL aIndex=%d", aIndex )
    
    if ( iForwardAddressList->MdcaCount() - 1 < aIndex ||
         KErrNone > aIndex )
        {
        SVPDEBUG1( "CSVPMoSession::ForwardToAddressL - Index err! -> Leave!" )
        User::Leave( KErrArgument );
        }
    else
        {
        SVPDEBUG1( "CSVPMoSession::ForwardToAddressL - New mce session" )

        // create SIP and ProfileRegistry for URI handling
        CSIP* sip = CSIP::NewLC( KSVPUid, *this );

        CSIPProfileRegistry* sipProfileRegistry = 
            CSIPProfileRegistry::NewLC( *sip, *this );

        // retrieve SIP profile by using sip profile id
        CSIPProfile* profile = sipProfileRegistry->ProfileL( iSipProfileId );
        CleanupStack::PushL( profile );

        RPointerArray< CRCSEProfileEntry > entryArray;
        CleanupResetAndDestroy<
             RPointerArray<CRCSEProfileEntry> >::PushL( entryArray );

        CRCSEProfileRegistry* reg = CRCSEProfileRegistry::NewLC();
        
        // Get VoIP profile by service id
        reg->FindByServiceIdL( iCallParameters->ServiceId(), entryArray );
        
        // Take first entry from array
        CRCSEProfileEntry* entry = entryArray[0];
       
        // check if user=phone needs to be added to outgoing INVITE     
        TBool userEqphone = EFalse;
        if ( CRCSEProfileEntry::EOn == entry->iUserPhoneUriParameter )
            {
            userEqphone = ETrue;
            }
            
        // cleanup
        CleanupStack::PopAndDestroy( 2, &entryArray );    // reg, entryArray

        // complete sip uri according to the security status
        HBufC8* uri = CompleteUriL( iForwardAddressList->MdcaPoint( aIndex ),
                *profile, SecurePreferred(), userEqphone );
        CleanupStack::PushL( uri );
        
        // Delete old mce session and create a new one
        if ( iSession )
            {
            delete iSession;
            iSession = NULL;
            }
        
        TRAPD( err, iSession = CMceOutSession::NewL( *iMceManager, *profile, *uri ) );
        
        if ( KUriUtilsErrInvalidUserInfo == err ||
             KUriUtilsErrInvalidHost == err )
            {
            SVPDEBUG1( "CSVPMoSession::ForwardToAddressL KSVPBadUriError" )
            User::Leave( KErrArgument );
            }
        if ( err )
            {
            SVPDEBUG1( "CSVPMoSession::ForwardToAddressL KErrDisconnected" )
            User::Leave( KErrDisconnected );
            }

        // constructs audio streams for this Mo session 
        // adds codecs to streams and streams to session 
        ConstructAudioStreamsL();

        CleanupStack::PopAndDestroy( 4 ); // uri, profile, sipProfileRegistry, sip
        
        User::LeaveIfError( Dial() );
        
        SVPDEBUG1( "CSVPMoSession::ForwardToAddressL Out" )
        }
    }

// ---------------------------------------------------------------------------
// CSVPMoSession::AddForwardObserverL
// ---------------------------------------------------------------------------
// 
void CSVPMoSession::AddForwardObserverL( const MCCPForwardObserver& aObserver )
    {
    SVPDEBUG1( "CSVPMoSession::AddForwardObserverL" )
    iForwardObserver = const_cast<MCCPForwardObserver*>( &aObserver );
    }

// ---------------------------------------------------------------------------
// CSVPMoSession::RemoveForwardObserver
// ---------------------------------------------------------------------------
// 
TInt CSVPMoSession::RemoveForwardObserver( const MCCPForwardObserver& aObserver )
    {
    SVPDEBUG1( "CSVPMoSession::RemoveForwardObserver In" )
    
    TInt err = KErrNone;
    
    if ( const_cast<MCCPForwardObserver*>( &aObserver ) == iForwardObserver )
        {
        iForwardObserver = NULL;
        }
    else
        {
        err = KErrNotFound;
        }
    
    SVPDEBUG2( "CSVPMoSession::AddForwardObserverL Out return=%d", err )
    return err;
    }

// ---------------------------------------------------------------------------
// CSVPMoSession::NotifyForwardEvent
// ---------------------------------------------------------------------------
// 
void CSVPMoSession::NotifyForwardEvent( TInt aEventCode )
    {
    SVPDEBUG1( "CSVPMoSession::NotifyForwardEvent In" )
    SVPDEBUG2( "CSVPMoSession::NotifyForwardEvent aEventCode=%d", aEventCode )
    
    if ( iForwardObserver )
        {
        switch ( aEventCode )
            {
            case KSVPMultipleChoicesVal:    // 300
                {
                iForwardObserver->ForwardEventOccurred( 
                    MCCPForwardObserver::ECCPMultipleChoices );
                break;
                }
            case KSVPMovedPermanentlyVal:   // 301
                {
                iForwardObserver->ForwardEventOccurred( 
                    MCCPForwardObserver::ECCPMovedPermanentlyEvent );
                break;
                }
            case KSVPMovedTemporarilyVal:   // 302
                {
                iForwardObserver->ForwardEventOccurred( 
                    MCCPForwardObserver::ECCPMovedTemporarily );
                break;
                }
            default:
                {
                SVPDEBUG1( "CSVPMoSession::NotifyForwardEvent unknown event" )
                break;
                }
            }
        }
    else
        {
        SVPDEBUG1( "CSVPMoSession::NotifyForwardEvent iForwardObserver not exists" )
        }
    
    SVPDEBUG1( "CSVPMoSession::NotifyForwardEvent Out" )
    }

// ---------------------------------------------------------------------------
// CSVPMoSession::AddForwardAddressL
// ---------------------------------------------------------------------------
// 
TInt CSVPMoSession::AddForwardAddressL( const TDesC8& aFwdAddress )
    {
    SVPDEBUG1("CSVPMoSession::AddForwardAddressL In" )

    // On return, result contains count of appended forward addresses 
    // or an error code
    TInt result( 0 );
    TInt count( 0 );

    // Indicates length of unsearched part of string
    TInt lenRight( aFwdAddress.Length() );
    TBool ready( EFalse );
    
    while ( !ready )
        {
        TPtrC8 remainingBuf = aFwdAddress.Right( lenRight );
        
        // Search comma
        TInt offset = remainingBuf.Find( KSVPComma );

        if ( KErrNotFound != offset )
            {
            lenRight -= offset + 1; // Remove comma from remaining string
            }
        else
            {
            // Only one forward address remains
            offset = lenRight;
            ready = ETrue;
            }

        // Remove unnecessary spaces from the string
        HBufC8* address = remainingBuf.Left( offset ).AllocLC();
        TPtr8 modAddress = address->Des();
        modAddress.TrimAll();

        iForwardAddressList->AppendL( *address );
        CleanupStack::PopAndDestroy( address );

        count++;
        result = count;
        }
    
    SVPDEBUG2( "CSVPMoSession::AddForwardAddressL Out return=%d", result )
    return result;
    }

// ---------------------------------------------------------------------------
// CSVPMoSession::ResetForwardAddressChoices
// ---------------------------------------------------------------------------
// 
void CSVPMoSession::ResetForwardAddressChoices()
    {
    SVPDEBUG1( "CSVPMoSession::ResetForwardAddressChoices" )
    iForwardAddressList->Reset();
    }

// SIP

// ---------------------------------------------------------------------------
// CSVPMoSession::IncomingRequest
// ---------------------------------------------------------------------------
//  
void CSVPMoSession::IncomingRequest( TUint32 /*aIapId*/, 
        CSIPServerTransaction* /*aTransaction*/ )
    {
    SVPDEBUG1( "CSVPMoSession::IncomingRequest" )
    }

// ---------------------------------------------------------------------------
// CSVPMoSession::TimedOut
// ---------------------------------------------------------------------------
//                               
void CSVPMoSession::TimedOut( CSIPServerTransaction& /*aSIPServerTransaction*/ )
    {
    SVPDEBUG1( "CSVPMoSession::TimedOut SIPServerTransaction timed out" )
    }

// ---------------------------------------------------------------------------
// CSVPMoSession::ProfileRegistryErrorOccurred
// ---------------------------------------------------------------------------
//  
void CSVPMoSession::ProfileRegistryErrorOccurred( TUint32 /*aSIPProfileId*/,
        TInt /*aError*/ )
    {
    SVPDEBUG1( "CSVPMoSession::ProfileRegistryErrorOccurred" )
    }

// ---------------------------------------------------------------------------
// CSVPMoSession::ProfileRegistryEventOccurred
// ---------------------------------------------------------------------------
//  
void CSVPMoSession::ProfileRegistryEventOccurred( TUint32 /*aProfileId*/,
        TEvent /*aEvent*/ )
    {
    SVPDEBUG1( "CSVPMoSession::ProfileRegistryEventOccurred" )
    }

