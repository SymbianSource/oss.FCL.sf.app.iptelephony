/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CCchUIHandler declaration
*
*/


#ifndef C_CCHUIHANDLER_H
#define C_CCHUIHANDLER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "cchetelnetworkstatusobserver.h"
#include "cchfeaturemanager.h"
// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CCCHServiceHandler;
class CCCHServerBase;
class CRepository;
class CCchNoteHandler;
class CCchEtelNetworkStatusNotifier; 
class MCchEtelNetworkStatusObserver;

// CLASS DECLARATION

/**
 *  CCchUIHandler declaration
 *  Displays handles CCH's UI, emergency notes, small icons
 *  @lib cchserver.exe
 *  @since S60 3.2
 */
class CCchUIHandler : public CActive, 
                      public MCchEtelNetworkStatusObserver
    {

public: // Constructors and destructor

    /**
    * Two-phased constructor.
    */
    static CCchUIHandler* NewL( 
        CCCHServerBase& aServer,
        CCCHServiceHandler& aCCchServiceHandler );

    /**
     * Two-phased constructor.
     */
    static CCchUIHandler* NewLC( 
        CCCHServerBase& aServer,
        CCCHServiceHandler& aCCchServiceHandler );

    /**
     * Destructor.
     */
    virtual ~CCchUIHandler();

public: // New functions

    /**
    * shows emergency warning note.
    * @param: TInt
    * @return void
    */
    void ShowEmergencyWarningNoteL( TBool aVoIPEnabledFirstTime );
    
    /**
    * Updates the user interface (e.g. voip small icon)
    * @param: TInt
    * @return void
    */
    void UpdateUI( );
    
    /**
    * Checks gprs first usage. If used first time shows 
    * gprs roaming cost warning note.
    * @return void
    */
    void CheckGprsFirstUsageL();
    
    /**
    * Return network connections allowed status.
    * @return ETrue if network connections are allowed
    */
    TBool NetworkConnectionsAllowed() const;
    
    /**
     * Waits one second to give enought time to end async requests
     */
    void Destroy();

public: // From MCchEtelNetworkStatusObserver

    /**
    * This is a callback function which is called when CS 
    * has gone out of coverage 
    */
    void MobileNetworkNoService( );

    /**
    Implements cancellation of an outstanding request.
    @see CActive::Cancel
    */
    virtual void DoCancel();


    /**
    Handles an active object's request completion event.
    */
    virtual void RunL();
    
    /**
    Handles leaving of RunL.
    */
    virtual TInt RunError(TInt aError);
    
private:

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * C++ default constructor.
     */
    CCchUIHandler( 
        CCCHServerBase& aServer, 
        CCCHServiceHandler& aCCchServiceHandler );
    
    /**
    * Sets the indicator's aIndicator state to state aState.
    * @param TInt aIndicator 
    * @param TInt aState
    * @return void
    */
    void SetIndicatorStateL( TInt aIndicator, TInt aState ) const;

    /**
    * Called when VoIP status is changed
    * @param aStatus for VoIP status
    */
    void HandleVoipStateChanged( TBool aStatus );
    
    /**
    * Called (from HandleVoipStateChanged) when VoIP status is changed
    * @param aStatus for VoIP status
    */
    void HandleVoipStateChangedL( TBool aStatus );
    
    /**
     * Checks offline status from KCRUidCoreApplicationUIs cenrep
     * and starts to monitor changes in offline status
     */
    void MonitorOfflineStatusL();

private: // data

    /**
     * Handle to server
     */
    CCCHServerBase& iServer;

    /**
     * Handle to service handler
     */
    CCCHServiceHandler& iCCchServiceHandler;

    /**
     * Note handler
     */
    CCchNoteHandler* iNoteHandler;

    /**
     * Are we out of cs network?
     */
    TBool iVoIPEmergencyNoteState;
    
    /**
     * ETel observer
     */
    CCchEtelNetworkStatusNotifier* iCchEtelNetworkStatusNotifier;
    
    /**
     * Are we showing the VoIP small indicator?
     */    
    TBool iVoIPSmallIndicatorShown;
    
    /**
     * Cch's repository
     */ 
    CRepository* iCchRepository;
    
    /**
     * Repository for monitoring offline mode
     */ 
    CRepository* iOfflineRepository;
    
    /**
     * is the connection allowed
     */ 
    TInt iNetworkConnectionAllowed;
    
    /**
     * Reference to RTimer 
     */
    RTimer iTimer;

    /**
     * Should I commit suicide
     */
    TBool iDie;
    };

#endif // C_CCHUIHANDLER_H

// End of file
