/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MCchNoteHandler class definition. A virtual class.
*
*/


#ifndef MCCHNOTEHANDLER_H
#define MCCHNOTEHANDLER_H

#include "cchclientserverinternal.h"

// CLASS DECLARATION
/**
*  MCchNoteHandler
*  
*  @lib   cchserver
*  @since 5.0
*/

class MCchNoteHandler
    {
    public:

    /**
    * Launches cch global note.
    * @since Series60 5.0
    * 
    * @param aCchGlobalNoteType cch global note type.
    * @param aSoftKeyConfig softkey configuration.
    * @param aType type of confirmation query.
    * @param aSecondaryDisplayIndex secondary display index.
    */
    virtual void LaunchGlobalNoteL( 
        const TCCHGlobalNoteType aCchGlobalNoteType,
        const TInt aResourceID, 
        const TInt aSoftKeyConfig,
        const TInt aSecondaryDisplayIndex )=0;
                
    };

#endif // MCCHNOTEHANDLER_H

// End of File
