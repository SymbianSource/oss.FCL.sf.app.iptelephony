/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  SVP transfer observer interface
*
*/


#ifndef SVPTRANSFEROBSERVER_H
#define SVPTRANSFEROBSERVER_H

#include <e32base.h>

enum TSVPTransferNotifyCode
    {
    ESVPTransferOKHangUp,
    ESVPTransferDecline,
    ESVPIncomingRefer
    };
    
/**
 *  Transfer observer
 *
 *  An interface to be implemented by user of the transfer controller.
 *
 *  @lib svp.dll
 *  @since S60 3.2
 */
class MSVPTransferObserver
    {
    
public:  

    /**
     * Notifier for transfer.
     *
     * @since S60 3.2
     * @param aNotifyCode transfer notify code
     */
    virtual void TransferNotification( TInt aNotifyCode ) = 0;    
				
    /**
     * Notifier for transfer failure.
     *
     * @since S60 3.2
     * @param aError transfer error code
     */
    virtual void TransferFailed( TInt aError ) = 0;
				
    };

#endif // SVPTRANSFEROBSERVER_H
