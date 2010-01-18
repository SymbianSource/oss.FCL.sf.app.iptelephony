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
* Description:  account creation plugin params source file
*
*/


#include <f32file.h>
#include <mmtsy_names.h> // For getting network information.

#include "acpbrowserparams.h"
#include "accountcreationpluginlogger.h"
#include "accountcreationengineconstants.h"


// ---------------------------------------------------------------------------
// CACPBrowserParams::NewL
// ---------------------------------------------------------------------------
//
CACPBrowserParams* CACPBrowserParams::NewL()
    {
    CACPBrowserParams* self = NewLC();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CACPBrowserParams::NewLC
// ---------------------------------------------------------------------------
//
CACPBrowserParams* CACPBrowserParams::NewLC()
    {
    CACPBrowserParams* self = new (ELeave) CACPBrowserParams();
    CleanupStack::PushL(self);
    self->ConstructL( );
    return self;
    }

// ---------------------------------------------------------------------------
// CACPBrowserParams::CACPBrowserParams
// ---------------------------------------------------------------------------
//
CACPBrowserParams::CACPBrowserParams()         
    {
    }

// ---------------------------------------------------------------------------
// CACPBrowserParams::~CACPBrowserParams
// ---------------------------------------------------------------------------
//
CACPBrowserParams::~CACPBrowserParams()
    {
    iPhone.Close();
    iTelServer.Close();
    
#ifdef __PLUG_AND_PLAY_MOBILE_SERVICES    
    delete iPnpUtil;
#endif    
    }

// ---------------------------------------------------------------------------
// CACPBrowserParams::ConstructL
// ---------------------------------------------------------------------------
//
void CACPBrowserParams::ConstructL( )
    {
    ACPLOG( "CACPBrowserParams::ConstructL begin" );

#ifdef __PLUG_AND_PLAY_MOBILE_SERVICES    
    // Create PnP utility class instance.
    iPnpUtil = CPnpUtilImpl::NewLC();
    CleanupStack::Pop( iPnpUtil );
#endif
    
    // Initialize the telephone server
    User::LeaveIfError( iTelServer.Connect() );
    User::LeaveIfError( iTelServer.LoadPhoneModule( KPhoneModuleName ) );
    User::LeaveIfError( iPhone.Open( iTelServer, KPhoneName ) );

    RetrieveParamsL();

    ACPLOG( "CACPBrowserParams::ConstructL end" );
    }

// ---------------------------------------------------------------------------
// CACPBrowserParams::RetrieveParamsL
// ---------------------------------------------------------------------------
//
void CACPBrowserParams::RetrieveParamsL()
    {         
    ACPLOG( "CACPBrowserParams::RetrieveParamsL begin" );

    RetrievePhoneModel();
    RetrieveHomeNetworkInfoL();
    RetrieveSigningParamsL();
    RetrieveWlanMacAddress();
    RetrievePhoneInfoL();

    ACPLOG( "CACPBrowserParams::RetrieveParamsL end" );
    }

// ---------------------------------------------------------------------------
// CACPBrowserParams::RetrievePhoneModel
// ---------------------------------------------------------------------------
//
void CACPBrowserParams::RetrievePhoneModel()
    {
    ACPLOG( "CACPBrowserParams::RetrievePhoneModelL begin" );

    RMobilePhone::TMobilePhoneIdentityV1 phoneIdentity;
    TRequestStatus status( KRequestPending );
    iPhone.GetPhoneId( status, phoneIdentity );
    User::WaitForRequest( status );

    iPhoneModel.Copy( phoneIdentity.iModel );

    ACPLOG( "CACPBrowserParams::RetrievePhoneModelL end" );
    }

// ---------------------------------------------------------------------------
// CACPBrowserParams::RetrieveHomeNetworkInfoL
// ---------------------------------------------------------------------------
//
void CACPBrowserParams::RetrieveHomeNetworkInfoL()
    {
    ACPLOG( "CACPBrowserParams::RetrieveHomeNetworkInfoL begin" );

    RMobilePhone::TMobilePhoneNetworkCountryCode homeMcc( KNullDesC );
    RMobilePhone::TMobilePhoneNetworkIdentity homeMnc( KNullDesC );

    if ( RegisteredInNetwork() )
        {
        ACPLOG( " - registered in network, fetch info" );
        iPnpUtil->FetchHomeNetworkInfoL();
        homeMcc = iPnpUtil->HomeMccL();
        homeMnc = iPnpUtil->HomeMncL();
        }

    _LIT( KZero, "0" );
    if ( homeMcc.Length() )
        {
        iHomeMcc.Copy( homeMcc );
        }
    else
        {
        iHomeMcc.Copy( KZero );
        }
    
    if ( homeMnc.Length() )
        {
        iHomeMnc.Copy( homeMnc );
        }
    else
        {
        iHomeMnc.Copy( KZero );
        }
    
    ACPLOG( "CACPBrowserParams::RetrieveHomeNetworkInfoL end" );
    }

// ---------------------------------------------------------------------------
// CACPBrowserParams::RetrieveSigningParamsL
// ---------------------------------------------------------------------------
//
void CACPBrowserParams::RetrieveSigningParamsL()
    {
    ACPLOG( "CACPBrowserParams::RetrieveSigningParamsL begin" );

    // First reset nonce.
    iNonce.Zero();

#ifdef __PLUG_AND_PLAY_MOBILE_SERVICES    
    // Create new nonce.
    iPnpUtil->CreateNewNonceL( 0, iNonce );
    
    // Get key info.
    iPnpUtil->GetKeyInfoL( iKeyInfo );
#endif
    
    ACPLOG( "CACPBrowserParams::RetrieveSigningParamsL end" );
    }

// ---------------------------------------------------------------------------
// CACPBrowserParams::RetrieveWlanMacAddressL
// ---------------------------------------------------------------------------
//
void CACPBrowserParams::RetrieveWlanMacAddress()
    {
    ACPLOG( "CACPBrowserParams::RetrieveWlanMacAddress begin" );

    TBuf8<KPSWlanMacAddressLength> address;

    RProperty::Get( 
        KPSUidWlan, 
        KPSWlanMacAddress, 
        address );

    for ( TInt counter = 0; counter < address.Length(); counter++ )
        {
        TUint a = (TUint)address[counter];
        iWlanMACAddress.AppendNumFixedWidthUC( a, EHex, 2 );
        iWlanMACAddress.Append( KHyph );
        iWlanMACAddress.LowerCase();
        }

    // Delete the last ColonMark.
    if ( iWlanMACAddress.Length() )
        {
        iWlanMACAddress.Delete( iWlanMACAddress.Length() -1, 1 );
        }
  
    ACPLOG( "CACPBrowserParams::RetrieveWlanMacAddress end" );
    }

// ---------------------------------------------------------------------------
// CACPBrowserParams::GetParameters
// ---------------------------------------------------------------------------
//
void CACPBrowserParams::GetParameters( TDes& aUrl )
    {
    ACPLOG2( "CACPBrowserParams::GetParameters begin: %S", &aUrl );

    if ( aUrl.Find( KQuestionMark ) != KErrNotFound )
        {
        aUrl.Append( KAmperSand );
        }
    else
        {
        aUrl.Append( KQuestionMark );
        }    

    // Home MNC.
    aUrl.Append( KMnc );     
    aUrl.Append( KPlacing );
    if( iHomeMnc != KNullDesC )
        {
        aUrl.Append( iHomeMnc ); 
        }
    else
        {
        aUrl.Append( _L("0") );
        }

    aUrl.Append( KAmperSand );

    // Home MCC.
    aUrl.Append( KMcc );     
    aUrl.Append( KPlacing );
    if( iHomeMcc != KNullDesC )
        {
        aUrl.Append( iHomeMcc );
        }
    else
        {
        aUrl.Append( _L("0") );
        }

    aUrl.Append( KAmperSand );

    // Nonce.
    aUrl.Append( KNonce );     
    aUrl.Append( KPlacing );
    
    TBuf<KNonceLength> nonce16;
    nonce16.Copy( iNonce );
    aUrl.Append( nonce16 );

    aUrl.Append( KAmperSand );

    // Key Info.
    aUrl.Append( KKeyInfo );     
    aUrl.Append( KPlacing );
    
    TBuf<KMaxKeyInfoLength> keyInfo16;
    keyInfo16.Copy( iKeyInfo );
    aUrl.Append( keyInfo16 );

    aUrl.Append( KAmperSand );

    // Phone Model.
    aUrl.Append( KDeviceInfo );     
    aUrl.Append( KPlacing );
    aUrl.Append( iPhoneModel );

    // SType value is 2
    aUrl.Append( KAmperSand );
    aUrl.Append( KsType );     
    aUrl.Append( KPlacing );
    aUrl.Append( KCharacterTwo ); 

    // client version
    aUrl.Append( KAmperSand );
    aUrl.Append( KcVersion );     
    aUrl.Append( KPlacing );
    aUrl.Append( KValuecVersion );
    
    // IMEI
    aUrl.Append( KAmperSand );
    aUrl.Append( KImei );
    aUrl.Append( KPlacing );
    aUrl.Append( iImei );

    // Variant
    aUrl.Append( KAmperSand );
    aUrl.Append( KVariant );
    aUrl.Append( KPlacing );
    aUrl.Append( iVariant );
    
    ACPLOG2( "CACPBrowserParams::GetParameters end: %S", &aUrl );    
    }

// ---------------------------------------------------------------------------
// CACPBrowserParams::RetrievePhoneInfoL
// ---------------------------------------------------------------------------
//
void CACPBrowserParams::RetrievePhoneInfoL()
    {
    ACPLOG( "CACPBrowserParams::RetrievePhoneInfoL begin" );
    
    // Get IMEI.    
    RMobilePhone::TMobilePhoneIdentityV1 id;
    
    TRequestStatus status;
    iPhone.GetPhoneId( status, id );
    User::WaitForRequest( status );
    
    iImei.Copy( id.iSerialNumber );
    
    // Get product code.
    SysVersionInfo::GetVersionInfo( SysVersionInfo::EProductCode, iVariant );
    
    ACPLOG( "CACPBrowserParams::RetrievePhoneInfoL end" );
    }

// ---------------------------------------------------------------------------
// CACPBrowserParams::RegisteredInNetworkL
// ---------------------------------------------------------------------------
//
TBool CACPBrowserParams::RegisteredInNetwork()
    {
    ACPLOG( "CACPBrowserParams::RegisteredInNetwork begin" );
    // Get registeration status
    TRequestStatus status;
    RMobilePhone::TMobilePhoneRegistrationStatus regStatus( 
        RMobilePhone::ERegistrationUnknown );
    iPhone.GetNetworkRegistrationStatus( status, regStatus );
    ACPLOG2( " - network registration status %d", regStatus );
    User::WaitForRequest( status );
    if ( RMobilePhone::ERegistrationUnknown == regStatus || 
        RMobilePhone::ENotRegisteredNoService == regStatus ||
        RMobilePhone::ENotRegisteredEmergencyOnly == regStatus ||
        RMobilePhone::ENotRegisteredSearching == regStatus )
        {
        ACPLOG( "CACPBrowserParams::RegisteredInNetwork end" );
        return EFalse;
        }
    ACPLOG( "CACPBrowserParams::RegisteredInNetwork end" );
    return ETrue;
    }


// End of file.
