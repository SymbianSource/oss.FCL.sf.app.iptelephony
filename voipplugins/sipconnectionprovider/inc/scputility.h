/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef SCPUTILITY_H
#define SCPUTILITY_H

#include <cchservice.h>
#include <cchclientserver.h>

#include "scpdefs.h"
#include "scpsipconnection.h"

_LIT8( KSipScheme,  "sip:" );
_LIT8( KSipsScheme, "sips:" );
_LIT8( KAt, "@" );
_LIT8( KColon, ":" );

const TInt KSipSchemeMaxLength = 5;

/**
*  Static utility function for SCP usage.
*
*  @lib sipconnectionprovider.dll
*/
class TScpUtility
    {
public:
                              
   /**
    * Converts a sip connection state to connection event
    * @param aState sip connection state
    * @return CCH state
    */
    static TScpConnectionEvent ConvertToConnectionEvent( 
        CScpSipConnection::TConnectionState aState,
        TInt aError );

   /**
    * Converts a CCH state to conection event
    * @param aState sip connection state
    * @return CCH state
    */
    static TScpConnectionEvent ConvertToConnectionEvent( 
        TCCHSubserviceState aState );
    
   /**
    * Implementation for cleanup item.
    * Resets and destroys array of the RCSE
    * entries. 
    * @param anArray RPointerArray pointer.
    */
    static void ResetAndDestroyEntries( TAny* anArray );
    
   /**
    * Converts a sip error to CCH error
    * @param aState sip error
    * @return CCH error
    */
    static TInt ConvertToCchError( TInt aErrorCode );

    /**
     * Parse prefix and/or domain from username.
     * @param aUsername Username with prefix/domain
     * @param aFormattedUsername parsed username without prefix/domain is stored to this
     * @return Symbian error code
     */
    static TInt RemovePrefixAndDomain( 
        const TDesC8& aUsername, 
        RBuf8& aFormattedUsername );
    
    /**
     * Check if passed username is valid.
     * @param aUsername Username to check
     * @return True if valid, false if no.
     */
    static TBool CheckSipUsername( const TDesC8& aUsername );

    /**
     * Check if passed password is valid.
     * @param aPassweord Password to check
     * @return True if valid, false if no.
     */
    static TBool CheckSipPassword( const TDesC8& aPassword );
    
    /**
     * Prefix getter.
     * @param aUsername used username
     * @param aPrefix valid prefix
     */
    static void GetValidPrefix( const TDesC8& aUsername, TDes8& aPrefix );
    
private:

    // Structure that maps protocol specific errors to generic CCH errors
    struct SCchErrorMapping 
        {
        TInt iServiceError;
        TInt iProtocolError;
        };
    
    // Collection of mapped error codes
    static const SCchErrorMapping iErrorTable[];

    };

#endif      // SCPUTILITY_H   
            
// End of File
