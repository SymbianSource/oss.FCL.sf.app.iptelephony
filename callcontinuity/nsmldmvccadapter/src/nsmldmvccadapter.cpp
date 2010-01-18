/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation file for nsmldmvccadapter
*
*/


#include <e32std.h>
#include <escapeutils.h>
#include <spentry.h>
#include <spproperty.h>
#include <nsmldmuri.h>
#include <crcseprofileentry.h>
#include <crcseprofileregistry.h>

#include "nsmldmvccadapter.h"
#include "vccspsettings.h"
#include "vccmiscutils.h"
#include "rubydebug.h"


const TInt KNSmlDMVCC_DefaultResultSize = 255;

/** VCC DDF keywords */
_LIT8( KNSmlDMVCC_DDFVersion,               "1.0" );
_LIT8( KNSmlDMVCC_Node,                     "VCC001" );
_LIT( KNSmlDMVCC_ServiceName,               "VCC" );

/** VCC */ 
_LIT8( KNSmlDMVCC_Name,                     "Name" );
_LIT8( KNSmlDMVCC_VDI,                      "VDI" );
_LIT8( KNSmlDMVCC_VDN,                      "VDN" );
_LIT8( KNSmlDMVCC_PreferredDomain,          "Preferred Domain" );
_LIT8( KNSmlDMVCC_ImmediateDT,              "Immediate DT" );
_LIT8( KNSmlDMVCC_DTCsToIM,                 "DT CS-to-IM CN direction" );
_LIT8( KNSmlDMVCC_DTIMToCs,                 "DT IM CN-to-CS direction" );
_LIT8( KNSmlDMVCC_DTHeldWaitingCalls,       "DT in held_waiting calls" );

_LIT8( KNSmlDMVCC_WlanHOTreshold, 			"WLAN HO Treshold" );
_LIT8( KNSmlDMVCC_WlanHOHysteresis,			"WLAN HO Hysteresis" );
_LIT8( KNSmlDMVCC_WlanHysteresisTimerLow, 	"WLAN Hysteresis Timer Low" );
_LIT8( KNSmlDMVCC_WlanHysteresisTimerHigh, 	"WLAN Hysteresis Timer High" );
_LIT8( KNSmlDMVCC_CSHOTreshold, 			"CS HO Treshold" );
_LIT8( KNSmlDMVCC_CSHOHysteresis, 			"CS HO Hysteresis" );
_LIT8( KNSmlDMVCC_CSHysteresisTimerLow,		"CS Hysteresis Timer Low" );
_LIT8( KNSmlDMVCC_CSHysteresisTimerHigh, 	"CS Hysteresis Timer High" );
_LIT8( KNSmlDMVCC_VoipServiceId, 			"VoIP" );
_LIT8( KNSmlDMVCC_DtAllowedWhenCsOriginated,"DT Allowed When CS Originated" );

_LIT8( KNSmlDMVCC_NodeExp,                  "Main node for VCC settings" );
_LIT8( KNSmlDMVCC_NameExp,                  "Name for the Communication Continuity settings" );
_LIT8( KNSmlDMVCC_VDIExp,                   "VCC Transfer URI " );
_LIT8( KNSmlDMVCC_VDNExp,                   "VCC Transfer Number" );
_LIT8( KNSmlDMVCC_PreferredDomainExp,       "Preferred domain for UE originated calls/sessions" );
_LIT8( KNSmlDMVCC_ImmediateDTExp,           "Initiate a VCC DT immediately when domain available or not" );
_LIT8( KNSmlDMVCC_DTCsToIMExp,              "DT from CS domain to PS is restricted or not" );
_LIT8( KNSmlDMVCC_DTIMToCsExp,              "DT from PS domain to CS is restricted or not" );
_LIT8( KNSmlDMVCC_DTHeldWaitingCallsExp,    "DT restricted during active and a held/waiting call/session" );
_LIT8( KNSmlDMVCC_WlanHOTresholdExp, 		"Handover signal strength treshold value for WLAN" );
_LIT8( KNSmlDMVCC_WlanHOHysteresisExp,		"Handover hysteresis value for WLAN" );
_LIT8( KNSmlDMVCC_WlanHysteresisTimerLowExp,"Handover hysteresis timer value for WLAN used below treshold" );
_LIT8( KNSmlDMVCC_WlanHysteresisTimerHighExp, 	"Handover hysteresis timer value for WLAN used above treshold" );
_LIT8( KNSmlDMVCC_CSHOTresholdExp, 			"Handover signal strength treshold value for CS" );
_LIT8( KNSmlDMVCC_CSHOHysteresisExp, 		"Handover hysteresis value for CS" );
_LIT8( KNSmlDMVCC_CSHysteresisTimerLowExp,	"Handover hysteresis timer value for CS used below treshold" );
_LIT8( KNSmlDMVCC_CSHysteresisTimerHighExp, "Handover hysteresis timer value for CS used above treshold" );
_LIT8( KNSmlDMVCC_VoipServiceIdExp, 		"VoIP Service Id to be associated with VCC" );
_LIT8( KNSmlDMVCC_DmToConRef, 				"ToConRef" );
_LIT8( KNSmlDMVCC_DmConRef, 				"ConRef" );
_LIT8( KNSmlDMVCC_DmToConRefExp, 			"Refers to a collection of connectivity definitions" );
_LIT8( KNSmlDMVCC_DmConRefExp,		 		"Indicates the linkage to connectivity parameters" );

_LIT8( KNSmlDMVCC_DmVoipLinkPrefix, 			"./VoIP");
_LIT8( KNSmlDMVCC_DmVoipId,					"VoIPId");
_LIT8( KNSmlDMVCC_DmVoip,					"VoIP");
_LIT( KVccServiceName,               		"VCC" );


/** Others */
_LIT8( KNSmlDMVCCTextPlain,                 "text/plain" );
_LIT8( KNSmlDMVCCSeparator,                 "/" );
_LIT( KNSmlDMVCCValueNullDesc,              "" );
_LIT( KNSmlDMVCCValueZero,                 "0" );

const TInt KNSmlDMVCCSeparatorChar            = '/';
const TInt KNSmlDMVCCDefaultResultSize        = 64;
const TInt KNSmlDMVCCMaxResultLength          = 256;
const TInt  KNSmlDMVCCMaxUriLength            = 100;


// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ Constructor
// ---------------------------------------------------------------------------
//
CNSmlDmVCCAdapter::CNSmlDmVCCAdapter( TAny* aEcomArguments )
: CSmlDmAdapter( aEcomArguments )
    {
    RUBY_DEBUG_BLOCK( "CNSmlDmVCCAdapter::CNSmlDmVCCAdapter" );
    }


// ---------------------------------------------------------------------------
// Second phase constructor
// ---------------------------------------------------------------------------
//
void CNSmlDmVCCAdapter::ConstructL()
    {
    RUBY_DEBUG_BLOCKL( "CNSmlDmVCCAdapter::ConstructL" );
    iVCCSettings = CVccSPSettings::NewL();
    iVCCSettings->SetServiceNameL( KNSmlDMVCC_ServiceName );
    iVCCSettings->ReadSettingsL( KVccServiceName );
    }


