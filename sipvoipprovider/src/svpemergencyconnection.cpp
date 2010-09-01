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



#include <nifman.h>   // For global variables

#include "svpemergencyconnection.h"
#include "svplogger.h" // For logging


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSVPEmergencyConnection
// ---------------------------------------------------------------------------
//
CSVPEmergencyConnection::CSVPEmergencyConnection( 
    TPriority aPriority, MSVPEmergencyConnectionObserver& aObserver )
    : CActive( aPriority ), 
      iObserver( aObserver )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CSVPEmergencyConnection::ConstructL()
    {
    SVPDEBUG1("CSVPEmergencyConnection::ConstructL()")
    
    User::LeaveIfError( iSocketServer.Connect() );
    User::LeaveIfError( iConnection.Open( iSocketServer ) );
    }

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CSVPEmergencyConnection* CSVPEmergencyConnection::NewL( 
    TPriority aPriority, MSVPEmergencyConnectionObserver& aObserver )
    {
    CSVPEmergencyConnection* self = CSVPEmergencyConnection::NewLC( 
        aPriority, aObserver );
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
CSVPEmergencyConnection* CSVPEmergencyConnection::NewLC( 
    TPriority aPriority, MSVPEmergencyConnectionObserver& aObserver )
    {
    CSVPEmergencyConnection* self = 
        new( ELeave ) CSVPEmergencyConnection( aPriority, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CSVPEmergencyConnection::~CSVPEmergencyConnection()
    {
    SVPDEBUG1("CSVPEmergencyConnection::~CSVPEmergencyConnection()")
    
    Cancel();

    iConnection.Close();
    iSocketServer.Close();
    }

// ---------------------------------------------------------------------------
// Connects asynchronically with SNAP ID
// ---------------------------------------------------------------------------
//
void CSVPEmergencyConnection::ConnectWithSnapIdL( TUint32 aSnapId )
    {
    SVPDEBUG2("CSVPEmergencyConnection::ConnectWithSnapIdL, ID: %d", aSnapId)

    if ( IsActive() )
        {
        User::Leave( KErrInUse );
        }

    // Start connection
    iRequestType = ESVPSnapConnect;
    iSnapConnPref.SetSnap( aSnapId );
    iConnection.Start( iSnapConnPref, iStatus );
    SetActive();
    }

// ---------------------------------------------------------------------------
// Returns the used IAP ID of SNAP connection
// ---------------------------------------------------------------------------
//
TInt CSVPEmergencyConnection::IapId( TUint32& aIapId )
    {
    SVPDEBUG1("CSVPEmergencyConnection::IapIdL()")
    
    _LIT( KIapId, "IAP\\Id" );
    TRAPD( error, iConnection.GetIntSetting( KIapId, aIapId ) )
    return error;
    }

// ---------------------------------------------------------------------------
// Connects with IAP ID
// ---------------------------------------------------------------------------
//
void CSVPEmergencyConnection::ConnectL( TUint32 aIapId )
    {
    SVPDEBUG2("CSVPEmergencyConnection::ConnectL(), IAP ID: %d", aIapId)
    
    if ( IsActive() )
        {
        User::Leave( KErrInUse );
        }
    
    // Start connection
    iRequestType = ESVPConnect;
    iConnPref.SetIapId( aIapId );
    iConnPref.SetDialogPreference( ECommDbDialogPrefDoNotPrompt );
    iConnection.Start( iConnPref, iStatus );
    SetActive();
    }

// ---------------------------------------------------------------------------
// Requests for SIP proxy address
// ---------------------------------------------------------------------------
//
#ifdef _DEBUG
void CSVPEmergencyConnection::RequestSipProxyAddressL( TUint32 aIapId )
#else
void CSVPEmergencyConnection::RequestSipProxyAddressL( TUint32 /*aIapId*/ )
#endif // __DEBUG
    {
    SVPDEBUG2("CSVPEmergencyConnection::RequestSipProxyAddressL(),\
        IAP ID: %d", aIapId)

    if ( IsActive() )
        {
        User::Leave( KErrInUse );
        }

    // Request SIP proxy address
    iRequestType = ESVPSipProxyAddress;
    iSipServerAddrBuf().index = 0;
    iConnection.Ioctl( 
        KCOLConfiguration, 
        KConnGetSipServerAddr, // DHCP option 120
        iStatus, 
        &iSipServerAddrBuf );
    SetActive();
    }
    
// ---------------------------------------------------------------------------
// From class CSVPEmergencyConnection.
// DoCancel
// ---------------------------------------------------------------------------
//
void CSVPEmergencyConnection::DoCancel()
    {
    SVPDEBUG1("CSVPEmergencyConnection::DoCancel()")
    
    iRequestType = ESVPNone;
    iConnection.CancelIoctl();
    iConnection.Close();
    }
    
// ---------------------------------------------------------------------------
// From class CActive.
// RunL
// ---------------------------------------------------------------------------
//
void CSVPEmergencyConnection::RunL()
    {
    TInt error = iStatus.Int();
    SVPDEBUG2( "CSVPEmergencyConnection::RunL(), error: %d", error )
    SVPDEBUG2( "CSVPEmergencyConnection::RunL(), Request type: %d", iRequestType )
    
    // execution goes to RunError() if error occured
    User::LeaveIfError( iStatus.Int() );
    switch ( iRequestType )
        {
        case ESVPSnapConnect:
            SVPDEBUG1( "CSVPEmergencyConnection::RunL() - ESVPSnapConnect" )
            iRequestType = ESVPNone;
            iObserver.SnapConnected();
            break;
        
        case ESVPConnect:
            SVPDEBUG1( "CSVPEmergencyConnection::RunL() - ESVPConnect" )
            iRequestType = ESVPNone;
            iObserver.Connected();
            break;
            
        case ESVPSipProxyAddress:
            {
            SVPDEBUG1( "CSVPEmergencyConnection::RunL() - ESVPSipProxyAddress" )
            iRequestType = ESVPNone;
            
            // Copy SIP proxy address in dotted-decimal notation
            HBufC16* sipProxyAddrBuf = HBufC16::NewLC( 39 ); // CS:1
            TPtr16 sipProxyAddrPtr = sipProxyAddrBuf->Des();
            iSipServerAddrBuf().address.Output( sipProxyAddrPtr );
            SVPDEBUG2(
                    "CSVPEmergencyConnection::RunL(), iSipServerAddrBuf: %S",
                    sipProxyAddrBuf )
            // Call observer
            iObserver.SipProxyAddressReady( *sipProxyAddrBuf );
            CleanupStack::PopAndDestroy( sipProxyAddrBuf ); // CS:0
            break;
            }
        case ESVPSipDomainAddress:
            {
            SVPDEBUG1( "CSVPEmergencyConnection::RunL() - ESVPSipDomainAddress" )
            iRequestType = ESVPNone;
            
            // Copy SIP proxy domain in dotted-decimal notation
            HBufC16* sipDomainAddrBuf = HBufC16::NewLC( iSipServerDomainBuf().domainName.Length() ); // CS:1
            TPtr16 sipDomainAddrPtr = sipDomainAddrBuf->Des();
            sipDomainAddrPtr.Copy( iSipServerDomainBuf().domainName );
            SVPDEBUG2(
                    "CSVPEmergencyConnection::RunL(), iSipDomainAddrBuf: %S",
                    sipDomainAddrBuf )
            // Call observer
            iObserver.SipProxyAddressReady( *sipDomainAddrBuf );
            CleanupStack::PopAndDestroy( sipDomainAddrBuf ); // CS:0
            break;
            }
            
        default:
            SVPDEBUG1( "CSVPEmergencyConnection::RunL() - Default" )
            iRequestType = ESVPNone;
            iObserver.ConnectionError( KErrGeneral );
            break;
        }
    }

// ---------------------------------------------------------------------------
// CSVPEmergencyConnection::RunError
//
// ---------------------------------------------------------------------------
//
TInt CSVPEmergencyConnection::RunError( TInt aError )
    {
    SVPDEBUG2( "CSVPEmergencyConnection::RunError() %d", aError )
    if ( iRequestType == ESVPSipProxyAddress )
        {
        /* 
         * sip proxy address might be NULL if requested with KConnGetSipServerAddr
         * and dhcp returns domain name to sip, instead of ipv4 or ipv6 type address.
         * So try once again with KConnGetSipServerDomain.
         */
        SVPDEBUG1( "CSVPEmergencyConnection::RunError() - RequestDomain" )
        RequestSipServerDomainL();    
        return KErrNone;
        }
    else
        {
        iRequestType = ESVPNone;
        SVPDEBUG2( "CSVPEmergencyConnection::RunError() - Notify Observer ConnectionError( %d )", aError )
        iObserver.ConnectionError( aError );
        return KErrNone;
        }
    }

// ---------------------------------------------------------------------------
// Requests for SIP proxy address
// ---------------------------------------------------------------------------
//
void CSVPEmergencyConnection::RequestSipServerDomainL()
    {
    SVPDEBUG1( 
        "CSVPEmergencyConnection::RequestSipServerDomainL() - Try to get Sip server domain" )

    if ( IsActive() )
        {
        User::Leave( KErrInUse );
        }

    iRequestType = ESVPSipDomainAddress;
    iSipServerDomainBuf().index = 0;
    // Request SIP server domain
    iConnection.Ioctl(
        KCOLConfiguration,
        KConnGetSipServerDomain, // domain in textual format
        iStatus,
        &iSipServerDomainBuf );
    SetActive();
    }
