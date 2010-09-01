/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CCCHRequestStorage implementation
*
*/


// INCLUDE FILES
#include "cchlogger.h"
#include "cchserverbase.h"
#include "cchrequeststorage.h"
#include "cchconnmonhandler.h"
#include "cchservicehandler.h"
#include "cchsubsession.h"

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

// ============================= LOCAL FUNCTIONS =============================

// ---------------------------------------------------------------------------
// IdentityRequestRelation
// Compare two request storage entries.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool IdentityRequestRelation(
    const TCCHStorageEntry& aFirst,
    const TCCHStorageEntry& aSecond )
    {
    TBool result( EFalse );

    result = ( aFirst.iRequest == aSecond.iRequest &&
         aFirst.iSubsession == aSecond.iSubsession );

    return result;
    }

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// CCCHRequestStorage::CCCHRequestStorage
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CCCHRequestStorage::CCCHRequestStorage( CCCHServerBase& aServer ) :
    iServer( aServer )
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// CCCHRequestStorage::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CCCHRequestStorage* CCCHRequestStorage::NewL( CCCHServerBase& aServer )
    {
    CCCHRequestStorage* self = CCCHRequestStorage::NewLC( aServer );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CCCHRequestStorage::NewLC
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CCCHRequestStorage* CCCHRequestStorage::NewLC( CCCHServerBase& aServer )
    {
    CCCHRequestStorage* self = new (ELeave)CCCHRequestStorage( aServer );
    CleanupStack::PushL( self );
    return self;
    }

// Destructor
CCCHRequestStorage::~CCCHRequestStorage()
    {
    iRequests.Close();
    iSubsessions.Close();
    }

// ---------------------------------------------------------------------------
// CCCHRequestStorage::AddRequestL
// Add new request into storage.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHRequestStorage::AddRequestL( TCCHCommands aRequest,
                                      const RMessage2& aMessage,
                                      const CCCHSubsession* aSubsession )
    {
    CCHLOGSTRING( "CCCHRequestStorage::AddRequestL: IN" );
    CCHLOGSTRING3
        ("CCCHRequestStorage::AddRequestL: Request = %d, Session = 0x%x",
            aRequest, aSubsession );

    // Create new request entry
    TCCHStorageEntry* entry = new (ELeave) TCCHStorageEntry;
    CleanupStack::PushL( entry );

    // Set entry data
    entry->iRequest = aRequest;
    entry->iMessage = new (ELeave) RMessage2( aMessage );
    CleanupStack::PushL( entry->iMessage );
    entry->iSubsession = aSubsession;
    
    // Check that subsession has not already made the request
    if ( KErrNotFound ==
         iRequests.Find( *entry, IdentityRequestRelation ) )
        {
        iRequests.AppendL( *entry );
        
        switch ( aRequest )
            {
            case ECCHEnableService:
                {
                TBool connectivityCheck( EFalse );
                TServiceSelection serviceSelection( aMessage.Int0(), 
                    static_cast<TCCHSubserviceType>( aMessage.Int1() ) );
                connectivityCheck = 
                    aMessage.Int2();
                
                TInt err = iServer.ServiceHandler().EnableService( 
                    serviceSelection, connectivityCheck );
                    
                CompleteRequest( ECCHEnableService, aSubsession, err );
                }
                break;
            case ECCHDisableService:
                {
                TServiceSelection serviceSelection( aMessage.Int0(), 
                    static_cast<TCCHSubserviceType>( aMessage.Int1() ) );
                    
                TInt err = iServer.ServiceHandler().DisableService( 
                    serviceSelection );
                    
                CompleteRequest( ECCHDisableService, aSubsession, err );
                }
                break;
            case ECCHSetConnectionInfo:
                {
                TServiceConnectionInfo serviceConnInfo;
                
                TPckgBuf<TServiceConnectionInfo> serviceConnInfoPckg;
                aMessage.ReadL( 0, serviceConnInfoPckg );
    
                serviceConnInfo.iServiceSelection.iServiceId = serviceConnInfoPckg().iServiceSelection.iServiceId;
                serviceConnInfo.iServiceSelection.iType      = serviceConnInfoPckg().iServiceSelection.iType;
                serviceConnInfo.iServiceSelection.iParameter = serviceConnInfoPckg().iServiceSelection.iParameter;
                
                serviceConnInfo.iSNAPId    = serviceConnInfoPckg().iSNAPId;
                serviceConnInfo.iIapId     = serviceConnInfoPckg().iIapId;
                serviceConnInfo.iReserved   = serviceConnInfoPckg().iReserved;

                if( serviceConnInfo.iServiceSelection.iParameter  == ECchUsername )
                    {
                    serviceConnInfo.iUsername = serviceConnInfoPckg().iUsername;
                    serviceConnInfo.iUsername.ZeroTerminate();
                    }
                if( serviceConnInfo.iServiceSelection.iParameter  == ECchPassword )
                    {                
                    /* : verify password will be zeroed correctly */
                    serviceConnInfo.iPassword = serviceConnInfoPckg().iPassword;
                    serviceConnInfo.iPassword.ZeroTerminate();
                    }
                
                TInt err = iServer.ServiceHandler().SetConnectionInfo( 
                    serviceConnInfo );
                    
                CompleteRequest( ECCHSetConnectionInfo, aSubsession, err );
                }
                break;
            case ECCHGetConnectionInfo:
                {
                TServiceConnectionInfo serviceConnInfo;
                
                TPckgBuf<TServiceConnectionInfo> serviceConnInfoPckg;
                aMessage.ReadL( 0, serviceConnInfoPckg );
    
                serviceConnInfo.iServiceSelection.iServiceId = serviceConnInfoPckg().iServiceSelection.iServiceId;
                serviceConnInfo.iServiceSelection.iType      = serviceConnInfoPckg().iServiceSelection.iType;
                serviceConnInfo.iServiceSelection.iParameter = serviceConnInfoPckg().iServiceSelection.iParameter;

                TInt err = iServer.ServiceHandler().GetConnectionInfo(
                    serviceConnInfo );

                CompleteRequestL( ECCHGetConnectionInfo, aSubsession,
                    &serviceConnInfo, err );
                }
                break;

            case ECCHGetServices:
                {
                TInt scanAllowed( KErrNone );
                
                // if service id is greater than or equals zero then
                // this is Quick service discovery request. Network scan is
                // not performed. 
                scanAllowed = aMessage.Int0();
                
                if ( KErrNotFound == scanAllowed )
                    {
                    ScanNetworks();    
                    
                    iServer.ServiceHandler().UpdateL( ETrue );
                    }
                
               
                TRAPD( err, 
                    iServer.ServiceHandler().GetServicesL( aMessage ) );
                
                CompleteRequest( ECCHGetServices, aSubsession, err );
                }
                break;
            default:
                break;
            }
        }
    else
        {
        // Notify request is already active
        CCHLOGSTRING
            ( "CCCHRequestStorage::AddRequestL: Request already active!" );
        iServer.PanicClient( aMessage, KErrAlreadyExists );
        }
    CleanupStack::Pop( entry->iMessage );
    CleanupStack::PopAndDestroy( entry );
    }

