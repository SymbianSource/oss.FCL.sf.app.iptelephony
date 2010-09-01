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
* Description:  Establishing state class for incoming hold/resume requests.
*
*/


#ifndef SVPHOLDINESTABLISHINGSTATE_H
#define SVPHOLDINESTABLISHINGSTATE_H

#include    "svpholdstatebase.h"
#include    "svpholdcontext.h"
#include    "svputdefs.h"

/**
*  Establishing state for incoming hold/resume requests
*  @lib svp.dll
*  @since Series 60 3.2
*/
class CSVPHoldInEstablishingState : public CSVPHoldStateBase
    {
public:  // Constructors and destructor
    
    /**
    * Two-phased constructor.
    */
    static CSVPHoldInEstablishingState* NewLC();
    
    /**
    * Destructor.
    */
    virtual ~CSVPHoldInEstablishingState();
    

protected:  // Functions from base classes
    
    /**
    * From CSVPHoldStateBase Apply concrete state.
    */
    void DoApplyL( CSVPHoldContext& aContext );

    /**
    * From CSVPHoldStateBase Called when the state is entered.
    */
    void DoEnter( CSVPHoldContext& aContext );

    /**
    * Checks if OutEstablishingState is active.
    */
    TBool IsOutEstablishingStateActive();
    

private:

    /**
    * C++ default constructor.
    */
    CSVPHoldInEstablishingState();

    /**
    * Handles local hold case.
    */
    void HandleRemoteHolding( TSVPHoldStateIndex& aNextState,
                              TSVPHoldStateIndex aState = 0 );

    /**
    * Handles local resume case.
    */
    void HandleRemoteResuming( TSVPHoldStateIndex& aNextState,
                               TSVPHoldStateIndex aState = 0 );

    /**
    * Checks acceptability of direction attribute from request.
    */
    TInt CheckAttribute( MDesC8Array* aAttributeLines,
                         KSVPHoldAttributeIndex aNeededAttribute,
                         TSVPHoldStateIndex& aNextState,
                         CSVPHoldContext& aContext );
                         
    /**
    * Defines state change in basic schene.
    */
    TInt DefineDefaultCaseStateChange( CSVPHoldContext& aContext,
                                       TSVPHoldStateIndex& aNextState );
        
private:

    // For testing
    SVP_UT_DEFS
    
    };

#endif      // SVPHOLDINESTABLISHINGSTATE_H  
            
