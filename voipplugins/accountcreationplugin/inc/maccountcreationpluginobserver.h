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
* Description:  Header file for MAccountCreationPluginObserver
 *
*/


#ifndef MACCOUNTCREATIONPLUGINOBSERVER_H
#define MACCOUNTCREATIONPLUGINOBSERVER_H

class MAccountCreationPluginObserver
    {
public:
    /**
     * Notifies observer about AIW events.
     *
     * @since S60 v3.2
     * @param aError for system wide error code
     */ 
    virtual void NotifyAiwEventL( TInt aError ) = 0;
    
    
    /**
     * Notifies observer about downloaded sis file.
     *
     * @since S60 v5.0
     * @param aFileName sis file name
     */ 
    virtual void NotifySISDownloaded( TDesC& aFileName ) = 0;

    };

#endif // MACCOUNTCREATIONPLUGINOBSERVER_H

// End of file.
