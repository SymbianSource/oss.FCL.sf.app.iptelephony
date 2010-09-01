/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of class CCCHUiNoteHandler.
*
*/


#include <avkon.rsg>
#include <AknGlobalNote.h>
#include <AknQueryDialog.h>
#include <AknGlobalMsgQuery.h> 
#include <cenrepnotifyhandler.h>

#include "cchuilogger.h"
#include "cchuinotehandler.h"
#include "cchuiglobalqueryhandler.h"
#include "mcchuinoteresultobserver.h"

// ======== MEMBER FUNCTIONS ========

CCCHUiNoteHandler::CCCHUiNoteHandler( 
    MCchUiNoteResultObserver& aObserver ) : 
    CActive ( EPriorityHigh ),
    iObserver( aObserver ),
    iCurrentNote( MCchUiObserver::ECchUiDialogTypeNotSet )
    {  
    }

void CCCHUiNoteHandler::ConstructL()
    {
    CCHUIDEBUG( "CCCHUiNoteHandler::ConstructL - IN" );
    
    CActiveScheduler::Add( this );
    iGlobalQueryHandler = CCchUiGlobalQueryHandler::NewL();
    
    CCHUIDEBUG( "CCCHUiNoteHandler::ConstructL - OUT" );
    }

CCCHUiNoteHandler* CCCHUiNoteHandler::NewL( 
    MCchUiNoteResultObserver& aObserver )
    {
    CCCHUiNoteHandler* self = 
        new( ELeave ) CCCHUiNoteHandler( aObserver );  
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CCCHUiNoteHandler::~CCCHUiNoteHandler()
    {
    CCHUIDEBUG( "CCCHUiNoteHandler::~CCCHUiNoteHandler - IN" );
    
    Cancel();
    delete iGlobalQueryHandler;
    iGlobalQueryHandler = NULL;
    
    CCHUIDEBUG( "CCCHUiNoteHandler::~CCCHUiNoteHandler - OUT" );
    }

// ---------------------------------------------------------------------------
// Launch note.
// ---------------------------------------------------------------------------
//
void CCCHUiNoteHandler::LaunchNoteL( 
    MCchUiObserver::TCchUiDialogType aNote, 
    TUint aServiceId,
    TUint aIapId,
    RBuf& aUserName )
    {
    CCHUIDEBUG( "CCCHUiNoteHandler::LaunchNoteL - IN");
    CCHUIDEBUG2( "LaunchNoteL - aNote: %d", aNote );
    CCHUIDEBUG2( "LaunchNoteL - aServiceId: %d", aServiceId );
    CCHUIDEBUG2( "LaunchNoteL - aIapId: %d", aIapId );
    CCHUIDEBUG2( "LaunchNoteL - aUserName: %S", &aUserName );
    CCHUIDEBUG2( "LaunchNoteL - IsActive: %d", IsActive() );
    
    // If already showing note => leave with KErrAlreadyExists to signal
    // client as API definition dictates.
    __ASSERT_ALWAYS( !IsActive(), User::Leave( KErrAlreadyExists ));
    switch ( aNote )
        {
        case MCchUiObserver::ECchUiDialogTypeUsernamePasswordFailed:
        case MCchUiObserver::ECchUiDialogTypeAuthenticationFailed:
        case MCchUiObserver::ECchUiDialogTypeNoConnectionAvailable:
        case MCchUiObserver::ECchUiDialogTypeNoConnectionDefined:      
        case MCchUiObserver::ECchUiDialogTypeConfirmChangeConnection:
        case MCchUiObserver::ECchUiDialogTypeChangeConnection:
        case MCchUiObserver::ECchUiDialogTypeDefectiveSettings:
        case MCchUiObserver::ECchUiDialogTypeErrorInConnection:
            {
            iCurrentNote = aNote;
            iGlobalQueryHandler->ShowMsgQueryL(
                    aNote, iStatus, aServiceId, aIapId, aUserName );
            SetActive();
            }
            break;        
        
        default:
            {
            CCHUIDEBUG( "LaunchNoteL - default switch case" );
            User::Leave( KErrNotSupported );
            }
            break;        
        }
    CCHUIDEBUG( "CCCHUiNoteHandler::LaunchNoteL - OUT" );     
    }

// ---------------------------------------------------------------------------
// Active dialog check. By dialog we mean dialog which needs user action.
// ---------------------------------------------------------------------------
//
TBool CCCHUiNoteHandler::ActiveDialogExists()
    {
    CCHUIDEBUG( "CCCHUiNoteHandler::ActiveDialogExists" );
    
    switch ( iCurrentNote )
        {
        case MCchUiObserver::ECchUiDialogTypeAuthenticationFailed:
        case MCchUiObserver::ECchUiDialogTypeNoConnectionAvailable:
        case MCchUiObserver::ECchUiDialogTypeNoConnectionDefined:     
        case MCchUiObserver::ECchUiDialogTypeConfirmChangeConnection:
        case MCchUiObserver::ECchUiDialogTypeChangeConnection:
            {
            return ETrue;
            }
        case MCchUiObserver::ECchUiDialogTypeUsernamePasswordFailed:
        case MCchUiObserver::ECchUiDialogTypeDefectiveSettings:
        case MCchUiObserver::ECchUiDialogTypeErrorInConnection:    
        default:
             {
             return EFalse;
             }
        }
    }

// ---------------------------------------------------------------------------
// CancelOldNotes.
// ---------------------------------------------------------------------------
//
void CCCHUiNoteHandler::CancelOldNotes()
    {
    CCHUIDEBUG( "CCCHUiNoteHandler::CancelOldNotes - IN" );
    
    if ( iGlobalQueryHandler && IsActive() )
        {
        iGlobalQueryHandler->CancelMsgQuery();
        Cancel();
        }
    
    CCHUIDEBUG( "CCCHUiNoteHandler::CancelOldNotes - OUT" );
    }
  
// ---------------------------------------------------------------------------
// Returns current note.
// ---------------------------------------------------------------------------
//    
MCchUiObserver::TCchUiDialogType CCCHUiNoteHandler::CurrentNote()
    {
    return iCurrentNote;
    }

// ---------------------------------------------------------------------------
// From class CActive
// RunL.
// ---------------------------------------------------------------------------
//
void CCCHUiNoteHandler::RunL()
    {
    CCHUIDEBUG2( "CCCHUiNoteHandler::RunL, status: %d", iStatus.Int() );    
    
    TInt status = iStatus.Int();
    if ( iGlobalQueryHandler )
        {
        iCurrentNote = MCchUiObserver::ECchUiDialogTypeNotSet;
        
        TRAP_IGNORE( iObserver.DialogCompletedL( status, 
            iGlobalQueryHandler->ResultParams() ) );
        }
    }

// ---------------------------------------------------------------------------
// From class CActive
// DoCancel.
// ---------------------------------------------------------------------------
//
void CCCHUiNoteHandler::DoCancel()
    {
    CCHUIDEBUG( "CCCHUiNoteHandler::DoCancel - IN" ); 
    
    if ( iGlobalQueryHandler )
    	{
    	CCHUIDEBUG( "DoCancel - cancel message query");
    	iGlobalQueryHandler->CancelMsgQuery();
    	}
    
    CCHUIDEBUG( "CCCHUiNoteHandler::DoCancel - OUT" );  
    }

// ---------------------------------------------------------------------------
// From class CActive.
// RunError.
// ---------------------------------------------------------------------------
//
TInt CCCHUiNoteHandler::RunError( TInt /*aError*/ )
    {         
    return KErrNone;
    }

