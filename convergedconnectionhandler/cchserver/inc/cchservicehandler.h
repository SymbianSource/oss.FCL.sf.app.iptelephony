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
* Description:  CCCHServiceHandler declaration
*
*/


#ifndef C_CCHSERVICEHANDLER_H
#define C_CCHSERVICEHANDLER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <mspnotifychangeobserver.h>

#include "cchserviceinfo.h"
#include "cchsubserviceinfo.h"
#include "cchservicenotifier.h"
#include "cchclientserver.h"
#include "cchcommdbwatcherobserver.h"

// CONSTANTS
// None

// MACROS
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CCCHServiceInfo;
class CCCHServerBase;
class CCCHSubserviceInfo;
class CSPNotifyChange;
class CCchUIHandler;
class CCchWlanExtension;
class CCCHCommDbWatcher;

// DATA TYPES
typedef RPointerArray<CCCHServiceInfo> RServiceArray;

// CLASS DECLARATION

/**
 *  CCCHServiceHandler declaration
 *  Handles services and service related components
 *  @lib cchserver.exe
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CCCHServiceHandler ) : public CBase,
                                          private MCCHServiceNotifier,
                                          private MSPNotifyChangeObserver,
                                          private MCCHCommDbWatcherObserver
    {

public: // Constructors and destructor

    /**
    * Two-phased constructor.
    */
    static CCCHServiceHandler* NewL( CCCHServerBase& aServer );

    /**
     * Two-phased constructor.
     */
    static CCCHServiceHandler* NewLC( CCCHServerBase& aServer );

    /**
     * Destructor.
     */
    virtual ~CCCHServiceHandler();

