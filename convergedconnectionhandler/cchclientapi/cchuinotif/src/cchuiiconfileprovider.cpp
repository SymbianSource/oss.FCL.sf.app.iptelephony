/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implements icon file provider for avkon icon framework
*
*/


#include <aknappui.h>
#include <AknIconUtils.h>

#include "cchuipluginiconfileprovider.h"


// ======== MEMBER FUNCTIONS ========

CCchUiPluginIconFileProvider::CCchUiPluginIconFileProvider( RFile& aFile ):
    iFile( aFile )
    {
    }

CCchUiPluginIconFileProvider* 
    CCchUiPluginIconFileProvider::NewL( RFile& aFile )
    {
    CCchUiPluginIconFileProvider* self = 
        new( ELeave ) CCchUiPluginIconFileProvider( aFile );
    return self;
    }

CCchUiPluginIconFileProvider::~CCchUiPluginIconFileProvider()
    {
    }

// ---------------------------------------------------------------------------
// From class MAknIconFileProvider.
// ---------------------------------------------------------------------------
//
void CCchUiPluginIconFileProvider::RetrieveIconFileHandleL(
    RFile& aFile, const TIconFileType /*aType*/ )
    {
    User::LeaveIfError( aFile.Duplicate( iFile ) );
    }

// ---------------------------------------------------------------------------
// From class MAknIconFileProvider.
// ---------------------------------------------------------------------------
//
void CCchUiPluginIconFileProvider::Finished()
    {
    delete this;
    }

