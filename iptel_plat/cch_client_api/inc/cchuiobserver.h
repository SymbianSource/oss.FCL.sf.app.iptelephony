/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Observer definition for CCH UI clients
*
*/


#ifndef M_CCHUIOBSERVER_H
#define M_CCHUIOBSERVER_H

/**
 *  CCH UI Observer declaration class.
 *
 *  This class defines observer implementation for the CCH UI
 *  clients. Every client must derive this class in order to
 *  be able to use services provided by CCH UI and to be able
 *  to receive observer callbacks.
 *  @code
 *  inherit in header:
 *  class CMyClass : public CBase, public MCchUiObserver
 *  @endcode
 *
 *  @lib cch.lib
 *  @since S60 5.0
 */
class MCchUiObserver
    {
public:

    /**  Defines dialog types available through this API */
    enum TCchUiDialogType
    	{
    	ECchUiDialogTypeNotSet,
    	// For connecting note showing
    	ECchUiDialogTypeConnecting,
    	// For disconnecting note showing
        ECchUiDialogTypeDisconnecting,
        // For showing info note about invalid username/password.
        // Will also automatically show username/password query after
        // showing info note.
        ECchUiDialogTypeUsernamePasswordFailed,
        // For showing username/password query.
    	ECchUiDialogTypeAuthenticationFailed,
    	// For showing no connections defined query. When user inputs
    	// username/password and accepts query, CCHUI will save them
    	// and also will re-enable service.
    	ECchUiDialogTypeNoConnectionDefined,    
    	// For showing no connections available query. When user selects
    	// new access point, CCHUI adds it to service큦 SNAP and re-enables
    	// service automatically.
    	ECchUiDialogTypeNoConnectionAvailable,   
    	// For showing confirmation query to change connection.
    	// Will show automatically also change connection query if
    	// confirmation query accepted by user.
    	ECchUiDialogTypeConfirmChangeConnection,
    	// For showing change connection query. When user selects
    	// new access point, CCHUI add it to service큦 SNAP and re-enables
    	// service. CCHUI will also delete old access point from service큦
    	// SNAP if access point is not used by other service(s).
    	ECchUiDialogTypeChangeConnection,
    	// For showing defective settings info note.
    	ECchUiDialogTypeDefectiveSettings,
    	// For showing error in connection info note.
    	// Will show automatically also confirm change connection query
    	// and confirmation query is accepted by user, change connection
    	// query is showed also.
    	ECchUiDialogTypeErrorInConnection
    	};
    	
    /**  Defines operation results ie. indicates what have been done. 
         Client can implement handling based on these on callback. */
    enum TCchUiOperationResult
    	{
    	// Operation result not set
    	ECchUiClientOperationResultNotSet,
    	// User has cancelled query.
    	ECchUiClientOperationResultUserCancelled,
    	// Failure that prevents enabling service has happened.
    	ECchUiClientOperationResultGeneralFailure,
    	// CCHUI has added new access point to service큦 SNAP.
    	ECchUiClientOperationResultAccessPointAdded,
    	// CCHUI has added new access point to service큦 SNAP. 
    	// CCHUI has also removed previously used access point from
    	// service큦 SNAP if it was not used by other service(s).
    	ECchUiClientOperationResultConnectionChanged,
    	// CCHUI has saved new username/password for service
    	ECchUiClientOperationResultCredentialsChanged
    	};
        
    /**
     * Gets called when cchui has finished showing connectivity dialogs
     * and possible connectivity related operations based on user
     * actions in those dialogs. 
     *
     * @since S60 5.0
     * @param aServiceId Service id.
     * @param aServiceState Service state after cchui operations.
     * @param aOperationResult Contains last cchui operation.
     * @return None
     */
    virtual void ConnectivityDialogsCompletedL(
        TInt aServiceId, 
        TCchUiOperationResult aOperationResult ) = 0;
    };

#endif // M_CCHUIOBSERVER_H
