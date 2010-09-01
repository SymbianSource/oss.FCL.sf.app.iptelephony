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
* Description:  Pauses and resumes audio stream.
*
*/

#include    <e32cmn.h>

#include    <mcesession.h>
#include    <mcemediastream.h>
#include    <mceaudiostream.h>
#include    <mcemediasource.h>
#include    <mcemicsource.h>
#include    <mcemediasink.h>
#include    <mcespeakersink.h>
#include    <mcertpsource.h>
#include    <mcertpsink.h>

#include    "svpholdcontext.h"
#include    "svpholdmediahandler.h"
#include    "svplogger.h"
#include    "svpaudioutility.h"

// ---------------------------------------------------------------------------
// CSVPHoldMediaHandler::CSVPHoldMediaHandler
// ---------------------------------------------------------------------------
//
CSVPHoldMediaHandler::CSVPHoldMediaHandler()
    {
    }


// ---------------------------------------------------------------------------
// CSVPHoldMediaHandler::NewL
// ---------------------------------------------------------------------------
//
CSVPHoldMediaHandler* CSVPHoldMediaHandler::NewL()
    {
    CSVPHoldMediaHandler* self = new ( ELeave ) CSVPHoldMediaHandler;
    return self;
    }


// ---------------------------------------------------------------------------
// CSVPHoldMediaHandler::~CSVPHoldMediaHandler
// ---------------------------------------------------------------------------
//
CSVPHoldMediaHandler::~CSVPHoldMediaHandler()
    {
    }

// ---------------------------------------------------------------------------
// CSVPHoldMediaHandler::PerformMediaActionL
// ---------------------------------------------------------------------------
//
void CSVPHoldMediaHandler::PerformMediaActionL( CMceMediaStream& aMediaStream,
                                                TSVPHoldRequestType aRequest )
    {
    SVPDEBUG1( "CSVPHoldMediaHandler::PerformMediaActionL - IN" );

    CMceMediaStream& boundStream = aMediaStream.BoundStreamL();
    
    // Always set the priorites because MCE/MCC must have those correctly
    // all the time. This is due to the fact that we may do session update
    // later and then the priority settings need to be correct.
    SVPAudioUtility::SetAudioStreamPrioritiesL( aMediaStream, boundStream );
    
    switch ( aRequest )
        {
        case ESVPLocalHold:
        case ESVPLocalDoubleHold:
            {            
            HandleLocalHoldingL( aMediaStream, boundStream );
            break;
            }
            
        case ESVPLocalResume:
        case ESVPLocalDoubleHoldResume:
            {
            HandleLocalResumingL( aMediaStream, boundStream, aRequest );
            break;
            }
                        
        case ESVPRemoteHold:
        case ESVPRemoteDoubleHold:
            {
            HandleRemoteHoldingL( aMediaStream, boundStream, aRequest );
            break;
            }
        
        case ESVPRemoteResume:
        case ESVPRemoteDoubleHoldResume:
            {
            HandleRemoteResumingL( aMediaStream, boundStream, aRequest );
            break;
            }

        default:
            {
            SVPDEBUG1( "CSVPHoldMediaHandler::PerformMediaActionL: Leave" );
            User::Leave( KErrNotFound );            
            }
        }
    SVPDEBUG1( "CSVPHoldMediaHandler::PerformMediaActionL - Out" );
    }
    
// ---------------------------------------------------------------------------
// CSVPHoldMediaHandler::HandleLocalHoldingL
// ---------------------------------------------------------------------------
//
void CSVPHoldMediaHandler::
HandleLocalHoldingL( CMceMediaStream& aMediaStream,
                     CMceMediaStream& aBoundStream )
    {
    SVPDEBUG1( "CSVPHoldMediaHandler::HandleLocalHoldingL - In" );
    
    // In case of local hold RTP source must be disabled
    if ( aMediaStream.Source() &&
         KMceRTPSource == aMediaStream.Source()->Type() )
        {
        aMediaStream.DisableL();
        }
    else if ( aBoundStream.Source() &&
              KMceRTPSource == aBoundStream.Source()->Type() )
        {
        aBoundStream.DisableL();
        }
    else
        {
        SVPDEBUG1( "CSVPHoldMediaHandler::HandleLocalHoldingL:" );
        SVPDEBUG1( "Local hold: No stream - User::Leave" );
        User::Leave( KErrNotFound );
        }
    
    SVPDEBUG1( "CSVPHoldMediaHandler::HandleLocalHoldingL - Out" );
    }

