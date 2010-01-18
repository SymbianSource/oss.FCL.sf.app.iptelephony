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
* Description:  Constants
*
*/




#ifndef IPVMBXCONSTANTS_H
#define IPVMBXCONSTANTS_H

#include <spdefinitions.h>

const TInt KMaxIpVoiceMailBoxUriLength = KSPMaxDesLength;



#ifdef EUNIT_TEST_IMPLEMENTATION
    
    #define EUNIT_IMPLEMENTATION \
        friend class UT_CIpVmbxTimer;\
        friend class UT_CIpVmbxInterface;\
        friend class UT_CIpVmbxEngine;\
        friend class UT_CIpVmbxBase;\
        friend class UT_TIpVmbxEventMonitor;\
        friend class CIpVmbxBaseStub;\
        friend class CIpVmbxEngineStub;
        
#else
        
    #define EUNIT_IMPLEMENTATION
        
#endif

#endif // IPVMBXCONSTANTS_H
