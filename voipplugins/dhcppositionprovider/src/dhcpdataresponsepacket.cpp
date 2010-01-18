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
* Description:  Handle and parse dhcp server response message.
*
*/

#include "dhcppsylogging.h"          // For logging
#include "dhcpconstants.h"
#include "dhcpdataresponsepacket.h"

const TInt KLengthOverMCookie = 236;
const TInt KDhcpMessageEndOption = 255;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// TDhcpDataResponsePacket
// ---------------------------------------------------------------------------
//
TDhcpDataResponsePacket::TDhcpDataResponsePacket()
    {
    iXid = 0;
    }

// ---------------------------------------------------------------------------
// Parses the location information part from the DHCPv4 Inform message,
// Currently supported Options 123 and 99.
// ---------------------------------------------------------------------------
//
TInt TDhcpDataResponsePacket::ParseDHCPAck ()
    {
    TRACESTRING( "TDhcpDataResponsePacket::ParseDHCPAck" );
    TInt dataFound = KErrNotFound;
    if (Length() > KLengthOverMCookie)
        {
        TPtrC8 magicCookie (KDhcpMagicCookie, sizeof(KDhcpMagicCookie));
        //Locate the 'magic cookie' after which the option field begins
        TInt offset = Find( magicCookie );
        if (offset!=KErrNotFound)
            {
            offset += 4; // Skip the cookie
            while (iBuffer[offset] != KDhcpMessageEndOption )
                {
                TInt messageOption = iBuffer[offset];
                switch (messageOption)
                    {
                    case KDhcpMessageGeoSpatialCoordinates:
                        {
                        // Parse Option 123
                        TUint8 dataLength = iBuffer[offset+1];
                        iGeoConf = Mid( ( offset + 2 ), dataLength );
                        offset++;
                        offset += dataLength + 1;
                        dataFound = KErrNone;
                        }
                        break;
                    case KDhcpMessageCivicAddress:
                        {
                        // Parse Option 99
                        // GeoConfCivic can be of variable length
                        TUint8 dataLength = iBuffer[offset+1];
                        iCivicAddress = Mid( ( offset + 2 ), dataLength );
                        offset++;
                        offset += dataLength + 1;
                        dataFound = KErrNone;
                        }
                        break;
                    default:
                        {
                        offset++;
                        offset = offset + (iBuffer[offset]+1) ;
                        }
                        break;
                    }
                }
            }
        }
    return dataFound;
    }

