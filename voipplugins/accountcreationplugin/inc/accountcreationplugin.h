/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header file for CAccountCreationPlugin
 *
*/


#ifndef ACCOUNTCREATIONPLUGIN_H
#define ACCOUNTCREATIONPLUGIN_H

#include <AiwServiceIfBase.h>
#include <AiwServiceHandler.h>

#include "accountcreationplugin.hrh"
#include "maccountcreationpluginobserver.h"

class MAiwNotifyCallback;
class CAcpProviderListView;
class MAccountCreationPluginObserver;
class CEikonEnv;

/**
 *  CAccountCreationPlugin class
 *  Declarition of CAccountCreationPlugin.
 *
 *  @lib accountcreationplugin.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CAccountCreationPlugin ) : public CAiwServiceIfBase,
    public MAccountCreationPluginObserver
    {
public:

    static CAccountCreationPlugin* NewL();

    /**
     * Destructor.
     */
    ~CAccountCreationPlugin();

// from base class CAiwServiceIfBase

    /**
     * From CAiwServiceIfBase.
     * Initialises provider with necessary information from the Service 
     * Handler. This method is called when the consumer makes the attach 
     * operation.
     * Not used but here because of inheritance.
     * 
     * @since S60 v3.2
     * @param aFrameworkCallback Framework provided callback for provider
     *                           to send events to framework. Not used.
     * @param aInterest List of criteria items which invoked the provider.
     *                  Not used.
     */
    void InitialiseL( MAiwNotifyCallback& /*aFrameworkCallback*/,
        const RCriteriaArray& /*aInterest*/ );

    /**
     * From CAiwServiceIfBase.
     * Executes generic service commands included in criteria.
     * 
     * @since S60 v3.2
     * @param aCmdId Command to be executed.
     * @param aInParamList Input parameters, can be an empty list.
     * @param aOutParamList Output parameters, can be an empty list.
     * @param aCmdOptions Options for the command, see KAiwOpt* in 
     *                    AiwCommon.hrh.
     * @param aCallback Callback for asynchronous command handling, 
     *                  parameter checking, etc.
     * @leave KErrArgument Callback is missing when required.
     * @leave KErrNotSupported No provider supports service.
     */
    void HandleServiceCmdL( 
        const TInt& /*aCmdId*/, 
        const CAiwGenericParamList& /*aInParamList*/,
        CAiwGenericParamList& aOutParamList,
        TUint aCmdOptions = 0,
        const MAiwNotifyCallback* aCallback = NULL );

// from base class MAccountCreationPluginObserver

    /**
     * From MAccountCreationPluginObserver.
     * Informs that an AIW event has occurred.
     * 
     * @since S60 v3.2
     * @param aError Error code.
     */
    void NotifyAiwEventL( TInt aError );
    
    /**
     * From MAccountCreationPluginObserver.
     * Informs that sis file is downloaded.
     * 
     * @since S60 v5.0
     * @param aFileName sis file name
     */
    void NotifySISDownloaded( TDesC& aFileName );

private:
    
    CAccountCreationPlugin();
    void ConstructL();

    /**
     * Sends AIW service command when plugin initialization is completed. 
     *
     * @since S60 v3.2
     */ 
    void SendInitializedCallbackL() const;

    /**
     * Sends AIW service command when plugin is stopped its work. 
     *
     * @since S60 v3.2
     */         
    void SendCompletedCallbackL() const;

    /**
     * Sends AIW service command when plugin error is reached.
     *
     * @since S60 v3.2 
     */ 
    void SendErrorCallbackL() const;

    /**
     * Compares AIW service command connected uid to our plugin uid.
     *
     * @since S60 v3.2
     * @param aOutParamList AIW service command parameters.
     * @return ETrue if UID's matched, EFalse if not.
     */
    TBool CompareUids( const CAiwGenericParamList& aOutParamList ) const;

    /**
     * Launches ACP UI. 
     *
     * @since S60 v3.2
     */ 
    void LaunchPluginUiL();

private: // data

    /**
     * Resource file offset.
     */
    TInt iResourceOffset;

    /**
     * Handle to AIW Service Handler.
     * Own.
     */
    CAiwServiceHandler* iServiceHandler;

    /**
     * Handle to AIW callback notifier.
     * Not own.
     */
    const MAiwNotifyCallback* iNotifyCallback;
    
    /**
     * Handle to AIW callback notifier.
     * Own.
     */
    CAiwGenericParamList* iCBEventParamList;

    /**
     * Handle to provider list view.
     * Not own.
     */
    CAcpProviderListView* iProviderListView;

    /**
     * CEikonEnv.
     * Own.
     */
    CEikonEnv* iEikEnv;

    /**
     * Boolean indicating if CSC has been started from active idle
     */ 
    TBool iLaunchedFromAI;    
    };  

#endif  // ACCOUNTCREATIONPLUGIN_H

// End of file.
