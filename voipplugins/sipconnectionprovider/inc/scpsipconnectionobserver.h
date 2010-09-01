/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef M_SCPSIPCONNECTIONOBSERVER_H
#define M_SCPSIPCONNECTIONOBSERVER_H

#include <e32std.h>
#include <e32base.h>

#include "scpdefs.h"

/**
 *  MScpSipConnectionObserver declaration.
 *  @lib sipconnectionprovider.dll
 */
class MScpSipConnectionObserver
    {
public:

    /**
     * SIP profile state observer.
     * @param aProfileId SIP profile id
     * @param aSipEvent A new event of SIP profile.
     */
    virtual void HandleSipConnectionEvent( TUint32 aProfileId,
                                           TScpConnectionEvent aEvent ) = 0;
      
    /**
     * Query is starting to ALR allowed.
     * Client can start ALR later if needed to do something first
     * 
     * @return ETrue if allowed.
     */
    virtual TBool IsSipProfileAllowedToStartAlr() = 0;
    
    };

/**
 *  MScpConnectionObserver declaration.
 *  @lib sipconnectionprovider.dll
 */
class MScpConnectionObserver
    {
public:

    /**
     * SIP profile state observer.
     * @param aProfileId SIP profile id
     * @param aSipEvent A new event of SIP profile.
     */
    virtual void HandleConnectionEvent( TScpConnectionEvent aEvent ) = 0;
        
    };


#endif // M_SCPSIPCONNECTIONOBSERVER_H

// End of file
