/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of class CCchUiApiImpl.
*
*/


#include "cchuilogger.h"
#include "cchuiapiimpl.h"
#include "cchuiobserver.h"
#include "cchuispshandler.h"
#include "cchuicchhandler.h"
#include "cchuinotehandler.h"
#include "cchuiclientobserver.h"
#include "cchuiconnectionhandler.h"

const TInt KServiceNameMaxLength = 255;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CCchUiApiImpl* CCchUiApiImpl::NewL( CCch& aCch )
    {
    CCchUiApiImpl* cchUiApiImpl = 
        new ( ELeave ) CCchUiApiImpl();
    CleanupStack::PushL( cchUiApiImpl );
    cchUiApiImpl->ConstructL( aCch );
    CleanupStack::Pop( cchUiApiImpl ); 
    return cchUiApiImpl;
    }    

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CCchUiApiImpl::CCchUiApiImpl(): 
    iReEnableService( EFalse ),
    iRemovableConnection( KErrNone ),
    iObservedService( 0 )
    {
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
void CCchUiApiImpl::ConstructL( CCch& aCch )
    {
    iCCHHandler = CCchUiCchHandler::NewL( aCch );    
    iSpsHandler = CCchUiSpsHandler::NewL();
    iNoteController = CCCHUiNoteHandler::NewL( *this );
    iConnectionHandler = CCchUiConnectionHandler::NewL( 
        *iCCHHandler, *iSpsHandler );
    // voip sub service is by default the supported one. 
    // Client can overwrite these
    iAllowedSubServices.AppendL( ECCHVoIPSub );   
    
    iLastOperationResult = 
          MCchUiObserver::ECchUiClientOperationResultNotSet;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CCchUiApiImpl::~CCchUiApiImpl()
    {    
    if ( iObservedService && iCCHHandler )
        {
        TRAP_IGNORE( iCCHHandler->StopObservingL( 
           iObservedService, *this ) );
        }
    
    iAllowedNotes.Close();
    iAllowedSubServices.Close();    
    iObservervedServices.Close();
    iObservers.ResetAndDestroy();
    iObservers.Close();
    delete iCCHHandler;
    delete iNoteController;
    delete iSpsHandler;
    delete iConnectionHandler;      
    }

// ---------------------------------------------------------------------------
// AddObserver.
// ---------------------------------------------------------------------------
//
void CCchUiApiImpl::AddObserverL( MCchUiObserver& aObserver  )
    {
    CCHUIDEBUG( "CCchUiApiImpl::AddObserver - IN"); 
        
    // Check if already exists
    TBool alreadyExists = EFalse;
    for ( TInt i( 0 ) ; i < iObservers.Count() ; i++ )
        {        
        if ( &aObserver == &iObservers[ i ]->Observer() )
            {
            CCHUIDEBUG( "AddObserver - already exists");           
            alreadyExists = ETrue;
            }
        }
    
    if ( !alreadyExists )
        {
        CCHUIDEBUG( "AddObserver - add observer"); 
        
        CCchUiClientObserver* capsulatedObs = 
            CCchUiClientObserver::NewLC( aObserver );
        
        iObservers.AppendL( capsulatedObs );     
        CleanupStack::Pop( capsulatedObs );
        } 
    
    CCHUIDEBUG( "CCchUiApiImpl::AddObserver - OUT");
    }

// ---------------------------------------------------------------------------
// RemoveObserver.
// ---------------------------------------------------------------------------
//
void CCchUiApiImpl::RemoveObserver( MCchUiObserver& aObserver  )
    {
    for ( TInt i = 0 ; i < iObservers.Count() ; i++ )
        {
        if ( &aObserver == &iObservers[ i ]->Observer() )
            {           
            delete iObservers[ i ];
            iObservers.Remove( i );
               
            CCHUIDEBUG( "RemoveObserver - observer removed" );
            }
        }
    iObservers.Compress();
    }

// ---------------------------------------------------------------------------
// Show dialog.
// ---------------------------------------------------------------------------
//
void CCchUiApiImpl::ShowDialogL( 
    TUint32 aServiceId, 
    MCchUiObserver::TCchUiDialogType aDialog )
    {
    CCHUIDEBUG2( "CCchUiApiImpl::ShowDialogL - aServiceId: %d", 
        aServiceId );            
    CCHUIDEBUG2( "CCchUiApiImpl::ShowDialogL - aDialog: %d", 
        aDialog );      
    
    TInt err( KErrNone );
    
    // Get current connection iap id
    TInt iapId( KErrNone );
    iCCHHandler->GetCurrentConnectionIapIdL(
        aServiceId, ECCHUnknown, iapId, err );
    
    // Get current service username
    RBuf userName;
    CleanupClosePushL( userName );
    userName.CreateL( KServiceNameLength );
    iCCHHandler->GetUsernameL( aServiceId, userName, err );
    
    if ( KErrNone == err || KErrNotFound == err )
        {
        CCHUIDEBUG( "ShowDialogL -Launch note" );
        iNoteController->LaunchNoteL( aDialog, aServiceId, iapId, userName );
        }
    else // leave if error code other that KErrNone or KErrNotFound
        {
        CCHUIDEBUG2( "ShowDialogL - leave with err=%d", err );
        User::Leave( err );
        }
    
    CleanupStack::PopAndDestroy( &userName );
    }
    
// ---------------------------------------------------------------------------
// Get currently showing dialog.
// ---------------------------------------------------------------------------
//
void CCchUiApiImpl::ConfigureVisualizationL( 
    RArray<MCchUiObserver::TCchUiDialogType>& aAllowedNotes,
    RArray<TCCHSubserviceType>& aAllowedSubServices )
    {
    CCHUIDEBUG( "CCchUiApiImpl::ConfigureVisualizationL - IN" );
    
    CCHUIDEBUG2( "ConfigureVisualizationL - aAllowedNotes count=%d", 
        aAllowedNotes.Count() );
    CCHUIDEBUG2( "ConfigureVisualizationL - aAllowedSubServices count=%d", 
        aAllowedSubServices.Count() );
    
    TBool subServicesHandled = EFalse;
    TBool dialogsHandled = EFalse;
    
    // Reset old data
    iAllowedNotes.Reset();
    iAllowedSubServices.Reset();

    if ( KErrNotFound != aAllowedNotes.Find( 
        MCchUiObserver::ECchUiDialogTypeNotSet ) )
        {
        CCHUIDEBUG( "ConfigureVisualizationL - all dialogs are supported" );
        dialogsHandled = ETrue;
        }
    
    for ( TInt noteIndex = 0 ; 
          noteIndex < aAllowedNotes.Count() && !dialogsHandled ; 
          noteIndex++ )
        {
        CCHUIDEBUG2( "ConfigureVisualizationL - appending allowed note: %d",
            aAllowedNotes[ noteIndex ] );
        iAllowedNotes.AppendL( aAllowedNotes[ noteIndex ] );
        }
    
    if ( KErrNotFound != aAllowedSubServices.Find( ECCHUnknown ) )
        {
        CCHUIDEBUG( 
            "ConfigureVisualizationL - all sub services are supported" );
        iAllowedSubServices.AppendL( ECCHUnknown );
        subServicesHandled = ETrue;
        }
    for ( TInt servIndex = 0 ; 
          servIndex < aAllowedSubServices.Count() && subServicesHandled ; 
          servIndex++ )
        {
        CCHUIDEBUG2( 
            "ConfigureVisualizationL - appending allowed sub service: %d", 
            aAllowedSubServices[ servIndex ] );
        iAllowedSubServices.AppendL( aAllowedSubServices[ servIndex ] );
        }    
    
    CCHUIDEBUG( "CCchUiApiImpl::ConfigureVisualizationL - OUT" );
    }

// ---------------------------------------------------------------------------
// Cancel notes/dialogs.
// ---------------------------------------------------------------------------
//
void CCchUiApiImpl::CancelNotes()
    {
    CCHUIDEBUG( "CCchUiApiImpl::CancelNotes" );            
    iNoteController->CancelOldNotes();
    }

// ---------------------------------------------------------------------------
// Manual enable has proceeded
// ---------------------------------------------------------------------------
//
void CCchUiApiImpl::ManualEnableResultL( 
    TUint aServiceId, 
    TInt aEnableResult )
    {
    CCHUIDEBUG( "CCchUiApiImpl::ManualEnableResultL - IN" );    
        
    if ( KErrNone == aEnableResult )
        {
        CCHUIDEBUG( "ManualEnableStartedL - start observing service" );
        iCCHHandler->StartObservingL( aServiceId, *this );   
          
        iObservedService = aServiceId;
               
        // Check if id already exists in observed services
        TInt err = iObservervedServices.Find( aServiceId ); 
               
        // Append id only if it is not found.
        if ( KErrNotFound == err )
            {
            CCHUIDEBUG( "ManualEnableStartedL - add to observerd services" );
            iObservervedServices.AppendL( aServiceId );
            }   
        }
    else
        {
        HandleManualEnableErrorL( aServiceId, aEnableResult );
        }
    
    CCHUIDEBUG( "CCchUiApiImpl::ManualEnableStartedL - OUT" );    
    }    
   
// ---------------------------------------------------------------------------
// Manual enable error handling.
// ---------------------------------------------------------------------------
//
void CCchUiApiImpl::HandleManualEnableErrorL( 
    TUint aServiceId, 
    TInt aEnableResult )
    {
    CCHUIDEBUG( "CCchUiApiImpl::HandleManualEnableErrorL - IN" );    
    CCHUIDEBUG2( "  ---> aEnableResult: %d", aEnableResult );
    
    MCchUiObserver::TCchUiDialogType dialogType = 
        MCchUiObserver::ECchUiDialogTypeNotSet;
    
    switch ( aEnableResult )
        {
        case KCCHErrorAccessPointNotDefined:
            {
            dialogType = MCchUiObserver::ECchUiDialogTypeNoConnectionDefined;
            }
            break;
        case KCCHErrorAuthenticationFailed:
            {
            dialogType = MCchUiObserver::ECchUiDialogTypeAuthenticationFailed;    
            }
            break;
        case KCCHErrorInvalidSettings:
            {
            dialogType = MCchUiObserver::ECchUiDialogTypeDefectiveSettings;            
            }
            break;
            
        case KCCHErrorBandwidthInsufficient:
            {
            dialogType = MCchUiObserver::ECchUiDialogTypeErrorInConnection; 
            }
            break;
        case KCCHErrorInvalidIap:
        case KCCHErrorNetworkLost:
        case KCCHErrorServiceNotResponding:
            {
            dialogType = MCchUiObserver::ECchUiDialogTypeNoConnectionAvailable;
            }
            break;           
        default:
            break;
        }
    
    // show dialog if dialog type set and dialog allowed
    if ( ( MCchUiObserver::ECchUiDialogTypeNotSet != dialogType ) &&
        ( DialogIsAllowed( dialogType ) ) )
        {
        ShowDialogL( aServiceId, dialogType );
        }
    
    CCHUIDEBUG( "CCchUiApiImpl::HandleManualEnableErrorL - OUT" );    
    }

// ---------------------------------------------------------------------------
// Handle dialog completed situation.
// ---------------------------------------------------------------------------
//
void CCchUiApiImpl::DialogCompletedL( 
    TInt aCompleteCode, 
    TCCHUiNotifierParams aResultParams )
    {
    CCHUIDEBUG( "CCchUiApiImpl::DialogCompletedL - IN");            
    CCHUIDEBUG2( "DialogCompletedL - complete code=%d", aCompleteCode );            
    CCHUIDEBUG2( "DialogCompletedL - dialog mode=%d", 
            aResultParams.iDialogMode  );        
    
    switch ( aResultParams.iDialogMode )
        {        
        case MCchUiObserver::ECchUiDialogTypeUsernamePasswordFailed:
            {
            // Show username/password query.
            __ASSERT_ALWAYS( DialogIsAllowed( 
                MCchUiObserver::ECchUiDialogTypeAuthenticationFailed ), 
                User::Leave( KErrNotSupported ) );
            
            CancelNotes();
            ShowDialogL( 
                aResultParams.iServiceId, 
                MCchUiObserver::ECchUiDialogTypeAuthenticationFailed );
            }
            break; 
        case MCchUiObserver::ECchUiDialogTypeAuthenticationFailed:
            {
            DoHandleAuthenticationFailedCompleteL( 
                aCompleteCode, aResultParams );
            }
            break;  
        case MCchUiObserver::ECchUiDialogTypeNoConnectionAvailable:
        case MCchUiObserver::ECchUiDialogTypeNoConnectionDefined:        
            {
            DoHandleNoConnectionsCompleteL( 
                aCompleteCode, aResultParams );
            }
            break;
        
        case MCchUiObserver::ECchUiDialogTypeDefectiveSettings:
            {
            // Defective settings is permanent type of condition.
            iCCHHandler->DisableL( aResultParams.iServiceId );
            iLastOperationResult = 
                MCchUiObserver::ECchUiClientOperationResultGeneralFailure;
            }
            break;           
        case MCchUiObserver::ECchUiDialogTypeErrorInConnection:
            {
            // Show confirm change connection query
            __ASSERT_ALWAYS( DialogIsAllowed( 
                MCchUiObserver::ECchUiDialogTypeConfirmChangeConnection ), 
                User::Leave( KErrNotSupported ) );
                        
            CancelNotes();
            ShowDialogL( 
                aResultParams.iServiceId, 
                MCchUiObserver::ECchUiDialogTypeConfirmChangeConnection );
            }
            break;
        case MCchUiObserver::ECchUiDialogTypeConfirmChangeConnection:
            {
            DoHandleConfirmChangeConnectionCompleteL( 
                aCompleteCode, aResultParams );
            }
            break;
        case MCchUiObserver::ECchUiDialogTypeChangeConnection:
            {
            DoHandleChangeConnectionCompleteL( 
                aCompleteCode, aResultParams );
            }
            break;            
        default:
            CCHUIDEBUG( "DialogCompletedL - default switch case" );  
            break;    
        }    
    
    CCHUIDEBUG( "CCchUiApiImpl::DialogCompletedL - OUT" );  
    }

// ---------------------------------------------------------------------------
// Checks if specific note is allowed
// ---------------------------------------------------------------------------
//
TBool CCchUiApiImpl::DialogIsAllowed( 
    MCchUiObserver::TCchUiDialogType aDialog )
    {
    CCHUIDEBUG( "CCchUiApiImpl::DialogIsAllowed");
    CCHUIDEBUG2( "CCchUiApiImpl::DialogIsAllowed - allowed notes count=%d", 
        iAllowedNotes.Count() );
    CCHUIDEBUG2( "CCchUiApiImpl::DialogIsAllowed - aDialog find result=%d", 
        iAllowedNotes.Find( aDialog ) );
    
    TBool ret = EFalse;
    if ( !iAllowedNotes.Count() ) return ETrue; // not configured, all allowed
    else if ( KErrNotFound != iAllowedNotes.Find( 
        MCchUiObserver::ECchUiDialogTypeNotSet ) ) return ETrue;
    else if ( KErrNotFound != iAllowedNotes.Find( aDialog ) ) return ETrue;
    
    CCHUIDEBUG2( "DialogIsAllowed - return: %d", ret );
    return ret;            
    }

// ---------------------------------------------------------------------------
// Checks if specific sub service  is allowed
// ---------------------------------------------------------------------------
//
TBool CCchUiApiImpl::SubServiceIsAllowed( TCCHSubserviceType aSubService )
    {
    CCHUIDEBUG( "CCchUiApiImpl::SubServiceIsAllowed");            
    
    CCHUIDEBUG2( "SubServiceIsAllowed - allowed subserv=%d", 
        iAllowedSubServices.Count() );
    CCHUIDEBUG2( "SubServiceIsAllowed - aSubService find=%d", 
        iAllowedSubServices.Find( aSubService ) );
    CCHUIDEBUG2( "SubServiceIsAllowed - all supported=%d", 
        iAllowedSubServices.Find( ECCHUnknown ) );
    
    // If subservice type is ECCHUnknown -> subservice is allowed, other
    // types are checked from array.
    TBool allowed = EFalse;
    TInt result = iAllowedSubServices.Find( aSubService );
    
    if ( ECCHUnknown == aSubService )
        {
        allowed = ETrue;
        }
    else if ( KErrNotFound != result )
        {
        allowed = ETrue;
        }
    else
        {
        allowed = EFalse;
        }

    CCHUIDEBUG2( "SubServiceIsAllowed - return: %d", allowed );
    return allowed;
    }

// ---------------------------------------------------------------------------
// Checks subservice state is allowed for showing dialogs.
// ---------------------------------------------------------------------------
//
TBool CCchUiApiImpl::StateIsAllowed( const TCCHSubserviceState aState )
    {
    CCHUIDEBUG( "CCchUiApiImpl::StateIsAllowed");  
    
    TBool allowed( EFalse );
    
    switch ( aState )
        {
        case ECCHUninitialized:
        case ECCHDisabled:
        case ECCHDisconnecting:
            {
            CCHUIDEBUG( "CCchUiApiImpl::StateIsAllowed: Not allowed");
            }
            break;           
        case ECCHConnecting:
        case ECCHEnabled:
            {
            CCHUIDEBUG( "CCchUiApiImpl::StateIsAllowed: Allowed");
            allowed = ETrue;
            }
            break;
        default:
            CCHUIDEBUG( "CCchUiApiImpl::StateIsAllowed: DEFAULT case");
            break;
        }
    
    return allowed;
    }

// ---------------------------------------------------------------------------
// Handles re-enabling of service.
// ---------------------------------------------------------------------------
//
void CCchUiApiImpl::HandleServiceReEnablingL( TUint aServiceId )
    {
    if ( iRemovableConnection )
        {
        CCHUIDEBUG( 
            "DoHandleServiceEventL - removing old connection" );
        
        RBuf serviceName;
        CleanupClosePushL( serviceName );
        serviceName.CreateL( KServiceNameMaxLength );
        
        iSpsHandler->ServiceNameL( 
                aServiceId,
                serviceName );
        
        // Remove old connection
        iConnectionHandler->RemoveConnectionL( 
            serviceName, iRemovableConnection );
        
        CleanupStack::PopAndDestroy( &serviceName );
        }
    
    CCHUIDEBUG( "HandleServiceReEnablingL - Re-Enabling service" );
    
    iReEnableService = EFalse;
    iCCHHandler->EnableL( aServiceId );    
    }

// ---------------------------------------------------------------------------
// Handle authentication failed completed situation.
// ---------------------------------------------------------------------------
//
void CCchUiApiImpl::DoHandleAuthenticationFailedCompleteL( 
    TInt aCompleteCode, 
    TCCHUiNotifierParams aResultParams )
    {
    CCHUIDEBUG( "CCchUiApiImpl::DoHandleAuthenticationFailedCompleteL - IN");            
        
    switch ( aCompleteCode )
        {
        case KErrNone:
            {
            TInt err( KErrNone );
                    
            // Set username
            iCCHHandler->SetUsernameL( aResultParams, err );   
                    
            // If no error set also password
            if ( !err )
                {       
                iCCHHandler->SetPasswordL( aResultParams, err );
                }
            else
                {
                iLastOperationResult = 
                    MCchUiObserver::ECchUiClientOperationResultGeneralFailure;
                }
                    
            // Both username and password set succesfully.
            if ( !err )
                {   
                iLastOperationResult = 
                MCchUiObserver::ECchUiClientOperationResultCredentialsChanged;
                }
            else
                {
                iLastOperationResult = 
                    MCchUiObserver::ECchUiClientOperationResultGeneralFailure;
                }
            
            aCompleteCode = err;
            break;
            }
        case KErrAlreadyExists:
            {
            // dialog active, do nothing
            break;
            }
        case KErrCancel:
            {        
            iLastOperationResult = 
                MCchUiObserver::ECchUiClientOperationResultUserCancelled;
            break;
            }
        default:
            {
            CCHUIDEBUG( 
               "DoHandleAuthenticationFailedCompleteL - switch default case");
            iLastOperationResult = 
                MCchUiObserver::ECchUiClientOperationResultGeneralFailure;
            break;
            }
        }
    
    switch ( iLastOperationResult )
        {
        case MCchUiObserver::ECchUiClientOperationResultCredentialsChanged:
            {
            iCCHHandler->EnableL( aResultParams.iServiceId );
            break;
            }
        default:
            {
            iCCHHandler->DisableL( aResultParams.iServiceId );
            break;
            }
        }
    
    CCHUIDEBUG( "CCchUiApiImpl::DoHandleAuthenticationFailedCompleteL - OUT");       
    }
    

// ---------------------------------------------------------------------------
// Handle no connections complete situation.
// ---------------------------------------------------------------------------
//
void CCchUiApiImpl::DoHandleNoConnectionsCompleteL( 
    TInt aCompleteCode, 
    TCCHUiNotifierParams aResultParams )
    {
    CCHUIDEBUG( "CCchUiApiImpl::DoHandleNoConnectionsCompleteL - IN"); 

    if ( KErrNone == aCompleteCode )
        {
        switch( aResultParams.iOperationCommand )
            {
            case ECchUiCommandSearchWlan:
                {
                HandleSearchWlanCompleteL( 
                        aCompleteCode, aResultParams );
                break;
                }
            case ECchUiCommandCopyGprs:
                {
                HandleCopyGprsCompleteL( 
                        aCompleteCode, aResultParams );                
                break;
                }
            case ECchUiCommandGprsNotFound:
                {
                CCHUIDEBUG( " -> GPRS was not found"); 
                iLastOperationResult = 
                    MCchUiObserver::ECchUiClientOperationResultGeneralFailure;
                break;
                }    
            case ECchUiCommandEnableCancelled:
                {
                iLastOperationResult = 
                    MCchUiObserver::ECchUiClientOperationResultUserCancelled;
                break;
                }
            case ECchUiCommandConnectWhenAvailable:
                {
                iLastOperationResult = 
                    MCchUiObserver::ECchUiClientOperationResultNotSet;
                break;
                }
            default:
                {
                CCHUIDEBUG( " -> !DEFAULT!"); 
                iLastOperationResult = 
                MCchUiObserver::ECchUiClientOperationResultGeneralFailure;
                }
                break;
            }
        }
    else if ( KErrAlreadyExists == aCompleteCode )
        {
        // dialog active, do nothing
        }
    else if ( KErrCancel == aCompleteCode )
        {
        iLastOperationResult = 
            MCchUiObserver::ECchUiClientOperationResultUserCancelled;
        }
    else
        {
        iLastOperationResult = 
            MCchUiObserver::ECchUiClientOperationResultGeneralFailure;
        }
    
    switch ( iLastOperationResult )
        {
        case MCchUiObserver::ECchUiClientOperationResultAccessPointAdded:
            {
            // New access point was added --> enable service
            iCCHHandler->EnableL( aResultParams.iServiceId );
            break;
            }
        case MCchUiObserver::ECchUiClientOperationResultNotSet:
            {
            // If no operation result set, do nothing
            break;
            }
        // By default disable service
        default:
            {
            CCHUIDEBUG( " -> !DEFAULT!");
            CCHUIDEBUG( " -> Disabling service");
            iCCHHandler->DisableL( aResultParams.iServiceId );
            break;
            }
        }       
    
    CCHUIDEBUG( "CCchUiApiImpl::DoHandleNoConnectionsCompleteL - OUT"); 
    }
  
// ---------------------------------------------------------------------------
// Handle change connection confirmation note complete situation.
// ---------------------------------------------------------------------------
//
void CCchUiApiImpl::DoHandleConfirmChangeConnectionCompleteL( 
    TInt aCompleteCode, 
    TCCHUiNotifierParams aResultParams )
    {    
    CCHUIDEBUG( 
        "CCchUiApiImpl::DoHandleConfirmChangeConnectionCompleteL - IN"); 
    
    switch( aCompleteCode )
        {
        case KErrNone:
            {            
            // show change connection query
            __ASSERT_ALWAYS( DialogIsAllowed( 
                MCchUiObserver::ECchUiDialogTypeChangeConnection ), 
                User::Leave( KErrNotSupported ) );
            
            // Set removable connection which is removed when connection
            // is changed successfully
            if ( aResultParams.iRemoveOldConnection )
                {
                iRemovableConnection = aResultParams.iCurrentConnectionIapId;
                }
            
            CancelNotes();
            ShowDialogL( 
                aResultParams.iServiceId, 
                MCchUiObserver::ECchUiDialogTypeChangeConnection );
            break;
            }
        case KErrCancel:
            {      
            iLastOperationResult = 
                MCchUiObserver::ECchUiClientOperationResultUserCancelled;
            break;
            }
        default:
            {
            CCHUIDEBUG( 
            "DoHandleConfirmChangeConnectionCompleteL - default switch case");
            
            iLastOperationResult = 
                MCchUiObserver::ECchUiClientOperationResultGeneralFailure;
            break;
            }
        }
    
    CCHUIDEBUG( 
            "CCchUiApiImpl::DoHandleConfirmChangeConnectionCompleteL - OUT"); 
    }

// ---------------------------------------------------------------------------
// Handle change connection query complete situation.
// ---------------------------------------------------------------------------
//
void CCchUiApiImpl::DoHandleChangeConnectionCompleteL( 
    TInt aCompleteCode, 
    TCCHUiNotifierParams aResultParams )
    {  
    CCHUIDEBUG( "CCchUiApiImpl::DoHandleChangeConnectionCompleteL - IN"); 
        
    switch( aCompleteCode )
        {
        case KErrNone:
            {
            HandleChangeConnectionL( 
                aCompleteCode, aResultParams );
            break;
            }
        case KErrCancel:
            {
            iLastOperationResult = 
                MCchUiObserver::ECchUiClientOperationResultUserCancelled;
            break;
            }
        default:
            {
            CCHUIDEBUG( 
                "DoHandleChangeConnectionCompleteL - default switch case"); 
            
            iLastOperationResult = 
                MCchUiObserver::ECchUiClientOperationResultGeneralFailure;
            
            break;
            }
        }
    
    CCHUIDEBUG( "CCchUiApiImpl::DoHandleChangeConnectionCompleteL - OUT"); 
    }

// ---------------------------------------------------------------------------
// Handles search wlan dialog result.
// ---------------------------------------------------------------------------
//    
void CCchUiApiImpl::HandleSearchWlanCompleteL( 
    TInt& aCompleteCode, 
    TCCHUiNotifierParams& aResultParams )
    {
    CCHUIDEBUG( "CCchUiApiImpl::HandleSearchWlanCompleteL - IN"); 
    
    CCHUIDEBUG( "HandleSearchWlanCompleteL - begin wlan search"); 
    
    TInt err( KErrNone );
    TInt snapId( KErrNone );
    
    iCCHHandler->GetConnectionSnapIdL( 
        aResultParams.iServiceId, snapId, err );
                    
    if ( !err )
        {
        CCHUIDEBUG2( 
            "HandleSearchWlanCompleteL - service snap is: %d", snapId ); 
                        
        HBufC* serviceName = HBufC::NewLC( KServiceNameMaxLength );
        TPtr serviceNamePtr( serviceName->Des() );
        
        CCHUIDEBUG( "HandleSearchWlanCompleteL - get service name");    
                        
        iSpsHandler->ServiceNameL( 
            aResultParams.iServiceId, serviceNamePtr );
                        
        CCHUIDEBUG( "HandleSearchWlanCompleteL - proceed to wlan search");                 
                        
        TRAP( err, iConnectionHandler->SearchAccessPointsL(
                aResultParams.iServiceId,
                snapId,
                serviceNamePtr ) ); 
                        
        CCHUIDEBUG2( "HandleSearchWlanCompleteL - wlan search err: %d", err );          
                        
        CleanupStack::PopAndDestroy( serviceName );
        
        HandleSearchAccessPointsErrorL( 
                err, aCompleteCode, aResultParams );
        }
    
    CCHUIDEBUG( "CCchUiApiImpl::HandleSearchWlanCompleteL - OUT"); 
    }
    
// ---------------------------------------------------------------------------
// Handles search access points error code.
// ---------------------------------------------------------------------------
//
void CCchUiApiImpl::HandleSearchAccessPointsErrorL( 
    TInt aErr,
    TInt& aCompleteCode, 
    TCCHUiNotifierParams& /*aResultParams*/ )
    {
    CCHUIDEBUG( "CCchUiApiImpl::HandleSearchAccessPointsErrorL - IN" );
    
    switch ( aErr )
        {
        case KErrNone:
            {
            CCHUIDEBUG( 
                "HandleSearchAccessPointsErrorL - access point added" );             
            iLastOperationResult = 
                MCchUiObserver::ECchUiClientOperationResultAccessPointAdded;
            break;
            }
        case KErrCancel: // user cancel
            {
            CCHUIDEBUG( "HandleSearchAccessPointsErrorL - error, cancelled");           
            iLastOperationResult = 
                MCchUiObserver::ECchUiClientOperationResultUserCancelled;
            break;
            }
        default: // other errors
            {
            CCHUIDEBUG( 
              "HandleSearchAccessPointsErrorL - error, set general failure");
               
            aCompleteCode = aErr;
            iLastOperationResult = 
                MCchUiObserver::ECchUiClientOperationResultGeneralFailure;
            }
        }
    
    CCHUIDEBUG( "CCchUiApiImpl::HandleSearchAccessPointsErrorL - OUT" );
    }

// ---------------------------------------------------------------------------
// Handles search wlan dialog result when use gprs selected.
// ---------------------------------------------------------------------------
//    
void CCchUiApiImpl::HandleCopyGprsCompleteL( 
    TInt& /*aCompleteCode*/, 
    TCCHUiNotifierParams& aResultParams )
    {
    CCHUIDEBUG( "CCchUiApiImpl::HandleCopyGprsCompleteL - IN" );
    
    // User selected gprs access point which will now be stored in
    // service snap
    TInt snapId( KErrNone );
                    
    HBufC* serviceName = HBufC::NewLC( KServiceNameMaxLength );
    TPtr serviceNamePtr( serviceName->Des() );
    
    iSpsHandler->ServiceNameL( 
            aResultParams.iServiceId,
            serviceNamePtr );
                
    TInt err( KErrNone );
    iCCHHandler->GetConnectionSnapIdL( 
            aResultParams.iServiceId, 
            snapId,
            err );
                    
    if ( !err )
        {
        CCHUIDEBUG2( 
            "HandleCopyGprsCompleteL - proceed, target snap is: %d", snapId );        
        CCHUIDEBUG2( 
            "HandleCopyGprsCompleteL - proceed, service id is: %d",
            aResultParams.iServiceId ); 
        CCHUIDEBUG2( "HandleCopyGprsCompleteL - gprs iap id is: %d", 
            aResultParams.iGprsIapId );                 
                        
        iConnectionHandler->CopyIapToServiceSnapL( 
                aResultParams.iServiceId, serviceNamePtr, 
                aResultParams.iGprsIapId, snapId );
                        
        CCHUIDEBUG( "HandleCopyGprsCompleteL - copy gprs iap done ok");                 
                        
        CleanupStack::PopAndDestroy( serviceName );
                        
        iLastOperationResult = 
            MCchUiObserver::ECchUiClientOperationResultAccessPointAdded;
                       
        CCHUIDEBUG( "  -> copy gprs iap done ok, enable");                 
        iCCHHandler->EnableL( aResultParams.iServiceId );
        }
    else
        {
        iLastOperationResult = 
            MCchUiObserver::ECchUiClientOperationResultGeneralFailure;
        User::Leave( err );
        }
    
    CCHUIDEBUG( "CCchUiApiImpl::HandleCopyGprsCompleteL - OUT" );
    }

// ---------------------------------------------------------------------------
// Handles changing to new connection.
// ---------------------------------------------------------------------------
//    
void CCchUiApiImpl::HandleChangeConnectionL( 
    TInt& aCompleteCode, 
    TCCHUiNotifierParams& aResultParams )
    {
    CCHUIDEBUG( "CCchUiApiImpl::HandleChangeConnectionL - IN" );        
    
    switch ( aResultParams.iOperationCommand )
        {
        case ECchUiCommandSearchWlan:
            {
            CCHUIDEBUG( "HandleChangeConnectionL - search wlan");                 
            HandleSearchWlanCompleteL( 
                aCompleteCode, aResultParams );
            break;
            }
        case ECchUiCommandCopyGprs:
            {
            CCHUIDEBUG( "HandleChangeConnectionL - copy gprs");       
            HandleCopyGprsCompleteL( 
                aCompleteCode, aResultParams );
            break;
            }
        default:
            {
            CCHUIDEBUG( "HandleChangeConnectionL - default switch case");
            iLastOperationResult = 
                MCchUiObserver::ECchUiClientOperationResultGeneralFailure;
            break;
            }
        }
    
    switch ( iLastOperationResult )
        {
        case MCchUiObserver::ECchUiClientOperationResultAccessPointAdded:
            {
            iLastOperationResult = 
                MCchUiObserver::ECchUiClientOperationResultConnectionChanged;
                            
            CCHUIDEBUG( "HandleChangeConnectionL - re-enable service");
                            
            // Disable service and set iReEnable flag so that after service
            // has disconnected we can connect with new access point.
            iCCHHandler->StartObservingL( aResultParams.iServiceId, *this );
            
            iReEnableService = ETrue;
                
            iObservedService = aResultParams.iServiceId;
            
            InformObserversL( aResultParams.iServiceId );
            iCCHHandler->DisableL( aResultParams.iServiceId );
                            
            // Check if id already exists in observed services
            TInt error = iObservervedServices.Find( 
                aResultParams.iServiceId );
                            
            // Append id only if it is not found.
            if ( KErrNotFound == error )
                {
                CCHUIDEBUG( 
                   "HandleChangeConnectionL add to observerd services" );
                iObservervedServices.AppendL( aResultParams.iServiceId );
                }           
            break;
            }
        default:
            break;
        }
    
    CCHUIDEBUG( "CCchUiApiImpl::HandleChangeConnectionL - OUT" );
    }

// ---------------------------------------------------------------------------
// Observer implementation for service events.
// ---------------------------------------------------------------------------
//    
void CCchUiApiImpl::ServiceStatusChanged( 
    TInt aServiceId, 
    TCCHSubserviceType aType, 
    const TCchServiceStatus& aServiceStatus )
    {
    CCHUIDEBUG( "CCchUiApiImpl::ServiceStatusChanged - IN");
    
    CCHUIDEBUG2( "ServiceStatusChanged - aServiceId: %d", 
        aServiceId ); 
    CCHUIDEBUG2( "ServiceStatusChanged - aError: %d", 
        aServiceStatus.Error() ); 
    CCHUIDEBUG2( "ServiceStatusChanged - aState: %d",
        aServiceStatus.State() ); 
    CCHUIDEBUG2( "ServiceStatusChanged - aType: %d", 
        aType ); 
    
    TInt observedPos = iObservervedServices.Find( aServiceId );
    
    CCHUIDEBUG2( "  ->  observed position: %d", observedPos ); 
    
    if ( KErrNotFound != observedPos )
        {
        switch ( aServiceStatus.Error() )
            {
            case KErrNone:
                {
                TRAP_IGNORE( DoHandleServiceEventL( 
                        aServiceId, aType, aServiceStatus ) );
                break;
                }
            default: // all error codes but KErrNone
                {
                TRAP_IGNORE( DoHandleServiceErrorL( 
                        aServiceId, aType, aServiceStatus ) );
                break;
                }
            }
        }
    
    CCHUIDEBUG( "CCchUiApiImpl::ServiceStatusChanged - OUT");
    }    
    
// ---------------------------------------------------------------------------
// Observer implementation for service events.
// ---------------------------------------------------------------------------
//    
void CCchUiApiImpl::DoHandleServiceErrorL( 
    TInt aServiceId, 
    TCCHSubserviceType aType, 
    const TCchServiceStatus& aServiceStatus )
    {
    CCHUIDEBUG( "CCchUiApiImpl::DoHandleServiceErrorL - IN"); 
    
    CCHUIDEBUG2( "DoHandleServiceErrorL - aServiceId: %d", 
        aServiceId ); 
    CCHUIDEBUG2( "DoHandleServiceErrorL - aError: %d", 
        aServiceStatus.Error() );
    CCHUIDEBUG2( "DoHandleServiceErrorL - aState: %d", 
        aServiceStatus.State() ); 

    // Check that sub service is allowed
    __ASSERT_ALWAYS( SubServiceIsAllowed( aType ), 
        User::Leave( KErrNotSupported ) );
    
    // Double-check subservice state
    TInt err( KErrNone );
    TCCHSubserviceState state = iCCHHandler->GetServiceStateL( 
        aServiceId, aType, err );
    
    if ( err )
        {
        User::Leave( err );
        }
    
    // Check that dialog showing is allowed.
    __ASSERT_ALWAYS( StateIsAllowed( state ) || 
            KCCHErrorInvalidSettings == aServiceStatus.Error(),
        User::Leave( KErrNotSupported ) );
    
    switch ( aServiceStatus.Error() )
        {
        case KCCHErrorAuthenticationFailed:
            {
            __ASSERT_ALWAYS( DialogIsAllowed( 
                MCchUiObserver::ECchUiDialogTypeUsernamePasswordFailed ),
                User::Leave( KErrNotSupported ) );
                         
            ShowDialogL( 
                aServiceId, 
                MCchUiObserver::ECchUiDialogTypeUsernamePasswordFailed );
            }
            break;
        
        case KCCHErrorInvalidSettings:
            {
            __ASSERT_ALWAYS( DialogIsAllowed( 
                MCchUiObserver::ECchUiDialogTypeDefectiveSettings ), 
                User::Leave( KErrNotSupported ) );

            ShowDialogL( 
                aServiceId, 
                MCchUiObserver::ECchUiDialogTypeDefectiveSettings );
            }
            break;
        
        case KCCHErrorInvalidIap:
        case KCCHErrorNetworkLost:
        case KCCHErrorServiceNotResponding:
            {
            __ASSERT_ALWAYS( DialogIsAllowed( 
                MCchUiObserver::ECchUiDialogTypeNoConnectionAvailable ), 
                User::Leave( KErrNotSupported ) );
            
            ShowDialogL( 
                aServiceId, 
                MCchUiObserver::ECchUiDialogTypeNoConnectionAvailable );
            }
            break;
        
        case KCCHErrorAccessPointNotDefined:
            {
            __ASSERT_ALWAYS( DialogIsAllowed( 
                MCchUiObserver::ECchUiDialogTypeNoConnectionDefined ), 
                User::Leave( KErrNotSupported ) );
            
            ShowDialogL(
                aServiceId, 
                MCchUiObserver::ECchUiDialogTypeNoConnectionDefined );
            }
            break;
        case KCCHErrorBandwidthInsufficient:
            {
            __ASSERT_ALWAYS( DialogIsAllowed( 
                MCchUiObserver::ECchUiDialogTypeErrorInConnection ), 
                User::Leave( KErrNotSupported ) );
                  
            ShowDialogL( 
                aServiceId, 
                MCchUiObserver::ECchUiDialogTypeErrorInConnection );
            }
            break;
            
        case KErrCancel:
            {
            CCHUIDEBUG( "CCchUiApiImpl::DoHandleServiceErrorL - cancelled"); 
            CancelNotes();
            }
            break;            
            
        default:
            break;
        }
    
    CCHUIDEBUG( "CCchUiApiImpl::DoHandleServiceErrorL - OUT"); 
    }
    
// ---------------------------------------------------------------------------
// Observer implementation for service events.
// ---------------------------------------------------------------------------
//    
void CCchUiApiImpl::DoHandleServiceEventL( 
    TInt aServiceId, 
    TCCHSubserviceType aType, 
    const TCchServiceStatus& aServiceStatus )
    {
    CCHUIDEBUG( "CCchUiApiImpl::DoHandleServiceEventL - IN" );
    
    CCHUIDEBUG2( "DoHandleServiceEventL - aServiceId:   %d", 
        aServiceId ); 
    CCHUIDEBUG2( "DoHandleServiceEventL - aError:       %d", 
        aServiceStatus.Error() ); 
    CCHUIDEBUG2( "DoHandleServiceEventL - aState:       %d", 
        aServiceStatus.State() );
    CCHUIDEBUG2( "DoHandleServiceEventL - aType:        %d", 
        aType );
    
    // Check that sub service is allowed
    __ASSERT_ALWAYS( SubServiceIsAllowed( aType ), 
        User::Leave( KErrNotSupported ) );
    
    switch ( aServiceStatus.State() )
        {
        case ECCHDisabled:
            {
            CCHUIDEBUG( "DoHandleServiceEventL - disabled event" );
            
            // Check if all subservices are disabled
            TBool allSubserviceDisabled = 
                iCCHHandler->AllSubservicesInStateL( aServiceId, ECCHDisabled );
            
            CCHUIDEBUG2( "DoHandleServiceEventL - all disabled=%d", 
                allSubserviceDisabled );
            
            // If all subservices are disabled stop observing service events.
            if ( allSubserviceDisabled )
                {
                TInt observedPos = iObservervedServices.Find( aServiceId );
                                 
                if ( KErrNotFound != observedPos )
                    {
                    CCHUIDEBUG( 
                        "DoHandleServiceEventL - removed observed service" );
                    iObservervedServices.Remove( observedPos );    
                    }
                                                                                        
                CCHUIDEBUG( "DoHandleServiceEventL - stop observing" );                              
                iCCHHandler->StopObservingL( aServiceId, *this );
                iObservedService = 0;
                }
             
            if ( iReEnableService )
                {
                if ( allSubserviceDisabled )
                    {
                    CCHUIDEBUG( "DoHandleServiceEventL - Re-Enable service" );
                    HandleServiceReEnablingL( aServiceId );
                    }
                else
                    {
                    // do nothing and wait next disabled event
                    // to see if then all subservices are disabled
                    }
                }
            else if ( allSubserviceDisabled && !iReEnableService )
                {
                CCHUIDEBUG( "DoHandleServiceEventL - all subservices disabled" );
                                
                // Service is in disabled state. Inform cchui observers.
                InformObserversL( aServiceId );
                
                iLastOperationResult = 
                    MCchUiObserver::ECchUiClientOperationResultNotSet;
                }
            else
                {
                CCHUIDEBUG( "DoHandleServiceEventL - all subservices not disabled" );
                // not all subservices are disable, do nothing
                }
            }
            break;
            
        case ECCHEnabled:
            {      
            if ( iReEnableService && iCCHHandler->AllSubservicesInStateL( aServiceId, ECCHEnabled ) )
                {
                CCHUIDEBUG( "DoHandleServiceEventL - reset re-enabling" );
                // Just in case set re-enable flag to EFalse
                iReEnableService = EFalse;
                }
            
            CCHUIDEBUG( "DoHandleServiceEventL - service is enabled" );
            
            // Cancel notes and stop observing only if no active dialog exists
            // which needs user action
            if ( !iNoteController->ActiveDialogExists() && !iReEnableService )
                {              
                CancelNotes(); 
                
                TInt observedPos = iObservervedServices.Find( aServiceId );
                if ( KErrNotFound != observedPos )
                    {
                    CCHUIDEBUG( 
                        "DoHandleServiceEventL - removed observed service" );
                    iObservervedServices.Remove( observedPos );    
                    }
                         
                CCHUIDEBUG( "DoHandleServiceEventL - stop observing" );              
                iCCHHandler->StopObservingL( aServiceId, *this );
                iObservedService = 0;
                
                // Service is in enabled state. Inform cchui observers.
                InformObserversL( aServiceId );
                
                iLastOperationResult = 
                    MCchUiObserver::ECchUiClientOperationResultNotSet;
                } 
            
            if ( iNoteController && 
                MCchUiObserver::ECchUiDialogTypeNoConnectionAvailable ==
                iNoteController->CurrentNote() )
                {
                CCHUIDEBUG( "DoHandleServiceEventL - cancel notifier" );
                // Connection enabled again so this notify can be canceled.
                CancelNotes();
                }
            }
            break;     
        default:
            CCHUIDEBUG( "DoHandleServiceEventL - default switch case" );
            break;
        }
    
    CCHUIDEBUG( "CCchUiApiImpl::DoHandleServiceEventL - OUT" );
    }
    
// ---------------------------------------------------------------------------
// Informs observers about cch ui results.
// ---------------------------------------------------------------------------
//
void CCchUiApiImpl::InformObserversL( TInt aServiceId )
    {
    CCHUIDEBUG( "CCchUiApiImpl::InformObserversL - IN"); 

    for ( TInt index = 0 ; index < iObservers.Count() ; index++ )
        {    
        CCHUIDEBUG2( "InformObserversL - aServiceId: %d", aServiceId ); 
        CCHUIDEBUG2( "InformObserversL - iLastOperationResult: %d", 
            iLastOperationResult ); 
        
        iObservers[index]->Observer().ConnectivityDialogsCompletedL( 
            aServiceId,
            iLastOperationResult );
        }
    
    CCHUIDEBUG( "CCchUiApiImpl::InformObserversL - OUT"); 
    }

