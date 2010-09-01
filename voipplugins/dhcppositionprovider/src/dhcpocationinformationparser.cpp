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
* Description:  Parses location information from DHCP message to LCI
*
*/


#include <e32math.h>
#include "dhcplocationinformationparser.h"
#include "dhcppsylogging.h"

// Dynamic Host Configuration Protocol (DHCPv4 and DHCPv6) Option for Civic
// Addresses Configuration Information
// draft-ietf-geopriv-dhcp-civil-09

// Civic Address CAtypes
const TUint8 KDhcpState = 1;
const TUint8 KDhcpCounty = 2;
const TUint8 KDhcpCity = 3;
const TUint8 KDhcpBorough = 4;
const TUint8 KDhcpBlock = 5;
const TUint8 KDhcpGroupOfStreets = 6;

// Civic Address Additional CAtypes
// const TUint8 KLanguage = 0;
const TUint8 KDhcpLeadingStreetDirection = 16;
const TUint8 KDhcpTrailingStreetDirection = 17;
const TUint8 KDhcpStreetSuffix = 18;
const TUint8 KDhcpHouseNumber = 19;
const TUint8 KDhcpHouseNumberSuffix = 20;
const TUint8 KDhcpVanityAddress = 21;
const TUint8 KDhcpAdditionalLocationInformation = 22;
const TUint8 KDhcpName = 23;
const TUint8 KDhcpPostalCode = 24;
const TUint8 KDhcpFloor = 27;
const TUint8 KDchpCASeat = 33;

// XML civilLoc elements
_LIT8( KDhcpCountry, "country" );
_LIT8( KDhcpA1, "A1" );     // state
_LIT8( KDhcpA2, "A2" );     // county
_LIT8( KDhcpA3, "A3" );     // city
_LIT8( KDhcpA4, "A4" );     // borough
_LIT8( KDhcpA5, "A5" );     // block
_LIT8( KDhcpA6, "A6" );     // street
_LIT8( KDhcpPRD, "PRD" );   // leading street direction
_LIT8( KDhcpPOD, "POD" );   // trailing street direction
_LIT8( KDhcpSTS, "STS" );   // street suffix
_LIT8( KDhcpHNO, "HNO" );   // house number
_LIT8( KDhcpHNS, "HNS" );   // house number suffix
_LIT8( KDhcpLMK, "LMK" );   // landmark or vanity address
_LIT8( KDhcpLOC, "LOC" );   // additional location information
_LIT8( KDhcpNAM, "NAM" );   // name
_LIT8( KDhcpPC, "PC" );     // postal code
_LIT8( KDhcpFLR, "FLR" );   // floor
_LIT8( KDhcpSEAT, "SEAT" ); // seat


// XML message parsing

// General
_LIT8( KOpenBracket, "<" );
_LIT8( KCloseBracket, ">" );
_LIT8( KForwardSlash, "/" );
_LIT8( KQuotationMark, "\"" );
_LIT8( KColon, ":" );
_LIT8( KSpaceChar, " " );
_LIT8( KHyphen, "-" );

// XML (RFC 4119)
_LIT8( KXMLBegin, "?xml version=\"1.0\" encoding=\"UTF-8\"?" );
_LIT8( KPresence, "presence" );
_LIT8( KTuple, "tuple" );
_LIT8( KStatus, "status" );
_LIT8( KTimeStamp, "<timestamp>" );
_LIT8( KTimeStampEnd, "</timestamp>" );
_LIT8( KXmlnsNamespace, " xmlns=\"urn:ietf:params:xml:ns:pidf\"" );
_LIT8( KId, " id=\"" );
_LIT8( KTupleId1, "tuple1" );
_LIT8( KT, "T" );
_LIT8( KZ, "Z" );

// gp namespace (RFC 4119)
_LIT8( KGpNamespace, " xmlns:gp=\"urn:ietf:params:xml:ns:pidf:geopriv10\"" );
_LIT8( KGp, "<gp:" );
_LIT8( KGpEnd, "</gp:" );
_LIT8( KGeopriv, "geopriv" );
_LIT8( KLocationInfo, "location-info" );
_LIT8( KUsageRules, "usage-rules" );
_LIT8( KRetransmissionAllowed, "retransmission-allowed" );
_LIT8( KRetentionExpiry, "retention-expiry" );
_LIT8( KMethod, "method" );
_LIT8( KNo, "no" );
_LIT8( KDhcp, "DHCP" );

