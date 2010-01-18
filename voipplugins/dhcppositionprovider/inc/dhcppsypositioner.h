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



#ifndef DHCPPSYPOSITIONER_H
#define DHCPPSYPOSITIONER_H

//  INCLUDES
#include <e32base.h>
#include <lbs/epos_cpositioner.h> // CPositioner

// FORWARD DECLARATIONS
class CDhcpPsySingletonHandler;
class MPositionerStatus;

// CLASS DECLARATION

/**
*  This class implements Positioning Plug-in API for DHCP PSY
*
*  This class provides the interface to Location Framework as specified by
*  Positioning Plug-in API. Each client connection to DHCP PSY makes an
*  instance of this class.
*
*  This class owns a instance of CNppPsyRequester if there is location
*  request received.
*
*  @lib dhcppositionprovider.dll
*  @since 3.2
*/
class CDhcpPsyPositioner : public CPositioner
    {
    public:  // Constructors and destructor

        /**
        * Two - phased constructor.
        */
        static CDhcpPsyPositioner* NewL( TAny* aConstructionParameters );

        /**
        * Destructor.
        */
        virtual ~CDhcpPsyPositioner();

        /**
        * Get status report interface
        * @return Reference to status report interface
        */
        MPositionerStatus& StatusReportInterface();

    public: // Functions from base classes

        /**
        * From CPositioner See Positioning Plug-in API
        * @since 3.2
        */
        void NotifyPositionUpdate  ( TPositionInfoBase& aPosInfo,
            TRequestStatus&    aStatus );

        /**
        * From CPositioner See Positioning Plug-in API.
        * @since 3.2
        */
        void CancelNotifyPositionUpdate();

    private:

        /**
        * C++default constructor.
        */
        CDhcpPsyPositioner();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( TAny* aConstructionParameters );

    private:    // Data

        CDhcpPsySingletonHandler* iDhcpRequestManager;

    };

#endif      // DHCPPSYPOSITIONER_H

// End of File
