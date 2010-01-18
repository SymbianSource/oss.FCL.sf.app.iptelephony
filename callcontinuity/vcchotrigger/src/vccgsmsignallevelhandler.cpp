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
* Description:   
*
*/



#include <mmtsy_names.h>

#include "vccgsmsignallevelhandler.h"
#include "rubydebug.h"

static const TInt32 KStrengthMin = 110;

// ======== MEMBER FUNCTIONS ========
// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CVccGsmSignalLevelHandler::CVccGsmSignalLevelHandler( 
                                             MVccSignalLevelObserver& aObserver, 
                                             const TSignalLevelParams& aParams )
    : CVccSignalLevelHandler( aObserver, aParams )
    {
    }
    
// ---------------------------------------------------------------------------
// Symbian constructor.
// ---------------------------------------------------------------------------
//
void CVccGsmSignalLevelHandler::ConstructL()
    {
    RUBY_DEBUG_BLOCKL( "CVccGsmSignalLevelHandler::ConstructL" );

    CVccSignalLevelHandler::ConstructL();
    User::LeaveIfError( iEtelServer.Connect() );
    User::LeaveIfError( iEtelServer.LoadPhoneModule( KMmTsyModuleName ) );
    User::LeaveIfError( iPhone.Open( iEtelServer, KMmTsyPhoneName ) );
    iGsmNotifications = CVccGsmNotifications::NewL( iPhone );
    }   
    
// ---------------------------------------------------------------------------
// Symbian constructor.
// ---------------------------------------------------------------------------
// 
CVccGsmSignalLevelHandler * CVccGsmSignalLevelHandler::NewL( 
                                            MVccSignalLevelObserver& aObserver, 
                                            const TSignalLevelParams& aParams )
    {
    CVccGsmSignalLevelHandler * self = 
                         CVccGsmSignalLevelHandler::NewLC( aObserver, aParams );
    CleanupStack::Pop(self);
    return self;
    }    

// ---------------------------------------------------------------------------
// Symbian constructor.
// ---------------------------------------------------------------------------
// 
CVccGsmSignalLevelHandler * CVccGsmSignalLevelHandler::NewLC( 
                                            MVccSignalLevelObserver& aObserver, 
                                            const TSignalLevelParams& aParams )
    {
    CVccGsmSignalLevelHandler * self = 
                    new(ELeave) CVccGsmSignalLevelHandler( aObserver, aParams );
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// C++ destructor.
// ---------------------------------------------------------------------------
//    
CVccGsmSignalLevelHandler::~CVccGsmSignalLevelHandler()
    {
    Cancel(); // Cancel any request, if outstanding
    delete iGsmNotifications;
    iPhone.Close();
    iEtelServer.Close();
    }
    
// ---------------------------------------------------------------------------
// Enables signal level change notifications.
// ---------------------------------------------------------------------------
// 
void CVccGsmSignalLevelHandler::EnableNotificationsL()
    {
    RUBY_DEBUG_BLOCK( "CVccGsmSignalLevelHandler::EnableNotificationsL" );

    iGsmNotifications->ActivateNotifications( this,
                                  iParams.iLowLevel, iParams.iHighLevel );
    }
    
// ---------------------------------------------------------------------------
// Disables signal level change notifications.
// ---------------------------------------------------------------------------
// 
void CVccGsmSignalLevelHandler::DisableNotifications()
    {
    iGsmNotifications->CancelNotifications();
    }
    
// ---------------------------------------------------------------------------
// Issues signal strength request to RMobilePhone.
// ---------------------------------------------------------------------------
// 
void CVccGsmSignalLevelHandler::GetStrength()
    {
    RUBY_DEBUG_BLOCK( "CVccGsmSignalLevelHandler::GetStrength" );

    iPhone.GetSignalStrength( iStatus, iStrength, iBar );
    RUBY_DEBUG3( " -iStatus = %d, iStrength = %d, iBar = %d",
                            iStatus.Int(), iStrength, iBar );
    }

// ---------------------------------------------------------------------------
// Cancels signal strength request
// ---------------------------------------------------------------------------
// 
void CVccGsmSignalLevelHandler::CancelGetStrength()
    {
    RUBY_DEBUG_BLOCK( "CVccGsmSignalLevelHandler::CancelGetStrength" );
   
    // EMobilePhoneNotifyModeChange
    iPhone.CancelAsyncRequest( EMobilePhoneGetSignalStrength ); 
    }
    
// ---------------------------------------------------------------------------
// Notification that signal strength has changed is received.
// ---------------------------------------------------------------------------
//
void CVccGsmSignalLevelHandler::GsmStrengthChanged( TInt32 aStrength )
    {
    RUBY_DEBUG_BLOCK( "CVccGsmSignalLevelHandler::GsmStrengthChanged" );
    RUBY_DEBUG1( " -aStrength = %d", aStrength );

    iStrength = aStrength  ? aStrength : KStrengthMin;
    StrengthChanged();
    }

// ---------------------------------------------------------------------------
// Notififies observer about signal level change.
// ---------------------------------------------------------------------------
//
void CVccGsmSignalLevelHandler::NotifyChanges(
    TInt32 aSignalStrength,
    MVccSignalLevelObserver::TSignalStrengthClass aClass )
    {
    RUBY_DEBUG_BLOCK( "CVccGsmSignalLevelHandler::NotifyChanges" );
    iObserver.GsmSignalChanged( aSignalStrength, aClass );
    }
