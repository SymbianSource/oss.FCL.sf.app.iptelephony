/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Timer services for SVP
*
*/



#ifndef SVPTIMER_H
#define SVPTIMER_H

#include <e32base.h>
#include "svputdefs.h"

class MSVPTimerObserver;

/**
 *  SVP timer
 *
 *  Provides timer service for SVP 
 *
 *  @lib 
 *  @since S60 3.2
 */
class CSVPTimer : public CTimer
    {

public:

    /**
    * Two-phased constructor.
    */
    static CSVPTimer* NewL( MSVPTimerObserver& aObserver,
                            TInt aTimerId );
    
    /**
    * Destructor.
    */
    virtual ~CSVPTimer();

public: // new methods

    /**
    * Sets the timer to be fired after given number of milliseconds.
    * @since Series 60 3.2
    * @param aMilliSeconds Time in milliseconds 
    * @return 
    */
    void SetTime( TInt aMilliSeconds );

    /**
    * Stops the timer.
    * @since Series 60 3.2
    * @return 
    */
    void Stop();

    /**
    * Returns the Timer ID
    * @since Series 60 3.2
    * @return TInt The timer ID.
    */
    TInt Id() const;
    
public: // methods from base classes

    /**
    * From CActive Called when an aychronic request is completed.
    * @since Series 60 3.0
    * @param None
    * @return None
    */
    void RunL();

private:

    /**
    * C++ default constructor.
    */
    CSVPTimer( MSVPTimerObserver& aObserver, TInt aTimerId );

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();

private: // data
    
    /**
    * Reference to the observer.
    */
    MSVPTimerObserver& iObserver;
  
    /**
    * The timer ID, given to observer when the timer fires.
    * Timers can be identifies with this ID.
    */
    TInt iId;

private:

    // For testing
    SVP_UT_DEFS
		
    };

#endif // SVPTIMER_H