/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef C_SCPSETTINGHANDLER_H
#define C_SCPSETTINGHANDLER_H

#include <e32base.h>
#include <cchclientserver.h>
#include <spdefinitions.h>

class CRCSEProfileRegistry;
class CRCSEProfileEntry;
class CSPSettings;
class CSPEntry;
class CSPSettingsVoIPUtils;
class CScpService;
class CScpServiceHandlerBase;
class CScpProfileHandler;
class CScpServiceStorage;

/**
*  Settings handler.
*  @lib sipconnectionprovider.dll
*/
class CScpSettingHandler : public CBase
    {
public:
    
    /**
     * Two-phased constructor.
     */
    static CScpSettingHandler* NewL( CScpProfileHandler& iProfileHandler );
    
    /**
     * Destructor.
     */
    virtual ~CScpSettingHandler();

    /**
     * Get SIP Profile Id by Service Id.
     * @param aServiceId Service Id.
     * @param aSubService Sub service type.
     * @param aSipId SIP Profile Id.
     */
    void GetSipProfileIdByTypeL( TUint32 aServiceId,
                                 TCCHSubserviceType aSubServiceType, 
                                 TUint32& aSipId ) const;
                                 
    /**
     * Get SPSetting TInt property by Service Id and property name.
     * @param aServiceId Service Id.
     * @param aPropertyName Settings property name.
     * @param aProperty a property value.
     */
    void GetSPSettingsIntPropertyL( TUint32 aServiceId,
                                    TServicePropertyName aPropertyName, 
                                    TInt& aProperty ) const;
     /**
     * Get settings for service by Service Id.
     * @param aServiceId Service Id.
     * @param aSetting Service Settings.
     */
    void UpdateSettingsL( CScpService& aSetting,
                          TCCHSubserviceType aSubServiceType ) const;
       
     /**
     * Updates xdm settings username
     * @param aService Service handle.
     * @param aSubServiceType sub service type
     * @param aUsername username to set
     */                          
    void UpdateXdmUsernameL( CScpService& aService, 
        TCCHSubserviceType aSubServiceType,
        const TDesC8& aUsername );

     /**
     * Updates xdm settings password
     * @param aService Service handle.
     * @param aSubServiceType sub service type
     * @param aPassword password to set
     */                          
    void UpdateXdmPasswordL( CScpService& aService, 
        TCCHSubserviceType aSubServiceType,
        const TDesC8& aPassword );

    /**
     * Get Service Id by SIP Profile Id.
     * @param aSipId SIP Profile Id.
     * @param aServiceId Service id array.
     */
    TInt GetServiceIds( TUint32 aSipId, 
                        RArray<TUint32>& aServiceId ) const;

    /**
     * Checks if service exists in service table
     * @param aServiceId Service id array.
     * @return ETrue if the service exists
     */
    TBool ServiceExistsL( TUint32 aServiceId ) const;
    
    /**
     * Checks is VoIP supported.
     * @return ETrue if Common VoIP and Dynamic VoIP are ON
     */
    TBool IsVoIPSupported() const;
    
    /**
     * Checks is VoIP's user agent header terminal type defined.
     * @param aServiceId service id
     * @return ETrue if terminal type is defined.
     */
    TBool IsUahTerminalTypeDefinedL( TUint32 aServiceId ) const;
    
    /**
     * Checks is VoIP's user agent header WLAN mac address defined.
     * @param v
     * @return ETrue if WLAN mac address is defined.
     */
    TBool IsUahWLANMacDefinedL( TUint32 aServiceId ) const;
    
    /**
     * VoIP's user agent header free string length getter.
     * @param aServiceId service id
     * @return TInt length of free string.
     */
    TInt UahStringLengthL( TUint32 aServiceId ) const;
         
private: 

    /**
     * C++ default constructor.
     */
    CScpSettingHandler( CScpProfileHandler& iProfileHandler );

    /**
     * 2nd phase destructor
     */
    void ConstructL();

    /**
     * Get Service Id by SIP Profile Id.
     * @param aSipId SIP Profile Id.
     * @param aServiceId Service id array.
     */
    void GetServiceIdsL( TUint32 aSipId, 
                         RArray<TUint32>& aServiceId ) const;

     /**
     * Updates Presence settings to the given service
     * @param aService Service
     */
    void UpdatePresenceSettingsL( CScpService& aService ) const;

    /**
     * Check if presence settings are valid
     * @param aEntry  Entry
     * @return ETrue if valid
     */
    TBool ArePresenceSettingsValid( const CSPEntry& aEntry ) const;
       
    /**
     * Determines presence settings id from the given entry
     * @param aEntry Entry
     * @return ETrue if valid
     */ 
    TInt GetPresenceSettingsId( const CSPEntry& aEntry ) const;

    /**
     * Updates VoIP settings to the given service
     * @param aService Service
     */
    void UpdateVoIPSettingsL( CScpService& aService ) const;

    /**
     * Check if VoIP settings are valid in the given entry
     * @param aEntry Entry
     * @return ETrue if valid
     */
    TBool AreVoipSettingsValid( const CSPEntry& aEntry ) const;

    /**
     * Updates voice mail box settings to the given service
     * @param aService Service
     */
    void UpdateVmbxSettingsL( CScpService& aService ) const;

    /**
     * Check if voice mail box settings are valid
     * @param aEntry  Entry
     * @return ETrue if valid
     */
    TBool AreVmbxSettingsValid( const CSPEntry& aEntry ) const;

    /**
     * Gets sip profile id for VoIP sub service
     * @param aServiceId Service id
     * @param aSipId
     */
    void GetVoipSipProfileIdL( TUint32 aServiceId, TUint32& aSipId ) const;

    /**
     * Gets sip profile id for VMBx sub service
     * @param aServiceId Service id
     * @param aSipId
     */
    void GetVmbxSipProfileIdL( TUint32 aServiceId, 
                               TUint32& aSipId ) const;

    /**
     * Gets sip profile id for Presence sub service
     * @param aServiceId Service id
     * @param aSipId
     */
    void GetPresenceSipProfileIdL( TUint32 aServiceId, 
                                   TUint32& aSipId ) const;

    /**
     * Gets sip profile id for all sub services within the service.
     * If subservices contain different sip profile ids, will leave with
     * KErrArgument.
     * @param aServiceId Service id
     * @param aSipId
     */
    void GetUnknownSipProfileIdL( TUint32 aServiceId, 
                                  TUint32& aSipId ) const;

    /**
     * Update IM settings to the given service.
     *
     * @since S60 v5.1
     * @param aService service
     */
    void UpdateImSettingsL( CScpService& aService ) const;

    /**
     * Check if IM settings are valid in the given entry.
     *
     * @since S60 v5.1
     * @param aEntry Entry
     * @return ETrue if valid
     */
    TBool AreImSettingsValid( const CSPEntry& aEntry ) const;

private:

    /**
     * Profile registry. Own.
     */
    CRCSEProfileRegistry* iRcseProfileRegistry;

    /**
     * Profile entry. Own.
     */
    CRCSEProfileEntry* iRcseProfileEntry;

    /**
     * SPSettings. Own.
     */
    CSPSettings* iSpSettings;

    /**
     * Profile handler
     */
    CScpProfileHandler& iProfileHandler;
    
    /**
     * SPSettings VoIP utils. Own.
     */
    CSPSettingsVoIPUtils* iSpsVoIPUtils;
    
private:

#ifdef _DEBUG
    friend class T_CScpSettingHandler;
    friend class T_CScpServiceManager;
    friend class T_CScpSipHelper;
#endif

    };

#endif      // C_SCPSETTINGHANDLER_H   
            
// End of File
