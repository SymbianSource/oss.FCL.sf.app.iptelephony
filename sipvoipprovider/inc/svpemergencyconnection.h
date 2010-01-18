/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides connection for emergency call when necessary
*
*/


#ifndef C_SVPEMERGENCYCONNECTION_H
#define C_SVPEMERGENCYCONNECTION_H


#include <es_sock.h>                // For RSocketServ, RConnection
#include <commdbconnpref.h>         // For TCommDbConnPref
#include <comms-infras/es_config.h> // For TSipServerAddrBuf

#include "svpemergencyconnectionobserver.h"
#include "svputdefs.h"

/**
 *  Connection for emergency call.
 *  Asynchronous services: IAP and SNAP connect, retrieval of used IAP ID of 
 *  SNAP connection, and retrieval of SIP proxy address.
 *
 *  @lib svp.dll
 *  @since S60 v3.2
 */
class CSVPEmergencyConnection : public CActive
    {

public:

    /** Request type */
    enum TSVPRequestType
        {
        ESVPNone, 
        ESVPSnapConnect, 
        ESVPConnect, 
        ESVPSipProxyAddress
        };

    /**
     * Two-phased constructor.
     * @param aPriority Active object priority
     * @param aObserver Observer
     */
    static CSVPEmergencyConnection* NewL( 
        TPriority aPriority, MSVPEmergencyConnectionObserver& aObserver );
    
    static CSVPEmergencyConnection* NewLC( 
        TPriority aPriority, MSVPEmergencyConnectionObserver& aObserver );
    
    /**
     * Destructor.
     */
    virtual ~CSVPEmergencyConnection();

    /**
     * Connects with SNAP ID
     *
     * @since S60 v3.2
     * @param aSnapId SNAP ID
     */
    void ConnectWithSnapIdL( TUint32 aSnapId );

    /**
     * Returns the IAP ID used by the SNAP connection
     *
     * @since S60 v3.2
     * @param aIapId IAP ID 
     * @return Error code
     */
    TInt IapId( TUint32& aIapId );

    /**
     * Connects with IAP ID
     *
     * @since S60 v3.2
     * @param aIapId IAP ID 
     */
    void ConnectL( TUint32 aIapId );

    /**
     * Requests for SIP proxy address
     *
     * @since S60 v3.2
     * @param aIapId IAP ID
     */
    void RequestSipProxyAddressL( TUint32 aIapId );

private:

    CSVPEmergencyConnection( 
        TPriority aPriority, MSVPEmergencyConnectionObserver& aObserver );

    void ConstructL();
    
// from base class CActive

    void DoCancel();
    
    void RunL();
    
private: // data

    /**
     * Observer
     */
    MSVPEmergencyConnectionObserver& iObserver;

    /**
     * Request type
     */
    TSVPRequestType iRequestType;

    /**
     * Snap connection preference
     */    
    TCommSnapPref iSnapConnPref;

    /**
     * Connection preference overrider
     */
    TCommDbConnPref iConnPref;
    
    /**
     * SIP server address buffer
     */
    TSipServerAddrBuf iSipServerAddrBuf;

    /**
     * Connection
     */
    RConnection iConnection;

    /**
     * Socket server
     */
    RSocketServ iSocketServer;

private:

    // For testing
    SVP_UT_DEFS

    };

#endif // C_SVPEMERGENCYCONNECTION_H
