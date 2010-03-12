/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  implementation of sipmxresolver
*
*/


// INCLUDE FILES
#include "sipmxresolver.h"
#include "csipclientresolverutils.h"
#include <e32base.h>
#include <e32std.h>
#include <e32property.h>

//for checking CS call status
#include <ctsydomainpskeys.h>

//SIP message content parsing APIs
#include <sipheaderbase.h>
#include <sipacceptcontactheader.h>
#include <sipaddress.h>
#include <sdpdocument.h>
#include <sdpmediafield.h>
#include <sdpcodecstringpool.h>
#include <sdpcodecstringconstants.h>
#include <sdpattributefield.h>
#include <sipstrings.h>
#include <sipallowheader.h>
#include <sipsupportedheader.h>

//For checking dynamic voip status and MuS availability
#include <featmgr.h>
#include <centralrepository.h>
#include <settingsinternalcrkeys.h>

//Multimedia Sharing client API
#include <musmanager.h>

//Incoming Call Monitor API
#include <icmapi.h>


// CONSTANTS
_LIT8( KFTagChar, "+" ); 						// All feature tags start with +
_LIT8( KVSFeatureTag, "+g.3gpp.cs-voice" );		// For checking VS specific tag
_LIT8( KPoCFeatureTag, "+g.poc.talkburst" ); 	// PoC specific tag (for ruling out)
_LIT8( KContentType, "application/sdp" );		// For content-type header
_LIT8( KMediaTypeApp, "application" );        	// For content-type header checks
_LIT8( KMediaSubtypeSdp, "sdp" );            	// For content-type header checks
_LIT8( KSendOnly, "sendonly" );            		// For attribute checks
_LIT8( KApplicationAttr, "application" );   	// For attribute checks
_LIT8( KXApplicationAttr, "X-application" );	// For attribute checks
_LIT8( KNokiaRtvs, "com.nokia.rtvs" );       	// For attribute checks
_LIT8( KSIPMethodsInAllowHeader, "INVITE,ACK,CANCEL,OPTIONS,BYE,PRACK,SUBSCRIBE,REFER,NOTIFY,UPDATE");	// SIP Methods allowed by various plugins
_LIT8( KSIPExtensionsSupported, "100rel,timer,sec-agree"); //Extensions supported by various plugins

/**
 * Cleanup function for RPointerArray
 * Called in case of a leave in SupportedSdpMediasL
 */
void CleanupSdpMediasArray( TAny* aObj )
    {
    if ( aObj )
        {
        static_cast<RPointerArray<CSdpMediaField>*>( aObj )->ResetAndDestroy();
        }
    }


// ============================ MEMBER FUNCTIONS =============================
// ---------------------------------------------------------------------------
// CSipMXResolver::CSipMXResolver
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CSipMXResolver::CSipMXResolver() 
    {
    }


// ---------------------------------------------------------------------------
// CSipMXResolver::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
// 
void CSipMXResolver::ConstructL()
    {
    SIPMXRLOG( "[SIPMXRESOLVER] -> CSipMXResolver::ConstructL()" )
    
    // Open sdp string pool (needed in media field checks)
    TRAPD( err, SdpCodecStringPool::OpenL() );
    
    switch ( err )
	    {
    	case KErrNone:
	    	{
	    	//close pool at destructor, not opened by others
			iCloseStringPool = ETrue;
			break;
	    	}
	    
	    case KErrAlreadyExists:
	    	{
	    	//already opened, do not try to close at destructor
	    	iCloseStringPool = EFalse;
	    	break;
	    	}
	    
	    default:
	    	{
	    	User::Leave( err );
	    	}
	    }
    
    // Check VoIP and Multimedia Sharing availability
    FeatureManager::InitializeLibL();
    
    TInt dynvoip = 0;
    CRepository* repository = CRepository::NewL( KCRUidTelephonySettings );
    repository->Get( KDynamicVoIP, dynvoip );
    delete repository;
    
    iVoIPEnabled = ( dynvoip && FeatureManager::FeatureSupported( KFeatureIdCommonVoip ) );
    
    if ( FeatureManager::FeatureSupported( KFeatureIdMultimediaSharing ) )
        {
        iMuSManager = CMusManager::NewL();
        }
    
    FeatureManager::UnInitializeLib();
    
    // initialize media type strings
    iAudioType = SdpCodecStringPool::StringPoolL().StringF(
                    SdpCodecStringConstants::EMediaAudio,
                        SdpCodecStringPool::StringTableL() );
    
    iVideoType = SdpCodecStringPool::StringPoolL().StringF(
                    SdpCodecStringConstants::EMediaVideo,
                        SdpCodecStringPool::StringTableL() );
    
    SIPMXRLOG( "[SIPMXRESOLVER] <- CSipMXResolver::ConstructL()" )
    }


