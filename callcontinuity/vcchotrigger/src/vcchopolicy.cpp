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



#include <e32const.h>

#include "vcchopolicy.h"

// ======== MEMBER FUNCTIONS ========
// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
TVccHoPolicy::TVccHoPolicy()
    : iPreferredDomain( ECsPreferred ), iAllowedDirection( 0 ),
      iImmediate( EFalse ), iHeldWaitingCalls( ETrue )
    {
    
    }

// ---------------------------------------------------------------------------
// Returns preferred mode
// ---------------------------------------------------------------------------
//
TVccHoPolicyPreferredDomain TVccHoPolicy::PreferredDomain() const
    {
    return iPreferredDomain;   
    }
    
// ---------------------------------------------------------------------------
// Sets preferred mode
// ---------------------------------------------------------------------------
//
void TVccHoPolicy::SetPreferredDomain( 
    const TVccHoPolicyPreferredDomain& aPreferredDomain ) 
    {
    iPreferredDomain = aPreferredDomain;   
    }

// ---------------------------------------------------------------------------
// Returns allowed direction
// ---------------------------------------------------------------------------
//    
TInt TVccHoPolicy::AllowedDirection() const
    {
    return iAllowedDirection;
    }
    
// ---------------------------------------------------------------------------
// Sets allowed direction
// ---------------------------------------------------------------------------
//    
void TVccHoPolicy::SetAllowedDirection( const TInt aAllowedDirection )
    {
    iAllowedDirection = aAllowedDirection;
    }
    
// ---------------------------------------------------------------------------
// Returns preferred mode
// ---------------------------------------------------------------------------
//        
TBool TVccHoPolicy::DoImmediateHo() const
    {
    return iImmediate;    
    }
    
// ---------------------------------------------------------------------------
// Sets preferred mode
// ---------------------------------------------------------------------------
//   
void TVccHoPolicy::SetDoImmediateHo( const TBool& aImmediate )
    {
    iImmediate = aImmediate;
    }
    
// ---------------------------------------------------------------------------
// Returns domain transfer while held waiting calls active in transferring-out
// domain.
// ---------------------------------------------------------------------------
//       
TBool TVccHoPolicy::DoHoInHeldWaitingCalls() const
    {
    return iHeldWaitingCalls; 
    }

// ---------------------------------------------------------------------------
// Sets flag for domain transfer while held waiting calls ongoing
// ---------------------------------------------------------------------------
//   
void TVccHoPolicy::SetDoHoInHeldWaitingCalls( const TBool& aHeldWaitingCalls )
    {
    iHeldWaitingCalls = aHeldWaitingCalls;
    }

// ---------------------------------------------------------------------------
// Sets flag for domain transfer when cs originated call
// ---------------------------------------------------------------------------
//   
void TVccHoPolicy::SetDtAllowedWhenCsOriginated( 
        const TBool& aDtAllowedWhenCsOriginated )
    {
    iDtAllowedWhenCsOriginated = aDtAllowedWhenCsOriginated;
    }

// ---------------------------------------------------------------------------
// Returns is HO allowed when CS originated call
// ---------------------------------------------------------------------------
//   
TBool TVccHoPolicy::DtAllowedWhenCsOriginated() const
    {
    return iDtAllowedWhenCsOriginated;
    }
