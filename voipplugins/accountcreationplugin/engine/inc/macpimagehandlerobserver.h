/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header file for MAcpImageHandlerObserver
 *
*/


#ifndef MACPIMAGEHANDLEROBSERVER_H
#define MACPIMAGEHANDLEROBSERVER_H

class MImageHandlerObserver
    {
public:
    /**
     * Notifies completion of image handling.
     * 
     * @since S60 v3.2
     * @param aErr Error code.
     */
    virtual void NotifyImageCompletion( TInt aErr ) = 0;
    };

#endif // MACPIMAGEHANDLEROBSERVER_H

// End of file.
