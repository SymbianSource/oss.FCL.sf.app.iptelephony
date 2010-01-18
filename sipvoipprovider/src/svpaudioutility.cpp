/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides static utility functions for SVP
*
*/


#include <mceaudiostream.h>
#include <mcertpsource.h>
#include <mceaudiocodec.h>
#include <mcespeakersink.h>
#include <mcemicsource.h>
#include <AudioPreference.h>
#include "svpaudioutility.h"
#include "svplogger.h"
#include "svpconsts.h"
#include "svpsessionbase.h"
#include "svpemergencysession.h"
#include "svpholdcontext.h"
#include "svpholdcontroller.h"

// LOCAL CONSTANTS
// Constants for VoIP uplink audio preferences, renamed for better
// readability and to clarify between IB/OB DTMF's. 
// See <audiopreference.h> for more information.
// Inband signalling uplink preference (i.e inband DTMF)
const TUint KSVPAudioPrefUplinkIbDtmf = KAudioPrefVoipAudioUplinkNonSignal;
// Outband signalling uplink preference (i.e outband DTMF)
const TUint KSVPAudioPrefUplinkOobDtmf = KAudioPrefVoipAudioUplink;


// ---------------------------------------------------------------------------
// SVPAudioUtility::IsDownlinkStream
// ---------------------------------------------------------------------------
//
TBool SVPAudioUtility::IsDownlinkStream( CMceAudioStream& aStream )
    {
    SVPDEBUG2( "SVPAudioUtility::IsDownlinkStream source: 0x%x", aStream.Source() );
    
    if ( aStream.Source() && KMceRTPSource == aStream.Source()->Type() )
        {
        SVPDEBUG1( "SVPAudioUtility::IsDownlinkStream RTP SOURCE => DOWNLINK" );
        return ETrue;
        }
    else
        {
        SVPDEBUG1( "SVPAudioUtility::IsDownlinkStream EFalse" );
        
        return EFalse;
        }
    }

// ---------------------------------------------------------------------------
// SVPAudioUtility::SetPriorityCodecValuesL
// ---------------------------------------------------------------------------
//
TBool SVPAudioUtility::SetPriorityCodecValuesL(
    CMceAudioStream& aAudioInStream,
    CMceAudioStream& aAudioOutStream )
    {
    SVPDEBUG1("SVPAudioUtility::SetPriorityCodecValuesL In");
    
    // set MMF codec priorities  for all codecs in both streams
    const TInt inCodecCount = aAudioInStream.Codecs().Count();
    SVPDEBUG2(
        "SVPAudioUtility::SetPriorityCodecValuesL codeccount IN: %d",
            inCodecCount )
     
    const TInt outCodecCount = aAudioOutStream.Codecs().Count();
    SVPDEBUG2(
        "SVPAudioUtility::SetPriorityCodecValuesL codeccount OUT: %d",
            outCodecCount )
    
    const TBool inDtmf = SVPAudioUtility::SetDtmfPriorityL( aAudioInStream );
    const TBool outDtmf = SVPAudioUtility::SetDtmfPriorityL( aAudioOutStream );
    
    // inDtmf and outDtmf _must_ match. Either inband (both EFalse) or outband
    // (both ETrue).
    if ( inDtmf != outDtmf )
        {
        SVPDEBUG1("SVPAudioUtility::SetPriorityCodecValuesL KErrNotFound");
        User::Leave( KErrNotFound );
        }
    
    SVPDEBUG1( "SVPAudioUtility::SetPriorityCodecValuesL set IN priorities" );
    
    SetAudioCodecPrioritiesL( aAudioInStream, KAudioPrefVoipAudioDownlink );
    
    // Check which version of DTMF's are we using, inband or outband.
    TBool outband( EFalse );
    TUint uplinkPreference = KSVPAudioPrefUplinkIbDtmf;
    if ( inDtmf && outDtmf )
        {
        SVPDEBUG1(
            "SVPAudioUtility::SetPriorityCodecValuesL change to OOB DTMF" );
        
        uplinkPreference = KSVPAudioPrefUplinkOobDtmf;
        outband = ETrue;
        }
    // else inband used.
    
    SVPDEBUG1( "SVPAudioUtility::SetPriorityCodecValuesL set OUT priorities" );
    
    SetAudioCodecPrioritiesL( aAudioOutStream, uplinkPreference );
    
    SVPDEBUG1( "SVPAudioUtility::SetPriorityCodecValuesL Out" );
    
    return outband;
    }

