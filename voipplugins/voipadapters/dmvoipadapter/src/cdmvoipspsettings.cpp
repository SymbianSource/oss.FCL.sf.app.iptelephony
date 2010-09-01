/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  SPSettings class implementation for dmvoipadapter
*
*/


#include "cdmvoipspsettings.h"
#include "smldmvoipdebug.h"

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::NewL
// ---------------------------------------------------------------------------
//
CDMVoIPSPSettings* CDMVoIPSPSettings::NewL()
    {
    CDMVoIPSPSettings* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::NewLC
// ---------------------------------------------------------------------------
//
CDMVoIPSPSettings* CDMVoIPSPSettings::NewLC()
    {
    CDMVoIPSPSettings* self = new (ELeave) CDMVoIPSPSettings();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::CDMVoIPSPSettings
// ---------------------------------------------------------------------------
//
CDMVoIPSPSettings::CDMVoIPSPSettings()
    {
    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::~CDMVoIPSPSettings
// ---------------------------------------------------------------------------
//
CDMVoIPSPSettings::~CDMVoIPSPSettings()
    {
    DBG_PRINT( "CDMVoIPSPSettings::~CDMVoIPSPSettings - begin" );
    delete iVmbxMwiUri;
    delete iVmbxListenUri;
    delete iServiceProviderBookmark;
    delete iBrandingDataUri;
    DBG_PRINT( "CDMVoIPSPSettings::~CDMVoIPSPSettings - end" );
    }

// ---------------------------------------------------------------------------
// CNSmlDmVoIPAdapter::ConstructL
// ---------------------------------------------------------------------------
//
void CDMVoIPSPSettings::ConstructL()
    {
    DBG_PRINT( "CDMVoIPSPSettings::ConstructL - begin" );
    iVmbxMwiUri              = HBufC::NewL( 0 );
    iVmbxListenUri           = HBufC::NewL( 0 );
    iServiceProviderBookmark = HBufC::NewL( 0 );
    iBrandingDataUri         = HBufC::NewL( 0 );
    iReSubscribeInterval     = KErrNotFound;
    iAutoAcceptBuddy         = KErrNotFound;
    iAutoEnableService       = KErrNotFound;
    iImEnabled               = KErrNotFound;
    DBG_PRINT( "CDMVoIPSPSettings::ConstructL - end" );
    }

// End of file.
