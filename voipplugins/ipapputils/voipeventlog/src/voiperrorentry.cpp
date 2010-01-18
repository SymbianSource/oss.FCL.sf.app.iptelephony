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
#include "voiperrorentry.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CVoipErrorEntry::CVoipErrorEntry()
    {
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
void CVoipErrorEntry::ConstructL()
    {
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CVoipErrorEntry* CVoipErrorEntry::NewL()
    {
    CVoipErrorEntry* self = CVoipErrorEntry::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CVoipErrorEntry* CVoipErrorEntry::NewLC()
    {
    CVoipErrorEntry* self = new( ELeave ) CVoipErrorEntry;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CVoipErrorEntry* CVoipErrorEntry::NewLC( TInt aErrorCode, const TDesC& aErrorText )
    {
    CVoipErrorEntry* self = new( ELeave ) CVoipErrorEntry;
    CleanupStack::PushL( self );
    self->ConstructL();
    self->SetErrorCode( aErrorCode );
    self->SetErrorText( aErrorText );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CVoipErrorEntry::~CVoipErrorEntry()
    {
    iErrorText.Close();
    iTimeStamp.Close();
    }


// ---------------------------------------------------------------------------
// Returns error VoIP code.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CVoipErrorEntry::ErrorCode() const
	{
	return iErrorCode;
	}

// ---------------------------------------------------------------------------
// Sets VoIP error code.
// ---------------------------------------------------------------------------
//
EXPORT_C void CVoipErrorEntry::SetErrorCode( TInt aErrorCode )
	{
	iErrorCode = aErrorCode;
	}

// ---------------------------------------------------------------------------
// Returns error text
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CVoipErrorEntry::ErrorText() const
	{
	return iErrorText;
	}

// ---------------------------------------------------------------------------
// Sets error text
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CVoipErrorEntry::SetErrorText( const TDesC& aErrorText )
	{
    TInt err( KErrNone );
    TInt length( aErrorText.Length() );

    if ( NCentralRepositoryConstants::KMaxBinaryLength < length )
        {
        err = KErrArgument;
        }
    else
        {
        iErrorText.Close();
        err = iErrorText.Create( aErrorText );
        }

    return err;
	
	}

// ---------------------------------------------------------------------------
// Returns time stamp
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CVoipErrorEntry::TimeStamp() const
	{
	return iTimeStamp;
	}
	

// ---------------------------------------------------------------------------
// Sets time stamp
// ---------------------------------------------------------------------------
//
TInt CVoipErrorEntry::SetTimeStamp( const TDesC& aTimeStamp )
	{
    TInt err( KErrNone );
    TInt length( aTimeStamp.Length() );

    if ( NCentralRepositoryConstants::KMaxBinaryLength < length )
        {
        err = KErrArgument;
        }
    else
        {
        iTimeStamp.Close();
        err = iTimeStamp.Create( aTimeStamp );
        }

    return err;
	}

