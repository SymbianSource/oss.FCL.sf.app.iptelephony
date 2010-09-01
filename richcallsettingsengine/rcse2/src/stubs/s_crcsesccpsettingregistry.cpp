/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Stores and loads CRCSESCCPEntry to/from database.
*
*/



// INCLUDE FILES
#include "crcsesccpsettingregistry.h"
#include "crcsesccpentry.h"
#include "rcseregistrybase.h"
#include "rcsepanic.h"
#include "rcseconstants.h"
#include "rcselogger.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CRCSESCCPSettingRegistry::CRCSESCCPSettingRegistry
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CRCSESCCPSettingRegistry::CRCSESCCPSettingRegistry()
    : CRCSERegistryBase( ESCCPProfile )
    {
    }

// -----------------------------------------------------------------------------
// CRCSESCCPSettingRegistry::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CRCSESCCPSettingRegistry::ConstructL()
    {
    RCSELOGSTRING( "CRCSESCCPSettingRegistry::ConstructL() - STUB impl" );
    BaseConstructL();
    }
    
// -----------------------------------------------------------------------------
// CRCSESCCPSettingRegistry::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CRCSESCCPSettingRegistry* CRCSESCCPSettingRegistry::NewL()
    {
    CRCSESCCPSettingRegistry* self = CRCSESCCPSettingRegistry::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CRCSESCCPSettingRegistry::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CRCSESCCPSettingRegistry* CRCSESCCPSettingRegistry::NewLC()
    {
    CRCSESCCPSettingRegistry* self = new( ELeave ) CRCSESCCPSettingRegistry;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

    
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CRCSESCCPSettingRegistry::~CRCSESCCPSettingRegistry()
    {
    }
    
// -----------------------------------------------------------------------------
// CRCSESCCPSettingRegistry::FindL
// Creates sql query and sets the data to aFoundQuery.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CRCSESCCPSettingRegistry::FindL( 
    TUint32 /*aSCCPSettingId*/, 
    CRCSESCCPEntry& /*aFoundEntry*/ )
    {
    RCSELOGSTRING( "CRCSESCCPSettingRegistry::FindL() - KErrNotSupported" );
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CRCSESCCPSettingRegistry::AddL
// Inserts aProfile to database.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CRCSESCCPSettingRegistry::AddL( 
    const CRCSESCCPEntry& /*aNewEntry*/ )
    {
    RCSELOGSTRING( "CRCSESCCPSettingRegistry::AddL() - KErrNotSupported" );
    User::Leave( KErrNotSupported );
    return 0;//sccpid;
    }

// -----------------------------------------------------------------------------
// CRCSESCCPSettingRegistry::DeleteL
// Deletes a profile entry which profile id is aId.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CRCSESCCPSettingRegistry::DeleteL( TUint32 /*aId*/ )
    {
    RCSELOGSTRING( "CRCSESCCPSettingRegistry::DeleteL() - KErrNotSupported" );
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CRCSESCCPSettingRegistry::UpdateL
// Updates profile entry, which profile identifier is aId.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CRCSESCCPSettingRegistry::UpdateL(
    TUint32 /*aId*/,
    const CRCSESCCPEntry& /*aUpdateData*/ )
    {
    RCSELOGSTRING( "CRCSESCCPSettingRegistry::UpdateL() - KErrNotSupported" );
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CRCSESCCPSettingRegistry::GetDefaultProfile
// Sets a default profile.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CRCSESCCPSettingRegistry::GetDefaultProfile(
    CRCSESCCPEntry& aDefaultProfile ) const
    {
    aDefaultProfile.ResetDefaultValues();
    }

// -----------------------------------------------------------------------------
// CRCSESCCPSettingRegistry::GetAllIdsL
// Gets all profile identifiers from data base.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CRCSESCCPSettingRegistry::GetAllIdsL( 
    RArray<TUint32>& /*aAllIds*/ )
    {
    RCSELOGSTRING( "CRCSESCCPSettingRegistry::GetAllIdsL() - No impl !" );
    }


//  End of File  
