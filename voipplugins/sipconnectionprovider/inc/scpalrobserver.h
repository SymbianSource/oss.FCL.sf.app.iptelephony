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


#ifndef M_SCPALROBSERVER_H
#define M_SCPALROBSERVER_H

#include <e32std.h>
#include <e32base.h>

/**
 *  MScpSipAlrObserver declaration.
 *  @lib sipconnectionprovider.dll
 */
class MScpSipAlrObserver
    {
public:

    /** 
     * Enumeration for ALR events 
     */
    enum TScpAlrEvent
        {
        /** 
         * A new IAP became available.
         * When receiving this event the client must call 
         * CSipProfileAlrController::AllowMigrationL or DisallowMigrationL.
         */           
        EScpIapAvailable = 0,
        /** 
         * All the clients have allowed the migration to the new IAP and
         * the migration to the new IAP for the SIP profile has started. 
         */
        EScpMigrationStarted,
        /** 
         * Migration to a new IAP for a SIP profile has been completed and
         * the related SIP profile has been registered via the new IAP.
         */
        EScpMigrationCompleted
        };
            
    /** 
     * ALR event occured.
     * @param aEvent New ALR event.
     * @param aProfileId SIP profile id
     * @param aSnapId Snap id
     * @param aIapId Iap id
     */             
    virtual void HandleAlrEvent( TScpAlrEvent aEvent,
                                 TUint32 aProfileId,
                                 TUint32 aSnapId,
                                 TUint32 aIapId ) = 0;

private:
    
    };

#endif // M_SCPALROBSERVER_H

// End of file
