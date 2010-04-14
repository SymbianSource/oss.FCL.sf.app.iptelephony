/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Cch client api
*
*/


// INCLUDE FILES
#include <e32def.h>

#include "cchserviceimpl.h"
#include "cchlogger.h"
#include "cchimpl.h"
#include "cchclientobserver.h"
#include "cchclientserverinternal.h"
#include "cchserviceimplasynchroniser.h"

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
// CCchServiceImpl::CCchServiceImpl
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CCchServiceImpl::CCchServiceImpl( CCchImpl& aCch, 
                                  TInt aServiceId, 
                                  CCchUiPrivateApi& aCchUi )
    : iCch( aCch ), 
      iServiceId( aServiceId ), 
      iCchUi( aCchUi )
	{
	
    }
    
CCchServiceImpl::~CCchServiceImpl()
    {
    if( iObserver )
    	{
    	RemoveObserver();
    	}
    delete iObserver;
    delete iAsynchroniser;
    }

// ---------------------------------------------------------------------------
// CCchServiceImpl::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CCchServiceImpl::ConstructL()
    {
    CCHLOGSTRING( "CCchServiceImpl::ConstructL: IN" );
    iObserver = CCchClientObserver::NewL( *this );
    iAsynchroniser = CCchServiceImplAsynchroniser::NewL(iCch, iServiceId, iCchUi );
    CCHLOGSTRING( "CCchServiceImpl::ConstructL: OUT" );
    }
// ---------------------------------------------------------------------------
// CCchServiceImpl::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CCchServiceImpl* CCchServiceImpl::NewL( CCchImpl& aCch, 
                                        TInt aServiceId,
                                        CCchUiPrivateApi& aCchUi )
    {
    CCHLOGSTRING( "CCchServiceImpl::NewL: IN" );
    CCchServiceImpl* self = 
        CCchServiceImpl::NewLC( aCch, aServiceId, aCchUi );
    CleanupStack::Pop( self );
    CCHLOGSTRING( "CCchServiceImpl::NewL: OUT" );
    return self;
    }

// ---------------------------------------------------------------------------
// CCchServiceImpl::NewLC
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CCchServiceImpl* CCchServiceImpl::NewLC( CCchImpl& aCch, TInt aServiceId,
    CCchUiPrivateApi& aCchUi )
    {
    CCHLOGSTRING( "CCchServiceImpl::NewLC: IN" );
    CCchServiceImpl* self = 
        new (ELeave) CCchServiceImpl( aCch, aServiceId, aCchUi );
    CleanupStack::PushL( self );
    self->ConstructL();
    CCHLOGSTRING( "CCchServiceImpl::NewLC: OUT" );
    return self;
    }

// ---------------------------------------------------------------------------
// CCchServiceImpl::Enable
// ---------------------------------------------------------------------------
//
TInt CCchServiceImpl::Enable( TCCHSubserviceType aType )
    {
    CCHLOGSTRING( "CCchServiceImpl::Enable: IN" );
    TInt error = KErrNone;
    if (iCch.ConnectivityDialogsAllowed())
        {
        CCHLOGSTRING( "CCchServiceImpl::Enable: Async mode" );
        iAsynchroniser->Enable(aType);
        }
    else
        {
        CCHLOGSTRING( "CCchServiceImpl::Enable: Sync mode" );
        TServiceSelection selection( iServiceId, aType );
        TRequestStatus status = KErrNone;
        iCch.CchClient().EnableService( selection, status, EFalse );
   
        //even the cchclient api seems to be asynchronous, 
        //this method is completed immediately
        User::WaitForRequest( status );
        error = status.Int();
        }
        
    CCHLOGSTRING2( " CCchServiceImpl::Enable: return  %d", error );
    
	CCHLOGSTRING( "CCchServiceImpl::Enable: OUT" );
    return error;
    }

