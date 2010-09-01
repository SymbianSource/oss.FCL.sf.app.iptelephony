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
* Description:  Header file for MAcpHttpHandlerObserver
 *
*/


#ifndef MACPHTTPHANDLEROBSERVER_H
#define MACPHTTPHANDLEROBSERVER_H

class RHTTPTransaction;

class MAcpHttpHandlerObserver
    {
public:

    /**
     * Notifies observer about HTTP events.
     *
     * @since S60 v3.2
     * @param aEvent HTTP event.
     */ 
    virtual void NotifyHttpEvent( TInt aEvent ) = 0;

    /**
     * Notifies observer about HTTP event errors.
     *
     * @since S60 v3.2
     * @param aError HTTP event error.
     */ 
    virtual void NotifyHttpError( TInt aError ) = 0;

    /**
     * Notifies observer about received body.
     *
     * @since S60 v3.2
     * @param aBodyData Body data.
     */ 
    virtual void NotifyBodyReceived( const TDesC8& aBodyData ) = 0;

    /**
     * Notifies observer about received header.
     *
     * @since S60 v3.2
     * @param aContentType Content data.
     */
    virtual void NotifyContentTypeReceived( 
        const TDesC8& aContentType ) = 0; 
    
    /**
     * Notifies observer about received session id.
     * 
     * @since S60 v3.2
     * @param aSession Id Session id received from the server.
     */
    virtual void NotifySessionIdReceivedL(
        const TDesC8& aSessionId ) = 0;
    };
    
#endif // MACPHTTPHANDLEROBSERVER_H

// End of file.
