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
* Description:  Establishing state class for outgoing hold/resume requests.
*
*/


#ifndef SVPHOLDOUTESTABLISHINGSTATE_H
#define SVPHOLDOUTESTABLISHINGSTATE_H

#include    "svpholdstatebase.h"
#include    "svpholdcontext.h"
#include    "svputdefs.h"

/**
*  Establishing state for local hold/resume requests.
*  @lib svp.dll
*  @since Series 60 3.2
*/
class CSVPHoldOutEstablishingState : public CSVPHoldStateBase
    {
public:  // Constructors and destructor
    
    /**
    * Two-phased constructor.
    */
    static CSVPHoldOutEstablishingState* NewLC();
    
    /**
    * Destructor.
    */
    virtual ~CSVPHoldOutEstablishingState();

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
    CSVPHoldOutEstablishingState();
    
    /**
    * Handles local cases.
    */
    void HandleLocalHoldL( CSVPHoldContext&  aContext,
                           CMceMediaStream* aMediaStream,
                           TSVPHoldStateIndex& aNextState );
    /**
    * Handles local hold case.
    */
    void HandleLocalHoldingL( CSVPHoldContext& aContext,
                              CMceMediaStream* mediaStream,
                              TSVPHoldStateIndex& aNextState,
                              TSVPHoldStateIndex aState = 0 );

    /**
    * Handles local resume case.
    */
    void HandleLocalResumingL( CSVPHoldContext& aContext,
                               CMceMediaStream* mediaStream,
                               TSVPHoldStateIndex& aNextState,
                               TSVPHoldStateIndex aState = 0 );

    /**
    * Checks acceptability of direction attribute from response.
    */
    TInt CheckAttributeL( MDesC8Array* aAttributeLines,
                          KSVPHoldAttributeIndex aNeededAttribute,
                          TSVPHoldStateIndex& aNextState,
                          CSVPHoldContext& aContext );
                          
    /**
    * Checks acceptability of direction attribute from response.
    */
    TInt DefineDefaultCaseStateChange( CSVPHoldContext& aContext,
                                       TSVPHoldStateIndex& aNextState );
                                       
    /**
    * Checks acceptability of response in case of sendrecv -response
    */
    TInt ResolveStateFromSendRecvIndex(
                KSVPHoldAttributeIndex aNeededAttribute,
                TSVPHoldStateIndex& aNextState );

private:

    // For testing
    SVP_UT_DEFS
    
    };

#endif      // SVPHOLDOUTESTABLISHINGSTATE_H  
            
