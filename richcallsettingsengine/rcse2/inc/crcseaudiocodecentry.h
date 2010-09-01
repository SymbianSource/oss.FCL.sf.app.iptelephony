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
* Description:  Single audio codec entry, which is stored to RCSE
*
*/



#ifndef CRCSEAUDIOCODECENTRY_H
#define CRCSEAUDIOCODECENTRY_H

//  INCLUDES
#include <e32base.h>

#include <crcseprofileentry.h>

// CONSTANTS
const TInt KMaxMediaTypeNameLenght = 64;
const TInt KMaxCodecNameLength = 32;

// Literals that are used to SetDefaultCodecValueSet for different
// codecs (aCodecType).
_LIT( KAudioCodecAMR,      "AMR"         );
_LIT( KAudioCodeciLBC,     "iLBC"        );
_LIT( KAudioCodecVMRWB,    "VMR-WB"      );
_LIT( KAudioCodecPCMA,     "PCMA"        );
_LIT( KAudioCodecPCMU,     "PCMU"        );
_LIT( KAudioCodecG726_40,  "G726-40"     );
_LIT( KAudioCodecG726_32,  "G726-32"     );
_LIT( KAudioCodecG726_24,  "G726-24"     );
_LIT( KAudioCodecG726_16,  "G726-16"     );
_LIT( KAudioCodecCN,       "CN"          );
_LIT( KAudioCodecG729,     "G729"        );
_LIT( KAudioCodecAMRWB,    "AMR-WB"      );
_LIT( KAudioCodecGSMEFR,   "GSM-EFR"     );
_LIT( KAudioCodecGSMFR,    "GSM-FR"      );
_LIT( KAudioCodecEVRC,     "EVRC"        );
_LIT( KAudioCodecEVRC0,    "EVRC0"       );
_LIT( KAudioCodecSMV,      "SMV"         );
_LIT( KAudioCodecSMV0,     "SMV0"        );


// CLASS DECLARATION

/**
*  Audio Codec entry, which is stored to RCSE.
*
*  @lib RCSE.lib
*  @since Series 60 3.0
*/
class CRCSEAudioCodecEntry : public CBase
    {
    public: // Enums
    
    enum TOnOff
        {
        EOONotSet = KNotSet,
        EOff = 0,
        EOn = 1
        };

    public: // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CRCSEAudioCodecEntry* NewL();

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CRCSEAudioCodecEntry* NewLC();

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CRCSEAudioCodecEntry();
        
    public: // New methods
      
        /**
        * Resets entry to default values.
        * @since Series 60 3.0
        */
        void ResetDefaultCodecValues();
        
        /**
        * Sets defeault values for different codecs.
        * @param aCodecType defines codec specific parameter default values.
        * @since Series 60 3.0
        */      
        IMPORT_C void SetDefaultCodecValueSet( TPtrC aCodecType );
        
    private:
    
        /**
        * C++ default constructor.
        * Initialises values to ENotSet.
        */
        CRCSEAudioCodecEntry();

	    /**
    	* By default Symbian 2nd phase constructor is private.
    	*/
    	void ConstructL();
        
    public: // Data
    
        // Identifier of audio codec.
        TUint32 iCodecId;
        
        // Identifier of used for define Media type name.
        TBuf<KMaxMediaTypeNameLenght> iMediaTypeName;
            
        // Identifier of used for define media subtype name. This values is set by CRCSEAudioCodec, when
        // audio codec entry is got from CRCSEAudioCodec.
        TBuf<KMaxCodecNameLength> iMediaSubTypeName;
        
        // Jitter buffer size.        
        TInt32 iJitterBufferSize;
        
        // Octet-align.
        TOnOff iOctetAlign;
        
        // Mode-set.
        RArray<TUint32> iModeSet;
                
        // Mode-change-period.
        TInt32 iModeChangePeriod;
        
        // Mode-change-neighbor.
        TOnOff iModeChangeNeighbor;
                
        // Ptime.
        TInt32 iPtime;
        
        // Maxptime.
        TInt32 iMaxptime;
        
        // Crc.
        TOnOff iCrc;
        
        // Robust-sorting.
        TOnOff iRobustSorting;
        
        // Interleaving.
        TInt32 iInterLeaving;
        
        // Channels.
        RArray<TUint32> iChannels;
        
        // Voice Activation Deactivation (VAD).
        TOnOff iVAD;
        
        // Discontinous transmission (DTX).
        TOnOff iDTX;
        
        // Sampling rate.
        TInt32 iSamplingRate;
        
        // Annex b.
        TOnOff iAnnexb;

        // Mode change capability.
        TInt32 iModeChangeCapability;

        // Max-red.
        TInt32 iMaxRed;
    };

#endif      // CRCSEAUDIOCODECENTRY_H   
            
// End of File