// ---------------------------------------------------------------------------
// Two-phased constructor
// ---------------------------------------------------------------------------
//
CNSmlDmVCCAdapter* CNSmlDmVCCAdapter::NewL( MSmlDmCallback* aDmCallback )
    {
    RUBY_DEBUG_BLOCKL( "CNSmlDmVCCAdapter::NewL" );
    CNSmlDmVCCAdapter* self = CNSmlDmVCCAdapter::NewLC( aDmCallback );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Two-phased constructor
// ---------------------------------------------------------------------------
//
CNSmlDmVCCAdapter* CNSmlDmVCCAdapter::NewLC( MSmlDmCallback* aDmCallback )
    {
    RUBY_DEBUG0( "CNSmlDmVCCAdapter::NewLC --Start" );
    CNSmlDmVCCAdapter* self = new( ELeave ) CNSmlDmVCCAdapter( aDmCallback );
    CleanupStack::PushL( self );
    self->ConstructL();
    RUBY_DEBUG0( "CNSmlDmVCCAdapter::NewLC --End" );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CNSmlDmVCCAdapter::~CNSmlDmVCCAdapter()
    {
    RUBY_DEBUG0( "CNSmlDmVCCAdapter::~CNSmlDmVCCAdapter() - ENTER" );
    delete iVCCSettings;
    delete iTempBuf;
    RUBY_DEBUG0( "CNSmlDmVCCAdapter::~CNSmlDmVCCAdapter() - EXIT" );
    }

// ---------------------------------------------------------------------------
//  
// ---------------------------------------------------------------------------
//
void CNSmlDmVCCAdapter::DDFVersionL( CBufBase& aVersion )
    {
    RUBY_DEBUG_BLOCK( "CNSmlDmVCCAdapter::DDFVersionL" );
    aVersion.InsertL( 0,KNSmlDMVCC_DDFVersion );
    }

// ---------------------------------------------------------------------------
//  
// ---------------------------------------------------------------------------
//
void CNSmlDmVCCAdapter::DDFStructureL( MSmlDmDDFObject& aDDF )
    {
    RUBY_DEBUG_BLOCK( "CNSmlDmVCCAdapter::DDFStructureL" );
  
    TSmlDmAccessTypes accessTypesGet;
    accessTypesGet.SetGet();

    TSmlDmAccessTypes accessTypesGetReplace;
    accessTypesGetReplace.SetGet();
    accessTypesGetReplace.SetReplace();
     
    
    // Main node
    MSmlDmDDFObject& vccNode = aDDF.AddChildObjectL( KNSmlDMVCC_Node );
    vccNode.SetAccessTypesL( accessTypesGet );
    vccNode.SetOccurenceL( MSmlDmDDFObject::EOneOrMore );
    vccNode.SetScopeL( MSmlDmDDFObject::EPermanent );
    vccNode.SetDFFormatL( MSmlDmDDFObject::ENode );
    vccNode.SetDescriptionL( KNSmlDMVCC_NodeExp );
    
    // Name leaf -read only
    MSmlDmDDFObject& vccName = vccNode.AddChildObjectL( KNSmlDMVCC_Name );    
    vccName.SetAccessTypesL( accessTypesGet );
    vccName.SetOccurenceL( MSmlDmDDFObject::EZeroOrMore );
    vccName.SetScopeL( MSmlDmDDFObject::EPermanent );
    vccName.SetDFFormatL( MSmlDmDDFObject::EChr );
    vccName.AddDFTypeMimeTypeL( KNSmlDMVCCTextPlain );
    vccName.SetDescriptionL( KNSmlDMVCC_NameExp );
    
    // VDI leaf
    MSmlDmDDFObject& vccVDI = vccNode.AddChildObjectL( KNSmlDMVCC_VDI );    
    vccVDI.SetAccessTypesL( accessTypesGetReplace );
    vccVDI.SetOccurenceL( MSmlDmDDFObject::EOne );
    vccVDI.SetScopeL( MSmlDmDDFObject::EPermanent );
    vccVDI.SetDFFormatL( MSmlDmDDFObject::EChr );
    vccVDI.SetDescriptionL( KNSmlDMVCC_VDIExp );
    
    // VDN leaf
    MSmlDmDDFObject& vccVDN = vccNode.AddChildObjectL( KNSmlDMVCC_VDN );
    vccVDN.SetAccessTypesL( accessTypesGetReplace );
    vccVDN.SetOccurenceL( MSmlDmDDFObject::EOne );
    vccVDN.SetScopeL( MSmlDmDDFObject::EPermanent );
    vccVDN.SetDFFormatL( MSmlDmDDFObject::EChr );
    vccVDN.SetDescriptionL( KNSmlDMVCC_VDNExp );
    
    // Preferred domain leaf
    MSmlDmDDFObject& vccPD = 
                          vccNode.AddChildObjectL( KNSmlDMVCC_PreferredDomain );
    vccPD.SetAccessTypesL( accessTypesGetReplace );
    vccPD.SetOccurenceL( MSmlDmDDFObject::EOne );
    vccPD.SetScopeL( MSmlDmDDFObject::EPermanent );
    vccPD.SetDFFormatL( MSmlDmDDFObject::EChr );
    vccPD.AddDFTypeMimeTypeL( KNSmlDMVCCTextPlain );
    vccPD.SetDescriptionL( KNSmlDMVCC_PreferredDomainExp );
    
    // Immediate DT leaf
    MSmlDmDDFObject& vccIDT = vccNode.AddChildObjectL( KNSmlDMVCC_ImmediateDT );
    vccIDT.SetAccessTypesL( accessTypesGetReplace );
    vccIDT.SetOccurenceL( MSmlDmDDFObject::EOne );
    vccIDT.SetScopeL( MSmlDmDDFObject::EPermanent );
    vccIDT.SetDFFormatL( MSmlDmDDFObject::EChr );
    vccIDT.AddDFTypeMimeTypeL( KNSmlDMVCCTextPlain );
    vccIDT.SetDescriptionL( KNSmlDMVCC_ImmediateDTExp );
    
    // CS to PS leaf
    MSmlDmDDFObject& vccDTCS = vccNode.AddChildObjectL( KNSmlDMVCC_DTCsToIM );
    vccDTCS.SetAccessTypesL( accessTypesGetReplace );
    vccDTCS.SetOccurenceL( MSmlDmDDFObject::EOne );
    vccDTCS.SetScopeL( MSmlDmDDFObject::EPermanent );
    vccDTCS.SetDFFormatL( MSmlDmDDFObject::EChr );
    vccDTCS.AddDFTypeMimeTypeL( KNSmlDMVCCTextPlain );
    vccDTCS.SetDescriptionL( KNSmlDMVCC_DTCsToIMExp );
    
    // PS to CS leaf
    MSmlDmDDFObject& vccDTIM = vccNode.AddChildObjectL( KNSmlDMVCC_DTIMToCs );
    vccDTIM.SetAccessTypesL( accessTypesGetReplace );
    vccDTIM.SetOccurenceL( MSmlDmDDFObject::EOne );
    vccDTIM.SetScopeL( MSmlDmDDFObject::EPermanent );
    vccDTIM.SetDFFormatL( MSmlDmDDFObject::EChr );
    vccDTIM.AddDFTypeMimeTypeL( KNSmlDMVCCTextPlain );
    vccDTIM.SetDescriptionL( KNSmlDMVCC_DTIMToCsExp );
    
    // DT restricted while held or waiting calls leaf
    MSmlDmDDFObject& vccDTHeld = 
                       vccNode.AddChildObjectL( KNSmlDMVCC_DTHeldWaitingCalls );
    vccDTHeld.SetAccessTypesL( accessTypesGetReplace );
    vccDTHeld.SetOccurenceL( MSmlDmDDFObject::EOne );
    vccDTHeld.SetScopeL( MSmlDmDDFObject::EPermanent );
    vccDTHeld.SetDFFormatL( MSmlDmDDFObject::EChr );
    vccDTHeld.AddDFTypeMimeTypeL( KNSmlDMVCCTextPlain );
    vccDTHeld.SetDescriptionL( KNSmlDMVCC_DTHeldWaitingCallsExp );
    
    // DT Allowed when CS Originated Call
    MSmlDmDDFObject& vccDTAllowedWhenCsOrig = 
               vccNode.AddChildObjectL( KNSmlDMVCC_DtAllowedWhenCsOriginated );
    vccDTAllowedWhenCsOrig.SetAccessTypesL( accessTypesGetReplace );
    vccDTAllowedWhenCsOrig.SetOccurenceL( MSmlDmDDFObject::EOne );
    vccDTAllowedWhenCsOrig.SetScopeL( MSmlDmDDFObject::EPermanent );
    vccDTAllowedWhenCsOrig.SetDFFormatL( MSmlDmDDFObject::EChr );
    vccDTAllowedWhenCsOrig.AddDFTypeMimeTypeL( KNSmlDMVCCTextPlain );
    vccDTAllowedWhenCsOrig.SetDescriptionL( KNSmlDMVCC_DtAllowedWhenCsOriginated );
        
    
    // WLAN HO treshold leaf
    MSmlDmDDFObject& vccWLANHotreshold = 
                       vccNode.AddChildObjectL( KNSmlDMVCC_WlanHOTreshold );
    vccWLANHotreshold.SetAccessTypesL( accessTypesGetReplace );
    vccWLANHotreshold.SetOccurenceL( MSmlDmDDFObject::EOne );
    vccWLANHotreshold.SetScopeL( MSmlDmDDFObject::EPermanent );
    vccWLANHotreshold.SetDFFormatL( MSmlDmDDFObject::EChr );
    vccWLANHotreshold.AddDFTypeMimeTypeL( KNSmlDMVCCTextPlain );
    vccWLANHotreshold.SetDescriptionL( KNSmlDMVCC_WlanHOTresholdExp );
    
    // WLAN HO hysteresis leaf
    MSmlDmDDFObject& vccWLANHoHysteresis = 
                       vccNode.AddChildObjectL( KNSmlDMVCC_WlanHOHysteresis );
    vccWLANHoHysteresis.SetAccessTypesL( accessTypesGetReplace );
    vccWLANHoHysteresis.SetOccurenceL( MSmlDmDDFObject::EOne );
    vccWLANHoHysteresis.SetScopeL( MSmlDmDDFObject::EPermanent );
    vccWLANHoHysteresis.SetDFFormatL( MSmlDmDDFObject::EChr );
    vccWLANHoHysteresis.AddDFTypeMimeTypeL( KNSmlDMVCCTextPlain );
    vccWLANHoHysteresis.SetDescriptionL( KNSmlDMVCC_WlanHOHysteresisExp );
    
    // WLAN HO hysteresis timer low leaf
    MSmlDmDDFObject& vccWLANHoHysteresisTimerLow = 
                       vccNode.AddChildObjectL( KNSmlDMVCC_WlanHysteresisTimerLow );
    vccWLANHoHysteresisTimerLow.SetAccessTypesL( accessTypesGetReplace );
    vccWLANHoHysteresisTimerLow.SetOccurenceL( MSmlDmDDFObject::EOne );
    vccWLANHoHysteresisTimerLow.SetScopeL( MSmlDmDDFObject::EPermanent );
    vccWLANHoHysteresisTimerLow.SetDFFormatL( MSmlDmDDFObject::EChr );
    vccWLANHoHysteresisTimerLow.AddDFTypeMimeTypeL( KNSmlDMVCCTextPlain );
    vccWLANHoHysteresisTimerLow.SetDescriptionL( KNSmlDMVCC_WlanHysteresisTimerLowExp );
    
    // WLAN HO hysteresis timer high leaf
    MSmlDmDDFObject& vccWLANHoHysteresisTimerHigh = 
                       vccNode.AddChildObjectL( KNSmlDMVCC_WlanHysteresisTimerHigh );
    vccWLANHoHysteresisTimerHigh.SetAccessTypesL( accessTypesGetReplace );
    vccWLANHoHysteresisTimerHigh.SetOccurenceL( MSmlDmDDFObject::EOne );
    vccWLANHoHysteresisTimerHigh.SetScopeL( MSmlDmDDFObject::EPermanent );
    vccWLANHoHysteresisTimerHigh.SetDFFormatL( MSmlDmDDFObject::EChr );
    vccWLANHoHysteresisTimerHigh.AddDFTypeMimeTypeL( KNSmlDMVCCTextPlain );
    vccWLANHoHysteresisTimerHigh.SetDescriptionL( KNSmlDMVCC_WlanHysteresisTimerHighExp );
    
    // CS HO treshold leaf
    MSmlDmDDFObject& vccCSHotreshold = 
                       vccNode.AddChildObjectL( KNSmlDMVCC_CSHOTreshold );
    vccCSHotreshold.SetAccessTypesL( accessTypesGetReplace );
    vccCSHotreshold.SetOccurenceL( MSmlDmDDFObject::EOne );
    vccCSHotreshold.SetScopeL( MSmlDmDDFObject::EPermanent );
    vccCSHotreshold.SetDFFormatL( MSmlDmDDFObject::EChr );
    vccCSHotreshold.AddDFTypeMimeTypeL( KNSmlDMVCCTextPlain );
    vccCSHotreshold.SetDescriptionL( KNSmlDMVCC_CSHOTresholdExp );
    
    // CS HO hysteresis leaf
    MSmlDmDDFObject& vccCSHoHysteresis = 
                       vccNode.AddChildObjectL( KNSmlDMVCC_CSHOHysteresis );
    vccCSHoHysteresis.SetAccessTypesL( accessTypesGetReplace );
    vccCSHoHysteresis.SetOccurenceL( MSmlDmDDFObject::EOne );
    vccCSHoHysteresis.SetScopeL( MSmlDmDDFObject::EPermanent );
    vccCSHoHysteresis.SetDFFormatL( MSmlDmDDFObject::EChr );
    vccCSHoHysteresis.AddDFTypeMimeTypeL( KNSmlDMVCCTextPlain );
    vccCSHoHysteresis.SetDescriptionL( KNSmlDMVCC_CSHOHysteresisExp );
    
    // CS HO hysteresis timer low leaf
    MSmlDmDDFObject& vccCSHoHysteresisTimerLow = 
                       vccNode.AddChildObjectL( KNSmlDMVCC_CSHysteresisTimerLow );
    vccCSHoHysteresisTimerLow.SetAccessTypesL( accessTypesGetReplace );
    vccCSHoHysteresisTimerLow.SetOccurenceL( MSmlDmDDFObject::EOne );
    vccCSHoHysteresisTimerLow.SetScopeL( MSmlDmDDFObject::EPermanent );
    vccCSHoHysteresisTimerLow.SetDFFormatL( MSmlDmDDFObject::EChr );
    vccCSHoHysteresisTimerLow.AddDFTypeMimeTypeL( KNSmlDMVCCTextPlain );
    vccCSHoHysteresisTimerLow.SetDescriptionL( KNSmlDMVCC_CSHysteresisTimerLowExp );
    
    // CS HO hysteresis timer high leaf
    MSmlDmDDFObject& vccCSHoHysteresisTimerHigh = 
                       vccNode.AddChildObjectL( KNSmlDMVCC_CSHysteresisTimerHigh );
    vccCSHoHysteresisTimerHigh.SetAccessTypesL( accessTypesGetReplace );
    vccCSHoHysteresisTimerHigh.SetOccurenceL( MSmlDmDDFObject::EOne );
    vccCSHoHysteresisTimerHigh.SetScopeL( MSmlDmDDFObject::EPermanent );
    vccCSHoHysteresisTimerHigh.SetDFFormatL( MSmlDmDDFObject::EChr );
    vccCSHoHysteresisTimerHigh.AddDFTypeMimeTypeL( KNSmlDMVCCTextPlain );
    vccCSHoHysteresisTimerHigh.SetDescriptionL( KNSmlDMVCC_CSHysteresisTimerHighExp );
    
    // ./VCC001/ToConRef/
    
    MSmlDmDDFObject& toConRefNode = vccNode.AddChildObjectL(KNSmlDMVCC_DmToConRef);
    toConRefNode.SetAccessTypesL( accessTypesGet );
    toConRefNode.SetOccurenceL( MSmlDmDDFObject::EOne );
    toConRefNode.SetScopeL( MSmlDmDDFObject::EDynamic );
    toConRefNode.SetDFFormatL( MSmlDmDDFObject::ENode );
    toConRefNode.SetDescriptionL( KNSmlDMVCC_DmToConRefExp );
    
    // ./VCC001/ToConRef/VoIP
        
    MSmlDmDDFObject& voipNode = toConRefNode.AddChildObjectL( KNSmlDMVCC_VoipServiceId );
    voipNode.SetAccessTypesL( accessTypesGet );
    voipNode.SetOccurenceL( MSmlDmDDFObject::EOne );
    voipNode.SetScopeL( MSmlDmDDFObject::EDynamic );
    voipNode.SetDFFormatL( MSmlDmDDFObject::ENode );
    voipNode.SetDescriptionL( KNSmlDMVCC_VoipServiceIdExp );
    
    // ./VCC001/ToConRef/VoIP/ConRef
    
    MSmlDmDDFObject& conRefVoipNode = voipNode.AddChildObjectL(KNSmlDMVCC_DmConRef);
    conRefVoipNode.SetAccessTypesL( accessTypesGetReplace );
    conRefVoipNode.SetOccurenceL( MSmlDmDDFObject::EOne );
    conRefVoipNode.SetScopeL( MSmlDmDDFObject::EDynamic );
    conRefVoipNode.SetDFFormatL( MSmlDmDDFObject::EChr );
    conRefVoipNode.SetDescriptionL( KNSmlDMVCC_DmConRefExp );
        
    // !NOTE! Ext node not inserted yet
  }

// ---------------------------------------------------------------------------
//  Note: HBufC* buf points to member iTempBuf that is deleted every time 
//  ConvertToUnicodeL is called and in class' destructor.
// ---------------------------------------------------------------------------
//
void CNSmlDmVCCAdapter::UpdateLeafObjectL( const TDesC8& aURI, 
            const TDesC8& /*aLUID*/, const TDesC8& aObject, 
            const TDesC8& /*aType*/, TInt aStatusRef )
    {
    RUBY_DEBUG_BLOCK( "CNSmlDmVCCAdapter::UpdateLeafObjectL" );
  	RUBY_DEBUG0( "ReadSPSettings" );
  	iVCCSettings->ReadSettingsL( KVccServiceName );
    CSmlDmAdapter::TError retValue = CSmlDmAdapter::EOk;
    HBufC* buf = ConvertToUnicodeL( aObject );
    
    // Find the last leaf in URI
    TPtrC8 uriSeg( LastURISeg( aURI ) );
  
    if ( KNSmlDMVCC_Name() == uriSeg )
        {
        // Don't do anything at the moment
        //iVCCSettings->SetNameL( aObject );
        }
    else if ( KNSmlDMVCC_VDI() == uriSeg )
        {
        if ( KErrNone != iVCCSettings->SetVdiL( buf->Des() ) )
            {
            retValue = CSmlDmAdapter::EInvalidObject;
            }
        }
    else if ( KNSmlDMVCC_VDN() == uriSeg )
        {
        if ( !( iVCCSettings->SetVdnL( buf->Des() ) ) )
            {
            retValue = CSmlDmAdapter::EInvalidObject;
            }
        }
    else if ( KNSmlDMVCC_PreferredDomain() == uriSeg )
        {
        TInt value = iVCCSettings->ConvertToIntL( buf->Des() );
    
        switch ( value )
            {
            case 0:
            case 1:
            case 2:
            case 3:
                iVCCSettings->SetPreferredDomainL( buf->Des() );
                break;
            default:
                retValue = CSmlDmAdapter::EInvalidObject;
                break;
          }
        }
    else if ( KNSmlDMVCC_ImmediateDT() == uriSeg )
        {
        TInt value = iVCCSettings->ConvertToIntL( buf->Des() );
    
        switch ( value )
            {
            case 0:
            case 1:
                iVCCSettings->SetImmediateDtL( buf->Des() );
                break;
            default:
                retValue = CSmlDmAdapter::EInvalidObject;
                break;
            }
        }
    else if ( KNSmlDMVCC_DTCsToIM() == uriSeg )
        {
        TInt value = iVCCSettings->ConvertToIntL( buf->Des() );
        
        switch ( value )
            {
            case 0:
            case 1:
                iVCCSettings->SetDtCsToPsAllowedL( buf->Des() );
                break;
            default:
                retValue = CSmlDmAdapter::EInvalidObject;
                break;
            }
        }
    else if ( KNSmlDMVCC_DTIMToCs() == uriSeg )
        {
        TInt value = iVCCSettings->ConvertToIntL( buf->Des() );
    
        switch ( value )
            {
            case 0:
            case 1:
                iVCCSettings->SetDtPsToCsAllowedL( buf->Des() );
                break;
            default:
                retValue = CSmlDmAdapter::EInvalidObject;
                break;
            }
        }
    else if ( KNSmlDMVCC_DtAllowedWhenCsOriginated() == uriSeg )
        {
        TInt value = iVCCSettings->ConvertToIntL( buf->Des() );
        
        switch ( value )
            {
            case 0:
            case 1:
                iVCCSettings->SetDtAllowedWhenCsOriginated( buf->Des() );
                break;
            default:
                retValue = CSmlDmAdapter::EInvalidObject;
                break;
            }
        }
    
    else if ( KNSmlDMVCC_DTHeldWaitingCalls() == uriSeg )
        {
        TInt value = iVCCSettings->ConvertToIntL( buf->Des() );

        switch ( value )
            {
            case 0:
            case 1:
                iVCCSettings->SetDtHeldWaitingCallsAllowedL( buf->Des() );
                break;
            default:
                retValue = CSmlDmAdapter::EInvalidObject;
                break;
            }
        }
           
    else if ( KNSmlDMVCC_WlanHOTreshold() == uriSeg )
        {
        iVCCSettings->SetDtWlanHoTresholdL( buf->Des() );
        }
    else if ( KNSmlDMVCC_WlanHOHysteresis() == uriSeg )
        {
        iVCCSettings->SetDtWlanHoHysteresisL( buf->Des() );
        }
    else if ( KNSmlDMVCC_WlanHysteresisTimerLow() == uriSeg )
        {
        iVCCSettings->SetDtWlanHoHysteresisTimerLowL( buf->Des() );
        }
    else if ( KNSmlDMVCC_WlanHysteresisTimerHigh() == uriSeg )
        {
        iVCCSettings->SetDtWlanHoHysteresisTimerHighL( buf->Des() );
        }
    
    else if ( KNSmlDMVCC_CSHOTreshold() == uriSeg )
        {
        iVCCSettings->SetDtCsHoTresholdL( buf->Des() );
        }
    else if ( KNSmlDMVCC_CSHOHysteresis() == uriSeg )
        {
        iVCCSettings->SetDtCsHoHysteresisL( buf->Des() );
        }
    else if ( KNSmlDMVCC_CSHysteresisTimerLow() == uriSeg )
        {
        iVCCSettings->SetDtCsHoHysteresisTimerLowL( buf->Des() );
        }
    else if ( KNSmlDMVCC_CSHysteresisTimerHigh() == uriSeg )
        {
        iVCCSettings->SetDtCsHoHysteresisTimerHighL( buf->Des() );
        }
    else if ( KNSmlDMVCC_DmConRef() == uriSeg )
        {
        retValue = UpdateVoipConRefL(aURI, aObject);
        }
        
    else
        { // No recognized leaf object found
        retValue = CSmlDmAdapter::EInvalidObject;
        }
  
    if ( CSmlDmAdapter::EOk == retValue )
        {
        RUBY_DEBUG0( "StoreSPSettings" );
        // Set operation successfull, store changes into SP Settings table
        iVCCSettings->StoreL();
        }
  
    Callback().SetStatusL( aStatusRef, retValue ); 
    }

// ---------------------------------------------------------------------------
//  
// ---------------------------------------------------------------------------
//
void CNSmlDmVCCAdapter::UpdateLeafObjectL( const TDesC8& /*aURI*/, 
            const TDesC8& /*aLUID*/,
                  RWriteStream*& /*aStream*/, const TDesC8& /*aType*/,
                  TInt aStatusRef )
    {
    RUBY_DEBUG_BLOCK( "CNSmlDmVCCAdapter::UpdateLeafObjectL -stream" );
    Callback().SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
    }

// ---------------------------------------------------------------------------
//  
// ---------------------------------------------------------------------------
//
void CNSmlDmVCCAdapter::DeleteObjectL( const TDesC8& aURI,
                                       const TDesC8& /*aLUID*/, 
                                       TInt aStatusRef )
    {
    RUBY_DEBUG_BLOCK( "CNSmlDmVCCAdapter::DeleteObjectL" );
   	iVCCSettings->ReadSettingsL( KVccServiceName );
    
    CSmlDmAdapter::TError retValue = CSmlDmAdapter::EOk;
    TPtrC8 uriSeg( LastURISeg( aURI ) );
  
    if ( KNSmlDMVCC_Name() == uriSeg )
        {
        // Don't do anything at the moment
        //iVCCSettings->SetNameL( KNSmlDMVCCValueNullDesc );
        }
    else if ( KNSmlDMVCC_VDI() == uriSeg )
        {
        iVCCSettings->SetVdiL( KNSmlDMVCCValueNullDesc );
        }
    else if ( KNSmlDMVCC_VDN() == uriSeg )
        {
        iVCCSettings->SetVdnL( KNSmlDMVCCValueNullDesc );
        }
    else if ( KNSmlDMVCC_PreferredDomain() == uriSeg )
        {
        iVCCSettings->SetPreferredDomainL( KNSmlDMVCCValueZero );
        }
    else if ( KNSmlDMVCC_ImmediateDT() == uriSeg )
        {
        iVCCSettings->SetImmediateDtL( KNSmlDMVCCValueZero );
        }
    else if ( KNSmlDMVCC_DTCsToIM() == uriSeg )
        {
        iVCCSettings->SetDtCsToPsAllowedL( KNSmlDMVCCValueZero );
        }
    else if ( KNSmlDMVCC_DTIMToCs() == uriSeg )
        {
        iVCCSettings->SetDtPsToCsAllowedL( KNSmlDMVCCValueZero );
        }
    else if ( KNSmlDMVCC_DTHeldWaitingCalls() == uriSeg )
        {
        iVCCSettings->SetDtHeldWaitingCallsAllowedL( KNSmlDMVCCValueZero );
        }
     else if ( KNSmlDMVCC_WlanHOTreshold() == uriSeg )
        {
        iVCCSettings->SetDtWlanHoTresholdL( KNSmlDMVCCValueZero );
        }
    else if ( KNSmlDMVCC_WlanHOHysteresis() == uriSeg )
        {
        iVCCSettings->SetDtWlanHoHysteresisL( KNSmlDMVCCValueZero );
        }
    else if ( KNSmlDMVCC_WlanHysteresisTimerLow() == uriSeg )
        {
        iVCCSettings->SetDtWlanHoHysteresisTimerLowL( KNSmlDMVCCValueZero );
        }
    else if ( KNSmlDMVCC_WlanHysteresisTimerHigh() == uriSeg )
        {
        iVCCSettings->SetDtWlanHoHysteresisTimerHighL( KNSmlDMVCCValueZero );
        }
    else if ( KNSmlDMVCC_CSHOTreshold() == uriSeg )
        {
        iVCCSettings->SetDtCsHoTresholdL( KNSmlDMVCCValueZero );
        }
    else if ( KNSmlDMVCC_CSHOHysteresis() == uriSeg )
        {
        iVCCSettings->SetDtCsHoHysteresisL( KNSmlDMVCCValueZero );
        }
    else if ( KNSmlDMVCC_CSHysteresisTimerLow() == uriSeg )
        {
        iVCCSettings->SetDtCsHoHysteresisTimerLowL( KNSmlDMVCCValueZero );
        }
    else if ( KNSmlDMVCC_CSHysteresisTimerHigh() == uriSeg )
        {
        iVCCSettings->SetDtCsHoHysteresisTimerHighL( KNSmlDMVCCValueZero );
        }
    else if ( KNSmlDMVCC_DmConRef() == uriSeg )
        {
        iVCCSettings->SetVoipServiceIdL( KNSmlDMVCCValueZero );
        }
    else if ( KNSmlDMVCC_DtAllowedWhenCsOriginated() == uriSeg )
        {
        iVCCSettings->SetDtAllowedWhenCsOriginated( KNSmlDMVCCValueZero );
        }
    else
        {
        retValue = CSmlDmAdapter::EInvalidObject;
        }
  
    if ( CSmlDmAdapter::EOk == retValue )
        {
        iVCCSettings->StoreL();
        }
  
    Callback().SetStatusL( aStatusRef, retValue ); 
    }

// ---------------------------------------------------------------------------
//  
// ---------------------------------------------------------------------------
//
void CNSmlDmVCCAdapter::FetchLeafObjectL( const TDesC8& aURI, 
             const TDesC8& /*aLUID*/, const TDesC8& aType, 
             TInt aResultsRef, TInt aStatusRef )
  {
  RUBY_DEBUG_BLOCK( "CNSmlDmVCCAdapter::FetchLeafObjectL" );
  
    CBufBase* result = CBufFlat::NewL( KNSmlDMVCCDefaultResultSize );
    CleanupStack::PushL( result );
    CSmlDmAdapter::TError status = FetchObjectL( aURI, *result );

    if ( status == CSmlDmAdapter::EOk )
        {
        Callback().SetResultsL( aResultsRef, *result, aType );
        }
    
    Callback().SetStatusL( aStatusRef, status );

    CleanupStack::PopAndDestroy( result );
  }

// ---------------------------------------------------------------------------
//  
// ---------------------------------------------------------------------------
//
void CNSmlDmVCCAdapter::FetchLeafObjectSizeL( const TDesC8& /*aURI*/, 
               const TDesC8& /*aLUID*/, const TDesC8& /*aType*/, 
               TInt /*aResultsRef*/, TInt aStatusRef )
  {
  RUBY_DEBUG_BLOCK( "CNSmlDmVCCAdapter::FetchLeafObjectSizeL" );
  
  // Not supported 
  Callback().SetStatusL( aStatusRef, CSmlDmAdapter::EError );
  }

// ---------------------------------------------------------------------------
//  
// ---------------------------------------------------------------------------
//
void CNSmlDmVCCAdapter::ChildURIListL( const TDesC8& aURI, 
                                       const TDesC8& /*aLUID*/,
              const CArrayFix<TSmlDmMappingInfo>& /*aPreviousURISegmentList*/,
              TInt aResultsRef, TInt aStatusRef )
  {
  RUBY_DEBUG_BLOCK( "CNSmlDmVCCAdapter::ChildURIListL" );
  
    CSmlDmAdapter::TError status = CSmlDmAdapter::EError;

    CBufBase *currentURISegmentList = CBufFlat::NewL( KNSmlDMVCCDefaultResultSize );
    CleanupStack::PushL( currentURISegmentList );
    TBuf8<KNSmlDMVCCMaxUriLength> mappingInfo( KNullDesC8 );
    TBuf8<KNSmlDMVCCMaxUriLength> uri = aURI;
    TBuf8<KSmlMaxURISegLen> segmentName;
    
     if ( KNSmlDMVCC_Node() == LastURISeg( aURI ) )
        {
        // Name
        segmentName.Copy( KNSmlDMVCC_Name );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMVCCSeparator );
        // VDI
        segmentName.Copy( KNSmlDMVCC_VDI );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMVCCSeparator );
        // VDN
        segmentName.Copy( KNSmlDMVCC_VDN );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMVCCSeparator );
        // Preferred domain
        segmentName.Copy( KNSmlDMVCC_PreferredDomain );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMVCCSeparator );
        // Immediate dt
        segmentName.Copy( KNSmlDMVCC_ImmediateDT );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMVCCSeparator );
        // DT CS to PS
        segmentName.Copy( KNSmlDMVCC_DTCsToIM );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMVCCSeparator );
        // DT PS to CS
        segmentName.Copy( KNSmlDMVCC_DTIMToCs );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMVCCSeparator );
        // DT Held waiting calls
        segmentName.Copy( KNSmlDMVCC_DTHeldWaitingCalls );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMVCCSeparator );
            
        // Wlan Ho Treshold
        segmentName.Copy( KNSmlDMVCC_WlanHOTreshold );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMVCCSeparator );
        
         // Wlan Ho Hysteresis
        segmentName.Copy( KNSmlDMVCC_WlanHOHysteresis );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMVCCSeparator );
            
         // Wlan Ho Hysteresis Timer Low
        segmentName.Copy( KNSmlDMVCC_WlanHysteresisTimerLow );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMVCCSeparator );
            
         // Wlan Ho Hysteresis Timer High
        segmentName.Copy( KNSmlDMVCC_WlanHysteresisTimerHigh );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMVCCSeparator );
        
        // CS Ho Treshold
        segmentName.Copy( KNSmlDMVCC_CSHOTreshold );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMVCCSeparator );
        
         // CS Ho Hysteresis
        segmentName.Copy( KNSmlDMVCC_CSHOHysteresis );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMVCCSeparator );
            
         // CS Ho Hysteresis Timer Low
        segmentName.Copy( KNSmlDMVCC_CSHysteresisTimerLow );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMVCCSeparator );
            
         // CS Ho Hysteresis Timer High
        segmentName.Copy( KNSmlDMVCC_CSHysteresisTimerHigh );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMVCCSeparator );
        
        // DT Allowed When CS Originated
        segmentName.Copy( KNSmlDMVCC_DtAllowedWhenCsOriginated );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            segmentName );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), 
            KNSmlDMVCCSeparator );
                
                
    	status = CSmlDmAdapter::EOk;
        }
    else if( KNSmlDMVCC_DmToConRef() == LastURISeg( aURI ) )    // ./VCC001/ToConRef
      		{
        	segmentName.Copy( KNSmlDMVCC_VoipServiceId );
        	currentURISegmentList->InsertL( currentURISegmentList->Size(), segmentName );
        	currentURISegmentList->InsertL( currentURISegmentList->Size(), KNSmlDMVCCSeparator );
			status = CSmlDmAdapter::EOk;
	       	}
    else if( KNSmlDMVCC_VoipServiceId() == LastURISeg( aURI ) )         // ./VCC001/ToConRef/VoIP
        	{
        	segmentName.Copy( KNSmlDMVCC_DmConRef );
        	currentURISegmentList->InsertL( currentURISegmentList->Size(), segmentName );
        	currentURISegmentList->InsertL( currentURISegmentList->Size(), KNSmlDMVCCSeparator );
    		status = CSmlDmAdapter::EOk;
        	}
         
    
    Callback().SetStatusL( aStatusRef, status );
    Callback().SetResultsL( aResultsRef, *currentURISegmentList, KNullDesC8 );
    CleanupStack::PopAndDestroy( currentURISegmentList ); 
  }

