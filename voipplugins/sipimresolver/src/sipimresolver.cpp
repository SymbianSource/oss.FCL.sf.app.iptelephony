/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  implementation of sipimresolver
*
*/


#include <spentry.h>
#include <spproperty.h>
#include <spsettings.h>
#include <sdpmediafield.h>

#include "sipimresolver.h"
#include "csipclientresolverutils.h"
#include "sipimresolverdebug.h"

// media field descriptors
_LIT8( KIMField1, "m=application 0 TCP SIPIM\r\n" );
// For content-type header
_LIT8( KIMContentType, "text/plain" );
// For content-type header checks
_LIT8( KIMMediaTypeApp, "text" );
// SDP media subtype
_LIT8( KIMMediaSubtypeSdp, "plain" );

// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// CleanupSdpMediasArray
// ---------------------------------------------------------------------------
//
void CleanupSdpMediasArray( TAny* aObj )
    {
    if ( aObj )
        {
        static_cast<RPointerArray<CSdpMediaField>*>( aObj )->
            ResetAndDestroy();
        }
    }

// ---------------------------------------------------------------------------
// CleanupSipContentTypeHeaderArray
// ---------------------------------------------------------------------------
//
void CleanupSipContentTypeHeaderArray( TAny* aObj )
    {
    if ( aObj )
        {
        static_cast<RPointerArray<CSIPContentTypeHeader>*>( aObj )->
            ResetAndDestroy();
        }
    }


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CSipImResolver::CSipImResolver
// ---------------------------------------------------------------------------
//
CSipImResolver::CSipImResolver() 
    {
    }

// ---------------------------------------------------------------------------
// CSipImResolver::NewL
// ---------------------------------------------------------------------------
//
CSipImResolver* CSipImResolver::NewL()
	{
	SIPIMRLOG( "[SIPIMRESOLVER] -> CSipImResolver::NewL()" )
	CSipImResolver* self = new ( ELeave ) CSipImResolver;
	return self;
	}

// ---------------------------------------------------------------------------
// CSipImResolver::~CSipImResolver
// ---------------------------------------------------------------------------
//
CSipImResolver::~CSipImResolver()
	{
    SIPIMRLOG( "[SIPIMRESOLVER] -> CSipImResolver::~CSipImResolver()" )
	}

// ---------------------------------------------------------------------------
// From class CSIPResolvedClient2
// CSipImResolver::MatchAcceptContactsL
// ---------------------------------------------------------------------------
//
TBool CSipImResolver::MatchAcceptContactsL(
    RStringF /*aMethod*/,
    const CUri8& /*aRequestUri*/,
    const RPointerArray<CSIPHeaderBase>& /*aHeaders*/,
    const TDesC8& /*aContent*/,
    const CSIPContentTypeHeader* /*aContentType*/,
    TUid& /*aClientUid*/ )
    {
    return EFalse;
    }

// ---------------------------------------------------------------------------
// From class CSIPResolvedClient2
// CSipImResolver::MatchEventL
// ---------------------------------------------------------------------------
// 
TBool CSipImResolver::MatchEventL(
    RStringF /*aMethod*/,
    const CUri8& /*aRequestUri*/,
    const RPointerArray<CSIPHeaderBase>& /*aHeaders*/,
    const TDesC8& /*aContent*/,
    const CSIPContentTypeHeader* /*aContentType*/,
    TUid& /*aClientUid*/)
    {
    return EFalse;
    }

