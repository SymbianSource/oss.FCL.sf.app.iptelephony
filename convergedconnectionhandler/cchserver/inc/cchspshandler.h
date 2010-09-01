/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CCCHSPSHandler declaration
*
*/


#ifndef C_CCHSPSHANDLER_H
#define C_CCHSPSHANDLER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <cchclientserver.h>
#include <spproperty.h>

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
class CSPSettings;

// CLASS DECLARATION

/**
 *  CCCHSPSHandler declaration.
 *  Service provider settings handler is used to access service and
 *  sub-service configuration data.
 *  @lib cchserver.exe
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CCCHSPSHandler ) : public CBase
    {

public: // Constructors and destructor

    /**
    * Two-phased constructor.
    */
    static CCCHSPSHandler* NewL( CCCHServerBase& aServer );

    /**
     * Two-phased constructor.
     */
    static CCCHSPSHandler* NewLC( CCCHServerBase& aServer );

    /**
     * Destructor.
     */
    virtual ~CCCHSPSHandler();

public: // New functions

    /**
     * Get service count
     * @since S60 3.2
     * @return TInt Count of services
     */
    TInt GetServicesCountL() const;
    
    /**
     * Get service ids
     * @since S60 3.2
     * @param aServiceIds On completion contains service ids
     */
    void GetServiceIdsL( RArray<TUint>& aServiceIds ) const;
    
    /**
     * Get service's information
     * @since S60 3.2
     * @param aServiceId Used Service
     * @param aService On completion, contains service information
     */
    void GetServiceInfoL( const TUint32 aServiceId, TCCHService& aService ) const;
    
    /**
     * Get connectivity Plug-in's Uid
     * @since S60 3.2
     * @param aSerivceId Used Service
     * @param aType Subservice's type
     * @param aUid On completion, contains Plug-ins Uid
     */
    void GetConnectivityPluginUidL( TUint32 aServiceId,
        TCCHSubserviceType aType, 
        TUid& aUid ) const;
        
    /**
     * Get Service's load at startup information
     * @since S60 3.2
     * @param aServiceId Used Service
     * @param aType Subservice's type
     * @param aOnOff On completion, contains Service's startup info
     */                
    void LoadAtStartUpL( TUint32 aServiceId,
        TCCHSubserviceType aType,
        TBool& aOnOff ) const;
    
    /**
     * Set Service's load at startup information to service table
     * @since S60 3.2
     * @param aServiceId Used Service
     * @param aType Subservice's type
     * @param aOnOff 
     */    
    void SetLoadAtStartUpL( const TUint32 aServiceId,
        const TCCHSubserviceType aType,
        const TBool aOnOff ) const;
        
private:

    /**
     * Changes Subservice's type to property type and name
     * @since S60 3.2
     * @param aType Subservice's type
     * @param aPropertyType On completion, contains property's type
     * @param aPropertyName On completion, contains property's name
     */                
    void ChangeToPropertyStyleL( TCCHSubserviceType aType,
        TSPItemType& aPropertyType,
        TServicePropertyName& aPropertyName ) const;
       
    /**
     * Checks is given subservice valid.
     * @since S60 3.2
     * aServiceId Used Service
     * @param aPropertyName Property's name
     * @return ETrue if subservice is valid, otherwise EFalse.
     */  
    TBool ValidSubserviceL( const TUint32 aServiceId, 
        TServicePropertyName aPropertyName ) const;
        
private:

    /**
     * C++ default constructor.
     */
    CCCHSPSHandler( CCCHServerBase& aServer );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private: // data

    /**
     * Handle to server
     */
    CCCHServerBase&                 iServer;

    /**
     * CSPSettings. Own
     */
    CSPSettings*                    iSettings;
    
    };

#endif // C_CCHSPSHANDLER_H

// End of file
