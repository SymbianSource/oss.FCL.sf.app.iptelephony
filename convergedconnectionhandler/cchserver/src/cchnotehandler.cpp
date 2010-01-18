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
* Description:  CCh note handler.
*
*/

// INCLUDE FILES
#include "cchnotehandler.h"
#include "cchprivatecrkeys.h"
#include "cchserverbase.h"
#include "cchfeaturemanager.h"
#include <StringLoader.h>
#include <cch.rsg>
#include <AknGlobalNote.h>
#include <AknGlobalMsgQuery.h> 
#include "cchsecondarydisplayapi.h"
#include <aknSDData.h>
#include <barsread.h>
#include <bautils.h>
#include "cchlogger.h"

_LIT( KCchResourceDir, "\\resource\\apps\\" );
_LIT( KCchResourceFileName, "cch.r??" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCchNoteHandler::CCchNoteHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCchNoteHandler::CCchNoteHandler( CCCHServerBase& aServer )
      : CActive ( EPriorityHigh ),
      iServer( aServer ),
      iCoverDisplaySupported( aServer.FeatureManager().CoverDisplaySupported() )
    {
    }

// -----------------------------------------------------------------------------
// CCchNoteHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCchNoteHandler* CCchNoteHandler::NewL( CCCHServerBase& aServer )
    {
    CCchNoteHandler* self = 
        new (ELeave) CCchNoteHandler( aServer );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// ----------------------------------------------------------------------------
// CCchNoteHandler::ConstructL()
// ----------------------------------------------------------------------------

void CCchNoteHandler::ConstructL()
    {
    iGlobalMsgQuery = CAknGlobalMsgQuery::NewL();
    iCchCenRep = CRepository::NewL( KCRUidCch );
    User::LeaveIfError( iFsSession.Connect() );
    iResourceFile = ScanFileL( TFileName( KCchResourceFileName ),
         TFileName( KCchResourceDir ) );
    User::LeaveIfNull( iResourceFile );
    CActiveScheduler::Add( this );
    }    

// ----------------------------------------------------------------------------
// CCchNoteHandler::~CCchNoteHandler()
// Destructor
// ----------------------------------------------------------------------------

CCchNoteHandler::~CCchNoteHandler()
    {
    Cancel();
    iMsgQueryInfoArray.Close();
    iFsSession.Close();
    delete iResourceFile;
    delete iCchCenRep;
    delete iGlobalMsgQuery;
    }

// -----------------------------------------------------------------------------
// CCchNoteHandler::RunL
// From CActive, handles note/query dismissal.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCchNoteHandler::RunL()
    {
    TInt status = iStatus.Int();
    CCHLOGSTRING2( "CCchNoteHandler::RunL iResourceId: %d", iResourceId );

    // update central repository
    switch ( iResourceId )
        {
        case R_QTN_FIRST_EMERGENCY_WARNING_NOTE:
            {
            if ( status == EAknSoftkeyYes || status == EAknSoftkeyOk )
                {
                iCchCenRep->Set( 
                    KCCHVoIPEmergencyWarningShown, 
                    ECCHVoIPEmergencyWarningAlreadyShown );
                }
            if( iDie )
            	{

            	delete this;
                return;
            	}
            else
                {
                // now we are show this emergency note -> we can show
                // another if needed
                iServer.SetVoIPEmergencyNoteShown( EFalse );
                }
            break;
            }
        case R_QTN_VOIP_DO_NOT_SHOW_WARNING:
            {
            if ( status == EAknSoftkeyYes || status == EAknSoftkeyOk )
                {
                iCchCenRep->Set( 
                    KCCHVoIPShowEmergencyWarningOnOff,
                    ECCHVoIPEmergencyWarningOnOffDoNotShowWarning );  
                
                }
            if( iDie )
            	{

            	delete this;
                return;
            	}
            else
                {
                // now we are show this emergency note -> we can show
                // another if needed
                iServer.SetVoIPEmergencyNoteShown( EFalse );
                }
            break;
            }
        case R_QTN_SERVTAB_ALLOW_GPRS_WHEN_ROAMING_QUERY:
            {
            if ( status == EAknSoftkeyYes || status == EAknSoftkeyOk )
                {       
                // Set gprs roaming cost warning note as shown to cenrep
                iCchCenRep->Set( 
                    KCCHGprsRoamingCostWarningShown, 
                    ECCHGprsRoamingCostWarningAlreadyShown );
                }
            }
        default:
            break;
        }
        
    // delete self
    if( iResourceId != R_QTN_VOIP_EM_CALL_ERROR_NOTE_NO_CS && iDie )
        {
        delete this;
        return;
        }
        
    // peek one message from query array
    if( iMsgQueryInfoArray.Count() > 0 )
        {
        TGlobalMsgQueryInfo msgQueryInfo = iMsgQueryInfoArray[0];
        DoLaunchGlobalNoteL( msgQueryInfo.iResourceId,
                msgQueryInfo.iSoftkeyId,
                msgQueryInfo.iSecondaryDisplayIndex );
        
        iMsgQueryInfoArray.Remove( 0 );
        }
    }

// -----------------------------------------------------------------------------
// CCchNoteHandler::CanBeDestroyed
// -----------------------------------------------------------------------------
//
TBool CCchNoteHandler::CanBeDestroyed()
	{
	if( IsActive() )
		{
		iDie = ETrue;
		return EFalse;
		}
	else
		{
		return ETrue;
		}
	}

// -----------------------------------------------------------------------------
// CCchNoteHandler::DoCancel
// From CActive, cancels current operation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCchNoteHandler::DoCancel()
    {
    iGlobalMsgQuery->CancelMsgQuery();
    }

// -----------------------------------------------------------------------------
// CCchNoteHandler::RunError
// From CActive, handles exceptions in RunL.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCchNoteHandler::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }
    

// -----------------------------------------------------------------------------
// CCchNoteHandler::LaunchGlobalNoteL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void CCchNoteHandler::LaunchGlobalNoteL( const TInt aResourceId, 
        const TInt aSoftKeyConfig,
        const TInt aSecondaryDisplayIndex )
    {
    // Check there is global message query is shown at the moment
    if ( IsActive() )
        {
        // add the message query info to array
        TGlobalMsgQueryInfo msgQueryInfo;
        msgQueryInfo.iResourceId = aResourceId;
        msgQueryInfo.iSoftkeyId = aSoftKeyConfig;
        msgQueryInfo.iSecondaryDisplayIndex = aSecondaryDisplayIndex;
        
        iMsgQueryInfoArray.AppendL( msgQueryInfo );
        }
    else
        {
        DoLaunchGlobalNoteL( aResourceId, aSoftKeyConfig, aSecondaryDisplayIndex );
        }
        
    if( aResourceId == R_QTN_VOIP_EM_CALL_ERROR_NOTE_NO_CS )
        {
        TGlobalMsgQueryInfo msgQueryInfo;
        msgQueryInfo.iResourceId = R_QTN_VOIP_DO_NOT_SHOW_WARNING;
        msgQueryInfo.iSoftkeyId = R_AVKON_SOFTKEYS_OK_CANCEL;
        msgQueryInfo.iSecondaryDisplayIndex = 
            SecondaryDisplay::ECmdShowDoNotShowVoipEmergencyCallWarningQuery;
        
        iMsgQueryInfoArray.AppendL( msgQueryInfo );
        }
        
    }

// -----------------------------------------------------------------------------
// CCchNoteHandler::DoLaunchGlobalNoteL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void CCchNoteHandler::DoLaunchGlobalNoteL( const TInt aResourceId, 
        const TInt aSoftKeyConfig,
        const TInt aSecondaryDisplayIndex )
    {
    iResourceId = aResourceId;
    HBufC* textBuffer = LoadResourceL( aResourceId );
    CleanupStack::PushL( textBuffer );    
         
    // Set secondary display data if necessary
    if ( iCoverDisplaySupported )
        {
        CAknSDData* sd = CAknSDData::NewL(
            SecondaryDisplay::KCatCch, 
            aSecondaryDisplayIndex, 
            KNullDesC8);
           
        iGlobalMsgQuery->SetSecondaryDisplayData(sd); 
        }    
       
    iGlobalMsgQuery->ShowMsgQueryL( 
        iStatus, 
        textBuffer->Des(), 
        aSoftKeyConfig, 
        KNullDesC, 
        KNullDesC ); 

    CleanupStack::PopAndDestroy( textBuffer );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CCchNoteHandler::LoadResourceL
// Loads the localized text resource from resource file
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC* CCchNoteHandler::LoadResourceL( TInt aResourceID )
    {
    RResourceFile resourceFile;
    CleanupClosePushL( resourceFile );    
    resourceFile.OpenL( iFsSession, *iResourceFile );
	resourceFile.ConfirmSignatureL( 0 );
	HBufC8* readBuffer=resourceFile.AllocReadLC( 
	    aResourceID );
    const TPtrC16 ptrReadBuffer( 
        (TText16*) readBuffer->Ptr(),(readBuffer->Length()+1) >> 1 );
    HBufC* textBuffer=HBufC16::NewL( ptrReadBuffer.Length() );
    *textBuffer=ptrReadBuffer;
    CleanupStack::PopAndDestroy( readBuffer );
    CleanupStack::PopAndDestroy( &resourceFile );
    return textBuffer;   
    }

// -----------------------------------------------------------------------------
// CCchNoteHandler::ScanFileC
// Finds out the correct path for a file
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC* CCchNoteHandler::ScanFileL( const TDesC& aFileName, 
                                             const TDesC& aFilePath )
    {
    CCHLOGSTRING( "CCchNoteHandler::ScanFileL: IN" );
    TFindFile search( iFsSession); 
    CDir* dirlist; 
    HBufC* fullName = NULL;
    TFileName filename(aFileName);
    TInt err = search.FindWildByDir( filename, aFilePath, dirlist ); 
    if (err == KErrNone)
        {
        CCHLOGSTRING( "CCchNoteHandler::ScanFileL: dir found" );
        if ( dirlist && 0 < dirlist->Count() )
            {
            TParse fullentry;
            fullentry.Set( (*dirlist)[0].iName, &search.File(), NULL ); 
            TFileName nearestFile( fullentry.FullName() );
            BaflUtils::NearestLanguageFile( iFsSession, nearestFile );   
            
            fullName = HBufC::NewL( nearestFile.Length() );
            fullName->Des().Copy( nearestFile );  
            }
        delete dirlist; 
        }     
    CCHLOGSTRING2( "CCchNoteHandler::ScanFileL: OUT value=%s",fullName );
    return fullName;
    }

// -----------------------------------------------------------------------------
// CCchNoteHandler::CchCenRep
// -----------------------------------------------------------------------------
//
CRepository* CCchNoteHandler::CchCenRep()
    {
    return iCchCenRep;
    }
//  End of File
