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
* Description:  Local holded state for hold state machine.
*
*/


#include    <badesca.h>
#include    <mcesession.h>
#include    <mcertpsource.h>
#include    <mcertpsink.h>
#include    <mcemediastream.h>
#include    <mceaudiostream.h>
#include    "svpholdoutstate.h"
#include    "svpholdcontext.h"
#include    "svpholdcontroller.h"
#include    "svpholdobserver.h"
#include    "svpholdattributehandler.h"
#include    "svpholdmediahandler.h"
#include    "svplogger.h"


// ---------------------------------------------------------------------------
// CSVPHoldOutState::CSVPHoldOutState
// ---------------------------------------------------------------------------
CSVPHoldOutState::CSVPHoldOutState()
    {
    }

// ---------------------------------------------------------------------------
// CSVPHoldOutState::NewLC
// ---------------------------------------------------------------------------
CSVPHoldOutState* CSVPHoldOutState::NewLC()
    {
    CSVPHoldOutState* self = new ( ELeave ) CSVPHoldOutState;
    CleanupStack::PushL( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CSVPHoldOutState::~CSVPHoldOutState
// ---------------------------------------------------------------------------
CSVPHoldOutState::~CSVPHoldOutState()
    {
    }

// ---------------------------------------------------------------------------
// CSVPHoldOutState::DoApplyL
// ---------------------------------------------------------------------------
//
void CSVPHoldOutState::DoApplyL( CSVPHoldContext& aContext )
    {
    SVPDEBUG1( "CSVPHoldOutState::DoApply" );
    CMceSession* session = aContext.SessionObject();
    const RPointerArray< CMceMediaStream >& streams = session->Streams();
    TSVPHoldStateIndex nextState = KSVPHoldEstablishingStateIndex;
    
    TInt audioStreamsHandled = 0;
    TInt streamCount = streams.Count();
    SVPDEBUG2( "CSVPHold”utState::DoApply - stream count = %i", streamCount ); 

    for ( TInt i = 0; i < streamCount; i++ )
        {
        CMceMediaStream* mediaStream = streams[ i ];
        TMceMediaType mediaType = mediaStream->Type();
        if ( KMceAudio == mediaType )
            {
            // This media is audio stream. Handling depends on the request
            SVPDEBUG2( "CSVPHoldOutState::DoApply - Hold request is = %i",
                        aContext.HoldRequest() );

            nextState = PerformRequestL( aContext, *mediaStream, *session );
            audioStreamsHandled++;                
            }
        }
    
    if ( 0 == audioStreamsHandled )
        {
        SVPDEBUG1(
        "CSVPHoldOutState::DoApply - No audio streams found!" );
        User::Leave( KErrSVPHoldStateError );
        }
        
    if ( aContext.ForceHoldOccured() )
        {
        nextState = KSVPHoldConnectedStateIndex;
        aContext.SetForceHold( EFalse ); 
        }

    aContext.SetCurrentStateL( aContext, nextState );
    SVPDEBUG1( "CSVPHoldOutState::DoApply - Handled" );    
    }

// ---------------------------------------------------------------------------
// CSVPHoldOutState::PerformRequestL
// ---------------------------------------------------------------------------
//
TSVPHoldStateIndex 
CSVPHoldOutState::PerformRequestL( CSVPHoldContext& aContext,
                                   CMceMediaStream& aMediaStream,
                                   CMceSession& aSession )
    {
    TSVPHoldStateIndex nextState = KSVPHoldEstablishingStateIndex;
    switch ( aContext.HoldRequest() )
        {
        case ESVPLocalResume:
            {
            ResumeSessionLocallyL( aContext, aMediaStream, aSession );
            break;
            }

        case ESVPRemoteDoubleHold:
            {
            RemoteSessionDoubleHoldL( aContext, aMediaStream, aSession );
            break;
            }                    
            
        case ESVPLocalHold:
        case ESVPLocalDoubleHold:
        case ESVPLocalDoubleHoldResume:
        case ESVPRemoteHold:
        case ESVPRemoteResume:
        case ESVPRemoteDoubleHoldResume:
            {
            // Cannot occur in Out state
            SVPDEBUG1( "CSVPHoldOutState::PerformRequestL - StateError" );
            User::Leave( KErrSVPHoldStateError );                    
            }

        default:
            {
            // No state change needed:
            nextState = KSVPHoldOutStateIndex;
            
            // Error in previous request solving
            SVPDEBUG2( "CSVPHoldOutState::PerformRequestL - Error, request %i",
                        aContext.HoldRequest() );
                        
            User::Leave( KErrSVPHoldStateError );
            }               
        }    

    return nextState;
    }
    
// ---------------------------------------------------------------------------
// CSVPHoldOutState::ResumeSessionLocallyL
// ---------------------------------------------------------------------------
//
void CSVPHoldOutState::ResumeSessionLocallyL( CSVPHoldContext& aContext,
                                              CMceMediaStream& aMediaStream,
                                              CMceSession& aSession )
    {
    SVPDEBUG1( "CSVPHoldOutState::ResumeSessionLocallyL - In" );

    // Handle media stream:
    aContext.MediaHandler().
        PerformMediaActionL( aMediaStream,
                             ESVPLocalResume );        		    

    aSession.UpdateL();
    SVPDEBUG1( "CSVPHoldOutState::ResumeSessionLocallyL - done" );    
    }
    
// ---------------------------------------------------------------------------
// CSVPHoldOutState::RemoteSessionDoubleHoldL
// ---------------------------------------------------------------------------
//
void CSVPHoldOutState::RemoteSessionDoubleHoldL( CSVPHoldContext& aContext,
                                                 CMceMediaStream& aMediaStream,
                                                 CMceSession& aSession )
    {
    SVPDEBUG1(
    "CSVPHoldOutState::RemoteSessionDoubleHoldL - In" );
    
    MDesC8Array* attributeLines = aMediaStream.MediaAttributeLinesL();
    CleanupDeletePushL( attributeLines );
    
    // Check that request has reasonable direction attribute:
    TSVPHoldRequestType attribute =
                  aContext.SolveRequestL( aSession,
                                          attributeLines );

    if ( ESVPNoType == attribute )
        {
        // Not hold request
        SVPDEBUG1( "CSVPHoldOutState::RemoteSessionDoubleHoldL:");
        SVPDEBUG1( "Not hold request" );
        
        User::Leave( KErrSVPHoldNotHoldRequest );
        }
    
    CleanupStack::PopAndDestroy( attributeLines );
    
    // Handle media stream:
    aContext.MediaHandler().
        PerformMediaActionL( aMediaStream,
                             ESVPRemoteDoubleHold );
    
    // Update session
    aSession.UpdateL();
    
    SVPDEBUG1( "CSVPHoldOutState::RemoteSessionDoubleHoldL - done" );    
    }
    
// ---------------------------------------------------------------------------
// CSVPHoldOutState::DoEnter
// ---------------------------------------------------------------------------
//
void CSVPHoldOutState::DoEnter( CSVPHoldContext& aContext )
    {
    SVPDEBUG2( "CSVPHoldOutState::DoEnter - Request %i",
                aContext.HoldRequest() );
    
    TInt err = KErrNone;
    TRAP( err, aContext.MediaHandler().DisableAudioL( aContext ) );
    
    // succesfull action is informed to client
    switch ( aContext.HoldRequest() )
        {
        case ESVPLocalHold:
            {
            SVPDEBUG1( 
            "CSVPHoldOutState::DoEnter - LocalHold" );
            if ( err )
                {
                aContext.HoldObserver().HoldRequestFailed();
                }
                
            else
                {
                aContext.HoldObserver().SessionLocallyHeld();
                }
            
            break;
            }
            
        case ESVPRemoteDoubleHoldResume:
            {
            SVPDEBUG1(
            "CSVPHoldOutState::DoEnter - ESVPRemoteDoubleHoldResume" );
            if ( err )
                {
                //aContext.HoldObserver().ResumeRequestFailed();
                }
                
            else
                {
                aContext.HoldObserver().SessionRemoteResumed();
                }

            break;            
            }
            
        default:
            {
            // Nothing to do; occurs only with state rollback
            SVPDEBUG1( "CSVPHoldOutState::DoEnter - Default" );
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CSVPHoldOutState::IsOutEstablishingStateActive
// ---------------------------------------------------------------------------
//
TBool CSVPHoldOutState::IsOutEstablishingStateActive()
    {
    return EFalse;
    }


