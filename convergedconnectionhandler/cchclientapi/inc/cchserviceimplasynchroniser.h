/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/

#ifndef CCHSERVICEIMPLASYNCHRONISER_H
#define CCHSERVICEIMPLASYNCHRONISER_H

#include <e32base.h>	// For CActive, link against: euser.lib
#include <e32std.h>		// For RTimer, link against: euser.lib
//from cchclient
#include <cchtypes.h>
#include "cchimpl.h"
#include "cch.h"

class CCchImpl;

#ifdef CCHAPI_USE_CCHUI
class CCchUiPrivateApi;
#else 
#define CCchUiPrivateApi TInt 
#endif



NONSHARABLE_CLASS ( CCchServiceImplAsynchroniser) : public CActive
    {
public:

    enum TCchServiceImplState
        {
        EIdle = 0,
        EEnabling = 1,
        EDisabling = 2 
        };
    // Cancel and destroy
    ~CCchServiceImplAsynchroniser();

    // Two-phased constructor.
    static CCchServiceImplAsynchroniser* NewL(CCchImpl& aCch, TInt iServiceId, CCchUiPrivateApi& aCchUi);

    // Two-phased constructor.
    static CCchServiceImplAsynchroniser* NewLC(CCchImpl& aCch, TInt iServiceId, CCchUiPrivateApi& aCchUi);

    void Enable( TCCHSubserviceType aType );
    
    void Disable( TCCHSubserviceType aType );

private:
    // C++ constructor
    CCchServiceImplAsynchroniser(CCchImpl& aCch, TInt iServiceId, CCchUiPrivateApi& aCchUi);

    // Second-phase constructor
    void ConstructL();

private:
    // From CActive
    // Handle completion
    void RunL();

    // How to cancel me
    void DoCancel();

    // Override to handle leaves from RunL(). Default implementation causes
    // the active scheduler to panic.
    TInt RunError(TInt aError);

private:
    CCchImpl& iCch;
    TInt iServiceId;
    CCchUiPrivateApi& iCchUi; 
    TCchServiceImplState iState;
    };

#endif // CCHSERVICEIMPLASYNCHRONISER_H
