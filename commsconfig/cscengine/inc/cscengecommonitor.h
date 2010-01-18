/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  For monitoring ecom events.
*
*/


#ifndef C_CSCENGECOMMONITOR_H
#define C_CSCENGECOMMONITOR_H

#include <ecom/ecom.h>

class MCSCEngEcomObserver;

/**
 * An instance of CCSCEngEcomMonitor
 * For monitoring ecom events.
 *
 * @lib cscengine.lib
 * @since Series 60 3.2
 */
NONSHARABLE_CLASS( CCSCEngEcomMonitor ) : public CActive
    {    
    public:
        
        /**
         * Two-phased constructor.
         *
         * @param aObserver for Ecom observer
         */ 
        static CCSCEngEcomMonitor* NewL( MCSCEngEcomObserver& aObserver );


        /**
         * Destructor.
         */
        virtual ~CCSCEngEcomMonitor();
    
    protected:      

        // from base class CActive
    
        /**
         * @see CActive.
         */
        void RunL();


        /**
         * @see CActive.
         */
        void DoCancel();
     
    private:

        CCSCEngEcomMonitor( MCSCEngEcomObserver& aObserver );

        void ConstructL();
        
        
        /**
         * Starts monitoring about ECom notifications.
         *
         * @since S60 v3.2
         */
        void StartMonitoring();
                
        /**
         * Frees memory allocated by array in case of leave.
         *
         * @since S60 v3.2
         * @param aArray for array to be freed
         */
        static void ResetAndDestroy( TAny* aArray );
              
    private:  // data
    
        /**
         * Reference to ECom observer.
         */
        MCSCEngEcomObserver& iObserver;
    
        /**
         * Handle to ECom session.
         * Own.
         */
        REComSession iEcomSession;
        
#ifdef _DEBUG
    friend class UT_CSCEngEcomMonitor;
#endif
    };

#endif // C_CSCENGECOMMONITOR_H