// ---------------------------------------------------------------------------
//  
// ---------------------------------------------------------------------------
//
void CNSmlDmVCCAdapter::AddNodeObjectL( const TDesC8& /*aURI*/, 
           const TDesC8& /*aParentLUID*/, TInt aStatusRef )
  {
  RUBY_DEBUG_BLOCK( "CNSmlDmVCCAdapter::AddNodeObjectL" );
  
  // Not supported 
  Callback().SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
  }

// ---------------------------------------------------------------------------
//  
// ---------------------------------------------------------------------------
//
void CNSmlDmVCCAdapter::ExecuteCommandL( const TDesC8& /*aURI*/, 
            const TDesC8& /*aLUID*/, const TDesC8& /*aArgument*/, 
            const TDesC8& /*aType*/, TInt aStatusRef )
  {
  RUBY_DEBUG_BLOCK( "CNSmlDmVCCAdapter::ExecuteCommandL" );
  
  // Not supported 
  Callback().SetStatusL( aStatusRef, CSmlDmAdapter::EError ); 
  }

// ---------------------------------------------------------------------------
//  
// ---------------------------------------------------------------------------
//
void CNSmlDmVCCAdapter::ExecuteCommandL( const TDesC8& /*aURI*/, 
            const TDesC8& /*aLUID*/, RWriteStream*& /*aStream*/, 
            const TDesC8& /*aType*/, TInt aStatusRef )
  {
  RUBY_DEBUG_BLOCK( "CNSmlDmVCCAdapter::ExecuteCommandL -stream" );
  
  // Not supported 
  Callback().SetStatusL( aStatusRef, CSmlDmAdapter::EError );
  }
                      
