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
* Description:  Transfer state representing the terminating state
*
*/


#ifndef SVPTRANSFERTERMINATINGSTATE_H
#define SVPTRANSFERTERMINATINGSTATE_H

#include "svptransferstatebase.h"
#include "svputdefs.h"

/**
 *  Transfer terminating state
 *  Handles logic concerning terminating state
 *
 *  @lib svp.dll
 *  @since S60 3.2
 */
class CSVPTransferTerminatingState : public CSVPTransferStateBase
    {

public:

	/**
    * Two-phased constructor.
    */
    static CSVPTransferTerminatingState* NewLC();
    
    /**
    * Destructor.
    */
    virtual ~CSVPTransferTerminatingState();

protected:

// from base class CSVPTransferStateBase

    /**
    * From CSVPTransferStateBase Apply concrete state
    */
    void DoApplyL( CSVPTransferStateContext& aContext );

    /**
    * From CSVPTransferStateBase Called when the state is entered.
    */
    void DoEnter( CSVPTransferStateContext& aContext );


private:

    /**
    * C++ default constructor.
    */
    CSVPTransferTerminatingState();

private:

    // For testing
    SVP_UT_DEFS
		
    };

#endif // SVPTRANSFERTERMINATINGSTATE_H
