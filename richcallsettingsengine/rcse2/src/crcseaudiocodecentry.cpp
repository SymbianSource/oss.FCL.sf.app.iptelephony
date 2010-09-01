/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Audio codec entry, which is stored to db
*
*/



// INCLUDE FILES
#include	<featmgr.h>

#include    "crcseaudiocodecentry.h"
#include    "rcseconstants.h"
#include    "rcsepanic.h"
#include    "rcsedefaults.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CRCSEAudioCodecEntry::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CRCSEAudioCodecEntry* CRCSEAudioCodecEntry::NewL()
    {
    CRCSEAudioCodecEntry* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CRCSEAudioCodecEntry::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CRCSEAudioCodecEntry* CRCSEAudioCodecEntry::NewLC()
    {
    CRCSEAudioCodecEntry* self = new (ELeave) CRCSEAudioCodecEntry();
    CleanupStack::PushL( self );
    self->ConstructL();
    self->ResetDefaultCodecValues();
    return self;
    }

// -----------------------------------------------------------------------------
// CRCSEAudioCodecEntry::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CRCSEAudioCodecEntry::ConstructL()
    {
    // Check VoIP support from feature manager
    FeatureManager::InitializeLibL();
    TBool support = FeatureManager::FeatureSupported( KFeatureIdCommonVoip );
    FeatureManager::UnInitializeLib();
    if (!support)
        {
        User::Leave(KErrNotSupported);
        }
    }
 
// -----------------------------------------------------------------------------
// CRCSEAudioCodecEntry::CRCSEAudioCodecEntry
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CRCSEAudioCodecEntry::CRCSEAudioCodecEntry()
    {
    }
    
// Destructor
EXPORT_C CRCSEAudioCodecEntry::~CRCSEAudioCodecEntry()
    {
    iChannels.Reset();
    iChannels.Close(); 
    
    iModeSet.Reset();
    iModeSet.Close();      
    }

// -----------------------------------------------------------------------------
// CRCSEAudioCodecEntry::SetDefaultCodecValueSet
// Sets default parameter values separately for each codec.
// -----------------------------------------------------------------------------
//
EXPORT_C void CRCSEAudioCodecEntry::SetDefaultCodecValueSet( TPtrC aCodecType )
    {
    
    if ( aCodecType == KAudioCodecAMR )
        {
        iCodecId                  = 0;
        iMediaTypeName            = KRCSEDefaultMediaTypeName;
        iMediaSubTypeName         = KAudioCodecAMR;
        iJitterBufferSize         = KRCSEDefaultJitterBufferSize;
        iOctetAlign               = EOff;
        iModeSet.Reset();
        iModeChangePeriod         = KNotSet;
        iModeChangeNeighbor       = EOn;
        iPtime                    = 20;
        iMaxptime                 = KRCSEDefaultMaxptime;
        iCrc                      = EOONotSet;
        iRobustSorting            = EOONotSet;
        iInterLeaving             = KNotSet;
        iChannels.Append(1);
        iVAD                      = EOff;
        iDTX                      = EOff;
        iSamplingRate             = KNotSet;
        iAnnexb                   = EOONotSet;
        iModeChangeCapability     = KNotSet;
        iMaxRed                   = KNotSet;
        }
    else if ( aCodecType == KAudioCodeciLBC )
        {
        iCodecId                  = 0;
        iMediaTypeName            = KRCSEDefaultMediaTypeName;
        iMediaSubTypeName         = KAudioCodeciLBC;
        iJitterBufferSize         = KRCSEDefaultJitterBufferSize;
        iOctetAlign               = EOONotSet;
        iModeSet.Reset();
        iModeSet.Append(30);
        iModeChangePeriod         = KNotSet;
        iModeChangeNeighbor       = EOONotSet;
        iPtime                    = 30;
        iMaxptime                 = 180;
        iCrc                      = EOONotSet;
        iRobustSorting            = EOONotSet;
        iInterLeaving             = KNotSet;
        iChannels.Reset();
        iVAD                      = EOff;
        iDTX                      = EOff;
        iSamplingRate             = KNotSet;
        iAnnexb                   = EOONotSet;    
        iModeChangeCapability     = KNotSet;
        iMaxRed                   = KNotSet;
        }
    else if (aCodecType == KAudioCodecVMRWB)
        {
        iCodecId                  = 0;
        iMediaTypeName            = KRCSEDefaultMediaTypeName;
        iMediaSubTypeName         = KAudioCodecVMRWB;
        iJitterBufferSize         = KRCSEDefaultJitterBufferSize;
        iOctetAlign               = EOff;
        iModeSet.Reset();
        iModeChangePeriod         = KNotSet;
        iModeChangeNeighbor       = EOONotSet;
        iPtime                    = KNotSet;
        iMaxptime                 = KRCSEDefaultMaxptime;
        iCrc                      = EOONotSet;
        iRobustSorting            = EOONotSet;
        iInterLeaving             = KNotSet;
        iChannels.Reset();
        iVAD                      = EOONotSet;
        iDTX                      = EOff;
        iSamplingRate             = KNotSet;
        iAnnexb                   = EOONotSet;        
        iModeChangeCapability     = KNotSet;
        iMaxRed                   = KNotSet;
        }
    else if (aCodecType == KAudioCodecPCMA)
        {
        iCodecId                  = 0;
        iMediaTypeName            = KRCSEDefaultMediaTypeName;
        iMediaSubTypeName         = KAudioCodecPCMA;
        iJitterBufferSize         = KRCSEDefaultJitterBufferSize;
        iOctetAlign               = EOn;
        iModeSet.Reset();
        iModeChangePeriod         = KNotSet;
        iModeChangeNeighbor       = EOONotSet;
        iPtime                    = 20;
        iMaxptime                 = KRCSEDefaultMaxptime;
        iCrc                      = EOONotSet;
        iRobustSorting            = EOONotSet;
        iInterLeaving             = KNotSet;
        iChannels.Reset();
        iVAD                      = EOff;
        iDTX                      = EOff;
        iSamplingRate             = KNotSet;
        iAnnexb                   = EOONotSet;    
        iModeChangeCapability     = KNotSet;
        iMaxRed                   = KNotSet;
        }
    else if (aCodecType == KAudioCodecPCMU)
        {
        iCodecId                  = 0;
        iMediaTypeName            = KRCSEDefaultMediaTypeName;
        iMediaSubTypeName         = KAudioCodecPCMU;
        iJitterBufferSize         = KRCSEDefaultJitterBufferSize;
        iOctetAlign               = EOn;
        iModeSet.Reset();
        iModeChangePeriod         = KNotSet;
        iModeChangeNeighbor       = EOONotSet;
        iPtime                    = 20;
        iMaxptime                 = KRCSEDefaultMaxptime;
        iCrc                      = EOONotSet;
        iRobustSorting            = EOONotSet;
        iInterLeaving             = KNotSet;
        iChannels.Reset();
        iVAD                      = EOff;
        iDTX                      = EOff;
        iSamplingRate             = KNotSet;
        iAnnexb                   = EOONotSet;
        iModeChangeCapability     = KNotSet;
        iMaxRed                   = KNotSet;
        }
    else if (aCodecType == KAudioCodecCN)
        {
        iCodecId                  = 0;
        iMediaTypeName            = KRCSEDefaultMediaTypeName;
        iMediaSubTypeName         = KAudioCodecCN;
        iJitterBufferSize         = KRCSEDefaultJitterBufferSize;
        iOctetAlign               = EOONotSet;
        iModeSet.Reset();
        iModeChangePeriod         = KNotSet;
        iModeChangeNeighbor       = EOONotSet;
        iPtime                    = KNotSet;
        iMaxptime                 = KNotSet;
        iCrc                      = EOONotSet;
        iRobustSorting            = EOONotSet;
        iInterLeaving             = KNotSet;
        iChannels.Reset();
        iVAD                      = EOff;
        iDTX                      = EOONotSet;
        iSamplingRate             = 8000;
        iAnnexb                   = EOONotSet;
        iModeChangeCapability     = KNotSet;
        iMaxRed                   = KNotSet;
        }
    else if (aCodecType == KAudioCodecG729)
        {
        iCodecId                  = 0;
        iMediaTypeName            = KRCSEDefaultMediaTypeName;
        iMediaSubTypeName         = KAudioCodecG729;
        iJitterBufferSize         = KRCSEDefaultJitterBufferSize;
        iOctetAlign               = EOONotSet;
        iModeSet.Reset();
        iModeChangePeriod         = KNotSet;
        iModeChangeNeighbor       = EOONotSet;
        iPtime                    = KNotSet;
        iMaxptime                 = KRCSEDefaultMaxptime;
        iCrc                      = EOONotSet;
        iRobustSorting            = EOONotSet;
        iInterLeaving             = KNotSet;
        iChannels.Reset();
        iVAD                      = EOff;
        iDTX                      = EOff;
        iSamplingRate             = KNotSet;
        iAnnexb                   = EOONotSet;
        iModeChangeCapability     = KNotSet;
        iMaxRed                   = KNotSet;
        }
    else if (aCodecType == KAudioCodecAMRWB)
        {
        iCodecId                  = 0;
        iMediaTypeName            = KRCSEDefaultMediaTypeName;
        iMediaSubTypeName         = KAudioCodecAMRWB;
        iJitterBufferSize         = KRCSEDefaultJitterBufferSize;
        iOctetAlign               = EOff;
        iModeSet.Reset();
        iModeChangePeriod         = KNotSet;
        iModeChangeNeighbor       = EOn;
        iPtime                    = 20;
        iMaxptime                 = KRCSEDefaultMaxptime;
        iCrc                      = EOONotSet;
        iRobustSorting            = EOONotSet;
        iInterLeaving             = KNotSet;
        iChannels.Append(1);
        iVAD                      = EOff;
        iDTX                      = EOff;
        iSamplingRate             = KNotSet;
        iAnnexb                   = EOONotSet;
        iModeChangeCapability     = KNotSet;
        iMaxRed                   = KNotSet;
        }
    else if (aCodecType == KAudioCodecEVRC)
        {
        iCodecId                  = 0;
        iMediaTypeName            = KRCSEDefaultMediaTypeName;
        iMediaSubTypeName         = KAudioCodecEVRC;
        iJitterBufferSize         = KRCSEDefaultJitterBufferSize;
        iOctetAlign               = EOONotSet;
        iModeSet.Reset();
        iModeChangePeriod         = KNotSet;
        iModeChangeNeighbor       = EOONotSet;
        iPtime                    = KNotSet;
        iMaxptime                 = KRCSEDefaultMaxptime;
        iCrc                      = EOONotSet;
        iRobustSorting            = EOONotSet;
        iInterLeaving             = 5;
        iChannels.Reset();
        iVAD                      = EOff;
        iDTX                      = EOONotSet;
        iSamplingRate             = KNotSet;
        iAnnexb                   = EOONotSet;
        iModeChangeCapability     = KNotSet;
        iMaxRed                   = KNotSet;
        }
    else if (aCodecType == KAudioCodecEVRC0)
        {
        iCodecId                  = 0;
        iMediaTypeName            = KRCSEDefaultMediaTypeName;
        iMediaSubTypeName         = KAudioCodecEVRC0;
        iJitterBufferSize         = KRCSEDefaultJitterBufferSize;
        iOctetAlign               = EOONotSet;
        iModeSet.Reset();
        iModeChangePeriod         = KNotSet;
        iModeChangeNeighbor       = EOONotSet;
        iPtime                    = KNotSet;
        iMaxptime                 = KNotSet;
        iCrc                      = EOONotSet;
        iRobustSorting            = EOONotSet;
        iInterLeaving             = KNotSet;
        iChannels.Reset();
        iVAD                      = EOff;
        iDTX                      = EOONotSet;
        iSamplingRate             = KNotSet;
        iAnnexb                   = EOONotSet;
        iModeChangeCapability     = KNotSet;
        iMaxRed                   = KNotSet;
        }
    else if (aCodecType == KAudioCodecSMV)
        {
        iCodecId                  = 0;
        iMediaTypeName            = KRCSEDefaultMediaTypeName;
        iMediaSubTypeName         = KAudioCodecSMV;
        iJitterBufferSize         = KRCSEDefaultJitterBufferSize;
        iOctetAlign               = EOONotSet;
        iModeSet.Reset();
        iModeChangePeriod         = KNotSet;
        iModeChangeNeighbor       = EOONotSet;
        iPtime                    = KNotSet;
        iMaxptime                 = KRCSEDefaultMaxptime;
        iCrc                      = EOONotSet;
        iRobustSorting            = EOONotSet;
        iInterLeaving             = 5;
        iChannels.Reset();
        iVAD                      = EOff;
        iDTX                      = EOONotSet;
        iSamplingRate             = KNotSet;
        iAnnexb                   = EOONotSet;
        iModeChangeCapability     = KNotSet;
        iMaxRed                   = KNotSet;
        }
    else if (aCodecType == KAudioCodecSMV0)
        {
        iCodecId                  = 0;
        iMediaTypeName            = KRCSEDefaultMediaTypeName;
        iMediaSubTypeName         = KAudioCodecSMV0;
        iJitterBufferSize         = KRCSEDefaultJitterBufferSize;
        iOctetAlign               = EOONotSet;
        iModeSet.Reset();
        iModeChangePeriod         = KNotSet;
        iModeChangeNeighbor       = EOONotSet;
        iPtime                    = KNotSet;
        iMaxptime                 = KNotSet;
        iCrc                      = EOONotSet;
        iRobustSorting            = EOONotSet;
        iInterLeaving             = KNotSet;
        iChannels.Reset();
        iVAD                      = EOff;
        iDTX                      = EOONotSet;
        iSamplingRate             = 8000;
        iAnnexb                   = EOONotSet;
        iModeChangeCapability     = KNotSet;
        iMaxRed                   = KNotSet;
        }
    else
        {
        iOctetAlign               = KRCSEDefaultOctetAlign;
        iModeSet.Reset();
        iModeChangePeriod         = 0;
        iModeChangeNeighbor       = KRCSEDefaultModeChangeNeighbor;
        iPtime                    = 0;
        iMaxptime                 = 0;
        iCrc                      = KRCSEDefaultCRC;
        iRobustSorting            = KRCSEDefaultRobustSorting;
        iInterLeaving             = 0;
        iChannels.Reset();
        iVAD                      = KRCSEDefaultVAD;
        iDTX                      = KRCSEDefaultDTX;
        iSamplingRate             = KNotSet;
        iAnnexb                   = EOONotSet;    
        iModeChangeCapability     = KNotSet;
        iMaxRed                   = KNotSet;
        }    
    }

// -----------------------------------------------------------------------------
// CRCSEAudioCodecEntry::ResetDefaultCodecValues
// Resets codec entry to default settings.
// -----------------------------------------------------------------------------
//
void CRCSEAudioCodecEntry::ResetDefaultCodecValues()
    {
    iModeSet.Reset();
    iChannels.Reset();
    // These parameters default values are same regardless used codec.
    iCodecId                 = 0;
    iMediaTypeName           = KRCSEDefaultMediaTypeName;
    iMediaSubTypeName        = KRCSEDefaultMediaSubtypeName;
    iJitterBufferSize        = KRCSEDefaultJitterBufferSize;
    
    // These parameters default values change with used codec, here there are set to KNotSet.
    iOctetAlign              = KRCSEDefaultOctetAlign;
    iModeChangePeriod        = 0;
    iModeChangeNeighbor      = KRCSEDefaultModeChangeNeighbor;
    iPtime                   = 0;
    iMaxptime                = 0;
    iCrc                     = KRCSEDefaultCRC;
    iRobustSorting           = KRCSEDefaultRobustSorting;
    iInterLeaving            = 0;
    iVAD                     = KRCSEDefaultVAD;
    iDTX                     = KRCSEDefaultDTX;
    iSamplingRate            = 0;
    iAnnexb                  = KRCSEDefaultAnnexb;
    iModeChangeCapability    = KRCSEDefaultModeChangeCapability;
    iMaxRed                  = KRCSEDefaultMaxRed;
    }

//  End of File  