public: // New functions

    /**
     * Initialize object
     * @since S60 3.2
     */
    void InitServiceHandlerL();

    /**
     * Update service and subservice's state
     * @since S60 3.2
     * @param aReadSettingTable If ETrue class reads and updates 
     * all services from Service Provider Settings
     */
    void UpdateL( TBool aReadSettingTable = EFalse );

    /**
     * Enables service
     * @since S60 3.2
     * @param aServiceSelection Service to enable
     * @param aConnectivityCheck Is value is True Connectivity Plug-in will
     * make test call to check is the connection truly working.
     * @return General symbian error code.
     */
    TInt EnableService( const TServiceSelection aServiceSelection,
                        const TBool aConnectivityCheck );

    /**
     * Disables service
     * @since S60 3.2
     * @param aServiceSelection Service to disable
     * @return General symbian error code.
     */
    TInt DisableService( const TServiceSelection aServiceSelection );

    /**
     * Set new connection information to Service
     * @since S60 3.2
     * @param aServiceConnInfo Connection information
     * @return General symbian error code.
     */
    TInt SetConnectionInfo( const TServiceConnectionInfo aServiceConnInfo );

    /**
     * Get connection information from Service
     * @since S60 3.2
     * @param aServiceSelection Selected Service
     * @param aServiceConnInfo On completion contains Service's 
     * connection infomation
     * @return General symbian error code.
     */
    TInt GetConnectionInfo( TServiceConnectionInfo& aServiceConnInfo );

    /**
     * Is any service's startup-flag set to true
     * @since S60 3.2
     * @return ETrue if startup-flag is set to true
     */
    TBool IsStartupFlagSet() const;
    
    /**
     * Subservice's enable at startup info setter. 
     * Method does not change value for permanently. 
     * CCCHSPSHandler::SetLoadAtStartUpL sets value for permanently. 
     * @since S60 3.2
     * @param aServiceSelection Service which owns startup flag
     * @param aLoadAtStartUp New startup flag
     * @return KErrNotFound if service or subservice does not exist
     */
    TInt SetStartupFlag( const TServiceSelection aServiceSelection, 
        TBool aLoadAtStartUp ) const;
    
    /**
     * Load Plug-ins
     * @since S60 3.2
     */
    void LoadPluginsL();
    
    /**
     * Unloads Plug-in(s) of certain service; if all
     * subservices in one plugin are disabled, those are unloaded.
     * @since S60 3.2
     */
    void UnloadDisabledPluginsL( );
    
    /**
     * Checks if certain subservice exists in certain state
     * @param aType Subservice type
     * @param aState Subservice state to be checked
     * @since S60 3.2
     */
    TBool Exists( TCCHSubserviceType aType, 
                  TCCHSubserviceState aState,
                  TInt aError ) const;
                                  
    /**
     * Count of Services
     * @since S60 3.2
     * @param aMessage IPC message;
     */
    void ServiceCountL( RMessage2 aMessage ) const;
    
    /**
     * Get all services and subservices.
     * @since S60 3.2
     * @param aMessage IPC message
     */
    void GetServicesL( RMessage2 aMessage ) const;

    /** 
     * Get state of service or subservice of a certain service
     * @since S60 3.2
     * @param aMessage IPC message
     * @return General symbian error code.
     */
    TInt GetServiceState( const RMessage2 aMessage );

    /** 
     * Get state of service
     * @since S60 3.2
     * @param aServiceId Service which to be checked
     * @return A state of the service.
     */
    TCCHSubserviceState CCCHServiceHandler::ServiceState(
            const TUint aServiceId );
    
    /** 
     * Get info of service or subservice of a certain service
     * @since S60 3.2
     * @param aMessage IPC message
     * @return General symbian error code.
     */
    TInt GetServiceInfo( const RMessage2 aMessage );

    /** 
     * Reserves the service for exclusive use
     * @since S60 3.2
     * @param aMessage IPC message
     * @return General symbian error code.
     */
    TInt ReserveService( const RMessage2 aMessage );

    /** 
     * Frees the exclusive service reservation
     * @since S60 3.2
     * @param aMessage IPC message
     * @return General symbian error code.
     */
    TInt FreeService( const RMessage2& aMessage );
    
    /** 
     * Is the service exclusively reserved?
     * @since S60 3.2
     * @param aMessage IPC message
     * @return ETrue if service is reserved for exclusive use.
     */
    void IsReserved( RMessage2 aMessage ) const;
    
    /**
     * Service exist
     * @since S60 3.2
     * @param aSelection Checked Service
     * @return TInt index if found else KErrNotFound 
     */
    TInt ServiceExist( TServiceSelection aSelection ) const;
    
    /**
     * Find service by service id
     * @since S60 3.2
     * @param serviceId
     * @return TInt position within the iServices array or KErrNotFound
     */
    TInt FindService( TUint32 aServiceId ) const;
    
    /**
     * Subscribe to service change notifies
     * @since S60 3.2
     */
    void EnableNotifyChange();
    
    /**
     * Cancel service change notifies
     * @since S60 3.2
     */
    void DisableNotifyChange();
        
    /**
     * Get all subservice types which uses given Plug-in uid.
     * @since S60 3.2
     * @param aServiceId Service which to be checked
     * @param aSubserviceTypes On completion contains subservice types.
     */
    void GetSubserviceTypesL( const TUint aServiceId, 
        RArray<TCCHSubserviceType>& aSubserviceTypes ) const;
    
    /**
     * Disables Wlan scan if needed
     */
    void DisableWlanScan();
    
    /**
     * Handle restart situation, all services and subservices
     * goes to disabled state.
     */
    void HandleRestartL();
    
protected: // From base classes
    
    /**
     * Determines if we should start recovery
     * @since S60 3.2
     * @param aServiceId Service which state has changed
     * @param aError Subservice's error. 
     * @return ETrue if recovery is needed
     */

    TBool IsRecoveryNeeded( const TCCHSubserviceState aState, 
                            TInt aError );
                               
    /**
     * Service state notifier.
     * @since S60 3.2
     * @param aServiceId Service which state has changed
     * @param aSubService Subservice's type. 
     * @param aState A new state of the service.
     */
    void StateChangedL( const TUint aServiceId,
                        const TCCHSubserviceType aSubService,
                        const TCCHSubserviceState aState,
                        const TInt aError );
        
    /**
     * Handle notify change event.
     * From MSPNotifyChangeObserver;
     *
     * @since S60 3.2
     * @param aServiceId The service ID of changed service
     */
    void HandleNotifyChange( TServiceId aServiceId );
    
    /**
     * Handle error
     * 
     * @since S60 3.2
     */
    void HandleError( TInt aError );
    
