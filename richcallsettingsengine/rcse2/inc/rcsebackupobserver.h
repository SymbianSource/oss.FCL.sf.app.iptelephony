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



#ifndef CRCSEBACKUPOBSERVER_H
#define CRCSEBACKUPOBSERVER_H

#include <e32base.h>
#include <e32property.h>

class MRCSEBackupObserver;

/**
 *  Implements observer class for backup events.
 *
 *  @lib rcse.lib
 *  @since S60 3.0
 */
NONSHARABLE_CLASS( CRCSEBackupObserver ) : public CActive
{
public:
    static CRCSEBackupObserver* NewL( MRCSEBackupObserver& aObserver );

    ~CRCSEBackupObserver();

    /**
     * Return backup / restore status
     *
     * @since S60 3.0
     * @return ETrue if backup or restore is ongoing, else EFalse
     */
    TBool IsBackupOperationRunning();

protected:

    /**
     * From CActive.
     * Cancel.
     *
     * @since S60 3.0
     */
    void DoCancel();

    /**
     * From CActive.
     * Handle backup event notification.
     *
     * @since S60 3.0
     */
    void RunL();

private:

    CRCSEBackupObserver( MRCSEBackupObserver& aObserver );
    void ConstructL();

private:

    /**
     * Publish & Subscribe connection
     */
    RProperty       iBackupStatus;

    /**
     * Observer object
     */
    MRCSEBackupObserver& iObserver;

private:
    // For testing
    #ifdef TEST_EUNIT
        friend class UT_CRCSEBackupObserver;
    #endif                                  



};

#endif // CRCSEBACKUPOBSERVER_H

