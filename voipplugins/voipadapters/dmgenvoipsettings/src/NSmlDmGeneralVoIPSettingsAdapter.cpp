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
* Description:  Device Management General VoIP Settings Adapter
*
*/


#include <utf.h>
#include <implementationproxy.h> // For TImplementationProxy definition
#include <sysutil.h>
#include <e32property.h>
#include <centralrepository.h>
#include <crcseprofileentry.h>
#include <crcseprofileregistry.h>
#include <settingsinternalcrkeys.h>
#include <featmgr.h>

#include "NSmlDmGeneralVoIPSettingsAdapter.h"
#include "nsmlconstants.h"
#include "nsmldmconstants.h"
#include "nsmldmconst.h"

const TUint KNSmlDMGenVoIPAdapterImplUid = 0x10275413;
const TInt  KNSmlGenVoIPMaxResultLength  = 256;
const TInt  KNSmlGenVoIPMaxUriLength     = 100;
const TInt  KDefaultResultSize           = 64;
const TInt  KTwoSegs                     = 2;
const TInt  KMaxUriLength                = 64;

// Reference: OMA_Device_Management_DDF_for_General_VoIP_Settings
_LIT8( KNSmlDMGenVoIPDDFVersion,            "0.1" );
_LIT8( KNSmlDMGenVoIPNodeName,              "GenVoIP" );
_LIT8( KNSmlDMVoIPNodeName,                 "VoIP" );
_LIT8( KNSmlDMGenVoIPClir,                  "Clir" );
_LIT8( KNSmlDMGenVoIPAnonCallBlockRule,     "AnonCallBlockRule" );
_LIT8( KNSmlDMGenVoIPPreferredTelephony,    "PrefTelephony" );
_LIT8( KNSmlDMGenVoIPDoNotDisturb,          "DoNotDisturb" );
_LIT8( KNSmlDMGenVoIPPSCallWaiting,         "PSCallWaiting" );
_LIT8( KNSmlDMGenVoIPProfileId,             "VoIPProfileId" );

_LIT8( KNSmlDMGenVoIPNodeNameExp,           "Main node for General VoIP settings" );
_LIT8( KNSmlDMGenVoIPClirExp,               "Calling Line Identification Restriction" );
_LIT8( KNSmlDMGenVoIPPSCallWaitingExp,      "PS call waiting" );
_LIT8( KNSmlDMGenVoIPAnonCallBlockRuleExp,  "Anonymous call blocking rule" );
_LIT8( KNSmlDMGenVoIPPreferredTelephonyExp, "Preferred telephony" );
_LIT8( KNSmlDMGenVoIPDoNotDisturbExp,       "Do not disturb" );
_LIT8( KNSmlDMGenVoIPProfileIdExp,          "VoIP node id for linking" );

// Other
_LIT8( KNSmlDMGenVoIPTextPlain,             "text/plain" );
_LIT8( KNSmlDMGenVoIPSeparator,             "/" );
_LIT8( KNSmlDMGenVoIPUriDotSlash,           "./");
_LIT8( KVoipId,                             "VoIPId" );

//MACROS 
#define KNSMLDMURISEPARATORDEF '/'

// Following lines are for enabling debug prints.
#ifdef _DEBUG
#define _DBG_FILE(p) RDebug::Print(_L(p))
#define _DBG_FILE2(p,a) RDebug::Print(_L(p),a)
#else
#define _DBG_FILE(p)
#define _DBG_FILE2(p,a)
#endif // _DEBUG

// ======== LOCAL FUNCTIONS ========

template <class T>
inline void CleanupResetAndDestroy<T>::PushL(T& aRef)
	{CleanupStack::PushL(TCleanupItem(&ResetAndDestroy,&aRef));}

template <class T>
void CleanupResetAndDestroy<T>::ResetAndDestroy(TAny *aPtr)
	{(STATIC_CAST(T*,aPtr))->ResetAndDestroy();}

template <class T>
inline void CleanupResetAndDestroyL(T& aRef)
	{CleanupResetAndDestroy<T>::PushL(aRef);}


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CNSmlDmGeneralVoIPSettingsAdapter::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CNSmlDmGeneralVoIPSettingsAdapter::ConstructL()
    {
    _DBG_FILE( "CNSmlDmGeneralVoIPSettingsAdapter::ConstructL(): begin" );
    // Check VoIP support from feature manager
    FeatureManager::InitializeLibL();
    TBool support = FeatureManager::FeatureSupported( KFeatureIdCommonVoip );
    FeatureManager::UnInitializeLib();

    if( !support )
        {
        _DBG_FILE( 
            "CNSmlDmGeneralVoIPSettingsAdapter::ConstructL(): no support" );
        User::Leave( KErrNotSupported );
        }
    _DBG_FILE( "CNSmlDmGeneralVoIPSettingsAdapter::ConstructL(): end" );
    }

// ---------------------------------------------------------------------------
// CNSmlDmGeneralVoIPSettingsAdapter* CNSmlDmGeneralVoIPSettingsAdapter::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CNSmlDmGeneralVoIPSettingsAdapter* CNSmlDmGeneralVoIPSettingsAdapter::NewL( 
    MSmlDmCallback* aDmCallback )
    {
    _DBG_FILE( "CNSmlDmGeneralVoIPSettingsAdapter::NewL(): begin" );
    CNSmlDmGeneralVoIPSettingsAdapter* self = NewLC( aDmCallback );
    CleanupStack::Pop( self );
    _DBG_FILE( "CNSmlDmGeneralVoIPSettingsAdapter::NewL(): end" );
    return self;
    }

