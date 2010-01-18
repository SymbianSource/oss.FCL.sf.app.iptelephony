/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  provides uri parser methods for SVP
*
*/



#ifndef SVPURIPARSER_H
#define SVPURIPARSER_H

#include <escapeutils.h>
#include "svputdefs.h"

 /**
 *  Uri parser methods for SVP
 *
 *  @lib 
 *  @since S60 3.2
 */
class CSVPUriParser : public CBase
    {

public: 
    
    /**
    * Two-phased constructor.
    */
    static CSVPUriParser* NewL();
    
    /**
    * Two-phased constructor.
    */
    static CSVPUriParser* NewLC();
    
    /**
    * Destructor.
    */
    virtual ~CSVPUriParser();


public: // new methods

    
    /**
    * Method for setting user=phone parameter to invite
    * @sinceSeries 60 3.2
    * @param aValue ETrue or EFalse
    */
    void SetUserEqualsPhoneRequiredL( TBool aValue );

    /**
    * Method for checking if uri is valid for user=phone parameter
    * @since Series 60 3.2
    * @param aUri Uri to be checked
    * @return TBool URI ETrue if valid, EFalse otherwise
    */
    static TBool IsUriValidForUserEqualsPhoneL( const TDesC8& aUri );
    
    /**
    * Method for checking whether user=phone is needed
    * @since Series 60 3.2
    * @return TBool ETrue if valid, EFalse otherwise
    */
    TBool UserEqualsPhoneRequiredL() const;

    /**
    * Adds user=phone if neccessary
    * @since Series 60 3.2
    * @param aUri URI where the user=phone parameter is added
    */
    void AddUserEqualsPhoneL( HBufC8*& aUri ) const;

   
    /**
    * Completes the recipients SIP Uri. 
    * Adds "sip:", if it's missing.
    * Adds "@", if it's missing.
    * Adds the domain from own registered SIP Profile to the
    * domain part of address if it's missing.
    * Leaves on error.
    * @since Series 60 3.2
    * @param aUri The URI to be completed, non-modifiable descriptor.
    * @param aAOR the address-of-record containing the domain part
    * @param aIsEmergency Indicates emergency call
    * @return Pointer to the altered URI
    */
    HBufC8* CompleteSipUriL( 
        const TDesC8& aUri, 
        const TDesC8& aAOR, 
        TBool aIsEmergency = EFalse ) const;
    
    /**
    * Completes the recipients SIP Uri. 
    * Adds "sips:", if it's missing.
    * Adds "@", if it's missing.
    * Adds the domain from own registered SIP Profile to the
    * domain part of address if it's missing.
    * Leaves on error.
    * @since Series 60 3.2
    * @param aUri The URI to be completed, non-modifiable descriptor.
    * @param aAOR the address of record containing the domain part
    * @return Pointer to the altered URI
    */
    HBufC8* CompleteSecureSipUriL( const TDesC8& aUri, 
                                   const TDesC8& aAOR ) const;
    
    /**
    * Completes the recipients SIP Uri for SIP events. 
    * Adds "sip:", if it's missing.
    * Leaves on error.
    * @since Series 60 3.2
    * @param aUri The URI to be completed, non-modifiable descriptor.
    * @return Pointer to the altered URI
    */
    HBufC8* CompleteEventSipUriL( const TDesC8& aUri ) const;

    /**
    * Parses aUri and returns its domain part
    * @sinceSeries 60 3.2
    * @param aUri URI for extracting domain
    * @return Pointer to a newly allocated domain part
    */
    static HBufC8* DomainL( const TDesC8& aUri );
    
    /**
    * Escape decodes the characters which are not acceptable in SIP URI  
    * @since Series 60 3.2
    * @param aSipUri SIP Uri to decode
    * @return None
    */  
    static void EscapeDecodeSipUriL( HBufC8*& aSipUri );
    
    /**
    * Escape encodes the characters which are not acceptable in SIP URI  
    * @since Series 60 3.2
    * @param aSipUri SIP Uri to encode
    * @param aMode Defines the escaping mode
    * @return None
    */
    static void EscapeEncodeSipUriL( HBufC8*& aSipUri, EscapeUtils::TEscapeMode aMode );

    /**
    * Parses and returns display name of the given remote party address  
    * @since Series 60 3.2
    * @param aAddress Remote party address
    * @return Display name
    */
    static HBufC* ParseDisplayNameL( const TDesC8& aAddress );

    /**
    * Parses and returns URI of the given remote party address  
    * @since Series 60 3.2
    * @param aAddress Remote party address
    * @return URI
    */
    static HBufC* ParseRemotePartyUriL( const TDesC8& aAddress );
    
private: // new methods
    
    /**
    * Checks, if the URI given as parameter has
    * "sips:" prefix.
    * @since Series 60 3.2
    * @param aUri The URI to be checked, non-modifiable descriptor.
    * @return TBool ETrue, if the prefix is there,
    *         EFalse, if not.
    */
    TBool CheckSipsPrefix( const TDesC8& aUri ) const;     
    
    
    /**
    * Checks, if the URI given as parameter has
    * "sip:" prefix.
    * @since Series 60 3.2
    * @param aUri The URI to be checked, non-modifiable descriptor.
    * @return TBool ETrue, if the prefix is there,
    *         EFalse, if not.
    */
    TBool CheckSipPrefix( const TDesC8& aUri ) const;

    /**
    * Checks, if the URI given as parameter has
    * the "@" character.
    * @since Series 60 3.2
    * @param aUri The URI to be checked, non-modifiable descriptor.
    * @return TBool ETrue, if the chareacter is there,
    *         EFalse, if not.
    */
    TBool CheckAt( const TDesC8& aUri ) const;
    
    /**
    * Checks, if the URI given as parameter has
    * the domain part.
    * @since Series 60 3.2
    * @param aUri The URI to be checked,non-modifiable descriptor.
    * @return TBool ETrue, if the domain is there,
    * EFalse, if not.
    */
    TBool CheckDomain( const TDesC8& aUri ) const;

    /**
    * Adds the "sip:" prefix to the URI given as parameter.
    * Leaves if the adding failes.
    * @since Series 60 3.2
    * @param aUri The URI to be altered, modifiable descriptor.
    * @return None
    */
    void AddSipPrefixL( HBufC8*& aUri ) const;
    
     /**
    * Adds the "sips:" prefix to the URI given as parameter.
    * Leaves if the adding failes.
    * @since Series 60 3.2
    * @param aUri The URI to be altered, modifiable descriptor.
    * @return None
    */
    void AddSipsPrefixL( HBufC8*& aUri ) const;
    
    /**
    * Removes the "sip:" prefix from the URI given as parameter.
    * Leaves if the adding failes.
    * @since Series 60 3.2
    * @param aUri The URI to be altered, modifiable descriptor.
    * @return None
    */
    void RemoveSipPrefixL( HBufC8*& aUri ) const;
    
    /**
    * Removes the "sips:" prefix from the URI given as parameter.
    * Leaves if the adding failes.
    * @since Series 60 3.2
    * @param aUri The URI to be altered, modifiable descriptor.
    * @return None
    */
    void RemoveSipsPrefixL( HBufC8*& aUri ) const;

    /**
    * Checks, if the URI given as parameter has
    * the "<" character.
    * @since Series 60 3.2
    * @param aUri The URI to be checked, non-modifiable descriptor.
    * @return TBool ETrue, if the chareacter is there,
    *         EFalse, if not.
    */
    TBool CheckLeftBracket( const TDesC8& aUri ) const;

    /**
    * Removes the "<" character from the URI given as parameter.
    * @since Series 60 3.2
    * @param aUri The URI to be altered, modifiable descriptor.
    * @return None
    */
    void RemoveLeftBracket( HBufC8*& aUri ) const;

    /**
    * Checks, if the URI given as parameter has
    * the ">" character.
    * @since Series 60 3.2
    * @param aUri The URI to be checked, non-modifiable descriptor.
    * @return TBool ETrue, if the chareacter is there,
    *         EFalse, if not.
    */
    TBool CheckRightBracket( const TDesC8& aUri ) const;

    /**
    * Removes the ">" character from the URI given as parameter.
    * @since Series 60 3.2
    * @param aUri The URI to be altered, modifiable descriptor.
    * @return None
    */
    void RemoveRightBracket( HBufC8*& aUri ) const;

    /**
    * Adds the "@" character to the URI given as parameter.
    * Leaves if the adding failes.
    * @since Series 60 3.2
    * @param aUri The URI to be altered, modifiable descriptor.
    * @return None
    */
    void AddAtL( HBufC8*& aUri ) const;

    /**
    * Adds the domain part to the URI
    * Leaves if the adding failes.
    * @since Series 60 3.2
    * @param aUri The URI to be altered, modifiable descriptor.
    * @return None
    */
    void AddDomainL( HBufC8*& aUri, const TDesC8& aAOR ) const;
        
private: // methods
    
    /**
    * C++ default constructor.
    */
    CSVPUriParser();
    
private: // data
    
    /**
    * Variable for to check user=phone setting
    */    
    TBool iUserEqualsPhoneRequired;


private:

    // For testing
    SVP_UT_DEFS
		
    };


#endif // SVPUTILITY_H