// ---------------------------------------------------------------------------
// CSipMXResolver::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
// 
CSipMXResolver* CSipMXResolver::NewL()
    {
    CSipMXResolver* self = new( ELeave ) CSipMXResolver;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self);
    return self;
    }


// Destructor
CSipMXResolver::~CSipMXResolver()
    {
    SIPMXRLOG( "[SIPMXRESOLVER] -> CSipMXResolver::~CSipMXResolver()" )
    
    delete iMuSManager;
    iAudioType.Close();
    iVideoType.Close();
    
    if ( iCloseStringPool )
	    {
	    SdpCodecStringPool::Close();
	    }
    
    SIPMXRLOG( "[SIPMXRESOLVER] <- CSipMXResolver::~CSipMXResolver()" )
    }


// ---------------------------------------------------------------------------
// CSipMXResolver::MatchAcceptContactsL
// ---------------------------------------------------------------------------
// 
TBool CSipMXResolver::MatchAcceptContactsL(
            RStringF aMethod,
            const CUri8& /*aRequestUri*/,
            const RPointerArray<CSIPHeaderBase>& aHeaders,
            const TDesC8& /*aContent*/,
            const CSIPContentTypeHeader* /*aContentType*/,
            TUid& aClientUid )
    {
    SIPMXRLOG( "[SIPMXRESOLVER] -> CSipMXResolver::MatchAcceptContactsL()" )
    
    TBool match = EFalse;
    
    // check if we have active CS call
    // and MM Sharing is enabled (otherwise this can't be MuS)
    if ( iMuSManager && IsCSCallActive() )
        {
        if ( CheckForACHeaderTagL( aHeaders, KVSFeatureTag ) )
            {
            match = ETrue;
            aClientUid = ResolveVSUidL( aMethod );
            }
        }
    
    SIPMXRLOGP( \
    "[SIPMXRESOLVER] <- CSipMXResolver::MatchAcceptContactsL(), ret:%d", match )
    return match;
    }


// ---------------------------------------------------------------------------
// CSipMXResolver::MatchEventL
// ---------------------------------------------------------------------------
// 
TBool CSipMXResolver::MatchEventL(
            RStringF /*aMethod*/,
            const CUri8& /*aRequestUri*/,
            const RPointerArray<CSIPHeaderBase>& /*aHeaders*/,
            const TDesC8& /*aContent*/,
            const CSIPContentTypeHeader* /*aContentType*/,
            TUid& /*aClientUid*/)
    {
    //SipMXResolver returns always EFalse for MatchEventL calls
    SIPMXRLOG( "[SIPMXRESOLVER] <-> CSipMXResolver::MatchEventL()" )
    return EFalse;
    }


