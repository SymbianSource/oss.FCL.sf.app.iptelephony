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



#ifndef CRCSECREATETABLE_H
#define CRCSECREATETABLE_H

//  INCLUDES
#include <e32base.h>
#include <d32dbms.h>

// CLASS DECLARATION

/**
*  Provides table creation for databases.
*
*  @lib RCSE.lib
*  @since S60 3.0
*/
class CRCSECreateTable: public CBase
    {     
public: // New functions
        
    /**
    * Creates Audio Codec table to Audio Codec database.
    * @since S60 3.0
    */
    static void CreateAudioCodecTableL();
    
    /**
    * Creates SIP setting table to SIP database.
    * @since S60 3.0
    */
    static void CreateSIPSettingTableL();
    
    /**
    * Creates Profile table to Profile database.
    * @since S60 3.0
    */
    static void CreateProfileTableL();
    
    /**
    * Creates SCCP table to SCCP setting database.
    * @since S60 3.0
    */
    static void CreateSCCPSettingTableL();

private:

    /**
    * C++ default constructor.
    */
    CRCSECreateTable();

    /**
    * Destructor.
    */
    virtual ~CRCSECreateTable();
    
    /**
    * Adds column to table.
    * @since S60 3.0.
    * @param aTable Table, where the column is created.
    * @param aColumnName Name of the column
    * @param aColumnType Type of the column
    * @param aMaxLenghtOfColumn Columns maximum length.
    */
    static void AddColumnToTableL( CDbColSet& aTable, const TDesC& aColumnName,
        TDbColType aColumnType, 
        TInt aMaxLengthOfColumn = KDbDefaultTextColLength );

    };

#endif      // CRCSECREATETABLE_H  
            
// End of File
