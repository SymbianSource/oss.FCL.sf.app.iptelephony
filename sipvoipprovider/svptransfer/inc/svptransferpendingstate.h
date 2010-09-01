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
* Description:  Transfer state representing the pending state
*
*/


#ifndef SVPTRANSFERPENDINGSTATE_H
#define SVPTRANSFERPENDINGSTATE_H

#include "svptransferstatebase.h"
#include "svputdefs.h"

/**
 *  Transfer pending state
 *
 *  Handles logic concerning pending state
 *
 *  @lib svp.dll
 *  @since S60 3.2
 */
class CSVPTransferPendingState : public CSVPTransferStateBase
    {

public:

    /**
    * Two-phased constructor.
    */
    static CSVPTransferPendingState* NewLC();
    
    /**
    * Destructor.
    */
    virtual ~CSVPTransferPendingState();

protected:

// from base class CSVPTransferStateBase
    
    /**
    * From CSVPTransferStateBase. Apply concrete state.
    */
    void DoApplyL( CSVPTransferStateContext& aContext );

    /**
    * From CSVPTransferStateBase. Called when the state is entered.
    */
    void DoEnter( CSVPTransferStateContext& aContext );


private:

    /**
    * C++ default constructor.
    */
    CSVPTransferPendingState();


private:

    // For testing
    SVP_UT_DEFS
		
    };

#endif // SVPTRANSFERPENDINGSTATE_H
