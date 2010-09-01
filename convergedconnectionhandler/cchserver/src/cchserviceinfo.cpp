/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CCCHServiceInfo implementation
*
*/


// INCLUDE FILES
#include "cchserviceinfo.h"
#include "cchsubserviceinfo.h"
#include "cchserverbase.h"
#include "cchpluginhandler.h"
#include "cchrequeststorage.h"
#include "cchspshandler.h"
#include "cchservicehandler.h"
#include "cchlogger.h"
#include "cchpluginhandler.h"

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES
// None

// CONSTANTS
// None

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// =========================== LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// CCCHServiceInfo::CCCHServiceInfo
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CCCHServiceInfo::CCCHServiceInfo( CCCHServerBase& aServer ):
    iServer( aServer ), 
    iReserved( ETrue )
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// CCCHServiceInfo::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CCCHServiceInfo* CCCHServiceInfo::NewL( CCCHServerBase& aServer )
    {
    CCCHServiceInfo* self = CCCHServiceInfo::NewLC( aServer );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CCCHServiceInfo::NewLC
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CCCHServiceInfo* CCCHServiceInfo::NewLC( CCCHServerBase& aServer )
    {
    CCCHServiceInfo* self = new (ELeave) CCCHServiceInfo( aServer );
    CleanupStack::PushL( self );
    return self;
    }

// Destructor
CCCHServiceInfo::~CCCHServiceInfo()
    {
    iSubservices.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// CCCHServiceInfo::GetServiceId
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TUint32 CCCHServiceInfo::GetServiceId() const
    {
    return iServiceId;
    }

// ---------------------------------------------------------------------------
// CCCHServiceInfo::SetServiceId
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHServiceInfo::SetServiceId( TUint32 aServiceId )
    {
    iServiceId = aServiceId;
    }

// ---------------------------------------------------------------------------
// CCCHServiceInfo::GetName
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
const TDesC CCCHServiceInfo::GetName() const
    {
    return iServiceName;
    }
    
// ---------------------------------------------------------------------------
// CCCHServiceInfo::SetName
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHServiceInfo::SetName( const TDesC& aName )
    {
    iServiceName.Copy( aName );
    }

// ---------------------------------------------------------------------------
// CCCHServiceInfo::EnableL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHServiceInfo::EnableL( 
    const TCCHSubserviceType aServiceType, 
    const TBool aConnectivityCheck,
    MCCHServiceNotifier* aNotifier )
    {
    CCHLOGSTRING( "CCCHServiceInfo::EnableL: IN" );
    CCHLOGSTRING2( "CCCHServiceInfo::EnableL: ServiceId = %d", iServiceId );
    CCHLOGSTRING2( "CCCHServiceInfo::EnableL: iType %d", aServiceType );
    CCHLOGSTRING2( "CCCHServiceInfo::EnableL: connectivitycheck %d", 
        aConnectivityCheck );
    
    RArray<TUid> uids;
    CleanupClosePushL( uids );
        
    TUid pluginUid( KNullUid );
    TBool found( EFalse );
    TUint iapId( 0 );
        
    for ( TInt i( 0 ); i < iSubservices.Count(); i++ )
        {
        pluginUid = iSubservices[ i ]->GetPluginUid();
            
        found = ( KErrNotFound != uids.Find( pluginUid ) );
           
        if ( !found && pluginUid != KNullUid && 
            ( ECCHUnknown == aServiceType ||
              aServiceType == iSubservices[ i ]->Type() ) )
            {
            uids.Append( pluginUid );
            }
        }
    
    0 < uids.Count() ? iServer.PluginHandler().EnableServiceL( 
        iServiceId, uids, aServiceType, aNotifier, iapId, aConnectivityCheck ) : 
            User::Leave( KErrNotFound );
    
    CleanupStack::PopAndDestroy( &uids );
    
    // Update service table
    if ( ECCHUnknown != aServiceType )
        {        
        iServer.SPSHandler().SetLoadAtStartUpL( iServiceId,
            aServiceType, ETrue );
        }
    else
        {
        for ( TInt i( 0 ); i < iSubservices.Count(); i++ )
            {
            iServer.SPSHandler().SetLoadAtStartUpL( iServiceId,
            iSubservices[ i ]->Type(), ETrue );
            }
        }        
    CCHLOGSTRING( "CCCHServiceInfo::EnableL: OUT" );
    }

// ---------------------------------------------------------------------------
// CCCHServiceInfo::DisableL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHServiceInfo::DisableL( const TCCHSubserviceType aServiceType )
    {
    CCHLOGSTRING( "CCCHServiceInfo::DisableL: IN" );
    CCHLOGSTRING2( "CCCHServiceInfo::DisableL: ServiceId = %d", iServiceId );
    CCHLOGSTRING2( "CCCHServiceInfo::DisableL: iType %d", aServiceType );
    
    RArray<TUid> uids;
    CleanupClosePushL( uids );
        
    TUid pluginUid( KNullUid );
    TBool found( EFalse );
        
    for ( TInt i( 0 ); i < iSubservices.Count(); i++ )
        {
        pluginUid = iSubservices[ i ]->GetPluginUid();
            
        found = ( KErrNotFound != uids.Find( pluginUid ) );
                       
        if ( !found && pluginUid != KNullUid && 
            ( ECCHUnknown == aServiceType ||
              aServiceType == iSubservices[ i ]->Type() ) )
            {
            uids.Append( pluginUid );
            }
        }
    
    0 < uids.Count() ? iServer.PluginHandler().DisableServiceL( 
        iServiceId, uids, aServiceType ) : User::Leave( KErrNotFound );
        
    CleanupStack::PopAndDestroy( &uids );
    
    // Update service table
    if ( ECCHUnknown != aServiceType )
        {        
        iServer.SPSHandler().SetLoadAtStartUpL( iServiceId,
            aServiceType, EFalse );
        }
    else
        {
        for ( TInt i( 0 ); i < iSubservices.Count(); i++ )
            {
            iServer.SPSHandler().SetLoadAtStartUpL( iServiceId,
            iSubservices[ i ]->Type(), EFalse );
            }
        }
                
    CCHLOGSTRING( "CCCHServiceInfo::DisableL: OUT" );
    }

// ---------------------------------------------------------------------------
// CCCHServiceInfo::SetConnectionInfoL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHServiceInfo::SetConnectionInfoL( 
    const TServiceConnectionInfo aServiceConnInfo )
    {
    CCHLOGSTRING( "CCCHServiceInfo::SetConnectionInfoL: IN" );
    
    RArray<TUid> uids;
    CleanupClosePushL( uids );
        
    TUid pluginUid( KNullUid );
    TBool found( EFalse );
        
    for ( TInt i( 0 ); i < iSubservices.Count(); i++ )
        {        
        pluginUid = iSubservices[ i ]->GetPluginUid();
            
        found = ( KErrNotFound != uids.Find( pluginUid ) );
        
        if ( !found && pluginUid != KNullUid && 
            ( ECCHUnknown == aServiceConnInfo.iServiceSelection.iType ||
              aServiceConnInfo.iServiceSelection.iType == 
              iSubservices[ i ]->Type() ) )
            {
            uids.Append( pluginUid );
            }
        // Set connection information to subservice
        if ( ECCHUnknown == aServiceConnInfo.iServiceSelection.iType ||
              aServiceConnInfo.iServiceSelection.iType == 
              iSubservices[ i ]->Type() ) 
            {
            iSubservices[ i ]->SetIapId( aServiceConnInfo.iIapId );
            iSubservices[ i ]->SetSNAPId( aServiceConnInfo.iSNAPId );
            }
        }
    
    if ( 0 == uids.Count() )
        {
        User::Leave( KErrNotFound );
        }
                
    if ( !aServiceConnInfo.iIapId && aServiceConnInfo.iSNAPId )
        {
        iServer.PluginHandler().SetSnapIdL( 
            aServiceConnInfo.ServiceId(), uids, 
                aServiceConnInfo.Type(), 
                aServiceConnInfo.iSNAPId );
        }
    else if ( aServiceConnInfo.iIapId && !aServiceConnInfo.iSNAPId )
        {
        iServer.PluginHandler().SetIapIdL( 
            aServiceConnInfo.ServiceId(), uids, 
                aServiceConnInfo.Type(), 
                aServiceConnInfo.iIapId );
        }
    else if ( aServiceConnInfo.iIapId && aServiceConnInfo.iSNAPId )
        {
        iServer.PluginHandler().SetIapIdL( 
            aServiceConnInfo.ServiceId(), uids, 
                aServiceConnInfo.Type(),
                aServiceConnInfo.iIapId );
        iServer.PluginHandler().SetSnapIdL( 
            aServiceConnInfo.ServiceId(), uids, 
            aServiceConnInfo.Type(),
                aServiceConnInfo.iSNAPId );
        }
   
    // username must not be empty. And check this field should be set 
    if ( aServiceConnInfo.iServiceSelection.iParameter == ECchUsername && 
         aServiceConnInfo.iUsername.Length() )
        {
        iServer.PluginHandler().SetUsernameL( 
            aServiceConnInfo.ServiceId(), uids, 
                aServiceConnInfo.Type(), 
                aServiceConnInfo.iUsername );
        }

    // empty password is ok. And check this field should be set
    if ( aServiceConnInfo.iServiceSelection.iParameter == ECchPassword )
        {
        iServer.PluginHandler().SetPasswordL( 
            aServiceConnInfo.ServiceId(), uids, 
                aServiceConnInfo.Type(), 
                aServiceConnInfo.iPassword );
        }
    
    if ( aServiceConnInfo.iServiceSelection.iParameter == ECchReserved )
        {
        iReserved = aServiceConnInfo.iReserved;
        CCHLOGSTRING2( "CCCHServiceInfo::SetConnectionInfoL: reserved no action=%d", iReserved );
        }

    CleanupStack::PopAndDestroy( &uids );
    
    CCHLOGSTRING( "CCCHServiceInfo::SetConnectionInfoL: OUT" );
    }
    
