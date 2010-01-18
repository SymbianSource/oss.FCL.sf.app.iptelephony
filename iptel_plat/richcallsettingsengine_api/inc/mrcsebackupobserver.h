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
* Description:  Call back interface for backup events
*
*/



#ifndef MRCSEBACKUPOBSERVER_H
#define MRCSEBACKUPOBSERVER_H


/**
 *  Call back interface for backup events
 *
 *  @lib rcse.lib
 *  @since S60 3.0
 */

class MRCSEBackupObserver
    {
public:

    /**
     * Handle backup events
     *
     * @since S60 3.0
     */
    virtual void HandleBackupOperationEventL() = 0;

    };

#endif // MRCSEBACKUPOBSERVER_H