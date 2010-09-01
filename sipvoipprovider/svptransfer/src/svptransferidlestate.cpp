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
* Description:  Transfer state representing the idle state
*
*/


#include "svptransferidlestate.h"
#include "svptransferstatecontext.h"
#include "svptransferobserver.h"                // MSVPTransferObserver
#include "svplogger.h"

// ---------------------------------------------------------------------------
// CSVPTransferIdleState::CSVPTransferIdleState
// ---------------------------------------------------------------------------
//
CSVPTransferIdleState::CSVPTransferIdleState()
    {
    }

// ---------------------------------------------------------------------------
// CSVPTransferIdleState::NewLC
// ---------------------------------------------------------------------------
//
CSVPTransferIdleState* CSVPTransferIdleState::NewLC()
    {
    CSVPTransferIdleState* self = new( ELeave ) CSVPTransferIdleState;
    CleanupStack::PushL( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CSVPTransferIdleState::~CSVPTransferIdleState
// ---------------------------------------------------------------------------
//
CSVPTransferIdleState::~CSVPTransferIdleState()
    {
    }    

// ---------------------------------------------------------------------------
// CSVPTransferIdleState::DoApplyL
// ---------------------------------------------------------------------------
//
void CSVPTransferIdleState::DoApplyL( CSVPTransferStateContext& aContext )
    {
    SVPDEBUG1( "CSVPTransferIdleState::DoApplyL In" )

    if ( KSVPTransferIdleStateIndex  == aContext.CurrentState() )         
        {          
        if ( !aContext.IsIncoming() )
            {                
            // Execute the refer, creates mce out event. 
            TRAPD( err, aContext.ExecuteReferL() );
                                                                  
            if ( KErrNone != err )
                {
                SVPDEBUG2( "CSVPTransferIdleState::DoApplyL: error: %i", err )
                // Notify the observer about the error.
                aContext.TransferObserver().TransferFailed( err );
                // Set and apply to the terminating state
                aContext.SetCurrentStateL( KSVPTransferTerminatingStateIndex );
                aContext.ApplyCurrentStateL();
                User::Leave( err );
                }
            }
        
        // Set and apply the next state - transfer pending
        aContext.SetCurrentStateL( KSVPTransferPendingStateIndex );
        aContext.ApplyCurrentStateL();
        }
    else
        {
        // Notify the observer about the error.
        aContext.TransferObserver().TransferFailed( KSVPErrTransferStateError );
        }
    
    SVPDEBUG1( "CSVPTransferIdleState::DoApplyL Out" )
    }

// ---------------------------------------------------------------------------
// CSVPTransferIdleState::DoEnter
// ---------------------------------------------------------------------------
//
void CSVPTransferIdleState::DoEnter( CSVPTransferStateContext& /*aContext*/ )
    {
    }

