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
* Description:  Internal note result observer definition
*
*/


#ifndef M_CCHUINOTERESULTOBSERVER_H
#define M_CCHUINOTERESULTOBSERVER_H

#include <e32base.h>

#include "cchuicommon.hrh"

/**
 * CCH UI note result observer.
 *
 * @code
 *  inherit in header:
 *  class CMyClass : public CBase, public MCchUiObserver
 * @endcode
 *
 * @lib cchui.lib
 * @since S60 5.0
 */
NONSHARABLE_CLASS( MCchUiNoteResultObserver )
    {
public:

    /**
     * Gets called when certain dialog has completed.
     *
     * @since S60 5.0
     * @param aCompleteCode Complete code.
     * @param aResultParams Result parameters of the completed dialog.
     */
    virtual void DialogCompletedL( 
        TInt aCompleteCode, 
        TCCHUiNotifierParams aResultParams ) = 0;
    };


#endif // M_CCHUINOTERESULTOBSERVER_H
