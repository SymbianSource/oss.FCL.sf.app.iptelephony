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
* Description:  Implements CAcpController methods
*
*/

#include <apgcli.h>
#include <apgtask.h>
#include <eikenv.h>
#include <utf.h>
#include <pathinfo.h> // For getting phone memory boot path.
#include <f32file.h>  // For getting phone memory boot path.
#include <sip.h>
#include <sipprofileregistry.h>
#include <sipmanagedprofileregistry.h>
#include <sipprofile.h>
#include <sipmanagedprofile.h>
#include <XdmSettingsApi.h>
#include <pressettingsapi.h>
#include <centralrepository.h>
#include <CWPEngine.h>
#include <wspdecoder.h>
#include <PnpUtilImpl.h>
#include <imcvcodc.h>

#include "acpprovider.h"
#include "acpcontroller.h"
#include "acpxmlhandler.h"
#include "acphttphandler.h"
#include "accountcreationpluginlogger.h"
#include "macpcontrollerobserver.h"
#include "acpbrowserparams.h"
#include "acpimagehandler.h"
#include "acpprivatecrkeys.h"

// ---------------------------------------------------------------------------
// CAcpController::CAcpController
// ---------------------------------------------------------------------------
//
CAcpController::CAcpController( MAcpControllerObserver& aObserver ) 
    : iObserver( aObserver ),
    iActiveIndex( KErrNone ),
    iSisInstallation( EFalse )
    {
    }

// ---------------------------------------------------------------------------
// CAcpController::ConstructL
// ---------------------------------------------------------------------------
//
void CAcpController::ConstructL()
    {
    ACPLOG( "CAcpController::ConstructL begin" );

    iParser = CAcpXmlHandler::NewL( *this );
    iHttpHandler = CAcpHttpHandler::NewL( *this );
    iData = HBufC8::NewL( KBufferSize );
    iImageHandler = CAcpImageHandler::NewL( *this );
    User::LeaveIfError( iFs.Connect() );
    iSessionId = HBufC8::NewL( 0 );

    ACPLOG( "CAcpController::ConstructL end" );
    }

