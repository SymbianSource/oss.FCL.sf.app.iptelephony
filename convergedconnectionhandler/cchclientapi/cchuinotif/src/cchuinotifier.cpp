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
* Description:  Implements base class for notifier implementations
*
*/


#include <eikenv.h>
#include <bautils.h>
#include <featmgr.h>
#include <spentry.h> 
#include <spsettings.h>
#include <spproperty.h>
#include <cchuinotif.rsg>
#include <StringLoader.h>
#include <aknnotewrappers.h>
#include <centralrepository.h>
#include <cenrepnotifyhandler.h>
#include <CoreApplicationUIsSDKCRKeys.h> // KCRUidCoreApplicationUIs, 
                                         // KCoreAppUIsNetworkConnectionAllowed
#include "cchuilogger.h"
#include "cchuinotifier.h"
#include "cchuinotifconnectionhandler.h"

_LIT( KResourceDir, "\\resource\\apps\\" );
_LIT( KResourceFileName, "cchuinotif.r*" );

// SCP-plugin uid for Sip VoIP
const TInt32 KSipVoIPSubServicePlugId = 0x1027545A;

// ======== MEMBER FUNCTIONS ========

CCCHUiNotifierBase::CCCHUiNotifierBase(): CActive(EPriorityStandard)
    {
    CActiveScheduler::Add(this);
    }

void CCCHUiNotifierBase::ConstructL()
    {      
    CCHUIDEBUG( "CCCHUiNotifierBase::ConstructL - IN" );
    
    iEikEnv = CEikonEnv::Static();

    // load the resource file
    CCHUIDEBUG( "ConstructL - load resource");
   
    HBufC* resourceFile = ScanFileL( iEikEnv->FsSession(), 
        TFileName( KResourceFileName ), TFileName( KResourceDir ) );
    
    if ( resourceFile )
        {
        CCHUIDEBUG( "ConstructL - resource ready, adding");   
        
        CleanupStack::PushL( resourceFile );
        iResourceFileFlag = iEikEnv->AddResourceFileL(
            *resourceFile );    
        CleanupStack::PopAndDestroy( resourceFile );
        
        CCHUIDEBUG( "ConstructL - resource added");    
        }
    
    iSettings = CSPSettings::NewL();
    
    CCHUIDEBUG( "CCCHUiNotifierBase::ConstructL - OUT" );
    }

CCCHUiNotifierBase::~CCCHUiNotifierBase()
    {
    CCHUIDEBUG( "CCCHUiNotifierBase::~CCCHUiNotifierBase - IN" );
    Cancel();    
    
    delete iSettings;
    CompleteMessage( KErrDied );  
    iEikEnv->DeleteResourceFile( iResourceFileFlag );
    
    CCHUIDEBUG( "CCCHUiNotifierBase::~CCCHUiNotifierBase - OUT" );
    }

// ---------------------------------------------------------------------------
// CompleteMessage.
// ---------------------------------------------------------------------------
//
void CCCHUiNotifierBase::CompleteMessage( TInt aErr )
    {      
    CCHUIDEBUG2( "CCCHUiNotifierBase::CompleteMessage - err: %d", aErr );
    
    if( !iMessage.IsNull() )
        {
        CCHUIDEBUG( "CompleteMessage - do complete");
        iMessage.Complete( aErr );
        }
    
    iReplySlot = NULL;                    
    }

// ---------------------------------------------------------------------------
// From class MEikSrvNotifierBase2.
// Cancelling method.
// ---------------------------------------------------------------------------
//
void CCCHUiNotifierBase::Cancel()
    {   
    CCHUIDEBUG( "CCCHUiNotifierBase::Cancel - IN" );
        
    // Complete with KErrCancel
    CompleteMessage(KErrCancel);        
    CActive::Cancel();
    
    CCHUIDEBUG( "CCCHUiNotifierBase::Cancel - OUT" );
    }

