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
* Description:  Connection progress notifier
*
*/


#include <es_sock.h>
#include <in_iface.h>
#include "dhcppsy.hrh"
#include "dhcppsylogging.h"
#include "dhcpconnectionstateobserver.h"
#include "dhcpconnectionprogressnotifier.h"

// ---------------------------------------------------------------------------
// Statical constructor
// ---------------------------------------------------------------------------
//
CDhcpConnectionProgressNotifier*   CDhcpConnectionProgressNotifier::NewL(RConnection& aConnection,
        MDhcpConnectionStateObserver*   aObserver)
    {
    CDhcpConnectionProgressNotifier* self = new (ELeave)
        CDhcpConnectionProgressNotifier (aConnection, aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop (self);
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CDhcpConnectionProgressNotifier::~CDhcpConnectionProgressNotifier ()
    {
    Cancel();
    }

// ---------------------------------------------------------------------------
// Second-phase constructor which can leave.
// ---------------------------------------------------------------------------
//
void CDhcpConnectionProgressNotifier::ConstructL()
    {
    iConnection.ProgressNotification(iProgress, iStatus);
    SetActive();
    }

// ---------------------------------------------------------------------------
// RunL
// ---------------------------------------------------------------------------
//
void CDhcpConnectionProgressNotifier::RunL()
    {
    TRACESTRING2( "CDhcpConnectionProgressNotifier::RunL, %d", iProgress().iStage);
    switch (iProgress().iStage)
        {
        case KConnectionUninitialised:
            {
            // Connection unitialised
            iState = EDhcpConnectStateNotConnected;
            }
            break;
        case KStartingSelection:
            {
            // Starting connetion selection
            iState = EDhcpConnectStateConnecting;
            }
            break;
        case KFinishedSelection:
            {
            // Selection finished
            if (iProgress().iError == KErrNone)
                {
                // The user successfully selected an IAP to be used
                iState = EDhcpConnectStateConnecting;
                }
            else
                {
                iState = EDhcpConnectStateNotConnected;
                }
            }
            break;
        case KConnectionFailure:
            {
            // Connection failure
            iState = EDhcpConnectStateNotConnected;
            }
            break;
        case KPsdStartingConfiguration:
        case KPsdFinishedConfiguration:
        case KCsdFinishedDialling:
        case KCsdScanningScript:
        case KCsdGettingLoginInfo:
        case KCsdGotLoginInfo:
            {
            // Prepearing connection (e.g. dialing)
            iState = EDhcpConnectStateConnecting;
            }
            break;
         case KCsdStartingConnect:
         case KCsdFinishedConnect:
            {
            // Creating connection (e.g. GPRS activation)
            iState = EDhcpConnectStateConnecting;
            }
            break;
        case KCsdStartingLogIn:
            {
            // Starting log in
            iState = EDhcpConnectStateConnecting;
            }
            break;
        case KCsdFinishedLogIn:
            {
            // Finished login
            iState = EDhcpConnectStateConnecting;
            }
            break;
        case KConnectionOpen:
            {
            iState = EDhcpConnectStateConnecting;
            }
            break;
        case KLinkLayerOpen:
            {
            // Connection open
            iState = EDhcpConnectStateConnected;
            }
            break;
        case KDataTransferTemporarilyBlocked:
            {
            // Connection blocked or suspended
            iState = EDhcpConnectStateConnected;
            }
            break;

        case KConnectionStartingClose:
            {
            // Hangup or GRPS deactivation
            iState = EDhcpConnectStateDisconnecting;
            }
            break;

        case KConnectionClosed:
        case KLinkLayerClosed:
            {
            // Connection closed
            iState = EDhcpConnectStateNotConnected;
            }
            break;
        default:
            {
            // Unhandled state
            iState = EDhcpConnectStateUnknown;
            }
            break;
        }
    iObserver->StateChangedL();
    iConnection.ProgressNotification(iProgress, iStatus);
    SetActive();
    }

// ---------------------------------------------------------------------------
// GetState
// ---------------------------------------------------------------------------
//
TInt CDhcpConnectionProgressNotifier::GetState ()
    {
    return iState;
    }

// ---------------------------------------------------------------------------
// DoCancel
// ---------------------------------------------------------------------------
//
void CDhcpConnectionProgressNotifier::DoCancel ()
    {
    TRACESTRING( "CDhcpConnectionProgressNotifier::DoCancel");
    iConnection.CancelProgressNotification();
    }

// ---------------------------------------------------------------------------
// CDhcpConnectionProgressNotifier
// ---------------------------------------------------------------------------
//
CDhcpConnectionProgressNotifier::CDhcpConnectionProgressNotifier (RConnection& aConnection,
    MDhcpConnectionStateObserver* aObserver) :
    CActive ( EPriorityStandard ), iConnection (aConnection),
    iObserver (aObserver)
    {
    CActiveScheduler::Add (this);
    }

// End of file
