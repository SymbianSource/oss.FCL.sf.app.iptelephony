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
* Description:  Base class for DHCP packet message.
*
*/

#include "dhcppsylogging.h"          // For logging
#include "dhcpdatapacket.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// TDhcpBasePacket
// ---------------------------------------------------------------------------
//
TDhcpBasePacket::TDhcpBasePacket() :
        TPtr8( iBuffer, 0, KDhcpPacketLength )
    {
    }

#ifdef  _DEBUG
// ---------------------------------------------------------------------------
// TDhcpBasePacket::PrintTraceLog
// ---------------------------------------------------------------------------
//
void TDhcpBasePacket::PrintTraceLog ()
    {
    TRACEHEXDUMP(iBuffer, Length());
    }

#endif