// ---------------------------------------------------------------------------
// SVPAudioUtility::SetDtmfPriorityL
// ---------------------------------------------------------------------------
//
TBool SVPAudioUtility::SetDtmfPriorityL( CMceAudioStream& aAudioStream )
    {
    SVPDEBUG1( "SVPAudioUtility::SetDtmfPriorityL In" );
    
    const RPointerArray<CMceAudioCodec>& codecs = aAudioStream.Codecs();
    TInt codCount( codecs.Count() );
    
    // loop through codecs in in stream
    while ( codCount )
        {
        codCount--;
        
        SVPDEBUG2( "SVPAudioUtility::SetDtmfPriorityL, round: %d", codCount );
        
        // Check if codec is DTMF, note the return values of CompareF
        if( !codecs[ codCount ]->SdpName().CompareF( KTELEVENT ) )
            {
            SVPDEBUG1( "SVPAudioUtility::SetDtmfPriorityL ETrue" );
            
            SVPDEBUG2( "SVPAudioUtility::SetDtmfPriorityL curr.prio: %d",
                codecs[codCount]->MMFPriority() )
            SVPDEBUG2( "SVPAudioUtility::SetDtmfPriorityL curr.pref: 0x%x",
                codecs[codCount]->MMFPriorityPreference() )
            
            // Set the priorities.
            codecs[ codCount ]->SetMMFPriorityL( KAudioPriorityDTMFString );
            codecs[ codCount ]->SetMMFPriorityPreferenceL( KAudioDTMFString );
            return ETrue;
            }
        }
    
    SVPDEBUG1( "SVPAudioUtility::SetDtmfPriorityL EFalse" );
    return EFalse;
    }

// ---------------------------------------------------------------------------
// SVPAudioUtility::SetAudioCodecPrioritiesL
// ---------------------------------------------------------------------------
//
void SVPAudioUtility::SetAudioCodecPrioritiesL(
    CMceAudioStream& aAudioStream,
    TUint aAudioPref )
    {
    SVPDEBUG2( "SVPAudioUtility::SetAudioCodecPrioritiesL aAudioPref: 0x%x",
        aAudioPref );
    
    const RPointerArray<CMceAudioCodec>& codecs = aAudioStream.Codecs();
    TInt codCount( codecs.Count() );
    
    // loop through codecs in in stream
    while ( codCount )
        {
        codCount--;
        
        SVPDEBUG2( "SVPAudioUtility::SetAudioCodecPrioritiesL, round: %d",
            codCount );
        
        // Handle only real audio codecs.
        if( codecs[ codCount ]->SdpName().CompareF( KTELEVENT ) )
            {
            SVPDEBUG2( "SVPAudioUtility::SetAudioCodecPrioritiesL curr.prio: %d",
                codecs[codCount]->MMFPriority() )
            SVPDEBUG2( "SVPAudioUtility::SetAudioCodecPrioritiesL curr.pref: 0x%x",
                codecs[codCount]->MMFPriorityPreference() )
                
            // Priority is always the same for VoIP.
            codecs[ codCount ]->SetMMFPriorityL( KAudioPriorityPhoneCall );
            codecs[ codCount ]->SetMMFPriorityPreferenceL( aAudioPref );
            }
        } 
    }

