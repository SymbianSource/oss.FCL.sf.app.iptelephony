/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Transfer state base class for different transfer states
*
*/


#ifndef SVPTRANSFERSTATEBASE_H
#define SVPTRANSFERSTATEBASE_H

#include <e32base.h>

class CSVPTransferStateContext;

/**
 *  Transfer state base class
 *
 *  @lib svp.dll
 *  @since S60 3.2
 */
class CSVPTransferStateBase : public CBase
    {

public: // Constructors and destructor
    
    /**
     * Destructor.
     */
    virtual ~CSVPTransferStateBase();
    
public: // New functions

    /**
     * Applies the current state. Called after the state transition.
     * Leaves on error.
     * @since S60 3.2
     * @param aContext SVP Transfer context
     */
    void ApplyL( CSVPTransferStateContext& aContext );
    
    /**
     * Called when the state is entered, before it is applied.
     *
     * @since S60 3.2
     * @param aContext SVP Transfer context
     */
    void Enter( CSVPTransferStateContext& aContext );


protected: // New functions

    /**
     * Applies the current concrete state. Leaves on error.
     * Implemented by the concrete states.
     *
     * @since S60 3.2
     * @param aContext SVP Transfer context
     */
    virtual void DoApplyL( CSVPTransferStateContext& aContext ) = 0;

    /**
     * Called when the state is entered. Implemented by the concrete states.
     *
     * @since S60 3.2
     * @param aContext SVP Transfer context
     */  
    virtual void DoEnter( CSVPTransferStateContext& aContext ) = 0;

    /**
    * C++ default constructor.
    */
    CSVPTransferStateBase();
 
    };

#endif // SVPTRANSFERSTATEBASE_H