// ---------------------------------------------------------------------------
// CCCHServiceInfo::GetConnectionInfoL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHServiceInfo::GetConnectionInfoL( 
        TServiceConnectionInfo& aServiceConnInfo ) const
        {
        CCHLOGSTRING( "CCCHServiceInfo::GetConnectionInfoL: IN" );
               
        TUid pluginUid( KNullUid );
        TInt index( KErrNone );
               
        if ( aServiceConnInfo.iServiceSelection.iType != ECCHUnknown )
            {
            index = FindSubservice( aServiceConnInfo.iServiceSelection.iType );
            }

               
        if ( KErrNotFound != index && iSubservices.Count() )
            {
                   
            pluginUid = iSubservices[ index ]->GetPluginUid();

            if( aServiceConnInfo.iServiceSelection.iParameter == ECchUsername )
                {
                const CCCHPluginHandler& ch = iServer.PluginHandler();
                const TServiceConnectionInfo& css = aServiceConnInfo;

                RBuf buffer; 
                buffer.CreateL(KCCHMaxProtocolNameLength);
                iServer.PluginHandler().GetConnectionParameter( 
                        pluginUid,
                        css.iServiceSelection, 
                        buffer );

                aServiceConnInfo.iUsername = buffer;
                buffer.Close();
                }
            else if( aServiceConnInfo.iServiceSelection.iParameter == 
                                                        ECchReserved )
                {
                CCHLOGSTRING2( "CCCHServiceInfo::GetConnectionInfoL: reserved no action=%d", iReserved );
                aServiceConnInfo.iReserved = iReserved;
                }
            else
                {
                User::LeaveIfError( iServer.PluginHandler().GetServiceNetworkInfo( 
                    aServiceConnInfo.ServiceId(), pluginUid, aServiceConnInfo.Type(), 
                        aServiceConnInfo.iSNAPId, aServiceConnInfo.iIapId, 
                        aServiceConnInfo.iSNAPLocked, aServiceConnInfo.iPasswordSet ) );                }
            }
        else
            {
            User::Leave( KErrNotFound );
            }
            
        CCHLOGSTRING( "CCCHServiceInfo::GetConnectionInfoL: OUT" );
    }

