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
* Description:  Header file for MAcpControllerObserver
 *
*/


#ifndef MACPCONTROLLEROBSERVER_H
#define MACPCONTROLLEROBSERVER_H

class MAcpControllerObserver
    {
public:
    /**
     * Notifies observer about readiness of provider list.
     *
     * @since S60 v3.2
     * @param aError System wide error code.
     */ 
    virtual void NotifyProviderListReady( TInt aError ) = 0;

    /**
     * Notifies observer about downloading.
     *
     * @since S60 v3.2
     * @param aError System wide error code.
     */ 
    virtual void NotifyDownloadingCompleted( TInt aError ) = 0;
    
    /**
     * Notifies observer about downloading sis.
     *
     * @since S60 v5.0
     * @param aFileName sis file name
     */ 
    virtual void NotifyDownloadingSISCompleted( TDesC& aFileName ) = 0;
    
    /**
     * Notifies observer about successfully parser/provisioned content.
     * 
     * @since S60 v3.2
     */
    virtual void NotifyProvisioningCompleted() = 0;
    
    /**
     * Notifies observer when settings have been completely saved.
     * 
     * @since S60 v3.2
     */
    virtual void NotifySettingsSaved() = 0;
    };
    
#endif // MACPCONTROLLEROBSERVER_H

// End of file.
