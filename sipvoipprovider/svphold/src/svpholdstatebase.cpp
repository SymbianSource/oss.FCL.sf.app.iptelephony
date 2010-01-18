/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Base class for hold state machine state
*
*/


// INCLUDE FILES
#include    "svpholdstatebase.h"
#include    "svpholdcontext.h" 

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSVPHoldStateBase::CSVPHoldStateBase
// ---------------------------------------------------------------------------
CSVPHoldStateBase::CSVPHoldStateBase()
    {
    }

// ---------------------------------------------------------------------------
// CSVPHoldStateBase::~CSVPHoldStateBase
// -----------------------------------------------------------------------------   
CSVPHoldStateBase::~CSVPHoldStateBase()
    {
    }

// ---------------------------------------------------------------------------
// CSVPHoldStateBase::ApplyL
// ---------------------------------------------------------------------------
//
void CSVPHoldStateBase::ApplyL( CSVPHoldContext& aContext )
    {
    DoApplyL( aContext );
    }

// ---------------------------------------------------------------------------
// CSVPHoldStateBase::Enter
// ---------------------------------------------------------------------------
//
void CSVPHoldStateBase::Enter( CSVPHoldContext& aContext )
    {
    DoEnter( aContext );
    }

// ---------------------------------------------------------------------------
// CSVPHoldStateBase::IsOutEstablishingStateActive
// ---------------------------------------------------------------------------
//
TBool CSVPHoldStateBase::IsOutEstablishingStateActive()
    {
    return EFalse;
    }
