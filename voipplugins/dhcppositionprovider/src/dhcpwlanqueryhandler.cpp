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
* Description: 
*
*/

#include <sip.h>
#include <sipprofileregistry.h>
#include <sipprofile.h>
#include <comms-infras/es_config.h>
#include <in_sock.h>

#include "dhcppsylogging.h"
#include "dhcppsyrequesterprogressobserver.h"
#include "dhcppsypanic.h"
#include "dhcpsocketlistener.h"
#include "dhcpwlanqueryhandler.h"
#include "dhcplocationinformationparser.h"
#include "dhcpdatasender.h"
#include "dhcpdatalocationrequest.h"
#include "dhcpconstants.h"
#include "dhcpconnectionengine.h"
#include "dhcpserveripaddressresolver.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CDhcpWlanQueryHandler* CDhcpWlanQueryHandler::NewL(MDhcpPsyRequesterProgressObserver *aObserver)
    {
    TRACESTRING( "CDhcpWlanQueryHandler::NewL" );
    CDhcpWlanQueryHandler* self = new(ELeave) CDhcpWlanQueryHandler (aObserver);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CDhcpWlanQueryHandler::~CDhcpWlanQueryHandler()
    {
    TRACESTRING( "CDhcpWlanQueryHandler::~CDhcpWlanQueryHandler" );

    // Close opened resources and delete reserved memory
    Cancel();

    Flush ();

    delete iLctnDataRequestor;
    delete iLctnDataListener;

    iSocket.Close ();

    iConnection.Close();
    iSocketServer.Close();

    TRACESTRING( "CDhcpWlanQueryHandler::~CDhcpWlanQueryHandler -- end" );
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CDhcpWlanQueryHandler::CDhcpWlanQueryHandler (MDhcpPsyRequesterProgressObserver *aObserver)
    : CActive (CActive::EPriorityStandard), iPsyProgressObserver (aObserver)
    {
    TRACESTRING( "CDhcpWlanQueryHandler::CDhcpWlanQueryHandler" );
    CActiveScheduler::Add(this);
    }

// ---------------------------------------------------------------------------
// LocationData
// ---------------------------------------------------------------------------
//
HBufC8*  CDhcpWlanQueryHandler::LocationData ()
    {
    return iLocationBufferData;
    }

// ---------------------------------------------------------------------------
// MakeLocationQueryL
// ---------------------------------------------------------------------------
//
void CDhcpWlanQueryHandler::MakeLocationQueryL (TUint32 aIapProfileId)
    {
    TRACESTRING( "CDhcpWlanQueryHandler::MakeLocationQueryL" )
    iIapProfileId = aIapProfileId;

    // Check is the device already connected to the wlan accessoint.
    TBool connectionAttached = iConnectionEngine->IsConnectedL(iIapProfileId);
    if (connectionAttached)
        {
        TRACESTRING("CDhcpWlanQueryHandler::MakeLocationQueryL, Attached" )
        iDhcpResolver = CDhcpPsyDhcpServerIPResolver::NewL(iConnection, iStatus);
        iState = EDhcpIpAddressQuery;
        }
    else
        {
        TRACESTRING("CDhcpWlanQueryHandler::MakeLocationQueryL, Not attached" )
        iConnectionEngine->ConnectL (aIapProfileId, iStatus);
        iState = EAttachToConnection;
        }

    iPsyProgressObserver->ProgressPsyRequestNotificationL (MDhcpPsyRequesterProgressObserver::ELocationQueryStarted,
         KErrNone);

    SetActive();
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CDhcpWlanQueryHandler::ConstructL()
    {
    TRACESTRING( "CDhcpWlanQueryHandler::ConstructL" );
    User::LeaveIfError( iSocketServer.Connect() );
    User::LeaveIfError( iConnection.Open( iSocketServer ) );

    iConnectionEngine = CDhcpConnectionEngine::NewL(iConnection);
    // Add socket listener and requestor classes up. Rest parameters will be
    // provided later on.
    iLctnDataRequestor = CDhcpDataRequestor::NewL(iSocket);
    iLctnDataListener = CDhcpPsySocketListener::NewL(&iSocket);
    }

// -----------------------------------------------------------------------------
// CDhcpWlanQueryHandler::RunL
// -----------------------------------------------------------------------------
//
void CDhcpWlanQueryHandler::RunL()
    {
    TRACESTRING2( "CDhcpWlanQueryHandler::RunL, %d ", iStatus.Int());
    TRACESTRING2( "CDhcpWlanQueryHandler::RunL, state %d ", iState);
    TInt result = iStatus.Int();
    if (result != KErrNone && iState!=EDhcpIpAddressQuery)
        {
        iState=EDhpWlanIdle;
        iPsyProgressObserver->ProgressPsyRequestNotificationL (MDhcpPsyRequesterProgressObserver::ELocationQueryFinished,
             iStatus.Int());
        return;
        }
    switch (iState)
        {
        case EAttachToConnection:
            {
            TRACESTRING( "CDhcpWlanQueryHandler::RunL, AttachToConnection, start");

            iState=EDhcpIpAddressQuery;

            // Get ip address of the dhcp server.
            iDhcpResolver = CDhcpPsyDhcpServerIPResolver::NewL(iConnection, iStatus);
            SetActive ();
            }
            break;
        case EDhcpIpAddressQuery:
            {
            TRACESTRING( "CDhcpWlanQueryHandler::RunL, create and send the dhcp inform message to the wlan net, start");

            User::LeaveIfError( iSocket.Open( iSocketServer,
                    KAfInet,
                    KSockDatagram,
                    KProtocolInetUdp,
                    iConnection ) );

            TInetAddr addr( KInetAddrAny, KDhcpDefaultCliPort );
            iSocket.Bind( addr );

            // Create a dhcp inform message and send it.
            iLctnDataRequestor->SendDHCPMessageL(iDhcpResolver->DhcpServerInetAddress(),
                iIapProfileId, iStatus);

            iState = ESendDhcpLocatinQuery;
            SetActive ();
            }
            break;

        case ESendDhcpLocatinQuery:
            {
            TRACESTRING( "CDhcpWlanQueryHandler::RunL, send complete" );
            // Store transaction id to the listener object
            iLctnDataListener->SetTransActionIdToFollow (iLctnDataRequestor->TransActionIdToFollow());
            // And start to listen the dhcp ack message from the subnet.
            iLctnDataListener->RcvDHCPMessage (iStatus);
            iState = EReceiveDhcpLocationAck;
            SetActive ();
            }
            break;
        case EReceiveDhcpLocationAck:
            {
            TRACESTRING( "CDhcpWlanQueryHandler::RunL, response received." );
            TInt statusCode = iStatus.Int();

            // Make final XML buffer if possible.
            TPtrC8 geoConfigInformation (iLctnDataListener->GeoConfigDhcpInformation());
            TPtrC8 civicInformation (iLctnDataListener->LocationCivicInformation ());

            TRACESTRING2( "CDhcpWlanQueryHandler::RunL, Response, geoConfig length %d",
                    geoConfigInformation.Length()
                    );

            TRACESTRING2 ( "CDhcpWlanQueryHandler::RunL, Response, civicInformation length %d",
                    civicInformation.Length()
                    );

            if(iLocationBufferData)
                {
                delete iLocationBufferData;
                iLocationBufferData = NULL;
                }

            // We should able to calculate maximun length of target buffer
            iLocationBufferData = HBufC8::NewL(KDhcpMaxLocInfoLength);
            TDhcpLocationInformationParser informationParser;
            TPtr8 ptr (iLocationBufferData->Des());

            // Parse at next preferred order option 123 and if doesn't found then option 99 if
            // possible ...
            if (geoConfigInformation.Length())          // option 123, GeoSpatial location information
                {
                informationParser.ParseLocationInformation(
                    geoConfigInformation,
                    TDhcpLocationInformationParser::EDHCPCoordinates, ptr);
                }
            else if (civicInformation.Length())        // option 99, civic address location information
                {
                informationParser.ParseLocationInformation(
                    civicInformation,
                    TDhcpLocationInformationParser::EDHCPCivicAddress, ptr);
                }
            else
                {
                // No valid information from the dhcp server,  notify caller for that.
                statusCode = KErrNotFound;
                }

            iState=EDhpWlanIdle;
            iPsyProgressObserver->ProgressPsyRequestNotificationL (MDhcpPsyRequesterProgressObserver::ELocationQueryFinished,
                 statusCode );
            }
            break;
        default:
            {
            TRACESTRING( "CDhcpWlanQueryHandler::RunL, invalid switch-case" );
            User::Leave (KErrGeneral);
            }
            break;
        }
    }

// -----------------------------------------------------------------------------
// CDhcpWlanQueryHandler::DoCancel
// -----------------------------------------------------------------------------
//
void CDhcpWlanQueryHandler::DoCancel()
    {
    TRACESTRING( "CDhcpWlanQueryHandler::DoCancel" )
    if (iConnectionEngine)
        {
        iConnectionEngine->Cancel();
        }
    // Cancel onqoing request if needed.
    if (iDhcpResolver)
        {
        iDhcpResolver->Cancel();
        }

    if (iLctnDataRequestor)
        {
        iLctnDataRequestor->Cancel();
        }

    if (iLctnDataListener)
        {
        iLctnDataListener->Cancel();
        }

    TRAPD (err,
        iPsyProgressObserver->ProgressPsyRequestNotificationL (MDhcpPsyRequesterProgressObserver::ECancelled,
             KErrNone ) ) ;

    TRequestStatus *status = &iStatus;
    User::RequestComplete ( status, err );

    TRACESTRING( "CDhcpWlanQueryHandler::DoCancel finished" )
    }

// -----------------------------------------------------------------------------
// CDhcpWlanQueryHandler::RunError
// -----------------------------------------------------------------------------
//
TInt CDhcpWlanQueryHandler::RunError (TInt aErrorCode)
    {
    TRACESTRING2( "CDhcpWlanQueryHandler::RunError %d", aErrorCode )
    TRACESTRING2( "CDhcpWlanQueryHandler::RunError, state, %d", iState )
    iState=EDhpWlanIdle;
    TRAPD (err,
        iPsyProgressObserver->ProgressPsyRequestNotificationL (MDhcpPsyRequesterProgressObserver::ELocationQueryFinished,
            aErrorCode ) );
    return err;
    }

// -----------------------------------------------------------------------------
// CDhcpWlanQueryHandler::Flush
// -----------------------------------------------------------------------------
//
void CDhcpWlanQueryHandler::Flush ()
    {
    TRACESTRING( "CDhcpWlanQueryHandler::Flush" )
    delete iConnectionEngine;
    iConnectionEngine = NULL;
    delete iDhcpResolver;
    iDhcpResolver = NULL;
    delete iLocationBufferData;
    iLocationBufferData=NULL;
    }

