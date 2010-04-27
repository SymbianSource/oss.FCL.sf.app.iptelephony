/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Actual notifier implementation
*
*/


#include <eikenv.h>
#include <bautils.h>
#include <eikclbd.h>
#include <eikimage.h>
#include <cchuinotif.rsg>
#include <StringLoader.h>
#include <aknnotewrappers.h>
#include <cenrepnotifyhandler.h>
#include <ctsydomainpskeys.h>
#include <crcseprofileregistry.h>
#include <AknNotiferAppServerApplication.h>  // Application Key enable/disable

#include "cchuilogger.h"
#include "cchuicommon.hrh"
#include "cchuinotifierimpl.h"
#include "cchuipluginbrandinghandler.h"
#include "cchuinotifconnectionhandler.h"
#include "cchuicallstatelistener.h"


// ======== MEMBER FUNCTIONS ========

CCCHUiNotifierImpl::CCCHUiNotifierImpl()
    {
    }

CCCHUiNotifierImpl* CCCHUiNotifierImpl::NewL()
    {
    CCCHUiNotifierImpl* self = new (ELeave) CCCHUiNotifierImpl();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

void CCCHUiNotifierImpl::ConstructL()
    {
    CCCHUiNotifierBase::ConstructL();
    iCallStateListener = CCchUiCallStateListener::NewL( *this );
    }

CCCHUiNotifierImpl::~CCCHUiNotifierImpl()
    {
    CCHUIDEBUG( "CCCHUiNotifierImpl::~CCCHUiNotifierImpl - IN" );	
    
    if ( iAppKeyBlocked )
        {
        // Remove application key blocking
        (void) ((CAknNotifierAppServerAppUi*)
            iEikEnv->EikAppUi())->SuppressAppSwitching(EFalse);    
        iAppKeyBlocked = EFalse;
        }
    
    CCCHUiNotifierBase::Cancel();
    iListQueryDialog = NULL;
    delete iBrandingHandler;
    delete iCallStateListener;
    
    CCHUIDEBUG( "CCCHUiNotifierImpl::~CCCHUiNotifierImpl - OUT" );
    }

// ---------------------------------------------------------------------------
// RunL
// From CActive.
// ---------------------------------------------------------------------------
//
void CCCHUiNotifierImpl::RunL(  )
    {
    CCHUIDEBUG( "CCCHUiNotifierImpl::RunL - IN" );

    switch( iDialogMode )
        {
        // Only ECchUiDialogTypeNoConnectionAvailable is launched via RunL
        // currently.
        case MCchUiObserver::ECchUiDialogTypeNoConnectionAvailable:
            {
            CCHUIDEBUG( "CCCHUiNotifierImpl::RunL - ECchUiDialogTypeNoConnectionAvailable" );               
            ShowNoConnectionsQueryL( iReplySlot, iMessage, iServiceId, EFalse );
            }
            break;    

        default:
            {
            CCHUIDEBUG( "CCCHUiNotifierImpl::RunL - Does nothing" );
            }
            break;    
        }    
        
    CCHUIDEBUG( "   CCCHUiNotifierImpl::RunL - OUT" );
    }


// ---------------------------------------------------------------------------
// From class MEikSrvNotifierBase2.
// Cancelling method.
// ---------------------------------------------------------------------------
//
void CCCHUiNotifierImpl::Cancel()
    {
    CCHUIDEBUG( "CCCHUiNotifierImpl::Cancel - IN" );
    if ( iListQueryDialog )
        {
        delete iListQueryDialog;
        iListQueryDialog = NULL;
        CCCHUiNotifierBase::Cancel();
        }
    
    CCHUIDEBUG( "CCCHUiNotifierImpl::Cancel - OUT" );
    }


// ---------------------------------------------------------------------------
// Shows username & password query.
// ---------------------------------------------------------------------------
//
void CCCHUiNotifierImpl::ShowUsernamePasswordQueryL(
    TInt aReplySlot, 
    const RMessagePtr2& aMessage, 
    TUint aServiceId,
    const TDesC& aUserName )
    { 
    CCHUIDEBUG( "CCCHUiNotifierImpl::ShowUsernamePasswordQueryL - IN" );
    
    iMessage = aMessage;
    iReplySlot = aReplySlot;
    
    HBufC* userName = HBufC::NewLC( KUserNameLength );
    HBufC* passWord = HBufC::NewLC( KPasswordLength );

    // Set current username
    userName->Des().Copy( aUserName );
    
    TPtr ptrUserName( userName->Des() );
    TPtr ptrPassWord( passWord->Des() );

    CCHUIDEBUG( "ShowUsernamePasswordQueryL - create query");	
    
    CAknMultiLineDataQueryDialog* dlg = CAknMultiLineDataQueryDialog::NewL( 
        ptrUserName, ptrPassWord );
     
    // Block application key while showing query
    (void) ((CAknNotifierAppServerAppUi*)
        iEikEnv->EikAppUi())->SuppressAppSwitching(ETrue); 
    iAppKeyBlocked = ETrue;
    
    if ( dlg->ExecuteLD( R_CCHUINOTIF_USERNAME_PWD_DIALOG ) )
        {	
        iResultParams.iDialogMode = 
            MCchUiObserver::ECchUiDialogTypeAuthenticationFailed;
        iResultParams.iServiceId = aServiceId;
        iResultParams.iUsername = *userName;
        iResultParams.iPwd = *passWord;
        iMessage.WriteL( iReplySlot, 
                         TPckgBuf<TCCHUiNotifierParams>( iResultParams ) );
        CompleteMessage(KErrNone);                              
        }
    else
        {
        CCHUIDEBUG( "ShowUsernamePasswordQueryL - cancelled" );	
        CompleteMessage( KErrCancel );
        }
    
    // Remove application key blocking
    (void) ((CAknNotifierAppServerAppUi*)
        iEikEnv->EikAppUi())->SuppressAppSwitching(EFalse);    
    iAppKeyBlocked = EFalse;
    
    CleanupStack::PopAndDestroy( passWord );
    CleanupStack::PopAndDestroy( userName );
    
    CCHUIDEBUG( "CCCHUiNotifierImpl::ShowUsernamePasswordQueryL - OUT" );
    }

// ---------------------------------------------------------------------------
// Shows no connections query.
// ---------------------------------------------------------------------------
//
void CCCHUiNotifierImpl::ShowNoConnectionsQueryL(
    TInt aReplySlot, 
    const RMessagePtr2& aMessage, 
    TUint aServiceId,
    TBool aNoConnectionsDefined )
    { 
    CCHUIDEBUG( "CCCHUiNotifierImpl::ShowNoConnectionsQueryL - IN" );	
    
    iServiceId = aServiceId;
    iMessage = aMessage;
    iReplySlot = aReplySlot;

    CDesCArray* arrayforDialog = new (ELeave) CDesCArrayFlat( 3 );
    CleanupStack::PushL( arrayforDialog );  
    
    CCHUIDEBUG( "ShowNoConnectionsQueryL - form content" );	

    RArray<TCchUiOperationCommand> commandArray;
    CleanupClosePushL( commandArray );

    TInt result = KErrNotFound;
    MCchUiObserver::TCchUiDialogType currentType = 
        MCchUiObserver::ECchUiDialogTypeNotSet;
    HBufC* string = NULL;
    if ( aNoConnectionsDefined )
        {
        string = StringLoader::LoadLC( 
            R_QTN_CCHUINOTIF_NO_CONNECTIONS_DEFINED_HEADING );
        FillNoConnectionsDefinedListboxL( *arrayforDialog, commandArray );                        
        currentType = MCchUiObserver::ECchUiDialogTypeNoConnectionDefined;            
        }
    else
        {
        string = StringLoader::LoadLC( 
            R_QTN_CCHUINOTIF_NO_CONNECTIONS_HEADING );
        FillNoConnectionsAvailableListboxL( 
                *arrayforDialog, commandArray, aServiceId );
        currentType = MCchUiObserver::ECchUiDialogTypeNoConnectionAvailable;
        }

    iListQueryDialog = new (ELeave) CAknListQueryDialog( &result );
    iListQueryDialog->PrepareLC( R_CCHUINOTIF_NO_CONNECTIONS_AVAILABLE );

    iListQueryDialog->MessageBox()->SetMessageTextL( string );

    CCHUIDEBUG( "ShowNoConnectionsQueryL - form heading" );	
    
    // Set heading ie. service name
    HBufC* serviceName = HBufC::NewLC( KServiceNameLength );
    TPtr serviceNamePtr( serviceName->Des() );
    GetServiceNameL( aServiceId, serviceNamePtr );    
    CCHUIDEBUG( "ShowNoConnectionsQueryL - set heading" );  
    iListQueryDialog->QueryHeading()->SetTextL( serviceNamePtr );
    CCHUIDEBUG( "ShowNoConnectionsQueryL - heading set ok" );   
    CleanupStack::PopAndDestroy(); // serviceName
    CCHUIDEBUG( "ShowNoConnectionsQueryL - proceed to list items" );    
      
    CCHUIDEBUG( "ShowNoConnectionsQueryL - set item array" );   
    iListQueryDialog->SetItemTextArray( arrayforDialog );
    iListQueryDialog->SetOwnershipType( ELbmDoesNotOwnItemArray );
      
    CCHUIDEBUG( "ShowNoConnectionsQueryL - retrieve branded icon" );    
    CFbsBitmap* myBitmap = NULL;
    CFbsBitmap* myMask = NULL;
    TRAPD( err, BrandingHandlerL().RetrieveServiceIconL( 
        aServiceId, myBitmap, myMask ) );
    CCHUIDEBUG2( "ShowNoConnectionsQueryL - brand err: %d", err );  
      
    if ( !err && iListQueryDialog->QueryHeading() )
        {
        CleanupStack::PushL( myBitmap );
        CleanupStack::PushL( myMask );
        CEikImage* image = new (ELeave) CEikImage();
        image->SetPicture( myBitmap, myMask );     
        CleanupStack::Pop( myMask );
        CleanupStack::Pop( myBitmap );     
        CleanupStack::PushL( image );    
        iListQueryDialog->QueryHeading()->SetHeaderImageL( image );
        iListQueryDialog->QueryHeading()->ActivateL();
        CleanupStack::PopAndDestroy( image );
        }
    else if ( !err )
        {
        delete myMask;
        delete myBitmap;
          }
    else
        {
        // Error occurred in RetrieveServiceIconL. Nothing to do.
        }
         
    // Block application key while showing query
    (void) ((CAknNotifierAppServerAppUi*)
        iEikEnv->EikAppUi())->SuppressAppSwitching(ETrue); 
    iAppKeyBlocked = ETrue;
    
    CCHUIDEBUG( "ShowNoConnectionsQueryL - run dialog" );   
    iListQueryDialog->RunLD();
    CCHUIDEBUG( "ShowNoConnectionsQueryL - run dialog done" );

    // write result, result now holds the item number
    if ( KErrNotFound != result )
        {
        iResultParams.iDialogMode = currentType;
        iResultParams.iServiceId = aServiceId;
        iResultParams.iOperationCommand = commandArray[ result ];        
          
        if ( ECchUiCommandCopyGprs == commandArray[ result ] )
            {
            ShowGprsSelectionL( aServiceId );
            }
        CCHUIDEBUG( "ShowNoConnectionsQueryL - write and complete" );   
        iMessage.WriteL( iReplySlot, 
            TPckgBuf<TCCHUiNotifierParams>( iResultParams ) );
        CompleteMessage( KErrNone );    
        }
    else
        {
        CCHUIDEBUG( "ShowNoConnectionsQueryL - complete with cancel" ); 
        CompleteMessage( KErrCancel );
        }
    
    // Remove application key blocking
    (void) ((CAknNotifierAppServerAppUi*)
        iEikEnv->EikAppUi())->SuppressAppSwitching(EFalse);    
    iAppKeyBlocked = EFalse;
    
    CleanupStack::PopAndDestroy( string );
    CleanupStack::PopAndDestroy( &commandArray );
    CleanupStack::PopAndDestroy( arrayforDialog );    
      
    CCHUIDEBUG( "CCCHUiNotifierImpl::ShowNoConnectionsQueryL - OUT" );   
    }

// ---------------------------------------------------------------------------
// Shows change connection query.
// ---------------------------------------------------------------------------
//
void CCCHUiNotifierImpl::ShowChangeConnectionQueryL(
    TInt aReplySlot,
    const RMessagePtr2& aMessage,
    TUint aServiceId, 
    TInt aIapId )
    { 
    CCHUIDEBUG( "CCCHUiNotifierImpl::ShowChangeConnectionQueryL - IN" );   
    
    iMessage = aMessage;
    iReplySlot = aReplySlot;

    CDesCArray* arrayforDialog = new (ELeave) CDesCArrayFlat( 3 );
    CleanupStack::PushL( arrayforDialog );
    
    CCHUIDEBUG( "ShowChangeConnectionQueryL - form content" ); 

    RArray<TCchUiOperationCommand> commandArray;
    CleanupClosePushL( commandArray );

    TInt result = KErrNotFound;
    MCchUiObserver::TCchUiDialogType type = 
        MCchUiObserver::ECchUiDialogTypeChangeConnection;
    HBufC* string = NULL;
       
    RBuf iapName;
    CleanupClosePushL( iapName );
    
    // If we have current connection, show it in query heading
    if ( aIapId )
        {
        CurrentConnectionNameL( 
            aServiceId, 
            aIapId,
            iapName );
        
        string = StringLoader::LoadLC( 
            R_QTN_CCHUINOTIF_CHANGE_CURRENT_CONNECTION_HEADING, iapName ); 
        
        FillChangeCurrentConnectionListboxL( *arrayforDialog, commandArray );
        }
    else // otherwise show in heading that we have no current connection
        {
        string = StringLoader::LoadLC( 
            R_QTN_CCHUINOTIF_CHANGE_CONNECTION_HEADING );
        
        FillChangeConnectionListboxL( *arrayforDialog, commandArray );
        }
    
    iListQueryDialog = new (ELeave) CAknListQueryDialog( &result );
    iListQueryDialog->PrepareLC( R_CCHUINOTIF_CHANGE_CONNECTION_QUERY );

    iListQueryDialog->MessageBox()->SetMessageTextL( string );
    
    CCHUIDEBUG( "ShowChangeConnectionQueryL - form heading" ); 
    // Set heading ie. service name
    HBufC* serviceName = HBufC::NewLC( KServiceNameLength );
    TPtr serviceNamePtr( serviceName->Des() );
    GetServiceNameL( aServiceId, serviceNamePtr );    
    CCHUIDEBUG( "ShowChangeConnectionQueryL - set heading" );  
    iListQueryDialog->QueryHeading()->SetTextL( serviceNamePtr );
    CCHUIDEBUG( "ShowChangeConnectionQueryL - heading set ok" );   
    CleanupStack::PopAndDestroy(); // serviceName
    CCHUIDEBUG( "ShowChangeConnectionQueryL - proceed to list items" );    
    
    CCHUIDEBUG( "ShowChangeConnectionQueryL - set item array" );   
    iListQueryDialog->SetItemTextArray( arrayforDialog );
    iListQueryDialog->SetOwnershipType( ELbmDoesNotOwnItemArray );

    CCHUIDEBUG( "ShowChangeConnectionQueryL - retrieve branded icon" );    

    CFbsBitmap* myBitmap = NULL;
    CFbsBitmap* myMask = NULL;
    TRAPD( err, BrandingHandlerL().RetrieveServiceIconL( 
        aServiceId, myBitmap, myMask ) );

    CCHUIDEBUG2( "ShowChangeConnectionQueryL - brand err: %d", err );
    
    if ( !err && iListQueryDialog->QueryHeading() )
        {
        CleanupStack::PushL( myBitmap );
        CleanupStack::PushL( myMask );
        CEikImage* image = new (ELeave) CEikImage();
        image->SetPicture( myBitmap, myMask );      
        CleanupStack::Pop( myMask );
        CleanupStack::Pop( myBitmap );    
        CleanupStack::PushL( image );               
        iListQueryDialog->QueryHeading()->SetHeaderImageL( image );
        iListQueryDialog->QueryHeading()->ActivateL();
        CleanupStack::PopAndDestroy( image );
        }
    else if ( !err )
        {
        delete myMask;
        delete myBitmap;
        }
    else
        {
        // Error occurred in RetrieveServiceIconL. Nothing to do.
        }
   
    // Block application key while showing query
    (void) ((CAknNotifierAppServerAppUi*)
        iEikEnv->EikAppUi())->SuppressAppSwitching(ETrue); 
    iAppKeyBlocked = ETrue;
    
    CCHUIDEBUG( "ShowChangeConnectionQueryL - run dialog" );
    
    // List query dialog is deleted via RunLD except if there is
    // incoming call at the same time the dialog is shown on screen.
    // In case of incoming call CallStateChanged() method handles
    // the deletion
    iListQueryDialog->RunLD();
    // coverity[check_after_deref]
    if( iListQueryDialog )
        {
        // write result, result now holds the item number
        if ( KErrNotFound != result )
            {
            iResultParams.iDialogMode = type;
            iResultParams.iServiceId = aServiceId;
            iResultParams.iOperationCommand = commandArray[ result ];    
            
            if ( ECchUiCommandCopyGprs == commandArray[ result ] )
                {
                ShowGprsSelectionL( aServiceId );
                }
            CCHUIDEBUG( "ShowChangeConnectionQueryL - write and complete" );  
            iMessage.WriteL( iReplySlot, 
                TPckgBuf<TCCHUiNotifierParams>( iResultParams ) );
            CompleteMessage( KErrNone );    
            }
        else
            {
            CCHUIDEBUG( "ShowChangeConnectionQueryL - complete with cancel" ); 
            CompleteMessage( KErrCancel );
            }
        
        iListQueryDialog = NULL;
        }
    
    // Remove application key blocking
    (void) ((CAknNotifierAppServerAppUi*)
        iEikEnv->EikAppUi())->SuppressAppSwitching(EFalse);    
    iAppKeyBlocked = EFalse;
    
    CleanupStack::PopAndDestroy( string );    
    CleanupStack::PopAndDestroy( &iapName );   
    CleanupStack::PopAndDestroy( &commandArray );
    CleanupStack::PopAndDestroy( arrayforDialog );
    
    CCHUIDEBUG( "CCCHUiNotifierImpl::ShowChangeConnectionQueryL - OUT" );   
    }
	
// ---------------------------------------------------------------------------
// Shows defective settings error note.
// ---------------------------------------------------------------------------
//
void CCCHUiNotifierImpl::ShowDefectiveSettingsL( 
    TInt aReplySlot, 
	const RMessagePtr2& aMessage, 
	TUint aServiceId )
	{ 
	CCHUIDEBUG( "CCCHUiNotifierImpl::ShowDefectiveSettingsL - IN" );

    iMessage = aMessage;
    iReplySlot = aReplySlot;
    
    // Get service name
    HBufC* serviceName = HBufC::NewLC( KServiceNameLength );
    TPtr serviceNamePtr( serviceName->Des() );
    GetServiceNameL( aServiceId, serviceNamePtr ); 
    
    // Show info note about defective settings
    HBufC* defectiveText = StringLoader::LoadLC( 
        R_QTN_SERVTAB_CONN_ESTABLSH_FAILED_PERMANENTLY_NOTE_TEXT, serviceNamePtr );
    CAknInformationNote* note = 
        new( ELeave ) CAknInformationNote( ETrue );
    note->SetTimeout(CAknNoteDialog::ELongTimeout);
    note->ExecuteLD( *defectiveText );   
    CleanupStack::PopAndDestroy( defectiveText );     
    CleanupStack::PopAndDestroy( serviceName );
    
    iResultParams.iDialogMode = 
        MCchUiObserver::ECchUiDialogTypeDefectiveSettings;
    iResultParams.iServiceId = aServiceId;
    iMessage.WriteL( iReplySlot, 
                     TPckgBuf<TCCHUiNotifierParams>( iResultParams ) );
    CompleteMessage(KErrNone);                              

    CCHUIDEBUG( "CCCHUiNotifierImpl::ShowDefectiveSettingsL - OUT" );	
	}    	
	
// ---------------------------------------------------------------------------
// Shows error in connection error note.
// ---------------------------------------------------------------------------
//
void CCCHUiNotifierImpl::ShowErrorInConnectionNoteL( 
    TInt aReplySlot, 
    const RMessagePtr2& aMessage, 
    TUint aServiceId )
    { 
    CCHUIDEBUG( "CCCHUiNotifierImpl::ShowErrorInConnectionL - IN" ); 

    iMessage = aMessage;
    iReplySlot = aReplySlot;
    
    // Show info note about error in connection 
    //(access point not suitable for internet calls)
    HBufC* noteText = StringLoader::LoadLC( 
        R_QTN_VOIP_ERROR_IN_CONNECTION_NOTE_TEXT );
    CAknInformationNote* note = 
        new( ELeave ) CAknInformationNote( ETrue );
    note->SetTimeout(CAknNoteDialog::ELongTimeout);
    note->ExecuteLD( *noteText );   
    CleanupStack::PopAndDestroy( noteText );
    
    iResultParams.iDialogMode = 
        MCchUiObserver::ECchUiDialogTypeErrorInConnection;
    iResultParams.iServiceId = aServiceId;
    iMessage.WriteL( iReplySlot, 
                     TPckgBuf<TCCHUiNotifierParams>( iResultParams ) );
    CompleteMessage(KErrNone);                              

    CCHUIDEBUG( "CCCHUiNotifierImpl::ShowErrorInConnectionL - OUT" ); 
    }  

// ---------------------------------------------------------------------------
// Shows invalid username/password note.
// ---------------------------------------------------------------------------
//
void CCCHUiNotifierImpl::ShowInvalidUsernamePasswordNoteL( 
    TInt aReplySlot, 
    const RMessagePtr2& aMessage, 
    TUint aServiceId )
    { 
    CCHUIDEBUG( "CCCHUiNotifierImpl::ShowInvalidUsernamePasswordNoteL - IN" );

    iMessage = aMessage;
    iReplySlot = aReplySlot;
        
    // Show info note about invalid username password
    HBufC* textForNote = StringLoader::LoadLC( 
        R_QTN_VOIP_WRONG_SIP_AUTH_NOTE_TEXT );
    CAknInformationNote* note = 
        new( ELeave ) CAknInformationNote( ETrue );
    note->SetTimeout(CAknNoteDialog::ELongTimeout);
    note->ExecuteLD( *textForNote );   
    CleanupStack::PopAndDestroy( textForNote );
        
    iResultParams.iDialogMode = 
        MCchUiObserver::ECchUiDialogTypeUsernamePasswordFailed;
    iResultParams.iServiceId = aServiceId;
    iMessage.WriteL( iReplySlot, 
                     TPckgBuf<TCCHUiNotifierParams>( iResultParams ) );
    CompleteMessage(KErrNone);                              

    CCHUIDEBUG( 
        "CCCHUiNotifierImpl::ShowInvalidUsernamePasswordNoteL - OUT" );
    } 

// ---------------------------------------------------------------------------
// Shows confirmation note to change connection.
// ---------------------------------------------------------------------------
//
void CCCHUiNotifierImpl::ShowConfirmChangeConnectionL( 
    TInt aReplySlot, 
    const RMessagePtr2& aMessage,
    TUint aServiceId,
    TInt aIapId )
    { 
    CCHUIDEBUG( "CCCHUiNotifierImpl::ShowConfirmChangeConnectionL - IN" ); 
    
    CCHUIDEBUG2( 
        "ShowConfirmChangeConnectionL - aServiceId: %d", aServiceId );
    CCHUIDEBUG2( 
        "ShowConfirmChangeConnectionL - aIapId: %d", aIapId );

    iMessage = aMessage;
    iReplySlot = aReplySlot;
        
    // Show confirmation note to change connection
    RBuf iapName;
    CleanupClosePushL( iapName );
        
    CurrentConnectionNameL( 
        aServiceId, 
        aIapId,
        iapName );
    
    HBufC* textForQuery = NULL;
    CAknQueryDialog* query = 
               new( ELeave ) CAknQueryDialog( CAknQueryDialog::ENoTone );
    
    CleanupStack::PushL( query );
    
    TBool askToRemoveOldConnection( EFalse );
     
    // Check if current connection is used by any other service
    if ( aIapId && !ConnectionUsedByOthersL( aServiceId, aIapId ) )
        {
        // connection not used by others -> ask to remove old connection
        askToRemoveOldConnection = ETrue;
        }
    
    if ( askToRemoveOldConnection )
        {
        CCHUIDEBUG( 
            "ShowConfirmChangeConnectionL - ask to remove connection" );
        
        query->PrepareLC( 
            R_CCHUINOTIF_REMOVE_AND_CHANGE_CONNECTION_CONFIRMATION_QUERY );
        
        textForQuery = StringLoader::LoadLC( 
            R_QTN_SERVTAB_AP_VOIP_NOT_SUPPORTED_RECONNECT_QUERY_TEXT, 
            iapName );
        }
    else
        {
        CCHUIDEBUG( 
            "ShowConfirmChangeConnectionL - ask to change connection" );
        
        query->PrepareLC( R_CCHUINOTIF_CHANGE_CONNECTION_CONFIRMATION_QUERY );
        
        textForQuery = StringLoader::LoadLC( 
            R_QTN_SERVTAB_AP_VOIP_NOT_SUPPORTED_QUERY_TEXT, iapName );  
        }
    
    CCHUIDEBUG( "ShowConfirmChangeConnectionL - set prompt" );
    
    query->SetPromptL( *textForQuery );    
    CleanupStack::PopAndDestroy( textForQuery );
    CleanupStack::Pop( query );
       
    // Block application key while showing query
    (void) ((CAknNotifierAppServerAppUi*)
        iEikEnv->EikAppUi())->SuppressAppSwitching(ETrue); 
    iAppKeyBlocked = ETrue;
    
    // Run query
    if( query->RunLD() )
        {
        iResultParams.iDialogMode = 
            MCchUiObserver::ECchUiDialogTypeConfirmChangeConnection;
        iResultParams.iServiceId = aServiceId;
       
        if ( askToRemoveOldConnection )
            {
            CCHUIDEBUG( 
                "ShowConfirmChangeConnectionL - set remove old connection");
            
            iResultParams.iRemoveOldConnection = ETrue;
            iResultParams.iCurrentConnectionIapId = aIapId;
            }     
        else
            {
            iResultParams.iRemoveOldConnection = EFalse;
            }
            
        iMessage.WriteL( iReplySlot, 
                         TPckgBuf<TCCHUiNotifierParams>( iResultParams ) );
        
        CCHUIDEBUG( "ShowConfirmChangeConnectionL - complete message" ); 
        
        CompleteMessage( KErrNone );                  
        }
    else
        {
        CCHUIDEBUG( "ShowConfirmChangeConnectionL - canceled" );
        
        CompleteMessage( KErrCancel );
        }
    
    // Remove application key blocking
    (void) ((CAknNotifierAppServerAppUi*)
        iEikEnv->EikAppUi())->SuppressAppSwitching(EFalse);    
    iAppKeyBlocked = EFalse;
    
    CleanupStack::PopAndDestroy( &iapName );
    
    CCHUIDEBUG( "CCCHUiNotifierImpl::ShowConfirmChangeConnectionL - OUT" );
    } 

// ---------------------------------------------------------------------------
// Shows gprs iap selection.
// ---------------------------------------------------------------------------
//
void CCCHUiNotifierImpl::ShowGprsSelectionL( TUint aServiceId )
	{ 
	CCHUIDEBUG( "CCCHUiNotifierImpl::ShowGprsSelectionL - IN" );	

    RArray<TUint32> iapIds;
    CleanupClosePushL( iapIds );

    CDesCArray* arrayforDialog = new (ELeave) CDesCArrayFlat( 3 );
    CleanupStack::PushL( arrayforDialog );            
    CCHUIDEBUG( "ShowGprsSelectionL - form content" );	

    TInt result = KErrNotFound;
    HBufC* string = StringLoader::LoadLC( 
        R_QTN_CCHUINOTIF_SELECT_GPRS_ACCESS_POINT_HEADING );

    CAknListQueryDialog* dialog = new (ELeave) CAknListQueryDialog( &result );
    dialog->PrepareLC( R_CCHUINOTIF_SELECT_GPRS_DIALOG );

    CCHUIDEBUG( "ShowGprsSelectionL - form heading" );	
	dialog->QueryHeading()->SetTextL( *string );
	    
    CCHUIDEBUG( "ShowGprsSelectionL - set item array" );	
    
    TUint sourceSnap = KErrNone;
    // Fill array here
    CCchUiNotifConnectionHandler* connHandler = 
        CCchUiNotifConnectionHandler::NewLC();
    sourceSnap = connHandler->GetGprsAccessPointsL( *arrayforDialog, iapIds ); 
    CleanupStack::PopAndDestroy( connHandler );    
    
    CCHUIDEBUG( "ShowGprsSelectionL - set item array" );	
    dialog->SetItemTextArray( arrayforDialog );
    dialog->SetOwnershipType( ELbmDoesNotOwnItemArray );
      
    // Block application key while showing query
    (void) ((CAknNotifierAppServerAppUi*)
        iEikEnv->EikAppUi())->SuppressAppSwitching(ETrue); 
    iAppKeyBlocked = ETrue;
    
    CCHUIDEBUG( "ShowGprsSelectionL - run dialog" );	
    dialog->RunLD();

    // write result, result now holds the item number
    if ( KErrNotFound != result )
        {
        iResultParams.iServiceId = aServiceId;
        iResultParams.iGprsSourceSnap = sourceSnap;
        iResultParams.iGprsIapId = iapIds[ result ];
        }
    else
        {
        User::Leave( KErrCancel );
        }
    
    // Remove application key blocking
    (void) ((CAknNotifierAppServerAppUi*)
        iEikEnv->EikAppUi())->SuppressAppSwitching(EFalse);    
    iAppKeyBlocked = EFalse;

    CleanupStack::PopAndDestroy( string );
    CleanupStack::PopAndDestroy( arrayforDialog );     
    CleanupStack::PopAndDestroy( &iapIds );

    CCHUIDEBUG( "CCCHUiNotifierImpl::ShowGprsSelectionL - OUT" );
	}   	

// ---------------------------------------------------------------------------
// Returns handle to branding handler.
// ---------------------------------------------------------------------------
//
CCchUiPluginBrandingHandler& CCCHUiNotifierImpl::BrandingHandlerL()
    {
    if ( !iBrandingHandler )
        {
        iBrandingHandler = 
            CCchUiPluginBrandingHandler::NewL( *iSettings );    
        }
    return *iBrandingHandler;
    }

// ---------------------------------------------------------------------------
// Fills list items and commands for no connections available dialog
// ---------------------------------------------------------------------------
//
void CCCHUiNotifierImpl::FillNoConnectionsAvailableListboxL( 
	CDesCArray& aListItems, 
	RArray<TCchUiOperationCommand>& aCommandArray,
	TUint aServiceId )
	{
	CCHUIDEBUG( 
	    "CCCHUiNotifierImpl::FillNoConnectionsAvailableListboxL - IN" );	

	HBufC* connectWhenAv = StringLoader::LoadLC( 
        R_QTN_CCHUINOTIF_CONNECT_WHEN_AV_TEXT );
	HBufC* searchWlan = StringLoader::LoadLC( 
		R_QTN_CCHUINOTIF_SEARCH_WLAN_NO_COVERAGE_TEXT );	
	HBufC* useGprs = StringLoader::LoadLC( 
		R_QTN_CCHUINOTIF_USE_GPRS_NO_COVERAGE_TEXT );	
	    
	// Item is available only for SIP protocol services.
	// Check if service supports SIP/VoIP.
	TBool sipVoip( EFalse );
	TRAPD( err, sipVoip = IsSIPVoIPL( aServiceId ) );
	
	if ( !err && sipVoip )
	    {
	    aListItems.AppendL( *connectWhenAv );
	    aCommandArray.AppendL( ECchUiCommandConnectWhenAvailable );
	    }
    
    aListItems.AppendL( *searchWlan );
    aCommandArray.AppendL( ECchUiCommandSearchWlan );
    
    if ( !IsPhoneOfflineL() && IsGprsIapsAvailableL() 
            && IsVoIPOverWCDMAAllowedL() )
        {
        aListItems.AppendL( *useGprs );	
        aCommandArray.AppendL( ECchUiCommandCopyGprs );    
        }

    CleanupStack::PopAndDestroy( useGprs );	
    CleanupStack::PopAndDestroy( searchWlan );	
    CleanupStack::PopAndDestroy( connectWhenAv );	
    
    CCHUIDEBUG( 
        "CCCHUiNotifierImpl::FillNoConnectionsAvailableListboxL - OUT" ); 
	}

// ---------------------------------------------------------------------------
// Fills list items and commands for no connections defined dialog.
// ---------------------------------------------------------------------------
//
void CCCHUiNotifierImpl::FillNoConnectionsDefinedListboxL( 
	CDesCArray& aListItems, RArray<TCchUiOperationCommand>& aCommandArray )
	{
	CCHUIDEBUG( 
	    "CCCHUiNotifierImpl::FillNoConnectionsDefinedListboxL - IN" );	

	HBufC* searchWlan = StringLoader::LoadLC( 
		R_QTN_CCHUINOTIF_SEARCH_WLAN_NOT_DEF_TEXT );
	
	HBufC* useGprs = StringLoader::LoadLC( 
		R_QTN_CCHUINOTIF_USE_GPRS_NOT_DEF_TEXT );	
	
    aListItems.AppendL( *searchWlan );
    aCommandArray.AppendL( ECchUiCommandSearchWlan );

    if ( !IsPhoneOfflineL() && IsGprsIapsAvailableL() 
            && IsVoIPOverWCDMAAllowedL() )
        {
        aListItems.AppendL( *useGprs );	
        aCommandArray.AppendL( ECchUiCommandCopyGprs );    
        }

    CleanupStack::PopAndDestroy( useGprs );	
    CleanupStack::PopAndDestroy( searchWlan );	
    
    CCHUIDEBUG( 
        "CCCHUiNotifierImpl::FillNoConnectionsDefinedListboxL - OUT" );  	    
	}
	
// ---------------------------------------------------------------------------
// Fills list items and commands for no connections defined dialog
// ---------------------------------------------------------------------------
//
void CCCHUiNotifierImpl::FillNoConnectionsFoundListboxL( 
	CDesCArray& aListItems, RArray<TCchUiOperationCommand>& aCommandArray )
	{
	CCHUIDEBUG( "CCCHUiNotifierImpl::FillNoConnectionsFoundListboxL - IN" );	

	HBufC* searchWlan = StringLoader::LoadLC( 
		R_QTN_CCHUINOTIF_SEARCH_WLAN_NOT_FOUND_TEXT );	
	
	HBufC* useGprs = StringLoader::LoadLC( 
		R_QTN_CCHUINOTIF_USE_GPRS_NOT_FOUND_TEXT );	
	
    aListItems.AppendL( *searchWlan );
    aCommandArray.AppendL( ECchUiCommandSearchWlan );

    if ( !IsPhoneOfflineL() && IsGprsIapsAvailableL() 
            && IsVoIPOverWCDMAAllowedL() )
        {
        aListItems.AppendL( *useGprs );	
        aCommandArray.AppendL( ECchUiCommandCopyGprs );    
        }

    CleanupStack::PopAndDestroy( useGprs );	
    CleanupStack::PopAndDestroy( searchWlan );	
    
    CCHUIDEBUG( "CCCHUiNotifierImpl::FillNoConnectionsFoundListboxL - OUT" );  
	}	

// ---------------------------------------------------------------------------
// Fills list items and commands for change current connection dialog
// ---------------------------------------------------------------------------
//
void CCCHUiNotifierImpl::FillChangeCurrentConnectionListboxL( 
    CDesCArray& aListItems, RArray<TCchUiOperationCommand>& aCommandArray )
    {
    CCHUIDEBUG( 
        "CCCHUiNotifierImpl::FillChangeCurrentConnectionListboxL - IN" ); 

    HBufC* searchWlan = StringLoader::LoadLC( 
        R_QTN_CCHUINOTIF_SEARCH_WLAN_CHANGE_CURRENT_CONNECTION_TEXT );  
        
    HBufC* useGprs = StringLoader::LoadLC( 
        R_QTN_CCHUINOTIF_USE_GPRS_CHANGE_CURRENT_CONNECTION_TEXT ); 
        
    aListItems.AppendL( *searchWlan );
    aCommandArray.AppendL( ECchUiCommandSearchWlan );

    if ( !IsPhoneOfflineL() && IsGprsIapsAvailableL() 
            && IsVoIPOverWCDMAAllowedL() )
        {
        aListItems.AppendL( *useGprs ); 
        aCommandArray.AppendL( ECchUiCommandCopyGprs ); 
        }

    CleanupStack::PopAndDestroy( useGprs ); 
    CleanupStack::PopAndDestroy( searchWlan );  
    
    CCHUIDEBUG( 
            "CCCHUiNotifierImpl::FillChangeCurrentConnectionListboxL - OUT" );
    }


// ---------------------------------------------------------------------------
// Fills list items and commands for change connection dialog.
// ---------------------------------------------------------------------------
//
void CCCHUiNotifierImpl::FillChangeConnectionListboxL( 
    CDesCArray& aListItems, RArray<TCchUiOperationCommand>& aCommandArray )
    {
    CCHUIDEBUG( "CCCHUiNotifierImpl::FillChangeConnectionListboxL - IN" ); 

    HBufC* searchWlan = StringLoader::LoadLC( 
        R_QTN_CCHUINOTIF_SEARCH_WLAN_CHANGE_CONNECTION_TEXT );  
           
    HBufC* useGprs = StringLoader::LoadLC( 
        R_QTN_CCHUINOTIF_USE_GPRS_CHANGE_CONNECTION_TEXT ); 
           
    aListItems.AppendL( *searchWlan );
    aCommandArray.AppendL( ECchUiCommandSearchWlan );

    if ( !IsPhoneOfflineL() && IsGprsIapsAvailableL() 
            && IsVoIPOverWCDMAAllowedL() )
        {
        aListItems.AppendL( *useGprs ); 
        aCommandArray.AppendL( ECchUiCommandCopyGprs ); 
        }

    CleanupStack::PopAndDestroy( useGprs ); 
    CleanupStack::PopAndDestroy( searchWlan );  
       
    CCHUIDEBUG( "CCCHUiNotifierImpl::FillChangeConnectionListboxL - OUT" );
    }

// ---------------------------------------------------------------------------
// Check if VoIP over WCDMA is allowed
// ---------------------------------------------------------------------------
//
TBool CCCHUiNotifierImpl::IsVoIPOverWCDMAAllowedL()
    {
    CCHUIDEBUG( "CCCHUiNotifierImpl::IsVoIPOverWCDMAAllowed - IN" );
    
    CRCSEProfileRegistry* cRCSEProfileRegistry;
    cRCSEProfileRegistry = CRCSEProfileRegistry::NewLC();
    TBool ret = EFalse;
    RPointerArray<CRCSEProfileEntry> entries;
    cRCSEProfileRegistry->FindByServiceIdL( iServiceId, entries );
    if( entries.Count() )
        {
        ret = CRCSEProfileEntry::EOn == entries[ 0 ]->iAllowVoIPoverWCDMA;
        }
    
    entries.ResetAndDestroy();
    entries.Close();
    CleanupStack::PopAndDestroy(); //cRCSEProfileRegistry
    
    return ret;
    }

// ---------------------------------------------------------------------------
// From class CCCHUiNotifierBase.
// ---------------------------------------------------------------------------
//
CCCHUiNotifierImpl::TNotifierInfo CCCHUiNotifierImpl::RegisterL()
    {
    iInfo.iUid=KCchUiNotifierUid;
    iInfo.iChannel=KCchUiNotifierChannel;
    iInfo.iPriority=ENotifierPriorityVHigh;
    return iInfo;
    }

// ---------------------------------------------------------------------------
// From class CCCHUiNotifierBase.
// ---------------------------------------------------------------------------
//
TPtrC8 CCCHUiNotifierImpl::UpdateL( const TDesC8& /*aBuffer*/ )
    {
    TPtrC8 ret( KNullDesC8 );
    return ret;
    }

// ---------------------------------------------------------------------------
// From class CCCHUiNotifierBase.
// ---------------------------------------------------------------------------
//
void CCCHUiNotifierImpl::StartL(
    const TDesC8& aBuffer, 
    TInt aReplySlot, 
    const RMessagePtr2& aMessage)
    {
    CCHUIDEBUG( "CCCHUiNotifierImpl::StartL - IN" );
    
    __ASSERT_DEBUG( iMessage.IsNull(), User::Leave( KErrAlreadyExists ) );
    
    // Read aBuffer for control information
    TCCHUiNotifierParams param;
    TPckgC<TCCHUiNotifierParams> pckg(param);
    pckg.Set( aBuffer );

    CCCHUiNotifierBase::StartL( aBuffer, aReplySlot, aMessage );    
    iDialogMode = pckg().iDialogMode;
    iServiceId = pckg().iServiceId;
    
    switch( pckg().iDialogMode )
        {
        case MCchUiObserver::ECchUiDialogTypeUsernamePasswordFailed:
            {
            CCHUIDEBUG( "StartL - wrong username/password" ); 
            ShowInvalidUsernamePasswordNoteL( 
                aReplySlot, aMessage, pckg().iServiceId );
            }
            break;
        case MCchUiObserver::ECchUiDialogTypeAuthenticationFailed:
            {
            CCHUIDEBUG( "StartL - auth failed" );	
            ShowUsernamePasswordQueryL( 
                aReplySlot, aMessage, pckg().iServiceId, pckg().iUsername );
            }
            break;

        case MCchUiObserver::ECchUiDialogTypeNoConnectionAvailable:
            {
            CCHUIDEBUG( "StartL - no connections" );	            
            // Do nothing here. Uses RunL to show notifier instead. 
            // Needed for cancel functionality.
            }
            break;    
        case MCchUiObserver::ECchUiDialogTypeNoConnectionDefined:
            {
            CCHUIDEBUG( "StartL - no conn defined" );	                        
            ShowNoConnectionsQueryL( 
                aReplySlot, aMessage, pckg().iServiceId, ETrue );
            }
            break;
        case MCchUiObserver::ECchUiDialogTypeDefectiveSettings:
            {
            CCHUIDEBUG( "StartL - defective settings" );	                        
            ShowDefectiveSettingsL( 
                aReplySlot, aMessage, pckg().iServiceId );
            }
            break;          
        case MCchUiObserver::ECchUiDialogTypeErrorInConnection:
            {
            CCHUIDEBUG( "StartL - error in connection" );                            
            ShowErrorInConnectionNoteL( 
                aReplySlot, aMessage, pckg().iServiceId );
            }
            break;
        case MCchUiObserver::ECchUiDialogTypeConfirmChangeConnection:
            {
            CCHUIDEBUG( "StartL - confirm change connection" );                            
            ShowConfirmChangeConnectionL( 
                aReplySlot,
                aMessage,
                pckg().iServiceId, 
                pckg().iCurrentConnectionIapId );
            }
            break;
        case MCchUiObserver::ECchUiDialogTypeChangeConnection:
            {
            CCHUIDEBUG( "StartL - change connection" );                        
            ShowChangeConnectionQueryL( 
                aReplySlot,
                aMessage,
                pckg().iServiceId,
                pckg().iCurrentConnectionIapId );
            }
            break;     
        default:
            {
            User::Leave( KErrNotSupported );
            }
            break;    
        }
    
    CCHUIDEBUG( "CCCHUiNotifierImpl::StartL - OUT" );
    }

// ---------------------------------------------------------------------------
// From class MCchUiCallStateObserver.
// ---------------------------------------------------------------------------
//
void CCCHUiNotifierImpl::CallStateChanged( TInt aCallState )
    {
    CCHUIDEBUG( "CCCHUiNotifierImpl::CallStateChangedL - Start" );
    
    switch( aCallState )
        {
        case EPSCTsyCallStateRinging:
            {
            if ( iListQueryDialog )
                {
                delete iListQueryDialog;
                iListQueryDialog = NULL;
                
                CompleteMessage( KErrCancel );
                CCCHUiNotifierBase::Cancel();
                }
            }
            break;
        default:
            {
            // Nothing to do.
            }
            break;
        }
    
    CCHUIDEBUG( "CCCHUiNotifierImpl::CallStateChangedL - End" );
    }
