/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: CCH UI call state callback observer
*
*/

#ifndef M_CCHUICALLSTATEOBSERVER_H
#define M_CCHUICALLSTATEOBSERVER_H


/**
 *  MCchUiCallStateObserver
 *
 *  @since S60 v5.0
 */
class MCchUiCallStateObserver
    {

public:

    /**
     * Callback for notifying of a call state change
     *
     * @since S60 v5.0
     * @param  aCallState state from CTSYDomainPSKeys.h
     */
    virtual void CallStateChanged( TInt aCallState ) = 0;
    
    };


#endif // M_CCHUICALLSTATEOBSERVER_H