// ---------------------------------------------------------------------------
// SVPAudioUtility::SetAudioCodecPrioritiesL
// ---------------------------------------------------------------------------
//
void SVPAudioUtility::SetAudioStreamPrioritiesL(
    CMceMediaStream& aMediaStream,
    CMceMediaStream& aBoundStream )
    {
    SVPDEBUG1( "SVPAudioUtility::SetAudioStreamPrioritiesL In" );
    
    if ( KMceAudio != aMediaStream.Type() ||
         KMceAudio != aBoundStream.Type() )
        {
        SVPDEBUG1( "SVPAudioUtility::SetAudioStreamPrioritiesL KErrArgument" );
        SVPDEBUG2( "SVPAudioUtility::SetAudioStreamPrioritiesL media.Type: %u",
            aMediaStream.Type() );
        SVPDEBUG2( "SVPAudioUtility::SetAudioStreamPrioritiesL bound.Type: %u",
            aBoundStream.Type() );
        
        User::Leave( KErrArgument );
        }
    // else stream types are OK, thus we proceed.
    
    // Set the codec MMF priorities here, first get the audiostreams.
    CMceAudioStream& stream = static_cast<CMceAudioStream&>( aMediaStream );
    CMceAudioStream& bound = static_cast<CMceAudioStream&>( aBoundStream );
    
    if ( SVPAudioUtility::IsDownlinkStream( stream ) )
        {
        SVPAudioUtility::SetPriorityCodecValuesL( stream, bound );
        }
    else
        {
        SVPAudioUtility::SetPriorityCodecValuesL( bound, stream );
        }
    
    SVPDEBUG1( "SVPAudioUtility::SetAudioStreamPrioritiesL Out" );
    }

// ---------------------------------------------------------------------------
// SVPAudioUtility::EnableSpeakerSinkL
// ---------------------------------------------------------------------------
//
void SVPAudioUtility::EnableSpeakerSinkL( 
    const RPointerArray< CMceMediaSink >& aSink )
    {
    TInt snkCount( aSink.Count() );
    while ( snkCount )
        {
        snkCount--;
        if ( KMceSpeakerSink == aSink[ snkCount ]->Type() && 
             !aSink[ snkCount ]->IsEnabled() )
            {
            SVPDEBUG1( "SVPAudioUtility::EnableSpeakerSinkL Speaker found" );
            
            aSink[ snkCount ]->EnableL();
            
            SVPDEBUG1( "SVPAudioUtility::EnableSpeakerSinkL Speaker ENABLED" );
            }
        } 
    }
    
// ---------------------------------------------------------------------------
// SVPAudioUtility::DisableSpeakerSinkL
// ---------------------------------------------------------------------------
//
void SVPAudioUtility::DisableSpeakerSinkL( 
    const RPointerArray< CMceMediaSink >& aSink )
    {
    TInt snkCount( aSink.Count() );
    while ( snkCount )
        {
        snkCount--;
        if ( KMceSpeakerSink == aSink[ snkCount ]->Type() &&
             aSink[ snkCount ]->IsEnabled() )
            {
            SVPDEBUG1( "SVPAudioUtility::DisableSpeakerSinkL Speaker found" );
            
            aSink[ snkCount ]->DisableL();
            
            SVPDEBUG1( "SVPAudioUtility::DisableSpeakerSinkL Speaker DISABLED" );
            }
        } 
    }

// ---------------------------------------------------------------------------
// SVPAudioUtility::EnableMicSourceL
// ---------------------------------------------------------------------------
//
void SVPAudioUtility::EnableMicSourceL( CMceMediaSource& aMicSource )
    {            
    if ( KMceMicSource == aMicSource.Type() && !aMicSource.IsEnabled() )
        {
        SVPDEBUG1("SVPAudioUtility::EnableMicSourceL Mic found");
        
        aMicSource.EnableL();
        
        SVPDEBUG1("SVPAudioUtility::EnableMicSourceL Mic ENABLED");
        }
    }

// ---------------------------------------------------------------------------
// SVPAudioUtility::DisableMicSourceL
// ---------------------------------------------------------------------------
//
void SVPAudioUtility::DisableMicSourceL( CMceMediaSource& aMicSource )
    {
    if ( KMceMicSource == aMicSource.Type() && aMicSource.IsEnabled() )
        {
        SVPDEBUG1( "SVPAudioUtility::DisableMicSourceL Mic found" );
        
        aMicSource.DisableL();
        
        SVPDEBUG1( "SVPAudioUtility::DisableMicSourceL Mic DISABLED" );
        }
    }
    
