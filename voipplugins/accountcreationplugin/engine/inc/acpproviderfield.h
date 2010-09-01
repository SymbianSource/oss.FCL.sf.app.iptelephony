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
* Description:  Declarition of CAcpProviderField
*
*/

 
#ifndef ACPPROVIDERFIELD_H
#define ACPPROVIDERFIELD_H

#include <e32base.h>

/**
 *  CAcpProviderField class
 *  Declarition of CAcpProviderField.
 *
 *  @lib accountcreationplugin.lib
 *  @since S60 v3.2
 */
class CAcpProviderField : public CBase
    {
public:

    /**
     * Different kind of field types supported by ACP.
     */
    enum TFieldTypes
        {
        ETextual = 0, // default
        ENumeric,
        EPassword
        };    

    /**
     * Two-phased constructor.
     */
    static CAcpProviderField* NewL();

    /**
     * Two-phased constructor.
     */
    static CAcpProviderField* NewLC();

    /**
     * Destructor.
     */
    virtual ~CAcpProviderField();

    /**
     * Copies provider fields data from given parameter to member data.
     *
     * @since S60 v3.2
     * @param aProviderField for provider fields to be copied
     */ 
    void CopyL( const CAcpProviderField& aProviderField );

    /**
     * Returns name of the field.
     *
     * @since S60 v3.2
     * @return name of the field
     */ 
    TPtrC FieldName() const;

    /**
     * Sets name of the field.
     *
     * @since S60 v3.2
     * @param aFieldName for field name
     */ 
    void SetFieldNameL( const TDesC& aFieldName );

    /**
     * Returns type of the field.
     *
     * @since S60 v3.2
     * @return type of the field
     */ 
    CAcpProviderField::TFieldTypes FieldType() const;

    /**
     * Sets type of the field.
     *
     * @since S60 v3.2
     * @param aType for field type
     */ 
    void SetFieldType( CAcpProviderField::TFieldTypes aType );

    /**
     * Returns data of the field.
     *
     * @since S60 v3.2
     * @return data of the field
     */ 
    TPtrC FieldData() const;

    /**
     * Sets data of the field.
     *
     * @since S60 v3.2
     * @param aFieldData for field data
     */ 
    void SetFieldDataL( const TDesC& aFieldData );

private:

    CAcpProviderField();

private: // data

    /**
     * Name of the field.
     * Own.
     */
    HBufC* iFieldName;

    /**
     * Type of the field.
     */  
    TFieldTypes iType;

    /**
     * Data of the field.
     * Own.
     */
    HBufC* iFieldData;
    };

#endif // ACPPROVIDERFIELD_H

// End of file.