// ---------------------------------------------------------------------------
// CNSmlDmGeneralVoIPSettingsAdapter* CNSmlDmGeneralVoIPSettingsAdapter::NewLC
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CNSmlDmGeneralVoIPSettingsAdapter* CNSmlDmGeneralVoIPSettingsAdapter::NewLC( 
    MSmlDmCallback* aDmCallback )
    {
    _DBG_FILE( "CNSmlDmGeneralVoIPSettingsAdapter::NewLC(): begin" );
    CNSmlDmGeneralVoIPSettingsAdapter* self = 
        new ( ELeave ) CNSmlDmGeneralVoIPSettingsAdapter( aDmCallback );
    CleanupStack::PushL( self );

    self->ConstructL();
    self->iDmCallback = aDmCallback;
    
    _DBG_FILE( "CNSmlDmGeneralVoIPSettingsAdapter::NewLC(): end" );
    return self;
    }
    
// ---------------------------------------------------------------------------
// CNSmlDmGeneralVoIPSettingsAdapter::~CNSmlDmGeneralVoIPSettingsAdapter()
// Destructor
// ---------------------------------------------------------------------------
//
CNSmlDmGeneralVoIPSettingsAdapter::~CNSmlDmGeneralVoIPSettingsAdapter()
    {
    _DBG_FILE( "CNSmlDmGeneralVoIPSettingsAdapter::~CNSmlDmGeneralVoIPSettingsAdapter(): begin" );
    _DBG_FILE( "CNSmlDmGeneralVoIPSettingsAdapter::~CNSmlDmGeneralVoIPSettingsAdapter(): end" );
    }

// ---------------------------------------------------------------------------
// CNSmlDmGeneralVoIPSettingsAdapter::CNSmlDmGeneralVoIPSettingsAdapter
// ---------------------------------------------------------------------------
//
CNSmlDmGeneralVoIPSettingsAdapter::CNSmlDmGeneralVoIPSettingsAdapter(
    TAny* aEcomArguments ):CSmlDmAdapter( aEcomArguments )
    {
    _DBG_FILE( "CNSmlDmGeneralVoIPSettingsAdapter::CNSmlDmGeneralVoIPSettingsAdapter( aEcomArguments ): begin" );
    iEntry.iAreSettingsLoaded = EFalse;
    _DBG_FILE( "CNSmlDmGeneralVoIPSettingsAdapter::CNSmlDmGeneralVoIPSettingsAdapter( aEcomArguments ): end" ); 
    }

// ---------------------------------------------------------------------------
// CNSmlDmGeneralVoIPSettingsAdapter::FetchObjectL
// Fetches the values of leaf objects.
// ---------------------------------------------------------------------------
//
CSmlDmAdapter::TError CNSmlDmGeneralVoIPSettingsAdapter::FetchObjectL( 
    const TDesC8& aURI, 
    CBufBase& aResult )
    {
    _DBG_FILE( "CNSmlDmGeneralVoIPSettingsAdapter::FetchObjectL(): begin" );
    CSmlDmAdapter::TError status = CSmlDmAdapter::EError;
    TBuf8< KNSmlGenVoIPMaxResultLength > segmentResult;
    TPtrC8 lastURISeg = LastURISeg( aURI );
    TPtrC8 uriTmp = RemoveLastURISeg( aURI );
    TPtrC8 secondLastURISeg = LastURISeg( uriTmp );

    if ( !( iEntry.iAreSettingsLoaded ) )
        {
        TInt err = LoadSettingsL();
        if ( KErrNone != err )
            {
            return status;
            }
        }

    if ( 0 == secondLastURISeg.Compare( KNSmlDMGenVoIPNodeName() )
        && KTwoSegs == NumOfURISegs( aURI ) )
        {
        status = CSmlDmAdapter::EOk;
        // Clir
        if ( KNSmlDMGenVoIPClir() == lastURISeg )
            {
            segmentResult.Num( iEntry.iClir );
            }
        // AnonCallBlockRule
        else if ( KNSmlDMGenVoIPAnonCallBlockRule() == lastURISeg )
            {
            segmentResult.Num( iEntry.iACBRule );
            }
        // PreferredTelephony
        else if ( KNSmlDMGenVoIPPreferredTelephony() == lastURISeg )
            {
            segmentResult.Num( iEntry.iPrefTel );
            }
        // DoNotDisturb
        else if ( KNSmlDMGenVoIPDoNotDisturb() == lastURISeg )
            {
            segmentResult.Num( iEntry.iDnd );
            }
        // PSCallWaiting
        else if ( KNSmlDMGenVoIPPSCallWaiting() == lastURISeg )
            {
            segmentResult.Num( iEntry.iPSCallWaiting );
            }
        // ProfileName
        else if ( KNSmlDMGenVoIPProfileId() == lastURISeg )
            {
            segmentResult.Copy( iEntry.iVoipProfileId );
            }
        else
            {
            status = CSmlDmAdapter::ENotFound;
            }
        }
    else
        {
        status = CSmlDmAdapter::ENotFound;
        }

    if ( CSmlDmAdapter::EOk == status )    
        {
        aResult.InsertL( aResult.Size(), segmentResult );
        }
    _DBG_FILE( "CNSmlDmGeneralVoIPSettingsAdapter::FetchObjectL(): end" );
    return status;
    }

// ---------------------------------------------------------------------------
// CNSmlDmGeneralVoIPSettingsAdapter::LoadSettingsL
// Loads general VoIP settings from Central Repository.
// ---------------------------------------------------------------------------
//
TInt CNSmlDmGeneralVoIPSettingsAdapter::LoadSettingsL()
    {
    _DBG_FILE( "CNSmlDmGeneralVoIPSEttingsAdapter::LoadSettingsL(): begin" );
    CRepository* cRepository = CRepository::NewLC( KCRUidRichCallSettings );
    
    TInt err( KErrNone );
    err = cRepository->Get( KRCSEClir, iEntry.iClir );
    if ( KErrNone == err )
        {
        err = cRepository->Get( KRCSEAnonymousCallBlockRule,
            iEntry.iACBRule );
        }
    if ( KErrNone == err )
        {
        err = cRepository->Get( KRCSEPreferredTelephony, iEntry.iPrefTel );
        }
    if ( KErrNone == err )
        {
        err = cRepository->Get( KRCSEDoNotDisturb, iEntry.iDnd );
        }
    if ( KErrNone == err )
        {
        err = cRepository->Get( KRCSPSCallWaiting, iEntry.iPSCallWaiting );
        }
    if ( KErrNone == err )
        {
        err = ResolvePreferredServiceNameL( *cRepository );
        }
    if ( KErrNone == err )
        {
        iEntry.iAreSettingsLoaded = ETrue;
        }
    CleanupStack::PopAndDestroy( cRepository );
    _DBG_FILE( "CNSmlDmGeneralVoIPSEttingsAdapter::LoadSettingsL(): end" );
    return err;
    }

