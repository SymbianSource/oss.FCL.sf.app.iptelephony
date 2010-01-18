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
* Description:  Implements CAcpProviderField methods
*
*/


#include "acpproviderfield.h"
#include "accountcreationpluginlogger.h"


// ---------------------------------------------------------------------------
// CAcpProviderField::CAcpProviderField
// ---------------------------------------------------------------------------
//
CAcpProviderField::CAcpProviderField() 
    {
    }

// ---------------------------------------------------------------------------
// CAcpProviderField::NewL
// ---------------------------------------------------------------------------
//
CAcpProviderField* CAcpProviderField::NewL()
    {
    CAcpProviderField* self = CAcpProviderField::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CAcpProviderField::NewLC
// ---------------------------------------------------------------------------
//
CAcpProviderField* CAcpProviderField::NewLC()
    {
    CAcpProviderField* self = new ( ELeave ) CAcpProviderField();
    CleanupStack::PushL( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CAcpProviderField::~CAcpProviderField
// ---------------------------------------------------------------------------
//
CAcpProviderField::~CAcpProviderField()
    {
    ACPLOG( "CAcpProviderField::~CAcpProviderField begin" );
    delete iFieldName; // Field name
    delete iFieldData; // Field data
    ACPLOG( "CAcpProviderField::~CAcpProviderField end" );
    }

// ---------------------------------------------------------------------------
// CAcpProviderField::CopyL
// Copies provider fields data from given parameter to member data.
// ---------------------------------------------------------------------------
//
void CAcpProviderField::CopyL( const CAcpProviderField& aProviderField )
    {
    ACPLOG( "CAcpProviderField::CopyL begin" );

    // Set field name descriptor 
    SetFieldNameL( aProviderField.FieldName() );
    // Set field type
    iType = aProviderField.iType;

    ACPLOG( "CAcpProviderField::CopyL end" );
    }

// ---------------------------------------------------------------------------
// CAcpProviderField::FieldName
// Returns name of the field.
// ---------------------------------------------------------------------------
//
TPtrC CAcpProviderField::FieldName() const
    {
    if ( iFieldName )
        {
        return *iFieldName;
        }
    return KNullDesC();
    }

// ---------------------------------------------------------------------------
// CAcpProviderField::SetFieldNameL
// Sets name of the field.
// ---------------------------------------------------------------------------
//
void CAcpProviderField::SetFieldNameL( const TDesC& aFieldName )
    {
    // Check whether field name defined or not
    if( aFieldName.Length() > KErrNone )
        {
        // Old filed name should be deleted at first
        delete iFieldName;
        iFieldName = NULL;
        // Allocate descriptor for the filed name with the 
        // requested maximum length
        iFieldName = HBufC::NewL( aFieldName.Length() );
        // Copy the filed name into the descriptor
        iFieldName->Des().Copy( aFieldName );
        }
    }

// ---------------------------------------------------------------------------
// CAcpProviderField::FieldType
// Returns type of the field.
// ---------------------------------------------------------------------------
//
CAcpProviderField::TFieldTypes CAcpProviderField::FieldType() const
    {
    ACPLOG2( "CAcpProviderField::FieldType: %d", iType );
    return iType; // Field type
    }

// ---------------------------------------------------------------------------
// CAcpProviderField::SetFieldType
// Sets type of the field.
// ---------------------------------------------------------------------------
//
void CAcpProviderField::SetFieldType( CAcpProviderField::TFieldTypes aType )
    {
    iType = aType; // Set field type
    }    

// ---------------------------------------------------------------------------
// CAcpProviderField::FieldData
// Returns data of the field.
// ---------------------------------------------------------------------------
//
TPtrC CAcpProviderField::FieldData() const
    {
    if( iFieldData ) return *iFieldData; // The field data
    return KNullDesC(); // Unspecified
    }

// ---------------------------------------------------------------------------
// CAcpProviderField::SetFieldDataL
// Sets data of the field.
// ---------------------------------------------------------------------------
//
void CAcpProviderField::SetFieldDataL( const TDesC& aFieldData )
    {
    // Check whether the requested field data definec or not
    if( aFieldData.Length() > KErrNone )
        {
        // The field data descriptor should be deleted at first
        delete iFieldData;
        iFieldData = NULL;

        // Allocate new descriptor for the field data with the requested
        // maximum lenght
        iFieldData = HBufC::NewL( aFieldData.Length() );
        // Copy the field data into the descriptor
        iFieldData->Des().Copy( aFieldData );
        }
    }

// End of file.
