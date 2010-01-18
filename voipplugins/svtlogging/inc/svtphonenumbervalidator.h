/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef C_SVTPHONENUMBERVALIDATOR_H
#define C_SVTPHONENUMBERVALIDATOR_H

#include <e32base.h>

/**
 *  Provides services to validate phone numbers.
 *
 *  @lib svtlogging.dll
 *  @since S60 v5.1
 */
class SvtPhoneNumberValidator
    {

public:

    /**
     * Checks whether the given candidate presents phone number. Empty
     * candidate is considered to be invalid phone number.
     *
     * @since   S60 v5.1
     * @param   aCandidate    Phone number candidate.
     * @return  ETrue if candidate presents phone number, EFalse otherwise.
     */
    static TBool IsValidNumber( const TDesC& aCandidate );
    
private:
    
    static TBool ContainsValidCharacters( const TDesC& aCandidate );    
    
    static TBool AreStartCharactersValid( const TDesC& aCandidate );
    };

#endif // C_SVTPHONENUMBERVALIDATOR_H
