/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides notification service of changes in VoIP profiles.
*
*/



// INCLUDE FILES
#include <centralrepository.h>
#include <featmgr.h>

#include "crcsechangenotifier.h"
#include "rcseconstants.h"
#include "rcsebackupobserver.h"
#include "rcselogger.h"
#include "rcsepanic.h"
#include "rcseprivatecrkeys.h"


const TUint32 KUnknownNotifier = KMaxTUint;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CRCSEChangeNotifier::CRCSEChangeNotifier
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CRCSEChangeNotifier::CRCSEChangeNotifier()
    {
    iRange = KUnknownNotifier;
    iNotifyRequested = EFalse;
    }

// -----------------------------------------------------------------------------
// CRCSEChangeNotifier::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CRCSEChangeNotifier::ConstructL( const TDesC& aName )
    {
    RCSELOGSTRING( "CRCSEChangeNotifier::ConstructL() - IN" );
    RCSELOGTHREAD();

    // Check VoIP support from feature manager
    FeatureManager::InitializeLibL();
    TBool support = FeatureManager::FeatureSupported( KFeatureIdCommonVoip );
    FeatureManager::UnInitializeLib();
    if (!support)
        {
        User::Leave(KErrNotSupported);
        }

    iRepository = CRepository::NewL( KCRUidRCSE );

    iName.Copy( aName );
    iBackupObserver = CRCSEBackupObserver::NewL( *this );

    RCSELOGSTRING( "CRCSEChangeNotifier::ConstructL() - OUT" );
    }

// -----------------------------------------------------------------------------
// CRCSEChangeNotifier::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CRCSEChangeNotifier* CRCSEChangeNotifier::NewL( const TDesC& aName )
    {
    CRCSEChangeNotifier* self = new( ELeave ) CRCSEChangeNotifier();
    CleanupStack::PushL( self );
    self->ConstructL( aName );
    CleanupStack::Pop( self );
    return self;
    }

    
// -----------------------------------------------------------------------------
// CRCSEChangeNotifier::~CRCSEChangeNotifier
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CRCSEChangeNotifier::~CRCSEChangeNotifier()
    {
    RCSELOGSTRING( "CRCSEChangeNotifier::~CRCSEChangeNotifier() - IN" );
    RCSELOGTHREAD();

    if ( iBackupObserver )
        {
        iBackupObserver->Cancel();
        delete iBackupObserver;
        iBackupObserver = NULL;
        }

    delete iRepository;

    RCSELOGSTRING( "CRCSEChangeNotifier::~CRCSEChangeNotifier() - OUT" );
    }

// -----------------------------------------------------------------------------
// CRCSEChangeNotifier::HandleBackupOperationEventL
// Handle backup events. 
// -----------------------------------------------------------------------------
//
void CRCSEChangeNotifier::HandleBackupOperationEventL()
	{
	RCSELOGSTRING( "CRCSEChangeNotifier::HandleBackupOperationEventL() - IN" );
    RCSELOGTHREAD();

	if ( iBackupObserver->IsBackupOperationRunning() )
        {
        CancelNotify();
        }
    else if ( iReqStatus )
        {
        NotifyChange( *iReqStatus );
        iReqStatus = NULL;
        }

    RCSELOGSTRING( "CRCSEChangeNotifier::HandleBackupOperationEventL() - OUT" );
	}

// -----------------------------------------------------------------------------
// CRCSEChangeNotifier::NotifyChange
// -----------------------------------------------------------------------------
//
EXPORT_C void CRCSEChangeNotifier::NotifyChange( TRequestStatus& aStatus )
    {
    RCSELOGSTRING( "CRCSEChangeNotifier::NotifyChange() - IN" );
    RCSELOGTHREAD();

    // If backup is active, store request handle for later use.
    if ( iBackupObserver->IsBackupOperationRunning() )
        {
        RCSELOGSTRING( "CRCSEChangeNotifier::NotifyChange() - BUR active" );
        iReqStatus = &aStatus;
        aStatus = KRequestPending;
        }
    else
        {
        TRAP_IGNORE( SetNotifierL( aStatus ) );
        }

    RCSELOGSTRING( "CRCSEChangeNotifier::NotifyChange() - OUT" );
    }

// -----------------------------------------------------------------------------
// CRCSEChangeNotifier::CancelNotify
// -----------------------------------------------------------------------------
//
EXPORT_C void CRCSEChangeNotifier::CancelNotify()
    {
    RCSELOGSTRING( "CRCSEChangeNotifier::CancelNotify() - IN" );
    RCSELOGTHREAD();

    iRepository->NotifyCancel( iRange, KRCSEColumnMask );
    iReqStatus = NULL;
    iNotifyRequested = EFalse;

    RCSELOGSTRING( "CRCSEChangeNotifier::CancelNotify() - OUT" );
    }
// -----------------------------------------------------------------------------
// CRCSEChangeNotifier::SetNotifierL
// Set up notifier to requested data source
// -----------------------------------------------------------------------------
//
void CRCSEChangeNotifier::SetNotifierL( TRequestStatus& aStatus )
    {
    RCSELOGSTRING2( "CRCSEChangeNotifier::SetNotifierL( %S ) - IN", &iName );

    if ( KDbName() == iName ) // VoIP profile IDs
        {
        iRange = KColProfileId;
        }
    else if ( KDbCodecName() == iName ) // Audio Codec IDs
        {
        iRange = KColAudioCodecId;
        }
    else // Unknown
        {
        User::Leave( KErrArgument );
        }

    User::LeaveIfError(
        iRepository->NotifyRequest( iRange, KRCSEColumnMask, aStatus ) );

    iNotifyRequested = ETrue;

    RCSELOGSTRING( "CRCSEChangeNotifier::SetNotifierL() - OUT" );
    }


//  End of File  
