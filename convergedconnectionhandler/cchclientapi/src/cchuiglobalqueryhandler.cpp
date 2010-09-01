/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of class CCchUiGlobalQueryHandler.
*
*/

#include <s32mem.h>
#include <AknPanic.h>
#include <aknSDData.h>
#include <AknNotifyStd.h>
#include <AknNotifySignature.h>
#include <cenrepnotifyhandler.h>

#include "cchuilogger.h"
#include "cchuiglobalqueryhandler.h"

// ======== MEMBER FUNCTIONS ========

CCchUiGlobalQueryHandler::CCchUiGlobalQueryHandler() :
    iNotifyStarted( EFalse )
    {
    }

void CCchUiGlobalQueryHandler::ConstructL()
    {
    CCHUIDEBUG( "CCchUiGlobalQueryHandler::ConstructL - IN" );
    
    User::LeaveIfError( iNotify.Connect() );
    
    CCHUIDEBUG( "CCchUiGlobalQueryHandler::ConstructL - OUT" );
    }

CCchUiGlobalQueryHandler* CCchUiGlobalQueryHandler::NewL()
    {
    CCchUiGlobalQueryHandler* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

CCchUiGlobalQueryHandler* CCchUiGlobalQueryHandler::NewLC()
    {
    CCchUiGlobalQueryHandler* self = new(ELeave) CCchUiGlobalQueryHandler;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CCchUiGlobalQueryHandler::~CCchUiGlobalQueryHandler()
    {
    CCHUIDEBUG( "CCchUiGlobalQueryHandler::~CCchUiGlobalQueryHandler - IN ");        
    
    CancelMsgQuery();
    iNotify.Close();
    
    CCHUIDEBUG( "CCchUiGlobalQueryHandler::~CCchUiGlobalQueryHandler - OUT ");        
    }

// ---------------------------------------------------------------------------
// Show query.
// ---------------------------------------------------------------------------
//
void CCchUiGlobalQueryHandler::ShowMsgQueryL( 
    MCchUiObserver::TCchUiDialogType aNote,
    TRequestStatus& aStatus,
    TUint aServiceId,
    TUint aIapId,
    RBuf& aUserName )
    {
    CCHUIDEBUG( "CCchUiGlobalQueryHandler::ShowMsgQueryL - IN" );
    CCHUIDEBUG2( "ShowMsgQueryL - aNote: %d", aNote );
    CCHUIDEBUG2( "ShowMsgQueryL - aServiceId: %d", aServiceId );
    
    iPckg().iDialogMode = aNote;
    iPckg().iServiceId = aServiceId;
    iPckg().iCurrentConnectionIapId = aIapId;
    iPckg().iUsername.Copy( aUserName );
    
    iNotify.StartNotifierAndGetResponse( 
        aStatus, 
        KCchUiNotifierUid,
        iPckg, 
        iPckg );
    
    iNotifyStarted = ETrue;
    
    CCHUIDEBUG( "CCchUiGlobalQueryHandler::ShowMsgQueryL - OUT" );
    }

// ---------------------------------------------------------------------------
// Update query.
// ---------------------------------------------------------------------------
//
void CCchUiGlobalQueryHandler::UpdateMsgQuery( TInt aSoftkeys )
    {
    CCHUIDEBUG( "CCchUiGlobalQueryHandler::UpdateMsgQuerys - IN" );
    
    iSoftkeys = aSoftkeys;
    iCmd = EAknUpdateGlobalQuery;
    TPckgBuf<SAknNotifierPackage<SAknGlobalMsgQueryParams> > pckg;
    pckg().iParamData.iCmd = iCmd;
    pckg().iParamData.iSoftkeys = iSoftkeys;

    TPckgBuf<TInt> ret;
    iNotify.UpdateNotifier( KCchUiNotifierUid, pckg, ret);
    
    CCHUIDEBUG( "CCchUiGlobalQueryHandler::UpdateMsgQuerys - OUT" );
    }

// ---------------------------------------------------------------------------
// Cancel query.
// ---------------------------------------------------------------------------
//
void CCchUiGlobalQueryHandler::CancelMsgQuery()
    {
    CCHUIDEBUG( "CCchUiGlobalQueryHandler::CancelMsgQuery - IN" );
    
    if ( iNotifyStarted )
        {
        iNotify.CancelNotifier( KCchUiNotifierUid );
        iNotifyStarted = EFalse;
        }
    
    CCHUIDEBUG( "CCchUiGlobalQueryHandler::CancelMsgQuery - OUT" );
    }

// ---------------------------------------------------------------------------
// Get notifier parameters.
// ---------------------------------------------------------------------------
//
TCCHUiNotifierParams CCchUiGlobalQueryHandler::ResultParams()
    {
    return iPckg();
    }
