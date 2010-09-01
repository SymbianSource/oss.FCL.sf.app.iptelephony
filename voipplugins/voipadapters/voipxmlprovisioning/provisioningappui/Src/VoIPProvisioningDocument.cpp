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
* Description:  Document implementation.
*
*/


// INCLUDE FILES
#include <apgwgnam.h>
#include "VoIPProvisioningDocument.h"
#include "VoIPProvisioningAppUi.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CVoIPProvisioningDocument::~CVoIPProvisioningDocument()
// ----------------------------------------------------------------------------

CVoIPProvisioningDocument::~CVoIPProvisioningDocument()
    {
    }

// ----------------------------------------------------------------------------
// CVoIPProvisioningDocument::CreateAppUiL()
// Creates CVoIPProvisioningAppUi
// ----------------------------------------------------------------------------
CEikAppUi* CVoIPProvisioningDocument::CreateAppUiL()
    {
    // Create new instance of appui class
    return new (ELeave) CVoIPProvisioningAppUi();
    }

// ----------------------------------------------------------------------------
// CVoIPProvisioningDocument::UpdateTaskNameL( CApaWindowGroupName* aWgName )
// ----------------------------------------------------------------------------
void CVoIPProvisioningDocument::UpdateTaskNameL( CApaWindowGroupName* aWgName )
	{
	// Set task name to be hidden from the tasklist
	CAknDocument::UpdateTaskNameL( aWgName );
	aWgName->SetHidden( ETrue );
	}

// ----------------------------------------------------------------------------
// CVoIPProvisioningDocument::OpenFileL
// ----------------------------------------------------------------------------
CFileStore* CVoIPProvisioningDocument::OpenFileL( TBool aDoOpen,
    const TDesC& aFilename, RFs& aFs )
	{
	if ( aDoOpen )
	    {
        RFile file;
        TInt error( file.Open( aFs, aFilename, EFileShareAny | EFileRead ) );
        if ( error == KErrNone )
            {
            static_cast<CVoIPProvisioningAppUi*>(iAppUi)->HandleFileL( file );
            }
	    }
	
	return NULL;
	}

// ----------------------------------------------------------------------------
// CVoIPProvisioningDocument::OpenFileL
// ----------------------------------------------------------------------------
void CVoIPProvisioningDocument::OpenFileL( CFileStore*& /*aFileStore*/,
    RFile& aFile )
	{
	// Request appui to handle this file
	static_cast<CVoIPProvisioningAppUi*>( iAppUi )->HandleFileL( aFile );
	}

// End of File
