/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef C_CSCPVOIPHANDLER_H
#define C_CSCPVOIPHANDLER_H

#include <e32base.h>

#include "scpservicehandlerbase.h"
#include "scppropertyobserver.h"

class CScpProfileHandler;
class CRepository;
class CScpPropertyNotifier;
class CSipClientResolverUtils;

/**
*  Voip sub service handler.
*
*  @lib sipconnectionprovider.dll
*  @since Series 60 3.2
*/
class CScpVoipHandler : public CScpServiceHandlerBase,
                        public MScpPropertyObserver
    {
public:  // Constructors and destructor
 
    static CScpVoipHandler* NewL( CScpSubService& aSubService );
    
    /**
     * Destructor.
     */
    virtual ~CScpVoipHandler();

public: // From base class
    /**
     * Reports about changed property
     * @param aPropertyValue Value of the property 
     */
    void HandlePropertyChanged( TInt aPropertyValue ); 

private: // From base class
    
    /**
     * Enables service.
     */
    void EnableSubServiceL();

    /**
     * Disables service.
     */
    TInt DisableSubService();
            
    /**
     * Returns type of sub service (VoIP, Vmbx, Presence).
     * @return Type of the sub service.
     */
    TCCHSubserviceType SubServiceType() const;

    /**
     * Handlers sip connection event
     * @param aProfileId Sip profile id
     * @param aEvent Connection event
     */
    void HandleSipConnectionEvent( TUint32 aProfileId,
                                   TScpConnectionEvent aEvent );

private:

    /**
     * C++ default constructor.
     */
    CScpVoipHandler( CScpSubService& aSubService );
    
    /**
     * 2nd phase constructor
     */
    void ConstructL();
    
    /**
     * Removes the the routing used for Sip profiles contact header 
     * from Sip Client routing table
     */
    void DeregisterContactHeaderL();

    /**
     * Finds Sip profile contact header user name for the given sub service
     * @param aUserName The found user name is inserted to here
     */
    void GetSipProfileContactHeaderUserL( RBuf8& aContactHeader ) const;

    /**
     * Handlers sip connection event
     * @param aProfileId Sip profile id
     * @param aEvent Connection event
     */
    void HandleSipConnectionEventL( TUint32 aProfileId,
                                   TScpConnectionEvent aEvent );
    
    // Get CallProvider uid from SPSettings
    TUid GetCallProviderImplementationUidL();
                                   
private:

    // Monitors network mode P&S variable, owned.
    CScpPropertyNotifier* iNetworkMode;
    
    // Updates VoIP availibility status to SIP client resolver cenrep-table, owned.
    CSipClientResolverUtils* iSipClientResolver;

    
#ifdef _DEBUG
    CScpPropertyNotifier* iNetworkModeDebug;
    friend class T_CScpServiceManager;
    friend class T_CScpVoipHandler;
#endif
    };

#endif      // C_CSCPVOIPHANDLER_H   
            
// End of File
