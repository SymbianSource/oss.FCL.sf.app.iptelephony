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
* Description:  account creation plugin params header file
*
*/


#ifndef ACPBROWSERPARAMS_H
#define ACPBROWSERPARAMS_H

#include <e32base.h>
#include <etelmm.h>

#ifdef __PLUG_AND_PLAY_MOBILE_SERVICES    
#include <PnpUtilImpl.h>
#else
/*
* temporary defines, remove when PNP is stuff available
*/

const TInt KMaxVersionStringLength(32);
const TInt KNonceLength(8);
const TInt KMaxKeyInfoLength(20);
_LIT( KPnpUtilDllFileName, "Pnp.dll" );
const TUid KPnpUtilDllUid = { 0x1020433A };

class CPnpUtilImpl;
#endif

#include <wlaninternalpskeys.h>
#include <sysversioninfo.h>

#include "accountcreationengineconstants.h"


/*
* CACPBrowserParams
* Encapsulates parameters needed for the Activation Server link
*/
class CACPBrowserParams : public CBase
    {
public:

    static CACPBrowserParams* NewL();
    static CACPBrowserParams* NewLC();

    /**
     * Destructor.
     */
    virtual ~CACPBrowserParams();

    /**
     * Gets parameters, i.e. appends parameters to given URL.
     * 
     * @since S60 v3.2
     * @param aUrl URL in which parameters are appended.
     */
    void GetParameters( TDes& aUrl );

private:

    CACPBrowserParams();
    void ConstructL( );

    /**
     * Retrieves parameters for the Service Activation link.
     * 
     * @since S60 v3.2 
     */
    void RetrieveParamsL();

    /**
     * Gets phone model and stores it to iPhoneModel. 
     * 
     * @since S60 v3.2
     */
    void RetrievePhoneModel();

    /**
     * Retrieves info on home network (incl. MCC, MNC).
     * 
     * @since S60 v3.2
     */
    void RetrieveHomeNetworkInfoL();

    /**
     * Retrieves Key info and creates nonce for signing
     * (PnpProvisioning client).
     * 
     * @since S60 v3.2
     */
    void RetrieveSigningParamsL();

    /**
     * Retrieves WLAN MAC address.
     * 
     * @since S60 v3.2 
     */
    void RetrieveWlanMacAddress();
    
    /**
     * Retrieves phone serial number (IMEI) and variant code.
     *
     * @since S60 v3.2 
     */
    void RetrievePhoneInfoL();

    /**
     * Checks if the phone has network or is searching for one.
     * 
     * @since S60 v3.2
     * @return ETrue is network available, EFalse if no network available.
     */
    TBool RegisteredInNetwork();

private:

    /**
     * Telephony Server (etelmm).
     */
    RTelServer iTelServer;

    /**
     * Phone Object
     */
    RMobilePhone iPhone;

    /**
     * Phone model.
     */
    TBuf<RMobilePhone::KPhoneModelIdSize> iPhoneModel;

    /**
     * Home MCC.
     */
    TBuf<KMaxMCC> iHomeMcc;

    /**
     * Home MNC.
     */
    TBuf<KMaxMNC> iHomeMnc;

    /**
     * Key information.
     */
    TBuf8<KMaxKeyInfoLength> iKeyInfo;

    /**
     * Nonce.
     */
    TBuf8<KNonceLength> iNonce;

    /**
     * WLAN MAC address.
     */
    TBuf<KPSWlanMacAddressLength*3> iWlanMACAddress;
    
    /**
     * IMEI.
     */
    TBuf<RMobilePhone::KPhoneSerialNumberSize> iImei;
    
    /**
     * Variant code.
     */
    TBuf<KSysVersionInfoTextLength> iVariant;
    
    /** 
     * PnP utility class instance used in retrieving parameters.
     */
    CPnpUtilImpl* iPnpUtil;

    };

#endif // ACPBROWSERPARAMS_H

// End of file.
