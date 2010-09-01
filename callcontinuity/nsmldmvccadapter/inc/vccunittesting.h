/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Unit testing definitions for VCC
*
*/



#ifndef _VCCUNITTESTING_H_
#define _VCCUNITTESTING_H_

#ifdef UNIT_TESTING // if unit testing

    #define VCC_UNITTEST( aClassName ) friend class aClassName;

#else

    #define VCC_UNITTEST( aClassName ) 

#endif // UNIT_TESTING


#endif // _VCCUNITTESTING_H_