// gml namespace (RFC 4119)
_LIT8( KGmlNamespace,
    " xmlns:gml=\"urn:opengis:specification:gml:schema-xsd:feature:v3.0\"" );
_LIT8( KGml, "<gml:" );
_LIT8( KGmlEnd, "</gml:" );
_LIT8( KGmlId, " gml:id=\"" );
_LIT8( KLocation, "location" );
_LIT8( KPoint, "Point" );
_LIT8( KCoordinates, "coordinates" );
_LIT8( KPointId1, "point1" );
_LIT8( KSrsName, " srsName=\"" );
_LIT8( KNorth, "N" );
_LIT8( KEast, "E" );
_LIT8( KSouth, "S" );
_LIT8( KWest, "W" );

// Datum related (RFC 3825)
_LIT8( KEpsg4269, "epsg:4269" ); // NAD83 (North American Datum 1983)
_LIT8( KEpsg4326, "epsg:4326" ); // EPSG:4326 - WGS84 (World Geodesic Datum)

// cl namespace (RFC 4119)
_LIT8( KClNamespace,
    " xmlns:cl=\" urn:ietf:params:xml:ns:pidf:geopriv10:civicLoc\"" );
_LIT8( KCl, "<cl:" );
_LIT8( KClEnd, "</cl:" );
_LIT8( KCivicAddress, "civicAddress" );

// Other constants
const TUint8 KDhcpMinAsciiValue = 0x1f;
const TUint8 KDhcpMaxAsciiValue = 0x7f;
const TReal KDhcp25BitDivisor = 0x2000000;          // 33554432dec;

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
TDhcpLocationInformationParser::TDhcpLocationInformationParser()
    {
    iTimeStamp.HomeTime();
    }