// ---------------------------------------------------------------------------
//  
// ---------------------------------------------------------------------------
//
void CNSmlDmVCCAdapter::CopyCommandL( const TDesC8& /*aTargetURI*/, 
           const TDesC8& /*aTargetLUID*/, const TDesC8& /*aSourceURI*/, 
           const TDesC8& /*aSourceLUID*/, const TDesC8& /*aType*/, 
           TInt aStatusRef )
  {
  RUBY_DEBUG_BLOCK( "CNSmlDmVCCAdapter::CopyCommandL" );
  
  // Not supported 
  Callback().SetStatusL( aStatusRef, CSmlDmAdapter::EError ); 
  }

// ---------------------------------------------------------------------------
//  
// ---------------------------------------------------------------------------
//
void CNSmlDmVCCAdapter::StartAtomicL()
  {
  RUBY_DEBUG_BLOCK( "CNSmlDmVCCAdapter::StartAtomicL()" );
  // Not supported 
  }

// ---------------------------------------------------------------------------
//  
// ---------------------------------------------------------------------------
//
void CNSmlDmVCCAdapter::CommitAtomicL()
  {
  RUBY_DEBUG_BLOCK( "CNSmlDmVCCAdapter::CommitAtomicL()" );
  // Not supported 
  }

// ---------------------------------------------------------------------------
//  
// ---------------------------------------------------------------------------
//
void CNSmlDmVCCAdapter::RollbackAtomicL()
  {
  RUBY_DEBUG_BLOCK( "CNSmlDmVCCAdapter::RollbackAtomicL()" );
  // Not supported 
  }

