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
* Description:  Transfer state representing the terminated state
*
*/


#include "svptransferterminatedstate.h"
#include "svptransferstatecontext.h"
#include "svplogger.h"

// ---------------------------------------------------------------------------
// CSVPTransferTerminatedState::CSVPTransferTerminatedState
// ---------------------------------------------------------------------------
//
CSVPTransferTerminatedState::CSVPTransferTerminatedState()
    {
    }

// ---------------------------------------------------------------------------
// CSVPTransferTerminatedState::NewLC
// ---------------------------------------------------------------------------
//
CSVPTransferTerminatedState* CSVPTransferTerminatedState::NewLC()
    {
    CSVPTransferTerminatedState* self = new( ELeave ) 
                                        CSVPTransferTerminatedState;
    CleanupStack::PushL( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CSVPTransferTerminatedState::~CSVPTransferTerminatedState
// ---------------------------------------------------------------------------
//
CSVPTransferTerminatedState::~CSVPTransferTerminatedState()
    {
    }

// ---------------------------------------------------------------------------
// CSVPTransferTerminatedState::DoApplyL
// ---------------------------------------------------------------------------
//
void CSVPTransferTerminatedState::DoApplyL( 
                                    CSVPTransferStateContext& aContext )
    {
    SVPDEBUG1( "CSVPTransferTerminatedState::DoApplyL In" )
      
    // Clear incoming refer handling indication data.
    aContext.ResetIncomingReferTo();
    aContext.ResetIncomingReferredBy();
    aContext.ResetIncomingReplaces();
    
    // Clear refer and event data.
    aContext.SetMceEvent( NULL );
    aContext.SetMceRefer( NULL );
    
    // Set next state - idle. Do not apply yet.
    aContext.SetCurrentStateL( KSVPTransferIdleStateIndex );
    
    SVPDEBUG1( "CSVPTransferTerminatedState::DoApplyL Out" )
    }
  
// ---------------------------------------------------------------------------
// CSVPTransferTerminatedState::DoEnter
// ---------------------------------------------------------------------------
//
void CSVPTransferTerminatedState::DoEnter( 
                                    CSVPTransferStateContext& /*aContext*/ )
    {
    }
