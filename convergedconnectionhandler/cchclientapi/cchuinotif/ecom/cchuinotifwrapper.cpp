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
* Description:  ECOM wrapper for CCH UI Notifier plugin
*
*/


#include <AknNotifierWrapper.h> // link against aknnotifierwrapper.lib
#include <implementationproxy.h>

#include "cchuicommon.hrh"

const TInt KMaxSynchReplyBufLength = 256;
const TInt KCchUiNotifierPriority  = 
    MEikSrvNotifierBase2::ENotifierPriorityVHigh;
const TInt KCchUiNotifierArrayIncrement = 1;


// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// CleanupArray.
// ---------------------------------------------------------------------------
//
void CleanupArray(TAny* aArray)
    {
    CArrayPtrFlat<MEikSrvNotifierBase2>*     
        subjects=static_cast<CArrayPtrFlat<MEikSrvNotifierBase2>*>(aArray);
    TInt lastInd = subjects->Count()-1;
    for (TInt i=lastInd; i >= 0; i--)
        subjects->At(i)->Release();             
    delete subjects;
    }


// ---------------------------------------------------------------------------
// DoCreateNotifierArrayL.
// ---------------------------------------------------------------------------
//
CArrayPtr<MEikSrvNotifierBase2>* DoCreateNotifierArrayL()
    {       
    CArrayPtrFlat<MEikSrvNotifierBase2>* subjects=
        new (ELeave)CArrayPtrFlat<MEikSrvNotifierBase2>(
                KCchUiNotifierArrayIncrement);
    
    CleanupStack::PushL(TCleanupItem(CleanupArray, subjects));

    // Create Wrappers

    // Session owning notifier
    CAknCommonNotifierWrapper* master = 
        CAknCommonNotifierWrapper::NewL( KCchUiNotifierUid,
                                   KCchUiNotifierChannel,
                                   KCchUiNotifierPriority,
                                   _L("cchuinotif.dll"),
                                   KMaxSynchReplyBufLength);               
               
    subjects->AppendL( master );
    
    CleanupStack::Pop();    // array cleanup
    return(subjects);
    }

// ---------------------------------------------------------------------------
// NotifierArray.
// ---------------------------------------------------------------------------
//
CArrayPtr<MEikSrvNotifierBase2>* NotifierArray()
    // old Lib main entry point
    {            
    CArrayPtr<MEikSrvNotifierBase2>* array = 0;
    TRAPD(ignore, array = DoCreateNotifierArrayL());
    if (ignore != KErrNone) 
        {
        ignore = KErrNone;
        }
    return array;
    }


// ---------------------------------------------------------------------------
// ImplementationTable.
// ---------------------------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] =
    {
#ifdef __EABI__
    {{0x200195A7},(TFuncPtr)NotifierArray}
#else
    {{0x200195A7},NotifierArray}
#endif
    };


// ---------------------------------------------------------------------------
// ImplementationGroupProxy.
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
    TInt& aTableCount )
    {       
    
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }


