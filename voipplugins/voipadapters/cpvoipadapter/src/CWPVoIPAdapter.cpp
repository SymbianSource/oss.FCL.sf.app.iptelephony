/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Receives and stores VoIP settings.
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <CWPAdapter.h>
#include <CWPCharacteristic.h>
#include <CWPParameter.h>
#include <WPVoIPAdapter.rsg>
#include <WPAdapterUtil.h> // Adapter utils
#include <s32mem.h>        // RDesWriteStream
#include <s32strm.h>
#include <e32des8.h>
#include <utf.h> // Unicode conversion
#include <featmgr.h>
#include <cmmanagerext.h>
#include <cmdestinationext.h>
#include <cmconnectionmethoddef.h>
#include <crcseprofileentry.h>
#include <crcseprofileregistry.h>
#include <crcseaudiocodecentry.h>
#include <crcseaudiocodecregistry.h>

#include "CWPVoIPAdapter.h"
#include "CWPVoIPItem.h"

// The following four includes are because of SIP User Agent Header.
#include "CSIPProfileRegistryObserver.h"
#include <sipmanagedprofile.h>
#include <sipprofileregistryobserver.h>
#include <sipmanagedprofileregistry.h>

// LOCAL CONSTANTS
// Application related constants
_LIT( KVoIPAdapterName, "WPVoIPAdapter");
_LIT( KVoIPAppID,       "w9013" );  // OMA CP registration document for VoIP
_LIT8( KVoIPAppID8,     "w9013" );  // OMA CP registration document for VoIP
_LIT8( KSIPAppID8,      "w9010" );  // OMA CP registration document for SIP
_LIT8( KSCCPAppID8,     "w9018" );  // OMA CP registration document for SCCP
_LIT8( KNATFWAppID8,    "w902E" );  // OMA CP registration document for NATFW
_LIT8( KPresenceAppId8, "ap0002" ); // OMA CP registration document for 
                                    // SIMPLE Presence
_LIT8( KSNAPAppID8,     "w904C" );  // OMA CP registration document for 
                                    // Destination Networks
// Characterictic clear text names.
_LIT( KCODEC16,         "CODEC" );
_LIT( KVOIPURIS16,      "VOIPURIS" );

// VoIP profile parameters.
// OMA CP registration document for VoIP, w9013.txt.
_LIT( KParmSMPORT,               "SMPORT" );
_LIT( KParmEMPORT,               "EMPORT" );
_LIT( KParmMEDIAQOS,             "MEDIAQOS" );
_LIT( KParmNODTMFIB,             "NODTMFIB" );
_LIT( KParmNODTMFOOB,            "NODTMFOOB" );
_LIT( KParmSECURECALLPREF,       "SECURECALLPREF" );
_LIT( KParmRTCP,                 "RTCP" );
_LIT( KParmUAHTERMINALTYPE,      "UAHTERMINALTYPE" );
_LIT( KParmUAHWLANMAC,           "UAHWLANMAC" );
_LIT( KParmUAHSTRING,            "UAHSTRING" );
_LIT( KParmPROFILELOCKEDTOIAP,   "PROFILELOCKEDTOIAP" );
_LIT( KParmVOIPPLUGINUID,        "VOIPPLUGINUID" );
_LIT( KParmALLOWVOIPOVERWCDMA,   "ALLOWVOIPOVERWCDMA" );
_LIT( KParmVOIPDIGITS,           "VOIPDIGITS" );
_LIT( KParmIGNDOMPART,           "IGNDOMPART" );
_LIT( KParmAABUDDYREQUEST,       "AABUDDYREQUEST" );
_LIT( KParmADDUSERPHONE,         "ADDUSERPHONE" );
_LIT( KParmSIPCONNTESTURI,       "SIPCONNTESTURI" );
_LIT( KParmPROVIDERURI,          "PROVIDERURI" );
_LIT( KParmMINSE,                "MINSE" );
_LIT( KParmSESSIONEXP,           "SESSIONEXP" );
_LIT( KParmBRANDINGURI,          "BRANDINGURI" );
_LIT( KParmNATPROTOCOL,          "NATPROTOCOL" );
_LIT( KParmAUTOENABLE,           "AUTOENABLE" );
_LIT( KParmIMENABLED,            "IMENABLED" );

// RESOURCE parameters.
// OMA CP registration document for VoIP, w9013.txt.
// RESOURCETYPE indicates if the following parameters are for a codec, SIP 
// specific (URIs), Handover or VoIP Presence settings.
_LIT( KParmMEDIASUBTYPE,         "MEDIASUBTYPE" );// Codec.
_LIT( KParmPRIORITYINDEX,        "PRIORITYINDEX" );// Codec.
_LIT( KParmJITTERBUFFERSIZE,     "JITTERBUFFERSIZE" );// Codec.
_LIT( KParmOCTETALIGN,           "OCTET-ALIGN" );// Codec.
_LIT( KParmMODESET,              "MODE-SET" );// Codec.
_LIT( KParmMODECHANGEPERIOD,     "MODE-CHANGE-PERIOD" );// Codec.
_LIT( KParmMODECHANGENEIGHBOR,   "MODE-CHANGE-NEIGHBOR" );// Codec.
_LIT( KParmPTIME,                "PTIME" );// Codec.
_LIT( KParmMAXPTIME,             "MAXPTIME" );// Codec.
_LIT( KParmVAD,                  "VAD" );// Codec.
_LIT( KParmANNEXB,               "ANNEXB" );// Codec.
_LIT( KParmMAXRED,               "MAXRED" );// Codec.
_LIT( KParmIPVOICEMAILBOXURI,    "IPVOICEMAILBOXURI" );// VMBX.
_LIT( KParmVMBXLISTENURI,        "VMBXLISTENURI" );// VMBX.
_LIT( KParmRESUBSCRIBE,          "RESUBSCRIBE" );// VMBX.

