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
* Description:  Base class for session context
*
*/


#include    <s32mem.h>
#include    <in_sock.h>
#include    <in_pkt.h>
#include    <badesca.h>
#include    <mcesession.h>
#include    <mcemanager.h>
#include    <mcetransactiondatacontainer.h>
#include    <mcertpsource.h>
#include    <mcertpsink.h>
#include    <mcemediastream.h>
#include    <mceaudiostream.h>

#include    <sdpcodecstringpool.h>
#include    <sdpcodecstringconstants.h>
#include    <sdpdocument.h>
#include    <sdpconnectionfield.h>
#include    "svpholdcontext.h"
#include    "svpholdoutstate.h"
#include    "svpholdinestablishingstate.h"
#include    "svpholdoutestablishingstate.h"
#include    "svpholdinstate.h"
#include    "svpholdconnectedstate.h"
#include    "svpholddhstate.h"
#include    "svpholdcontroller.h"
#include    "svpholdmediahandler.h"
#include    "svpholdattributehandler.h"
#include    "svpholdobserver.h"
#include    "svplogger.h"
#include    "svpaudioutility.h"
#include    "svpsipconsts.h"


// ---------------------------------------------------------------------------
// CSVPHoldContext::CSVPHoldContext
// ---------------------------------------------------------------------------
//
CSVPHoldContext::CSVPHoldContext( TMceTransactionDataContainer& aContainer,
                                  TBool aIsMobileOriginated ) :
        iStates( NULL ),
        iObserver( NULL ),
        iCurrentState( NULL ),
        iHoldRequest( ESVPNoType ),
        iSession( NULL ),
        iOutgoing( ETrue ),
        iAttributeHandler( NULL ),
        iMediaHandler( NULL ),
        iFirstAttempt( ETrue ),
        iRemoteRequestNotProcessed( ETrue ),
        iContainer( aContainer ),
        iAllowAudioHandling( EFalse ),
        iResponseStatusCode( KErrNotFound ),
        iForceHoldOccured( EFalse ),
        iTimedOut( EFalse ),
        iRollBack( EFalse ),
        iHoldFail( EFalse ),
        iResumeFail( EFalse ),
        iMuted( EFalse ),
        iCrossOver( EFalse ),
        iCallRequest( EFalse )
    {
    iIsMobileOriginated = aIsMobileOriginated;
    }

// ---------------------------------------------------------------------------
// CSVPHoldContext::ConstructL
// ---------------------------------------------------------------------------
//
void CSVPHoldContext::ConstructL( CMceSession& aSession,
                                  MSVPHoldObserver* aObserver )
    {
    SVPDEBUG1( "CSVPHoldContext::ConstructL In" );

    iObserver = aObserver;
    // Create the states in the state array.
    InitializeStateArrayL();
    iAttributeHandler = CSVPHoldAttributeHandler::NewL();
    iMediaHandler = CSVPHoldMediaHandler::NewL();
    
    SetSessionObject( &aSession );
    
    // Initialize connected state to current state. Note, that it is not
    // applied yet.
    SetCurrentStateL( *this, KSVPHoldConnectedStateIndex );
    SetAudioHandlingAllowed( ETrue );
        
    SVPDEBUG1( "CSVPHoldContext::ConstructL Done" );
    }

