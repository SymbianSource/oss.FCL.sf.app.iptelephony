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
* Description:  Base class for state.
*
*/


#ifndef SVPHOLDSTATEBASE_H
#define SVPHOLDSTATEBASE_H

#include <e32base.h>

class CSVPHoldContext;

/**
*  Base class for state.
*  
*  @lib svp.dll
*  @since Series 60 3.2
*/
class CSVPHoldStateBase : public CBase
    {
public:  // Constructors and destructor
    
    /**
    * Destructor.
    */
    virtual ~CSVPHoldStateBase();

public: // New functions
    
    /**
    * Applies the current state. Called after the state transition.
    * Leaves on error.
    * @since Series 60 3.2
    * @param aContext The Context of hold
    */
    void ApplyL( CSVPHoldContext& aContext );

    /**
    * Called when the state is entered, before it is applied.
    * @since Series 60 3.2
    * @param aSession The MCE session
    */
    void Enter( CSVPHoldContext& aSession );

    /**
    * Checks if OutEstablishingState is active.
    * @since Series 60 3.2
    * @return ETrue if establishing state active, EFalse otherways
    */
    virtual TBool IsOutEstablishingStateActive();

   
protected:  // New functions
    
    /**
    * Applies the current concrete state. Leaves on error.
    * @since Series 60 3.2
    * @param aContext SVP Hold context
    * @return None
    */
    virtual void DoApplyL( CSVPHoldContext& aContex ) = 0;

    /**
    * Called when the state is entered. Implemented by
    * concrete states.
    * @since Series 60 3.2
    * @param aContext SVP Hold context
    * @return None
    */
    virtual void DoEnter( CSVPHoldContext& aContex ) = 0;
    
    /**
    * C++ default constructor.
    */
    CSVPHoldStateBase();
        
    };

#endif      // SVPHOLDSTATEBASE_H 
            
