/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Sends and receives DTMF tones
*
*/

#include <cconvergedcallprovider.h>

#include "cvccdtmfprovider.h"
#include "cvccdirector.h"
#include "rubydebug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Symbian constructor
// ---------------------------------------------------------------------------
//
CVccDtmfProvider* CVccDtmfProvider::NewL(
        RPointerArray<CConvergedCallProvider>& /*aProviders*/, 
        const MCCPDTMFObserver& aObserver,
        CVccDirector& aDirector )
	{
    RUBY_DEBUG_BLOCK( "CVccDtmfProvider::NewL" );
	CVccDtmfProvider* self;
	self = new( ELeave ) CVccDtmfProvider( aObserver, aDirector );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CVccDtmfProvider::~CVccDtmfProvider()
	{
	iObservers.Close();
	iProviders.Close();
	}

// ---------------------------------------------------------------------------
// CVccDtmfProvider::CVccDtmfProvider
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CVccDtmfProvider::CVccDtmfProvider( const MCCPDTMFObserver& aObserver,
                                    CVccDirector& aDirector):
    iDirector( &aDirector )                                   
    {
    RUBY_DEBUG_BLOCK( "CVccDtmfProvider::CVccDtmfProvider" );		
    iObservers.InsertInAddressOrder( &aObserver );
    }
		
// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CVccDtmfProvider::ConstructL()
	{
    RUBY_DEBUG_BLOCKL( "CVccDtmfProvider::ConstructL" );
	}


// ---------------------------------------------------------------------------
// @ see MCCPDTMFObserver::HandleDTMFEvent
// ---------------------------------------------------------------------------
//
void CVccDtmfProvider::HandleDTMFEvent( const MCCPDTMFObserver::TCCPDtmfEvent aEvent,
								        const TInt aError,
								        const TChar aTone ) const
    {
    RUBY_DEBUG_BLOCK( "CVccDtmfProvider::HandleDTMFEvent" );	
    RUBY_DEBUG1( "CVccDtmfProvider::HandleDTMFEvent -- obs count: %d",
                                                  iObservers.Count() );
    for ( TInt i = 0; i < iObservers.Count(); i++ )
        {
        iObservers[i]->HandleDTMFEvent( aEvent, aError, aTone );
        }    
    }

// ---------------------------------------------------------------------------
// @see MCCPDTMFProvider::CancelDtmfStringSending
// ---------------------------------------------------------------------------
//
TInt CVccDtmfProvider::CancelDtmfStringSending()
	{
    RUBY_DEBUG_BLOCK( "CVccDtmfProvider::CancelDtmfStringSending" );
    RUBY_DEBUG1( "-- call type = %d (0 = cs, 4 = ps)",
                        iDirector->CurrentCallTypeForDTMF() );
    TInt err( KErrNotSupported );
    
    CConvergedCallProvider* prov = iDirector->GetProvider();
    if( prov )
        {
        MCCPDTMFProvider* dtmf = NULL;
        TRAPD( err1, dtmf = prov->DTMFProviderL( *this ) );
        if( err1 == KErrNone )
            {
            err = dtmf->CancelDtmfStringSending();
            }
        else
            {
            err = err1;
            }
         
        }
    
    RUBY_DEBUG1( "err = %d ", err );
  
    return err;
	}

// ---------------------------------------------------------------------------
// @see MCCPDTMFProvider::StartDtmfTone
// ---------------------------------------------------------------------------
//
TInt CVccDtmfProvider::StartDtmfTone( const TChar aTone )
	{
    RUBY_DEBUG_BLOCK( "CVccDtmfProvider::StartDtmfTone" );
    RUBY_DEBUG1( "-- call type = %d (0 = cs, 4 = ps)",
                         iDirector->CurrentCallTypeForDTMF() );
    TInt err( KErrNotSupported );
    
    CConvergedCallProvider* prov = iDirector->GetProvider();
    if( prov )
        {
        MCCPDTMFProvider* dtmf = NULL;
        TRAPD( err1, dtmf = prov->DTMFProviderL( *this ) );
        if( err1 == KErrNone )
            {
            err = dtmf->StartDtmfTone( aTone );
            }
        else
            {
            err = err1;
            }
        }
    return err;
	}

// ---------------------------------------------------------------------------
// @see MCCPDTMFProvider::StopDtmfTone
// ---------------------------------------------------------------------------
//
TInt CVccDtmfProvider::StopDtmfTone()
	{
    RUBY_DEBUG_BLOCK( "CVccDtmfProvider::StopDtmfTone" );
    RUBY_DEBUG1( "-- call type = %d (0 = cs, 4 = ps)",
                       iDirector->CurrentCallTypeForDTMF() );
    TInt err( KErrNotSupported );
    CConvergedCallProvider* prov = iDirector->GetProvider();
    if( prov )
        {
        MCCPDTMFProvider* dtmf = NULL;
        TRAPD( err1, dtmf = prov->DTMFProviderL( *this ) );
        if( err1 == KErrNone )
            {
            err = dtmf->StopDtmfTone();
            }
        else
            {
            err = err1;
            }
        }
    return err;
	}

// ---------------------------------------------------------------------------
// @see MCCPDTMFProvider::SendDtmfToneString
// ---------------------------------------------------------------------------
//
TInt CVccDtmfProvider::SendDtmfToneString( const TDesC& aString )
	{
    RUBY_DEBUG_BLOCK( "CVccDtmfProvider::SendDtmfToneString" );
    RUBY_DEBUG1( "-- call type = %d (0 = cs, 4 = ps)",
                      iDirector->CurrentCallTypeForDTMF() );
    TInt err( KErrNotSupported );
    
    CConvergedCallProvider* prov = iDirector->GetProvider();
    if( prov )
        {
        MCCPDTMFProvider* dtmf = NULL;
        TRAPD( err1, dtmf = prov->DTMFProviderL( *this ) );
        if( err1 == KErrNone )
            {
            err = dtmf->SendDtmfToneString( aString );
            }
        else
            {
            err = err1;
            }
        }
    RUBY_DEBUG1( "err = %d ", err );
   
    return err;
	}

// ---------------------------------------------------------------------------
// @see MCCPDTMFProvider::ContinueDtmfStringSending
// ---------------------------------------------------------------------------
//
TInt CVccDtmfProvider::ContinueDtmfStringSending( const TBool aContinue )
	{
    RUBY_DEBUG_BLOCK( "CVccDtmfProvider::ContinueDtmfStringSending" );
    RUBY_DEBUG1( "-- call type = %d (0 = cs, 4 = ps)", 
                        iDirector->CurrentCallTypeForDTMF() );
    TInt err ( KErrNotSupported );
    
    CConvergedCallProvider* prov = iDirector->GetProvider();
    if( prov )
        {
        MCCPDTMFProvider* dtmf = NULL;
        TRAPD( err1, dtmf = prov->DTMFProviderL( *this ) );
        if( err1 == KErrNone )
            {
            err = dtmf->ContinueDtmfStringSending( aContinue );
            }
        else
            {
            err = err1;
            }
        }
    return err;
    }

// ---------------------------------------------------------------------------
// @see MCCPDTMFProvider::AddObserverL
// ---------------------------------------------------------------------------
//
void CVccDtmfProvider::AddObserverL( const MCCPDTMFObserver& aObserver )
	{
    RUBY_DEBUG_BLOCK( "CVccDtmfProvider::AddObserverL" );
    iObservers.InsertInAddressOrderL( &aObserver );
	}

// ---------------------------------------------------------------------------
// @see MCCPDTMFProvider::RemoveObserver
// ---------------------------------------------------------------------------
//
TInt CVccDtmfProvider::RemoveObserver( const MCCPDTMFObserver& aObserver )
	{
    RUBY_DEBUG_BLOCK( "CVccDtmfProvider::RemoveObserver" );
    TInt res = iObservers.Find( &aObserver );
    if ( res != KErrNotFound )
        {
        iObservers.Remove( res );
        res = KErrNone;
        }
    return res;
	}
