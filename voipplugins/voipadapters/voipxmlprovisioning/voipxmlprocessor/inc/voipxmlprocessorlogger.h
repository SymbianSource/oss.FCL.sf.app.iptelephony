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
* Description:  VoIP XML processor logger
*
*/



#ifndef VOIPXMLPROCESSORLOGGER_H
#define VOIPXMLPROCESSORLOGGER_H

//  INCLUDES
#include <e32def.h>
#include <e32debug.h>

//----------------------------------------------------------------------------
//  USER LOGGING SETTINGS
//----------------------------------------------------------------------------

#ifndef _DEBUG

/***************************
* Logging method variants:
*   0 = No logging
*   1 = RDebug
***************************/ 

#define VOIPXMLPROCESSOR_DEBUG     0   // UREL BUILD

#else

#define VOIPXMLPROCESSOR_DEBUG     1   // UDEB BUILD

#endif // _DEBUG

//----------------------------------------------------------------------------
//  LOGGING MACROs (DO NOT EDIT)
//
//  USE THESE MACROS IN YOUR CODE
//----------------------------------------------------------------------------

#if VOIPXMLPROCESSOR_DEBUG == 1    // RDebug

#define DBG_PRINT( AA )           { RDebug::Print( _L( AA ) ); }
#define DBG_PRINT2( AA, BB )      { RDebug::Print( _L( AA ), BB ); }
#define DBG_PRINT3( AA, BB, CC )  { RDebug::Print( _L( AA ), BB, CC ); }

#else   // VOIPXMLPROCESSOR_DEBUG == 0 or invalid -> Disable loggings

#define DBG_PRINT( AA )          
#define DBG_PRINT2( AA, BB )     
#define DBG_PRINT3( AA, BB, CC )    

#endif  // IAS_DEBUG

#endif  // VOIPXMLPROCESSORLOGGER_H
