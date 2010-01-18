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
* Description:  SIP constants for SVP.
*
*/



#ifndef SVPSIPCONSTS_H
#define SVPSIPCONSTS_H

#include <e32base.h>

// Reason phrases
_LIT8( KSVPIntervalTooSmallReason, "Session Interval Too Small" );
_LIT8( KSVPCallTransactDoesNotExistReason, "Call/Transaction Does Not Exist");
_LIT8( KSVPQueuedReason, "Queued" );
_LIT8( KSVPRequestTerminatedReason, "Request Terminated");
_LIT8( KSVPIncompatibleMediaFormatReason, "Incompatible media format");
_LIT8( KSVPNotAcceptableHereReason, "Not Acceptable Here" );

//Response codes
_LIT8( KSVPNotifyTrying, "SIP/2.0 100 Trying" );
_LIT8( KSVPNotifyRinging, "SIP/2.0 180 Ringing" );
_LIT8( KSVPNotifyOK, "SIP/2.0 200 OK" );
_LIT8( KSVPBusyHereReason, "SIP/2.0 486 Busy Here" );
_LIT8( KSVPNotifyServiceUnavailable, "SIP/2.0 503 Service Unavailable" );


//Response messages
//Provisional 1xx
const TInt KSVPTryingVal                   = 100;
const TInt KSVPRingingVal                  = 180;
const TInt KSVPForwardedVal                = 181;
const TInt KSVPQueuedVal                   = 182;
const TInt KSVPSessionProgressVal          = 183;

//Successful 2xx
const TInt KSVPOKVal                       = 200;
const TInt KSVPAcceptedVal                 = 202;

// Call forwarding 3xx
const TInt KSVPMultipleChoicesVal          = 300;
const TInt KSVPMovedPermanentlyVal         = 301;
const TInt KSVPMovedTemporarilyVal         = 302;
const TInt KSVPUseProxyVal                 = 305;
const TInt KSVPAlternativeServiceVal       = 380;

//Request Failure 4xx
const TInt KSVPBadRequestVal               = 400;
const TInt KSVPUnauthorizedVal             = 401;
const TInt KSVPPaymentRequiredVal          = 402;
const TInt KSVPForbiddenVal                = 403;
const TInt KSVPNotFoundVal                 = 404;
const TInt KSVPMethodNotAllowedVal         = 405;
const TInt KSVP406NotAcceptableVal         = 406;
const TInt KSVPProxyAuthenticationRequiredVal = 407;
const TInt KSVPRequestTimeoutVal           = 408;
const TInt KSVPGoneVal                     = 410;
const TInt KSVPRequestEntityTooLargeVal    = 413;
const TInt KSVPRequestUriTooLongVal        = 414;
const TInt KSVPUnsupportedMediaTypeVal     = 415;
const TInt KSVPUnsupportedUriSchemeVal     = 416;
const TInt KSVPBadExtensionVal             = 420;
const TInt KSVPExtensionRequiredVal        = 421;
const TInt KSVPSessionIntervalTooSmallVal  = 422;
const TInt KSVPIntervalTooBriefVal         = 423;
const TInt KSVPTemporarilyUnavailableVal   = 480;
const TInt KSVPCallDoesNotExistVal         = 481;
const TInt KSVPLoopDetectedVal             = 482;
const TInt KSVPTooManyHopsVal              = 483;
const TInt KSVPAddressIncompleteVal        = 484;
const TInt KSVPAmbiguousVal                = 485;
const TInt KSVPBusyHereVal                 = 486;
const TInt KSVPRequestTerminatedVal        = 487;
const TInt KSVPNotAcceptableHereVal        = 488;
const TInt KSVPBadEventVal                 = 489;
const TInt KSVPRequestPendingVal           = 491;
const TInt KSVPSecurityAgreementRequiredVal = 494;

//Server Failure 5xx
const TInt KSVPServerInternalErrorVal      = 500;
const TInt KSVPNotImplementedVal           = 501;
const TInt KSVPBadGatewayVal               = 502;
const TInt KSVPServiceUnavailableVal       = 503;
const TInt KSVPServerTimeoutVal            = 504;
const TInt KSVPVersionNotSupportedVal      = 505;
const TInt KSVPMessageTooLargeVal          = 513;
const TInt KSVPPreconditionFailureVal      = 580;

//Global Failures 6xx
const TInt KSVPDeclineVal                  = 603;
const TInt KSVPBusyEverywhereVal           = 600;
const TInt KSVPDoesNotExistAnywhereVal     = 604;
const TInt KSVPNotAcceptableVal            = 606;

//WarnCodes of the Response
//Media type not available
const TInt KSVPWarnMediaTypeNotAvailable   = 304;
//Incompatible media format
const TInt KSVPWarnIncompatibleMediaFormat = 305;


#endif // SVPSIPCONSTS_H