// ---------------------------------------------------------------------------
// CNSmlDmGeneralVoIPSettingsAdapter::DesToInt
// Converts a 8 bit descriptor to int.
// ---------------------------------------------------------------------------
//
TInt CNSmlDmGeneralVoIPSettingsAdapter::DesToInt( 
    const TDesC8& aDes )
    {
    TLex8 lex( aDes );
    TInt value( 0 );
    lex.Val( value );
    return value;
    }

// ---------------------------------------------------------------------------
// CNSmlDmGeneralVoIPSettingsAdapter::RemoveLastURISeg
// returns parent uri, i.e. removes last uri segment
// ---------------------------------------------------------------------------
//
TPtrC8 CNSmlDmGeneralVoIPSettingsAdapter::RemoveLastURISeg( 
    const TDesC8& aURI )
    {
    TInt counter( 0 );
    
    // Check that aURI exists
    if ( 0 < aURI.Length() )
        {
        for ( counter = aURI.Length() - 1; counter >= 0; counter-- )
            {
            if ( KNSMLDMURISEPARATORDEF == aURI[ counter ] )
                {
                break;
                }
            }
        }
    return aURI.Left( counter );
    }

// ---------------------------------------------------------------------------
// CNSmlDmGeneralVoIPSettingsAdapter::NumOfURISegs
// For getting the number of uri segs
// ---------------------------------------------------------------------------
//
TInt CNSmlDmGeneralVoIPSettingsAdapter::NumOfURISegs( 
    const TDesC8& aURI )
    {
    TInt numOfURISegs( 1 );
    for ( TInt counter = 0; counter < aURI.Length(); counter++ )
        {
        if ( KNSMLDMURISEPARATORDEF == aURI[ counter ] )
            {
            numOfURISegs++;
            }
        }
    return numOfURISegs;
    }

// ---------------------------------------------------------------------------
// CNSmlDmGeneralVoIPSettingsAdapter::LastURISeg
// Returns only the last uri segment
// ---------------------------------------------------------------------------
//
TPtrC8 CNSmlDmGeneralVoIPSettingsAdapter::LastURISeg( 
    const TDesC8& aURI ) const
    {
    _DBG_FILE( "CNSmlDmGeneralVoIPSettingsAdapter::LastURISeg() : begin" );
    TInt counter( 0 );
    
    // Check that aURI exists
    if ( 0 < aURI.Length() )
        {
        for ( counter = aURI.Length() - 1; counter >= 0; counter-- )
            {
            if ( KNSMLDMURISEPARATORDEF == aURI[ counter ] )
                {
                break;
                }
            }
        }
    _DBG_FILE( "CNSmlDmGeneralVoIPSettingsAdapter::LastURISeg() : end" );
    if ( 0 == counter  )
        {
        return aURI;
        }
    else
        {
        return aURI.Mid( counter + 1 );
        }
    }
    
// ---------------------------------------------------------------------------
// CNSmlDmGeneralVoIPSettingsAdapter::RemoveDotSlash
// return uri without dot and slash in start
// ---------------------------------------------------------------------------
//
TPtrC8 CNSmlDmGeneralVoIPSettingsAdapter::RemoveDotSlash( 
    const TDesC8& aURI )
    {
    if ( 0 == aURI.Find( KNSmlDMGenVoIPUriDotSlash ) )
        {
        return aURI.Right( 
            aURI.Length() - KNSmlDMGenVoIPUriDotSlash().Length() );
        }
    else
        {
        return aURI;
        }
    }

// ---------------------------------------------------------------------------
// CNSmlDmGeneralVoIPSettingsAdapter::DDFVersionL
// From base class CNSmlDmAdapter.
// Inserts DDF version of the adapter to aDDFVersion
// ---------------------------------------------------------------------------
//
void CNSmlDmGeneralVoIPSettingsAdapter::DDFVersionL(
    CBufBase& aDDFVersion )
    {
    _DBG_FILE( "CNSmlDmGeneralVoIPSettingsAdapter::DDFVersionL( TDes& aDDFVersion): begin" );
    aDDFVersion.InsertL( 0, KNSmlDMGenVoIPDDFVersion );
    _DBG_FILE( "CNSmlDmGeneralVoIPSettingsAdapter::DDFVersionL( TDes& aDDFVersion): end" );
    }

