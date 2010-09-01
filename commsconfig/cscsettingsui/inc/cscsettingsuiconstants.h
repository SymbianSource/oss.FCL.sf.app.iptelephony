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
* Description:  Constants for CSCSettingsUi
*
*/


#ifndef CSCSETTINGSUICONSTANTS_H
#define CSCSETTINGSUICONSTANTS_H

// View Ids.
const TUid KCSCSettingsUiMainViewId        = { 1928 }; // main view

// Resource file location.
_LIT( KCSCSettingsUiResourceFile, "cscsettingsui.rsc" );  // file

// Default IM message ton
_LIT( KDefaultTone, "Z:\\Data\\Sounds\\Digital\\Message 2.aac" );


// Lengths & sizes.
const TInt KCSCSettingsUiItemLength = 128;               // setting item
const TInt KCSCSettingsUiArrayGranularity = 5;          // granularity
const TInt KCSCSettingsUiBrandingIdLength = 512;        // branding id          
const TInt KCSCMaxImToneLength = 512;                   // im tone


#endif  // CSCSETTINGSUICONSTANTS_H
