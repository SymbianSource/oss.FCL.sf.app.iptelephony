/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Private Central Repository definitions of the
*                iptelephony's cch subsystem
*
*/


#ifndef CCHPRIVATECRKEYS_H
#define CCHPRIVATECRKEYS_H


const TUid KCRUidCch = { 0x10282CE5 }; 

/**
* Indicates if WLAN VoIP warning text, that comes when it is not possible to
* establish and/or get emergency call through when user resides inside WLAN VoIP area,
* should be shown. Target is to show this warning to the user only once by SysAp.
* Possible values: 1 = warning already shown, 0 = warning not shown yet.
*/
const TUint32 KCCHVoIPEmergencyWarningShown = 0x00000001;
enum TCCHVoIPEmergencyWarningShown
    {
    ECCHVoIPEmergencyWarningNotYetShown = 0,
    ECCHVoIPEmergencyWarningAlreadyShown = 1
    };

/**
* Indicates if WLAN VoIP warning should be shown at all 
* Possible values: 1 = Warning should be shown, 0 = Warning should not be shown.
*/
const TUint32 KCCHVoIPShowEmergencyWarningOnOff = 0x00000002;
enum TCCHVoIPEmergencyWarningOnOff
    {
    ECCHVoIPEmergencyWarningOnOffDoNotShowWarning = 0,
    ECCHVoIPEmergencyWarningOnOffShowWarning = 1
    };

/**
* Indicates if WLAN scan was activated before service enable
* Possible values: 0 = not defined, 1 = was enabled, 2 = was not enabled
*/
const TUint32 KCCHWasWlanScanActivatedBeforeServiceEnabling = 0x00000003;
enum TCCHWasWlanScanActivatedBeforeServiceEnabling
    {
    ECCHWlanScanNotDefined = 0,
    ECCHWlanScanWasEnabled = 1,
    ECCHWlanScanWasNotEnabled = 2
    };

/**
* Indicates count of CCH server startups, if startup flag is set to ON
*/
const TUint32 KCCHStartupCounter = 0x00000004;


/**
* Indicates if gprs roaming cost warning text, that comes when gprs connection
* is tried to use first time, should be shown. Target is to show this warning
* to the user only once by Cch. 1 = warning already shown, 0 = warning not shown yet.
*/
const TUint32 KCCHGprsRoamingCostWarningShown = 0x00000005;
enum TCCHGprsRoamingCostWarningShown
    {
    ECCHGprsRoamingCostWarningNotYetShown = 0,
    ECCHGprsRoamingCostWarningAlreadyShown = 1
    };

#endif      // CCHPRIVATECRKEYS_H

// End of File

