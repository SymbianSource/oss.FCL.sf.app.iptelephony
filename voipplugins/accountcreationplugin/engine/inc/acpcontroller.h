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
* Description:  Declarition of CAcpController
*
*/

 
#ifndef ACPCONTROLLER_H
#define ACPCONTROLLER_H

#include <e32base.h>
#include <s32file.h>

#include <sipobserver.h>
#include <sipprofileregistryobserver.h>
#include <sipmanagedprofileregistry.h>

#include "accountcreationengineconstants.h"
#include "macpxmlhandlerobserver.h"
#include "macphttphandlerobserver.h"
#include "macpimagehandlerobserver.h"

class CFbsBitmap;
class CAcpProvider;
class CAcpXmlHandler;
class CAcpImageHandler;
class CAcpHttpHandler;
class CAcpImageHandler;
class MAcpControllerObserver;
class CACPBrowserParams;
class MSIPProfileRegistryObserver;
class MImageHandlerObserver;

/**
 *  CAcpController class
 *  Declarition of CAcpController.
 *
 *  @lib accountcreationplugin.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CAcpController ) : public CBase,
                                      public MAcpXmlHandlerObserver,
                                      public MAcpHttpHandlerObserver,
                                      public MSIPProfileRegistryObserver,
                                      public MImageHandlerObserver
    {
public: 

    /**
     * Different kind of field types supported by ACP.
     */
    enum TFetchTypes
        {
        EXml = 0, // provider list in xml form (default)
        ESis,     // sis file
        EIcon,    // icon file (png)
        EAction   // action file containing wbxml provisioning data
        };    

    /**
     * Two-phased constructor.
     *
     * @param aObserver Controller observer.
     */
    IMPORT_C static CAcpController* NewL(
        MAcpControllerObserver& aObserver );

    /**
     * Two-phased constructor.
     *
     * @param aObserver Controller observer.
     */
    IMPORT_C static CAcpController* NewLC(
        MAcpControllerObserver& aObserver );

    /**
     * Destructor.
     */
    virtual ~CAcpController();

    /**
     * Fetches provider list from network and parses it.
     *
     * @since S60 v3.2
     */
    IMPORT_C void FetchProviderListFromNetworkL();

    /**
     * Fetches SIS file from network.
     *
     * @since S60 v3.2
     * @param aSisUrl URL where to download.
     */
    IMPORT_C void FetchSisFileFromNetworkL( TDesC8& aSisUrl );

    /**
     * Fetches icon file from network.
     *
     * @since S60 v3.2
     * @param aIconUrl URL where to download.
     */
    IMPORT_C void FetchIconFileFromNetworkL( TDesC8& aIconUrl );

    /**
     * Returns count of providers in providers array.
     *
     * @since S60 v3.2
     * @return Number of providers
     */
    IMPORT_C TInt CountOfProviders() const;

    /**
     * Returns provider information referenced by index.
     *
     * @since S60 v3.2
     * @param aIndex Provider list index.
     * @return Pointer to provider name.
     */
    IMPORT_C const TPtrC ProviderNameFromIndexL( TInt aIndex ) const;

    /**
     * Returns provider information referenced by index.
     *
     * @since S60 v3.2
     * @param aIndex Provider list index.
     * @return Pointer to provider SIS URL.
     */
    IMPORT_C const TPtrC8 ProviderSisUrlFromIndexL( TInt aIndex ) const;

    /**
     * Returns provider information referenced by index.
     *
     * @since S60 v3.2
     * @param aIndex Provider list index.
     * @return Pointer to provider activation URL.
     */
    IMPORT_C const TPtrC8 ProviderActivationUrlFromIndexL( 
        TInt aIndex ) const;

    /**
     * Returns provider information referenced by index.
     *
     * @since S60 v3.2
     * @param aIndex Provider list index.
     * @return pointer Provider creation urlURL.
     */
    IMPORT_C const TPtrC8 ProviderCreationUrlFromIndexL( 
        TInt aIndex ) const;

    /**
     * Returns provider information referenced by index.
     *
     * @since S60 v3.2
     * @param aIndex Provider list index.
     * @return Pointer to provider description.
     */
    IMPORT_C const TPtrC ProviderDescriptionFromIndexL( 
        TInt aIndex ) const;

    /**
     * Returns provider information referenced by index.
     *
     * @since S60 v3.2
     * @param aIndex Provider list index.
     * @return Pointer to provider Type.
     */
    IMPORT_C const TPtrC ProviderTypeFromIndexL( TInt aIndex ) const;

    /**
     * Returns provider icon URL referenced by index.
     *
     * @since S60 v3.2
     * @param aIndex Provider list index
     * @return Pointer to provider icon URL.
     */
    IMPORT_C const TPtrC8 ProviderIconUrlFromIndexL( TInt aIndex ) const;

    /**
     * Returns provider bitmap referenced by index.
     *
     * @since S60 v3.2
     * @param aIndex Provider list index.
     * @return Pointer to bitmap owned by provider.
     */
    IMPORT_C void ProviderBitmapsFromIndexL( TInt aIndex,
        CFbsBitmap*& aBitmap, CFbsBitmap*& aMask ) const;

    /**
     * Stores active list index in provider list view to member data.
     *
     * @since S60 v3.2
     * @param aIndex Index to be stored.
     */
    IMPORT_C void SaveActiveIndex( TInt aIndex );

    /**
     * Returns active list index from member data.
     *
     * @since S60 v3.2
     * @return Active list index in provider list view.
     */
    IMPORT_C TInt ActiveIndex() const;

    /**
     * Fetches the needed parameters for provisioning.
     *
     * @since S60 v3.2
     * @param aUrl for URL.
     */
    IMPORT_C void FetchParametersL( TDes& aUrl );

    /**
     * Cancels ongoing HTTP request.
     *
     * @since S60 v3.2
     */
    IMPORT_C void CancelHttpRequest();

    /**
     * Starts downloading action (wbxml) file from the server.
     * 
     * @since S60 v3.2
     * @param aUrl URL of the action file.
     */
    IMPORT_C void FetchActionFileL( const TDesC8& aUrl );

    /**
     * Gets session id that was received from the server.
     * 
     * @since S60 v3.2
     * @return Session id descriptor.
     */
    IMPORT_C const TDesC8& SessionId();
    
