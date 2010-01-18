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
* Description:  Double hold state for hold state machine.
*
*/


#include    <badesca.h>
#include    <mcesession.h>
#include    <mcertpsource.h>
#include    <mcertpsink.h>
#include    <mcemediastream.h>
#include    <mceaudiostream.h>
#include    "svpholddhstate.h"
#include    "svpholdcontroller.h"
#include    "svpholdobserver.h"
#include    "svpholdmediahandler.h"
#include    "svplogger.h"


// ---------------------------------------------------------------------------
// CSVPHoldDHState::CSVPHoldDHState
// ---------------------------------------------------------------------------
//
CSVPHoldDHState::CSVPHoldDHState()
    {
    }

// ---------------------------------------------------------------------------
// CSVPHoldDHState::NewL
// ---------------------------------------------------------------------------
//
CSVPHoldDHState* CSVPHoldDHState::NewLC()
    {
    CSVPHoldDHState* self = new ( ELeave ) CSVPHoldDHState;
    CleanupStack::PushL( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CSVPHoldDHState::~CSVPHoldDHState
// ---------------------------------------------------------------------------
//
CSVPHoldDHState::~CSVPHoldDHState()
    {
    }

// ---------------------------------------------------------------------------
// CSVPHoldDHState::DoApplyL
// ---------------------------------------------------------------------------
//
void CSVPHoldDHState::DoApplyL( CSVPHoldContext& aContext )
    {
    SVPDEBUG1( "CSVPHoldDHState::DoApply" );
    CMceSession* session = aContext.SessionObject();
    const RPointerArray< CMceMediaStream >& streams = session->Streams();
    TSVPHoldStateIndex nextState = KSVPHoldEstablishingStateIndex;
    
    TInt audioStreamsHandled = 0;
    TInt streamCount = streams.Count();    
    for ( TInt i = 0; i < streamCount; i++ )
        {
        CMceMediaStream* mediaStream = streams[ i ];
        TMceMediaType mediaType = mediaStream->Type();
        if ( KMceAudio == mediaType )
            {
            // This media is audio stream. Handling depends on the request
            nextState = PerformRequestL( aContext, *mediaStream, *session );
            audioStreamsHandled++;                
            }
        }
    
    if ( 0 == audioStreamsHandled )
        {
        SVPDEBUG1( "CSVPHoldDHState::DoApply - No streams - Leave" );
        User::Leave( KErrSVPHoldStateError );
        }    

    aContext.SetCurrentStateL( aContext, nextState );
    SVPDEBUG1( "CSVPHoldDHState::DoApply - Handled" );
    }

// ---------------------------------------------------------------------------
// CSVPHoldDHState::PerformRequestL
// ---------------------------------------------------------------------------
//
TSVPHoldStateIndex 
CSVPHoldDHState::PerformRequestL( CSVPHoldContext& aContext,
                                  CMceMediaStream& aMediaStream,
                                  CMceSession& aSession )
    {
    TSVPHoldStateIndex nextState = KSVPHoldEstablishingStateIndex;
    switch ( aContext.HoldRequest() )
        {
        case ESVPLocalDoubleHoldResume:
            {
            ResumeDoubleHoldSessionLocallyL( aContext, aMediaStream, aSession );
            break;
            }                    
            
        case ESVPRemoteDoubleHoldResume:
            {
            RemoteDoubleHoldSessionResumeL( aContext, aMediaStream, aSession );
            break;
            }

        case ESVPLocalHold:
        case ESVPLocalResume:
        case ESVPLocalDoubleHold:
        case ESVPRemoteHold:
        case ESVPRemoteResume:
        case ESVPRemoteDoubleHold:
            {
            // Cannot occur in doublehold state
            User::Leave( KErrSVPHoldStateError );
            break;                
            }

        default:
            {
            // Error in request solving,no state change needed:
            nextState = KSVPHoldDHStateIndex;
            
            SVPDEBUG2( "CSVPHoldDHState::PerformRequestL - Error, request %i", 
                        aContext.HoldRequest() );
            break;
            }                    
        }    

    return nextState;
    }
    
// ---------------------------------------------------------------------------
// CSVPHoldDHState::ResumeDoubleHoldSessionLocallyL
// ---------------------------------------------------------------------------
//
void CSVPHoldDHState::
ResumeDoubleHoldSessionLocallyL( CSVPHoldContext& aContext,
                                 CMceMediaStream& aMediaStream,
                                 CMceSession& aSession )
    {
    SVPDEBUG1( "CSVPHoldDHState::ResumeDoubleHoldSessionLocallyL - In" );

    // Local resume from double holded stream. Handle media stream:
    aContext.MediaHandler().
        PerformMediaActionL( aMediaStream,
                             ESVPLocalDoubleHoldResume );        		    
    
    // Update session
    aSession.UpdateL();
    SVPDEBUG1( "CSVPHoldDHState::ResumeDoubleHoldSessionLocallyL - done" );    
    }
    
// ---------------------------------------------------------------------------
// CSVPHoldDHState::RemoteSoubleHoldSessionResumeL
// ---------------------------------------------------------------------------
//
void CSVPHoldDHState::
RemoteDoubleHoldSessionResumeL( CSVPHoldContext& aContext,
                                CMceMediaStream& aMediaStream,
                                CMceSession& aSession )
    {
    // Remote Resume from double holded stream:
    SVPDEBUG1( "CSVPHoldDHState::RemoteDoubleHoldSessionResumeL - In" );
    
    MDesC8Array* attributeLines = aMediaStream.MediaAttributeLinesL();
    CleanupDeletePushL( attributeLines );
    
    // Check that request has reasonable direction attribute:
    TSVPHoldRequestType attribute =
                aContext.SolveRequestL( aSession, attributeLines );

    if ( ESVPNoType == attribute )
        {
        // Not hold request
        SVPDEBUG1( "CSVPHoldDHState::RemoteSoubleHoldSessionResumeL:");
        SVPDEBUG1( "Not hold request" );
        
        User::Leave( KErrSVPHoldNotHoldRequest );
        }
    
    CleanupStack::PopAndDestroy( attributeLines );
    
    // Handle media stream:
    aContext.MediaHandler().
        PerformMediaActionL( aMediaStream,
                             ESVPRemoteDoubleHoldResume );        		    
    
    // Update session
    aSession.UpdateL();
    
    SVPDEBUG1( "CSVPHoldDHState::RemoteSoubleHoldSessionResumeL - done" );    
    }
    
// ---------------------------------------------------------------------------
// CSVPHoldDHState::DoEnter
// ---------------------------------------------------------------------------
//
void CSVPHoldDHState::DoEnter( CSVPHoldContext& aContext )
    {
    SVPDEBUG1( "CSVPHoldDHState::DoEnter" );
    
    TInt err = KErrNone;
    TRAP( err, aContext.MediaHandler().DisableAudioL( aContext ) );

    // succesfull action is informed to client
    switch ( aContext.HoldRequest() )
        {
        case ESVPLocalDoubleHold:
            {
            SVPDEBUG1( "CSVPHoldDHState::DoEnter - LocalDoubleHold" );
            aContext.HoldObserver().SessionLocallyHeld();
            break;
            }
            
        case ESVPRemoteDoubleHold:
            {
            SVPDEBUG1( "CSVPHoldDHState::DoEnter - RemoteDoubleHold" );
            aContext.HoldObserver().SessionRemoteHeld();
            break;
            }
            
        default:
            {
            // Nothing to do; occurs only with state rollback
            SVPDEBUG1( "CSVPHoldDHState::DoEnter - Default" );
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CSVPHoldDHState::IsOutEstablishingStateActive
// ---------------------------------------------------------------------------
//
TBool CSVPHoldDHState::IsOutEstablishingStateActive()
    {
    return EFalse;
    }

