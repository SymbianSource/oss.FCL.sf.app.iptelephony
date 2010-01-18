/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Parsing options for optional data
*
*/



#include <ipvoicemailengine.rsg>

#include "ipvmbxparsetype.h"


_LIT8( KTo8, "To" );
_LIT8( KToShort8, "t" );
_LIT8( KFrom8, "From" );
_LIT8( KFromShort8, "f" );
_LIT8( KSubject8, "Subject" );
_LIT8( KSubjectShort8, "s" );
_LIT8( KDate8, "Date" );
_LIT8( KPriority8, "Priority" );
_LIT8( KId8, "Message-ID" );


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// Set type of tag this instance presents
// ---------------------------------------------------------------------------
//
TIpVmbxParseType::TIpVmbxParseType( const TParseTypes aType )
    {
    Set( aType );
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TIpVmbxParseType::TIpVmbxParseType() :
    iTag( &KNullDesC8 ), iResourceId( KErrNotFound )
    {
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
const TDesC8& TIpVmbxParseType::Tag() const
    {
    return *iTag;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt32 TIpVmbxParseType::ResourceId() const
    {
    return iResourceId;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void TIpVmbxParseType::Set( const TParseTypes aType )
    {
    // Set instance content
    switch ( aType )
        {
        case EDetailTo:
            {
            iTag = &KTo8;
            iResourceId = R_VOIP_VM_TO;
            break;
            }
        case EDetailToShort:
            {
            iTag = &KToShort8;
            iResourceId = R_VOIP_VM_TO;
            break;
            }
        case EDetailFrom:
            {
            iTag = &KFrom8;
            iResourceId = R_VOIP_VM_FROM;
            break;
            }
        case EDetailFromShort:
            {
            iTag = &KFromShort8;
            iResourceId = R_VOIP_VM_FROM;
            break;
            }
        case EDetailSubject:
            {
            iTag = &KSubject8;
            iResourceId = R_VOIP_VM_SUBJECT;
            break;
            }
        case EDetailSubjectShort:
            {
            iTag = &KSubjectShort8;
            iResourceId = R_VOIP_VM_SUBJECT;
            break;
            }
        case EDetailDate:
            {
            iTag = &KDate8;
            iResourceId = R_VOIP_VM_DATE;
            break;
            }
        case EDetailPriority:
            {
            iTag = &KPriority8;
            iResourceId = R_VOIP_VM_PRIORITY;
            break;
            }
        case EDetailId:
            {
            iTag = &KId8;
            iResourceId = R_VOIP_VM_MSG_ID;
            break;
            }
        default:
            iTag = &KNullDesC8;
            iResourceId = KErrNotFound;
        }
    }

