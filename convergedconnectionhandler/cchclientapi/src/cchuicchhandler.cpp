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
* Description:  Provider access to CCH
*
*/


#include "cchuilogger.h"
#include "cchuicchhandler.h"


// ======== MEMBER FUNCTIONS ========

CCchUiCchHandler::CCchUiCchHandler( CCch& aCch ):
    iCchClientApi( aCch )
    {    
    }

CCchUiCchHandler* CCchUiCchHandler::NewL( CCch& aCch )
    {
    CCchUiCchHandler* self = 
           new ( ELeave ) CCchUiCchHandler( aCch );
    return self;
    }

CCchUiCchHandler::~CCchUiCchHandler()
    {
    CCHUIDEBUG( "CCchUiCchHandler::~CCchUiCchHandler - IN" );
    CCHUIDEBUG( "CCchUiCchHandler::~CCchUiCchHandler - OUT" ); 
    }

// ---------------------------------------------------------------------------
// Gets snap that service uses.
// ---------------------------------------------------------------------------
//
void CCchUiCchHandler::GetConnectionSnapIdL( 
    TInt aServiceId, TInt& aSnapId, TInt& aErr )
    {  
    CCHUIDEBUG( "CCchUiCchHandler::GetConnectionSnapIdL - IN" );   
    
    CCchService* service = 
        iCchClientApi.GetService( aServiceId );
        
    User::LeaveIfNull( service );
    
    aErr = service->GetConnectionParameter( 
        ECCHUnknown, ECchSnapId, aSnapId );
    
    CCHUIDEBUG( "CCchUiCchHandler::GetConnectionSnapIdL - OUT" );
    } 

// ---------------------------------------------------------------------------
// Sets snap for service.
// ---------------------------------------------------------------------------
//
void CCchUiCchHandler::SetConnectionSnapIdL( 
    TInt aServiceId, TInt aSnapId, TInt& aErr )
    {  
    CCHUIDEBUG( "CCchUiCchHandler::SetConnectionSnapIdL - IN" );    
    
    CCchService* service = iCchClientApi.GetService( aServiceId );
    User::LeaveIfNull( service );
    
    aErr = service->SetConnectionParameter( 
        ECCHUnknown, ECchSnapId, aSnapId );
    
    CCHUIDEBUG( "CCchUiCchHandler::SetConnectionSnapIdL - OUT" );  
    } 

// ---------------------------------------------------------------------------
// Gets service username.
// ---------------------------------------------------------------------------
//
void CCchUiCchHandler::GetUsernameL( 
    TInt aServiceId, RBuf& aUserName, TInt& aErr )
    {      
    CCHUIDEBUG( "CCchUiCchHandler::GetUsernameL - IN" );       
    
    CCchService* service = iCchClientApi.GetService( aServiceId );
    User::LeaveIfNull( service );

    aErr = service->GetConnectionParameter( 
            ECCHUnknown, ECchUsername, aUserName );
    
    CCHUIDEBUG( "CCchUiCchHandler::GetUsernameL - OUT" );       
    }

// ---------------------------------------------------------------------------
// Sets username for service.
// ---------------------------------------------------------------------------
//
void CCchUiCchHandler::SetUsernameL( 
    TCCHUiNotifierParams aResultParams, TInt& aErr )
    {      
    CCHUIDEBUG( "CCchUiCchHandler::SetUsernameL - IN" );       
    
    CCchService* service = 
        iCchClientApi.GetService( aResultParams.iServiceId );
    User::LeaveIfNull( service );

    aErr = service->SetConnectionParameter( 
        ECCHUnknown, ECchUsername, aResultParams.iUsername );
    
    CCHUIDEBUG( "CCchUiCchHandler::SetUsernameL - OUT" );     
    }
    
