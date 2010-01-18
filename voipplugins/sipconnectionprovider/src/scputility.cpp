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
* Description:  Provides static utility functions for SCP
*
*/


#include <e32property.h>
#include <crcseprofileregistry.h>
#include <ccherror.h>
#include <siperr.h>

#include "scputility.h"
#include "scplogger.h"

const TScpUtility::SCchErrorMapping TScpUtility::iErrorTable[] = 
    {
        { KCCHErrorInvalidSettings, KErrArgument },
        { KCCHErrorAccessPointNotDefined, KErrBadName },
        { KCCHErrorAuthenticationFailed, KErrSIPForbidden }
    };

// -----------------------------------------------------------------------------
// TScpUtility::ConvertToConnectionEvent
// -----------------------------------------------------------------------------
//
TScpConnectionEvent TScpUtility::ConvertToConnectionEvent( 
    CScpSipConnection::TConnectionState aState,
    TInt aError )
    {
    TScpConnectionEvent returnEvent( EScpUnknown );    

    switch( aState )
        {
        case CScpSipConnection::ERegistered:
            {
            returnEvent = EScpRegistered;    
            }
            break;

        case CScpSipConnection::ERegistering:
            {
            if( aError == KErrNone )
                {
                returnEvent = EScpNetworkFound;
                }
            else if( aError == KCCHErrorNetworkLost )
                {
                returnEvent = EScpNetworkLost;
                }
            else
                {
                returnEvent = EScpRegistrationFailed;
                }
            }
            break;

        case CScpSipConnection::EDeregistered:
            {
            returnEvent = EScpDeregistered;
            }
            break;

        case CScpSipConnection::EDeregistering:
            {
            returnEvent = EScpNetworkNotFound;
            }
            break;

        default:
            User::Panic( KNullDesC, KErrGeneral );
            break;
        }

    return returnEvent;
    }

// -----------------------------------------------------------------------------
// TScpUtility::ConvertToConnectionEvent
// -----------------------------------------------------------------------------
//
TScpConnectionEvent TScpUtility::ConvertToConnectionEvent( 
    TCCHSubserviceState aState )
    {
    TScpConnectionEvent returnEvent( EScpUnknown );    

    switch( aState )
        {
        case ECCHEnabled:
            {
            returnEvent = EScpRegistered;
            }
            break;

        case ECCHConnecting:
            {
            returnEvent = EScpNetworkFound;
            }
            break;

        case ECCHDisabled:
            {
            returnEvent = EScpDeregistered;
            }
            break;

        case ECCHDisconnecting:
            {
            returnEvent = EScpNetworkNotFound;
            }
            break;

        default:
            User::Panic( KNullDesC, KErrGeneral );
            break;
        }

    return returnEvent;
    }

// -----------------------------------------------------------------------------
// TScpUtility::ResetAndDestroyEntries
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void TScpUtility::ResetAndDestroyEntries( TAny* anArray )
    {
    SCPLOGSTRING( "TScpUtility::ResetAndDestroyEntries" );
    
    RPointerArray<CRCSEProfileEntry>* array = 
        reinterpret_cast<RPointerArray<CRCSEProfileEntry>*>( anArray );
        
    if (array)
        {
        array->ResetAndDestroy();
        array->Close();
        }
    }
    
// -----------------------------------------------------------------------------
// TScpUtility::ConvertToCchError
// -----------------------------------------------------------------------------
//
TInt TScpUtility::ConvertToCchError( TInt aErrorCode )
    {
    TInt count = sizeof( iErrorTable ) / sizeof( struct SCchErrorMapping );
    TInt error = KErrUnknown;
    while( count-- )
        {
        if ( iErrorTable[ count ].iProtocolError == aErrorCode )
            {
            error = iErrorTable[ count ].iServiceError;
            break;
            }
        }
    return error; 
    }

// -----------------------------------------------------------------------------
// TScpUtility::RemovePrefixAndDomain
// -----------------------------------------------------------------------------
//
TInt TScpUtility::RemovePrefixAndDomain( const TDesC8& aUsername, 
    RBuf8& aFormattedUsername )
    {
    TInt err( KErrNone );
    
    TInt startPosition = aUsername.Find( KColon() );
    if ( KErrNotFound == startPosition )
        {
        startPosition = 0;
        }
    else
        {
        startPosition = ( startPosition + KColon().Length() );
        }
    TInt endPos = aUsername.Find( KAt() );
    if ( KErrNotFound == endPos )
        {
        endPos = ( aUsername.Length() - startPosition );
        }            
    else
        {
        endPos = ( endPos - startPosition );
        }
    
    TPtrC8 formatted = aUsername.Mid( startPosition, endPos );            
    err = aFormattedUsername.Create( formatted.Length() );
    
    if ( KErrNone == err )
        {
        aFormattedUsername.Copy( formatted );
        }
    
    return err;
    }

// -----------------------------------------------------------------------------
// TScpUtility::CheckSipUsername
// -----------------------------------------------------------------------------
//
TBool TScpUtility::CheckSipUsername( const TDesC8& aUsername )
    {
    TInt retval = ETrue;

    if ( KErrNotFound != aUsername.Find( KSipScheme() ) || 
         KErrNotFound != aUsername.Find( KAt() ) )
        {
        SCPLOGSTRING( "TScpUtility::CheckSipUsername: username has prefix or domain" );
        retval = EFalse;
        }

    if( aUsername == KNullDesC8 )
        {
        SCPLOGSTRING( "TScpUtility::CheckSipUsername: username is empty - not allowed" );
        retval = EFalse;
        }
        
    return retval; 
    }

// -----------------------------------------------------------------------------
// TScpUtility::CheckSipPassword
// -----------------------------------------------------------------------------
//
TBool TScpUtility::CheckSipPassword( const TDesC8& /*aPassword*/ )
    {
    TInt retval = ETrue;

    return retval; 
    }

// -----------------------------------------------------------------------------
// TScpUtility::GetValidPrefix
// -----------------------------------------------------------------------------
//
void TScpUtility::GetValidPrefix( const TDesC8& aUsername, TDes8& aPrefix )
    {
    // no prefix -> use sip:
    aPrefix.Copy( KSipScheme() );
    
    TInt index = aUsername.Find( KColon() );
    if ( KErrNotFound != index )
        {
        index += KColon().Length();
        // if prefix isn't sip or sips, prefix isn't valid
        if ( !aUsername.Left( index ).Compare( KSipScheme() ) )
            {
            aPrefix.Copy( KSipScheme() );
            }
        else if ( !aUsername.Left( index ).Compare( KSipsScheme() ) )
            {
            aPrefix.Copy( KSipsScheme() );
            }
        else
            {
            // prefix not valid -> use sip:
            }
        }
        
    SCPLOGSTRING2( "TScpUtility::GetValidPrefix: %S", &aPrefix );
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
