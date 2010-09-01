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
* Description:  Address resolver utility class
*
*/


#ifndef C_IPAPPUTILSADDRESSRESOLVER_H
#define C_IPAPPUTILSADDRESSRESOLVER_H

#include <e32base.h>
#include <es_sock.h>
#include <in_sock.h>

/**
 *  Utility class for querying local ip address and wlan mac address
 *
 *  @code
 *      TInetAddr localAddr;
 *      TUint32 iapId = 1;
 *      iAddressResolver = CIPAppUtilsAddressResolver::NewL();
 *      iAddressResolver->GetLocalAddressFromIapL( 
 *              localAddr, iapId );
 *      delete iAddressResolver;
 *  @endcode
 *
 *  @lib ipapputils.lib
 *  @since S60 v3.2 
 */
NONSHARABLE_CLASS( CIPAppUtilsAddressResolver ) : public CBase
    {
public: // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CIPAppUtilsAddressResolver* NewL();
    /**
     * Two-phased constructor.
     */
    IMPORT_C static CIPAppUtilsAddressResolver* NewLC();

    /**
    * Destructor.
    */
    virtual ~CIPAppUtilsAddressResolver();


public: // New functions

    /**
     * Get Wlan MAC address from the device. If WLAN interface couldn't found,
     * return error code.
     *
     * @param aWlanMacAddress on return, this descriptor will contain the 
     *     WLAN mac address. Client must see that the descriptor is long enough
     *   to hold the address in given format.
     * @param aByteFormat defines the format for WLAN MAC bytes, e.g. "%02X:"
     *   if aByteFormat == KNullDesC(), function will return raw hwaddress data 
     *   (six bytes long descriptor, no formatting whatsoever)
     * @return error code. KErrNone if successful, KErrNotFound if WLAN IF is 
     *   not found from the system or other system error code
     * @since S60 5.0
     */
    IMPORT_C TInt GetWlanMACAddress (
        TDes8& aWlanMacAddress, const TDesC8& aByteFormat );

    /**
     * Retrieve local ip address
     *
     * @param aLocalIpAddr on return, this parameter will contain the local ip address.
     * @param aIapId specifies the IAP from which the ip address is queried from
     * @return error code. KErrNone if successful, KErrNotFound if cannot fetch the
     *   addr from given iap, or other system error code
     * @since S60 5.0
     */
    IMPORT_C TInt GetLocalIpAddressFromIap( 
        TInetAddr& aLocalIpAddr, TUint32 aIapId );

    /**
     * Returns WLAN SSID based on given IAP id.
     *
     * @since S60 v5.0
     * @param aIapId for IAP id 
     * @param aWlanSsid for WLAN SSID
     */
    IMPORT_C void WlanSsidL( TUint32 aIapId, TDes& aWlanSsid );


private:

    /**
     * C++ default constructor.
     */
    CIPAppUtilsAddressResolver();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * Helper function used from GetLocalIPAddressFromIap.
     * Checks the validity of the fetched local ip addr and sets it to target
     * @param aTarget will contain the local ip addr on return
     * @param aCandidate proposed ip address
     * @param aCandidateIap IAP of the proposed IP address
     * @param aSpecifiedIap reference IAP ID. Must match with aCandidateIap.
     * @return Status Code. KErrNotFound if cannot set the target.
     */
    TInt CheckAndSetAddr( TInetAddr& aTarget, TInetAddr& aCandidate,
        TUint32 aCandidateIap, TUint32 aSpecifiedIap ) const;


private: // data
    /**
     * Handle to Socket's server.
     * Own.
     */
    RSocketServ iSocketServer; 

    /**
     * Handle to Socket.
     * Own.
     */
    RSocket iSocket; 


#ifdef _DEBUG        
        friend class T_CIPAppUtilsAddressResolver;
#endif
    };

#endif // C_IPAPPUTILSADDRESSRESOLVER_H
