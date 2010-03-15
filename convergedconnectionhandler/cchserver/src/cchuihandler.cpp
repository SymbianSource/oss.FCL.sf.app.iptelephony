/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CCchUIHandler implementation
*
*/


// INCLUDE FILES
#include "cchlogger.h"
#include "cchserverbase.h"
#include "cchservicehandler.h"
#include "cchclientserverinternal.h"
#include <centralrepository.h>
#include "cchetelnetworkstatusnotifier.h"
#include "cchfeaturemanager.h"
#include "cchnotehandler.h"
#include "cchprivatecrkeys.h"
#include <cch.rsg>
#include "cchuihandler.h"
#include "cchsecondarydisplayapi.h"
#include <CoreApplicationUIsSDKCRKeys.h>

// From AVKON
#include <AknSmallIndicator.h>
#include <avkon.hrh>

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES
// None

// CONSTANTS
// None

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
const TInt KDelayTimeOneSec(1000000);

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ============================= LOCAL FUNCTIONS =============================

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// CCchUIHandler::CCchUIHandler
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CCchUIHandler::CCchUIHandler( 
    CCCHServerBase& aServer, 
    CCCHServiceHandler& aCCchServiceHandler ) :
    CActive( CActive::EPriorityStandard ),
    iServer( aServer ),
    iCCchServiceHandler( aCCchServiceHandler )
    {
    CCHLOGSTRING( "CCchUIHandler::CCchUIHandler" );
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CCchUIHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CCchUIHandler::ConstructL()
    {
    CCHLOGSTRING( "CCchUIHandler::ConstructL" );
    User::LeaveIfError( iTimer.CreateLocal() );
    
    iCchEtelNetworkStatusNotifier = CCchEtelNetworkStatusNotifier::NewL( *this );
    iNoteHandler = CCchNoteHandler::NewL( iServer );
    iOfflineRepository = CRepository::NewL( KCRUidCoreApplicationUIs );
    iCchRepository = iNoteHandler->CchCenRep();

    MonitorOfflineStatusL();
    }

// ---------------------------------------------------------------------------
// CCchUIHandler::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CCchUIHandler* CCchUIHandler::NewL( 
    CCCHServerBase& aServer, 
    CCCHServiceHandler& aCCchServiceHandler )
    {
    CCHLOGSTRING( "CCchUIHandler::NewL" );
    CCchUIHandler* self = 
        CCchUIHandler::NewLC( aServer, aCCchServiceHandler );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CCchUIHandler::NewLC
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CCchUIHandler* CCchUIHandler::NewLC( 
    CCCHServerBase& aServer, 
    CCCHServiceHandler& aCCchServiceHandler )
    {
    CCHLOGSTRING( "CCchUIHandler::NewLC" );
    CCchUIHandler* self = 
        new (ELeave) CCchUIHandler( aServer, aCCchServiceHandler );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CCchUIHandler::NewLC
// Destructor.
// ---------------------------------------------------------------------------
//
CCchUIHandler::~CCchUIHandler()
    {   
    CCHLOGSTRING( "CCchUIHandler::~CCchUIHandler" );
    Cancel();
    iTimer.Close();
    
    delete iCchEtelNetworkStatusNotifier;
    delete iOfflineRepository;
    
    if ( iNoteHandler && iNoteHandler->CanBeDestroyed() )
    	{
    	delete iNoteHandler;
    	}  
    }
// ---------------------------------------------------------------------------
// CCchUIHandler::SetIndicatorStateL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCchUIHandler::SetIndicatorStateL( TInt aIndicator, 
                                        TInt aState ) const
    {
    CCHLOGSTRING( "CCchUIHandler::SetIndicatorStateL" );
    CAknSmallIndicator* theIndicator = 
        CAknSmallIndicator::NewLC( TUid::Uid( aIndicator ) );
    theIndicator->SetIndicatorStateL( aState );
    CleanupStack::PopAndDestroy( theIndicator );
    }

// ---------------------------------------------------------------------------
// CCchUIHandler::MobileNetworkNoService
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCchUIHandler::MobileNetworkNoService(  )
    {
    CCHLOGSTRING( "CCchUIHandler::MobileNetworkNoService" );
    }

// ---------------------------------------------------------------------------
// CCchUIHandler::HandleVoipStateChanged
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCchUIHandler::HandleVoipStateChanged( TBool aStatus )
    {
    CCHLOGSTRING( "CCchUIHandler::HandleVoipStateChanged" );
    TRAP_IGNORE( HandleVoipStateChangedL(aStatus) );
    }

// ---------------------------------------------------------------------------
// CCchUIHandler::HandleVoipStateChangedL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCchUIHandler::HandleVoipStateChangedL( TBool aStatus )
    {
    CCHLOGSTRING( "CCchUIHandler::HandleVoipStateChangedL" );
    TInt err;
    if ( aStatus && !iVoIPSmallIndicatorShown )   // Change VoIP indicator visibility
        {
        SetIndicatorStateL( EAknIndicatorVoIP, 
                EAknIndicatorStateOn ) ;
        iVoIPSmallIndicatorShown = ETrue;
        TInt emergencyWarningShown( 0 );
        err = iCchRepository->Get( 
             KCCHVoIPEmergencyWarningShown, emergencyWarningShown );
        if( err == KErrNone )
            {
            if( !emergencyWarningShown )
                {
                ShowEmergencyWarningNoteL( ETrue );
                }
            }
        }
    else if ( !aStatus && iVoIPSmallIndicatorShown )
        {
        iVoIPSmallIndicatorShown = EFalse;
        iServer.SetVoIPEmergencyNoteShown( EFalse );
        TRAP( err, SetIndicatorStateL( EAknIndicatorVoIP, 
                EAknIndicatorStateOff ) );
        }
    } 
// ---------------------------------------------------------------------------
// CCchUIHandler::UpdateUI
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//      
void CCchUIHandler::UpdateUI( )
    {
    CCHLOGSTRING( "CCchUIHandler::UpdateUI" );
    if( iCCchServiceHandler.Exists( ECCHVoIPSub, ECCHEnabled, KErrNone ) )
        {
        HandleVoipStateChanged( ETrue );
        }
    else
        {
        HandleVoipStateChanged( EFalse );
        }

    }
// ---------------------------------------------------------------------------
// CCchUIHandler::CheckGprsFirstUsageL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//      
void CCchUIHandler::CheckGprsFirstUsageL( )
    { 
    CCHLOGSTRING( "CCchUIHandler::CheckGprsFirstUsageL - IN" );
    
    // Check value from cenrep
    TInt gprsRoamingCostWarningShown( 0 );
    
    User::LeaveIfError( iCchRepository->Get( 
        KCCHGprsRoamingCostWarningShown, 
        gprsRoamingCostWarningShown ) );
    
    // Show gprs roaming cost warning note if not already shown
    if( !gprsRoamingCostWarningShown )
        {
        iNoteHandler->LaunchGlobalNoteL( 
            R_QTN_SERVTAB_ALLOW_GPRS_WHEN_ROAMING_QUERY, 
            R_AVKON_SOFTKEYS_OK_EMPTY,
            SecondaryDisplay::ECmdNoNote );     
        }
    
    CCHLOGSTRING( "CCchUIHandler::CheckGprsFirstUsageL - OUT" );
    }

// ---------------------------------------------------------------------------
// CCchUIHandler::NetworkConnectionsAllowed
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//      
TBool CCchUIHandler::NetworkConnectionsAllowed() const
    { 
    return iNetworkConnectionAllowed;
    }

// ----------------------------------------------------------------------------
// CCchUIHandler::ShowEmergencyWarningNoteL()
// (other items were commented in a header).
// ----------------------------------------------------------------------------
    
void CCchUIHandler::ShowEmergencyWarningNoteL( TBool aVoIPEnabledFirstTime )
    {
    CCHLOGSTRING( "CCchUIHandler::ShowEmergencyWarningNoteL" );
    if ( iServer.FeatureManager().VoIPSupported() )
        {
        if ( aVoIPEnabledFirstTime )
            {
            iNoteHandler->LaunchGlobalNoteL( 
                R_QTN_FIRST_EMERGENCY_WARNING_NOTE, 
                R_AVKON_SOFTKEYS_OK_EMPTY,
                SecondaryDisplay::ECmdShowVoipEmergencyCallReadinessQuery );
            iServer.SetVoIPEmergencyNoteShown( ETrue );
            }

        }
    } 

// ----------------------------------------------------------------------------
// CCchUIHandler::MonitorOfflineStatusL()
// ----------------------------------------------------------------------------
void CCchUIHandler::MonitorOfflineStatusL()
    {
    CCHLOGSTRING("CCchUIHandler::MonitorOfflineStatusL() IN"); 
     
    iOfflineRepository->Get( 
        KCoreAppUIsNetworkConnectionAllowed, 
        iNetworkConnectionAllowed );
          
    iOfflineRepository->NotifyRequest( 
            KCoreAppUIsNetworkConnectionAllowed, iStatus);
    
    SetActive();
    CCHLOGSTRING("CCchUIHandler::MonitorOfflineStatusL() OUT"); 
    }

// ----------------------------------------------------------------------------
// CCchUIHandler::Cancel()
// ----------------------------------------------------------------------------
void CCchUIHandler::DoCancel()
    {
    CCHLOGSTRING("CCchUIHandler::DoCancel()");
    iTimer.Cancel();
    iOfflineRepository->NotifyCancel(
            KCoreAppUIsNetworkConnectionAllowed );
    }

// ----------------------------------------------------------------------------
// CCchUIHandler::RunL()
// ----------------------------------------------------------------------------
void CCchUIHandler::RunL()
    {
    CCHLOGSTRING("CCchUIHandler::RunL() IN"); 
    
    if ( iDie )
        {
        delete this;
        }
    else
        {
        MonitorOfflineStatusL();
        }

    CCHLOGSTRING("CCchUIHandler::RunL() OUT"); 
    }

// ----------------------------------------------------------------------------
// CCchUIHandler::RunError()
// ----------------------------------------------------------------------------
TInt CCchUIHandler::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CCchUIHandler::Destroy()
// ----------------------------------------------------------------------------
void CCchUIHandler::Destroy()
    {
    CCHLOGSTRING("CCchUIHandler::Destroy() IN"); 
    iDie = ETrue;
    
    // Soon we are down so remove VoIP icon
    TRAP_IGNORE( SetIndicatorStateL( 
        EAknIndicatorVoIP, EAknIndicatorStateOff ) );
    Cancel();
    iTimer.After ( iStatus, KDelayTimeOneSec );
    SetActive();
    
    CCHLOGSTRING("CCchUIHandler::Destroy() OUT");
    }

// ========================== OTHER EXPORTED FUNCTIONS =======================

//  End of File
