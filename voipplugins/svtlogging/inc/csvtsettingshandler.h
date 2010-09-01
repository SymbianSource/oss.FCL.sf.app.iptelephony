/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/


#ifndef C_CSVTSETTINGSHANDLER_H
#define C_CSVTSETTINGSHANDLER_H

#include <e32base.h>
#include <sipprofileregistryobserver.h>

class CSPSettings;
class CRCSEProfileRegistry;
class CSIPManagedProfileRegistry;
class CRCSEProfileEntry;

/**
 *  Encapsulates handling of system settings having an effect on plugin.
 *
 *  @lib svtlogging.dll
 *  @since S60 v5.1
 */
class CSvtSettingsHandler : public CBase, public MSIPProfileRegistryObserver
    {

public:

    /**
     * Two-phased constructor.
     * @param aServiceId    The service identifier.
     * @leave KErrNotFound if service provider settings for given identifier
     * do not exist.
     */
    static CSvtSettingsHandler* NewL( TUint aServiceId );

    /**
     * Two-phased constructor.
     * @param aServiceId    The service identifier.
     * @leave KErrNotFound if service provider settings for given identifier
     * do not exist.
     */
    static CSvtSettingsHandler* NewLC( TUint aServiceId );
    
    /**
    * Destructor.
    */
    virtual ~CSvtSettingsHandler();

    /**
     * Returns SIP address domain part clipping setting.
     *
     * @since   S60 v5.1
     * @return  Clipping setting.
     */
    TInt DomainPartClippingSetting() const;
    
    /**
     * Returns local user's AOR.
     *
     * @since   S60 v5.1
     * @param   aUserAor      User AOR.
     */
    void GetUserAorL( RBuf& aUserAor );

// from base class MSIPProfileRegistryObserver.

    /**
     * From MSIPProfileRegistryObserver.
     * @see MSIPProfileRegistryObserver.
     */
    void ProfileRegistryEventOccurred( TUint32 aProfileId, TEvent aEvent );

    /**
     * From MSIPProfileRegistryObserver.
     * @see MSIPProfileRegistryObserver.
     */
    void ProfileRegistryErrorOccurred( TUint32 aProfileId, TInt aError );

private:

    CSvtSettingsHandler() {};
    
    CSvtSettingsHandler( TUint aServiceId );

    void ConstructL();
    
    TBool IsServiceConfiguredL( TUint aServiceId ) const;
    
    CSPSettings* CreateServiceProviderSettingsL() const;
    
    CRCSEProfileRegistry* CreateRcseProfileRegistryLC() const;
    
    CSIPManagedProfileRegistry* CreateSipProfileRegistryLC();
    
    CRCSEProfileEntry* RcseProfileForServiceL( TUint aServiceId ) const;
    
    TInt SipProfileIdForServiceL( TUint aServiceId ) const;
    
private: // data

    /**
     * Service identifier.
     */
    TUint iServiceId;

    };

#endif // C_CSVTSETTINGSHANDLER_H