// ---------------------------------------------------------------------------
// CSipMXResolver::MatchRequestL
// ---------------------------------------------------------------------------
// 
TBool CSipMXResolver::MatchRequestL(
            RStringF aMethod,
            const CUri8& aRequestUri,
            const RPointerArray<CSIPHeaderBase>& aHeaders,
            const TDesC8& aContent,
            const CSIPContentTypeHeader* aContentType,
            TUid& aClientUid )
    {
    SIPMXRLOG( "[SIPMXRESOLVER] -> CSipMXResolver::MatchRequestL()" )
    
    TBool match = EFalse;
    
    //poc specific requests will not be processed
    TBool poc = CheckForACHeaderTagL( aHeaders, KPoCFeatureTag );
    
    //OPTIONS and INVITE requests are processed
    TBool requestok = ( aMethod == SIPStrings::StringF( SipStrConsts::EOptions ) ||
         aMethod == SIPStrings::StringF( SipStrConsts::EInvite ) );
    
    //application/sdp content type is required
    TBool contentok = ( aContent.Length() > 0 && aContentType &&
         aContentType->MediaType().CompareF( KMediaTypeApp ) == 0 &&
         aContentType->MediaSubtype().CompareF( KMediaSubtypeSdp ) == 0 );
    
    // do further checks only if we have either VoIP and/or MuS active,
    // content type must be valid and accept-contact is not poc specific
    if ( ( iVoIPEnabled || iMuSManager ) && requestok && contentok && !poc )
        {
        CSdpDocument* sdpDocument = CSdpDocument::DecodeLC( aContent );
        
        // determine the parameters
        TBool hasAudio = CheckForMedia( sdpDocument, iAudioType );
        TBool hasVideo = CheckForMedia( sdpDocument, iVideoType );
        TBool cscall = IsCSCallActive();
        
        if ( iMuSManager && hasVideo && hasAudio && cscall )
            {
            // both audio and video medias present and cs call on
            // and multimedia sharing enabled =>
            // we need to do some further resolving for client
            if ( CheckForVSAttributes( sdpDocument->MediaFields() ) )
                {
                // this is VS
                match = ETrue;
                aClientUid = ResolveVSUidL( aMethod );
                }
            else if ( iVoIPEnabled )
                {
                // no vs attributes and voip status is enabled
                match = ETrue;
                ResolveCPPluginUidL( aClientUid, aRequestUri );
                }
            else
                {
                // possible voip match, but voip not activated
                match = EFalse;
                }
            }
        else if ( iMuSManager && hasVideo && cscall )
            {
            //video media only && cs call ongoing && multimedia sharing enabled
            // => Multimedia Sharing
            match = ETrue;
            aClientUid = ResolveVSUidL( aMethod );
            }
        else if ( hasAudio && iVoIPEnabled )
            {
            // audio only or audio and video and no cs call + voip supported
            // => this is VoIP
            match = ETrue;
            ResolveCPPluginUidL( aClientUid, aRequestUri );
            }
        else
            {
            // no medias or has video but no CS call or has audio
            // but voip status is disabled 
            // => no match
            match = EFalse;
            }
        
        CleanupStack::PopAndDestroy( sdpDocument );
        }
    else if ( aMethod == SIPStrings::StringF( SipStrConsts::EInvite ) &&
    		  iVoIPEnabled && !poc && aContent.Length() == 0 )
    	{
    	// Pull model: this is a re-Invite without content
    	ResolveCPPluginUidL( aClientUid, aRequestUri );
    	match = ETrue;
    	}
    else
    	{
    	//no match
    	match = EFalse;
        }
    
    SIPMXRLOGP( \
    "[SIPMXRESOLVER] <- CSipMXResolver::MatchRequestL(), ret:%d", match )
    return match;
    }


// ---------------------------------------------------------------------------
// CSipMXResolver::ConnectSupported
// ---------------------------------------------------------------------------
// 
TBool CSipMXResolver::ConnectSupported()
    {
    return ETrue;
    }


// ---------------------------------------------------------------------------
// CSipMXResolver::ConnectL
// ---------------------------------------------------------------------------
//
void CSipMXResolver::ConnectL( const TUid& aClientUid )
    {
    SIPMXRLOGP(\
    "[SIPMXRESOLVER] -> CSipMXResolver::ConnectL(), uid:%d", aClientUid.iUid )
    
    // If Uid matches with MuS, forward to MusManager
    if ( iMuSManager &&
         ( aClientUid.iUid == CMusManager::ESipOptions ||
           aClientUid.iUid == CMusManager::ESipInviteDesired ||
           aClientUid.iUid == CMusManager::ESipInviteNotDesired ) )
        {
        iMuSManager->HandleSipRequestL(
                  ( CMusManager::TRequestType ) aClientUid.iUid );
        }
    else
        {
        // start through ICM
        TInt result = RProperty::Set( KPSUidICMIncomingCall,
        		KPropertyKeyICMPluginUID, aClientUid.iUid );
        User::LeaveIfError( result );
        }
    
    SIPMXRLOG( "[SIPMXRESOLVER] <- CSipMXResolver::ConnectL()" )
    }


// ---------------------------------------------------------------------------
// CSipMXResolver::CancelConnect
// ---------------------------------------------------------------------------
// 
void CSipMXResolver::CancelConnect( const TUid& /*aClientUid*/ )
    {
    SIPMXRLOG( "[SIPMXRESOLVER] <-> CSipMXResolver::CancelConnect()" )
    // No implementation for VoIP / MuS.
    }


