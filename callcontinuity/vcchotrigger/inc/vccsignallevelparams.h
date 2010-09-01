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
* Description:   Definition of signal level parameters
*
*/



#ifndef VCCSIGNALLEVELPARAMS_
#define VCCSIGNALLEVELPARAMS_

/**  Signal level parameters, timeouts and signal strength levels */
struct TSignalLevelParams
    {
    TTimeIntervalMicroSeconds32 iLowTimeout;
    TTimeIntervalMicroSeconds32 iHighTimeout;
    TInt32 iLowLevel;
    TInt32 iHighLevel;
    };

#endif /*VCCSIGNALLEVELPARAMS_*/
