/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declarition of CAcpHttpHandler
*
*/

 
#ifndef ACPHTTPHANDLER_H
#define ACPHTTPHANDLER_H

#include <e32base.h>
#include <http/rhttpsession.h>
#include <http/mhttpdatasupplier.h>
#include <http/mhttptransactioncallback.h>
#include <es_sock.h>

class RHTTPTransaction;
class MAcpHttpHandlerObserver;

/**
 *  CAcpHttpHandler class
 *  Declarition of CAcpHttpHandler.
 *
 *  @lib accountcreationplugin.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CAcpHttpHandler ) : public CActive,
                                       public MHTTPDataSupplier,
                                       public MHTTPTransactionCallback
    {
public: 

    /**
     * Two-phased constructor.
     *
     * @param aObserver for observer
     */
    static CAcpHttpHandler* NewL( MAcpHttpHandlerObserver& aObserver );

    /**
     * Two-phased constructor.
     *
     * @param aObserver for observer
     */
    static CAcpHttpHandler* NewLC( MAcpHttpHandlerObserver& aObserver );

    /**
     * Destructor.
     */
    virtual ~CAcpHttpHandler();

    /**
     * Fetches data from network.
     *
     * @since S60 v3.2
     * @param aUrl Data URL.
     * @param aContentType Content type.
     * @param aBody Post data.
     * @param aMimeTypeImage Image's mime type.
     * @param aSessionId Session id to be stored in a cookie. Can be KNullDesC8.
     */
    void GetDataL( 
        const TDesC8& aUrl, 
        const TDesC8& aContentType,
        const TDesC8& aBody,
        TBool aMimeTypeImage,
        const TDesC8& aSessionId,
        TBool aGetSisFile = EFalse );

    /**
     * Cancels ongoing transaction and frees the resources.
     *
     * @since S60 v3.2
     */
    void CancelTransaction();
    
    /**
     * Shuts down connection (RConnection and RSocketServ).
     * 
     * @since S60 v3.2
     */
    void ShutdownConnection();
    
    /**
     * Handles known HTTP headers.
     * 
     * @since S60 v3.2
     * @param aTransaction HTTP transaction that received the headers.
     */
    void ProcessHeadersL( RHTTPTransaction& aTransaction );
    
// from base class MHTTPTransactionCallback

    /**
     * From MHTTPTransactionCallback.
     * Called when the filter's registration conditions are satisfied
     * for events that occur on a transaction.
     * 
     * @since S60 v3.2
     * @param aTransaction The transaction that the event has occurred on.
     * @param aEvent The event that has occurred.
     */
    void MHFRunL( RHTTPTransaction aTransaction, const THTTPEvent& aEvent );

    /**
     * From MHTTPTransactionCallback.
     * Called when RunL leaves from a transaction event.
     * 
     * @since S60 v3.2
     * @param aError The leave code that RunL left with.
     * @param aTransaction The transaction that was being processed.
     * @param aEvent The Event that was being processed.
     * @return KErrNone if the error has been cancelled or the code
     *         of the continuing error otherwise.
     */
    TInt MHFRunError( TInt aError, RHTTPTransaction aTransaction, 
        const THTTPEvent& aEvent );

private:

    CAcpHttpHandler( MAcpHttpHandlerObserver& aObserver );
    void ConstructL();

    /**
     * Returns request method based on type.
     *
     * @since S60 v3.2
     * @param aType for type of method
     * @return request method
     */
    RStringF GetRequestMethod( TInt aType ) const;

    /**
     * Starts the connection asynchronously.
     *
     * @since S60 v3.2
     */
    void StartConnectionL();

    /**
     * Sets transaction's request headers.
     *
     * @since S60 v3.2
     * @param aHeaders for headers
     * @param aHdrField for header fields
     * @param aHdrValue for header value
     */
    void SetHeaderL( RHTTPHeaders aHeaders, TInt aHdrField,  
        const TDesC8& aHdrValue );
    
    /**
     * Submits HTTP transaction.
     * 
     * @since S60 v3.2
     */
    void SubmitTransactionL();
    
// from base class MHTTPDataSupplier

    /**
     * From MHTTPDataSupplier.
     * Release the current data part being held at the data
     * supplier.
     * 
     * @since S60 v3.2
     */
    void ReleaseData();

    /**
     * From MHTTPDataSupplier.
     * Obtain a data part from the supplier.
     * 
     * @since S60 v3.2
     * @param aDataPart The data part.
     * @return ETrue if this is the last part. EFalse otherwise
     */
    TBool GetNextDataPart( TPtrC8& aDataPart );

    /**
     * From MHTTPDataSupplier.
     * Reset the data supplier.
     * 
     * @since S60 v3.2
     * @return Error code.
     */
    TInt Reset();

    /**
     * From MHTTPDataSupplier.
     * Obtain the overall size of the data being supplied,
     * if known to the supplier.
     * 
     * @since S60 v3.2
     * @return A size in bytes, or KErrNotFound if the size is not known.
     */
    TInt OverallDataSize();

// from base class CActive.
    
    /**
     * From CActive.
     * 
     * @since S60 v3.2
     */
    void RunL();

    /**
     * From CActive.
     *      
     * @since S60 v3.2
     */
    void DoCancel();
    
private: // data

    /**
     * Reference to observer.
     * Not own.
     */
    MAcpHttpHandlerObserver& iObserver;

    /**
     * Handle for HTTP session.
     * Own. 
     */
    RHTTPSession iSession;

    /**
     * Handle for HTTP transaction.
     * Own. 
     */
    RHTTPTransaction iTransaction;
    
    /**
     * Handle for connection.
     */
    RConnection iConnection;
    
    /*
     * Handle for socket server.
     */
    RSocketServ iSocketServer;

    /**
     * Handle to post data.
     * Own. 
     */
    HBufC8* iPostData;

    /**
     * Tells if a transaction is running.
     */
    TBool iTransactionRunning;
    
    /**
     * Tells if the connection is open.
     */
    TBool iConnectionOpen;

    /**
     * Tells if MIME type is image.
     */
    TBool iMimeTypeImage;
    
    /**
     * Tells if connection prompt has been shown once.
     */
    TBool iPromptShown;
    
    /**
     * Selected IAP id.
     */
    TUint32 iIapId;
    
    /**
     * Selected SNAP id.
     */
    TUint32 iNetId;
    
    /**
     * Connection progress.
     */
    TNifProgress iNifProgress;

    // For unit testing.
#ifdef _DEBUG
    friend class T_CAcpHttpHandler;
#endif
    };

#endif // ACPHTTPHANDLER_H

// End of file.