// ---------------------------------------------------------------------------
// CNSmlDmGeneralVoIPSettingsAdapter::DDFStructureL
// From base class CNSmlDmAdapter.
// Builds the DDF structure of adapter
// ---------------------------------------------------------------------------
//
void CNSmlDmGeneralVoIPSettingsAdapter::DDFStructureL( MSmlDmDDFObject& aDDF )
    {
    _DBG_FILE("CNSmlDmGeneralVoIPSettingsAdapter::DDFStructureL(): begin");

    TSmlDmAccessTypes accessTypesGet;
    accessTypesGet.SetGet();

    TSmlDmAccessTypes accessTypesGetAdd;
    accessTypesGetAdd.SetGet();
    accessTypesGetAdd.SetAdd();

    TSmlDmAccessTypes accessTypesGetReplaceAdd;
    accessTypesGetReplaceAdd.SetGet();
    accessTypesGetReplaceAdd.SetReplace();
    accessTypesGetReplaceAdd.SetAdd();

    TSmlDmAccessTypes accessTypesAll;
    accessTypesAll.SetGet();
    accessTypesAll.SetDelete();
    accessTypesAll.SetAdd();
    accessTypesAll.SetReplace();

    // GenVoIP
    MSmlDmDDFObject& genVoIP = aDDF.AddChildObjectL( KNSmlDMGenVoIPNodeName );
    genVoIP.SetAccessTypesL( accessTypesGet );
    genVoIP.SetOccurenceL( MSmlDmDDFObject::EOne );
    genVoIP.SetScopeL( MSmlDmDDFObject::EPermanent );
    genVoIP.SetDFFormatL( MSmlDmDDFObject::ENode );
    genVoIP.AddDFTypeMimeTypeL( KNSmlDMGenVoIPTextPlain );
    genVoIP.SetDescriptionL( KNSmlDMGenVoIPNodeNameExp );

    // Clir
    MSmlDmDDFObject& clir = genVoIP.AddChildObjectL( KNSmlDMGenVoIPClir );
    clir.SetAccessTypesL( accessTypesGetReplaceAdd );
    clir.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    clir.SetScopeL( MSmlDmDDFObject::EDynamic );
    clir.SetDFFormatL( MSmlDmDDFObject::EInt );
    clir.AddDFTypeMimeTypeL( KNSmlDMGenVoIPTextPlain );
    clir.SetDescriptionL( KNSmlDMGenVoIPClirExp );

    // AnonCallBlockRule
    MSmlDmDDFObject& acbRule = genVoIP.AddChildObjectL( 
        KNSmlDMGenVoIPAnonCallBlockRule );
    acbRule.SetAccessTypesL( accessTypesGetReplaceAdd );
    acbRule.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    acbRule.SetScopeL( MSmlDmDDFObject::EDynamic );
    acbRule.SetDFFormatL( MSmlDmDDFObject::EInt );
    acbRule.AddDFTypeMimeTypeL( KNSmlDMGenVoIPTextPlain );
    acbRule.SetDescriptionL( KNSmlDMGenVoIPAnonCallBlockRuleExp );

    // PreferredTelephony
    MSmlDmDDFObject& prefTel = genVoIP.AddChildObjectL( 
        KNSmlDMGenVoIPPreferredTelephony );
    prefTel.SetAccessTypesL( accessTypesGetReplaceAdd );
    prefTel.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    prefTel.SetScopeL( MSmlDmDDFObject::EDynamic );
    prefTel.SetDFFormatL( MSmlDmDDFObject::EInt );
    prefTel.AddDFTypeMimeTypeL( KNSmlDMGenVoIPTextPlain );
    prefTel.SetDescriptionL( KNSmlDMGenVoIPPreferredTelephonyExp );

    // DoNotDisturb
    MSmlDmDDFObject& dnd = genVoIP.AddChildObjectL( 
        KNSmlDMGenVoIPDoNotDisturb );
    dnd.SetAccessTypesL( accessTypesGetReplaceAdd );
    dnd.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    dnd.SetScopeL( MSmlDmDDFObject::EDynamic );
    dnd.SetDFFormatL( MSmlDmDDFObject::EInt );
    dnd.AddDFTypeMimeTypeL( KNSmlDMGenVoIPTextPlain );
    dnd.SetDescriptionL( KNSmlDMGenVoIPDoNotDisturbExp );

    // PSCallWaiting
    MSmlDmDDFObject& psCallWaiting = genVoIP.AddChildObjectL( 
        KNSmlDMGenVoIPPSCallWaiting );
    psCallWaiting.SetAccessTypesL( accessTypesGetReplaceAdd );
    psCallWaiting.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    psCallWaiting.SetScopeL( MSmlDmDDFObject::EDynamic );
    psCallWaiting.SetDFFormatL( MSmlDmDDFObject::EInt );
    psCallWaiting.AddDFTypeMimeTypeL( KNSmlDMGenVoIPTextPlain );
    psCallWaiting.SetDescriptionL( KNSmlDMGenVoIPPSCallWaitingExp );

    // ProfileName
    MSmlDmDDFObject& voipProfileId = genVoIP.AddChildObjectL(
        KNSmlDMGenVoIPProfileId );
    voipProfileId.SetAccessTypesL( accessTypesGetReplaceAdd );
    voipProfileId.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    voipProfileId.SetScopeL( MSmlDmDDFObject::EDynamic );
    voipProfileId.SetDFFormatL( MSmlDmDDFObject::EChr );
    voipProfileId.AddDFTypeMimeTypeL( KNSmlDMGenVoIPTextPlain );
    voipProfileId.SetDescriptionL( KNSmlDMGenVoIPProfileIdExp );

    _DBG_FILE("CNSmlDmGeneralVoIPSettingsAdapter::DDFStructureL(): end");
    }

