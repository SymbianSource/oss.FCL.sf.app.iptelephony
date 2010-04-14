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
* Description:  Notifier base class definition
*
*/



#ifndef C_CCHUINOTIFIER_H
#define C_CCHUINOTIFIER_H

#include <eikenv.h>
#include <eiknotapi.h>
#include <data_caging_path_literals.hrh> 
#include <AknNotiferAppServerApplication.h>

#include "cchuicommon.hrh"

class CSPSettings;
class CAknQueryDialog;
class CAknTextQueryDialog;

/**
 *  Defines base class for notifier implementation(s).
 *  Provides implementation that is common for all notifier
 *  implementation.
 *
 *  @code
 *  @endcode
 *
 *  @lib cchuinotif.lib
 *  @since S60 5.0
 */
NONSHARABLE_CLASS( CCCHUiNotifierBase ): 
    public CActive, public MEikSrvNotifierBase2
    {
public:

    /**
     * Destructor.
     */
    virtual ~CCCHUiNotifierBase();

protected:

    CCCHUiNotifierBase();
    
    virtual void ConstructL();
   
    /**
     * Complete Client/Server message and Nulled the reply slot.
     *
     * @since S60 5.0
     * @param aErr  Error code for client notification.
     */
    void CompleteMessage( TInt aErr );
    
    /**
     * Scans for resource file. Ownership of returned value
     * is passed.
     *
     * @since S60 5.0
     * @param aSession file server session
     * @param aFileName file name
     * @param aFilePath file path
     * @return resource file path and name
     */    
    HBufC* ScanFileL( 
        RFs& aSession, 
        const TDesC& aFileName,
        const TDesC& aFilePath );
     
    /**
     * Resolves service name.
     *
     * @since S60 5.0
     * @param aServiceId service id
     * @param aServiceName resolved service name
     */
    void GetServiceNameL( TUint aServiceId, TDes& aServiceName );
    
    /**
     * Get current connections name.
     *
     * @since S60 5.0
     * @param aServiceId service id
     * @param aIapId iap id
     * @param aIapName iap name to be stored to this
     */    
    void CurrentConnectionNameL( 
        TUint aServiceId, 
        TUint aIapid, 
        RBuf& aIapName );
    
    /**
     * Resolves if service is SIP/VoIP.
     *
     * @since S60 5.0
     * @param aServiceId service id
     * @return ETrue if service is SIP/VoIP
     */
    TBool IsSIPVoIPL( TUint aServiceId );

    /**
     * Checks if phone is in offline mode.
     *
     * @since S60 5.0
     * @return ETrue if phone is in offline mode.
     */    
    TBool IsPhoneOfflineL() const;    
    
    /**
     * Checks if there is GPRS IAPs available in Internet SNAP.
     *
     * @since S60 5.0
     * @return ETrue if phone is in offline mode.
     */    
    TBool IsGprsIapsAvailableL() const;
    
    /**
     * Checks if connection is used by other services.
     *
     * @since S60 5.0
     * @param aServiceId service id.
     * @param aIapId access point id.
     * @return ETrue if connection used by other services.
     */    
    TBool ConnectionUsedByOthersL( TUint aServiceId, TInt aIapId );

// from base class MEikSrvNotifierBase2

    /**
     * From MEikSrvNotifierBase2.
     * Called when a notifier is first loaded 
     * to allow any initial construction that is required.
     *
     * @return A structure containing priority and channel info.
     */
    virtual TNotifierInfo RegisterL() = 0;
    
    /**
     * From MEikSrvNotifierBase2.
     * The notifier has been deactivated 
     * so resources can be freed and outstanding messages completed.
     */
    virtual void Cancel();

    /**
     * From MEikSrvNotifierBase2.
     * Asynchronic notifier launch.
     *
     * @param aBuffer A buffer containing received parameters
     * @param aReturnVal The return value to be passed back.
     * @param aMessage Should be completed when the notifier is deactivated.
     * @return A pointer to return value.
     */
    virtual void StartL(
        const TDesC8& aBuffer, 
        TInt aReplySlot, 
        const RMessagePtr2& aMessage );
        
    
private:

// from base class MEikSrvNotifierBase2

    /**
     * From MEikSrvNotifierBase2.
     * Called when all resources allocated by notifiers should be freed.
     */
    virtual void Release();
    
    /**
     * From MEikSrvNotifierBase2.
     * Called when all resources allocated by notifiers should be freed.
     * 
     * @return A structure containing priority and channel info.
     */
    virtual TNotifierInfo Info() const;
    
    /**
     * From MEikSrvNotifierBase2.
     * Synchronic notifier launch.
     *
     * @param aBuffer Received parameter data.
     * @return A pointer to return value.
     */
    virtual TPtrC8 StartL( const TDesC8& aBuffer );
        
    /**
     * From MEikSrvNotifierBase2.
     * Updates a currently active notifier.
     *
     * @param aBuffer The updated data.
     * @return A pointer to return value.
     */
    virtual TPtrC8 UpdateL( const TDesC8& aBuffer );
    
    /**
     * From CActive Gets called when a request is cancelled.
     * @param None.
     * @return None.
     */
    virtual void DoCancel();  
       
    /**
     * From CActive Gets called when a request is cancelled.
     * @param None.
     * @return None.
     */
    virtual void RunL();
    
protected: // data

    /**
     * Pointer to service provider settings.
     * Own.
     */
    CSPSettings* iSettings;
    
    /**
     * Local eikonenv.
     * Own.
     */
    CEikonEnv* iEikEnv;
    
    /**
     * Received message.
     */
    RMessagePtr2 iMessage;
    
    /**
     * Notifier parameters structure.
     */
    TNotifierInfo iInfo;
    
    /**
     * Reply slot.
     */
    TInt iReplySlot;
    
    /**
     * Flag for eikon env.
     */
    TInt iResourceFileFlag;
    
    /**
     * Dialog mode.
     */
    MCchUiObserver::TCchUiDialogType    iDialogMode;
    
    /**
     * Service id.
     */
    TUint   iServiceId;
    
    /**
     * Flag to determine if application key is blocked.
     */
    TBool iAppKeyBlocked;
    
    CCHUI_UNIT_TEST( T_CchUiNotifierBase )
    };

#endif // C_CCHUINOTIFIER_H
