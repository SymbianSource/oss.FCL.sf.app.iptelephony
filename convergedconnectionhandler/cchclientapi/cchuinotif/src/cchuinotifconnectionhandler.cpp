/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provider access to SNAPs
*
*/


#include <cmmanagerdef.h>
#include <cmpluginwlandef.h> // bearer type
#include <cmdestinationext.h>
#include <cmconnectionmethoddef.h>
#include <cmconnectionmethodext.h>
#include <cmpluginpacketdatadef.h>

#include "cchuilogger.h"
#include "cchuinotifconnectionhandler.h"

// ======== MEMBER FUNCTIONS ========

CCchUiNotifConnectionHandler::CCchUiNotifConnectionHandler()
    {
    }

void CCchUiNotifConnectionHandler::ConstructL()
    {
    iCmManagerExt.OpenL();
    }

CCchUiNotifConnectionHandler* CCchUiNotifConnectionHandler::NewL()
    {
    CCchUiNotifConnectionHandler* self = NewLC();
    CleanupStack::Pop(self);
    return self;
    }

CCchUiNotifConnectionHandler* CCchUiNotifConnectionHandler::NewLC()
    {
    CCchUiNotifConnectionHandler* self =
        new (ELeave) CCchUiNotifConnectionHandler();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CCchUiNotifConnectionHandler::~CCchUiNotifConnectionHandler()
    {
    iCmManagerExt.Close();
    }

// ---------------------------------------------------------------------------
// Gets GPRS access points from Internet SNAP
// ---------------------------------------------------------------------------
//
TUint32 CCchUiNotifConnectionHandler::GetGprsAccessPointsL( 
    CDesCArray& aIaps, RArray<TUint32>& aIapIds )
    {    
    CCHUIDEBUG( "CCchUiNotifConnectionHandler::GetGprsAccessPointsL - IN" );
    
    TUint32 internetSnapId = KErrNone;
    RArray<TUint32> destIds = RArray<TUint32>( 1 );
    CleanupClosePushL( destIds );
    iCmManagerExt.AllDestinationsL( destIds );
    
    for ( TInt index = 0 ; index < destIds.Count() ; index++ )
        {
        RCmDestinationExt refDestination = 
            iCmManagerExt.DestinationL( destIds[index] );
        CleanupClosePushL( refDestination );    
        
        if ( refDestination.MetadataL( CMManager::ESnapMetadataInternet ) )
            {
            // Get all GPRS access point from this destination
            GetGprsAccessPointsFromSnapL( aIaps, aIapIds, refDestination );
            internetSnapId = refDestination.Id();
            }
        CleanupStack::PopAndDestroy( &refDestination );
        }
    CleanupStack::PopAndDestroy( &destIds );
    
    CCHUIDEBUG( "CCchUiNotifConnectionHandler::GetGprsAccessPointsL - OUT" );
    
    return internetSnapId;
    } 

// ---------------------------------------------------------------------------
// Gets connection name.
// ---------------------------------------------------------------------------
//
void CCchUiNotifConnectionHandler::ConnectionNameL( 
    TUint aIapId, TDesC& aServiceName, RBuf& aIapName )
    {    
    CCHUIDEBUG( "CCchUiNotifConnectionHandler::ConnectionNameL - IN" );
    
    if ( aIapId )
        {
        RCmConnectionMethodExt connMethod = 
        iCmManagerExt.ConnectionMethodL( aIapId );
        CleanupClosePushL( connMethod );
        
        HBufC* connName = connMethod.GetStringAttributeL( 
            CMManager::ECmName );
        CleanupStack::PushL( connName );
                    
        aIapName.CreateL( connName->Des().Length() );
        aIapName.Copy( connName->Des() );
                    
        CleanupStack::PopAndDestroy( connName );              
        CleanupStack::PopAndDestroy( &connMethod );
        }
    else
        {
        RArray<TUint32> destinationIds;
        CleanupClosePushL( destinationIds );
        iCmManagerExt.AllDestinationsL( destinationIds );
        
        for ( TInt i( 0 ) ; i < destinationIds.Count() ; i++ )
            {
            RCmDestinationExt dest = 
                iCmManagerExt.DestinationL( destinationIds[ i ] );
            CleanupClosePushL( dest );

            HBufC* destName = dest.NameLC();
            if ( destName->Des().Compare( aServiceName ) == 0  )
                {
                // Get highest priority connecton method from snap
                RCmConnectionMethodExt cm = dest.ConnectionMethodL( 0 );
                CleanupClosePushL( cm );
                
                HBufC* cmName = NULL;
                cmName = cm.GetStringAttributeL( CMManager::ECmName );
                
                if ( cmName )
                    {
                    CleanupStack::PushL( cmName );
                    aIapName.CreateL( cmName->Des().Length() );
                    aIapName.Copy( cmName->Des() );
                    CleanupStack::PopAndDestroy( cmName );
                    }
                
                CleanupStack::PopAndDestroy( &cm );
                }
            
            CleanupStack::PopAndDestroy( destName );
            CleanupStack::PopAndDestroy( &dest );
            }
        
        CleanupStack::PopAndDestroy( &destinationIds );
        }
    
    CCHUIDEBUG2( "ConnectionNameL - NAME=%S", &aIapName );
        
    CCHUIDEBUG( 
            "CCchUiNotifConnectionHandler::ConnectionNameL - OUT" );
    }

// ---------------------------------------------------------------------------
// Gets all but specified service´s destinations.
// ---------------------------------------------------------------------------
//
void CCchUiNotifConnectionHandler::GetDestinationsL( 
    TDesC& aServiceName, RArray<TUint32>& aDestinationIds )
    {  
    CCHUIDEBUG( "CCchUiNotifConnectionHandler::GetDestinationsL - IN" );
    
    iCmManagerExt.AllDestinationsL( aDestinationIds );
    
    // Remove our services destination from destination ids.
    for ( TInt i( 0 ) ; i < aDestinationIds.Count() ; i++ )
        {
        RCmDestinationExt refDestination = 
        iCmManagerExt.DestinationL( aDestinationIds[ i ] );
        CleanupClosePushL( refDestination );
               
        HBufC* destName = refDestination.NameLC();
               
        if ( destName->Des().Compare( aServiceName ) == 0 )
            {
            CCHUIDEBUG( 
                "GetDestinationsL - remove current service´s destination" );
            
            aDestinationIds.Remove( i );
            aDestinationIds.Compress();
            }
               
        CleanupStack::PopAndDestroy( destName );      
        CleanupStack::PopAndDestroy( &refDestination );
        }
    
    CCHUIDEBUG( "CCchUiNotifConnectionHandler::GetDestinationsL - OUT" );
    }

// ---------------------------------------------------------------------------
// Gets all access point ids from SNAP.
// ---------------------------------------------------------------------------
//
void CCchUiNotifConnectionHandler::GetAccessPointsFromSnapL( 
    RArray<TUint32>& aIapIds, TUint32 aDestinationId )
    {    
    CCHUIDEBUG( 
        "CCchUiNotifConnectionHandler::GetAccessPointsFromSnapL - IN" );

    CleanupClosePushL(aIapIds);
    
    RCmDestinationExt destination = 
                iCmManagerExt.DestinationL( aDestinationId );
    CleanupClosePushL( destination );
    
    for ( TInt index = 0 ; 
        index < destination.ConnectionMethodCount() ; index++ )
        {
        RCmConnectionMethodExt connMethod = 
            destination.ConnectionMethodL( index );
        CleanupClosePushL( connMethod );      
        TUint32 iapId = connMethod.GetIntAttributeL( CMManager::ECmIapId );
        aIapIds.AppendL( iapId );
        CleanupStack::PopAndDestroy( &connMethod );
        }

    CleanupStack::PopAndDestroy( &destination );
    CleanupStack::Pop();
    
    CCHUIDEBUG( 
        "CCchUiNotifConnectionHandler::GetAccessPointsFromSnapL - OUT" );
    }

// ---------------------------------------------------------------------------
// Gets GPRS access points from Internet SNAP
// ---------------------------------------------------------------------------
//
void CCchUiNotifConnectionHandler::GetGprsAccessPointsFromSnapL( 
    CDesCArray& aIaps, 
    RArray<TUint32>& aIapIds, 
    RCmDestinationExt& aDestination )
    {    
    CCHUIDEBUG( 
        "CCchUiNotifConnectionHandler::GetGprsAccessPointsFromSnapL - IN" );

    __ASSERT_ALWAYS( aDestination.ConnectionMethodCount(), 
        User::Leave( KErrNotFound ) );

    for ( TInt index = 0 ; 
        index < aDestination.ConnectionMethodCount() ; index++ )
        {
        RCmConnectionMethodExt connMethod = 
            aDestination.ConnectionMethodL( index );
        CleanupClosePushL( connMethod );
        
        if ( KUidPacketDataBearerType == connMethod.GetIntAttributeL( 
                CMManager::ECmBearerType ) )
            {
            HBufC* connName = 
                connMethod.GetStringAttributeL( CMManager::ECmName );
            CleanupStack::PushL( connName );
            
            CCHUIDEBUG( "GetGprsAccessPointsFromSnapL - iap name ok" );
            
            aIaps.AppendL( *connName );
            CleanupStack::PopAndDestroy( connName );    
            TUint32 iapId = connMethod.GetIntAttributeL( CMManager::ECmIapId );
            aIapIds.AppendL( iapId );
            
            CCHUIDEBUG2( "GetGprsAccessPointsFromSnapL - iap id: %d", iapId );
            }
        CleanupStack::PopAndDestroy( &connMethod );
        }

    CCHUIDEBUG( 
        "CCchUiNotifConnectionHandler::GetGprsAccessPointsFromSnapL - OUT" );
    }
