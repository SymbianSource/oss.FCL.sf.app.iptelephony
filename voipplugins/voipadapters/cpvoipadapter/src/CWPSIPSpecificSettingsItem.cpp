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
* Description:  Handles and stores the SIP profile relateted SIP URIs needed
*                by VoIP when functioning in SIP mode.
*
*/


// INCLUDE FILES
#include    "CWPSIPSpecificSettingsItem.h"

// ============================ MEMBER FUNCTIONS =============================
//----------------------------------------------------------------------------
// CWPSIPSpecificSettingsItem::CWPSIPSpecificSettingsItem
// C++ default constructor can NOT contain any code, that
// might leave.
//----------------------------------------------------------------------------
//
CWPSIPSpecificSettingsItem::CWPSIPSpecificSettingsItem()
    {
    }

//----------------------------------------------------------------------------
// CWPSIPSpecificSettingsItem::ConstructL
// Symbian 2nd phase constructor can leave.
//----------------------------------------------------------------------------
void CWPSIPSpecificSettingsItem::ConstructL()
    {
    iToAppRef = HBufC8::NewL( 0 );
    iConferencingFactoryURI = HBufC::NewL( 0 );
    iMusicServerURI = HBufC::NewL( 0 );
    iIPVoiceMailBoxURI = HBufC::NewL( 0 );
    iCFNoAnsURI = HBufC::NewL( 0 );
    iCFBusyURI = HBufC::NewL( 0 );
    iCFUncondURI = HBufC::NewL( 0 );
    }

//----------------------------------------------------------------------------
// CWPSIPSpecificSettingsItem::NewL
// Two-phased constructor.
//----------------------------------------------------------------------------
CWPSIPSpecificSettingsItem* CWPSIPSpecificSettingsItem::NewL()
    {
    CWPSIPSpecificSettingsItem* self = 
        new( ELeave ) CWPSIPSpecificSettingsItem;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// Destructor
CWPSIPSpecificSettingsItem::~CWPSIPSpecificSettingsItem()
    {
    delete iConferencingFactoryURI;
    delete iMusicServerURI;
    delete iIPVoiceMailBoxURI;
    delete iCFNoAnsURI;
    delete iCFBusyURI;
    delete iCFUncondURI;
    delete iToAppRef;
    }

//----------------------------------------------------------------------------
// CWPSIPSpecificSettingsItem::SetConferencingFactoryURIL
// 
//----------------------------------------------------------------------------
void CWPSIPSpecificSettingsItem::SetConferencingFactoryURIL( 
    const TDesC& aParamameter )
    {
    delete iConferencingFactoryURI;
    iConferencingFactoryURI = NULL;
    iConferencingFactoryURI = aParamameter.AllocL();
    }
//----------------------------------------------------------------------------
// CWPSIPSpecificSettingsItem::SetMusicServerURIL
// 
//----------------------------------------------------------------------------
void CWPSIPSpecificSettingsItem::SetMusicServerURIL( 
    const TDesC& aParamameter )
    {
    delete iMusicServerURI;
    iMusicServerURI = NULL;
    iMusicServerURI = aParamameter.AllocL();
    }

//----------------------------------------------------------------------------
// CWPSIPSpecificSettingsItem::SetIPVoiceMailBoxURIL
// 
//----------------------------------------------------------------------------
void CWPSIPSpecificSettingsItem::SetIPVoiceMailBoxURIL( 
    const TDesC& aParamameter )
    {
    delete iIPVoiceMailBoxURI;
    iIPVoiceMailBoxURI = NULL;
    iIPVoiceMailBoxURI = aParamameter.AllocL();
    }
    
//----------------------------------------------------------------------------
// CWPSIPSpecificSettingsItem::SetCFNoAnsURIL
// 
//----------------------------------------------------------------------------
void CWPSIPSpecificSettingsItem::SetCFNoAnsURIL( 
    const TDesC& aParamameter )
    {
    delete iCFNoAnsURI;
    iCFNoAnsURI = NULL;
    iCFNoAnsURI = aParamameter.AllocL();
    }
    
//----------------------------------------------------------------------------
// CWPSIPSpecificSettingsItem::SetCFBusyURIL
// 
//----------------------------------------------------------------------------
void CWPSIPSpecificSettingsItem::SetCFBusyURIL( 
    const TDesC& aParamameter )
    {
    delete iCFBusyURI;
    iCFBusyURI = NULL;
    iCFBusyURI = aParamameter.AllocL();
    }

//----------------------------------------------------------------------------
// CWPSIPSpecificSettingsItem::SetCFUncondURIL
// 
//----------------------------------------------------------------------------
void CWPSIPSpecificSettingsItem::SetCFUncondURIL( 
    const TDesC& aParamameter )
    {
    delete iCFUncondURI;
    iCFUncondURI = NULL;
    iCFUncondURI = aParamameter.AllocL();
    }
    
//----------------------------------------------------------------------------
// CWPSIPSpecificSettingsItem::SetToAppRefL
// 
//----------------------------------------------------------------------------
void CWPSIPSpecificSettingsItem::SetToAppRefL( 
    const TDesC8& aParamameter )
    {
    delete iToAppRef;
    iToAppRef = NULL;
    iToAppRef = aParamameter.AllocL();
    }

//  End of File  
