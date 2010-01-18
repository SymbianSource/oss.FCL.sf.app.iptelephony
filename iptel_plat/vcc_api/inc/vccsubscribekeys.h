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
* Description:   Vcc application IDs for Publish&Subscribe API.
*
*/



#ifndef VCCSUBSCRIBEKEYS_H_
#define VCCSUBSCRIBEKEYS_H_

/**
 * Main PS property category Vcc.
 * @note uid is not valid!!!
 */
const TUid KPSVccPropertyCategory = { 0x10207FD9 };

/**
 * PS property key KVccPropKeyHoRequest for category Vcc.
 * Indicates the state of handover as TVccUiRequest.
 * UI makes HO request by updating this key's value. VCC server component
 * observers it.
 */
 const TUint32 KVccPropKeyHoRequest = 0x00000001;

/**
* PS property key KVccPropKeyHoStatus for category Vcc.
* Indicates the state of handover on the HO server as TVccHoStatus.
* Only VCC server components update this value. Other components may observer it.
*/
const TUint32 KVccPropKeyHoStatus = 0x00000002;

/**
* PS property key KVccPropHoError for category Vcc.
* Indicates the Ho error code.
*/
const TUint32 KVccPropKeyHoError = 0x00000003;

/**
 * The key values used with property KVccPropKeyHoRequest.
 */
enum TVccHoRequest
     {
     EVccNoRequestOngoing = 3000, // Empty state, ok

     EVccManualStartCsToPsHoRequest, // User has requested to start CS->PS HO
     EVccManualCancelCsToPsHoRequest, // User has requested to cancel CS->PS HO

     EVccManualStartPsToCsHoRequest, // User has requested to start PS->CS HO
     EVccManualCancelPsToCsHoRequest, // User has requested to cancel PS->CS HO

     EVccAutomaticStartCsToPsHoRequest, // User has requested to start CS->PS HO
     EVccAutomaticCancelCsToPsHoRequest, // User has requested to cancel CS->PS HO
     EVccAutomaticStartCsToPsHoRequestIfSingleCall, // User has requested to 
                                                    // start CS->PS HO, but ho is 
                                                    // forbidden if there are 
                                                    // held or waiting calls.

     EVccAutomaticStartPsToCsHoRequest, // User has requested to start PS->CS HO
     EVccAutomaticCancelPsToCsHoRequest, // User has requested to cancel PS->CS HO
     EVccAutomaticStartPsToCsHoRequestIfSingleCall, // User has requested to 
                                                         // start PS->CS HO, but ho is 
                                                         // forbidden if there are 
                                                         // held or waiting calls.

     EVccUndefinedRequest   //nok, key value could not be fetched

     };

/**
* The key values used with property KVccPropKeyHoStatus.
*/
enum TVccHoStatus
    {
    EVccHoStateIdle = 4000,     // Everything ok
    EVccCsToPsHoStarted,        // CS->PS HO started
    EVccCsToPsHoInprogress,     // CS->PS HO in progress.
    EVccCsToPsHoSuccessful,     // CS->PS HO ended successfully.
    EVccCsToPsHoFailure,        // HO server encountered failure.

    EVccPsToCsHoStarted,        // PS->CS HO started
    EVccPsToCsHoInprogress,     // PS->CS  HO in progress.
    EVccPsToCsHoSuccessful,     // PS->CS  HO ended successfully.
    EVccPsToCsHoFailure,        // HO server encountered failure.
    EVccHoStateUnknown,         // nok, key value could not be fetched
    EVccHoUnavailable,          // HO cannot be done (no other network etc.)
    EVccCsToPsNotAllowed,       // No ho to wlan
    EVccPsToCsNotAllowed,       // No ho to GSM
    EVccHoStateIdleIfSingleCall, // No ho if multicall situation, if singe call both directions ok
    EVccHoAllowedToCsIfSingleCall, // Ho allowed to CS if not in multicall situation
    EVccHoAllowedToPsIfSingleCall  // Ho allowed to PS if not in multicall situation

    };

#endif /*VCCSUBSCRIBEKEYS_H_*/
