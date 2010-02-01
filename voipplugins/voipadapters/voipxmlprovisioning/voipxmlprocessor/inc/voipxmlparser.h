/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  VoIP settings XML parser
*
*/


#ifndef VOIPXMLPARSER_H
#define VOIPXMLPARSER_H

#include <GMXMLParser.h>
#include <GMXMLElement.h>
#include <GMXMLNode.h>

#include "voipxmlparamhandler.h"

class CVoipXmlParamHandler;

/**
 *  VoIP settings XML parser.
 *  Parses an XML file and passes the parameters to XML parameter handler.
 *
 *  @code
 *    CVoIPXmlParser* parser = CVoIPXmlParser::NewLC();
 *    TFileName myfile( PathInfo::PhoneMemoryRootPath() );
 *    _LIT( KXmlFilePath, "testsettings.xml" );
 *    myfile.Append( KXmlFilePath );
 *    TUint32 serviceTabId;
 *    TInt err = parser->ImportDocument( myfile, serviceTabId );
 *    CleanupStack::PopAndDestroy( parser );
 *  @endcode
 *
 *  @lib voipxmlprocessor.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CVoipXmlParser ) : public CBase, 
                                      public MMDXMLParserObserver
    {
#ifdef _DEBUG
    friend class UT_CVoipXmlParser;
#endif

public:

    IMPORT_C static CVoipXmlParser* NewL();
    IMPORT_C static CVoipXmlParser* NewLC();
    virtual ~CVoipXmlParser();

    /**
     * Method for importing XML and saving settings.
     *
     * @since S60 v5.0
     * @param aFile File handle to XML file. Ownsership is taken and moved
     *              to CMDXMLParser object.
     * @param aServiceTabId Service tab ID of the created service.
     * @return KErrEof if no <nokia-provdoc> found in XML file,
     *         KErrCorrupt if XML file is corrupted,
     *         KErrAbort if no <settings type=""> found in XML file,
     *         KErrBadHandle if unrecognized node types found in XML file,
     *         KErrNotSupported if no SIP or VoIP settings found,
     *         KErrCompletion if error in storing the settings.
     */
    IMPORT_C TInt ImportDocument( RFile& aFile, TUint32& aServiceTabId );

    /**
     * Reads description from inside <nokia-provdescription> tags
     * and returns it in HBufC object which has been pushed into cleanup
     * stack. Also reads branding URL from XML file and sets it into argument.
     *
     * @since S60 v5.0
     * @param aFile File handle to XML file. Ownsership is taken and moved
     *              to CMDXMLParser object.
     * @param aBrandUrl Branding URL.
     * @return Descriptor object containing the description.
     */
    IMPORT_C HBufC* ReadDescriptionLC( RFile& aFile, TDes8& aBrandUrl );

private:

    /**
     * For parsing nokia-provdescription node.
     *
     * @since S60 v5.0
     * @param aXmlElement XML element to be parsed.
     */
    void ParseDescription( CMDXMLElement& aXmlElement );

    /**
     * For parsing root node.
     *
     * @since S60 v5.0
     * @param aXmlElement XML element to be parsed.
     */
    void ParseRoot( CMDXMLElement& aXmlElement );

    /**
     * For parsing settings node.
     *
     * @since S60 v5.0
     * @param aXmlElement XML element to be parsed.
     */
    void ParseSettings( CMDXMLNode* aXmlNode );

protected:

// From base class MMDXMLParserObserver.

    /**
     * From base class MMDXMLParserObserver.
     * Call back function used to inform a client of the Parser when a
     * parsing operation completes.
     * @since S60 v5.0
     */
    void ParseFileCompleteL();

private:

    CVoipXmlParser();

    void ConstructL();

private: // data

    /**
     * XML parser object.
     * Own.
     */
    CMDXMLParser* iParser;

    /**
     * XML document.
     * Own.
     */
    CMDXMLDocument* iXmlDoc;

    /**
     * XML parameter handler.
     * Own.
     */
    CVoipXmlParamHandler* iParamHandler;

    /**
     * Error code to be returned in ImportDocument.
     */
    TInt iError;

    /**
     * Flag for telling if we are to only read
     * <nokia-provdescription> contents from the XML document.
     */
    TBool iParseDescription;

    /**
     * Object for holding description read from <nokia-provdescription>.
     * Own.
     */
    HBufC* iDescription;

    /**
     * Object for holding branding URL read from <nokia-provbrandurl>.
     * Own.
     */
    HBufC* iBrandUrl;
    };

#endif // VOIPXMLPARSER_H

// End of file.

