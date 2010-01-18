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
* Description:  Locally holded state for hold state machine.
*
*/


#ifndef SVPHOLDOUTSTATE_H
#define SVPHOLDOUTSTATE_H

#include "svpholdstatebase.h"
#include "svpholdcontext.h"
#include "svputdefs.h"

/**
*  Hold state for MO holded call.
*  @lib svp.dll
*  @since Series 60 3.2
*/
class CSVPHoldOutState : public CSVPHoldStateBase
    {
public:  // Constructors and destructor
    
    /**
    * Two-phased constructor.
    */
    static CSVPHoldOutState* NewLC();
    
    /**
    * Destructor.
    */
    virtual ~CSVPHoldOutState();

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
    CSVPHoldOutState();
    
    /*
    * Performs hold/resume request
    */
    TSVPHoldStateIndex PerformRequestL( CSVPHoldContext& aContext,
                                        CMceMediaStream& aMediaStream,
                                        CMceSession& aSession );   
    /*
    * Resumes session locally
    */
    void ResumeSessionLocallyL( CSVPHoldContext& aContext,
                                CMceMediaStream& aMediaStream,
                                CMceSession& aSession );
    /*
    * Handles remote doublehold requests
    */
    void RemoteSessionDoubleHoldL( CSVPHoldContext& aContext,
                                   CMceMediaStream& aMediaStream,
                                   CMceSession& aSession );

private:
    
    // For testing
    SVP_UT_DEFS
    
    };

#endif      // SVPHOLDOUTSTATE_H  
            