// ---------------------------------------------------------------------------
// CCCHServiceInfo::ReserveServiceL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHServiceInfo::ReserveServiceL( 
    const TCCHSubserviceType aServiceType ) 
    {
    CCHLOGSTRING( "CCCHServiceInfo::ReserveServiceL: IN" );
    
    TUid pluginUid( KNullUid );
    TInt index( FindSubservice( aServiceType ) );
    if ( KErrNotFound != index )
        {
        pluginUid = iSubservices[ index ]->GetPluginUid();
        User::LeaveIfError( iServer.PluginHandler().ReserveService( 
            iServiceId, pluginUid, aServiceType ));
        }
    else
        {
        User::Leave( KErrNotFound );
        }
        
    CCHLOGSTRING( "CCCHServiceInfo::ReserveServiceL: OUT" );
    }

// ---------------------------------------------------------------------------
// CCCHServiceInfo::FreeServiceL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHServiceInfo::FreeServiceL( 
    const TCCHSubserviceType aServiceType )
    {
    CCHLOGSTRING( "CCCHServiceInfo::FreeServiceL: IN" );
    
    TUid pluginUid( KNullUid );
    TInt index( FindSubservice( aServiceType ) );
    if ( KErrNotFound != index )
        {
        pluginUid = iSubservices[ index ]->GetPluginUid();
        User::LeaveIfError( iServer.PluginHandler().FreeService( 
            iServiceId, pluginUid, aServiceType ));
        }
    else
        {
        User::Leave( KErrNotFound );
        }
        
    CCHLOGSTRING( "CCCHServiceInfo::FreeServiceL: OUT" );
    }

