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



#ifndef C_CSVTSIPURIPARSER_H
#define C_CSVTSIPURIPARSER_H

#include <e32base.h>

/**
 *  Provides services for SIP URI parsing.
 *
 *  @lib svtlogging.dll
 *  @since S60 v5.1
 */
class CSvtSipUriParser : public CBase
    {

public:

    /**  Domain part clipping options for SIP URIs. */
    enum TDomainPartClippingSetting
        {
        /** Domain part is not clipped. */ 
        ENoClipping           = 0,
        /** Domain part is clipped if user name part presents 
            a telephony number. */
        EClipDomainIfNumber   = 1,
        /** Domain part is always clipped. */
        EClipDomain           = 2
        };

    /**
     * Two-phased constructor.
     * @param   aSipUri     The SIP URI to be parsed.
     * @param   aOptions    Clipping options.
     */
    static CSvtSipUriParser* NewL( const TDesC& aSipUri,
        TDomainPartClippingSetting aOptions );

    /**
     * Two-phased constructor.
     * @param   aSipUri     The SIP URI to be parsed.
     * @param   aOptions    Clipping options.
     */
    static CSvtSipUriParser* NewLC( const TDesC& aSipUri,
        TDomainPartClippingSetting aOptions );
    
    /**
    * Destructor.
    */
    virtual ~CSvtSipUriParser();

    /**
     * Returns parsed VoIP address if available. If VoIP address is not
     * available a KNullDesC is returned. That can happen in case of private
     * address.
     *
     * @since   S60 v5.1
     * @param   aVoipAddress    Parsed VoIP address or KNullDesC.
     * @return  System wide error code.
     */
    TInt GetVoipAddress( RBuf& aVoipAddress ) const;

    /**
     * Returns parsed phone number if available. If phone number is not
     * available a KNullDesC is returned.
     *
     * @since   S60 v5.1
     * @param   aPhoneNumber    Parsed phone number or KNullDesC.
     * @return  System wide error code.
     */
    TInt GetPhoneNumber( RBuf& aPhoneNumber ) const;
    
    /**
     * Returns display name if available. If display name is not available
     * a KNullDesC is returned.
     *
     * @since   S60 v5.1
     * @param   aPhoneNumber    Display name or KNullDesC.
     * @return  System wide error code.
     */
    TInt GetDisplayName( RBuf& aDisplayName ) const;
    
private:

    CSvtSipUriParser() {};
    
    CSvtSipUriParser( TDomainPartClippingSetting aOptions );

    void ConstructL( const TDesC& aSipUri );
    
    void ParseSipUriL( const TDesC& aSipUri );
    
    void ParseDisplayNameL( const TDesC& aSipUri );
    
    void RemoveElbows( TDes& sipUri ) const;
    
    void ParseUserNameL( const TDesC& aSipUri );
    
    void ParseDomainL( const TDesC& aSipUri );
    
    void ParsePrivateAddress();
    
    TInt AddressLength( const TDesC& aUserName, const TDesC& aDomain,
        TDomainPartClippingSetting aClipSetting ) const;
    
    TBool IsAnonymousUri( const TDesC& aCandidate ) const;

private: // data

    /**
     * Domain part clipping options.
     */
    TDomainPartClippingSetting iOptions;

    /**
     * User name part of the parsed SIP URI.
     * Own.
     */
    RBuf iUserName;

    /**
     * Domain name part of the parsed SIP URI.
     * Own.
     */
    RBuf iDomain;
    
    /**
     * Display name from the parsed SIP URI if available.
     * Own.
     */
    RBuf iDisplayName;

    };

#endif // C_CSVTSIPURIPARSER_H