// from base class MAcpXmlHandlerObserver
    
    /**
     * From MAcpXmlHandlerObserver.
     * Notifies observer when parsing is completed.
     *
     * @since S60 v3.2
     * @param aError System wide error code.
     */
    IMPORT_C void NotifyParsingCompleted( TInt aError );

    /**
     * From MAcpXmlHandlerObserver.
     * Notifies when new provider is available.
     *
     * @since S60 v3.2
     * @param aError System wide error code.
     */
    IMPORT_C void NotifyParsedProviderL( const CAcpProvider& aProvider );

// from base class MAcpHttpHandlerObserver

    /**
     * From MAcpHttpHandlerObserver.
     * Notifies observer about HTTP events.
     *
     * @since S60 v3.2
     * @param aEvent HTTP event.
     */
    void NotifyHttpEvent( TInt aEvent );

    /**
     * From MAcpHttpHandlerObserver.
     * Notifies observer about HTTP event errors.
     *
     * @since S60 v3.2
     * @param aError HTTP event error.
     */
    void NotifyHttpError( TInt aError );

    /**
     * From MAcpHttpHandlerObserver.
     * Notifies observer about received body.
     *
     * @since S60 v3.2
     * @param aBodyData Body data.
     */
    void NotifyBodyReceived( const TDesC8& aBodyData );

    /**
     * From MAcpHttpHandlerObserver.
     * Notifies observer about received header.
     *
     * @since S60 v3.2
     * @param aContentType Content data.
     */
    void NotifyContentTypeReceived( const TDesC8& aBodyData );

    /**
     * From MAcpHttpHandlerObserver.
     * Notifies observer about received session id.
     * 
     * @since S60 v3.2
     * @param aSession Id Session id received from the server.
     */
    void NotifySessionIdReceivedL( const TDesC8& aSessionId );
    
