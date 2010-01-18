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
* Description:  Transfer state representing the pending state
*
*/


#include "svptransferpendingstate.h"
#include "svptransferstatecontext.h"
#include "svptransferobserver.h"            // MSVPTransferObserver
#include "svplogger.h"

// ---------------------------------------------------------------------------
// CSVPTransferPendingState::CSVPTransferPendingState
// ---------------------------------------------------------------------------
//
CSVPTransferPendingState::CSVPTransferPendingState()
    {
    }

// ---------------------------------------------------------------------------
// CSVPTransferPendingState::NewLC
// ---------------------------------------------------------------------------
//
CSVPTransferPendingState* CSVPTransferPendingState::NewLC()
    {
    CSVPTransferPendingState* self = new( ELeave ) CSVPTransferPendingState;
    CleanupStack::PushL( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CSVPTransferPendingState::~CSVPTransferPendingState
// ---------------------------------------------------------------------------
//
CSVPTransferPendingState::~CSVPTransferPendingState()
    {
    }

// ---------------------------------------------------------------------------
// CSVPTransferPendingState::DoApplyL
// ---------------------------------------------------------------------------
//
void CSVPTransferPendingState::DoApplyL( CSVPTransferStateContext& aContext )
    {
    SVPDEBUG1( "CSVPTransferPendingState::DoApplyL In" )

    if ( aContext.CurrentState() == KSVPTransferPendingStateIndex )         
        {        
        if ( aContext.IsIncoming() )
            {
            // Incoming new refer received
            
            // Notify the observer
            aContext.TransferObserver().TransferNotification( 
                                                ESVPIncomingRefer );

            // Next state is defined & applied when user action 
            // event received in CSVPTransferController::AcceptTransfer
            
            }

        // For outgoing refer -
        // Next state is defined & applied when event received 
        // in CSVPTransferController::HandleReferStateChangeL
        }
    else
        {
        // Notify the observer about the error.
        aContext.TransferObserver().TransferFailed( KSVPErrTransferStateError );
        // Set and apply to the terminating state
        aContext.SetCurrentStateL( KSVPTransferTerminatingStateIndex );
        aContext.ApplyCurrentStateL();
        }
    
    SVPDEBUG1( "CSVPTransferPendingState::DoApplyL Out" )
    }

// ---------------------------------------------------------------------------
// CSVPTransferPendingState::DoEnter
// ---------------------------------------------------------------------------
//
void CSVPTransferPendingState::DoEnter( 
                                CSVPTransferStateContext& /*aContext*/ )
    {
    }

