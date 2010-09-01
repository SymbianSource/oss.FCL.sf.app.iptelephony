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
* Description:  Implementation of class CCchUiPrivateApi.
*
*/


#include <cch.h>
#include "cchuiprivateapi.h"
#include "cchuiapiimpl.h"
#include "cchuilogger.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//    
CCchUiPrivateApi::CCchUiPrivateApi()
    {
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//    
void CCchUiPrivateApi::ConstructL( CCch& aCch )
    {
    iImpl = CCchUiApiImpl::NewL( aCch );
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//    
CCchUiPrivateApi* CCchUiPrivateApi::NewL( CCch& aCch )
    {
    CCHUIDEBUG( "CCchUiPrivateApi::NewL" );
    
    CCchUiPrivateApi* privateApi = new ( ELeave ) CCchUiPrivateApi();
    CleanupStack::PushL( privateApi );
    privateApi->ConstructL( aCch );
    CleanupStack::Pop( privateApi ); 
    return privateApi;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//    
CCchUiPrivateApi::~CCchUiPrivateApi()
    {
    CCHUIDEBUG( "CCchUiPrivateApi::~CCchUiPrivateApi - IN" );

    delete iImpl;
    
    CCHUIDEBUG( "CCchUiPrivateApi::~CCchUiPrivateApi - OUT" );
    }
    
// ---------------------------------------------------------------------------
// Manual enable has proceeded
// ---------------------------------------------------------------------------
//
void CCchUiPrivateApi::ManualEnableResultL( 
    TUint32 aServiceId, TInt aEnableResult )
    {
    CCHUIDEBUG2( "CCchUiPrivateApi::ManualEnableStartedL - aServiceId: %d", 
        aServiceId );
    CCHUIDEBUG2( "CCchUiPrivateApi::ManualEnableStartedL - aEnableResult: %d", 
        aEnableResult );        
    iImpl->ManualEnableResultL( aServiceId, aEnableResult );
    }
    
// ---------------------------------------------------------------------------
// AddObserver
// ---------------------------------------------------------------------------
//
void CCchUiPrivateApi::AddObserverL( MCchUiObserver& aObserver )
    {
    iImpl->AddObserverL( aObserver );
    }

// ---------------------------------------------------------------------------
// RemoveObserver
// ---------------------------------------------------------------------------
//
void CCchUiPrivateApi::RemoveObserver( MCchUiObserver& aObserver )
    {
    iImpl->RemoveObserver( aObserver );
    }

// ---------------------------------------------------------------------------
// ShowDialogL
// ---------------------------------------------------------------------------
//
void CCchUiPrivateApi::ShowDialogL( 
    TUint32 aServiceId, 
    MCchUiObserver::TCchUiDialogType aDialog )
    {
    iImpl->ShowDialogL( aServiceId, aDialog );
    }

// ---------------------------------------------------------------------------
// ConfigureVisualizationL
// ---------------------------------------------------------------------------
//
void CCchUiPrivateApi::ConfigureVisualizationL( 
    RArray<MCchUiObserver::TCchUiDialogType>& aAllowedNotes,
    RArray<TCCHSubserviceType>& aAllowedSubServices )
    {
    iImpl->ConfigureVisualizationL( aAllowedNotes, aAllowedSubServices );
    }

// ---------------------------------------------------------------------------
// CancelNotes
// ---------------------------------------------------------------------------
//
void CCchUiPrivateApi::CancelNotes()
    {
    iImpl->CancelNotes();
    }

// ---------------------------------------------------------------------------
// Reserved1
// ---------------------------------------------------------------------------
//
void CCchUiPrivateApi::Reserved1()
    {
    }

// ---------------------------------------------------------------------------
// Reserved2
// ---------------------------------------------------------------------------
//
void CCchUiPrivateApi::Reserved2()
    {
    }
