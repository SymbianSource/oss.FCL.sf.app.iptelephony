/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handles and stores the VoIP codec settings.
*
*/


// INCLUDE FILES
#include    <crcseaudiocodecentry.h>
#include    "CWPVoIPCodec.h"

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// CWPVoIPCodec::CWPVoIPCodec
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
CWPVoIPCodec::CWPVoIPCodec()
    {
    }

// ---------------------------------------------------------------------------
// CWPVoIPCodec::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
void CWPVoIPCodec::ConstructL()
    {
    iMediaTypeName = HBufC::NewL( 0 );
    iMediaSubtypeName = HBufC::NewL( 0 );
    iJitterBufferSize = KNotSet;
    iOctetAlign = KNotSet;
    iModeChangePeriod = KNotSet;
    iModeChangeNeighbor = KNotSet;
    iPTime = KNotSet;
    iMaxPTime = KNotSet;
    iCrc = KNotSet;
    iRobustSorting = KNotSet;
    iInterLeaving = KNotSet;
    iVAD = KNotSet;
    iDTX = KNotSet;
    iSamplingRate = KNotSet;
    iAnnexB = KNotSet;
    iMaxRed = KNotSet;
    }

// ---------------------------------------------------------------------------
// CWPVoIPCodec::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
CWPVoIPCodec* CWPVoIPCodec::NewL()
    {
    CWPVoIPCodec* self = new( ELeave ) CWPVoIPCodec;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// Destructor
CWPVoIPCodec::~CWPVoIPCodec()
    {
    delete iMediaTypeName;
    delete iMediaSubtypeName;
    if ( iModeSet.Count() )
        {
        iModeSet.Reset();
        iModeSet.Close();
        }
    if ( iChannels.Count() )
        {
        iChannels.Reset();
        iChannels.Close();
        }
    }

// ---------------------------------------------------------------------------
// CWPVoIPCodec::SetMediaSubtypeNameL
//
// ---------------------------------------------------------------------------
void CWPVoIPCodec::SetMediaSubtypeNameL( const TDesC& aMediaSubtypeName )
	{
	delete iMediaSubtypeName;
	iMediaSubtypeName = NULL;
	iMediaSubtypeName = aMediaSubtypeName.AllocL();
	}

//  End of File  
