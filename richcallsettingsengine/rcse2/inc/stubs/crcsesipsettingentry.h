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
* Description:  Single SIP setting entry, which is stored to RCSE
*
*/



#ifndef CRCSESIPSETTINGENTRY_H
#define CRCSESIPSETTINGENTRY_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h>


// CONSTANTS
const TInt KMaxConferencingFactoryURILength = 200;

const TInt KMaxMusicServerURILenght = 200;

const TInt KMaxIPVoiceMailBoxURILenght = 200;

const TInt KMaxCFBusyURILenght = 200;

const TInt KMaxCFNoAnsURILenght = 200;

const TInt KMaxCFUncondURILenght = 200;

// OMA Provisioning Content 1.1 defines the minimum lengths of
// these constants. These constants are doubled.

// CLASS DECLARATION

/**
*  SIP Setting entry, which is stored to RCSE.
*
*  @lib RCSE.lib
*  @since S60 3.0
*/
class CRCSESIPSettingEntry : public CBase
    {
    public: // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CRCSESIPSettingEntry* NewL();

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CRCSESIPSettingEntry* NewLC();

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CRCSESIPSettingEntry();
        
    public: // New functions
                    
        /**
        * Resets entry to default values.
        * @since S60 3.0
        */
        void ResetDefaultValues();

    private:
                
        /**
        * C++ default constructor.
        * Initialises values to ENotSet.
        */
        CRCSESIPSettingEntry();

    public: // Data

        // Identifier of profile. This values is set by CRCSESSIPSetting, when
        // profile entry is got from CRCSESSIPSetting.
        TInt iVOIPSIPProfileSpecificId;
        
        // SIP Profile ID.
        TInt iSIPProfileId;

        // Conferencing factory URI.
        TBuf<KMaxConferencingFactoryURILength> iConferenceFactoryURI;
        
        // Music server URI.
        TBuf<KMaxMusicServerURILenght> iMusicServerURI;
        
        // IP VoiceMailBox URI.
        TBuf<KMaxIPVoiceMailBoxURILenght> iVoiceMailBoxURI;
        
        // Call forwarding no answer SIP or TEL URI.
        TBuf<KMaxCFNoAnsURILenght> iCFNoAnsURI;
        
        // Call forwarding when busy SIP or TEL URI.
        TBuf<KMaxCFBusyURILenght> iCFBusyURI;
        
        // Call forwarding unconditional SIP or TEL URI.
        TBuf<KMaxCFUncondURILenght> iCFUncondURI;
    };

#endif      // CRCSESIPSETTINGENTRY_H  
            
// End of File
