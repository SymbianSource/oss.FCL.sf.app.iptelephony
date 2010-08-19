/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/


#ifndef CCHSERVICE_INL
#define CCHSERVICE_INL

//INCLUDES
#include "cchserviceobserver.h"

// ---------------------------------------------------------------------------
// CCchService::CCchService 
// ---------------------------------------------------------------------------
//
inline CCchService::CCchService()
    {
    }

// ---------------------------------------------------------------------------
// CCchService::NewL 
// ---------------------------------------------------------------------------
//
inline CCchService* CCchService::NewL( TUid aImplementationUid,
                                       MCchServiceObserver& aCchObserver )
    {
    return reinterpret_cast< CCchService* >( 
                REComSession::CreateImplementationL( 
                    aImplementationUid, 
                    _FOFF( CCchService, iInstanceKey ),
                    ( TAny* )( &aCchObserver ) ) );
    }

// ---------------------------------------------------------------------------
// CCchService::CCchService 
// ---------------------------------------------------------------------------
//
inline CCchService::~CCchService()
    {
    REComSession::DestroyedImplementation( iInstanceKey );
    }

#endif // CCHSERVICE_INL

// End of File
