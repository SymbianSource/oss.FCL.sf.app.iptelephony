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
* Description:  Observes emergency connection
*
*/


#ifndef M_SVPEMERGENCYCONNECTIONOBSERVER_H
#define M_SVPEMERGENCYCONNECTIONOBSERVER_H


/**
 *  Observes emergency connection.
 *  Observes: SNAP connected, IAP connected, SIP proxy address ready, and 
 *  error occurred.
 *
 *  @lib svp.dll
 *  @since S60 v3.2
 */
class MSVPEmergencyConnectionObserver
    {

public:

    /**
     * Notifies observer that SNAP connection is established
     *
     * @since S60 3.2
     */
    virtual void SnapConnected() = 0;

    /**
     * Notifies observer that IAP connection is established
     *
     * @since S60 3.2
     */
    virtual void Connected() = 0;

    /**
     * Notifies observer that SIP proxy address is ready
     *
     * @since S60 3.2
     * @param aAddress SIP proxy address
     */
    virtual void SipProxyAddressReady( const TDesC16& aAddress ) = 0;

    /**
     * Notifies observer that error has occurred
     *
     * @since S60 3.2
     * @param aError Symbian error code
     */
    virtual void ConnectionError( TInt aError ) = 0;

    };

#endif // M_SVPEMERGENCYCONNECTIONOBSERVER_H