// ---------------------------------------------------------------------------
// CCCHServiceInfo::IsReservedL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CCCHServiceInfo::IsReservedL( 
    const TCCHSubserviceType aServiceType ) const
    {
    CCHLOGSTRING( "CCCHServiceInfo::IsReservedL: IN" );
    
    TBool ret(EFalse);
    TUid pluginUid( KNullUid );
    TInt index( FindSubservice( aServiceType ) );
    if ( KErrNotFound != index )
        {
        pluginUid = iSubservices[ index ]->GetPluginUid();
        User::LeaveIfError( ret = iServer.PluginHandler().IsReserved( 
            iServiceId, pluginUid, aServiceType ));
        }
    else
        {
        User::Leave( KErrNotFound );
        }
        
    CCHLOGSTRING( "CCCHServiceInfo::IsReservedL: OUT" );
    return ret;
    }         
                             
// ---------------------------------------------------------------------------
// CCCHServiceInfo::IsEnabled
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CCCHServiceInfo::IsEnabled() const
    {
    return iIsEnabled;
    }

// ---------------------------------------------------------------------------
// CCCHServiceInfo::AddSubservice
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHServiceInfo::AddSubserviceL( TCCHSubservice& aSubservice )
    {
    CCHLOGSTRING( "CCCHServiceInfo::AddSubserviceL: IN" );
    
    CCCHSubserviceInfo* subserviceInfo = NULL;
    if ( ECCHUnknown != aSubservice.iConnectionInfo.iServiceSelection.iType )
        {
        // Create new instance and set service id
        subserviceInfo = 
            CCCHSubserviceInfo::NewLC( aSubservice.iConnectionInfo.iServiceSelection.iServiceId, iServer );
        // Set subserviceID
        subserviceInfo->SetSubserviceId( aSubservice.iSubserviceId );
        
        // Get plugin uid from service table
        TUid pUid( KNullUid );
        
        iServer.SPSHandler().GetConnectivityPluginUidL( 
                aSubservice.iConnectionInfo.iServiceSelection.iServiceId, aSubservice.iConnectionInfo.iServiceSelection.iType, pUid ); 
        subserviceInfo->SetPluginUid( pUid ); 
          
        subserviceInfo->SetState( aSubservice.iState );
        subserviceInfo->SetType( aSubservice.iConnectionInfo.iServiceSelection.iType );
        subserviceInfo->SetIapId( aSubservice.iConnectionInfo.iIapId );
        subserviceInfo->SetSNAPId( aSubservice.iConnectionInfo.iSNAPId );    
        TBool startupFlag( EFalse );
        iServer.SPSHandler().LoadAtStartUpL( aSubservice.iConnectionInfo.iServiceSelection.iServiceId,
             aSubservice.iConnectionInfo.iServiceSelection.iType, startupFlag );
        subserviceInfo->SetStartupFlag( startupFlag );        
        // Append pointer to array
        iSubservices.Append( subserviceInfo );
        CleanupStack::Pop( subserviceInfo );
        }
   
    CCHLOGSTRING( "CCCHServiceInfo::AddSubserviceL: OUT" );
    }

