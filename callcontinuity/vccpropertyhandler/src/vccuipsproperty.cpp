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
* Description:   VCC property manager on the UI side
*
*/



#include <e32cmn.h>
#include <vccsubscribekeys.h>

#include "vccuipsproperty.h"
#include "rubydebug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CVccUiPsProperty::CVccUiPsProperty()
    {
    RUBY_DEBUG_BLOCK( "CVccUiPsProperty::CVccUiPsProperty" );
    }

// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CVccUiPsProperty::ConstructL()
    {
    RUBY_DEBUG_BLOCKL( "CVccUiPsProperty::ConstructL" );

    DefineKeysL();
    CreateHandlesL();
    }

// ---------------------------------------------------------------------------
// Symbian constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CVccUiPsProperty* CVccUiPsProperty::NewL()
    {
    RUBY_DEBUG_BLOCKL( "CVccUiPsProperty::NewL" );

    CVccUiPsProperty* self = new ( ELeave ) CVccUiPsProperty();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CVccUiPsProperty::~CVccUiPsProperty()
    {
    RUBY_DEBUG0( "CVccUiPsProperty::~CVccUiPsProperty() - ENTER" );
    iHoRequestKey.Close();
    RUBY_DEBUG0( "CVccUiPsProperty::~CVccUiPsProperty() - EXIT" );
    }

// ---------------------------------------------------------------------------
// Sets the value of a VCC property
// ---------------------------------------------------------------------------
//

EXPORT_C void CVccUiPsProperty::NotifySubscriberL( TVccHoRequest aValue )
    {
    RUBY_DEBUG_BLOCKL( "CVccUiPsProperty::NotifySubscriberL" );
    RUBY_DEBUG1( " -value [%d]", aValue);

    TVccHoRequest currVal;

    GetCurrentRequest( currVal );

    RUBY_DEBUG1( " -current value [%d]", currVal);

    switch( aValue )
        {
        case EVccNoRequestOngoing:
        case EVccManualStartCsToPsHoRequest:
        case EVccManualCancelCsToPsHoRequest:
        case EVccManualStartPsToCsHoRequest:
        case EVccManualCancelPsToCsHoRequest:
        case EVccAutomaticStartCsToPsHoRequest:
        case EVccAutomaticStartCsToPsHoRequestIfSingleCall:    
        case EVccAutomaticCancelCsToPsHoRequest:
        case EVccAutomaticStartPsToCsHoRequest:
        case EVccAutomaticStartPsToCsHoRequestIfSingleCall:
        case EVccAutomaticCancelPsToCsHoRequest:        
            {
            RUBY_DEBUG0( " -setting key");

            User::LeaveIfError( iHoRequestKey.Set( aValue ));

            break;
            }

        case EVccUndefinedRequest:
        default:
            {
            RUBY_DEBUG0( " -Undefined key!!");

            User::Leave( KErrNotFound );

            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// Gets the value of a VCC property
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CVccUiPsProperty::GetCurrentRequest(  TVccHoRequest& aValue )
    {
    RUBY_DEBUG_BLOCK( "CVccUiPsProperty::GetCurrentRequest" );

    TInt value(0);
    TInt err = iHoRequestKey.Get( value );
    aValue = static_cast<TVccHoRequest>( value );

    return err;
    }

// ---------------------------------------------------------------------------
// Deletes the keys used by ui
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CVccUiPsProperty::DeleteKeys()
    {
    RUBY_DEBUG_BLOCK( "CVccUiPsProperty::DeleteKeys" );

    iHoRequestKey.Close();
    TInt err = RProperty::Delete( KPSVccPropertyCategory, KVccPropKeyHoRequest );
    return err;
    }

// ---------------------------------------------------------------------------
// Defines the keys the UI can update
// ---------------------------------------------------------------------------
//
void CVccUiPsProperty::DefineKeysL()
    {
    RUBY_DEBUG_BLOCKL( "CVccUiPsProperty::DefineKeysL" );

    RProcess myProcess;

    _LIT_SECURITY_POLICY_PASS( KPassReadPolicy );
    _LIT_SECURITY_POLICY_S0( KAllowSidPolicy, myProcess.SecureId() );

    TInt err = RProperty::Define( KPSVccPropertyCategory,
                                  KVccPropKeyHoRequest,
                                  RProperty::EInt,
                                  KPassReadPolicy,
                                  KAllowSidPolicy );

    if ( err != KErrNone && err != KErrAlreadyExists )
        {
        User::Leave( err );
        }
    }

// ---------------------------------------------------------------------------
// Defines the keys the UI can update
// ---------------------------------------------------------------------------
//
void CVccUiPsProperty::CreateHandlesL()
    {
    RUBY_DEBUG_BLOCKL( "CVccUiPsProperty::CreateHandlesL" );

    User::LeaveIfError( iHoRequestKey.Attach( KPSVccPropertyCategory,
              KVccPropKeyHoRequest ));
    }
