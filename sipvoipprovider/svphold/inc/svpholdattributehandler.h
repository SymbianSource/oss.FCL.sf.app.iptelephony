/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handles SDP direction attribute related issues.
*
*/


#ifndef SVPHOLDATTRIBUTEHANDLER_H
#define SVPHOLDATTRIBUTEHANDLER_H

#include <e32base.h>


/**
*  Handles SDP direction attribute related issues.
*  @lib svp.dll
*  @since Series 60 3.2
*/
class CSVPHoldAttributeHandler : public CBase
    {
public:  // Constructors and destructor
    
    /**
    * Two-phased constructor.
    */
    static CSVPHoldAttributeHandler* NewL();
    
    /**
    * Destructor.
    */
    virtual ~CSVPHoldAttributeHandler();

public:  // New functions

    /**
    * Returns index of direction attribute.
    * @since Series 60 3.2
    * @param aAttributeLines Array of attribute lines. Ownership not transferred.
    * @return Index to KSVPHoldDirectionAttributeMap.
    */
    TInt FindDirectionAttribute( MDesC8Array* aAttributeLines );

private:

    /**
    * C++ default constructor.
    */
    CSVPHoldAttributeHandler();

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();
    
private: // data
            
    // Direction attribute array
    CDesC8Array* iAttributeArray;

    };

#endif      // SVPHOLDATTRIBUTEHANDLER_H  
            
