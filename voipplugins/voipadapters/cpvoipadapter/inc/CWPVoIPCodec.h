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


#ifndef CWPVOIPCODEC_H
#define CWPVOIPCODEC_H

//  INCLUDES
#include <e32base.h>

// CONSTANTS

// Media type name.
_LIT( KAudio, "audio" );

// Media sub-type names.
_LIT( KAMR,      "AMR"         ); // 0
_LIT( KILBC,     "iLBC"        ); // 1
_LIT( KVMRWB,    "VMR-WB"      ); // 2
_LIT( KG711ALAW, "PCMA"        ); // 3
_LIT( KG711uLAW, "PCMU"        ); // 4
_LIT( KG726_40,  "G726-40"     ); // 5
_LIT( KG726_32,  "G726-32"     ); // 6
_LIT( KG726_24,  "G726-24"     ); // 7
_LIT( KG726_16,  "G726-16"     ); // 8
_LIT( KCN,       "CN"          ); // 9
_LIT( KG729B,    "G729"        ); // 10
_LIT( KAMRWB,    "AMR-WB"      ); // 11
_LIT( KGSMEFR,   "GSM-EFR"     ); // 12
_LIT( KGSMFR,    "GSM-FR"      ); // 13
_LIT( KEVRC,     "EVRC"        ); // 14
_LIT( KEVRC0,    "EVRC0"       ); // 15
_LIT( KSMV,      "SMV"         ); // 16
_LIT( KSMV0,     "SMV0"        ); // 17


