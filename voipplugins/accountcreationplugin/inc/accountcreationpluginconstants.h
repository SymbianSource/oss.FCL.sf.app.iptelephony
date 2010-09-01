/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Constants for accountcreationplugin
*
*/


#ifndef ACCOUNTCREATIONPLUGINCONSTANTS_H
#define ACCOUNTCREATIONPLUGINCONSTANTS_H

#include "accountcreationpluginuids.hrh"

// UID of this plugin.
const TUid KAccountCreationPluginUid = { KAccountCreationPluginDllUid };
// View IDs
const TUid KProviderListViewId       = { 7520 };
const TUid KProviderSpecificViewId   = { 1321 };
const TInt KArrayGranuality          = 5;
const TInt KMaxSettingLength         = 64;
const TInt KMaxUrlLength             = 256;

// Name of the resource file.
_LIT( KResourceFilename, "accountcreationplugin.rsc" );
_LIT( KListFormat,       "%d\t%S\t\t" ); // Format of setting item.
_LIT( KIconFileName,     "acpicons.mif" ); // Filename for ACP icons.
_LIT( KEmpty,            "" );
_LIT( KAcpSettingItemTextFormat, "\t%S\t\t%S" ); // Setting item text format.

const TUid KPhoneBookTabUid = { 0x20012423 };

#endif  // ACCOUNTCREATIONPLUGINCONSTANTS_H

// End of file.
