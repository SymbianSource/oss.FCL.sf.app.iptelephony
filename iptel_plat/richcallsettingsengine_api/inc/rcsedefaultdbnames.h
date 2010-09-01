/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Database name constants
*
*/



#ifndef RCSEDEFAULTDBNAMES
#define RCSEDEFAULTDBNAMES

// CONSTANTS

// Profile database name, format is [drive letter]:[database name]
_LIT( KDbName, "c:RCSE_VoIPProfiles" );

// Audio Codec database name, format is [drive letter]:[database name].
_LIT( KDbCodecName, "c:RCSE_AudioCodec" );

// SCCP setting database name, format is [drive letter]:[database name]
_LIT( KDbSCCPName, "c:RCSE_SCCPSetting" );

// SIP setting database name, format is [drive letter]:[database name]
_LIT( KDbSIPName, "c:RCSE_SIPSetting" );


// Temporary database name, format is [drive letter]:[database name]
_LIT( KDbTempName, "c:RCSE_Temp" );


#endif      // RCSEDEFAULTDBNAMES   
            
// End of File
