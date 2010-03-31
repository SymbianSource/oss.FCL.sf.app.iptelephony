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
* Description:  VoIP settings XML handler
*
*/


#include <e32base.h>
#include <gmxmldocument.h>
#include <gmxmltext.h>
#include <e32cmn.h>
#include <e32def.h>

#include "voipxmlparser.h"
#include "voipxmlparamhandler.h"
#include "voipxmlprocessordefaults.h"
#include "voipxmlprocessorlogger.h"


// ---------------------------------------------------------------------------
// CVoipXmlParser::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CVoipXmlParser* CVoipXmlParser::NewL()
    {
    CVoipXmlParser* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CVoipXmlParser::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CVoipXmlParser* CVoipXmlParser::NewLC()
    {
    CVoipXmlParser* self = new ( ELeave ) CVoipXmlParser();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CVoipXmlParser::ConstructL
// ---------------------------------------------------------------------------
//
void CVoipXmlParser::ConstructL()
    {
    DBG_PRINT( "CVoipXmlParser::ConstructL begin" );

    iParser = CMDXMLParser::NewL( this );
    iParamHandler = CVoipXmlParamHandler::NewL();
    iDescription = HBufC::NewL( 0 );
    iBrandUrl = HBufC::NewL( 0 );

    DBG_PRINT( "CVoipXmlParser::ConstructL end" );
    }

// ---------------------------------------------------------------------------
// CVoIPXmlHandler::~CVoIPXmlHandler()
// Destructor
// ---------------------------------------------------------------------------
//
CVoipXmlParser::~CVoipXmlParser()
    {
    delete iParser;
    delete iParamHandler;
    if ( iXmlDoc )
        {
        delete iXmlDoc;
        }
    if ( iDescription )
        {
        delete iDescription;
        }
    if( iBrandUrl )
        {
        delete iBrandUrl;
        }
    }

// ---------------------------------------------------------------------------
// CVoipXmlParser::CVoipXmlParser
// ---------------------------------------------------------------------------
//
CVoipXmlParser::CVoipXmlParser()
    {
    }

// ---------------------------------------------------------------------------
// CVoipXmlParser::ImportDocumentL
// Reads document.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CVoipXmlParser::ImportDocument( RFile& aFile, 
    TUint32& aServiceTabId )
    {
    DBG_PRINT( "CVoipXmlParser::ImportDocumentL - begin" );
    iParseDescription = EFalse;
    iParser->ParseFile( aFile );
    CActiveScheduler::Start();
    if ( KErrNone == iError )
        {
        iError = iParamHandler->StoreSettings();
        }
    if ( KErrNone == iError )
        {
        // Give other applications 2 seconds to react
        // to the newly created service.
        const TInt KTwoSeconds( 2000000 );
        User::After( KTwoSeconds );
        aServiceTabId = iParamHandler->ServiceTabId();
        }
    DBG_PRINT2( "CVoipXmlParser::ImportDocumentL - end (%d)", iError );
    return iError;
    }

// ---------------------------------------------------------------------------
// CVoipXmlParser::ReadDescription
// Reads document description.
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC* CVoipXmlParser::ReadDescriptionLC( RFile& aFile, 
    TDes8& aBrandUrl )
    {
    DBG_PRINT( "CVoipXmlParser::ReadDescriptionLC begin" );
    iParseDescription = ETrue;
    iParser->ParseFile( aFile );
    CActiveScheduler::Start();
    if ( !iDescription )
        {
        iDescription = HBufC::NewL( 0 );
        }
    HBufC* buf = HBufC::NewLC( iDescription->Des().Length() );
    buf->Des().Copy( iDescription->Des() );
    if ( !iBrandUrl )
        {
        iBrandUrl = HBufC::NewL( 0 );
        }
    aBrandUrl.Copy( iBrandUrl->Des() );
    DBG_PRINT( "CVoipXmlParser::ReadDescriptionLC end" );
    return buf;
    }

// ---------------------------------------------------------------------------
// From base class MMDXMLParserObserver.
// CVoipXmlParser::ParseFileCompleteL
// ---------------------------------------------------------------------------
//
void CVoipXmlParser::ParseFileCompleteL()
    {
    DBG_PRINT( "CVoipXmlParser::ParseFileCompleteL - begin" );
    // We may have already received XML document from iParser since we
    // commit two parsing operations (first the description and then the
    // settings). Thus we need to delete iXmlDoc because iParser has given
    // its responsibility here.
    if ( iXmlDoc )
        {
        delete iXmlDoc;
        iXmlDoc = NULL;
        }
    iXmlDoc = iParser->DetachXMLDoc();
    CActiveScheduler::Stop();
    if ( !iXmlDoc )
        {
        iError = KErrCorrupt;
        return;
        }
    CMDXMLElement* xmlElement = iXmlDoc->DocumentElement();
    if ( !xmlElement )
        {
        iError = KErrCorrupt;
        DBG_PRINT( "CVoipXmlParser::ParseFileCompleteL - end KErrCorrupt" );
        return;
        }

    if ( iParseDescription )
        {
        ParseDescription( *xmlElement );
        }
    else
        {
        ParseRoot( *xmlElement );
        }
    DBG_PRINT( "CVoipXmlParser::ParseFileCompleteL - end" );
    }

// ---------------------------------------------------------------------------
// Parses description node.
// ---------------------------------------------------------------------------
//
void CVoipXmlParser::ParseDescription( CMDXMLElement& aXmlElement )
    {
    CMDXMLNode* node = aXmlElement.FirstChild();
    if ( !node )
        {
        iError = KErrCorrupt;
        return;
        }
    TBuf<KMaxNodeNameLength> nodeName;
    nodeName.Copy( node->NodeName() );
    nodeName.LowerCase();

    // <nokia-confdoc> is the root node; thus parse it first.
    while ( nodeName != KNodeNokiaConfDoc && node )
        {
        node = node->NextSibling();
        if ( !node )
            {
            DBG_PRINT( "CVoipXmlParser::ParseRoot - end KErrEof" );
            iError = KErrEof;
            return;
            }
        nodeName.Copy( node->NodeName() );
        nodeName.LowerCase();
        }
    // After <nokia-confdoc> has been found, search for <confdescription>
    // and <confbrandurl> from its children.
    node = node->FirstChild();
    while ( node )
        {
        nodeName.Copy( node->NodeName() );
        nodeName.LowerCase();
        CMDXMLNode* child = node->FirstChild();
        //lint -e{960} No need for else statement here
        if ( KNodeConfDescription() == nodeName && child && 
            CMDXMLNode::ETextNode == child->NodeType() )
            {
            CMDXMLText* text = NULL;
            text = static_cast<CMDXMLText*>( child );
            delete iDescription;
            iDescription = NULL;
            TRAP_IGNORE( iDescription = text->Data().AllocL() );
            }
        else if ( KNodeConfBrandUrl() == nodeName && child && 
            CMDXMLNode::ETextNode == child->NodeType() )
            {
            CMDXMLText* text = NULL;
            text = static_cast<CMDXMLText*>( child );
            delete iBrandUrl;
            iBrandUrl = NULL;
            TRAP_IGNORE( iBrandUrl = text->Data().AllocL() );
            }
        node = node->NextSibling();
        }
    }

// ---------------------------------------------------------------------------
// CVoipXmlParser::ParseRoot
// Parses root node.
// ---------------------------------------------------------------------------
//
void CVoipXmlParser::ParseRoot( CMDXMLElement& aXmlElement )
    {
    DBG_PRINT( "CVoipXmlParser::ParseRoot - begin" );

    CMDXMLNode* node = aXmlElement.FirstChild();
    if ( !node )
        {
        iError = KErrCorrupt;
        return;
        }
    TBuf<KMaxNodeNameLength> nodeName;
    nodeName.Copy( node->NodeName() );
    nodeName.LowerCase();

    while ( nodeName != KNodeNokiaConfDoc )
        {
        node = node->NextSibling();
        if ( !node )
            {
            DBG_PRINT( "CVoipXmlParser::ParseRoot - end KErrEof" );
            iError = KErrEof;
            return;
            }
        nodeName.Copy( node->NodeName() );
        nodeName.LowerCase();
        }

    node = node->FirstChild();

    TInt settingsCount( 0 );
    while ( node )
        {
        nodeName.Copy( node->NodeName() );
        nodeName.LowerCase();
        // At this level in XML tree only 'settings' nodes are allowed.
        if ( nodeName == KNodeSettings )
            {
            ParseSettings( node );
            settingsCount++;
            }
        node = node->NextSibling();
        }
    if ( !settingsCount )
        {
        iError = KErrAbort;
        }
    DBG_PRINT( "CVoipXmlParser::ParseRoot - end" );

    }

// ---------------------------------------------------------------------------
// CVoipXmlParser::ParseSettingsL
// Parses 'settings' nodes.
// ---------------------------------------------------------------------------
//
void CVoipXmlParser::ParseSettings( CMDXMLNode* aXmlNode )
    {
    DBG_PRINT( "CVoipXmlParser::ParseSettings - begin" );
    if ( CMDXMLNode::EElementNode != aXmlNode->NodeType() )
        {
        DBG_PRINT( 
            "CVoipXmlParser::ParseSettings - node type not element, end" );
        iError = KErrBadHandle;
        return;
        }

    CMDXMLElement* element = NULL;
    element = static_cast<CMDXMLElement*>( aXmlNode );
    TPtrC attributeValue;
    element->GetAttribute( KAttributeType(), attributeValue );
    if ( !attributeValue.Length() )
        {
        DBG_PRINT( 
            "CVoipXmlParser::ParseSettings - attribute type not ok, end" );
        iError = KErrBadName;
        }

    iParamHandler->SettingsStart( attributeValue );


    CMDXMLNode* node = element->FirstChild();

    while ( node )
        {
        if ( node->NodeType() == CMDXMLNode::EElementNode )
            {

            TBuf<KMaxNodeNameLength> nodeName;
            nodeName.Copy( node->NodeName() );
            nodeName.LowerCase();
            element = NULL;
            element = static_cast<CMDXMLElement*>( node );

            //lint -e{960} No need for else statement here
            if ( KNodeParam() == nodeName )
                {
                TPtrC nameVal;
                element->GetAttribute( KAttributeName(), nameVal );
                if ( nameVal.Length() )
                    {
                    TPtrC attVal;
                    element->GetAttribute( KAttributeValue(), attVal );
                        {
                        if ( attVal.Length() )
                            {
                            iParamHandler->HandleParameter( 
                                nameVal, attVal );
                            }
                        }
                    }
                }
            else if ( KNodeSettings() == nodeName )
                {
                ParseSettings( element );
                }
            }
        node = node->NextSibling();
        }

    iParamHandler->SettingsEnd();

    DBG_PRINT( "CVoipXmlParser::ParseSettings - end" );
    }

// End of file.