// CLASS DECLARATION
/**
*  This class keeps transiently the VoIP codec data and stores the 
*  codec via interface provided by RCSE.
*
*  @lib CWPVoIPAdapter.lib
*  @since Series 60 3.0.
*/
class CWPVoIPCodec : public CBase
    {    
    #ifdef _DEBUG
    /**
     * Friend class for unit testing.
     */
    friend class UT_CWPVoIPAdapter;
    friend class UT_CWPVoIPCodec;
    friend class UT_CWPVoIPItem;
    #endif


    public: // Enums

    enum TAudioCodecs
        {
        EAMR = 0,
        EiLBC,
        EVMRWB,
        EPCMA,
        EPCMU,
        EG726_40,
        EG726_32,
        EG726_24,
        EG726_16,
        ECN,
        EG729,
        EAMRWB,
        EGSMEFR,
        EGSMFR,
        EEVRC,
        EEVRC0,
        ESMV,
        ESMV0,
        };

    public: // Constructors and destructor

        /**
         * Two-phased constructor.
         */
        static CWPVoIPCodec* NewL();

        /**
         * Destructor.
         */
        virtual ~CWPVoIPCodec();

    public: // New functions

        /**
         * Setter for MediaSubtypeName.
         * @since Series 60 3.0.
         * @param TDesC& aMediaSubtypeName.
         */
		void SetMediaSubtypeNameL( const TDesC& aMediaSubtypeName );

    private:

        /**
         * C++ default constructor.
         */
        CWPVoIPCodec();

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

    public: // Data
        /** 
         * Existence of the data member value is dependent on the type of the 
         * particular codec. The following table describes the codecs 
         * parameter  by parameter. See further description on VoIP software 
         * settings document.
         *
         * Table legend:
         * X: there must be a value.
         * D: there is a default value (VoIP software settings document).
         * -: there is no value at all.
         * +: the value is optional.
         *
         * Codecs: (same values as in enum TVoIPCodec)
         *         0: AMR-NB
         *         1: iLBC
         *         2: VMR-WB
         *         3: G.711 A-law
         *         4: G.711 µ-law
         *         5: G726-40
         *         6: G726-32
         *         7: G726-24
         *         8: G726-16
         *         9: CN
         *        10: G.729B
         *        11: AMR-WB
         *        12: GSM-EFR
         *        13: GSM-FR
         *        14: EVRC
         *        15: EVRC0
         *        16: SMV
         *        17: SMV0
         *
         * Codec                             0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17
         * VoIP codec id.                    X  X  X  X  X  X  X  X  X  X  X  X  X  X  X  X  X  X
         * Media type name.                  XD XD XD XD XD XD XD XD XD XD XD XD XD XD XD XD XD XD
         * Media subtype name.               XD XD XD XD XD XD XD XD XD XD XD XD XD XD XD XD XD XD
         * Jitter buffer size.               +D +D +D +D +D -  -  -  -  +D +D +D -  -  +D +D +D +D
         * octet-align.                      +D -  +D +D +D -  -  -  -  -  -  +  -  -  -  -  -  -
         * Mode-set.                         +  +  +  -  -  -  -  -  -  -  -  +  -  -  -  -  -  - 
         * Mode-change-period.               +  -  -  -  -  -  -  -  -  -  -  +D -  -  +D -  -  -
         * Mode-change-neighbor.             +D -  -  -  -  -  -  -  -  -  -  +D -  -  -  -  -  -
         * Ptime.                            +D +D +  +D +D -  -  -  -  -  +  +D -  -  +  +  +  -
         * Maxptime.                         +D +D +  +D +D -  -  -  -  -  +  +D -  -  +D +  +  -
         * Crc.                              +  -  -  -  -  -  -  -  -  -  -  +  -  -  -  -  -  -
         * Robust-sorting.                   +  -  -  -  -  -  -  -  -  -  -  +  -  -  -  -  -  -
         * Interleaving.                     +  -  +  -  -  -  -  -  -  -  -  +  -  -  -  -  +D -
         * Channels.                         +D -  +  -  -  -  -  -  -  -  -  +D -  -  -  -  -  -
         * Voice Activation Detection (VAD). +  -  -  +D +D -  -  -  -  -  -  +  -  -  -  -  -  -
         * Discontinuous transmission (DTX). +  -  +D -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
         * Sampling rate.                    -  -  +D +D -  -  -  -  -  -  -  -  -  -  -  -  -  +D
         * Annex b.                          -  -  -  -  -  -  -  -  -  +D -  -  -  -  -  -  -  -
         * Max-red                           -  -  -  -  -  -  -  -  -  -  -  +D -  -  -  -  -  -
         */

        /*
         * VoIP codec id.
         */
        TUint iId;

        /*
         * Media type name.
         * Own.
         */
        HBufC* iMediaTypeName;

        /*
         * Media subtype name.
         * Own.
         */
        HBufC* iMediaSubtypeName;

        /*
         * PriorityIndex by which the codecs of the VoIP profile are ordered.
         */
        TInt32 iPriorityIndex;

        /*
         * Jitter buffer size.
         */
        TInt32 iJitterBufferSize;

        /*
         * octet-align.
         */
        TInt32 iOctetAlign;

        /*
         * Mode-set.
         * Own.
         */
        RArray<TInt32> iModeSet;

        /*
         * Mode-change-period.
         */
        TInt32 iModeChangePeriod;

        /*
         * Mode-change-neighbor.
         */
        TInt32 iModeChangeNeighbor;

        /*
         * Ptime.
         */
        TInt32 iPTime;

        /*
         * Maxptime.
         */
        TInt32 iMaxPTime;

        /*
         * Crc.
         */
        TInt32 iCrc;

        /*
         * Robust-sorting.
         */
        TInt32 iRobustSorting;

        /*
         * Interleaving.
         */
        TInt32 iInterLeaving;

        /*
         * Channels.
         * Own.
         */
        RArray<TInt32> iChannels;

        /*
         * Voice Activation Detection (VAD).
         */
        TInt32 iVAD;

        /*
         * Discontinuous transmission (DTX).
         */
        TInt32 iDTX;

        /*
         * Sampling rate.
         */
        TInt32 iSamplingRate;

        /*
         * Annex b.
         */
        TInt iAnnexB;

        /*
         * Max-red
         */
        TInt32 iMaxRed;

    };

#endif      // CWPVOIPCODEC_H

// End of File