// from base class MImageHandlerObserver

    /**
     * From MImageHandlerObserver.
     * Notifies completion of image handling.
     * 
     * @since S60 v3.2
     * @param aErr Error code.
     */
    void NotifyImageCompletion( TInt aErr );   

private:

    CAcpController( MAcpControllerObserver& aObserver );
    void ConstructL();

    /**
     * Creates XML file from received data.
     *
     * @since S60 v3.2
     * @param aFilename File to be created.
     */
    void CreateFileFromDataL( const TDesC& aFilename );
    
    /**
     * Parses received action file for wbxml content and sends it to
     * provisioning engine.
     * 
     * @since S60 v3.2
     */
    void HandleActionFileL();

    /**
     * Returns next index with an icon URL in the providers list.
     * 
     * @since S60 v3.2
     * 
     * @param aResultUrl On return points to the icon URL if found.
     * @return TInt Index in the providers list or KErrNotFound.
     */
    TInt NextIndexWithIconUrl( TPtrC8& aResultUrl );
    
    /**
     * Parses a specific header value from head wrapper format.
     * 
     * @since S60 v3.2
     * @param aHeaders Descriptor containing the headers.
     * @param aParam Parameter to be parsed.
     * @return Parameter value part of the header.
     */
    TPtrC8 GetHeaderParamL( const TDesC8& aHeaders, const TDesC8& aParam );
    
    /** 
     * From MSIPProfileRegistryObserver.
     * SIP profile information event.
     *
     * @since S60 v3.2
     * @param aProfileId is id for profile
     * @param aEvent type of information event
     */
    void ProfileRegistryEventOccurred(
        TUint32 aSIPProfileId, 
        TEvent aEvent );

    /**
     * From MSIPProfileRegistryObserver.
     * An asynchronous error has occurred related to SIP profile.
     *
     * @since S60 v3.2
     * @param aSIPProfileId the id of failed profile 
     * @param aError a error code
     */
    void ProfileRegistryErrorOccurred(
        TUint32 aSIPProfileId,
        TInt aError );
    
private: // data

    /**
     * Reference for contoller observer.
     */
    MAcpControllerObserver& iObserver;

    /**
     * Handle to XML parser.
     * Own.
     */
    CAcpXmlHandler* iParser;

    /**
     * Handle to HTTP handler.
     * Own.
     */
    CAcpHttpHandler* iHttpHandler;

    /**
     * Array for providers.
     * Own.
     */
    RPointerArray<CAcpProvider> iProviders;

    /**
     * Active list index, updated when provider specific view is opened.
     */
    TInt iActiveIndex;

    /**
     * Data fetched from network.
     * Own.
     */
    HBufC8* iData;

    /**
     * Type of data fetched from network.
     */
    TFetchTypes iType;

    /**
     * File server.
     */
    RFs iFs; 

    /**
     * Name of the file.
     */
    TBuf<KNameMaxLength> iFilename;

    /**
     * Handler for PnPMS parameters.
     */
    CACPBrowserParams* iPnpmsparams;

    /**
     * Provides the image convertions.
     * Own.
     */
    CAcpImageHandler* iImageHandler;
    
    /**
     * Session id to be used for all transactions.
     */
    HBufC8* iSessionId;
    
    /**
     * For getting SIP events.
     */
    CSIPManagedProfileRegistry* iSipModel;
    
    /**
     * A flag telling if the previous download attempt failed.
     * If it's ETrue, a new connection method is asked during the
     * next download.
     */
    TBool iErrorDownloading;
    
    /**
     * A flag telling if sis packet is downoloaded and sis installation
     * will be made.
     */
    TBool iSisInstallation;

    /**
     * File write stream for SIS file downloading.
     */
    RFileWriteStream iWs;

    // For unit testing.
#ifdef _DEBUG
    friend class T_CAcpController;
#endif
    };

#endif // ACPCONTROLLER_H

// End of file.
