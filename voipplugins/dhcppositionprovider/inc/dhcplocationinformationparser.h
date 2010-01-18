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


#ifndef DHCPLOCATIONINFORMATIONPARSER_H
#define DHCPLOCATIONINFORMATIONPARSER_H

#include <e32base.h>

/**
 *  Parses location information from DHCP message to LCI
 *
 *  @lib sspcontroller
 *  @since S60 3,0
 */
class TDhcpLocationInformationParser
    {
    public:

        /** Location information parsing type */
        enum TParsingType
            {
            EDHCPCoordinates,
            EDHCPCivicAddress
            };

    public:

        /**
         * Constructor
        */
        TDhcpLocationInformationParser();

        /**
         * Parses location information
         *
         * @since S60 3.2
         * @param aInput Input data
         * @param aParsingType Parsing type
         * @param aOutput Result
         * @return None
         */
        void ParseLocationInformation( const TDesC8& aInput,
                const TParsingType aParsingType, TDes8& aOutput );

    private:

        /**
         * Parses coordinate parameters from the input data
         *
         * @since S60 3.2
         * @param aInput Input data
         * @param aOutput Result
         * @return None
         */
        void ParseLCI( const TDesC8& aInput, TDes8& aOutput );

        /**
         * Parses datum from the input data
         *
         * @since S60 3.2
         * @param aInput Input data
         * @param aOutput Result
         * @return Error code
         */
        TInt ParseDatum(const TDesC8& aInput, TDes8& aOutput );

        /**
         * Parses the input to the DMS coordinate form
         *
         * @since S60 3.2
         * @param aIsLatitude latitude if ETrue, otherwise longitude
         * @param aResolution Resolution, i.e. the amount of significant bits
         * @param aInteger Integer part of coordinate, or degree
         * @param aFraction Fraction part of coordinate
         * @param aOutput Result
         * @return None
         */
        void ParseDMS(TBool aIsLatitude, TUint8 aResolution,
            TUint16 aInteger, TUint32 aFraction, TDes8& aOutput  );

        /**
         * Parses the civic address elements into the XML description
         *
         * @since S60 3.2
         * @param aInput Input data
         * @param aOutput Result
         * @return None
         */
        void ParseCivicAddressElements( const TDesC8& aInput, TDes8& aOutput );

        /**
         * Extract a single civic address element from the input
         *
         * @since S60 3.2
         * @param aInput Input data
         * @param aOutput Result
         * @return None
         */
        void ExtractCivicAddressElement( TLex8& aInput, TDes8& aOutput );

        /**
         * Parses the timestamp
         *
         * @since S60 3.2
         * @param aTimeStamp Resulting timestamp
         * @return None
         */
        void ParseTimeStamp( TDes8& aTimeStamp );

        /**
         * Parses the retenrion-expiry
         *
         * @since S60 3.2
         * @param aTimeStamp Resulting timestamp
         * @return None
         */
        void ParseRetentionExpiry( TDes8& aTimeStamp );

    private: // data

        /**
         * Timestamp
         */
        TTime iTimeStamp;

    };

#endif // DHCPLOCATIONINFORMATIONPARSER_H
