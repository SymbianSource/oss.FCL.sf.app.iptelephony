/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Application implementation.
*
*/


// INCLUDE FILES
#include <eikstart.h>
#include "VoIPProvisioningApp.h"
#include "VoIPProvisioningDocument.h"

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CVoIPProvisioningApp::AppDllUid()
// Returns application UID
// ---------------------------------------------------------------------------

TUid CVoIPProvisioningApp::AppDllUid() const
    {
    return KUidVoIPProvisioningAppUid;
    }

// ---------------------------------------------------------------------------
// CVoIPProvisioningApp::CreateDocumentL()
// Creates CVoIPProvisioningDocument object
// ---------------------------------------------------------------------------

CApaDocument* CVoIPProvisioningApp::CreateDocumentL()
    {
    return new (ELeave) CVoIPProvisioningDocument( *this );
    }

// ===================== OTHER EXPORTED FUNCTIONS ============================

LOCAL_C CApaApplication* NewApplication()
    {
    return new CVoIPProvisioningApp;
    }

GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication( NewApplication );
    }


// End of File

