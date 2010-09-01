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
* Description:   Panic codes
*
*/



#ifndef __VCCHO_PAN__
#define __VCCHO_PAN__

/**  Panic codes */
enum TvcchoPanic
	{
	EvcchoNullPointer
	};

/**
 * Panics
 *
 * @since S60 3.2
 * @param aPanic Panic code
  */
GLREF_C void Panic( TvcchoPanic aPanic );

#endif  // __VCCHO_PAN__


