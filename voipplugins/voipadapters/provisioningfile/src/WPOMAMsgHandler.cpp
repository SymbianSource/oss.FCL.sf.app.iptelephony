/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Used by ProvisioningFile to read provisioning messages.
*
*/


// INCLUDE FILES
#include    "WPOMAMsgHandler.h"
#include    "wpprovisioningfiledebug.h"

#include    <e32std.h>
#include    <f32file.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWPOMAMsgHandler::CWPOMAMsgHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWPOMAMsgHandler::CWPOMAMsgHandler()
    : iProvData( NULL, 0 )
    {
    }

// -----------------------------------------------------------------------------
// CWPOMAMsgHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWPOMAMsgHandler::ConstructL()
    {
    FLOG( "[ProvisioningFile] CWPOMAMsgHandler::ConstructL" );
    User::LeaveIfError( iFs.Connect() );
    iProvisioningDoc = HBufC8::NewL( 0 );
    }
    
// -----------------------------------------------------------------------------
// CWPOMAMsgHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWPOMAMsgHandler* CWPOMAMsgHandler::NewL()
    {
    CWPOMAMsgHandler* self = CWPOMAMsgHandler::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CWPOMAMsgHandler::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWPOMAMsgHandler* CWPOMAMsgHandler::NewLC()
    {
    CWPOMAMsgHandler* self = new( ELeave ) CWPOMAMsgHandler;    
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CWPOMAMsgHandler::~CWPOMAMsgHandler
// Destructor
// -----------------------------------------------------------------------------
//
CWPOMAMsgHandler::~CWPOMAMsgHandler()
    {
    if ( iProvisioningDoc )
        {
        delete iProvisioningDoc;
        iProvisioningDoc = NULL;
        }
    
    iFs.Close();
    }

// -----------------------------------------------------------------------------
// CWPOMAMsgHandler::ReadProvFileL
//
// -----------------------------------------------------------------------------
//
void CWPOMAMsgHandler::ReadProvFileL( TFileName aConfig )
    {
    FLOG( "[ProvisioningFile] CWPOMAMsgHandler::ReadProvFileL" );

    TInt fileSize( 0 );

    iFileName = aConfig;
    User::LeaveIfError( GetFileSize( fileSize ) );
      
    if ( fileSize )
        {
        delete iProvisioningDoc;
        iProvisioningDoc = NULL;
        iProvisioningDoc = HBufC8::NewL( fileSize );
        iProvData.Set( iProvisioningDoc->Des() );
        
	    User::LeaveIfError( iFile.Open( iFs, iFileName, EFileShareReadersOnly ) );
	    User::LeaveIfError( iFile.Read( iProvData ) );
	    iFile.Close();
        }
	}

// -----------------------------------------------------------------------------
// CWPOMAMsgHandler::DocL
//
// -----------------------------------------------------------------------------
//
HBufC8* CWPOMAMsgHandler::DocL()
    {
    FLOG( "[ProvisioningFile] CWPOMAMsgHandler::DocL" );
    
    if( iProvisioningDoc )
        {
        return iProvisioningDoc->AllocL();
        }
    else
        {
        return NULL;
        }
    }

// -----------------------------------------------------------------------------
// CWPOMAMsgHandler::GetFileSize
// 
// -----------------------------------------------------------------------------
//
TInt CWPOMAMsgHandler::GetFileSize( TInt& aSize )
    {
    FLOG( "[ProvisioningFile] CWPOMAMsgHandler::GetFileSize" );
    
    TInt err( iFile.Open( iFs, iFileName , EFileShareReadersOnly ) );
    if ( KErrNone == err )
        {
        err = iFile.Size( aSize );
        iFile.Close();
        }
    return err;
    }

//  End of File
