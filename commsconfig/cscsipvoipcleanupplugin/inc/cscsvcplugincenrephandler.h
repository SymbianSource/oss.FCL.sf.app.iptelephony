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
* Description:  
*
*/


#ifndef C_CSCSVCPLUGINCENREPHANDLER_H
#define C_CSCSVCPLUGINCENREPHANDLER_H


class CRepository;


/**
 *  An instance of class CCSCSvcPluginCenrepHandler.
 *
 *  @lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS (CCSCSvcPluginCenrepHandler) : public CBase
    {
    public:

        static CCSCSvcPluginCenrepHandler* NewL();

				/**
         * Destructor.
         */
        ~CCSCSvcPluginCenrepHandler();


        /**
         * Destroys IAP spesific NAT/FW settings based on IAP id.
         * Function leaves on failure.
         *
         * @since S60 v3.2
         * @param aIapId for IAP id
         */
        void RemoveIapSpecificNatfwSettingsL( TInt aIapId );


        /**
         * Destroys Domain spesific NAT/FW settings based on domain name.
         * Function leaves on failure.
         *
         * @since S60 v3.2
         * @param aIapId for IAP id
         */
        void RemoveDomainSpecificNatfwSettingsL( const TDesC8& aDomainName );


    private:
                
        CCSCSvcPluginCenrepHandler();

        void ConstructL();

    
    private: // data
            
        /*
         * Handle to NAT/FW repository.
         * Own.
         */
        CRepository* iNatfwRepository;      
    };

#endif  // C_CSCSVCPLUGINCENREPHANDLER_H


