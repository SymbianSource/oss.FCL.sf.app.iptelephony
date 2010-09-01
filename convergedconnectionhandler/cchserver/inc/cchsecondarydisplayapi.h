/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The set of messages provided to Secondary Display software by
*              : cch application.
*
*/

#ifndef CCH_SECONDARY_DISPLAY_API_H
#define CCH_SECONDARY_DISPLAY_API_H

// INCLUDES
#include <e32base.h>

/*
* ==============================================================================
* This file contains the following sections:
*   - Dialog definitions
*   - Command definitions
*   - Event definitions
*   - Parameter definitions
* ==============================================================================
*/

namespace SecondaryDisplay
{
	
// The category UID for the messages in this header file.
//
const TUid KCatCch = {0x10282CE6};

/*
* ==============================================================================
* Dialogs shown by Cch subsystem. These messages are handled using the
* Secondary Display support in Avkon.
* ==============================================================================
*/
enum TSecondaryDisplayCchDialogs
    {
    /**
    * No note. Error condition if this comes to CoverUI
    */
    ECmdNoNote = 0,
    
    /**
    * A command for showing the VoIP "Do not show this emergency call warning in the future" 
    * query on secondary display. 
    */
    ECmdShowDoNotShowVoipEmergencyCallWarningQuery,

    /**
    * A command for showing the query about VoIP call readiness being reached
    * first time on secondary display.
    */
    ECmdShowVoipEmergencyCallReadinessQuery,

    /**
    * A command for showing the query about only having VoIP call capability
    * on secondary display.
    */
    ECmdShowVoipEmergencyCallErrorNoteNoCsQuery,
    };

} // namespace SecondaryDisplay

#endif      // CCH_SECONDARY_DISPLAY_API_H
            
// End of File