// ---------------------------------------------------------------------------
// CCchServiceImpl::Disable
// ---------------------------------------------------------------------------
//
TInt CCchServiceImpl::Disable( TCCHSubserviceType aType )
    {
    CCHLOGSTRING( "CCchServiceImpl::Disable: IN" );
  
    iAsynchroniser->Disable(aType);
    CCHLOGSTRING( "CCchServiceImpl::Disable: OUT" );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CCchServiceImpl::GetStatus
// ---------------------------------------------------------------------------
//
TInt CCchServiceImpl::GetStatus( TCCHSubserviceType aType, 
                                 TCchServiceStatus& aStatus ) const
    {
    CCHLOGSTRING( "CCchServiceImpl::GetStatus: IN" );
    TCCHSubserviceState state( ECCHUninitialized );
    TServiceSelection selection( iServiceId, aType );
    TInt error = iCch.CchClient().GetServiceState( selection, state );
    //if this is a cch error, return it in the structure, otherwise in return
    if( KCCHErrorInvalidIap >= error || KErrNone == error )
    	{
        aStatus.SetError( error );
        aStatus.SetState( state );
        error = KErrNone;
    	}
    CCHLOGSTRING( "CCchServiceImpl::GetStatus: OUT" );
    return error;
    }

// ---------------------------------------------------------------------------
// CCchServiceImpl::GetConnectionParameter
// ---------------------------------------------------------------------------
//
TInt CCchServiceImpl::GetConnectionParameter( TCCHSubserviceType aType, 
		TCchConnectionParameter aParameter, TInt& aValue ) const
    {
    CCHLOGSTRING( "CCchServiceImpl::GetConnectionParameter: IN" );
    TRequestStatus status = KErrNone;
    
    switch( aParameter )
    {
    	case ECchIapId:
    	case ECchSnapId:
    	case ECchSnapLocked:
        case ECchPasswordSet:
        case ECchReserved:
    		{
    		TServiceSelection selection( iServiceId, aType, aParameter );
    	    TServiceConnectionInfo serviceConnInfo;
    	    TPckgBuf<TServiceConnectionInfo> serviceConnInfoPckg;
    	    
    		iCch.CchClient().GetConnectionInfo( selection, 
    	    		serviceConnInfoPckg, status );
    	    //even the cchclient api seems to be asynchronous, 
    		//this method is completed immediately
    		User::WaitForRequest( status );
    		if ( KErrNone == status.Int() )
    	        {
    	        serviceConnInfo = serviceConnInfoPckg();
    	        if( ECchSnapId == aParameter )
    	        	{
    	        	aValue = serviceConnInfo.iSNAPId;
    	        	}
    	        else if( ECchIapId == aParameter )
    	        	{
    	        	aValue = serviceConnInfo.iIapId;
    	        	}
    	        else if( ECchSnapLocked == aParameter )
    	        	{
    	        	aValue = serviceConnInfo.iSNAPLocked;
    	        	}
    	        else  if( ECchPasswordSet == aParameter )
                    {
                    aValue = serviceConnInfo.iPasswordSet;
                    }
                else  if( ECchReserved == aParameter )
                    {
                    aValue = serviceConnInfo.iReserved;
                    }
    	        }
    		}
    		break;
        default:
            {
            status = KErrArgument;
            }
            break;
    }
    
	CCHLOGSTRING( "CCchServiceImpl::GetConnectionParameter: OUT" );
    return status.Int();
    }

// ---------------------------------------------------------------------------
// CCchServiceImpl::SetConnectionParameter
// ---------------------------------------------------------------------------
//
TInt CCchServiceImpl::SetConnectionParameter( TCCHSubserviceType aType, 
		TCchConnectionParameter aParameter, TInt aValue )
    {
    CCHLOGSTRING( "CCchServiceImpl::SetConnectionParameter: IN" );
    TPckgBuf<TServiceConnectionInfo> serviceConnInfoPckg;
    TRequestStatus status = KErrNone;
    TInt error = KErrNone;
    
    switch( aParameter )
    {
    	case ECchIapId:
    	case ECchSnapId:
        case ECchReserved:
    		{
    		serviceConnInfoPckg().SetServiceId( iServiceId );
    	    serviceConnInfoPckg().SetType( aType );
    	    if( ECchSnapId == aParameter )
	        	{
	        	serviceConnInfoPckg().iSNAPId = aValue;
	        	}
	        else if ( ECchIapId == aParameter )
	        	{
	        	serviceConnInfoPckg().iIapId = aValue;
	        	}
            else if ( ECchReserved == aParameter )
                {
                serviceConnInfoPckg().SetParameter( aParameter );
                serviceConnInfoPckg().iReserved = aValue;
                }
            else
                {
                return KErrArgument;
                }
    	    iCch.CchClient().SetConnectionInfo( serviceConnInfoPckg, status );
    	    //even the cchclient api seems to be asynchronous, 
    		//this method is completed immediately
    		User::WaitForRequest( status );
    		error = status.Int();
    		}
    		break;
    	case ECchSnapLocked:
        case ECchPasswordSet:
    		{
    		error = KErrNotSupported;
    		}
    		break;
        default:
            {
            error = KErrArgument;
            }
            break;
    }
    
    CCHLOGSTRING( "CCchServiceImpl::SetConnectionParameter: OUT" );
    return error;
    }

// ---------------------------------------------------------------------------
// CCchServiceImpl::GetConnectionParameters
// ---------------------------------------------------------------------------
//
TInt CCchServiceImpl::GetConnectionParameter( TCCHSubserviceType aType, 
		TCchConnectionParameter aParameter, RBuf& aValue ) const
    {
    CCHLOGSTRING( "CCchServiceImpl::GetConnectionParameter: IN" );
    TServiceSelection selection( iServiceId, aType, aParameter );
    TRequestStatus status = KErrNone;
    TInt error = KErrNone; 
    
    switch( aParameter )
    {
        case ECchServiceInfo:
    		{
    		error = iCch.CchClient().GetServiceInfo( selection, aValue );
    		}
    		break;

        case ECchUsername:
            {
            TServiceConnectionInfo serviceConnInfo;
            TPckgBuf<TServiceConnectionInfo> serviceConnInfoPckg;
            
            iCch.CchClient().GetConnectionInfo( selection,serviceConnInfoPckg, status );
            //even the cchclient api seems to be asynchronous, 
            //this method is completed immediately
            User::WaitForRequest( status );

            serviceConnInfo = serviceConnInfoPckg();
            aValue = serviceConnInfo.iUsername;
            }
            break;
        case ECchPassword:
            {
            error = KErrNotSupported;
            }
            break;
        default:
            {
            error = KErrArgument;
    		}
    		break;
    }
    
	CCHLOGSTRING( "CCchServiceImpl::GetConnectionParameter: OUT" );
    return error;
    }

// ---------------------------------------------------------------------------
// CCchServiceImpl::SetConnectionParameters
// ---------------------------------------------------------------------------
//
TInt CCchServiceImpl::SetConnectionParameter( TCCHSubserviceType aType, 
		TCchConnectionParameter aParameter, const TDesC& aValue )
    {
    CCHLOGSTRING( "CCchServiceImpl::SetConnectionParameter: IN" );
    CCHLOGSTRING2( "    aValue:     %S", &aValue );

    // Type of subservice has to be ECCHUnknown for username and password
    __ASSERT_ALWAYS( aType == ECCHUnknown,
                    User::Panic( KNullDesC, KErrArgument ) );
    
    TPckgBuf<TServiceConnectionInfo> serviceConnInfoPckg;
    TRequestStatus status = KErrNone;
    TInt error = KErrNone;

    switch( aParameter )
    {
        case ECchUsername:
            {
            serviceConnInfoPckg().SetServiceId( iServiceId );
            serviceConnInfoPckg().SetType( aType );
            serviceConnInfoPckg().SetParameter( aParameter );

            if ( aValue.Length() < KCCHMaxUsernameLength )
                {
                serviceConnInfoPckg().iUsername.Append( aValue );
                
                iCch.CchClient().SetConnectionInfo( serviceConnInfoPckg, status );
                //even the cchclient api seems to be asynchronous, 
                //this method is completed immediately
                User::WaitForRequest( status );
                error = status.Int();
                }
            else
                {
                error = KErrArgument;
                }
            }
            break;
        case ECchPassword:
            {
            serviceConnInfoPckg().SetServiceId( iServiceId );
            serviceConnInfoPckg().SetType( aType );
            serviceConnInfoPckg().SetParameter( aParameter );
            if ( aValue.Length() < KCCHMaxPasswordLength )
                {
                serviceConnInfoPckg().iPassword.Append( aValue );
                
                iCch.CchClient().SetConnectionInfo( serviceConnInfoPckg, status );
                //even the cchclient api seems to be asynchronous, 
                //this method is completed immediately
                User::WaitForRequest( status );
                error = status.Int();
                }
            else
                {
                error = KErrArgument;
                }
            }
            break;
        default:
            {
            error = KErrArgument;
            }
            break;
    }
    
    CCHLOGSTRING( "CCchServiceImpl::SetConnectionParameter: OUT" );
    return error;
    }

// ---------------------------------------------------------------------------
// CCchServiceImpl::Reserve
// ---------------------------------------------------------------------------
//
TInt CCchServiceImpl::Reserve( TCCHSubserviceType aType )
    {
    CCHLOGSTRING( "CCchServiceImpl::Reserve: IN" );
    TServiceSelection selection( iServiceId, aType );
    TInt error = iCch.CchClient().ReserveService( selection );
    CCHLOGSTRING( "CCchServiceImpl::Reserve: OUT" );
    return error;
    }

// ---------------------------------------------------------------------------
// CCchServiceImpl::Free
// ---------------------------------------------------------------------------
//
TInt CCchServiceImpl::Free( TCCHSubserviceType aType )
    {
    CCHLOGSTRING( "CCchServiceImpl::Free: IN" );
    TServiceSelection selection( iServiceId, aType );
    TInt error = iCch.CchClient().FreeService( selection );
    CCHLOGSTRING( "CCchServiceImpl::Free: OUT" );
    return error;
    }

// ---------------------------------------------------------------------------
// CCchServiceImpl::IsReserved
// ---------------------------------------------------------------------------
//
TInt CCchServiceImpl::IsReserved( TCCHSubserviceType aType, 
                                   TBool& aReserved ) const
    {
    CCHLOGSTRING( "CCchServiceImpl::IsReserved: IN" );
    TServiceSelection selection( iServiceId, aType );
    aReserved = iCch.CchClient().IsReserved( selection );
    CCHLOGSTRING( "CCchServiceImpl::IsReserved: OUT" );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CCchServiceImpl::ServiceId
// ---------------------------------------------------------------------------
//
TInt CCchServiceImpl::ServiceId() const
    {
    CCHLOGSTRING( "CCchServiceImpl::ServiceId: IN" );
    return iServiceId;
    }

// ---------------------------------------------------------------------------
// CCchServiceImpl::IsSupported
// ---------------------------------------------------------------------------
//
TInt CCchServiceImpl::IsSupported( TCCHSubserviceType aType, 
                                   TBool& aSupported ) const
    {
    CCHLOGSTRING( "CCchServiceImpl::IsSupported: IN" );
    TCCHSubserviceState state( ECCHUninitialized );
    TServiceSelection selection( iServiceId, aType );
    TInt error = iCch.CchClient().GetServiceState( selection, state );
    if( error == KErrNone ||
    	error <= KCCHErrorInvalidIap ) // cch errors are ok
    	{
    	aSupported = ETrue;
    	}
    else
    	{
    	aSupported = EFalse;
    	}
	
	CCHLOGSTRING( "CCchServiceImpl::IsSupported: OUT" );
    return error;
    }      

// ---------------------------------------------------------------------------
// CCchClientObserver::SetObserver
// ---------------------------------------------------------------------------
//
void CCchServiceImpl::SetObserver( MCchServiceStatusObserver& aObserver )
    {
    CCHLOGSTRING( "CCchServiceImpl::SetObserver: IN - deprecated - do not use this anymore " );
    iObserver->SetObserver( aObserver );
    CCHLOGSTRING( "CCchServiceImpl::SetObserver: OUT  - deprecated - do not use this anymore " );
    }
    
// ---------------------------------------------------------------------------
// CCchClientObserver::RemoveObserver
// ---------------------------------------------------------------------------
//
void CCchServiceImpl::RemoveObserver( )
    {
    CCHLOGSTRING( "CCchServiceImpl::RemoveObserver: IN - deprecated - do not use this anymore " );
    iObserver->RemoveObserver( );
    CCHLOGSTRING( "CCchServiceImpl::RemoveObserver: OUT - deprecated - do not use this anymore " );
    }

// ---------------------------------------------------------------------------
// CCchClientObserver::AddObserver
// ---------------------------------------------------------------------------
//
TInt CCchServiceImpl::AddObserver( 
                      MCchServiceStatusObserver& aObserver )
    {
    CCHLOGSTRING( "CCchServiceImpl::AddObserver: IN" );
    TInt err = iObserver->AddObserver( aObserver );
    CCHLOGSTRING( "CCchServiceImpl::AddObserver: OUT" );
    
    return err;
    }

// ---------------------------------------------------------------------------
// CCchClientObserver::RemoveObserver
// ---------------------------------------------------------------------------
//
TInt CCchServiceImpl::RemoveObserver( 
                     MCchServiceStatusObserver& aObserver )
    {
    CCHLOGSTRING( "CCchServiceImpl::RemoveObserver: IN" );
    TInt err = iObserver->RemoveObserver( aObserver );
    CCHLOGSTRING( "CCchServiceImpl::RemoveObserver: OUT" );
    
    return err;
    }


// ---------------------------------------------------------------------------
// CCchClientObserver::CchImpl
// ---------------------------------------------------------------------------
//
CCchImpl* CCchServiceImpl::CchImpl() const
	{
	return &iCch;
	}



// ========================== OTHER EXPORTED FUNCTIONS =========================


//  End of File