// ---------------------------------------------------------------------------
// CSipMXResolver::SupportedContentTypesL
// ---------------------------------------------------------------------------
// 
RPointerArray<CSIPContentTypeHeader> CSipMXResolver::SupportedContentTypesL()
    {
    RPointerArray<CSIPContentTypeHeader> ret;
    
    CSIPContentTypeHeader* ctype = CSIPContentTypeHeader::DecodeL( KContentType );
    CleanupStack::PushL( ctype );
    ret.AppendL( ctype );
    CleanupStack::Pop( ctype );
    
    return ret;
    }


// ---------------------------------------------------------------------------
// CSipMXResolver::SupportedSdpMediasL
// ---------------------------------------------------------------------------
// 
RPointerArray<CSdpMediaField> CSipMXResolver::SupportedSdpMediasL()
    {
    // Initialise return array
    RPointerArray<CSdpMediaField> ret;
    TCleanupItem tci( CleanupSdpMediasArray, &ret );
    CleanupStack::PushL( tci );
    
    // media field descriptors
    _LIT8( KMuSField1, "m=video 0 RTP/AVP 96\r\na=application:com.nokia.rtvs\r\na=X-application:com.nokia.rtvs\r\n" );
    _LIT8( KMuSField2, "m=audio 0 RTP/AVP 97\r\n" );
    _LIT8( KVoIPField1, "m=video 0 RTP/AVP 0\r\n" );
    _LIT8( KVoIPField2, "m=application 0 tcp wb\r\n" );
    _LIT8( KVoIPField3, "m=audio 0 RTP/AVP 0\r\n" );
    _LIT8( KVoIPField4, "m=audio 0 RTP/SAVP 0\r\n" );
    
    CSdpMediaField* field = NULL;
    
    // add media fields to array
    if ( iMuSManager )
        {
        field = CSdpMediaField::DecodeLC( KMuSField1 );
        ret.AppendL( field ); // ownership to array
        CleanupStack::Pop( field );
        
        field = CSdpMediaField::DecodeLC( KMuSField2 );
        ret.AppendL( field );
        CleanupStack::Pop( field );
        }
    
    if ( iVoIPEnabled )
        {
        field = CSdpMediaField::DecodeLC( KVoIPField1 );
        ret.AppendL( field );
        CleanupStack::Pop( field );
        
        field = CSdpMediaField::DecodeLC( KVoIPField2 );
        ret.AppendL( field );
        CleanupStack::Pop( field );
        
        field = CSdpMediaField::DecodeLC( KVoIPField3 );
        ret.AppendL( field );
        CleanupStack::Pop( field );
        
        field = CSdpMediaField::DecodeLC( KVoIPField4 );
        ret.AppendL( field );
        CleanupStack::Pop( field );
        }
    
    CleanupStack::Pop(); // tcleanupitem
    return ret;
    }


// ---------------------------------------------------------------------------
// CSipMXResolver::AddClientSpecificHeadersForOptionsResponseL
// ---------------------------------------------------------------------------
// 
void CSipMXResolver::AddClientSpecificHeadersForOptionsResponseL(
    RPointerArray<CSIPHeaderBase>& aHeaders )
    {
    SIPMXRLOG( "[SIPMXRESOLVER] -> \
    CSipMXResolver::AddClientSpecificHeadersForOptionsResponseL()" )
	//Add Allow Header		
	RPointerArray<CSIPAllowHeader> allowheaders = 
	    CSIPAllowHeader::DecodeL(KSIPMethodsInAllowHeader);
	TInt count = allowheaders.Count();
	for(TInt i = 0; i<count; i++)
		{
		User::LeaveIfError(aHeaders.Append(allowheaders[i]));
		}
	allowheaders.Reset();

	//Add Supported Header
	RPointerArray<CSIPSupportedHeader> supportedheaders = 
	    CSIPSupportedHeader::DecodeL(KSIPExtensionsSupported);
	count = supportedheaders.Count();
	for(TInt j = 0; j<count; j++)
		{
		User::LeaveIfError(aHeaders.Append(supportedheaders[j]));
		}
	supportedheaders.Reset();
    }


