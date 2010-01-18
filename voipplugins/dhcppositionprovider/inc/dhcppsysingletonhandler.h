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


#ifndef DHCPPSYSINGLETONHANDLER_H
#define DHCPPSYSINGLETONHANDLER_H

#include <e32base.h>

class CDhcpPsyRequester;

// Implements the singleton handler for the dhcpositionprovider module
class CDhcpPsySingletonHandler : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Singleton can be only created through GetInstanceL.
        */
        static CDhcpPsySingletonHandler* GetInstanceL();

        /**
        * Destructor
        */
        virtual ~CDhcpPsySingletonHandler ();

        /**
         * Release the reference to singleton handler. If there is no ojbect
         * has the reference of this object, it will delete itself.
         */
         void ReleaseInstance();

    public:

        /**
         * Requester for positionar usage.
         *
         */
         CDhcpPsyRequester* Requester ();

    private:

        /**
        * Second - phase constructor
        */
        void ConstructL ();

    private:
        // Reference count
        TInt iRefCount;

        //
        CDhcpPsyRequester*  iPsyRequester;

    };

#endif /* DHCPPSYSINGLETONHANDLER_H */
