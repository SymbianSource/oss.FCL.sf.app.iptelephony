/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  For handling interactions betweed UI and CCH.
*
*/


#include <cch.h>

#include "cscenglogger.h"
#include "cscengcchhandler.h"
#include "mcscengcchobserver.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCEngCCHHandler::CCSCEngCCHHandler( MCSCEngCCHObserver& aObserver ) :
    iObserver( aObserver )
    {    
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CCSCEngCCHHandler::ConstructL()
    {
    CSCENGDEBUG( "CCSCEngCCHHandler::ConstructL - begin" ); 
    
    iCchClientApi = CCch::NewL();
    
    CSCENGDEBUG( "CCSCEngCCHHandler::ConstructL - end" ); 
    }   
    
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CCSCEngCCHHandler* CCSCEngCCHHandler::NewL( 
    MCSCEngCCHObserver& aObserver )
    {    
    CCSCEngCCHHandler* self = new ( ELeave ) CCSCEngCCHHandler( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCEngCCHHandler::~CCSCEngCCHHandler()
    {
    CSCENGDEBUG( "CCSCEngCCHHandler::~CCSCEngCCHHandler - begin" ); 
    
    delete iCchClientApi;    
    
    CSCENGDEBUG( "CCSCEngCCHHandler::~CCSCEngCCHHandler - end" ); 
    }


// ---------------------------------------------------------------------------
// Disable service.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCSCEngCCHHandler::DisableService( TInt aServiceId )
    {    
    CCchService* service = iCchClientApi->GetService( aServiceId );
    
    TInt err( KErrNone );
    if ( service )
        {
        service->SetObserver( *this );
        err = service->Disable( ECCHUnknown );
        }
    else
        {
        err = KErrArgument;
        }
    
    return err;
    }


// ---------------------------------------------------------------------------
// Check is service disabled
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CCSCEngCCHHandler::IsServiceDisabled( TInt aServiceId )
    {    
    TBool disabled( ETrue );

    if ( !IsDisabled( aServiceId, ECCHVoIPSub ) ) return EFalse;
    if ( !IsDisabled( aServiceId, ECCHPresenceSub ) ) return EFalse;
    if ( !IsDisabled( aServiceId, ECCHIMSub ) ) return EFalse;      
    if ( !IsDisabled( aServiceId, ECCHVMBxSub ) ) return EFalse;
        
    return disabled;
    }

// ---------------------------------------------------------------------------
// Check is service is valid
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CCSCEngCCHHandler::IsServiceValidL( TInt aServiceId )
    {    
    TBool validService( EFalse );
    
    TSupportedSubServices supportedSubServices;
    SupportedSubServicesL( aServiceId, supportedSubServices );

    // Service is valid if some subservice is supported
    if ( supportedSubServices.iVoIP || supportedSubServices.iIm ||
        supportedSubServices.iPresence || supportedSubServices.iVmbx )
        {
        validService = ETrue;
        }
    
    return validService;
    }

// ---------------------------------------------------------------------------
// Get supported subservices.
// ---------------------------------------------------------------------------
//
EXPORT_C void CCSCEngCCHHandler::SupportedSubServicesL( 
    TInt aServiceId, TSupportedSubServices& aSupSubServices )
    {
    CCchService* service = 
               iCchClientApi->GetService( aServiceId );
        
    TBool supported( EFalse );
    
    if ( service )
        {
        service->IsSupported( ECCHVoIPSub, supported );
        aSupSubServices.iVoIP = supported;
        
        service->IsSupported( ECCHPresenceSub, supported );
        aSupSubServices.iPresence = supported;
        
        service->IsSupported( ECCHIMSub, supported );
        aSupSubServices.iIm = supported;
        
        service->IsSupported( ECCHVMBxSub, supported );
        aSupSubServices.iVmbx = supported;
        }
        
    CSCENGDEBUG2( 
            "CCSCEngCCHHandler::SupportedSubServicesL VoIP=%d", 
                aSupSubServices.iVoIP );
    CSCENGDEBUG2( 
            "CCSCEngCCHHandler::SupportedSubServicesL Presence=%d", 
                aSupSubServices.iPresence );
    CSCENGDEBUG2( 
            "CCSCEngCCHHandler::SupportedSubServicesL IM=%d", 
                aSupSubServices.iIm );
    CSCENGDEBUG2( 
            "CCSCEngCCHHandler::SupportedSubServicesL VMBX=%d", 
                aSupSubServices.iVmbx );
    }

// ---------------------------------------------------------------------------
// Get cch connection parameter (RBuf).
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCSCEngCCHHandler::GetConnectionParameter( 
    TInt aServiceId, 
    TCchConnectionParameter aConnParam,
    RBuf& aConnParamValue )
    {   
    CSCENGDEBUG(  "CCSCEngCCHHandler::GetConnectionParameter" );
    
    CCchService* service = 
           iCchClientApi->GetService( aServiceId );

    TInt err( KErrNone );
    if ( service )
        {
        err = service->GetConnectionParameter( 
            ECCHUnknown, aConnParam, aConnParamValue );
        }
    else
        {
        err = KErrArgument;
        }
    
    return err;
    }


// ---------------------------------------------------------------------------
// Get cch connection parameter (TInt).
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCSCEngCCHHandler::GetConnectionParameter( 
    TInt aServiceId, 
    TCchConnectionParameter aConnParam,
    TInt& aConnParamValue )
    {   
    CSCENGDEBUG(  "CCSCEngCCHHandler::GetConnectionParameter" );
    
    CCchService* service = 
           iCchClientApi->GetService( aServiceId );

    
    TInt err( KErrNone );
    if ( service )
        {
        err = service->GetConnectionParameter( 
            ECCHUnknown, aConnParam, aConnParamValue );
        }
    else
        {
        err = KErrArgument;
        }
        
    return err;
    }


// ---------------------------------------------------------------------------
// Set cch connection parameter.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCSCEngCCHHandler::SetConnectionParameter( 
    TInt aServiceId,
    TCchConnectionParameter aConnParam,
    const TDesC& aConnParamValue )
    {       
    CSCENGDEBUG(  "CCSCEngCCHHandler::SetConnectionParameter" );
    
    CCchService* service = 
           iCchClientApi->GetService( aServiceId );
    
    TInt err( KErrNone );
    if ( service )
        {
        err =service->SetConnectionParameter( 
            ECCHUnknown, aConnParam, aConnParamValue );
        }
    else
        {
        err = KErrArgument;
        }
           
    return err;
    }

// ---------------------------------------------------------------------------
// CCSCEngCCHHandler::IsDisabled
// ---------------------------------------------------------------------------
//
TBool CCSCEngCCHHandler::IsDisabled( 
    TInt aServiceId, 
    TCCHSubserviceType aType  )                                     
    {                                                                                                          
    TBool disabled( EFalse );                                                                                   
    
    CCchService* service = 
          iCchClientApi->GetService( aServiceId );
    
    if ( service )
        {
        TCchServiceStatus status;
        TInt err = service->GetStatus( aType, status );
                                                                                                                                                                                       
        if ( KErrNotFound == err || 
             ( KErrNone == err && 
             ( ECCHUninitialized == status.State() || 
               ECCHDisabled == status.State() ) ) )
            {                                                                                                                                         
            disabled = ETrue;                                                                                                                        
            }          
        }

    return disabled;                                                                                                              
    }  
    
// ---------------------------------------------------------------------------
// From class MCchServiceStatusObserver
// ---------------------------------------------------------------------------
//
void CCSCEngCCHHandler::ServiceStatusChanged( 
    TInt aServiceId, 
    TCCHSubserviceType aType, 
    const TCchServiceStatus& aServiceStatus )                  
    {       
    CSCENGDEBUG(  "CCSCEngCCHHandler::ServiceStatusChanged" );
    
    if ( ECCHDisabled == aServiceStatus.State() )
        {
        CCchService* service = 
                  iCchClientApi->GetService( aServiceId );
        
        if ( service )
            {
            service->RemoveObserver();
            }
        }
    
    iObserver.ServiceStatusChanged(
            aServiceId, 
            aType, 
            aServiceStatus );                                                                                   
    }  
