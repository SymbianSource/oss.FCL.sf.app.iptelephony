/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  header file for sipimresolver
*
*/


#ifndef C_SIPIMRESOLVER_H
#define C_SIPIMRESOLVER_H


#include <implementationproxy.h>
#include <sipresolvedclient2.h>


/**
 *  Class for aiding SIP Client Resolver component to route the incoming
 *  invites to Instant Messaging (IM) according to request headers and
 *  content. It also takes relevant phone state variables into account
 *  when resolving.
 *
 *  @code
 *   CSipImResolver* imResolver = CSipImResolver::NewL();
 *   TBool match = imResolver->MatchRequestL( method, *requestUri, headers,
 *       *content, contentType, resolvedClientUid );
 *  @endcode
 *
 *  @lib sipimresolver.lib
 *  @since S60 v5.0
 */
class CSipImResolver : public CSIPResolvedClient2
    {

public:

    /**
     * Two-phased constructor.
     */
    static CSipImResolver* NewL();

    /**
     * Destructor.
     */
	virtual	~CSipImResolver();

// from base class CSIPResolvedClient2

    /**
     * From CSIPResolvedClient2
     * Defined in a base class
     */
    TBool MatchAcceptContactsL( RStringF aMethod,
                                const CUri8& aRequestUri,
                                const RPointerArray<CSIPHeaderBase>& aHeaders,
                                const TDesC8& aContent,
                                const CSIPContentTypeHeader* aContentType,
                                TUid& aClientUid );

    /**
     * From CSIPResolvedClient2
     * Defined in a base class
     */
    TBool MatchEventL( RStringF aMethod,
                       const CUri8& aRequestUri,
                       const RPointerArray<CSIPHeaderBase>& aHeaders,
                       const TDesC8& aContent,
                       const CSIPContentTypeHeader* aContentType,
                       TUid& aClientUid );

    /**
     * From CSIPResolvedClient2
     * Defined in a base class
     */
    TBool MatchRequestL( RStringF aMethod,
                         const CUri8& aRequestUri,
                         const RPointerArray<CSIPHeaderBase>& aHeaders,
                         const TDesC8& aContent,
                         const CSIPContentTypeHeader* aContentType,
                         TUid& aClientUid );

    /**
     * From CSIPResolvedClient2
     * Defined in a base class
     */
    TBool ConnectSupported();

    /**
     * From CSIPResolvedClient2
     * Defined in a base class
     */
    void ConnectL( const TUid& aClientUid );

    /**
     * From CSIPResolvedClient2
     * Defined in a base class
     */
    void CancelConnect( const TUid& aClientUid );

    /**
     * From CSIPResolvedClient2
     * Defined in a base class
     */
    RPointerArray<CSIPContentTypeHeader> SupportedContentTypesL();

    /**
     * From CSIPResolvedClient2
     * Defined in a base class
     */
    RPointerArray<CSdpMediaField> SupportedSdpMediasL();

    /**
     * From CSIPResolvedClient2
     * Defined in a base class
     */
    void AddClientSpecificHeadersForOptionsResponseL(
        RPointerArray<CSIPHeaderBase>& aHeaders );

private:

    CSipImResolver();

private: // data

    /**
     * Application UID
     */
    TUid iApplicationUID;
    };

#endif // C_SIPIMRESOLVER_H
