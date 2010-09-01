/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <sipprofileregistryobserver.h>

#include "cchuinotifier.h" // Base class
#include "cchuicommon.hrh"
#include "cchuicallstateobserver.h"

class CSIPManagedProfile;
class CSIPManagedProfileRegistry;
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
                                         public MCchUiCallStateObserver,
                                         public MSIPProfileRegistryObserver
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
     * @return Error code.
     */
    TInt ShowGprsSelectionL( TUint aServiceId );
    
    /**
     * Returns ETrue if VoIP over WCDMA is allowed.
     *
     * @since S60 5.0
     * @return ETrue if allowed.
     */
    TBool IsVoIPOverWCDMAAllowedL();

    /**
     * Returns ETrue if current service SNAP is missing gprs accespoints
     * which are set to the internet SNAP.
     *
     * @since S60 9.2
     * @return ETrue if the internet snap has gprs connection
     *         which are not in the services snap.
     */
    TBool IsServiceSnapMissingInternetGprsApsL() const;
    
    /**
     * Returns internet GPRS IAP's informations which are not
     * in the current service snap.
     * 
     * @since S60 9.2
     * @param aIaps Iap names in return.
     * @param aIapIds IapIds in return.
     * @return internet SNAP ID.
     */
    TUint32 InternetGprsApsMissingFromServiceSnapL(
        CDesCArray& aIaps, RArray<TUint32>& aIapIds ) const;

    /**
     * For deleting RPointerArray in case of leave.
     *
     * @since S60 5.0
     * @param aPointerArray for pointer array to be deleted
     */
     static void ResetAndDestroy( TAny* aPointerArray );
    
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
    
    
    // from base class MSIPProfileRegistryObserver
      
    /** 
     * From MSIPProfileRegistryObserver 
     * SIP profile information event.
     *
     * @since S60 v3.0
     * @param aProfileId is id for profile
     * @param aEvent type of information event
     */
    void ProfileRegistryEventOccurred(
        TUint32 aSIPProfileId, 
        TEvent aEvent );

          
    /**
     * From MSIPProfileRegistryObserver
     * An asynchronous error has occurred related to SIP profile.
     *
     * @since S60 v3.0
     * @param aSIPProfileId the id of failed profile 
     * @param aError a error code
     */
     void ProfileRegistryErrorOccurred(
         TUint32 aSIPProfileId,
         TInt aError );
    
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
    
    /**
     * Service ID.
     */
    TUint iServiceId;
    
    /**
     * Current connection IAP ID.
     */
    TUint iCurrentConnectionIapId;

    /*
     * Handle to SIP managed profile registry.
     * Own.
     */
    CSIPManagedProfileRegistry* iSipProfileRegistry;
    
    CCHUI_UNIT_TEST( T_CchUiNotifierImpl )
    };

#endif // C_CCHUINOTIFIERIMPL_H