// ---------------------------------------------------------------------------
// CSVPHoldMediaHandler::HandleLocalResumingL
// ---------------------------------------------------------------------------
//
void CSVPHoldMediaHandler::
HandleLocalResumingL( CMceMediaStream& aMediaStream,
                      CMceMediaStream& aBoundStream,
                      TSVPHoldRequestType aRequest )
    {
    SVPDEBUG1( "CSVPHoldMediaHandler::HandleLocalResumingL - In" );
    
    switch ( aRequest )
        {
        case ESVPLocalResume:
            {
            // In case of local resume disabled stream must be enabled              
            SVPDEBUG1( "CSVPHoldMediaHandler::HandleLocalResumingL - LocalResume" );
            if ( !aBoundStream.IsEnabled() )
                {
                aBoundStream.EnableL();
                }
                
            if ( !aMediaStream.IsEnabled() )
                {
                aMediaStream.EnableL();
                }
            
            break;
            }
            
        case ESVPLocalDoubleHoldResume:
            {
            // In case of localDH resume RTP source must be enabled
            if ( KMceRTPSource == aMediaStream.Source()->Type() )
                {
                aMediaStream.EnableL();
                }
                
            else if ( KMceRTPSource == aBoundStream.Source()->Type() )
                {
                aBoundStream.EnableL();
                }
                
            else
                {
                SVPDEBUG1( "CSVPHoldMediaHandler::HandleLocalResumingL:" );
                SVPDEBUG1( "Local DH resume: No stream - User::Leave" );
                User::Leave( KErrNotFound );
                }            

            break;
            }
            
        default:
            {
            User::Leave( KErrNotFound );
            }
        }
    
    SVPDEBUG1( "CSVPHoldMediaHandler::HandleLocalResumingL - Out" );
    }

// ---------------------------------------------------------------------------
// CSVPHoldMediaHandler::HandleRemoteHoldingL
// ---------------------------------------------------------------------------
//
void CSVPHoldMediaHandler::
HandleRemoteHoldingL( CMceMediaStream& aMediaStream,
                      CMceMediaStream& aBoundStream,
                      TSVPHoldRequestType aRequest )
    {
    SVPDEBUG1( "CSVPHoldMediaHandler::HandleRemoteHoldingL - In" );
    
    // In case of remote hold RTP sink must be disabled    
    if ( KMceRTPSource == aMediaStream.Source()->Type() )
        {
        aBoundStream.DisableL();
        if ( ESVPRemoteDoubleHold == aRequest )
            {
            aMediaStream.DisableL();
            }
        }
        
    else if ( KMceRTPSource == aBoundStream.Source()->Type() )
        {
        aMediaStream.DisableL();
        if ( ESVPRemoteDoubleHold == aRequest )
            {
            aBoundStream.DisableL();
            }
        }
        
    else
        {
        SVPDEBUG1( "CSVPHoldMediaHandler::HandleRemoteHoldingL:" );
        SVPDEBUG1( "Remote hold: No stream - User::Leave" );
        User::Leave( KErrNotFound );
        }    

    SVPDEBUG1( "CSVPHoldMediaHandler::HandleRemoteHoldingL - Out" );
    }

