/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Utilities methods for VoIP XML processor
*
*/



// ---------------------------------------------------------------------------
// Pushes an item to CleanupStack.
// ---------------------------------------------------------------------------
//
template <class T>
inline void CleanupResetAndDestroy<T>::PushL( T& aRef )
    {
    CleanupStack::PushL( TCleanupItem( &ResetAndDestroy, &aRef ) );
    }

// ---------------------------------------------------------------------------
// Cleans up an item.
// ---------------------------------------------------------------------------
//
template <class T>
inline void CleanupResetAndDestroy<T>::ResetAndDestroy( TAny *aPtr )
    {
    ( STATIC_CAST( T*, aPtr ) )->ResetAndDestroy();
    ( STATIC_CAST( T*, aPtr ) )->Close();
    }

// ---------------------------------------------------------------------------
// Calls CleanupResetAndDestroy<T>::PushL
// ---------------------------------------------------------------------------
//
template <class T>
inline void CleanupResetAndDestroyL( T& aRef )
    {
    CleanupResetAndDestroy<T>::PushL( aRef );
    }

// End of file.

