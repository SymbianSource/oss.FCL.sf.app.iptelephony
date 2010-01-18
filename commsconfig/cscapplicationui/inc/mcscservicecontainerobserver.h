/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Observer interface for notifying container change.
*
*/


#ifndef M_CSCSERVICECONTAINEROBSERVER_H
#define M_CSCSERVICECONTAINEROBSERVER_H

/**
 *  MCSCServiceContainerObserver class
 *
 *  Observer interface for notifying container change.
 *
 *  @lib cscengine.lib
 *  @since S60 v3.2
 */
class MCSCServiceContainerObserver
    {
    public:
    
        /**
         * Should be called when the current item in listbox changes.
         *
         * @since S60 v3.2
         */
        virtual void UpdateCbaL() = 0;
            
    };
    
#endif // M_CSCSERVICECONTAINEROBSERVER_H