// ---------------------------------------------------------------------------
// Parse XML description, geoConf or civic address, of the location
// information. RFC 4119
// ---------------------------------------------------------------------------
//
void TDhcpLocationInformationParser::ParseLocationInformation(
    const TDesC8& aInput, const TParsingType aParsingType, TDes8& aOutput )
    {
    TRACESTRING( "TLocationInformationParser::ParseLocationInformation" );

    // XML
    aOutput.Append( KOpenBracket );
    aOutput.Append( KXMLBegin );
    aOutput.Append( KCloseBracket );

    // presence
    aOutput.Append( KOpenBracket );
    aOutput.Append( KPresence );
    aOutput.Append( KXmlnsNamespace );
    aOutput.Append( KGpNamespace );
    if ( EDHCPCoordinates == aParsingType )
        {
        aOutput.Append( KGmlNamespace );
        }
    else // EDHCPCivicAddress
        {
        aOutput.Append( KClNamespace );
        }

    aOutput.Append( KCloseBracket );

    // tuple
    aOutput.Append( KOpenBracket );
    aOutput.Append( KTuple );
    aOutput.Append( KId );
    aOutput.Append( KTupleId1 );
    aOutput.Append( KQuotationMark );
    aOutput.Append( KCloseBracket );

    // timestamp
    aOutput.Append( KTimeStamp );
    ParseTimeStamp( aOutput );
    aOutput.Append( KTimeStampEnd );

    // status
    aOutput.Append( KOpenBracket );
    aOutput.Append( KStatus );
    aOutput.Append( KCloseBracket );

    // geopriv
    aOutput.Append( KGp );
    aOutput.Append( KGeopriv );
    aOutput.Append( KCloseBracket );

    // location-info
    aOutput.Append( KGp );
    aOutput.Append( KLocationInfo );
    aOutput.Append( KCloseBracket );

    if (aParsingType == EDHCPCoordinates)
        {
        // GML
        aOutput.Append( KGml );
        aOutput.Append( KLocation );
        aOutput.Append( KCloseBracket );

        // Point
        aOutput.Append( KGml );
        aOutput.Append( KPoint );
        aOutput.Append( KGmlId );
        aOutput.Append( KPointId1 );
        aOutput.Append( KQuotationMark );
        // srsName, i.e. datum
        ParseDatum( aInput, aOutput ); // Ignore error
        aOutput.Append( KCloseBracket );

        // coordinates
        aOutput.Append( KGml );
        aOutput.Append( KCoordinates );
        aOutput.Append( KCloseBracket );

        // Parse coordinates
        ParseLCI( aInput, aOutput );

        // coordinates end
        aOutput.Append( KGmlEnd );
        aOutput.Append( KCoordinates );
        aOutput.Append( KCloseBracket );

        // Point end
        aOutput.Append( KGmlEnd );
        aOutput.Append( KPoint );
        aOutput.Append( KCloseBracket );

        // GML end
        aOutput.Append( KGmlEnd );
        aOutput.Append( KLocation );
        aOutput.Append( KCloseBracket );
        }
    else // EDHCPCivicAddress
        {
        // civicAddress
        aOutput.Append( KCl );
        aOutput.Append( KCivicAddress );
        aOutput.Append( KCloseBracket );

        // Parse civic address elements
        ParseCivicAddressElements( aInput, aOutput );

        // civicAddress end
        aOutput.Append( KClEnd );
        aOutput.Append( KCivicAddress );
        aOutput.Append( KCloseBracket );
        }

    // location-info end
    aOutput.Append( KGpEnd );
    aOutput.Append( KLocationInfo );
    aOutput.Append( KCloseBracket );

    // usage-rules
    aOutput.Append( KGp );
    aOutput.Append( KUsageRules );
    aOutput.Append( KCloseBracket );
    // retransmission-allowed
    aOutput.Append( KGp );
    aOutput.Append( KRetransmissionAllowed );
    aOutput.Append( KCloseBracket );
    aOutput.Append( KNo );
    aOutput.Append( KGpEnd );
    aOutput.Append( KRetransmissionAllowed );
    aOutput.Append( KCloseBracket );
    // retention-expiry
    aOutput.Append( KGp );
    aOutput.Append( KRetentionExpiry );
    aOutput.Append( KCloseBracket );
    ParseRetentionExpiry( aOutput );
    aOutput.Append( KGpEnd );
    aOutput.Append( KRetentionExpiry );
    aOutput.Append( KCloseBracket );
    // method
    aOutput.Append( KGp );
    aOutput.Append( KMethod );
    aOutput.Append( KCloseBracket );
    aOutput.Append( KDhcp );
    aOutput.Append( KGpEnd );
    aOutput.Append( KMethod );
    aOutput.Append( KCloseBracket );

    // : provided-by

    // usage-rules end
    aOutput.Append( KGpEnd );
    aOutput.Append( KUsageRules );
    aOutput.Append( KCloseBracket );

    // geopriv end
    aOutput.Append( KGpEnd );
    aOutput.Append( KGeopriv );
    aOutput.Append( KCloseBracket );

    // status end
    aOutput.Append( KOpenBracket );
    aOutput.Append( KForwardSlash );
    aOutput.Append( KStatus );
    aOutput.Append( KCloseBracket );

    // tuple end
    aOutput.Append( KOpenBracket );
    aOutput.Append( KForwardSlash );
    aOutput.Append( KTuple );
    aOutput.Append( KCloseBracket );

    // presence end
    aOutput.Append( KOpenBracket );
    aOutput.Append( KForwardSlash );
    aOutput.Append( KPresence );
    aOutput.Append( KCloseBracket );
    }


// ---------------------------------------------------------------------------
// Parses datum from the input
// Datum usage: RFC 3825.
// ---------------------------------------------------------------------------
//
TInt TDhcpLocationInformationParser::ParseDatum (const TDesC8& aInput, TDes8& aOutput )
    {
    TRACESTRING( "TLocationInformationParser::ParseDatum" );

    if ( 16 > aInput.Length() )
        {
        return KErrNotFound;
        }

    // The LCI form is of fixed length (16 bytes). The last byte of the input
    // contains the datum value.
    switch ( ( TUint8 )aInput[ 15 ] )
        {
        case 1:
            {
            // WGS 84, 2D only (For 3D: WGS84 (Geographical 3D))
            aOutput.Append( KSrsName );
            aOutput.Append( KEpsg4326 );
            aOutput.Append( KQuotationMark );
            return KErrNone;
            }
        case 2:
            {
            // NAD83, 2D only (For 3D: NAD83 & NAVD88)
            aOutput.Append( KSrsName );
            aOutput.Append( KEpsg4269 );
            aOutput.Append( KQuotationMark );
            return KErrNone;
            }
        case 3:
            {
            // NAD83, 2D only (For 3D: NAD83 & MLLW)
            aOutput.Append( KSrsName );
            aOutput.Append( KEpsg4269 );
            aOutput.Append( KQuotationMark );
            return KErrNone;
            }
        default:
            {
            return KErrNotSupported;
            }
        }
    }


