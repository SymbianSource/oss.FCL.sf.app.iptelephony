/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CCCHServerBase declaration
*
*/

#ifndef M_CCHCONNMONHANDLERNOTIFIER_H
#define M_CCHCONNMONHANDLERNOTIFIER_H

#include <rconnmon.h>

/**
 *  MCCHConnMonHandlerNotifier declaration
 *  @lib cchserver.exe
 *  @since S60 9.2
 */
class MCCHConnMonHandlerNotifier
    {

public:
    
    /**
     * Informs observer when network scanning is completed.
     * 
     * @since S60 9.2
     * @param aSNAPs SNAPs info.
     * @param aError Error code
     */
    virtual void NetworkScanningCompletedL( const TConnMonSNAPInfo& aSNAPs, TInt aError ) = 0;
    
    /**
     * Informs observer when SNAPs availability is changed
     * 
     * @since S60 9.2
     * @param aError Error code.
     *        KErrTimeOut if SNAP availability is not changed in defined time.
     */
    virtual void SNAPsAvailabilityChanged( TInt aError ) = 0;

    };

#endif // M_CCHCONNMONHANDLERNOTIFIER_H

// End of file
