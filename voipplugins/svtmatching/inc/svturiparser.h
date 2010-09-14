/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Uri parser class for svtmatching.
*
*/


#ifndef C_SVTURIPARSER_H
#define C_SVTURIPARSER_H

#include <e32base.h>


/**
 *  Uri parser class for svtmatching.
 *
 *  @lib svtmatching
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CSvtUriParser ) : public CBase
    {
public:

    /**
     * Two-phased constructor.
     */
    static CSvtUriParser* NewL();
  
  
    /**
     * Destructor.
     */
    ~CSvtUriParser();
 
    /**
     * Parses address by rcse's ignore domain part setting.
     * @param aIgnoreDomain - Value of ignore domain part setting.
     * @param aOriginal - Original address to be parsed.
     * @param aParsedAddress - Parsed address.
     */     
    void ParseAddressL( TInt aIgnoreDomain, 
                        const TDesC& aOriginal, 
                        RBuf& aParsedAddress ) const;

    /**
     * Parses displayname part from uri.
     * @param aData - Original uri.
     * @param aDisplayname - Uris displayname part if found from
     *                       original uri.
     * @return KErrNone if succeed
     */ 
    TInt DisplayNameFromUri( const TDesC& aData, RBuf& aDisplayname ) const;
    
    /**
     * Parses displayname part from uri.
     * @leave Leaves with system wide error.
     * @param aData - Original uri.
     * @param aDisplayname - Uris displayname part if found from
     *                       original uri.
     * @param aResult - Result of operation, KErrNone if success.
     */ 
    void DisplayNameFromUriL( const TDesC& aData, RBuf& aDisplayname, TInt& aResult ) const;
    
    /**
     * Checks sip uri for spaces in begin and end of string.
     * @leave Leaves on failure.
     * @param aOriginal - Original sip uri.
     * @param aCheckedAddress - checked sip uri.
     */ 
    void CheckForSpacesL( const TDesC& aOriginal, 
                          RBuf& aCheckedAddress ) const;

    /**
     * Checks is a string valid for CS call.
     * @param aOriginal - String to be checked.
     * @return ETrue if string valid for CS call.
     */ 
    TBool IsValidGsmNumber( const TDesC& aOriginal ) const;
      
private:
    
    /**
     * Parses username part from sip uri.
     * @param aOriginal - Original sip uri.
     * @param aUserName - Username part of the sip uri.
     * @return KErrNone if operation succeed.
     */ 
    TInt GetUserNamePart( const TDesC& aOriginal,
                          TDes& aUserName  ) const;
    
    /**
     * Handles username parts for ignore domain part setting
     * values 1 and 2.
     * @leave Leaves on failure.
     * @param aIgnoreDomain - Value of ignore domain part setting.
     * @param aOriginal - Original sip uri.
     * @param aParsedAddress - Parsed sip uri.
     */ 
    void HandleUserNamePartL( TInt aIgnoreDomain, 
                              const TDesC& aOriginal, 
                              RBuf& aParsedAddress ) const;
    
private:
    
    CSvtUriParser();
    
private: // Data

#ifdef _DEBUG    
    friend class T_CSvtUriParser;
#endif
    
    };

#endif //C_SVTURIPARSER_H
