/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CSC Application
*
*/


#include <aknapp.h>
#include <AknDoc.h>
#include <eikstart.h>

#include "cscdocument.h"
#include "cscconstants.h"
#include "cscapplication.h"


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// Returns the uid of the app.
// ---------------------------------------------------------------------------
//
TUid CCSCApplication::AppDllUid() const
    {
    return KUidCSCApp;
    }


// ---------------------------------------------------------------------------
// Creates an instance of CCSCDocument and returns it.
// ---------------------------------------------------------------------------
//
CApaDocument* CCSCApplication::CreateDocumentL()
    {  
    return CCSCDocument::NewL( *this ); 
    }


// ======== GLOBAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructs and returns an application object.
// ---------------------------------------------------------------------------
//
LOCAL_C CApaApplication* NewApplication()
    {
    return new CCSCApplication;
    }


// ---------------------------------------------------------------------------
// Main function of the application executable.
// ---------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication( NewApplication );
    }
    
