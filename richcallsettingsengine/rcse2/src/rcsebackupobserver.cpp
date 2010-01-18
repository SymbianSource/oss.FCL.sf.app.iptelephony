/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implements observer class for backup events
*
*/


#include <connect/sbdefs.h>

#include "rcsebackupobserver.h"
#include "mrcsebackupobserver.h"
#include "rcselogger.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CRCSEBackupObserver::CRCSEBackupObserver( MRCSEBackupObserver& aObserver )
    : CActive( CActive::EPriorityStandard ),
      iObserver( aObserver )
    {
    CActiveScheduler::Add(this);
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CRCSEBackupObserver::ConstructL()
    {
    RCSELOGSTRING( "CRCSEBackupObserver::ConstructL() - IN" );
    User::LeaveIfError( iBackupStatus.Attach( KUidSystemCategory, 
                                              conn::KUidBackupRestoreKey ) );
    //iStatus = KRequestPending;
    iBackupStatus.Subscribe( iStatus );
    SetActive();
    RCSELOGSTRING( "CRCSEBackupObserver::ConstructL() - OUT" );
    }


// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CRCSEBackupObserver* CRCSEBackupObserver::NewL( MRCSEBackupObserver& aObserver )
    {
    CRCSEBackupObserver* self = new (ELeave) CRCSEBackupObserver( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CRCSEBackupObserver::~CRCSEBackupObserver()
    {
    RCSELOGSTRING( "CRCSEBackupObserver::~CRCSEBackupObserver() - IN" );
    Cancel();
    iBackupStatus.Close();
    RCSELOGSTRING( "CRCSEBackupObserver::~CRCSEBackupObserver() - OUT" );
    }

// ---------------------------------------------------------------------------
// From class CActive.
// Cancel backup/restore event subscription
// ---------------------------------------------------------------------------
//
void CRCSEBackupObserver::DoCancel()
    {
    iBackupStatus.Cancel();
    }


// ---------------------------------------------------------------------------
// From class CActive.
// Handle backup/restore event
// ---------------------------------------------------------------------------
//
void CRCSEBackupObserver::RunL()
    {
    RCSELOGSTRING( "CRCSEBackupObserver::RunL() - IN" );
    User::LeaveIfError( iStatus.Int() );
    iBackupStatus.Subscribe( iStatus );
    SetActive();

    iObserver.HandleBackupOperationEventL();
    RCSELOGSTRING( "CRCSEBackupObserver::RunL() - OUT" );
    }


// ---------------------------------------------------------------------------
// Return backup / restore status
// ---------------------------------------------------------------------------
//
TBool CRCSEBackupObserver::IsBackupOperationRunning()
    {
    TInt backupRestoreStatus;
    TInt err = RProperty::Get( KUidSystemCategory, 
                               conn::KUidBackupRestoreKey,
                               backupRestoreStatus );

    if ( KErrNone != err )
        {
        RCSELOGSTRING( 
            "CRCSEBackupObserver::IsBackupOperationRunning() - FALSE" );
        return EFalse;
        }

    if( backupRestoreStatus == conn::EBURUnset || 
        backupRestoreStatus & conn::EBURNormal ||
        (backupRestoreStatus & conn::KBackupIncTypeMask) == conn::ENoBackup)
        {
        RCSELOGSTRING( 
            "CRCSEBackupObserver::IsBackupOperationRunning() - FALSE" );
        return EFalse;
        }
    else
        {
        RCSELOGSTRING( "CRCSEBackupObserver::IsBackupOperationRunning() - TRUE" );
        return ETrue;
        }
    }

