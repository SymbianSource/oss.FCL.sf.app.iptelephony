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
* Description:  Generates DTMF events in Inband DTMF   
*
*/



#ifndef SVPDTMFEVENTGENERATOR_H
#define SVPDTMFEVENTGENERATOR_H

#include <e32base.h>
#include "svputdefs.h"

class MSVPDTMFEventObserver;

/**
 *  SVP DTMF event generator
 *  Generates DTMF events when sending Inband string sequence
 *
 *  @lib 
 *  @since S60 3.2
 */
class CSVPDTMFEventGenerator : public CActive 
    {
public:

    /**
     * Two-phased constructor.
     */
    static CSVPDTMFEventGenerator* NewL( MSVPDTMFEventObserver& aObserver );
    
    /**
     * Destructor.
     */
    virtual ~CSVPDTMFEventGenerator();

public: // new methods

    /**
     * Starts sending DTMF events
     * @Since S60 3.2
     * @return void
     */   
    void StartDtmfEvents( TInt aStringLength, TBool aPause = EFalse );
    
    /**
     * Stops the timer.
     * @since Series 60 3.2
     * @return 
     */
    void StopEvents();
 
protected: // methods from base classes

    /**
     * @see CActive
     */
    void RunL();
    
    /**
     * @see CActive
     */
    void DoCancel();
    
    /**
     * @see CActive
     */
    TInt RunError( TInt aError );
    
private:

    /**
     * C++ default constructor.
     */
    CSVPDTMFEventGenerator( MSVPDTMFEventObserver& aObserver );
    
    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private: // data
    
    /**
     * Reference to the DTMF observer.
     */
    MSVPDTMFEventObserver& iObserver;
    
    /** 
     * Holds the length of the string to be sent
     */ 
    TInt iStringLength;
   
    /**
     * Reference to RTimer 
     */
    RTimer iTimer;
     
    /**
     * Flag which determines if start event is sent
     * Needed for event sending logic when sending DMTF string
     */  
    TBool iStartSent;
    
    /**
     * Pause flag determines, whether pause should be activated
     */
    TBool iPause;

private:

    // For testing
    SVP_UT_DEFS
    
    };

#endif // SVPDTMFEVENTGENERATOR_H
