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
* Description:  Remote holded state for hold state machine.
*
*/


#include    <badesca.h>
#include    <mcesession.h>
#include    <mcertpsource.h>
#include    <mcertpsink.h>
#include    <mcemediastream.h>
#include    <mceaudiostream.h>
#include    "svpholdinstate.h" 
#include    "svpholdcontext.h"
#include    "svpholdcontroller.h"
#include    "svpholdobserver.h"
#include    "svpholdmediahandler.h"
#include    "svplogger.h"


// ---------------------------------------------------------------------------
// CSVPHoldInState::CSVPHoldInState
// ---------------------------------------------------------------------------
//
CSVPHoldInState::CSVPHoldInState()
    {
    }

// ---------------------------------------------------------------------------
// CSVPHoldInState::NewLC
// ---------------------------------------------------------------------------
//
CSVPHoldInState* CSVPHoldInState::NewLC()
    {
    CSVPHoldInState* self = new ( ELeave ) CSVPHoldInState;
    CleanupStack::PushL( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CSVPHoldInState::~CSVPHoldInState
// ---------------------------------------------------------------------------
//
CSVPHoldInState::~CSVPHoldInState()
    {
    }

// ---------------------------------------------------------------------------
// CSVPHoldInState::DoApplyL
// ---------------------------------------------------------------------------
//
void CSVPHoldInState::DoApplyL( CSVPHoldContext& aContext )
    {
    SVPDEBUG1( "CSVPHoldInState::DoApply" )
    
    CMceSession* session = aContext.SessionObject();
    const RPointerArray< CMceMediaStream >& streams = session->Streams();
    TSVPHoldStateIndex nextState = KSVPHoldEstablishingStateIndex;
    
    SVPDEBUG2( "CSVPHoldInState::DoApply streams: %d", streams.Count() )
    
    TInt audioStreamsHandled = 0;
    TInt streamCount = streams.Count();
    while ( streamCount )
        {
        streamCount--;
        CMceMediaStream* mediaStream = streams[ streamCount ];
        if ( KMceAudio == mediaStream->Type() )
            {
            // This media is audio stream. Handling depends on the request            
            SVPDEBUG2( "CSVPHoldInState::DoApply - Hold request = %i",
                        aContext.HoldRequest() )
            
            nextState = PerformRequestL( aContext, *mediaStream, *session );
            audioStreamsHandled++;
            }
        }
    
    if ( 0 == audioStreamsHandled )
        {
        SVPDEBUG1( "CSVPHoldInState::DoApply KErrSVPHoldStateError" )
        
        User::Leave( KErrSVPHoldStateError );
        }    
    
    aContext.SetCurrentStateL( aContext, nextState );
    
    SVPDEBUG1( "CSVPHoldInState::DoApply - Handled" )
    }

// ---------------------------------------------------------------------------
// CSVPHoldInState::PerformRequestL
// ---------------------------------------------------------------------------
//
TSVPHoldStateIndex 
CSVPHoldInState::PerformRequestL( CSVPHoldContext& aContext,
                                  CMceMediaStream& aMediaStream,
                                  CMceSession& aSession )
    {
    TSVPHoldStateIndex nextState = KSVPHoldEstablishingStateIndex;
    switch ( aContext.HoldRequest() )
        {
        case ESVPLocalDoubleHold:
            {
            LocalSessionDoubleHoldL( aContext, aMediaStream, aSession );
            break;
            }                    
            
        case ESVPRemoteResume:
            {
            RemoteSessionResumeL( aContext, aMediaStream, aSession );
            break;
            }                   

        case ESVPLocalHold:
        case ESVPLocalDoubleHoldResume:
        case ESVPLocalResume:
        case ESVPRemoteHold:
        case ESVPRemoteDoubleHold:
        case ESVPRemoteDoubleHoldResume:
            {
            // Cannot occur in In state
            User::Leave( KErrSVPHoldStateError );
            break;
            }

        default:
            {
            // Error in request solving, state change not needed:
            nextState = KSVPHoldInStateIndex;
            
            SVPDEBUG2( "CSVPHoldInState::PerformRequestL - Error, request %i", 
                        aContext.HoldRequest() );
            break;
            }                    
        }
    
    return nextState;
    }
    
// ---------------------------------------------------------------------------
// CSVPHoldInState::LocalSessionDoubleHoldL
// ---------------------------------------------------------------------------
//
void CSVPHoldInState::LocalSessionDoubleHoldL( CSVPHoldContext& aContext,
                                               CMceMediaStream& aMediaStream,
                                               CMceSession& aSession )
    {
    SVPDEBUG1( "CSVPHoldInState::LocalSessionDoubleHoldL - In" );
    
    // Handle media stream:
    aContext.MediaHandler().
        PerformMediaActionL( aMediaStream,
                             ESVPLocalDoubleHold );
    
    // Update session
    aSession.UpdateL();
    SVPDEBUG1( "CSVPHoldInState::LocalSessionDoubleHoldL - done" );   
    }
    
// ---------------------------------------------------------------------------
// CSVPHoldInState::RemoteSessionResumeL
// ---------------------------------------------------------------------------
//
void CSVPHoldInState::RemoteSessionResumeL( CSVPHoldContext& aContext,
                                            CMceMediaStream& aMediaStream,
                                            CMceSession& aSession )
    {
    SVPDEBUG1( "CSVPHoldInState::RemoteSessionResumeL - In" )
    
    MDesC8Array* attributeLines = aMediaStream.MediaAttributeLinesL();
    CleanupDeletePushL( attributeLines );
    
    // Check that request has reasonable direction attribute:
    if ( ESVPNoType == aContext.SolveRequestL( aSession, attributeLines ) )
        {
        // Not hold request
        SVPDEBUG1( "CSVPHoldInState::RemoteSessionResumeL ESVPNoType")
        
        User::Leave( KErrSVPHoldNotHoldRequest );
        }
    
    CleanupStack::PopAndDestroy( attributeLines );
    
    // Handle media stream:
    aContext.MediaHandler().PerformMediaActionL(
        aMediaStream, ESVPRemoteResume );
    
    // Update session
    aSession.UpdateL();
    
    SVPDEBUG1( "CSVPHoldInState::RemoteSessionResumeL - done" )
    }
    
// ---------------------------------------------------------------------------
// CSVPHoldInState::DoEnter
// ---------------------------------------------------------------------------
//
void CSVPHoldInState::DoEnter( CSVPHoldContext& aContext )
    {
    SVPDEBUG1( "CSVPHoldInState::DoEnter" );

    TInt err = KErrNone;
    
    // succesfull action is informed to client
    switch ( aContext.HoldRequest() )
        {
        case ESVPRemoteHold:
            {
            SVPDEBUG1( "CSVPHoldInState::DoEnter - RemoteHold" );
            // For music on hold IOP with some vendors leave speaker enabled:
            TBool leaveSpeakerEnabled = ETrue;
            TRAP( err, aContext.MediaHandler().
                            DisableAudioL( aContext,
                                           leaveSpeakerEnabled ) );

            aContext.HoldObserver().SessionRemoteHeld();
            break;
            }
            
        case ESVPLocalDoubleHoldResume:
            {
            SVPDEBUG1( "CSVPHoldInState::DoEnter - Local DH resume" );
            TRAP( err, aContext.MediaHandler().DisableAudioL( aContext ) );
            aContext.HoldObserver().SessionLocallyResumed();
            break;
            }
            
        default:
            {
            // Nothing to do; occurs only with state rollback
            SVPDEBUG1( "CSVPHoldInState::DoEnter - Default" );
            break;
            }        
        }

    if ( aContext.RemoteRequestNotProcessed() )
        {
        SVPDEBUG1( "CSVPHoldInState::DoEnter - Remote request completed" );
        aContext.SetRemoteRequestNotProcessed( EFalse );
        }
    }

// ---------------------------------------------------------------------------
// CSVPHoldInState::IsOutEstablishingStateActive
// ---------------------------------------------------------------------------
//
TBool CSVPHoldInState::IsOutEstablishingStateActive()
    {
    return EFalse;
    }
    

