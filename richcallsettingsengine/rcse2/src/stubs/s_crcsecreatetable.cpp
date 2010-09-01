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
* Description:  Provides table creation for databases.
*
*/


// INCLUDE FILES
#include    "crcsecreatetable.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CRCSECreateTable::CRCSECreateTable
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CRCSECreateTable::CRCSECreateTable()
    {
    }
    
// Destructor
CRCSECreateTable::~CRCSECreateTable()
    {
    }
    
// -----------------------------------------------------------------------------
// CRCSECreateTable::CreateProfileTableL
// Creates a table to database, where the profiles are stored.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRCSECreateTable::CreateProfileTableL()
    {
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CRCSECreateTable::CreateAudioCodecTableL
// Creates a table to database, where the Audio Codecs are stored.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRCSECreateTable::CreateAudioCodecTableL()
    {
    User::Leave( KErrNotSupported );
    }
    
// -----------------------------------------------------------------------------
// CRCSECreateTable::CreateSIPProfileTableL
// Creates a table to database, where the SIP profiles are stored.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRCSECreateTable::CreateSIPSettingTableL()
    {
    User::Leave( KErrNotSupported );
    }
    
// -----------------------------------------------------------------------------
// CRCSECreateTable::CreateSCCPSettingTableL
// Creates a table to database, where the SCCP settings are stored.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRCSECreateTable::CreateSCCPSettingTableL()
    {
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CRCSECreateTable::AddColumnToTableL
// Adds column to table
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRCSECreateTable::AddColumnToTableL(
    CDbColSet& /*aTable*/,
    const TDesC& /*aColumnName*/,
    TDbColType /*aColumnType*/,
    TInt /*aMaxLengthOfColumn*/ )
    {
    User::Leave( KErrNotSupported );
    }

//  End of File  
