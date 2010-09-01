/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  cch client api implementation
*
*/


// INCLUDE FILES
#include "cchimpl.h"
#include "cchlogger.h"
#include "cch.h"
#include "cchserviceimpl.h"

#ifdef CCHAPI_USE_CCHUI
#include "cchuiprivateapi.h"
#endif

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

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// CCchImpl::CCchImpl
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CCchImpl::CCchImpl()
     : CActive ( EPriorityStandard ),
       iCchUi(0), 
       iConnectivityDialogsAllowed( EFalse )
	{
	CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CCchImpl::destructor
// ---------------------------------------------------------------------------
//    
CCchImpl::~CCchImpl()
    {
    CCHLOGSTRING( "CCchImpl::~CCchImpl: IN" );
    
    delete iCchUi;

    iCchServiceImpls.ResetAndDestroy();
    iCchServiceImpls.Close();
    Cancel();
    
    iCchClient.Close();
    iCchServer.Close();
    
    CCHLOGSTRING( "CCchImpl::~CCchImpl: OUT" );
    }

// ---------------------------------------------------------------------------
// CCchImpl::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CCchImpl::ConstructL( CCch& aCch )
    {
    CCHLOGSTRING( "CCchImpl::ConstructL: IN" );

    User::LeaveIfError( iCchServer.Connect() );
    User::LeaveIfError( iCchClient.Open( iCchServer ) );

#ifdef CCHAPI_USE_CCHUI
    iCchUi = CCchUiPrivateApi::NewL( aCch );
#endif

    iCchClient.RegisterToServer( iStatus );
    SetActive();
    
    CCHLOGSTRING( "CCchImpl::ConstructL: OUT" );
    }

// ---------------------------------------------------------------------------
// CCchImpl::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CCchImpl* CCchImpl::NewL( CCch& aCch )
    {
    CCHLOGSTRING( "CCchImpl::NewL: IN" );
    CCchImpl* self = 
        CCchImpl::NewLC( aCch );
    CleanupStack::Pop( self );
    CCHLOGSTRING( "CCchImpl::NewL: OUT" );
    return self;
    }

// ---------------------------------------------------------------------------
// CCchImpl::NewLC
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CCchImpl* CCchImpl::NewLC( CCch& aCch )
    {
    CCHLOGSTRING( "CCchImpl::NewLC: IN" );
    CCchImpl* self = 
        new (ELeave) CCchImpl( );
    CleanupStack::PushL( self );
    self->ConstructL( aCch );
    CCHLOGSTRING( "CCchImpl::NewLC: OUT" );
    return self;
    }

// ---------------------------------------------------------------------------
// CCchImpl::NewLC
// Returns the RCCHClient 
// ---------------------------------------------------------------------------
//
RCCHClient& CCchImpl::CchClient( )
	{
	CCHLOGSTRING( "CCchImpl::CchClient: IN OUT" );
	return iCchClient;
	}

// ---------------------------------------------------------------------------
// CCchImpl::GetService
// ---------------------------------------------------------------------------
//
CCchService* CCchImpl::GetService( TInt aId )
	{
	CCHLOGSTRING( "CCchImpl::GetService: IN" );
	
	CCchServiceImpl* cchServiceImpl = NULL;
	
	TInt idx = FindService( aId );
	if( idx == KErrNotFound )
		{
		TInt error = KErrNone;
	    TRAP( error, cchServiceImpl = GetServiceL( aId ) );
		}
	else
		{
		cchServiceImpl = iCchServiceImpls[ idx ];
		}
	CCHLOGSTRING( "CCchImpl::GetService: OUT" );
	return cchServiceImpl;
	}

// ---------------------------------------------------------------------------
// CCchImpl::GetServiceL
// ---------------------------------------------------------------------------
//
CCchServiceImpl* CCchImpl::GetServiceL( TInt aId )
	{
	CCchServiceImpl* cchServiceImpl = NULL;	
	
	CArrayFixFlat<TCCHService>* cchServices =
		new ( ELeave ) CArrayFixFlat<TCCHService>( 1 );
	CleanupStack::PushL( cchServices );
	if( KErrNone == GetCchServicesL( aId, ECCHUnknown , *cchServices ) )
		{
		if( cchServices->Count() )
		    {
    		TCCHService service = cchServices->At( 0 );
    		cchServiceImpl = CCchServiceImpl::NewL( *this, 
    		                                        service.iServiceId, 
    		                                        *iCchUi );
    		
    		TInt error = iCchServiceImpls.Append( cchServiceImpl );
    		if( error == KErrNone )
    			{
    			cchServiceImpl = iCchServiceImpls[ iCchServiceImpls.Count() - 1 ];
    			}
    		else
    			{
    			delete cchServiceImpl;
    			cchServiceImpl = NULL;
    			}
		    }
		}
	cchServices->Reset();
	CleanupStack::PopAndDestroy( cchServices );
	return cchServiceImpl;
	}


// ---------------------------------------------------------------------------
// CCchImpl::GetCchServiceL
// ---------------------------------------------------------------------------
//
TInt CCchImpl::GetCchServicesL( TInt aServiceId, TCCHSubserviceType aType, 
								CArrayFixFlat<TCCHService>& aServices )
	{
	TServiceSelection selection( aServiceId, aType, ECchInitial  );
    TRequestStatus status = KErrNone;

    iCchClient.GetServicesL( selection, aServices, status );			
	//even the cchclient api seems to be asynchronous, 
	//this method is completed immediately
	User::WaitForRequest( status );
	return status.Int();
	}
	
// ---------------------------------------------------------------------------
// CCchImpl::GetServiceIdsL
// ---------------------------------------------------------------------------
//
void CCchImpl::GetServiceIdsL( RArray<TInt>& aIds )
    {
	CCHLOGSTRING( "CCchImpl::GetServiceIdsL: IN" );

	TRequestStatus status( KErrNone );
    CArrayFixFlat<TCCHService>* cchServices = new (ELeave) CArrayFixFlat<TCCHService>(1);
    CleanupStack::PushL( cchServices );

    iCchClient.GetServicesL( *cchServices, status );
	    
	//even the cchclient api seems to be asynchronous, 
	//this method is completed immediately	
	User::WaitForRequest( status );
	if( KErrNone == status.Int() )
		{
		for ( TInt i = 0; i < cchServices->Count(); i++ )
			{
			TCCHService service = cchServices->At( i );
			User::LeaveIfError( aIds.Append( service.iServiceId ) );
			}
		}
	else
		{
		User::Leave( status.Int() );
		}	
    CleanupStack::PopAndDestroy( cchServices );

	CCHLOGSTRING( "CCchImpl::GetServiceIdsL: OUT" );
    }
    
// ---------------------------------------------------------------------------
// CCchImpl::GetServiceIds
// ---------------------------------------------------------------------------
//
TInt CCchImpl::GetServiceIds( RArray<TInt>& aIds )
	{
	TRAPD( err, GetServiceIdsL( aIds ) );
	
	return err;
	}

// ---------------------------------------------------------------------------
// CCchImpl::GetServices
// ---------------------------------------------------------------------------
//
TInt CCchImpl::GetServices( TCCHSubserviceType aType, 
        RPointerArray<CCchService>& aServices )
	{
	TInt err = KErrNone;
	TRAP( err, GetServicesL( aType, aServices ) );
	return err;
	}

// ---------------------------------------------------------------------------
// CCchImpl::GetServicesL
// ---------------------------------------------------------------------------
//
void CCchImpl::GetServicesL( TCCHSubserviceType aType, 
        RPointerArray<CCchService>& aServices )
    {
    CCHLOGSTRING( "CCchImpl::GetServicesL: IN" );

    CleanupClosePushL( aServices );  // CS: 1
    
    TInt error = KErrNone;
    TServiceSelection selection( 0, aType, ECchInitial );

    CCchServiceImpl* cchServiceImpl = NULL;
    CArrayFixFlat<TCCHService>* cchServices;
    cchServices = new (ELeave) CArrayFixFlat<TCCHService>(1);
    CleanupStack::PushL( cchServices );  // CS: 2
    error = GetCchServicesL( 0, aType, *cchServices );
    
    if( KErrNone == error )
        {
        for ( TInt i = 0; i < cchServices->Count(); i++ )
            {
            TCCHService service = cchServices->At( i );
            TInt idx = FindService( service.iServiceId );
            
            if( KErrNotFound == idx )
                {
                cchServiceImpl = CCchServiceImpl::NewLC( *this, // CS: 3
                    service.iServiceId, *iCchUi );
                iCchServiceImpls.AppendL( cchServiceImpl );
                CleanupStack::Pop( cchServiceImpl );  // CS: 2
                }
            else
                {
                cchServiceImpl = iCchServiceImpls[ idx ];
                }
            
            aServices.AppendL( cchServiceImpl );
            }
        }
    
    cchServices->Reset();
    CleanupStack::PopAndDestroy( cchServices );  // CS: 1
    User::LeaveIfError( error );
    CleanupStack::Pop( &aServices );  // CS: 0
    
    CCHLOGSTRING( "CCchImpl::GetServicesL: OUT" );
    }

// ---------------------------------------------------------------------------
// CCCHServiceHandler::FindService
// ---------------------------------------------------------------------------
//
TInt CCchImpl::FindService( TInt aServiceId ) const
    {
    TBool exist = EFalse;
    TInt index = KErrNotFound;
    CCHLOGSTRING2( "CCchImpl::FindService: serviceId %d", aServiceId );
    for ( TInt i = 0; i < iCchServiceImpls.Count() && !exist; i++ )
        {
        exist = iCchServiceImpls[i]->ServiceId() == aServiceId;
        index = i;
        }
    return !exist ? KErrNotFound : index;
    }

// ---------------------------------------------------------------------------
// CCCHServiceHandler::ConnectivityDialogsAllowed
// ---------------------------------------------------------------------------
//
TBool CCchImpl::ConnectivityDialogsAllowed() const
    {
    CCHLOGSTRING( "CCchImpl::ConnectivityDialogsAllowed" );
    CCHLOGSTRING2( "  --> iConnectivityDialogsAllowed=%d", 
        iConnectivityDialogsAllowed );
    
    return iConnectivityDialogsAllowed;
    }

// ---------------------------------------------------------------------------
// CCCHServiceHandler::SetConnectivityDialogsAllowed
// ---------------------------------------------------------------------------
//
void CCchImpl::SetConnectivityDialogsAllowed( 
    TBool aConnectivityDialogsAllowed )
    {
    iConnectivityDialogsAllowed = aConnectivityDialogsAllowed;
    }

// ---------------------------------------------------------------------------
// CCCHServiceHandler::CchUiApi
// ---------------------------------------------------------------------------
//
MCchUi& CCchImpl::CchUiApi() const
    {
    return *iCchUi;
    }

// ---------------------------------------------------------------------------
// CCchImpl::DoCancel
// ---------------------------------------------------------------------------
//
void CCchImpl::DoCancel()
    {
    CCHLOGSTRING( "CCchImpl::DoCancel: IN" );
	iCchClient.RegisterToServerCancel();
	CCHLOGSTRING( "CCchImpl::DoCancel: OUT" );
    }

// ---------------------------------------------------------------------------
// CCchImpl::RunL
// ---------------------------------------------------------------------------
//
void CCchImpl::RunL()
    {
    CCHLOGSTRING( "CCchImpl::RunL: IN" );
    if ( iStatus.Int() == KErrServerTerminated )
        {
        // clean
        Cancel();
        iCchClient.Close();
        iCchServer.Close();
        
        // create new
        User::LeaveIfError( iCchServer.Connect() );
        User::LeaveIfError( iCchClient.Open( iCchServer ) );

        iCchClient.RegisterToServer( iStatus );
        SetActive();
        }
    CCHLOGSTRING( "CCchImpl::RunL: OUT" );
    }
    
// ---------------------------------------------------------------------------
// CCchImpl::RunL
// ---------------------------------------------------------------------------
//
TInt CCchImpl::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }    

//  End of File