// ---------------------------------------------------------------------------
// RunL
// From CActive.
// ---------------------------------------------------------------------------
//
void CCCHUiNotifierBase::RunL(  )
    {
    CCHUIDEBUG( "CCCHUiNotifierBase::RunL" );
    }

// ---------------------------------------------------------------------------
// From class MEikSrvNotifierBase2.
// Release itself. Call to destructor.
// ---------------------------------------------------------------------------
//
void CCCHUiNotifierBase::Release()
    { 
    CCHUIDEBUG( "CCCHUiNotifierBase::Release");
    delete this;
    }

// ---------------------------------------------------------------------------
// From class MEikSrvNotifierBase2.
// Return registered information.
// ---------------------------------------------------------------------------
//
CCCHUiNotifierBase::TNotifierInfo CCCHUiNotifierBase::Info() const
    {      
    CCHUIDEBUG( "CCCHUiNotifierBase::Info");
    return iInfo;
    }

// ---------------------------------------------------------------------------
// From class MEikSrvNotifierBase2.
// Synchronic notifier launch. Does nothing here.
// ---------------------------------------------------------------------------
//
TPtrC8 CCCHUiNotifierBase::StartL( const TDesC8& /*aBuffer*/ )
    {    
    CCHUIDEBUG( "CCCHUiNotifierBase::StartL");
    TPtrC8 ret( KNullDesC8 );
    return ret;
    }

// ---------------------------------------------------------------------------
// From class MEikSrvNotifierBase2.
// Asynchronic notifier launch.
// ---------------------------------------------------------------------------
//
void CCCHUiNotifierBase::StartL( 
    const TDesC8& aBuffer, 
    TInt aReplySlot, 
    const RMessagePtr2& aMessage )
    {
    CCHUIDEBUG( "CCCHUiNotifierBase::StartL2 - IN");
    iMessage = aMessage;
    iReplySlot = aReplySlot;
    
    SetActive();
    
    iStatus = KRequestPending;
    TRequestStatus* stat = &iStatus;
    User::RequestComplete(stat, KErrNone);

    CCHUIDEBUG( "CCCHUiNotifierBase::StartL2 - OUT");
    }

// ---------------------------------------------------------------------------
// From class MEikSrvNotifierBase2.
// Notifier update. Not supported.
// ---------------------------------------------------------------------------
//
TPtrC8 CCCHUiNotifierBase::UpdateL( const TDesC8& /*aBuffer*/ )
    {
    CCHUIDEBUG( "CCCHUiNotifierBase::UpdateL");
    TPtrC8 ret( KNullDesC8 );
    return ret;
    }
    
// ----------------------------------------------------------
// CCCHUiNotifierBase::DoCancel
// This method will be called by framework (CActive)
// if active object is still active.
// Does nothing here.
// ----------------------------------------------------------
//
void CCCHUiNotifierBase::DoCancel()
    {
    CCHUIDEBUG( "CCCHUiNotifierBase::DoCancel - IN");
    
    // Complete request when active object is still active
    TRequestStatus* stat = &iStatus;
    if (iStatus != KErrCancel)
        {
        User::RequestComplete(stat, KErrNone);
        }
        
    CCHUIDEBUG( "CCCHUiNotifierBase::DoCancel - OUT");
    }

