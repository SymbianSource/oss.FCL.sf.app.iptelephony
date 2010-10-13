/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Receives and stores general VoIP settings.
*
*/


#include <e32base.h>
#include <CWPAdapter.h>
#include <CWPCharacteristic.h>
#include <CWPParameter.h>
#include <WPGeneralVoIPSettingsAdapter.rsg>
#include <bldvariant.hrh>
#include <commdb.h>
#include <e32des8.h>
#include <WPAdapterUtil.h> // Adapter utils
#include <utf.h> // Unicode conversion
#include <crcseprofileentry.h>
#include <crcseprofileregistry.h>
#include <settingsinternalcrkeys.h>
#include <centralrepository.h>
#include <featmgr.h>

#include "cwpgeneralvoipsettingsadapter.h"

// Application related constants.
_LIT( KGeneralVoIPSettingsAdapterName, "WPGeneralVoIPSettingsAdapter");
_LIT( KGeneralVoIPSettingsAppID, "w9033" ); // OMA CP registration document.
const TInt KMaxCharsInTUint32 = 10;
_LIT8( KVoIPAppID8,       "w9013" );

// RESOURCE parameters.
// OMA CP registration document for general VoIP settings.
_LIT( KParmCLIR,           "CLIR" );
_LIT( KParmACBRULE,        "ACBRULE" );
_LIT( KParmPREFTEL,        "PREFTEL" );
_LIT( KParmDND,            "DND" );
_LIT( KParmPSCALLWAITING,  "PSCALLWAITING" );
// GOANNA:
_LIT( KParmPREFTELLOCK,    "PREFTELLOCK" );
_LIT( KParmCSRADIOWIFIOFF, "CSRADIOWIFIOFF" );


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CWPGeneralVoIPSettingsAdapter::CWPGeneralVoIPSettingsAdapter
// ---------------------------------------------------------------------------
//
CWPGeneralVoIPSettingsAdapter::CWPGeneralVoIPSettingsAdapter() : CWPAdapter()
    {
    }

// ---------------------------------------------------------------------------
// CWPGeneralVoIPSettingsAdapter::ConstructL
// ---------------------------------------------------------------------------
//
void CWPGeneralVoIPSettingsAdapter::ConstructL()
    {
    TFileName fileName;
    Dll::FileName( fileName );
    
    iTitle = WPAdapterUtil::ReadHBufCL( fileName,
        KGeneralVoIPSettingsAdapterName,
        R_QTN_SM_HEAD_GENERAL_VOIP_SETTINGS );
    
    iDefaultName = WPAdapterUtil::ReadHBufCL( fileName,
        KGeneralVoIPSettingsAdapterName,
        R_QTN_SM_DEFAULT_NAME_GENERAL_VOIP_SETTINGS );
    
    iName = HBufC::NewL( 0 );
    iToAppref = HBufC8::NewL( 0 );

    _LIT8( KFormatTxt,"%u" );
    iSavedDataId    = HBufC8::NewL( KMaxCharsInTUint32 );
    iSavedDataId->Des().Format( KFormatTxt, KCRUidRichCallSettings );

    iIsAppIdOk      = EFalse;
    iIsNameOk       = EFalse;
    iClir           = KNotSet;
    iACBRule        = KNotSet;
    iPrefTel        = KNotSet;
    iDnd            = KNotSet;
    iPSCallWaiting  = KNotSet;
    iPrefTelLock    = KNotSet;
    iCsRadioWifiOff = KNotSet;
    iVoipId         = (TUint32)KNotSet;

    FeatureManager::InitializeLibL();
    iFeatMgrInitialized = ETrue;
    
    if ( !FeatureManager::FeatureSupported( KFeatureIdCommonVoip ) )
        {
        User::Leave( KErrNotSupported );
        }
    }

