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
* Description:  Transfer state representing the terminating state
*
*/


#include "svptransferterminatingstate.h"
#include "svptransferstatecontext.h"
#include "svplogger.h"

// ---------------------------------------------------------------------------
// CSVPTransferTerminatingState::CSVPTransferTerminatingState
// ---------------------------------------------------------------------------
//
CSVPTransferTerminatingState::CSVPTransferTerminatingState()
    {
    }

// ---------------------------------------------------------------------------
// CSVPTransferTerminatingState::NewLC
// ---------------------------------------------------------------------------
//
CSVPTransferTerminatingState* CSVPTransferTerminatingState::NewLC()
    {
    CSVPTransferTerminatingState* self = 
        new( ELeave ) CSVPTransferTerminatingState;
    CleanupStack::PushL( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CSVPTransferTerminatingState::~CSVPTransferTerminatingState
// ---------------------------------------------------------------------------
//
CSVPTransferTerminatingState::~CSVPTransferTerminatingState()
    {
    }

// ---------------------------------------------------------------------------
// CSVPTransferTerminatingState::DoApplyL
// ---------------------------------------------------------------------------
//
void CSVPTransferTerminatingState::DoApplyL( 
                                    CSVPTransferStateContext& aContext )
    {
    SVPDEBUG1( "CSVPTransferTerminatingState::DoApplyL In" )
    
    // Set next state - terminated
    aContext.SetCurrentStateL( KSVPTransferTerminatedStateIndex );
    aContext.ApplyCurrentStateL();
    
    SVPDEBUG1( "CSVPTransferTerminatingState::DoApplyL Out" )
    }

// ---------------------------------------------------------------------------
// CSVPTransferTerminatingState::DoEnter
// ---------------------------------------------------------------------------
//
void CSVPTransferTerminatingState::DoEnter( 
                                    CSVPTransferStateContext& /*aContext*/ )
    {
    }

