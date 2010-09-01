/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declarition of CAcpXmlHandler
*
*/

 
#ifndef _ACPXMLHANDLER
#define _ACPXMLHANDLER

#include <e32base.h>
#include <f32file.h>
#include <xml/parser.h>
#include <xml/parserfeature.h>
#include <xml/contenthandler.h>

using namespace Xml;

class CAcpProvider;
class MAcpXmlHandlerObserver;

/**
 *  CAcpXmlHandler class
 *  Declarition of CAcpXmlHandler.
 *
 *  @lib accountcreationplugin.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CAcpXmlHandler ) : public CActive, MContentHandler
    {
public: 

    /**
     * Two-phased constructor.
     *
     * @param aObserver for notifying xml events
     */
    static CAcpXmlHandler* NewL( MAcpXmlHandlerObserver& aObserver );

    /**
     * Two-phased constructor.
     *
     * @param aObserver for notifying xml events
     */
    static CAcpXmlHandler* NewLC( MAcpXmlHandlerObserver& aObserver );

    /**
     * Destructor.
     */
    virtual ~CAcpXmlHandler();

    /**
     * Starts parsing xml with given filename.
     * Function leaves on failure.
     *
     * @since S60 v3.2
     * @param aFilename for file to be parsed
     */
    void StartParsingL( const TFileName& aFilename );

    /**
     * Returns provider information referenced by index.
     *
     * @since S60 v3.2
     * @param aIndex for provider list index
     * @return reference to provider information
     */
    const CAcpProvider& ProviderFromIndexL( TInt aIndex ) const;

private:

    CAcpXmlHandler( MAcpXmlHandlerObserver& aObserver );
    void ConstructL();

    /**
     * Handles finished provider and notifies observer.
     *
     * @since S60 v3.2
     */
    void HandleFinishedProviderL();

// from base class CActive

    /**
     * From CActive.
     * Cancels an outstanding request.
     * 
     * @since S60 v3.2
     */
    void DoCancel();

    /**
     * From CActive.
     * Handles request completion event.
     * 
     * @since S60 v3.2
     */
    void RunL();

// from base class MContentHandler

    /**
     * From MContentHandler.
     * Callback to indicate the start of the document.
     * 
     * @since S60 v3.2
     * @param aDocParam Specifies the various parameters of the document.
     * @param aErrorCode Error code. 
     */
    void OnStartDocumentL( const RDocumentParameters &aDocParam, 
        TInt aErrorCode );

    /**
     * From MContentHandler.
     * Callback to indicate the end of the document.
     * 
     * @since S60 v3.2
     * param aErrorCode Error code.
     */
    void OnEndDocumentL( TInt aErrorCode );

    /**
     * From MContentHandler.
     * Callback to indicate an element has been parsed.
     * 
     * @since S60 v3.2
     * @param aElement Handle to the element's details.
     * @param aAttributes Attributes for the element.
     * @param aErrorCode Error code.
     */
    void OnStartElementL( const RTagInfo &aElement, 
        const RAttributeArray &aAttributes,
        TInt aErrorCode );

    /**
     * From MContentHandler.
     * Callback to indicate the end of the element has been reached.
     * 
     * @since S60 v3.2
     * @param Handle to the element's details.
     * @param aErrorCode Error code.
     */
    void OnEndElementL( const RTagInfo &aElement, TInt aErrorCode );

    /**
     * From MContentHandler.
     * Callback that sends the content of the element.
     * Not all the content may be returned in one go. The data may be sent 
     * in chunks. When an OnEndElementL is received this means there is no
     *  more content to be sent.
     * 
     * @since S60 v3.2
     * @param aBytes Raw content data for the element.
     * @param aErrorCode Error code.
     */
    void OnContentL( const TDesC8 &aBytes, TInt aErrorCode );

    /**
     * From MContentHandler.
     * Notification of the beginning of the
     * scope of a prefix-URI Namespace mapping.
     * Not used but here because of inheritance.
     * 
     * @since S60 v3.2
     * @param aPrefix Namespace prefix being declared.
     * @param aUri Namespace URI the prefix is mapped to.
     * @param aErrorCode Error code.
     */
    void OnStartPrefixMappingL( const RString& /*aPrefix*/, 
        const RString& /*aUri*/, TInt /*aErrorCode*/ );

    /**
     * From MContentHandler.
     * Notification of the end of the scope of a prefix-URI mapping.
     * Not used but here because of inheritance.
     * 
     * @since S60 v3.2
     * @param aPrefix Namespace prefix that was mapped.
     * @param aErrorCode Error code.
     */
    void OnEndPrefixMappingL( const RString& /*aPrefix*/, 
        TInt /*aErrorCode*/ );

    /**
     * From MContentHandler.
     * Notification of ignorable whitespace in element content.
     * Not used but here because of inheritance.
     * 
     * @since S60 v3.2
     * @param aBytes Ignored bytes from the document being parsed.
     * @param aErrorCode Error code.
     */
    void OnIgnorableWhiteSpaceL( const TDesC8& /*aBytes*/, 
        TInt /*aErrorCode*/ );

    /**
     * From MContentHandler.
     * Notification of a skipped entity.
     * Not used but here because of inheritance.
     * 
     * @since S60 v3.2
     * @param aName Name of the skipped entity.
     * @param aErrorCode Error code.
     */
    void OnSkippedEntityL( 
        const RString& /*aName*/, 
        TInt /*aErrorCode*/ );

    /**
     * From MContentHandler.
     * Receive notification of a processing instruction.
     * Not used but here because of inheritance.
     * 
     * @since S60 v3.2
     * @param aTarget Processing instruction target.
     * @param aData Processing instruction data. If empty none was supplied.
     * @param aErrorCode Error code.
     */
    void OnProcessingInstructionL( 
        const TDesC8& /*aTarget*/, 
        const TDesC8& /*aData*/,
        TInt /*aErrorCode*/ );

    /**
     * From MContentHandler.
     * Indicates an error has occurred.
     * 
     * @since S60 v3.2
     * @param aErrorCode Error code.
     */
    void OnError( TInt aErrorCode );

    /**
     * From MContentHandler.
     * Obtains the interface matching the specified UID.
     * Not used but here because of inheritance (always returns NULL).
     * 
     * @since S60 v3.2
     * @param aUid UID identifying the required interface.
     */
    TAny* GetExtendedInterface( const TInt32 /*aUid*/ );

private: // data

    /**
     * Reference to observer.
     * Not own.
     */
    MAcpXmlHandlerObserver& iObserver;

    /**
     * Handle to parser object.
     * Own.
     */
    CParser* iParser;

    /**
     * Handle to File Server.
     */
    RFs iFileServer;

    /**
     * Handle to Xml file to be parsed.
     */
    RFile iFile;

    /**
     * Handle to buffer which contains parsed data.
     * Own.
     */
    HBufC8* iBuffer;

    /**
     * Handle to ongoing temporary provider.
     * Own.
     */
    CAcpProvider* iProvider;

    /**
     * Ongoing element. Parsing is reached to this point.
     * Own.
     */
    HBufC8* iOngoingAttr;

    /**
     * Flag indicates that next content data has to be saved.
     */
    TBool iGetContentNow;

    /**
     * XML file to be parsed and then deleted
     */
    TFileName iFilename;
    
    // For unit tests.
#ifdef _DEBUG
    friend class T_CAcpXmlHandler;
#endif
    };

#endif // ACPXMLHANDLER_H

// End of file.