// ---------------------------------------------------------------------------
// CSVPHoldMediaHandler::HandleRemoteResumingL
// ---------------------------------------------------------------------------
//
void CSVPHoldMediaHandler::
HandleRemoteResumingL( CMceMediaStream& aMediaStream,
                       CMceMediaStream& aBoundStream,
                       TSVPHoldRequestType aRequest )
    {
    SVPDEBUG1( "CSVPHoldMediaHandler::HandleRemoteResumingL - In" )
    
    switch ( aRequest )
        {
        case ESVPRemoteResume:
            {
            SVPDEBUG2( "CSVPHoldMediaHandler::HandleRemoteResumingL aBound ena: %d",
                aBoundStream.IsEnabled() )
            
            SVPDEBUG2( "CSVPHoldMediaHandler::HandleRemoteResumingL aMedia ena: %d",
                aMediaStream.IsEnabled() )
            
            // In case of remote resume disabled stream(s) must be enabled
            if ( !aBoundStream.IsEnabled() )
                {
                SVPDEBUG1( "CSVPHoldMediaHandler::HandleRemoteResumingL RR 1st ena/dis" )
                
                aBoundStream.EnableL();
                aMediaStream.DisableL();
                }
                
            if ( !aMediaStream.IsEnabled() )
                {
                SVPDEBUG1( "CSVPHoldMediaHandler::HandleRemoteResumingL RR 2nd ena/dis" )
                
                aMediaStream.EnableL();
                aBoundStream.DisableL();
                }
            
            break;
            }
            
        case ESVPRemoteDoubleHoldResume:
            {
            // In case of remoteDH resume RTP sink must be enabled                    
            if ( KMceRTPSource == aMediaStream.Source()->Type() )
                {
                aBoundStream.EnableL();
                aMediaStream.DisableL();
                }
            
            else if ( KMceRTPSource == aBoundStream.Source()->Type() )
                {
                aMediaStream.EnableL();
                aBoundStream.DisableL();
                }
            
            else
                {
                SVPDEBUG1( "CSVPHoldMediaHandler::HandleRemoteResumingL:" );
                SVPDEBUG1( "Remote DH resume: No stream - User::Leave" );
                User::Leave( KErrNotFound );
                }
            
            break;
            }
            
        default:
            {
            User::Leave( KErrNotFound );
            }
        }
    
    SVPDEBUG1( "CSVPHoldMediaHandler::HandleRemoteResumingL - Out" )
    }