// ---------------------------------------------------------------------------
// From class CSIPResolvedClient2
// CSIPImResolver::MatchRequestL
// ---------------------------------------------------------------------------
// 
TBool CSipImResolver::MatchRequestL(
    RStringF /*aMethod*/,
    const CUri8& aRequestUri,
    const RPointerArray<CSIPHeaderBase>& /*aHeaders*/,
    const TDesC8& aContent,
    const CSIPContentTypeHeader* aContentType,
    TUid& aClientUid )
    {
    SIPIMRLOG( "[SIPIMRESOLVER] MatchRequetsL() IN" )
    
    aClientUid.iUid = 0;
    RArray<TInt> callProviderIds;
    CleanupClosePushL( callProviderIds );
    
    TBool match( EFalse );
    TBool imrequest = ( aContent.Length() > 0 && aContentType &&
        aContentType->MediaType().CompareF( KIMMediaTypeApp ) == 0 &&
        aContentType->MediaSubtype().CompareF( KIMMediaSubtypeSdp ) == 0 );
    
    if( imrequest )
        {
        CSPSettings* spSettings = CSPSettings::NewLC();
        CSPProperty* property = CSPProperty::NewLC();
        
        RArray<TUint> serviceIds;
        CleanupClosePushL( serviceIds );
        spSettings->FindServiceIdsL( serviceIds );
        
        for( TInt i( 0 ) ; i < serviceIds.Count() ; i++ )
            {
            TInt err = spSettings->FindPropertyL( 
                serviceIds[ i ],
                EPropertyCallProviderPluginId,
                *property );
            
            if ( !err )
                {
                TInt callProviderId( 0 );
                err = property->GetValue( callProviderId );
                
                if ( !err )
                    {
                    callProviderIds.AppendL( callProviderId );
                    }
                }
            }
        
        CleanupStack::PopAndDestroy( &serviceIds );
        CleanupStack::PopAndDestroy( property );
        CleanupStack::PopAndDestroy( spSettings ); 
        
        CSipClientResolverUtils* resolver = CSipClientResolverUtils::NewLC();
                
        RArray<TUid> uids;
        CleanupClosePushL( uids );
        
        TRAPD( err, resolver->GetAllImplementationUidsWithUserL(
            aRequestUri.Uri().Extract( EUriUserinfo ), uids ) );
        if ( KErrNotFound != err && KErrNone != err )
            {
            User::Leave( err );
            }
        TInt imProviderId( KErrNotFound );
        for ( TInt j( 0 ) ; j < uids.Count() ; j++ )
            {
            TInt ret = callProviderIds.Find( uids[ j ].iUid );
            
            if ( KErrNotFound == ret )
                {
                imProviderId = uids[ j ].iUid;
                break;
                }         
            }
        
        CleanupStack::PopAndDestroy( &uids );
        CleanupStack::PopAndDestroy( resolver );
        
        if ( !err && ( KErrNotFound != imProviderId ) )
            {
            aClientUid.iUid = imProviderId;
            match = ETrue;
            }
        }
    
    CleanupStack::PopAndDestroy( &callProviderIds );
    
    SIPIMRLOGP( "[SIPIMRESOLVER] MatchRequetsL() match = %d", match  )
    SIPIMRLOGP( "[SIPIMRESOLVER] MatchRequetsL() client uid = %d", aClientUid.iUid  )
  
    return match; 
    }

// ---------------------------------------------------------------------------
// From class CSIPResolvedClient2
// CSIPImResolver::ConnectSupported
// ---------------------------------------------------------------------------
// 
TBool CSipImResolver::ConnectSupported()
    {
    return EFalse;
    }

// ---------------------------------------------------------------------------
// From class CSIPResolvedClient2
// CSIPImResolver::ConnectL
// ---------------------------------------------------------------------------
//
void CSipImResolver::ConnectL( const TUid& /*aClientUid*/ )
    {
    // do nothing
    }

// ---------------------------------------------------------------------------
// From class CSIPResolvedClient2
// CSIPImResolver::CancelConnect
// ---------------------------------------------------------------------------
// 
void CSipImResolver::CancelConnect( const TUid& /*aClientUid*/ )
    {
    // do nothing
    }

// ---------------------------------------------------------------------------
// From class CSIPResolvedClient2
// CSIPImResolver::SupportedContentTypesL
// ---------------------------------------------------------------------------
// 
RPointerArray<CSIPContentTypeHeader> CSipImResolver::SupportedContentTypesL()
    {
    SIPIMRLOG( "[SIPIMRESOLVER] -> CSipImResolver::SupportedContentTypesL()" )

    RPointerArray<CSIPContentTypeHeader> ret;   
    TCleanupItem tci( CleanupSipContentTypeHeaderArray, &ret );
    
    CleanupStack::PushL( tci );
    CSIPContentTypeHeader* ctype = CSIPContentTypeHeader::DecodeL(
        KIMContentType );
    ret.AppendL( ctype );
    CleanupStack::Pop( ); // TCleanupItem

    SIPIMRLOG( "[SIPIMRESOLVER] <- CSipImResolver::SupportedContentTypesL()" )
    return ret;
    }

// ---------------------------------------------------------------------------
// From class CSIPResolvedClient2
// CSIPImResolver::SupportedSdpMediasL
// ---------------------------------------------------------------------------
// 
RPointerArray<CSdpMediaField> CSipImResolver::SupportedSdpMediasL()
    {
    SIPIMRLOG( "[SIPIMRESOLVER] -> CSipImResolver::SupportedSdpMediasL()" )
    
    // Initialise return array
    RPointerArray<CSdpMediaField> ret;
    TCleanupItem tci( CleanupSdpMediasArray, &ret );
    CleanupStack::PushL( tci );  
    
    CSdpMediaField* field = CSdpMediaField::DecodeLC( KIMField1 );
    ret.AppendL( field );
    CleanupStack::Pop( field );    
    
    CleanupStack::Pop( ); // TCleanupItem

    SIPIMRLOG( "[SIPIMRESOLVER] <- CSipImResolver::SupportedSdpMediasL()" )
    return ret;
    }

// ---------------------------------------------------------------------------
// From class CSIPResolvedClient2
// CSIPImResolver::AddClientSpecificHeadersForOptionsResponseL
// ---------------------------------------------------------------------------
// 
void CSipImResolver::AddClientSpecificHeadersForOptionsResponseL(
    RPointerArray<CSIPHeaderBase>& /*aHeaders*/ )
    {
    // do nothing
    }
