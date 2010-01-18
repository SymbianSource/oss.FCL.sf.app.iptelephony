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
* Description:  Settings handler class for svtcallmenu.
*
*/


#include "svtcallstatehandler.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CSvtCallStateHandler::ConstructL( 
        const RArray<CTelMenuExtension::TCallInfo>& aCallArray )
    {
    TInt arrayCount = aCallArray.Count();
    for ( TInt i=0; i < arrayCount; i++ )
        {
        iCallArray.AppendL( aCallArray[i] );
        }
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CSvtCallStateHandler::CSvtCallStateHandler()
    {

    }

  
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CSvtCallStateHandler* CSvtCallStateHandler::NewL(
        const RArray<CTelMenuExtension::TCallInfo>& aCallArray )
    {
    CSvtCallStateHandler* self = new ( ELeave ) CSvtCallStateHandler();
    CleanupStack::PushL( self );
    self->ConstructL( aCallArray );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CSvtCallStateHandler::~CSvtCallStateHandler()
    {
    iCallArray.Close();    
    }

// ---------------------------------------------------------------------------
// Checks is feature supported
// ---------------------------------------------------------------------------
//
TBool CSvtCallStateHandler::FeatureSupported( 
    TFeature aFeature ) const
    {
    TBool ret( EFalse );
    
    switch( aFeature )
        {
        case ESvmFeatureUnattendedTransfer:
            {
            ret = UnattendedTransferPossible();
            break;
            }
        default:
            break;
        }
    
    return ret;
    }


// ---------------------------------------------------------------------------
// Checks is unattended transfer currently possible
// ---------------------------------------------------------------------------
//
TBool CSvtCallStateHandler::UnattendedTransferPossible() const
    {
    TBool ret( EFalse );
    TInt arrayCount = iCallArray.Count();

    for ( TInt i=0; i < arrayCount; i++ )
        {
        if ( iCallArray[i].iCallType == CTelMenuExtension::EPsVoice &&
             iCallArray[i].iCallState == CTelMenuExtension::EActive )
            {
            // Unattended transfer possible if there is active voip call
            ret = ETrue;
            }
        }
    return ret;
    }
