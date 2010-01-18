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
* Description:  Connected state class for hold state machine.
*
*/


#include    <badesca.h>
#include    <mcesession.h>
#include    <mcemediastream.h>
#include    <mcertpsource.h>
#include    <mcertpsink.h>
#include    <mceaudiostream.h>
#include    <mcemediasource.h>
#include    <mcemediasink.h>

#include    "svpholdconnectedstate.h"
#include    "svpholdcontroller.h"
#include    "svpholdattributehandler.h"
#include    "svpholdobserver.h"
#include    "svpholdmediahandler.h"
#include    "svplogger.h"


// ---------------------------------------------------------------------------
// CSVPHoldConnectedState::CSVPHoldConnectedState
// ---------------------------------------------------------------------------
CSVPHoldConnectedState::CSVPHoldConnectedState()
    {
    }

// ---------------------------------------------------------------------------
// CSVPHoldConnectedState::NewLC
// ---------------------------------------------------------------------------
CSVPHoldConnectedState* CSVPHoldConnectedState::NewLC()
    {
    CSVPHoldConnectedState* self = new ( ELeave ) CSVPHoldConnectedState;
    CleanupStack::PushL( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CSVPHoldConnectedState::~CSVPHoldConnectedState
// ---------------------------------------------------------------------------
CSVPHoldConnectedState::~CSVPHoldConnectedState()
    {
    }

// ---------------------------------------------------------------------------
// CSVPHoldConnectedState::DoApplyL
// ---------------------------------------------------------------------------
//
void CSVPHoldConnectedState::DoApplyL( CSVPHoldContext& aContext )
    {
    SVPDEBUG1( "CSVPHoldConnectedState::DoApply" );
    
    CMceSession* session = aContext.SessionObject();
    CMceSession::TState sessionState = session->State();
    SVPDEBUG2( "CSVPHoldConnectedState::DoApply - MCE Session state is = %i",
                sessionState );

    const RPointerArray< CMceMediaStream >& streams = session->Streams();
    TSVPHoldStateIndex nextState = KSVPHoldEstablishingStateIndex;
    
    TInt audioStreamsHandled = 0;
    TInt streamCount = streams.Count();
    SVPDEBUG2( "CSVPHoldConnectedState::DoApply - stream count = %i",
                streamCount ); 
    
    for ( TInt i = 0; i < streamCount; i++ )
        {
        CMceMediaStream* mediaStream = streams[ i ];
        TMceMediaType mediaType = mediaStream->Type();
        if ( KMceAudio == mediaType )
            {
            // This media is audio stream. Handling depends on the request
            SVPDEBUG2( "CSVPHoldConnectedState::DoApply - Hold request is = %i",
                        aContext.HoldRequest() );

            nextState = PerformRequestL( aContext, *mediaStream, *session );            
            audioStreamsHandled++;
            }
        }
    
    if ( 0 == audioStreamsHandled )
        {
        SVPDEBUG1( "CSVPHoldConnectedState::DoApply - No streams - Leave" );
        User::Leave( KErrSVPHoldStateError );
        }    

    aContext.SetCurrentStateL( aContext, nextState );
    SVPDEBUG1( "CSVPHoldConnectedState::DoApply - Handled" );
    }
    
// ---------------------------------------------------------------------------
// CSVPHoldConnectedState::PerformRequestL
// ---------------------------------------------------------------------------
//
TSVPHoldStateIndex 
CSVPHoldConnectedState::PerformRequestL( CSVPHoldContext& aContext,
                                         CMceMediaStream& aMediaStream,
                                         CMceSession& aSession )
    {
    TSVPHoldStateIndex nextState = KSVPHoldEstablishingStateIndex;
    switch ( aContext.HoldRequest() )
        {
        case ESVPLocalHold:
            {
            HoldSessionLocallyL( aContext, aMediaStream, aSession );
            break;
            }

        case ESVPRemoteHold:
            {
            RemoteSessionHoldL( aContext, aMediaStream, aSession );
            break;
            }                    
            
        case ESVPLocalResume:
        case ESVPLocalDoubleHold:
        case ESVPLocalDoubleHoldResume:
        case ESVPRemoteResume:
        case ESVPRemoteDoubleHold:
        case ESVPRemoteDoubleHoldResume:
            {
            // Cannot occur in connected state
            SVPDEBUG1( "CSVPHoldConnectedState::PerformRequestL - StateError" );
            User::Leave( KErrSVPHoldStateError );                    
            }

        default:
            {
            // Error in request solving, no state change needed:
            nextState = KSVPHoldConnectedStateIndex;
            
            SVPDEBUG2( "CSVPHoldConnectedState::PerformRequestL - Error, request %i", 
                        aContext.HoldRequest() );
            break;
            }
        }
        
    return nextState;
    }
    
// ---------------------------------------------------------------------------
// CSVPHoldConnectedState::HoldSessionLocallyL
// ---------------------------------------------------------------------------
//
void CSVPHoldConnectedState::
HoldSessionLocallyL( CSVPHoldContext& aContext,
                     CMceMediaStream& aMediaStream,
                     CMceSession& aSession )
    {
    SVPDEBUG1( "CSVPHoldConnectedState::HoldSessionLocallyL - IN" );
    
    // Handle media stream:
    aContext.MediaHandler().
        PerformMediaActionL( aMediaStream,
                             ESVPLocalHold );
        
    // Disable all sources and sinks:
    aContext.MediaHandler().DisableAudioL( aContext );
                            
    aSession.UpdateL();
    SVPDEBUG1( "CSVPHoldConnectedState::HoldSessionLocallyL done" );    
    }

// ---------------------------------------------------------------------------
// CSVPHoldConnectedState::RemoteSessionHoldL
// ---------------------------------------------------------------------------
//
void CSVPHoldConnectedState::
RemoteSessionHoldL( CSVPHoldContext& aContext,
                      CMceMediaStream& aMediaStream,
                      CMceSession& aSession )
    {
    SVPDEBUG1( "CSVPHoldConnectedState::RemoteSessionHoldL - In" );
    
    MDesC8Array* sessionAttributeLines = aSession.SessionSDPLinesL();
    CleanupDeletePushL( sessionAttributeLines );
    TSVPHoldRequestType attribute =
                      aContext.SolveRequestL( aSession,
                                              sessionAttributeLines ); 
    
    CleanupStack::PopAndDestroy( sessionAttributeLines );
    
    MDesC8Array* attributeLines = aMediaStream.MediaAttributeLinesL();
    CleanupDeletePushL( attributeLines );

    // Check that request has reasonable direction attribute:
    if ( ESVPNoType == attribute )
        {
        attribute = aContext.SolveRequestL( aSession,
                                            attributeLines,
                                            ETrue);        
        }
        
    if ( ESVPNoType == attribute )
        {
        // Not hold request
        SVPDEBUG1( "CSVPHoldConnectedState::RemoteSessionHoldL:");
        SVPDEBUG1( "Not hold request" );
        
        User::Leave( KErrSVPHoldNotHoldRequest );
        }
    
    CleanupStack::PopAndDestroy( attributeLines );
    
    // Handle media stream:
    aContext.MediaHandler().
        PerformMediaActionL( aMediaStream,
                             ESVPRemoteHold );
    
    // Update session
    aSession.UpdateL();
    
    SVPDEBUG1( "CSVPHoldConnectedState::RemoteSessionHoldL - done" );
    }

// ---------------------------------------------------------------------------
// CSVPHoldConnectedState::DoEnter
// ---------------------------------------------------------------------------
//
void CSVPHoldConnectedState::DoEnter( CSVPHoldContext& aContext )
    {
    SVPDEBUG1( "CSVPHoldConnectedState::DoEnter" );

    TInt err = KErrNone;
    TRAP( err, aContext.MediaHandler().EnableAudioL( aContext ) );

    // succesfull action is informed to client
    switch ( aContext.HoldRequest() )
        {
        case ESVPRemoteResume:
            {
            SVPDEBUG1( "CSVPHoldConnectedState::DoEnter - RemoteResume" );
            aContext.HoldObserver().SessionRemoteResumed();
            break;
            }
            
        case ESVPLocalResume:
            {
            SVPDEBUG1( "CSVPHoldConnectedState::DoEnter - LocalResume" );
            aContext.HoldObserver().SessionLocallyResumed();
            break;
            }
            
        default:
            {
            // Nothing to do; occurs only with state rollback
            SVPDEBUG1( "CSVPHoldConnectedState::DoEnter - Default" );
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CSVPHoldConnectedState::IsOutEstablishingStateActive
// ---------------------------------------------------------------------------
//
TBool CSVPHoldConnectedState::IsOutEstablishingStateActive()
    {
    return EFalse;
    }

