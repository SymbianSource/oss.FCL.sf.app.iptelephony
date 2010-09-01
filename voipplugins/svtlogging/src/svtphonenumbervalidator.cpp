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


#include "svtphonenumbervalidator.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// SvtPhoneNumberValidator::IsValidNumber
// ---------------------------------------------------------------------------
//
TBool SvtPhoneNumberValidator::IsValidNumber( const TDesC& aCandidate )
    {
    if ( aCandidate.Length() != 0 
            && AreStartCharactersValid( aCandidate )
            && ContainsValidCharacters( aCandidate ) )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }


// ---------------------------------------------------------------------------
// SvtPhoneNumberValidator::ContainsValidCharacters
// ---------------------------------------------------------------------------
//
TBool SvtPhoneNumberValidator::ContainsValidCharacters( 
        const TDesC& aCandidate )
    {
    _LIT( KAllowedCharsInPhoneNumber, "0123456789*+pw#PW" );
    
    TLex input( aCandidate );
    TPtrC validChars( KAllowedCharsInPhoneNumber );
    while ( validChars.Locate( input.Peek() ) != KErrNotFound )
        {
        input.Inc();
        }
    
    return ( !input.Remainder().Length() );
    }


// ---------------------------------------------------------------------------
// SvtPhoneNumberValidator::AreStartCharactersValid
// ---------------------------------------------------------------------------
//
TBool SvtPhoneNumberValidator::AreStartCharactersValid( 
        const TDesC& aCandidate )
    {
    TBool isValidStart( ETrue );
    
    TChar firstChar  = '0';
    TChar secondChar = '0';
    TInt candidateLength( aCandidate.Length() );
    if ( 0 < candidateLength )
        {
        firstChar = aCandidate[0];
        firstChar.LowerCase();
        }
    
    if ( 1 < candidateLength )
        {
        secondChar = aCandidate[1];
        secondChar.LowerCase();
        }
    
    if ( firstChar == 'p' || firstChar == 'w' )
        {
        isValidStart = EFalse;
        }
    
    if ( ( firstChar == '+' && secondChar == '+' ) ||
         ( firstChar == '+' && secondChar == 'p' ) ||
         ( firstChar == '+' && secondChar == 'w' ) )
        {
        isValidStart = EFalse;
        }
    
    return isValidStart;
    }
