/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Project wide unit testing definitions.
*
*/

#ifndef SVPUTDEFS_H
#define SVPUTDEFS_H

/*
#ifdef IMPORT_C
#undef IMPORT_C
#define IMPORT_C
#endif // IMPORT_C

#ifdef EXPORT_C
#undef EXPORT_C
#define EXPORT_C
#endif // EXPORT_C
*/

//#if (defined _DEBUG || defined DEBUG)
#if (defined TEST_EUNIT)

#define SVP_UT_DEFS \
  friend class UT_SipVoipProviderUsecases;\
  friend class UT_CSVPController;\
  friend class UT_CSVPSessionBase;\
  friend class CSVPSession_stub;\
  friend class UT_CSVPDTMFEventGenerator;\
  friend class UT_CSVPEmergencyConnection;\
  friend class UT_CSVPEmergencyIapProvider;\
  friend class UT_CSVPEmergencySession;\
  friend class UT_CSVPPositioningProvider;\
  friend class UT_CSVPHoldConnectedState;\
  friend class UT_CSVPHoldContext;\
  friend class UT_CSVPHoldController;\
  friend class UT_CSVPHoldMediaHandler;\
  friend class UT_CSVPHoldOutEstablishingState;\
  friend class UT_CSVPHoldInEstablishingState;\
  friend class UT_CSVPHoldDHState;\
  friend class UT_CSVPHoldInState;\
  friend class UT_CSVPHoldOutState;\
  friend class UT_CSVPForwardProvider;\
  friend class UT_CSVPMoSession;\
  friend class UT_CSVPMtSession;\
  friend class UT_CSVPPropertyWatch;\
  friend class UT_CSVPSSLogCall;\
  friend class UT_CSVPSuppServices;\
  friend class UT_CSVPTimer;\
  friend class UT_CSVPTransferController;\
  friend class UT_CSVPTransferStateContext;\
  friend class UT_CSVPTransferAcceptedState;\
  friend class UT_CSVPTransferIdleState;\
  friend class UT_CSVPTransferPendingState;\
  friend class UT_CSVPTransferTerminatedState;\
  friend class UT_CSVPTransferTerminatingState;\
  friend class UT_CSVPUriParser;\
  friend class UT_CSVPVolumeObserver;\
  friend class UT_CSVPUtility;\
  friend class UT_CSVPAudioUtility;

#else // (defined _DEBUG || defined DEBUG)

#define SVP_UT_DEFS

#endif // (defined _DEBUG || defined DEBUG)


#endif // SVPUTDEFS_H

// end of file
