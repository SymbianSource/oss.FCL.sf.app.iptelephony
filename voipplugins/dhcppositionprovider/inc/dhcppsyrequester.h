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
* Description:  Header file DHCP PSY requester class.
*
*/



#ifndef DHCPPSYREQUESTER_H
#define DHCPPSYREQUESTER_H

//  INCLUDES
#include <e32base.h>
#include <lbspositioninfo.h>

#include "dhcppsyrequesterprogressobserver.h"

// FORWARD DECLARATIONS
class CDhcpPsyPositioner;
class CPositioner;
class CDhcpWlanQueryHandler;

// CLASS DECLARATION


/**
*  This class implements the position requester.
*
*  Position requester is responsible for loading appropriate PSY vi ECom Plug-in
*  and forward location request to the loaded PSY.
*
*  This class may owns an instance of CPositioner.
*
*  @lib dhcppositionprovider.dll
*  @since 3.2
*/
class CDhcpPsyRequester : public CBase, MDhcpPsyRequesterProgressObserver
    {
    public:  // Constructors and destructor

        /**
        * Two - phased constructor.
        *
        */
        static CDhcpPsyRequester* NewL ();

        /**
        * Destructor.
        */
        virtual ~CDhcpPsyRequester();

    public:

        /**
        * Notify position update
        * @param aPosInfo Reference to TPositionerInfoBase, used to store
        *                 fix information.
        * @param aStatus  Reference to TRequestStatus, used to complete location
        *                 request.
        * @since 3.2
        */
        void NotifyPositionUpdate ( TPositionInfoBase& aPosInfo,
            TRequestStatus& aStatus );

        /**
         * Cancel ongoing location query.
         */
        void CancelRequest ();

    protected:

        void  ProgressPsyRequestNotificationL (TRequestProgressPhases aPhase,
            TInt aPhaseValue);

    private:

        enum TDhcpRequesterStates
            {
            EDhpWlanIdle,
            EDchpQueryInitalising,
            EDhcpLocationQueryInProgress
            };

    private:

        /**
        * C++default constructor.
        */
        CDhcpPsyRequester ();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Make location request to the first PSY
        */
        void MakeLocationRequest();

        /**
        * Complete location request
        */
        void CompleteRequest( TInt aErr );

        /**
        * Free all reserved memory
        */
        void Flush();

    private:    // Methods

        /**
        *
        * Fetch position generic data from input to make location query
        * to the wlan network.
        *
        * @param aPosInfo Reference to TPositionerInfoBase, used to store
        *                 fix information.
        *
        * @return TInt  Status code.
        *
        * @since 3.2
        */
        TInt FetchPositioningGenericData  ( TPositionInfoBase& aPosInfo );

    private:    // Data
        // States
        TDhcpRequesterStates    iQueryState;

        // Internet access point id
        TUint32 iIAPProfileId;

        //
        CDhcpWlanQueryHandler* iWlanDhcpQuery;

        // Pointer to position info
        TPositionInfoBase* iPositionInfo;

        // Status to notify the LF
        TRequestStatus* iRequestStatus;

        // To hold location data until module deleted or
        // new request is made.
        HBufC8* iLocationData;

        //
        TBool iCancelled;

    };

#endif      // DHCPPSYREQUESTER_H

// End of File
