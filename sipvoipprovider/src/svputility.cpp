/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides static utility functions for SVP
*
*/


// #include <WlanInternalPSKeys.h>
#include <sysutil.h>
#include <sipprofile.h>
 
#include <CommonEngineDomainCRKeys.h> // for accept-language -header
#include <centralrepository.h> // for language code
#include <settingsinternalcrkeys.h> // Settings UIs CR keys
#include <unsafprotocolscrkeys.h>  // Keys for keep-alive value,ipappsrv_dom\nat_settings_api

#include <mceaudiocodec.h>
#include <mceamrcodec.h>
#include <mceg711codec.h>
#include <mceilbccodec.h>
#include <mcecodec.h>
#include <mcedtmfcodec.h>
#include <mceamrwbcodec.h>
#include <mcertpsource.h>
#include <mcemicsource.h>
#include <mcertpsink.h>
#include <pathinfo.h>
#include <crcseprofileentry.h>
#include <crcseprofileregistry.h>
#include <crcseaudiocodecregistry.h>
#include <crcseaudiocodecentry.h>

#include "voipeventlog.h"                   // CVoipEventLog
#include "voiperrorentry.h"                 // CVoipErrorEntry
#include "cipapputilsaddressresolver.h"     // CIPAppUtilsAddressResolver

#include "svputility.h"
#include "svplogger.h"
#include "svpsipconsts.h"
#include "svplanguagetagconsts.h"
#include "svpaudioutility.h"

// LOCAL CONSTANTS
// Default audio frame size in milliseconds for calculating jitterbuffer size
const TInt32 KJitterBufferFrameSize = 20;

// Constants for iLBC 20 and 30ms modes
const TUint32 KSvpilbcMode20ms = 20;
const TUint32 KSvpilbcMode30ms = 30;