// ---------------------------------------------------------------------------
// CNSmlDmGeneralVoIPSettingsAdapter::UpdateLeafObjectL
// From base class CNSmlDmAdapter.
// Updates value of a leaf object
// ---------------------------------------------------------------------------
//
void CNSmlDmGeneralVoIPSettingsAdapter::UpdateLeafObjectL( 
    const TDesC8& aURI, 
    const TDesC8& /*aLUID*/, 
    const TDesC8& aObject, 
    const TDesC8& /*aType*/, 
    const TInt aStatusRef )
    {
    _DBG_FILE( "CNSmlDmGeneralVoIPSettingsAdapter::UpdateLeafObjectL(): begin" );

    CSmlDmAdapter::TError status = EError;

    TPtrC8 lastURISeg = LastURISeg( aURI );
    TPtrC8 uriTmp = RemoveLastURISeg( aURI );
    TPtrC8 secondLastURISeg = LastURISeg( uriTmp );

    // Make sure iEntry holds values.
    if ( !( iEntry.iAreSettingsLoaded ) )
        {
        TInt err = LoadSettingsL();
        if ( KErrNone != err )
            {
            status = CSmlDmAdapter::EError;
            iDmCallback->SetStatusL( aStatusRef, status );
            return;
            }
        }

    if( 0 == secondLastURISeg.Compare( KNSmlDMGenVoIPNodeName() )
        && KTwoSegs == NumOfURISegs( aURI ) )
        {
        status = CSmlDmAdapter::EOk;
        iEntry.iStatusRef = aStatusRef;

        // Clir
        if ( KNSmlDMGenVoIPClir() == lastURISeg )
            {
            iEntry.iClir = DesToInt( aObject );
            }
        // AnonCallBlockRule
        else if ( KNSmlDMGenVoIPAnonCallBlockRule() == lastURISeg )
            {
            iEntry.iACBRule = DesToInt( aObject );
            }
        // PreferredTelephony
        else if ( KNSmlDMGenVoIPPreferredTelephony() == lastURISeg )
            {
            iEntry.iPrefTel = DesToInt( aObject );
            }
        // DoNotDisturb
        else if ( KNSmlDMGenVoIPDoNotDisturb() == lastURISeg )
            {
            iEntry.iDnd = DesToInt( aObject );
            }
        // PSCallWaiting
        else if ( KNSmlDMGenVoIPPSCallWaiting() == lastURISeg )
            {
            iEntry.iPSCallWaiting = DesToInt( aObject );
            }
        else if ( KNSmlDMGenVoIPProfileId() == lastURISeg )
            {
            iEntry.iVoipProfileId = aObject;
            }
        else
            {
            status = CSmlDmAdapter::ENotFound;
            }
        }
    else
        {
        status = CSmlDmAdapter::ENotFound;
        }

    iDmCallback->SetStatusL( aStatusRef, status ); 

    _DBG_FILE("CNSmlDmGeneralVoIPSettingsAdapter::UpdateLeafObjectL(): end");
    }

// ---------------------------------------------------------------------------
// CNSmlDmGeneralVoIPSettingsAdapter::UpdateLeafObjectL
// From base class CNSmlDmAdapter.
// Not supported (see smldmadapter.h).
// ---------------------------------------------------------------------------
//
void CNSmlDmGeneralVoIPSettingsAdapter::UpdateLeafObjectL( 
    const TDesC8& /*aURI*/, 
    const TDesC8& /*aLUID*/, 
    RWriteStream*& /*aStream*/, 
    const TDesC8& /*aType*/, 
    const TInt aStatusRef )
    {
    _DBG_FILE( "CNSmlDmGeneralVoIPSettingsAdapter::UpdateLeafObjectL(): stream: begin" );
    iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    _DBG_FILE( "CNSmlDmGeneralVoIPSettingsAdapter::UpdateLeafObjectL(): stream: end" );
    }

// ---------------------------------------------------------------------------
// CNSmlDmGeneralVoIPSettingsAdapter::DeleteObjectL
// From base class CNSmlDmAdapter.
// Not supported.
// ---------------------------------------------------------------------------
//
void CNSmlDmGeneralVoIPSettingsAdapter::DeleteObjectL( 
    const TDesC8& /*aURI*/, 
    const TDesC8& /*aLUID*/, 
    const TInt aStatusRef )
    {
    _DBG_FILE( "CNSmlDmGeneralVoIPSettingsAdapter::DeleteObjectL(): begin" );
    iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    _DBG_FILE( "CNSmlDmGeneralVoIPSettingsAdapter::DeleteObjectL(): end" );
    }

// ---------------------------------------------------------------------------
// CNSmlDmGeneralVoIPSettingsAdapter::FetchLeafObjectL
// From base class CNSmlDmAdapter.
// Fetches the values of leaf objects.
// ---------------------------------------------------------------------------
//
void CNSmlDmGeneralVoIPSettingsAdapter::FetchLeafObjectL( 
    const TDesC8& aURI, 
    const TDesC8& /*aLUID*/, 
    const TDesC8& aType, 
    const TInt aResultsRef, 
    const TInt aStatusRef )
    {
    _DBG_FILE( "CNSmlDmGeneralVoIPSettingsAdapter::FetchLeafObjectL(): begin" );

    CBufBase *result = CBufFlat::NewL( KDefaultResultSize );
    CleanupStack::PushL( result );
    CSmlDmAdapter::TError status = FetchObjectL( aURI, *result );

    if ( CSmlDmAdapter::EOk == status )
        {
        iDmCallback->SetResultsL( aResultsRef, *result, aType );
        }
    iDmCallback->SetStatusL( aStatusRef, status );

    CleanupStack::PopAndDestroy( result ); 
    _DBG_FILE( "CNSmlDmGeneralVoIPSettingsAdapter::FetchLeafObjectL(): end" );
    }

// ---------------------------------------------------------------------------
// CNSmlDmGeneralVoIPSettingsAdapter::FetchLeafObjectSizeL
// From base class CNSmlDmAdapter.
// Fetches the size of a leaf object.
// ---------------------------------------------------------------------------
//
void CNSmlDmGeneralVoIPSettingsAdapter::FetchLeafObjectSizeL( 
    const TDesC8& aURI, 
    const TDesC8& /*aLUID*/, 
    const TDesC8& aType, 
    const TInt aResultsRef, 
    const TInt aStatusRef )
    {
    _DBG_FILE( "CNSmlDmGeneralVoIPSettingsAdapter::FetchLeafObjectSizeL(): begin" );
    CBufBase *result = CBufFlat::NewL( 1 );
    CleanupStack::PushL( result );
    CSmlDmAdapter::TError status = FetchObjectL( aURI, *result );
    
    TInt objSizeInBytes = result->Size();
    TBuf8<16> stringObjSizeInBytes;
    stringObjSizeInBytes.Num( objSizeInBytes );
    result->Reset();
    result->InsertL( 0, stringObjSizeInBytes );

    iDmCallback->SetStatusL( aStatusRef, status );
    iDmCallback->SetResultsL( aResultsRef, *result, aType );
    CleanupStack::PopAndDestroy( result );
    _DBG_FILE("CNSmlDmGeneralVoIPSettingsAdapter::FetchLeafObjectSizeL(): end" );
    }

