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


#ifndef CWPSIPSPECIFICSETTINGSITEM_H
#define CWPSIPSPECIFICSETTINGSITEM_H

//  INCLUDES
#include <e32base.h> // CBase

// CLASS DECLARATION
/**
*  This class keeps transiently the VoIP codec data and stores the 
*  codec via interface provided by RCSE.
*
*  @lib CWPSIPSpecificSettingsItem.lib
*  @since Series 60 3.0
*/
class CWPSIPSpecificSettingsItem : public CBase
    {
    #ifdef _DEBUG
    /**
     * Friend class for unit testing.
     */
    friend class UT_CWPVoIPAdapter;
    friend class UT_CWPSIPSpecificSettingsItem;
    friend class UT_CWPVoIPItem;
    #endif

    
    public: // Constructors and destructor
        
        /**
         * Two-phased constructor.
        */
        static CWPSIPSpecificSettingsItem* NewL();
        
        /**
         * Destructor.
         */
        virtual ~CWPSIPSpecificSettingsItem();

    public: // New functions
        /**
         * SetConferencingFactoryURIL.
         * @since Series 60 3.0
         * @param TDesC& aParamameter
         */
        void SetConferencingFactoryURIL( const TDesC& aParamameter );

        /**
         * SetMusicServerURIL
         * @since Series 60 3.0
         * @param TDesC& aParamameter
         */
        void SetMusicServerURIL( const TDesC& aParamameter );
        
        /**
         * SetIPVoiceMailBoxURIL
         * @since Series 60 3.0
         * @param TDesC& aParamameter
         */
        void SetIPVoiceMailBoxURIL( const TDesC& aParamameter );
        
        /**
         * SetCFNoAnsURIL
         * @since Series 60 3.0
         * @param TDesC& aParamameter
         */
        void SetCFNoAnsURIL( const TDesC& aParamameter );
        
        /**
         * SetCFBusyURIL
         * @since Series 60 3.0
         * @param TDesC& aParamameter
         */
        void SetCFBusyURIL( const TDesC& aParamameter );
        
        /**
         * SetCFUncondURIL
         * @since Series 60 3.0
         * @param TDesC& aParamameter
         */
        void SetCFUncondURIL( const TDesC& aParamameter );
        
        /**
         * SetToAppRefL
         * @since Series 60 3.0
         * @param TDesC& aParamameter
         */
        void SetToAppRefL( const TDesC8& aParamameter );
        
    private:

        /**
         * C++ default constructor.
         */
        CWPSIPSpecificSettingsItem();

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

    public: // Data
        
        // SIP related parameters described in the document VoIP software 
        // settings.
        
        /*
         * Id of the SIP profile specific VoIP settings.
         */
        TUint iId;
        
        /*
         * APPREF value which is referred (SIP or SCCP settings).
         * Own.
         */
        HBufC8* iToAppRef;
        
        /*
         * SIP profile id.
         */
        TUint iSipProfileReferred;
        

        /*
         * Conferencing factory URI.
         * Own.
         */
        HBufC* iConferencingFactoryURI;
        
        /*
         * Music server URI.
         * Own.
         */
        HBufC* iMusicServerURI;
        
        /*
         * IP VoiceMailBox URI.
         * Own.
         */
        HBufC* iIPVoiceMailBoxURI;
        
        /*
         * Call forwarding URI when no answer.
         * Own.
         */
        HBufC* iCFNoAnsURI;
        
        /*
         * Call forwarding URI when busy.
         * Own.
         */
        HBufC* iCFBusyURI;
        
        /*
         * Unconditional call forwarding URI.
         * Own.
         */
        HBufC* iCFUncondURI;
    };

#endif      // CWPSIPSPECIFICSETTINGSITEM_H
            
// End of File