// ---------------------------------------------------------------------------
// CSVPHoldMediaHandler::DisableAudioL
// ---------------------------------------------------------------------------
//
void CSVPHoldMediaHandler::DisableAudioL( CSVPHoldContext& aContext,
                                          TBool aLeaveSpeakerEnabled )
    {
    if ( aContext.AudioHandlingAllowed() )
        {
        // Disable speaker & microphone
        SVPDEBUG1( "CSVPHoldMediaHandler::DisableAudioL - Disabling audio..." );
        CMceSession* session = aContext.SessionObject();
        const RPointerArray< CMceMediaStream >& streams = session->Streams();
        TInt streamCount = streams.Count();
        for ( TInt i = 0; i < streamCount; i++ )
            {
            CMceMediaStream* mediaStream = streams[ i ];
            TMceMediaType mediaType = mediaStream->Type();
            if ( KMceAudio == mediaType )
                {
                TSVPHoldRequestType request = aContext.HoldRequest();
                
                CMceMediaStream& boundStream = mediaStream->BoundStreamL();
                DisableAudioStreamL( boundStream, aLeaveSpeakerEnabled );
                DisableAudioStreamL( *mediaStream, aLeaveSpeakerEnabled );
                SVPDEBUG1( "CSVPHoldMediaHandler::DisableAudioL - Audio disabled" );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CSVPHoldMediaHandler::DisableAudioStreamL
// ---------------------------------------------------------------------------
//
void CSVPHoldMediaHandler::DisableAudioStreamL( CMceMediaStream& aAudioStream,
                                                TBool aLeaveSpeakerEnabled )
    {
    SVPDEBUG1( "CSVPHoldMediaHandler::DisableAudioL - In" );
    
    SVPAudioUtility::DisableMicSourceL( aAudioStream );
    if ( !aLeaveSpeakerEnabled )
        {
        SVPAudioUtility::DisableSpeakerSinkL( aAudioStream.Sinks() );
        }
    
    SVPDEBUG1( "CSVPHoldMediaHandler::DisableAudioL - Out" );
    }

// ---------------------------------------------------------------------------
// CSVPHoldMediaHandler::EnableAudioL
// ---------------------------------------------------------------------------
//
void CSVPHoldMediaHandler::EnableAudioL( CSVPHoldContext& aContext )
    {
    if ( aContext.AudioHandlingAllowed() )
        {
        // Enable speaker & microphone
        SVPDEBUG1( "CSVPHoldMediaHandler::EnableAudioL - Enabling audio..." );
        
        const RPointerArray< CMceMediaStream >& streams =
            aContext.SessionObject()->Streams();
        const TInt streamCount = streams.Count();
        for ( TInt i = 0; i < streamCount; i++ )
            {
            CMceMediaStream* mediaStream = streams[ i ];
            if ( KMceAudio == mediaStream->Type() )
                {
                EnableAudioL( *mediaStream, aContext );
                }
            }
        }
    }


// ---------------------------------------------------------------------------
// CSVPHoldMediaHandler::EnableAudioL (private)
// ---------------------------------------------------------------------------
//
void CSVPHoldMediaHandler::EnableAudioL( CMceMediaStream& aMediaStream,
                                         CSVPHoldContext& aContext )
    {
    SVPDEBUG1( "CSVPHoldMediaHandler::EnableAudioL - Audio enabled In" );

    TBool muted = aContext.Muted();
    TBool mobileOriginated = aContext.IsMobileOriginated();

    // Now enable the audio streams.
    if ( !muted )
        {
        // Normal case, audio not muted
        SVPDEBUG1( "CSVPHoldMediaHandler::EnableAudioL - no mute" );
        EnableAudioStreamL( aMediaStream.BoundStreamL() );
        EnableAudioStreamL( aMediaStream );
        }
        
    else
        {
        // Enabling depends on the direction of muted stream
        if ( mobileOriginated || aContext.IsRemoteRequest() )
            {
            SVPDEBUG1( "CSVPHoldMediaHandler::EnableAudioL - Bound" );
            EnableAudioStreamL( aMediaStream.BoundStreamL(), EFalse );
            EnableAudioStreamL( aMediaStream, EFalse );
            // Make sure that mic is disabled
            SVPAudioUtility::DisableMicSourceL( *( aMediaStream.Source() ) );
            SVPAudioUtility::DisableMicSourceL( *( aMediaStream.BoundStreamL().
                                                                  Source() ) );
            }
        else
            {
            SVPDEBUG1( "CSVPHoldMediaHandler::EnableAudioL - Main" );
            EnableAudioStreamL( aMediaStream, EFalse );
            EnableAudioStreamL( aMediaStream.BoundStreamL(), EFalse );
            // Make sure that mic is disabled
            SVPAudioUtility::DisableMicSourceL( aMediaStream );
            SVPAudioUtility::DisableMicSourceL( aMediaStream.BoundStreamL() );
            }
        }

    SVPDEBUG1( "CSVPHoldMediaHandler::EnableAudioL - Audio enabled Out" );    
    }


// ---------------------------------------------------------------------------
// CSVPHoldMediaHandler::EnableAudioStreamL
// ---------------------------------------------------------------------------
//
void CSVPHoldMediaHandler::EnableAudioStreamL( CMceMediaStream& aAudioStream,
                                               TBool aAllowMicEnable )
    {
    SVPDEBUG1( "CSVPHoldMediaHandler::EnableAudioStreamL - In" );
    

    const RPointerArray< CMceMediaSink >& sinks = aAudioStream.Sinks();
    TInt snkCount( sinks.Count() );

    // Go throug every sink that is associated to that stream
    for ( TInt i = 0; i < snkCount; i++ )
        {
        SVPDEBUG2( "CSVPHoldMediaHandler::EnableAudioStreamL -  Check sink # %d", i );
        if ( !sinks[ i ]->IsEnabled() )
            {
            SVPDEBUG1( "CSVPHoldMediaHandler::EnableAudioStreamL - disabled" )
            sinks[ i ]->EnableL();            
            SVPDEBUG1( "CSVPHoldMediaHandler::EnableAudioStreamL - ENABLED" )
            }
        }

    if ( aAudioStream.Source() &&
         ( KMceMicSource == aAudioStream.Source()->Type() ||
           KMceRTPSource == aAudioStream.Source()->Type() ) )
        {
        CMceMediaSource* source = aAudioStream.Source();
        if ( !source->IsEnabled() && aAllowMicEnable )
            {
            // Enable source
            source->EnableL();
            SVPDEBUG1( "CSVPHoldMediaHandler::EnableAudioStreamL - Source" );
            }
        }
    
    SVPDEBUG1( "CSVPHoldMediaHandler::EnableAudioStreamL - Out" );
    }


