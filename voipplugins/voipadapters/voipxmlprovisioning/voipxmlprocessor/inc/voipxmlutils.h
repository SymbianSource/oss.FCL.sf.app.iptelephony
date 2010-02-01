/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Utilities class for VoIP XML processor
*
*/


#ifndef VOIPXMLUTILS_H
#define VOIPXMLUTILS_H

#include <e32base.h>

/**
 *  Cleanup item for deleting contents of RPointerArray.
 *
 *  @lib voipxmlprocessor.lib
 *  @since S60 v5.0
 */
template <class T>
class CleanupResetAndDestroy
    {
public:

    /**
     * Pushes aRef to cleanup stack using TCleanupItem (and ResetAndDestroy). 
     *
     * @since S60 v5.0
     * @param aRef Reference to object to be pushed to cleanup stack.
     */
    inline static void PushL( T& aRef );

private:

    /**
     * Method for TCleanupItem used in PushL.
     * 
     * @since S60 v5.0
     * @param aPtr Pointer to be cleaned up.
     */
    inline static void ResetAndDestroy( TAny *aPtr );
    };

template <class T>
inline void CleanupResetAndDestroyL( T& aRef );


/**
 *  Utilities class for VoIP XML processor.
 *  Provides common methods for VoIP XML processor classes.
 *
 *  @lib voipxmlprocessor.lib
 *  @since S60 v5.0
 */
class VoipXmlUtils
    {
#ifdef _DEBUG
    friend class UT_VoipXmlUtils; 
#endif

public:

    /**
     * Converts a descriptor to integer.
     *
     * @since S60 v5.0
     * @param aDesValue Descriptor value.
     * @param aIntValue Interger value (this will hold the converted value).
     * @return Error code.
     */
    static TInt DesToInt( const TDesC& aDesValue, TInt& aIntValue );

    /**
     * Converts an 8 bit descriptor to integer.
     *
     * @since S60 v5.0
     * @param aDesValue Descriptor value.
     * @param aIntValue Interger value (this will hold the converted value).
     * @return Error code.
     */
    static TInt Des8ToInt( const TDesC8& aDesValue, TInt& aIntValue );
    };

#include "voipxmlutils.inl"

#endif // VOIPXMLUTILS_H

// End of file.

