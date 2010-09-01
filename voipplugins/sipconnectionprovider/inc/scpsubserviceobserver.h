/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef M_SCPSUBSERVICEOBSERVER_H
#define M_SCPSUBSERVICEOBSERVER_H

#include <e32std.h>
#include <e32base.h>
#include <cchclientserver.h>

/**
 *  MScpSipConnectionObserver declaration.
 *  @lib sipconnectionprovider.dll
 */
class MScpSubServiceObserver
    {
public:

    /**
     * Handle SubService state change.
     * @param 
     */
    virtual void HandleSubServiceChange( TCCHSubserviceState aState, TInt aError ) = 0;
        
    };
#endif // M_SCPSUBSERVICEOBSERVER_H

// End of file