// ---------------------------------------------------------------------------
// SVPAudioUtility::EnableSpeakerSinksL
// ---------------------------------------------------------------------------
//
void SVPAudioUtility::EnableSpeakerSinksL(
    const RPointerArray< CMceMediaStream >& aStreams )
    {
    SVPDEBUG1("SVPAudioUtility::EnableSpeakerSinksL In");
    
    TInt index( aStreams.Count() );
    while( index-- )
        {
        if ( KMceAudio == aStreams[ index ]->Type() )
            {
            EnableSpeakerSinkL( aStreams[ index ]->Sinks() );
            }
        
        if ( KMceAudio == aStreams[ index ]->BoundStreamL().Type() )
            {
            EnableSpeakerSinkL( aStreams[ index ]->BoundStreamL().Sinks() );
            }
        }
    
    SVPDEBUG1("SVPAudioUtility::EnableSpeakerSinksL Out");
    }

// ---------------------------------------------------------------------------
// SVPAudioUtility::DisableMicSourceL
// ---------------------------------------------------------------------------
//
void SVPAudioUtility::DisableMicSourceL( CMceMediaStream& aStream )
    {
    SVPDEBUG1("SVPAudioUtility::DisableMicSourceL CMceMediaStream In");
    
    CMceMediaSource* source = NULL;
    source = aStream.Source();
    if ( source )
        {
        SVPDEBUG1("SVPAudioUtility::DisableMicSourceL aStream");
        
        DisableMicSourceL( *source );
        }
    
    if ( aStream.BoundStream() )
        {
        source = aStream.BoundStreamL().Source();
        if ( source )
            {
            SVPDEBUG1("SVPAudioUtility::DisableMicSourceL BoundStream");
            
            DisableMicSourceL( *source );
            }
        }
        
    SVPDEBUG1("SVPAudioUtility::DisableMicSourceL CMceMediaStream Out");
    }

// ---------------------------------------------------------------------------
// SVPAudioUtility::FindCodec
// ---------------------------------------------------------------------------
//
CMceAudioCodec* SVPAudioUtility::FindCodec( CMceAudioStream& aAudiostream,
    const TDesC8& aCodecname )
    {
    SVPDEBUG1("SVPAudioUtility::FindCodec In");
    
    const RPointerArray<CMceAudioCodec>& codecs = aAudiostream.Codecs();
    TInt codecCount = codecs.Count();
    while ( codecCount )
        {
        codecCount--;
        if ( !codecs[codecCount]->SdpName().CompareF( aCodecname ) )
            {
            SVPDEBUG1("SVPAudioUtility::SVPAudioUtility Codec found");
            
            return codecs[codecCount];
            }
        }
    
    SVPDEBUG1("SVPAudioUtility::FindCodec Out NULL");
    return NULL;
    }

// ---------------------------------------------------------------------------
// SVPAudioUtility::RemoveCodecL
// ---------------------------------------------------------------------------
//
void SVPAudioUtility::RemoveCodecL( CMceAudioStream& aAudiostream,
    const TDesC8& aCodecname )
    {
    SVPDEBUG1("SVPAudioUtility::RemoveCodecL In");
    
    CMceAudioCodec* codec = SVPAudioUtility::FindCodec( aAudiostream,
        aCodecname );
    
    while ( codec )
        {
        SVPDEBUG1("SVPAudioUtility::RemoveCodecL Removing");
        
        aAudiostream.RemoveCodecL( *codec );
        codec = SVPAudioUtility::FindCodec( aAudiostream, aCodecname );
        }
    
    SVPDEBUG1("SVPAudioUtility::RemoveCodecL Out");
    }