// ---------------------------------------------------------------------------
// CWPGeneralVoIPSettingsAdapter::NewL
// ---------------------------------------------------------------------------
//
CWPGeneralVoIPSettingsAdapter* CWPGeneralVoIPSettingsAdapter::NewL()
    {
    CWPGeneralVoIPSettingsAdapter* self = new (ELeave) 
        CWPGeneralVoIPSettingsAdapter;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------------------------
// CWPGeneralVoIPSettingsAdapter::~CWPGeneralVoIPSettingsAdapter
// ---------------------------------------------------------------------------
//
CWPGeneralVoIPSettingsAdapter::~CWPGeneralVoIPSettingsAdapter()
    {
    delete iTitle;
    delete iDefaultName;
    delete iName;
    delete iSavedDataId;
    delete iToAppref;

    if ( iFeatMgrInitialized )
        {
        FeatureManager::UnInitializeLib();
        }
    }

// ---------------------------------------------------------------------------
// CWPGeneralVoIPSettingsAdapter::SetNameL
// Set iName, the user viewable name of general VoIP settings.
// ---------------------------------------------------------------------------
//
void CWPGeneralVoIPSettingsAdapter::SetNameL( const TDesC& aName )
    {
    if ( !iIsNameOk )
        {
        delete iName;
        iName = NULL;
        iName = aName.AllocL();
        }
    }

// ---------------------------------------------------------------------------
// CWPGeneralVoIPSettingsAdapter::SetClir
// Sets CLIR in enabled or disabled mode.
// ---------------------------------------------------------------------------
//
void CWPGeneralVoIPSettingsAdapter::SetClir( const TInt aClir )
    {
    iClir = aClir;
    }

// ---------------------------------------------------------------------------
// CWPGeneralVoIPSettingsAdapter::SetACBRule
// Sets a rule for Anonymous Call Blocking feature.
// ---------------------------------------------------------------------------
//
void CWPGeneralVoIPSettingsAdapter::SetACBRule( const TInt aACBRule )
    {
    iACBRule = aACBRule;
    }

// ---------------------------------------------------------------------------
// CWPGeneralVoIPSettingsAdapter::SetPrefTel
// Sets the preferred telephony mode.
// ---------------------------------------------------------------------------
//
void CWPGeneralVoIPSettingsAdapter::SetPrefTel( const TInt aPrefTel )
    {
    iPrefTel = aPrefTel;
    }

// ---------------------------------------------------------------------------
// CWPGeneralVoIPSettingsAdapter::SetDnd
// Enables or disables the Do Not Disturb setting.
// ---------------------------------------------------------------------------
//
void CWPGeneralVoIPSettingsAdapter::SetDnd( const TInt aDnd )
    {
    iDnd = aDnd;
    }

// ---------------------------------------------------------------------------
// CWPGeneralVoIPSettingsAdapter::SetPSCallWaiting
// Sets call waiting in enabled or disabled mode.
// ---------------------------------------------------------------------------
//
void CWPGeneralVoIPSettingsAdapter::SetPSCallWaiting( 
    const TInt aPSCallWaiting )
    {
    iPSCallWaiting = aPSCallWaiting;
    }

// ---------------------------------------------------------------------------
// CWPGeneralVoIPSettingsAdapter::SetPrefTelLock
// Enables or disables preferred telephony mode to be edited from UI.
// ---------------------------------------------------------------------------
//
void CWPGeneralVoIPSettingsAdapter::SetPrefTelLock( const TInt aPrefTelLock )
    {
    iPrefTelLock = aPrefTelLock;
    }


// ---------------------------------------------------------------------------
// CWPGeneralVoIPSettingsAdapter::SetCsRadioWifiOff
// Sets CS radio in WiFi mode in turned on or off mode.
// ---------------------------------------------------------------------------
//
void CWPGeneralVoIPSettingsAdapter::SetCsRadioWifiOff( 
    const TInt aCsRadioWifiOff )
    {
    iCsRadioWifiOff = aCsRadioWifiOff;
    }

// ---------------------------------------------------------------------------
// CWPGeneralVoIPSettingsAdapter::SetToApprefL
// Sets TO-APPREF if it hasn't been set before.
// ---------------------------------------------------------------------------
//
void CWPGeneralVoIPSettingsAdapter::SetToApprefL( const TDesC8& aToAppRef )
    {
    if ( !iToApprefSet )
        {        
        delete iToAppref;
        iToAppref = NULL;
        iToAppref = aToAppRef.AllocL(); 
        }
    }

// ---------------------------------------------------------------------------
// CWPGeneralVoIPSettingsAdapter::Name
// Returns the user viewable name of general VoIP settings.
// ---------------------------------------------------------------------------
//
TDesC& CWPGeneralVoIPSettingsAdapter::Name()
    {
    return *iName;
    }

// ---------------------------------------------------------------------------
// CWPGeneralVoIPSettingsAdapter::Clir
// Tells whether CLIR is enabled or disabled.
// ---------------------------------------------------------------------------
//
TInt CWPGeneralVoIPSettingsAdapter::Clir()
    {
    return iClir;
    }

// ---------------------------------------------------------------------------
// CWPGeneralVoIPSettingsAdapter::ACBRule
// Returns the rule of Anonymous Call Blocking feature.
// ---------------------------------------------------------------------------
//
TInt CWPGeneralVoIPSettingsAdapter::ACBRule()
    {
    return iACBRule;
    }

// ---------------------------------------------------------------------------
// CWPGeneralVoIPSettingsAdapter::PrefTel
// Returns the preferred telephony mode.
// ---------------------------------------------------------------------------
//
TInt CWPGeneralVoIPSettingsAdapter::PrefTel()
    {
    return iPrefTel;
    }

// ---------------------------------------------------------------------------
// CWPGeneralVoIPSettingsAdapter::Dnd
// Returns the state of the Do Not Disturb setting.
// ---------------------------------------------------------------------------
//
TInt CWPGeneralVoIPSettingsAdapter::Dnd()
    {
    return iDnd;
    }

// ---------------------------------------------------------------------------
// CWPGeneralVoIPSettingsAdapter::PSCallWaiting
// Returns the state of VoIP call waiting feature.
// ---------------------------------------------------------------------------
//
TInt CWPGeneralVoIPSettingsAdapter::PSCallWaiting()
    {
    return iPSCallWaiting;
    }

// ---------------------------------------------------------------------------
// CWPGeneralVoIPSettingsAdapter::PrefTelLock
// Returns preferred telephony lock, i.e. tells whether preferred telephony
// can be edited from UI.
// ---------------------------------------------------------------------------
//
TInt CWPGeneralVoIPSettingsAdapter::PrefTelLock()
    {
    return iPrefTelLock;
    }

// ---------------------------------------------------------------------------
// CWPGeneralVoIPSettingsAdapter::CsRadioWifiOff
// Tells whether CS radio is turned off in WiFi mode.
// ---------------------------------------------------------------------------
//
TInt CWPGeneralVoIPSettingsAdapter::CsRadioWifiOff()
    {
    return iCsRadioWifiOff;
    }

// ---------------------------------------------------------------------------
// CWPGeneralVoIPSettingsAdapter::ItemCount
// From class CWPAdapter.
// Checks the number of settings items. Since there can only be one set of
// general VoIP settings, always return 1.
// ---------------------------------------------------------------------------
//
TInt CWPGeneralVoIPSettingsAdapter::ItemCount() const
    {
    return iIsAppIdOk ? 1 : 0;
    }

// ---------------------------------------------------------------------------
// From class CWPAdapter.
// CWPGeneralVoIPSettingsAdapter::SummaryTitle
// Returns the summary title of general VoIP settings item.
// ---------------------------------------------------------------------------
//
const TDesC16& CWPGeneralVoIPSettingsAdapter::SummaryTitle( 
    TInt /*aIndex*/ ) const
    {
    if ( iIsAppIdOk )
        {
        return *iTitle;
        }
    return KNullDesC16;
    }

// ---------------------------------------------------------------------------
// CWPGeneralVoIPSettingsAdapter::SummaryText
// From class CWPAdapter.
// Returns the summary text of general VoIP settings.
// ---------------------------------------------------------------------------
//
const TDesC16& CWPGeneralVoIPSettingsAdapter::SummaryText( 
    TInt /*aIndex*/ ) const
    {
    if ( iIsAppIdOk )
        {
        if ( 0 == iName->Length() )
            {
            return *iDefaultName;
            }
        return *iName;
        }
    return KNullDesC16;
    }

// ---------------------------------------------------------------------------
// CWPGeneralVoIPSettingsAdapter::SaveL
// From class CWPAdapter.
// Saves the general VoIP settings via Central Repository.
// ---------------------------------------------------------------------------
//
void CWPGeneralVoIPSettingsAdapter::SaveL( TInt /*aIndex*/ )
    {
    // Settings will not be saved if the APPID parameter is not w9033, i.e. 
    // OMA APPID for general VoIP settings.
    if ( iIsAppIdOk )
        {
        CRepository* cRepository = CRepository::NewLC( 
            KCRUidRichCallSettings );
        if( KNotSet != Clir() )
            {
            User::LeaveIfError( cRepository->Set( KRCSEClir, Clir() ) );
            }
        if ( KNotSet != ACBRule() )
            {
            User::LeaveIfError( cRepository->Set( KRCSEAnonymousCallBlockRule,
                ACBRule() ) );
            }
        if ( KNotSet != PrefTel() )
            {
            User::LeaveIfError( cRepository->Set( KRCSEPreferredTelephony,
                PrefTel() ) );
            }
        if ( KNotSet != Dnd() )
            {
            User::LeaveIfError( cRepository->Set( KRCSEDoNotDisturb,
                Dnd() ) );
            }
        if ( KNotSet != PSCallWaiting() )
            {
            User::LeaveIfError( cRepository->Set( KRCSPSCallWaiting,
                PSCallWaiting() ) );
            }
        CleanupStack::PopAndDestroy( cRepository );
        }
    }

// ---------------------------------------------------------------------------
// CWPGeneralVoIPSettingsAdapter::CanSetAsDefault
// From class CWPAdapter.
// Return EFalse since there can only be one set of general VoIP settings.
// ---------------------------------------------------------------------------
//
TBool CWPGeneralVoIPSettingsAdapter::CanSetAsDefault( TInt /*aIndex*/ ) const
    {
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CWPGeneralVoIPSettingsAdapter::DetailsL
// From class CWPAdapter.
// Not supported.
// ---------------------------------------------------------------------------
//
TInt CWPGeneralVoIPSettingsAdapter::DetailsL( TInt /*aItem*/, 
    MWPPairVisitor& /*aVisitor */ )
    {
    return KErrNotSupported;
    }  

// ---------------------------------------------------------------------------
// CWPGeneralVoIPSettingsAdapter::VisitL
// From class CWPAdapter.
// ---------------------------------------------------------------------------
//
void CWPGeneralVoIPSettingsAdapter::VisitL( 
    CWPCharacteristic& aCharacteristic )
    {
    switch( aCharacteristic.Type() )
        {
        // Core general VoIP settings.
        case KWPApplication:
            {
            aCharacteristic.AcceptL( *this );
            if ( 0 == iAppID.Compare( KGeneralVoIPSettingsAppID() ) )
                {
                iIsAppIdOk = ETrue;
                iIsNameOk = ETrue;
                iToApprefSet = ETrue; // TO-APPREF can't be modified anymore.
                }
            if ( !iIsAppIdOk )
                {
                iName->Des().Zero(); // Otherwise iName may have wrong value.
                }
            break;
            }
        default:
            break;
        }
    }


// ---------------------------------------------------------------------------
// CWPGeneralVoIPSettingsAdapter::SettingsSavedL
// From class CWPAdapter.
// ---------------------------------------------------------------------------
// 
void CWPGeneralVoIPSettingsAdapter::SettingsSavedL( const TDesC8& aAppIdOfSavingItem,
    const TDesC8& aAppRef, const TDesC8& aStorageIdValue )
    {
    if ( PrefTel() && KVoIPAppID8().CompareF( aAppIdOfSavingItem ) == 0 &&
         0 == iToAppref->Compare( aAppRef ) )
        {
        TInt voipId;
        TLex8 lex( aStorageIdValue );
        User::LeaveIfError( lex.Val( voipId ) );
        iVoipId = voipId;
        }
    }

// ---------------------------------------------------------------------------
// CWPGeneralVoIPSettingsAdapter::VisitL
// From class CWPAdapter.
// General VoIP settings parameters are handled in this method.
// ---------------------------------------------------------------------------
//
void CWPGeneralVoIPSettingsAdapter::VisitL( CWPParameter& aParameter )
    {
    // tmpValue holds the value converted from Unicode to UTF8.
    HBufC8* tmpValue = HBufC8::NewLC( aParameter.Value().Length() ); // CS:1
    TPtr8 ptrTmpValue( tmpValue->Des() );
    CnvUtfConverter::ConvertFromUnicodeToUtf8( ptrTmpValue,
        aParameter.Value() );
    
    switch ( aParameter.ID() )
        {
        case EWPParameterToAppRef:
            {
            SetToApprefL( tmpValue->Des() );
            }
            break;
            
        case EWPParameterAppID:
            {
	        iAppID.Set( aParameter.Value() );
            break;
            }
        
        // Case 0 handles the general VoIP parameters that are extensions to
        // OMA Client Provisioning parameter set.
        case 0:
            if ( 0 == aParameter.Name().Compare( KParmCLIR ) )
                {
                TInt tIntParameterValue;
                TLex8 lex( tmpValue->Des() );
                TInt err( KErrNone );            
                err = lex.Val( tIntParameterValue );
                if ( KErrNone != err )
                    {
                    break;
                    }
                SetClir( tIntParameterValue );    
                }
            else if ( 0 == aParameter.Name().Compare( KParmACBRULE ) )
                {
                TInt tIntParameterValue;
                TLex8 lex( tmpValue->Des() );
                TInt err( KErrNone );            
                err = lex.Val( tIntParameterValue );
                if ( KErrNone != err )
                    {
                    break;
                    }
                SetACBRule( tIntParameterValue );    
                }
            else if ( 0 == aParameter.Name().Compare( KParmPREFTEL ) )
                {
                TInt tIntParameterValue;
                TLex8 lex( tmpValue->Des() );
                TInt err( KErrNone );            
                err = lex.Val( tIntParameterValue );
                if ( KErrNone != err )
                    {
                    break;
                    }
                SetPrefTel( tIntParameterValue );
                }
            else if ( 0 == aParameter.Name().Compare( KParmDND ) )
                {
                TInt tIntParameterValue;
                TLex8 lex( tmpValue->Des() );
                TInt err( KErrNone );            
                err = lex.Val( tIntParameterValue );
                if ( KErrNone != err )
                    {
                    break;
                    }
                SetDnd( tIntParameterValue );    
                }
            else if ( 0 == aParameter.Name().Compare( KParmPSCALLWAITING ) )
                {
                TInt tIntParameterValue;
                TLex8 lex( tmpValue->Des() );
                TInt err( KErrNone );            
                err = lex.Val( tIntParameterValue );
                if ( KErrNone != err )
                    {
                    break;
                    }
                SetPSCallWaiting( tIntParameterValue );    
                }
            else if ( 0 == aParameter.Name().Compare( KParmPREFTELLOCK ) )
                {
                TInt tIntParameterValue;
                TLex8 lex( tmpValue->Des() );
                TInt err( KErrNone );            
                err = lex.Val( tIntParameterValue );
                if ( KErrNone != err )
                    {
                    break;
                    }
                SetPrefTelLock( tIntParameterValue );    
                }
            else if ( 0 == aParameter.Name().Compare( KParmCSRADIOWIFIOFF ) )
                {
                TInt tIntParameterValue;
                TLex8 lex( tmpValue->Des() );
                TInt err( KErrNone );            
                err = lex.Val( tIntParameterValue );
                if ( KErrNone != err )
                    {
                    break;
                    }
                SetCsRadioWifiOff( tIntParameterValue );    
                }
            break;
            
        case EWPParameterName: // APPLICATION/NAME
            SetNameL( aParameter.Value() );
            break;

		default:
			break;
        }// switch
    CleanupStack::PopAndDestroy( tmpValue ); // CS:0				
    }

// ---------------------------------------------------------------------------
// CWPGeneralVoIPSettingsAdapter::SavingFinalizedL
// From class CWPAdapter.
// ---------------------------------------------------------------------------
//
void CWPGeneralVoIPSettingsAdapter::SavingFinalizedL()
    {
    if ( (TUint32)KNotSet == iVoipId )
        {
        return;
        }

    // Get service ID from RCSE.
    CRCSEProfileRegistry* registry = CRCSEProfileRegistry::NewLC();
    CRCSEProfileEntry* entry = CRCSEProfileEntry::NewLC();        

    registry->FindL( iVoipId, *entry );        
    TUint serviceId = entry->iServiceProviderId;

    CleanupStack::PopAndDestroy( 2, registry ); // entry, registry

    // Set the preferred service ID.
    CRepository* repository = CRepository::NewL( KCRUidRichCallSettings );
    repository->Set( KRCSPSPreferredService, (TInt)serviceId );
    delete repository;
    }

// ---------------------------------------------------------------------------
// CWPGeneralVoIPSettingsAdapter::ContextExtension
// From class MWPContextExtension.
// Returns a pointer to context extension.
// ---------------------------------------------------------------------------
//
TInt CWPGeneralVoIPSettingsAdapter::ContextExtension( 
    MWPContextExtension*& aExtension )
    {
    aExtension = this;
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CWPGeneralVoIPSettingsAdapter::SaveDataL
// From class MWPContextExtension.
// Returns ID of saved data.
// ---------------------------------------------------------------------------
//
const TDesC8& CWPGeneralVoIPSettingsAdapter::SaveDataL( TInt /*aIndex*/ ) const
    {
    return *iSavedDataId;
    }

// ---------------------------------------------------------------------------
// CWPGeneralVoIPSettingsAdapter::DeleteL
// From class MWPContextExtension.
// Deletes general VoIP settings from Central Repository.
// ---------------------------------------------------------------------------
//
void CWPGeneralVoIPSettingsAdapter::DeleteL( const TDesC8& aSaveData )
    {
    TInt tInt( 0 );
    TLex8 lex( aSaveData );
    TInt err( KErrNone );            
    err = lex.Val( tInt );
    TUid cenRepUid = { tInt };
    if ( KErrNone == err && KCRUidRichCallSettings == cenRepUid )
        {
        CRepository* cRepository = CRepository::NewLC( cenRepUid );
        User::LeaveIfError( cRepository->Reset( KRCSEClir ) );
        User::LeaveIfError( cRepository->Reset(
            KRCSEAnonymousCallBlockRule ) );
        User::LeaveIfError( cRepository->Reset( KRCSEPreferredTelephony ) );
        User::LeaveIfError( cRepository->Reset( KRCSEDoNotDisturb ) );
        User::LeaveIfError( cRepository->Reset( KRCSPSCallWaiting ) );
        User::LeaveIfError( cRepository->Reset( KRCSPSPreferredService ) );
        
        CleanupStack::PopAndDestroy( cRepository );
        cRepository = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CWPGeneralVoIPSettingsAdapter::Uid
// From class MWPContextExtension.
// Returns the UID of this adapter.
// ---------------------------------------------------------------------------
//
TUint32 CWPGeneralVoIPSettingsAdapter::Uid() const
    {
    return iDtor_ID_Key.iUid;
    }

//  End of File  
