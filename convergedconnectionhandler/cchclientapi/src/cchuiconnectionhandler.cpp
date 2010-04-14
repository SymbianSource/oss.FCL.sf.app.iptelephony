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
* Description:  Provider WLAN search and SNAP handling functionality
*
*/


// Search WLAN
#include <cmmanagerdef.h>
#include <cmpluginwlandef.h> // bearer type
#include <cmdestinationext.h>
#include <cmconnectionmethoddef.h>
#include <ConnectionUiUtilities.h>
#include <cmconnectionmethodext.h>
#include <WEPSecuritySettingsUI.h>
#include <WPASecuritySettingsUI.h>

#include "cchuilogger.h"
#include "cchuicchhandler.h"
#include "cchuispshandler.h"
#include "cchuiconnectionhandler.h"

const TInt KSsidLength = 256;
const TInt KCmHighestPriority = 0;
const TInt KDestinationArrayGranularity = 10;


// ======== MEMBER FUNCTIONS ========

CCchUiConnectionHandler::CCchUiConnectionHandler(
    CCchUiCchHandler& aCchHandler,
    CCchUiSpsHandler& aSpsHandler ):
    iCCHHandler( aCchHandler ),
    iSpsHandler( aSpsHandler ),
    iSearchWlanOngoing( EFalse )
    {
    }

void CCchUiConnectionHandler::ConstructL()
    {
    iConnUiUtils = CConnectionUiUtilities::NewL();
    iCmManagerExt.OpenL();
    User::LeaveIfError( iConnMon.ConnectL() );
    }

CCchUiConnectionHandler* CCchUiConnectionHandler::NewL(
    CCchUiCchHandler& aCchHandler,
    CCchUiSpsHandler& aSpsHandler )
    {
    CCchUiConnectionHandler* self = NewLC( 
        aCchHandler, aSpsHandler );
    CleanupStack::Pop(self);
    return self;
    }

