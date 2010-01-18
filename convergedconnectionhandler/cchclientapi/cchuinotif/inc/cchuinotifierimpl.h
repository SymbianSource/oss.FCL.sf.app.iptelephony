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
* Description:  Actual notifier class implementation.
*
*/


#ifndef C_CCHUINOTIFIERIMPL_H
#define C_CCHUINOTIFIERIMPL_H

#include <cchuiobserver.h> // for dialog types
#include <AknQueryDialog.h>

#include "cchuinotifier.h" // Base class
#include "cchuicommon.hrh"
#include "cchuicallstateobserver.h"

class CCchUiPluginBrandingHandler;
class CCchUiCallStateListener;

/**
 *  Implementation class for notifier.
 *  Implements specific notifier, extends base class functionality.
 *
 *  @code
 *  @endcode
 *
 *  @lib cchuinotif.lib
 *  @since S60 5.0
 */
NONSHARABLE_CLASS( CCCHUiNotifierImpl ): public CCCHUiNotifierBase,
                                         public MCchUiCallStateObserver
    {
public:
    
    static CCCHUiNotifierImpl* NewL();

    /**
     * Destructor.
     */
    virtual ~CCCHUiNotifierImpl();

protected:
    
    /**
     * From MEikSrvNotifierBase2.
     * The notifier has been deactivated 
     * so resources can be freed and outstanding messages completed.
     */
    void Cancel();
    
private:

    CCCHUiNotifierImpl();
    
    void ConstructL();
    
    /**
     * Shows username/password query.
     *
     * @since S60 5.0
     * @param aReplySlot Reply slot.
     * @param aMessage Should be completed when the notifier is deactivated.
     * @param aServiceId Service id to which this note related to.
     * @param aUserName service username.
     */
    void ShowUsernamePasswordQueryL( 
        TInt aReplySlot, 
        const RMessagePtr2& aMessage, 
        TUint aServiceId,
        const TDesC& aUserName );
    
    /**
     * Shows no connections available query
     *
     * @since S60 5.0
     * @param aReplySlot Reply slot.
     * @param aMessage Should be completed when the notifier is deactivated.
     * @param aServiceId Service id to which this note related to.
     * @param aNoConnectionsDefined If ETrue, no connections defined query 
     * is used
     */
    void ShowNoConnectionsQueryL(
        TInt aReplySlot, 
        const RMessagePtr2& aMessage,
        TUint aServiceId,
        TBool aNoConnectionsDefined );
    
    /**
     * Shows change connection query.
     *
     * @since S60 5.0
     * @param aReplySlot Reply slot.
     * @param aMessage Should be completed when the notifier is deactivated.
     * @param aServiceId Service id to which this note related to.
     * @param aIapid iap id of currently used connection
     */
    void ShowChangeConnectionQueryL(
        TInt aReplySlot, 
        const RMessagePtr2& aMessage,
        TUint aServiceId,
        TInt aIapid );
    
    /**
     * Shows defective settings info note.
     *
     * @since S60 5.0
     * @param aReplySlot Reply slot.
     * @param aMessage Should be completed when the notifier is deactivated.
     * @param aServiceId Service id to which this note related to.
     */    
    void ShowDefectiveSettingsL( 
        TInt aReplySlot, 
        const RMessagePtr2& aMessage,
        TUint aServiceId );
    
    /**
     * Shows error in connection info note.
     *
     * @since S60 5.0
     * @param aReplySlot Reply slot.
     * @param aMessage Should be completed when the notifier is deactivated.
     * @param aServiceId Service id to which this note related to.
     */    
    void ShowErrorInConnectionNoteL( 
        TInt aReplySlot, 
        const RMessagePtr2& aMessage,
        TUint aServiceId );
    
    /**
     * Shows invalid username/password note.
     *
     * @since S60 5.0
     * @param aReplySlot Reply slot.
     * @param aMessage Should be completed when the notifier is deactivated.
     * @param aServiceId Service id to which this note related to.
     */    
    void ShowInvalidUsernamePasswordNoteL(
        TInt aReplySlot, 
        const RMessagePtr2& aMessage,
        TUint aServiceId );
    
    /**
     * Shows confirmation note to change connection.
     *
     * @since S60 5.0
     * @param aReplySlot Reply slot.
     * @param aMessage Should be completed when the notifier is deactivated.
     * @param aServiceId Service id to which this note related to.
     * @param aIapid iap id of currently used connection.
     */    
    void ShowConfirmChangeConnectionL( 
        TInt aReplySlot, 
        const RMessagePtr2& aMessage, 
        TUint aServiceId, 
        TInt aIapid  );
        
    /**
     * Return a handle to CCchUiPluginBrandingHandler.
     *
     * @since S60 5.0
     * @return A handler to CCchUiPluginBrandingHandler.
     */
    CCchUiPluginBrandingHandler& BrandingHandlerL();

    /**
     * Fills list items for no connections available query. aListItems will
     * contain shown items as text, aCommandArray will contain the corresponding
     * commands. When query is dismissed, item selection can be mapped to command
     * in aCommandArray.
     *
     * @since S60 5.0
     * @param aListItems array for list items
     * @param aCommandArray array for commands
     * @param aServiceId service id
     */
    void FillNoConnectionsAvailableListboxL( 
        CDesCArray& aListItems, 
	    RArray<TCchUiOperationCommand>& aCommandArray,
	    TUint aServiceId );   
	    
    /**
     * Fills list items for no connections defined query. aListItems will
     * contain shown items as text, aCommandArray will contain the corresponding
     * commands. When query is dismissed, item selection can be mapped to command
     * in aCommandArray.
     *
     * @since S60 5.0
     * @param aListItems array for list items
     * @param aCommandArray array for commands
     */
    void FillNoConnectionsDefinedListboxL( 
        CDesCArray& aListItems, 
	    RArray<TCchUiOperationCommand>& aCommandArray );   	    

    /**
     * Fills list items for no connections are found query. aListItems will
     * contain shown items as text, aCommandArray will contain the corresponding
     * commands. When query is dismissed, item selection can be mapped to command
     * in aCommandArray.
     *
     * @since S60 5.0
     * @param aListItems array for list items
     * @param aCommandArray array for commands
     */
    void FillNoConnectionsFoundListboxL( 
        CDesCArray& aListItems, 
	    RArray<TCchUiOperationCommand>& aCommandArray );
    
    /**
     * Fills list items for change current connection query. aListItems will
     * contain shown items as text, aCommandArray will contain the corresponding
     * commands. When query is dismissed, item selection can be mapped to command
     * in aCommandArray.
     *
     * @since S60 5.0
     * @param aListItems array for list items
     * @param aCommandArray array for commands
     */
    void FillChangeCurrentConnectionListboxL( 
        CDesCArray& aListItems, 
        RArray<TCchUiOperationCommand>& aCommandArray );
    
    /**
     * Fills list items for change connection query. aListItems will
     * contain shown items as text, aCommandArray will contain the corresponding
     * commands. When query is dismissed, item selection can be mapped to command
     * in aCommandArray.
     *
     * @since S60 5.0
     * @param aListItems array for list items
     * @param aCommandArray array for commands
     */
    void FillChangeConnectionListboxL( 
        CDesCArray& aListItems, 
        RArray<TCchUiOperationCommand>& aCommandArray ); 

    /**
     * Shows custom GPRS access point selection. User can select GPRS access
     * point from list which is formed from access points in Internet SNAP.
     *
     * @since S60 5.0
     * @param aServiceId, service id
     */
    void ShowGprsSelectionL( TUint aServiceId );
    
    TBool IsVoIPOverWCDMAAllowedL();

// from base class CCCHUiNotifierBase

    /**
     * From CCCHUiNotifierBase.
     * Called when a notifier is first loaded.     
     *
     * @since S60 5.0
     * @return A structure containing priority and channel info.
     */
    TNotifierInfo RegisterL();
    
    /**
     * From CCCHUiNotifierBase.
     * Updates a currently active notifier.
     *
     * @since S60 5.0
     * @param aBuffer The updated data.
     * @return A pointer to return value.
     */
    TPtrC8 UpdateL( const TDesC8& aBuffer );
    
    /**
     * From CCCHUiNotifierBase.
     * Used in asynchronous notifier launch to 
     * store received parameters into members variables and 
     * make needed initializations.
     *
     * @since S60 5.0
     * @param aBuffer A buffer containing received parameters
     * @param aReplySlot Reply slot.
     * @param aMessage Should be completed when the notifier is deactivated.
     */
    void StartL(
        const TDesC8& aBuffer,
        TInt aReplySlot, 
        const RMessagePtr2& aMessage );

    /** 
     * From CActive
     */
    void RunL();
    
public:

// from base class MCchUiCallStateObserver

    /**
     * From MCchUiCallStateObserver.
     * @see MCchUiCallStateObserver.
     */
    void CallStateChanged( TInt aCallState );

private: // data

    /**
     * Branding handler. Lifetime of this pointer must be the same as 
     * for icons fetched using it.
     * Own.
     */
    CCchUiPluginBrandingHandler* iBrandingHandler;
    
    /**
     * Result parameters.
     */
    TCCHUiNotifierParams iResultParams;

    /**
     * Handle to created CAknListQueryDialog.
     * Own.
     */
    CAknListQueryDialog* iListQueryDialog;

    /**
     * Listener for call state changes.
     * Own.
     */
    CCchUiCallStateListener* iCallStateListener;
    
    TUint iServiceId;
    
    CCHUI_UNIT_TEST( T_CchUiNotifierImpl )
    };

#endif // C_CCHUINOTIFIERIMPL_H
