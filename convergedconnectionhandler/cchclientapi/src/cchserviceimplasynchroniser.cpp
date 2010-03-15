/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  cch client api implementation
*
*/


#include "cchserviceimplasynchroniser.h"
#include "cchlogger.h"

CCchServiceImplAsynchroniser::CCchServiceImplAsynchroniser(CCchImpl& aCch, TInt aServiceId, CCchUiPrivateApi& aCchUi) :
    CActive(EPriorityStandard),// Standard priority
    iCch( aCch ),
    iServiceId( aServiceId ),
    iCchUi( aCchUi ),
    iState ( EIdle )

    {
    }

CCchServiceImplAsynchroniser* CCchServiceImplAsynchroniser::NewLC(CCchImpl& aCch, TInt aServiceId, CCchUiPrivateApi& aCchUi)
    {
    CCchServiceImplAsynchroniser* self =
            new (ELeave) CCchServiceImplAsynchroniser(aCch, aServiceId, aCchUi );
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CCchServiceImplAsynchroniser* CCchServiceImplAsynchroniser::NewL(CCchImpl& aCch, TInt aServiceId, CCchUiPrivateApi& aCchUi)
    {
    CCchServiceImplAsynchroniser* self =
            CCchServiceImplAsynchroniser::NewLC(aCch, aServiceId, aCchUi );
    CleanupStack::Pop(); // self;
    return self;
    }

void CCchServiceImplAsynchroniser::Enable( TCCHSubserviceType aType )
    {
    CCHLOGSTRING( "CCchServiceImplAsynchroniser::Enable IN" );
    if (iState == EIdle)
        {
        iState = EEnabling;
        TServiceSelection selection( iServiceId, aType );
        SetActive();
        iCch.CchClient().EnableService( selection, iStatus, EFalse );
        }
    else
        {
        CCHLOGSTRING( "CCchServiceImplAsynchroniser already active" );
        }
    CCHLOGSTRING( "CCchServiceImplAsynchroniser::Enable OUT" );
    }

void CCchServiceImplAsynchroniser::Disable( TCCHSubserviceType aType )
    {
    CCHLOGSTRING( "CCchServiceImplAsynchroniser::Disable IN" );
    if (iState == EIdle)
        {
        iState = EDisabling;
        TRequestStatus status = KErrNone;
        TServiceSelection selection( iServiceId, aType );
        SetActive();
        iCch.CchClient().DisableService( selection, iStatus );
        }
    else
        {
        CCHLOGSTRING( "CCchServiceImplAsynchroniser already active" );
        }
    CCHLOGSTRING( "CCchServiceImplAsynchroniser::Disable OUT" );
    }


void CCchServiceImplAsynchroniser::ConstructL()
    {
       CActiveScheduler::Add(this); // Add to scheduler
    }

CCchServiceImplAsynchroniser::~CCchServiceImplAsynchroniser()
    {
    Cancel(); // Cancel any request, if outstanding
    // Delete instance variables if any
    }

void CCchServiceImplAsynchroniser::DoCancel()
    {

    }

void CCchServiceImplAsynchroniser::RunL()
    {
	CCHLOGSTRING( "CCchServiceImplAsynchroniser::RunL IN" );
    if (iStatus.Int() != KErrCancel)
        {
        switch (iState)
            {
            case EEnabling:
                {
                if ( iCch.ConnectivityDialogsAllowed() )
                    {
                    iCchUi.ManualEnableResultL( iServiceId, iStatus.Int() );
                    }
                CCHLOGSTRING( "CCchServiceImplAsynchroniser::RunL EEnabling done" );
                iState = EIdle;
                break;
                }
            case EDisabling:
                {
				CCHLOGSTRING( "CCchServiceImplAsynchroniser::RunL EDisabling done" );
                iState = EIdle;
                break;
                }
            default:
                {
                break;
                }

            }
        }
    iState = EIdle;
    CCHLOGSTRING( "CCchServiceImplAsynchroniser::RunL OUT" );
    }

TInt CCchServiceImplAsynchroniser::RunError(TInt aError)
    {
    return aError;
    }