// For other purposes.
_LIT8( KHexPrefix,              "0x" ); // For parsing PLUGINUID.
const TUint KTempStringlength( 200 ); // For SIP User-Agent Header.
const TInt KMaxCharsInTUint32( 10 );
const TInt KTen( 10 ); 

static const TInt32 KNotSaved( -1 );

// Following lines are for enabling debug prints.
#ifdef _DEBUG
#define DBG_PRINT(p) RDebug::Print(_L(p))
#define DBG_PRINT2(p,a) RDebug::Print(_L(p),a)
#else
#define DBG_PRINT(p)
#define DBG_PRINT2(p,a)
#endif // _DEBUG


// ---------------------------------------------------------------------------
// CWPVoIPAdapter::CWPVoIPAdapter
// ---------------------------------------------------------------------------
//
CWPVoIPAdapter::CWPVoIPAdapter() : CWPAdapter()
    {
    }

// ---------------------------------------------------------------------------
// CWPVoIPAdapter::ConstructL
// ---------------------------------------------------------------------------
//
void CWPVoIPAdapter::ConstructL()
    {
    DBG_PRINT( "CWPVoIPAdapter::ConstructL - begin" );
    TFileName fileName;
    Dll::FileName( fileName );
    iTitle = WPAdapterUtil::ReadHBufCL( fileName, KVoIPAdapterName, 
        R_QTN_SM_HEAD_VOIP );
    iUids = HBufC8::NewL( 0 );
    iCurrentVoIPItem = NULL;

    FeatureManager::InitializeLibL();
    iFeatMgrInitialized = ETrue;

    DBG_PRINT( "CWVoIPAdapter::ConstructL - Checking VoIP support." );
    if ( !FeatureManager::FeatureSupported( KFeatureIdCommonVoip ) )
        {
        DBG_PRINT( "CWVoIPAdapter::ConstructL - VoIP not supported; leave." );
        User::Leave( KErrNotSupported );
        }
    DBG_PRINT( "CWPVoIPAdapter::ConstructL - end" );
    }

// ---------------------------------------------------------------------------
// CWPVoIPAdapter::NewL
// ---------------------------------------------------------------------------
//
CWPVoIPAdapter* CWPVoIPAdapter::NewL()
    {
    DBG_PRINT( "CWPVoIPAdapter::NewL - begin" );
    CWPVoIPAdapter* self = new (ELeave) CWPVoIPAdapter;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    DBG_PRINT( "CWVoIPAdapter::NewL - end" );
    return self;
    }

// ---------------------------------------------------------------------------
// CWPVoIPAdapter::~CWPVoIPAdapter
// ---------------------------------------------------------------------------
//
CWPVoIPAdapter::~CWPVoIPAdapter()
    {
    DBG_PRINT( "CWVoIPAdapter::~CWPVoIPAdapter - begin" );
    delete iTitle;
    delete iUids;
    iDatas.ResetAndDestroy();
    iDatas.Close();

    if ( iFeatMgrInitialized )
        {
        FeatureManager::UnInitializeLib();
        }

    DBG_PRINT( "CWVoIPAdapter::~CWPVoIPAdapter - end" );
    //lint -e{1740} iCurrentCodec is deleted in another class
    }

