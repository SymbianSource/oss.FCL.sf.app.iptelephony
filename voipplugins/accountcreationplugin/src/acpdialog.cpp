/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implements CAcpDialog methods
*
*/


#include <avkon.rsg>
#include <StringLoader.h>
#include <AknGlobalNote.h>
#include <AknQueryDialog.h> 
#include <aknnotewrappers.h>
#include <AknGlobalMsgQuery.h> 
#include <accountcreationplugin.rsg>

#include "acpdialog.h"
#include "accountcreationpluginlogger.h"
#include "macpdialogobserver.h"


// ---------------------------------------------------------------------------
// CAcpDialog::CAcpDialog
// ---------------------------------------------------------------------------
//
CAcpDialog::CAcpDialog( MAcpDialogObserver& aObserver )
    : CActive ( EPriorityStandard ),
    iObserver( aObserver )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CAcpDialog::NewL
// ---------------------------------------------------------------------------
//
CAcpDialog* CAcpDialog::NewL( MAcpDialogObserver& aObserver )
    {
    CAcpDialog* self = CAcpDialog::NewLC( aObserver );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CAcpDialog::NewLC
// ---------------------------------------------------------------------------
//
CAcpDialog* CAcpDialog::NewLC( MAcpDialogObserver& aObserver )
    {
    CAcpDialog* self = new ( ELeave ) CAcpDialog( aObserver );
    CleanupStack::PushL( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CAcpDialog::~CAcpDialog
// ---------------------------------------------------------------------------
//
CAcpDialog::~CAcpDialog()
    {
    Cancel();
    }

// ---------------------------------------------------------------------------
// CAcpDialog::LaunchWaitNoteL
// Launches a wait note.
// ---------------------------------------------------------------------------
//
void CAcpDialog::LaunchWaitNoteL( TInt aWaitNote )
    {
    ACPLOG( "CAcpDialog::LaunchWaitNoteL begin" );

    if ( !IsActive() )
        {
        // Show wait note with appropriate text.
        HBufC* text = StringLoader::LoadLC( aWaitNote );
        iWaitNote = CAknGlobalNote::NewL();
        iWaitNote->SetSoftkeys( R_AVKON_SOFTKEYS_CANCEL );
        iNoteId = iWaitNote->ShowNoteL( 
            iStatus, 
            EAknGlobalWaitNote, 
            *text );
        CleanupStack::PopAndDestroy( text );
        ACPLOG( " - SetActive();" );
        SetActive();
        }
     ACPLOG( "CAcpDialog::LaunchWaitNoteL end" );
     }

// ---------------------------------------------------------------------------
// CAcpDialog::LaunchWaitNoteL
// Launches a wait note with a custom string.
// ---------------------------------------------------------------------------
//
void CAcpDialog::LaunchWaitNoteL( TInt aWaitNote, const TDesC& aString )
    {
    ACPLOG( "CAcpDialog::LaunchWaitNoteL begin" );

    if ( !IsActive() )
        {
        // Show wait note with appropriate text.
        HBufC* text = StringLoader::LoadLC( aWaitNote, aString );
        iWaitNote = CAknGlobalNote::NewL();
        iWaitNote->SetSoftkeys( R_AVKON_SOFTKEYS_CANCEL );
        iNoteId = iWaitNote->ShowNoteL( 
            iStatus, 
            EAknGlobalWaitNote, 
            *text );
        CleanupStack::PopAndDestroy( text );
        ACPLOG( " - SetActive();" );
        SetActive();
        }
     ACPLOG( "CAcpDialog::LaunchWaitNoteL end" );
     }

// ---------------------------------------------------------------------------
// CAcpDialog::DestroyWaitNote
// Destroys a wait note.
// ---------------------------------------------------------------------------
//
void CAcpDialog::DestroyWaitNote()
    {
    ACPLOG( "CAcpDialog::DestroyWaitNote begin" );
    Cancel();
    
    // If dialog handler wasn't active anymore, DoCancel wasn't called and
    // the wait note needs to be deleted separately.
    if ( iWaitNote )
        {
        TRAP_IGNORE( iWaitNote->CancelNoteL( iNoteId ) );
        delete iWaitNote;
        iWaitNote = NULL;
        }
    
    ACPLOG( "CAcpDialog::DestroyWaitNoteL end" );
    }

// ---------------------------------------------------------------------------
// CAcpDialog::ShowQueryL
// ---------------------------------------------------------------------------
//
TInt CAcpDialog::ShowQueryL( TInt aQuery )
    {
    ACPLOG( "CAcpDialog::ShowQueryL begin" );
    CAknQueryDialog* query = new ( ELeave ) 
        CAknQueryDialog( CAknQueryDialog::ENoTone );
    ACPLOG( " - return ( query->ExecuteLD( aQuery ) );" );
    ACPLOG( "CAcpDialog::ShowQueryL end" );
    return ( query->ExecuteLD( aQuery ) );
    }

// ---------------------------------------------------------------------------
// CAcpDialog::ShowMessageQueryL
// ---------------------------------------------------------------------------
//
void CAcpDialog::ShowMessageQueryL( TInt aMessageQuery, const TDesC& aString )
    {
    ACPLOG( "CAcpDialog::ShowMessageQueryL begin" );
    
    if ( !IsActive() )
        {
        HBufC* text = StringLoader::LoadLC( aMessageQuery, aString );
        iGlobalMsgQuery = CAknGlobalMsgQuery::NewL();
        
        ACPLOG( "   --> show query" );
 
        iGlobalMsgQuery->ShowMsgQueryL( 
           iStatus, 
           text->Des(), 
           R_AVKON_SOFTKEYS_OK_EMPTY,
           KNullDesC, 
           KNullDesC ); 

        CleanupStack::PopAndDestroy( text );
        SetActive();
        }
    
    ACPLOG( "CAcpDialog::ShowMessageQueryL end" );
    }

// ---------------------------------------------------------------------------
// CAcpDialog::ShowNoteL
// ---------------------------------------------------------------------------
//
void CAcpDialog::ShowNoteL( TInt aNote, const TDesC& aString )
    {
    ACPLOG( "CAcpDialog::ShowNoteL(string) begin" );
    HBufC* text = StringLoader::LoadLC( aNote, aString );
    CAknInformationNote* note = new (ELeave) CAknInformationNote( ETrue );
    note->ExecuteLD( *text );
    CleanupStack::PopAndDestroy( text );
    ACPLOG( "CAcpDialog::ShowNoteL(string) end" );
    }

// ---------------------------------------------------------------------------
// CAcpDialog::ShowGlobalNoteL
// ---------------------------------------------------------------------------
//
void CAcpDialog::ShowGlobalNoteL( TInt aNote )
    {
    ACPLOG( "CAcpDialog::ShowGlobalNoteL begin" );    
    HBufC* text = StringLoader::LoadLC( aNote );
    CAknGlobalNote* note = CAknGlobalNote::NewLC();
    note->SetSoftkeys( R_AVKON_SOFTKEYS_OK_EMPTY );
    note->ShowNoteL( EAknGlobalInformationNote, *text );
    CleanupStack::PopAndDestroy( 2, text );    
    ACPLOG( "CAcpDialog::ShowGlobalNoteL end" );
    }

// ---------------------------------------------------------------------------
// CAcpDialog::ShowGlobalNoteL
// ---------------------------------------------------------------------------
//
void CAcpDialog::ShowGlobalNoteL( TInt aNote, const TDesC& aString )
    {
    ACPLOG( "CAcpDialog::ShowGlobalNoteL(string) begin" );
    HBufC* text = StringLoader::LoadLC( aNote, aString );
    CAknGlobalNote* note = CAknGlobalNote::NewLC();
    note->SetSoftkeys( R_AVKON_SOFTKEYS_OK_EMPTY );
    note->ShowNoteL( EAknGlobalInformationNote, *text );
    CleanupStack::PopAndDestroy( 2, text );
    ACPLOG( "CAcpDialog::ShowGlobalNoteL(string) end" );
    }

// ---------------------------------------------------------------------------
// CAcpDialog::ShowSelectionDialogL
// ---------------------------------------------------------------------------
//
TInt CAcpDialog::ShowSelectionDialogL( TInt aDialog, MDesC16Array* aArray )
    {
    ACPLOG( "CAcpDialog::ShowSelectionDialogL begin" );
    TInt index( KErrNotFound );
    CAknListQueryDialog* dialog = new (ELeave) CAknListQueryDialog( &index );
    dialog->PrepareLC( aDialog );
    dialog->SetItemTextArray( aArray );
    dialog->SetOwnershipType( ELbmOwnsItemArray );
    dialog->RunLD();
    
    ACPLOG( "CAcpDialog::ShowSelectionDialogL end" );
    return index;
    }

// ---------------------------------------------------------------------------
// CAcpDialog::RunL
// From CActive.
// ---------------------------------------------------------------------------
//
void CAcpDialog::RunL()
    {
    ACPLOG2( "CAcpDialog::RunL: iStatus=%d", iStatus.Int() );
    
    TInt status = iStatus.Int();
    
    if ( iWaitNote )
        {
        ACPLOG( "CAcpDialog::RunL --> wait note" );
        
        iObserver.DialogDismissedL( iStatus.Int() );
        }
    else if ( iGlobalMsgQuery && 
        ( status == EAknSoftkeyYes || status == EAknSoftkeyOk ) )
        {
        ACPLOG( "CAcpDialog::RunL --> message query" );
        
        iGlobalMsgQuery->CancelMsgQuery();
        iObserver.DialogDismissedL( iStatus.Int() );
        }
    else if ( iGlobalMsgQuery && 
        ( status != EAknSoftkeyYes || status != EAknSoftkeyOk ) )
        {
        ACPLOG( "CAcpDialog::RunL --> waiting user to check message query" );
        
        // do nothing, wait user to press ok to message query.
        }
    else
        {        
        ACPLOG( "CAcpDialog::RunL --> cancel" );
        
        // in other cases cancel
        Cancel();
        }
    }

// ---------------------------------------------------------------------------
// CAcpDialog::DoCancel
// From CActive.
// ---------------------------------------------------------------------------
//
void CAcpDialog::DoCancel()
    {
    ACPLOG( "CAcpDialog::DoCancel - begin" );

    if ( iWaitNote )
        {
        TRAP_IGNORE( iWaitNote->CancelNoteL( iNoteId ) );
        delete iWaitNote;
        iWaitNote = NULL;
        }
    
    if ( iGlobalMsgQuery )
        {
        iGlobalMsgQuery->CancelMsgQuery();
        delete iGlobalMsgQuery;
        iGlobalMsgQuery = NULL;
        }
      
    ACPLOG( "CAcpDialog::DoCancel - end" );
    }

// End of file.
