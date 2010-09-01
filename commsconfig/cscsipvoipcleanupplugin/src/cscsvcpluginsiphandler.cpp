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


#include <uriutils.h>
#include <sipmanagedprofile.h>
#include <sipmanagedprofileregistry.h>

#include "cscsvcpluginlogger.h"
#include "cscsvcpluginsiphandler.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCSvcPluginSipHandler::CCSCSvcPluginSipHandler()
    {
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CCSCSvcPluginSipHandler::ConstructL()
    {
    CSCSVCPLUGINDEBUG("CCSCSvcPluginSipHandler::ConstructL - begin");
    
    iSipProfileRegistry = CSIPManagedProfileRegistry::NewL( *this );
    
    CSCSVCPLUGINDEBUG("CCSCSvcPluginSipHandler::ConstructL - end");
    }

    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCSvcPluginSipHandler* CCSCSvcPluginSipHandler::NewL()
    {
    CCSCSvcPluginSipHandler* self = 
        new ( ELeave ) CCSCSvcPluginSipHandler();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCSvcPluginSipHandler::~CCSCSvcPluginSipHandler()
    {
    CSCSVCPLUGINDEBUG(
        "CCSCSvcPluginSipHandler::~CCSCSvcPluginSipHandler - begin");
    
    delete iSipProfileRegistry;
    
    CSCSVCPLUGINDEBUG(
        "CCSCSvcPluginSipHandler::~CCSCSvcPluginSipHandler - end");
    }

    
// ---------------------------------------------------------------------------
// Destroys SIP profile from SIP Stack based on SIP profile id.
// ---------------------------------------------------------------------------
//
void CCSCSvcPluginSipHandler::RemoveProfileL(
    TUint32 aProfileId )
    {    
    CSCSVCPLUGINDEBUG( "CCSCSvcPluginSipHandler::RemoveProfileL - begin");
    
    // Destroy VoIP profile from RCSE.
    CSIPManagedProfile* profile = 
        static_cast<CSIPManagedProfile*> 
            ( iSipProfileRegistry->ProfileL( aProfileId ) );
            
    CleanupStack::PushL( profile );
    if ( !iSipProfileRegistry->IsInUseL( *profile ) )
        {
        iSipProfileRegistry->DestroyL( *profile );
        }
    CleanupStack::PopAndDestroy( profile );
    
    CSCSVCPLUGINDEBUG( "CCSCSvcPluginSipHandler::RemoveProfileL - end");
    }


// ---------------------------------------------------------------------------
// SIP profile information event.
// ---------------------------------------------------------------------------
//
void CCSCSvcPluginSipHandler::ProfileRegistryEventOccurred( 
    TUint32 /*aSIPProfileId*/, 
    TEvent /*aEvent*/ )
    {
    }


// ---------------------------------------------------------------------------
// An asynchronous error has occurred related to SIP profile.
// ---------------------------------------------------------------------------
//
void CCSCSvcPluginSipHandler::ProfileRegistryErrorOccurred(
    TUint32 /*aSIPProfileId*/,
    TInt /*aError*/ )
    {
    }
    
void CCSCSvcPluginSipHandler::SetRegistrationModeL( TUint32 aId, TBool aRegMode )
	{
    CSIPManagedProfile* profile = static_cast<CSIPManagedProfile*> 
            ( iSipProfileRegistry->ProfileL( aId ) );
            
    CleanupStack::PushL( profile );
    User::LeaveIfError( profile->SetParameter( KSIPAutoRegistration, aRegMode ) );
    // Save profile back to the persistent storage.
    iSipProfileRegistry->SaveL( *profile );
    CleanupStack::PopAndDestroy( profile );
	
	}

void CCSCSvcPluginSipHandler::GetApIdL( TUint32 aId,  TUint32& aApId )
	{
    CSIPProfile* profile = iSipProfileRegistry->ProfileL( aId );
    CleanupStack::PushL( profile );
    User::LeaveIfError( profile->GetParameter( KSIPAccessPointId, aApId ) );
    CleanupStack::PopAndDestroy( profile );
	}

void CCSCSvcPluginSipHandler::GetUserAorL( TUint32 aId,  TDes8& aUserAor )
	{
    CSIPProfile* profile = iSipProfileRegistry->ProfileL( aId );
    CleanupStack::PushL( profile );
    const TDesC8* aor( NULL );
    User::LeaveIfError( profile->GetParameter( KSIPUserAor, aor ) );
    if ( aor )
        {
        if( aor->Length() <= aUserAor.MaxLength() )
            {
            aUserAor.Copy( *aor );
            }
        else
            {
            User::Leave( KErrOverflow );
            }
        }
    CleanupStack::PopAndDestroy( profile );
	}


