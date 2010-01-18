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
* Description:  Header file for MAcpXmlHandlerObserver
 *
*/


#ifndef MACPXMLHANDLEROBSERVER_H
#define MACPXMLHANDLEROBSERVER_H

class CAcpProvider;

class MAcpXmlHandlerObserver
    {
public:
    /**
     * Notifies observer when parsing is completed.
     *
     * @since S60 v3.2
     * @param aError System wide error code.
     */ 
    virtual void NotifyParsingCompleted( TInt aError ) = 0;

    /**
     * Notifies observer when new provider is available.
     *
     * @since S60 v3.2
     * @param aError System wide error code.
     */ 
    virtual void NotifyParsedProviderL( 
        const CAcpProvider& aProvider ) = 0;
    };

#endif // MACPXMLHANDLEROBSERVER_H

// End of file.