// ---------------------------------------------------------------------------
//  
// ---------------------------------------------------------------------------
//
TBool CNSmlDmVCCAdapter::StreamingSupport( TInt& /*aItemSize*/ )
  {
  RUBY_DEBUG_BLOCK( "CNSmlDmVCCAdapter::StreamingSupport" );
  return EFalse;
  }

// ---------------------------------------------------------------------------
//  
// ---------------------------------------------------------------------------
//
void CNSmlDmVCCAdapter::StreamCommittedL()
  {
  RUBY_DEBUG_BLOCK( "CNSmlDmVCCAdapter::StreamCommittedL" );
  // Not supported 
  }

// ---------------------------------------------------------------------------
//  
// ---------------------------------------------------------------------------
//
void CNSmlDmVCCAdapter::CompleteOutstandingCmdsL()
  {
  RUBY_DEBUG_BLOCK( "CNSmlDmVCCAdapter::CompleteOutstandingCmdsL()" );
  
  iVCCSettings->StoreL();
  }

// ---------------------------------------------------------------------------
// Returns only the last uri segment.
// ---------------------------------------------------------------------------
//
const TPtrC8 CNSmlDmVCCAdapter::LastURISeg( const TDesC8& aURI )
    {
    RUBY_DEBUG_BLOCK( "CNSmlDmVCCAdapter::LastURISeg" );
    TInt offset = aURI.LocateReverse( KNSmlDMVCCSeparatorChar  );
    
    RUBY_DEBUG1( " - offset == %d", offset );    
    
    TInt i;
    for ( i=aURI.Length()-1; i >= 0; i-- )
        {
        if ( aURI[i]=='/' )
            {
            break;
            }
        }
    if ( i==0 )
        {
        return aURI;
        }
    else
        {
        return aURI.Mid( i + 1 );
        }    
    }

