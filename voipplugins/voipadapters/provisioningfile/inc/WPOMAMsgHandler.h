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
* Description:  Used by ProvisioningFile to read provisioning messages.
*
*/



#ifndef CWPOMAMSGHANDLER_H
#define CWPOMAMSGHANDLER_H

// INCLUDES
#include    "wpprovisioningfileconst.h"

#include    <e32base.h>
#include    <f32file.h>

// CLASS DECLARATION

/**
*  CWPOMAMsgHandler used te read OMA messages.
*
*  @lib ProvisioningFile.lib
*  @since Series 60 3.0
*/    
NONSHARABLE_CLASS(CWPOMAMsgHandler) : public CBase
    {
    
#ifdef PROVISIONINGFILE_TEST
    
    friend class UT_OmaMsgHandler;
    
#endif
    
    public:  // Constructors and destructor
       
        /**
        * Two-phased constructor.
        */
        static CWPOMAMsgHandler* NewL();

        /**
        * Two-phased constructor.
        */
        static CWPOMAMsgHandler* NewLC();
        
        /**
        * Destructor.
        */
        virtual ~CWPOMAMsgHandler();

    public: 
        
        /**
        * ReadProvFileL, Reads provisioning file.
        * @since Series 60 3.0
        * @param aConfig configuration path.
        */
        void ReadProvFileL( TFileName aConfig );

        /**
        * DocL, Fetches the read provisioning document.
        * @since Series 60 3.0
        * @return pointer to a copy of the prov doc
        */
        HBufC8* DocL();

    private:
    
        /**
        * GetFileSize, Gets the size of the provisioning document.
        * @since Series 60 3.0
        * @param aSize Messages size.
        * @return TInt system-wide error code.
        */
        TInt GetFileSize( TInt& aSize );
       
    private:

        /**
        * C++ default constructor.
        */
        CWPOMAMsgHandler();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data

        HBufC8*             iProvisioningDoc;

        TPtr8               iProvData;
    
        RFs                 iFs;
        
        RFile               iFile;
        
        TFileName           iFileName;
    
    };

#endif // CWPOMAMSGHANDLER_H   
            
// End of File
