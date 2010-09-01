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
* Description:  CCCHServiceInfo declaration
*
*/


#ifndef C_CCHSERVICEINFO_H
#define C_CCHSERVICEINFO_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <cchclientserver.h>

// CONSTANTS
// None

// MACROS
// None

// FORWARD DECLARATIONS
class CCCHSubserviceInfo;
class CCCHServerBase;
class MCCHServiceNotifier;

// DATA TYPES
typedef RPointerArray<CCCHSubserviceInfo> RSubserviceArray;

// FUNCTION PROTOTYPES
// None

// CLASS DECLARATION

/**
 *  CCCHServiceInfo declaration
 *  Contains Service information
 *  @lib cchserver.exe
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CCCHServiceInfo ) : public CBase
    {

public: // Constructors and destructor

    /**
    * Two-phased constructor.
    */
    static CCCHServiceInfo* NewL( CCCHServerBase& aServer );

    /**
     * Two-phased constructor.
     */
    static CCCHServiceInfo* NewLC( CCCHServerBase& aServer );

    /**
     * Destructor.
     */
    virtual ~CCCHServiceInfo();

public: // New functions
    
    /**
     * ServiceId getter
     * @since S60 3.2
     * @return TUint Service's Id
     */
    TUint32 GetServiceId() const;
    
    /**
     * ServiceId setter
     * @since S60 3.2
     * @param aServiceId Service's Id
     */
    void SetServiceId( TUint32 aServiceId );
    
    /**
     * Name getter
     * @since S60 3.2
     * @return const TDesC Service's name
     */
    const TDesC GetName() const;
    
    /**
     * Name setter
     * @since S60 3.2
     * @param aName Service's name
     */
    void SetName( const TDesC& aName );
    
    /**
     * Enable given service
     * @since S60 3.2
     * @param aServiceType Service's Subservice type
     * @param aConnectivityCheck Is value is True Connectivity Plug-in will
     * make test call to check is the connection truly working.
     * @param aObserver Pointer to observer.
     */
    void EnableL( const TCCHSubserviceType aServiceType, 
                  const TBool aConnectivityCheck,
                  MCCHServiceNotifier* aNotifier );
    
    /**
     * Disable given service
     * @since S60 3.2
     * @param aServiceType Service's Subservice type
     */
    void DisableL( const TCCHSubserviceType aServiceType );
    
    /**
     * Connection information setter
     * @since S60 3.2
     * @param aServiceConnInfo Service's new connection information
     */
    void SetConnectionInfoL( const TServiceConnectionInfo aServiceConnInfo );
    
    /**
     * Connection information getter
     * @since S60 3.2
     * @param aServiceConnInfo On completion contains Service's 
     * connection(SNAP/IAP) information
     */
    void GetConnectionInfoL( TServiceConnectionInfo& aServiceConnInfo ) const;
    
    /**
     * Is service enabled
     * @since S60 3.2
     * @return ETrue if service is enabled.
     */
    TBool IsEnabled() const;
    
    /**
     * Add new Subservice to Service
     * @since S60 3.2
     * @param aSubservice Subservice information
     */
    void AddSubserviceL( TCCHSubservice& aSubservice );
    
    /**
     * Subservice getter
     * @since S60 3.2
     * @param aType Subservice's type
     */
    CCCHSubserviceInfo& GetSubserviceL( TCCHSubserviceType aType ) const;
    
    /**
     * Update service and subservice's states
     * @since S60 3.2
     * @param aReadServiceTable If ETrue class reads/updates all Services 
     * directly from Service Provider Settings 
     */ 
    void UpdateL( TBool aReadServiceTable = EFalse );
    
    /**
     * Update service and subservice's states
     * @since S60 3.2
     * @param aType Subservice's type
     * @param aState Service's new state
     */ 
    void SetStateL( const TCCHSubserviceType aType, 
                    const TInt aState );

    /**
     * Update service and subservice's states
     * @since S60 3.2
     * @param aPluginUid plugin uid wich services are updated
     * @param aState Service's new state
     */ 
    void SetStateL( const TUid& aPluginUid, 
                    const TCCHSubserviceState& aState );
    /**
     * Update service and subservice's error
     * @since S60 3.2
     * @param aType Subservice's type
     * @param aError Service's error
     */ 
    void SetErrorL( const TCCHSubserviceType aType, 
                    const TInt aError );
    /**
     * Update service and subservice's states
     * @since S60 3.2
     * @return TInt Count of subservices
     */ 
    TInt SubserviceCount() const;
    
    /**
     * Get Plug-in's Uid
     * @since S60 3.2
     * @param aSubserviceIndex Index of subservice
     * @return TUid Subservice's Uid
     */ 
    TUid GetUidL( TInt aSubserviceIndex ) const;
    
    /**
     * Fill Service's information
     * @since S60 3.2
     * @param aService On completion contains service and subservice information
     */
    void FillServiceInfo( TCCHService& aService );
    
    /**
     * Get Service's status
     * @since S60 3.2
     * @param aType Subservice's type
     * @param aState in return, state of subservice
     * @return error code, KErrNotFound if subservice is not existing
     */ 
    TInt GetStatus( TCCHSubserviceType aType, TCCHSubserviceState& aState ) const;

    
    /**
     * Get Service's error
     * @since S60 3.2
     * @param aType Subservice's type
     * @return TInt Current error of subservice
     */ 
    TInt GetError( TCCHSubserviceType aType ) const;
    
    /**
     * Reserve a service
     * @since S60 3.2
     * @param aType Subservice's type
     */ 
    void ReserveServiceL( TCCHSubserviceType aType );    
    
    /**
     * Free the service reservation
     * @since S60 3.2
     * @param aType Subservice's type
     */ 
    void FreeServiceL( TCCHSubserviceType aType ); 
    
    /**
     * Return the service reference count
     * @since S60 3.2
     * @param aType Subservice's type
     * @return Reference count
     */ 
    TBool IsReservedL( TCCHSubserviceType aType ) const;
    
    /**
     * Get Service's protocol information
     * @since S60 3.2
     * @param aBuffer On completion contains selected service's protocol
     * information.
     * @param aType Subservice's type
     */ 
    void GetServiceInfoL( TDes& aBuffer, TCCHSubserviceType aType ) const;
    
    /**
     * Subservice exist
     * @since S60 3.2
     * @param aType Subservice's type
     * @return ETrue if Subservice exits or type is not specified.
     */ 
    TBool SubserviceExist( TCCHSubserviceType aType ) const;
    
    /**
     * Find subservice by type
     * @since S60 3.2
     * @param aType Subservice's type
     * @return TInt position within iSubservices array or KErrNotFound
     */
    TInt FindSubservice( TCCHSubserviceType aType ) const;
    
    /**
     * Is any Subservice's startup-flag set to true
     * @since S60 3.2
     * @return ETrue if any startup-flag is set to true
     */
    TBool StartupFlagSet() const;
    
    /**
     * Is Subservice's startup-flag set to true
     * @since S60 3.2
     * @param aSubserviceIndex Subservice's index
     * @return ETrue if startup-flag is set to true
     */
    TBool StartupFlagSet( TInt aSubserviceIndex ) const;
    
    /**
     * Subservice's enable at startup info setter
     * @since S60 3.2
     * @param aType Subservice's type
     * @param aLoadAtStartUp Enable Subservice at the startup
     */
    TInt SetStartupFlag( TCCHSubserviceType aType, TBool aLoadAtStartUp ) const;
    
    /**
     * Gets Subservice's type
     * @since S60 3.2
     * @param aSubserviceIndex Subservice's index
     * @return TCCHSubserviceType type of Subservice
     */
    TCCHSubserviceType GetSubserviceType( TInt aSubserviceIndex ) const;
    
    /**
     * Gets services state
     * @since S60 3.2
     * @return TCCHSubserviceStates Status of service
     */
    TCCHSubserviceState GetState() const;
    
private:
    
    /**
     * Exception errors cases
     * @since S60 3.2
     */
    void HandleErrorExceptions();
    
private:

    /**
     * C++ default constructor.
     */
    CCCHServiceInfo( CCCHServerBase& aServer );
    
private: // data
    
    /**
     * Handle to server
     */
    CCCHServerBase&                 iServer;
    
    /**
     * Service Id
     */
    TUint32                         iServiceId;

    /**
     * Service provider name
     */
    TBuf<KCCHMaxServiceNameLength>  iServiceName;
    
    /**
     * Subservice objects array
     */
    RSubserviceArray                iSubservices;

    /**
     * Is Servie enabled
     */
    TBool                           iIsEnabled;
    
    /**
    * Reserved, obsolote. When adding next new parameter (TInt)
    * rename Reserved to that, implementation is ready then.
    */
    TBool                           iReserved;
    };

#endif // C_CCHSERVICEINFO_H

// End of file