// ---------------------------------------------------------------------------
// CNSmlDmGeneralVoIPSettingsAdapter::ChildURIListL
// From base class CNSmlDmAdapter.
// Asks for the list of child objects of the node.
// ---------------------------------------------------------------------------
//
void CNSmlDmGeneralVoIPSettingsAdapter::ChildURIListL( 
    const TDesC8& aURI,
    const TDesC8& /*aLUID*/, 
    const CArrayFix<TSmlDmMappingInfo>& /*aPreviousURISegmentList*/, 
    const TInt aResultsRef, 
    const TInt aStatusRef ) 
    {
    _DBG_FILE( "CNSmlDmGeneralVoIPSettingsAdapter::ChildURIListL(): start" );
    CSmlDmAdapter::TError status = CSmlDmAdapter::EError;

    CBufBase *currentURISegmentList = CBufFlat::NewL( KDefaultResultSize );
    CleanupStack::PushL( currentURISegmentList );
    TBuf8<KNSmlGenVoIPMaxUriLength> mappingInfo( KNullDesC8 );
    TBuf8<KNSmlGenVoIPMaxUriLength> uri = aURI;
    TBuf8<KSmlMaxURISegLen> segmentName;

    if ( KNSmlDMGenVoIPNodeName() == LastURISeg( aURI ) )
        {
        // Clir
        segmentName.Copy( KNSmlDMGenVoIPClir );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMGenVoIPSeparator );
        // AnonCallBlockRule
        segmentName.Copy( KNSmlDMGenVoIPAnonCallBlockRule );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMGenVoIPSeparator );
        // PreferredTelephony
        segmentName.Copy( KNSmlDMGenVoIPPreferredTelephony );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMGenVoIPSeparator );
        // DoNotDisturb
        segmentName.Copy( KNSmlDMGenVoIPDoNotDisturb );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMGenVoIPSeparator );
        // PSCallWaiting
        segmentName.Copy( KNSmlDMGenVoIPPSCallWaiting );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMGenVoIPSeparator );
        // ProfileName
        segmentName.Copy( KNSmlDMGenVoIPProfileId );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMGenVoIPSeparator );
        status = CSmlDmAdapter::EOk;
        }

    iDmCallback->SetStatusL( aStatusRef, status );
    iDmCallback->SetResultsL( aResultsRef, 
        *currentURISegmentList, KNullDesC8 );
    CleanupStack::PopAndDestroy( currentURISegmentList );

    _DBG_FILE( "CNSmlDmGeneralVoIPSettingsAdapter::ChildURIListL(): end" );
    }
    
// ---------------------------------------------------------------------------
// CNSmlDmGeneralVoIPSettingsAdapter::AddNodeObjectL
// From base class CNSmlDmAdapter.
// Not supported since there can only be one general VoIP settings node.
// ---------------------------------------------------------------------------
//
void CNSmlDmGeneralVoIPSettingsAdapter::AddNodeObjectL( 
    const TDesC8& /*aURI*/, 
    const TDesC8& /*aParentLUID*/, 
    const TInt aStatusRef )
    {
    _DBG_FILE( "CNSmlDmGeneralVoIPSettingsAdapter::AddNodeObjectL(): begin" );
    iDmCallback->SetStatusL( aStatusRef, EOk );
    _DBG_FILE( "CNSmlDmGeneralVoIPSettingsAdapter::AddNodeObjectL(): end" );
    }
    
// ---------------------------------------------------------------------------
// CNSmlDmGeneralVoIPSettingsAdapter::ExecuteCommandL
// From base class CNSmlDmAdapter.
// Not supported (see smldmadapter.h).
// ---------------------------------------------------------------------------
//
void CNSmlDmGeneralVoIPSettingsAdapter::ExecuteCommandL( 
    const TDesC8& /*aURI*/, 
    const TDesC8& /*aLUID*/, 
    const TDesC8& /*aArgument*/, 
    const TDesC8& /*aType*/, 
    const TInt aStatusRef )
    {
    _DBG_FILE( "CNSmlDmGeneralVoIPSettingsAdapter::ExecuteCommandL(): begin" );
    iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    _DBG_FILE( "CNSmlDmGeneralVoIPSettingsAdapter::ExecuteCommandL(): end" );
    }

// ---------------------------------------------------------------------------
// CNSmlDmGeneralVoIPSettingsAdapter::ExecuteCommandL
// From base class CNSmlDmAdapter.
// Not supported (see smldmadapter.h).
// ---------------------------------------------------------------------------
//
void CNSmlDmGeneralVoIPSettingsAdapter::ExecuteCommandL( 
    const TDesC8& /*aURI*/, 
    const TDesC8& /*aParentLUID*/, 
    RWriteStream*& /*aStream*/, 
    const TDesC8& /*aType*/, 
    const TInt aStatusRef )
    {
    _DBG_FILE( "CNSmlDmGeneralVoIPSettingsAdapter::ExecuteCommandL(): stream: begin" );
    iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    _DBG_FILE( "CNSmlDmGeneralVoIPSettingsAdapter::ExecuteCommandL(): stream: end" );
    }

// ---------------------------------------------------------------------------
// CNSmlDmGeneralVoIPSettingsAdapter::CopyCommandL
// From base class CNSmlDmAdapter.
// Not supported (see smldmadapter.h).
// ---------------------------------------------------------------------------
//
void CNSmlDmGeneralVoIPSettingsAdapter::CopyCommandL( 
    const TDesC8& /*aTargetURI*/, 
    const TDesC8& /*aTargetLUID*/, 
    const TDesC8& /*aSourceURI*/, 
    const TDesC8& /*aSourceLUID*/, 
    const TDesC8& /*aType*/, 
    TInt aStatusRef )
    {
    _DBG_FILE( "CNSmlDmGeneralVoIPSettingsAdapter::CopyCommandL(): begin" );
    iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    _DBG_FILE( "CNSmlDmGeneralVoIPSettingsAdapter::CopyCommandL(): end" );
    }

