/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/


#include "scppropertynotifier.h"
#include "scplogger.h"
#include "scppropertyobserver.h"

// -----------------------------------------------------------------------------
// CScpPropertyNotifier::CCScpPropertyNotifier
// -----------------------------------------------------------------------------
//
CScpPropertyNotifier::CScpPropertyNotifier( TUid aUid, 
                                            TInt aType, 
                                            MScpPropertyObserver& aObserver ) :
    CActive( CActive::EPriorityStandard ),
    iUid( aUid ),
    iType( aType ),
    iObserver( aObserver )
    {
    SCPLOGSTRING( "CScpPropertyNotifier::CScpPropertyNotifier" );

    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CScpPropertyNotifier::ConstructL
// -----------------------------------------------------------------------------
//
void CScpPropertyNotifier::ConstructL()
    {
    SCPLOGSTRING( "CScpPropertyNotifier::ConstructL" );
    
    User::LeaveIfError( iProperty.Attach( iUid, iType ) );

    Subscribe();
    }

// -----------------------------------------------------------------------------
// CScpPropertyNotifier::NewL
// -----------------------------------------------------------------------------
//
CScpPropertyNotifier* CScpPropertyNotifier::NewL( TUid aUid, 
                                                  TInt aType,
                                                  MScpPropertyObserver& aObserver )
    {
    SCPLOGSTRING( "CScpPropertyNotifier::NewL" );
    
    CScpPropertyNotifier* self = new(ELeave) CScpPropertyNotifier( aUid, 
                                                                   aType,
                                                                   aObserver );
    CleanupStack::PushL( self );    
    self->ConstructL();    
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CScpPropertyNotifier::~CScpPropertyNotifier
// -----------------------------------------------------------------------------
//
CScpPropertyNotifier::~CScpPropertyNotifier()
    {
    SCPLOGSTRING( "CScpPropertyNotifier::~CScpPropertyNotifier" );
    
    Cancel();
    iProperty.Close();
    }

// -----------------------------------------------------------------------------
// CScpPropertyNotifier::GetValue
// -----------------------------------------------------------------------------
//
TInt CScpPropertyNotifier::GetValue( TInt& aValue ) const
    {
    SCPLOGSTRING( "CScpPropertyNotifier::GetValue" );
    
    return iProperty.Get( iUid, iType, aValue );
    }

// -----------------------------------------------------------------------------
// CScpPropertyNotifier::Subscribe
// -----------------------------------------------------------------------------
//
void CScpPropertyNotifier::Subscribe()
    {
    SCPLOGSTRING( "CScpPropertyNotifier::Subscribe" );      
    __ASSERT_DEBUG(!IsActive(), User::Panic( KNullDesC, KErrGeneral ) );

    if ( !IsActive() )
        {
        iProperty.Subscribe( iStatus );    
        SetActive();
        }
    }

// -----------------------------------------------------------------------------
// CScpPropertyNotifier::RunL
// -----------------------------------------------------------------------------
//
void CScpPropertyNotifier::RunL()
    {
    SCPLOGSTRING( "CScpPropertyNotifier::RunL" );

    if( iStatus.Int() == KErrNone )
        {        
        TInt value = 0;
        GetValue( value );

        iObserver.HandlePropertyChanged( value );
        }

    Subscribe();            
    }

// -----------------------------------------------------------------------------
// CScpPropertyNotifier::NewL
// -----------------------------------------------------------------------------
//
#ifdef _DEBUG
TInt CScpPropertyNotifier::RunError( TInt aError )
    {
    SCPLOGSTRING2( "CScpPropertyNotifier::RunError: %d", aError );
#else
TInt CScpPropertyNotifier::RunError( TInt /*aError*/ )
    {
#endif
    // Errors are disarded
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CScpPropertyNotifier::NewL
// -----------------------------------------------------------------------------
//
void CScpPropertyNotifier::DoCancel()
    {
    SCPLOGSTRING( "CScpPropertyNotifier::DoCancel" );
    
    iProperty.Cancel();
    }

//  End of File  
