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
* Description:  SIP setting entry, which is stored to db
*
*/



// INCLUDE FILES
#include    "crcsesipsettingentry.h"
#include    "rcsedefaults.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CRCSESIPSettingEntry::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CRCSESIPSettingEntry* CRCSESIPSettingEntry::NewL()
    {
    CRCSESIPSettingEntry* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CRCSESIPSettingEntry::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CRCSESIPSettingEntry* CRCSESIPSettingEntry::NewLC()
    {
    CRCSESIPSettingEntry* self = new (ELeave) CRCSESIPSettingEntry();
    CleanupStack::PushL( self );
    self->ResetDefaultValues();
    return self;
    }

// -----------------------------------------------------------------------------
// CRCSESIPSettingEntry::CRCSESIPSettingEntry
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CRCSESIPSettingEntry::CRCSESIPSettingEntry()
    {
    }
    
// Destructor
EXPORT_C CRCSESIPSettingEntry::~CRCSESIPSettingEntry()
    {
    }

// -----------------------------------------------------------------------------
// CRCSESIPSettingEntry::ResetDefaultValues
// Resets SIP setting entry to default settings.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRCSESIPSettingEntry::ResetDefaultValues()
    {
    //SIP & VOIP specific ids 
    iVOIPSIPProfileSpecificId = 0;
        
    // SIP Profile ID.
    iSIPProfileId = 0;
    
    iConferenceFactoryURI    = KRCSEDefaultConferenceFactoryURI;
    iMusicServerURI          = KRCSEDefaultMusicServerURI;
    iVoiceMailBoxURI         = KRCSEDefaultVoiceMailBoxURI;
    iCFNoAnsURI              = KRCSEDefaultCFNoAnsURI;
    iCFBusyURI               = KRCSEDefaultCFBusyURI;
    iCFUncondURI             = KRCSEDefaultCFUncondURI;
    }

//  End of File  