// ---------------------------------------------------------------------------
// Parses the input to the LCI form
// ---------------------------------------------------------------------------
//
void TDhcpLocationInformationParser::ParseLCI(
    const TDesC8& aInput, TDes8& aOutput )
    {
    TRACESTRING( "TLocationInformationParser::ParseLCI" );

    TLex8 input( aInput );

    // Latitude

    // Latitude resolution (6 bits)
    TUint8 laRes = ( TUint8 )input.Peek();
    laRes >>= 2;

    // Latitude (9 bits integer, 25 bits fraction)
    TUint16 laInt = ( TUint16 )input.Get();
    laInt <<= 14;
    laInt >>= 6;
    laInt += ( TUint8 )input.Peek();
    laInt >>= 1;

    TUint32 laFra = ( TUint32 )input.Get();
    laFra <<= 8;
    laFra += ( TUint8 )input.Get();
    laFra <<= 8;
    laFra += ( TUint8 )input.Get();
    laFra <<= 8;
    laFra += ( TUint8 )input.Get();
    laFra <<= 7;
    laFra >>= 7;

    ParseDMS( ETrue, laRes, laInt, laFra, aOutput );

    aOutput.Append( KSpaceChar );

    // Longitude

    // Longitude resolution (6 bits)
    TUint8 loRes = ( TUint8 )input.Peek();
    loRes >>= 2;

    // Longitude (9 bits integer, 25 bits fraction)
    TUint16 loInt = ( TUint16 )input.Get();
    loInt <<= 14;
    loInt >>= 6;
    loInt += ( TUint8 )input.Peek();
    loInt >>= 1;

    TUint32 loFra = ( TUint32 )input.Get();
    loFra <<= 8;
    loFra += ( TUint8 )input.Get();
    loFra <<= 8;
    loFra += ( TUint8 )input.Get();
    loFra <<= 8;
    loFra += ( TUint8 )input.Get();
    loFra <<= 7;
    loFra >>= 7;

    ParseDMS( EFalse, loRes, loInt, loFra, aOutput );

    // Altitude
    if ( 15 <= aInput.Length() )
        {
        // AT (4 bits)
        // 1 for meters, 2 for floors
        TUint8 AT = ( TUint8 )input.Peek();
        AT >>= 4;

        // Altitude resolution (6 bits)
        TUint16 altRes = ( TUint16 )input.Get();
        altRes <<= 8;
        altRes += ( TUint8 )input.Peek();
        altRes >>= 6;

        // Altitude (30 bits)
        TUint32 alt = ( TUint32 )input.Get();
        alt <<= 26;
        alt >>= 18;
        alt += ( TUint8 )input.Get();
        alt <<= 8;
        alt += ( TUint8 )input.Get();
        alt <<= 8;
        alt += ( TUint8 )input.Get();
        alt <<= 8;

        TRACESTRING2( "AT: %d", AT );
        TRACESTRING2( "altRes: %d", altRes );
        TRACESTRING2( "alt: %d", alt );
        }

    // No support for parsing altitude to XML yet
    }


