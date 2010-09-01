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
* Description:  Handles provisioning document.
*
*/


// INCLUDE FILES
#include    "WPProvisioningFile.h"
#include    "WPOMAMsgHandler.h"
#include    "WPProtocolProfileHandlerSIP.h"
#include    "wpprovisioningfileconst.h"
#include    "wpprovisioningfiledebug.h"

#include    <CWPEngine.h>
#include    <centralrepository.h>
#include    <settingsinternalcrkeys.h>
// CONSTANTS
_LIT( KPathLetter, "E:" );
_LIT( KFilePath, "\\data\\settings.wbxml" );

// ============================ LOCAL FUNCTIONS ================================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWPProvisioningFile::CWPProvisioningFile
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWPProvisioningFile::CWPProvisioningFile( TFileName aFileName )
    : iIapAllowed( EFalse ),
    iInitDone( EFalse ),
    iFileName( aFileName )
    {
    }

// -----------------------------------------------------------------------------
// CWPProvisioningFile::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWPProvisioningFile::ConstructL()
    {
    FLOG( "[ProvisioningFile] CWPProvisioningFile::ConstructL" );
    
    iProvEngine = CWPEngine::NewL();
    
    iMsgHandler = CWPOMAMsgHandler::NewL();
    }

// -----------------------------------------------------------------------------
// CWPProvisioningFile::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CWPProvisioningFile* CWPProvisioningFile::NewL( TFileName aFileName )
    {
    CWPProvisioningFile* self = CWPProvisioningFile::NewLC( aFileName );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CWPProvisioningFile::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWPProvisioningFile* CWPProvisioningFile::NewLC( TFileName aFileName )
    {
    CWPProvisioningFile* self = new( ELeave ) CWPProvisioningFile( aFileName );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CWPProvisioningFile::Destructor
// 
// -----------------------------------------------------------------------------
//
CWPProvisioningFile::~CWPProvisioningFile()
    {
    delete iProvisioningDoc;
    if ( iSipHandler )
        {
        delete iSipHandler;
        iSipHandler = NULL;
        }
    delete iMsgHandler;
    delete iProvEngine;
    delete iCenRep;
    }

// -----------------------------------------------------------------------------
// CWPProvisioningFile::ProvFileContainsIapL
//
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CWPProvisioningFile::ProvFileContainsIapL()
    {
    FLOG( "[ProvisioningFile] CWPProvisioningFile::ProvFileContainsIapL" );
    
    const TDesC16* settingsName;
    TBool iapExist( EFalse );
    
    if ( !iInitDone )
        {
        InitMsgL();
        }
    
    // Check is there IAP
    for ( TInt i = 0; i < iProvEngine->ItemCount(); i++ )
        {
        // Next settings
        settingsName = &iProvEngine->SummaryTitle( i );
        
        // Is settings IAP
        if ( !settingsName->Compare( KProvFileIapSetting ) )
            {
            // Message contains IAP settings
            iapExist = ETrue;
            }
        }
        
    return iapExist;        
    }
    
// -----------------------------------------------------------------------------
// CWPProvisioningFile::ProvFileL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CWPProvisioningFile::ProvFileL()
    {
    FLOG( "[ProvisioningFile] CWPProvisioningFile::ProvFileL" );
    
    if( iMsgHandler )
        {
        // Do the provisioning
        DoProvL();
        }
    }
    
// -----------------------------------------------------------------------------
// CWPProvisioningFile::ProvFileL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CWPProvisioningFile::ProvFileL( TUint32& aIapId )
    {
    FLOG( "[ProvisioningFile] CWPProvisioningFile::ProvFileL IAP" ); 
    
    if( iMsgHandler )
        {
        // Is offered IAP allowed
        iIapAllowed = ETrue;
        
        iIapId = aIapId;
                    
        // Do the provisioning
        DoProvL();
        }
    
    iIapAllowed = EFalse;        
    }    

// -----------------------------------------------------------------------------
// CWPProvisioningFile::DoProvL
//
// -----------------------------------------------------------------------------
//
void CWPProvisioningFile::DoProvL()
    {
    FLOG( "[ProvisioningFile] CWPProvisioningFile::DoProvL" ); 
    
    // Is init done
    if ( !iInitDone )
        {
        InitMsgL();
        }
    
    // Save OMA message
    SaveDocL();
    
    // If new IAP is allowed
    if( iIapAllowed )
        {
        for ( TInt i = 0; i < iSipHandler->ProvisionedCount(); i++ )
            {
            iSipHandler->SetProfilesIapL( i, iIapId );
            }
        }
    }

// -----------------------------------------------------------------------------
// CWPProvisioningFile::InitMsgL
//
// -----------------------------------------------------------------------------
//
void CWPProvisioningFile::InitMsgL()
    {
    FLOG( "[ProvisioningFile] CWPProvisioningFile::InitMsgL" );
    
    FLOG2( "[ProvisioningFile] *** CWPProvisioningFile::InitMsgL %S", &iFileName );
    if ( KErrNone == iFileName.Compare( KDefaultFilePath ) )
        {
        iFileName.Zero();
        iFileName.Append( KPathLetter );
        iFileName.Append( KFilePath );
        }
    // Read OMA message
    iMsgHandler->ReadProvFileL( iFileName );
    
    // Get OMA message
    if ( iProvisioningDoc )
        {
        delete iProvisioningDoc;
        iProvisioningDoc = NULL;
        }
    iProvisioningDoc = iMsgHandler->DocL();

    // Import the OMA message
    iProvEngine->ImportDocumentL( iProvisioningDoc->Des() );

    // Populate OMA message to Adapters
    iProvEngine->PopulateL();
    
    iInitDone = ETrue;
    FLOG( "[ProvisioningFile] CWPProvisioningFile::InitMsgL out" );
    }
    
// -----------------------------------------------------------------------------
// CWPProvisioningFile::SaveDocL
//
// -----------------------------------------------------------------------------
//
void CWPProvisioningFile::SaveDocL()
    {
    FLOG( "[ProvisioningFile] CWPProvisioningFile::SaveDocL" );
    
    const TDesC16* settingsName( NULL );
    
    // SIP Profile handler
    if ( iSipHandler )
        {
        delete iSipHandler;
        iSipHandler = NULL;
        }
    iSipHandler = CWPProtocolProfileHandlerSIP::NewL();
    
    // Message contains SIP information
    TBool sipEnable( EFalse );
    
    // Save OMA message
    for ( TInt i = 0; i < iProvEngine->ItemCount(); i++ )
        {
        // Next settings
        settingsName = &iProvEngine->SummaryTitle( i );
        
        // Ongoing setting
        if ( !settingsName->Compare( KProvFileIapSetting ) )
            {
            // Message contains IAP settings, which priority is higher than given IAPs
            iIapAllowed = EFalse;
            }
        else if ( !settingsName->Compare( KProvFileSipSetting ) )
            {
            // There is SIP setting
            sipEnable = ETrue;
            }
        
        iProvEngine->SaveL( i );
        }
    
    // If there was new SIP Profiles and new IAP is allowed, save provisioned SIP Ids
    if ( sipEnable && iIapAllowed )
        {
        // Lets save provisioned SIP Id
        iSipHandler->SetProvisioningIdL();
        }
    
    }

//  End of File  
