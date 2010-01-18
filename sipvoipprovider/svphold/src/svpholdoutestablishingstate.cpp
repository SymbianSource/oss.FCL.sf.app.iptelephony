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
* Description:  Outgoing request establishing state for hold state machine.
*
*/


#include    <badesca.h>
#include    "mcesession.h"
#include    "mcemediastream.h"
#include    "mceaudiostream.h"
#include    "svpholdoutestablishingstate.h"
#include    "svpholdcontext.h"
#include    "svpholdcontroller.h"
#include    "svpholdattributehandler.h"
#include    "svpholdobserver.h"
#include    "svplogger.h"


// ---------------------------------------------------------------------------
// CSVPHoldOutEstablishingState::CSVPHoldOutEstablishingState
// ---------------------------------------------------------------------------
CSVPHoldOutEstablishingState::CSVPHoldOutEstablishingState()
    {
    }

// ---------------------------------------------------------------------------
// CSVPHoldOutEstablishingState::NewLC
// ---------------------------------------------------------------------------
CSVPHoldOutEstablishingState* CSVPHoldOutEstablishingState::NewLC()
    {
    CSVPHoldOutEstablishingState* self = 
        new ( ELeave ) CSVPHoldOutEstablishingState;
    CleanupStack::PushL( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CSVPHoldOutEstablishingState::~CSVPHoldOutEstablishingState
// ---------------------------------------------------------------------------
CSVPHoldOutEstablishingState::~CSVPHoldOutEstablishingState()
    {
    }

// ---------------------------------------------------------------------------
// CSVPHoldOutEstablishingState::DoApplyL
// ---------------------------------------------------------------------------
//
void CSVPHoldOutEstablishingState::DoApplyL( CSVPHoldContext& aContext )
    {
    SVPDEBUG1( "CSVPHoldOutEstablishingState::DoApply" );
    
    TSVPHoldStateIndex nextState = KSVPHoldEstablishingStateIndex; 
    CMceSession* session = aContext.SessionObject();
    const RPointerArray< CMceMediaStream >& streams = session->Streams();
    TInt streamCount = streams.Count();
    
    // Hold / Resume handling depends on the response received from network and
    // timers; any differences from normal handled in SpecialResponseHandling:
    if ( !aContext.SpecialResponseHandling( nextState ) )
        {
        for ( TInt i = 0; i < streamCount; i++ )
            {
            CMceMediaStream* mediaStream = streams[ i ];
            TMceMediaType mediaType = mediaStream->Type();
            if ( KMceAudio == mediaType )
                {
                // This media is audio stream. Handling depends on the request
                switch ( aContext.HoldRequest() )
                    {
                    case ESVPLocalHold:
                    case ESVPLocalResume:
                    case ESVPLocalDoubleHold:
                    case ESVPLocalDoubleHoldResume:
                        {
                        HandleLocalHoldL( aContext, mediaStream, nextState );
                        break;
                        }
                    
                    case ESVPRemoteHold:
                    case ESVPRemoteResume:
                    case ESVPRemoteDoubleHold:
                    case ESVPRemoteDoubleHoldResume:
                        {
                        SVPDEBUG1(
                        "CSVPHoldOutEstablishingState::DoApply\
                         - Remote action - State Error" );                   
                        User::Leave( KErrSVPHoldStateError );
                        }
                        
                    default:
                        {
                        // Error in request solving
                        SVPDEBUG2(
                        "CSVPHoldOutEstablishingState::CSVPHoldOutEstablishingState\
                         - Error, request %i",
                        aContext.HoldRequest() );
                        break;
                        }
                    }
                }
            }    
        }
    
    aContext.SetCurrentStateL( aContext, nextState );
    SVPDEBUG1( "CSVPHoldOutEstablishingState::DoApply - Handled" );
    }
    
// ---------------------------------------------------------------------------
// CSVPHoldOutEstablishingState::DoEnter
// ---------------------------------------------------------------------------
//
void CSVPHoldOutEstablishingState::DoEnter( CSVPHoldContext& /*aContext*/ )
    {
    SVPDEBUG1( "CSVPHoldOutEstablishingState::DoEnter" );
    }

// ---------------------------------------------------------------------------
// CSVPHoldOutEstablishingState::IsOutEstablishingStateActive
// ---------------------------------------------------------------------------
//
TBool CSVPHoldOutEstablishingState::IsOutEstablishingStateActive()
    {
    return ETrue;
    }
    
// ---------------------------------------------------------------------------
// CSVPHoldOutEstablishingState::HandleLocalHoldL
// ---------------------------------------------------------------------------
//
void CSVPHoldOutEstablishingState::
HandleLocalHoldL( CSVPHoldContext& aContext,
                  CMceMediaStream* aMediaStream,
                  TSVPHoldStateIndex& aNextState )
    {
    switch ( aContext.HoldRequest() )
        {
        case ESVPLocalHold:
            {
            // From Connected state
            HandleLocalHoldingL( aContext,
                                 aMediaStream,
                                 aNextState );
            break;
            }
            
        case ESVPLocalResume:
            {
            // From Out state
            HandleLocalResumingL( aContext,
                                  aMediaStream,
                                  aNextState );
            break;
            }

        case ESVPLocalDoubleHold:
            {
            // From In state
            HandleLocalHoldingL( aContext,
                                 aMediaStream,
                                 aNextState,
                                 KSVPHoldDHStateIndex );
            break;
            }

         case ESVPLocalDoubleHoldResume:
            {
            // From Doublehold state
            HandleLocalResumingL( aContext,
                                  aMediaStream,
                                  aNextState,
                                  KSVPHoldInStateIndex );
            break;
            }       
        }
    }

// ---------------------------------------------------------------------------
// CSVPHoldOutEstablishingState::HandleLocalHoldingL
// ---------------------------------------------------------------------------
//
void CSVPHoldOutEstablishingState::
HandleLocalHoldingL( CSVPHoldContext& aContext,
                     CMceMediaStream* mediaStream,
                     TSVPHoldStateIndex& aNextState,
                     TSVPHoldStateIndex aState )
    {
    SVPDEBUG1(
    "CSVPHoldOutEstablishingState::HandleLocalHoldingL" );
    
    MDesC8Array* attributeLines = mediaStream->MediaAttributeLinesL();
    CleanupDeletePushL( attributeLines );

    if ( KSVPHoldDHStateIndex == aState )
        {
        SVPDEBUG1(
        "CSVPHoldOutEstablishingState::HandleLocalHoldingL\
         - Next KSVPHoldDHStateIndex" );
        //aNextState = KSVPHoldDHStateIndex;
        KSVPHoldAttributeIndex neededAttribute = KSVPHoldInactiveIndex;
        CheckAttributeL( attributeLines,
                         neededAttribute,
                         aNextState,
                         aContext );
        }
    else
        {
        SVPDEBUG1(
        "CSVPHoldOutEstablishingState::HandleLocalHoldingL\
         - Next KSVPHoldOutStateIndex" );
        //aNextState = KSVPHoldOutStateIndex;
        KSVPHoldAttributeIndex neededAttribute = KSVPHoldRecvonlyIndex;
        CheckAttributeL( attributeLines,
                         neededAttribute,
                         aNextState,
                         aContext );
        }

    CleanupStack::PopAndDestroy( attributeLines );
    
    SVPDEBUG1( "CSVPHoldOutEstablishingState::HandleLocalHoldingL - Done" );
    }

// ---------------------------------------------------------------------------
// CSVPHoldOutEstablishingState::HandleLocalResumingL
// ---------------------------------------------------------------------------
//
void CSVPHoldOutEstablishingState::
HandleLocalResumingL( CSVPHoldContext& aContext,
                      CMceMediaStream* mediaStream,
                      TSVPHoldStateIndex& aNextState,
                      TSVPHoldStateIndex aState )
    {
    SVPDEBUG1(
    "CSVPHoldOutEstablishingState::HandleLocalResumingL" );
    
    MDesC8Array* attributeLines = mediaStream->MediaAttributeLinesL();
    CleanupDeletePushL( attributeLines );
    
    if ( KSVPHoldInStateIndex == aState )
        {
        // Resume from doublehold:
        SVPDEBUG1(
        "CSVPHoldOutEstablishingState::HandleLocalResumingL\
         - Next KSVPHoldInStateIndex" );
        //aNextState = KSVPHoldInStateIndex;
        KSVPHoldAttributeIndex neededAttribute = KSVPHoldSendonlyIndex;
        CheckAttributeL( attributeLines,
                         neededAttribute,
                         aNextState,
                         aContext );    
        }
        
    else
        {
        // Basic resume:
        SVPDEBUG1(
        "CSVPHoldOutEstablishingState::HandleLocalResumingL\
         - Next KSVPHoldConnectedStateIndex" );
        //aNextState = KSVPHoldConnectedStateIndex;
        KSVPHoldAttributeIndex neededAttribute = KSVPHoldSendrecvIndex;
        CheckAttributeL( attributeLines,
                         neededAttribute,
                         aNextState,
                         aContext );                                  
        }
        
    CleanupStack::PopAndDestroy( attributeLines );

    SVPDEBUG1(
    "CSVPHoldOutEstablishingState::HandleLocalResumingL - Done" );
    }
    
// ---------------------------------------------------------------------------
// CSVPHoldOutEstablishingState::CheckAttributeL
// ---------------------------------------------------------------------------
//
TInt CSVPHoldOutEstablishingState::
CheckAttributeL( MDesC8Array* aAttributeLines,
                KSVPHoldAttributeIndex aNeededAttribute,
                TSVPHoldStateIndex& aNextState,
                CSVPHoldContext& aContext )
    {
    SVPDEBUG1( "CSVPHoldOutEstablishingState::CheckAttributeL" );

    KSVPHoldAttributeIndex responseAttribute =
        aContext.AttributeHandler().FindDirectionAttribute( aAttributeLines );

    if ( KErrNotFound == responseAttribute )
        {
        SVPDEBUG1( "CSVPHoldOutEstablishingState::CheckAttributeL - Session level" );
        
        MDesC8Array* sessionAttributeLines =
            aContext.SessionObject()->SessionSDPLinesL();
        
        responseAttribute =
            aContext.AttributeHandler().FindDirectionAttribute(
                sessionAttributeLines );
        
        aContext.SessionObject()->SetSessionSDPLinesL(
            static_cast<CDesC8Array*>( sessionAttributeLines ) );
        
        SVPDEBUG1( "CSVPHoldOutEstablishingState::CheckAttributeL - Session level done" );
        }
        
    if ( aNeededAttribute == responseAttribute || 
         KErrNotFound == responseAttribute )
        {
        // If there is no attributes in response; but 200Ok is received;
        // state change is done based to default cases.
        return DefineDefaultCaseStateChange( aContext, aNextState );
        }
        
    else if ( KSVPHoldSendrecvIndex == aNeededAttribute &&
              KErrNotFound == responseAttribute )
        {
        // If there is no direction attribute at all, default is sendrecv
        SVPDEBUG1(
        "CSVPHoldOutEstablishingState::CheckAttributeL - No attribute" );
        aNextState = KSVPHoldConnectedStateIndex;
        return KErrNone;
        }

    else if ( KSVPHoldSendrecvIndex == aNeededAttribute &&
              KSVPHoldRecvonlyIndex == responseAttribute )
        {
        // Radvision client fix (it responds to resume with recvonly)
        SVPDEBUG1(
        "CSVPHoldOutEstablishingState::CheckAttributeL - recvonly to resume.." );
        aNextState = KSVPHoldConnectedStateIndex;
        return KErrNone;
        }
    
    else if ( KSVPHoldRecvonlyIndex == aNeededAttribute &&
              KSVPHoldInactiveIndex == responseAttribute )
        {
        // Inactive response to local hold is allowed
        SVPDEBUG1(
        "CSVPHoldOutEstablishingState::CheckAttributeL - No attribute" );
        aNextState = KSVPHoldOutStateIndex;
        return KErrNone;
        }
        
    else if( KSVPHoldSendonlyIndex == responseAttribute &&
             KSVPHoldInactiveIndex == aNeededAttribute )
        {
        // Sendonly response to local DH (accept for IOP)
        SVPDEBUG1(
        "CSVPHoldOutEstablishingState::CheckAttributeL - Sendonly to Inactive request" );
        aNextState = KSVPHoldDHStateIndex;
        return KErrNone;        
        }

    else if( KSVPHoldSendonlyIndex == aNeededAttribute &&
             KSVPHoldInactiveIndex == responseAttribute )
        {
        // Inactive response to local DH resume (accept for IOP)
        SVPDEBUG1(
        "CSVPHoldOutEstablishingState::CheckAttributeL - Inactive to DH resume request" );
        aNextState = KSVPHoldInStateIndex;
        return KErrNone;        
        }   
    
    else if ( KSVPHoldSendrecvIndex == responseAttribute )
        {
        // Response has sendrecv -attribute
        return ResolveStateFromSendRecvIndex( aNeededAttribute, aNextState );
        }

    else
        {
        SVPDEBUG1(
        "CSVPHoldOutEstablishingState::CheckAttributeL - else" );
        return KErrNone;
        }        
    }
    
// ---------------------------------------------------------------------------
// CSVPHoldOutEstablishingState::DefineDefaultCaseStateChange
// ---------------------------------------------------------------------------
//
TInt CSVPHoldOutEstablishingState::
DefineDefaultCaseStateChange( CSVPHoldContext& aContext,
                              TSVPHoldStateIndex& aNextState )
    {
    SVPDEBUG1( "CSVPHoldOutEstablishingState::DefineDefaultCaseStateChange" );
    // Basic cases (e.g for holding "sendonly" answer has "recvonly")
    switch ( aContext.HoldRequest() )
        {
        case ESVPLocalHold:
            {
            SVPDEBUG1( "    ESVPLocalHold" );
            aNextState = KSVPHoldOutStateIndex;
            return KErrNone;
            }
             
        case ESVPLocalResume:
            {
            SVPDEBUG1( "    ESVPLocalResume" );
            aNextState = KSVPHoldConnectedStateIndex;
            return KErrNone;
            }
            
        case ESVPLocalDoubleHold:
            {
            SVPDEBUG1( "    ESVPLocalDoubleHold" );
            aNextState = KSVPHoldDHStateIndex;
            return KErrNone;
            }
            
        case ESVPLocalDoubleHoldResume:
            {
            SVPDEBUG1( "    ESVPLocalDoubleHoldResume" );
            aNextState = KSVPHoldInStateIndex;
            return KErrNone;
            }
            
        default:
            {
            SVPDEBUG1(
            "CSVPHoldOutEstablishingState::DefineDefaultCaseStateChange" );
             return KErrNone;
            }
        }    
    }

// ---------------------------------------------------------------------------
// CSVPHoldOutEstablishingState::ResolveStateFromSendRecvIndex
// ---------------------------------------------------------------------------
//
TInt CSVPHoldOutEstablishingState::
ResolveStateFromSendRecvIndex( KSVPHoldAttributeIndex aNeededAttribute,
                               TSVPHoldStateIndex& aNextState )
    {
    SVPDEBUG1( "CSVPHoldOutEstablishingState::ResolveStateFromSendRecvIndex" );
    switch ( aNeededAttribute )
        {
        case KSVPHoldRecvonlyIndex:
            {
            aNextState = KSVPHoldOutStateIndex;
            return KErrNone;
            }

        case KSVPHoldInactiveIndex:
            {
            aNextState = KSVPHoldDHStateIndex;
            return KErrNone;
            }

        case KSVPHoldSendonlyIndex:
            {
            aNextState = KSVPHoldInStateIndex;
            return KErrNone;
            }
        
        default:
            {
            SVPDEBUG1(
            "CSVPHoldOutEstablishingState::CheckAttributeL - Last default" );
            return KErrNone;
            }
        }
    }    


