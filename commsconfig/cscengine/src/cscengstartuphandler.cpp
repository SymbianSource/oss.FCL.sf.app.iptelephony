/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  For CSC startup handling
*
*/


#include <e32base.h>

#include "cscenglogger.h"
#include "cscengstartuphandler.h"

_LIT( KTagURL, "URL=" );
_LIT( KTagAppUid, "APPUID=" );
_LIT( KTagAction, "ACTION=" );
_LIT( KTagServiceId, "SERID=" );
_LIT( KTagPluginUid, "PLUGINUID=" );



const TUint KTagUidPrefixLength = 2;
const TUint KUidLength = 8;

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCEngStartupHandler::CCSCEngStartupHandler()
    {    
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CCSCEngStartupHandler::ConstructL()
    {    
    CSCENGDEBUG( "CCSCEngStartupHandler::ConstructL begin" );    
    
    iAction = EDefaultAction;
    iServiceId = 0;
    iPluginUid = KNullUid;
    iAppUid = KNullUid;
     
    CSCENGDEBUG( "CCSCEngStartupHandler::ConstructL end" ); 
    }
    
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CCSCEngStartupHandler* CCSCEngStartupHandler::NewL()
    {
    CCSCEngStartupHandler* self = new (ELeave) CCSCEngStartupHandler;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CCSCEngStartupHandler::~CCSCEngStartupHandler()
    {
    CSCENGDEBUG( 
        "CCSCEngStartupHandler::~CCSCEngStartupHandler - begin" ); 
    
    delete iURL;
    
    CSCENGDEBUG( 
        "CCSCEngStartupHandler::~CCSCEngStartupHandler - end" );
    }


// -----------------------------------------------------------------------------
// For setting CSC application startup parameters
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CCSCEngStartupHandler::SetStartupParametersL( 
    const TDesC& aParameters )
    {
    CSCENGDEBUG( "CCSCEngStartupHandler::SetStartupParameters - begin" );
    
    TInt error( KErrNone );   

    SetAction( aParameters );
    
    if ( EOpenSettingsUi == GetParamAction() )
        {
        SetServiceId( aParameters );
        }
    else
        {
        SetUid( aParameters, EPluginUid );
        SetUid( aParameters, EAppUid );
        SetUrlL( aParameters );
        }
    
    CSCENGDEBUG( "CCSCEngStartupHandler::SetStartupParameters - end" );
    
    return error;
    }


// -----------------------------------------------------------------------------
// For getting parameter action
// -----------------------------------------------------------------------------
//
EXPORT_C CCSCEngStartupHandler::TAction 
    CCSCEngStartupHandler::GetParamAction() const
    {
    CSCENGDEBUG( "CCSCEngStartupHandler::GetParamAction" );
    return iAction;
    }


// -----------------------------------------------------------------------------
// For getting parameter service id
// -----------------------------------------------------------------------------
//
EXPORT_C TUint CCSCEngStartupHandler::GetParamServiceId() const
    {
    CSCENGDEBUG( "CCSCEngStartupHandler::GetParamServiceId" );
    return iServiceId;
    }


// -----------------------------------------------------------------------------
// For getting parameter plugin uid
// -----------------------------------------------------------------------------
//
EXPORT_C const TUid CCSCEngStartupHandler::GetParamPluginUid() const
    {
    CSCENGDEBUG( "CCSCEngStartupHandler::GetParamPluginUid" );
    return iPluginUid;
    }


// -----------------------------------------------------------------------------
// For getting parameter application uid
// -----------------------------------------------------------------------------
//
EXPORT_C const TUid CCSCEngStartupHandler::GetParamAppUid() const
    {
    CSCENGDEBUG( "CCSCEngStartupHandler::GetParamAppUid" );
    return iAppUid;
    }    


// -----------------------------------------------------------------------------
// For Getting parameter URL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CCSCEngStartupHandler::GetParamUrl( TDes& aUrl ) const
    {
    CSCENGDEBUG( "CCSCEngStartupHandler::GetParamUrlL - begin" );   
        
    if ( !iURL )
        {
        return KErrNotFound;
        }
    else
        {
        aUrl.Copy( iURL->Des() );    
        }
    
    CSCENGDEBUG( "CCSCEngStartupHandler::GetParamUrlL - end" ); 
        
    return KErrNone;
    }   


// -----------------------------------------------------------------------------
// For resetting uid values
// -----------------------------------------------------------------------------
//
EXPORT_C void CCSCEngStartupHandler::ResetUid( TUidType aType )
    {
    CSCENGDEBUG( "CCSCEngStartupHandler::ResetUid - begin" );   
    
    if ( EPluginUid == aType )
        {
        iPluginUid = KNullUid;
        }
    else if ( EAppUid == aType )
        {  
        iAppUid = KNullUid;
        }
    
    CSCENGDEBUG( "CCSCEngStartupHandler::ResetUid - end" );   
    }


// -----------------------------------------------------------------------------
// For resetting service id value
// -----------------------------------------------------------------------------
//
EXPORT_C void CCSCEngStartupHandler::ResetServiceId()
    {
    iServiceId = 0;
    }

// -----------------------------------------------------------------------------
// For resetting service id value
// -----------------------------------------------------------------------------
//
EXPORT_C void CCSCEngStartupHandler::SetStartedFromHomescreen( 
    TBool aStartedFromHomescreen )
    {
    iStartedFromHomescreen = aStartedFromHomescreen;
    }

// -----------------------------------------------------------------------------
// For resetting service id value
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CCSCEngStartupHandler::StartedFromHomescreen() const
    {
    return iStartedFromHomescreen;
    }

// ---------------------------------------------------------------------------
// For parsing and setting parameter action
// ---------------------------------------------------------------------------
//
void CCSCEngStartupHandler::SetAction( const TDesC& aParameters )
    {
    CSCENGDEBUG( "CCSCEngStartupHandler::SetAction - begin" );    
        
    TInt start( 0 );
    
    // Parse Action parameter
    start = aParameters.Find( KTagAction );
    
    if ( KErrNotFound != start )
        {
        start = start + KTagAction().Length();
        TPtrC pPtr = aParameters.Mid( start, 1 );
            
        // Convert string to TUint
        TUint value( 0 );
        TLex lex( pPtr );
        lex.Val( value, EDecimal );
    
        // Set action
        if ( EOpenSettingsUi < iAction )
            {
            iAction = EDefaultAction;
            }
        else
            {
            iAction = static_cast<CCSCEngStartupHandler::TAction> ( value );
            }    
        }
    
    CSCENGDEBUG( "CCSCEngStartupHandler::SetAction - end" ); 
    }


// ---------------------------------------------------------------------------
// For parsing and setting service id parameter
// ---------------------------------------------------------------------------
//
void CCSCEngStartupHandler::SetServiceId( const TDesC& aParameters )
    {
    CSCENGDEBUG( "CCSCEngStartupHandler::SetServiceId - begin" );   
        
    TInt start( 0 );
    TUint value( 0 );

    // Parse plug-in uid parameter
    start = aParameters.Find( KTagServiceId );
        
    if ( KErrNotFound != start )
        {
        // exclude 'SERID='
        start = start + KTagServiceId().Length();
        
        TInt serviceIdLength = aParameters.Length() - start;
        
        CSCENGDEBUG2( "CCSCEngStartupHandler::SetServiceId: Length=%d", serviceIdLength );   
        
              
        TPtrC pPtr = aParameters.Mid( start, serviceIdLength );
        
        TLex lex( pPtr );
        lex.Val( value, EDecimal );
        
        iServiceId = value;
        }
  
                           
    CSCENGDEBUG( "CCSCEngStartupHandler::SetServiceId - end" );
    }


// ---------------------------------------------------------------------------
// For parsing and setting uid parameters
// ---------------------------------------------------------------------------
//
void CCSCEngStartupHandler::SetUid( 
    const TDesC& aParameters, TUidType aUidType )
    {
    CSCENGDEBUG( "CCSCEngStartupHandler::SetPluginUid - begin" );   
        
    TInt start( 0 );
    TUint value( 0 );
    
    if ( EPluginUid == aUidType )
        {
         // Parse plug-in uid parameter
        start = aParameters.Find( KTagPluginUid );
        
        if ( KErrNotFound != start )
            {
            // exclude 'PLUGINUID=' and '0x'
            start = start + KTagPluginUid().Length() + KTagUidPrefixLength;
              
            TPtrC pPtr = aParameters.Mid( start, KUidLength );
        
            TLex lex( pPtr );
            lex.Val( value, EHex );
        
            iPluginUid.iUid = value;
            }
  	    }
  	else
  	    {
  	    // Parse application uid parameter
        start = aParameters.Find( KTagAppUid );
        
        if ( KErrNotFound != start )
            {
            // exclude 'APPUID=' and '0x'
            start = start + KTagAppUid().Length() + KTagUidPrefixLength;
              
            TPtrC pPtr = aParameters.Mid( start, KUidLength );
        
            TLex lex( pPtr );
            lex.Val( value, EHex );
        
            iAppUid.iUid = value;
            }
  	    }
                           
    CSCENGDEBUG( "CCSCEngStartupHandler::SetPluginUid - end" );
    }


// ---------------------------------------------------------------------------
// For parsing and setting parameter URL
// ---------------------------------------------------------------------------
//
void CCSCEngStartupHandler::SetUrlL( const TDesC& aParameters )
    {
    CSCENGDEBUG( "CCSCEngStartupHandler::SetUrl - begin" );
        
    TInt start( 0 );
    
    // Parse URL parameter
    start = aParameters.Find( KTagURL );
    
    if ( KErrNotFound != start )
        {
        // exclude 'URL='
        start = start + KTagURL().Length();
    
        TInt urlLength = aParameters.Length() - start;
        
        // If iURL exists, delete it first
        if ( iURL )
            {
            delete iURL;
            iURL = NULL;
            }
        
        // Set URL
        if ( urlLength )
            {
            iURL = HBufC::NewL( urlLength );            
            iURL->Des().Copy( aParameters.Mid( start, urlLength ) );
            }
        }
    
    CSCENGDEBUG( "CCSCEngStartupHandler::SetUrl - end" );
    }

