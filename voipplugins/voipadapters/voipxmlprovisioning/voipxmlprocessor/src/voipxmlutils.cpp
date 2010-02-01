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
* Description:  Utilities class for VoIP XML processor
*
*/


//#include <e32base.h>
#include "voipxmlutils.h"


// ---------------------------------------------------------------------------
// VoipXmlUtils::DesToInt()
// ---------------------------------------------------------------------------
//
TInt VoipXmlUtils::DesToInt( const TDesC& aDesValue, TInt& aIntValue )
    {
    TLex lex( aDesValue );
    return lex.Val( aIntValue );
    }

// ---------------------------------------------------------------------------
// VoipXmlUtils::DesToInt()
// ---------------------------------------------------------------------------
//
TInt VoipXmlUtils::Des8ToInt( const TDesC8& aDesValue, TInt& aIntValue )
    {
    TLex8 lex( aDesValue );
    return lex.Val( aIntValue );
    }

// End of file.

