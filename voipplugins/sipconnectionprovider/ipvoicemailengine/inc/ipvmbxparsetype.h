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
* Description:  Parsing options for optional data
*
*/



#ifndef TIPVMBXPARSETYPE_H
#define TIPVMBXPARSETYPE_H

#include <e32base.h>


/**
 *  Parsing options for content
 *
 *  @lib ipvoicemailengine.dll
 */
class TIpVmbxParseType
    {

public:

    /**  Supported parse types */
    enum TParseTypes
        {
        EDetailTo,
        EDetailToShort,
        EDetailFrom,
        EDetailFromShort,
        EDetailSubject,
        EDetailSubjectShort,
        EDetailDate,
        EDetailPriority,
        EDetailId
        };


    /**
     * Constructor.
     *
     * @param aType Type to be parsed
     */
    TIpVmbxParseType( const TParseTypes aType );

    TIpVmbxParseType();

    /**
     * Tag this instance is presenting
     *
     * @return Tag
     */
    const TDesC8& Tag() const;

    /**
     * Tag this instance is presenting
     *
     * @return Tag
     */
    TInt32 ResourceId() const;

    /**
     * Set new tag
     *
     * @param aType Set different parse type
     */
    void Set( const TParseTypes aType );


private: // data

    /**
     * Tag
     */
    const TDesC8* iTag;

    /**
     * Resource matching to tag
     */
    TInt32 iResourceId;

    };

#include "ipvmbxparsetype.inl"

#endif // TIPVMBXPARSETYPE_H
