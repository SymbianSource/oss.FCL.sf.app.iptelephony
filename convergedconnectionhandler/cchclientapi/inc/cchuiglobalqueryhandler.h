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
* Description:  Class handling notifier plugin usage.
*
*/


#ifndef C_CCHUIGLOBALQUERYHANDLER_H
#define C_CCHUIGLOBALQUERYHANDLER_H

#include <AknNotify.h>
#include <AknsItemID.h>
#include <AknNotifyStd.h>
#include <AknQueryDialog.h>

#include "cchuicommon.hrh"

class CAknSDData;


/**
 * Global query handler.
 * This class handles qlobal query usage in cchui.
 *
 * @code
 *  //Example how to show change connection query. You need to inherint class
 *  //that is using globalqueryhandler from CActive to get result in 
 *  //RunL when dialog is completed.
 *  iGlobalQueryHandler = CCchUiGlobalQueryHandler::NewL();
 *  TInt serviceId( KServiceId );
 *  MCchUiObserver::TCchUiDialogType dialog = 
 *      MCchUiObserver::ECchUiDialogTypeChangeConnection;
 *  iGlobalQueryHandler->ShowMsgQueryL( dialog, iStatus, serviceId );
 *  SetActive();
 * @endcode
 *
 * @lib cchui.lib
 * @since S60 5.0
 */
NONSHARABLE_CLASS( CCchUiGlobalQueryHandler ): public CBase
    {
public:
    
    /**
     * Two-phased constructor.
     */
    static CCchUiGlobalQueryHandler* NewL();
    
    /**
     * Two-phased constructor.
     */
    static CCchUiGlobalQueryHandler* NewLC();

    /**
     * Destructor.
     */
    virtual ~CCchUiGlobalQueryHandler();
    
    
    /**
     * Shows global message query asynchronously.
     *
     * @since S60 5.0
     * @param aNote Note mode.
     * @param aStatus Asynchronic observer iStatus.
     * @param aServiceId Service id.
     * @param aIapId iap id.
     * @param aUserName service username.
     */
    void ShowMsgQueryL( 
        MCchUiObserver::TCchUiDialogType aNote,
        TRequestStatus& aStatus,
        TUint aServiceId,
        TUint aIapId,
        RBuf& aUserName );
       
    /**
     * Update the softkeys of the message query.
     *
     * @since S60 5.0
     * @param aSoftkeys New resource for softkeys.
     */
    void UpdateMsgQuery( TInt aSoftkeys );
    
    /**
     * Cancel the message query.
     *
     * @since S60 5.0
     */
    void CancelMsgQuery();
    
    /**
     * Returns result params.
     *
     * @since S60 5.0
     * @return Result params.
     */
    TCCHUiNotifierParams ResultParams();
    
    
private:

    CCchUiGlobalQueryHandler();
    
    void ConstructL();
    

private: // data
    
    /**
     * Handle to RNotifier.
     */
    RNotifier iNotify;
    
    /**
     * Global query command.
     */
    TAknGlobalQueryCmd iCmd;
    
    /**
     * Notifier params package.
     */
    TCCHUiNotifierParamsPckg iPckg;
    
    /**
     * Contains resource id for softkeys.
     */
    TInt iSoftkeys;
    
    /**
     * State of iNotify
     */
    TBool iNotifyStarted;
    
    CCHUI_UNIT_TEST( UT_CchUi )
    };

#endif // C_CCHUIGLOBALQUERYHANDLER_H
