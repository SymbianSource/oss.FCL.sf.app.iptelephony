/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Active object for P&S property watching.
*
*/


#include "svppropertywatch.h"
#include "svplogger.h"



// ---------------------------------------------------------------------------
// CSVPPropertyWatch::CSVPPropertyWatch
// ---------------------------------------------------------------------------
//
CSVPPropertyWatch::CSVPPropertyWatch( MSVPPropertyWatchObserver& aClient,
    TInt aKey ) : CActive( EPriorityStandard ), iClient( aClient ), 
                                                      iKey( aKey )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CSVPPropertyWatch::ConstructL
// ---------------------------------------------------------------------------
//
void CSVPPropertyWatch::ConstructL( const TUid& aCategory )
    {
    SVPDEBUG1("CSVPPropertyWatch::ConstructL - ATTACHING TO PROPERTY...");
    User::LeaveIfError( iProperty.Attach( aCategory, iKey ) );

    SVPDEBUG1("CSVPPropertyWatch::ConstructL - SUBSCRIBING TO PROPERTY...");
    iProperty.Subscribe( iStatus );

    SetActive();
    }

// ---------------------------------------------------------------------------
// CSVPPropertyWatch::NewL
// ---------------------------------------------------------------------------
//
CSVPPropertyWatch* CSVPPropertyWatch::NewL( 
    MSVPPropertyWatchObserver& aClient,
    const TUid& aCategory, TInt aKey )
    {
    CSVPPropertyWatch* self = new( ELeave ) CSVPPropertyWatch( aClient, 
                                                               aKey );
    
    CleanupStack::PushL( self );
    self->ConstructL( aCategory );
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------------------------
// CSVPPropertyWatch::~CSVPPropertyWatch
// ---------------------------------------------------------------------------
//
CSVPPropertyWatch::~CSVPPropertyWatch()
    {
    Cancel();
    iProperty.Close();
    }

// ---------------------------------------------------------------------------
// CSVPPropertyWatch::DoCancel
// ---------------------------------------------------------------------------
//
void CSVPPropertyWatch::DoCancel()
    {
    iProperty.Cancel();
    }

// ---------------------------------------------------------------------------
// CSVPPropertyWatch::RunL
// ---------------------------------------------------------------------------
//
void CSVPPropertyWatch::RunL()
    {
    if( KErrNone == iStatus.Int() )
        {
        // Resubscribe before processing new value to prevent missing updates
        iProperty.Subscribe( iStatus );
        SetActive();

        // Property updated, get new value
        TInt newValue;
        if ( KErrNotFound == iProperty.Get( newValue ) )
            {
            iClient.PropertyDeleted( iKey );
            }
        else
            {
            iClient.ValueChangedL( iKey, newValue );
            }
        }
    else
        {  
        User::Leave( iStatus.Int() );
        }
    }

// ---------------------------------------------------------------------------
// CSVPPropertyWatch::RunError
// ---------------------------------------------------------------------------
//
TInt CSVPPropertyWatch::RunError( TInt /* aError */)
    {
    return KErrNone;
    }

