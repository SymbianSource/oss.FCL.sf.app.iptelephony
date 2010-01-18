/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class for timer handling.
*
*/



#ifndef C_CSCENGTIMER_H
#define C_CSCENGTIMER_H

#include <e32base.h> 

class MCSCEngTimerObserver;

/**
 * An instance of CCSCEngTimer
 * For timer handling.
 *
 * @lib cscengine.lib
 * @since Series 60 3.2
 */
NONSHARABLE_CLASS( CCSCEngTimer ) : public CTimer
    {    
    public:

        /**
         * Enumeration which indicates timer type
         */
        enum TTimerType
            {
            EConnectionMonitoringTimer = 0
            };
    
        /**
         * Two-phased constructor.
         */ 
        IMPORT_C static CCSCEngTimer* NewL( 
            MCSCEngTimerObserver& aObserver );


        /**
         * Destructor.
         */
        IMPORT_C virtual ~CCSCEngTimer();

        
        /**
         * Start timer.
         *
         * @since S60 v3.2
         * @return KErrNone when timer started successfully
         */
        IMPORT_C TInt StartTimer( TTimerType aType );
        
        
        /**
         * Stop timer.
         *
         * @since S60 v3.2
         */
        IMPORT_C void StopTimer();
        

    protected: 
    
        // from base class CTimer
        
        /**
         * From CTimer. Called when timer expires.
         */
        void RunL();
        
               
    private:

        CCSCEngTimer( MCSCEngTimerObserver& aObserver );

        void ConstructL();
        
        
    private:  // data
        
        /**
         * Reference for observer.
         */
        MCSCEngTimerObserver& iObserver;
        
#ifdef _DEBUG
    friend class UT_CSCEngTimer;
#endif 
    };

#endif // C_CSCENGTIMER_H