// -----------------------------------------------------------------------------
// Scan resource file.
// -----------------------------------------------------------------------------
//
HBufC* CCCHUiNotifierBase::ScanFileL( 
    RFs& aSession, 
    const TDesC& aFileName, 
    const TDesC& aFilePath )
    {
    CCHUIDEBUG( "CCCHUiNotifierBase::ScanFileL");     
    
    TFindFile search( aSession ); 
    CDir* dirlist; 
    HBufC* fullName = NULL;
    TFileName filename( aFileName );
    TInt err = search.FindWildByDir( filename, aFilePath, dirlist ); 
    
    CCHUIDEBUG2( "ScanFileL - FindWildByDir err: %d", err );    
    
    if ( err == KErrNone )
        {
        CCHUIDEBUG2( "ScanFileL - dir list count: %d", dirlist->Count() );      
        
        if ( 0 < dirlist->Count() )
            {
            CCHUIDEBUG( "ScanFileL - Parsing full entry" );
            
            TParse fullentry;
            fullentry.Set( (*dirlist)[0].iName, &search.File(), NULL ); 
            TFileName nearestFile( fullentry.FullName() );
            BaflUtils::NearestLanguageFile( aSession, nearestFile );   
            fullName = HBufC::NewL( nearestFile.Length() );
            fullName->Des().Copy( nearestFile );  
            }
        delete dirlist; 
        
        CCHUIDEBUG2( "ScanFileL - full name: %S", fullName );             
        }     
    
    return fullName;
    }  

// ---------------------------------------------------------------------------
// Resolves service name.
// ---------------------------------------------------------------------------
//
void CCCHUiNotifierBase::GetServiceNameL( 
    TUint aServiceId, TDes& aServiceName )
    {
    CCHUIDEBUG( "CCCHUiNotifierBase::GetServiceNameL" );	
	CSPEntry* entry = CSPEntry::NewLC();
	TRAPD( err, iSettings->FindEntryL( aServiceId, *entry ) );
	if ( !err )
	    {
	    aServiceName = entry->GetServiceName();
	    }
	CleanupStack::PopAndDestroy( entry );    
    CCHUIDEBUG( "CCCHUiNotifierBase::GetServiceNameL OUT" );
    }

// ---------------------------------------------------------------------------
// Resolves current connection name.
// ---------------------------------------------------------------------------
//
void CCCHUiNotifierBase::CurrentConnectionNameL( 
    TUint aServiceId, TUint aIapId, RBuf& aIapName )
    {
    CCHUIDEBUG( "CCCHUiNotifierBase::CurrentConnectionNameL - IN" );    
   
    CCchUiNotifConnectionHandler* connHandler = 
            CCchUiNotifConnectionHandler::NewLC();
    
    RBuf serviceName;
    CleanupClosePushL( serviceName );
    serviceName.CreateL( KServiceNameLength );
    GetServiceNameL( aServiceId, serviceName );
    
    // get connection name
    connHandler->ConnectionNameL( aIapId, serviceName, aIapName );
    
    CleanupStack::PopAndDestroy( &serviceName );
    CleanupStack::PopAndDestroy( connHandler );
    
    CCHUIDEBUG( "CCCHUiNotifierBase::CurrentConnectionNameL - OUT" );
    }
    
// ---------------------------------------------------------------------------
// Resolves if service is SIP/VoIP.
// ---------------------------------------------------------------------------
//
TBool CCCHUiNotifierBase::IsSIPVoIPL( TUint aServiceId )
    {
    CCHUIDEBUG( "CCCHUiNotifierBase::IsSIPVoIPL" );
    
    TInt uid( 0 );
    TBool sipVoIP( EFalse );
       
    CSPProperty* property = CSPProperty::NewLC();
       
    User::LeaveIfError( iSettings->FindPropertyL( 
            aServiceId, EPropertyVoIPSubServicePluginId , *property ) );
       
    User::LeaveIfError( property->GetValue( uid ) );
       
    if ( KSipVoIPSubServicePlugId == uid )
        {
        sipVoIP = ETrue;
        }
       
    CleanupStack::PopAndDestroy( property );
       
    CCHUIDEBUG2( "CCCHUiNotifierBase::IsSIPVoIPL: %d", sipVoIP );
    
    return sipVoIP;
    }
    
