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
* Description:  CSC Application´s Document
*
*/

#include <apgwgnam.h>
#include <cchclient.h>
#include <xSPViewServices.h>
#include <AiwServiceHandler.h>

#include "cscappui.h"
#include "csclogger.h"
#include "cscdocument.h"
#include "cscengservicepluginhandler.h"
#include "cscenguiextensionpluginhandler.h"


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCDocument::CCSCDocument( CEikApplication& aApp ) : 
    CAknDocument( aApp )
    {
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CCSCDocument::ConstructL()
    {
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CCSCDocument::UpdateTaskNameL( CApaWindowGroupName* aWgName )
    {
    CAknDocument::UpdateTaskNameL( aWgName );
    aWgName->SetHidden( ETrue );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCDocument* CCSCDocument::NewL( CEikApplication& aApp ) 
    {
    CCSCDocument* self = NewLC( aApp );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCDocument* CCSCDocument::NewLC( CEikApplication& aApp )
    {
    CCSCDocument* self = new (ELeave) CCSCDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// Transfers ownership to document.
// ---------------------------------------------------------------------------
//
void CCSCDocument::SetOwnershipOfPluginHandlers( 
    CCSCEngServicePluginHandler* aServicePluginHandler,
    CCSCEngUiExtensionPluginHandler* aUiExtensionPluginHandler )
    {
    iServicePluginHandler = aServicePluginHandler;
    iUiExtensionPluginHandler = aUiExtensionPluginHandler;
    }


// ---------------------------------------------------------------------------
// Returns reference to service plug-in handler.
// ---------------------------------------------------------------------------
//
CCSCEngServicePluginHandler& CCSCDocument::ServicePluginHandler()
    {
    return *iServicePluginHandler;
    }

// ---------------------------------------------------------------------------
// From class CAknDocument
// Create the application user interface
// ---------------------------------------------------------------------------
//
CEikAppUi* CCSCDocument::CreateAppUiL()
    {
    // Create the application user interface, and return a pointer to it
    // the framework takes ownership of this object
    return ( static_cast<CEikAppUi*>( new (ELeave) CCSCAppUi ) ); 
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCDocument::~CCSCDocument()
    {
    CSCDEBUG( "CCSCDocument::~CCSCDocument - begin" );
    
    delete iServicePluginHandler;
    delete iUiExtensionPluginHandler;
    
    CSCDEBUG( "CCSCDocument::~CCSCDocument - end" );
    }
    
