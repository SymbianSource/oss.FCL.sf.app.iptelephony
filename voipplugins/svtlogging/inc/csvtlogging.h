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
* Description:  Implements call logging extension plugin for SIP VoIP.
*
*/


#ifndef C_CSVTLOGGING_H
#define C_CSVTLOGGING_H

#include <e32base.h>
#include <telloggingextension.h>
#include "csvtsipuriparser.h"

class CSPSettings;
class CSvtSettingsHandler;
class CRCSEProfileRegistry;

/**
 *  Implements call logging extension plugin inteface for SIP VoIP.
 *
 *  @lib svtlogging.dll
 *  @since S60 v5.1
 */
class CSvtLogging : public CTelLoggingExtension
    {

public:

    static CSvtLogging* NewL();
    static CSvtLogging* NewLC();

    /**
    * Destructor.
    */
    virtual ~CSvtLogging();

// from base class CTelLoggingExtension

    /**
    * From CTelLoggingExtension.
    * @see CTelLoggingExtension.
    */
    void InitializeL( TUint aServiceId, const TDesC& aOrigAddress );
    
    /**
    * From CTelLoggingExtension.
    * @see CTelLoggingExtension.
    */
    TInt GetPhoneNumber( RBuf& aPhoneNumber );

    /**
    * From CTelLoggingExtension.
    * @see CTelLoggingExtension.
    */
    TInt GetVoipAddress( RBuf& aVoipAddress );
    
    /**
    * From CTelLoggingExtension.
    * @see CTelLoggingExtension.
    */
    TInt GetMyAddress( RBuf& aMyAddress );
    
    /**
    * From CTelLoggingExtension.
    * @see CTelLoggingExtension.
    */
    TInt GetRemotePartyName( RBuf& aRemotePartyName );
    
private:

    CSvtLogging();

    CSvtSettingsHandler* CreateSvtSettingsHandlerL( TUint aServiceId ) const;
            
    CSvtSipUriParser::TDomainPartClippingSetting ConvertToUriParserSetting( 
        TInt aDomainClipSetting ) const;
    
    CSvtSipUriParser* CreateSipUriParserL( const TDesC& aOrigAddress, 
        TInt aDomainClipSetting ) const;
    
private: // data

    /**
     * SIP URI parser.
     * Own.
     */
    CSvtSipUriParser* iParser;    
    
    /**
     * Settings handler.
     * Own.
     */
    CSvtSettingsHandler* iSettingsHandler;
    
    /**
     * User name if plugin is initialized with SIP user name only.
     * Own.
     */
    RBuf iSipUserName;

    };

#endif // C_CSVTLOGGING_H
