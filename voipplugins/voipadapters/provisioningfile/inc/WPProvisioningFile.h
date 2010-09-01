/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handles provisioning settings.
*
*/


#ifndef WPPROVISIONINGFILE_H
#define WPPROVISIONINGFILE_H

//  INCLUDES
#include    <e32base.h>
#include    "wpprovisioningfileconst.h"

// FORWARD DECLARATIONS
class CWPEngine;
class CWPOMAMsgHandler;
class CWPProtocolProfileHandlerSIP;
class CRepository;
    
// CLASS DECLARATION

/**
*  CWPProvisioningFile handles OMA provisioning document. 
*
*  @lib ProvisioningFile.lib
*  @since Series 60 3.0
*/
NONSHARABLE_CLASS(CWPProvisioningFile) : public CBase
    {
    
#ifdef PROVISIONINGFILE_TEST
    
    friend class UT_ProvisioningFile;
    
#endif
    
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CWPProvisioningFile* NewL( TFileName aFileName = KDefaultFilePath() );

        /**
        * Second two-phased constructor.
        */ 
        static CWPProvisioningFile* NewLC( TFileName aFileName );

        /**
        * Destructor.
        */
        virtual ~CWPProvisioningFile();

    public: // New functions
        
        /**
        * ProvFileContainsIapL, OMA provisioning message constains IAP.
        * @since Series 60 3.0
        * @return TBool ETrue if message contains IAP.
        */
        IMPORT_C TBool ProvFileContainsIapL();
        
        /**
        * ProvFileL, Handles OMA provisioning messages handling without IAP.
        * @since Series 60 3.0
        */
        IMPORT_C void ProvFileL();

        /**
        * ProvFileL, Handles OMA provisioning messages handling with IAP.
        * @since Series 60 3.0
        * @param aIapId Attach IAP to OMA message.
        */
        IMPORT_C void ProvFileL( TUint32& aIapId );

    private:
    
        /**
        * C++ default constructor.
        */
        CWPProvisioningFile( TFileName aFileName );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        /**
        * InitMsgL, Read-Get-Import-Populate the provisioning document.
        * @since Series 60 3.0
        */
        void InitMsgL();
        
        /**
        * SaveDocL, Saves the provisioning document.
        * @since Series 60 3.0
        */
        void SaveDocL();
        
        /**
        * DoProvL, Do provisioning.
        * @since Series 60 3.0
        */
        void DoProvL();
        
    private:
    
        HBufC8*                         iProvisioningDoc;
        
        CWPOMAMsgHandler*               iMsgHandler;
        
        CWPProtocolProfileHandlerSIP*   iSipHandler;
        
        CWPEngine*                      iProvEngine;
        
        CRepository*                    iCenRep;
        
        TBool                           iIapAllowed;
        
        TBool                           iInitDone;
        
        TUint32                         iIapId;
        
        TFileName                       iFileName;

    };

#endif // WPPROVISIONINGFILE_H   
             
// End of File