// ---------------------------------------------------------------------------
// CSipMXResolver::IsCsCallActive
// ---------------------------------------------------------------------------
// 
TBool CSipMXResolver::IsCSCallActive() const
    {
    SIPMXRLOG( "[SIPMXRESOLVER] -> CSipMXResolver::IsCSCallActive()" )
    
    TBool retval = EFalse;
    
    TInt callType = KErrNone;
    RProperty::Get( KPSUidCtsyCallInformation,
    		KCTsyCallType, callType );
    
    if ( EPSCTsyCallTypeCSVoice == callType )
        {
        TInt callState = KErrNone;
        RProperty::Get( KPSUidCtsyCallInformation,
        		KCTsyCallState, callState );
        
        if ( EPSCTsyCallStateConnected == callState ||
             EPSCTsyCallStateHold == callState )
            {
            retval = ETrue;
            }
        }
    
    SIPMXRLOGP( \
    "[SIPMXRESOLVER] <- CSipMXResolver::IsCSCallActive(), ret:%d", retval )
    
    return retval;
    }

// -----------------------------------------------------------------------------
// CSipMXResolver::CheckForMedia
// -----------------------------------------------------------------------------
//
TBool CSipMXResolver::CheckForMedia( CSdpDocument* aSdpDoc,
    const RStringF& aMediaType ) const
    {
    SIPMXRLOG( "[SIPMXRESOLVER] -> CSipMXResolver::CheckForMedia()" )
    
    TBool present = EFalse;
    
    if ( aSdpDoc )
        {
        RPointerArray<CSdpMediaField>& mFields = aSdpDoc->MediaFields();
        const TInt count = mFields.Count();
        
        // if there is media fields, check them for type
        for ( TInt i = 0; i < count && !present; i++ )
            {
            if ( aMediaType == mFields[ i ]->Media() )
                {
                present = ETrue;
                }
            }
        }
    
    SIPMXRLOGP( \
    "[SIPMXRESOLVER] <- CSipMXResolver::CheckForMedia(), ret:%d", present )
    
    return present;
    }


// ---------------------------------------------------------------------------
// CSipMXResolver::CheckForACHeaderTagL
// ---------------------------------------------------------------------------
// 
TBool CSipMXResolver::CheckForACHeaderTagL(
    const RPointerArray<CSIPHeaderBase>& aHeaders,
    const TDesC8& aTag ) const
    {
    TBool match = EFalse;
    TInt ftagcount = 0;
    
    for ( TInt i = 0; i < aHeaders.Count(); i++ )
        {
        if ( aHeaders[i]->Name() ==
             SIPStrings::StringF( SipStrConsts::EAcceptContactHeader ) ||
             aHeaders[i]->Name() ==
             SIPStrings::StringF( SipStrConsts::EAcceptContactHeaderCompact ) )
            {
            RStringF featureTagStr = SIPStrings::Pool().OpenFStringL( aTag );
            CleanupClosePushL( featureTagStr );
            
            CSIPAcceptContactHeader* ach = 
                static_cast<CSIPAcceptContactHeader*>(aHeaders[i]);
            TInt pcount = ach->ParamCount();
            
            for ( TInt p = 0; p < pcount; p++ )
                {
                RStringF mparam;
                CleanupClosePushL( mparam );
                
                if ( KErrNone == ach->Param( p, mparam ) )
                    {
                    //we need to count all feature tags
                    if ( mparam.DesC().Left(1).Compare( KFTagChar ) == 0 )
                        {
                        ftagcount++;
                        }
                    if ( mparam == featureTagStr )
                        {
                        match = ETrue;
                        //loop is continued after match to count feature tags
                        }
                    }
                
                CleanupStack::PopAndDestroy( 1 ); //mparam
                }
            
            // Use the dynamic string.
            CleanupStack::PopAndDestroy( 1 ); // featureTagStr
            }
        }
    
    //return ETrue only if there's only one feature tag and it is MuS specific
    return ( match && ftagcount == 1 );
    }