// ---------------------------------------------------------------------------
// CCCHServiceInfo::UpdateL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHServiceInfo::UpdateL( TBool aReadServiceTable )
    {
    CCHLOGSTRING( "CCCHServiceInfo::UpdateL: IN" );
    
    iIsEnabled = EFalse;
    if ( aReadServiceTable )
        {
        TUint validSubserviceCount( 0 );
        TInt index( KErrNotFound );
        TCCHService service;
        // Read service information from service table
        iServer.SPSHandler().GetServiceInfoL( iServiceId , service );

        for ( TInt i( 0 ); i < service.iSubservices.Count(); i++ )
            {            
            TCCHSubservice subservice = service.iSubservices[ i ];
            
            if ( KErrNotFound != ( index = FindSubservice( 
                subservice.iConnectionInfo.iServiceSelection.iType ) ) )
                {
                TBool startupFlag( EFalse );
                iServer.SPSHandler().LoadAtStartUpL( subservice.iConnectionInfo.iServiceSelection.iServiceId,
                    subservice.iConnectionInfo.iServiceSelection.iType, startupFlag );
                iSubservices[ index ]->SetStartupFlag( startupFlag );
                iSubservices[ index ]->Update();
                if ( !iIsEnabled )
                    {
                    iIsEnabled = iSubservices[ index ]->IsEnabled();
                    }
                
                validSubserviceCount++;
                }
            else if ( ECCHUnknown != subservice.iConnectionInfo.iServiceSelection.iType )
                {
                AddSubserviceL( subservice );
                validSubserviceCount++;
                }
            else
                {
                // do nothing
                }
            }
        
        // Check that all subservices really exist in service table
        if ( validSubserviceCount < iSubservices.Count() )
            {
            CCHLOGSTRING( "CCCHServiceInfo::UpdateL: check subservices -> need update" );
        
            for ( TInt i( 0 ); i < iSubservices.Count(); i++ )
                {
                TBool found( EFalse );
                TCCHSubserviceType type = iSubservices[ i ]->Type();
                         
                for ( TInt j( 0 ) ; j < service.iSubservices.Count() && !found ; j++ )
                    {
                    CCHLOGSTRING2( "CCCHServiceInfo::UpdateL: type 1=%d", type );
                    CCHLOGSTRING2( "CCCHServiceInfo::UpdateL: type 2=%d", 
                        service.iSubservices[ j ].iConnectionInfo.iServiceSelection.iType );
                
                    if ( service.iSubservices[ j ].iConnectionInfo.iServiceSelection.iType == type )
                        {
                        CCHLOGSTRING( "CCCHServiceInfo::UpdateL: check subservices -> found" );
                        found = ETrue;
                        }
                    }
                
                if ( !found )
                    {
                    CCHLOGSTRING( "CCCHServiceInfo::UpdateL: remove subservice" );
                
                    delete iSubservices[ i ];
                    iSubservices.Remove( i );
                    i--;
                    }              
                }
            }
        }
    else
        {
        for ( TInt i( 0 ); i < iSubservices.Count(); i++ )
            {
            iSubservices[ i ]->Update();
            if ( !iIsEnabled )
                {
                iIsEnabled = iSubservices[ i ]->IsEnabled();    
                }
            }
        }
            
    CCHLOGSTRING( "CCCHServiceInfo::UpdateL: OUT" );
    }

// ---------------------------------------------------------------------------
// CCCHServiceInfo::SetStateL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//    
void CCCHServiceInfo::SetStateL( 
    const TCCHSubserviceType aType, 
    const TInt aState )
    {
    CCHLOGSTRING( "CCCHServiceInfo::SetStateL IN" );
    
    TInt index( FindSubservice( aType ) );
    if ( KErrNotFound != index )
        {
        iSubservices[ index ]->SetState( 
                static_cast<TCCHSubserviceState>( aState ) );
        }
    else
        {
        User::Leave( index );
        }       

    // Before we update our clients handle exception error cases
    HandleErrorExceptions();
    UpdateL();    
    CCHLOGSTRING( "CCCHServiceInfo::SetStateL :OUT" );
    }

// ---------------------------------------------------------------------------
// CCCHServiceInfo::SetStateL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//    
void CCCHServiceInfo::SetStateL( 
    const TUid& aPluginUid, 
    const TCCHSubserviceState& aState )
    {
    CCHLOGSTRING( "CCCHServiceInfo::SetStateL IN" );
    
    
    for ( TInt i( 0 ); i < iSubservices.Count() && 
        i < KCCHMaxSubservicesCount; i++ )
        {
        if( aPluginUid == GetUidL( i ) )
            {
            iSubservices[ i ]->SetState( aState );
            }
        }
        
    UpdateL();    
    CCHLOGSTRING( "CCCHServiceInfo::SetStateL :OUT" );
    }

