/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Activates the notification of gsm signal level strengths
*
*/



#ifndef VCCGSMNOTIFICATIONS_H_
#define VCCGSMNOTIFICATIONS_H_

#include <etelmm.h> 

/**
 *  Observer which will receive notification about gsm signal level changes
 *
 *  @lib vcchotrigger.lib
 *  @since S60 3.2
 */
class MVccGsmNotifications
    {
    public:
        /**
        * Constructor.
        *
        * @since S60 3.2
        */
        virtual void GsmStrengthChanged( TInt32 aStrength ) = 0;
    };
    
/**
 *  Clas which starts and stops notification of the gsm signal level 
 *
 *  @lib vcchotrigger.lib
 *  @since S60 3.2
 */
class CVccGsmNotifications : public CActive
    {
    public:
        /**
        * Two-phased constructor.
        *
        * @param aPhone RMobilePhone
        */
        static CVccGsmNotifications* NewL( RMobilePhone& aPhone );
        
        /**
        * Destructor.
        */
        ~CVccGsmNotifications();
        
        /**
         * Activates gsm signal level notification
         *
         * @since S60 3.2
         * @param aObserver observer which will receive notification
         * @param aLow value for low signal level
         * @param aHigh value for high signal level
         *
         */
        void ActivateNotifications( MVccGsmNotifications* aObserver, TInt32 aLow, TInt32 aHigh );
        
        /**
         * Cancels gsm signal level notification
         *
         * @since S60 3.2
         */
        void CancelNotifications();
    
    protected:
        
        /**
         * Symbian second-phase constructor
         */
        void ConstructL();
        
        /**
         * C++ constructor
         */
        CVccGsmNotifications( RMobilePhone& aPhone );
    
    private: // From CActive
        
        /**
         * @see CActive::RunL()
         */
        void RunL();
        
        /**
         * @see CActive::Cancel()
         */
        void DoCancel();
    
    private:
        /**
        * RMobilePhone handle
        * Not own
        */
        RMobilePhone& iPhone;
        
        /**
         * Observer
         * Not own
         */
        MVccGsmNotifications* iObserver;
        
        /**
        * Gsm signal strength value
        */
        TInt32 iStrength;

        /**
        * Gsm signal bar value
        */
        TInt8 iBar;
        
        /**
        * Gsm signal strength value for low signal level
        */
        TInt32 iLow;

        /**
        * Gsm signal strength value for high signal level
        */
        TInt32 iHigh;
    };

#endif // VCCGSMNOTIFICATIONS_H_
