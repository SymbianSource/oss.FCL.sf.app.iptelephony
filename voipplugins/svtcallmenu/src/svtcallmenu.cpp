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
* Description:  Main class for svtcallmenu.
*
*/


#include "svtcallmenu.h"
#include "svtcallstatehandler.h"

#include <eikmenup.h>
#include <phoneappcommands.hrh>
#include <phoneappvoipcommands.hrh>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CSvtCallMenu::CSvtCallMenu()
    {
    }

  
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CSvtCallMenu* CSvtCallMenu::NewL()
    {
    CSvtCallMenu* self = new ( ELeave ) CSvtCallMenu;
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CSvtCallMenu::~CSvtCallMenu()
    {
    delete iCallStateHandler;
    }
      
       
// ---------------------------------------------------------------------------
// Modifys call menu
// ---------------------------------------------------------------------------
//
void CSvtCallMenu::ModifyExtensionsCallMenuL( 
        const RArray<CTelMenuExtension::TCallInfo>& aCallArray,
        CEikMenuPane& aMenuPane )
    {
    CreateCallStateHandlerL( aCallArray );
    CEikMenuPane*  menuPane= &aMenuPane;
    RArray<TInt> menuItemArray;
    CleanupClosePushL( menuItemArray );

    // Menu items to array
    TInt menuItemCount = menuPane->NumberOfItemsInPane();
    for ( TInt i( 0 ); i < menuItemCount; i++ )
        {
        menuItemArray.AppendL( menuPane->MenuItemCommandId( i ) );
        }

    // Menu items to be deleted 
    for ( TInt i( 0 ); i < menuItemArray.Count(); i++ )
        {
        switch( menuItemArray[ i ] )
            {
            case EPhoneInCallCmdUnattendedTransfer:
                {
                if ( !iCallStateHandler->FeatureSupported( 
                       CSvtCallStateHandler::ESvmFeatureUnattendedTransfer ) )
                    {
                    menuPane->DeleteMenuItem( menuItemArray[ i ] ); 
                    }
                break;
                }
            case EPhoneInCallCmdSwitchToVideo:
            case EPhoneInCallCmdCreateConference:
            case EPhoneInCallCmdConferenceMenu:
            case EPhoneInCallCmdJoin:
            case EPhoneCallComingCmdSoftReject:
            case EPhoneInCallCmdSendToCallerMenu:
                    {
                    // SIP voip doesn't support above features
                    // Remove those items from the menu
                    menuPane->DeleteMenuItem( menuItemArray[ i ] ); 
                    }
                break;
            default: 
                break;
            }
        }
    
    CleanupStack::PopAndDestroy( &menuItemArray );    

    }

// ---------------------------------------------------------------------------
// Modifys touch pane
// ---------------------------------------------------------------------------
//
void CSvtCallMenu::ModifyExtensionsTouchPaneButtonsL( 
        const RArray<CTelMenuExtension::TCallInfo>& /*aCallArray*/,
        MTelTouchPaneExtension& aTouchPane )
    {  
    TInt buttonCount = aTouchPane.NumberOfButtonsInPane();
        
    for ( TInt i=0; i < buttonCount; i++ )    
        {
        TInt commandId = aTouchPane.ButtonCommandId( i );
    
        switch ( commandId )
            {
            case EPhoneInCallCmdConferenceMenu:
            case EPhoneInCallCmdCreateConference:
            case EPhoneInCallCmdJoin:
                {
                // SIP VoIP not support conference -> set item dimmed
                aTouchPane.SetButtonDimmed( commandId, ETrue );         
                break;  
                }
            default:
                break;                    
            } // switch
        } // for        
    }
          
// ---------------------------------------------------------------------------
// Handles custom menu commands.
// ---------------------------------------------------------------------------
//
TBool CSvtCallMenu::HandleCommandL( TInt /*aCommand*/ )
    {        
    return EFalse;
    }
    
// ---------------------------------------------------------------------------
// Creates call state handler
// ---------------------------------------------------------------------------
//
void CSvtCallMenu::CreateCallStateHandlerL(
        const RArray<CTelMenuExtension::TCallInfo>& aCallArray )
    {
    if ( iCallStateHandler )
        {
        delete iCallStateHandler;
        iCallStateHandler = NULL;
        }
    
    iCallStateHandler = CSvtCallStateHandler::NewL( aCallArray );
    }
