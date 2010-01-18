/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/


// INCLUDE FILES
#include <e32std.h>
#include <lbspositioninfo.h>        // TPositionInfoBase
#include <lbs/epos_cpositioner.h>       // CPositioner

#include <dhcppsy.h>
#include "dhcpwlanqueryhandler.h"
#include "dhcppsylogging.h"
#include "dhcppsypanic.h"
#include "dhcppsy.hrh"
#include "dhcppsypositioner.h"
#include "dhcppsyrequester.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDhcpPsyRequester::CDhcpPsyRequester
// C++default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDhcpPsyRequester::CDhcpPsyRequester ()
    {
    TRACESTRING( "CDhcpPsyRequester::CDhcpPsyRequester" )
    }


// -----------------------------------------------------------------------------
// CDhcpPsyRequester::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDhcpPsyRequester::ConstructL()
    {
    TRACESTRING( "CDhcpPsyRequester::ConstructL start" )
    iWlanDhcpQuery = CDhcpWlanQueryHandler::NewL(this);
    TRACESTRING( "CDhcpPsyRequester::ConstructL end" )
    }


// -----------------------------------------------------------------------------
// CDhcpPsyRequester::NewL
// Two - phased constructor.
// -----------------------------------------------------------------------------
//
CDhcpPsyRequester* CDhcpPsyRequester::NewL()
    {
    TRACESTRING( "CDhcpPsyRequester::NewL" )
    CDhcpPsyRequester* self = new(ELeave) CDhcpPsyRequester ();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CDhcpPsyRequester::~CDhcpPsyRequester
//
// -----------------------------------------------------------------------------
//
CDhcpPsyRequester::~CDhcpPsyRequester()
    {
    TRACESTRING( "CDhcpPsyRequester::~CDhcpPsyRequester" )
    Flush();
    delete iLocationData;
    TRACESTRING( "CDhcpPsyRequester::~CDhcpPsyRequester,destructed" )
    }


// -----------------------------------------------------------------------------
// CDhcpPsyRequester::NotifyPositionUpdate
//
// -----------------------------------------------------------------------------
//
void CDhcpPsyRequester::NotifyPositionUpdate (TPositionInfoBase& aPosInfo,
    TRequestStatus& aStatus )
    {
    TRACESTRING( "CDhcpPsyRequester::NotifyPositionUpdate start" )
    aStatus = KRequestPending;
    iRequestStatus = &aStatus;
    TInt result=FetchPositioningGenericData (aPosInfo);
    if (result==KErrNone)
        {
        // Start the location query
        TRAPD (err, iWlanDhcpQuery->MakeLocationQueryL (iIAPProfileId)) ;
        result = err;
        }
    if (result!=KErrNone)
        {
        CompleteRequest (result);
        }
    TRACESTRING( "CDhcpPsyRequester::NotifyPositionUpdate" )
    }

// -----------------------------------------------------------------------------
// CDhcpPsyRequester::FetchPositioningGenericData
//
// -----------------------------------------------------------------------------
//
TInt CDhcpPsyRequester::FetchPositioningGenericData (TPositionInfoBase& aPosInfo)
    {
    TInt result=KErrArgument;
    TRACESTRING( "CDhcpPsyRequester::FetchPositioningGenericData" )
    iPositionInfo = &aPosInfo;
    if (aPosInfo.PositionClassType () & EPositionGenericInfoClass)
        {
        HPositionGenericInfo* posInfo =
            static_cast<HPositionGenericInfo*>(iPositionInfo);
        if ( posInfo->IsRequestedField(EDhcpPsyIAPProfileIdField))
            {
            result = posInfo->GetValue (EDhcpPsyIAPProfileIdField, iIAPProfileId);
            TRACESTRING2( "CDhcpPsyRequester::FetchPositioningGenericData, IAP id : %d ", iIAPProfileId );
            }
        }
    TRACESTRING2( "CDhcpPsyRequester::FetchPositioningGenericData, result : %d ", result );
    return result;
    }

// -----------------------------------------------------------------------------
// CDhcpPsyRequester::ProgressPsyRequestNotificationL
// -----------------------------------------------------------------------------
//
void CDhcpPsyRequester::ProgressPsyRequestNotificationL(TRequestProgressPhases aPhase,
    TInt aPhaseValue)
    {
    TRACESTRING2( "CDhcpPsyRequester::ProgressPsyRequestNotificationL, %d",aPhase);
    TRACESTRING2( "CDhcpPsyRequester::ProgressPsyRequestNotificationL, %d",aPhaseValue);
    switch ( aPhase )
        {
        case MDhcpPsyRequesterProgressObserver::EConnectionInitiated:
            {
            // Nothing yet
            }
            break;
        case MDhcpPsyRequesterProgressObserver::ELocationQueryStarted:
            {
            // Location query started
            }
            break;
        case MDhcpPsyRequesterProgressObserver::ELocationQueryFinished:
            {
            // Location query finished, without problem or not.
            if(aPhaseValue==KErrNone&&!iCancelled)
                {
                HPositionGenericInfo* posInfo =
                    static_cast<HPositionGenericInfo*>(iPositionInfo);
                posInfo->SetRequestedField(EDhcpPsyLocationResultDataField);
                if(iLocationData)
                    {
                    delete iLocationData;
                    iLocationData = NULL;
                    }

                TPtrC8 ptr(*iWlanDhcpQuery->LocationData());
                iLocationData = ptr.AllocL();
                ptr.Set(*iLocationData);
                posInfo->SetValue (EDhcpPsyLocationResultDataField, ptr );
                }
            CompleteRequest (aPhaseValue);
            }
            break;
        case MDhcpPsyRequesterProgressObserver::ECancelled:
            {
            // Nothing yet
            }
            break;
       default:
            {
            TRACESTRING( "CDhcpPsyRequester::ProgressPsyRequestNotificationL, Not supported" )
            User::Leave(KErrNotSupported);
            }
            break;
        }
    }

// -----------------------------------------------------------------------------
// CDhcpPsyRequester::CancelRequest
// -----------------------------------------------------------------------------
//
void CDhcpPsyRequester::CancelRequest ()
    {
    TRACESTRING( "CDhcpPsyRequester::CancelRequest" )    //
    if (iWlanDhcpQuery)
        {
        iWlanDhcpQuery->Cancel();
        }

    if (iRequestStatus)
        {
        User::RequestComplete ( iRequestStatus, KErrNone );
        }

    iCancelled = ETrue;
    TRACESTRING( "CDhcpPsyRequester::CancelRequest" )    //
    }

// -----------------------------------------------------------------------------
// CDhcpPsyRequester::Flush
// -----------------------------------------------------------------------------
//
void CDhcpPsyRequester::Flush()
    {
    TRACESTRING( "CDhcpPsyRequester::Flush" )
    delete iWlanDhcpQuery;
    iWlanDhcpQuery = NULL;
    }

// -----------------------------------------------------------------------------
// CDhcpPsyRequester::CompleteRequest
// -----------------------------------------------------------------------------
//
void CDhcpPsyRequester::CompleteRequest(TInt aStatusCode)
    {
    TRACESTRING( "CDhcpPsyRequester::CompleteRequest" )    //
    //Set UID before complete the request
    iPositionInfo->SetModuleId( TUid::Uid( KDhcpPsyImplUid ) );
    User::RequestComplete( iRequestStatus, aStatusCode );
    TRACESTRING( "CDhcpPsyRequester::CompleteRequest, request complete" )
    }

//  End of File