private:

    /**
     * C++ default constructor.
     */
    CCCHServiceHandler( CCCHServerBase& aServer );
    
    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * Add service to service array
     * @since S60 3.2
     * @param aService service information
     */
    void AddServiceL( TCCHService& aService );
    
    /**
     * Counts how many Service contains this kind of Subservice
     * @since S60 3.2
     * @param aType Subservice's type
     * @return TInt Count of Service
     */
    TInt ServiceCount( const TCCHSubserviceType aType ) const;

    /**
     * Starts connection recovery timer
     * @param aFunction The function to call after the time out
     */
    void StartConnectionRecoveryTimer( TInt (*aFunction)(TAny* aPtr) );

    /**
     * Cancels the connection recovery timer
     */
    void CancelConnectionRecoveryTimer();
    
    /**
     * Connection recovery callback
     * @param aSelf this object
     */
    static TInt ConnectionRecoveryEvent( TAny* aSelf );
    
    /**
     * Handles connection recovery event
     */
    void HandleConnectionRecovery();
    
    /**
     * Starts plugin unload timer
     * @param aFunction The function to call after the time out
     */
    void StartPluginUnloadTimer();

    /**
     * Cancels the connection recovery timer
     */
    void CancelPluginUnloadTimer();
    
    /**
     * Starts handle notify delay timer
     * @param aFunction The function to call after the time out
     */
    void StartHandleNotifyDelayTimer();

    /**
     * Cancels the handler notify delay timer
     */
    void CancelHandleNotifyDelayTimer();
    
    /**
     * Plugin unload callback
     * @param aSelf this object
     */
    static TInt PluginUnloadEvent( TAny* aSelf );
    
    /**
     * Handle notify event callback
     * @param aSelf this object
     */
    static TInt HandleNotifyEvent( TAny* aSelf );
    
    /**
     * Handles plugin unload event
     */
    void HandlePluginUnload();
    
    /**
     * Handles delayed notify event
     */
    void HandleDelayedNotifyEvent();
    
    /**
     * From MCCHCommDbWatcherObserver, handles commsdb events
     */
    void HandleWLANIapAdded( TInt aSNAPId );
    
    /**
     * Checks if there is wlan iap used, second part
     */
    TBool UsesWlanIap( const TServiceSelection& aServiceSelection );
    
    /**
     * Checks if there is wlan iap used
     */
    TBool HasWlanIap( const TServiceSelection& aServiceSelection );
    
    /**
     * Checks if IAP or SNAP is defined for certain subservice
     */
    TBool IsConnectionDefinedL( const TServiceSelection& aServiceSelection );
    
    /**
     * Checks if IAP or SNAP is defined for certain service
     */
    TBool IsServiceConnectivityDefinedL( const TServiceSelection& aServiceSelection );
    
private: // data

    /**
     * Handle to server
     */
    CCCHServerBase&                 iServer;

    /**
     * Service array
     */
    RServiceArray                   iServices;
    
    /**
     * Pointer to CSPNotifyChange
     */
    CSPNotifyChange*                iNotifier;
    
    /**
     * Service id array
     */
    RIdArray                        iServiceIds;
    
    /**
     * UI handler to show VoIP small icon etc.
     */
    CCchUIHandler*                  iCchUIHandler;
    
    /**
     * Connection recovery timer. Owned.
     */
    CPeriodic* iConnectionRecoveryTimer;
    
    /**
     * Plugin unload timer. Owned.
     */
    CPeriodic* iPluginUnloadTimer;
    
    /**
     * Handle notify delay timer. Owned.
     */
    CPeriodic* iHandleNotifyDelayTimer;
    
    /**
     * Recovery trial counter
     */
    TInt iConnectionRecoveryTry;
    
    /**
     * Services that require recovery
     */
    RArray<TServiceSelection> iServicesInRecovery;
    
    /**
     * Recovery intervals
     */
    RArray<TInt> iRecoveryInterval;
    
    /**
     * WLAN extension
     */
    CCchWlanExtension* iWlanExtension;
    
    /**
     * CommsDb watcher
     */
    CCCHCommDbWatcher* iCommDbWatcher;
    
    /**
     * Disable SPSettings notifications
     */
    TBool iCancelNotify;
    
    /**
     * Service id of delayed handle notify event
     */
    TServiceId iDelayedHandleNotifyServiceId;
    };

#endif // C_CCHSERVICEHANDLER_H

// End of file
