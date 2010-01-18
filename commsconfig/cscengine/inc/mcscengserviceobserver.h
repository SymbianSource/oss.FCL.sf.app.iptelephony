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
* Description:  Observer interface for notifying service changes
*
*/



#ifndef M_CSCENGSERVICEOBSERVER_H
#define M_CSCENGSERVICEOBSERVER_H


/**
 *  MCSCEngServiceObserver class
 *
 *  Observer interface for notifying service changes
 *
 *  @lib
 *  @since S60 v3.2
 */
class MCSCEngServiceObserver
    {
    public:
    
        /**
         * Should be called when change in service(s) occur
         *
         * @since S60 v3.2
         */
        virtual void NotifyServiceChange() = 0;
        
    };
    
#endif // M_CSCENGSERVICEOBSERVER_H
