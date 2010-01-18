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
* Description:  Single SCCP entry, which is stored to RCSE
*
*/



#ifndef CRCSEPSCCPENTRY_H
#define CRCSEPSCCPENTRY_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h>

#include <crcseprofileentry.h>

// CONSTANTS

// OMA Provisioning Content 1.1 defines the minimum lengths of
// these constants. These constants are doubled.

const TInt KMaxSCCPStackNameLength = 16;

const TInt KMaxTFTPServerAddressLength = 64;

const TInt KMaxCallManagerAddressLength = 64;

const TInt KMaxSCCPNumberLength = 64;

const TInt KMaxSCCPCertificatesNameLength = 128;

const TInt KMaxMusicServerAddressLength = 128;

const TInt KMaxCFUncondAddLength = 64;

const TInt KMaxArray = 5;

const TInt KMaxSCCPProfileNameLength = 64;

const TInt KMaxSCCPVoiceMailboxLength = 128;

const TInt KMaxSCCPIntCallPrefixLength = 20;

// CLASS DECLARATION

/**
*  SCCP setting entry, which is stored to RCSE.
*
*  @lib RCSE.lib
*  @since S60 3.0
*/
class CRCSESCCPEntry : public CBase
    {
    public: // Enums

    enum TOnOff
        {
        EOONotSet = KNotSet,
        EOff = 0,
        EOn = 1
        };
    
    enum TManualAutomatic
        {
        EManual = 0,
        EAutomatic = 1    
        };

    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CRCSESCCPEntry* NewL();

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CRCSESCCPEntry* NewLC();

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CRCSESCCPEntry();
        
    public: // New methods
                    
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
        CRCSESCCPEntry();
        
    public:     // Data        
    
        // Identifier of profile. This values is set by CRCSESCCPSetting, when
        // profile entry is got from CRCSESCCPSetting.
        TUint32 iSCCPSettingId;

        // SCCP Profile name
        TBuf<KMaxSCCPProfileNameLength> iProfileName;
    
        // Voip login.
        TManualAutomatic iVOIPLogin;
        
        // ID of the access used access point.
        TInt32    iAccessPoint;

        // Array of call manager addresses(0-5).        
        TBuf<100> iCallManager[KMaxArray];
                
        // SCCP stack version.
        TBuf<KMaxSCCPStackNameLength> iStackVersion; 
        
        // Usage of TFTP server and DHCP services.
        TOnOff iDHCPTFTPEnabled;
        
        // TFTP server address.
        TBuf<KMaxTFTPServerAddressLength> iTFTPServerAddress;
        
        // SCCP profile phone number.
        TBuf<KMaxSCCPNumberLength> iPhoneNumber;
        
        // List of SCCP certificates.
        TBuf<KMaxSCCPCertificatesNameLength> iSCCPCertificates;
        
        // Music server address.
        TBuf<KMaxMusicServerAddressLength> iMusicServerAdd;
        
        // CFUncond address.
        TBuf<KMaxCFUncondAddLength> iCFUncondAdd;

        // SCCP Voice Mailbox address
        TBuf<KMaxSCCPVoiceMailboxLength> iVoiceMailBoxURI;

        // SCCP International call  prefix
        TBuf<KMaxSCCPIntCallPrefixLength> iSCCPIntCallPrefix;
    };

#endif      // CRCSESCCPENTRY_H   
            
// End of File
