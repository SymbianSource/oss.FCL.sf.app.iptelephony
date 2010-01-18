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



#include "vcchopolicyreader.h"
#include "vccspsettings.h"
#include "rubydebug.h"

_LIT( KVccServiceName,               "VCC" );

// ======== MEMBER FUNCTIONS ========
// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CVccHoPolicyReader::CVccHoPolicyReader()
    : iPolicy()
    {
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CVccHoPolicyReader::ConstructL()
    {
    RUBY_DEBUG_BLOCKL( "ConstructL" );

    iSpSettings = CVccSPSettings::NewL();
    }
    
// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//          
CVccHoPolicyReader* CVccHoPolicyReader::NewL()
    {
    RUBY_DEBUG_BLOCKL( "NewL" );

    CVccHoPolicyReader* self = CVccHoPolicyReader::NewLC();
    CleanupStack::Pop( self );

    return self;
    }
    
// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//  
CVccHoPolicyReader* CVccHoPolicyReader::NewLC()
    {
    CVccHoPolicyReader* self = new( ELeave ) CVccHoPolicyReader();
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }
    
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CVccHoPolicyReader::~CVccHoPolicyReader()
    {
    RUBY_DEBUG0( "~CVccHoPolicyReader START" );

    delete iSpSettings;

    RUBY_DEBUG0( "~CVccHoPolicyReader STOP" ); 
    }

// ---------------------------------------------------------------------------
// Returns ho policy 
// ---------------------------------------------------------------------------
//
TVccHoPolicy CVccHoPolicyReader::HoPolicy() const
    {
    return iPolicy;
    }  
      
// ---------------------------------------------------------------------------
// Reads current settings which define when Ho can occur 
// from Sp table and user modifiable repositories.
// ---------------------------------------------------------------------------
//      
void CVccHoPolicyReader::ReadSettingsL()
    {
    RUBY_DEBUG_BLOCK( "ReadSettingsL" );

    iSpSettings->ReadSettingsL( KVccServiceName );
    //read user settings
    
    ResolvePreferredDomainL();
    ResolveImmediateHoL();
    ResolveHoDirectionL();
    ResolveHoInHeldWaitingCallsL();   
    ResolveSignalLevelParametersL();
    ResolveDtAllowedWhenCsOriginatedL();
    }

// ---------------------------------------------------------------------------
// Defines what is the preferred domain the user should use for MO calls.
// 3GPP 24.216-720 defines operator preference:
// 0 - Indicates the preference for CS domain.
// 1 - Indicates the preference for IM CN subsystem.
// 2 - Indicates the preference for CS domain only.
// 3 - Indicates the preference for IM CN subsystem only.
//
// User preferred domain overrides operator preference.
// User preference modes: CS/PS 
// ---------------------------------------------------------------------------
//     
void CVccHoPolicyReader::ResolvePreferredDomainL()
    {
    RUBY_DEBUG_BLOCK( "ResolvePreferredDomainL" );

    TVccHoPolicyPreferredDomain val = 
    static_cast<TVccHoPolicyPreferredDomain>( iSpSettings->PreferredDomainL() );
    
    iPolicy.SetPreferredDomain( val );
    }
    
// ---------------------------------------------------------------------------
// Defines should handover occur immediately when preferred network becomes
// available
// 3GPP 24.216-720 defines operator preference: 
//  0 - Indicates the preference to not initiate domain transfer immediately 
//      to the preferred domain when that domain becomes available.
//  1 - Indicates the preference to initiate domain transfer immediately to 
//      the preferred domain when that domain becomes available.
//
// ---------------------------------------------------------------------------
//  
void  CVccHoPolicyReader::ResolveImmediateHoL()
    {
    RUBY_DEBUG_BLOCK( "ResolveImmediateHoL" );

    TBool val( iSpSettings->ImmediateDtL() );
    
    iPolicy.SetDoImmediateHo( val );
    }
    
// ---------------------------------------------------------------------------
// Resolves into which domain hanover can occur 
// 3GPP 24.216-720 defines operator preference:
// 0 - Indicates that the network operator prefers the domain transfer in the
//     CS to IM CN/IM CN to CS subsystem direction can occur.
// 1 - Indicates that the network operator prefers the domain transfer in the 
//     CS to IM CN/IM CN to CS subsystem direction cannot occur.
//
// ---------------------------------------------------------------------------
//   
void CVccHoPolicyReader::ResolveHoDirectionL()
    {
    RUBY_DEBUG_BLOCK( "ResolveHoDirectionL" );

    TInt val( 0 );
    
    if( iSpSettings->DtCsToPsAllowedL() == 0 )
        val |= ECsToPsAllowed;
    
    if( iSpSettings->DtPsToCsAllowedL() == 0 )
        val |= EPsToCsAllowed;
    
    iPolicy.SetAllowedDirection( val );
    }    
    
// ---------------------------------------------------------------------------
// Resolves can handover occur when held or waiting calls exist. 
// 3GPP 24.216-720 defines operator preference:
// 0 - Indicates that the network operator prefers the domain transfer can 
//     occur when the VCC UE is engaged in an active and a held/waiting 
//     call/session on the transferring-out domain.
// 1 - Indicates that the network operator prefers the domain transfer cannot
//     occur when the VCC UE is engaged in an active and a held/waiting 
//     call/session on the transferring-out domain.
//
// ---------------------------------------------------------------------------
//   
void CVccHoPolicyReader::ResolveHoInHeldWaitingCallsL()
    {
    RUBY_DEBUG_BLOCK( "ResolveHoInHeldWaitingCallsL" );

    TBool val( !iSpSettings->DtHeldWaitingCallsAllowedL() );
    iPolicy.SetDoHoInHeldWaitingCalls( val );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CVccHoPolicyReader::ResolveSignalLevelParametersL()
    {
    iCsParams.iHighLevel = iSpSettings->CsHoTresholdL();
    iCsParams.iLowLevel = iCsParams.iHighLevel + iSpSettings->CsHoHysteresisL();
    iCsParams.iLowTimeout = iSpSettings->CsHoHysteresisTimerLowL();
    iCsParams.iHighTimeout = iSpSettings->CsHoHysteresisTimerHighL();
    
    iPsParams.iHighLevel = iSpSettings->PsHoTresholdL();
    iPsParams.iLowLevel = iPsParams.iHighLevel + iSpSettings->PsHoHysteresisL();
    iPsParams.iLowTimeout = iSpSettings->PsHoHysteresisTimerLowL();
    iPsParams.iHighTimeout = iSpSettings->PsHoHysteresisTimerHighL();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TSignalLevelParams CVccHoPolicyReader::CsSignalLevelParams() const
    {
    return iCsParams;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TSignalLevelParams CVccHoPolicyReader::PsSignalLevelParams() const
    {
    return iPsParams;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CVccHoPolicyReader::ResolveDtAllowedWhenCsOriginatedL()
    {
    TBool val( iSpSettings->DtAllowedWhenCsOriginatedL() );
    iPolicy.SetDtAllowedWhenCsOriginated( val );
    }
