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
* Description:  Holds transiently and stores one VoIP settings item.
*
*/


// INCLUDE FILES
#include <e32std.h>
#include    <WPVoIPAdapter.rsg>
#include    <charconv.h>
#include    <in_sock.h>                // TInetAddr, TSockAddr
#include    <crcseprofileentry.h>
#include    <crcseprofileregistry.h>
#include    <crcseaudiocodecentry.h>
#include    <crcseaudiocodecregistry.h>
#include    <CWPCharacteristic.h>
#include    <sysutil.h>            // For GetWlanMacAddressL.
#include    <wlaninternalpskeys.h> // For GetWlanMacAddressL.
#include    <centralrepository.h>
#include    <settingsinternalcrkeys.h>
#include <coemain.h> // For GetPhoneModelL.
#include <WPAdapterUtil.h>
#include <sipmanagedprofile.h>
#include <sipprofileregistryobserver.h>
#include <sipmanagedprofileregistry.h>
#include <spsettings.h>     // for SPS
#include <spentry.h>        // for SPS
#include <spproperty.h>     // for SPS
#include <spdefinitions.h> 
#include <cmmanagerext.h>
#include <cmdestinationext.h>
#include <cmconnectionmethodext.h>
#include <cmconnectionmethoddef.h>
#include <cmpluginwlandef.h> // For WLAN plugin UID, i.e. bearer type.
#include <versit.h>
#include <cvimpstsettingsstore.h> // For IM tone path
#include <pathinfo.h> // For getting phone rom root path.

#include "CWPVoIPItem.h"
#include "CSIPProfileRegistryObserver.h"
#include "cipappphoneutils.h"           // For GetPhoneModelL, GetTerminalTypeL
#include "cipapputilsaddressresolver.h"    // For GetWlanMacAddressL

// LOCAL CONSTANTS
static const TInt32 KNotSaved( -1 );
static const TInt KMaxCharsInTUint32( 10 );
const TInt32 KCCHPresenceSubServicePlugId = 0x1027545A;
_LIT8( KUserAgent,        "User-Agent" );
_LIT8( KColonMark,        ":" );
// Default IM message tone
_LIT( KDefaultTone,       "Message 2.aac" );
_LIT8( KSpaceMark,        " " );
_LIT( KVoIPAdapterName,   "WPVoIPAdapter");
_LIT( KOpenParenthesis,   "(" );
_LIT( KClosedParenthesis, ")" );
const TInt KTwo( 2 );
const TInt KVmbxResubscribeDefault = 600;

// IM related constants.
const TUint32 KIMSubServicePluginId = 0x1027545A; // KSIPConnectivityPluginImplUid
const TUint32 KIMLaunchUid          = 0x200255D0;
const TInt    KIMSettingsId         = 1;