// ---------------------------------------------------------------------------
// SVPAudioUtility::MmfPriorityUpdateNeededL
// ---------------------------------------------------------------------------
//
TBool SVPAudioUtility::MmfPriorityUpdateNeededL(
    const RPointerArray<CMceMediaStream>& aStreams )
    {
    SVPDEBUG1( "SVPAudioUtility::MmfPriorityUpdateNeededL In" )
    
    // We need to check if DTMF codec is found and then check uplink stream
    // MMF priorities.
    // 1) If DTMF codec is found and uplink stream codecs have preference
    //    KSVPAudioPrefAudioUplinkOobDtmf no update is needed.
    // 2) If DTMF codec is not found and uplink stream codecs have preference
    //    KSVPAudioPrefAudioUplinkIbDtmf no update is needed.
    // 3) In other cases we need to do MMF preference/priority update.
    
    // In order to do this we need to find uplink audio streams, check whether
    // the stream has DTMF codec and then check which MMF preference the codec
    // codec has. We may have multiple streams, but for simplicity sake if
    // we find just one stream needing priority/preference update, we return
    // ETrue as update need.
    TInt strmCount( aStreams.Count() );
    
    SVPDEBUG2( "SVPAudioUtility::MmfPriorityUpdateNeededL strmCount: %d",
        strmCount )
    
    while ( strmCount )
        {
        SVPDEBUG2( "SVPAudioUtility::MmfPriorityUpdateNeededL tb checked",
            strmCount )
        
        strmCount--;
        
        // Basic assertions. Streams have to be KMceAudio and 'main' stream
        // must have a bound stream.
        __ASSERT_ALWAYS( KMceAudio == aStreams[strmCount]->Type(),
            User::Leave( KErrArgument ) );
        __ASSERT_ALWAYS( aStreams[strmCount]->BoundStream(),
            User::Leave( KErrArgument ) );
        __ASSERT_ALWAYS( KMceAudio == aStreams[strmCount]->BoundStreamL().Type(),
            User::Leave( KErrArgument ) );
        
        // Now that we are clear from the asserts, we can safely do the casts
        // and find the uplink stream for priority/preference checking.
        CMceAudioStream* audioStream =
            static_cast<CMceAudioStream*>( aStreams[strmCount] );
        CMceAudioStream& boundStream =
            static_cast<CMceAudioStream&>( audioStream->BoundStreamL() );
        
        CMceAudioStream* uplinkStream = NULL;
        
        if ( !IsDownlinkStream( *audioStream ) )
            {
            SVPDEBUG1( "SVPAudioUtility::MmfPriorityUpdateNeededL main is uplink" )
            
            uplinkStream = audioStream;
            }
        else if ( !IsDownlinkStream( boundStream ) )
            {
            SVPDEBUG1( "SVPAudioUtility::MmfPriorityUpdateNeededL bound is uplink" )
            
            uplinkStream = &boundStream;
            }
        
        // Next assert handles the missing else branch.
        __ASSERT_ALWAYS( uplinkStream, User::Leave( KErrNotFound ) );
        
        TBool res = MmfPriorityUpdateNeededL( *uplinkStream );
        if ( res )
            {
            SVPDEBUG1( "SVPAudioUtility::MmfPriorityUpdateNeededL ETrue" )
            return ETrue;
            }
        }
    
    SVPDEBUG1( "SVPAudioUtility::MmfPriorityUpdateNeededL EFalse" )
    return EFalse;
    }

// ---------------------------------------------------------------------------
// SVPAudioUtility::MmfPriorityUpdateNeededL
// ---------------------------------------------------------------------------
//
TBool SVPAudioUtility::MmfPriorityUpdateNeededL(
    CMceAudioStream& aUplinkStream )
    {
    SVPDEBUG1( "SVPAudioUtility::MmfPriorityUpdateNeededL (stream) In" )
    
    // Check which uplink preference we need to have based whether DTMF codec
    // is part of the codec list.
    TUint uplinkPreference = 0;
    if ( FindCodec( aUplinkStream, KTELEVENT ) )
        {
        SVPDEBUG1( "SVPAudioUtility::MmfPriorityUpdateNeededL (stream) DTMF OB" )
        
        uplinkPreference = KSVPAudioPrefUplinkOobDtmf;
        }
    else
        {
        SVPDEBUG1( "SVPAudioUtility::MmfPriorityUpdateNeededL (stream) DTMF IB" )
        
        uplinkPreference = KSVPAudioPrefUplinkIbDtmf;
        }
    
    // Given stream must have at least one codec in order to do the priority /
    // preference check. This function is private and called inside, thus this
    // assertion just adds to the checks done in caller.
    const RPointerArray<CMceAudioCodec>& codecs = aUplinkStream.Codecs();
    TInt codecCount = codecs.Count();
    
    SVPDEBUG2( "SVPAudioUtility::MmfPriorityUpdateNeededL (stream) codecCount: %d",
        codecCount )
    
    __ASSERT_ALWAYS( codecCount, User::Leave( KErrArgument ) );
    
    while( codecCount )
        {
        SVPDEBUG2( "SVPAudioUtility::MmfPriorityUpdateNeededL (stream) tb checked: %d",
            codecCount )
        
        codecCount--;
        
        SVPDEBUG2( "SVPAudioUtility::MmfPriorityUpdateNeededL curr.prio: %d",
                codecs[codecCount]->MMFPriority() )
        SVPDEBUG2( "SVPAudioUtility::MmfPriorityUpdateNeededL curr.pref: 0x%x",
                codecs[codecCount]->MMFPriorityPreference() )
        
        // Do not include DTMF codec into the preference check. Note CompareF
        // return value also, 0 means match.
        if ( codecs[codecCount]->SdpName().CompareF( KTELEVENT ) &&
            uplinkPreference != codecs[codecCount]->MMFPriorityPreference() )
            {
            SVPDEBUG1( "SVPAudioUtility::MmfPriorityUpdateNeededL (stream) ETrue" )
            
            return ETrue;
            }
        
        // else preferences match, ne-ext please!!!
        }
    
    SVPDEBUG1( "SVPAudioUtility::MmfPriorityUpdateNeededL (stream) EFalse" )
    return EFalse;
    }

