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
* Description:  Implementation of class CCchUiSpsHandler.
*
*/


#include <spentry.h>
#include <spsettings.h>
#include <spproperty.h>
#include <spdefinitions.h>

#include "cchuilogger.h"
#include "cchuispshandler.h"


// ======== MEMBER FUNCTIONS ========

CCchUiSpsHandler::CCchUiSpsHandler()
    {
    }

void CCchUiSpsHandler::ConstructL()
    {
    CCHUIDEBUG( "CCchUiSpsHandler::ConstructL - IN" );
    
    iSettings = CSPSettings::NewL();
    
    CCHUIDEBUG( "CCchUiSpsHandler::ConstructL - OUT" );
    }

CCchUiSpsHandler* CCchUiSpsHandler::NewL()
    {
    CCchUiSpsHandler* self = NewLC();
    CleanupStack::Pop(self);
    return self;
    }

CCchUiSpsHandler* CCchUiSpsHandler::NewLC()
    {
    CCchUiSpsHandler* self = new (ELeave) CCchUiSpsHandler();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CCchUiSpsHandler::~CCchUiSpsHandler()
    {
    delete iSettings;
    }

// ---------------------------------------------------------------------------
// Set temprorary iap id.
// ---------------------------------------------------------------------------
//
void CCchUiSpsHandler::SetTemporaryIapIdL( 
    TUint32 aServiceId, TUint32 aIapId )
    {
    CCHUIDEBUG( "CCchUiSpsHandler::SetTemporaryIapIdL - IN" );    
    
    CSPProperty* property = CSPProperty::NewLC();
    User::LeaveIfError( 
        property->SetName( ESubPropertyVoIPTemporaryIAPId ) );
    User::LeaveIfError( property->SetValue( aIapId ) );
            
    iSettings->AddOrUpdatePropertyL( aServiceId, *property );
    CleanupStack::PopAndDestroy( property );
    
    CCHUIDEBUG( "CCchUiSpsHandler::SetTemporaryIapIdL - OUT" );
    }

// ---------------------------------------------------------------------------
// Set snap id.
// ---------------------------------------------------------------------------
//
void CCchUiSpsHandler::SetSnapIdL( 
    TUint32 aServiceId, TUint32 aSnapId )
    {
    CCHUIDEBUG( "CCchUiSpsHandler::SetSnapIdL - IN" );    
    
    CSPProperty* property = CSPProperty::NewLC();
    User::LeaveIfError( 
        property->SetName( ESubPropertyVoIPPreferredSNAPId ) );
    User::LeaveIfError( property->SetValue( aSnapId ) );
        
    iSettings->AddOrUpdatePropertyL( aServiceId, *property );
    CleanupStack::PopAndDestroy( property );
    
    CCHUIDEBUG( "CCchUiSpsHandler::SetSnapIdL - OUT" );    
    }      
    
// ---------------------------------------------------------------------------
// Resolves service name
// ---------------------------------------------------------------------------
//
void CCchUiSpsHandler::ServiceNameL( 
    TUint32 aServiceId, TDes& aServiceName )
    {
    CCHUIDEBUG( "CCchUiSpsHandler::ServiceNameL - IN" );    
	
    CSPEntry* entry = CSPEntry::NewLC();
	TInt err = iSettings->FindEntryL( aServiceId, *entry );
	
	if ( !err )
	    {
	    aServiceName = entry->GetServiceName();
	    }
	
	CleanupStack::PopAndDestroy( entry );    
    
	CCHUIDEBUG( "CCchUiSpsHandler::ServiceNameL - OUT" );    
    }          

