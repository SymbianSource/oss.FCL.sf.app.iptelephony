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
* Description:  cchwakeupeventnotifier declaration
*
*/


#ifndef C_CCHWAKEUPEVENTNOTIFIER_H
#define C_CCHWAKEUPEVENTNOTIFIER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <mspnotifychangeobserver.h>
#include "cchetelnetworkstatusobserver.h"

// CONSTANTS

// Security Policy

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CSPNotifyChange;
class CCchEtelNetworkStatusNotifier;
class MCchWakeUpEventObserver;
class CRepository;
// CLASS DECLARATION

/**
 *  CCchWakeUpEventNotifier declaration
 *  The server of CCH services.
 *  @lib cchserver.exe
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CCchWakeUpEventNotifier ) : public CActive,
                            public MCchEtelNetworkStatusObserver,
                            public MSPNotifyChangeObserver
    {
public: // Constructors and destructor

    /**
    * Two-phased constructor.
    */
    static CCchWakeUpEventNotifier* NewL( 
            MCchWakeUpEventObserver& aWakeUpEventObserver );

    /**
     * Two-phased constructor.
     */
    static CCchWakeUpEventNotifier* NewLC( 
            MCchWakeUpEventObserver& aWakeUpEventObserver );

    /**
     * Destructor.
     */
    virtual ~CCchWakeUpEventNotifier();

public: // New functions

    /**
     * Starts observering change events.
     */
    void StartL();
    
    /**
     * Stops observering change events.
     */
    void Stop();
    
private: 
    
    /**
     * Starts offline timer
     * @param aFunction The function to call after the time out
     */
    void StartOfflineTimer( TInt (*aFunction)(TAny* aPtr) );

    /**
     * Cancels the offline timer
     */
    void CancelOfflineTimer();
        
    /**
     * Offline tiemout callback
     * @param aSelf this object
     */
    static TInt OfflineTimeout( TAny* aSelf );
        
    /**
     * Handles offline timeout
     */
    void HandleOfflineTimeout();

public: // From CActive
    
    /**
    * Implements cancellation of an outstanding request.
    * @see CActive::Cancel
    */
    virtual void DoCancel();


    /**
    * Handles an active object's request completion event.
    */
    virtual void RunL();
    
    /**
    * Handles leaving of RunL.
    */
    virtual TInt RunError( TInt aError );

     
public: // From MCchEtelNetworkStatusObserver
    
    /**
    * This is a callback function which is called when CS 
    * has gone out of coverage 
    */         
    virtual void MobileNetworkNoService( );

public: // From MSPNotifyChangeObserver
    
    /**
    * Handle notify change event
    * 
    * @param aServiceId the service ID of added/changed/deleted service
    * @since S60 3.2
    */
    virtual void HandleNotifyChange( TServiceId aServiceId );

    /**
    * Handle error
    * 
    * @param aError error code
    * @since S60 3.2
    */
    virtual void HandleError( TInt aError );

private:

    /**
     * C++ default constructor.
     */
    CCchWakeUpEventNotifier( MCchWakeUpEventObserver& aWakeUpEventObserver );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();


private: // data

    // Observer of wake up events
    MCchWakeUpEventObserver& iWakeUpEventObserver;

    // Change notifer of spsettings
    CSPNotifyChange* iSPSNotifier;
    
    // Mobile network notifer
    CCchEtelNetworkStatusNotifier* iCchEtelNetworkNotifier;
    
    // Repository for monitoring offline mode
    CRepository* iOfflineRepository;

    // Are we in state that network connection is allowed
    TInt iNetworkConnectionAllowed;
    
    // Indicates is change events observation started.
    TBool iObservationOngoing;
    
    // Offline timer. Owned.
    CPeriodic* iOfflineTimer;
    };

#endif // C_CCHWAKEUPEVENTNOTIFIER_H

// End of file
