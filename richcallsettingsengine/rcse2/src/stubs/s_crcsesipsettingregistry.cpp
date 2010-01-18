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
* Description:  Stores and loads CRCSESIPSettingEntry to/from database.
*
*/



// INCLUDE FILES
#include "crcsesipsettingregistry.h"
#include "crcsesipsettingentry.h"
#include "rcsepanic.h"
#include "rcseconstants.h"
#include "rcselogger.h"

// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CRCSESIPSettingRegistry::CRCSESIPSettingRegistry
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CRCSESIPSettingRegistry::CRCSESIPSettingRegistry()
    : CRCSERegistryBase( ESIPSetting )
    {
    }

// -----------------------------------------------------------------------------
// CRCSESIPSettingRegistry::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CRCSESIPSettingRegistry::ConstructL()
    {
    RCSELOGSTRING( "CRCSESIPSettingRegistry::ConstructL() - STUB impl" );
    BaseConstructL();
    }

// -----------------------------------------------------------------------------
// CRCSESIPSettingRegistry::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CRCSESIPSettingRegistry* CRCSESIPSettingRegistry::NewL()
    {
    CRCSESIPSettingRegistry* self = CRCSESIPSettingRegistry::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CRCSESIPSettingRegistry::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CRCSESIPSettingRegistry* CRCSESIPSettingRegistry::NewLC()
    {
    CRCSESIPSettingRegistry* self = new( ELeave ) CRCSESIPSettingRegistry;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

    
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CRCSESIPSettingRegistry::~CRCSESIPSettingRegistry()
    {
    }
    
// -----------------------------------------------------------------------------
// CRCSESIPSettingRegistry::FindL
// Creates sql query and sets the data to aFoundQuery.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CRCSESIPSettingRegistry::FindL( 
    TUint32 /*aSIPProfileId*/, 
    CRCSESIPSettingEntry& /*aFoundEntry*/ )
    {
    RCSELOGSTRING( "CRCSESIPSettingRegistry::FindL() - KErrNotSupported" );
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CRCSESIPSettingRegistry::AddL
// Inserts aNewEntry to database.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CRCSESIPSettingRegistry::AddL( 
    const CRCSESIPSettingEntry& /*aNewEntry*/ )
    {
    RCSELOGSTRING( "CRCSESIPSettingRegistry::AddL() - KErrNotSupported" );
    User::Leave( KErrNotSupported );
    return 0; //sipid;
    }

// -----------------------------------------------------------------------------
// CRCSESIPSettingRegistry::DeleteL
// Deletes a SIP entry which SIP id is aId.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CRCSESIPSettingRegistry::DeleteL( TUint32 /*aId*/ )
    {
    RCSELOGSTRING( "CRCSESIPSettingRegistry::Delete() - KErrNotSupported" );
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CRCSESIPSettingRegistry::UpdateL
// Updates SIP profile entry, which SIP identifier is aId.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CRCSESIPSettingRegistry::UpdateL(
    TUint32 /*aId*/,
    const CRCSESIPSettingEntry& /*aUpdateData*/ )
    {
    RCSELOGSTRING( "CRCSESIPSettingRegistry::UpdateL() - KErrNotSupported" );
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CRCSESIPSettingRegistry::GetDefaultProfile
// Sets a default profile.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CRCSESIPSettingRegistry::GetDefaultProfile( 
    CRCSESIPSettingEntry& aDefaultProfile ) const
    {
    aDefaultProfile.ResetDefaultValues();
    }

// -----------------------------------------------------------------------------
// CRCSESIPSettingRegistry::GetAllIdsL
// Gets all SIP setting identifiers from data base.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CRCSESIPSettingRegistry::GetAllIdsL( 
    RArray<TUint32>& /*aAllIds*/ )
    {
    RCSELOGSTRING( "CRCSESIPSettingRegistry::GetAllIdsL() - No impl !" );
    }


//  End of File  