// Following lines are for enabling debug prints.
#ifdef _DEBUG
#define DBG_PRINT(p) RDebug::Print(_L(p))
#define DBG_PRINT2(p,a) RDebug::Print(_L(p),a)
#else
#define DBG_PRINT(p)
#define DBG_PRINT2(p,a)
#endif // _DEBUG

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// CWPVoIPItem::CWPVoIPItem
// 
// ---------------------------------------------------------------------------
//
CWPVoIPItem::CWPVoIPItem()
    {
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::NewL
// 
// ---------------------------------------------------------------------------
//
CWPVoIPItem* CWPVoIPItem::NewL() 
    {
    DBG_PRINT( "CWPVoIPItem::NewL - begin" );
    CWPVoIPItem* self = new (ELeave) CWPVoIPItem;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    DBG_PRINT( "CWPVoIPItem::NewL - end" );
    return self;
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::ConstructL()
// 
// ---------------------------------------------------------------------------
//
void CWPVoIPItem::ConstructL()
    {
    // Initialization with empty string or KNotSaved or -1 or if default 
    // value exists with default values.
    iItemId = TUint( KNotSaved );
    TFileName fileName;
    Dll::FileName(fileName);

    // Default provider value read from the resource file.
    iProviderId = WPAdapterUtil::ReadHBufCL( fileName, 
        KVoIPAdapterName, R_QTN_SM_PROVIDER_VOIP );
    // Default name value read from the resource file.
    iName = WPAdapterUtil::ReadHBufCL( fileName, KVoIPAdapterName, 
        R_QTN_SM_HEAD_VOIP );

    iStartMediaPort            = KNotSet;
    iEndMediaPort              = KNotSet;
    iMediaQoS                  = KNotSet;
    iDTMFInBand                = KNotSet;
    iDTMFOutBand               = KNotSet;
    iSavedProfileId            = HBufC8::NewL( 0 );
    iSecureCallPref            = KNotSet;
    iRTCP                      = KNotSet;
    iUAHTerminalType           = KNotSet;
    iUAHWLANMAC                = KNotSet;
    iUAHString                 = HBufC::NewL( 0 );
    iProfileLockedToIAP        = KNotSet;
    iVoIPPluginUid             = KNotSet;
    iAllowVoIPOverWCDMA        = KNotSet;
    iVoIPDigits                = KNotSet;
    iDomainPartIgnoreRule      = KNotSet;
    iAddUserPhoneToAllNumbers  = KNotSet;
    iSIPConnTestAddress        = HBufC::NewL( 0 );
    iServiceProviderBookmark   = HBufC::NewL( 0 );
    iSIPMinSE                  = KNotSet;
    iSIPSessionExpires         = KNotSet;
    iIPVoiceMailBoxURI         = HBufC::NewL( 0 );
    iVmbxListenAddress         = HBufC::NewL( 0 );
    iAutoAcceptBuddyRequest    = KNotSet;
    iBrandingDataAddress       = HBufC::NewL( 0 );
    iToNapId                   = HBufC8::NewL( 0 );
    iVoipUrisToAppRef          = HBufC8::NewL( 0 );
    iUsedNatProtocol           = KNotSet;
    iAppRef                    = HBufC8::NewL( 0 );
    iReSubscribeInterval       = KVmbxResubscribeDefault;
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::~CWPVoIPItem
//
// ---------------------------------------------------------------------------
//
CWPVoIPItem::~CWPVoIPItem()
    {
    DBG_PRINT( "CWPVoIPItem::~CWPVoIPItem - begin" );
    delete iProviderId;
    delete iName;
    iToAppRefs.ResetAndDestroy();
    iToAppRefs.Close();
    delete iSavedProfileId;
    delete iUAHString;
    delete iSIPConnTestAddress;
    delete iServiceProviderBookmark;
    delete iIPVoiceMailBoxURI;
    delete iVmbxListenAddress;
    delete iBrandingDataAddress;
    delete iToNapId;
    delete iVoipUrisToAppRef;
    delete iAppRef;

    iCodecs.ResetAndDestroy();
    iCodecs.Close();

    DBG_PRINT( "CWPVoIPItem::~CWPVoIPItem - end" );
    //lint -e{1740} iNapDef is not owned
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::SetAppRefL
//
// ---------------------------------------------------------------------------
//
void CWPVoIPItem::SetAppRefL( const TDesC8& aAppRef )
    {
    delete iAppRef;
    iAppRef = NULL;
    iAppRef = aAppRef.AllocL();
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::SetProviderIdL
//
// ---------------------------------------------------------------------------
//
void CWPVoIPItem::SetProviderIdL( const TDesC& aPROVIDERID )
    {
    delete iProviderId;
    iProviderId = NULL;
    iProviderId = aPROVIDERID.AllocL();
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::SetNameL
//
// ---------------------------------------------------------------------------
//
void CWPVoIPItem::SetNameL( const TDesC& aNAME )
    {
    delete iName;
    iName = NULL;
    iName = aNAME.AllocL();
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::AddToAppRefL
//
// ---------------------------------------------------------------------------
//
void CWPVoIPItem::AddToAppRefL( const TDesC8& aToAppRef )
    {
    // Dependencies to SIP, SCCP, NATFW and SNAP settings.
    
    // Must put string on cleanup stack in case memory alloc fails in AppendL.
    HBufC8* tmp = aToAppRef.AllocLC();
    iToAppRefs.AppendL( tmp );
    CleanupStack::Pop( tmp );
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::SetStartMediaPort
//
// ---------------------------------------------------------------------------
//
void CWPVoIPItem::SetStartMediaPort( TInt aStartMediaPort )
    {
    iStartMediaPort = aStartMediaPort;
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::SetEndMediaPort
//
// ---------------------------------------------------------------------------
//
void CWPVoIPItem::SetEndMediaPort( TInt aEndMediaPort )
    {
    iEndMediaPort = aEndMediaPort;
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::SetMediaQoS
//
// ---------------------------------------------------------------------------
//
void CWPVoIPItem::SetMediaQoS( TInt aMediaQoS )
    {
    iMediaQoS = aMediaQoS;
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::SetDTMFInBand
//
// ---------------------------------------------------------------------------
//
void CWPVoIPItem::SetDTMFInBand( TInt aDTMFInBand )
    {
    iDTMFInBand = aDTMFInBand;
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::SetDTMFOutOfBand
//
// ---------------------------------------------------------------------------
//
void CWPVoIPItem::SetDTMFOutBand( TInt aDTMFOutBand )
    {
    iDTMFOutBand = aDTMFOutBand;
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::AddCodec
//
// ---------------------------------------------------------------------------
//
void CWPVoIPItem::AddCodecL( const CWPVoIPCodec* aVoIPCodec )
    {
    iCodecs.AppendL( aVoIPCodec );
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::SetSecureCallPref
//
// ---------------------------------------------------------------------------
//
void CWPVoIPItem::SetSecureCallPref( TInt aSecureCallPref )
    {
    iSecureCallPref = aSecureCallPref;
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::SetRTCP
//
// ---------------------------------------------------------------------------
//
void CWPVoIPItem::SetRTCP( TInt aRTCP )
    {
    iRTCP = aRTCP;
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::SetUAHTerminalType
//
// ---------------------------------------------------------------------------
//
void CWPVoIPItem::SetUAHTerminalType( TInt aUAHTerminalType )
    {
    iUAHTerminalType = aUAHTerminalType;
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::SetUAHWLANMAC
//
// ---------------------------------------------------------------------------
//
void CWPVoIPItem::SetUAHWLANMAC( TInt aUAHWLANMAC )
    {
    iUAHWLANMAC = aUAHWLANMAC;
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::SetUAHStringL
//
// ---------------------------------------------------------------------------
//
void CWPVoIPItem::SetUAHStringL( const TDesC& aUAHString )
    {
    delete iUAHString;
    iUAHString = NULL;
    iUAHString = aUAHString.AllocL();
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::SetProfileLockedToIAP
//
// ---------------------------------------------------------------------------
//
void CWPVoIPItem::SetProfileLockedToIAP( TInt aProfileLockedToIAP )
    {
    iProfileLockedToIAP = aProfileLockedToIAP;
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::SetVoIPPluginUid
//
// ---------------------------------------------------------------------------
//
void CWPVoIPItem::SetVoIPPluginUid( TInt aVoIPPluginUid )
    {
    iVoIPPluginUid = aVoIPPluginUid;
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::SetAllowVoIPOverWCDMA
//
// ---------------------------------------------------------------------------
//
void CWPVoIPItem::SetAllowVoIPOverWCDMA( TInt aAllowVoIPOverWCDMA )
    {
    iAllowVoIPOverWCDMA = aAllowVoIPOverWCDMA;
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::SetVoIPDigits
//
// ---------------------------------------------------------------------------
//
void CWPVoIPItem::SetVoIPDigits( TInt aVoIPDigits )
    {
    iVoIPDigits = aVoIPDigits;
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::SetDomainPartIgnoreRule
//
// ---------------------------------------------------------------------------
//
void CWPVoIPItem::SetDomainPartIgnoreRule( TInt aDomainPartIgnoreRule )
    {
    iDomainPartIgnoreRule = aDomainPartIgnoreRule;
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::SetAddUserPhone
//
// ---------------------------------------------------------------------------
//
void CWPVoIPItem::SetAddUserPhone( TInt32 aAddUserPhone )
    {
    iAddUserPhoneToAllNumbers = aAddUserPhone;
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::SetSipConnTestAddress
//
// ---------------------------------------------------------------------------
//
void CWPVoIPItem::SetSipConnTestAddressL( const TDesC& aSIPConnTestAddress )
    {
    delete iSIPConnTestAddress;
    iSIPConnTestAddress = NULL;
    iSIPConnTestAddress = aSIPConnTestAddress.AllocL();
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::SetServiceProviderBookmarkL
//
// ---------------------------------------------------------------------------
//
void CWPVoIPItem::SetServiceProviderBookmarkL( 
    const TDesC& aServiceProviderBookmark )
    {
    delete iServiceProviderBookmark;
    iServiceProviderBookmark = NULL;
    iServiceProviderBookmark = aServiceProviderBookmark.AllocL();
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::SetSipMinSe
//
// ---------------------------------------------------------------------------
//
void CWPVoIPItem::SetSipMinSe( TInt32 aSIPMinSE )
    {
    iSIPMinSE = aSIPMinSE;
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::SetSipSessionExpires
//
// ---------------------------------------------------------------------------
//
void CWPVoIPItem::SetSipSessionExpires( TInt32 aSIPSessionExpires )
    {
    iSIPSessionExpires = aSIPSessionExpires;
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::SetIPVoiceMailBoxURIL
// 
// ---------------------------------------------------------------------------
void CWPVoIPItem::SetIPVoiceMailBoxURIL( const TDesC& aIPVoiceMailBoxURI )
    {
    delete iIPVoiceMailBoxURI;
    iIPVoiceMailBoxURI = NULL;
    iIPVoiceMailBoxURI = aIPVoiceMailBoxURI.AllocL();
    iVmbxSettingsExist = ETrue;
    }    

// ---------------------------------------------------------------------------
// CWPVoIPItem::SetVoiceMailBoxListenURIL
//
// ---------------------------------------------------------------------------
//
void CWPVoIPItem::SetVoiceMailBoxListenURIL( const TDesC& aVmbxListenAddress )
    {
    delete iVmbxListenAddress;
    iVmbxListenAddress = NULL;
    iVmbxListenAddress = aVmbxListenAddress.AllocL();
    iVmbxSettingsExist = ETrue;
    }
    
// ---------------------------------------------------------------------------
// CWPVoIPItem::SetReSubscribeInterval
//
// ---------------------------------------------------------------------------
//
void CWPVoIPItem::SetReSubscribeInterval( TInt32 aReSubscribeInterval )
    {
    iReSubscribeInterval = aReSubscribeInterval;
    iVmbxSettingsExist = ETrue;
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::SetBrandingDataAddressL
//
// ---------------------------------------------------------------------------
//
void CWPVoIPItem::SetBrandingDataAddressL( const TDesC& aBrandingDataAddress )
    {
    delete iBrandingDataAddress;
    iBrandingDataAddress = NULL;
    iBrandingDataAddress = aBrandingDataAddress.AllocL();
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::SetAutoAcceptBuddyRequest
//
// ---------------------------------------------------------------------------
//
void CWPVoIPItem::SetAutoAcceptBuddyRequest( TInt32 aAutoAcceptBuddyRequest )
    {
    iAutoAcceptBuddyRequest = aAutoAcceptBuddyRequest;
    }    
    
// ---------------------------------------------------------------------------
// CWPVoIPItem::SetAutoEnableService
//
// ---------------------------------------------------------------------------
//
void CWPVoIPItem::SetAutoEnableService( TInt32 aAutoEnableService )
    {
    iAutoEnableService = aAutoEnableService;
    }
    
// ---------------------------------------------------------------------------
// CWPVoIPItem::SetNapDef
//
// ---------------------------------------------------------------------------
//
void CWPVoIPItem::SetNapDef( CWPCharacteristic* aNapDef )
    {
    iNapDef = aNapDef;
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::SetToNapIdL
//
// ---------------------------------------------------------------------------
void CWPVoIPItem::SetToNapIdL( const TDesC8& aToNapId )
    {
    delete iToNapId;
    iToNapId = NULL;
    iToNapId = aToNapId.AllocL();
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::SetIapId
//
// ---------------------------------------------------------------------------
//
void CWPVoIPItem::SetIapId( TUint32 aIapId )
    {
    iVmbxIapId = aIapId;
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::SetStorageId
//
// ---------------------------------------------------------------------------
//
TBool CWPVoIPItem::SetStorageId( TSettingsType aProfileType, 
    TUint32 aStorageId, const TDesC8& aAppRef )
    {
    DBG_PRINT( "CWPVoIPItem::SetStorageId - begin" );
    const TInt toAppRefCount = iToAppRefs.Count();
    TInt counter( KErrNone );

    //lint -e{961} No need for else statement here
    if ( ESIP == aProfileType )
        {
        for ( counter = 0; counter < toAppRefCount; counter++ )
            {
            if ( KErrNone == iToAppRefs[counter]->CompareF( aAppRef ) 
                && !iSipIdSet )
                {
                iSipId = aStorageId;
                iSipIdSet = ETrue;
                DBG_PRINT( "CWPVoIPItem::SetStorageId - SIP ID set; end" );
                return ETrue;
                }
            }
        if ( KErrNone == iVoipUrisToAppRef->CompareF( aAppRef ) )
            {
            iVmbxSipId = aStorageId;
            iVmbxSipIdSet = ETrue;
            DBG_PRINT( 
            	"CWPVoIPItem::SetStorageId - SIP ID for VMBX set; end" );
            return ETrue;
            }
        }

    else if ( ESCCP == aProfileType )
        {
        for ( counter = 0; counter < toAppRefCount; counter++ )
            {
            if ( KErrNone == iToAppRefs[counter]->CompareF( aAppRef ) 
                && !iSccpIdSet )
                {
                iSccpId = aStorageId;
                iSccpIdSet = ETrue;
                DBG_PRINT( "CWPVoIPItem::SetStorageId - SCCP ID set; end" );
                return ETrue;
                }
            }
        }

    else if ( ENATFW == aProfileType )
        {
        for ( counter = 0; counter < toAppRefCount; counter++ )
            {
            if ( KErrNone == iToAppRefs[counter]->CompareF( aAppRef ) 
                && !iNatFwIdSet )
                {
                iNatFwId = aStorageId;
                iNatFwIdSet = ETrue;
                DBG_PRINT(
                    "CWPVoIPItem::SetStorageId - NAT/FW ID set; end" );
                return ETrue;
                }
            }
        }

    else if ( EPresence == aProfileType )
        {
        for ( counter = 0; counter < toAppRefCount; counter++ )
            {
            if ( KErrNone == iToAppRefs[counter]->CompareF( aAppRef )
                && !iPresenceIdSet )
                {
                iPresenceId = aStorageId;
                iPresenceIdSet = ETrue;
                DBG_PRINT(
                    "CWPVoIPItem::SetStorageId - Presence ID set; end" );
                return ETrue;
                }
            }
        }
        
    else if ( ESNAP == aProfileType )
        {
        for ( counter = 0; counter < toAppRefCount; counter++ )
            {
            if ( KErrNone == iToAppRefs[counter]->CompareF( aAppRef ) 
                && !iSnapIdSet )
                {
                iSnapId = aStorageId;
                iSnapIdSet = ETrue;
                DBG_PRINT( "CWPVoIPItem::SetStorageId - SNAP ID set; end" );
                return ETrue;
                }
            }
        }

    return EFalse;
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::SetVoipUrisToAppRefL
//
// ---------------------------------------------------------------------------
//
void CWPVoIPItem::SetVoipUrisToAppRefL( const TDesC8& aToAppRef )
    {
    // VMBX dependency to SIP settigns, VoIP dependency to Presence settings.
    delete iVoipUrisToAppRef;
    iVoipUrisToAppRef = NULL;
    iVoipUrisToAppRef = aToAppRef.AllocL();
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::SetUsedNatProtocol
//
// ---------------------------------------------------------------------------
//
void CWPVoIPItem::SetUsedNatProtocol( TInt32 aUsedNatProtocol)
    {
    iUsedNatProtocol = aUsedNatProtocol;
    }    

// Getters

// ---------------------------------------------------------------------------
// CWPVoIPItem::Name
//
// ---------------------------------------------------------------------------
//
const TDesC& CWPVoIPItem::Name() const
    {
    return *iName;
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::UAHString
//
// ---------------------------------------------------------------------------
//
const TDesC& CWPVoIPItem::UAHString() const
    {
    return *iUAHString;
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::ToNapId
//
// ---------------------------------------------------------------------------
//
const TDesC8& CWPVoIPItem::ToNapId() const
    {
    return *iToNapId;
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::NapDef
//
// ---------------------------------------------------------------------------
//
CWPCharacteristic* CWPVoIPItem::NapDef()
    {
    return iNapDef;
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::VoipUrisToAppRef
//
// ---------------------------------------------------------------------------
//
const TDesC8& CWPVoIPItem::VoipUrisToAppRef() const
    {
    return *iVoipUrisToAppRef;
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::StoreL
// 1) Set VoIP parameters into the VoIP profile entry.
// 2) Save audio codecs and add their storage ids into the VoIP profile entry
//    and VoIP item.
// 3) Save the VoIP profile entry.
// SIP ID's, SIP profile specific data, SCCP ID & Presence ID are not stored
// here, nor are VoIP settings belonging to SPS. They are all stored when the
// SavingFinalized method is called because only then it is guaranteed that
// the data needed is received and ready for saving.
// ---------------------------------------------------------------------------
//
TUint CWPVoIPItem::StoreL()
    {
    DBG_PRINT( "CWPVoIPItem::StoreL - begin" );
    // Save the VoIP profile.
    CRCSEProfileEntry* cRCSEProfileEntry = CRCSEProfileEntry::NewLC(); // CS:1
    cRCSEProfileEntry->iId = iItemId;
    cRCSEProfileEntry->iProviderName.Copy( iProviderId->Des() );
    cRCSEProfileEntry->iSettingsName.Copy( iName->Des() );

    // Saving the codecs and and storaging their ids in VoIP profile.
    typedef CRCSEAudioCodecEntry AudioCodecEntry;
    CRCSEAudioCodecRegistry* cRCSEAudioCodecRegistry = 
        CRCSEAudioCodecRegistry::NewLC(); // CS:2

    CWPVoIPCodec* tmpCodec = NULL;
    const TInt numberOfCodecs = iCodecs.Count();

    for ( TInt codecIndex = 0; codecIndex < numberOfCodecs; codecIndex++ )
        {
        tmpCodec = iCodecs[codecIndex];
        CRCSEAudioCodecEntry* cRCSEAudioCodecEntry = 
            CRCSEAudioCodecEntry::NewLC(); // CS:3

        // Codec initiated with it's default values.
        cRCSEAudioCodecEntry->SetDefaultCodecValueSet( tmpCodec->
            iMediaSubtypeName->Des() );

        cRCSEAudioCodecEntry->iMediaSubTypeName.Zero();
        cRCSEAudioCodecEntry->iMediaSubTypeName.Copy( tmpCodec->
            iMediaSubtypeName->Des() );

        // Set codec values, take values only when they exist.

        // JitterBufferSize
        if ( KNotSet != tmpCodec->iJitterBufferSize )
            {
            cRCSEAudioCodecEntry->iJitterBufferSize = 
                tmpCodec->iJitterBufferSize;
            }

        // OctetAlign
        if ( KNotSet != tmpCodec->iOctetAlign )
            {
            cRCSEAudioCodecEntry->iOctetAlign = 
                static_cast<AudioCodecEntry::TOnOff>
                ( tmpCodec->iOctetAlign );
            }

        // ModeSet
        TInt modeSetCount = iCodecs[codecIndex]->iModeSet.Count();
        if ( modeSetCount > 0 )
            {
            // Take off default values if some values are provisioned.
            cRCSEAudioCodecEntry->iModeSet.Reset();
            for ( TInt modeSetIndex = 0; modeSetIndex < modeSetCount; 
                modeSetIndex++ )
                {
                cRCSEAudioCodecEntry->iModeSet.Append( iCodecs[codecIndex]->
                    iModeSet[modeSetIndex] );
                }// for
            }// if

        // ModeChangePeriod
        if ( KNotSet != tmpCodec->iModeChangePeriod )
            {
            cRCSEAudioCodecEntry->iModeChangePeriod = 
                tmpCodec->iModeChangePeriod;
            }

        // ModeChangeNeighbor
        if ( KNotSet != tmpCodec->iModeChangeNeighbor )
            {
            cRCSEAudioCodecEntry->iModeChangeNeighbor = 
                static_cast<AudioCodecEntry::TOnOff>
                ( tmpCodec->iModeChangeNeighbor );
            }

        // PTime    
        if ( KNotSet != tmpCodec->iPTime )
            {
            cRCSEAudioCodecEntry->iPtime = tmpCodec->iPTime;
            }

        // MaxPTime
        if ( KNotSet != tmpCodec->iMaxPTime )
            {
            cRCSEAudioCodecEntry->iMaxptime = tmpCodec->iMaxPTime;
            }

        // VAD
        if ( KNotSet != tmpCodec->iVAD )
            {
            cRCSEAudioCodecEntry->iVAD = 
                static_cast<AudioCodecEntry::TOnOff>( tmpCodec->iVAD );
            }

        // AnnexB
        if ( KNotSet != tmpCodec->iAnnexB )
            {
            cRCSEAudioCodecEntry->iAnnexb = 
                static_cast<AudioCodecEntry::TOnOff>( tmpCodec->iAnnexB );
            }

        // MaxRed
        if ( KNotSet != tmpCodec->iMaxRed )
            {
            cRCSEAudioCodecEntry->iMaxRed = tmpCodec->iMaxRed;
            }
        // Simple check, that there is at least media sub type name.
        // If not ok, then the codec is not saved.
        if ( tmpCodec->iMediaSubtypeName->Length() > 0 )
            {
            // Store the codec entry and get the storage id.
            tmpCodec->iId = cRCSEAudioCodecRegistry->AddL( 
                *cRCSEAudioCodecEntry );
            }// if

        CleanupStack::PopAndDestroy( cRCSEAudioCodecEntry ); // CS:2
        }// for

    CleanupStack::PopAndDestroy ( cRCSEAudioCodecRegistry ); // CS:1

    if ( numberOfCodecs > 1 )
        {
        // Sort the codec array into priority order (by PRIORITYINDEX).
        RArray<TInt> priorityArray;
        for ( TInt codecIndex = 0; codecIndex < numberOfCodecs; codecIndex++ )
            {
            priorityArray.Append( iCodecs[codecIndex]->iPriorityIndex );
            }
        priorityArray.Sort();
        RPointerArray<CWPVoIPCodec> tmpArray;

        for ( TInt priorityIndex = 0; priorityIndex < numberOfCodecs; 
            priorityIndex++ )
            {
            for ( TInt codecIndex = 0; codecIndex < numberOfCodecs; 
                codecIndex++ )
                {
                if ( priorityArray[priorityIndex] == 
                    iCodecs[codecIndex]->iPriorityIndex )
	            
                    {
                    tmpArray.Append( iCodecs[codecIndex] );
                    }
                }
            }

        for ( TInt codecIndex = 0; codecIndex < numberOfCodecs; codecIndex++ )
            {
            iCodecs[codecIndex] = tmpArray[codecIndex];
            }
        tmpArray.Close();
        priorityArray.Close();
        } // if

    // Set the priority list of codecs.
    for ( TInt codecIndex = 0; codecIndex < numberOfCodecs; codecIndex++ )
        {
        cRCSEProfileEntry->iPreferredCodecs.Append( 
            iCodecs[codecIndex]->iId );
        }// for

    // If no codecs exist create default codecs
    if ( !numberOfCodecs )
        {
        AddDefaultCodecsL( *cRCSEProfileEntry );
        }

    // StartMediaPort
    if ( KNotSet != iStartMediaPort )
        {
        cRCSEProfileEntry->iStartMediaPort = iStartMediaPort;
        }

    // EndMediaPort
    if ( KNotSet != iEndMediaPort )
        {
        cRCSEProfileEntry->iEndMediaPort = iEndMediaPort;
        }

    // MediaQoS
    if ( KNotSet != iMediaQoS )
        {
        cRCSEProfileEntry->iMediaQOS = iMediaQoS;
        }

    // InbandDTMF
    if ( KNotSet != iDTMFInBand )
        {
        cRCSEProfileEntry->iInbandDTMF = 
            static_cast<VoIPProfileEntry::TOnOff>( iDTMFInBand );
        }

    // OutbandDTMF
    if ( KNotSet != iDTMFOutBand )
        {
        cRCSEProfileEntry->iOutbandDTMF = 
            static_cast<VoIPProfileEntry::TOnOff>( iDTMFOutBand );
        }

    // SecureCallPref
    if ( KNotSet != iSecureCallPref )
        {
        cRCSEProfileEntry->iSecureCallPreference = iSecureCallPref;
        }

    // RTCP
    if ( KNotSet != iRTCP )
        {
        cRCSEProfileEntry->iRTCP = iRTCP;
        }

    // UAHTerminalType
    if ( KNotSet != iUAHTerminalType )
        {
        cRCSEProfileEntry->iSIPVoIPUAHTerminalType = iUAHTerminalType;
        }

    // UAHWLANMAC
    if ( KNotSet != iUAHWLANMAC )
        {
        cRCSEProfileEntry->iSIPVoIPUAHeaderWLANMAC = iUAHWLANMAC;
        }

    // UAHString
    if ( iUAHString->Des().Length() > 0 )
        {
        cRCSEProfileEntry->iSIPVoIPUAHeaderString.Zero();
        cRCSEProfileEntry->iSIPVoIPUAHeaderString.Insert( 0, 
            iUAHString->Des() );
        }

    // ProfileLockedToIAP
    if ( KNotSet != iProfileLockedToIAP )
        {
        cRCSEProfileEntry->iProfileLockedToIAP = 
            static_cast<VoIPProfileEntry::TOnOff>( iProfileLockedToIAP );
        }

    // VoIPPluginUid
    if ( KNotSet != iVoIPPluginUid )
        {
        cRCSEProfileEntry->iVoIPPluginUID = iVoIPPluginUid;
        }

    // AllowVoIPOverWCDMA
    if ( KNotSet != iAllowVoIPOverWCDMA )
        {
        cRCSEProfileEntry->iAllowVoIPoverWCDMA = 
            static_cast<VoIPProfileEntry::TOnOff>( iAllowVoIPOverWCDMA );
        }

    // VoIPDigits
    if ( KNotSet != iVoIPDigits )
        {
        cRCSEProfileEntry->iMeanCountOfVoIPDigits = iVoIPDigits;
        }

    // DomainPartIgnoreRule
    if ( KNotSet != iDomainPartIgnoreRule )
        {
        cRCSEProfileEntry->iIgnoreAddrDomainPart = iDomainPartIgnoreRule;
        }

    // UserPhoneToAllNumbers
    if ( KNotSet != iAddUserPhoneToAllNumbers )
        {
        cRCSEProfileEntry->iUserPhoneUriParameter = 
            static_cast<VoIPProfileEntry::TOnOff>
            ( iAddUserPhoneToAllNumbers );
        }

    // SIPConnTestAddress
    if ( iSIPConnTestAddress->Des().Length() > 0 )
        {
        cRCSEProfileEntry->iSIPConnTestAddress.Zero();
        cRCSEProfileEntry->iSIPConnTestAddress.Insert( 0, 
            iSIPConnTestAddress->Des() );
        }

    // SIPMinSE
    if ( KNotSet != iSIPMinSE )
        {
        cRCSEProfileEntry->iSIPMinSE = iSIPMinSE;
        }

    // SIPSessionExpires
    if ( KNotSet != iSIPSessionExpires )
        {
        cRCSEProfileEntry->iSIPSessionExpires = iSIPSessionExpires;
        }

    // UsedNATProtocol
    if ( KNotSet != iUsedNatProtocol )
        {
        cRCSEProfileEntry->iNATProtocol = iUsedNatProtocol;
        }                

    if ( iSipIdSet )
        {
        TSettingIds referredIds;
        referredIds.iProfileType = 0; // SIP.
        referredIds.iProfileId = TInt( iSipId );
        // Never reference to profile specific settings.
        referredIds.iProfileSpecificSettingId = KNotSet; 
        cRCSEProfileEntry->iIds.Append( referredIds );
        }

    // Save the new profile into the registry.
    CRCSEProfileRegistry* cRCSEProfileRegistry = 
        CRCSEProfileRegistry::NewLC();
    // CS:2
    CheckDuplicatesL( *cRCSEProfileEntry );
    iItemId = cRCSEProfileRegistry->AddL( *cRCSEProfileEntry );

    CleanupStack::PopAndDestroy( cRCSEProfileRegistry ); // CS:1
    CleanupStack::PopAndDestroy( cRCSEProfileEntry ); // CS:0

    // __UHEAP_MARKEND; // for testing

    HBufC8* tmpId = HBufC8::NewL( KMaxCharsInTUint32 );
    _LIT8( KFormatTxt,"%u" );
    tmpId->Des().Format( KFormatTxt, iItemId );
    delete iSavedProfileId;
    iSavedProfileId = NULL;
    iSavedProfileId = tmpId;
    tmpId = NULL;
    DBG_PRINT( "CWPVoIPItem::StoreL - end" );
    return iItemId;
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::SaveData
//
// ---------------------------------------------------------------------------
//
const TDesC8& CWPVoIPItem::SaveData() const
    {
    return *iSavedProfileId;
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::SavingFinalizedL()
// 1) Save TO-APPREF dependent data and set SIP User Agent Header in SIP
//    profile.
// 2) Update the VoIP profile with references to the saved TO-APPREF dependent 
//    data.
// 3) Reset the data collections that are based on data received via call to
//    method SettingsSaved (i.e. APPREF related data). This is because the
//    user might for some reason save the settings twice and then the data is
//    received once again (there would be then dublicates of data).
//    
// ---------------------------------------------------------------------------
//
void CWPVoIPItem::SavingFinalizedL()
    {
    DBG_PRINT( "CWPVoIPItem::SavingFinalizedL - begin" );
    // Load the VoIP profile for update from the registry.
    CRCSEProfileRegistry* cRCSEProfileRegistry = 
        CRCSEProfileRegistry::NewLC();
    // CS:1
    CRCSEProfileEntry* cRCSEProfileEntry = CRCSEProfileEntry::NewLC(); // CS:2
    cRCSEProfileRegistry->FindL( iItemId, *cRCSEProfileEntry );

    // Data dependent on the other adapters can be saved here, because it is 
    // now quaranteed that TO-APPREF pointed information is known.
    // Test if this VoIP settings item of type SIP or SCCP dependent.

    // Save RCSE related data.
    //lint -e{961} No need for else statement here
    if ( iSccpIdSet )
        {
        TSettingIds referredIds;
        referredIds.iProfileType = 1; // SCCP.
        referredIds.iProfileId = TInt( iSccpId );
        // Never reference to profile specific settings.
        referredIds.iProfileSpecificSettingId = KNotSet; 
        cRCSEProfileEntry->iIds.Append( referredIds );
        cRCSEProfileRegistry->UpdateL( iItemId, *cRCSEProfileEntry );
        DBG_PRINT( "CWPVoIPItem::SavingFinalizedL - SCCP ID set" );
        }

    if ( iNatFwIdSet )
        {
        cRCSEProfileEntry->iNATSettingsStorageId = iNatFwId;
        cRCSEProfileRegistry->UpdateL( iItemId, *cRCSEProfileEntry );
        DBG_PRINT( "CWPVoIPItem::SavingFinalizedL - NAT/FW ID set" );
        }

    // Save Service Provider Settings related data if service ID exists.
    TUint32 serviceId = cRCSEProfileEntry->iServiceProviderId;
    if ( 0 == serviceId )
        {
        CleanupStack::PopAndDestroy( 2, cRCSEProfileRegistry );
        DBG_PRINT( "CWPVoIPItem::SavingFinalizedL - end" );
        return;
        }
    CSPSettings* spSettings = CSPSettings::NewLC(); // CS:3
    CSPEntry* spEntry = CSPEntry::NewLC(); // CS:4

    TInt err = spSettings->FindEntryL( serviceId, *spEntry );
    User::LeaveIfError( err );

    CSPProperty* property = CSPProperty::NewLC(); // CS:5

    // VoiceMailBox: SIP ID
    //lint -e{961} No need for else statement here
    if ( iVmbxSipIdSet )
        {
        err = property->SetName( ESubPropertyVMBXSettingsId );
        property->SetValue( iVmbxSipId );
        if ( KErrNone == err )
            {
            err = spEntry->AddPropertyL( *property );
            if ( KErrAlreadyExists == err )
                {
                User::LeaveIfError( spEntry->UpdateProperty( 
                    ESubPropertyVMBXSettingsId, iVmbxSipId ) );
                }
            else
                {
                User::LeaveIfError( err );
                }
            }
        DBG_PRINT( "CWPVoIPItem::SavingFinalizedL - Vmbx SIP ID set" );
        }
    // Use the same SIP ID as in RCSE. Store only if some 
    // vmbx settings are provisioned.
    else if ( iSipIdSet && iVmbxSettingsExist )
        {
        err = property->SetName( ESubPropertyVMBXSettingsId );
        property->SetValue( iSipId );
        if ( KErrNone == err )
            {
            err = spEntry->AddPropertyL( *property );
            if ( KErrAlreadyExists == err )
                {
                User::LeaveIfError( spEntry->UpdateProperty( 
                    ESubPropertyVMBXSettingsId, iSipId ) );
                }
            else
                {
                User::LeaveIfError( err );
                }
            }
        DBG_PRINT( "CWPVoIPItem::SavingFinalizedL - \
            Vmbx SIP ID set (same SIP ID as in RCSE)" );
        }
        
    // VoiceMailBox: MWI-URI
    if ( iIPVoiceMailBoxURI->Des().Length() > 0 )
        {
        err = property->SetName( ESubPropertyVMBXMWIAddress );
        property->SetValue( iIPVoiceMailBoxURI->Des() );
        if ( KErrNone == err )
            {
            err = spEntry->AddPropertyL( *property );
            if ( KErrAlreadyExists == err )
                {
                User::LeaveIfError( spEntry->UpdateProperty( 
                    ESubPropertyVMBXMWIAddress, 
                    iIPVoiceMailBoxURI->Des() ) );
                }
            else
                {
                User::LeaveIfError( err );
                }
            }
        DBG_PRINT( "CWPVoIPItem::SavingFinalizedL - Vmbx MWI-URI set" );
        }

    // VoiceMailBox: Listening URI
    if ( !iVmbxListenAddress->Des().Length() 
        && iIPVoiceMailBoxURI->Des().Length() )
        {
        // Use same URI as with MWI-URI.
        delete iVmbxListenAddress;
        iVmbxListenAddress = NULL;
        iVmbxListenAddress = iIPVoiceMailBoxURI->Des().AllocL();
        }

    if ( iVmbxListenAddress->Des().Length() > 0 )
        {        
        err = property->SetName( ESubPropertyVMBXListenAddress );
        property->SetValue( iVmbxListenAddress->Des() );
        if ( KErrNone == err )
            {
            err = spEntry->AddPropertyL( *property );
            if ( KErrAlreadyExists == err )
                {
                User::LeaveIfError( spEntry->UpdateProperty( 
                    ESubPropertyVMBXListenAddress, 
                    iVmbxListenAddress->Des() ) );
                }
            else
                {
                User::LeaveIfError( err );
                }
            }
        DBG_PRINT(
            "CWPVoIPItem::SavingFinalizedL - Vmbx Listening URI set" );
        }        

    // VoiceMailBox: Re-Subscribe interval
    if ( iVmbxSettingsExist )
        {
        err = property->SetName( ESubPropertyVMBXMWISubscribeInterval );
        property->SetValue( iReSubscribeInterval );
        if ( KErrNone == err )
            {
            err = spEntry->AddPropertyL( *property );
            if ( KErrAlreadyExists == err )
                {
                User::LeaveIfError( spEntry->UpdateProperty( 
                    ESubPropertyVMBXMWISubscribeInterval, 
                    iReSubscribeInterval ) );
                }
            else
                {
                User::LeaveIfError( err );
                }
            }
        DBG_PRINT( "CWPVoIPItem::SavingFinalizedL - \
            Vmbx re-SUBSCRIBE interval set" );
        }

    // VoiceMailBox: Preferred IAP ID
    if ( iVmbxIapId )
        {
        err = property->SetName( ESubPropertyVMBXPreferredIAPId );
        property->SetValue( iVmbxIapId );
        if ( KErrNone == err )
            {
            err = spEntry->AddPropertyL( *property );
            if ( KErrAlreadyExists == err )
                {
                User::LeaveIfError( spEntry->UpdateProperty( 
                    ESubPropertyVMBXPreferredIAPId, iVmbxIapId ) );
                }
            else
                {
                User::LeaveIfError( err );
                }
            }
        DBG_PRINT( "CWPVoIPItem::SavingFinalizedL - Vmbx IAP ID set" );
        }

    // Service provider bookmark URI
    if ( iServiceProviderBookmark->Des().Length() > 0 )
        {
        err = property->SetName( EPropertyServiceBookmarkUri );
        property->SetValue( iServiceProviderBookmark->Des() );
        if ( KErrNone == err )
            {
            err = spEntry->AddPropertyL( *property );
            if ( KErrAlreadyExists == err )
                {
                User::LeaveIfError( spEntry->UpdateProperty( 
                    EPropertyServiceBookmarkUri, 
                    iServiceProviderBookmark->Des() ) );
                }
            else
                {
                User::LeaveIfError( err );
                }
            }
        DBG_PRINT( "CWPVoIPItem::SavingFinalizedL - \
            Service provider bookmark URI set" );
        }

    // Branding data URI
    if ( iBrandingDataAddress->Des().Length() > 0 )
        {
        err = property->SetName( ESubPropertyVoIPBrandDataUri );
        property->SetValue( iBrandingDataAddress->Des() );
        if ( KErrNone == err )
            {
            err = spEntry->AddPropertyL( *property );
            if ( KErrAlreadyExists == err )
                {
                User::LeaveIfError( spEntry->UpdateProperty( 
                    ESubPropertyVoIPBrandDataUri, 
                    iBrandingDataAddress->Des() ) );
                }
            else
                {
                User::LeaveIfError( err );
                }
            }
        DBG_PRINT( "CWPVoIPItem::SavingFinalizedL - Branding data URI set" );
        }

    // Presence settings ID and Presence Subservice plug-in UID
    if ( iPresenceIdSet )
        {
        err = property->SetName( ESubPropertyPresenceSettingsId );
        property->SetValue( iPresenceId );
        if ( KErrNone == err )
            {
            err = spEntry->AddPropertyL( *property );
            if ( KErrAlreadyExists == err )
                {
                User::LeaveIfError( spEntry->UpdateProperty( 
                    ESubPropertyPresenceSettingsId, iPresenceId ) );
                }
            else
                {
                User::LeaveIfError( err );
                }
            }
        
        // Converged Connection Handler (CCH) Presence Subservice plug-in UID
        err = property->SetName( EPropertyPresenceSubServicePluginId );
        property->SetValue( KCCHPresenceSubServicePlugId );
        if ( KErrNone == err )
            {
            err = spEntry->AddPropertyL( *property );
            if ( KErrAlreadyExists == err )
                {
                User::LeaveIfError( spEntry->UpdateProperty( 
                    EPropertyPresenceSubServicePluginId, 
                    KCCHPresenceSubServicePlugId ) );
                }
            else
                {
                User::LeaveIfError( err );
                }
            }

        // ***************
        // IM settings
        // ***************
        //
        if ( iImEnabled )
            {
            property->SetName( ESubPropertyIMEnabled );
            property->SetValue( EOn );
            err = spEntry->AddPropertyL( *property );
            if ( KErrAlreadyExists == err )
                {
                User::LeaveIfError( spEntry->UpdateProperty( 
                    ESubPropertyIMEnabled, EOn ) );
                }

            property->SetName( ESubPropertyIMLaunchUid );
            property->SetValue( KIMLaunchUid );
            err = spEntry->AddPropertyL( *property );
            if ( KErrAlreadyExists == err )
                {
                User::LeaveIfError( spEntry->UpdateProperty( 
                    ESubPropertyIMLaunchUid, KIMLaunchUid ) );
                }

            property->SetName( ESubPropertyIMSettingsId );
            // The value only needs to be different from 0,
            // no-one actually uses it.
            property->SetValue( KIMSettingsId );
            err = spEntry->AddPropertyL( *property );
            if ( KErrAlreadyExists == err )
                {
                User::LeaveIfError( spEntry->UpdateProperty( 
                    ESubPropertyIMSettingsId, KIMSettingsId ) );
                }

            property->SetName( EPropertyIMSubServicePluginId );
            property->SetValue( KIMSubServicePluginId );
            err = spEntry->AddPropertyL( *property );
            if ( KErrAlreadyExists == err )
                {
                User::LeaveIfError( spEntry->UpdateProperty( 
                    EPropertyIMSubServicePluginId, KIMSubServicePluginId ) );
                }

            DBG_PRINT( "CWPVoIPItem::SavingFinalizedL - set IM tonepath" );

            MVIMPSTSettingsStore* settings =
                CVIMPSTSettingsStore::NewLC(); // CS: 1

            // Set default tone.
            TFileName toneFile;
            toneFile.Copy( PathInfo::RomRootPath() );
            toneFile.Append( PathInfo::DigitalSoundsPath() );
            toneFile.Append( KDefaultTone );
            User::LeaveIfError( settings->SetL( serviceId,
                EServiceToneFileName, toneFile ) );
            // settings
            CleanupStack::PopAndDestroy(); // CS: 0

            DBG_PRINT( "CWPVoIPItem::SavingFinalizedL - IM settings set" );
            }
        DBG_PRINT( 
        	"CWPVoIPItem::SavingFinalizedL - Presence settings ID set" );
        }

    // AutoAcceptBuddyRequest
    if ( KNotSet != iAutoAcceptBuddyRequest )
        {
        err = property->SetName( ESubPropertyPresenceRequestPreference );
        property->SetValue( static_cast<TOnOff> ( iAutoAcceptBuddyRequest ) );
        if ( KErrNone == err )
            {
            err = spEntry->AddPropertyL( *property );
            if ( KErrAlreadyExists == err )
                {
                User::LeaveIfError( spEntry->UpdateProperty( 
                    ESubPropertyPresenceRequestPreference, 
                    static_cast<TOnOff> ( iAutoAcceptBuddyRequest ) ) );
                }
            else
                {
                User::LeaveIfError( err );
                }
            }
        DBG_PRINT( "CWPVoIPItem::SavingFinalizedL - \
            AutoAcceptBuddyRequest set" );
        }
        
    // AutoEnableService
    if ( KNotSet != iAutoEnableService )
        {
        err = property->SetName( ESubPropertyVoIPEnabled );
        property->SetValue( static_cast<TOnOff>( iAutoEnableService ) );
        if ( KErrNone == err )
            {
            err = spEntry->AddPropertyL( *property );
            if ( KErrAlreadyExists == err )
                {
                User::LeaveIfError( spEntry->UpdateProperty( 
                    ESubPropertyVoIPEnabled, 
                    static_cast<TOnOff> ( iAutoEnableService ) ) );
                }
            else
                {
                User::LeaveIfError( err );
                }
            DBG_PRINT( "CWPVoIPItem::SavingFinalizedL - \
                voip AutoEnableService set" );
            }
        if ( iPresenceIdSet )
            {
            err = property->SetName( ESubPropertyPresenceEnabled );
            property->SetValue( 
                static_cast<TOnOff>( iAutoEnableService ) );
            if ( KErrNone == err )
                {
                err = spEntry->AddPropertyL( *property );
                if ( KErrAlreadyExists == err )
                    {
                    User::LeaveIfError( spEntry->UpdateProperty( 
                        ESubPropertyPresenceEnabled, 
                        static_cast<TOnOff> ( iAutoEnableService ) ) );
                    }
                else
                    {
                    User::LeaveIfError( err );
                    }
                DBG_PRINT( "CWPVoIPItem::SavingFinalizedL - \
                    presence AutoEnableService set" );
                }

            }
        if ( iVmbxSipIdSet || ( iSipIdSet && iVmbxSettingsExist ) 
            && iIPVoiceMailBoxURI->Des().Length() )    
            {
            err = property->SetName( ESubPropertyVMBXEnabled );
            property->SetValue( static_cast<TOnOff>( iAutoEnableService ) );
            if ( KErrNone == err )
                {
                err = spEntry->AddPropertyL( *property );
                if ( KErrAlreadyExists == err )
                    {
                    User::LeaveIfError( spEntry->UpdateProperty( 
                        ESubPropertyVMBXEnabled, 
                        static_cast<TOnOff> ( iAutoEnableService ) ) );
                    }
                else
                    {
                    User::LeaveIfError( err );
                    }
                }
                DBG_PRINT( "CWPVoIPItem::SavingFinalizedL - \
                    vmbx AutoEnableService set" );
            }
        
        if ( iImEnabled )
            {
            err = property->SetName( ESubPropertyIMEnabled );
            property->SetValue( static_cast<TOnOff>( iAutoEnableService ) );
            if ( KErrNone == err )
                {
                err = spEntry->AddPropertyL( *property );
                if ( KErrAlreadyExists == err )
                    {
                    User::LeaveIfError( spEntry->UpdateProperty( 
                        ESubPropertyIMEnabled, 
                        static_cast<TOnOff> ( iAutoEnableService ) ) );
                    }
                else
                    {
                    User::LeaveIfError( err );
                    }
                }
            DBG_PRINT( "CWPVoIPItem::SavingFinalizedL - \
            im AutoEnableService set" );         
            }
        }

    // SNAP ID
    if ( iSnapIdSet && iSipIdSet )
        {
        CSIPProfileRegistryObserver* sipObs = 
            CSIPProfileRegistryObserver::NewLC(); // CS:1
        CSIPManagedProfileRegistry* sipReg =
            CSIPManagedProfileRegistry::NewLC( *sipObs ); // CS:2
        CSIPProfile* sipProf = NULL;
        sipProf = sipReg->ProfileL( iSipId );
        CleanupStack::PushL( sipProf ); // CS:3
        CSIPManagedProfile* sipManagedProf = 
            static_cast<CSIPManagedProfile*>( sipProf );
        CleanupStack::PushL( sipManagedProf ); // CS:4
        sipManagedProf->SetParameter( KSIPSnapId, iSnapId );
        sipManagedProf->SetParameter( KSIPAccessPointId, (TUint32)0 );
        sipReg->SaveL( *sipManagedProf );
        CleanupStack::PopAndDestroy( sipManagedProf ); // CS:3
        CleanupStack::Pop( sipProf ); // CS:2
        sipProf = NULL;
        // sipReg, sipObs
        CleanupStack::PopAndDestroy( 2, sipObs ); // CS:0
        }
    else if ( !iSnapIdSet && iSipIdSet )
        {
        CSIPProfileRegistryObserver* sipObs = 
            CSIPProfileRegistryObserver::NewLC(); // CS:1
        CSIPManagedProfileRegistry* sipReg =
            CSIPManagedProfileRegistry::NewLC( *sipObs ); // CS:2
        CSIPProfile* sipProf = NULL;
        sipProf = sipReg->ProfileL( iSipId );
        CleanupStack::PushL( sipProf ); // CS:3
        CSIPManagedProfile* sipManagedProf = 
            static_cast<CSIPManagedProfile*>( sipProf );
        CleanupStack::PushL( sipManagedProf ); // CS:4

        TUint32 snapId( 0 );
        err = sipManagedProf->GetParameter( KSIPSnapId, snapId );
                
        if ( KErrNone == err )
            {
            iSnapId = snapId;
            iSnapIdSet = ETrue;
            }
        else if ( KErrNotFound == err )
            {
            TUint32 iapId( 0 );
            err = sipManagedProf->GetParameter( KSIPAccessPointId, iapId );
            
            if ( KErrNone == err )
                {
                // Copy connection method to default SNAP
                RCmManagerExt cmManager;
                CleanupClosePushL( cmManager );
                cmManager.OpenL();
                
                TCmDefConnValue defConn;
                cmManager.ReadDefConnL( defConn );
                iSnapId = defConn.iId;  
                iSnapIdSet = ETrue;
                
                RCmDestinationExt defaultSnap;
                CleanupClosePushL( defaultSnap );
                defaultSnap = cmManager.DestinationL( iSnapId );
                
                RCmConnectionMethodExt connection = 
                    cmManager.ConnectionMethodL( iapId );
                CleanupClosePushL( connection );
                
                // Get connection name
                HBufC* connectionName = 
                    connection.GetStringAttributeL( CMManager::ECmName );
                CleanupStack::PushL( connectionName );   
                
                RBuf parsedConnectionName;
                CleanupClosePushL( parsedConnectionName );
                parsedConnectionName.CreateL( connectionName->Des().Length() );
                
                // Parse possible unique number from end of connection
                // method name. accesspoint(xx) --> accesspoint
                TInt pos = connectionName->Des().Locate( '(' );
                if ( KErrNotFound != pos )
                    {
                    parsedConnectionName.Copy( 
                        connectionName->Des().Left( pos ) );
                    }
                else
                    {
                    parsedConnectionName.Copy( 
                        connectionName->Des() );
                    }  
                
                // Check if connection method already exists in default snap
                TBool matchFound( EFalse );
                TInt conMethodCount = defaultSnap.ConnectionMethodCount();
                
                for ( TInt i( 0 ) ; 
                    i < conMethodCount && matchFound == 0; i ++ )
                    {
                    RCmConnectionMethodExt cm = 
                        defaultSnap.ConnectionMethodL( i );
                    CleanupClosePushL( cm );
                        
                    HBufC* cmName = 
                        cm.GetStringAttributeL( CMManager::ECmName );
                    CleanupStack::PushL( cmName );    
   
                    RBuf parsedCmName;
                    CleanupClosePushL( parsedCmName );
                    parsedCmName.CreateL( cmName->Des().Length() );
                    
                    // Parse possible unique number from end of connection
                    // method name. accesspoint(xx) --> accesspoint
                    TInt pos = cmName->Des().Locate( '(' );
                    if ( KErrNotFound != pos )
                        {
                        parsedCmName.Copy( cmName->Des().Left( pos ) );
                        }
                    else
                        {
                        parsedCmName.Copy( cmName->Des() );
                        }   
                    
                    // Compare connection method names
                    if ( parsedConnectionName.Compare( parsedCmName ) == 0 )
                        {
                        matchFound = ETrue;
                        }

                    CleanupStack::PopAndDestroy( &parsedCmName );
                    CleanupStack::PopAndDestroy( cmName );
                    CleanupStack::PopAndDestroy( &cm );
                    }
                
                CleanupStack::PopAndDestroy( &parsedConnectionName );
                CleanupStack::PopAndDestroy( connectionName );
                
                // Add copy only if not already exists with same name
                if ( !matchFound )
                    {
                    defaultSnap.AddConnectionMethodL( 
                        connection.CreateCopyL() );
                    }
                
                // Change seamlessness level for linked WLAN IAP so that
                // roaming is allowed without asking it from the user.
                TUint32 bearerType = connection.GetIntAttributeL( 
                    CMManager::ECmBearerType );
                
                if ( KUidWlanBearerType == bearerType )
                    {
                    connection.SetIntAttributeL( 
                        CMManager::ECmSeamlessnessLevel, 
                        CMManager::ESeamlessnessShowprogress );
                    }
                
                defaultSnap.UpdateL();
                
                CleanupStack::PopAndDestroy( &connection );
                CleanupStack::PopAndDestroy( &defaultSnap );
                CleanupStack::PopAndDestroy( &cmManager );

                // Update sip profile to use default snap
                sipManagedProf->SetParameter( KSIPAccessPointId, (TUint32)0 );
                sipManagedProf->SetParameter( KSIPSnapId, iSnapId );
                sipReg->SaveL( *sipManagedProf );
                }
            }
        
        CleanupStack::PopAndDestroy( sipManagedProf ); // CS:3
        CleanupStack::Pop( sipProf ); // CS:2
        sipProf = NULL;
        // sipReg, sipObs
        CleanupStack::PopAndDestroy( 2, sipObs ); // CS:0
        }

    if ( iSnapIdSet )
        {
        err = property->SetName( ESubPropertyVoIPPreferredSNAPId );
        property->SetValue( iSnapId );
        if ( KErrNone == err )
            {
            err = spEntry->AddPropertyL( *property );
            if ( KErrAlreadyExists == err )
                {
                User::LeaveIfError( spEntry->UpdateProperty( 
                    ESubPropertyVoIPPreferredSNAPId, iSnapId ) );
                }
            else
                {
                User::LeaveIfError( err );
                }
            DBG_PRINT( "CWPVoIPItem::SavingFinalizedL - SNAP ID set" );
            }
        }

    // ***************
    // IM SNAP ID
    // ***************
    //
    if ( iImEnabled )
        {
        property->SetName( ESubPropertyIMPreferredSNAPId );
        err = property->SetValue( iSnapId );
        if ( KErrNone == err )
            {
            err = spEntry->AddPropertyL( *property );
           if ( KErrAlreadyExists == err )
                {
                User::LeaveIfError( spEntry->UpdateProperty( 
                    ESubPropertyIMPreferredSNAPId, iSnapId ) );
                }
            }
        }

    User::LeaveIfError( spSettings->UpdateEntryL( *spEntry ) );
    DBG_PRINT( "CWPVoIPItem::SavingFinalizedL - \
        Service provider entry updated" );

    // property, spEntry spSettings, cRCSEProfileEntry, cRCSEProfileRegistry
    CleanupStack::PopAndDestroy( 5, cRCSEProfileRegistry ); // CS:0
    iSipIdSet = EFalse;

    DBG_PRINT( "CWPVoIPItem::SavingFinalizedL - end" );
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::CheckDuplicatesL
// Checks if duplicate provider and settings name. Renames if same.
// ---------------------------------------------------------------------------
//
TBool CWPVoIPItem::CheckDuplicatesL( 
    CRCSEProfileEntry& aEntry ) const
    {
    DBG_PRINT( "CWPVoIPItem::CheckDuplicatesL - begin" );
    CRCSEProfileRegistry *cRCSEProfile = CRCSEProfileRegistry::NewLC();

    // Array of pointers to all profile entries.
    CArrayPtrFlat<CRCSEProfileEntry>* profileEntries = new (ELeave) 
        CArrayPtrFlat<CRCSEProfileEntry>( 1 );
    TCleanupItem cleanupItem( CWPVoIPItem::CleanupArrayItem, profileEntries );
    CleanupStack::PushL( cleanupItem );

    // Load profiles to pointerarray.
    RArray<TUint32> allIds;
    CleanupClosePushL( allIds );
    cRCSEProfile->GetAllIdsL( allIds ); 

    for ( TInt counter = 0; counter < allIds.Count(); counter++ )
        {
        CRCSEProfileEntry* profile = CRCSEProfileEntry::NewLC();
        cRCSEProfile->FindL( allIds[counter], *profile );
        profileEntries->AppendL( profile );
        CleanupStack::Pop( profile ); 
        }
    CleanupStack::PopAndDestroy( &allIds );

    // Check and rename duplicate provider.

    TBool isValid( EFalse );

    HBufC* newProviderName = HBufC::NewLC( KMaxProviderNameLength * KTwo );
    newProviderName->Des().Copy( aEntry.iProviderName );

    TInt count( profileEntries->Count() );

    for ( TInt n = 0; n < count; n++ )
        {
        const TDesC& existingName = profileEntries->At( n )->iProviderName;
        if ( existingName.Compare( aEntry.iProviderName ) == 0 )
            {
            TBool isUnique( EFalse );

            for ( TInt i = 1; !isUnique; i++ )
                {
                TBool found( EFalse );
                newProviderName->Des().Copy( aEntry.iProviderName );
                newProviderName->Des().Append( KOpenParenthesis() );
                newProviderName->Des().AppendNum( static_cast<TInt64>( i ) );
                newProviderName->Des().Append( KClosedParenthesis() );
                for ( TInt m = 0; m < count; m++ )
                    {
                    if ( profileEntries->At( m )->iProviderName.Compare( 
                        newProviderName->Des() ) == 0 )
                        {
                        found = ETrue;
                        }
                    }
                if ( !found )
                    {
                    isUnique = ETrue;
                    }
                }
            }
        }

    // Change setting only if length is smaller than max length.
    if ( newProviderName->Length() < KMaxProviderNameLength )
        {
        aEntry.iProviderName.Copy( newProviderName->Des() );
        isValid = ETrue;
        }

    // Check and rename also duplicate settingsName.
    HBufC* newSettingsName = HBufC::NewLC( KMaxSettingsNameLength * KTwo );
    newSettingsName->Des().Copy( aEntry.iSettingsName );
    isValid = EFalse;
    for ( TInt n = 0; n < count; n++ )
        {
        const TDesC& existingName = profileEntries->At( n )->iSettingsName;
        if ( existingName.Compare( aEntry.iSettingsName ) == 0 )
            {
            TBool isUnique( EFalse );

            for ( TInt i = 1; !isUnique; i++ )
                {
                TBool found( EFalse );
                newSettingsName->Des().Copy( aEntry.iSettingsName );
                newSettingsName->Des().Append( KOpenParenthesis() );
                newSettingsName->Des().AppendNum( static_cast<TInt64>( i ) );
                newSettingsName->Des().Append( KClosedParenthesis() );
                for ( TInt m = 0; m < count; m++ )
                    {
                    if ( profileEntries->At( m )->iSettingsName.Compare( 
                        newSettingsName->Des() ) == 0 )
                        {
                        found = ETrue;
                        } // if
                    } // for
                    
                if ( !found )
                    {
                    isUnique = ETrue;
                    }
                } // for ( TInt m = 0; m < count; m++ )
            } // if ( existingName.Compare( ...
        } // for ( TInt n = 0; n < count; n++ )

    // Change setting only if length is smaller than max length.
    if ( newSettingsName->Length() < KMaxSettingsNameLength )
        {
        aEntry.iSettingsName.Copy( newSettingsName->Des() );
        isValid = ETrue;
        }

    // newSettingsName, newProviderName, profileEntries, cRCSEProfile
    CleanupStack::PopAndDestroy( 4, cRCSEProfile );
    DBG_PRINT( "CWPVoIPItem::CheckDuplicatesL - end" );
    return isValid;
    }

// ---------------------------------------------------------------------------
// void CWPVoIPItem::SetTelephonyPreferenceL
// For setting telephony preference.
// ---------------------------------------------------------------------------
//
void CWPVoIPItem::SetTelephonyPreferenceL ( const TTelephonyPreference& 
    aTelephonyPreference ) const
    {
    DBG_PRINT( "CWPVoIPItem::SetTelephonyPreferenceL - begin" );
    // Central Repository for richcall settings
    CRepository* rep = CRepository::NewLC( KCRUidRichCallSettings );
    //lint -e{961} No need for else statement here
    if ( EPSPreferred == aTelephonyPreference )
        {
        rep->Set( KRCSEPreferredTelephony, EPSPreferred );
        }
    else if ( ECSPreferred == aTelephonyPreference )
        {
        rep->Set( KRCSEPreferredTelephony, ECSPreferred );
        }
    
    CleanupStack::PopAndDestroy( rep );
    
    DBG_PRINT( "CWPVoIPItem::SetTelephonyPreferenceL - end" );
    }

// ---------------------------------------------------------------------------
// void CWPVoIPItem::AppRef
// ---------------------------------------------------------------------------
//
HBufC8* CWPVoIPItem::AppRef()
    {
    DBG_PRINT( "CWPVoIPItem::AppRef()" );
    return iAppRef;
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::GetTerminalTypeL
// Collect terminal type used in SIP User Agent Header.
// ---------------------------------------------------------------------------
//
void CWPVoIPItem::GetTerminalTypeL(
    TBuf<KMaxTerminalTypeLength>& aTerminalType ) const
    {
    DBG_PRINT( "CWPVoIPItem::GetTerminalTypeL - begin" );

    aTerminalType.Zero(); // Reset before use

    CIpAppPhoneUtils* phoneUtils = CIpAppPhoneUtils::NewLC();
    phoneUtils->GetTerminalTypeL( aTerminalType ); // Gets phone model + type
    CleanupStack::PopAndDestroy( phoneUtils );

    DBG_PRINT( "CWPVoIPItem::GetTerminalTypeL - end" );
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::GetWlanMacAddressL
// Gets WLAN MAC address used in SIP User Agent Header (same as *#62209526#).
// ---------------------------------------------------------------------------
//
void CWPVoIPItem::GetWlanMacAddressL( 
    TBuf<KWlanMacAddressLength>& aMac ) const
    {
    DBG_PRINT( "CWPVoIPItem::GetWlanMacAddressL - begin" );

    CIPAppUtilsAddressResolver* resolver = 
        CIPAppUtilsAddressResolver::NewLC();

    TBuf8<KWlanMacAddressLength> wlanmac;
    _LIT8( KFormat, "-" );
    resolver->GetWlanMACAddress( wlanmac, KFormat );
    // wlanmac buffer contains now the wlan mac address like 00-15-a0-99-10-ec
    CleanupStack::PopAndDestroy( resolver );
    aMac.Copy( wlanmac );

    DBG_PRINT( "CWPVoIPItem::GetWlanMacAddressL - end" );
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::GetUserAgentHeaderL
// Gets SIP User Agent Header.
// ---------------------------------------------------------------------------
//
void CWPVoIPItem::GetUserAgentHeaderL( 
    TBuf8<KUAHLength>& aUserAgentHeader ) const
    {
    DBG_PRINT( "CWPVoIPItem::GetUserAgentHeaderL - begin" );
    // Create a CRCSEProfileEntry to get default values for User-Agent header
    // parameters.
    CRCSEProfileEntry* cRCSEProfileEntry = CRCSEProfileEntry::NewLC();

    aUserAgentHeader.Zero(); // Reset before use
    TBuf<KMaxTerminalTypeLength> tempTerminalType;
    TBuf<KWlanMacAddressLength> tempWlanMac;
    TBuf<KMaxSettingsLength32> tempFreeString;
    tempFreeString = UAHString();

    TInt uahTerminalType = iUAHTerminalType;
    TInt uahWlanMac = iUAHWLANMAC;

    if ( KNotSet == iUAHTerminalType )
        {
        uahTerminalType = cRCSEProfileEntry->iSIPVoIPUAHTerminalType;
        }

    if ( KNotSet == uahWlanMac )
        {
        uahWlanMac = cRCSEProfileEntry->iSIPVoIPUAHeaderWLANMAC;
        }

    if ( uahTerminalType || uahWlanMac || ( tempFreeString.Length() > 0 ) )
        {
        aUserAgentHeader.Append( KUserAgent );
        aUserAgentHeader.Append( KColonMark );
        aUserAgentHeader.Append( KSpaceMark );
        }

    if ( uahTerminalType )
        {
        GetTerminalTypeL( tempTerminalType );
        aUserAgentHeader.Append( tempTerminalType );
        aUserAgentHeader.Append( KSpaceMark );
        }
    if ( uahWlanMac )
        {
        GetWlanMacAddressL( tempWlanMac );
        aUserAgentHeader.Append( tempWlanMac );
        aUserAgentHeader.Append( KSpaceMark );
        }
    if ( tempFreeString.Length() > 0 )
        {
        aUserAgentHeader.Append( tempFreeString );
        }
    CleanupStack::PopAndDestroy( cRCSEProfileEntry );
    DBG_PRINT( "CWPVoIPItem::GetUserAgentHeader - end" );
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::CleanupArrayItem
// Cleans up an array.
// ---------------------------------------------------------------------------
//
void CWPVoIPItem::CleanupArrayItem( TAny* aArray )
    {
    CArrayPtrFlat<CRCSEProfileEntry>* tempArray = 
        static_cast<CArrayPtrFlat<CRCSEProfileEntry>*>( aArray );
    if ( tempArray )
        {
        tempArray->ResetAndDestroy();
        }
    delete tempArray;
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::AddDefaultCodecsL
// Creates default codecs.
// ---------------------------------------------------------------------------
//
void CWPVoIPItem::AddDefaultCodecsL( CRCSEProfileEntry& aProfileEntry )
    {
    DBG_PRINT( "CWPVoIPItem::AddDefaultCodecsL - start" );

    CRCSEAudioCodecRegistry* audioCodecRegistry = 
        CRCSEAudioCodecRegistry::NewLC();
    CRCSEAudioCodecEntry* audioCodecEntry = CRCSEAudioCodecEntry::NewLC();

    TUint32 codecId( KErrNone );

    audioCodecEntry->SetDefaultCodecValueSet( KAudioCodecAMRWB() );
    codecId = audioCodecRegistry->AddL( *audioCodecEntry );
    aProfileEntry.iPreferredCodecs.AppendL( codecId );

    audioCodecEntry->SetDefaultCodecValueSet( KAudioCodecAMR() );
    codecId = audioCodecRegistry->AddL( *audioCodecEntry );
    aProfileEntry.iPreferredCodecs.AppendL( codecId );

    audioCodecEntry->SetDefaultCodecValueSet( KAudioCodecPCMU() );
    codecId = audioCodecRegistry->AddL( *audioCodecEntry );
    aProfileEntry.iPreferredCodecs.AppendL( codecId );

    audioCodecEntry->SetDefaultCodecValueSet( KAudioCodecPCMA() );
    codecId = audioCodecRegistry->AddL( *audioCodecEntry );
    aProfileEntry.iPreferredCodecs.AppendL( codecId );

    audioCodecEntry->SetDefaultCodecValueSet( KAudioCodeciLBC() );
    codecId = audioCodecRegistry->AddL( *audioCodecEntry );
    aProfileEntry.iPreferredCodecs.AppendL( codecId );

    audioCodecEntry->SetDefaultCodecValueSet( KAudioCodecG729() );
    codecId = audioCodecRegistry->AddL( *audioCodecEntry );
    aProfileEntry.iPreferredCodecs.AppendL( codecId );

    audioCodecEntry->SetDefaultCodecValueSet( KAudioCodecCN() );
    codecId = audioCodecRegistry->AddL( *audioCodecEntry );
    aProfileEntry.iPreferredCodecs.AppendL( codecId );

    // audioCodecEntry, audioCodecRegistry
    CleanupStack::PopAndDestroy( 2, audioCodecRegistry );

    DBG_PRINT( "CWPVoIPItem::AddDefaultCodecsL - end" );
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::ItemId
// ---------------------------------------------------------------------------
//
TUint32 CWPVoIPItem::ItemId()
    {
    return iItemId;
    }

// ---------------------------------------------------------------------------
// CWPVoIPItem::EnableIm
// ---------------------------------------------------------------------------
//
void CWPVoIPItem::EnableIm()
    {
    iImEnabled = ETrue;
    }

//  End of File