// ---------------------------------------------------------------------------
//  
// ---------------------------------------------------------------------------
//
CSmlDmAdapter::TError CNSmlDmVCCAdapter::FetchObjectL( const TDesC8& aURI, 
                                                   CBufBase& aResult )
  {
    RUBY_DEBUG_BLOCK( "CNSmlDmVCCAdapter::FetchObjectL" );
    TInt composeResult = ETrue; 
    iVCCSettings->ReadSettingsL( KVccServiceName );
    
    CSmlDmAdapter::TError retValue = CSmlDmAdapter::EOk; 
    TInt err( KErrNone );
    TPtrC8 lastUriSeg = LastURISeg( aURI );
    CSPEntry* entry = CSPEntry::NewLC();
    
    iVCCSettings->FindServiceEntryL( KNSmlDMVCC_ServiceName, *entry );
    
    if ( entry->PropertyCount() > 0 )
      {
      RUBY_DEBUG0( "- Service Entry found" );
      TServiceId serviceId = entry->GetServiceId();   
      CSPProperty* property = CSPProperty::NewLC();
      
    if ( KNSmlDMVCC_Name() == lastUriSeg )
      {
      RUBY_DEBUG0( "- Name" );
      err = iVCCSettings->FindPropertyL( serviceId, ESubPropertyVccName, 
                               *property );
      }
    else if ( KNSmlDMVCC_VDI() == lastUriSeg )
          {
          RUBY_DEBUG0( "- VDI" );
          err = iVCCSettings->FindPropertyL( serviceId, ESubPropertyVccVDI, 
                                   *property );
          }
    else if ( KNSmlDMVCC_VDN() == lastUriSeg )
      {
      RUBY_DEBUG0( "- VDN" );
      err = iVCCSettings->FindPropertyL( serviceId, ESubPropertyVccVDN, 
                               *property );
      }
    else if ( KNSmlDMVCC_PreferredDomain() == lastUriSeg )
      {
      RUBY_DEBUG0( "- Preferred Domain" );
      err = iVCCSettings->FindPropertyL( serviceId, 
                               ESubPropertyVccPreferredDomain, 
                               *property );
      }
    else if ( KNSmlDMVCC_ImmediateDT() == lastUriSeg )
      {
      RUBY_DEBUG0( "- Immediate DT" );
      err = iVCCSettings->FindPropertyL( serviceId, 
                               ESubPropertyVccImmediateDomainTransfer, 
                               *property );
      }
    else if ( KNSmlDMVCC_DTCsToIM() == lastUriSeg )
      {
      RUBY_DEBUG0( "- CS to PS" );
      err = iVCCSettings->FindPropertyL( serviceId, 
                               ESubPropertyVccDtCstoPsAllowed, 
                               *property );
      }
    else if ( KNSmlDMVCC_DTIMToCs() == lastUriSeg )
      {
      RUBY_DEBUG0( "- PS to CS" );
      err = iVCCSettings->FindPropertyL( serviceId, 
                                     ESubPropertyVccDtPstoCsAllowed, 
                                     *property );
      }
    else if ( KNSmlDMVCC_DTHeldWaitingCalls() == lastUriSeg )
      {
      RUBY_DEBUG0( "- Held waiting calls" );
      err = iVCCSettings->FindPropertyL( serviceId, 
                             ESubPropertyVccDtHeldWaitingCallsAllowed, 
                             *property );
      }
   
    else if ( KNSmlDMVCC_WlanHOTreshold() == lastUriSeg )
      {
      RUBY_DEBUG0( "- WlanHOTreshold" );
      err = iVCCSettings->FindPropertyL( serviceId, 
                             ESubPropertyVccDtWLANHoTriggerLevel, 
                             *property );
      }
    else if ( KNSmlDMVCC_WlanHOHysteresis() == lastUriSeg )
      {
      RUBY_DEBUG0( "- WlanHOHysteresis" );
      err = iVCCSettings->FindPropertyL( serviceId, 
                             ESubPropertyVccDtWLANHoHysteresis, 
                             *property );
      }

    else if ( KNSmlDMVCC_WlanHysteresisTimerLow() == lastUriSeg )
      {
      RUBY_DEBUG0( "- _WlanHysteresisTimerLow" );
      err = iVCCSettings->FindPropertyL( serviceId, 
                             ESubPropertyVccDtWLANHoHysteresisTimerLow, 
                             *property );
      }

    else if ( KNSmlDMVCC_WlanHysteresisTimerHigh() == lastUriSeg )
      {
      RUBY_DEBUG0( "- WlanHysteresisTimerHigh" );
      err = iVCCSettings->FindPropertyL( serviceId, 
                             ESubPropertyVccDtCSHoHysteresisTimerHigh, 
                             *property );
      }
   
    else if ( KNSmlDMVCC_CSHOTreshold() == lastUriSeg )
      {
      RUBY_DEBUG0( "- CSHOTreshold" );
      err = iVCCSettings->FindPropertyL( serviceId, 
                             ESubPropertyVccDtCSHoTriggerLevel, 
                             *property );
      }
    else if ( KNSmlDMVCC_CSHOHysteresis() == lastUriSeg )
      {
      RUBY_DEBUG0( "- CSHOHysteresis" );
      err = iVCCSettings->FindPropertyL( serviceId, 
                             ESubPropertyVccDtCSHoHysteresis, 
                             *property );
      }

    else if ( KNSmlDMVCC_CSHysteresisTimerLow() == lastUriSeg )
      {
      RUBY_DEBUG0( "- CSHysteresisTimerLow" );
      err = iVCCSettings->FindPropertyL( serviceId, 
                             ESubPropertyVccDtCSHoHysteresisTimerLow, 
                             *property );
      }

    else if ( KNSmlDMVCC_CSHysteresisTimerHigh() == lastUriSeg )
      {
      RUBY_DEBUG0( "- CSHysteresisTimerHigh" );
      err = iVCCSettings->FindPropertyL( serviceId, 
                             ESubPropertyVccDtCSHoHysteresisTimerHigh, 
                             *property );
      }
  
    else if( KNSmlDMVCC_DmConRef() == lastUriSeg )
      {
      TPtrC8 conRefUri = NSmlDmURI::RemoveLastSeg(aURI);
      TPtrC8 lastConRefSeg = NSmlDmURI::LastURISeg(conRefUri);
      composeResult = EFalse;  
      if(KNSmlDMVCC_VoipServiceId() == lastConRefSeg )
            {
            // Voip ConRef
            retValue= FetchVoipConRefL(aURI, aResult);
            
            }
      else
            {
             retValue = CSmlDmAdapter::ENotFound;
            } 
      }
    else if ( KNSmlDMVCC_DtAllowedWhenCsOriginated() == lastUriSeg )
        {
        RUBY_DEBUG0( "- DT allowed when CS originated" );
        err = iVCCSettings->FindPropertyL( serviceId, 
                             ESubPropertyVccDtAllowedWhenCsOriginated,
                             *property );
        }
    else
      {
      RUBY_DEBUG0( "- Invalid object" );
      retValue = CSmlDmAdapter::EInvalidObject;
      }
    
    if ( KErrNone == err && composeResult )
      {
      RUBY_DEBUG0( "- Result handling STARTt" );
      // Get result from property
      TBuf<KNSmlDMVCCMaxResultLength> buf;
      err = property->GetValue( buf );
      
      // Insert result into result buffer
      TBuf8<KNSmlDMVCCMaxResultLength> buf8;
      buf8.Append( buf );
      aResult.InsertL( 0, buf8 );
      RUBY_DEBUG0( "- Result handling STOP" );
      }
    
    CleanupStack::PopAndDestroy( property );
      }
    else
      {
      RUBY_DEBUG0( "- Service not found" );
      // Service not found
      retValue = CSmlDmAdapter::ENotFound;
      }
    
  if ( KErrNone != err )
    {
    RUBY_DEBUG1( "- Other error occurred [%d]", err );
    // Other error occurred
    retValue = CSmlDmAdapter::EError;
    }
    
    CleanupStack::PopAndDestroy( entry );
    
    return retValue;
  }

