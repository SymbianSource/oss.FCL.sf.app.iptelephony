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
* Description:  Application definition.
*
*/


#ifndef CVOIPPROVISIONINGAPP_H
#define CVOIPPROVISIONINGAPP_H

#ifdef _DEBUG
#define EUNIT_FRIEND_CLASS( x ) friend class x;
#else
#define EUNIT_FRIEND_CLASS( x )
#endif

// INCLUDES
#include <aknapp.h>

// CONSTANTS
const TUid KUidVoIPProvisioningAppUid = { 0x20026FE3 };

// CLASS DECLARATION
/**
*	Application class definition
*/
class CVoIPProvisioningApp : public CAknApplication
    {
    private: // from CApaApplication
        /**
        * Create CVoIPProvisioningDocument document object.
        */
        CApaDocument* CreateDocumentL();

        /**
        * Return CVoIPProvisioningApp uid.
        */
        TUid AppDllUid() const;
        
        EUNIT_FRIEND_CLASS( UT_CVoIPProvisioningApp )
    };

#endif // CVOIPPROVISIONINGAPP_H
            
// End of File