// ---------------------------------------------------------------------------
// Sets password for service.
// ---------------------------------------------------------------------------
//
void CCchUiCchHandler::SetPasswordL( 
    TCCHUiNotifierParams aResultParams, TInt& aErr )
    {         
    CCHUIDEBUG( "CCchUiCchHandler::SetPasswordL - IN" );                 
    
    CCchService* service = 
        iCchClientApi.GetService( aResultParams.iServiceId );
    User::LeaveIfNull( service );
    
    aErr = service->SetConnectionParameter( 
      ECCHUnknown, ECchPassword, aResultParams.iPwd );
    
    CCHUIDEBUG( "CCchUiCchHandler::SetPasswordL - OUT" );  
    }

// ---------------------------------------------------------------------------
// Starts observing for service events
// ---------------------------------------------------------------------------
//
void CCchUiCchHandler::StartObservingL( 
    TUint aServiceId, MCchServiceStatusObserver& aObserver )
    {         
    CCHUIDEBUG2( "CCchUiCchHandler::StartObservingL: service: %d", 
        aServiceId );                 
    CCchService* service = iCchClientApi.GetService( aServiceId );
    User::LeaveIfNull( service );
    
    TInt err = service->AddObserver( aObserver );
    
    if ( KErrNone != err && KErrAlreadyExists != err )
        {
        CCHUIDEBUG2( "CCchUiCchHandler::StartObservingL: ERR: %d", 
                err );        
        
        User::Leave( err );
        }
    }
    
// ---------------------------------------------------------------------------
// Enables service
// ---------------------------------------------------------------------------
//
void CCchUiCchHandler::EnableL( TUint aServiceId )
    {         
    CCHUIDEBUG( "CCchUiCchHandler::EnableL - IN" );    
    
    CCchService* service = iCchClientApi.GetService( aServiceId );
    User::LeaveIfNull( service );  
    service->Enable( ECCHUnknown );
    
    CCHUIDEBUG( "CCchUiCchHandler::EnableL - OUT" );                 
    }    
    
// ---------------------------------------------------------------------------
// Disables service
// ---------------------------------------------------------------------------
//
void CCchUiCchHandler::DisableL( TUint aServiceId )
    {
    CCHUIDEBUG( "CCchUiCchHandler::DisableL - IN" );     
    
    CCchService* service = iCchClientApi.GetService( aServiceId );
    User::LeaveIfNull( service );
    service->Disable( ECCHUnknown );
    
    CCHUIDEBUG( "CCchUiCchHandler::DisableL - OUT" );                 
    }    
    
// ---------------------------------------------------------------------------
// CCchUiCchHandler::AllSubservicesInStateL
// ---------------------------------------------------------------------------
//
TBool CCchUiCchHandler::AllSubservicesInStateL( 
    TUint aServiceId,
    TCCHSubserviceState aState )
    {         
    CCHUIDEBUG( "CCchUiCchHandler::AllSubservicesInStateL - IN" );
    CCHUIDEBUG2( "CCchUiCchHandler::AllSubservicesInStateL : %d", aState );
    
    CCchService* service = iCchClientApi.GetService( aServiceId );
    User::LeaveIfNull( service );
      
    RArray<TCCHSubserviceType> types;
    CleanupClosePushL( types );
    types.AppendL( ECCHVoIPSub );
    types.AppendL( ECCHPresenceSub );
    types.AppendL( ECCHIMSub );
    types.AppendL( ECCHVMBxSub );
    
    TBool response( ETrue ); 

    for ( TInt i( 0 ) ; i < types.Count() ; i++ )
        {        
        TBool supported( EFalse );
        TInt err = service->IsSupported( types[ i ], supported );
                
        if ( supported && !err )
            {            
            TCCHSubserviceState state = 
                GetServiceStateL( aServiceId, types[ i ], err );
            
            if ( aState != state && !err )
                {                
                response = EFalse;
                i = types.Count(); // End loop
                }
            }       
        }

    CleanupStack::PopAndDestroy( &types );
    
    CCHUIDEBUG2( "CCchUiCchHandler::AllSubservicesInStateL : %d", response );
    CCHUIDEBUG( "CCchUiCchHandler::AllSubservicesInStateL - OUT" );
    return response;
    }

