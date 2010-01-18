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
* Description:  Provides a central repository observer for telephone volume
*
*/


#ifndef CSVPVOLUMEOBSERVER_H
#define CSVPVOLUMEOBSERVER_H

#include <e32base.h>
#include <cenrepnotifyhandler.h>
#include <MTelephonyAudioRoutingObserver.h>
#include "svputdefs.h"

// FORWARD DECLARATIONS
class MSVPVolumeUpdateObserver;
class CTelephonyAudioRouting;

 /**
  *  Provides a telephone volume for SVP session.
  *
  *  @lib svp.dll
  *  @since S60 3.2
  */
class CSVPVolumeObserver : public CBase,
                           public MCenRepNotifyHandlerCallback,
                           public MTelephonyAudioRoutingObserver 
    {
    public: // default constructor and destructor
           
        /**
         * Two-phased constructor.
         */
        static CSVPVolumeObserver* NewL(
            MSVPVolumeUpdateObserver& aObserver );
        
        /**
         * Destructor.
         */
        virtual ~CSVPVolumeObserver();
            
    private:
    
        // from MTelephonyAudioRoutingObserver
       
        void AvailableOutputsChanged(
            CTelephonyAudioRouting& aTelephonyAudioRouting );
        
        void OutputChanged( CTelephonyAudioRouting& aTelephonyAudioRouting );
        
        void SetOutputComplete(
            CTelephonyAudioRouting& aTelephonyAudioRouting, TInt aError );
        
        // from MCenRepNotifyHandlerCallback
        
        /**
          Notifies when a generic value has been changed
          @since S60 3.2
          @param aId Id of the key that has changed
         */
        void HandleNotifyGeneric( TUint32 aId );
        
    private:

        /**
         * C++ default constructor.
         */
        CSVPVolumeObserver( MSVPVolumeUpdateObserver& aObserver );

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();
         
    private:    // Data
           
        /**
         * Current telephone IHF volume setting
         */
        TInt iCurrentIhfVolume;

        /**
         * Current telephone earpiece volume setting
         */
        TInt iCurrentEarVolume;

        /**
         * Current telephone mode (IHF/EARPIECE)
         */
        TInt iMode;
        
        /**
         * Central repository instance
         * Own.
         */
        CRepository* iRepository;
        
        /**
         * Central repository notifier instance
         * Own.
         */
        CCenRepNotifyHandler* iNotifier;
       
         /**
         * Instance of telephony audio routing
         * used to track mode changes between ear and Ihf
         * Own.
         */
        CTelephonyAudioRouting* iTelephonyAudioRouting;
           
        /**
         * Reference to volume upadte observer
         * Used to inform session about volume updates
         */
        MSVPVolumeUpdateObserver& iVolumeUpdate;

        
    private: 
    
    	// For testing
      	SVP_UT_DEFS
    
    };
    

                                  
#endif //  C_SVPVOLUMEOBSERVER_H






