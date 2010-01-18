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
* Description:  CCCHPlugin implementation
*
*/


// INCLUDE FILES
#include "cchlogger.h"
#include "cchplugin.h"
#include "cchservice.h"
#include "cchservicenotifier.h"

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES
// None

// CONSTANTS
// None

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
// CCCHPlugin::CCCHPlugin
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CCCHPlugin::CCCHPlugin( TUid aUid ) :
    iUid( aUid )
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// CCCHPlugin::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CCCHPlugin::ConstructL( TUid aUid )
    {
    CCHLOGSTRING( "CCCHPlugin::ConstructL" );
    CCHLOGSTRING2( "    Uid:        0x%X", aUid );

    iPlugin = CCchService::NewL( aUid, *this );
    }

// ---------------------------------------------------------------------------
// CCCHPlugin::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CCCHPlugin* CCCHPlugin::NewL( TUid aUid )
    {
    CCCHPlugin* self = new (ELeave) CCCHPlugin( aUid );
    CleanupStack::PushL( self );
    self->ConstructL( aUid );
    CleanupStack::Pop( self );
    return self;
    }

// Destructor
CCCHPlugin::~CCCHPlugin()
    {
    iObservers.Reset();
    
    delete iPlugin;
    iPlugin = NULL;
    }

// ---------------------------------------------------------------------------
// CCCHPlugin::CheckUid
// 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//    
TBool CCCHPlugin::CheckUid( const TUid aUid ) const
    {
    return iUid == aUid;
    }
        
// ---------------------------------------------------------------------------
// CCCHPlugin::Plugin
// 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//    
CCchService* CCCHPlugin::Plugin() const
    {
    return iPlugin;
    }

// ---------------------------------------------------------------------------
// CCCHPlugin::SetServiceNotifier
// 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHPlugin::SetServiceNotifier( MCCHServiceNotifier* aObserver )
    {
    CCHLOGSTRING( "CCCHPlugin::SetServiceNotifier" );
    
    TInt index( iObservers.Find( aObserver ) );
        
    if ( KErrNotFound == index )
        {            
        iObservers.Append( aObserver );
        }
    }

// ---------------------------------------------------------------------------
// CCCHPlugin::RemoveServiceNotifier
// 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHPlugin::RemoveServiceNotifier( MCCHServiceNotifier* aObserver )
    {
    CCHLOGSTRING( "CCCHPlugin::RemoveServiceNotifier" );
    
    TInt index( iObservers.Find( aObserver ) );
        
    if ( KErrNotFound != index )
        {
        iObservers.Remove( index );
        iObservers.Compress();
        }
    }
        
// ---------------------------------------------------------------------------
// CCCHPlugin::ServiceStateChanged
// 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//    
void CCCHPlugin::ServiceStateChanged( 
    const TServiceSelection aServiceSelection,
    TCCHSubserviceState aState, TInt aError  )
    {
    CCHLOGSTRING( "CCCHPlugin::ServiceStateChanged" );
    CCHLOGSTRING2( "    Service Id:        %d", aServiceSelection.iServiceId );
    CCHLOGSTRING2( "    SubService's Type: %d", aServiceSelection.iType );
    CCHLOGSTRING2( "    State:             %d", aState );
    CCHLOGSTRING2( "    Error:             %d", aError );
    
    for ( TInt i( 0 ); i < iObservers.Count(); i++ )
        {
        if ( iObservers[ i ] )
            {
            TRAP_IGNORE(
                iObservers[ i ]->StateChangedL( 
                    aServiceSelection.iServiceId, 
                    aServiceSelection.iType, 
                    aState,
                    aError  ) );
            }
        }
    }
                                          
// ========================== OTHER EXPORTED FUNCTIONS =======================

//  End of File
