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
* Description:  Defines presencehandler's request ids and types
*
*/


#ifndef SCPPRESENCEHANDLERREQUEST_H_
#define SCPPRESENCEHANDLERREQUEST_H_

// INCLUDES
#include <ximpbase.h>

/**
 * Enumeration for request types.
 */
enum TScpReqType
        {
        EUnknownReq = 0,
        EBindReq ,
        EUnBindReq,
        EPublishOfflineReq,
        EPublishOnlineReq,
        ESubscribeReq
        };

        
/**
 * Class for defining presence handler request id and type
 */
class TScpReqId
    {
public:
    inline TScpReqId();
    inline TScpReqId( const TXIMPRequestId& aId,
                   TScpReqType aType );
    inline void SetId( const TXIMPRequestId& aId );
    inline void SetType( TScpReqType aType );
    inline const TXIMPRequestId& ReqId() const;
    inline TScpReqType Type() const;
private:
    TXIMPRequestId iReqId;
    TScpReqType iType;
    };
    
#include "scppresencehandlerrequest.inl"
    

#endif /*SCPPRESENCEHANDLERREQUEST_H_*/
