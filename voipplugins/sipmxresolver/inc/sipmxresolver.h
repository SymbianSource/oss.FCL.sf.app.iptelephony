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
* Description:  header file for sipmxresolver
*
*/

#ifndef CSIPMXRESOLVER_H
#define CSIPMXRESOLVER_H

//  INCLUDES
#include <apgcli.h>
#include <e32base.h>
#include <implementationproxy.h>
#include <sipresolvedclient2.h>
#include "sipmxresolverdebug.h"

// FORWARD DECLARATIONS
class CSdpDocument;
class CSdpMediaField;
class CMusManager;

/**
*  SIP MX Resolver 
*  
*  SIP MX Resolver implements CSIPResolvedClient2 ECOM API.
*  It aids SIP Client Resolver component to route the incoming invites
*  either to Multimedia Sharing / VoIP client according to
*  request headers and content. It also takes relevant phone state
*  variables into account when resolving.
*  
*  @lib sipmxresolver
*  @since Series 60 3.2
*/
class CSipMXResolver : public CSIPResolvedClient2
    {
public: // Constructors and destructor
	
	/**
	* Two-phased constructor.
    */
    static CSipMXResolver* NewL();
    
    /**
    * Destructor.
    */
    virtual ~CSipMXResolver();


public: // From CSIPResolvedClient2

	/**
    * Matches the Accept-Contact-headers
    * to the client(s) represented by this plug-in.
    * This function is called for an incoming SIP request
    * if it contains Accept-Contact-header(s).
    * 
    * @param aMethod the method of the SIP request
    * @param aRequestUri the request-URI of the SIP request
    * @param aHeaders all the headers in the SIP request
    * @param aContent SIP request body; 
    *        zero-length descriptor if not present
    * @param aContentType the content-type of the SIP request. 
	*        Zero-pointer if body is not present.
	* @param aClientUid indicates client's UID for 
	*        SIP e.g. the one passed as a parameter to CSIP::NewL().
	* @return ETrue, if the Accept-Contact-headers match to the client
	*         represented by this plug-in, otherwise EFalse. 
	*/
	TBool MatchAcceptContactsL(
            RStringF aMethod,
            const CUri8& aRequestUri,
            const RPointerArray<CSIPHeaderBase>& aHeaders,
            const TDesC8& aContent,
            const CSIPContentTypeHeader* aContentType,
            TUid& aClientUid );
	
    /**
    * Matches the Event-header to the client(s) represented by this plug-in.
    * This function is called for an incoming SIP request, 
    * if it contains an Event-header and 
    * MatchAcceptContactsL returned EFalse.
    * 
    * @param aMethod the method of the SIP request
	* @param aRequestUri the request-URI of the SIP request
	* @param aHeaders all the headers in the SIP request
	* @param aContent SIP request body; 
	*        zero-length descriptor if not present
	* @param aContentType the content-type of the SIP request. 
	*        Zero-pointer if body is not present.
	* @param aClientUid indicates client's UID for 
	*        SIP e.g. the one passed as a parameter to CSIP::NewL().
	* @return ETrue, if the Event-header matches to the client
	*         represented by this plug-in, otherwise EFalse. 
	*/
	TBool MatchEventL(
            RStringF aMethod,
            const CUri8& aRequestUri,
            const RPointerArray<CSIPHeaderBase>& aHeaders,
            const TDesC8& aContent,
            const CSIPContentTypeHeader* aContentType,
            TUid& aClientUid );
	
	/**
    * Matches the whole SIP request to the client(s)
	* represented by this plug-in.
	* This function is called if the SIP request does not contain
	* Accept- or Event-headers or
	* MatchAcceptContactsL and MatchEventL returned EFalse.
	* 
	* @param aMethod the method of the SIP request
	* @param aRequestUri the request-URI of the SIP request
	* @param aHeaders all the headers in the SIP request
	* @param aContent SIP request body; 
	*        zero-length descriptor if not present
	* @param aContentType the content-type of the SIP request. 
	*        Zero-pointer if body is not present.
	* @param aClientUid indicates client's UID for 
	*        SIP e.g. the one passed as a parameter to CSIP::NewL().
	* @return ETrue, if the request can be handled by the client
	*         represented by this plug-in, otherwise EFalse. 
	*/
	TBool MatchRequestL(
            RStringF aMethod,
            const CUri8& aRequestUri,
            const RPointerArray<CSIPHeaderBase>& aHeaders,
            const TDesC8& aContent,
            const CSIPContentTypeHeader* aContentType,
            TUid& aClientUid );
	
	/**
	* Indicates whether the plug-in implements CSIPResolvedClient2::ConnectL
	* and by calling CSIPResolvedClient2::ConnectL 
	* SIP stack is able to force the client to connect to SIP stack.
	* 
	* @return ETrue, if the plug-in supports
	*         CSIPResolvedClient2::ConnectL, otherwise EFalse.
	*/
	TBool ConnectSupported();
	
	/**
	* Requests the client to connect to SIP with resolved
	* UID in case there's no connection with resolved channel UID.
	* 
	* @param aClientUid previously resolved channel UID
	* @leave KErrNoMemory if out of memory
	* @leave KErrNotFound in case non-existing channel UID was provided
	*/
	void ConnectL( const TUid& aClientUid );
	
	/**
	* Cancels a ConnectL request for a client.
	* Is called when for example a CANCEL for an INVITE is received
	* before the client connects to SIP stack.
	* 
	* @param aClientUid a UID for which ConnectL was previously called
	*/
	void CancelConnect( const TUid& aClientUid );
	
	/**
	* Gets all the SIP message content types supported by the client.
	* 
	* @return 0..n SIP Content-Type-headers.
	*         The ownership of the headers is transferred.
	*/
	RPointerArray<CSIPContentTypeHeader> SupportedContentTypesL();
	
	/**
	* Gets all the SDP media-fields supported by the client.
	* 
	* @return 0..n SDP media-fields describing the client's media support.
	*         The ownership of the media-fields is transferred.
	*/
	RPointerArray<CSdpMediaField> SupportedSdpMediasL();
	
	/**
    * Adds client specific SIP-headers for the 200 OK for OPTIONS.
	* Each plug-in must check that the header to be added
	* is not yet in the array. For example when adding header
	* "Allow: INVITE" the client must check that
	* the header is not already present in the array.
	* 
	* @param aHeaders headers to be added to 200 OK for OPTIONS.
	*        The ownership of the added headers is transferred to the caller.
	*/
	void AddClientSpecificHeadersForOptionsResponseL(
            RPointerArray<CSIPHeaderBase>& aHeaders );


private: // Constructors

	/**
    * C++ default constructor.
	*/
	CSipMXResolver();
	
	/**
	* By default Symbian 2nd phase constructor is private.
	*/
	void ConstructL();


private: // New functions

	/**
	* Checks given header array for accept-contact (AC) headers and
	* returns ETrue if there is an AC header and it contains
	* given feature tag.
	* 
	* @param aHeaders Headers from request to check
	* @param aTag Feature specific tag to check AC header for
	* @ret ETrue if the AC header contain the given feature tag
	*/
	TBool CheckForACHeaderTagL(
            const RPointerArray<CSIPHeaderBase>& aHeaders,
            const TDesC8& aTag ) const;
	
	/**
	* Checks a SDP document for specified media type lines
	* 
	* @param aSpdDoc pointer to sdp document to check for medias
	* @param aMediaType specifies the media type to check for
	* @return ETrue if aSpdDoc contains m lines with
	*         given media type.
	*/
	TBool CheckForMedia(
            CSdpDocument* aSdpDoc,
            const RStringF& aMediaType ) const;

	/**
	* Checks given sdp media fields array for VS specific attributes
	* 
	* @aFields array of media fields to check
	* @return ETrue if the given media fields contain
	*         VS specific attributes
	*/
	TBool CheckForVSAttributes(
            RPointerArray<CSdpMediaField>& aFields ) const;
	
	/**
	* Checks if a CS call is ongoing. This information is
	* used in the resolving logic.
	* 
	* @return ETrue if there is an active CS call
	*/
	TBool IsCSCallActive() const;
	
	/**
	* Resolves the Uid to return in VS cases based on request method
	* 
	* @param aMethod SIP request method
	* @return Uid that corresponds to the given method. KNullUid if no match.
	*/
	TUid ResolveVSUidL( const RStringF& aMethod );
	
	/**
	* Resolves the Uid of CCP plugin if VoIP is recogniced
	* 
	* @param aUid Resolved UID
	* @param aRequestUri URI received in request
	*/
	void ResolveCPPluginUidL( TUid& aUid, const CUri8& aRequestUri );


private: // Data
        
	///own: VoIP availability status
	TBool iVoIPEnabled;
	///own: Multimedia Sharing client for Uid resolving
	CMusManager* iMuSManager;
	///own: RStringF handles to video/audio media strings from SIPStrings
	RStringF iVideoType;
	RStringF iAudioType;
	///own: String pool closing control; if ETrue, pool is closed in dtor
	TBool iCloseStringPool;
	
	SIPMXR_TEST( UT_CSipMXResolver ) // for unit tests
	
    };

#endif  // CSIPMXRESOLVER_H   
            
// End of File

