/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Inline functions
*
*/


// --------------------------------------------------------------------------
// TScpReqId::TScpReqId
// --------------------------------------------------------------------------
//
inline TScpReqId::TScpReqId() :
    iType( EUnknownReq )
    {
    }

// --------------------------------------------------------------------------
// TScpReqId::TScpReqId
// --------------------------------------------------------------------------
//
inline TScpReqId::TScpReqId( const TXIMPRequestId& aId, TScpReqType aType ) :
    iReqId( aId ),
    iType( aType )
    {
    }

// --------------------------------------------------------------------------
// TScpReqId::SetId
// --------------------------------------------------------------------------
//
inline void TScpReqId::SetId( const TXIMPRequestId& aId )
    {
    iReqId = aId;
    }

// --------------------------------------------------------------------------
// TScpReqId::SetType
// --------------------------------------------------------------------------
//
inline void TScpReqId::SetType( TScpReqType aType )
    {
    iType = aType;
    }


// --------------------------------------------------------------------------
// TScpReqId::ReqId
// --------------------------------------------------------------------------
//
inline const TXIMPRequestId& TScpReqId::ReqId()const
    {
    return iReqId;
    }

// --------------------------------------------------------------------------
// TScpReqId::Type
// --------------------------------------------------------------------------
//
inline TScpReqType TScpReqId::Type() const
    {
    return iType;
    }

// end of file
