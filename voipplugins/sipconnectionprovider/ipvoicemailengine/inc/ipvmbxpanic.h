/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Panic procedure
*
*/




#ifndef IPVMBXPANIC_H
#define IPVMBXPANIC_H

#include <e32base.h>

_LIT( KIpVmePanic, "IpVmePanic" );

/**
 * Provides panic message with id and value
 *
 * @param Error value
 */
inline void Panic( TInt aErr )
    {
    User::Panic( KIpVmePanic, aErr );
    }

#endif // IPVMBXPANIC_H