CCchUiConnectionHandler* CCchUiConnectionHandler::NewLC(
    CCchUiCchHandler& aCchHandler,
    CCchUiSpsHandler& aSpsHandler )
    {
    CCchUiConnectionHandler* self =
        new (ELeave) CCchUiConnectionHandler( 
            aCchHandler, aSpsHandler );
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CCchUiConnectionHandler::~CCchUiConnectionHandler()
    {
    delete iConnUiUtils;
    iCmManagerExt.Close();
    iConnMon.Close();
    }

// ---------------------------------------------------------------------------
// Searches access poinsts and adds selected access point to snap
// ---------------------------------------------------------------------------
//
void CCchUiConnectionHandler::SearchAccessPointsL( 
    TUint aServiceId, 
    TUint32 aSnapId, 
    const TDesC& aServiceName )
    {    
    CCHUIDEBUG( "CCchUiConnectionHandler::SearchAccessPointsL - IN" );
    
    // SSID of the WLAN network selected by user
    TWlanSsid ssid; 
    
    // Connection mode of the WLAN network
    TWlanConnectionMode connectionMode; 
    
    // Security mode of the WLAN network
    TWlanConnectionSecurityMode securityMode; 
    
    // Flag for determining if hidden wlan
    TBool hiddenWlan( EFalse );
    
    CCHUIDEBUG( 
        "CCchUiConnectionHandler::SearchAccessPointsL - begin search wlan" );
        
    // Set flag which indicates that search wlan query is open
    iSearchWlanOngoing = ETrue;
    
    TBool ret = iConnUiUtils->SearchWLANNetwork( 
        ssid, connectionMode, securityMode );
        
    iSearchWlanOngoing = EFalse;
    
    if ( !ret )
        {        
        //ret is ETrue if user pressed OK softkey. Otherwise leave
        //with cancel
        User::Leave( KErrCancel );
        }
    
    CCHUIDEBUG( 
      "CCchUiConnectionHandler::SearchAccessPointsL - search wlan finished" ); 
    
    // Check if hidden wlan
    TUint32 easyWlanId( 0 );
    easyWlanId = iCmManagerExt.EasyWlanIdL();
    
    if ( easyWlanId )
        {
        RCmConnectionMethodExt cmConnMethodExt = 
            iCmManagerExt.ConnectionMethodL( easyWlanId );
        CleanupClosePushL( cmConnMethodExt );
        
        if ( cmConnMethodExt.GetBoolAttributeL( CMManager::EWlanScanSSID ) )
            {
        CCHUIDEBUG( 
             "CCchUiConnectionHandler::SearchAccessPointsL - hidden wlan" );
        
            hiddenWlan = ETrue;
            }
        
        CleanupStack::PopAndDestroy( &cmConnMethodExt );
        }
    
    HBufC* ssid16 = HBufC::NewL( KSsidLength );
    CleanupStack::PushL( ssid16 );
    
    TPtr ssid16Ptr( ssid16->Des() );
    ssid16Ptr.Copy( ssid );

    //  Check if the snap returned by Cch really exists.
    if ( aSnapId )
        {
        RArray<TUint32> destIds = RArray<TUint32>( 1 );
        CleanupClosePushL( destIds );
        iCmManagerExt.AllDestinationsL( destIds );
        
        TBool found( EFalse );
        for ( TInt index = 0 ; index < destIds.Count() ; index++ )
            {
            if ( destIds[index] == aSnapId )
                {
                found = ETrue;
                }
            }
        CleanupStack::PopAndDestroy( &destIds );
        
        // Reset snap id if the snap doesn't exist.
        if ( !found )
            {
            aSnapId = 0;
            }
        }

    if ( !aSnapId )
        {
        RCmDestinationExt newDestination = 
            CreateServiceSnapL( aServiceName );
        CleanupClosePushL( newDestination );
        
        TBool alreadyExists = EFalse;

        TInt iapId = AddNewConnectionMethodL( 
            newDestination, 
            *ssid16, 
            securityMode,
            alreadyExists,
            hiddenWlan );
        
        if ( KErrCancel == iapId )
            {
            //Leave with cancel if user pressed Cancel softkey
            CleanupStack::PopAndDestroy( &newDestination );
            CleanupStack::PopAndDestroy( ssid16 );
            User::Leave( KErrCancel );
            }

        if ( !alreadyExists )
            {
            iSpsHandler.SetTemporaryIapIdL( aServiceId, iapId );
            }
        
        // Now the snap is created, set it to use with cch
        SetSnapToUseL( aServiceId, newDestination.Id() );
        
        // Self created, write id to service table in order to remove
        // in service uninstall
        TRAP_IGNORE( iSpsHandler.SetSnapIdL( 
            aServiceId, newDestination.Id() ) );
        
        CleanupStack::PopAndDestroy( &newDestination );
        CleanupStack::PopAndDestroy( ssid16 );
        }
    else
        {
        RCmDestinationExt destination = 
            iCmManagerExt.DestinationL( aSnapId );
        CleanupClosePushL( destination );
        
        TBool alreadyExists = EFalse;
        TInt iapId = AddNewConnectionMethodL( 
            destination,
            *ssid16,
            securityMode,
            alreadyExists, 
            hiddenWlan );
        
        CleanupStack::PopAndDestroy( &destination );
        CleanupStack::PopAndDestroy( ssid16 );
        
        if ( KErrCancel == iapId )
            {
            //Leave with cancel if user pressed Cancel softkey
            User::Leave( KErrCancel );
            }

        if ( !alreadyExists )
            {
            iSpsHandler.SetTemporaryIapIdL( 
                aServiceId, iapId );
            }
        else
            {
            // Get current connection iap id
            TInt currentIapId( KErrNone );
            TInt error( KErrNone );
            iCCHHandler.GetCurrentConnectionIapIdL(
                aServiceId, ECCHUnknown, currentIapId, error );
            
            CCHUIDEBUG2( "CCchUiConnectionHandler::SearchAccessPointsL - error:            %d", error );
            CCHUIDEBUG2( "CCchUiConnectionHandler::SearchAccessPointsL - current iap id:   %d", currentIapId );
            CCHUIDEBUG2( "CCchUiConnectionHandler::SearchAccessPointsL - selected iap id:  %d", iapId );
            
            // if selected iap is same as current iap no need to continue
            // because if we continue there will be unnecessary disable enable loop
            if ( currentIapId == iapId )
                {
                User::Leave( KErrCancel );
                }
            }
        
        SetSnapToUseL( aServiceId, aSnapId ); 
        }
    
    // Reset Easy Wlan EWlanScanSSID parameter to EFalse
    if ( easyWlanId && hiddenWlan )
        {
        RCmConnectionMethodExt cmConnMethodExt = 
            iCmManagerExt.ConnectionMethodL( easyWlanId );
        CleanupClosePushL( cmConnMethodExt );
        cmConnMethodExt.SetBoolAttributeL( CMManager::EWlanScanSSID, EFalse );
        cmConnMethodExt.UpdateL();
        CleanupStack::PopAndDestroy( &cmConnMethodExt );
        }
    
    CCHUIDEBUG( "CCchUiConnectionHandler::SearchAccessPointsL - OUT" );
    } 

// ---------------------------------------------------------------------------
// Check if connnection method already exists. If exists, connection method
// id is returned, othewise KErrNotFound.
// ---------------------------------------------------------------------------
//
TInt CCchUiConnectionHandler::ConnectionMethodAlreadyExistsL( 
    RCmDestinationExt& aDestination,
    const TDesC& aSsid,
    CMManager::TWlanSecMode aSecurityMode,
    TBool& aAlreadyExists,
    TBool aHidden )
    {
    CCHUIDEBUG( "CCchUiConnectionHandler::ConnectionMethodAlreadyExistsL" );
    
    TInt connectionMethodId = KErrNotFound;
    for ( TInt i=0 ; 
          i < aDestination.ConnectionMethodCount() && !aAlreadyExists ; 
          i++ )
        {
        HBufC* refSsid( NULL );
        
        RCmConnectionMethodExt cm = aDestination.ConnectionMethodL(i);
        CleanupClosePushL( cm );
        
        // If snap contains non wlan iaps, this will leave so trap
        TRAPD( err, refSsid = cm.GetStringAttributeL( CMManager::EWlanSSID ));
        if ( refSsid && !err )
            {
            CleanupStack::PushL( refSsid );
            
            if ( refSsid->Compare( aSsid ) == 0 )
                {
                CCHUIDEBUG( "Matching wlan SSID Found" );
                
                // check security and hidden
                TBool refHidden = cm.GetBoolAttributeL( 
                   CMManager::EWlanScanSSID );
                TUint refSecurity = cm.GetIntAttributeL( 
                    CMManager::EWlanSecurityMode );
                
                // ref AP security mode is stored as EWlanSecModeWpa in case of EWlanSecModeWpa2
                TBool exception( EFalse ); 
                
                if ( aSecurityMode == CMManager::EWlanSecModeWpa2 && 
                     refSecurity == CMManager::EWlanSecModeWpa )
                    {
                    exception = ETrue;
                    }
                
                if ( refHidden == aHidden &&
                    ( refSecurity == aSecurityMode || exception ) )
                    {              
                    connectionMethodId = 
                        cm.GetIntAttributeL( CMManager::ECmIapId );
                    aAlreadyExists = ETrue;
                    }
                }
            CleanupStack::PopAndDestroy( refSsid );
            }
        else if ( KErrNoMemory == err )
            {
            User::Leave( err );
            }
        else
            {
            // for note from code analysis tool
            }
        CleanupStack::PopAndDestroy( &cm );
        }
    
    CCHUIDEBUG2( "ConnectionMethodAlreadyExistsL connection method id=%d", 
         connectionMethodId );
    
    return connectionMethodId;
    }

// ---------------------------------------------------------------------------
// Add new connection method.
// ---------------------------------------------------------------------------
//
TInt CCchUiConnectionHandler::AddNewConnectionMethodL( 
    RCmDestinationExt& aDestination, 
    const TDesC& aSsid,
    TWlanConnectionSecurityMode aSecurityMode,
    TBool& aAlreadyExists,
    TBool aSetHidden )
    {
    CCHUIDEBUG( "CCchUiConnectionHandler::AddNewConnectionMethodL" );
    
    CMManager::TWlanSecMode securityModeToSet = CMManager::EWlanSecModeOpen;
    switch ( aSecurityMode )
        {
        case EWlanConnectionSecurityOpen:
            {
            securityModeToSet = CMManager::EWlanSecModeOpen;
            }
            break;
        
        case EWlanConnectionSecurityWep:
            {
            securityModeToSet = CMManager::EWlanSecModeWep;
            }
            break;
        
        case EWlanConnectionSecurity802d1x:
            {
            securityModeToSet = CMManager::EWlanSecMode802_1x;
            }
            break;
        
        case EWlanConnectionSecurityWpa:
            {
            securityModeToSet = CMManager::EWlanSecModeWpa;
            }
            break;
        
        case EWlanConnectionSecurityWpaPsk:
            {
            securityModeToSet = CMManager::EWlanSecModeWpa2;
            }
            break;
            
        default:
            break;
        }    
    
    // Check is iap with same ssid already exists
    RArray<TUint32> destArray;
    CleanupClosePushL( destArray );
    
    iCmManagerExt.AllDestinationsL( destArray );
    
    for ( TInt i( 0 ); i < destArray.Count(); i++ )
        {
        RCmDestinationExt destination =
            iCmManagerExt.DestinationL( destArray[ i ] );
        CleanupClosePushL( destination );
        
        TInt connectionId = ConnectionMethodAlreadyExistsL( 
            destination, aSsid, securityModeToSet, aAlreadyExists, aSetHidden );
        
        if ( aAlreadyExists && KErrNotFound != connectionId )
           {
           RCmConnectionMethodExt connectionMethod =
               destination.ConnectionMethodByIDL( connectionId );
           CleanupClosePushL( connectionMethod );
           if ( destination.Id() != aDestination.Id() )
               {
               CCHUIDEBUG( "Copy existing connection method to destination" );
               
               aDestination.AddConnectionMethodL( 
                   connectionMethod.CreateCopyL() );
               }
           TRAP_IGNORE( aDestination.ModifyPriorityL( 
               connectionMethod, KCmHighestPriority ) );
           aDestination.UpdateL();
           
           CleanupStack::PopAndDestroy( &connectionMethod );
           CleanupStack::PopAndDestroy( &destination );
           CleanupStack::PopAndDestroy( &destArray );
           return connectionId;
           }
        CleanupStack::PopAndDestroy( &destination );
        }
    CleanupStack::PopAndDestroy( &destArray );
    
    TInt ret = KErrNone;
    
    RCmConnectionMethodExt newConnMethod = 
        aDestination.CreateConnectionMethodL( KUidWlanBearerType );
    CleanupClosePushL( newConnMethod );
    
    // Set attributes
    newConnMethod.SetStringAttributeL( CMManager::ECmName, aSsid );
    newConnMethod.SetStringAttributeL( CMManager::EWlanSSID, aSsid );
    
    if ( aSetHidden )
        {
        newConnMethod.SetBoolAttributeL( 
            CMManager::EWlanScanSSID, ETrue );
        }
    
    newConnMethod.SetIntAttributeL( 
        CMManager::EWlanSecurityMode, securityModeToSet );

    TRAP_IGNORE( aDestination.ModifyPriorityL( 
        newConnMethod, KCmHighestPriority ));
    
    aDestination.UpdateL();
    ret = newConnMethod.GetIntAttributeL( CMManager::ECmIapId );

    CConnectionUiUtilities* connUiUtils = CConnectionUiUtilities::NewL();
    CleanupStack::PushL( connUiUtils );
    
    TInt createdUid( 0 );
    createdUid = newConnMethod.GetIntAttributeL( CMManager::EWlanServiceId );

    HBufC* key = HBufC::NewL( KWlanWpaPskMaxLength );
    CleanupStack::PushL( key );
    TPtr keyPtr( key->Des() );
    
    TBool hex = EFalse;  
    
    if ( securityModeToSet == CMManager::EWlanSecModeWep )    
        {
        if ( connUiUtils->EasyWepDlg( &keyPtr, hex ) )
            {
            SaveSecuritySettingsL( aSecurityMode, key, hex, createdUid );
            }
        else
            {
            aDestination.DeleteConnectionMethodL( newConnMethod );
            aDestination.UpdateL();
            
            ret = KErrCancel;
            }
        }
    else if ( securityModeToSet == CMManager::EWlanSecModeWpa || 
              securityModeToSet == CMManager::EWlanSecModeWpa2 )
        {
        if ( connUiUtils->EasyWpaDlg( &keyPtr ) )
            {
            SaveSecuritySettingsL( aSecurityMode, key, hex, createdUid );
            }
        else
            {
            aDestination.DeleteConnectionMethodL( newConnMethod );
            aDestination.UpdateL();
            
            ret = KErrCancel;
            }
        }
    else
        {
        CCHUIDEBUG( "AddNewConnectionMethodL - WlanConnectionSecurityOpen" );
        }
    CleanupStack::PopAndDestroy( key );          
    CleanupStack::PopAndDestroy( connUiUtils );   
    CleanupStack::PopAndDestroy( &newConnMethod );
    
    CCHUIDEBUG2( "AddNewConnectionMethodL - return: %d", ret );
    return ret;
    }

// ---------------------------------------------------------------------------
// Save security settings.
// ---------------------------------------------------------------------------
//
void CCchUiConnectionHandler::SaveSecuritySettingsL(
    const TWlanConnectionSecurityMode aSecMode, 
    const HBufC* aKey, 
    const TBool aHex, 
    const TUint32 aIapId )
    {
    CCHUIDEBUG( "CCchUiConnectionHandler::SaveSecuritySettingsL - IN" );
    
    CMDBSession* db = CMDBSession::NewL( CMDBSession::LatestVersion() );
    CleanupStack::PushL( db );
    switch ( aSecMode )
        {
        case EWlanConnectionSecurityWep:
            {
            CWEPSecuritySettings* wepSecSettings = 
                CWEPSecuritySettings::NewL();
            CleanupStack::PushL( wepSecSettings );            
            wepSecSettings->LoadL( aIapId, *db );            
            User::LeaveIfError( 
                wepSecSettings->SetKeyDataL( 0, *aKey, aHex ) );
            wepSecSettings->SaveL( aIapId, *db );             
            CleanupStack::PopAndDestroy( wepSecSettings ); 
            break;
            }
        case EWlanConnectionSecurityWpaPsk:
            {   
            CWPASecuritySettings* wpaSecSettings = 
                CWPASecuritySettings::NewL( ESecurityModeWpa );
            CleanupStack::PushL( wpaSecSettings );           
            wpaSecSettings->LoadL( aIapId, *db );            
            User::LeaveIfError( wpaSecSettings->SetWPAPreSharedKey( *aKey ) );
            wpaSecSettings->SaveL( aIapId, *db, ESavingBrandNewAP, 0 ) ;            
            CleanupStack::PopAndDestroy( wpaSecSettings );         
            break;                     
            }
        case EWlanConnectionSecurityWpa:            
        case EWlanConnectionSecurity802d1x: 
            {
            CWPASecuritySettings* wpaSecSettings = 
                    CWPASecuritySettings::NewL( 
                        aSecMode == EWlanConnectionSecurityWpa ? 
                        ESecurityModeWpa : ESecurityMode8021x );
            CleanupStack::PushL( wpaSecSettings );            
            wpaSecSettings->LoadL( aIapId, *db );            
            wpaSecSettings->SaveL( aIapId, *db, ESavingBrandNewAP, 0 );             
            CleanupStack::PopAndDestroy( wpaSecSettings );   
            break;
            }
        default:
            // Fore example EWlanConnectionSecurityOpen -> no need to save
            // any security settings.
            break;
        }
    CleanupStack::PopAndDestroy( db ); // db
    
    CCHUIDEBUG( "CCchUiConnectionHandler::SaveSecuritySettingsL - OUT" );
    }

// ---------------------------------------------------------------------------
// Sets SNAP into use via CCH. 
// ---------------------------------------------------------------------------
//
void CCchUiConnectionHandler::SetSnapToUseL( 
    TUint aServiceId, TUint32 aSNAPId )
    {
    CCHUIDEBUG( "CCchUiConnectionHandler::SetSnapToUseL - IN" );
    
    CCHUIDEBUG2( "SetSnapToUseL - aServiceId: %d", aServiceId );
    CCHUIDEBUG2( "SetSnapToUseL - aSNAPId: %d", aSNAPId );
    
    TInt err( KErrNone );
    iCCHHandler.SetConnectionSnapIdL( aServiceId, aSNAPId, err );
    
    if ( err )
        {
        User::Leave( err );
        }
    
    CCHUIDEBUG( "CCchUiConnectionHandler::SetSnapToUseL - OUT" );
    } 
    
// ---------------------------------------------------------------------------
// Copies specific iap from specific snap to target snap
// ---------------------------------------------------------------------------
//
void CCchUiConnectionHandler::CopyIapToServiceSnapL( 
    TUint aServiceId, 
    const TDesC& aServiceName, 
    TUint32 aSourceIap, 
    TUint32 aTargetSnap )
    {
    CCHUIDEBUG( "CCchUiConnectionHandler::CopyIapToServiceSnapL - IN" );
    
    CCHUIDEBUG3( "CopyIapToServiceSnapL - aSourceIap: %d, aTargetSnap: %d", 
        aSourceIap, aTargetSnap );
    
    RCmDestinationExt targetSnap;
    TRAPD( err, ( targetSnap = iCmManagerExt.DestinationL( aTargetSnap ) ) );
    CCHUIDEBUG2( " -> get target snap err: %d", err );
    if ( err )
        {
        CCHUIDEBUG( "CopyIapToServiceSnapL - Create snap for service");
        
        targetSnap = CreateServiceSnapL( aServiceName );
        CleanupClosePushL( targetSnap );
        // Now the snap is created, set it to use with cch
        SetSnapToUseL( aServiceId, targetSnap.Id() );
        }
    else
        {
        CleanupClosePushL( targetSnap );
        }
    CCHUIDEBUG( "CopyIapToServiceSnapL - Get source connection");    
    
    RCmConnectionMethodExt sourceConn = 
        iCmManagerExt.ConnectionMethodL( aSourceIap );       
    CleanupClosePushL( sourceConn );
    
    TInt conMethodCount = targetSnap.ConnectionMethodCount();
    TUint32 sourceIapId = sourceConn.GetIntAttributeL( CMManager::ECmIapId );
    TBool matchFound( EFalse );
    
    for ( TInt ndx = 0 ; ndx < conMethodCount && matchFound == 0; ndx ++ )
        {
        RCmConnectionMethodExt cm = targetSnap.ConnectionMethodL( ndx );
        CleanupClosePushL( cm );
        
        TUint32 targetIapId = cm.GetIntAttributeL( CMManager::ECmIapId );
        
        if( targetIapId == sourceIapId )
            {
            matchFound = ETrue;
            }
        CleanupStack::PopAndDestroy( &cm );
        }
    if( !matchFound )
        {
        CCHUIDEBUG( 
          "CopyIapToServiceSnapL - Get source connection ok -> add connection");
        
        targetSnap.AddConnectionMethodL( sourceConn.CreateCopyL() );
        }
    CleanupStack::PopAndDestroy( &sourceConn );
    CleanupStack::PopAndDestroy( &targetSnap );      
    
    CCHUIDEBUG( "CCchUiConnectionHandler::CopyIapToServiceSnapL - OUT" );
    }     
    
// ---------------------------------------------------------------------------
// Removes connection method from SNAP.
// ---------------------------------------------------------------------------
//
void CCchUiConnectionHandler::RemoveConnectionL( 
    const TDesC& aServiceName, TInt aIapId )
    {
    CCHUIDEBUG( "CCchUiConnectionHandler::RemoveConnectionL - IN" );
    CCHUIDEBUG2( "RemoveConnectionL - aServiceName=%S", &aServiceName );
    CCHUIDEBUG2( "RemoveConnectionL - aIapId=%d", aIapId );
    
    RArray<TUint32> destIds = RArray<TUint32>( 1 );
    CleanupClosePushL( destIds );
    iCmManagerExt.AllDestinationsL( destIds );
    
    for ( TInt i( 0 ) ; i < destIds.Count() ; i++ )
        {
        RCmDestinationExt dest = iCmManagerExt.DestinationL( destIds[ i ] );
        CleanupClosePushL( dest );
        
        HBufC* destName = dest.NameLC();        
        if ( destName->Des().Compare( aServiceName ) == 0 )
            {
            CCHUIDEBUG( "RemoveConnectionL - get connection method");
            
            RCmConnectionMethodExt connMethod = 
                   iCmManagerExt.ConnectionMethodL( aIapId );        
            
            CCHUIDEBUG( "RemoveConnectionL - remove connection method");
            
            dest.RemoveConnectionMethodL( connMethod );
            dest.UpdateL();
            
            CCHUIDEBUG( "RemoveConnectionL - connection method removed");
            }
        
        CleanupStack::PopAndDestroy( destName );
        CleanupStack::PopAndDestroy( &dest );
        }
    
    CleanupStack::PopAndDestroy( &destIds );
    
    CCHUIDEBUG( "CCchUiConnectionHandler::RemoveConnectionL - OUT" );
    }

// ---------------------------------------------------------------------------
// Returns ETrue if wlan search (query) is ongoing.
// ---------------------------------------------------------------------------
//
TBool CCchUiConnectionHandler::SearchWlanOngoing()
    {    
    CCHUIDEBUG2( "CCchUiConnectionHandler::SearchWlanOngoing ongoing=%d",
        iSearchWlanOngoing );
    
    return iSearchWlanOngoing;
    }


// ---------------------------------------------------------------------------
// Creates service snap
// ---------------------------------------------------------------------------
//
RCmDestinationExt CCchUiConnectionHandler::CreateServiceSnapL(
    const TDesC& aServiceName )
    {
    CCHUIDEBUG( "CCchUiConnectionHandler::CreateServiceSnapL" );

    // Create snap again, user might have deleted it
    RCmDestinationExt newDestination;
    
    // Check if snap with service name already exists.
    TBool snapAlreadyExists = EFalse;
    RArray<TUint32> destIdArray = 
        RArray<TUint32>( KDestinationArrayGranularity );
    CleanupClosePushL( destIdArray );
    
    CCHUIDEBUG( "CreateServiceSnapL - check if snap exists");
    
    iCmManagerExt.AllDestinationsL( destIdArray );
    
    CCHUIDEBUG2( "CreateServiceSnapL - destination count: %d", 
        destIdArray.Count() );
    
    for ( TInt i = 0; 
          i < destIdArray.Count() && !snapAlreadyExists; 
          i++ )
        {
        RCmDestinationExt dest = 
            iCmManagerExt.DestinationL( destIdArray[i] );
        CleanupClosePushL( dest );
        HBufC* destName = dest.NameLC();
        if ( *destName == aServiceName )
            {
            CCHUIDEBUG( "CreateServiceSnapL - snap already exists" );
            snapAlreadyExists = ETrue;
            newDestination = iCmManagerExt.DestinationL( destIdArray[i] );
            }
        CleanupStack::PopAndDestroy( destName );
        CleanupStack::PopAndDestroy( &dest );
        }
    CleanupStack::PopAndDestroy( &destIdArray );                
    
    if ( !snapAlreadyExists )
        {
        newDestination = 
            iCmManagerExt.CreateDestinationL( aServiceName );
        CleanupClosePushL( newDestination );
        newDestination.SetMetadataL( 
            CMManager::ESnapMetadataHiddenAgent, ETrue );
        newDestination.UpdateL();    
        CleanupStack::Pop( &newDestination );
        }
    
    return newDestination;
    }

