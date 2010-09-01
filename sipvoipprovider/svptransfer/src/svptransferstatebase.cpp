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
* Description:  Transfer state base class for different states
*
*/


#include "svptransferstatebase.h"

// ---------------------------------------------------------------------------
// CSVPTransferStateBase::CSVPTransferStateBase
// ---------------------------------------------------------------------------
//
CSVPTransferStateBase::CSVPTransferStateBase()
    {
    }

// ---------------------------------------------------------------------------
// CSVPTransferStateBase::~CSVPTransferStateBase
// ---------------------------------------------------------------------------
//
CSVPTransferStateBase::~CSVPTransferStateBase()
    {
    }

// -----------------------------------------------------------------------------
// CSVPTransferStateBase::ApplyL
// -----------------------------------------------------------------------------
//
void CSVPTransferStateBase::ApplyL( CSVPTransferStateContext& aContext )
    {
    DoApplyL( aContext );
    }

// -----------------------------------------------------------------------------
// CSscSrvStateBase::Enter
// -----------------------------------------------------------------------------
//
void CSVPTransferStateBase::Enter( CSVPTransferStateContext& aContext )
    {
    DoEnter( aContext );
    }

