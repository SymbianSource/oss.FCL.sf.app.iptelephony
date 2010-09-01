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
* Description:  Logging header
*
*/


#ifndef SMLDMVOIPDEBUG_H
#define SMLDMVOIPDEBUG_H

//  INCLUDES
#include <e32debug.h>

#ifdef _DEBUG
#define DBG_PRINT( p )     RDebug::Print( _L( p ) )
#define DBG_PRINT2( p, a ) RDebug::Print( _L( p ), a )
#else
#define DBG_PRINT( p )
#define DBG_PRINT2( p, a )
#endif // _DEBUG

#endif  // SMLDMVOIPDEBUG_H

// End of file.