// ---------------------------------------------------------------------------
// Parses the input to the DMS coordinate form
// ---------------------------------------------------------------------------
//
void TDhcpLocationInformationParser::ParseDMS(
    TBool aIsLatitude,
    TUint8 aResolution,
    TUint16 aInteger,
    TUint32 aFraction,
    TDes8& aOutput )
    {
    TRACESTRING( "TLocationInformationParser::ParseDMS" );

    TRACESTRING2( "aResolution: %d", aResolution );
    TRACESTRING2( "aInteger: %d", aInteger );
    TRACESTRING2( "aFraction: %d", aFraction );

    // Check the first bit for sign
    TUint16 sign = aInteger; // 9 bits
    sign >>= 8;

    // 2s complement
    if ( sign )
        {
        aInteger = ~aInteger;
        aInteger <<= 7;
        aInteger >>= 7;
        aFraction = ~aFraction;
        aFraction <<= 7;
        aFraction >>= 7;
        }

    // Apply resolution
    if ( 9 < aResolution )
        {
        // Fractional part
        aFraction >>= ( 34 - aResolution );
        aFraction <<= ( 34 - aResolution );
        }
    else
        {
        // No fractional part
        aFraction = 0;
        // Integer part
        aInteger >>= ( 9 - aResolution );
        aInteger <<= ( 9 - aResolution );
        }

    // Fractional part
    TReal minutes( 0.0 );
    TReal seconds( 0.0 );
    if ( aFraction )
        {
        TReal frac = ( TReal )aFraction / KDhcp25BitDivisor;
        Math::Int( minutes, 60.0 * frac );
        frac = 60.0 * frac - minutes;
        Math::Int( seconds, 60.0 * frac );
        }

    // Parse the result
    aOutput.AppendNum( aInteger );
    aOutput.Append( KColon );
    aOutput.AppendNum( ( TUint8 )minutes );
    aOutput.Append( KColon );
    aOutput.AppendNum( ( TUint8 )seconds );
    if ( aIsLatitude )
        {
        if ( sign )
            {
            aOutput.Append( KSouth );
            }
        else
            {
            aOutput.Append( KNorth );
            }
        }
    else
        {
        if ( sign )
            {
            aOutput.Append( KWest );
            }
        else
            {
            aOutput.Append( KEast );
            }
        }
    }


