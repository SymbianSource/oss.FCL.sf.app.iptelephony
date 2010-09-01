/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Constants for CSC Application
*
*/


#ifndef CSCCONSTANTS_H
#define CSCCONSTANTS_H

// UID for the csc application
const TUid KUidCSCApp = {0x10275458};

const TUid KPhoneBookTabUid = { 0x20012423 };

// Account creation plugin UID
const TUid KAccountCreationPluginUID = { 0x2000BEF1 };

// Advanced settings plugin UID
const TUid KAdvancedSettingsPluginUID = { 0x1028331D };

// Help context UID
const TUid KCscHelpUid = { 0x1020E566 };

// Filename and path for bitmaps and icons.
_LIT( KDriveZ, "z:" );
_LIT( KCSCAifFile, "csc_aif.mif");

// For icon size
const TInt KItemHeightPortrait = 67;
const TInt KIconSizeCorrectionPortrait = 8;
const TInt KIconSizeCorrectionLandscape = 7;

#endif  // CSCCONSTANTS_H
