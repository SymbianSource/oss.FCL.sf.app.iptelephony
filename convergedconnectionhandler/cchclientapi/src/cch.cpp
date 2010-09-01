/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  cch client api
*
*/


// INCLUDE FILES
#include "cch.h"
#include "cchimpl.h"

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES
// None

// CONSTANTS
// None

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ============================= LOCAL FUNCTIONS =============================

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// CCch::CCch
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
EXPORT_C CCch::CCch() :
iImplementation ( NULL )
    {
    
    }
    
EXPORT_C CCch::~CCch()
    {
    delete iImplementation;
    }

// ---------------------------------------------------------------------------
// CCch::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
EXPORT_C void CCch::ConstructL()
    {
    iImplementation = CCchImpl::NewL( *this );
    
    }
// ---------------------------------------------------------------------------
// CCch::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CCch* CCch::NewL( )
    {
    CCch* self = CCch::NewLC( );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CCch::NewLC
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CCch* CCch::NewLC( )
    {
    CCch* self = new (ELeave) CCch( );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CCch::GetService
// ---------------------------------------------------------------------------
//
EXPORT_C CCchService* CCch::GetService( TInt aId )
	{
	return iImplementation->GetService( aId );
	}

// ---------------------------------------------------------------------------
// CCch::GetServiceIds
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCch::GetServiceIds( RArray<TInt>& aIds ) const
	{
	return iImplementation->GetServiceIds( aIds );
	}

// ---------------------------------------------------------------------------
// CCch::GetServices
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCch::GetServices( TCCHSubserviceType aType, 
        RPointerArray<CCchService>& aServices )
	{
	return iImplementation->GetServices( aType, aServices );
	}

// ---------------------------------------------------------------------------
// CCch::ConnectivityDialogsAllowed
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CCch::ConnectivityDialogsAllowed() const
    {
    return iImplementation->ConnectivityDialogsAllowed();
    }
  
// ---------------------------------------------------------------------------
// CCch::SetConnectivityDialogsAllowed
// ---------------------------------------------------------------------------
//
EXPORT_C void CCch::SetConnectivityDialogsAllowed( 
    TBool aConnectivityDialogsAllowed )
    {
    iImplementation->SetConnectivityDialogsAllowed( 
        aConnectivityDialogsAllowed );
    }

// ---------------------------------------------------------------------------
// CCch::CchUiApi
// ---------------------------------------------------------------------------
//
EXPORT_C MCchUi& CCch::CchUiApi() const
    {
    return iImplementation->CchUiApi();
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