// ---------------------------------------------------------------------------
// CNSmlDmGeneralVoIPSettingsAdapter::StartAtomicL
// From base class CNSmlDmAdapter.
// Not supported (see smldmadapter.h).
// ---------------------------------------------------------------------------
//
void CNSmlDmGeneralVoIPSettingsAdapter::StartAtomicL()
    {
    _DBG_FILE( "CNSmlDmGeneralVoIPSettingsAdapter::StartAtomicL(): begin" );
    _DBG_FILE( "CNSmlDmGeneralVoIPSettingsAdapter::StartAtomicL(): end" );
    }
    
// ---------------------------------------------------------------------------
// CNSmlDmGeneralVoIPSettingsAdapter::CommitAtomicL
// From base class CNSmlDmAdapter.
// Not supported (see smldmadapter.h).
// ---------------------------------------------------------------------------
//
void CNSmlDmGeneralVoIPSettingsAdapter::CommitAtomicL()
    {
    _DBG_FILE( "CNSmlDmGeneralVoIPSettingsAdapter::CommitAtomicL(): begin" );
    _DBG_FILE( "CNSmlDmGeneralVoIPSettingsAdapter::CommitAtomicL(): end" );
    }

// ---------------------------------------------------------------------------
// CNSmlDmGeneralVoIPSettingsAdapter::RollbackAtomicL
// From base class CNSmlDmAdapter.
// Not supported (see smldmadapter.h).
// ---------------------------------------------------------------------------
//
void CNSmlDmGeneralVoIPSettingsAdapter::RollbackAtomicL()
    {
    _DBG_FILE( "CNSmlDmGeneralVoIPSettingsAdapter::RollbackAtomicL(): begin" );
    _DBG_FILE( "CNSmlDmGeneralVoIPSettingsAdapter::RollbackAtomicL(): end" );
    }

