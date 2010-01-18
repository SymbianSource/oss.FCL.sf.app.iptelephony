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
* Description:  Implements notifier creation, required for notifier fw.
*
*/


#include <eikenv.h>
#include <cenrepnotifyhandler.h>

#include "cchuinotifierimpl.h"

const TInt KCchUiNotifierArrayIncrement = 10;


// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// Instantiate notifiers.
// ---------------------------------------------------------------------------
//
LOCAL_C void CreateCchUiNotifiersL( 
    CArrayPtrFlat<MEikSrvNotifierBase2>* aNotifiers )
    {
    __ASSERT_ALWAYS( aNotifiers, User::Leave( KErrArgument ) );
    CCCHUiNotifierImpl* notifierImpl = 
        CCCHUiNotifierImpl::NewL();
    CleanupStack::PushL( notifierImpl );
    aNotifiers->AppendL( notifierImpl );
    CleanupStack::Pop( notifierImpl );
    }

// ---------------------------------------------------------------------------
// Creates a notifiers array.
// ---------------------------------------------------------------------------
//
EXPORT_C CArrayPtr<MEikSrvNotifierBase2>* NotifierArray()
    {    
    CArrayPtrFlat<MEikSrvNotifierBase2>* notifiers = NULL;

    TRAPD(err, notifiers = 
               new (ELeave)CArrayPtrFlat<MEikSrvNotifierBase2>( 
                       KCchUiNotifierArrayIncrement ));
               
    if ( err == KErrNone )
        {
        if( notifiers )
            {
            TRAPD( err2, CreateCchUiNotifiersL( notifiers ));
            if( err2 )
                {
                TInt count = notifiers->Count();
                while(count--)
                    (*notifiers)[count]->Release();
                delete notifiers;
                notifiers = NULL;
                }
            }
        }
    else
        {   
        }
        
    return notifiers;
    }



