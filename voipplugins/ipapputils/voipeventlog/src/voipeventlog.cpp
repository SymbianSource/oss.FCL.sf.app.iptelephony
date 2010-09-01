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
* Description: 
*
*/

#include "voipeventlogengine.h"
#include "voipeventlogconstants.h"
#include "voiperrorentry.h"
#include "voipeventloglogger.h" // For logging
#include "voipeventlog.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CVoipEventLog::CVoipEventLog()
    {
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
void CVoipEventLog::ConstructL()
    {
    VELLOGSTRING( "CVoipEventLog::ConstructL() - IN" );

    iEngine = CVoipEventLogEngine::NewL();

    VELLOGSTRING( "CVoipEventLog::ConstructL() - OUT" );
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CVoipEventLog* CVoipEventLog::NewL()
    {
    CVoipEventLog* self = CVoipEventLog::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CVoipEventLog* CVoipEventLog::NewLC()
    {
    CVoipEventLog* self = new( ELeave ) CVoipEventLog;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CVoipEventLog::~CVoipEventLog()
    {
    VELLOGSTRING( "CVoipEventLog::~CVoipEventLog() - IN" );

    delete iEngine;

    VELLOGSTRING( "CVoipEventLog::~CVoipEventLog() - OUT" );
    }


// ---------------------------------------------------------------------------
// Writes an error data to event log
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CVoipEventLog::WriteError( const CVoipErrorEntry& aErrorEntry )
    {
    TRAPD( err, DoWriteErrorL( aErrorEntry ) );

    return err;
    }


// ---------------------------------------------------------------------------
// Writes an error code to event log
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CVoipEventLog::WriteError( TInt aErrorCode )
    {
    TRAPD( err, DoWriteErrorL( aErrorCode ) );

    return err;
    }

// ---------------------------------------------------------------------------
// Reads error count from the log
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CVoipEventLog::ErrorCount() const
    {
    TInt count( 0 );
    TRAPD( err, iEngine->ErrorCountL( count ) );

    if( err == KErrNone )
        {
        return count;
        }
    else
        {
        return err;
        }
    }

// ---------------------------------------------------------------------------
// Reads an error informaiton from the log
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CVoipEventLog::ReadError( TInt aIndex, CVoipErrorEntry& aErrorEntry ) const
    {
    TRAPD( err, iEngine->ReadErrorL( aIndex, aErrorEntry); );

    return err;
    }

// ---------------------------------------------------------------------------
// Reset log history. After this method has called, all voip errors are cleared
// from central repository.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CVoipEventLog::ResetLog ()
    {
    VELLOGSTRING( "CVoipEventLog::ResetLog - IN" );
    TInt err = iEngine->ResetLogHistory ();
    return err;
    }

// ---------------------------------------------------------------------------
// Writes an error data to event log
// ---------------------------------------------------------------------------
//
void CVoipEventLog::DoWriteErrorL( const CVoipErrorEntry& aErrorEntry )
    {
    iEngine->BeginTransactionLC();

    iEngine->WriteErrorL( aErrorEntry );

    iEngine->CommitTransactionL();
    }

// ---------------------------------------------------------------------------
// Writes an error code to event log
// ---------------------------------------------------------------------------
//
void CVoipEventLog::DoWriteErrorL( TInt aErrorCode )
    {
    CVoipErrorEntry* entry = CVoipErrorEntry::NewLC();
    entry->SetErrorCode( aErrorCode );

    DoWriteErrorL( *entry );

    CleanupStack::PopAndDestroy( entry );
    }