// ---------------------------------------------------------------------------
// CCCHServiceInfo::SetErrorL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//    
void CCCHServiceInfo::SetErrorL( 
    const TCCHSubserviceType aType, 
    const TInt aError )
    {
    CCHLOGSTRING( "CCCHServiceInfo::SetErrorL IN" );
    
    TInt index( FindSubservice( aType ) );
    if ( KErrNotFound != index )
        {
        iSubservices[ index ]->SetError( aError );        
        }
    else
        {
        User::Leave( index );
        }           
    CCHLOGSTRING( "CCCHServiceInfo::SetErrorL :OUT" );
    }

// ---------------------------------------------------------------------------
// CCCHServiceInfo::SubserviceCount
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CCCHServiceInfo::SubserviceCount() const
    {
    return iSubservices.Count();
    }

// ---------------------------------------------------------------------------
// CCCHServiceInfo::GetUidL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TUid CCCHServiceInfo::GetUidL( TInt aSubserviceIndex ) const
    {
    if ( aSubserviceIndex > iSubservices.Count() )
        {
        User::Leave( KErrNotFound );
        }
    return iSubservices[ aSubserviceIndex ]->GetPluginUid();
    }

// ---------------------------------------------------------------------------
// CCCHServiceInfo::FillServiceInfo
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHServiceInfo::FillServiceInfo( TCCHService& service )
    {
    CCHLOGSTRING( "CCCHServiceInfo::FillServiceInfo: IN" );
    service.iServiceId = iServiceId;
    service.iServiceName.Copy( iServiceName );
    
    for ( TInt i( 0 ); i < iSubservices.Count() && 
        i < KCCHMaxSubservicesCount; i++ )
        {
        iSubservices[ i ]->FillSubserviceInfo( service.iSubservices.At( i ) );              
        }
        
    CCHLOGSTRING( "CCCHServiceInfo::FillServiceInfo: OUT" );
    }

// ---------------------------------------------------------------------------
// CCCHServiceInfo::GetStatus
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CCCHServiceInfo::GetStatus( 
    TCCHSubserviceType aType, TCCHSubserviceState& aState ) const
    {    
    TInt error = KErrNone;
    TInt index( FindSubservice( aType ) );
    
    if( index == KErrNotFound )
        {
        error = KErrNotFound;
        }
    else
        {
        aState = iSubservices[ index ]->GetState();
        }
        
    return error;
    }

// ---------------------------------------------------------------------------
// CCCHServiceInfo::GetState
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TCCHSubserviceState CCCHServiceInfo::GetState() const
    {    
    TCCHSubserviceState ret( ECCHUninitialized );

    for ( TInt i( 0 ); i < iSubservices.Count() && !ret; i++ )
        {
        if( iSubservices[ i ]->GetState() > ret )
            {
            ret = iSubservices[ i ]->GetState();
            }
        }
        
    CCHLOGSTRING2( "CCCHServiceHandler::GetState: %d", ret );
    return ret;
    }

// ---------------------------------------------------------------------------
// CCCHServiceInfo::GetError
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CCCHServiceInfo::GetError( 
    TCCHSubserviceType aType ) const
    {    
    TInt index( FindSubservice( aType ) );
    return ( KErrNotFound != index ) ? iSubservices[ index ]->GetError() :
        KErrNone;
    }

// ---------------------------------------------------------------------------
// CCCHServiceInfo::GetServiceInfoL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHServiceInfo::GetServiceInfoL( 
    TDes& aBuffer,
    TCCHSubserviceType aType ) const
    {
    TUid pluginUid( KNullUid );
    TInt index( FindSubservice( aType ) );
    if ( KErrNotFound != index )
        {
        pluginUid = iSubservices[ index ]->GetPluginUid();
        
        User::LeaveIfError( iServer.PluginHandler().GetServiceInfo( 
            iServiceId, pluginUid, aType, aBuffer ) );
        }
    else
    	{
    	User::Leave( KErrNotFound );
    	}
    }