// ---------------------------------------------------------------------------
// Parse XML description of civic address elements
// Correspondence of tags can be found in Ch.3.4 Civic Address Components in
// http://www.ietf.org/internet-drafts/draft-ietf-geopriv-dhcp-civil-09.txt
// ---------------------------------------------------------------------------
//
void TDhcpLocationInformationParser::ParseCivicAddressElements(
    const TDesC8& aInput, TDes8& aOutput )
    {
    TRACESTRING( "TLocationInformationParser::ParseCivicAddressElements" );

    TLex8 input( aInput );

    // Skip what
    input.Inc();

    // country code, two letters long
    aOutput.Append( KCl );
    aOutput.Append( KDhcpCountry );
    aOutput.Append( KCloseBracket );
    aOutput.Append( input.Get() );
    aOutput.Append( input.Get() );
    aOutput.Append( KClEnd );
    aOutput.Append( KDhcpCountry );
    aOutput.Append( KCloseBracket );

    // civicAddress elements
    while ( !input.Eos() )
        {
        TInt type = ( TUint8 )input.Get();
        switch ( type )
            {
            case KDhcpState:
                {
                TRACESTRING( "KDhcpState" );
                aOutput.Append( KCl );
                aOutput.Append( KDhcpA1 );
                aOutput.Append( KCloseBracket );
                ExtractCivicAddressElement( input, aOutput );
                aOutput.Append( KClEnd );
                aOutput.Append( KDhcpA1 );
                aOutput.Append( KCloseBracket );
                break;
                }
            case KDhcpCounty:
                {
                TRACESTRING( "KDhcpCounty" );
                aOutput.Append( KCl );
                aOutput.Append( KDhcpA2 );
                aOutput.Append( KCloseBracket );
                ExtractCivicAddressElement( input, aOutput );
                aOutput.Append( KClEnd );
                aOutput.Append( KDhcpA2 );
                aOutput.Append( KCloseBracket );
                break;
                }
            case KDhcpCity:
                {
                TRACESTRING( "KDhcpCity" );
                aOutput.Append( KCl );
                aOutput.Append( KDhcpA3 );
                aOutput.Append( KCloseBracket );
                ExtractCivicAddressElement( input, aOutput );
                aOutput.Append( KClEnd );
                aOutput.Append( KDhcpA3 );
                aOutput.Append( KCloseBracket );
                break;
                }
            case KDhcpBorough:
                {
                TRACESTRING( "KDhcpBorough" );
                aOutput.Append( KCl );
                aOutput.Append( KDhcpA4 );
                aOutput.Append( KCloseBracket );
                ExtractCivicAddressElement( input, aOutput );
                aOutput.Append( KClEnd );
                aOutput.Append( KDhcpA4 );
                aOutput.Append( KCloseBracket );
                break;
                }
            case KDhcpBlock:
                {
                TRACESTRING( "KBlock" );
                aOutput.Append( KCl );
                aOutput.Append( KDhcpA5 );
                aOutput.Append( KCloseBracket );
                ExtractCivicAddressElement( input, aOutput );
                aOutput.Append( KClEnd );
                aOutput.Append( KDhcpA5 );
                aOutput.Append( KCloseBracket );
                break;
                }
            case KDhcpGroupOfStreets:
                {
                TRACESTRING( "KGroupOfStreets" );
                aOutput.Append( KCl );
                aOutput.Append( KDhcpA6 );
                aOutput.Append( KCloseBracket );
                ExtractCivicAddressElement( input, aOutput );
                aOutput.Append( KClEnd );
                aOutput.Append( KDhcpA6 );
                aOutput.Append( KCloseBracket );
                break;
                }
            case KDhcpLeadingStreetDirection:
                {
                TRACESTRING( "KLeadingStreetDirection" );
                aOutput.Append( KCl );
                aOutput.Append( KDhcpPRD );
                aOutput.Append( KCloseBracket );
                ExtractCivicAddressElement( input, aOutput );
                aOutput.Append( KClEnd );
                aOutput.Append( KDhcpPRD );
                aOutput.Append( KCloseBracket );
                break;
                }
            case KDhcpTrailingStreetDirection:
                {
                TRACESTRING( "KTrailingStreetDirection" );
                aOutput.Append( KCl );
                aOutput.Append( KDhcpPOD );
                aOutput.Append( KCloseBracket );
                ExtractCivicAddressElement( input, aOutput );
                aOutput.Append( KClEnd );
                aOutput.Append( KDhcpPOD );
                aOutput.Append( KCloseBracket );
                break;
                }
            case KDhcpStreetSuffix:
                {
                TRACESTRING( "KStreetSuffix" );
                aOutput.Append( KCl );
                aOutput.Append( KDhcpSTS );
                aOutput.Append( KCloseBracket );
                ExtractCivicAddressElement( input, aOutput );
                aOutput.Append( KClEnd );
                aOutput.Append( KDhcpSTS );
                aOutput.Append( KCloseBracket );
                break;
                }
            case KDhcpHouseNumber:
                {
                TRACESTRING( "KHouseNumber" );
                aOutput.Append( KCl );
                aOutput.Append( KDhcpHNO );
                aOutput.Append( KCloseBracket );
                ExtractCivicAddressElement( input, aOutput );
                aOutput.Append( KClEnd );
                aOutput.Append( KDhcpHNO );
                aOutput.Append( KCloseBracket );
                break;
                }
            case KDhcpHouseNumberSuffix:
                {
                TRACESTRING( "KHouseNumberSuffix" );
                aOutput.Append( KCl );
                aOutput.Append( KDhcpHNS );
                aOutput.Append( KCloseBracket );
                ExtractCivicAddressElement( input, aOutput );
                aOutput.Append( KClEnd );
                aOutput.Append( KDhcpHNS );
                aOutput.Append( KCloseBracket );
                break;
                }
            case KDhcpVanityAddress:
                {
                TRACESTRING( "KVanityAddress" );
                aOutput.Append( KCl );
                aOutput.Append( KDhcpLMK );
                aOutput.Append( KCloseBracket );
                ExtractCivicAddressElement( input, aOutput );
                aOutput.Append( KClEnd );
                aOutput.Append( KDhcpLMK );
                aOutput.Append( KCloseBracket );
                break;
                }
            case KDhcpAdditionalLocationInformation:
                {
                TRACESTRING( "KAdditionalLocationInformation" );
                aOutput.Append( KCl );
                aOutput.Append( KDhcpLOC );
                aOutput.Append( KCloseBracket );
                ExtractCivicAddressElement( input, aOutput );
                aOutput.Append( KClEnd );
                aOutput.Append( KDhcpLOC );
                aOutput.Append( KCloseBracket );
                break;
                }
            case KDhcpName:
                {
                TRACESTRING( "KName" );
                aOutput.Append( KCl );
                aOutput.Append( KDhcpNAM );
                aOutput.Append( KCloseBracket );
                ExtractCivicAddressElement( input, aOutput );
                aOutput.Append( KClEnd );
                aOutput.Append( KDhcpNAM );
                aOutput.Append( KCloseBracket );
                break;
                }
            case KDhcpPostalCode:
                {
                TRACESTRING( "KPostalCode" );
                aOutput.Append( KCl );
                aOutput.Append( KDhcpPC );
                aOutput.Append( KCloseBracket );
                ExtractCivicAddressElement( input, aOutput );
                aOutput.Append( KClEnd );
                aOutput.Append( KDhcpPC );
                aOutput.Append( KCloseBracket );
                break;
                }
            case KDchpCASeat:
                {
                TRACESTRING( "KCASeat" );
                aOutput.Append( KCl );
                aOutput.Append( KDhcpSEAT );
                aOutput.Append( KCloseBracket );
                ExtractCivicAddressElement( input, aOutput );
                aOutput.Append( KClEnd );
                aOutput.Append( KDhcpSEAT );
                aOutput.Append( KCloseBracket );
                break;
                }
            case KDhcpFloor:
                {
                TRACESTRING( "KFloor" );
                aOutput.Append( KCl );
                aOutput.Append( KDhcpFLR );
                aOutput.Append( KCloseBracket );
                ExtractCivicAddressElement( input, aOutput );
                aOutput.Append( KClEnd );
                aOutput.Append( KDhcpFLR );
                aOutput.Append( KCloseBracket );
                break;
                }
            default:
                {
                TRACESTRING2( "Not a supported CAtype: %d", type );
                input.Inc( ( TInt )input.Get() ); // Skip value
                break;
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// Extract a single civic address element from the input
// ---------------------------------------------------------------------------
//
void TDhcpLocationInformationParser::ExtractCivicAddressElement(
    TLex8& aInput, TDes8& aOutput )
    {
    TRACESTRING( "TLocationInformationParser::ExtractCivicAddressElement" );

    // Extract data length
    TInt length = ( TInt )aInput.Get();
    // Extract data
    while ( length )
        {
        if ( KDhcpMinAsciiValue < ( TUint8 )aInput.Peek() &&
             KDhcpMaxAsciiValue > ( TUint8 )aInput.Peek() )
            {
            aOutput.Append( aInput.Get() );
            }
        length--;
        }
    }


// ---------------------------------------------------------------------------
// Parses the timestamp
// ---------------------------------------------------------------------------
//
void TDhcpLocationInformationParser::ParseTimeStamp( TDes8& aTimeStamp )
    {
    TRACESTRING( "TLocationInformationParser::ParseTimeStamp" );

    TDateTime timeStamp = iTimeStamp.DateTime();
    aTimeStamp.AppendNum( timeStamp.Year() );
    aTimeStamp.Append( KHyphen );
    aTimeStamp.AppendNum( timeStamp.Month() + 1 );
    aTimeStamp.Append( KHyphen );
    aTimeStamp.AppendNum( timeStamp.Day() + 1 );
    aTimeStamp.Append( KT );
    aTimeStamp.AppendNum( timeStamp.Hour() );
    aTimeStamp.Append( KColon );
    aTimeStamp.AppendNum( timeStamp.Minute() );
    aTimeStamp.Append( KColon );
    aTimeStamp.AppendNum( timeStamp.Second() );
    aTimeStamp.Append( KZ );
    }

// ---------------------------------------------------------------------------
// Parses the retention expiry. It is the timestamp value plus one day
// ---------------------------------------------------------------------------
//
void TDhcpLocationInformationParser::ParseRetentionExpiry( TDes8& aTargetBfr )
    {
    TRACESTRING( "TLocationInformationParser::ParseRetentionExpiry" );
    // Define the retention-expiry
    TTime expiryTime( iTimeStamp );
    TTimeIntervalDays day( 1 );
    expiryTime += day;
    TDateTime retentionExpiry = expiryTime.DateTime();

    // Parse the retention-expiry
    aTargetBfr.AppendNum( retentionExpiry.Year() );
    aTargetBfr.Append( KHyphen );
    aTargetBfr.AppendNum( retentionExpiry.Month() + 1 );
    aTargetBfr.Append( KHyphen );
    aTargetBfr.AppendNum( retentionExpiry.Day() + 1 );
    aTargetBfr.Append( KT );
    aTargetBfr.AppendNum( retentionExpiry.Hour() );
    aTargetBfr.Append( KColon );
    aTargetBfr.AppendNum( retentionExpiry.Minute() );
    aTargetBfr.Append( KColon );
    aTargetBfr.AppendNum( retentionExpiry.Second() );
    aTargetBfr.Append( KZ );
    }
