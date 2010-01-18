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
* Description:  Address resolver utility class
*
*/


#include <wlaninternalpskeys.h>
#include <cmmanagerext.h>
#include <cmpluginwlandef.h>

#include "cipapputilsaddressresolver.h"

const TInt KWlanMacAddressPos = 2;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CIPAppUtilsAddressResolver::CIPAppUtilsAddressResolver()
    {
    }

// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CIPAppUtilsAddressResolver::ConstructL()
    {
    // Connect to Socket server.
    User::LeaveIfError( iSocketServer.Connect() );
    
    // Connect to Socket.
    User::LeaveIfError( iSocket.Open( iSocketServer,
                                      KAfInet,
                                      KSockStream,
                                      KProtocolInetTcp ) );

    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CIPAppUtilsAddressResolver* CIPAppUtilsAddressResolver::NewL()
    {
    CIPAppUtilsAddressResolver* self = CIPAppUtilsAddressResolver::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CIPAppUtilsAddressResolver* CIPAppUtilsAddressResolver::NewLC()
    {
    CIPAppUtilsAddressResolver* self = new( ELeave ) CIPAppUtilsAddressResolver;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CIPAppUtilsAddressResolver::~CIPAppUtilsAddressResolver()
    {
    iSocket.Close();
    iSocketServer.Close();
    }

// -----------------------------------------------------------------------------
// CIPAppUtilsAddressResolver::GetWlanMACAddress
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIPAppUtilsAddressResolver::GetWlanMACAddress (
    TDes8& aWlanMacAddress,
    const TDesC8& aByteFormat )
    {
    TBuf8<KPSWlanMacAddressLength> address;
    TInt retval = KErrNone;

    retval = RProperty::Get( 
        KPSUidWlan, 
        KPSWlanMacAddress, 
        address );
    
    if ( retval == KErrNone )
        {
        if ( aByteFormat.Length() == 0 )
            {
            //client wants raw data
            aWlanMacAddress.Copy( address );
            }
        else
            {
            for ( TInt counter = 0; counter < address.Length(); counter++ )
                {
                TUint16 a = (TUint16)address[ counter ];
                aWlanMacAddress.AppendNumFixedWidthUC( a, EHex, KWlanMacAddressPos );
                aWlanMacAddress.Append( aByteFormat );
                aWlanMacAddress.UpperCase();
                }

            // Delete the last format
            if ( aWlanMacAddress.Length() )
                {
                aWlanMacAddress.Delete( aWlanMacAddress.Length() - aByteFormat.Length(), aByteFormat.Length() );
                }
            }
        
        if ( aWlanMacAddress.Length() == 0 ) 
            {
            retval = KErrNotFound;
            }
        }
   
    return retval;
    }

// -----------------------------------------------------------------------------
// CIPAppUtilsAddressResolver::GetLocalIpAddressFromIap
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIPAppUtilsAddressResolver::GetLocalIpAddressFromIap( 
    TInetAddr& aLocalIpAddr,
    TUint32 aIapId )
    {
    TInt retval( KErrNotFound );
    
    TInt err = iSocket.SetOpt( KSoInetEnumInterfaces, KSolInetIfCtrl );
    
    if ( KErrNone == err )
        {
        TPckgBuf<TSoInetInterfaceInfo> info;
        
        err = iSocket.GetOpt( KSoInetNextInterface,
                KSolInetIfCtrl,info );
        
        while ( KErrNone == err && KErrNone != retval )
            {
            TPckgBuf<TSoInetIfQuery> query;
            query().iName = info().iName;
            
            err = iSocket.GetOpt( KSoInetIfQueryByName,
                    KSolInetIfQuery,query );
            
            if ( KErrNone == err )
                {
                retval = CheckAndSetAddr( 
                        aLocalIpAddr, (TInetAddr&)info().iAddress,
                        query().iZone[1], aIapId );
                
                err = iSocket.GetOpt( KSoInetNextInterface,
                        KSolInetIfCtrl,info );
                }
            }
        }

    if ( KErrNoMemory == err )
        {
        retval = err;
        }
    
    return retval;
    }

// -----------------------------------------------------------------------------
// CLocalAddrResolver::CheckAndSetAddr
// -----------------------------------------------------------------------------
//
TInt CIPAppUtilsAddressResolver::CheckAndSetAddr( 
    TInetAddr& aTarget,
    TInetAddr& aCandidate,
    TUint32 aCandidateIap,
    TUint32 aSpecifiedIap ) const
    {
    TInt retval = KErrNotFound;
    if ( !aCandidate.IsLinkLocal() &&
         !aCandidate.IsUnspecified() && 
         !aCandidate.IsLoopback() )
        {
        if ( aCandidate.IsV4Mapped())
            {
            aCandidate.ConvertToV4();
            }
        if ( aCandidateIap == aSpecifiedIap )
            {
            aTarget = aCandidate;
            aTarget.SetScope(0);
            retval = KErrNone;
            }
        }
    return retval;
    }                                         

// ---------------------------------------------------------------------------
// CIPAppUtilsAddressResolver::WlanSsidL
// Returns WLAN SSID based on given IAP id.
// ---------------------------------------------------------------------------
//
EXPORT_C void CIPAppUtilsAddressResolver::WlanSsidL(
    TUint32 aIapId, TDes& aWlanSsid )
    {
    RCmManagerExt cmManager;
    cmManager.OpenL();
    CleanupClosePushL( cmManager );
    // Check that bearer is WLAN.
    if ( KUidWlanBearerType == 
        cmManager.GetConnectionMethodInfoIntL( 
            aIapId, CMManager::ECmBearerType ) )
        {
        RCmConnectionMethodExt connMethod = 
            cmManager.ConnectionMethodL( aIapId );
        CleanupClosePushL( connMethod );
        
        // Get SSID for WLAN.
        HBufC* wlanSsid = connMethod.GetStringAttributeL( 
            CMManager::EWlanSSID );
        aWlanSsid.Copy( *wlanSsid );
        delete wlanSsid;
        
        CleanupStack::PopAndDestroy( &connMethod );    
        }
    else
        {
        User::Leave( KErrNotFound ); // leave if bearer is not WLAN
        }
    
    CleanupStack::PopAndDestroy( &cmManager );
    }