// ---------------------------------------------------------------------------
// CSVPUtility::NewL
// ---------------------------------------------------------------------------
//
CSVPUtility* CSVPUtility::NewL()
    {
    CSVPUtility* self = new( ELeave ) CSVPUtility();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CSVPUtility::ConstructL
// ---------------------------------------------------------------------------
//
void CSVPUtility::ConstructL()
    {
    SVPDEBUG1("CSVPUtility::ConstructL()");                                                                            
    iPreferredCodec = 0;
    iRoundIndex = 0;
    
    // create CVoipEventLog
    TRAP_IGNORE( iVoipEventLogger = CVoipEventLog::NewL() );
    }

// ---------------------------------------------------------------------------
// CSVPUtility::CSVPUtility
// ---------------------------------------------------------------------------
//
CSVPUtility::CSVPUtility()
    {
    SVPDEBUG1("CSVPUtility::CSVPUtility()");
    }


// ---------------------------------------------------------------------------
// CSVPUtility::~CSVPUtility
// ---------------------------------------------------------------------------
//
CSVPUtility::~CSVPUtility()
    {
    SVPDEBUG1("CSVPUtility::~CSVPUtility()");
    delete iVoipEventLogger;
    }

// ---------------------------------------------------------------------------
// CSVPUtility::SetAudioCodecsMOL
// ---------------------------------------------------------------------------
//
void CSVPUtility::SetAudioCodecsMOL( TUint32 aVoIPProfileId, 
                                     CMceAudioStream& aAudioInStream,
                                     TInt aKeepAliveValue )
    {
    SVPDEBUG1("CSVPUtility::SetAudioCodecsMOL: IN");
    
    RArray< TUint32 > codecIds;
    CleanupClosePushL( codecIds );
     
    // clear codec priority preferrence variable
    iPreferredCodec = 0;

    CRCSEProfileEntry* profile = CRCSEProfileEntry::NewLC();
    CRCSEAudioCodecEntry* codecprofile = CRCSEAudioCodecEntry::NewLC();
    CRCSEProfileRegistry* rCSEProfile = CRCSEProfileRegistry::NewLC();
    CRCSEAudioCodecRegistry* rCSEAudioCodec = 
                                     CRCSEAudioCodecRegistry::NewLC();
    
    // Get VoIP profile from RCSE
    rCSEProfile->FindL( aVoIPProfileId, *profile );
    
    // fetch codec id's
    GetCodecIdsByVoIPProfileIdL( *profile, codecIds );
    TBool capabilitiesSet = EFalse;
        
    // Set capabilities
    for( TInt j = 0; j < codecIds.Count(); j++ )
        {
        rCSEAudioCodec->FindL( codecIds[ j ], *codecprofile );
        if ( !codecprofile->iMediaSubTypeName.CompareF( KAudioCodecCN ) )
            {
            iCN = ETrue;
            }
        else
            {
            SVPDEBUG2("CSVPUtility::SetAudioCodecsMOL: j: %d", j);
            MoCallCapabilitiesL( aAudioInStream, 
                                 *profile, 
                                 *codecprofile );
            capabilitiesSet = ETrue;
            }
        }
     
    // in Mo case, redundancy is never offered
    iRed = EFalse;
    
    if ( capabilitiesSet )
        {
        SetDtmfL( *profile, aAudioInStream );
        }
    
    SetMediaQoSL( profile->iMediaQOS, aAudioInStream.Session() );
    
    // set comfort noise for PCMA/PCMU & ILBC if CN provisioned
    // also handles keep alive logics
    if ( iCN )
        {
        SetComfortNoiseL( aAudioInStream );
        RemoveUnnecessaryCodecsL( aAudioInStream );
        SetCNKeepAliveL( aAudioInStream, aKeepAliveValue ); 
        }
    else
        {
        RemoveUnnecessaryCodecsL( aAudioInStream );
        SetKeepAliveL( aAudioInStream, aKeepAliveValue ); 
        }
   
    // Check RTCP for actual and bound stream as we do not have knowledge where
    // the actual RTP sink is residing
    CheckRtcpSettingL( profile->iRTCP, aAudioInStream );
    if ( aAudioInStream.BoundStream() )
        {
        CheckRtcpSettingL( profile->iRTCP, aAudioInStream.BoundStreamL() );
        }
    
    // RCSE objects can be deleted
    CleanupStack::PopAndDestroy( 5, &codecIds );

    // Leave if codec information was not found.
    if ( !aAudioInStream.Codecs().Count() )
        {
        SVPDEBUG1("CSVPUtility::SetAudioCodecsMOL KErrNotFound");
        User::Leave( KErrNotFound );
        }
    
    SVPDEBUG1("CSVPUtility::SetAudioCodecsMOL: OUT");
    }

// ---------------------------------------------------------------------------
// CSVPUtility::SetKeepAliveL
// ---------------------------------------------------------------------------
//
void CSVPUtility::SetKeepAliveL( CMceAudioStream& aAudioStream,
                                 TInt aKeepAliveValue )
    {
    SVPDEBUG1( "CSVPUtility::SetKeepAliveL In ")
    
    TInt count = aAudioStream.Codecs().Count();
    SVPDEBUG2( "CSVPUtility::SetKeepAliveL Codec count: %d ", count )
    
    // values should be set to bound stream also
    CMceAudioStream& boundStream = 
        static_cast< CMceAudioStream& > ( aAudioStream.BoundStreamL() );
    
    __ASSERT_ALWAYS( &boundStream, User::Leave( KSVPErrCodecMismatch ) );
    // Amount of codecs should be the same in audio- and boundstreams
    __ASSERT_ALWAYS( count == boundStream.Codecs().Count(),
                     User::Leave( KSVPErrCodecMismatch ) );
    
    TInt maxPl( 0 );
    TInt minPl( 0 );
    // first check the max payload in the codec set
    while ( count-- )
        {
        minPl = aAudioStream.Codecs()[ count ]->PayloadType();
        if ( minPl > maxPl )
            {
            maxPl = minPl;
            }  
        }
    
    count = aAudioStream.Codecs().Count();
    // if max pl less than def 120, set 120 as pl
    if ( KSVPDefKeepAlivePl > maxPl )
        {
        while( count-- )
            {
            SVPDEBUG1( "CSVPUtility::SetKeepAliveL A ")
            aAudioStream.Codecs()[ count ]->SetKeepAlivePayloadTypeL( KSVPDefKeepAlivePl );
            aAudioStream.Codecs()[ count ]->SetKeepAliveDataL( KNullDesC8 );
            aAudioStream.Codecs()[ count ]->SetKeepAliveTimerL( aKeepAliveValue );
            boundStream.Codecs()[ count ]->SetKeepAlivePayloadTypeL( KSVPDefKeepAlivePl );
            boundStream.Codecs()[ count ]->SetKeepAliveDataL( KNullDesC8 );
            boundStream.Codecs()[ count ]->SetKeepAliveTimerL( aKeepAliveValue );
            }
        }
    else if ( KSVPDefKeepAlivePlMax > maxPl )
        {
        while( count-- )
            {
            SVPDEBUG1( "CSVPUtility::SetKeepAliveL B ")
            aAudioStream.Codecs()[ count ]->SetKeepAlivePayloadTypeL( maxPl + 1 );
            aAudioStream.Codecs()[ count ]->SetKeepAliveDataL( KNullDesC8 );
            aAudioStream.Codecs()[ count ]->SetKeepAliveTimerL( aKeepAliveValue );
            boundStream.Codecs()[ count ]->SetKeepAlivePayloadTypeL( maxPl + 1 );
            boundStream.Codecs()[ count ]->SetKeepAliveDataL( KNullDesC8 );
            boundStream.Codecs()[ count ]->SetKeepAliveTimerL( aKeepAliveValue );
            }
        }
    
    SVPDEBUG1( "CSVPUtility::SetKeepAliveL Out ")
    }

// ---------------------------------------------------------------------------
// CSVPUtility::SetCNKeepAliveL
// ---------------------------------------------------------------------------
//
void CSVPUtility::SetCNKeepAliveL( CMceAudioStream& aAudioStream,
                                   TInt aKeepAliveValue )
    {
    SVPDEBUG1( "CSVPUtility::SetCNKeepAliveL In ")
    
    TInt count = aAudioStream.Codecs().Count();
    SVPDEBUG2( "CSVPUtility::SetCNKeepAliveL Codec count: %d ", count )
    
    TInt mostPreferred = 0; 
    // bound stream should be present always
    CMceAudioStream& boundStream = 
        static_cast< CMceAudioStream& > ( aAudioStream.BoundStreamL() );
    
    __ASSERT_ALWAYS( &boundStream, User::Leave( KSVPErrCodecMismatch ) );
    // Amount of codecs should be the same in audio- and boundstreams
    __ASSERT_ALWAYS( count == boundStream.Codecs().Count(),
                     User::Leave( KSVPErrCodecMismatch ) );
    
    // find most preferred codec and set keepalive timer
    // value and payload for most preferred codec
    while( count-- )
        {
        const TDesC8& codec = aAudioStream.Codecs()[ count ]->SdpName();
         
        // handle only real audio codecs
        if (  KErrNotFound == codec.Find( KCN ) && KErrNotFound == codec.Find( KTELEVENT )  )
            {
            if ( 0 == aAudioStream.Codecs()[ count ]->Preference() )
                {
                SVPDEBUG2( "Most preferred codec: %d ", count )
                SVPDEBUG2( "Payload: %d ", aAudioStream.Codecs()[ count ]->PayloadType() )
                aAudioStream.Codecs()[ count ]->SetKeepAliveTimerL( aKeepAliveValue );
                aAudioStream.Codecs()[ count ]->SetKeepAlivePayloadTypeL( KSVPCNPayloadType );
                aAudioStream.Codecs()[ count ]->SetKeepAliveDataL( KNullDesC8 );
                boundStream.Codecs()[ count ]->SetKeepAliveTimerL( aKeepAliveValue );
                boundStream.Codecs()[ count ]->SetKeepAlivePayloadTypeL( KSVPCNPayloadType );
                boundStream.Codecs()[ count ]->SetKeepAliveDataL( KNullDesC8 );
                mostPreferred = count;
                }
           }
        }
    
    count = aAudioStream.Codecs().Count();
    while ( count-- )
        {
        if ( mostPreferred != count )
            {
            SVPDEBUG2( "Timer value 0 for codec: %d ", count )
            aAudioStream.Codecs()[ count ]->SetKeepAliveTimerL( 0 );
            aAudioStream.Codecs()[ count ]->SetKeepAlivePayloadTypeL( KSVPCNPayloadType );
            aAudioStream.Codecs()[ count ]->SetKeepAliveDataL( KNullDesC8 );
            boundStream.Codecs()[ count ]->SetKeepAliveTimerL( 0 );
            boundStream.Codecs()[ count ]->SetKeepAlivePayloadTypeL( KSVPCNPayloadType );
            boundStream.Codecs()[ count ]->SetKeepAliveDataL( KNullDesC8 );
            }
        }
    
    SVPDEBUG1( "CSVPUtility::SetCNKeepAliveL Out ")
    }

// ---------------------------------------------------------------------------
// CSVPUtility::IsComfortNoise
// ---------------------------------------------------------------------------
//
TBool CSVPUtility::IsComfortNoise( CMceAudioStream& aStream )
    {
    SVPDEBUG1( "CSVPUtility::IsComfortNoise In ")
    TInt count = aStream.Codecs().Count();
    while ( count-- )
        {
        if ( !aStream.Codecs()[ count ]->SdpName().CompareF( KCN ) )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CSVPUtility::SetAudioCodecsForEmergencyL
// ---------------------------------------------------------------------------
//
void CSVPUtility::SetAudioCodecsForEmergencyL( 
    CMceAudioStream& aAudioInStream,
    TInt aKeepAliveValue, 
    TInt32 aVoipProfileId )
    {
    SVPDEBUG1("CSVPUtility::SetAudioCodecsForEmergencyL() In")
        
    // clear codec priority preferrence variable
    iPreferredCodec = 0;

    // Default profile
    CRCSEProfileEntry* profile = CRCSEProfileEntry::NewLC(); // CS:1
    
    // Set codecs
    if ( KErrNotFound == aVoipProfileId )
        {
        // No VoIP profile, use default profile values
        SVPDEBUG1("CSVPUtility::SetAudioCodecsForEmergencyL, no VoIP profile")

        // Create default codecs (PCMA, PCMU, AMR, iLBC, G729) and set 
        // capabilities
        CRCSEAudioCodecEntry* codec = CRCSEAudioCodecEntry::NewLC(); // CS:2
        codec->SetDefaultCodecValueSet( KAudioCodecPCMA() );
        MoCallCapabilitiesL( aAudioInStream, *profile, *codec );
        CleanupStack::PopAndDestroy( codec ); // CS:1

        codec = CRCSEAudioCodecEntry::NewLC(); // CS:2
        codec->SetDefaultCodecValueSet( KAudioCodecPCMU() );
        MoCallCapabilitiesL( aAudioInStream, *profile, *codec );
        CleanupStack::PopAndDestroy( codec ); // CS:1

        codec = CRCSEAudioCodecEntry::NewLC(); // CS:2
        codec->SetDefaultCodecValueSet( KAudioCodecAMR() );
        MoCallCapabilitiesL( aAudioInStream, *profile, *codec );
        CleanupStack::PopAndDestroy( codec ); // CS:1

        codec = CRCSEAudioCodecEntry::NewLC(); // CS:2
        codec->SetDefaultCodecValueSet( KAudioCodeciLBC() );
        MoCallCapabilitiesL( aAudioInStream, *profile, *codec );
        CleanupStack::PopAndDestroy( codec ); // CS:1

        codec = CRCSEAudioCodecEntry::NewLC(); // CS:2
        codec->SetDefaultCodecValueSet( KAudioCodecG729() );
        MoCallCapabilitiesL( aAudioInStream, *profile, *codec );
        CleanupStack::PopAndDestroy( codec ); // CS:1
        }
    else
        {
        // VoIP profile exists
        SVPDEBUG1("CSVPUtility::SetAudioCodecsForEmergencyL, VoIP profile")
        User::LeaveIfError( aVoipProfileId );
            
        // Get codec IDs from profile
        CRCSEProfileRegistry* profileRegistry = 
            CRCSEProfileRegistry::NewLC(); // CS:2
        profileRegistry->FindL( aVoipProfileId, *profile );
        CleanupStack::PopAndDestroy( profileRegistry ); // CS:1
        RArray< TUint32 > codecIds;
        CleanupClosePushL( codecIds ); // CS:2
        GetCodecIdsByVoIPProfileIdL( *profile, codecIds );
        
        // Set flags
        TBool isSetPCMA( EFalse );
        TBool isSetPCMU( EFalse );

        // Set capabilities
        CRCSEAudioCodecRegistry* codecRegistry = 
            CRCSEAudioCodecRegistry::NewLC(); // CS:3
        CRCSEAudioCodecEntry* codec = NULL;
        for( TInt j = 0; j < codecIds.Count(); j++ )
            {
            codec = CRCSEAudioCodecEntry::NewLC(); // CS:4
            codecRegistry->FindL( codecIds[j], *codec );
            MoCallCapabilitiesL( aAudioInStream, *profile, *codec );
            
            // Check G711
            if ( !codec->iMediaSubTypeName.CompareF( KAudioCodecPCMA ) )
                {
                isSetPCMA = ETrue;
                }
            if ( !codec->iMediaSubTypeName.CompareF( KAudioCodecPCMU ) )
                {
                isSetPCMU = ETrue;
                }
            CleanupStack::PopAndDestroy( codec ); // CS:3
            }
        CleanupStack::PopAndDestroy( codecRegistry ); // CS:2
        CleanupStack::PopAndDestroy( &codecIds ); // CS:1
        

        // Set G711 default capabilities if not set from profile
        if ( !isSetPCMA )
            {
            codec = CRCSEAudioCodecEntry::NewLC(); // CS:2
            codec->SetDefaultCodecValueSet( KAudioCodecPCMA() );
            MoCallCapabilitiesL( aAudioInStream, *profile, *codec );
            CleanupStack::PopAndDestroy( codec ); // CS:1
            }
        if ( !isSetPCMU )
            {
            codec = CRCSEAudioCodecEntry::NewLC(); // CS:2
            codec->SetDefaultCodecValueSet( KAudioCodecPCMU() );
            MoCallCapabilitiesL( aAudioInStream, *profile, *codec );
            CleanupStack::PopAndDestroy( codec ); // CS:1
            }
        }
    
    SetDtmfL( *profile, aAudioInStream );
    
    SetMediaQoSL( profile->iMediaQOS, aAudioInStream.Session() );
    
    // in Mo case, redundancy is never offered
    iRed = EFalse;
    
    // Set comfort noise for PCMA/PCMU & ILBC if CN provisioned and keep-alive
    if ( iCN )
        {
        SetComfortNoiseL( aAudioInStream );
        RemoveUnnecessaryCodecsL( aAudioInStream );
        SetCNKeepAliveL( aAudioInStream, aKeepAliveValue );
        }
    else
        {
        RemoveUnnecessaryCodecsL( aAudioInStream );
        SetKeepAliveL( aAudioInStream, aKeepAliveValue );
        }
    
    // Check RTCP for actual and bound stream as we do not have knowledge where
    // the actual RTP sink is residing
    CheckRtcpSettingL( profile->iRTCP, aAudioInStream );
    if ( aAudioInStream.BoundStream() )
        {
        CheckRtcpSettingL( profile->iRTCP, aAudioInStream.BoundStreamL() );
        }
    
    CleanupStack::PopAndDestroy( profile ); // CS:0

    // Leave if codec information was not found.
    if ( !aAudioInStream.Codecs().Count() )
        {
        SVPDEBUG1("CSVPUtility::SetAudioCodecsForEmergencyL KErrNotFound")
        User::Leave( KErrNotFound );
        }
    
    SVPDEBUG1("CSVPUtility::SetAudioCodecsForEmergencyL() Out")
    }

// ---------------------------------------------------------------------------
// CSVPUtility::SetComfortNoiseL
// ---------------------------------------------------------------------------
//  
void CSVPUtility::SetComfortNoiseL( CMceAudioStream& aAudioStream )
    {
    SVPDEBUG1("CSVPUtility::SetComfortNoiseL In");
    
    // fetch available codecs
    const RPointerArray<CMceAudioCodec>& codecs = aAudioStream.Codecs();
    
    // Comfort Noise must be enabled for bound stream codecs also
    CMceAudioStream& boundStream = 
        static_cast< CMceAudioStream& > ( aAudioStream.BoundStreamL() ); 
   
    // fetch bound stream codecs 
    const RPointerArray< CMceAudioCodec >& 
        boundCodecs = boundStream.Codecs();
    
    iCN = EFalse;

    for ( TInt z = 0; z < codecs.Count(); z++ )
        {
        if ( !codecs[ z ]->SdpName().CompareF( KCN ) && !iCN  )
            {
            SVPDEBUG1("CSVPUtility::SetComfortNoiseL CN found");
            // enable CN from main and bound stream
            aAudioStream.Codecs()[ z ]->SetStateL( CMceCodec::EEnabled );
            boundStream.Codecs()[ z ]->SetStateL( CMceCodec::EEnabled );
            // set flag to true, so CN is not removed from media attributes
            iCN = ETrue;
            
            // set also correct priority order for CN => lowest priority
            aAudioStream.Codecs()[ z ]->SetPreferenceL( codecs.Count() );
            }
        } 
    if ( iCN )
        {
        for ( TInt i = 0; i < codecs.Count(); i++ )
            {
            if( !codecs[ i ]->SdpName().CompareF( KPCMA ) || 
                !codecs[ i ]->SdpName().CompareF( KPCMU ) ||
                !codecs[ i ]->SdpName().CompareF( KILBC ) )
                {
                SVPDEBUG1("CSVPUtility::SetComfortNoiseL Setting VAD");
                codecs[ i ]->EnableVAD( ETrue );
                boundCodecs[ i ]->EnableVAD( ETrue );
                }
            } 
        }
    SVPDEBUG1("CSVPUtility::SetComfortNoiseL Out");
    }
    
// ---------------------------------------------------------------------------
// CSVPUtility::SetDtmfL
// ---------------------------------------------------------------------------
//
void CSVPUtility::SetDtmfL( const CRCSEProfileEntry& aVoIPProfile, 
    CMceAudioStream& aAudioStream )
    {
    SVPDEBUG3( "CSVPUtility::SetDtmfL IB STATUS: %d OB STATUS: %d",
        aVoIPProfile.iInbandDTMF, aVoIPProfile.iOutbandDTMF )
    
    // If both on, not set or both off, prefer outband.
    if ( ( CRCSEAudioCodecEntry::EOn == aVoIPProfile.iInbandDTMF &&
           CRCSEAudioCodecEntry::EOn == aVoIPProfile.iOutbandDTMF ) ||
         ( CRCSEAudioCodecEntry::EOONotSet == aVoIPProfile.iInbandDTMF &&
           CRCSEAudioCodecEntry::EOONotSet == aVoIPProfile.iOutbandDTMF ) ||
         ( CRCSEAudioCodecEntry::EOff == aVoIPProfile.iInbandDTMF &&
           CRCSEAudioCodecEntry::EOff == aVoIPProfile.iOutbandDTMF ) )
        {
        SVPDEBUG1( "CSVPUtility::SetDtmfL IB/OB both enabled, not set or off" )
        
        iTelEv = ETrue;
        SetDtmfCodecOrderL( aAudioStream );
        }
    // Check inband enabled situation
    else if ( CRCSEAudioCodecEntry::EOn == aVoIPProfile.iInbandDTMF &&
              CRCSEAudioCodecEntry::EOff == aVoIPProfile.iOutbandDTMF )
        {
        SVPDEBUG1( "CSVPUtility::SetDtmfL OB DTMF OFF, IB DTMF ON" )
        
        iTelEv = EFalse;
        }
    // Check outband enabled situation
    else if ( CRCSEAudioCodecEntry::EOff == aVoIPProfile.iInbandDTMF &&
              CRCSEAudioCodecEntry::EOn == aVoIPProfile.iOutbandDTMF )
        {
        SVPDEBUG1( "CSVPUtility::SetDtmfL OB DTMF ON, IB DTMF OFF" )
        
        iTelEv = ETrue;
        SetDtmfCodecOrderL( aAudioStream );
        }
    // else all cases handled above.
    }
    
//----------------------------------------------------------------------------
// CSVPUtility::GetDTMFMode
//----------------------------------------------------------------------------
//    
TBool CSVPUtility::GetDTMFMode()
    {
    SVPDEBUG2( "SVPUtility::GetDTMFMode iTelEv: %d", iTelEv )
    
    return iTelEv; // EFalse: DTMF Inband, ETrue: DTMF Outband
    }

//----------------------------------------------------------------------------
// CSVPUtility::SetDtmfMode
//----------------------------------------------------------------------------
//
void CSVPUtility::SetDtmfMode( TBool aOutbandEnabled )
    {
    SVPDEBUG2( "SVPUtility::SetDtmfMode aOutbandEnabled: %d",
        aOutbandEnabled )
    
    iTelEv = aOutbandEnabled;
    }

// ---------------------------------------------------------------------------
// CSVPUtility::SetAudioCodecsMTL
// ---------------------------------------------------------------------------
//
void CSVPUtility::SetAudioCodecsMTL( CRCSEProfileEntry& aVoIPProfile, 
                                     CMceMediaStream& aAudioStream,
                                     TInt aKeepAliveValue,
                                     TBool aSessionUpdateOngoing )
    {
    SVPDEBUG1("CSVPUtility::SetAudioCodecsMTL In")
    
    RArray< TUint32 > codecIds;
    CleanupClosePushL( codecIds );
    
    GetCodecIdsByVoIPProfileIdL( aVoIPProfile, codecIds );
    
    // create codec profile
    CRCSEAudioCodecEntry* codecprofile = 
                                    CRCSEAudioCodecEntry::NewLC();
    CRCSEAudioCodecRegistry* rCSEAudioCodec = 
                                    CRCSEAudioCodecRegistry::NewLC();
    
    // clearing the flag for next session 
    iCodecMatched = EFalse;  
    // Initialize round index variable (for the current session)
    iRoundIndex = 0;
 
    CMceAudioStream& audioStream =
        static_cast<CMceAudioStream&>( aAudioStream );
        
    const RPointerArray< CMceAudioCodec >& supportedCodecs = 
        audioStream.Codecs();
    
    const TInt suppCodecCount = supportedCodecs.Count();
    
    // loop as many times as needed 
    while ( iRoundIndex < suppCodecCount )
        {
        for ( TInt codecCnt = 0; codecCnt < codecIds.Count(); codecCnt++ )
            {
            if ( !iCodecMatched )
                {
                rCSEAudioCodec->FindL( codecIds[ codecCnt ], *codecprofile );
                MtCallCapabilitiesL( audioStream, aVoIPProfile, *codecprofile );
                }
            }
        
        if ( iCodecMatched )
            {
            SVPDEBUG1( "CSVPUtility::SetAudioCodecsMTL RED set" )
            SetRedundancyL( audioStream );
            SVPDEBUG1( "CSVPUtility::SetAudioCodecsMTL RED set done" )
            break;
            }
        else
            {
            iRoundIndex++;    
            }
        }
    
    if ( !iCodecMatched )
        {
        User::Leave( KSVPErrCodecMismatch );
        }
    
    SetMediaQoSL( aVoIPProfile.iMediaQOS, aAudioStream.Session() );
    
    // if outband DTMF offered, it's used even if inband is provisioned
    if ( DtmfOBInOffer( audioStream ) )
        {
        SetDtmfCodecOrderL( audioStream );
        iTelEv = ETrue;
        }
    else
        {
        SetDtmfL( aVoIPProfile, audioStream );
        }
    
    // set comfort noise if provisioned and offered by caller
    for ( TInt codecCnt = 0; codecCnt < codecIds.Count(); codecCnt++ )
        {
        // fetch codec profile
        rCSEAudioCodec->FindL( codecIds[ codecCnt ], *codecprofile );
        // set comfort noise
        HBufC8* tempSdpName = 
            HBufC8::NewLC( codecprofile->iMediaSubTypeName.Length() );
        TPtr8 ptr = tempSdpName->Des();
        ptr.Copy( codecprofile->iMediaSubTypeName );  
        if ( !tempSdpName->CompareF( KCN ) )
            {
            SVPDEBUG1( "CSVPUtility::SetAudioCodecsMTL - CN Provisioned" )
            SetComfortNoiseL( audioStream );    
            }

        CleanupStack::PopAndDestroy( tempSdpName );
        }
    
    RemoveUnnecessaryCodecsL( audioStream );
    
    // CN provisioned and in offer
    if ( iCN )
        {
        SVPDEBUG1( "CSVPUtility::SetAudioCodecsMTL - CN" )
        SetCNKeepAliveL( audioStream, aKeepAliveValue );
        }
    // CN not provisioned or offered
    else
        {
        SVPDEBUG1( "CSVPUtility::SetAudioCodecsMTL - No CN" )
        SetKeepAliveL( audioStream, aKeepAliveValue );
        }
    
    // Check RTCP for actual and bound stream as we do not have knowledge where
    // the actual RTP sink is residing
    CheckRtcpSettingL( aVoIPProfile.iRTCP, aAudioStream, aSessionUpdateOngoing );
    if ( aAudioStream.BoundStream() )
        {
        CheckRtcpSettingL( aVoIPProfile.iRTCP, aAudioStream.BoundStreamL(), aSessionUpdateOngoing );
        }
    
    // RCSE objects can be deleted
    CleanupStack::PopAndDestroy( 3, &codecIds );
         
    SVPDEBUG1( "CSVPUtility::SetAudioCodecsMTL Out" )
    }

// ---------------------------------------------------------------------------
// CSVPUtility::DtmfOBInOffer
// ---------------------------------------------------------------------------
//   
TBool CSVPUtility::DtmfOBInOffer( CMceAudioStream& aAudioStream )
	{
	SVPDEBUG1( "CSVPUtility::DtmfOBInOffer In" )
    const RPointerArray< CMceAudioCodec >& supportedCodecs = 
        aAudioStream.Codecs();
    TInt count = supportedCodecs.Count();
    while ( count-- )
        {
        if ( !supportedCodecs[ count ]->SdpName().CompareF( KTELEVENT ) )
            {
            SVPDEBUG1( "CSVPUtility::DtmfOBInOffer Outband DTMF in offer" )
            return ETrue;    
            }
        }
    return EFalse;
	}

// ---------------------------------------------------------------------------
// CSVPUtility::RemoveUnnecessaryCodecsL
// ---------------------------------------------------------------------------
//    
void CSVPUtility::RemoveUnnecessaryCodecsL( CMceAudioStream& aAudioInStream )
    {
    SVPDEBUG1("CSVPUtility::RemoveUnnecessaryCodecs In");
    // if codec was not found, remove code from the codec array 
    if ( !iAMR )
        {
        SVPAudioUtility::RemoveCodecL( aAudioInStream, KAMR );
        }
    if ( !iPCMA )
        {
        SVPAudioUtility::RemoveCodecL( aAudioInStream, KPCMA );
        }
    if ( !iPCMU )
        {
        SVPAudioUtility::RemoveCodecL( aAudioInStream, KPCMU );
        }
    if ( !iILBC )
        {
        SVPAudioUtility::RemoveCodecL( aAudioInStream, KILBC );
        }
    if ( !iG729 )
        {
        SVPAudioUtility::RemoveCodecL( aAudioInStream, KG729 );
        }
    if ( !iTelEv )
        {
        SVPAudioUtility::RemoveCodecL( aAudioInStream, KTELEVENT );
        }
    if ( !iCN )
        {
        SVPAudioUtility::RemoveCodecL( aAudioInStream, KCN );
        iCN = EFalse;
        }
    if ( !iRed )
        {
        SVPAudioUtility::RemoveCodecL( aAudioInStream, KRED );
        }
    if ( !iAMRWB )
        {
        SVPAudioUtility::RemoveCodecL( aAudioInStream, KAMRWB );
        }
    
    // clear codec flags for next session
    iAMR = EFalse; 
    iPCMA = EFalse;
    iPCMU = EFalse;
    iILBC = EFalse;
    iG729 = EFalse;
    iRed = EFalse;
    iAMRWB = EFalse;
    
    SVPDEBUG1("CSVPUtility::RemoveUnnecessaryCodecs Out"); 
    }
    
// ---------------------------------------------------------------------------
// CSVPUtility::GetCodecIdsByVoIPProfileIdL
// ---------------------------------------------------------------------------
//
void CSVPUtility::GetCodecIdsByVoIPProfileIdL( 
    const CRCSEProfileEntry& aProfile,
    RArray< TUint32 >& aCodecIds ) const
    {
    SVPDEBUG1("CSVPUtility::GetCodecIdsByVoIPProfileIdL IN");
    
    aCodecIds.Reset();
    
    const TInt codCount = aProfile.iPreferredCodecs.Count();
    for ( TInt codecCnt = 0; codecCnt < codCount; codecCnt++ )
        {
        // append codec id's in preference order
        aCodecIds.AppendL( aProfile.iPreferredCodecs[ codecCnt ] );
        }
    
    SVPDEBUG1("CSVPUtility::GetCodecIdsByVoIPProfileIdL OUT");
    }

// ---------------------------------------------------------------------------
// CSVPUtility::MtCallCapabilitiesL
// ---------------------------------------------------------------------------
//
void CSVPUtility::MtCallCapabilitiesL( 
    CMceAudioStream& aAudioInStream, 
    const CRCSEProfileEntry& aVoIPProfile,
    const CRCSEAudioCodecEntry& aCodecProfile )
    {
    SVPDEBUG1( "CSVPUtility::MtCallCapabilitiesL IN" )
    
    // fetch the codecs mce supports
    const RPointerArray< CMceAudioCodec >& supportedCodecs = 
        aAudioInStream.Codecs();
    
    SVPDEBUG2( "CSVPUtility::MtCallCapabilitiesL: Supp. codec count: %d",
         supportedCodecs.Count() )
    
    HBufC8* tempSdpName = 
        HBufC8::NewLC( aCodecProfile.iMediaSubTypeName.Length() );
    
    TPtr8 ptr = tempSdpName->Des();
    ptr.Copy( aCodecProfile.iMediaSubTypeName );
    
    // compare offered codec to codec supported by app. 
    if ( *tempSdpName != supportedCodecs[ iRoundIndex ]->SdpName() ||
         !supportedCodecs[ iRoundIndex ]->SdpName().CompareF( KCN ) ||
         !supportedCodecs[ iRoundIndex ]->SdpName().CompareF( KTELEVENT ) ||
         !supportedCodecs[ iRoundIndex ]->SdpName().CompareF( KRED ) )
        {
        SVPDEBUG1( "CSVPUtility::MtCallCapabilitiesL do nothing" )
        }
    else
        {
        SVPDEBUG2( "CSVPUtility::MtCallCapabilitiesL Mediasubtypename: %S",
            &aCodecProfile.iMediaSubTypeName )
        SVPDEBUG2( "CSVPUtility::MtCallCapabilitiesL - ptime: %d",
            aCodecProfile.iPtime )
        SVPDEBUG2( "CSVPUtility::MtCallCapabilitiesL - maxptime: %d",
            aCodecProfile.iMaxptime )
        
        if ( aVoIPProfile.iStartMediaPort )
            {
            aAudioInStream.SetLocalMediaPortL( aVoIPProfile.iStartMediaPort );
            
            SVPDEBUG1( "CSVPUtility::MtCallCapabilitiesL start media port set" )
            }

        SVPDEBUG2( "CSVPUtility::MtCallCapabilitiesL codec ptime: %u",
            supportedCodecs[ iRoundIndex ]->PTime() )
        SVPDEBUG2( "CSVPUtility::MtCallCapabilitiesL codec maxptime: %u",
            supportedCodecs[ iRoundIndex ]->MaxPTime() )
        
        if ( aCodecProfile.iPtime >= 0 && 
             aCodecProfile.iMaxptime < 
             supportedCodecs[ iRoundIndex ]->PTime() )
            {   
            supportedCodecs[ iRoundIndex ]->SetPTime( aCodecProfile.iPtime );
            }
        
        if ( !aCodecProfile.iMediaSubTypeName.CompareF( KAudioCodecAMR ) )
            {
            // No actions taken in MT calls for AMR parameters. We accept
            // them as-is for improved IOP.
            iAMR = ETrue;
            } 
        
        if ( !aCodecProfile.iMediaSubTypeName.CompareF( KAudioCodecPCMA ) )
            {
            iPCMA = ETrue;
            }

        if ( !aCodecProfile.iMediaSubTypeName.CompareF( KAudioCodecPCMU ) )
            {
            iPCMU = ETrue;
            }

        if ( !aCodecProfile.iMediaSubTypeName.CompareF( KAudioCodeciLBC ) )
            {
            iILBC = ETrue;
            }

        if ( !aCodecProfile.iMediaSubTypeName.CompareF( KAudioCodecAMRWB  ) )
            {
            iAMRWB = ETrue;
            } 

        if ( !aCodecProfile.iMediaSubTypeName.CompareF( KAudioCodecG729 ) )
            {
            // AnnexB is not set if it is not in the offer. MO side may not
            // support AnnexB so we do not want to enforce it.
            iG729 = ETrue;
            }
        else
            {
            // set VAD if other than G729 codec case
            if ( CRCSEAudioCodecEntry::EOONotSet != aCodecProfile.iVAD )
                {
                SVPDEBUG1( "CSVPUtility::MtCallCapabilitiesL, codec and boundcodec VAD" );
                // Setting VAD for codec
                SetVADL( aAudioInStream, aCodecProfile.iVAD, *tempSdpName ); 

                // Setting VAD also for bound codec
                CMceAudioStream& boundStream = 
                    static_cast< CMceAudioStream& > ( aAudioInStream.BoundStreamL() ); 
                SetVADL( boundStream, aCodecProfile.iVAD, *tempSdpName ); 
                }
            }

        // Take the largest jitterbuffer size from settings. NB. this does
        // not actually take into account that we have codec spesific
        // jitterbuffer sizes and MCE has session level jitterbuffer size.
        // So we may end up using wrong jitterbuffer size if codec spesific
        // jitterbuffer settings are different amongst each other.
        if ( aCodecProfile.iJitterBufferSize > iJitterBufferSize )
            {
            iJitterBufferSize = aCodecProfile.iJitterBufferSize;
            }

        SVPDEBUG2( "CSVPUtility::MtCallCapabilitiesL iJitterBufferSize: %d",
            iJitterBufferSize )

        // set codec matched flag to ETrue, codec matched and set up
        iCodecMatched = ETrue;
        }
    
    CleanupStack::PopAndDestroy( tempSdpName );
    
    SVPDEBUG1( "CSVPUtility::MtCallCapabilitiesL OUT" )
    }

// ---------------------------------------------------------------------------
// CSVPUtility::SetRedundancyL
// ---------------------------------------------------------------------------
//
void CSVPUtility::SetRedundancyL( CMceAudioStream& aAudioStream ) 
    {
    SVPDEBUG1( "CSVPUtility::SetRedundancyL In");
    const RPointerArray< CMceAudioCodec >& codecs = aAudioStream.Codecs();
    for ( TInt z = 0; z < codecs.Count(); z++ )
        {
        if ( !codecs[ z ]->SdpName().CompareF( KRED ) )
            {
            SVPDEBUG1( "CSVPUtility::SetRedundancyL Setting Red pref.");          
            // set preference 
            aAudioStream.Codecs()[ z ]->
                SetPreferenceL( codecs.Count() );
            // set red flag so red media attribute is not removed
            iRed = ETrue;
            }
        }
    SVPDEBUG1( "CSVPUtility::SetRedundancyL Out");
    }
// ---------------------------------------------------------------------------
// CSVPUtility::MoCallCapabilitiesL
// ---------------------------------------------------------------------------
//
void CSVPUtility::MoCallCapabilitiesL( 
                              CMceAudioStream& aAudioInStream, 
                              const CRCSEProfileEntry& aVoIPProfile,
                              const CRCSEAudioCodecEntry& aCodecProfile )
    {
    SVPDEBUG1("CSVPUtility::MoCallCapabilitiesL In")
    
    // fetch the codecs mce supports 
    const RPointerArray< CMceAudioCodec >& supportedCodecs = 
        aAudioInStream.Codecs();
    
    SVPDEBUG2("CSVPUtility::MoCallCapabilitiesL, Supported codec count: %d",
        supportedCodecs.Count() );
    
    HBufC8* tempSdpName = 
        HBufC8::NewLC( aCodecProfile.iMediaSubTypeName.Length() );
        
    SVPDEBUG2("CSVPUtility::MoCallCapabilitiesL Mediasubtypename: %S",
        &aCodecProfile.iMediaSubTypeName );
    
    // go through the array and find Sdp name match
    for ( TInt z = 0; z < supportedCodecs.Count(); z++ )
        {
        TPtr8 ptr = tempSdpName->Des();
        ptr.Copy( aCodecProfile.iMediaSubTypeName );
          
        if ( *tempSdpName == supportedCodecs[ z ]->SdpName() )
            {
            // first setting audio capabilities common for all codecs
            
            // set codec priority order
            SVPDEBUG2("CSVPUtility::MoCallCapabilitiesL Codec order: %d",
                iPreferredCodec );
            
            supportedCodecs[ z ]->SetPreferenceL( iPreferredCodec++ );
            
            // set media port
            if ( aVoIPProfile.iStartMediaPort )
                {
                aAudioInStream.SetLocalMediaPortL( aVoIPProfile.iStartMediaPort );
                SVPDEBUG1("CSVPUtility::MoCallCapabilitiesL start media port set");
                }
            
            // set VAD    
            if ( CRCSEAudioCodecEntry::EOONotSet != aCodecProfile.iVAD )
                {
                SVPDEBUG1( "CSVPUtility::MoCallCapabilitiesL Setting codec VAD" );

                // Setting VAD for codec
                SetVADL( aAudioInStream, aCodecProfile.iVAD, *tempSdpName ); 

                SVPDEBUG1( "CSVPUtility::MoCallCapabilitiesL Setting bound VAD" );
                // Setting VAD also for bound codec
                CMceAudioStream& boundStream = 
                    static_cast< CMceAudioStream& > ( aAudioInStream.BoundStreamL() ); 
                SetVADL( boundStream, aCodecProfile.iVAD, *tempSdpName ); 
                }
            
            // set maxptime
            if ( aCodecProfile.iMaxptime >= 0 ) 
                {
                supportedCodecs[ z ]->SetMaxPTime( aCodecProfile.iMaxptime );
                
                SVPDEBUG2("CSVPUtility::MoCallCapabilitiesL iMaxptime: %d",
                    aCodecProfile.iMaxptime );
                }
            
            // set ptime
            if ( aCodecProfile.iPtime >= 0 )     
                {
                SVPDEBUG2("CSVPUtility::MoCallCapabilitiesL, PTime: %d",
                    aCodecProfile.iPtime );
                
                supportedCodecs[ z ]->SetPTime( aCodecProfile.iPtime );
                } 
            
            if ( !aCodecProfile.iMediaSubTypeName.CompareF( KAudioCodecAMR ) )
                {
                iAMR = ETrue;
                
                // AMR specific settings: mode-set, mode-change-period,
                // mode-change-neighbor and octet alignment vs. bw-efficient
                // mode
                SetAMRParameters( aAudioInStream, aCodecProfile, z );
                if ( CRCSEAudioCodecEntry::EOn == aCodecProfile.iOctetAlign )
                    {
                    // use octet-alignment for MO call
                    supportedCodecs[ z ]->SetCodecMode(
                        EMceOctetAligned );
                    }
                }
            
            if ( !aCodecProfile.iMediaSubTypeName.
                 CompareF( KAudioCodecPCMA ) )
                {
                iPCMA = ETrue;
                }
            if ( !aCodecProfile.iMediaSubTypeName.
                 CompareF( KAudioCodecPCMU ) )
                {
                iPCMU = ETrue;
                }
            if ( !aCodecProfile.iMediaSubTypeName.
                 CompareF( KAudioCodeciLBC ) )
                {
                iILBC = ETrue;
                // Set mode according to mode-set.
                if ( aCodecProfile.iModeSet.Count() )
                    {
                    if ( KSvpilbcMode20ms == aCodecProfile.iModeSet[0] )
                        {
                        supportedCodecs[ z ]->SetBitrate(
                            KMceiLBCLevel20Bitrate );
                        }
                    else if( KSvpilbcMode30ms == aCodecProfile.iModeSet[0] )
                        {
                        supportedCodecs[ z ]->SetBitrate(
                            KMceiLBCLevel30Bitrate );
                        }
                    }
                }
            
            if ( !aCodecProfile.iMediaSubTypeName.
                 CompareF( KAudioCodecG729 ) )
                {
                iG729 = ETrue;
                // set annexb
                if ( CRCSEAudioCodecEntry::EOn == aCodecProfile.iAnnexb )
                    {
                    supportedCodecs[ z ]->EnableVAD( ETrue );
                    }
                }
     
            if ( !aCodecProfile.iMediaSubTypeName.CompareF( KAudioCodecAMRWB ) )
                {
                iAMRWB = ETrue;
                
                // AMR-WB specific settings: mode-set, mode-change-period,
                // mode-change-neighbor and octet alignment vs. bw-efficient
                // mode
                SetAmrWbParameters( aAudioInStream, aCodecProfile, z );
                if ( CRCSEAudioCodecEntry::EOn == aCodecProfile.iOctetAlign )
                    {
                    // use octet-alignment for MO call
                    supportedCodecs[ z ]->SetCodecMode(
                        EMceOctetAligned );
                    }
                }

			// Take the largest jitterbuffer size from settings. NB. this does
            // not actually take into account that we have codec spesific
            // jitterbuffer sizes and MCE has session level jitterbuffer size.
            // So we may end up using wrong jitterbuffer size if codec spesific
            // jitterbuffer settings are different amongst each other.
            if ( aCodecProfile.iJitterBufferSize > iJitterBufferSize )
                {
                iJitterBufferSize = aCodecProfile.iJitterBufferSize;
                }
            
            SVPDEBUG2( "CSVPUtility::MoCallCapabilitiesL iJitterBufferSize: %d",
                iJitterBufferSize );
            }   
        }
    CleanupStack::PopAndDestroy( tempSdpName );
    
    SVPDEBUG1( "CSVPUtility::MoCallCapabilitiesL OUT" );
    }
    

// ---------------------------------------------------------------------------
// CSVPUtility::SetAMRParameters
// ---------------------------------------------------------------------------
//
void CSVPUtility::SetAMRParameters( CMceAudioStream& aAudioInStream, 
                                    const CRCSEAudioCodecEntry& aCodecProfile,
                                    TInt aCodecNumber )
    {
    SVPDEBUG1( "CSVPUtility::SetAMRParameters Entry" )

    TUint allowedBitrates( 0 );
    
    // Set correct mode-set (default 0,1,2,3,4,5,6,7):
    const RArray< TUint32 >& modeSetArray = aCodecProfile.iModeSet;
    for ( TInt i = 0; i < modeSetArray.Count(); i++ )
        {
        TUint value = modeSetArray[ i ];
        switch ( value )
            {
            case 0: { allowedBitrates |= KMceAllowedAmrNbBitrate475;  break; }
            case 1: { allowedBitrates |= KMceAllowedAmrNbBitrate515;  break; }
            case 2: { allowedBitrates |= KMceAllowedAmrNbBitrate590;  break; }
            case 3: { allowedBitrates |= KMceAllowedAmrNbBitrate670;  break; }
            case 4: { allowedBitrates |= KMceAllowedAmrNbBitrate740;  break; }
            case 5: { allowedBitrates |= KMceAllowedAmrNbBitrate795;  break; }
            case 6: { allowedBitrates |= KMceAllowedAmrNbBitrate102;  break; }
            case 7: { allowedBitrates |= KMceAllowedAmrNbBitrate122;  break; }
            }
            
        SVPDEBUG3( "CSVPUtility::SetAMRParameters round %d, value: %u",
            i, value )
        SVPDEBUG2( "CSVPUtility::SetAMRParameters allowedBitrates 0x%x",
            allowedBitrates )
        }
    
    if ( !allowedBitrates )
        {
        allowedBitrates = KMceAllowedAmrNbBitrateAll;
        }
    
    SVPDEBUG2( "CSVPUtility::SetAMRParameters mode-set 0x%x", allowedBitrates )
    
    if ( 2 == aCodecProfile.iModeChangePeriod )
        {
        // Set correct mode-change-period (default == 1):
        allowedBitrates |= KMceAllowedAmrNbChangePeriod2;
        }

    // Set correct mode-change-neighbor (default == 0)
    if ( CRCSEAudioCodecEntry::EOn == aCodecProfile.iModeChangeNeighbor )
        {
        allowedBitrates |= KMceAllowedAmrNbChangeNeighbor1;
        }

    aAudioInStream.Codecs()[ aCodecNumber ]->SetAllowedBitrates(
        allowedBitrates );

    SVPDEBUG2( "CSVPUtility::SetAMRParameters, Exit 0x%x", allowedBitrates )
    }
    
// ---------------------------------------------------------------------------
// CSVPUtility::GetCCPError
// ---------------------------------------------------------------------------
//
TCCPError CSVPUtility::GetCCPError( TInt& aErrorCode, TCCPTone& aTone  ) const
    {
    SVPDEBUG2( "CSVPUtility::GetCCPError aErrorCode: %d", aErrorCode )
    
    // See ccpdefs.h - this may require additional checking so that the
    // tones are really correct.
    TCCPError error = ECCPErrorNone;
    TCCPTone tone = ECCPNoSoundSequence;
   
    switch ( aErrorCode )
        {    
        case KSVPDeclineVal:
        case KSVPBusyHereVal:
        case KSVPBusyEverywhereVal:
            {
            error = ECCPErrorBusy;  
            tone = ECCPToneUserBusy;
            break;
            }
            
        case KSVPLoopDetectedVal:
        case KSVPTooManyHopsVal:
        case KSVPServerInternalErrorVal:
        case KSVPNotImplementedVal:
        case KSVPBadGatewayVal:
        case KSVPServiceUnavailableVal:
        case KSVPServerTimeoutVal:
        case KSVPVersionNotSupportedVal:
        case KSVPMessageTooLargeVal:
        case KSVPPreconditionFailureVal:
            {
            error = ECCPErrorNetworkOutOfOrder;   
            tone = ECCPToneCongestion;
            break;
            }
           
        case KSVPUseProxyVal:
        case KSVPAlternativeServiceVal:
            {
            error = ECCPErrorNotReached; 
            tone = ECCPToneSpecialInformation;
            break;
            }
    
                   
        case KSVPRequestUriTooLongVal:
        case KSVPUnsupportedUriSchemeVal:
        case KSVPAddressIncompleteVal:
        case KSVPAmbiguousVal:
            {
            error = ECCPErrorInvalidURI;   
            tone = ECCPToneSpecialInformation;
            break;
            } 
            
        case KSVPBadRequestVal:
        case KSVPMethodNotAllowedVal:
        case KSVPUnsupportedMediaTypeVal:
        case KSVP406NotAcceptableVal:
        case KSVPRequestEntityTooLargeVal:
        case KSVPBadExtensionVal:
        case KSVPExtensionRequiredVal:
        case KSVPCallDoesNotExistVal:
        case KSVPNotAcceptableHereVal:
        case KSVPNotAcceptableVal:
            {
            error = ECCPErrorNotAcceptable; 
            tone = ECCPToneRadioPathNotAvailable;
            break;
            }
        
        case KSVPRequestTerminatedVal:
        case KSVPNotFoundVal:
        case KSVPTemporarilyUnavailableVal:
            {
            error = ECCPErrorNotResponding;  
            tone = ECCPToneUserBusy;
            break;
            }
            	
        case KSVPMovedPermanentlyVal:
        case KSVPGoneVal:
        case KSVPDoesNotExistAnywhereVal:
            {
            error = ECCPErrorMovedPermanently; 
            tone = ECCPToneRadioPathNotAvailable;
            break; 
            }
        
        case KSVPRequestTimeoutVal:
            {
            error = ECCPErrorConnectionErrorRedial;  
            tone = ECCPToneSpecialInformation;
            break;
            }
       
        case KSVPUnauthorizedVal:
        case KSVPPaymentRequiredVal:
        case KSVPForbiddenVal:
        case KSVPProxyAuthenticationRequiredVal:
        case KSVPIntervalTooBriefVal:
        case KSVPBadEventVal:
        case KSVPSecurityAgreementRequiredVal:
		    {
            error = ECCPErrorNoService;
            tone = ECCPToneCongestion;
            break;
            }
                  
        default:
            {   
            if ( 299 < aErrorCode && 400 > aErrorCode )
                {
                // Received a 3XX response not handled above
                error = ECCPErrorNotReached;
                break;
                }
            else if ( 399 < aErrorCode && 500 > aErrorCode )
                {
                // Received a 4XX response not handled above
                error = ECCPErrorNotAcceptable;
                tone = ECCPToneRadioPathNotAvailable;
                break;
                }
            else if ( 499 < aErrorCode && 600 > aErrorCode )
                {
                // Received a 5XX response not handled above
                error = ECCPErrorNetworkOutOfOrder;
                tone = ECCPToneCongestion;
                break;
                }
            else if ( 599 < aErrorCode && 700 > aErrorCode )
                {
                // Received a 6XX response not handled above
                error = ECCPGlobalFailure;
                tone = ECCPToneUserBusy;
                break;
                }
            else 
                {
                SVPDEBUG1("CSVPUtility::GetCCPError Not an error code")
                aErrorCode = KErrNone;
                tone = ECCPNoSoundSequence;
                break;
                }  
            }
        }
    SVPDEBUG3( "CSVPUtility::GetCCPError error: %d, tone: %d",
        error, tone )
    
    aTone = tone;
    return error;
    }
    

// ---------------------------------------------------------------------------
// CSVPUtility::LogVoipEventL
// ---------------------------------------------------------------------------
//
void CSVPUtility::LogVoipEventL( TInt aErrorCode, TUint32 aSipProfileId,
    const TDesC& aRemoteURI, const TDesC& aMethod ) const
    {  
    SVPDEBUG1( "CSVPUtility::LogVoipEventL In" );
    HBufC* remote = NULL; 

    if ( iVoipEventLogger )
        {            
        // set the profile id and remote URI to the error text parameter
        // tab is used as value delimiter
        TBuf<KSVPTempStringlength> entryArguments;
        entryArguments.AppendNum( aSipProfileId );
        entryArguments.Append( KSVPLogFieldDelimiter );
        
        // parse "sip:" away from the remote address
        if ( aRemoteURI.Length() > 0 )
            {
            remote = aRemoteURI.AllocLC();

            if ( 0 == remote->Find( KSVPSipPrefix2 ) )
                {
                remote->Des().Delete( 0, KSVPSipPrefixLength );
                }
                
            entryArguments.Append( *remote );                
        
            CleanupStack::PopAndDestroy( remote );    
            }                     
        else
            {
            entryArguments.Append( aRemoteURI );
            }
        
        entryArguments.Append( KSVPLogFieldDelimiter );
        entryArguments.Append( aMethod );      

        CVoipErrorEntry* entry = CVoipErrorEntry::NewLC( aErrorCode, 
                                                         entryArguments );
        iVoipEventLogger->WriteError( *entry );

        CleanupStack::PopAndDestroy( entry );
        
        SVPDEBUG3( "CSVPUtility::LogVoipEventL: \
                    err: %d, aMethod: %S", aErrorCode, &aMethod );       
        
        }

    SVPDEBUG1( "CSVPUtility::LogVoipEventL Out" );
    }


// ---------------------------------------------------------------------------
// CSVPUtility::GetDtmfEvent
// ---------------------------------------------------------------------------
//
MCCPDTMFObserver::TCCPDtmfEvent CSVPUtility::GetDtmfEvent( 
    MMceDtmfObserver::TMceDtmfEvent aEvent,
    TBool aDtmfStringSending )
    {
    SVPDEBUG1( "CSVPUtility::GetDtmfEvent In" )
    
    MCCPDTMFObserver::TCCPDtmfEvent event = MCCPDTMFObserver::ECCPDtmfUnknown;
    
    switch ( aEvent )
        {
        case MMceDtmfObserver::EDtmfSendStarted:
            {
            if ( aDtmfStringSending )
                {
                SVPDEBUG1( "CSVPUtility::GetDtmfEvent ECCPDtmfSequenceStart" )
                
                event = MCCPDTMFObserver::ECCPDtmfSequenceStart;
                }
            else
                {
                SVPDEBUG1( "CSVPUtility::GetDtmfEvent ECCPDtmfManualStart" )
                
                event = MCCPDTMFObserver::ECCPDtmfManualStart;  
                }
            
            break;            
            }
        case MMceDtmfObserver::EDtmfSendCompleted:
            {
            if ( aDtmfStringSending )
                {
                SVPDEBUG1( "CSVPUtility::GetDtmfEvent ECCPDtmfStringSendingCompleted" )
                
                event = MCCPDTMFObserver::ECCPDtmfStringSendingCompleted;
                }
            // else NOP
            
            break;                  
            } 
        default:
            {
            // This block should never be reached.
            SVPDEBUG1( "CSVPUtility::GetDtmfEvent default case!" )
            
            event = MCCPDTMFObserver::ECCPDtmfUnknown;
            break;
            }
        }
    
    return event;
    }


       
// ---------------------------------------------------------------------------
// CSVPUtility::SetProvisioningDataL
// ---------------------------------------------------------------------------
//
void CSVPUtility::SetProvisioningDataL( CRCSEProfileEntry& aVoipProfile,
                     CDesC8Array& aUserAgentHeaders, 
                     TUint32& aSecurityStatus,
                     TBuf<KSVPMaxTerminalTypeLength>& aTerminalType,
                     TBuf<KSVPWlanMacAddressLength>& aWlanMACAddress ) const
    {
    SVPDEBUG1("CSVPUtility::SetProvisioningDataL In" );

    // fetch security status
    aSecurityStatus = aVoipProfile.iSecureCallPreference;
    
    SVPDEBUG2("CSVPUtility::SetProvisioningDataL: aSecurityStatus = %d",
            aSecurityStatus );

    TBool terminalTypeEnabled = aVoipProfile.iSIPVoIPUAHTerminalType;
     SVPDEBUG2("CSVPUtility::SetProvisioningDataL: terminalTypeEnabled = %d",
            terminalTypeEnabled );
    
    TBool wlanMacEnabled = aVoipProfile.iSIPVoIPUAHeaderWLANMAC;
    
      SVPDEBUG2("CSVPUtility::SetProvisioningDataL: wlanMacEnabled = %d",
            wlanMacEnabled );
            
    // temp descriptor for user-agent header
    TBuf< KSVPMaxUAFreeTextLength > uaHeaderFreeString;
    
     // SIP VoIP User Agent header string
    uaHeaderFreeString = aVoipProfile.iSIPVoIPUAHeaderString;
    
    // UA header found, append header name 
    TBuf8<KSVPTempStringlength> userAgentString;
    userAgentString.Append( KSVPUserAgent );
    
    // add terminat type
    if( terminalTypeEnabled )
        {
        userAgentString.Append( KSVPSpaceMark );
        userAgentString.Append( aTerminalType );
        SVPDEBUG2("CSVPUtility::SetProvisioningDataL: aTerminalType = %S",
            &aTerminalType );
        userAgentString.Append( KSVPSpaceMark );
        }
    // add wlan mac address
    if( wlanMacEnabled )
        {
        userAgentString.Append( aWlanMACAddress );
        SVPDEBUG2("CSVPUtility::SetProvisioningDataL: aWlanMACAddress = %S",
            &aWlanMACAddress );
        userAgentString.Append( KSVPSpaceMark );
        }
    // add free string
    if( 0 != uaHeaderFreeString.Length() )  
        {
        // append user agent header string part
        userAgentString.Append( uaHeaderFreeString );
        SVPDEBUG2("CSVPUtility::SetProvisioningDataL:\
         uaHeaderFreeString = %S", &uaHeaderFreeString );
        userAgentString.Append( KSVPSpaceMark );
        } 
    
    // match language code and add to user-agent header    
    TInt langCode =  User::Language();
    TBuf< KSVPMaxLangCodeLength > languageTag; 
    if ( langCode > 0 )
        {
        SVPDEBUG2("CSVPUtility::SetProvisioningDataL\
         Language code: %d", langCode );
        
        MatchLanguageTags( langCode, languageTag );
        
        if ( languageTag.Length() != 0 )
            {
            SVPDEBUG2("CSVPUtility::SetProvisioninDataL ISO\
            stanard language code: %S", &languageTag );
            // add language tag to user agent string
            userAgentString.Append( KSVPOpeningBracket );
            userAgentString.Append( languageTag ); 
            userAgentString.Append( KSVPClosingBracket );
            }
        }
     
    // add collected User-Agent string to header
    aUserAgentHeaders.AppendL( userAgentString );
    
    // add accept language header
    TBuf8<KSVPAcceptLangStringLength> acceptLanguageString;
    acceptLanguageString.Append( KSVPAcceptLanguage );
    acceptLanguageString.Append( languageTag );
    
    // add expires header
     // add accept language header
    TBuf8<KSVPExpiresHeaderLength> expiresHeaderString;
    expiresHeaderString.Append( KSVPExpiresHeader );
    expiresHeaderString.Append( KSVPExpires );
    
    aUserAgentHeaders.AppendL( acceptLanguageString );
    aUserAgentHeaders.AppendL( expiresHeaderString );
        
    // Add Privacy header
    TBuf8<KSVPPrivacyLength> privacyString;
    privacyString.Append( KSVPPrivacy );

    if ( IsCLIROnL() )
        {
        privacyString.Append( KSVPId );
        }
    else
        {
        privacyString.Append( KSVPNone );
        }
    aUserAgentHeaders.AppendL( privacyString );
    }

// ---------------------------------------------------------------------------
// CSVPUtility::IsCLIROnL
// ---------------------------------------------------------------------------
//
TBool CSVPUtility::IsCLIROnL() const
    {

    TInt valueCLIR( KSVPClirOff ); 
    TBool clirOn = EFalse;

    CRepository* repository = NULL;
    repository = CRepository::NewL( KCRUidRichCallSettings );
    repository->Get( KRCSEClir, valueCLIR );
    delete repository;
    repository = NULL;

    SVPDEBUG2("CSVPUtility::IsCLIROnL valueCLIR : %d", valueCLIR );
    if ( KSVPClirOn == valueCLIR )
        {
        // 0 = KRCSEClirOff
        clirOn = ETrue;
        }
    else
        {
        // 1 = KRCSEClirOn or 2 = KRCSEClirDefault
        clirOn = EFalse;
        }

    SVPDEBUG2("CSVPUtility::IsCLIROnL returns %d", clirOn);
    return clirOn;
    }

// ---------------------------------------------------------------------------
// CSVPUtility::SetMoFromHeaderLC
// ---------------------------------------------------------------------------
//
HBufC8* CSVPUtility::SetMoFromHeaderLC( TUint aSecurityStatus )
    {

    HBufC8* fromheader = NULL;

    if ( IsCLIROnL() )
        {
        SVPDEBUG1("CSVPMoSession::SetMoFromHeaderLC CLIR is ON");
        if ( KSVPStatusNonSecure == aSecurityStatus )
            {
        	fromheader = HBufC8::NewLC(KSVPMyAnonymousAddress.iTypeLength);
        	fromheader->Des().Append( KSVPMyAnonymousAddress );
            }
        else
            {
        	fromheader = HBufC8::NewLC(KSVPMyAnonymousSecAddress.iTypeLength);
        	fromheader->Des().Append( KSVPMyAnonymousSecAddress );
            }
        }
    else
        {
        SVPDEBUG1("CSVPMoSession::SetMoFromHeaderLC CLIR is OFF");
    	fromheader = HBufC8::NewLC(KSVPEmpty.iTypeLength);
    	fromheader->Des().Append( KSVPEmpty );
        }

    return fromheader;
    }

// ---------------------------------------------------------------------------
// CSVPUtility::GetTerminalTypeL
// ---------------------------------------------------------------------------
//
void CSVPUtility::GetTerminalTypeL(
	    TBuf< KSVPMaxTerminalTypeLength >& aTerminalType )
    {
    // Using ipapputils functionality
    TBuf<KSVPMaxTerminalTypeLength> terminalType (KNullDesC);
    
    CIpAppPhoneUtils* libIpAppPhoneUtils = CIpAppPhoneUtils::NewLC();
    
    libIpAppPhoneUtils->GetTerminalTypeL( terminalType );    

    CleanupStack::PopAndDestroy( libIpAppPhoneUtils );
    aTerminalType.Copy( terminalType );    
    }

// ---------------------------------------------------------------------------
// CSVPUtility::GetWlanMACAddressL
// ---------------------------------------------------------------------------
//
void CSVPUtility::GetWlanMACAddressL(
    TBuf< KSVPWlanMacAddressLength >& aWlanMACAddress ) 
    {    
    // Using ipapputils functionality
    TBuf8<KSVPWlanMacAddressLength> wlanMacAddress (KNullDesC8);
    
    CIPAppUtilsAddressResolver* addressResolver = 
            CIPAppUtilsAddressResolver::NewLC();    
    
    TInt ret = addressResolver->GetWlanMACAddress( wlanMacAddress, 
            KSVPWlanMacAddressFrmt );
		
    SVPDEBUG2("CSVPUtility::GetWlanMACAddressL ret: %d", ret );
	
    CleanupStack::PopAndDestroy( addressResolver );
    aWlanMACAddress.Copy( wlanMacAddress );
    }

// ---------------------------------------------------------------------------
// CSVPUtility::ResolveSecurityMechanismL
// ---------------------------------------------------------------------------
//
void CSVPUtility::ResolveSecurityMechanismL( const CSIPProfile& aProfile,
                                             TUint32& aSecureCallPreference )
    {
    const TDesC8* securityMechanism = NULL;
    const TDesC8* sipUserAor = NULL;
    const TDesC8* sipOutboundProxy = NULL;
    const TDesC8* registeredContact = NULL;

    aProfile.GetParameter( KSIPNegotiatedSecurityMechanism,
                            securityMechanism );
    aProfile.GetParameter( KSIPUserAor,
                            sipUserAor );
    aProfile.GetParameter( KSIPOutboundProxy,
                            KSIPServerAddress,
                            sipOutboundProxy );
    aProfile.GetParameter( KSIPRegisteredContact,
                            KSIPServerAddress,
                            registeredContact );

    if ( KErrNotFound == securityMechanism->Find( KSVPTLS ) &&
         KErrNotFound == sipUserAor->Find( KSVPSipsPrefix ) &&
         KErrNotFound == sipOutboundProxy->Find( KSVPSipsPrefix ) &&
         KErrNotFound == registeredContact->Find( KSVPSipsPrefix ) )
        {
        SVPDEBUG1("CSVPUtility::ResolveSecurityMechanism:\
         TLS not enabled (tls and sips: wasn't found)");
        aSecureCallPreference = 0;
        }
    }

// ---------------------------------------------------------------------------
// CSVPUtility::MatchLanguageTags
// ---------------------------------------------------------------------------
//
void CSVPUtility::MatchLanguageTags( TInt aLanguageCode, 
    TDes& aLangMatch )
    {
    SVPDEBUG2("CSVPUtility::MatchLanguageTags Langcode: %d", aLanguageCode );
    
    switch( aLanguageCode)
        {
        case ESVPLangEnglish:
            {
            SVPDEBUG1("CSVPUtility::MatchLanguageTags -> English");
            aLangMatch.Copy( KSVPLanguageTagEn );
            break;
            }  
        case ESVPLangFrench:
            {
            aLangMatch.Copy( KSVPLanguageTagFr );
            break;
            }      
        case ESVPLangGerman:
            {
            aLangMatch.Copy( KSVPLanguageTagDe );
            break;
            }       
        case ESVPLangSpanish:
            {
            aLangMatch.Copy( KSVPLanguageTagEs );
            break;
            }      
        case ESVPLangItalian:
            {
            aLangMatch.Copy( KSVPLanguageTagIt );
            break;
            } 
        case ESVPLangSwedish:
            {
            aLangMatch.Copy( KSVPLanguageTagSv );
            break;
            } 
        case ESVPLangDanish:
            {
            aLangMatch.Copy( KSVPLanguageTagDa );
            break;
            }                      
        case ESVPLangNorwegian: 
            {
            aLangMatch.Copy( KSVPLanguageTagNo );
            break;
            }
        case ESVPLangFinnish: 
            {
            aLangMatch.Copy( KSVPLanguageTagFi );
            break;
            }
        case ESVPLangAmerican:
            {
            aLangMatch.Copy( KSVPLanguageTagEn );
            break;
            }
        case ESVPLangSwissFrench: 
            {
            aLangMatch.Copy( KSVPLanguageTagFr );
            break;
            }
        case ESVPLangSwissGerman:
            {
            aLangMatch.Copy( KSVPLanguageTagDe );
            break;
            }
        case ESVPLangPortuguese:
            {
            aLangMatch.Copy( KSVPLanguageTagPt );
            break;
            }
        case ESVPLangTurkish: 
            {
            aLangMatch.Copy( KSVPLanguageTagTr );
            break;
            }
        case ESVPLangIcelandic:
            {
            aLangMatch.Copy( KSVPLanguageTagIs );
            break;
            } 
        case ESVPLangRussian:
            {
            aLangMatch.Copy( KSVPLanguageTagRu );
            break;
            }
        case ESVPLangHungarian:
            {
            aLangMatch.Copy( KSVPLanguageTagHu );
            break;
            }
        case ESVPLangDutch:
            {
            aLangMatch.Copy( KSVPLanguageTagNl );
            break;
            }
        case ESVPLangBelgianFlemish:
            {
            aLangMatch.Copy( KSVPLanguageTagNl );
            break;
            }
        case ESVPLangAustralian:
            {
            aLangMatch.Copy( KSVPLanguageTagEn );
            break;
            }
        case ESVPLangBelgianFrench:
            {
            aLangMatch.Copy( KSVPLanguageTagFr );
            break;
            }
        case ESVPLangAustrian:
            {
            aLangMatch.Copy( KSVPLanguageTagDe );
            break;
            }
        case ESVPLangNewZealand:
            {
            aLangMatch.Copy( KSVPLanguageTagEn );
            break;
            }
        case ESVPLangInternationalFrench:
            {
            aLangMatch.Copy( KSVPLanguageTagFr );
            break;
            }
        case ESVPLangCzech:
            {
            aLangMatch.Copy( KSVPLanguageTagCs );
            break;
            }
        case ESVPLangSlovak:
            {
            aLangMatch.Copy( KSVPLanguageTagSk );
            break;
            }
        case ESVPLangPolish:
            {
            aLangMatch.Copy( KSVPLanguageTagPl );
            break;
            }
        case ESVPLangSlovenian:
            {
            aLangMatch.Copy( KSVPLanguageTagSl );
            break;
            }
        case ESVPLangTaiwanChinese:
            {
            aLangMatch.Copy( KSVPLanguageTagTaiwaneseChinese );
            break;
            }
        case ESVPLangHongKongChinese:
            {
            aLangMatch.Copy( KSVPLanguageTagHongKongChinese );
            break;
            }
        case ESVPLangPrcChinese:
            {
            aLangMatch.Copy( KSVPLanguageTagChinese );
            break;
            }
        case ESVPLangJapanese:
            {
            aLangMatch.Copy( KSVPLanguageTagJa );
            break;
            }
        case ESVPLangThai:
            {
            aLangMatch.Copy( KSVPLanguageTagTh );
            break;
            }
        case ESVPLangAfrikaans:
            {
            aLangMatch.Copy( KSVPLanguageTagAf );
            break;
            }
        case ESVPLangAlbanian:
            {
            aLangMatch.Copy( KSVPLanguageTagSq );
            break;
            }
        case ESVPLangAmharic:
            {
            aLangMatch.Copy( KSVPLanguageTagAm );
            break;
            }
        case ESVPLangArabic:
            {
            aLangMatch.Copy( KSVPLanguageTagAr );
            break;
            }
        case ESVPLangArmenian:
            {
            aLangMatch.Copy( KSVPLanguageTagHy );
            break;
            }
        case ESVPLangTagalog:
            {
            aLangMatch.Copy( KSVPLanguageTagTl );
            break;
            }
        case ESVPLangBelarussian:
            {
            aLangMatch.Copy( KSVPLanguageTagBe );
            break;
            }
        case ESVPLangBengali:
            {
            aLangMatch.Copy( KSVPLanguageTagBn );
            break;
            }
        case ESVPLangBulgarian:
            {
            aLangMatch.Copy( KSVPLanguageTagBg );
            break;
            }
        case ESVPLangBurmese:
            {
            aLangMatch.Copy( KSVPLanguageTagMy );
            break;
            }
        case ESVPLangCatalan:
            {
            aLangMatch.Copy( KSVPLanguageTagCa );
            break;
            }
        case ESVPLangCroatian:
            {
            aLangMatch.Copy( KSVPLanguageTagHr );
            break;
            }
        case ESVPLangCanadianEnglish:
        case ESVPLangInternationalEnglish:
        case ESVPLangSouthAfricanEnglish:
            {
            aLangMatch.Copy( KSVPLanguageTagEn );
            break;
            }
        case ESVPLangEstonian:
            {
            aLangMatch.Copy( KSVPLanguageTagEt );
            break;
            }
        case ESVPLangFarsi:
            {
            aLangMatch.Copy( KSVPLanguageTagFa );
            break;
            }
        case ESVPLangCanadianFrench:
            {
            aLangMatch.Copy( KSVPLanguageTagCanadianFrench );
            break;
            }
        case ESVPLangScotsGaelic:
            {
            aLangMatch.Copy( KSVPLanguageTagGd );
            break;
            }
        case ESVPLangGeorgian:
            {
            aLangMatch.Copy( KSVPLanguageTagKa );
            break;
            }
        case ESVPLangGreek:
            {
            aLangMatch.Copy( KSVPLanguageTagEl );
            break;
            }
        case ESVPLangCyprusGreek:
            {
            aLangMatch.Copy( KSVPLanguageTagCyprusGreek );
            break;
            }
        case ESVPLangGujarati:
            {
            aLangMatch.Copy( KSVPLanguageTagGu );
            break;
            }
        case ESVPLangHebrew:
            {
            aLangMatch.Copy( KSVPLanguageTagHe );
            break;
            }
        case ESVPLangHindi:
            {
            aLangMatch.Copy( KSVPLanguageTagHi );
            break;
            }
        case ESVPLangIndonesian:
            {
            aLangMatch.Copy( KSVPLanguageTagId );
            break;
            }
        case ESVPLangIrish:
            {
            aLangMatch.Copy( KSVPLanguageTagGa );
            break;
            }
        case ESVPLangSwissItalian:
            {
            aLangMatch.Copy( KSVPLanguageTagSwissItalian );
            break;
            }
        case ESVPLangKannada:
            {
            aLangMatch.Copy( KSVPLanguageTagKn );
            break;
            }
        case ESVPLangKazakh:
            {
            aLangMatch.Copy( KSVPLanguageTagKk );
            break;
            }
        case ESVPLangKhmer:
            {
            aLangMatch.Copy( KSVPLanguageTagKm );
            break;
            }
        case ESVPLangKorean:
            {
            aLangMatch.Copy( KSVPLanguageTagKo );
            break;
            }
        case ESVPLangLao:
            {
            aLangMatch.Copy( KSVPLanguageTagLo );
            break;
            }
        case ESVPLangLatvian:
            {
            aLangMatch.Copy( KSVPLanguageTagLv );
            break;
            }
        case ESVPLangLithuanian:
            {
            aLangMatch.Copy( KSVPLanguageTagLt );
            break;
            }
        case ESVPLangMacedonian:
            {
            aLangMatch.Copy( KSVPLanguageTagMk );
            break;
            }
        case ESVPLangMalay:
            {
            aLangMatch.Copy( KSVPLanguageTagMs );
            break;
            }
        case ESVPLangMalayalam:
            {
            aLangMatch.Copy( KSVPLanguageTagMl );
            break;
            }
        case ESVPLangMarathi:
            {
            aLangMatch.Copy( KSVPLanguageTagMr );
            break;
            }
        case ESVPLangMoldavian:
            {
            aLangMatch.Copy( KSVPLanguageTagMo );
            break;
            }
        case ESVPLangMongolian:
            {
            aLangMatch.Copy( KSVPLanguageTagMn );
            break;
            }
        case ESVPLangNorwegianNynorsk:
            {
            aLangMatch.Copy( KSVPLanguageTagNorwegianNynorsk );
            break;
            }
        case ESVPLangBrazilianPortuguese:
            {
            aLangMatch.Copy( KSVPLanguageTagBrazilianPortugese );
            break;
            }
        case ESVPLangPunjabi:
            {
            aLangMatch.Copy( KSVPLanguageTagPa );
            break;
            }
        case ESVPLangRomanian:
            {
            aLangMatch.Copy( KSVPLanguageTagRo );
            break;
            }
        case ESVPLangSerbian:
            {
            aLangMatch.Copy( KSVPLanguageTagSr );
            break;
            }
        case ESVPLangSinhalese:
            {
            aLangMatch.Copy( KSVPLanguageTagSi );
            break;
            }
        case ESVPLangSomali:
            {
            aLangMatch.Copy( KSVPLanguageTagSo );
            break;
            }
        case ESVPLangInternationalSpanish:
            {
            aLangMatch.Copy( KSVPLanguageTagEs );
            break;
            }
        case ESVPLangLatinAmericanSpanish:
            {
            aLangMatch.Copy( KSVPLanguageTagAmericanSpanish );
            break;
            }
        case ESVPLangSwahili:
            {
            aLangMatch.Copy( KSVPLanguageTagSw );
            break;
            }
        case ESVPLangFinlandSwedish:
            {
            aLangMatch.Copy( KSVPLanguageTagFinlandSwedish );
            break;
            }
        case ESVPLangReserved1:
            {
            aLangMatch.Copy( KSVPLanguageTagReserved );
            break;
            }
        case ESVPLangTamil:
            {
            aLangMatch.Copy( KSVPLanguageTagTa );
            break;
            }
        case ESVPLangTelugu:
            {
            aLangMatch.Copy( KSVPLanguageTagTe );
            break;
            }
        case ESVPLangTibetan:
            {
            aLangMatch.Copy( KSVPLanguageTagBo );
            break;
            }
        case ESVPLangTigrinya:
            {
            aLangMatch.Copy( KSVPLanguageTagTi );
            break;
            }
        case ESVPLangCyprusTurkish:
            {
            aLangMatch.Copy( KSVPLanguageTagCyprusTurkish );
            break;
            }
        case ESVPLangTurkmen:
            {
            aLangMatch.Copy( KSVPLanguageTagTurkaem );
            break;
            }
        case ESVPLangUkrainian:
            {
            aLangMatch.Copy( KSVPLanguageTagUk );
            break;
            }
        case ESVPLangUrdu:
            {
            aLangMatch.Copy( KSVPLanguageTagUr );
            break;
            }
        case ESVPLangReserved2:
            {
            aLangMatch.Copy( KSVPLanguageTagReserved );
            break;
            }
        case ESVPLangVietnamese:
            {
            aLangMatch.Copy( KSVPLanguageTagVi );
            break;
            }
        
        case ESVPLangWelsh:
            {
            aLangMatch.Copy( KSVPLanguageTagCy );
            break;
            }
        case ESVPLangZulu:
            {
            aLangMatch.Copy( KSVPLanguageTagZu );
            break;
            }
        case ESVPLangOther:
            {
            break;
            }
        case ESVPLangSouthSotho:
            {
            aLangMatch.Copy( KSVPLanguageTagSoth );
            break;
            }
        case ESVPLangManufacturerEnglish:
        case ESVPLangEnglish_Apac:
        case ESVPLangEnglish_Taiwan:
        case ESVPLangEnglish_HongKong:
        case ESVPLangEnglish_Prc:
        case ESVPLangEnglish_Japan:
        case ESVPLangEnglish_Thailand:
            {
            aLangMatch.Copy( KSVPLanguageTagEn );
            break;
            }
        case ESVPLangMalay_Apac:
            {
            aLangMatch.Copy( KSVPLanguageTagMalay );
            break;
            }
        case ESVPLangNone:
        case ESVPLangMaximum:
            {
            SVPDEBUG1("CSVPUtility::MatchLanguageTags Lang none/maximum");
            break;
            } 
        default:
            {
            SVPDEBUG1("CSVPUtility::MatchLanguageTags Default");
            aLangMatch.Copy( KSVPLanguageTagEn );
            }
        }
    }

// ---------------------------------------------------------------------------
// CSVPUtility::UpdateJitterBufferSizeL
// ---------------------------------------------------------------------------
//
void CSVPUtility::UpdateJitterBufferSizeL( CMceRtpSource& aRtpSource )
    {
    // Convert the jitterbuffer size from milliseconds to RTP packets. This
    // does not take into account the fact that RTP packet can hold n
    // milliseconds of audio. Also G.711 may use 10ms frames, so and we are
    // assuming 20ms frames here. But, MCC actually has the logic to handle
    // this issue, as it also assumes initially that G.711 has 20ms frames and
    // then adjusts accordingly.
    TInt32 jBufSize = KSvpJitterBufferLength;
    if ( iJitterBufferSize )
        {
        jBufSize = iJitterBufferSize / KJitterBufferFrameSize;
        }
    
    SVPDEBUG2( "CSVPUtility::UpdateJitterBufferSizeL size: %d", jBufSize );
    SVPDEBUG2( "CSVPUtility::UpdateJitterBufferSizeL iJitterBufferSize: %d",
        iJitterBufferSize );
    
    // If jitterbuffer size goes to zero, then force it back to default. We
    // cannot tolerate zero size jitterbuffer.
    if ( !jBufSize )
        {
        SVPDEBUG1( "CSVPUtility::UpdateJitterBufferSizeL need to default" )
        
        jBufSize = KSvpJitterBufferLength;
        }
    
    aRtpSource.UpdateL( jBufSize, 
                        KSvpJitterBufferThreshold,
                        KSvpStandbyTimerInMillisecs );
                        
    iJitterBufferSize = 0;
    }

// ---------------------------------------------------------------------------
// CSVPUtility::SetDtmfCodecOrderL
// ---------------------------------------------------------------------------
//
void CSVPUtility::SetDtmfCodecOrderL( CMceAudioStream& aAudioStream ) const
    {
    SVPDEBUG1( "CSVPUtility::SetDtmfCodecOrderL In" );
 
    const RPointerArray<CMceAudioCodec>& codecs = aAudioStream.Codecs();
    const TInt codCount( codecs.Count() );
     
    // loop through codecs in in stream
    for ( TInt i = 0; i < codCount; i++ )
        {
        // Check if codec is DTMF, note the return values of CompareF
        if( !codecs[ i ]->SdpName().CompareF( KTELEVENT ) )
            {
            codecs[ i ]->SetPreferenceL( iPreferredCodec );
            }
        }
    SVPDEBUG1( "CSVPUtility::SetDtmfCodecOrderL Out" );
    }

// ---------------------------------------------------------------------------
// CSVPUtility::SetMediaQoSL
// ---------------------------------------------------------------------------
//
void CSVPUtility::SetMediaQoSL( TUint32 aQosValue,
    CMceSession* aSession ) const
    {
    SVPDEBUG2( "CSVPUtility::SetMediaQoSL aQosValue: %u", aQosValue );
    
    __ASSERT_ALWAYS( aSession, User::Leave( KErrArgument ) );
    
    // We need to do bitshifting on the IP TOS, because it's the upper 6 bits
    // that are set and settings provide us the IP TOS as the lower 6 bits.
    // The lower 2 bits are reserver for explicit congestion notification.
    // See also more from:
    // Symbian Developer Library / in_sock.h Global variables / KSoIpTOS
    const TInt ipTosShift( 2 );
    const TUint32 tosBits( aQosValue << ipTosShift );
    
    SVPDEBUG2( "CSVPUtility::SetMediaQoSL tosBits: %u", tosBits );
    
    aSession->SetServiceTypeL( tosBits );
    
    SVPDEBUG1( "CSVPUtility::SetMediaQoSL out" );
    }

// ---------------------------------------------------------------------------
// CSVPUtility::GetKeepAliveByIapIdL
// ---------------------------------------------------------------------------
//
TBool CSVPUtility::GetKeepAliveByIapIdL( TUint32 aIapId,
    TInt& aKeepAliveValue ) const
    {
    SVPDEBUG2( "CSVPUtility::SetKeepAliveIapIdL IN, aIapId = %d", aIapId );

    TBool found = EFalse;
    TInt keepAliveValue = 0;
    aKeepAliveValue = KSVPDefaultUDPRefreshInterval;
    RArray< TUint32 > keys;
    CleanupClosePushL( keys );

    CRepository* rep = CRepository::NewLC( KCRUidUNSAFProtocols );

    TInt err = rep->FindEqL( KUNSAFProtocolsIAPIdMask,
                             KUNSAFProtocolsFieldTypeMask,
                             static_cast<TInt>(aIapId),
                             keys );

    if ( KErrNone == err  && keys.Count() == 1 )
        {

        TUint32 key = ( KUNSAFProtocolsIAPIdMask^( keys[ 0 ] ) );
        key |= KUNSAFProtocolsIAPTableMask;

        // UDP refresh interval
        SVPDEBUG2( "CSVPUtility::SetKeepAliveIapIdL UDP refresh interval key = %d", key );
        if ( KErrNone == rep->Get( KUNSAFProtocolsIntervalUDPMask|key, keepAliveValue ) )
            {
            aKeepAliveValue = keepAliveValue;
            found = ETrue;
            }
        }

    CleanupStack::PopAndDestroy( 2, &keys );  // keys & rep
    SVPDEBUG3( "CSVPUtility::SetKeepAliveIapIdL OUT keepAliveValue = %d found = %d",
                    keepAliveValue, found );
    return found;
    }

// ---------------------------------------------------------------------------
// CSVPUtility::GetKeepAliveByAORL
// ---------------------------------------------------------------------------
//
TBool CSVPUtility::GetKeepAliveByAORL( const TDesC8& aAor,
    TInt& aKeepAliveValue ) const
    {
    SVPDEBUG1( "CSVPUtility::GetKeepAliveByAORL In" );

    HBufC8* newAor = aAor.AllocLC();
    // Domain name is what is after @ character
    newAor->Des().Delete( 0, newAor->Find( KSVPAt ) + 1 );

    TBool found = EFalse;
    TInt keepAliveValue = 0;
    aKeepAliveValue = KSVPDefaultUDPRefreshInterval;
    RArray<TUint32> keys;
    CleanupClosePushL( keys );

    CRepository* rep = CRepository::NewLC( KCRUidUNSAFProtocols );

    TInt err = rep->FindEqL( KUNSAFProtocolsDomainMask,
                             KUNSAFProtocolsFieldTypeMask,
                             *newAor,
                             keys );

    if ( KErrNone == err && keys.Count() == 1 )
        {
        TUint32 key = ( KUNSAFProtocolsDomainMask^( keys[0] ) );
        key |= KUNSAFProtocolsDomainTableMask;

        // UDP refresh interval
        if ( KErrNone ==
             rep->Get( KUNSAFProtocolsDomainIntervalUDPMask|key, keepAliveValue ) )
            {
            aKeepAliveValue = keepAliveValue;
            found = ETrue;
            }
        }

    CleanupStack::PopAndDestroy( 3, newAor );

    if ( !found )
        {
        // Neither IapId nor Domain spesific keepalive value was not found.
        SVPDEBUG1( "CSVPUtility::GetKeepAliveByAORL::GetKeepAliveByAORL use default keepalive" );
        
        aKeepAliveValue = KSVPDefaultUDPRefreshInterval;
        }
    
    SVPDEBUG2( "CSVPUtility::GetKeepAliveByAORL Out found: %d", found );
    return found;
    }

// ---------------------------------------------------------------------------
// CSVPUtility::CheckRtcpSettingL
// ---------------------------------------------------------------------------
//
void CSVPUtility::CheckRtcpSettingL( TUint32 aRtcp,
    CMceMediaStream& aStream, TBool aSessionUpdateOngoing ) const
    {
    SVPDEBUG2( "CSVPUtility::CheckRtcpSettingL aRtcp: %u", aRtcp );
    
    TBool rtcpsuppression = ETrue;
    if ( CRCSEProfileEntry::EOn == aRtcp )
        {
        SVPDEBUG1("CSVPUtility::CheckRtcpSettingL RTCP=ON");
        rtcpsuppression = EFalse;
        }
    
    // At this point we should have only audio streams. Thus the leave.
    if ( KMceAudio == aStream.Type() )
        {
        if ( !aStream.Source()->IsEnabled() && aSessionUpdateOngoing )
            {
            aStream.Source()->EnableL();
            }

        const RPointerArray<CMceMediaSink>& sinks = aStream.Sinks();
        const TInt snkCount = sinks.Count();
        for ( TInt k = 0; k < snkCount; k++ )
            {

            if ( !sinks[k]->IsEnabled()&& aSessionUpdateOngoing )
                {
                sinks[k]->EnableL();
                }

            if ( KMceRTPSink == sinks[k]->Type() )
                {
                SVPDEBUG2( "CSVPUtility::CheckRtcpSettingL updating: %d", k );
                static_cast<CMceRtpSink*>(
                    sinks[k] )->UpdateL( rtcpsuppression );
                SVPDEBUG1( "CSspUtility::CheckRtcpSettingL update done" );
                }
            }
        }
    else
        {
        SVPDEBUG1( "CSspUtility::CheckRtcpSettingL KErrArgument" );
        User::Leave( KErrArgument );
        }
    
    SVPDEBUG1( "CSspUtility::CheckRtcpSettingL Exit" );
    }

// ---------------------------------------------------------------------------
// CSVPUtility::SetVADL
// ---------------------------------------------------------------------------
//
void CSVPUtility::SetVADL( CMceAudioStream& aInAudioStream,
                           TBool aVADSetting,
                           const TDesC8& aCodecName )
    {
    SVPDEBUG1( "CSVPUtility::SetVADL In" )

    __ASSERT_ALWAYS( &aCodecName, User::Leave( KErrArgument ) );

    if ( &aInAudioStream )
        {
        CMceAudioCodec* codec = SVPAudioUtility::FindCodec( aInAudioStream, aCodecName );
        if ( codec )
            {
            SVPDEBUG2( "CSVPUtility aInAudioStream and codec valid, setting VAD with %d", aVADSetting )
            codec->EnableVAD( aVADSetting );
            }
        }

    SVPDEBUG1( "CSVPUtility::SetVADL Out" )
    }

// ---------------------------------------------------------------------------
// CSVPUtility::SetAmrWbParameters
// ---------------------------------------------------------------------------
//
void CSVPUtility::SetAmrWbParameters( CMceAudioStream& aAudioInStream, 
                                    const CRCSEAudioCodecEntry& aCodecProfile,
                                    TInt aCodecNumber )
    {
    SVPDEBUG1( "CSVPUtility::SetAmrWbParameters In" )
    
    TUint allowedBitrates( 0 );
    
    // Set correct mode-set
    const RArray< TUint32 >& modeSetArray = aCodecProfile.iModeSet;
    for ( TInt i = 0; i < modeSetArray.Count(); i++ )
        {
        TUint value = modeSetArray[ i ];
        switch ( value )
            {
            case 0: { allowedBitrates |= KMceAllowedAmrWbBitrate660;  break; }
            case 1: { allowedBitrates |= KMceAllowedAmrWbBitrate885;  break; }
            case 2: { allowedBitrates |= KMceAllowedAmrWbBitrate1265;  break; }
            case 3: { allowedBitrates |= KMceAllowedAmrWbBitrate1425;  break; }
            case 4: { allowedBitrates |= KMceAllowedAmrWbBitrate1585;  break; }
            case 5: { allowedBitrates |= KMceAllowedAmrWbBitrate1825;  break; }
            case 6: { allowedBitrates |= KMceAllowedAmrWbBitrate1985;  break; }
            case 7: { allowedBitrates |= KMceAllowedAmrWbBitrate2305;  break; }
            case 8: { allowedBitrates |= KMceAllowedAmrWbBitrate2305;  break; }
            case 9: { allowedBitrates |= KMceAllowedAmrWbBitrate2385;  break; }
            default: { allowedBitrates |= KMceAllowedAmrWbBitrateAll;  break; }
            }

        SVPDEBUG3( "SetAmrWbParameters round %d, value: %u",
            i, value )
        SVPDEBUG2( "SetAmrWbParameters allowedBitrates 0x%x",
            allowedBitrates )
        }

    SVPDEBUG2( "SetAmrWbParameters mode-set 0x%x", allowedBitrates )
    
    // Set correct mode-change-period (mode-change-period=1):
    if ( 2 == aCodecProfile.iModeChangePeriod )
        {
        allowedBitrates |= KMceAllowedAmrWbModeChangePeriod2;
        }

    // Set correct mode-change-neighbor (mode-change-neighbor=0)
    if ( CRCSEAudioCodecEntry::EOn == aCodecProfile.iModeChangeNeighbor )
        {
        allowedBitrates |= KMceAllowedAmrWbModeChangeNeighbor1;
        }

    aAudioInStream.Codecs()[ aCodecNumber ]->SetAllowedBitrates(
        allowedBitrates );

    SVPDEBUG2( "CSVPUtility::SetAmrWbParameters Out, 0x%x", allowedBitrates )
    }

// EOF