// -----------------------------------------------------------------------------
// CSVPHoldContext::NewL
// -----------------------------------------------------------------------------
//
CSVPHoldContext* CSVPHoldContext::NewL( 
                            CMceSession& aSession,
                            TMceTransactionDataContainer& aContainer,
                            MSVPHoldObserver* aObserver,
                            TBool aIsMobileOriginated )
    {    
    CSVPHoldContext* self = new ( ELeave ) CSVPHoldContext( aContainer,
                                                            aIsMobileOriginated );
    
    CleanupStack::PushL( self );
    self->ConstructL( aSession, aObserver );
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------------------------
// CSVPHoldContext::IsStateTransitionAccepted
// ---------------------------------------------------------------------------
//
TBool CSVPHoldContext::
IsStateTransitionAccepted( const TSVPHoldStateIndex aStateIndex )
    {
    TSVPHoldStateIndex current = CurrentState();
    switch( current )
        {
        case KErrNotFound:
            {
            // Only connected state can be the first one.
            if ( KSVPHoldConnectedStateIndex == aStateIndex )
                {
                return ETrue;
                }
            else
                {
                return EFalse;
                }
            }

        case KSVPHoldConnectedStateIndex:
        case KSVPHoldInStateIndex:
        case KSVPHoldDHStateIndex:
            { 
            // From main states transitions to in or out establishing
            // states are accepted.
            if ( KSVPHoldEstablishingStateIndex  == aStateIndex )
                {
                return ETrue;
                }
            else
                {
                return EFalse;
                }
            }
        
        case KSVPHoldOutStateIndex:
            {
            // From hold out state transitions to in and out establishing
            // and connected states are accepted.
            if ( KSVPHoldEstablishingStateIndex  == aStateIndex ||
                 KSVPHoldConnectedStateIndex  == aStateIndex )
                {
                return ETrue;
                }
            else
                {
                return EFalse;
                }
            }
        
        case KSVPHoldEstablishingStateIndex:
            {
            // From establishing states transitions to main states
            // are accepted.
            if ( KSVPHoldEstablishingStateIndex == aStateIndex ||
                 KSVPHoldConnectedStateIndex  == aStateIndex ||
                 KSVPHoldOutStateIndex == aStateIndex ||
                 KSVPHoldInStateIndex == aStateIndex ||
                 KSVPHoldDHStateIndex == aStateIndex )
                {
                return ETrue;
                }
            else
                {
                return EFalse;
                }
            }
            
        default:
            {
            // Should not come here, since all the states are handled
            SVPDEBUG1( "CSVPHoldContext::IsStateTransitionAccepted - Error" );
            
            return EFalse;
            }
        }
    }

// ---------------------------------------------------------------------------
// CSVPHoldContext::~CSVPHoldContext
// ---------------------------------------------------------------------------   
//
CSVPHoldContext::~CSVPHoldContext()
    {
    SVPDEBUG1( "CSVPHoldContext::~CSVPHoldContext - In" );
    if ( iStates )
        {
        iStates->ResetAndDestroy();
        iStates->Close();
        delete iStates;
        }

    delete iAttributeHandler;
    delete iMediaHandler;
    
    SVPDEBUG1( "CSVPHoldContext::~CSVPHoldContext - Done" );
    }

// ---------------------------------------------------------------------------
// CSVPHoldContext::InitializeStateArrayL
// ---------------------------------------------------------------------------
//
void CSVPHoldContext::InitializeStateArrayL()
    {
    // Create state array
    iStates = new ( ELeave ) RPointerArray< CSVPHoldStateBase >
                                                ( KSVPHoldStateArraySize );
    // States are created here:

    // Connected state
    CSVPHoldConnectedState* connectedState =
                    CSVPHoldConnectedState::NewLC();
    User::LeaveIfError( iStates->Insert( connectedState,
                                         KSVPHoldConnectedStateIndex ) );
    CleanupStack::Pop( connectedState );

    // Establishing state initialized to outgoing (MO hold)
    CSVPHoldOutEstablishingState* outEstState =
                    CSVPHoldOutEstablishingState::NewLC();
    User::LeaveIfError( iStates->Insert( outEstState,
                                         KSVPHoldEstablishingStateIndex ) );
    CleanupStack::Pop( outEstState );

    // Outgoing state (MO hold)
    CSVPHoldOutState* outState =
                    CSVPHoldOutState::NewLC();
    User::LeaveIfError( iStates->Insert( outState,
                                         KSVPHoldOutStateIndex ) );
    CleanupStack::Pop( outState );
        
    // Incoming state (MT hold)
    CSVPHoldInState* inState =
                    CSVPHoldInState::NewLC();
    User::LeaveIfError( iStates->Insert( inState,
                                         KSVPHoldInStateIndex ) );
    CleanupStack::Pop( inState );
    
    // Doublehold state (MO & MT simultaneous hold)
    CSVPHoldDHState* dhState =
                    CSVPHoldDHState::NewLC();
    User::LeaveIfError( iStates->Insert( dhState,
                                         KSVPHoldDHStateIndex ) );
    CleanupStack::Pop( dhState );
    }

// ---------------------------------------------------------------------------
// CSVPHoldContext::SetCurrentStateL
// ---------------------------------------------------------------------------
//
void CSVPHoldContext::SetCurrentStateL( CSVPHoldContext& aContext,
                                        TSVPHoldStateIndex aStateIndex )
    {
    // Check that the transition is valid:
    if ( !IsStateTransitionAccepted( aStateIndex ) )
        {
        SVPDEBUG2( "CSVPHoldContext::SetCurrentStateL - State error, New: %i",
                    aStateIndex );

        User::Leave( KErrSVPHoldStateError );
        }        
    else
        {
        SVPDEBUG2( "CSVPHoldContext::SetCurrentStateL - Set state from %i",
                    CurrentState() );
         
        SVPDEBUG2( "CSVPHoldContext::SetCurrentStateL - Set state to %i",
                    aStateIndex );

        iCurrentState = ( *iStates )[ aStateIndex ];
        iCurrentState->Enter( aContext );
        iAllowAudioHandling = ETrue;
        }
    }

// ---------------------------------------------------------------------------
// CSVPHoldContext::CurrentState
// ---------------------------------------------------------------------------
//
TSVPHoldStateIndex CSVPHoldContext::CurrentState() const
   {
   return iStates->Find( iCurrentState );
   }

// ---------------------------------------------------------------------------
// CSVPHoldContext::ApplyCurrentStateL
// ---------------------------------------------------------------------------
//
void CSVPHoldContext::ApplyCurrentStateL( CMceSession* aSession,
                    TSVPHoldDesiredTransition aTransition )
    {
    SVPDEBUG1( "CSVPHoldContext::ApplyCurrentStateL IN" )
    
    // Set back to false, because new hold/resume attemp starts
    iRollBack = EFalse;
    
    SetSessionObject( aSession );
    if ( ToEstablishing() && FirstAttempt() )
        {
        SVPDEBUG1( "CSVPHoldContext::ApplyCurrentStateL - New request" )
        
        iHoldRequest = RequestType( aTransition );
        UpdateEstablishingStateL();
        }

    if ( ESVPHoldIncoming == aTransition )
        {
        SVPDEBUG1( "CSVPHoldContext::ApplyCurrentStateL - Remote request" )
        
        iHoldRequest = RequestType( aTransition );        
        SetRemoteRequestNotProcessed( ETrue );
        iCurrentState->ApplyL( *this );
        }
    
    else
        {
        SVPDEBUG1( "CSVPHoldContext::ApplyCurrentStateL - Local request" )
        
        iHoldRequest = RequestType( aTransition );
        if ( ESVPLocalResume == iHoldRequest )
            {
            SVPDEBUG1( "CSVPHoldContext::ApplyCurrentStateL - Enable oldway hold" );
            
            // establish Old way hold support 
            aSession->SetModifierL( KMceMediaDirection, 
                                    KMceMediaDirectionWithAddress );
            }
            
        iCurrentState->ApplyL( *this );
        }
        
    SVPDEBUG1( "CSVPHoldContext::ApplyCurrentStateL OUT" )
    }

// ---------------------------------------------------------------------------
// CSVPHoldContext::ApplyCurrentStateL
// ---------------------------------------------------------------------------
//
void CSVPHoldContext::ApplyCurrentStateL()
    {
    if ( KSVPHoldEstablishingStateIndex == CurrentState() )
        {
        SVPDEBUG1( "CSVPHoldContext::ApplyCurrentStateL2" );
        iCurrentState->ApplyL( *this );
        }
    else
        {
        SVPDEBUG1( "CSVPHoldContext::ApplyCurrentStateL2 - State error" );
        User::Leave( KErrSVPHoldStateError );
        }
    }

// ---------------------------------------------------------------------------
// CSVPHoldContext::SetSessionObject
// ---------------------------------------------------------------------------
//
void CSVPHoldContext::SetSessionObject( CMceSession* aSession )
    {
    iSession = aSession;
    }

// ---------------------------------------------------------------------------
// CSVPHoldContext::SessionObject
// ---------------------------------------------------------------------------
//
CMceSession* CSVPHoldContext::SessionObject()
    {
    return iSession;
    }

// ---------------------------------------------------------------------------
// CSVPHoldContext::HoldRequest
// ---------------------------------------------------------------------------
//
TSVPHoldRequestType CSVPHoldContext::HoldRequest()
    {
    return iHoldRequest;
    }


// ---------------------------------------------------------------------------
// CSVPHoldContext::IsRemoteRequest
// ---------------------------------------------------------------------------
//
TBool CSVPHoldContext::IsRemoteRequest()
    {
    TSVPHoldRequestType request = HoldRequest();
    if ( ESVPRemoteHold == request || ESVPRemoteResume == request ||
         ESVPRemoteDoubleHold == request ||
         ESVPRemoteDoubleHoldResume == request )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// ---------------------------------------------------------------------------
// CSVPHoldContext::HoldEvent
// ---------------------------------------------------------------------------
//
MCCPCallObserver::TCCPCallEvent CSVPHoldContext::HoldEvent()
    {
    return iHoldEvent;
    }

// ---------------------------------------------------------------------------
// CSVPHoldContext::SetFirstAttempt
// ---------------------------------------------------------------------------
//
void CSVPHoldContext::SetFirstAttempt( TBool aValue )
    {
    iFirstAttempt = aValue;
    }
    
// ---------------------------------------------------------------------------
// CSVPHoldContext::FirstAttempt
// ---------------------------------------------------------------------------
//
TBool CSVPHoldContext::FirstAttempt()
    {
    return iFirstAttempt;
    }

    
// ---------------------------------------------------------------------------
// CSVPHoldContext::SetRemoteRequestNotProcessed
// ---------------------------------------------------------------------------
//
void CSVPHoldContext::SetRemoteRequestNotProcessed( TBool aValue )
    {
    iRemoteRequestNotProcessed = aValue;
    }
    
// ---------------------------------------------------------------------------
// CSVPHoldContext::RemoteRequestNotProcessed
// ---------------------------------------------------------------------------
//
TBool CSVPHoldContext::RemoteRequestNotProcessed()
    {
    return iRemoteRequestNotProcessed;
    }

// ---------------------------------------------------------------------------
// CSVPHoldContext::SetAudioHandlingAllowed
// ---------------------------------------------------------------------------
//
void CSVPHoldContext::SetAudioHandlingAllowed( TBool aValue )
    {
    iAllowAudioHandling = aValue;
    }
    
// ---------------------------------------------------------------------------
// CSVPHoldContext::AudioHandlingAllowed
// ---------------------------------------------------------------------------
//
TBool CSVPHoldContext::AudioHandlingAllowed()
    {
    return iAllowAudioHandling;
    }

// ---------------------------------------------------------------------------
// CSVPHoldContext::CheckOldwayHoldL
// ---------------------------------------------------------------------------
//
TSVPHoldRequestType CSVPHoldContext::
CheckOldwayHoldL( CMceSession& aSession,
                  TSVPHoldRequestType aPresumedRequest )
    {
    SVPDEBUG1( "CSVPHoldContext::CheckOldwayHoldL" );
    
    TBool oldWayHold = EFalse;    
    const RPointerArray< CMceMediaStream >& mediaStreams = aSession.Streams();
        
    for ( TInt i = 0; i < mediaStreams.Count(); i++ )
        {
        CMceAudioStream* stream = 
            static_cast< CMceAudioStream* >( mediaStreams[ i ] );
        
        if ( !SVPAudioUtility::IsDownlinkStream( *stream ) )
            {
            if ( !stream->IsEnabled() )
                {
                oldWayHold = ETrue;
                }
            }

        else if ( stream->BoundStream() )
            {
            CMceMediaStream& boundStream = stream->BoundStreamL();
            if ( !boundStream.IsEnabled() )
                {
                oldWayHold = ETrue;
                }
            }
        }
    
    if ( !oldWayHold )
        {
        // Oldway hold not in offer
        SVPDEBUG1( "CSVPHoldContext::CheckOldwayHoldL - Direction only" );
        return ESVPNoType;
        }
        
    else
        {
        // Oldway hold used in offer
        SVPDEBUG1( "CSVPHoldContext::CheckOldwayHoldL - Oldway hold ON" );
        TUint modValue = KMceMediaDirectionWithAddress;
        TMceSessionModifier modifier = KMceMediaDirection;
        aSession.SetModifierL( modifier, modValue );
        return aPresumedRequest;
        }
    }

    
// ---------------------------------------------------------------------------
// CSVPHoldContext::HoldObserver
// ---------------------------------------------------------------------------
//
MSVPHoldObserver& CSVPHoldContext::HoldObserver()
    {
    return *iObserver;
    }
        
// ---------------------------------------------------------------------------
// CSVPHoldContext::AttributeHandler
// ---------------------------------------------------------------------------
//
CSVPHoldAttributeHandler& CSVPHoldContext::AttributeHandler()
    {
    return *iAttributeHandler;
    }

// ---------------------------------------------------------------------------
// CSVPHoldContext::MediaHandler
// ---------------------------------------------------------------------------
//
CSVPHoldMediaHandler& CSVPHoldContext::MediaHandler()
    {
    return *iMediaHandler;
    }

// ---------------------------------------------------------------------------
// CSVPHoldContext::SolveRequestL
// ---------------------------------------------------------------------------
//
TSVPHoldRequestType 
CSVPHoldContext::SolveRequestL( CMceSession& aSession,
                                MDesC8Array* aAttributeLines,
                                TBool aCheckOldwayHold )
    { 
    SVPDEBUG1( "CSVPHoldContext::SolveRequestL IN" )
    
    TSVPHoldRequestType presumedRequest = HoldRequest();
    TInt attributeIndex =
        iAttributeHandler->FindDirectionAttribute( aAttributeLines );

    SVPDEBUG2( "CSVPHoldContext::SolveRequestL - attributeIndex = %i",
        attributeIndex )
    SVPDEBUG2( "CSVPHoldContext::SolveRequestL - presumedRequest = %i",
        presumedRequest )
    
    // Match Current state, attributeIndex and presumedRequest
    TSVPHoldRequestType realizedRequest = 
        SolveRequestType( attributeIndex, presumedRequest );
    
    SVPDEBUG2( "CSVPHoldContext::SolveRequestL - realizedRequest = %i",
        realizedRequest )
    
    if ( ESVPNoType == realizedRequest && aCheckOldwayHold )
        {
        SVPDEBUG1( "CSVPHoldContext::SolveRequestL - Check oldway hold" )
        
        realizedRequest = CheckOldwayHoldL( aSession, presumedRequest );
                                            
        SVPDEBUG2( "CSVPHoldContext::SolveRequestL - if:realizedRequest = %i",
            realizedRequest )
        }
                
    return realizedRequest;
    }

// ---------------------------------------------------------------------------
// CSVPHoldContext::SolveRequestTypeL
// Solves type of incoming request and returns accepted request type
// ---------------------------------------------------------------------------
//
TSVPHoldRequestType 
CSVPHoldContext::SolveRequestType( TInt aAttributeIndex,
                                   TSVPHoldRequestType aPresumedRequest )
    {
    SVPDEBUG1( "CSVPHoldContext::SolveRequestType - Type is" );
    
    switch ( aPresumedRequest )
        {
        case ESVPRemoteHold:
            {
            // From connected state, MT hold requests:
            SVPDEBUG1( "    ESVPRemoteHold" );
            if ( KSVPHoldSendonlyIndex == aAttributeIndex ||
                 KSVPHoldInactiveIndex == aAttributeIndex )
                {
                // Incoming hold
                return ESVPRemoteHold; 
                }

            else
                {
                SVPDEBUG1( "    ESVPNoType" );
                return ESVPNoType;
                }
            }
        
        case ESVPRemoteResume:
            {            
            SVPDEBUG1( "    ESVPRemoteResume" );
            if ( KSVPHoldSendrecvIndex == aAttributeIndex ||
                 KErrNotFound == aAttributeIndex )
                {
                // Incoming resume
                return ESVPRemoteResume; 
                }
                                
            else
                {
                SVPDEBUG1( "    ESVPNoType" );
                return ESVPNoType;
                }                
            }
        
        case ESVPRemoteDoubleHold:
            {
            SVPDEBUG1( "    ESVPRemoteDoubleHold" );
            if ( KSVPHoldInactiveIndex == aAttributeIndex )
                {
                // Incoming doublehold
                return ESVPRemoteDoubleHold;
                }

            else
                {
                SVPDEBUG1( "    ESVPNoType" );
                return ESVPNoType;
                }
            }
        
        case ESVPRemoteDoubleHoldResume:
            {
            SVPDEBUG1( "    ESVPRemoteDHResume" );
            if ( KSVPHoldRecvonlyIndex == aAttributeIndex ||
                 KSVPHoldSendrecvIndex == aAttributeIndex )
                {
                // Incoming doublehold resume
                return ESVPRemoteDoubleHoldResume;
                }

            else
                {
                SVPDEBUG1( "    ESVPNoType" );
                return ESVPNoType;
                }
            }
        
        
        // -fallthtrough
        case ESVPLocalHold:
        case ESVPLocalResume:
        case ESVPLocalDoubleHold:
        case ESVPLocalDoubleHoldResume:
        default:
            {
            // Local actions not handled here:
            SVPDEBUG1( "CSVPHoldContext::SolveRequestType - Default" );            
            return ESVPNoType;
            }
        }
    }

// ---------------------------------------------------------------------------
// CSVPHoldContext::ToEstablishing
// Solves if the next state should be establishing state
// ---------------------------------------------------------------------------
//
TBool CSVPHoldContext::ToEstablishing()
    {
    if ( KSVPHoldEstablishingStateIndex != CurrentState() )
        {
        SVPDEBUG1( "CSVPHoldContext::ToEstablishing TRUE" )
        
        return ETrue;
        }
    else
        {
        SVPDEBUG1( "CSVPHoldContext::ToEstablishing FALSE" )
        
        return EFalse;
        }
    }
    
// ---------------------------------------------------------------------------
// CSVPHoldContext::RequestType
// Solves request type based on current state and external request and
// direction of request
// ---------------------------------------------------------------------------
//
TSVPHoldRequestType 
CSVPHoldContext::RequestType( TSVPHoldDesiredTransition aTransition )
    {
    // switch-case structure has no breaks because every case has return.
    switch ( aTransition )
        {
        case ESVPHoldToHold:
            {
            return RequestToHold();
            }
        
        case ESVPHoldToResume:
            {
            return RequestToResume();
            }
        
        case ESVPHoldIncoming:
            {
            return RequestIncoming();
            }
            
        default:
            {
            SVPDEBUG1( "CSVPHoldContext::RequestType - no transition!" );

            return ESVPNoType;
            }
        }
    }


// ---------------------------------------------------------------------------
// CSVPHoldContext::RequestToHold
// Solves request type for local hold
// ---------------------------------------------------------------------------
//
TSVPHoldRequestType CSVPHoldContext::RequestToHold()
    {
    SVPDEBUG1( "CSVPHoldContext::RequestToHold" );
        
    TSVPHoldStateIndex current = CurrentState();
    iOutgoing = ETrue;
    iHoldEvent = MCCPCallObserver::ECCPLocalHold;

    if ( KSVPHoldConnectedStateIndex == current )
        {
        return ESVPLocalHold;
        }

    else if ( KSVPHoldInStateIndex == current )
        {
        return ESVPLocalDoubleHold;
        }

    else
        {
        SVPDEBUG2( "CSVPHoldContext::RequestToHold - error(ToHold), %i",
                    current );

        return ESVPNoType;
        }    
    }
    
    
// ---------------------------------------------------------------------------
// CSVPHoldContext::RequestToResume
// Solves request type for local resume
// ---------------------------------------------------------------------------
//
TSVPHoldRequestType CSVPHoldContext::RequestToResume()
    {
    SVPDEBUG1( "CSVPHoldContext::RequestToResume" );
    
    TSVPHoldStateIndex current = CurrentState();
    iOutgoing = ETrue;
    iHoldEvent = MCCPCallObserver::ECCPLocalResume;
    if ( KSVPHoldOutStateIndex == current )
        {
        return ESVPLocalResume;
        }
        
    else if ( KSVPHoldDHStateIndex == current )
        {
        return ESVPLocalDoubleHoldResume;
        }
        
    else
        {
        SVPDEBUG2( "CSVPHoldContext::RequestToResume - error(ToResume), %i",
                    current );

        return ESVPNoType;
        }    
    }
    
    
// ---------------------------------------------------------------------------
// CSVPHoldContext::RequestIncoming
// Solves request type for incoming request
// ---------------------------------------------------------------------------
//
TSVPHoldRequestType CSVPHoldContext::RequestIncoming()
    {
    SVPDEBUG1( "CSVPHoldContext::RequestIncoming: Incoming" )

    // In this case return value stands for the most probable request:
    const TSVPHoldStateIndex current = CurrentState();
    SVPDEBUG2( "CSVPHoldContext::RequestIncoming - current = %i",
                current );    
    
    iOutgoing = EFalse;
    if ( KSVPHoldConnectedStateIndex == current )
        {
        iHoldEvent = MCCPCallObserver::ECCPRemoteHold;
        return ESVPRemoteHold;
        }
        
    else if ( KSVPHoldInStateIndex == current )
        {
        iHoldEvent = MCCPCallObserver::ECCPRemoteResume;
        return ESVPRemoteResume;
        }

    else if ( KSVPHoldOutStateIndex == current )
        {
        iHoldEvent = MCCPCallObserver::ECCPRemoteHold;
        return ESVPRemoteDoubleHold;
        }

    else if ( KSVPHoldDHStateIndex == current )
        {
        iHoldEvent = MCCPCallObserver::ECCPRemoteResume;
        return ESVPRemoteDoubleHoldResume;
        }
        
    else
        {
        SVPDEBUG2( "CSVPHoldContext::RequestIncoming - error(Incoming), %i",
                    current );

        return ESVPNoType;
        }    
    }
    

// ---------------------------------------------------------------------------
// CSVPHoldContext::UpdateEstablishingStateL
// Updates correct establishing state to state array
// ---------------------------------------------------------------------------
//
void CSVPHoldContext::UpdateEstablishingStateL()
    {
    TBool outEstablishingActive =
            ( *iStates )[ KSVPHoldEstablishingStateIndex ]->
                                IsOutEstablishingStateActive();
    
    if ( iOutgoing && !outEstablishingActive )
        {
        // OutEstablishingState is needed to state array
        delete ( *iStates )[ KSVPHoldEstablishingStateIndex ];
        iStates->Remove( KSVPHoldEstablishingStateIndex );

        CSVPHoldOutEstablishingState* outEstState =
                        CSVPHoldOutEstablishingState::NewLC();
                        
        User::LeaveIfError( iStates->Insert( outEstState,
                                             KSVPHoldEstablishingStateIndex ) );
        CleanupStack::Pop( outEstState );

        SVPDEBUG1( "CSVPHoldContext::UpdateEstablishingStateL - Outgoing" );
        }
        
    else if ( !iOutgoing && outEstablishingActive )
        {
        // InEstablishingState is needed to state array
        delete ( *iStates )[ KSVPHoldEstablishingStateIndex ];
        iStates->Remove( KSVPHoldEstablishingStateIndex );

        CSVPHoldInEstablishingState* inEstState =
                        CSVPHoldInEstablishingState::NewLC();
                        
        User::LeaveIfError( iStates->Insert( inEstState,
                                             KSVPHoldEstablishingStateIndex ) );
        CleanupStack::Pop( inEstState );

        SVPDEBUG1( "CSVPHoldContext::UpdateEstablishingStateL - Incoming" );
        }
        
    else
        {
        SVPDEBUG1( "CSVPHoldContext::UpdateEstablishingStateL - No update needed" );
        }
    }


// ---------------------------------------------------------------------------
// CSVPHoldContext::SetResponseStatusCode
// Sets response status code
// ---------------------------------------------------------------------------
//
void CSVPHoldContext::SetResponseStatusCode( TInt aStatusCode )
    {
    iResponseStatusCode = aStatusCode;
    }
    
    
// ---------------------------------------------------------------------------
// CSVPHoldContext::ResponseStatusCode
// Gets response status code
// ---------------------------------------------------------------------------
//
TInt CSVPHoldContext::ResponseStatusCode()
    {
    return iResponseStatusCode;
    }


// ---------------------------------------------------------------------------
// CSVPHoldContext::SetForceHold
// Sets force hold flag value
// ---------------------------------------------------------------------------
//
void CSVPHoldContext::SetForceHold( TBool aForceHold )
    {
    iForceHoldOccured = aForceHold;
    }
 
    
// ---------------------------------------------------------------------------
// CSVPHoldContext::ForceHoldOccured
// Gets force hold flag value
// ---------------------------------------------------------------------------
//
TBool CSVPHoldContext::ForceHoldOccured()
    {
    return iForceHoldOccured;
    }


// ---------------------------------------------------------------------------
// CSVPHoldContext::SpecialResponseHandling
// Sets force hold flag value 
// ---------------------------------------------------------------------------
//
TBool CSVPHoldContext::
SpecialResponseHandling( TSVPHoldStateIndex& aNextState )
    {
    SVPDEBUG2( "CSVPHoldContext::SpecialResponseHandling - Code is %d ", 
                ResponseStatusCode() );

    if ( KErrNotFound == ResponseStatusCode() && !iTimedOut )
        {
        SVPDEBUG1( "CSVPHoldContext::SpecialResponseHandling - Not needed" );
        return EFalse;
        }
        
    if ( iTimedOut )
        {
        // No response to request received
        SVPDEBUG1( "CSVPHoldContext::SpecialResponseHandling - Timed Out" );

        RollBack( aNextState );
        iTimedOut = EFalse;
        return ETrue;
        }
    
    if ( KSVPOKVal == ResponseStatusCode() && iCallRequest )
        {
        SVPDEBUG1( "CSVPHoldContext::SpecialResponseHandling - Forced" );
        RollBack( aNextState );
        iTimedOut = EFalse;
        return ETrue;
        }
        
    else if ( KSVPBadRequestVal <= ResponseStatusCode() )
        {
        // Need to roll back to previous state and inform client
        SVPDEBUG1( "CSVPHoldContext::SpecialResponseHandling - Request failure" );
        RollBack( aNextState );
        iTimedOut = EFalse;
        return ETrue;
        }
    else
        {
        SVPDEBUG1( "CSVPHoldContext::SpecialResponseHandling - N/A" );
        }
        
    return ETrue;   
    }

// ---------------------------------------------------------------------------
// CSVPHoldContext::TimedOut
// Sets force hold flag value
// ---------------------------------------------------------------------------
//
void CSVPHoldContext::TimedOut()
    {
    iTimedOut = ETrue;
    }

// ---------------------------------------------------------------------------
// CSVPHoldContext::HoldRolledBack
// Sets force hold flag value
// ---------------------------------------------------------------------------
//
TBool CSVPHoldContext::HoldRolledBack()
    {
    return iRollBack;
    }

// ---------------------------------------------------------------------------
// CSVPHoldContext::CrossOver
// Sets crossover situation on/off
// ---------------------------------------------------------------------------
//
void CSVPHoldContext::CrossOver( TBool aActive )
    {
    iCrossOver = aActive;
    }

// ---------------------------------------------------------------------------
// CSVPHoldContext::ResumeFailed
// Returns resume failed flag value
// ---------------------------------------------------------------------------
//
TBool CSVPHoldContext::ResumeFailed()
    {
    return iResumeFail;
    }

// ---------------------------------------------------------------------------
// CSVPHoldContext::HoldFailed
// Returns hold failed flag value
// ---------------------------------------------------------------------------
//
TBool CSVPHoldContext::HoldFailed()
    {
    if ( iHoldFail )
        {
        iHoldFail = EFalse;
        return ETrue;        
        }
    else
        {
        return EFalse;
        }
    }

// ---------------------------------------------------------------------------
// CSVPHoldContext::IsMobileOriginated
// Returns IsMobileOriginated -flag value
// ---------------------------------------------------------------------------
//
TBool CSVPHoldContext::IsMobileOriginated()
    {
    return iIsMobileOriginated;
    }
    
// ---------------------------------------------------------------------------
// CSVPHoldContext::Muted
// ---------------------------------------------------------------------------
//
void CSVPHoldContext::Muted( TBool aMuted )
    {
    iMuted = aMuted;
    }
    
// ---------------------------------------------------------------------------
// CSVPHoldContext::Muted
// ---------------------------------------------------------------------------
//
TBool CSVPHoldContext::Muted()
    {
    return iMuted;
    }    
    
// ---------------------------------------------------------------------------
// CSVPHoldContext::SetCallRequestFailed
// ---------------------------------------------------------------------------
//
void CSVPHoldContext::SetCallRequestFailed( TBool aCallRequest )
    {
    iCallRequest = aCallRequest;
    }

// ---------------------------------------------------------------------------
// CSVPHoldContext::CallRequestFailed
// ---------------------------------------------------------------------------
//
TBool CSVPHoldContext::CallRequestFailed()
    {
    return iCallRequest;
    }

// ---------------------------------------------------------------------------
// CSVPHoldContext::FallBack
// Rolls hold session state to its previous state
// ---------------------------------------------------------------------------
//
void CSVPHoldContext::RollBack( TSVPHoldStateIndex& aNextState )
    {
    SVPDEBUG1( "CSVPHoldContext::RollBack" );
    iRollBack = ETrue;
    switch ( HoldRequest() )
        {
        // in local hold request fail, not inform client until MCE session 
        // state is back to connected state.
        case ESVPLocalHold:
            {
            SVPDEBUG1( "CSVPHoldContext - Local Hold failed" );
            aNextState = KSVPHoldConnectedStateIndex;
            iHoldFail = ETrue;
            if ( !iCrossOver )
                {
                HoldObserver().HoldRequestFailed();
                }
            break;
            }
        
        case ESVPLocalDoubleHold:
            {
            SVPDEBUG1( "CSVPHoldContext - Local DoubleHold failed" );
            aNextState = KSVPHoldInStateIndex;
            iHoldFail = ETrue;
            if ( !iCrossOver )
                {
                HoldObserver().HoldRequestFailed();
                }
            break;
            }
        
        // Resume request fail can be informed immediately to client because
        // session will be terminated.
        case ESVPLocalResume:
            {
            SVPDEBUG1( "CSVPHoldContext - Local Resume failed" );
            iResumeFail = ETrue;
            aNextState = KSVPHoldOutStateIndex;
            if ( !iCrossOver )
                {
                HoldObserver().ResumeRequestFailed();
                }
            break;
            }
        
        case ESVPLocalDoubleHoldResume:
            {
            SVPDEBUG1( "CSVPHoldContext - Local DoubleHold Resume failed" );
            iResumeFail = ETrue;
            aNextState = KSVPHoldDHStateIndex;
            if ( !iCrossOver )
                {
                HoldObserver().ResumeRequestFailed();
                }
            break;
            }
        
        case ESVPRemoteResume:
            {
            SVPDEBUG1( "CSVPHoldContext - Remote Resume failed" );
            aNextState = KSVPHoldInStateIndex;
            break;
            }
            
        default:
            {
            SVPDEBUG1( "CSVPHoldContext - Local DoubleHold Resume failed" );
            break;
            }
        }    
    }




//  End of File  
