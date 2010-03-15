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
* Description:  Implements CAcpHttpHandler methods
*
*/


#include <http/thttpevent.h>
#include <http/rhttpheaders.h>
#include <httpstringconstants.h>
#include <http/rhttptransaction.h>
#include <http/rhttpconnectioninfo.h>
#include <http/thttphdrval.h>


#include <commdbconnpref.h>
#include <es_enum.h>
#include <escapeutils.h>

#include "acphttphandler.h"
#include "accountcreationpluginlogger.h"
#include "macphttphandlerobserver.h"
#include "accountcreationengineconstants.h"



// ---------------------------------------------------------------------------
// CAcpHttpHandler::CAcpHttpHandler
// ---------------------------------------------------------------------------
//
CAcpHttpHandler::CAcpHttpHandler( MAcpHttpHandlerObserver& aObserver )
    : CActive( CActive::EPriorityStandard ), iObserver( aObserver )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CAcpHttpHandler::ConstructL
// ---------------------------------------------------------------------------
//
void CAcpHttpHandler::ConstructL()
    {
    ACPLOG( "CAcpHttpHandler::ConstructL begin" );
    
    iSession.OpenL();
    
    ACPLOG( "CAcpHttpHandler::ConstructL end" );
    }

// ---------------------------------------------------------------------------
// CAcpHttpHandler::NewL
// ---------------------------------------------------------------------------
//
CAcpHttpHandler* CAcpHttpHandler::NewL( MAcpHttpHandlerObserver& aObserver )
    {
    CAcpHttpHandler* self = CAcpHttpHandler::NewLC( aObserver );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CAcpHttpHandler::NewLC
// ---------------------------------------------------------------------------
//
CAcpHttpHandler* CAcpHttpHandler::NewLC( MAcpHttpHandlerObserver& aObserver )
    {
    CAcpHttpHandler* self = new ( ELeave ) CAcpHttpHandler( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CAcpHttpHandler::~CAcpHttpHandler
// ---------------------------------------------------------------------------
//
CAcpHttpHandler::~CAcpHttpHandler()
    {
    ACPLOG( "CAcpHttpHandler::~CAcpHttpHandler begin" );

    if ( IsActive() )
        {
        Cancel();
        }

    // Closing the session closes all transactions.
    iSession.Close();
    
    ShutdownConnection();

    delete iPostData;

    ACPLOG( "CAcpHttpHandler::~CAcpHttpHandler end" );
    }

// ---------------------------------------------------------------------------
// CAcpHttpHandler::GetDataL
// Fetches data from network.
// ---------------------------------------------------------------------------
//
void CAcpHttpHandler::GetDataL( const TDesC8& aUrl,
    const TDesC8& aContentType, const TDesC8& aBody, TBool aMimeTypeImage,
    const TDesC8& aSessionId, TBool aGetSisFile )
    {
    ACPLOG( "CAcpHttpHandler::GetDataL begin" ); 

    // Mime type in use.
    iMimeTypeImage = aMimeTypeImage;

    // Copy data to be posted into member variable.
    delete iPostData;
    iPostData = NULL;
    iPostData = aBody.AllocL();

    RHTTPConnectionInfo connInfo = iSession.ConnectionInfo();
    RStringPool pool = iSession.StringPool();

    // Start connection.
    if ( !iConnectionOpen )
        {
        iPromptShown = EFalse; // Always show connection prompt first.
        StartConnectionL();
        }

    connInfo.SetPropertyL(
        pool.StringF( HTTP::EHttpSocketServ, RHTTPSession::GetTable() ),
        THTTPHdrVal( iSocketServer.Handle() ) );

    connInfo.SetPropertyL(
        pool.StringF( HTTP::EHttpSocketConnection, RHTTPSession::GetTable() ),
        THTTPHdrVal( reinterpret_cast<TInt>( &iConnection ) ) );

    TBuf8<KMaxUriLength> url;

    // Insert session id in URLs other than sis URLs.
    if ( aSessionId.Length() && KSisMimeType().Compare( aContentType ) )
        {
        // Change http://address?param=value to
        //        http://adress;jsessionid=[sessionid]?param=value

        // Find ? and insert the session id part before it.
        TInt index = aUrl.Find( KQuestionMark8 );
        if ( KErrNotFound != index )
            {
            url.Append( aUrl.Mid( 0, index ) );
            url.Append( KSessionId );
            url.Append( aSessionId );
            url.Append( aUrl.Mid( index ) );
            }
        }
    else
        {
        // Use original URL.
        url.Copy( aUrl );
        }

    HBufC8* encoded = EscapeUtils::EscapeEncodeL( url,
        EscapeUtils::EEscapeNormal );
    CleanupStack::PushL( encoded );

    TBuf<KMaxUriLength> urlTemp;
    urlTemp.Copy( *encoded );
    ACPLOG2( " - using url: %S", &urlTemp );

    TUriParser8 uriParser;
    User::LeaveIfError( uriParser.Parse( *encoded ) );

    // Use HTTP GET when downloading SIS files.
    if ( aGetSisFile )
        {
        iTransaction = iSession.OpenTransactionL( uriParser, *this,
            GetRequestMethod( HTTP::EGET ) );
        }
    else
        {
        iTransaction = iSession.OpenTransactionL( uriParser, *this,
            GetRequestMethod( HTTP::EPOST ) );
        }

    CleanupStack::PopAndDestroy( encoded );

    // Set request headers.
    RHTTPHeaders hdr = iTransaction.Request().GetHeaderCollection();

    SetHeaderL( hdr, HTTP::EUserAgent, KUserAgent );
    SetHeaderL( hdr, HTTP::EAccept, KAccept );

    // For POST add Content-Type header and set body.
    if ( !aGetSisFile )
        {
        SetHeaderL( hdr, HTTP::EContentType, aContentType );
        iTransaction.Request().SetBody( *this );
        }

    // If connection has been opened already, submit transaction.
    if ( iConnectionOpen )
        {
        iConnection.Progress( iNifProgress );

        if ( !iNifProgress.iError && KLinkLayerOpen == iNifProgress.iStage )
            {
            ACPLOG( " - connection still open, submit next transaction" );
            SubmitTransactionL();
            }
        else
            {
            // Connection lost or not started, restart it.
            ACPLOG( " - connection lost, reconnect" );

            iConnectionOpen = EFalse;

            if ( !IsActive() ) 
                {
                iConnection.Start( iStatus );
                SetActive();
                }
            }
        }

    ACPLOG( "CAcpHttpHandler::GetDataL end" );
    }

// ---------------------------------------------------------------------------
// CAcpHttpHandler::SubmitTransactionL
// Submits HTTP transaction.
// ---------------------------------------------------------------------------
//
void CAcpHttpHandler::SubmitTransactionL()
    {
    ACPLOG( "CAcpHttpHandler::SubmitTransactionL begin" );
    
    // Submit the transaction. 
    // From now on, response event comes to MHFRunL and/or MHFRunError.
    iTransaction.SubmitL();
    iTransactionRunning = ETrue;
    
    ACPLOG( "CAcpHttpHandler::SubmitTransactionL end" );
    }

// ---------------------------------------------------------------------------
// CAcpHttpHandler::GetRequestMethod
// Returns request method based on type.
// ---------------------------------------------------------------------------
//
RStringF CAcpHttpHandler::GetRequestMethod( TInt aType ) const
    {
    return iSession.StringPool().StringF( aType, RHTTPSession::GetTable() );
    }

// ---------------------------------------------------------------------------
// CAcpHttpHandler::StartConnectionL
// Starts the connection asynchronously.
// ---------------------------------------------------------------------------
//
void CAcpHttpHandler::StartConnectionL()
    {
    ACPLOG( "CAcpHttpHandler::StartConnectionL begin" );
    
    User::LeaveIfError( iSocketServer.Connect() );
    User::LeaveIfError( iConnection.Open( iSocketServer ) );
    
    TCommDbConnPref pref;
    pref.SetDirection( ECommDbConnectionDirectionOutgoing );
    pref.SetBearerSet( KCommDbBearerLAN | KCommDbBearerCSD |
        KCommDbBearerWcdma | KCommDbBearerWLAN );
    
    // Show connection prompt only once.
    if ( !iPromptShown )
        {
        ACPLOG( " - show connection prompt" );
        pref.SetDialogPreference( ECommDbDialogPrefPrompt );
        iPromptShown = ETrue;
        }
    else
        {
        ACPLOG3( " - set iap %d / net %d", iIapId, iNetId );
        pref.SetDialogPreference( ECommDbDialogPrefDoNotPrompt );
        pref.SetIapId( iIapId );
        pref.SetNetId( iNetId );
        }

    if ( !IsActive() ) 
        {
        iConnection.Start( pref, iStatus );
        SetActive();
        }

    ACPLOG( "CAcpHttpHandler::StartConnectionL end" );
    }

// ---------------------------------------------------------------------------
// CAcpHttpHandler::CancelTransaction
// Cancels ongoing transaction and frees the resources.
// ---------------------------------------------------------------------------
//
void CAcpHttpHandler::CancelTransaction()
    {
    ACPLOG( "CAcpHttpHandler::CancelTransaction begin" );
    
    if ( !iTransactionRunning )
        {
        ACPLOG( "CAcpHttpHandler::CancelTransaction end (not running)" );
        return;
        }
    
    iTransaction.Close();
    ACPLOG( " - transaction closed" );
    
    iTransactionRunning = EFalse;
    ACPLOG( "CAcpHttpHandler::CancelTransaction end" );
    }

// ---------------------------------------------------------------------------
// CAcpHttpHandler::ShutdownConnection
// Shuts down connection.
// ---------------------------------------------------------------------------
//
void CAcpHttpHandler::ShutdownConnection()
    {
    ACPLOG( "CAcpHttpHandler::ShutdownConnection begin" );
    
    iConnection.Close();
    iSocketServer.Close();
    
    ACPLOG( "CAcpHttpHandler::ShutdownConnection end" );
    }

// ---------------------------------------------------------------------------
// CAcpHttpHandler::SetHeaderL
// Sets transaction's request headers.
// ---------------------------------------------------------------------------
//
void CAcpHttpHandler::SetHeaderL( 
    RHTTPHeaders aHeaders, 
    TInt aHdrField, 
    const TDesC8& aHdrValue )
    {
    RStringF valStr = iSession.StringPool().OpenFStringL( aHdrValue );
    CleanupClosePushL( valStr );
    THTTPHdrVal val( valStr );
    aHeaders.SetFieldL( iSession.StringPool().StringF(
        aHdrField, RHTTPSession::GetTable() ), val );
    CleanupStack::PopAndDestroy( &valStr );

    ACPLOG( "CAcpHttpHandler::SetHeaderL: Request headers set." );
    }

// ---------------------------------------------------------------------------
// CAcpHttpHandler::GetNextDataPart
// From MHTTPDataSupplier.
// ---------------------------------------------------------------------------
//
TBool CAcpHttpHandler::GetNextDataPart( TPtrC8& aDataPart )
    {
    ACPLOG( "CAcpHttpHandler::GetNextDataPart begin" );
    
    if ( iPostData )
        {
        aDataPart.Set( iPostData->Des() );
        }
    
    ACPLOG( "CAcpHttpHandler::GetNextDataPart end" );
    return ETrue;
    }

// ---------------------------------------------------------------------------
// CAcpHttpHandler::ReleaseData
// From MHTTPDataSupplier.
// ---------------------------------------------------------------------------
//
void CAcpHttpHandler::ReleaseData()
    {
    ACPLOG( "CAcpHttpHandler::ReleaseData begin" );
    
    delete iPostData;
    iPostData = NULL;
    
    ACPLOG( "CAcpHttpHandler::ReleaseData end" );
    }

// ---------------------------------------------------------------------------
// CAcpHttpHandler::OverallDataSize
// From MHTTPDataSupplier.
// ---------------------------------------------------------------------------
//
TInt CAcpHttpHandler::OverallDataSize()
    {
    ACPLOG( "CAcpHttpHandler::OverallDataSize begin" );
    
    if ( iPostData )
        {
        ACPLOG2( "CAcpHttpHandler::OverallDataSize end (%d)",
            iPostData->Length() );
        return iPostData->Length();
        }
    else
        {
        ACPLOG( "CAcpHttpHandler::OverallDataSize end" );
        return KErrNotFound ;
        }
    }

// ---------------------------------------------------------------------------
// CAcpHttpHandler::Reset
// From MHTTPDataSupplier.
// ---------------------------------------------------------------------------
//
TInt CAcpHttpHandler::Reset()
    {
    if ( iPostData )
        {
        // iPostData (which is used as data source) is found.
        return KErrNone;
        }
    
    return KErrNotFound;
    }

// ---------------------------------------------------------------------------
// CAcpHttpHandler::MHFRunL
// From MHTTPTransactionCallback.
// ---------------------------------------------------------------------------
//
void CAcpHttpHandler::MHFRunL( 
    RHTTPTransaction aTransaction, 
    const THTTPEvent& aEvent )
    {
    ACPLOG2( "CAcpHttpHandler::MHFRunL: THTTPEvent=%d", aEvent.iStatus );

    switch ( aEvent.iStatus )
        {
        case THTTPEvent::EGotResponseHeaders:
            {
            ProcessHeadersL( aTransaction );
            iObserver.NotifyHttpEvent( aEvent.iStatus );
            }
            break;
        case THTTPEvent::EGotResponseBodyData:
            {
            MHTTPDataSupplier* body = aTransaction.Response().Body();
            TPtrC8 dataChunk;

            TBool isLast = body->GetNextDataPart( dataChunk );
            iObserver.NotifyBodyReceived( dataChunk );

            // Check if more data is expected.
            if ( isLast )
                {
                iObserver.NotifyHttpEvent( aEvent.iStatus );
                }

            // Always remember to release the body data.
            body->ReleaseData();
            }           
            break;
        case THTTPEvent::EResponseComplete:
            {
            iObserver.NotifyHttpEvent( aEvent.iStatus );
            }           
            break;
        case THTTPEvent::ESucceeded:
            {
            ACPLOG( " - HTTP event: succeeded" );
            CancelTransaction();
            iObserver.NotifyHttpEvent( aEvent.iStatus );
            }           
            break;
        case THTTPEvent::EFailed:
            {
            ACPLOG( " - HTTP event: failed" );
            CancelTransaction();
            iObserver.NotifyHttpEvent( aEvent.iStatus );
            }           
            break;
        default:
            {
            // Check if some other real error received.
            if ( aEvent.iStatus != THTTPEvent::ERedirectedTemporarily )
                {
                CancelTransaction();
                iObserver.NotifyHttpError( aEvent.iStatus );
                
                // HTTP error occurred, it's safer to restart the connection.
                iConnectionOpen = EFalse;
                iSession.Close();
                iConnection.Close();
                
                // Reopen session.
                iSession.OpenL();
                }
            }
            break;
        }
    }
    
// ---------------------------------------------------------------------------
// CAcpHttpHandler::MHFRunError
// From MHTTPTransactionCallback.
// ---------------------------------------------------------------------------
//
TInt CAcpHttpHandler::MHFRunError( 
    TInt aError, 
    RHTTPTransaction /*aTransaction*/, 
    const THTTPEvent& /*aEvent*/ )
    {
    ACPLOG2( "CAcpHttpHandler::MHFRunError: aError = %d", aError );
    iObserver.NotifyHttpError( aError );
    
    // HTTP error occurred, it's safer to restart the connection.
    iConnectionOpen = EFalse;
    iSession.Close();
    iConnection.Close();
    
    // Reopen session.
    TRAP_IGNORE( iSession.OpenL() );
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CAcpHttpHandler::RunL
// From CActive.
// ---------------------------------------------------------------------------
//
void CAcpHttpHandler::RunL()
    {
    ACPLOG( "CAcpHttpHandler::RunL begin" );
    
    if ( KErrNone == iStatus.Int() )
        {
        ACPLOG( " - Connection started, submit transaction" );
        TUint connCount( KErrNone );
        iConnection.EnumerateConnections( connCount );
        ACPLOG2( " - Connection count %d", connCount );
        
        if ( connCount )
            {
            TConnectionInfoBuf connInfoPckg;
            iConnection.GetConnectionInfo( 1, connInfoPckg );
            TConnectionInfo connInfo = connInfoPckg();
            
            ACPLOG3( "   - iapId: %d    netId: %d", connInfo.iIapId,
                connInfo.iNetId );
            
            iIapId = connInfo.iIapId;
            iNetId = connInfo.iNetId;
            
            iConnectionOpen = ETrue;
            }
        
        // Submit the first transaction. Further transactions are submitted
        // in GetDataL.
        SubmitTransactionL();
        }
    else
        {
        // Error occurred, notify observer.
        iObserver.NotifyHttpError( iStatus.Int() );
        }
    
    ACPLOG( "CAcpHttpHandler::RunL end" );
    }

// ---------------------------------------------------------------------------
// CAcpHttpHandler::DoCancel
// From CActive.
// ---------------------------------------------------------------------------
//
void CAcpHttpHandler::DoCancel()
    {
    ACPLOG( "CAcpHttpHandler::DoCancel" );
    }

// ---------------------------------------------------------------------------
// CAcpHttpHandler::ProcessHeadersL
// Handles known HTTP headers.
// ---------------------------------------------------------------------------
//
void CAcpHttpHandler::ProcessHeadersL( RHTTPTransaction& aTransaction )
    {
    ACPLOG( "CAcpHttpHandler::ProcessHeadersL begin" );
    
    RHTTPHeaders header = aTransaction.Response().GetHeaderCollection();
    RStringPool pool = aTransaction.Session().StringPool();
    THTTPHdrFieldIter filter = header.Fields();

    while ( !filter.AtEnd() )
        {
        RStringTokenF fieldName = filter();
        RStringF fieldNameStr = pool.StringF( fieldName );
        THTTPHdrVal fieldVal;
        if ( KErrNone == header.GetField( fieldNameStr, 0, fieldVal ) )
            {
            TBuf<KDefaultBufferSize> tmp;
            tmp.Copy( fieldNameStr.DesC() );
            ACPLOG2( " - field: %S", &tmp );
            
            if ( THTTPHdrVal::KStrFVal == fieldVal.Type() )
                {
                tmp.Copy( fieldVal.StrF().DesC() );
                ACPLOG2( " - value: %S", &tmp );
                }
            else if ( THTTPHdrVal::KStrVal == fieldVal.Type() )
                {
                tmp.Copy( fieldVal.Str().DesC() );
                ACPLOG2( " - value: %S", &tmp );
                }
            else if ( THTTPHdrVal::KTIntVal == fieldVal.Type() )
                {
                ACPLOG2( " - value: %d", fieldVal.Int() );
                }
            
            // Get image content type.
            if ( iMimeTypeImage )
                {
                RStringF contentType = pool.StringF(
                    HTTP::EContentType, RHTTPSession::GetTable() );
                if ( fieldNameStr == contentType )
                    {
                    if ( fieldVal.StrF().DesC().Length() )
                        {
                        // Informs provider for content type of image.
                        iObserver.NotifyContentTypeReceived( 
                            fieldVal.StrF().DesC() );
                        }
                    }
                }
            
            // Get session id from a cookie.
            RStringF setCookieStr = pool.StringF( HTTP::ESetCookie,
                RHTTPSession::GetTable() );
            if ( fieldNameStr == setCookieStr )
                {
                ACPLOG( " - cookie field found" );
                
                RStringF cookieStr = pool.StringF(
                    HTTP::ECookie, RHTTPSession::GetTable() );
                
                if ( cookieStr == fieldVal.StrF() )
                    {
                    RStringF cookieNameStr = pool.StringF(
                        HTTP::ECookieName, RHTTPSession::GetTable() );
                    
                    THTTPHdrVal cookieName;
                    
                    if ( KErrNone == header.GetParam(
                        setCookieStr, cookieNameStr, cookieName ) )
                        {
                        RStringF cookieValueStr = pool.StringF(
                            HTTP::ECookieValue, RHTTPSession::GetTable() );
                        THTTPHdrVal cookieVal;
                    
                        if ( KErrNone == header.GetParam(
                            setCookieStr, cookieValueStr, cookieVal ) ) 
                            {
                            if ( THTTPHdrVal::KStrFVal == cookieVal.Type() )
                                {
                                TBuf<KDefaultBufferSize> cookieTmp;
                                cookieTmp.Copy( cookieVal.StrF().DesC() );
                            
                                if ( cookieTmp.Length() )
                                    {
                                    iObserver.NotifySessionIdReceivedL(
                                            cookieVal.StrF().DesC() );
                                    }
                                ACPLOG2( " -session id set to %S", &cookieTmp );
                                }
                            else if ( THTTPHdrVal::KStrVal == cookieVal.Type() )
                                {
                                TBuf<KDefaultBufferSize> cookieTmp;
                                cookieTmp.Copy( cookieVal.Str().DesC() );
                                
                                if ( cookieTmp.Length() )
                                    {
                                    iObserver.NotifySessionIdReceivedL(
                                            cookieVal.Str().DesC() );
                                    }
                                ACPLOG2( " -session id set to %S", &cookieTmp );
                                }
                            }
                        }
                    }
                }
            }
        // Proceed to next header.
        ++filter;
        }
    
    ACPLOG( "CAcpHttpHandler::ProcessHeadersL end" );
    }


// End of file.
