/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class for timer handling
*
*/


#ifndef C_ACPTIMER_H
#define C_ACPTIMER_H

#include <e32base.h> 

class MAcpTimerObserver;

/**
 * An instance of CAcpTimer
 * For timer handling.
 *
 * @lib accountcreationplugin.lib
 * @since Series 60 5.0
 */
NONSHARABLE_CLASS( CAcpTimer ) : public CTimer
    {    
    public:

        /**
         * Enumeration which indicates timer type
         */
        enum TTimerType
            {
            EPhonebookStartupDelayTimer = 0
            };
    
        /**
         * Two-phased constructor.
         */ 
        static CAcpTimer* NewL( 
            MAcpTimerObserver& aObserver );


        /**
         * Destructor.
         */
        virtual ~CAcpTimer();

        
        /**
         * Start timer.
         *
         * @since S60 v5.0
         * @return KErrNone when timer started successfully
         */
        TInt StartTimer( TTimerType aType );
        
        
        /**
         * Stop timer.
         *
         * @since S60 v5.0
         */
        void StopTimer();
        
        /**
         * Is timer active
         *
         * @since S60 v5.0
         * @return ETrue if timer is active.
         */
        TBool IsActive();
        

    protected: 
    
        // from base class CTimer
        
        /**
         * From CTimer. Called when timer expires.
         */
        void RunL();
        
               
    private:

        CAcpTimer( MAcpTimerObserver& aObserver );

        void ConstructL();
        
        
    private:  // data
        
        /**
         * Reference for observer.
         */
        MAcpTimerObserver& iObserver;
    };

#endif // C_ACPTIMER_H
