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
* Description:  SCCP Setting entry, which is stored to db
*
*/



// INCLUDE FILES
#include "crcsesccpentry.h"
#include "rcsedefaults.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CRCSESCCPEntry::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CRCSESCCPEntry* CRCSESCCPEntry::NewL()
    {
    CRCSESCCPEntry* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CRCSESCCPEntry::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CRCSESCCPEntry* CRCSESCCPEntry::NewLC()
    {
    CRCSESCCPEntry* self = new (ELeave) CRCSESCCPEntry();
    CleanupStack::PushL( self );
    self->ResetDefaultValues();
    return self;
    }

// -----------------------------------------------------------------------------
// CRCSESCCPEntry::CRCSESCCPEntry
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CRCSESCCPEntry::CRCSESCCPEntry()
    {
    }
    
// Destructor
EXPORT_C CRCSESCCPEntry::~CRCSESCCPEntry()
    {
    }

// -----------------------------------------------------------------------------
// CRCSESCCPEntry::ResetDefaultValues
// Resets profile entry to default settings.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRCSESCCPEntry::ResetDefaultValues()
    {
    iSCCPSettingId        = 0;
    iProfileName          = KRCSEDefaultSCCPProfileName;
    iVOIPLogin            = KRCSEDefaultVOIPLogin;
    iAccessPoint          = KNotSet;      
    
    // Reset iCallManager.
    for ( TInt i = 0; i < KMaxArray; i++ )
        {
        iCallManager[ i ] = KNullDesC;
        }

    iStackVersion         = KRCSEDefaultSCCPStack;
    iDHCPTFTPEnabled      = KRCSEDefaultDHCPTFTP;
    iTFTPServerAddress    = KRCSEDefaultTFTPAddress;
    iPhoneNumber          = KRCSEDefaultSCCPNumber;
    iSCCPCertificates     = KRCSEDefaultSCCPCertificates;
    iMusicServerAdd       = KRCSEDefaultMusicServerAdd;
    iCFUncondAdd          = KRCSEDefaultCFUncondAdd;
    iVoiceMailBoxURI      = KRCSEDefaultSCCPVoiceMailboxName;
    iSCCPIntCallPrefix    = KRCSEDefaultSCCPIntCallPrefix;
    }

//  End of File  