// ---------------------------------------------------------------------------
// SVPAudioUtility::DtmfActionCapableSession
// ---------------------------------------------------------------------------
//
TBool SVPAudioUtility::DtmfActionCapableSession(
    const CSVPSessionBase& aSession )
    {
    if ( aSession.HasHoldController() &&
         ESVPConnected == aSession.HoldController().HoldState() )
        {
        SVPDEBUG1( "SVPAudioUtility::DtmfActionCapableSession ETrue 1" )
        
        return ETrue;
        }
    else if ( !aSession.HasHoldController() )
        {
        SVPDEBUG1( "SVPAudioUtility::DtmfActionCapableSession ETrue 2" )
        
        return ETrue;
        }
    else
        {
        SVPDEBUG1( "SVPAudioUtility::DtmfActionCapableSession EFalse" )
        
        return EFalse;
        }
    }

// ---------------------------------------------------------------------------
// Check DTMF capability of emergency session
// ---------------------------------------------------------------------------
//
TBool SVPAudioUtility::DtmfActionCapableSession(
    const CSVPEmergencySession& aSession )
    {
    if ( aSession.HasHoldController() &&
         ESVPConnected == aSession.HoldController().HoldState() )
        {
        SVPDEBUG1( "SVPAudioUtility::DtmfActionCapableSession ETrue 1" )
        
        return ETrue;
        }
    else if ( !aSession.HasHoldController() )
        {
        SVPDEBUG1( "SVPAudioUtility::DtmfActionCapableSession ETrue 2" )
        
        return ETrue;
        }
    else
        {
        SVPDEBUG1( "SVPAudioUtility::DtmfActionCapableSession EFalse" )
        
        return EFalse;
        }
    }

// ---------------------------------------------------------------------------
// SVPAudioUtility::DtmfActionCapableStream
// ---------------------------------------------------------------------------
//
TBool SVPAudioUtility::DtmfActionCapableStream(
    const CMceMediaStream& aStream )
    {
    // We used to check for source's DtmfAvailable and DtmfActive attributes,
    // but those are IPC calls which are not preferred for DTMF's real-time
    // nature. Thus leaving those out and letting the server return us a
    // error code if those attributes are not correct for DTMF sending
    // actually does the same thing but improves performance.
    // Thus what once was three IPC calls is now just one.
    TBool ret = EFalse;
    CMceMediaSource* source = aStream.Source();
    
#ifdef _DEBUG
    SVPDEBUG2( "SVPAudioUtility::DtmfActionCapableStream type: %d", aStream.Type() )
    SVPDEBUG2( "SVPAudioUtility::DtmfActionCapableStream source: 0x%x", source )
    if ( source )
        {
        SVPDEBUG2( "SVPAudioUtility::DtmfActionCapableStream enabled: %d", source->IsEnabled() )
        }
#endif
    
    if ( KMceAudio == aStream.Type() && source && source->IsEnabled() )
        {
        ret = ETrue;
        }
    
    SVPDEBUG2( "SVPAudioUtility::DtmfActionCapableStream ret: %d", ret )
    
    return ret;
    }