// ---------------------------------------------------------------------------
// CAcpController::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CAcpController* CAcpController::NewL( 
    MAcpControllerObserver& aObserver )
    {
    CAcpController* self = CAcpController::NewLC( aObserver );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CAcpController::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CAcpController* CAcpController::NewLC( 
    MAcpControllerObserver& aObserver )
    {
    CAcpController* self = new ( ELeave ) CAcpController( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CAcpController::~CAcpController
// ---------------------------------------------------------------------------
//
CAcpController::~CAcpController()
    {
    ACPLOG( "CAcpController::~CAcpController begin" );

    iFs.Close();
    iProviders.ResetAndDestroy();
    delete iParser;
    delete iHttpHandler;
    delete iData;

#ifdef __PLUG_AND_PLAY_MOBILE_SERVICES    
    delete iPnpmsparams;
#endif
   
    delete iImageHandler;
    delete iSessionId;
    delete iSipModel;

    ACPLOG( "CAcpController::~CAcpController end" );
    }

// ---------------------------------------------------------------------------
// CAcpController::FetchProviderListFromNetworkL
// Fetches provider list from network and parses it.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAcpController::FetchProviderListFromNetworkL()
    {
    ACPLOG( "CAcpController::FetchProviderListFromNetworkL begin" );

    // Create SIP profile registry. From now on we'll get SIP events.
    iSipModel = CSIPManagedProfileRegistry::NewL( *this );
    
    // Delete old data, otherwise Refresh won't work.
    delete iData;
    iData = NULL;
    iData = HBufC8::NewL( KBufferSize );    
    iFilename.Zero();
    iProviders.ResetAndDestroy();

    // 'Restart' XML parser.
    delete iParser;
    iParser = NULL;
    iParser = CAcpXmlHandler::NewL( *this );

    // Get server URL from CenRep.
    HBufC* url = HBufC::NewLC( KNameMaxLength * 2 );
    CRepository* cenrep = CRepository::NewLC( KAccountCreationSettingsUid );
    
    TPtr urlPtr( url->Des() );
    cenrep->Get( KAccountCreationPluginServerUri, urlPtr );
    
    CleanupStack::PopAndDestroy( cenrep );    

    // Descriptor for body data.
    TBuf8<KDefaultBufferSize> postData8;

    // Get the download parameters.
    FetchParametersL( urlPtr ); 
    
    // Convert unicode descriptor into UTF8.
    HBufC8 *url8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L( *url );
    CleanupStack::PushL( url8 );
    
    // Start dowloading provider list from network.
    iHttpHandler->GetDataL( *url8, KXmlMimeType, postData8, EFalse,
        KNullDesC8 );

    CleanupStack::PopAndDestroy( 2, url ); // url8, url

    // Set request type for the http observer because of downloading XML
    // file from network.
    // From now on, response events comes to NotifyHttpEvent, NotifyHttpError, 
    // NotifyBodyReceived and NotifyContentTypeReceived.
    iType = EXml;   

    // Reset active index.
    SaveActiveIndex( 0 );

    ACPLOG( "CAcpController::FetchProviderListFromNetworkL end" );
    }

// ---------------------------------------------------------------------------
// CAcpController::FetchSisFileFromNetworkL
// Fetches provider list from network and parses it.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAcpController::FetchSisFileFromNetworkL( TDesC8& aSisUrl )
    {
    ACPLOG( "CAcpController::FetchSisFileFromNetworkL begin" );

    // Delete old data.
    delete iData;
    iData = NULL;
    iData = HBufC8::NewL( KBufferSize ); 
    
    // Descriptor for body data.
    TBuf8<KDefaultBufferSize> postData8; 

    iFs.Delete( KSisFile ); // Old file should delete at first.
    iWs.Create( iFs , KSisFile, EFileWrite );// Create the file as requested.
    iFilename.Copy( KSisFile );

    // Start downloading sis file from network. Use saved session id.
    iHttpHandler->GetDataL( aSisUrl, KSisMimeType, postData8, EFalse,
        *iSessionId, ETrue );

    // Set request type for the HTTP observer because of downloading SIS file
    // from network.
    // From now on, response events comes to NotifyHttpEvent, NotifyHttpError, 
    // NotifyBodyReceived and NotifyContentTypeReceived.
    iType = ESis;    
    
    // Reset error flag.
    iErrorDownloading = EFalse;

    ACPLOG( "CAcpController::FetchSisFileFromNetworkL end" );
    }

// ---------------------------------------------------------------------------
// CAcpController::FetchIconFileFromNetworkL
// Fetches provider list from network and parses it.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAcpController::FetchIconFileFromNetworkL( TDesC8& aIconUrl )
    {
    ACPLOG( "CAcpController::FetchIconFileFromNetworkL begin" );
    ACPLOG2( " - fetching: %S", &aIconUrl );
    
    if ( aIconUrl.Length() )
        {
        // Descriptor for downloading icon file.
        TBuf<KDefaultBufferSize> url;

        // Copy the icon address into the descriptor with default size (256).
        url.Copy( aIconUrl.Right( KDefaultBufferSize ) );

        // Descriptor for body data.
        TBuf8<KDefaultBufferSize> postData8;

        // Use saved session id when fetching icons.
        iHttpHandler->GetDataL( aIconUrl, KIconMimeType, postData8, ETrue,
            *iSessionId );

        // Set request type for the HTTP observer because of downloading icon
        // file from network.
        // From now on, response events comes to NotifyHttpEvent,
        // NotifyHttpError, NotifyBodyReceived and NotifyContentTypeReceived.
        iType = EIcon; 
        }
    
    ACPLOG( "CAcpController::FetchIconFileFromNetworkL end" );
    }

// ---------------------------------------------------------------------------
// CAcpController::CountOfProviders
// Returns count of providers in providers array.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAcpController::CountOfProviders() const
    {
    TInt count = iProviders.Count();
    ACPLOG2( "CAcpController::CountOfProviders: count=%d", count );
    return count;
    }

// ---------------------------------------------------------------------------
// CAcpController::ProviderNameFromIndexL
// Returns pointer to provider name by index.
// ---------------------------------------------------------------------------
//
EXPORT_C const TPtrC CAcpController::ProviderNameFromIndexL(
    TInt aIndex ) const
    {
    if ( 0 > aIndex || aIndex >= iProviders.Count() ) 
        {
        User::Leave( KErrArgument );
        }
    CAcpProvider& provider = *iProviders[aIndex];
    return provider.ProviderName();
    }

// ---------------------------------------------------------------------------
// CAcpController::ProviderSisUrlFromIndexL
// Returns pointer to provider sisurl by index.
// ---------------------------------------------------------------------------
//
EXPORT_C const TPtrC8 CAcpController::ProviderSisUrlFromIndexL( 
    TInt aIndex ) const
    {
    if ( 0 > aIndex || aIndex >= iProviders.Count() )
        {
        User::Leave( KErrArgument );
        }
    CAcpProvider& provider = *iProviders[aIndex];
    return provider.SisUrl();   
    }

// ---------------------------------------------------------------------------
// CAcpController::ProviderActivateUrlFromIndexL
// Returns pointer to provider sisurl by index.
// ---------------------------------------------------------------------------
//
EXPORT_C const TPtrC8 CAcpController::ProviderActivationUrlFromIndexL( 
    TInt aIndex ) const
    {
    if ( 0 > aIndex || aIndex >= iProviders.Count() )
        {
        User::Leave( KErrArgument ); 
        }
    CAcpProvider& provider = *iProviders[aIndex];
    return provider.ActivationUrl(); 
    }

// ---------------------------------------------------------------------------
// CAcpController::ProviderCreateUrlFromIndexL
// Returns pointer to provider sisurl by index.
// ---------------------------------------------------------------------------
//
EXPORT_C const TPtrC8 CAcpController::ProviderCreationUrlFromIndexL( 
    TInt aIndex ) const
    {
    if ( 0 > aIndex || aIndex >= iProviders.Count() )
        {
        User::Leave( KErrArgument );
        }
    CAcpProvider& provider = *iProviders[aIndex];
    return provider.CreationUrl();    
    }

// ---------------------------------------------------------------------------
// CAcpController::ProviderDescriptionFromIndexL
// Returns pointer to provider description by index.
// ---------------------------------------------------------------------------
//
EXPORT_C const TPtrC CAcpController::ProviderDescriptionFromIndexL( 
    TInt aIndex ) const
    {
    if ( 0 > aIndex || aIndex >= iProviders.Count() )
        {
        User::Leave( KErrArgument );
        }
    CAcpProvider& provider = *iProviders[aIndex];
    return provider.ProviderDescription();    
    }

// ---------------------------------------------------------------------------
// CAcpController::ProviderTypeFromIndexL
// Returns pointer to provider Type by index.
// ---------------------------------------------------------------------------
//
EXPORT_C const TPtrC CAcpController::ProviderTypeFromIndexL( 
    TInt aIndex ) const
    {
    if ( 0 > aIndex || aIndex >= iProviders.Count() )
        {
        User::Leave( KErrArgument );
        }
    CAcpProvider& provider = *iProviders[aIndex];
    return provider.ProviderType();    
    }

// ---------------------------------------------------------------------------
// CAcpController::ProviderIconUrlFromIndexL
// Returns pointer to provider Type by index.
// ---------------------------------------------------------------------------
//
EXPORT_C const TPtrC8 CAcpController::ProviderIconUrlFromIndexL( 
    TInt aIndex ) const
    {
    if ( 0 > aIndex || aIndex >= iProviders.Count() )
        {
        User::Leave( KErrArgument );
        }

    CAcpProvider& provider = *iProviders[aIndex];
    return provider.IconUrl();
    }

// ---------------------------------------------------------------------------
// CAcpController::SaveActiveIndex
// Stores active list index in provider list view to member data.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAcpController::SaveActiveIndex( TInt aIndex )
    {
    iActiveIndex = aIndex;
    }

// ---------------------------------------------------------------------------
// CAcpController::ActiveIndex
// Returns active list index from member data.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAcpController::ActiveIndex() const
    {
    return iActiveIndex;
    }    

// ---------------------------------------------------------------------------
// CAcpController::FetchParametersL
// ---------------------------------------------------------------------------
//
EXPORT_C void CAcpController::FetchParametersL( TDes& aUrl )
    {
    ACPLOG( "CAcpController::FetchParametersL begin" );

#ifdef __PLUG_AND_PLAY_MOBILE_SERVICES    
    if ( !iPnpmsparams )
        {
        iPnpmsparams = CACPBrowserParams::NewL();
        }

    // This is going to append detail data after the requested address because
    // of downloading data from network later on.
    iPnpmsparams->GetParameters( aUrl );
#endif

    ACPLOG( "CAcpController::FetchParametersL end" );
    }

// ---------------------------------------------------------------------------
// CAcpController::CancelHttpRequest
// Cancels ongoing HTTP request (transaction).
// ---------------------------------------------------------------------------
//
EXPORT_C void CAcpController::CancelHttpRequest()
    {
    ACPLOG( "CAcpController::CancelHttpRequest begin" );

    iWs.Close();
    if ( iHttpHandler )
        {
        iHttpHandler->CancelTransaction();
        }
    
    ACPLOG( "CAcpController::CancelHttpRequest end" );
    }

// ---------------------------------------------------------------------------
// CAcpController::FetchActionFileL
// Starts downloading action (wbxml) file from the server.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAcpController::FetchActionFileL( const TDesC8& aUrl )
    {
    ACPLOG( "CAcpController::FetchActionFileL begin" );
    
    if ( !aUrl.Length() )
        {
        User::Leave( KErrArgument );
        }
    
    delete iData;
    iData = NULL;
    iData = HBufC8::NewL( KBufferSize );   

    iType = EAction;
    iHttpHandler->GetDataL( aUrl, KActionMimeType, *iData, EFalse,
        *iSessionId );
    
    // Reset error flag.
    iErrorDownloading = EFalse;
    
    ACPLOG( "CAcpController::FetchActionFileL end" );
    }

// ---------------------------------------------------------------------------
// CAcpController::SessionId
// Returns session id received from the server.
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CAcpController::SessionId()
    {
    return *iSessionId;
    }

// ---------------------------------------------------------------------------
// CAcpController::ProviderBitmapFromIndexL
// Returns pointer to provider icon by index.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAcpController::ProviderBitmapsFromIndexL(
    TInt aIndex, CFbsBitmap*& aBitmap, CFbsBitmap*& aMask ) const
    {
    ACPLOG2( "CAcpController::ProviderBitmapsFromIndexL (%d)", aIndex );
    
    // Check whether the index in range or not.
    if ( aIndex < 0 || aIndex >= iProviders.Count() ) 
        {
        // Not in range next leaves with error
        User::Leave( KErrArgument );
        }

    // Get provider.
    CAcpProvider& provider = *iProviders[aIndex];
    
    // Set bitmaps.
    provider.GetBitmaps( aBitmap, aMask );
    
    ACPLOG( "CAcpController::ProviderBitmapsFromIndexL out" );
    }

// ---------------------------------------------------------------------------
// CAcpController::CreateFileFromDataL
// Creates file from data.
// ---------------------------------------------------------------------------
//
void CAcpController::CreateFileFromDataL( const TDesC& aFilename )
    {
    ACPLOG2( "CAcpController::CreateFileFromDataL file=%S", &aFilename );

    RFileWriteStream ws; // Create stream for writing data into the file
    CleanupClosePushL( ws );

    iFs.Delete( aFilename ); // Old file should delete at first.
    ws.Create( iFs , aFilename, EFileWrite );// Create the file as requested.
    ws.WriteL( iData->Des() );// Write requested data into the file.
    ws.CommitL(); // Ensure that the stream is not contain data anymore.
    CleanupStack::PopAndDestroy( &ws );

    // Delete content of the descriptor so it's ready to receive data again
    // from network.
    TPtr8 dataPtr( iData->Des() );
    dataPtr.Delete( 0, iData->Length() );
    }

// ---------------------------------------------------------------------------
// CAcpController::HandleActionFileL
// Parses the received action file for wbxml content and sends it to
// provisioning engine.
// ---------------------------------------------------------------------------
//
void CAcpController::HandleActionFileL()
    {
    ACPLOG( "CAcpController::HandleActionFileL begin" );
    
    if ( !iData || !iData->Length() )
        {
        User::Leave( KErrArgument );
        }

    // These values are from PnPMS documentation.
    const TInt KContentTypeLenPos = 5;
    const TInt KSkippableBytes = 6;
    
    // Parse content from the wrapper.
    TUint32 headersLen( 0 );
    TUint32 contentLen( 0 );
    TUint8 contentTypeLen( (*iData)[KContentTypeLenPos] );

    TPtrC8 decodeBuf( iData->Ptr() + KSkippableBytes + contentTypeLen );
    TWspPrimitiveDecoder decoder( decodeBuf );
    User::LeaveIfError( decoder.UintVar( headersLen ) );
    ACPLOG2( " - headers length: %d", headersLen );
    
    User::LeaveIfError( decoder.UintVar( contentLen ) );
    ACPLOG2( " - content length: %d", contentLen );
    if ( !contentLen || !headersLen )
        {
        User::Leave( KErrCorrupt );
        }
    
    // Get data and header pointers.
    const TInt dataOffset( iData->Length() - contentLen );
    const TUint8* dataPtr( iData->Ptr() + dataOffset ); 
    const TInt headerOffset( iData->Length() - contentLen - headersLen );
    const TUint8* headerPtr( iData->Ptr() + headerOffset );
    
    TPtrC8 wbxmlPtr( dataPtr, contentLen ); // Pointer to wbxml data.
    TPtrC8 hdrPtr( headerPtr, headersLen ); // Pointer to headers.

    // Validate data with PnPUtil.
    CPnpUtilImpl* pnpUtil = CPnpUtilImpl::NewLC();
    
    HBufC8* nonce = HBufC8::NewLC( KNonceLength );
    TPtr8 noncePtr( nonce->Des() );
    pnpUtil->GetNonceL( noncePtr );
    
    _LIT8( KSignatureValue, "SignValue:" );
    _LIT8( KDigestValue, "DigValue:" );

    TPtrC8 digestEnc( GetHeaderParamL( hdrPtr, KDigestValue ) );
    TPtrC8 signatureEnc( GetHeaderParamL( hdrPtr, KSignatureValue ) );
    
    if ( pnpUtil->VerifySignatureL(
        digestEnc, signatureEnc, wbxmlPtr, *nonce ) )
        {
        ACPLOG( " - signature verified" );
        }
    else
        {
        ACPLOG( " - cannot verify signature" );
        User::Leave( KErrCorrupt );
        }
    
    CleanupStack::PopAndDestroy( 2, pnpUtil );    
    
    // Import the OMA CP message.
    CWPEngine* provEngine = CWPEngine::NewLC();
    provEngine->ImportDocumentL( wbxmlPtr );
    
    // Populate OMA message to adapters.
    provEngine->PopulateL();
    
    // Save OMA message.
    const TInt itemCount = provEngine->ItemCount();
    for ( TInt counter = 0; counter < itemCount; counter++ )
        {
        provEngine->SaveL( counter );
        }

    CleanupStack::PopAndDestroy( provEngine );
    
    iObserver.NotifyProvisioningCompleted();

    ACPLOG( "CAcpController::HandleActionFileL end" );
    }

// ---------------------------------------------------------------------------
// CAcpController::NextIndexWithIconUrl
// Returns next index with an icon URL in the providers list.
// ---------------------------------------------------------------------------
//
TInt CAcpController::NextIndexWithIconUrl( TPtrC8& aResultUrl )
    {
    ACPLOG( "CAcpController::NextIndexWithIconUrl begin" );
    
    TInt ret( KErrNotFound );

    const TInt startIndex( iActiveIndex );
    for ( TInt i = startIndex; i < iProviders.Count(); i++ )
        {
        TPtrC8 iconUrl;
        TRAPD( error, iconUrl.Set( ProviderIconUrlFromIndexL( i ) ) );
        if ( KErrNone == error && iconUrl.Length() )
            {
            SaveActiveIndex( i );
            aResultUrl.Set( iconUrl );
            ret = i;
            break;
            }
        }
    
    ACPLOG2( "CAcpController::NextIndexWithIconUrl end (%d)", ret );
    return ret;
    }

// ---------------------------------------------------------------------------
// CAcpController::GetHeaderParamL
// Parses a specific header value from head wrapper format.
// ---------------------------------------------------------------------------
//
TPtrC8 CAcpController::GetHeaderParamL( const TDesC8& aHeaders,
    const TDesC8& aParam )
    {
    ACPLOG( "CAcpController::GetHeaderParamL begin" );
    
    if ( !aHeaders.Length() || !aParam.Length() )
        {
        User::Leave( KErrArgument );
        }
    
    TInt paramLoc = aHeaders.Find( aParam );
    
    if ( KErrNotFound == paramLoc )
        {
        ACPLOG( " - error: Given parameter not found in headers" );
        User::Leave( KErrNotFound );
        }
    else
        {
        paramLoc = paramLoc + aParam.Length();
        }
    
    TPtrC8 paramToEndPtr( aHeaders.Mid( paramLoc ) );
    TInt paramLen = paramToEndPtr.Locate( '\n' );
    if ( KErrNotFound == paramLen )
        {
        paramLen = aHeaders.Length() - paramLoc;
        }
    ACPLOG2( " - parameter length: %d", paramLen );
    
    TPtrC8 param( aHeaders.Ptr() + paramLoc, paramLen );

    ACPLOG( "CAcpController::GetHeaderParamL end" );
    return param;
    }

// ---------------------------------------------------------------------------
// CAcpController::NotifyParsingCompleted
// From MAcpXmlHandlerObserver.
// ---------------------------------------------------------------------------
//
void CAcpController::NotifyParsingCompleted( TInt aError )
    {
    ACPLOG( "CAcpController::NotifyParsingCompleted begin" );

    iFs.Delete( iFilename ); 
    
    // Forward error code to the observer and return.
    if ( KErrNone != aError )
        {
        iObserver.NotifyProviderListReady( aError );
        return;
        }

    // Descriptor for downloading icon file from network.
    SaveActiveIndex( 0 );
    TPtrC8 iconUrl;
    TInt index = NextIndexWithIconUrl( iconUrl );
    
    if ( KErrNotFound != index )
        {
        // Provider with icon URL found, start downloading.
        TRAP( aError, FetchIconFileFromNetworkL( iconUrl ) );
        if ( KErrNone != aError )
            {
            iObserver.NotifyProviderListReady( aError );
            CancelHttpRequest();
            }
        }
    else
        {
        // None of the providers contains icons -> loading done.
        iObserver.NotifyProviderListReady( KErrNone );
        CancelHttpRequest();
        }

    ACPLOG( "CAcpController::NotifyParsingCompleted end" );
    }

// ---------------------------------------------------------------------------
// CAcpController::NotifyParsedProviderL
// From MAcpXmlHandlerObserver.
// ---------------------------------------------------------------------------
//
void CAcpController::NotifyParsedProviderL( const CAcpProvider& aProvider )
    {
    // To save a data from the received provider then new instace of provider
    // must be created at first.
    CAcpProvider* provider = CAcpProvider::NewLC();
    provider->CopyL( aProvider );
    iProviders.Append( provider );
    CleanupStack::Pop( provider );

    ACPLOG( "CAcpController::NotifyParsedProvider: New provider added!" );
    }

// ---------------------------------------------------------------------------
// CAcpController::NotifyHttpError
// From MAcpHttpHandlerObserver.
// ---------------------------------------------------------------------------
//
void CAcpController::NotifyHttpError( TInt aError )
    {
    ACPLOG2( "CAcpController::NotifyHttpError: Error occurred: %d", aError );
    iObserver.NotifyProviderListReady( aError );
    iWs.Close();
    // Set error flag if error occurred.
    if ( aError )
        {
        iErrorDownloading = ETrue;
        }
    }           

// ---------------------------------------------------------------------------
// CAcpController::NotifyHttpEvent
// From MAcpHttpHandlerObserver.
// ---------------------------------------------------------------------------
//
void CAcpController::NotifyHttpEvent( TInt aEvent )
    {
    ACPLOG2( "CAcpController::NotifyHttpEvent begin (%d)", aEvent );

    TInt error( KErrNone ); // For all operations

    switch ( aEvent )
        {
        // To ignore the following
        case THTTPEvent::EGotResponseHeaders:
            break;
        case THTTPEvent::EGotResponseBodyData:
            break;
        case THTTPEvent::EResponseComplete:
            break;
        // Request succeeded
        case THTTPEvent::ESucceeded:
            {
            switch ( iType )
                {
                // XML file for parsing the provider specific data.
                case EXml:
                    {
                    // Xml data has arrived from server successfully.

                    // First parse memory root path.
                    TParse parse;
                    parse.Set( PathInfo::PhoneMemoryRootPath(), NULL, NULL );
                    TFileName xmlFile( parse.Drive() );
                    // Then append file path to memory root.
                    xmlFile.Append( KXmlFile );
                    iFilename.Copy( xmlFile );
                    TRAP( error, CreateFileFromDataL( iFilename ) );

                    if ( KErrNone == error )
                        {
                        TRAP( error, iParser->StartParsingL( iFilename ) );
                        }
                    }
                    break;
                // SIS file for installing the provider data on phone.
                case ESis:
                    {
                    // SIS data has arrived from server successfully.

                    // First parse memory root path.
                    TParse parse;
                    parse.Set( PathInfo::PhoneMemoryRootPath(), NULL, NULL );
                    TFileName sisFile( parse.Drive() );
                    // Then append file path to memory root.
                    sisFile.Append( KSisFile );
                    iFilename.Copy( sisFile );

                    TRAP( error, iWs.CommitL() );
                    iWs.Close();
                    if ( KErrNone == error )
                        {
                        // Set sis installation flag and notify 
                        // sis downloading completed.
                        iSisInstallation = ETrue;
                        iObserver.NotifyDownloadingSISCompleted( iFilename );           
                        }
                    }
                    break;
                // Icon file of the provider for displaying own icon on the
                // provider list view.
                case EIcon:
                    {
                    const TInt index = ActiveIndex();
                    if ( index < iProviders.Count() && index >= 0 )
                        {
                        CAcpProvider& provider = *iProviders[index];
                        TPtrC8 mimeType = provider.ContentData();
                        TRAP( error,  iImageHandler->StartToDecodeL( 
                            *iData, mimeType ) );
                        }
                    }
                    break;
                // Action file received successfully.
                case EAction:
                    {
                    TRAP( error, HandleActionFileL() );
                    ACPLOG2( " - HandleActionFileL error %d", error );
                    iObserver.NotifyDownloadingCompleted( error );
                    }
                    break;
                }
            }            
            break;
        case THTTPEvent::EFailed:
            {
            ACPLOG( "CAcpController::NotifyHttpEvent:HTTP request failed!" );
            iWs.Close();
            iObserver.NotifyProviderListReady( KErrGeneral );
            
            // Set error flag.
            iErrorDownloading = ETrue;
            }            
            break;
        // Ignore all unspecified events.
        default:
            {
            ACPLOG( "CAcpController::NotifyHttpEvent:Unknown HTTP request!" );
            }
            break;
        }
    }

// ---------------------------------------------------------------------------
// CAcpController::NotifyBodyReceived
// From MAcpHttpHandlerObserver.
// ---------------------------------------------------------------------------
//
void CAcpController::NotifyBodyReceived( const TDesC8& aBodyData )
    {
    ACPLOG( "CAcpController::NotifyBodyReceived begin" );
    ACPLOG2( " - length of received data: %d", aBodyData.Length() );

    // For SIS files we use RFileWriteStream for storing data.
    if ( 0 == iFilename.Compare( KSisFile() ) )
        {
        TRAP_IGNORE( iWs.WriteL( aBodyData ) );
        ACPLOG( "CAcpController::NotifyBodyReceived end" );   
        return;
        }

    // Data should exist before going to copy the data received from network.
    if ( iData && aBodyData.Length() > 0 )
        {
        TPtr8 dataPtr( iData->Des() );
        TRAPD( error, iData = iData->ReAllocL(
            iData->Length() + aBodyData.Length() ) );

        if ( KErrNone == error )
            {
            // Get old descriptor data into the pointer.
            dataPtr.Set( iData->Des() );
            // Copy the new data at the end of the descriptor.
            dataPtr.Append( aBodyData );
            }
        }

    ACPLOG( "CAcpController::NotifyBodyReceived end" );   
    }           

// ---------------------------------------------------------------------------
// CAcpController::NotifyContentTypeReceived
// From MAcpHttpHandlerObserver.
// ---------------------------------------------------------------------------
//
void CAcpController::NotifyContentTypeReceived( const TDesC8& aContentData )
    {
    ACPLOG( "CAcpController::NotifyContentTypeReceived begin" );

    TInt index = ActiveIndex();

    if ( index >= 0 && index < iProviders.Count() )
        {
        CAcpProvider& provider = *iProviders[index];
        // Set the new content type of the received file
        // into the provider content 
        TRAP_IGNORE( provider.SetContentDataL( aContentData ) );
        }

    ACPLOG( "CAcpController::NotifyContentTypeReceived end" );
    }

// ---------------------------------------------------------------------------
// CAcpController::NotifySessionIdReceivedL
// From MAcpHttpHandlerObserver.
// ---------------------------------------------------------------------------
//
void CAcpController::NotifySessionIdReceivedL( const TDesC8& aSessionId )
    {
    delete iSessionId;
    iSessionId = NULL;
    iSessionId = aSessionId.AllocL();
    }

// ---------------------------------------------------------------------------
// CAcpController::NotifyImageCompletion
// From class MImageHandlerObserver.
// An asynchronous notify has occurred related to a icon 
// file downloading for the provider.
// ---------------------------------------------------------------------------
//
void CAcpController::NotifyImageCompletion( TInt aErr )
    {
    ACPLOG2( "CAcpController::NotifyImageCompletion begin status=%d", aErr );

    const TInt providerCount = iProviders.Count();

    if ( KErrNone == aErr )
        {
        // Gets an index activated.
        TInt index = ActiveIndex();

        // Checks the index validity.
        if ( index < providerCount )
            {
            // Gets a provider.
            CAcpProvider& provider = *iProviders[index];

            // Gets a bitmap already converted.
            CFbsBitmap* bitmap = iImageHandler->GetBitmap();
            CFbsBitmap* mask = iImageHandler->GetMask();

            if ( bitmap && mask )
                {
                // Set the bitmap to the provider.
                TRAP_IGNORE( provider.SetBitmapL( bitmap, mask ) );
                }

            // Handle next index if the providers list end wasn't reached.
            index++;
            SaveActiveIndex( index );
            if ( index < providerCount )
                {
                TPtrC8 iconUrl;
                
                // Find the next index with an icon URL.
                index = NextIndexWithIconUrl( iconUrl );

                if ( KErrNotFound != index )
                    {
                    // Delete data buffer.
                    TPtr8 dataPtr( iData->Des() );
                    dataPtr.Delete( 0, iData->Length() );
                    
                    // Downloading the icon file from network.
                    TRAPD( err, FetchIconFileFromNetworkL( iconUrl ) );
                    if ( KErrNone != err )
                        {
                        iObserver.NotifyProviderListReady( err );
                        }
                    }
                else // No more icons to load, notify the observer.
                    {
                    iObserver.NotifyProviderListReady( KErrNone );
                    }
                }
            else
                {
                // Tell the client that the provider list is now ready
                // for displaying on the screen.
                iObserver.NotifyProviderListReady( KErrNone );
                }
            }
        }
    else
        {
        // An error notify to the client.
        iObserver.NotifyProviderListReady( aErr );
        }
    ACPLOG( "CAcpController::NotifyImageCompletion end" );
    }

// ---------------------------------------------------------------------------
// CAcpController::ProfileRegistryEventOccurred
// From MSIPProfileRegistryObserver.
// SIP profile information event.
// ---------------------------------------------------------------------------
//
void CAcpController::ProfileRegistryEventOccurred( 
    TUint32 /*aSIPProfileId*/, 
    TEvent aEvent )
    {
    ACPLOG2( "CAcpController::ProfileRegistryEventOccurred: %d", aEvent );

    switch ( aEvent )
        {
        case EProfileCreated:
            {
            // Notify settings saved if not sis-installation
            if ( !iSisInstallation )
                {
                iObserver.NotifySettingsSaved();
                }           
            }
            break;
            
        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// CAcpController::ProfileRegistryErrorOccurred
// From MSIPProfileRegistryObserver.
// An asynchronous error has occurred related to SIP profile.
// ---------------------------------------------------------------------------
//
void CAcpController::ProfileRegistryErrorOccurred(
    TUint32 /*aSIPProfileId*/,
    TInt aError )
    {
    ACPLOG2( "CAcpController::ProfileRegistryErrorOccurred: %d", aError );
    //To remove warning: variable / argument 'aError' is not used in function
    aError = aError;
    }


// End of file. 
