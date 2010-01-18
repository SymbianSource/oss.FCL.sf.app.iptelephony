/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef C_CCHUITIMER_H
#define C_CCHUITIMER_H

#include <e32base.h> 

#include "cchuicommon.hrh"

class MCchUiTimerObserver;

/**
 * An instance of CCchUiTimer
 * For timer handling.
 *
 * @lib cch.lib
 * @since Series 60 5.0
 */
NONSHARABLE_CLASS( CCchUiTimer ) : public CTimer
    {    
    public:

        /**
         * Enumeration which indicates timer type
         */
        enum TTimerType
            {
            EReservedTimerType1 = 0
            };
    
        /**
         * Two-phased constructor.
         */ 
        static CCchUiTimer* NewL( 
            MCchUiTimerObserver& aObserver );


        /**
         * Destructor.
         */
        virtual ~CCchUiTimer();

        
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
        

    protected: 
    
        // from base class CTimer
        
        /**
         * From CTimer. Called when timer expires.
         */
        void RunL();
        
               
    private:

        CCchUiTimer( MCchUiTimerObserver& aObserver );

        void ConstructL();
        
        
    private:  // data
        
        /**
         * Reference for observer.
         */
        MCchUiTimerObserver& iObserver;

    CCHUI_UNIT_TEST( UT_CchUi )
    };

#endif // C_CCHUITIMER_H
