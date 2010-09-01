/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CCchEtelNetworkStatusNotifier implementation
 *
*/


// INCLUDES
#include "cchetelnetworkstatusnotifier.h"
#include "cchclientserver.h"
#include "cchuihandler.h"
#include <mmtsy_names.h>
#include "cchlogger.h"


const TUint KCCHEtelRetryDelay  = 1000;
// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CCchEtelNetworkStatusNotifier::NewL() 
// ----------------------------------------------------------------------------
CCchEtelNetworkStatusNotifier* CCchEtelNetworkStatusNotifier::NewL( 
    MCchEtelNetworkStatusObserver& aEtelNetworkStatusObserver )
    {
    CCchEtelNetworkStatusNotifier* self = new(ELeave) 
        CCchEtelNetworkStatusNotifier( aEtelNetworkStatusObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CCchEtelNetworkStatusNotifier::CCchEtelNetworkStatusNotifier() 
// ----------------------------------------------------------------------------
CCchEtelNetworkStatusNotifier::CCchEtelNetworkStatusNotifier( 
    MCchEtelNetworkStatusObserver& aEtelNetworkStatusObserver ) 
    : CActive( EPriorityStandard ), 
    iEtelNetworkStatusObserver( aEtelNetworkStatusObserver ),
    iRegistrationStatus( RMobilePhone::ERegistrationUnknown ),
    iDelay( KCCHEtelRetryDelay )
    {
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
// CCchEtelNetworkStatusNotifier::ConstructL() 
// ----------------------------------------------------------------------------
void CCchEtelNetworkStatusNotifier::ConstructL()
    {    
    iTimer.CreateLocal();
    iState = ECCHEtelConnect;
    Connect();
    }

// ----------------------------------------------------------------------------
// CCchEtelNetworkStatusNotifier::~CCchEtelNetworkStatusNotifier() 
// ----------------------------------------------------------------------------

CCchEtelNetworkStatusNotifier::~CCchEtelNetworkStatusNotifier()
    {
    if( IsActive() )
    	{
        Cancel();    	
    	}
    iPhone.Close();
    iEtelServer.UnloadPhoneModule( KMmTsyModuleName );
    iTimer.Close();
    }

// -----------------------------------------------------------------------------
// CCchEtelNetworkStatusNotifier::Connect
// Handle notification from pubsub session.
// -----------------------------------------------------------------------------
//
void CCchEtelNetworkStatusNotifier::Connect() 
    {
    CCHLOGSTRING( "CCchEtelNetworkStatusNotifier::Connect IN" );
    TInt error( 0 );

    switch( iState )
        {
        case ECCHEtelConnect:
            error = iEtelServer.Connect();
            if( error )
                {
                break;                    
                }
            iState = ECCHEtelLoadModule;
                
        case ECCHEtelLoadModule:
            error = iEtelServer.LoadPhoneModule( KMmTsyModuleName );
            if( error )
                {
                break;                    
                }
            iState = ECCHEtelOpenPhone;
            
        case ECCHEtelOpenPhone:
            error = iPhone.Open( iEtelServer, KMmTsyPhoneName );
            if( error )
                {
                break;                    
                }
            else
                {
                iState = ECCHEtelConnected;                    
                iPhone.GetNetworkRegistrationStatus( iStatus, iRegistrationStatus );
                }
                
        }
    if ( error )
        {
        iTimer.After( iStatus, iDelay  );
        iDelay += KCCHEtelRetryDelay;
        }
    SetActive();
    
    CCHLOGSTRING( "CCchEtelNetworkStatusNotifier::Connect OUT" );
    }

// -----------------------------------------------------------------------------
// CCchEtelNetworkStatusNotifier::RunL
// Handle notification from pubsub session.
// -----------------------------------------------------------------------------
//
void CCchEtelNetworkStatusNotifier::RunL()
    {
    CCHLOGSTRING( "CCchEtelNetworkStatusNotifier::RunL IN" );
    
    if ( iState == ECCHEtelConnected )
       {
       OrderNotification();
       
       if ( RMobilePhone::ENotRegisteredNoService == iRegistrationStatus &&
               iLastRegistrationStatus != iRegistrationStatus )
           {
           iEtelNetworkStatusObserver.MobileNetworkNoService();
           }
       
       iLastRegistrationStatus = iRegistrationStatus;      
       }
    else 
       {
       Connect();
       }

    CCHLOGSTRING( "CCchEtelNetworkStatusNotifier::RunL OUT" );
    }
// -----------------------------------------------------------------------------
// CCchEtelNetworkStatusNotifier::DoCancel
// Handle cancel order on this active object.
// -----------------------------------------------------------------------------
//
void CCchEtelNetworkStatusNotifier::DoCancel()
    {
    CCHLOGSTRING( "CCchEtelNetworkStatusNotifier::DoCancel IN" );
    if ( EFalse == iNotified )
        {
        iPhone.CancelAsyncRequest( 
            EMobilePhoneGetNetworkRegistrationStatus );
        }
    else
        {
        iPhone.CancelAsyncRequest( 
            EMobilePhoneNotifyNetworkRegistrationStatusChange );
        }
    iNotified = EFalse;    
    
    CCHLOGSTRING( "CCchEtelNetworkStatusNotifier::DoCancel OUT" );
    }

// -----------------------------------------------------------------------------
// CCchEtelNetworkStatusNotifier::OrderNotification
// Order new notification from CenRep.
// -----------------------------------------------------------------------------
//
void CCchEtelNetworkStatusNotifier::OrderNotification()
    {
    CCHLOGSTRING( "CCchEtelNetworkStatusNotifier::OrderNotification IN" );
    
    iNotified = ETrue;
    iPhone.NotifyNetworkRegistrationStatusChange( 
        iStatus, 
        iRegistrationStatus );
    SetActive();
    
    CCHLOGSTRING( "CCchEtelNetworkStatusNotifier::OrderNotification OUT" );
    }

// -----------------------------------------------------------------------------
// CCchEtelNetworkStatusNotifier::IsNetworkStatusNoService
// Is network status no service.
// -----------------------------------------------------------------------------
//
TBool CCchEtelNetworkStatusNotifier::IsNetworkStatusNoService() const
    {
    CCHLOGSTRING( "CCchEtelNetworkStatusNotifier::IsNetworkStatusNoService" );
    
    if ( RMobilePhone::ENotRegisteredNoService == iRegistrationStatus )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// End of file
