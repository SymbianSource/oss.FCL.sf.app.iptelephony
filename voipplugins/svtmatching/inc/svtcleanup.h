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
* Description:  Implementation of cleanup for pointer arrays
*
*/


#ifndef SVTMATCHINGCLEANUP_H
#define SVTMATCHINGCLEANUP_H

#include <e32base.h>

/**
 * Template class for cleaning up arrays that have a ResetAndDestroy() function.
 * To be used with the CleanupStack.
 * 
 * @lib svtmatching.lib
 * @since S60 v5.0
 */
template <class T>
class CleanupResetAndDestroy
	{
	public:	// New functions

		inline static void PushL( T& aRef );

	private: // New functions

		static void ResetAndDestroy( TAny *aPtr );

	};

// INLINE FUNCTIONS
template <class T>
inline void CleanupResetAndDestroy< T >::PushL( T& aRef )
	{
	CleanupStack::PushL( TCleanupItem( &ResetAndDestroy, &aRef ) );
	}

template <class T>
void CleanupResetAndDestroy<T>::ResetAndDestroy( TAny *aPtr )
	{
	( static_cast< T* >( aPtr ) )->ResetAndDestroy();
	( static_cast< T* >( aPtr ) )->Close();
	}

template <class T>
inline void CleanupResetAndDestroyPushL( T& aRef )
	{ CleanupResetAndDestroy< T >::PushL( aRef ); }

#endif // SVTMATCHINGCLEANUP_H