// ---------------------------------------------------------------------------
// CWPVoIPAdapter::ContextExtension
// ---------------------------------------------------------------------------
//
TInt CWPVoIPAdapter::ContextExtension( MWPContextExtension*& aExtension )
    {
    aExtension = this;
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CWPVoIPAdapter::SaveDataL
// ---------------------------------------------------------------------------
//
const TDesC8& CWPVoIPAdapter::SaveDataL( TInt aIndex ) const
    {
    return iDatas[aIndex]->SaveData();
    }

// ---------------------------------------------------------------------------
// CWPVoIPAdapter::DeleteL
// ---------------------------------------------------------------------------
//
void CWPVoIPAdapter::DeleteL( const TDesC8& aSaveData )
    {
    DBG_PRINT( "CWVoIPAdapter::DeleteL - begin" );
    TUint tId;
    TLex8 lex( aSaveData );
    TInt err( KErrNone );            
    err = lex.Val( tId );
    if ( KErrNone == err )
        {
        CRCSEProfileRegistry* cRCSEProfileRegistry = 
            CRCSEProfileRegistry::NewLC();
        cRCSEProfileRegistry->DeleteL( tId );
        
        // set CS preferred if last VoIP profile deleted
        RArray<TUint32> voipIds;
        cRCSEProfileRegistry->GetAllIdsL( voipIds );
        if ( KErrNone == voipIds.Count()  )
            {
            CWPVoIPItem* cVoIPItem = CWPVoIPItem::NewL();
            CleanupStack::PushL( cVoIPItem );
            cVoIPItem->SetTelephonyPreferenceL( CWPVoIPItem::ECSPreferred );
            CleanupStack::PopAndDestroy( cVoIPItem );
            }
        voipIds.Close();  
        CleanupStack::PopAndDestroy( cRCSEProfileRegistry );
        }// if
    DBG_PRINT( "CWVoIPAdapter::DeleteL - end" );
    }

// ---------------------------------------------------------------------------
// CWPVoIPAdapter::Uid
// ---------------------------------------------------------------------------
//
TUint32 CWPVoIPAdapter::Uid() const
    {
    return iDtor_ID_Key.iUid; 
    }

// ---------------------------------------------------------------------------
// CWPVoIPAdapter::DetailsL
// ---------------------------------------------------------------------------
//
TInt CWPVoIPAdapter::DetailsL( TInt /*aItem*/, MWPPairVisitor& /*aVisitor */ )
    {
    return KErrNotSupported;
    }  

// ---------------------------------------------------------------------------
// CWPVoIPAdapter::SummaryCount
// ---------------------------------------------------------------------------
//
TInt CWPVoIPAdapter::ItemCount() const
    {
    return iDatas.Count();
    }

// ---------------------------------------------------------------------------
// CWPVoIPAdapter::SummaryTitle
// ---------------------------------------------------------------------------
//
const TDesC16& CWPVoIPAdapter::SummaryTitle( TInt /*aIndex*/ ) const
    {
    return *iTitle;
    }

// ---------------------------------------------------------------------------
// CWPVoIPAdapter::SummaryText
// ---------------------------------------------------------------------------
//
const TDesC16& CWPVoIPAdapter::SummaryText( TInt aIndex ) const
    {
    return iDatas[aIndex]->Name();
    }

// ---------------------------------------------------------------------------
// CWPVoIPAdapter::SaveL
// ---------------------------------------------------------------------------
//
void CWPVoIPAdapter::SaveL( TInt aIndex )
    {
    DBG_PRINT( "CWVoIPAdapter::SaveL - begin" );

    CWPVoIPItem* voipItem = iDatas[aIndex];

    if ( voipItem->NapDef() && voipItem->NapDef()->Data().Length() <= 
        KMaxCharsInTUint32 )
        {
        // Get WAP ID.
        TPckgBuf<TUint32> uid;
        uid.Copy( voipItem->NapDef()->Data( 0 ) );
        TUint32 wapId( uid() );
        // Get corresponding IAP ID.
        TUint32 iapId = IapIdFromWapIdL( wapId );
        voipItem->SetIapId( iapId );
        }

    voipItem->StoreL();
    voipItem = NULL;
    DBG_PRINT( "CWVoIPAdapter::SaveL - end" );
    }

// ---------------------------------------------------------------------------
// CWPVoIPAdapter::CanSetAsDefault
// ---------------------------------------------------------------------------
//
TBool CWPVoIPAdapter::CanSetAsDefault( TInt /*aIndex*/ ) const
    {
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CWPVoIPAdapter::SetAsDefaultL
// ---------------------------------------------------------------------------
//
void CWPVoIPAdapter::SetAsDefaultL( TInt /*aIndex*/ )
    {
    // From VoIP release 3.0 onwards VoIP profiles cannot be set as default
    // => this mehod does nothing but is present because of inheritance.
    }

// ---------------------------------------------------------------------------
// CWPVoIPAdapter::VisitL
// ---------------------------------------------------------------------------
//
void CWPVoIPAdapter::VisitL( CWPCharacteristic& aCharacteristic )
    {
    DBG_PRINT( "CWVoIPAdapter::VisitL( characteristic ) - begin" );
    
    switch( aCharacteristic.Type() )
        {
        // Core VoIP settings.
        case KWPApplication:
            {
            DBG_PRINT(
                 "CWVoIPAdapter::VisitL(characteristic) - case APPLICATION" );
            iCurrentCharacteristic = KWPApplication;
            iCurrentVoIPItem = CWPVoIPItem::NewL();
            aCharacteristic.AcceptL( *this );
            // If VoIP settings, append the item into iDatas array and leave
            // iCurrentVoIPItem to point to it.
            if ( iAppID == KVoIPAppID ) 
                {
                User::LeaveIfError( iDatas.Append( iCurrentVoIPItem ) );
                }
            else // else the settigs are no longer of type VoIP.
                {
                DBG_PRINT( 
                    "CWVoIPAdapter::VisitL( characteristic ) - no VoIP." );
                delete iCurrentVoIPItem;
                iCurrentVoIPItem = NULL;
                }
            break;
            }
        // Codec and SIP profile specific settings.
        case KWPNamedCharacteristic:
            {
            iCurrentCharacteristic = KWPNamedCharacteristic;
            //lint -e{961} No need for else statement here
            if ( aCharacteristic.Name().Compare( KCODEC16() ) == 0 )
                {
                DBG_PRINT(
                    "CWVoIPAdapter::VisitL( characteristic ) - case CODEC" );
                iCurrentCodec = CWPVoIPCodec::NewL();
                iSetCodec = ETrue;
                aCharacteristic.AcceptL( *this ); 
                iCurrentVoIPItem->AddCodecL( iCurrentCodec );
                iSetCodec = EFalse;
                }
            else if ( aCharacteristic.Name().Compare( KVOIPURIS16() ) == 0 )
                {
                DBG_PRINT(
                    "CWVoIPAdapter::VisitL(characteristic) - case VOIPURIS" );
                iSetVoipUris = ETrue;
                aCharacteristic.AcceptL( *this );
                iSetVoipUris = EFalse;
                }
            break;
            }
        default:
            break;
        }
    DBG_PRINT( "CWVoIPAdapter::VisitL( characteristic ) - end" );
    }

// ---------------------------------------------------------------------------
// CWPVoIPAdapter::GetSavingInfoL
// ---------------------------------------------------------------------------
//
void CWPVoIPAdapter::GetSavingInfoL( TInt aIndex, 
    RPointerArray<HBufC8>& aSavingInfo )
    {
    // APPID into place [0].
    aSavingInfo.AppendL( KVoIPAppID8().AllocL() );
    // APPREF into place [1].
    aSavingInfo.AppendL( iDatas[aIndex]->AppRef()->AllocL() );
    // Profile id into place [2].
    aSavingInfo.AppendL( iDatas[aIndex]->SaveData().AllocL() );
    }

// ---------------------------------------------------------------------------
// CWPVoIPAdapter::VisitL
// ---------------------------------------------------------------------------
//
void CWPVoIPAdapter::VisitL( CWPParameter& aParameter )
    {
    DBG_PRINT( "CWVoIPAdapter::VisitL( parameter ) - begin" );
    // tmpValue holds the value converted from Unicode to UTF8.
    HBufC8* tmpValue = HBufC8::NewLC( aParameter.Value().Length() ); // CS:1
    TPtr8 ptrTmpValue( tmpValue->Des() );
    CnvUtfConverter::ConvertFromUnicodeToUtf8( ptrTmpValue, 
        aParameter.Value() );    
    TInt tIntParameterValue( 0 );
    
    switch( aParameter.ID() )
        {
        case EWPParameterAppRef:
            {
            iCurrentVoIPItem->SetAppRefL( tmpValue->Des() );
            break;
            }
        case EWPParameterAppID:
            {
            iAppID.Set( aParameter.Value() );
            break;
            }
        // Here case 0 are handled the VoIP parameters that are extensions  
        // to OMA Client Provisioning parameter set.
        case 0:
            //lint -e{961} No need for else statement here
            if ( aParameter.Name().Compare( KParmSMPORT ) == 0 )
                {
                if ( KErrNone == DescToInt( tmpValue, tIntParameterValue ) )
                    {
                    iCurrentVoIPItem->SetStartMediaPort( tIntParameterValue );
                    }// if
                }
            else if ( aParameter.Name().Compare( KParmEMPORT ) == 0 )
                {
                if ( KErrNone == DescToInt( tmpValue, tIntParameterValue ) )
                    {
                    iCurrentVoIPItem->SetEndMediaPort( tIntParameterValue );
                    }//if
                }// else if
            else if ( aParameter.Name().Compare( KParmMEDIAQOS ) == 0 )
                {
                if ( KErrNone == DescToInt( tmpValue, tIntParameterValue ) )
                    {
                    iCurrentVoIPItem->SetMediaQoS( tIntParameterValue );
                    }// if
                }// else if
            else if ( aParameter.Name().Compare( KParmNODTMFIB ) == 0 )
                {
                iCurrentVoIPItem->SetDTMFInBand( 0 );
                }// else if
            else if ( aParameter.Name().Compare( KParmNODTMFOOB ) == 0 )
                {
                iCurrentVoIPItem->SetDTMFOutBand( 0 );
                }
            else if ( aParameter.Name().Compare( KParmSECURECALLPREF ) == 0 )
                {
                if ( KErrNone == DescToInt( tmpValue, tIntParameterValue ) )
                    {
                    iCurrentVoIPItem->SetSecureCallPref( tIntParameterValue );
                    }// if
                }// else if

            else if ( aParameter.Name().Compare( KParmRTCP ) == 0 )
                {
                if ( KErrNone == DescToInt( tmpValue, tIntParameterValue ) )
                    {
                    iCurrentVoIPItem->SetRTCP( tIntParameterValue );
                    }// if
                }// else if

            else if ( aParameter.Name().Compare( 
                KParmUAHTERMINALTYPE ) == 0 )
                {
                if ( KErrNone == DescToInt( tmpValue, tIntParameterValue ) )
                    {
                    iCurrentVoIPItem->SetUAHTerminalType( 
                        tIntParameterValue );
                    }// if
                }// else if

            else if ( aParameter.Name().Compare( KParmUAHWLANMAC ) == 0 )
                {
                if ( KErrNone == DescToInt( tmpValue, tIntParameterValue ) )
                    {
                    iCurrentVoIPItem->SetUAHWLANMAC( tIntParameterValue );
                    }// if
                }// else if

            else if ( aParameter.Name().Compare( KParmUAHSTRING ) == 0 )
                {
                iCurrentVoIPItem->SetUAHStringL( aParameter.Value() );
                }

            else if ( aParameter.Name().Compare( 
                KParmPROFILELOCKEDTOIAP ) == 0 )
                {
                iCurrentVoIPItem->SetProfileLockedToIAP( 1 );
                }

            else if ( aParameter.Name().Compare( KParmVOIPPLUGINUID ) == 0 )
                {
                TInt hexStart ( tmpValue->Find( KHexPrefix() ) );
                if ( hexStart == KErrNotFound )
                    {
                    hexStart = KErrNone;
                    }
                else
                    {
                    hexStart = KHexPrefix().Length();
                    }
                TUint32 paramValue;
                TLex8 lex( tmpValue->Des().Mid( hexStart ));
                TInt err( KErrNone );
                err = lex.Val( paramValue, EHex );
                if ( KErrNone == err && paramValue <= KMaxTInt )
                    {
                    iCurrentVoIPItem->SetVoIPPluginUid( paramValue );
                    }// if
                }// else if

            else if ( aParameter.Name().Compare( 
                KParmALLOWVOIPOVERWCDMA ) == 0 )
                {
                iCurrentVoIPItem->SetAllowVoIPOverWCDMA( 1 );
                }
            
            else if ( aParameter.Name().Compare( KParmVOIPDIGITS ) == 0 )
                {
                if ( KErrNone == DescToInt( tmpValue, tIntParameterValue ) )
                    {
                    iCurrentVoIPItem->SetVoIPDigits( tIntParameterValue );
                    }
                }

            else if ( aParameter.Name().Compare( KParmIGNDOMPART ) == 0 )
                {
                if ( KErrNone == DescToInt( tmpValue, tIntParameterValue ) )
                    {
                    iCurrentVoIPItem->SetDomainPartIgnoreRule( 
                        tIntParameterValue );
                    }
                }

            else if ( KErrNone == aParameter.Name().Compare( 
                KParmADDUSERPHONE ) )
                {
                if ( KErrNone == DescToInt( tmpValue, tIntParameterValue ) )
                    {
                    iCurrentVoIPItem->SetAddUserPhone( tIntParameterValue );
                    }
                }

            else if ( KErrNone == aParameter.Name().Compare( 
                KParmSIPCONNTESTURI ) )
                {
                iCurrentVoIPItem->SetSipConnTestAddressL( 
                    aParameter.Value() );
                }

            else if ( KErrNone == aParameter.Name().Compare( 
                KParmPROVIDERURI ) )
                {
                iCurrentVoIPItem->SetServiceProviderBookmarkL( 
                    aParameter.Value() );
                }

            else if ( KErrNone == aParameter.Name().Compare( KParmMINSE ) )
                {
                if ( KErrNone == DescToInt( tmpValue, tIntParameterValue ) )
                    {
                    iCurrentVoIPItem->SetSipMinSe( tIntParameterValue );
                    }
                }

            else if ( KErrNone == aParameter.Name().Compare( 
                KParmSESSIONEXP ) )
                {
                if ( KErrNone == DescToInt( tmpValue, tIntParameterValue ) )
                    {
                    iCurrentVoIPItem->SetSipSessionExpires( 
                        tIntParameterValue );
                    }
                }

            else if ( KErrNone == aParameter.Name().Compare( 
                KParmBRANDINGURI ) )
                {
                iCurrentVoIPItem->SetBrandingDataAddressL( 
                    aParameter.Value() );
                }

            else if ( KErrNone == aParameter.Name().Compare( 
                KParmAABUDDYREQUEST ) )
                {
                if ( KErrNone == DescToInt( tmpValue, tIntParameterValue ) )
                    {
                    iCurrentVoIPItem->SetAutoAcceptBuddyRequest( 
                        tIntParameterValue );
                    }
                }                               

            // VoiceMailBox parameter.
            else if ( KErrNone == aParameter.Name().Compare( 
                KParmIPVOICEMAILBOXURI ) )
                {
                iCurrentVoIPItem->SetIPVoiceMailBoxURIL(
                    aParameter.Value() );
                }

            // VoiceMailBox parameter.
            else if ( KErrNone == aParameter.Name().Compare( 
                KParmVMBXLISTENURI ) )
                {
                iCurrentVoIPItem->SetVoiceMailBoxListenURIL( 
                    aParameter.Value() );
                }
                
            // VoiceMailBox parameter.
            else if ( KErrNone == aParameter.Name().Compare( 
                KParmRESUBSCRIBE ) )
                {
                if ( KErrNone == DescToInt( tmpValue, tIntParameterValue ) )
                    {
                    iCurrentVoIPItem->SetReSubscribeInterval( 
                        tIntParameterValue );
                    }
                }

            else if ( KErrNone == aParameter.Name().Compare( 
                KParmNATPROTOCOL ) )
                {                
                if ( KErrNone == DescToInt( tmpValue, tIntParameterValue ) )
                    {
                    iCurrentVoIPItem->SetUsedNatProtocol( 
                        tIntParameterValue );                    
                    }
                }     
                
            else if ( KErrNone == aParameter.Name().Compare( 
                KParmAUTOENABLE ) )
                {
                if ( KErrNone == DescToInt( tmpValue, tIntParameterValue ) )
                    {
                    iCurrentVoIPItem->SetAutoEnableService( 
                        tIntParameterValue );
                    }
                }             
            else if ( KErrNone == aParameter.Name().Compare( 
                KParmIMENABLED ) )
                {
                iCurrentVoIPItem->EnableIm();
                }

            // Codec parameter.    
            else if ( aParameter.Name().Compare( KParmMEDIASUBTYPE ) == 0 
                && iSetCodec )
                {
                typedef CWPVoIPCodec CMediaSubType;
                // don't need to check value because of switch-default:
                // coverity[check_return] coverity[unchecked_value]
                DescToInt( tmpValue, tIntParameterValue );

                switch ( tIntParameterValue )
                    {
                    case CMediaSubType::EAMR:
                        {
                        iCurrentCodec->SetMediaSubtypeNameL( 
                            KAudioCodecAMR() );
                        }
                    break;

                    case CMediaSubType::EiLBC:
                        {
                        iCurrentCodec->SetMediaSubtypeNameL( 
                            KAudioCodeciLBC() );
                        }
                    break;

                    case CMediaSubType::EPCMA:
                        {
                        iCurrentCodec->SetMediaSubtypeNameL( 
                            KAudioCodecPCMA() );
                        }
                    break;

                    case CMediaSubType::EPCMU:
                        {
                        iCurrentCodec->SetMediaSubtypeNameL( 
                            KAudioCodecPCMU() );
                        }
                    break;

                    case CMediaSubType::ECN:
                        {
                        iCurrentCodec->SetMediaSubtypeNameL( 
                            KAudioCodecCN() );
                        }
                    break;

                    case CMediaSubType::EG729:
                        {
                        iCurrentCodec->SetMediaSubtypeNameL( 
                            KAudioCodecG729() );
                        }
                    break;

                    case CMediaSubType::EAMRWB:
                        {
                        iCurrentCodec->SetMediaSubtypeNameL( 
                            KAudioCodecAMRWB() );
                        }
                    break;

                    case CMediaSubType::EVMRWB:
                    case CMediaSubType::EEVRC:
                    case CMediaSubType::EEVRC0:
                    case CMediaSubType::ESMV:
                    case CMediaSubType::ESMV0:
                    case CMediaSubType::EG726_40:
                    case CMediaSubType::EG726_32:
                    case CMediaSubType::EG726_24:
                    case CMediaSubType::EG726_16:
                    case CMediaSubType::EGSMEFR:
                    case CMediaSubType::EGSMFR:
                    default:
                        {
                        // Error or not supported => Codec deleted later.
                        iCurrentCodec->SetMediaSubtypeNameL( 
                            KNullDesC() );
                        }
                    }// switch
                }
            // Codec parameter.                    
            else if ( aParameter.Name().Compare( KParmPRIORITYINDEX ) == 0 
                && iSetCodec )
                {
                if ( KErrNone == DescToInt( tmpValue, tIntParameterValue ) )
                    {
                    iCurrentCodec->iPriorityIndex = tIntParameterValue;
                    }
                }
            // Codec parameter.    
            else if ( aParameter.Name().Compare( KParmJITTERBUFFERSIZE ) == 0
                && iSetCodec )
                {
                if ( KErrNone == DescToInt( tmpValue, tIntParameterValue ) )
                    {
                    iCurrentCodec->iJitterBufferSize = tIntParameterValue;
                    }
                }
            // Codec parameter.    
            else if ( aParameter.Name().Compare( KParmOCTETALIGN ) == 0 
                && iSetCodec )
                {
                iCurrentCodec->iOctetAlign = 1;
                }
            // Codec parameter.    
            else if ( aParameter.Name().Compare( KParmMODESET ) == 0 
                && iSetCodec )
                {
                TLex8 lexModeSet( tmpValue->Des() );
                TChar tmpChar;

                while ( lexModeSet.Peek() != 0 )                
                    {
                    TInt32 tmpModeSetValue ( 0 );
                    TBool cycle ( ETrue );
                    TBool stateDigit ( ETrue );
                    TBool numberExists ( EFalse );
                    
                    while ( cycle ) 
                        {
                        switch( stateDigit )
                            {
                            case ETrue:
                                {
                                if ( lexModeSet.Peek() != 0 )
                                    {
                                    tmpChar = lexModeSet.Get();
                                    if ( tmpChar.IsDigit() )
                                        {
                                        // for example: 25 =  2 * 10 + 5
                                        //             257 = 25 * 10 + 7  
                                        tmpModeSetValue = tmpModeSetValue * KTen 
                                                 + tmpChar.GetNumericValue();
                                        numberExists = ETrue;
                                        }
                                    else
                                        {
                                        stateDigit = EFalse;
                                        }
                                    }
                                else
                                    {
                                    stateDigit = EFalse;    
                                    }
                                break;
                                }
                                
                            case EFalse:
                                {
                                cycle = EFalse;
                                break;
                                }
                            }
                        }
                        if ( numberExists )
                            {
                            iCurrentCodec->iModeSet.Append( tmpModeSetValue );
                            }
                    }
                }
                
            // Codec parameter.    
            else if ( aParameter.Name().Compare( KParmMODECHANGEPERIOD ) == 0
                && iSetCodec )
                {
                if ( KErrNone == DescToInt( tmpValue, tIntParameterValue ) )
                    {
                    iCurrentCodec->iModeChangePeriod = tIntParameterValue;  
                    }
                }
            // Codec parameter.    
            else if ( 
                aParameter.Name().Compare( KParmMODECHANGENEIGHBOR ) == 0
                && iSetCodec )
                {
                if ( KErrNone == DescToInt( tmpValue, tIntParameterValue ) )
                    {
                    iCurrentCodec->iModeChangeNeighbor = tIntParameterValue;
                    }
                }
            // Codec parameter.    
            else if ( aParameter.Name().Compare( KParmPTIME ) == 0 
                && iSetCodec )
                {
                if ( KErrNone == DescToInt( tmpValue, tIntParameterValue ) )
                    {
                    iCurrentCodec->iPTime = tIntParameterValue;
                    }
                }
            // Codec parameter.    
            else if ( aParameter.Name().Compare( KParmMAXPTIME ) == 0 
                && iSetCodec )
                {
                if ( KErrNone == DescToInt( tmpValue, tIntParameterValue ) )
                    {
                    iCurrentCodec->iMaxPTime = tIntParameterValue;
                    }
                }

            // Codec parameter.    
            else if ( aParameter.Name().Compare( KParmVAD ) == 0 
                && iSetCodec )
                {
                iCurrentCodec->iVAD = 1;
                }

            // Codec parameter.    
            else if ( aParameter.Name().Compare( KParmANNEXB ) == 0 
                && iSetCodec )
                {
                iCurrentCodec->iAnnexB = 1;
                }
            // Codec parameter.
            else if ( aParameter.Name().Compare( KParmMAXRED ) == 0 
                && iSetCodec )
                {
                if ( KErrNone == DescToInt( tmpValue, tIntParameterValue ) )
                    {
                    iCurrentCodec->iMaxRed = tIntParameterValue;
                    }
                }
            break; // case 0:
            
        case EWPParameterProviderID: // APPLICATION/PROVIDER-ID
            iCurrentVoIPItem->SetProviderIdL( aParameter.Value() );
            break;

        case EWPParameterName: // APPLICATION/NAME
            iCurrentVoIPItem->SetNameL( aParameter.Value() );
            break;

        case EWPParameterToAppRef: // TO-APPREF
            {
            // APPLICATION/TO-APPREF
            if ( !iSetVoipUris )
                {
                iCurrentVoIPItem->AddToAppRefL( tmpValue->Des() );
                }
            else
                {
                if ( iCurrentVoIPItem && 
                    iCurrentVoIPItem->VoipUrisToAppRef().Length() == 0 )
                    {
                    iCurrentVoIPItem->SetVoipUrisToAppRefL( tmpValue->Des() );
                    }
                }// else if
            } //case
            break;

        // VOIPURIS/TO-NAPID
        case EWPParameterToNapID:
            {
            if ( iCurrentVoIPItem && iSetVoipUris 
                && iCurrentVoIPItem->ToNapId().Length() == 0 )
                {
                iCurrentVoIPItem->SetToNapIdL( tmpValue->Des() );
                }
            break;
            }

        default:
            break;
        }// switch
    CleanupStack::PopAndDestroy( tmpValue );
    DBG_PRINT( "CWVoIPAdapter::VisitL( parameter ) - end" );
    }

// ---------------------------------------------------------------------------
// CWPVoIPAdapter::VisitLinkL
// ---------------------------------------------------------------------------
//
void CWPVoIPAdapter::VisitLinkL( CWPCharacteristic& aLink )
    {
    DBG_PRINT( "CWVoIPAdapter::VisitLinkL - begin" );
    switch( aLink.Type() )
        {
        case KWPNapDef:
            {            
            if ( !iCurrentVoIPItem->NapDef() )
                {
                iCurrentVoIPItem->SetNapDef( &aLink );
                }
            break;
            }
        default:
            break;
        }
    DBG_PRINT( "CWVoIPAdapter::VisitLinkL - end" );
    }
// ---------------------------------------------------------------------------
// CWPVoIPAdapter::SettingsSavedL
// ---------------------------------------------------------------------------
//
void CWPVoIPAdapter::SettingsSavedL ( const TDesC8& aAppIdOfSavingItem,
    const TDesC8& aAppRef, const TDesC8& aStorageIdValue )
    {
    DBG_PRINT( "CWVoIPAdapter::SettingsSavedL - begin" );

    if ( !aAppIdOfSavingItem.Length() || !aAppRef.Length() || 
        !aStorageIdValue.Length() )
        {
        DBG_PRINT( "CWVoIPAdapter::SettingsSavedL - end" );
        return;
        }

    HBufC8* tmpAppId = aAppIdOfSavingItem.AllocLC(); // CS:1

    TUint32 storageId( 0 );
    TLex8 lex( aStorageIdValue );
    User::LeaveIfError( lex.Val( storageId, EDecimal ) );

    const TInt itemCount = iDatas.Count();

    //lint -e{961} No need for else statement here
    if ( KErrNone == KSIPAppID8().CompareF( tmpAppId->Des() ) )
        {
        for ( TInt itemIndex = 0; itemIndex < itemCount; itemIndex++ )
            {
            TBool setOk = iDatas[itemIndex]->SetStorageId( CWPVoIPItem::ESIP,
                storageId, aAppRef );

            TBuf8<KTempStringlength> userAgentHeader;
            iDatas[itemIndex]->GetUserAgentHeaderL( userAgentHeader );
            if ( setOk && storageId > 0 )
                {
                TUint32 itemId = iDatas[itemIndex]->ItemId();
                if ( TUint( KNotSaved ) != itemId )
                    {
                    // First update RCSE so that service will be created.
                    DBG_PRINT( "CWVoIPAdapter::SettingsSavedL - update RCSE..." );
                    CRCSEProfileRegistry* rcseReg = 
                        CRCSEProfileRegistry::NewLC();
                    CRCSEProfileEntry* rcseProf = CRCSEProfileEntry::NewLC();
                    rcseReg->FindL( itemId, *rcseProf );
                    TSettingIds referredIds;
                    referredIds.iProfileType = 0; // SIP.
                    referredIds.iProfileId = TInt( storageId );
                    // Never reference to profile specific settings.
                    referredIds.iProfileSpecificSettingId = KNotSet; 
                    rcseProf->iIds.Append( referredIds );
                    rcseReg->UpdateL( itemId, *rcseProf );
                    CleanupStack::PopAndDestroy( 2, rcseReg );
                    }

                // Then set SIP user-agent header
                DBG_PRINT(
                    "CWVoIPAdapter::SettingsSavedL - \
                    create SIP User-Agent Header..." );
                CSIPProfileRegistryObserver* cSIPProfRegObserver = 
                    CSIPProfileRegistryObserver::NewLC(); // CS:2
                CSIPManagedProfileRegistry* cSIPManagedProfileRegistry =
                    CSIPManagedProfileRegistry::NewLC( 
                        *cSIPProfRegObserver ); // CS:3
                CSIPProfile* cSIPProfile = NULL;
                cSIPProfile = cSIPManagedProfileRegistry->ProfileL( 
                    storageId );
                CleanupStack::PushL( cSIPProfile ); // CS:4
                CSIPManagedProfile* cSIPManagedProfile = 
                    static_cast<CSIPManagedProfile*>( cSIPProfile );
                CleanupStack::PushL( cSIPManagedProfile ); // CS:5
                // Create an array for setting the user agent header.
                // Granularity is 1 since only one item is appended into
                // the array.
                CDesC8ArrayFlat* uahArray = 
                    new ( ELeave ) CDesC8ArrayFlat( 1 );
                CleanupStack::PushL( uahArray ); // CS:6
                uahArray->AppendL( userAgentHeader );
                if ( userAgentHeader.Length() > 0 )
                    {
                    User::LeaveIfError( cSIPManagedProfile->SetParameter( 
                        KSIPHeaders, *uahArray ) );
                    }

                cSIPManagedProfileRegistry->SaveL( *cSIPManagedProfile );
                uahArray->Reset();
                CleanupStack::PopAndDestroy( uahArray ); // CS:5
                CleanupStack::PopAndDestroy( cSIPManagedProfile ); // CS:4
                CleanupStack::Pop( cSIPProfile ); // CS:3
                cSIPProfile = NULL;
                CleanupStack::PopAndDestroy( 
                    cSIPManagedProfileRegistry );// CS:2
                CleanupStack::PopAndDestroy( cSIPProfRegObserver );// CS:1
                DBG_PRINT(
                    "CWVoIPAdapter::SettingsSavedL - \
                    SIP User-Agent Header created and set." );
                } // if ( matches ... )
            } // for
        }
    else if ( KErrNone == KSCCPAppID8().CompareF( tmpAppId->Des() ) )
        {
        for ( TInt itemIndex = 0; itemIndex < iDatas.Count(); itemIndex++ )
            {
            iDatas[itemIndex]->SetStorageId( CWPVoIPItem::ESCCP, storageId, 
                aAppRef );
            }
        }
    else if ( KErrNone == KNATFWAppID8().CompareF( tmpAppId->Des() ) )
        {
        for ( TInt itemIndex = 0; itemIndex < iDatas.Count(); itemIndex++ )
            {
            iDatas[itemIndex]->SetStorageId( CWPVoIPItem::ENATFW, storageId,
                aAppRef );
            }
        }
    else if ( KErrNone == KPresenceAppId8().CompareF( tmpAppId->Des() ) )
        {
        for ( TInt itemIndex = 0; itemIndex < iDatas.Count(); itemIndex++ )
            {
            iDatas[itemIndex]->SetStorageId( CWPVoIPItem::EPresence, 
                storageId, aAppRef );
            }
        }
    else if ( KErrNone == KSNAPAppID8().CompareF( tmpAppId->Des() ) )
        {
        for ( TInt itemIndex = 0; itemIndex < iDatas.Count(); itemIndex++ )
            {
            iDatas[itemIndex]->SetStorageId( CWPVoIPItem::ESNAP, storageId, 
                aAppRef );
            }
        }

    CleanupStack::PopAndDestroy( tmpAppId ); // CS:0
    DBG_PRINT( "CWVoIPAdapter::SettingsSavedL - end" );
    }


// ---------------------------------------------------------------------------
// CWPVoIPAdapter::SavingFinalizedL()
// ---------------------------------------------------------------------------
//
void CWPVoIPAdapter::SavingFinalizedL()
    {
    DBG_PRINT( "CWVoIPAdapter::SavingFinalizedL - begin" );
    // Prerequisites: All data received and is stored at least transiently in
    // the data structures in VoIP items. Call SavingFinalized for everyone to
    // make them update the APPREF/TO-APPREF linkage data.
    for ( TInt itemIndex = 0; itemIndex < iDatas.Count(); itemIndex++ )
        {
        iDatas[itemIndex]->SavingFinalizedL();
        }// for
    DBG_PRINT( "CWVoIPAdapter::SavingFinalizedL - end" );
    }

// ---------------------------------------------------------------------------
// CWPVoIPAdapter::DescToInt()
// ---------------------------------------------------------------------------
//
TInt CWPVoIPAdapter::DescToInt( HBufC8* aDescValue, TInt& aIntValue ) const
    {
    TInt err( KErrNotFound );
    if ( aDescValue )
        {
        TLex8 lex( aDescValue->Des() );
        err = lex.Val( aIntValue );    
        }    
    return err;
    }

// ---------------------------------------------------------------------------
// CWPVoIPAdapter::IapIdFromWapIdL
// Returns IAP ID from WAP ID.
// ---------------------------------------------------------------------------
//
TUint32 CWPVoIPAdapter::IapIdFromWapIdL( TUint32 aWapId )
    {
    DBG_PRINT( "CWPVoIPAdapter::IapIdFromWapIdL - begin" );
    RCmManagerExt cmManager;
    cmManager.OpenLC(); // CS:1
    RArray<TUint32> iapIds;
    TUint32 iapId( 0 );

    // First get all free IAP ID's.
    cmManager.ConnectionMethodL( iapIds );
    CleanupClosePushL( iapIds ); // CS:2
 
    // Then get IAP ID's from all destinations.
    RArray<TUint32> destIds;
    cmManager.AllDestinationsL( destIds );
    CleanupClosePushL( destIds ); // CS:3
    
    TInt destCount = destIds.Count();
    
    for ( TInt destIndex = 0; destIndex < destCount; destIndex++ )
        {
        RCmDestinationExt dest = cmManager.DestinationL( 
            destIds[destIndex] );
        CleanupClosePushL( dest ); // CS:4
        TInt cmCount = dest.ConnectionMethodCount();
        for ( TInt cmIndex = 0; cmIndex < cmCount; cmIndex++ )
            {
            TUint32 apId = dest.ConnectionMethodL( 
                cmIndex ).GetIntAttributeL( CMManager::ECmIapId );
            iapIds.AppendL( apId );
            }
        CleanupStack::PopAndDestroy( &dest ); // CS:3
        }
    // Finally, go through all connection methods and find correct IAP ID.
    const TInt cmCount = iapIds.Count();
    for ( TInt counter = 0; counter < cmCount; counter++ )
        {
        TUint32 id = cmManager.GetConnectionMethodInfoIntL( 
            iapIds[counter], CMManager::ECmWapId );

        if ( id == aWapId )
            {
            iapId = iapIds[counter];
            // No need to go through rest of IAPs.
            break;
            }
        }
    
    // PopAndDestroy destIds, iapIds, cmManager.
    CleanupStack::PopAndDestroy( 3, &cmManager ); // CS:0

    DBG_PRINT( "CWPVoIPAdapter::IapIdFromWapIdL - end" );
    return iapId;
    }

//  End of File  