// ---------------------------------------------------------------------------
//  
// ---------------------------------------------------------------------------
//
HBufC* CNSmlDmVCCAdapter::ConvertToUnicodeL( const TDesC8& aSource )
    {
    if ( iTempBuf )
        {
        delete iTempBuf;
        iTempBuf = NULL;
        }

    HBufC* temp = EscapeUtils::ConvertToUnicodeFromUtf8L( aSource );
    CleanupStack::PushL( temp );
    iTempBuf = temp->AllocL();
    CleanupStack::PopAndDestroy( temp );
    
    //return static_cast<const TDesC&>(*iTempPtr);
    return iTempBuf;
    }

// ---------------------------------------------------------------------------
//  
// ---------------------------------------------------------------------------
//
CSmlDmAdapter::TError CNSmlDmVCCAdapter::FetchVoipConRefL( const TDesC8& /*aUri*/,
                                                              CBufBase& aObject )
	{
    RUBY_DEBUG_BLOCK( "CNSmlDmVCCAdapter::FetchVoipConRefL" );
    CSmlDmAdapter::TError status( CSmlDmAdapter::EOk );
    
    iVCCSettings->ReadSettingsL( KVccServiceName );
        
    // Fetch correct id here
    RUBY_DEBUG1( "ServiceId=%d", iVCCSettings->VoipServiceId() );
    CRCSEProfileRegistry* cRCSEProfileRegistry = CRCSEProfileRegistry::NewLC();   // CS: 1
    RPointerArray<CRCSEProfileEntry> foundEntries;
    CleanupStack::PushL( TCleanupItem( ResetAndDestroyEntries, &foundEntries ) ); // CS: 2
        
    cRCSEProfileRegistry->FindByServiceIdL( (TUint32)iVCCSettings->VoipServiceId(),
                                           foundEntries );
    
    RUBY_DEBUG1( "ProfileCount=%d", foundEntries.Count() );
    if ( foundEntries.Count() > 0 )
    	{
    	RUBY_DEBUG1( "VoipId=%d", foundEntries[0]->iId );
    	TInt voipId = foundEntries[0]->iId;
         
    	CBufBase* result = CBufFlat::NewL( 1 );
    	CleanupStack::PushL( result );                      // CS: 3
        
    	// Request all VoIP identifiers 
    	Callback().FetchLinkL( KNSmlDMVCC_DmVoipLinkPrefix, *result, status );
        
    	if( status == CSmlDmAdapter::EOk )
        	{
        	TInt resultSize( result->Size() );
        	HBufC8* linkList = HBufC8::NewLC( resultSize ); // CS: 4
        	TPtr8 linkListPtr( linkList->Des() );
        	TInt numOfUriSegs( KErrNone );
            
        	if( resultSize > 0 )
            	{
            	linkListPtr.Copy( result->Ptr(0) );
            	RemoveLastSeparator( linkListPtr );
            	numOfUriSegs = NSmlDmURI::NumOfURISegs( linkListPtr );
            	}
            
    		// Find correct VoIP profile
    		while( numOfUriSegs )
        		{
        		TBuf8<KNSmlDMVCC_DefaultResultSize> object; 
                
        		// Build URI: ./VoIP/<X>/VoIPId
        		object.Copy( KNSmlDMVCC_DmVoipLinkPrefix );
        		object.Append( KNSmlDMVCCSeparator );
        		object.Append( NSmlDmURI::LastURISeg( linkListPtr ) );
        		object.Append( KNSmlDMVCCSeparator );
        		object.Append( KNSmlDMVCC_DmVoipId );
                
        		// Fetch VoIPId from URI
        		result->Reset();
        		Callback().FetchLinkL( object, *result, status );
                
        		if( status == CSmlDmAdapter::EOk )
            		{
            		// Check if VoIPId match
            		TLex8 lex( result->Ptr( 0 ) );
    				TInt voipIdFromVoipAdapter( KErrNotFound );
    				lex.Val( voipIdFromVoipAdapter );
                                      
            		if( voipId == voipIdFromVoipAdapter )
                		{
                		aObject.InsertL( 0, NSmlDmURI::RemoveLastSeg( object ) );
                		break;
                		}
            		}
                
        		// VoIPId did not match --> continue
        		linkListPtr.Copy( NSmlDmURI::RemoveLastSeg( linkListPtr ) );
         
        		--numOfUriSegs;
        		}
            CleanupStack::PopAndDestroy( linkList );        // CS: 3
        	} 
        CleanupStack::PopAndDestroy( result );              // CS: 2
    	}
	else
    	{
    	RUBY_DEBUG0("Profiles not found");
    	status = CSmlDmAdapter::ENotFound;
    	}	
    CleanupStack::PopAndDestroy();							// CS: 1
    CleanupStack::PopAndDestroy( cRCSEProfileRegistry ); 	// CS: 0
    return status;
	}

