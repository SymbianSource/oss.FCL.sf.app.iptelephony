/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Miscallaneous service functions
*
*/



#ifndef T_VCCMISCUTILS_H
#define T_VCCMISCUTILS_H

#include <e32des8.h>
#include <e32des16.h>

/**
 *  T class for miscallenous service functions
 *
 *  @lib vccutils.dll
 *  @since S60 v3.2
 */
class TVCCMiscUtils
    {
public:

    /**
     * Check if reference is valid URI
     *
     *  @since S60 v3.2
     * @param aURI reference to a 16 bit value checked if valid URI
     * @return KErrNone if the descriptor has been parsed into uri components. 
     *         EUriUtilsParserErrInvalidUri if the descriptor is an invalid uri.
     */
    IMPORT_C static TInt URIValidL( const TDesC16& aURI );
    
    /**
     * Check if reference is valid VDN
     *
     *  @since S60 v3.2
     * @param aVDN reference to a 16 bit value checked if valid VDN
     * @return ETrue if the descriptor is a valid VDN. 
     *         EFalse if the descriptor is an invalid VDN.
     */
    IMPORT_C static TBool VDNValidL(  const TDesC16& aVDN );

    };

#endif /* T_VCCMISCUTILS_H */
