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
* Description:  For handling interactions betweed UI and RConnection.
*
*/



#ifndef C_CSCENGCONNECTIONHANDLER_H
#define C_CSCENGCONNECTIONHANDLER_H

#include <in_sock.h>
#include <es_sock.h>
#include <es_enum.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <es_enum_internal.h>
#endif

#include "mcscengtimerobserver.h"

class CCSCEngTimer;
class MCSCEngConnectionObserver;

/**
 * An instance of CCSCEngConnectionHandler
 * For handling interactions betweed UI and RConnection.
 *
 * @lib cscengine.lib
 * @since Series 60 3.2
 */
NONSHARABLE_CLASS( CCSCEngConnectionHandler ) : public CActive,
                                                public MCSCEngTimerObserver
    {    
    public:
        
        /**
         * Enumeration for connection events
         */
        enum TConnectionEvent
            {
            EEventConnectionDown = 0,
            EEventTimedOut
            };
                
        /**
         * Two-phased constructor.
         */ 
        IMPORT_C static CCSCEngConnectionHandler* NewL( 
            MCSCEngConnectionObserver& aObserver );


        /**
         * Destructor.
         */
        IMPORT_C virtual ~CCSCEngConnectionHandler();
        
        
        /**
         * Start listening connection events
         * 
         * @since S60 v3.2
         */
        IMPORT_C void StartListeningConnectionEvents();
        
        
        /**
         * Stop listening connection events
         * 
         * @since S60 v3.2
         */
        IMPORT_C void StopListeningConnectionEvents();
        
        
        // from base class MCSCEngTimerObserver
        
        /**
        * Called on timer expiration.
        *
        * @since S60 v3.2
        */
       void TimerExpired();


    protected: 
    
        // from base class CActive
        
        /**
         * From CActive.
         */
        void RunL();
        
        
        /**
         * From CActive.
         */
        void DoCancel();

               
    private:

        CCSCEngConnectionHandler( 
            MCSCEngConnectionObserver& aObserver );

        void ConstructL();
        
           
    private:  // data
        
        /**
         * Pointer to CSCEngTimer.
         * Own.
         */
        CCSCEngTimer* iTimer;
    
        /**
         * Reference for observer.
         */
        MCSCEngConnectionObserver& iObserver;
        
        /**
         * RConnection.
         */
        RConnection iConnection;
        
        /**
         * Socket server.
         */
        RSocketServ iSocketServ;
                
        /**
         * Interface notifiaction buffer
         */
        TInterfaceNotificationBuf iInfoBuf;
        
#ifdef _DEBUG
    friend class UT_CSCEngConnectionHandler;
#endif   
    };

#endif // C_CSCENGCONNECTIONHANDLER_H
