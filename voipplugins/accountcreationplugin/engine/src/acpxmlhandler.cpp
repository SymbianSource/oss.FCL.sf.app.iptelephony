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
* Description:  Implements CAcpXmlHandler methods
*
*/


#include <escapeutils.h>
#include "acpprovider.h"
#include "acpxmlhandler.h"
#include "accountcreationpluginlogger.h"
#include "macpxmlhandlerobserver.h"
#include "accountcreationengineconstants.h"


// ---------------------------------------------------------------------------
// CAcpXmlHandler::CAcpXmlHandler
// ---------------------------------------------------------------------------
//
CAcpXmlHandler::CAcpXmlHandler( MAcpXmlHandlerObserver& aObserver )
    : CActive( EPriorityStandard ),
    iObserver( aObserver )
    {
    // Add active scheduler.
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CAcpXmlHandler::ConstructL
// ---------------------------------------------------------------------------
//
void CAcpXmlHandler::ConstructL()
    {
    ACPLOG( "CAcpXmlHandler::ConstructL begin" );
    // Create XML parser
    iParser = CParser::NewL( KXmlMimeType, *this );
    ACPLOG( "CAcpXmlHandler::ConstructL end" );
    }

// ---------------------------------------------------------------------------
// CAcpXmlHandler::NewL
// ---------------------------------------------------------------------------
//
CAcpXmlHandler* CAcpXmlHandler::NewL( MAcpXmlHandlerObserver& aObserver )
    {
    CAcpXmlHandler* self = CAcpXmlHandler::NewLC( aObserver );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CAcpXmlHandler::NewLC
// ---------------------------------------------------------------------------
//
CAcpXmlHandler* CAcpXmlHandler::NewLC( MAcpXmlHandlerObserver& aObserver )
    {
    CAcpXmlHandler* self = new ( ELeave ) CAcpXmlHandler( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CAcpXmlHandler::~CAcpXmlHandler
// ---------------------------------------------------------------------------
//
CAcpXmlHandler::~CAcpXmlHandler()
    {
    ACPLOG( "CAcpXmlHandler::~CAcpXmlHandler begin" );

    Cancel(); // Ensures that the parsing is stopped
    delete iParser;
    delete iBuffer;
    delete iOngoingAttr;
    delete iProvider;

    ACPLOG( "CAcpXmlHandler::~CAcpXmlHandler end" );
    }

// ---------------------------------------------------------------------------
// CAcpXmlHandler::StartParsingL
// Starts parsing xml with given filename.
// ---------------------------------------------------------------------------
//
void CAcpXmlHandler::StartParsingL( const TFileName& aFilename )
    {
    ACPLOG( "CAcpXmlHandler::StartParsingL begin" );

    // Save the filename because of deleting file later on.
    iFilename.Append( aFilename );

    // Leave if parsing is active.
    if ( IsActive() )
        {
        User::Leave( KErrInUse );
        }

    // Connect to fileserver and open xml file to be parsed.    
    User::LeaveIfError( iFileServer.Connect() );
    User::LeaveIfError( iFile.Open( iFileServer, aFilename, EFileRead ) );

    delete iBuffer;
    iBuffer = NULL;    

    TEntry entry;

    // Get size of file.
    User::LeaveIfError( iFileServer.Entry( aFilename, entry ) );

    TInt fileSize = entry.iSize;

    // Create buffer for file reading and start process.
    iBuffer = HBufC8::NewL( fileSize );

    TPtr8 ptr( iBuffer->Des() );
    ptr.Zero();

    iFile.Read( ptr, fileSize, iStatus );
    SetActive();

    iParser->ParseBeginL();

    ACPLOG( "CAcpXmlHandler::StartParsingL end" );
    }

// ---------------------------------------------------------------------------
// CAcpXmlHandler::HandleFinishedProviderL
// Handles finished provider and notifies observer.
// ---------------------------------------------------------------------------
//
void CAcpXmlHandler::HandleFinishedProviderL()
    {
    ACPLOG( "CAcpXmlHandler::HandleFinishedProviderL" );

    // Provider exists, save it by calling observer.
    if ( iProvider )
        {
        iObserver.NotifyParsedProviderL( *iProvider );    
        delete iProvider;
        iProvider = NULL;
        }

    // Start new provider by creating instance from CAcpProvider.
    iProvider = CAcpProvider::NewL();
    }

// ---------------------------------------------------------------------------
// CAcpXmlHandler::DoCancel
// From CActive.
// ---------------------------------------------------------------------------
//
void CAcpXmlHandler::DoCancel()
    {
    ACPLOG( "CAcpXmlHandler::DoCancel begin" );
    
    ACPLOG( "CAcpXmlHandler::DoCancel - ReadCancel" );
    iFile.ReadCancel();
    
    TRAP_IGNORE( iParser->ParseEndL() );

    iFileServer.Delete( iFilename ); // Delete old file

    iFile.Close();
    iFileServer.Close();

    delete iBuffer;
    iBuffer = NULL;

    delete iOngoingAttr;
    iOngoingAttr = NULL;

    ACPLOG( "CAcpXmlHandler::DoCancel end" );
    }

// ---------------------------------------------------------------------------
// CAcpXmlHandler::RunL
// From CActive.
// ---------------------------------------------------------------------------
//
void CAcpXmlHandler::RunL()
    {
    ACPLOG( "CAcpXmlHandler::RunL begin" );

    if ( KErrNone == iStatus.Int() )
        {
        // When buffer length is zero, we have reached end of the document.
        if ( !iBuffer->Length() )
            {
            ACPLOG( " - end of XML document reached" );
            DoCancel();
            }
        // Otherwise continue reading of Xml file.
        else
            {
            iParser->ParseL( *iBuffer );
            TPtr8 ptr( iBuffer->Des() );
            iFile.Read( ptr, iStatus );
            SetActive();
            ACPLOG( " - continuing reading XML document" );
            }
        }
    else
        {
        iObserver.NotifyParsingCompleted( iStatus.Int() );
        }

    ACPLOG( "CAcpXmlHandler::RunL end" );
    }
    

// ---------------------------------------------------------------------------
// CAcpXmlHandler::OnStartDocumentL
// From MContentHandler.
// ---------------------------------------------------------------------------
//
void CAcpXmlHandler::OnStartDocumentL( 
    const RDocumentParameters& /*aDocParam*/,
    TInt aErrorCode )
    {
    if ( KErrNone != aErrorCode )
        {
        ACPLOG2( "CAcpXmlHandler::OnStartDocumentL: error=%d", aErrorCode );
        iObserver.NotifyParsingCompleted( aErrorCode );
        }
    }

// ---------------------------------------------------------------------------
// CAcpXmlHandler::OnEndDocumentL
// From MContentHandler.
// ---------------------------------------------------------------------------
//
void CAcpXmlHandler::OnEndDocumentL( TInt aErrorCode )
    {
    ACPLOG2( "CAcpXmlHandler::OnEndDocumentL begin: error=%d", aErrorCode );
    
    if ( KErrNone == aErrorCode )
        {
        // Save last one.
        HandleFinishedProviderL();
        delete iProvider;
        iProvider = NULL;
        }

    iObserver.NotifyParsingCompleted( aErrorCode );
    
    ACPLOG( "CAcpXmlHandler::OnEndDocumentL end" );
    }

// ---------------------------------------------------------------------------
// CAcpXmlHandler::OnStartElementL
// From MContentHandler.
// ---------------------------------------------------------------------------
//
void CAcpXmlHandler::OnStartElementL( 
    const RTagInfo& aElement,
    const RAttributeArray& , 
    TInt aErrorCode )
    {
    TBuf<KMaxElementLength>element;
    element.Copy(aElement.LocalName().DesC());

    ACPLOG2( "CAcpXmlHandler::OnStartElementL: element=%S", &element );
    ACPLOG2( "CAcpXmlHandler::OnStartElementL: error=%d", aErrorCode );

    if ( KErrNone == aErrorCode )
        {
        // No need to go through attributes since they're not used.
        // Parsing has reached to some of supported elements.
        // Save element value to member data for further actions.
        TPtrC8 ptr = aElement.LocalName().DesC();

        if ( !ptr.Compare( KServiceProvider ) ||
            !ptr.Compare( KProviderName ) ||
            !ptr.Compare( KIconUrl ) ||
            !ptr.Compare( KSisUrl ) ||
            !ptr.Compare( KCreateUrl ) ||
            !ptr.Compare( KActivateUrl ) ||
            !ptr.Compare( KProviderDescription ) ||
            !ptr.Compare( KProviderType ) ||
            !ptr.Compare( KReportUrl ) )
            {
            iOngoingAttr = HBufC8::NewL( ptr.Length() );
            iOngoingAttr->Des().Copy( ptr );
            iGetContentNow = ETrue;
            // just for creating provider
            if ( !ptr.Compare( KServiceProvider ) )
                {
                iGetContentNow = EFalse;
                }
            }

        if ( iOngoingAttr )
            {
            // Parsing of provider has been started.
            // This means that old provider is ready for saving 
            // if it's not first one. In that case, new provider is started.
            if ( !iOngoingAttr->Compare( KServiceProvider ) )
                {
                HandleFinishedProviderL();
                delete iOngoingAttr;
                iOngoingAttr = NULL;
               }
            }
        }
    else
        {
        ACPLOG2( "CAcpXmlHandler::OnStartElementL: error=%d", aErrorCode );
        iObserver.NotifyParsingCompleted( aErrorCode );
        }
    }

// ---------------------------------------------------------------------------
// CAcpXmlHandler::OnEndElementL
// From MContentHandler.
// ---------------------------------------------------------------------------
//
void CAcpXmlHandler::OnEndElementL( const RTagInfo& /*aElement*/, 
    TInt aErrorCode )
    {
    if ( KErrNone == aErrorCode )
        {
        // Reset member variables.
        iGetContentNow = EFalse;

        delete iOngoingAttr;
        iOngoingAttr = NULL;
        }
    else
        {
        ACPLOG2( "CAcpXmlHandler::OnEndElementL: error=%d", aErrorCode );
        iObserver.NotifyParsingCompleted( aErrorCode );
        }
    }

// ---------------------------------------------------------------------------
// CAcpXmlHandler::OnContentL
// From MContentHandler.
// ---------------------------------------------------------------------------
//
void CAcpXmlHandler::OnContentL( const TDesC8& aBytes, TInt aErrorCode )
    {
    if ( KErrNone == aErrorCode )
        {
        ACPLOG( "CAcpXmlHandler::OnContentL" );
        // We need to save content data if iGetContentNow is set.
        if ( iGetContentNow )
            {
            // Convert unicode data to 16-bit descriptor.
            HBufC* bytes = EscapeUtils::ConvertToUnicodeFromUtf8L( aBytes );

            // On the cleanup stack
            CleanupStack::PushL( bytes );

            ACPLOG2( "CAcpXmlHandler::OnContentL: aBytes=%S", bytes );

            // Provider name.
            if ( !iOngoingAttr->Compare( KProviderName ) )
                {
                ACPLOG( "CAcpXmlHandler::SetProviderNameL" );
                iProvider->SetProviderNameL( *bytes );
                }
            // Location of icons package.   
            else if ( !iOngoingAttr->Compare( KIconUrl ) )
                {
                ACPLOG( "CAcpXmlHandler::SetIconUrlL" );
                iProvider->SetIconUrlL( aBytes );
                }
            // Location of sis package.
            else if ( !iOngoingAttr->Compare( KSisUrl ) )
                {
                ACPLOG( "CAcpXmlHandler::SetSisUrlL" );
                iProvider->SetSisUrlL( aBytes );
                }
            // Location to create settings.    
            else if ( !iOngoingAttr->Compare( KCreateUrl ) )
                {
                ACPLOG( "CAcpXmlHandler::SetCreationUrlL" );
                iProvider->SetCreationUrlL( aBytes );
                }
            // Location to activate settings.    
            else if ( !iOngoingAttr->Compare( KActivateUrl ) )
                {
                ACPLOG( "CAcpXmlHandler::SetActivationUrlL" );
                iProvider->SetActivationUrlL( aBytes );
                }
            // Provider description.    
            else if ( !iOngoingAttr->Compare( KProviderDescription ) )
                {
                ACPLOG( "CAcpXmlHandler::SetProviderDescription" );
                iProvider->SetProviderDescriptionL( *bytes );
                }            
            // Provider type.    
            else if ( !iOngoingAttr->Compare( KProviderType ) )
                {
                ACPLOG( "CAcpXmlHandler::SetProviderType" );
                iProvider->SetProviderTypeL( *bytes );
                }            
            // Location to activate settings.    
            else if ( !iOngoingAttr->Compare( KReportUrl ) )
                {
                ACPLOG( " - Provider report url ignored" );
                }            
            // Should never happen.    
            else
                {
                User::Leave( KErrNotSupported );
                }
            CleanupStack::PopAndDestroy( bytes );
            }
        }
    else
        {
        ACPLOG2( "CAcpXmlHandler::OnContentL: error=%d", aErrorCode );
        iObserver.NotifyParsingCompleted( aErrorCode );
        }
    }

// ---------------------------------------------------------------------------
// CAcpXmlHandler::OnStartPrefixMappingL
// From MContentHandler.
// ---------------------------------------------------------------------------
//
void CAcpXmlHandler::OnStartPrefixMappingL( 
    const RString& /*aPrefix*/, 
    const RString& /*aUri*/,
    TInt /*aErrorCode*/ )
    {
    }

// ---------------------------------------------------------------------------
// CAcpXmlHandler::OnEndPrefixMappingL
// From MContentHandler.
// ---------------------------------------------------------------------------
//
void CAcpXmlHandler::OnEndPrefixMappingL( 
    const RString& /*aPrefix*/, 
    TInt /*aErrorCode*/ )
    {
    }

// ---------------------------------------------------------------------------
// CAcpXmlHandler::OnIgnorableWhiteSpaceL
// From MContentHandler.
// ---------------------------------------------------------------------------
//
void CAcpXmlHandler::OnIgnorableWhiteSpaceL( 
    const TDesC8& /*aBytes*/, 
    TInt /*aErrorCode*/ )
    {
    }

// ---------------------------------------------------------------------------
// CAcpXmlHandler::OnSkippedEntityL
// From MContentHandler.
// ---------------------------------------------------------------------------
//
void CAcpXmlHandler::OnSkippedEntityL( 
    const RString& /*aName*/, 
    TInt /*aErrorCode*/ )
    {
    }

// ---------------------------------------------------------------------------
// CAcpXmlHandler::OnProcessingInstructionL
// From MContentHandler.
// ---------------------------------------------------------------------------
//
void CAcpXmlHandler::OnProcessingInstructionL( 
    const TDesC8& /*aTarget*/, 
    const TDesC8& /*aData*/,
    TInt /*aErrorCode*/ )
    {
    }

// ---------------------------------------------------------------------------
// CAcpXmlHandler::OnError
// From MContentHandler.
// ---------------------------------------------------------------------------
//
void CAcpXmlHandler::OnError( TInt aErrorCode )
    {
    ACPLOG2( "CAcpXmlHandler::OnError: error=%d", aErrorCode );

    // Send a notification that the parsing stopped because of error.
    iObserver.NotifyParsingCompleted( aErrorCode );
    }

// ---------------------------------------------------------------------------
// CAcpXmlHandler::GetExtendedInterface
// From MContentHandler.
// ---------------------------------------------------------------------------
//
TAny* CAcpXmlHandler::GetExtendedInterface( const TInt32 /*aUid*/ )
    {
    return NULL;
    }

// End of file.
