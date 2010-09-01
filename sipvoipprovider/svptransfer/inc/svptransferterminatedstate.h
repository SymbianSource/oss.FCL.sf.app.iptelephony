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
* Description:  Transfer state representing the terminated state
*
*/



#ifndef SVPTRANSFERTERMINATEDSTATE_H
#define SVPTRANSFERTERMINATEDSTATE_H

#include "svptransferstatebase.h"
#include "svputdefs.h"

/**
 *  Transfer terminated state
 *
 *  Handles logic concerning terminated state
 *
 *  @lib svp.dll
 *  @since S60 3.2
 */
class CSVPTransferTerminatedState : public CSVPTransferStateBase
    {

public:

    /**
    * Two-phased constructor.
    */
    static CSVPTransferTerminatedState* NewLC();
    
    /**
    * Destructor.
    */
    virtual ~CSVPTransferTerminatedState();

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
    CSVPTransferTerminatedState();


private:

    // For testing
    SVP_UT_DEFS
		
    };

#endif // SVPTRANSFERTERMINATEDSTATE_H
