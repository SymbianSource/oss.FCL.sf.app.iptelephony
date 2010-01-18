/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CCchStartupCounter implementation
*
*/


// INCLUDE FILES
#include "cchlogger.h"
#include "cchstartupcounter.h"
#include "cchserverbase.h"
#include "cchspshandler.h"
#include "cchservicehandler.h"
#include "cchprivatecrkeys.h"
#include <centralrepository.h>

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES
// None

// CONSTANTS
// Some critical process has crashed three times during startup registration
// to avoid bootlock situation set startup flag to OFF after max count
const TInt KCCHMaxStartupCount = 3;

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ============================= LOCAL FUNCTIONS =============================

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// CCchStartupCounter::CCchStartupCounter
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CCchStartupCounter::CCchStartupCounter( CCCHServerBase& aServer ) 
    : iServer( aServer )
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// CCchStartupCounter::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CCchStartupCounter::ConstructL()
    {
    iStartupRepository = CRepository::NewL( KCRUidCch );
    }

// ---------------------------------------------------------------------------
// CCchStartupCounter::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CCchStartupCounter* CCchStartupCounter::NewL( CCCHServerBase& aServer )
    {
    CCchStartupCounter* self = new (ELeave) CCchStartupCounter( aServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// Destructor
CCchStartupCounter::~CCchStartupCounter()
    {
    delete iStartupRepository;
    }
    
// ---------------------------------------------------------------------------
// CCchStartupCounter::StartupOccuredL
// 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCchStartupCounter::StartupOccuredL()
    {
    CCHLOGSTRING( "CCchStartupCounter::StartupOccuredL IN" );
    
    TInt startupCounter( KErrNone );
    User::LeaveIfError( iStartupRepository->Get( 
        KCCHStartupCounter, startupCounter ) );
            
    startupCounter++;
    CCHLOGSTRING2( 
        "CCchStartupCounter -> startup counter: %d", startupCounter );
            
    if ( startupCounter >= KCCHMaxStartupCount )
        {
        CCHLOGSTRING( "CCchStartupCounter -> set startup flag to OFF" );
        // Some critical process has crashed three times during startup registration..
        startupCounter = 0;
        // ..to avoid bootlock situation set startup flag to OFF
        HandleStartupExceptionsL(); 
        }
            
    User::LeaveIfError( iStartupRepository->Set( 
        KCCHStartupCounter, startupCounter ) );
                        
    CCHLOGSTRING( "CCchStartupCounter::StartupOccuredL OUT" );
    }
      
// ---------------------------------------------------------------------------
// CCchStartupCounter::HandleStartupExceptionsL
// 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCchStartupCounter::HandleStartupExceptionsL()
    {
    CCHLOGSTRING( "CCchStartupCounter::HandleStartupExceptionsL IN" );
    
    RArray<TUint> serviceIds;
    CleanupClosePushL( serviceIds );
    RArray<TCCHSubserviceType> subserviceTypes;
    CleanupClosePushL( subserviceTypes );
                
    iServer.SPSHandler().GetServiceIdsL( serviceIds );
    for ( TInt i( 0 ); i < serviceIds.Count(); i++ )
        {
        iServer.ServiceHandler().GetSubserviceTypesL( 
            serviceIds[ i ], subserviceTypes );
            
        for ( TInt j( 0 ); j < subserviceTypes.Count(); j++ )
            {
            iServer.ServiceHandler().SetStartupFlag( TServiceSelection( 
                serviceIds[ i ], subserviceTypes[ j ] ), EFalse );
            iServer.SPSHandler().SetLoadAtStartUpL(
                serviceIds[ i ], subserviceTypes[ j ], EFalse );
            }
        }
    
    CleanupStack::PopAndDestroy( &subserviceTypes );
    CleanupStack::PopAndDestroy( &serviceIds ); 
            
    CCHLOGSTRING( "CCchStartupCounter::HandleStartupExceptionsL OUT" );
    }

// ---------------------------------------------------------------------------
// CCchStartupCounter::ResetStartupCounter
// 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCchStartupCounter::ResetStartupCounter()
    {
    CCHLOGSTRING( "CCchStartupCounter::ResetStartupCounter IN" );
    
    iStartupRepository->Set( 
        KCCHStartupCounter, KErrNone );
    
    CCHLOGSTRING( "CCchStartupCounter::ResetStartupCounter OUT" );
    }

// ========================== OTHER EXPORTED FUNCTIONS =======================

//  End of File