// ---------------------------------------------------------------------------
// CCCHServiceInfo::SubserviceExist
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CCCHServiceInfo::SubserviceExist( TCCHSubserviceType aType ) const     
    {
    // If subservice is specified then check if it really exist.
    // Otherwise return ETrue
    return ( ECCHUnknown != aType ) ?  
        ( ( KErrNotFound != FindSubservice( aType ) ) ? ETrue : EFalse ) :
        ETrue;
    }
    
// ---------------------------------------------------------------------------
// CCCHServiceInfo::FindSubservice
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CCCHServiceInfo::FindSubservice( TCCHSubserviceType aType ) const
    {
    TInt index( KErrNotFound );
    TBool exist( EFalse );
    for ( TInt i( 0 ); i < iSubservices.Count() && 
            i < KCCHMaxSubservicesCount && !exist; i++ )
        {
        exist = iSubservices[ i ]->Type() == aType;
        index = i;
        }
    return !exist ? KErrNotFound : index;
    }

// ---------------------------------------------------------------------------
// CCCHServiceInfo::StartupFlagSet
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CCCHServiceInfo::StartupFlagSet() const
    {
    TBool ret( EFalse );
    for ( TInt i( 0 ); i < iSubservices.Count() && !ret; i++ )
        {
        ret = iSubservices[ i ]->GetStartUpFlag(); 
        }
    CCHLOGSTRING2( "CCCHServiceInfo::StartupFlagSet: %d", ret );        
    return ret;
    }

// ---------------------------------------------------------------------------
// CCCHServiceInfo::StartupFlagSet
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//    
TBool CCCHServiceInfo::StartupFlagSet( TInt aSubserviceIndex ) const
    {
    return aSubserviceIndex > iSubservices.Count() ? EFalse :
        iSubservices[ aSubserviceIndex ]->GetStartUpFlag();
    }

// ---------------------------------------------------------------------------
// CCCHServiceInfo::SetStartupFlag
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CCCHServiceInfo::SetStartupFlag(
    TCCHSubserviceType aType,
    TBool aLoadAtStartUp ) const
    {
    TInt exist( KErrNotFound );
    for ( TInt i( 0 ); i < iSubservices.Count() && 
            i < KCCHMaxSubservicesCount; i++ )
        {
        if ( iSubservices[ i ]->Type() == aType )
            {
            iSubservices[ i ]->SetStartupFlag( aLoadAtStartUp );
            exist = KErrNone;
            break;
            }
        }
    return exist;
    }

// ---------------------------------------------------------------------------
// CCCHServiceInfo::GetSubserviceType
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//    
TCCHSubserviceType CCCHServiceInfo::GetSubserviceType( 
    TInt aSubserviceIndex ) const
    {
    return aSubserviceIndex > iSubservices.Count() ? ECCHUnknown :
        iSubservices[ aSubserviceIndex ]->Type();
    }
    
// ---------------------------------------------------------------------------
// CCCHServiceInfo::GetSubserviceL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CCCHSubserviceInfo& CCCHServiceInfo::GetSubserviceL( 
    TCCHSubserviceType aType ) const
    {
    TInt index( FindSubservice( aType ) );
    if ( KErrNotFound == index )
        {
        User::Leave( index );
        }
        
    return *iSubservices[ index ];
    }

// ---------------------------------------------------------------------------
// CCCHServiceInfo::HandleErrorExceptions
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//    
void CCCHServiceInfo::HandleErrorExceptions()
    {
    CCHLOGSTRING( "CCCHServiceInfo::HandleErrorExceptions IN" );
    
    for ( TInt i( 0 ); i < iSubservices.Count(); i++ )
        {
        switch ( iSubservices[ i ]->GetError() )
            {
            
            // KErrCancel caused by CSIPProfileRegistry::ErrorOccurred(-3)
            // User has canceled connecting dialog and we have to remove
            // service from the load at startup list.
            // At last make sure that unnecessary wlan scan is OFF
            case KErrCancel:
                {
                TRAP_IGNORE( iServer.SPSHandler().SetLoadAtStartUpL( 
                    iServiceId, iSubservices[ i ]->Type(), EFalse ) );
                iServer.ServiceHandler().DisableWlanScan();
                }
                break;
                
            default:
                break;
            }
        
        }
     
    CCHLOGSTRING( "CCCHServiceInfo::HandleErrorExceptions :OUT" );
    }

// ========================== OTHER EXPORTED FUNCTIONS =======================

//  End of File
