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
* Description:  CCCHPluginHandler declaration
*
*/


#ifndef C_CCHPLUGINHANDLER_H
#define C_CCHPLUGINHANDLER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <ecom/implementationinformation.h>

#include "cchclientserver.h"

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CCCHServerBase;
class CCCHPlugin; 
class REComSession;
class MCCHServiceNotifier;

// CLASS DECLARATION

/**
 *  CCCHPluginHandler declaration
 *  Class handles plug-in's for CCH server
 *  @lib cchserver.exe
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CCCHPluginHandler ) : public CBase
    {

public: // Constructors and destructor

    /**
    * Two-phased constructor.
    */
    static CCCHPluginHandler* NewL( CCCHServerBase& aServer );

    /**
     * Destructor.
     */
    virtual ~CCCHPluginHandler();
    
public: // New functions
    
    /**
     * Load all available plug-ins
     * @since S60 3.2
     */
    void LoadPluginsL();

    /**
     * Load a certain Plug-in
     * @since S60 3.2
     * @param aUid Plug-ins Uid
     */
    void LoadPluginsL( const TUid& aUid );
    
    /**
     * Unloads a certain Plug-in
     * @since S60 3.2
     * @param aUid Plug-ins Uid
     */
    void UnloadPlugin( const TUid& aUid );

    /**
     * Enable a certain Service
     * @since S60 3.2
     * @param aServiceId Service to enable
     * @param aUids Uids-array contains Connectivity Plug-ins Uids
     * @param aType Subservice's type to enable
     * @param aObserver Pointer to observer.
     * @param aIapId Used IAP Id
     * @param aConnectivityCheck Is value is True Connectivity Plug-in will
     * make test call to check is the connection truly working.
     */
    void EnableServiceL( const TUint32 aServiceId,
                         const RArray<TUid>& aUids,
                         const TCCHSubserviceType aType,
                         MCCHServiceNotifier* aNotifier,
                         const TUint32 aIapId = 0,
                         const TBool aConnectivityCheck = EFalse );

    /**
     * Disable a certain Service
     * @since S60 3.2
     * @param aServiceId Service to disable
     * @param aUids Uids-array contains Connectivity Plug-ins Uids
     * @param aType Subservice's type to disable
     */
    void DisableServiceL( const TUint32 aServiceId,
                          const RArray<TUid>& aUids,
                          const TCCHSubserviceType aType ) const;

    /**
     * Get a certain Service's state
     * @since S60 3.2
     * @param aServiceId Used Service Id
     * @param aUid Service's Uid 
     * @param aType Subservice's type
     * @param aState On completion contains Service's state
     * @param aError Returns the current error
     */
    void GetServiceState( const TUint32 aServiceId,
                          const TUid aUid,
                          const TCCHSubserviceType aType,
                          TCCHSubserviceState& aState,
                          TInt& aError );

    /**
     * Get a certain Service's network(SNAP/IAP) information
     * @since S60 3.2
     * @param aServiceId Used Service Id
     * @param aUid Service's Uid 
     * @param aType Subservice's type
     * @param aSnapId On completion contains Service's SNAP Id
     * @param aIapId On completion contains Service's IAP Id
     * @param aSNAPLocked On completion contains ETrue if Service's SNAP Id cannot be edited
     * @return General symbian error code.
     */
    TInt GetServiceNetworkInfo( const TUint32 aServiceId,
                                const TUid aUid,
                                const TCCHSubserviceType aType,
                                TUint32& aSnapId,
                                TUint32& aIapId,
                                TBool& aSNAPLocked,
                                TBool& aPasswordSet);

    /**
     * Set Service to use given SNAP Id
     * @since S60 3.2
     * @param aServiceId Used Service Id
     * @param aUids Uids-array contains Connectivity Plug-ins Uids
     * @param aType Subservice's type
     * @param aSnapId Set Service to use this SNAP Id
     */
    void SetSnapIdL( const TUint32 aServiceId,
                     const RArray<TUid>& aUids,
                     const TCCHSubserviceType aType,
                     const TUint32 aSnapId );

    /**
     * Set Service to use given IAP Id
     * @since S60 3.2
     * @param aServiceId Used Service Id
     * @param aUids Uids-array contains Connectivity Plug-ins Uids
     * @param aType Subservice's type
     * @param aIapId Set Service to use this IAP Id
     * @leave Symbian error code
     */
    void SetIapIdL( const TUint32 aServiceId,
                    const RArray<TUid>& aUids,
                    const TCCHSubserviceType aType,
                    const TUint32 aIapId );

    /**
     * Set Service to use given username
     * @since S60 3.2
     * @param aServiceId Used Service Id
     * @param aUids Uids-array contains Connectivity Plug-ins Uids
     * @param aType Subservice's type
     * @param aUsername Set Service to use this username
     * @return In case of an error leaves. 
     *         KErrNotFound or symbian error code.
     *         KErrArgument if passed argument(s) is/are incorrect
     */
    void SetUsernameL( const TUint32 aServiceId,
                       const RArray<TUid>& aUids,
                       const TCCHSubserviceType aType,
                       const TDesC& aUsername );

    /**
     * Returns the connection parameters
     * @since S60 3.2.3
     * Uid Plug-in uid.
     * @param aServiceSelection Selected service, ID and type
     * @param aValue Value of the parameter
     * @return Symbian error code
     */
    TInt GetConnectionParameter( const TUid aUid,
                                 const TServiceSelection& aServiceSelection, 
                                 RBuf& aValue );

    
    /**
     * Set Service to use given password
     * @since S60 3.2
     * @param aServiceId Used Service Id
     * @param aUids Uids-array contains Connectivity Plug-ins Uids
     * @param aType Subservice's type
     * @param aUsername Set Service to use this password
     * @return In case of an error leaves. 
     *         KErrNotFound or symbian error code.
     *         KErrArgument if passed argument(s) is/are incorrect
     */
    void SetPasswordL( const TUint32 aServiceId,
                       const RArray<TUid>& aUids,
                       const TCCHSubserviceType aType,
                       const TDesC& aPassword );
    
    /**
     * Get Service's protocol information
     * @since S60 3.2
     * @param aServiceId Used Service Id
     * @param aUid Service's Uid 
     * @param aType Subservice's type
     * @param aBuffer On completion contains Service's information
     * @return General symbian error code.
     */
    TInt GetServiceInfo( const TUint32 aServiceId,
                         const TUid aUid,
                         const TCCHSubserviceType aType,
                         TDes& aBuffer );

    /**
     * Reserve service for exclusive use
     * @since S60 3.2
     * @param aServiceId Used Service Id
     * @param aUid Service's Uid 
     * @param aType Subservice's type
     * @return General symbian error code.
     */
    TInt ReserveService( const TUint32 aServiceId,
                         const TUid aUid,
                         const TCCHSubserviceType aType );

    
    /**
     * Free exlusive service reservation
     * @since S60 3.2
     * @param aServiceId Used Service Id
     * @param aUid Service's Uid 
     * @param aType Subservice's type
     * @return General symbian error code.
     */
    TInt FreeService( const TUint32 aServiceId,
                         const TUid aUid,
                         const TCCHSubserviceType aType );
    
    /**
     * Returns the service's reservation status
     * @since S60 3.2
     * @param aServiceId Used Service Id
     * @param aUid Service's Uid 
     * @param aType Subservice's type
     * @return Reservation status.
     */
    TBool IsReserved( const TUint32 aServiceId,
                         const TUid aUid,
                         const TCCHSubserviceType aType ) const;
                         
    /**
     * Is Service available via these IAPs
     * @since S60 3.2
     * @param aServiceId Used Service Id
     * @param aUid Service's Uid 
     * @param aType Subservice's type
     * @param aIapIdArray Id array of the currently available IAPs.
     * @return ETrue if service is available.
     */
    TBool IsAvailableL( const TUint aServiceId,
                        const TUid aUid,
                        const TCCHSubserviceType aType,
                        RArray<TUint32>& aIapIds ) const;
                                                 
    /**
     * Adds a observer for Plug-in events.
     * @since S60 3.2
     * @param aObserver Pointer to observer.
     */
    void SetServiceNotifier( MCCHServiceNotifier* aObserver );

    /**
     * Removes a observer for Plug-in events.
     * @since S60 3.2
     * @param aObserver Pointer to observer.
     */
    void RemoveServiceNotifier( MCCHServiceNotifier* aObserver );
        
private:

    /**
     * Find Plug-in's index number
     * @since S60 3.2
     * @param aUid Plug-in's Uid
     * @param TInt Plug-in's index number or KErrNotFound
     */
    TInt Find( const TUid& aUid ) const;
    
    /**
     * Cleanup RImplInfoPtrArray
     * @since S60 3.2
     * @param aArray Which to be destroyed
     */
    static void ResetAndDestroy( TAny* aArray );

private:

    /**
     * C++ default constructor.
     */
    CCCHPluginHandler( CCCHServerBase& aServer );
    
private: // data

    /**
     * Server handle
     */
    CCCHServerBase&                 iServer;
    
    /**
     * Array of loaded Plug-ins
     */
    RPointerArray<CCCHPlugin>       iPlugins;

    };

#endif // C_CCHPLUGINHANDLER_H

// End of file
