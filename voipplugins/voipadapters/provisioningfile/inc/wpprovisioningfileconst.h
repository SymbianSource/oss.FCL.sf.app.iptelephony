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
* Description:  ProvisioningFile common header.
*
*/


#ifndef WPPROVISIONINGFILECONST_H
#define WPPROVISIONINGFILECONST_H

//  INCLUDES
#include <e32base.h>

// CONSTANTS
const TUid KProvisioningFileUid = { 0x10275409 };

_LIT( KProvFileIapSetting, "Access points:" );
_LIT( KProvFileSipSetting, "SIP settings" );
_LIT( KProvFileSccpSettings, "SCCP settings" );

/** File path configuration. No real path used 
    here because code scanner error */
_LIT( KDefaultFilePath, "defaultpath" );

enum TProtocolType
    {
    EProtocolNone    = -1,
    EProtocolSIP     = 0,
    EProtocolSCCP    = 1,
    };

#endif // WPPROVISIONINGFILECONST_H   
            
// End of File