// ---------------------------------------------------------------------------
// CNSmlDmGeneralVoIPSettingsAdapter::StreamingSupport
// From base class CNSmlDmAdapter.
// Not supported (see smldmadapter.h).
// ---------------------------------------------------------------------------
//
TBool CNSmlDmGeneralVoIPSettingsAdapter::StreamingSupport( TInt& /*aItemSize*/ )
    {
    _DBG_FILE( "CNSmlDmGeneralVoIPSettingsAdapter::StreamingSupport(): begin" );
    _DBG_FILE( "CNSmlDmGeneralVoIPSettingsAdapter::StreamingSupport(): end" );
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CNSmlDmGeneralVoIPSettingsAdapter::StreamCommittedL
// From base class CNSmlDmAdapter.
// Not supported (see smldmadapter.h).
// ---------------------------------------------------------------------------
//
void CNSmlDmGeneralVoIPSettingsAdapter::StreamCommittedL()
    {
    _DBG_FILE( "CNSmlDmGeneralVoIPSettingsAdapter::StreamCommittedL(): begin" );
    _DBG_FILE( "CNSmlDmGeneralVoIPSettingsAdapter::StreamCommittedL(): end" );
    }

// ---------------------------------------------------------------------------
// CNSmlDmGeneralVoIPSettingsAdapter::CompleteOutstandingCmdsL
// From base class CNSmlDmAdapter.
// Saves modified profiles back to permanent store.
// ---------------------------------------------------------------------------
//
void CNSmlDmGeneralVoIPSettingsAdapter::CompleteOutstandingCmdsL()
    {
    _DBG_FILE( "CNSmlDmGeneralVoIPSettingsAdapter::CompleteOutstandingCmdsL(): begin" );
    // All commands received, save changes.
    if ( iEntry.iAreSettingsLoaded )
        {
        CRepository* cRepository = CRepository::NewLC( KCRUidRichCallSettings );

        User::LeaveIfError( cRepository->Set( KRCSEClir, iEntry.iClir ) );
        User::LeaveIfError( cRepository->Set( KRCSEAnonymousCallBlockRule,
            iEntry.iACBRule ) );
        User::LeaveIfError( cRepository->Set( KRCSEPreferredTelephony,
            iEntry.iPrefTel ) );
        User::LeaveIfError( cRepository->Set( KRCSEDoNotDisturb,
            iEntry.iDnd ) );
        User::LeaveIfError( cRepository->Set( KRCSPSCallWaiting, 
            iEntry.iPSCallWaiting ) );

        // If preferred telephony mode has been set to VoIP and general VoIP
        // settings "links" to a VoIP profile, set the service id stored in 
        // the profile as the preferred service id.
        if ( 1 == iEntry.iPrefTel && iEntry.iVoipProfileId.Length() )
            {
            // Get VoIP profile id.
            TInt voipId( VoIPProfileIdL( iEntry.iVoipProfileId ) );
            
            // Get service ID from RCSE.
            CRCSEProfileRegistry* registry = CRCSEProfileRegistry::NewLC();
            CRCSEProfileEntry* entry = CRCSEProfileEntry::NewLC();        

            registry->FindL( voipId, *entry );        
            TUint serviceId = entry->iServiceProviderId;

            CleanupStack::PopAndDestroy( 2, registry ); // entry, registry

            cRepository->Set( KRCSPSPreferredService, (TInt)serviceId );
            }
        
        CleanupStack::PopAndDestroy( cRepository );
        }
    else
        {
        User::Leave( KErrGeneral );
        }
    
    _DBG_FILE( "CNSmlDmGeneralVoIPSettingsAdapter::CompleteOutstandingCmdsL(): end" );
    }

// ---------------------------------------------------------------------------
// CNSmlDmGeneralVoIPSettingsAdapter::VoIPProfileIdL
// Get VoIP profile ID.
// ---------------------------------------------------------------------------
//
TUint32 CNSmlDmGeneralVoIPSettingsAdapter::VoIPProfileIdL(
    const TDesC8& aObject ) const
    {
    _DBG_FILE("CNSmlDmGeneralVoIPSettingsAdapter::VoIPProfileIdL : begin");

    __ASSERT_ALWAYS( iDmCallback != NULL, User::Leave( KErrArgument ) );

    HBufC8* luid = iDmCallback->GetLuidAllocL( aObject );
    CleanupStack::PushL( luid ); // CS:1
    if ( luid->Des().Length() )
        {
        TBuf8<KMaxUriLength> tmp;
        tmp.Copy( luid->Des().Mid( KVoipId().Length() ) );
        TLex8 lex( tmp );
        TUint profileId( 0 );
        User::LeaveIfError( lex.Val( profileId ) );

        CleanupStack::PopAndDestroy( luid ); // CS:0
        _DBG_FILE("CNSmlDmGeneralVoIPSettingsAdapter::VoIPProfileIdL : end");
        return profileId;
        }
    CleanupStack::PopAndDestroy( luid ); // CS:0
    
    _DBG_FILE("CNSmlDmGeneralVoIPSettingsAdapter::VoIPProfileIdL : end");
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CNSmlDmGeneralVoIPSettingsAdapter::ResolvePreferredServiceNameL
// Get name of the preferred VoIP service.
// ---------------------------------------------------------------------------
//
TInt CNSmlDmGeneralVoIPSettingsAdapter::ResolvePreferredServiceNameL(
    CRepository& aRepository )
    {
    _DBG_FILE("CNSmlDmGeneralVoIPSettingsAdapter::ResolvePreferredServiceNameL : begin");

    // Reset VoIP profile id.
    iEntry.iVoipProfileId.Zero();
    
    // No need to resolve if preferred telephony isn't set to VoIP.
    if ( 1 != iEntry.iPrefTel )
        {
        return KErrNone;
        }
    
    // Get preferred service id from central repository.
    TInt serviceId( KErrNotFound );
    
    User::LeaveIfError( aRepository.Get(
        KRCSPSPreferredService, serviceId ) );
    
    // Find the VoIP profile with the given service id.
    CRCSEProfileRegistry* registry = CRCSEProfileRegistry::NewLC();
    RPointerArray<CRCSEProfileEntry> entries;
    CleanupResetAndDestroyL( entries );

    registry->FindByServiceIdL( serviceId, entries );

    if ( !entries.Count() )
        {
        User::Leave( KErrNotFound );
        }

    TInt voipProfileId = entries[0]->iId;
    
    CleanupStack::PopAndDestroy( 2, registry ); // entries, registry

    // Get URIs to all VoIP settings.
    CBufBase* result = CBufFlat::NewL( 1 );
    CleanupStack::PushL( result ); // CS:1
    CSmlDmAdapter::TError status;

    iDmCallback->FetchLinkL( KNSmlDMVoIPNodeName(), *result, status );
    
    result->Compress();
    
    HBufC8* childList = HBufC8::NewLC( result->Size() ); // CS:2
    TInt uriSegCount( 0 );

    if ( result->Size() )
        {
        childList->Des().Copy( result->Ptr( 0 ) );
        uriSegCount = NumOfURISegs( childList->Des() );
        }

    // Get LUIDs for URIs and save the correct profile name.
    for ( TInt index = 0; index < uriSegCount; index++ )
        {
        HBufC8* uri = HBufC8::NewLC( KMaxUriLength ); // CS:3
      
        uri->Des().Copy( KNSmlDMVoIPNodeName() );
        uri->Des().Append( KNSmlDMGenVoIPSeparator() );
        uri->Des().Append( LastURISeg( childList->Des() ) );        

        HBufC8* luid = iDmCallback->GetLuidAllocL( uri->Des() );
        CleanupStack::PushL( luid ); // CS:4

        if ( luid->Des().Length() && 
            voipProfileId == DesToInt(
                luid->Des().Mid( KVoipId().Length() ) ) )
            {
            iEntry.iVoipProfileId.Copy( uri->Des() );
            CleanupStack::PopAndDestroy( luid ); // CS:3
            CleanupStack::PopAndDestroy( uri );  // CS:2
            break;
            }
        childList->Des().Copy( RemoveLastURISeg( childList->Des() ) );
        CleanupStack::PopAndDestroy( luid ); // CS:3
        CleanupStack::PopAndDestroy( uri );  // CS:2
        }

    CleanupStack::PopAndDestroy( childList );  // CS:1
    CleanupStack::PopAndDestroy( result );     // CS:0

    _DBG_FILE("CNSmlDmGeneralVoIPSettingsAdapter::ResolvePreferredServiceNameL : end");
    return status;
    }

// ---------------------------------------------------------------------------
// TImplementationProxy ImplementationTable[]
// Needed because of ECOM architecture
// ---------------------------------------------------------------------------
//
#ifndef IMPLEMENTATION_PROXY_ENTRY
#define IMPLEMENTATION_PROXY_ENTRY( aUid, aFuncPtr ) {{ aUid }, ( aFuncPtr )}
#endif

const TImplementationProxy ImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY( KNSmlDMGenVoIPAdapterImplUid, 
        CNSmlDmGeneralVoIPSettingsAdapter::NewL )
    };

// ---------------------------------------------------------------------------
// ImplementationGroupProxy()
// Needed because of ECOM architecture
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( 
    TInt& aTableCount )
    {
    _DBG_FILE( "ImplementationGroupProxy() for CNSmlDmGeneralVoIPSettingsAdapter: begin" );
    aTableCount = sizeof( ImplementationTable ) / 
        sizeof( TImplementationProxy );
    _DBG_FILE( "ImplementationGroupProxy() for CNSmlDmGeneralVoIPSettingsAdapter: end" );
    return ImplementationTable;
    }
  
