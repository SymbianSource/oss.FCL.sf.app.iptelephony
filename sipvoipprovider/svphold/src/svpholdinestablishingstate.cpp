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
* Description:  Incoming request establishing state for hold state machine.
*
*/
 

#include    "mcesession.h"
#include    "mcemediastream.h"
#include    "mceaudiostream.h"
#include    "svpholdinestablishingstate.h"
#include    "svpholdcontroller.h"
#include    "svpholdattributehandler.h"
#include    "svplogger.h"


// ---------------------------------------------------------------------------
// CSVPHoldInEstablishingState::CSVPHoldInEstablishingState
// ---------------------------------------------------------------------------
//
CSVPHoldInEstablishingState::CSVPHoldInEstablishingState()
    {
    }

// ---------------------------------------------------------------------------
// CSVPHoldInEstablishingState::NewL
// ---------------------------------------------------------------------------
//
CSVPHoldInEstablishingState* CSVPHoldInEstablishingState::NewLC()
    {
    CSVPHoldInEstablishingState* self = 
        new ( ELeave ) CSVPHoldInEstablishingState;

    CleanupStack::PushL( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CSVPHoldInEstablishingState::~CSVPHoldInEstablishingState
// ---------------------------------------------------------------------------
//
CSVPHoldInEstablishingState::~CSVPHoldInEstablishingState()
    {
    }

// ---------------------------------------------------------------------------
// CSVPHoldInEstablishingState::DoApplyL
// ---------------------------------------------------------------------------
//
void CSVPHoldInEstablishingState::DoApplyL( CSVPHoldContext& aContext )
    {
    SVPDEBUG1( "CSVPHoldInEstablishingState::DoApply" );

    TSVPHoldStateIndex nextState = KSVPHoldEstablishingStateIndex; 
    
    const RPointerArray< CMceMediaStream >& streams =
        aContext.SessionObject()->Streams();
    
    const TInt streamCount = streams.Count();
    
    if ( !aContext.SpecialResponseHandling( nextState ) )
        {
        for ( TInt i = 0; i < streamCount; i++ )
            {
            CMceMediaStream* mediaStream = streams[ i ];
            if ( KMceAudio == mediaStream->Type() )
                {
                // This media is audio stream. Handling depends on the request
                switch ( aContext.HoldRequest() )
                    {
                    case ESVPRemoteHold:
                        {
                        // From Connected state
                        HandleRemoteHolding( nextState );
                        break;
                        }
                        
                    case ESVPRemoteResume:
                        {
                        // From In state
                        HandleRemoteResuming( nextState );
                        break;
                        }

                    case ESVPRemoteDoubleHold:
                        {
                        // From Out state
                        HandleRemoteHolding( nextState,
                                             KSVPHoldDHStateIndex );
                        break;
                        }

                     case ESVPRemoteDoubleHoldResume:
                        {
                        // From Doublehold state
                        HandleRemoteResuming( nextState,
                                              KSVPHoldOutStateIndex );
                        break;
                        }

                    case ESVPLocalHold:
                    case ESVPLocalResume:
                    case ESVPLocalDoubleHold:
                    case ESVPLocalDoubleHoldResume:
                        {
                        SVPDEBUG1(
                        "CSVPHoldInEstablishingState::DoApply\
                         - Local action - State Error" );
                        User::Leave( KErrSVPHoldStateError );
                        break;
                        }
                        
                    default:
                        {
                        // Error in request solving
                        SVPDEBUG2(
                        "CSVPHoldInEstablishingState::DoApply - Error, request %i",
                                                        aContext.HoldRequest() );
                        break;
                        }
                    }
                }
            }           
        }

    aContext.SetCurrentStateL( aContext, nextState );
    SVPDEBUG1( "CSVPHoldInEstablishingState::DoApply - Handled" );
    }
    
// ---------------------------------------------------------------------------
// CSVPHoldInEstablishingState::DoEnter
// ---------------------------------------------------------------------------
//
void CSVPHoldInEstablishingState::DoEnter( CSVPHoldContext& /*aContext*/ )
    {
    // Nothing to do.
    SVPDEBUG1( "CSVPHoldInEstablishingState::DoEnter" );
    }
    
// ---------------------------------------------------------------------------
// CSVPHoldInEstablishingState::IsOutEstablishingStateActive
// ---------------------------------------------------------------------------
//
TBool CSVPHoldInEstablishingState::IsOutEstablishingStateActive()
    {
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CSVPHoldInEstablishingState::HandleRemoteHoldingL
// ---------------------------------------------------------------------------
//
void CSVPHoldInEstablishingState::
HandleRemoteHolding( TSVPHoldStateIndex& aNextState,
                     TSVPHoldStateIndex aState )
    {
    SVPDEBUG1(
    "CSVPHoldInEstablishingState::HandleRemoteHoldingL" );
    
    if ( KSVPHoldDHStateIndex == aState )
        {
        SVPDEBUG1(
        "CSVPHoldOutEstablishingState::HandleLocalHoldingL\
         - Next KSVPHoldDHStateIndex" );
        aNextState = KSVPHoldDHStateIndex;                
        }
    else
        {
        aNextState = KSVPHoldInStateIndex;
        }
            
    SVPDEBUG1(
    "CSVPHoldInEstablishingState::HandleRemoteHoldingL - Done" );
    }

// ---------------------------------------------------------------------------
// CSVPHoldInEstablishingState::HandleRemoteResumingL
// ---------------------------------------------------------------------------
//
void CSVPHoldInEstablishingState::
HandleRemoteResuming( TSVPHoldStateIndex& aNextState,
                      TSVPHoldStateIndex aState )
    {
    if ( KSVPHoldOutStateIndex == aState )
        {
        SVPDEBUG1(
        "CSVPHoldInEstablishingState::HandleRemoteResumingL KSVPHoldOutStateIndex" )
        
        aNextState = KSVPHoldOutStateIndex;
        }
    else
        {
        SVPDEBUG1(
        "CSVPHoldInEstablishingState::HandleRemoteResumingL KSVPHoldConnectedStateIndex" )
        
        aNextState = KSVPHoldConnectedStateIndex;
        }
    }
    
// ---------------------------------------------------------------------------
// CSVPHoldInEstablishingState::CheckAttribute
// ---------------------------------------------------------------------------
//
TInt CSVPHoldInEstablishingState::
CheckAttribute( MDesC8Array* aAttributeLines,
                KSVPHoldAttributeIndex aNeededAttribute,
                TSVPHoldStateIndex& aNextState,
                CSVPHoldContext& aContext )
    {
    // Checks direction attribute from request whether it is acceptable or not.
    SVPDEBUG1( "CSVPHoldInEstablishingState::CheckAttribute" );

    KSVPHoldAttributeIndex requestAttribute =
        aContext.AttributeHandler().FindDirectionAttribute( aAttributeLines );
        
    if ( aNeededAttribute == requestAttribute )
        {
        return DefineDefaultCaseStateChange( aContext, aNextState );
        }
        
    else if ( KSVPHoldSendrecvIndex == aNeededAttribute &&
              KErrNotFound == requestAttribute )
        {
        // If there is no direction attribute at all, default is sendrecv
        SVPDEBUG1(
        "CSVPHoldInEstablishingState::CheckAttributeL - No attribute" );
        aNextState = KSVPHoldConnectedStateIndex;
        return KErrNone;
        }
        
    else if ( KSVPHoldSendonlyIndex == aNeededAttribute &&
              KErrNotFound == requestAttribute )
        {
        // no-op request received
        SVPDEBUG1(
        "CSVPHoldInEstablishingState::CheckAttributeL - No-op" );
        aNextState = KSVPHoldConnectedStateIndex;
        return KErrNone;
        }
        
    else
        {
        // Do not change the aNextState
        SVPDEBUG1(
        "CSVPHoldInEstablishingState::CheckAttributeL - else" );
        return KErrSVPHoldUnacceptableResponseAttribute;
        }
    }

// ---------------------------------------------------------------------------
// CSVPHoldInEstablishingState::DefineDefaultCaseStateChange
// ---------------------------------------------------------------------------
//
TInt CSVPHoldInEstablishingState::
DefineDefaultCaseStateChange( CSVPHoldContext& aContext,
                          TSVPHoldStateIndex& aNextState )
    {
    switch ( aContext.HoldRequest() )
        {
        case ESVPRemoteHold:
            {
            aNextState = KSVPHoldInStateIndex;
            return KErrNone;
            }
            
        case ESVPRemoteResume:
            {
            aNextState = KSVPHoldConnectedStateIndex;
            return KErrNone;
            }
            
        case ESVPRemoteDoubleHold:
            {
            aNextState = KSVPHoldDHStateIndex;
            return KErrNone;
            }
            
        case ESVPRemoteDoubleHoldResume:
            {
            aNextState = KSVPHoldOutStateIndex;
            return KErrNone;
            }
            
        default:
            {
            SVPDEBUG1(
            "CSVPHoldInEstablishingState::CheckAttributeL - Default" );
            return KErrSVPHoldUnacceptableResponseAttribute;
            }
        }
    }


