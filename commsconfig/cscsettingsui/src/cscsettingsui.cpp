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
* Description:  Implements CSCSettingsUi API methods
*
*/


#include <cchclient.h>

#include "cscsettingsui.h"
#include "cscsettingsuiimpl.h"
#include "cscsettingsuilogger.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCSettingsUi::CCSCSettingsUi()
    {    
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CCSCSettingsUi::ConstructL( CEikonEnv& /*aEikEnv*/ )
    {
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CCSCSettingsUi* CCSCSettingsUi::NewL( CEikonEnv& aEikEnv )
    {
    CCSCSettingsUi* self = new ( ELeave ) CCSCSettingsUi;
    CleanupStack::PushL( self );
    
    CCSCSettingsUiImpl* tlsImpl = (CCSCSettingsUiImpl*)Dll::Tls();
    if ( !tlsImpl )
        {    
        CCSCSettingsUiImpl* impl = CCSCSettingsUiImpl::NewL( aEikEnv );
        CleanupStack::PushL( impl ),
        User::LeaveIfError( Dll::SetTls( impl ) );
        CleanupStack::Pop( impl );
        self->iImpl = static_cast<CCSCSettingsUiImpl*>( Dll::Tls() );
        }
    else
        {
        self->iImpl = tlsImpl;
        }
    
    self->iImpl->ReferenceCounter()++;
    
    self->ConstructL( aEikEnv );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCSettingsUi::~CCSCSettingsUi()
    {
    CSCSETUIDEBUG( "CCSCSettingsUi::~CCSCSettingsUi - begin" );
    
    if ( iImpl )
        {
        iImpl->ReferenceCounter()--;
        if ( !iImpl->ReferenceCounter() )
            {
            delete iImpl;
            Dll::FreeTls();
            }
        }

    CSCSETUIDEBUG( "CCSCSettingsUi::~CCSCSettingsUi - end" );
    }

//---------------------------------------------------------------------------
// CCSCSettingsUi::InitializeL
// 
//---------------------------------------------------------------------------
//
EXPORT_C void CCSCSettingsUi::InitializeL( const TUid& aReturnViewId, 
        TUint aServiceId,
        const MCoeView* aDefaultAppView )
    {
    CSCSETUIDEBUG( "CCSCSettingsUi::InitializeL" );
    
    iImpl->InitializeL( aReturnViewId, aServiceId, aDefaultAppView );
    }

// ---------------------------------------------------------------------------
// CCSCSettingsUi::LaunchSettingsUiL
// Launches Converged Service Configurator Settings UI.
// ---------------------------------------------------------------------------
//
EXPORT_C void CCSCSettingsUi::LaunchSettingsUiL() const
    {
    CSCSETUIDEBUG( "CCSCSettingsUi::LaunchSettingsUiL - begin" );
    
    iImpl->LaunchSettingsUiL();
    
    CSCSETUIDEBUG( "CCSCSettingsUi::LaunchSettingsUiL - end" );
    }