// -----------------------------------------------------------------------------
// Checks if phone is in offline mode.
// -----------------------------------------------------------------------------
//
TBool CCCHUiNotifierBase::IsPhoneOfflineL() const
    {
    CCHUIDEBUG( "CCCHUiNotifierBase::IsPhoneOfflineL" );	
    
    if ( FeatureManager::FeatureSupported( KFeatureIdOfflineMode ) )
        {
        CRepository* repository = CRepository::NewLC(KCRUidCoreApplicationUIs);
        TInt connAllowed = ECoreAppUIsNetworkConnectionAllowed;
        repository->Get( KCoreAppUIsNetworkConnectionAllowed, connAllowed );
        CleanupStack::PopAndDestroy( repository ); 
        if ( !connAllowed )
            {
            CCHUIDEBUG( "IsPhoneOfflineL - phone is in offline" );	        
            return ETrue;
            }
        }
    
    CCHUIDEBUG( "IsPhoneOfflineL - phone is not in offline" );	
    return EFalse;
    }    

// -----------------------------------------------------------------------------
// Checks if there are GPRS IAPs available in Internet SNAP.
// -----------------------------------------------------------------------------
//
TBool CCCHUiNotifierBase::IsGprsIapsAvailableL() const
    {
    CCHUIDEBUG( "CCCHUiNotifierBase::IsGprsIapsAvailableL" );   
    
    TBool available( EFalse );
    
    CCchUiNotifConnectionHandler* connHandler = 
        CCchUiNotifConnectionHandler::NewLC();
    
    RArray<TUint32> gprsIapIds;
    CleanupClosePushL( gprsIapIds );
    
    // Array for iap names. No use here, we need just iap count.
    CDesCArray* iapNames = new (ELeave) CDesCArrayFlat( 3 );
    CleanupStack::PushL( iapNames );            
    
    TRAPD( err, connHandler->GetGprsAccessPointsL( *iapNames, gprsIapIds ) );
    
    if ( gprsIapIds.Count() && !err )
        {     
        CCHUIDEBUG( "IsGprsIapsAvailableL - gprs iaps available" );      
        available = ETrue;
        }
    
    CleanupStack::PopAndDestroy( iapNames );
    CleanupStack::PopAndDestroy( &gprsIapIds );
    CleanupStack::PopAndDestroy( connHandler );
    
    CCHUIDEBUG( "IsGprsIapsAvailableL gprs iaps not available" );
    return available;
    }

// -----------------------------------------------------------------------------
// Checks if connection is used by other services.
// -----------------------------------------------------------------------------
//
TBool CCCHUiNotifierBase::ConnectionUsedByOthersL( 
    TUint aServiceId, TInt aIapId )
    {
    CCHUIDEBUG( "CCCHUiNotifierBase::ConnectionUsedByOthersL" );
    
    TBool usedByOthers( EFalse );
    
    RArray<TUint32> destIds;
    CleanupClosePushL( destIds );
     
    CCchUiNotifConnectionHandler* connHandler = 
            CCchUiNotifConnectionHandler::NewLC();
    
    RBuf serviceName;
    CleanupClosePushL( serviceName );
    serviceName.CreateL( KServiceNameLength );
    GetServiceNameL( aServiceId, serviceName );
    
    CCHUIDEBUG( "ConnectionUsedByOthersL - get destinations" );
    
    // Get all but our services destinations.
    connHandler->GetDestinationsL( serviceName, destIds );
    
    CleanupStack::PopAndDestroy( &serviceName );
    
    for ( TInt i( 0 ) ; i < destIds.Count() ; i++ )
        {
        RArray<TUint32> iapIds;
        CleanupClosePushL( iapIds );
     
        connHandler->GetAccessPointsFromSnapL( iapIds, destIds[ i ] );
        
        TInt index = iapIds.Find( aIapId );
        
        if ( KErrNotFound != index  )
            {
            CCHUIDEBUG( 
            "ConnectionUsedByOthersL - iap is used by other service(s)" );
            
            usedByOthers = ETrue;
            }
        
        CleanupStack::PopAndDestroy( &iapIds );
        }
     
    CleanupStack::PopAndDestroy( connHandler );
    CleanupStack::PopAndDestroy( &destIds );
    
    return usedByOthers;
    }
    