// ---------------------------------------------------------------------------
// CSipMXResolver::CheckForVSAttributesL
// ---------------------------------------------------------------------------
// 
TBool CSipMXResolver::CheckForVSAttributes(
    RPointerArray<CSdpMediaField>& aFields ) const
    {
    SIPMXRLOG( "[SIPMXRESOLVER] -> CSipMXResolver::CheckForVSAttributes()" )
    
    TBool vsattributefound = EFalse;
    TInt sendonlycount = 0; 
    TInt videofields = 0;
    const TInt fieldcount = aFields.Count();
    
    for ( TInt i = 0; i < fieldcount; i++ )
        {
        CSdpMediaField* mField = aFields[i];
        
        //only check audio/video fields
        TBool audio = mField->Media() == iAudioType;
        TBool video = mField->Media() == iVideoType;
        
        if ( video )
            {
            videofields++;
            }
        
        if ( video || audio )
            {
            RPointerArray< CSdpAttributeField > attrList =
                mField->AttributeFields();
            
            TInt attrCount = attrList.Count();
            for (TInt j = 0; j < attrCount && !vsattributefound; j++ )
                {
                CSdpAttributeField* attributeField = attrList[j];
                RStringF attribute = attributeField->Attribute();
                
                // for each sendonly attribute, add counter
                if ( KErrNotFound != attribute.DesC().FindF( KSendOnly ) )
                    {
                    sendonlycount++;                        
                    }
                
                // check m=video fields for com.nokia.rtvs attribute
                if ( video )
                    {
                    if ( ( KErrNotFound !=
                           attribute.DesC().FindF( KApplicationAttr ) ||
                           KErrNotFound !=
                           attribute.DesC().FindF( KXApplicationAttr ) ) &&
                         KErrNotFound !=
                         attributeField->Value().FindF( KNokiaRtvs ) )
                        {
                        //attribute found
                        vsattributefound = ETrue;
                        }
                    }
                }
            }
        }
    
    // Video Sharing is assumed if nokia vs specific attributes are found
    // or if there is at least one m=video line and all media fields have
    // sendonly attribute.
    TBool retval = ( vsattributefound ||
        		     ( videofields > 0 && fieldcount == sendonlycount ) );
    
    SIPMXRLOGP( \
    "[SIPMXRESOLVER] <- CSipMXResolver::CheckForVSAttributes(),ret:%d", retval )
    
    return retval;
    }


// ---------------------------------------------------------------------------
// CSipMXResolver::ResolveVSUid
// ---------------------------------------------------------------------------
// 
TUid CSipMXResolver::ResolveVSUidL( const RStringF& aMethod )
    {
    SIPMXRLOG( "[SIPMXRESOLVER] -> CSipMXResolver::ResolveVSUidL()" )
    
    TUid muid  = KNullUid;
    
    if ( iMuSManager )
        {
        if ( aMethod == SIPStrings::StringF( SipStrConsts::EOptions ) )
            {
            muid.iUid = ( TInt32 ) CMusManager::ESipOptions;
            }
        else
            {
            MultimediaSharing::TMusAvailabilityStatus capability =
            	iMuSManager->AvailabilityL();
            
            //check if availability value is from non-error-range
            if( capability >= KErrNone &&
                capability < MultimediaSharing::EErrSipRegistration )
                {
                muid.iUid = ( TInt32 ) CMusManager::ESipInviteDesired;
                }
            else
                {
                muid.iUid = ( TInt32 ) CMusManager::ESipInviteNotDesired;
                }
            }
        }
    
    SIPMXRLOGP( \
    "[SIPMXRESOLVER] <- CSipMXResolver::ResolveVSUidL(), ret:%d", muid.iUid )
    
    return muid;
    }


// -----------------------------------------------------------------------------
// CSipMXResolver::ResolveCPPluginUidL
// -----------------------------------------------------------------------------
// 
void CSipMXResolver::ResolveCPPluginUidL( TUid& aUid, const CUri8& aRequestUri )
    {
    SIPMXRLOG( "[SIPMXRESOLVER] -> CSipMXResolver::ResolveCPPluginUidL()" )
    
    CSipClientResolverUtils* resolver = CSipClientResolverUtils::NewLC();
    resolver->GetImplementationUidWithUserL(
    		aRequestUri.Uri().Extract( EUriUserinfo ), aUid );
    CleanupStack::PopAndDestroy( resolver );
    
    SIPMXRLOGP( \
    "[SIPMXRESOLVER] <- CSipMXResolver::ResolveCPPluginUidL(),uID:%d", aUid.iUid )
    }


//  End of File