// ---------------------------------------------------------------------------
// CCCHRequestStorage::CompleteRequest
// Complete request and remove it from storage.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CCCHRequestStorage::CompleteRequest( TCCHCommands aRequest,
                                          const CCCHSubsession* aSubsession,
                                          TInt aError )
    {
    CCHLOGSTRING2
        ( "CCCHRequestStorage::CompleteRequest: error: %d", aError );
    
    TInt error( KErrNone );
    TCCHStorageEntry entry;
    entry.iRequest    = aRequest;
    entry.iSubsession = aSubsession;

    error = iRequests.Find( entry, IdentityRequestRelation );
    if ( KErrNotFound != error )
        {
        iRequests[ error ].iMessage->Complete( aError );
        DeleteRequest( error );

        // If no error -> return KErrNone
        error = KErrNone;
        }
    CCHLOGSTRING( "CCCHRequestStorage::CompleteRequest: OUT" );
    return error;
    }

// ---------------------------------------------------------------------------
// CCCHRequestStorage::CompleteRequestL
// Complete request and remove it from storage.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CCCHRequestStorage::CompleteRequestL( TCCHCommands aRequest,
                                           const CCCHSubsession* aSubsession,
                                           TAny* aParams,
                                           TInt aError )
    {
    CCHLOGSTRING2
        ( "CCCHRequestStorage::CompleteRequestL: error: %d", aError );
    
    TInt error( KErrNone );
    TCCHStorageEntry entry;
    entry.iRequest    = aRequest;
    entry.iSubsession = aSubsession;

    error = iRequests.Find( entry, IdentityRequestRelation );
    if ( KErrNotFound != error )
        {
        if ( ECCHGetConnectionInfo == entry.iRequest )
            {
            TPckgBuf<TServiceConnectionInfo> conInfo;
            iRequests[ error ].iMessage->ReadL( 0, conInfo );
            conInfo().iServiceSelection.iServiceId = 
                static_cast<TServiceConnectionInfo*>( aParams )->iServiceSelection.iServiceId;
            conInfo().iServiceSelection.iType = 
                static_cast<TServiceConnectionInfo*>( aParams )->iServiceSelection.iType;
            conInfo().iSNAPId = 
                static_cast<TServiceConnectionInfo*>( aParams )->iSNAPId;
            conInfo().iIapId = 
                static_cast<TServiceConnectionInfo*>( aParams )->iIapId;
            conInfo().iSNAPLocked = 
                static_cast<TServiceConnectionInfo*>( aParams )->iSNAPLocked;
            conInfo().iPasswordSet = 
                    static_cast<TServiceConnectionInfo*>( aParams )->iPasswordSet;
            conInfo().iUsername = static_cast<TServiceConnectionInfo*>( aParams )->iUsername;
            conInfo().iReserved = 
                    static_cast<TServiceConnectionInfo*>( aParams )->iReserved;
                    
            iRequests[ error ].iMessage->WriteL( 0, conInfo );
            }
       
        iRequests[ error ].iMessage->Complete( aError );
        DeleteRequest( error );

        // If no error -> return KErrNone
        error = KErrNone;
        }
    CCHLOGSTRING( "CCCHRequestStorage::CompleteRequestL: OUT" );
    return error;
    }
    
