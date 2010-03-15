/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   P&S property writer/reader
*
*/


#include "vccengpsproperty.h"
#include "rubydebug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CVccEngPsProperty::CVccEngPsProperty()
	{
	RUBY_DEBUG_BLOCK( "CVccEngPsProperty::CVccEngPsProperty" );
	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CVccEngPsProperty::~CVccEngPsProperty()
	{
    RUBY_DEBUG0( "CVccEngPsProperty::~CVccEngPsProperty() - ENTER" );
    iHoStatusKey.Close();
    iErrorCodeKey.Close();
    RUBY_DEBUG0( "CVccEngPsProperty::~CVccEngPsProperty() - EXIT" );
	}

// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CVccEngPsProperty::ConstructL()
    {
    RUBY_DEBUG_BLOCKL( "CVccEngPsProperty::ConstructL" );
   
    DefineKeysL();
    CreateHandlesL();
    }

// ---------------------------------------------------------------------------
// Symbian constructor
// ---------------------------------------------------------------------------
//
CVccEngPsProperty* CVccEngPsProperty::NewL()
    {
    RUBY_DEBUG_BLOCKL( "CVccEngPsProperty::NewL" );
   
    CVccEngPsProperty* self = new ( ELeave ) CVccEngPsProperty();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------------------------
// Sets the value of a VCC property
// ---------------------------------------------------------------------------
//
void CVccEngPsProperty::NotifySubscriberL( TVccHoStatus aStatus, 
		TInt aErrorCode )
	{
    RUBY_DEBUG_BLOCKL( "CVccEngPsProperty::NotifySubscriberL" );
    RUBY_DEBUG1( " -status = %d", aStatus );
    RUBY_DEBUG1( " -errorcode = %d", aErrorCode);
   
	switch( aStatus )
		{
		case EVccHoStateIdle:
		case EVccCsToPsHoStarted:
		case EVccPsToCsHoStarted:
		case EVccCsToPsHoInprogress:
		case EVccPsToCsHoInprogress:
		case EVccCsToPsHoSuccessful:
		case EVccCsToPsHoFailure:
        case EVccHoUnavailable:
        case EVccCsToPsNotAllowed:
        case EVccPsToCsNotAllowed:
        case EVccHoStateIdleIfSingleCall:
        case EVccHoAllowedToCsIfSingleCall:
        case EVccHoAllowedToPsIfSingleCall:
			{
	  	    RUBY_DEBUG0( " -updating keys" );
    
			User::LeaveIfError( iHoStatusKey.Set( aStatus ));	
			User::LeaveIfError( iErrorCodeKey.Set( aErrorCode ));
            break;
			}
		case EVccHoStateUnknown:	
		default:
			{
	  	    RUBY_DEBUG0( " -Error, unknown key!!" );

			User::Leave( KErrNotFound );
            break;
			}
		}
    }

// ---------------------------------------------------------------------------
// Gets the Ho status property
// ---------------------------------------------------------------------------
//
TInt CVccEngPsProperty::GetCurrentHoStatus(  TVccHoStatus& aStatus )
	{
    RUBY_DEBUG_BLOCK( "CVccEngPsProperty::GetCurrentHoStatus" );
   
	TInt value ( EVccHoStateUnknown );
	TInt err = iHoStatusKey.Get( value );
	aStatus = ( TVccHoStatus ) value;
	return err;
	}

// ---------------------------------------------------------------------------
// Gets the error code 
// ---------------------------------------------------------------------------
//
TInt CVccEngPsProperty::GetCurrentErrorCode(  TInt& aErrorCode )
	{
	RUBY_DEBUG_BLOCK( "CVccEngPsProperty::GetCurrentErrorCode" );
   
	TInt err = iErrorCodeKey.Get( aErrorCode );
	return err;
	}

// ---------------------------------------------------------------------------
// Defines the keys the UI can update
// ---------------------------------------------------------------------------
//
void CVccEngPsProperty::DefineKeysL()
	{
	RUBY_DEBUG_BLOCK( "CVccEngPsProperty::DefineKeysL" );
   
	RProcess myProcess;
	    
	_LIT_SECURITY_POLICY_PASS(KPassReadPolicy);
	_LIT_SECURITY_POLICY_S0( KAllowSidPolicy, myProcess.SecureId() );

	TInt err = RProperty::Define( KPSVccPropertyCategory,
								  KVccPropKeyHoStatus,
								  RProperty::EInt,
								  KPassReadPolicy,
								  KAllowSidPolicy );

	if ( err != KErrNone && err != KErrAlreadyExists )
		{
		User::Leave( err );		
		}
	
	err = RProperty::Define( KPSVccPropertyCategory,
							 KVccPropKeyHoError,
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
void CVccEngPsProperty::CreateHandlesL()
	{
	RUBY_DEBUG_BLOCKL( "CVccEngPsProperty::CreateHandlesL" );
   
	User::LeaveIfError( iHoStatusKey.Attach( KPSVccPropertyCategory,
			KVccPropKeyHoStatus ));
	User::LeaveIfError( iErrorCodeKey.Attach( KPSVccPropertyCategory,
			KVccPropKeyHoError ));
	}