// ---------------------------------------------------------------------------
// Stops observing for specific service
// ---------------------------------------------------------------------------
//
void CCchUiCchHandler::StopObservingL( 
    TUint aServiceId, MCchServiceStatusObserver& aObserver )
    {         
    CCHUIDEBUG( "CCchUiCchHandler::StopObservingL - IN" );   
    
    CCchService* service = iCchClientApi.GetService( aServiceId );
    User::LeaveIfNull( service );
      
    TInt err = service->RemoveObserver( aObserver );
    
    if ( KErrNone != err && KErrNotFound != err )
        {
        CCHUIDEBUG2( "CCchUiCchHandler::StopObservingL: ERR: %d", 
                       err );        
        
        User::Leave( err );
        }
    
    CCHUIDEBUG( "CCchUiCchHandler::StopObservingL - OUT");                          
    }    
    
// ---------------------------------------------------------------------------
//  For checking service state.
// ---------------------------------------------------------------------------
//
TCCHSubserviceState CCchUiCchHandler::GetServiceStateL( 
    TUint aServiceId, TCCHSubserviceType aSubServiceType, TInt& aErr )
    {         
    CCHUIDEBUG( "CCchUiCchHandler::GetServiceStateL"); 
    
    CCchService* service = iCchClientApi.GetService( aServiceId );
    User::LeaveIfNull( service );
    
    TCchServiceStatus serviceStatus;
    aErr = service->GetStatus( aSubServiceType, serviceStatus );
    
    CCHUIDEBUG2( "GetServiceStateL STATE=%d", serviceStatus.State() );
    
    return serviceStatus.State();
    }   

// ---------------------------------------------------------------------------
//  For getting current connections iap id.
// ---------------------------------------------------------------------------
//
void CCchUiCchHandler::GetCurrentConnectionIapIdL( 
    TUint aServiceId,
    TCCHSubserviceType aSubServiceType,
    TInt& aIapId,
    TInt& aErr )
    {     
    CCHUIDEBUG( "CCchUiCchHandler::GetCurrentConnectionIapIdL - IN" );         
    
    CCchService* service = iCchClientApi.GetService( aServiceId );
    User::LeaveIfNull( service );
    
    aErr = service->GetConnectionParameter(
            aSubServiceType,
            ECchIapId,
            aIapId );
    
    // Check that atleast some subservice is really in enabled state 
    //(connection exists), if not set iap id to 0
    if ( aIapId )
        {
        TBool enabledSubServiceFound( EFalse );
        
        RArray<TCCHSubserviceType> types;
        CleanupClosePushL( types );
        types.AppendL( ECCHVoIPSub );
        types.AppendL( ECCHPresenceSub );
        types.AppendL( ECCHIMSub );
        types.AppendL( ECCHVMBxSub );
        
        for ( TInt i( 0 ) ; i < types.Count() ; i++ )
            {
            TBool supported( EFalse );
            TInt err = service->IsSupported( types[ i ], supported );
            
            if ( supported && !err )
                {
                TCCHSubserviceState state = GetServiceStateL( 
                    aServiceId, types[ i ], err );
                
                if ( ECCHEnabled == state && !err )
                    {
                    // Enabled subservice found, end loop
                    enabledSubServiceFound = ETrue;
                    i = types.Count();
                    }              
                }       
            }
        
        // Set aIapId to zero if no subservices in enabled state
        if ( !enabledSubServiceFound )
            {
            aIapId = 0;
            }
        
        CleanupStack::PopAndDestroy( &types );
        }
    
    CCHUIDEBUG( "CCchUiCchHandler::GetCurrentConnectionIapIdL - OUT" );
    } 
