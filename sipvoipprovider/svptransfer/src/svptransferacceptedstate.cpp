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
* Description:  Transfer state representing the accepted state
*
*/


#include "svptransferacceptedstate.h"
#include "svptransferstatecontext.h"            // CSVPTransferStateContext
#include "svptransferobserver.h"                // MSVPTransferObserver
#include "svplogger.h"


// ======== LOCAL FUNCTIONS ========


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSVPTransferAcceptedState::CSVPTransferAcceptedState
// ---------------------------------------------------------------------------
//
CSVPTransferAcceptedState::CSVPTransferAcceptedState()
    {
    }

// ---------------------------------------------------------------------------
// CSVPTransferAcceptedState::NewLC
// ---------------------------------------------------------------------------
//
CSVPTransferAcceptedState* CSVPTransferAcceptedState::NewLC()
    {
    CSVPTransferAcceptedState* self = new( ELeave ) CSVPTransferAcceptedState;
    CleanupStack::PushL( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CSVPTransferAcceptedState::~CSVPTransferAcceptedState
// ---------------------------------------------------------------------------
//
CSVPTransferAcceptedState::~CSVPTransferAcceptedState()
    {
    }

// ---------------------------------------------------------------------------
// CSVPTransferAcceptedState::DoApplyL
// ---------------------------------------------------------------------------
//
void CSVPTransferAcceptedState::DoApplyL( CSVPTransferStateContext& aContext )
    {
    SVPDEBUG1( "CSVPTransferAcceptedState::DoApplyL In" )
    
    if ( KSVPTransferAcceptedStateIndex == aContext.CurrentState() )
        {
        // No need to notify the observer.
        }
    else
        {
        // Notify the observer about the error
        aContext.TransferObserver().TransferFailed( KSVPErrTransferStateError );
        }
    
    SVPDEBUG1( "CSVPTransferAcceptedState::DoApplyL Out" )
    }

// ---------------------------------------------------------------------------
// CSVPTransferAcceptedState::DoEnter
// ---------------------------------------------------------------------------
//
void CSVPTransferAcceptedState::DoEnter( 
                                    CSVPTransferStateContext& /*aContext*/ )
    {
    SVPDEBUG1( "CSVPTransferAcceptedState::DoEnter" );
    }



