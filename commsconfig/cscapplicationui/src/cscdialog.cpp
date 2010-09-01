/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CSC Application´s Dialog
*
*/


#include <avkon.rsg>
#include <StringLoader.h>
#include <AknGlobalNote.h>

#include "cscdialog.h"
#include "csclogger.h"

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCDialog::CCSCDialog() : CActive ( EPriorityHigh )
    {
    }
    
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CCSCDialog::ConstructL()
    {
    CSCDEBUG( "CCSCDialog::ConstructL - begin" );
    
    CActiveScheduler::Add( this );
    
    CSCDEBUG( "CCSCDialog::ConstructL - end" );
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCDialog* CCSCDialog::NewL()
    {
    CCSCDialog* self = CCSCDialog::NewLC();
    CleanupStack::Pop( self );    
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCDialog* CCSCDialog::NewLC()
    {
    CCSCDialog* self = new ( ELeave ) CCSCDialog();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCDialog::~CCSCDialog()
    {
    Cancel();
    }


// -----------------------------------------------------------------------------
// CCSCDialog::LaunchWaitNoteL
// Launches a wait note.
// -----------------------------------------------------------------------------
//
void CCSCDialog::LaunchWaitNoteL( const TInt aResourceId )
    {
    CSCDEBUG( "CCSCDialog::LaunchWaitNoteL" );
    
    if ( !IsActive() )
        {
        // Show wait note with appropriate text.
        HBufC* text = StringLoader::LoadLC( aResourceId );
        iWaitNote = CAknGlobalNote::NewL();
        iWaitNote->SetSoftkeys( R_AVKON_SOFTKEYS_EMPTY );
        iNoteId = iWaitNote->ShowNoteL( 
            iStatus, 
            EAknGlobalWaitNote, 
            *text );
        CleanupStack::PopAndDestroy( text );
        SetActive();
        }
    }


// -----------------------------------------------------------------------------
// CCSCDialog::DestroyWaitNote
// Destroys a wait note.
// -----------------------------------------------------------------------------
//
void CCSCDialog::DestroyWaitNote()
    {
    Cancel();
    }


// ---------------------------------------------------------------------------
// From CActive
// CCSCDialog::RunL
// ---------------------------------------------------------------------------
//
void CCSCDialog::RunL()
    {
    CSCDEBUG( "CCSCDialog::RunL" );
    }
    
    
// ---------------------------------------------------------------------------
// From CActive
// CCSCDialog::DoCancel
// ---------------------------------------------------------------------------
//
void CCSCDialog::DoCancel()
    {
    if ( iWaitNote )
        {
        TRAP_IGNORE( iWaitNote->CancelNoteL( iNoteId ) );
        delete iWaitNote;
        iWaitNote = NULL;
        }
    }
    
