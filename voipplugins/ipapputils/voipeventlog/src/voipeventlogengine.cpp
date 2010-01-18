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


#include <centralrepository.h>

#include "voipeventlogengine.h"
#include "voipeventlogconstants.h"
#include "voiperrorentry.h"
#include "voipeventloglogger.h" // For logging


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CVoipEventLogEngine::CVoipEventLogEngine()
    {
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
void CVoipEventLogEngine::ConstructL()
    {
    VELLOGSTRING( "CVoipEventLogEngine::ConstructL() - IN" );
    iRepository = CRepository::NewL( KCRUidVoIPEventLog );

    TInt err = iSemaphore.OpenGlobal( KVoIPEventLogSemaphore );
    if ( err != KErrNone )
        {
        User::LeaveIfError( iSemaphore.CreateGlobal( KVoIPEventLogSemaphore, 1 ) );
        }


    VELLOGSTRING( "CVoipEventLogEngine::ConstructL() - OUT" );
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CVoipEventLogEngine* CVoipEventLogEngine::NewL()
    {
    CVoipEventLogEngine* self = CVoipEventLogEngine::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CVoipEventLogEngine* CVoipEventLogEngine::NewLC()
    {
    CVoipEventLogEngine* self = new( ELeave ) CVoipEventLogEngine;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CVoipEventLogEngine::~CVoipEventLogEngine()
    {
    VELLOGSTRING( "CVoipEventLogEngine::~CVoipEventLogEngine() - IN" );

    delete iRepository;

    iSemaphore.Close();

    VELLOGSTRING( "CVoipEventLogEngine::~CVoipEventLogEngine() - OUT" );
    }

// ---------------------------------------------------------------------------
// release semaphore, this function is used in custom cleanup
// ---------------------------------------------------------------------------
//
void CVoipEventLogEngine::ReleaseSemaphore( TAny* aEngine )
    {
    ASSERT ( aEngine );
    CVoipEventLogEngine* engine = static_cast<CVoipEventLogEngine*>( aEngine );
    engine->DoReleaseSemaphore();
    }

// ---------------------------------------------------------------------------
// release semaphore
// ---------------------------------------------------------------------------
//
void CVoipEventLogEngine::DoReleaseSemaphore()
    {
    iSemaphore.Signal();
    }

// ---------------------------------------------------------------------------
// Begins transaction in repository.
// ---------------------------------------------------------------------------
//
void CVoipEventLogEngine::BeginTransactionLC()
    {
    VELLOGSTRING( "CVoipEventLogEngine::BeginTransaction() - IN" );

    iSemaphore.Wait();
    TCleanupItem cleanup( CVoipEventLogEngine::ReleaseSemaphore, this );
    CleanupStack::PushL( cleanup );

    User::LeaveIfError( iRepository->StartTransaction( CRepository::EReadWriteTransaction ) );

    iRepository->CleanupRollbackTransactionPushL(); // if leave happens, only roll back, no delete

    VELLOGSTRING( "CVoipEventLogEngine::BeginTransaction() - OUT" );

    }


// ---------------------------------------------------------------------------
// Commits changes in repository.
// ---------------------------------------------------------------------------
//
void CVoipEventLogEngine::CommitTransactionL()
    {
    VELLOGSTRING(
        "CVoipEventLogEngine::CommitTransactionL() - IN" );

    TUint32 temp;
    User::LeaveIfError( iRepository->CommitTransaction( temp ) );

    iSemaphore.Signal();

    CleanupStack::Pop( 2 ); // semaphore and repository

    VELLOGSTRING( "CVoipEventLogEngine::CommitTransaction() - OUT" );

    }


// ---------------------------------------------------------------------------
// Generate time stamp with current time
// ---------------------------------------------------------------------------
//
void CVoipEventLogEngine::GenerateTimeStampL( TDes& aTimeStamp) const
    {
    TTime now;
    now.HomeTime();
    now.FormatL( aTimeStamp, KVoIPTimeStampFormat );
    }

// ---------------------------------------------------------------------------
// Get latest index number
// ---------------------------------------------------------------------------
//
TInt CVoipEventLogEngine::GetLatestIndexL()
    {
    TInt latestIndex( 0 );
    User::LeaveIfError( iRepository->Get( KVELLatestErrorIndexKey, latestIndex ) );

    return latestIndex;
    }

// ---------------------------------------------------------------------------
// Get new index number
// ---------------------------------------------------------------------------
//
TInt CVoipEventLogEngine::GetNewIndexL()
    {
    TInt latestIndex = GetLatestIndexL();
    TInt maxCount = GetMaxErrorCountL();
    TInt newIndex = latestIndex + 1;
    if( newIndex >= maxCount )
        {
        newIndex = newIndex - maxCount;
        }

    return newIndex;
    }

// ---------------------------------------------------------------------------
// Get maximum error count
// ---------------------------------------------------------------------------
//
TInt CVoipEventLogEngine::GetMaxErrorCountL()
    {
    TInt count( 0 );
    User::LeaveIfError( iRepository->Get( KVELMaxErrorCountKey, count ) );

    return count;
    }

// ---------------------------------------------------------------------------
// Writes an error data to event log
// ---------------------------------------------------------------------------
//
void CVoipEventLogEngine::WriteErrorL( const CVoipErrorEntry& aErrorEntry )
    {
    if( aErrorEntry.ErrorText().Length() > NCentralRepositoryConstants::KMaxBinaryLength )
        {
        User::Leave( KErrArgument );
        }

    TInt newIndex = GetNewIndexL();
    TBuf<KTimeStampStrLen> timeStamp;
    GenerateTimeStampL( timeStamp );

    // error code
    TUint32 key = newIndex + KSPColumnErrorCode;
    User::LeaveIfError( iRepository->Set( key, aErrorEntry.ErrorCode() ) );

    // error text
    key += KColumnIncrement;
    User::LeaveIfError( iRepository->Set( key, aErrorEntry.ErrorText() ) );

    // time stamp
    key += KColumnIncrement;
    User::LeaveIfError( iRepository->Set( key, timeStamp ) );

    // save newIndex to latest index
    User::LeaveIfError( iRepository->Set( KVELLatestErrorIndexKey, newIndex ) );

    // save new count to cenrep
    TInt count( 0 );

    User::LeaveIfError( iRepository->Get( KVELErrorCountKey, count ) );
    if( count < GetMaxErrorCountL() )
        {
        count++;
        User::LeaveIfError( iRepository->Set( KVELErrorCountKey, count ) );
        }
    }

// ---------------------------------------------------------------------------
// Reads error count from the log
// ---------------------------------------------------------------------------
//
void CVoipEventLogEngine::ErrorCountL( TInt& aCount )
    {
    User::LeaveIfError( iRepository->Get( KVELErrorCountKey, aCount ) );
    }

// ---------------------------------------------------------------------------
// Reset voip errors log history.
// ---------------------------------------------------------------------------
//
TInt CVoipEventLogEngine::ResetLogHistory()
    {
    TInt retValue = iRepository->Reset();
    return retValue;
    }

// ---------------------------------------------------------------------------
// Reads an error informaiton from the log
// ---------------------------------------------------------------------------
//
void CVoipEventLogEngine::ReadErrorL( TInt aIndex, CVoipErrorEntry& aErrorEntry )
    {
    TInt count( 0 );
    ErrorCountL( count );

    if( aIndex > count - 1 )
        {
        User::Leave( KErrNotFound );
        }

    TInt latestIndex = GetLatestIndexL();
    TInt index = latestIndex - aIndex;
    if( index < 0 )
        {
        index += count;
        }

    // error code
    TUint32 key = index + KSPColumnErrorCode;
    TInt errorCode;
    User::LeaveIfError( iRepository->Get( key, errorCode ) );
    aErrorEntry.SetErrorCode( errorCode );

    // error text
    HBufC* errorText = HBufC::NewLC( NCentralRepositoryConstants::KMaxBinaryLength );
    TPtr ptrErrorText = errorText->Des();
    key += KColumnIncrement;
    User::LeaveIfError( iRepository->Get( key, ptrErrorText ) );

    // error timestamp
    HBufC* timeStamp = HBufC::NewLC( KTimeStampStrLen );
    TPtr ptrTimeStamp = timeStamp->Des();
    key += KColumnIncrement;
    User::LeaveIfError( iRepository->Get( key, ptrTimeStamp ) );

    // set new data to aErrorEntry
    User::LeaveIfError( aErrorEntry.SetErrorText( *errorText ) );
    User::LeaveIfError( aErrorEntry.SetTimeStamp( *timeStamp ) );

    CleanupStack::PopAndDestroy( 2, errorText );

    }

