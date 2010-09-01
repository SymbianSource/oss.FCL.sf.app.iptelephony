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
* Description:  
*
*/


#include "cscsvcplugin.h"
#include "cscsvcpluginlogger.h"
#include "cscsvcpluginhandler.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCSvcPlugin::CCSCSvcPlugin()
    { 
    }
    
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CCSCSvcPlugin::ConstructL()
    {
    CSCSVCPLUGINDEBUG("CCSCSvcPlugin::ConstructL - begin");
    
    iSvcPluginHandler = CCSCSvcPluginHandler::NewL();
    
    CSCSVCPLUGINDEBUG("CCSCSvcPlugin::ConstructL - end");
    }

    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCSvcPlugin* CCSCSvcPlugin::NewL()
    {
    CCSCSvcPlugin* self = new ( ELeave ) CCSCSvcPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCSvcPlugin::~CCSCSvcPlugin()
    {
    CSCSVCPLUGINDEBUG("CCSCSvcPlugin::~CCSCSvcPlugin - begin");
     
    delete iSvcPluginHandler;
    
    CSCSVCPLUGINDEBUG("CCSCSvcPlugin::~CCSCSvcPlugin - end");
    }


// ---------------------------------------------------------------------------
// For getting csc setting cleanup plugin type.
// ---------------------------------------------------------------------------
//
CCSCEngSettingsCleanupPluginInterface::TCSCPluginType 
    CCSCSvcPlugin::PluginType() const
    {
    return CCSCEngSettingsCleanupPluginInterface::ESipVoIPCleanupPlugin;
    }


// ---------------------------------------------------------------------------
// For removing setting based on service id.
// ---------------------------------------------------------------------------
//   
void CCSCSvcPlugin::RemoveSettingsL( TUint32 aServiceId )
    {
    CSCSVCPLUGINDEBUG2(
        "CCSCSvcPlugin::RemoveSettingsL - begin: SERVICE ID=%d", aServiceId );
    
    iSvcPluginHandler->RemoveSettingsL( aServiceId );
    
    CSCSVCPLUGINDEBUG(
      "CCSCSvcPlugin::RemoveSettingsL - end");
    }

