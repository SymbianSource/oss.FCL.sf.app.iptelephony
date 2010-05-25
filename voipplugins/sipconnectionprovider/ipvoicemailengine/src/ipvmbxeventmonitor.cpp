/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Monitors Events from VoiceMailBox -server
*
*/



#include <mcetransactiondatacontainer.h>
#include <mceevent.h>
#include <siperr.h>

#include "ipvmbxeventmonitor.h"
#include "ipvmbxengine.h"
#include "ipvmbxlogger.h"
#include "ipvmbxpanic.h"
#include "ipvmbxbase.h"
#include "ipvmbxconstants.h"


// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
TIpVmbxEventMonitor::TIpVmbxEventMonitor( CIpVmbxEngine& aEngine ) :
    iEngine( aEngine )
    {
    }


// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
void TIpVmbxEventMonitor::EventStateChanged(
    CMceEvent& aEvent,
    TMceTransactionDataContainer* /*aContainer*/ )
    {
    IPVMEPRINT2( 
        "CIpVmbxEventMonitor::EventStateChanged: %d", 
        aEvent.State() );
    
    CIpVmbxBase* subscription( 
        iEngine.SubscriptionByRecipient( aEvent.Recipient() ) );
    if ( subscription )
        {
        CIpVmbxEngine::TIpVmbxMessages message = 
            CIpVmbxEngine::EEngineUndefined;
        
        switch ( aEvent.State() )
            {
            case CMceEvent::ETerminated:
                {
                if ( CIpVmbxBase::ETerminating == subscription->State() )
                    {
                    IPVMEPRINT( 
                        "CIpVmbxEventMonitor state: CMceEvent::ETerminated" );
                    // Normal unsubscription case
                    message = CIpVmbxEngine::EEngineTerminated;
                    }
                else 
                    if ( CIpVmbxBase::ERegistered == subscription->State() )
                        {
                        // Network stops responding
                        message = CIpVmbxEngine::EEngineNetworkLost;
                        }
                    else
                        {
                        // Network does not accept subscription
                        message = CIpVmbxEngine::EEngineSubscribeRejected;
                        }
                break;
                }
            case CMceEvent::EPending:
                // Occures on rare 
                message = CIpVmbxEngine::EEngineSubscribeRejected;
                break;
            case CMceEvent::EActive:
            case CMceEvent::EIdle:
                {
                // This should never happen
                message = CIpVmbxEngine::EEngineUndefined;
                break;
                }                
            default:
                IPVMEPRINT( "Unhandled message!" );
            }
    
        iEngine.HandleMessage( subscription->ServiceProviderId(), message );
        }
    }


// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
void TIpVmbxEventMonitor::NotifyReceived(
    CMceEvent& aEvent,
    TMceTransactionDataContainer* /*aContainer*/ )
    {
    IPVMEPRINT( "CIpVmbxEventMonitor::NotifyReceived - IN" );
    CMceEvent::TState eventState( CMceEvent::EIdle );
    eventState = aEvent.State();
    TBool firstNotify = EFalse;

    CIpVmbxBase* subscription( 
        iEngine.SubscriptionByRecipient( aEvent.Recipient() ) );
    if ( subscription )
        {
        CIpVmbxEngine::TIpVmbxMessages message = 
            CIpVmbxEngine::EEngineUndefined;
        
        switch ( eventState )
            {
            case CMceEvent::EActive:
                {
                IPVMEPRINT( 
                    "CIpVmbxEventMonitor state: CMceEvent::EActive" );
                if ( CIpVmbxBase::ETerminating != subscription->State() && 
                    CIpVmbxBase::ERegistered != subscription->State() )
                    {
                    
                    subscription->SetStateRegistered();
                    IPVMEPRINT( "CIpVmbxEventMonitor::NotifyReceived - firstNotify" );
                    firstNotify = ETrue;
                    }
    
                TRAPD( err, iEngine.EventReceivedL( aEvent.Recipient() ) );
                if ( err )
                    {
                    if ( KErrNoMemory == err )
                        {
                        message = CIpVmbxEngine::EEngineSmsOom;
                        }
                    else
                        {
                        message = CIpVmbxEngine::EEngineSmsError;
                        }
                    }
                else if ( firstNotify )
                    {
                    message = CIpVmbxEngine::EEngineSubscribed;
                    }
                else
                    {
                    message = CIpVmbxEngine::EEngineUndefined;
                    }
                }
                break;
            case CMceEvent::ETerminated:
            // flow through
            case CMceEvent::EIdle:
            // flow through
            case CMceEvent::EPending:
            // flow through
            default:
                IPVMEPRINT( "Unhandled message!" );
            }
        
        iEngine.HandleMessage( subscription->ServiceProviderId(), message );
        }
    }


// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
void TIpVmbxEventMonitor::EventConnectionStateChanged( 
                    CMceEvent& /*aEvent*/, 
                    TBool /*aActive*/ )
    {
    IPVMEPRINT( "CIpVmbxEventMonitor::EventConnectionStateChanged" );
    }


// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
void TIpVmbxEventMonitor::Failed( CMceEvent& aEvent, TInt aError )
    {
    IPVMEPRINT2( "CIpVmbxEventMonitor::Failed: %d", aError );
    CIpVmbxBase* subscription( 
        iEngine.SubscriptionByRecipient( aEvent.Recipient() ) );
    if ( subscription )
        {
        CIpVmbxEngine::TIpVmbxMessages message = 
            CIpVmbxEngine::EEngineUndefined;
        
        switch ( aError )
            {
            case KErrSIPTransportFailure:
                message = CIpVmbxEngine::EEngineIncorrectAccount;
                break;
            case KErrTimedOut:
                message = CIpVmbxEngine::EEngineSubscribeRejected;
                break;
            case KErrSIPResolvingFailure:
                message = CIpVmbxEngine::EEngineFatalNetworkError;
                break;              
            default:
                message = CIpVmbxEngine::EEngineNetworkError;
            }
    
        iEngine.HandleMessage( subscription->ServiceProviderId(), message );
        }
    }