// -----------------------------------------------------------------------------
// CNSmlDmVCCAdapter::UpdateVoipConRefL
// -----------------------------------------------------------------------------
//
CSmlDmAdapter::TError CNSmlDmVCCAdapter::UpdateVoipConRefL(const TDesC8& /*aUri*/, 
                                                         const TDesC8& aObject)
    {
    RUBY_DEBUG_BLOCK("CNSmlDmVCCAdapter::UpdateVoipConRefL");
    
    CSmlDmAdapter::TError status( CSmlDmAdapter::EOk );
    
    CBufBase* result = CBufFlat::NewL(1);
    CleanupStack::PushL(result);			// CS: 1

    // Request all voip settings identifiers 
    Callback().FetchLinkL(KNSmlDMVCC_DmVoip, *result, status);
    
    if( status == CSmlDmAdapter::EOk )
        {
        RUBY_DEBUG0("Voip profiles found");
        // Fetch VoIP profile id
        TBuf8<KNSmlDMVCC_DefaultResultSize> object;
        object.Copy( aObject );
        object.Append( KNSmlDMVCCSeparator );
        object.Append( KNSmlDMVCC_DmVoipId );
        
        result->Reset();
        Callback().FetchLinkL( object, *result, status );
        
        if( status == CSmlDmAdapter::EOk )
            {
         	RUBY_DEBUG0("Voip Link fetched ok");
        
            // Convert VoipId to ServiceID
           	CRCSEProfileRegistry* cRCSEProfileRegistry = CRCSEProfileRegistry::NewLC(); // CS: 2
    	
    		TInt voipProfileId;
       		TLex8 temp( result->Ptr( 0 ) );
        	TInt res = temp.Val(voipProfileId) ;
    		
    		RUBY_DEBUG1( "VoipId=%d", voipProfileId );
    		if (res == KErrNone )
    			{
    			CRCSEProfileEntry* foundEntry = CRCSEProfileEntry::NewLC();		// CS: 3 
    			TRAP( res, cRCSEProfileRegistry->FindL( voipProfileId, *foundEntry ) );
    	   	
    			RUBY_DEBUG1( "ServiceId=%d", foundEntry->iServiceProviderId );
                if (res==KErrNone)
                	{
                	// And save
    				iVCCSettings->SetVoipServiceIdL(foundEntry->iServiceProviderId);
    			   	}
    			else
    				{
    				RUBY_DEBUG0("Service id not found");
        			status = CSmlDmAdapter::EError;
                    }
            	CleanupStack::PopAndDestroy( foundEntry );						// CS: 2
    		   	}
            else
                {
                RUBY_DEBUG0("Voip id fetch failed");
                status = CSmlDmAdapter::EError;
                }
    		CleanupStack::PopAndDestroy( cRCSEProfileRegistry );				// CS: 1
            }
        else
            {
        	RUBY_DEBUG0("Voip link not found");
            status = CSmlDmAdapter::EError;
            }
        }
    else
        {
        RUBY_DEBUG0("Voip profiles not found");
        status = CSmlDmAdapter::ENotFound;
        }
    
    CleanupStack::PopAndDestroy(result);										// CS: 0
    return status;
    }

    
// -----------------------------------------------------------------------------
// CNSmlDmVCCAdapter::RemoveLastSeparator
// -----------------------------------------------------------------------------
//
void CNSmlDmVCCAdapter::RemoveLastSeparator( TPtr8& aURI )
    {
        
    if( aURI.Length() > 0 )
    	{
    	TChar separator( '/' );
    	TInt lastchar( aURI.Length() - 1 );
    	
    	// Check if last character is separator.
    	if( aURI[lastchar] == separator )
    		{
    		// Delete separator.
    		aURI.Delete( lastchar, 1 );
    		}
    	}
    }
	
// -----------------------------------------------------------------------------
// CNSmlDmVCCAdapter::ResetAndDestroyEntries
// -----------------------------------------------------------------------------
//
void CNSmlDmVCCAdapter::ResetAndDestroyEntries( TAny* anArray )
	{

    RPointerArray<CRCSEProfileEntry>* array = 
        reinterpret_cast<RPointerArray<CRCSEProfileEntry>*>( anArray );
        
    if (array)
        {
        array->ResetAndDestroy();
        array->Close();
        }
	}

//  End of File  
	