// ---------------------------------------------------------------------------
// CCCHRequestStorage::RemoveRequestsBySession
// Remove all requests by session id. (Does not complete request)
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CCCHRequestStorage::RemoveRequestsBySession(
    const CCCHSubsession* aSubSession )
    {
    CCHLOGSTRING( "CCCHRequestStorage::RemoveRequestsBySession: IN" );
    
    TInt error( KErrNotFound );

    for ( TInt i( iRequests.Count() - 1 ); i >= 0; i-- )
        {
        if ( iRequests[ i ].iSubsession == aSubSession )
            {
            DeleteRequest( i );
            error = KErrNone;
            }
        }
    CCHLOGSTRING( "CCCHRequestStorage::RemoveRequestsBySession: OUT" );
    return error;
    }

// ---------------------------------------------------------------------------
// CCCHRequestStorage::NotifySubserviceStatusChangeL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHRequestStorage::NotifyServiceStatesChange(
    TServiceStatus aNewStatus )
    {
    CCHLOGSTRING( "CCCHRequestStorage::NotifyServiceStatesChange: IN" );
    //deliver the status event to all subsessions    
    for ( TInt i = 0; i < iSubsessions.Count(); i++ )
        {
        iSubsessions[ i ]->ServiceEventOccured( aNewStatus );
        }

    CCHLOGSTRING( "CCCHRequestStorage::NotifyServiceStatesChange: OUT" );
    }

// ---------------------------------------------------------------------------
// CCCHRequestStorage::DeleteRequest
// Delete message object
// Delete entry from array
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHRequestStorage::DeleteRequest( TInt aIndex )
    {
    CCHLOGSTRING( "CCCHRequestStorage::DeleteRequest: IN" );
    CCHLOGSTRING2("CCCHRequestStorage::DeleteRequest: Index = %d", aIndex );

    if ( iRequests.Count() > aIndex )
        {
         delete iRequests[ aIndex ].iMessage;
        iRequests[ aIndex ].iMessage = NULL;

        // Remove entry from storage
        iRequests.Remove( aIndex );
        }
   
    CCHLOGSTRING( "CCCHRequestStorage::DeleteRequest: OUT" );
    }

// ---------------------------------------------------------------------------
// CCCHRequestStorage::ScanNetworks
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHRequestStorage::ScanNetworks()
    {
    // ETrue indicates wlan network scan
    iServer.ConnMonHandler().ScanNetworks( ETrue );
    }

// ---------------------------------------------------------------------------
// CCCHRequestStorage::AddSession
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCCHRequestStorage::AddSession(
    CCCHSubsession* aSubSession )
    {
    CCHLOGSTRING( "CCCHRequestStorage::AddSession: IN" );
    
    if( KErrNotFound == iSubsessions.Find( aSubSession ) )
    	{
    	iSubsessions.Append( aSubSession );
    	}
  
    CCHLOGSTRING( "CCCHRequestStorage::AddSession: OUT" );
    }

// ---------------------------------------------------------------------------
// CCCHRequestStorage::RemoveSession
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CCCHRequestStorage::RemoveSession(
    const CCCHSubsession* aSubSession )
    {
    CCHLOGSTRING( "CCCHRequestStorage::RemoveSession: IN" );
    
    TInt error( KErrNotFound );

    for ( TInt i = 0; i < iSubsessions.Count(); i++ )
        {
        if ( iSubsessions[ i ] == aSubSession )
            {
            iSubsessions.Remove( i );
            error = KErrNone;
            break;
            }
        }
    CCHLOGSTRING( "CCCHRequestStorage::RemoveSession: OUT" );
    return error;
    }
    
// ========================== OTHER EXPORTED FUNCTIONS =======================

//  End of File
