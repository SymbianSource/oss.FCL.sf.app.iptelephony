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
* Description:  Browse and activate proper connection method.
*
*/


#include <es_sock.h>
#include <es_enum.h>
#include "dhcppsylogging.h"
#include "dhcppsy.hrh"
#include "dhcpconnectionprogressnotifier.h"
#include "dhcpconnectionstateobserver.h"
#include "dhcpconnectionengine.h"

// ---------------------------------------------------------------------------
// CDhcpConnectionEngine::NewL
// ---------------------------------------------------------------------------
//
CDhcpConnectionEngine* CDhcpConnectionEngine::NewL (RConnection& aConnection)
    {
    CDhcpConnectionEngine* self = new (ELeave) CDhcpConnectionEngine (aConnection);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Member methods
// ---------------------------------------------------------------------------
// CDhcpConnectionEngine::CDhcpConnectionEngine
// ---------------------------------------------------------------------------
//
CDhcpConnectionEngine::CDhcpConnectionEngine (RConnection& aConnection) :  CActive ( EPriorityStandard ),
        iConnection (aConnection)
    {
    TRACESTRING( "CDhcpConnectionEngine::CDhcpConnectionEngine" );
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CDhcpConnectionEngine::~CDhcpConnectionEngine
// ---------------------------------------------------------------------------
//
CDhcpConnectionEngine::~CDhcpConnectionEngine()
    {
    TRACESTRING( "CDhcpConnectionEngine::~CDhcpConnectionEngine" );
    Cancel();
    delete iProgressNotifier;
    }
// ---------------------------------------------------------------------------
// CDhcpConnectionEngine::ConstructL
// ---------------------------------------------------------------------------
//
void CDhcpConnectionEngine::ConstructL()
    {
    TRACESTRING( "CDhcpConnectionEngine::ContructL" );
    iProgressNotifier = CDhcpConnectionProgressNotifier::NewL(iConnection, this);
    }

// ---------------------------------------------------------------------------
// CDhcpConnectionEngine::DoCancel
// ---------------------------------------------------------------------------
//
void CDhcpConnectionEngine::DoCancel()
    {
    TRACESTRING2 ( "CDhcpConnectionEngine::DoCancel, state %d", iStates);
    // Is this really needed ? Most cases SVP will continue with same IAP connection so
    // it's not good to drop a connection beforehand ...
    iProgressNotifier->Cancel ();
    iConnection.Close ();
    TRACESTRING( "CDhcpConnectionEngine::DoCancel, end" );
    }


// ---------------------------------------------------------------------------
// CDhcpConnectionEngine::ActiveWlanConnection
// ---------------------------------------------------------------------------
//
TBool CDhcpConnectionEngine::IsConnectedL (TUint32 aIAPId )
    {
    TUint connectionCount = 0;
    TBool isConnected = EFalse;
    if (iProgressNotifier->GetState () == EDhcpConnectStateConnected)
        {
        isConnected = ETrue;
        return isConnected;
        }

    User::LeaveIfError( iConnection.EnumerateConnections(connectionCount));
    TRACESTRING2 ("CDhcpConnectionEngine::IsConnectedL, connCount, %d", connectionCount )
    // Loop shall start from second item, otherwise this will not work.
    for ( TUint i = 1; i <= connectionCount; i++ )
        {
        TPckgBuf<TConnectionInfo> connInfo;
        User::LeaveIfError( iConnection.GetConnectionInfo( i, connInfo ) );
        if ( connInfo().iIapId == aIAPId )
            {
            User::LeaveIfError( iConnection.Attach(
                connInfo, RConnection::EAttachTypeNormal ) );
            isConnected = ETrue;
            break;
            }
        }
    return isConnected;
    }

// ---------------------------------------------------------------------------
// CDhcpConnectionEngine::StateChangedL
// ---------------------------------------------------------------------------
//
void CDhcpConnectionEngine::StateChangedL ()
    {
    switch(iProgressNotifier->GetState ())
        {
        case EDhcpConnectStateConnected:
            {
            if (iStates==EConnectionStartup)
                {
                TRequestStatus *status = iClientStatus;
                User::RequestComplete ( status, KErrNone );
                iStates = EIdle;
                }
            }
            break;
        }
    }


// ---------------------------------------------------------------------------
// CDhcpConnectionEngine::ConnectL
// ---------------------------------------------------------------------------
//
void CDhcpConnectionEngine::ConnectL (TUint aIAPConnectionId, TRequestStatus& aStatus)
    {
    TRACESTRING( "CDhcpConnectionEngine::ConnectL" );
    aStatus = KRequestPending;
    iClientStatus = &aStatus;
    iIapProfileId = aIAPConnectionId;
    iPreferences.SetIapId( iIapProfileId );
    iPreferences.SetDialogPreference( ECommDbDialogPrefDoNotPrompt );
    iConnection.Start( iPreferences, iStatus );
    iStates = ETurnConnectionOn;
    SetActive ();
    }

// ---------------------------------------------------------------------------
// CDhcpConnectionEngine::RunL
// ---------------------------------------------------------------------------
//
void CDhcpConnectionEngine::RunL()
    {
    TRACESTRING2( "CDhcpConnectionEngine::RunL, %d ", iStates);
    TRACESTRING2( "CDhcpConnectionEngine::RunL, %d ", iStatus.Int() );
    TInt retValue = iStatus.Int();
    if(retValue!=KErrNone)
        {
        //
        // Even we failed to connect specified wlan network, we should continue with
        // location query progress until the DHCPInform message has been send to someone.
        // This is reason for that we complete with KErrNone in here.
        //
        TRequestStatus *status = iClientStatus;
        User::RequestComplete ( status, KErrNone );
        iStates = EIdle;
        }
    else
        {
        switch (iStates)
            {
            case ETurnConnectionOn:
                {
                // Ok we just flag on until progress notification tells to us that
                iStates = EConnectionStartup;
                }
                break;
            }
        }
    }

// End of file
