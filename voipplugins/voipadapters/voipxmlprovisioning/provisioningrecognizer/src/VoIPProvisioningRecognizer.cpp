/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CVoIPProvisioningRecognizer implementation.
*
*/


#include "VoIPProvisioningRecognizer.h"
#include "ecom/implementationproxy.h"

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// CVoIPProvisioningRecognizer::CVoIPProvisioningRecognizer
// Default constructor
// ---------------------------------------------------------
//
CVoIPProvisioningRecognizer::CVoIPProvisioningRecognizer() :
    CApaDataRecognizerType( KUidMimeVoIPProvisioningRecognizer,
    CApaDataRecognizerType::EHigh )
    {
    iCountDataTypes = KVoIPSupportedMimeTypes;
    }

// ---------------------------------------------------------
// CVoIPProvisioningRecognizer::PreferredBufSize
// Overwritten method from CApaDataRecognizerType
// ---------------------------------------------------------
//
TUint CVoIPProvisioningRecognizer::PreferredBufSize()
    {
    return 0x200;
    }

// ---------------------------------------------------------
// CVoIPProvisioningRecognizer::SupportedDataTypeL
// Overwritten method from CApaDataRecognizerType
// ---------------------------------------------------------
//
TDataType CVoIPProvisioningRecognizer::SupportedDataTypeL( TInt aIndex ) const
    {
    __ASSERT_DEBUG( aIndex >= 0 &&
        aIndex < KVoIPSupportedMimeTypes, User::Leave( KErrArgument ) );

    switch( aIndex )
        {
        case 0:
            {
            TDataType type( KVoIPConfMimetypeSimple );
            return type;
            }
        case 1:
            {
            TDataType type( KVoIPConfMimetypeApplication );
            return type;
            }            
        default:
            break;
        }

    return TDataType();
    }

// ---------------------------------------------------------------------------
// CVoIPProvisioningRecognizer::DoRecognizeL
// Overwritten method from CApaDataRecognizerType
// ---------------------------------------------------------------------------
//
void CVoIPProvisioningRecognizer::DoRecognizeL( const TDesC& /*aName*/,
    const TDesC8& aBuffer )
    {
    iConfidence = ENotRecognized;
    iDataType = TDataType();
    if ( aBuffer.FindF( KVoIPXMLTag ) != KErrNotFound )
        {
        iConfidence = ECertain;
        iDataType = TDataType( KVoIPConfMimetypeApplication );
        }
    }

// ---------------------------------------------------------
// CVoIPProvisioningRecognizer::CreateRecognizerL
// Static method to create instance of CVoIPProvisioningRecognizer
// ---------------------------------------------------------
//
CApaDataRecognizerType* CVoIPProvisioningRecognizer::CreateRecognizerL()
    {
    return new (ELeave) CVoIPProvisioningRecognizer();
    }

// ---------------------------------------------------------
// ImplementationTable
// Table containing the data concerning CVoIPProvisioningRecognizer 
// ---------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( KVoIPProvRecImplUIDValue,
        CVoIPProvisioningRecognizer::CreateRecognizerL )
    };

// ---------------------------------------------------------
// ImplementationGroupProxy
// Function called by framework to return data about this recognizer
// ---------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
    TInt& aTableCount )
    {
    aTableCount =
            sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
    return ImplementationTable;
    }

// End of file.
